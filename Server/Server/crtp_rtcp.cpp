/***********************************************************************************************
created: 		2023-02-20

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/
#include "crtp_rtcp.h"
#include "cbuffer.h"
#include "clog.h"
#include "crtc_sdp_define.h"
#include "crtp_rtcp_define.h"
namespace chen {
	crtcp_common::~crtcp_common()
	{
	}
	uint8_t crtcp_common::type() const
	{
		return m_header.type;
	}
	uint8_t crtcp_common::get_rc() const
	{
		return m_header.rc;
	}
	uint32_t crtcp_common::get_ssrc()
	{
		return m_ssrc;
	}
	void crtcp_common::set_ssrc(uint32_t ssrc)
	{
		m_ssrc = ssrc;
	}
	char * crtcp_common::data()
	{
		return m_data;
	}

	chen::int32 crtcp_common::size()
	{
		return m_nb_data;
	}

	bool crtcp_common::decode(cbuffer * buffer)
	{
		//return int32();
		m_data = buffer->head();
		m_nb_data = buffer->left();

		if (!decode_header(buffer) ) 
		{
			WARNING_EX_LOG("decode header");
			return false;
			//return srs_error_wrap(err, "decode header");
		}

		m_payload_len = (m_header.length + 1) * 4 - sizeof(crtcp_header) - 4;
		buffer->read_bytes((char *)m_payload, m_payload_len);
		return true;
	}

	uint64_t crtcp_common::nb_bytes()
	{
		return sizeof(crtcp_header) + 4 + m_payload_len ;
	}

	bool crtcp_common::encode(cbuffer * buffer)
	{
		WARNING_EX_LOG("not implement");
		return false;
	}

	bool crtcp_common::decode_header(cbuffer * buffer)
	{
		if (!buffer->require(sizeof(crtcp_header) + 4))
		{
			WARNING_EX_LOG("  require %u  ((sizeof(crtcp_header) + 4))",(sizeof(crtcp_header) + 4));
			return false;
		}

		buffer->read_bytes((char*)(&m_header), sizeof(crtcp_header));
		m_header.length = ntohs(m_header.length);

		int payload_len = m_header.length * 4;
		if (payload_len > buffer->left()) 
		{
			WARNING_EX_LOG("require payload len=%u, buffer left=%u", payload_len, buffer->left());
			return false;
			//return srs_error_new(ERROR_RTC_RTCP,
			//	"require payload len=%u, buffer left=%u", payload_len, buffer->left());
		}
		m_ssrc = buffer->read_4bytes();

		return true;
	}
	bool crtcp_common::encode_header(cbuffer * buffer)
	{
		if (!buffer->require(sizeof(crtcp_header) + 4))
		{
			 WARNING_EX_LOG( "require %d", sizeof(crtcp_header) + 4);
			 return false;
		}
		m_header.length = htons(m_header.length);
		buffer->write_bytes((char*)(&m_header), sizeof(crtcp_header));
		buffer->write_4bytes(m_ssrc);

		return true;
	}
	crtcp_compound::~crtcp_compound()
	{
		clear();
	}
	crtcp_common * crtcp_compound::get_next_rtcp()
	{
		if (m_rtcps.empty()) {
			return NULL;
		}
		crtcp_common *rtcp = m_rtcps.back();
		m_rtcps.pop_back();
		return rtcp;
	}
	bool crtcp_compound::add_rtcp(crtcp_common * rtcp)
	{
		int32 new_len = rtcp->nb_bytes();
		if ((new_len + m_nb_bytes) > kRtcpPacketSize)
		{
			WARNING_EX_LOG("overflow, new rtcp: %d, current: %d", new_len, m_nb_bytes);
			//return srs_error_new(ERROR_RTC_RTCP, "overflow, new rtcp: %d, current: %d", new_len, nb_bytes_);
		}
		m_nb_bytes += new_len;
		m_rtcps.push_back(rtcp);

		return true;
	}
	void crtcp_compound::clear()
	{
		std::vector<crtcp_common*>::iterator it;
		for (it = m_rtcps.begin(); it != m_rtcps.end(); ++it) 
		{
			crtcp_common *rtcp = *it;
			delete rtcp;
			rtcp = NULL;
		}
		m_rtcps.clear();
		m_nb_bytes = 0;
	}
	char * crtcp_compound::data()
	{
		return m_data;
	}
	int32 crtcp_compound::size()
	{
		return m_nb_bytes;
	}
	bool crtcp_compound::decode(cbuffer * buffer)
	{
		//srs_error_t err = srs_success;
		m_data = buffer->data();
		m_nb_data = buffer->size();

		while (!buffer->empty()) 
		{
			crtcp_common* rtcp = NULL;
			crtcp_header* header = (crtcp_header*)(buffer->head());
			if (header->type == ERtcpType_sr) 
			{
				rtcp = new crtcp_sr();
			}
			else if (header->type == ERtcpType_rr) 
			{
				 rtcp = new crtcp_rr();
			}
			else if (header->type == ERtcpType_rtpfb) 
			{
				if (1 == header->rc) 
				{
					//nack
					rtcp = new crtcp_nack();
				}
				else if (15 == header->rc)
				{
					//twcc
					rtcp = new crtcp_twcc();
				}
			}
			else if (header->type == ERtcpType_psfb)
			{
				if (1 == header->rc) 
				{
					// pli
					rtcp = new crtcp_pli();
				}
				else if (2 == header->rc) 
				{
					//sli
					rtcp = new crtcp_sli();
				}
				else if (3 == header->rc)
				{
					//rpsi
					rtcp = new crtcp_rpsi();
				}
				else
				{
					// common psfb
					rtcp = new crtcp_psfb_common();
				}
			}
			else if (header->type == ERtcpType_xr) 
			{
				rtcp = new crtcp_xr();
			}
			else 
			{
				rtcp = new crtcp_common();
			}

			if (! rtcp->decode(buffer) ) 
			{
				delete rtcp;
				rtcp = NULL;
			//	srs_freep(rtcp);

				// @doc https://tools.ietf.org/html/rfc3550#section-6.4.2
				// An empty RR packet (RC = 0) MUST be put at the head of a compound
				// RTCP packet when there is no data transmission or reception to
				// report. e.g. {80 c9 00 01 00 00 00 01}
				/*if (ERROR_RTC_RTCP_EMPTY_RR == srs_error_code(err))
				{
					srs_freep(err);
					continue;
				}*/
				WARNING_EX_LOG("decode rtcp type=%u rc=%u", header->type, header->rc);
				return false;
				//return srs_error_wrap(err, ");
			}

			m_rtcps.push_back(rtcp);
		}

		return true;
	}
	uint64_t crtcp_compound::nb_bytes()
	{
		return kRtcpPacketSize;
	}
	bool crtcp_compound::encode(cbuffer * buffer)
	{
		if (!buffer->require(m_nb_bytes))
		{
			WARNING_EX_LOG("requires %d bytes", m_nb_bytes);
			return false;
			//return srs_error_new(ERROR_RTC_RTCP, "requires %d bytes", nb_bytes_);
		}

		std::vector<crtcp_common*>::iterator it;
		for (it = m_rtcps.begin(); it != m_rtcps.end(); ++it)
		{
			crtcp_common *rtcp = *it;
			if (!rtcp->encode(buffer) ) 
			{
				WARNING_EX_LOG("encode compound type:%d", rtcp->type());
				return false;
				//return srs_error_wrap(err, "encode compound type:%d", rtcp->type());
			}
		}

		clear();
		return true;
	}

	SrsRtcpApp::SrsRtcpApp()
		:crtcp_common()
		, m_name()
	{
		m_ssrc = 0;
		m_header.padding = 0;
		m_header.type = ERtcpType_app;
		m_header.rc = 0;
		m_header.version = kRtcpVersion;
	}

	SrsRtcpApp::~SrsRtcpApp()
	{
	}

	bool SrsRtcpApp::is_rtcp_app(uint8 * data, int32 nb_data)
	{
		if (!data || nb_data < 12) 
		{
			return false;
		}

		crtcp_header *header = (crtcp_header*)data;
		if (header->version == kRtcpVersion
			&& header->type == ERtcpType_app
			&& ntohs(header->length) >= 2) 
		{
			return true;
		}

		return false;
	}

	uint8 SrsRtcpApp::type() const
	{
		return ERtcpType_app;
	}

	uint8 SrsRtcpApp::get_subtype() const
	{
		return m_header.rc;
	}

	std::string SrsRtcpApp::get_name() const
	{
		return std::string((char *)m_name, strnlen((char *)m_name, 4));
	}

	bool SrsRtcpApp::get_payload(uint8 *& payload, int32 & len)
	{
		len = m_payload_len;
		payload = m_payload;
		return true;
	}

	bool SrsRtcpApp::set_subtype(uint8 type)
	{
		if (31 < type)
		{
			WARNING_EX_LOG("invalid type = %u", type);
			return false;
		}
		m_header.rc = type;
		return true;
	}

	bool SrsRtcpApp::set_name(std::string name)
	{
		if (name.length() > 4)
		{
			WARNING_EX_LOG("invalid name length %zu", name.length());
			return false;
		}

		memset(m_name, 0, sizeof(m_name));
		memcpy(m_name, name.c_str(), name.length());
		return true;
	}

	bool SrsRtcpApp::set_payload(uint8 * payload, int32 len)
	{
		if (len > (kRtcpPacketSize - 12)) 
		{
			WARNING_EX_LOG("invalid payload length %d", len);
			return false;
			//return srs_error_new(ERROR_RTC_RTCP, "invalid payload length %d", len);
		}

		m_payload_len = (len + 3) / 4 * 4;;
		memcpy(m_payload, payload, len);
		if (m_payload_len > len)
		{
			memset(&m_payload[len], 0, m_payload_len - len); //padding
		}
		m_header.length = m_payload_len / 4 + 3 - 1;

		return true;
	}

	bool SrsRtcpApp::decode(cbuffer * buffer)
	{
		/*
	@doc: https://tools.ietf.org/html/rfc3550#section-6.7
	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P| subtype |   PT=APP=204  |             length            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           SSRC/CSRC                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                          name (ASCII)                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                   application-dependent data                ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
		 
		m_data = buffer->head();
		m_nb_data = buffer->left();

		if (! decode_header(buffer))
		{
			WARNING_EX_LOG("decode header");
			return false;
		}

		if (m_header.type != ERtcpType_app || !buffer->require(4)) 
		{
			WARNING_EX_LOG("not rtcp app");
			return false;
		}

		buffer->read_bytes((char *)m_name, sizeof(m_name));

		// TODO: FIXME: Should check size?
		m_payload_len = (m_header.length + 1) * 4 - 8 - sizeof(m_name);
		if (m_payload_len > 0) 
		{
			buffer->read_bytes((char *)m_payload, m_payload_len);
		}
		return true;
	}

	uint64 SrsRtcpApp::nb_bytes()
	{
		return sizeof(crtcp_header) + sizeof(m_ssrc) + sizeof(m_name) + m_payload_len;
	}

	bool SrsRtcpApp::encode(cbuffer * buffer)
	{
		/*
	@doc: https://tools.ietf.org/html/rfc3550#section-6.7
	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P| subtype |   PT=APP=204  |             length            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           SSRC/CSRC                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                          name (ASCII)                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                   application-dependent data                ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
		 
		if (!buffer->require(nb_bytes())) 
		{
			WARNING_EX_LOG("requires %d bytes", nb_bytes());
			return false;
		}

		if (!encode_header(buffer))
		{
			WARNING_EX_LOG("encode header");
			return false;
		}

		buffer->write_bytes((char*)m_name, sizeof(m_name));
		buffer->write_bytes((char*)m_payload, m_payload_len);

		return true;
	}

	crtcp_sr::crtcp_sr()
		: crtcp_common()
		, m_ntp(0)
		, m_rtp_ts(0)
		, m_send_rtp_packets(0)
		, m_send_rtp_bytes(0) 
	{
		m_header.padding = 0;
		m_header.type = ERtcpType_sr;
		m_header.rc = 0;
		m_header.version = 6;

		m_ssrc = 0;
	}

	crtcp_sr::~crtcp_sr()
	{
	}

	uint8 crtcp_sr::get_rc() const
	{
		return m_header.rc;
	}

	uint8 crtcp_sr::type() const
	{
		return ERtcpType_sr;
	}

	uint64 crtcp_sr::get_ntp() const
	{
		return m_ntp;
	}

	uint32 crtcp_sr::get_rtp_ts() const
	{
		return m_rtp_ts;
	}

	uint32 crtcp_sr::get_rtp_send_packets() const
	{
		return m_send_rtp_packets;
	}

	uint32 crtcp_sr::get_rtp_send_bytes() const
	{
		return m_send_rtp_bytes;
	}

	void crtcp_sr::set_ntp(uint64 ntp)
	{
		m_ntp = ntp;
	}

	void crtcp_sr::set_rtp_ts(uint32 ts)
	{
		m_rtp_ts = ts;
	}

	void crtcp_sr::set_rtp_send_packets(uint32 packets)
	{
		m_send_rtp_packets = packets;
	}

	void crtcp_sr::set_rtp_send_bytes(uint32 bytes)
	{
		m_send_rtp_bytes = bytes;
	}

	bool crtcp_sr::decode(cbuffer * buffer)
	{
		/* @doc: https://tools.ietf.org/html/rfc3550#section-6.4.1
	   0                   1                   2                   3
	   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=SR=200   |             length            |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                         SSRC of sender                        |
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
sender |              NTP timestamp, most significant word             |
info   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |             NTP timestamp, least significant word             |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                         RTP timestamp                         |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                     sender's packet count                     |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      sender's octet count                     |
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |           extended highest sequence number received           |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      interarrival jitter                      |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                         last SR (LSR)                         |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                   delay since last SR (DLSR)                  |
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_2 (SSRC of second source)                |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  2    :                               ...                             :
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	   |                  profile-specific extensions                  |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
		 
		m_data = buffer->head();
		m_nb_data = buffer->left();

		if (! decode_header(buffer)) 
		{
			WARNING_EX_LOG("decode header");
			return false;
		}

		m_ntp = buffer->read_8bytes();
		m_rtp_ts = buffer->read_4bytes();
		m_send_rtp_packets = buffer->read_4bytes();
		m_send_rtp_bytes = buffer->read_4bytes();

		if (m_header.rc > 0)
		{
			char buf[1500];
			buffer->read_bytes(buf, m_header.rc * 24);
		}

		return true;
	}

	uint64 crtcp_sr::nb_bytes()
	{
		return (m_header.length + 1) * 4;
	}

	bool crtcp_sr::encode(cbuffer * buffer)
	{
		/* @doc: https://tools.ietf.org/html/rfc3550#section-6.4.1
	   0                   1                   2                   3
	   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=SR=200   |             length            |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                         SSRC of sender                        |
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
sender |              NTP timestamp, most significant word             |
info   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |             NTP timestamp, least significant word             |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                         RTP timestamp                         |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                     sender's packet count                     |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      sender's octet count                     |
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |           extended highest sequence number received           |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      interarrival jitter                      |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                         last SR (LSR)                         |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                   delay since last SR (DLSR)                  |
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_2 (SSRC of second source)                |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  2    :                               ...                             :
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	   |                  profile-specific extensions                  |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
		if (!buffer->require(nb_bytes())) 
		{
			WARNING_EX_LOG("requires %d bytes", nb_bytes());
			return false;
		}

		if (! encode_header(buffer))
		{
			WARNING_EX_LOG("encode header");
			return false;
		}

		buffer->write_8bytes(m_ntp);
		buffer->write_4bytes(m_rtp_ts);
		buffer->write_4bytes(m_send_rtp_packets);
		buffer->write_4bytes(m_send_rtp_bytes);
		return true;
	}

	crtcp_rr::crtcp_rr(uint32 sender_ssrc)
		: crtcp_common()
		, m_rb()
	{
		m_header.padding = 0;
		m_header.type = ERtcpType_rr;
		m_header.rc = 0;
		m_header.version = kRtcpVersion;
		m_header.length = 7;
		m_ssrc = sender_ssrc;
		memset((void*)&m_rb, 0, sizeof(crtcp_rb));
	}

	crtcp_rr::~crtcp_rr()
	{
	}

	uint8 crtcp_rr::type() const
	{
		return ERtcpType_rr;
	}

	uint32 crtcp_rr::get_rb_ssrc() const
	{
		return m_rb.ssrc;
	}

	float crtcp_rr::get_lost_rate() const
	{
		return m_rb.fraction_lost / 256;
	}

	uint32 crtcp_rr::get_lost_packets() const
	{
		return m_rb.lost_packets;
	}

	uint32 crtcp_rr::get_highest_sn() const
	{
		return m_rb.highest_sn;
	}

	uint32 crtcp_rr::get_jitter() const
	{
		return m_rb.lsr;
	}

	uint32 crtcp_rr::get_lsr() const
	{
		return m_rb.lsr;
	}

	uint32 crtcp_rr::get_dlsr() const
	{
		return m_rb.dlsr;
	}

	void crtcp_rr::set_rb_ssrc(uint32 ssrc)
	{
		m_rb.ssrc = ssrc;
	}

	void crtcp_rr::set_lost_rate(float rate)
	{
		m_rb.fraction_lost = rate * 256;
	}

	void crtcp_rr::set_lost_packets(uint32 count)
	{
		m_rb.lost_packets = count;
	}

	void crtcp_rr::set_highest_sn(uint32 sn)
	{
		m_rb.highest_sn = sn;
	}

	void crtcp_rr::set_jitter(uint32 jitter)
	{
		m_rb.jitter = jitter;
	}

	void crtcp_rr::set_lsr(uint32 lsr)
	{
		m_rb.lsr = lsr;
	}

	void crtcp_rr::set_dlsr(uint32 dlsr)
	{
		m_rb.dlsr = dlsr;
	}

	void crtcp_rr::set_sender_ntp(uint64 ntp)
	{
		uint32 lsr = (uint32)((ntp >> 16) & 0x00000000FFFFFFFF);
		m_rb.lsr = lsr;
	}

	bool crtcp_rr::decode(cbuffer * buffer)
	{
		/*
	@doc: https://tools.ietf.org/html/rfc3550#section-6.4.2

		0                   1                   2                   3
		0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=RR=201   |             length            |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                     SSRC of packet sender                     |
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |           extended highest sequence number received           |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      interarrival jitter                      |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                         last SR (LSR)                         |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                   delay since last SR (DLSR)                  |
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_2 (SSRC of second source)                |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  2    :                               ...                             :
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	   |                  profile-specific extensions                  |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
	 
		m_data = buffer->head();
		m_nb_data = buffer->left();

		if (! decode_header(buffer) )
		{
			WARNING_EX_LOG("decode header");
			return false;
		}

		// @doc https://tools.ietf.org/html/rfc3550#section-6.4.2
		// An empty RR packet (RC = 0) MUST be put at the head of a compound
		// RTCP packet when there is no data transmission or reception to
		// report. e.g. {80 c9 00 01 00 00 00 01}
		if (m_header.rc == 0) 
		{
			WARNING_EX_LOG("rc=0");
			return false;
		}

		// TODO: FIXME: Security check for read.
		m_rb.ssrc = buffer->read_4bytes();
		m_rb.fraction_lost = buffer->read_1bytes();
		m_rb.lost_packets = buffer->read_3bytes();
		m_rb.highest_sn = buffer->read_4bytes();
		m_rb.jitter = buffer->read_4bytes();
		m_rb.lsr = buffer->read_4bytes();
		m_rb.dlsr = buffer->read_4bytes();

		// TODO: FIXME: Security check for read.
		if (m_header.rc > 1) 
		{
			char buf[1500];
			buffer->read_bytes(buf, (m_header.rc - 1) * 24);
		}

		return true;
	}

	uint64 crtcp_rr::nb_bytes()
	{
		return (m_header.length + 1) * 4;
	}

	bool crtcp_rr::encode(cbuffer * buffer)
	{
		/*
	@doc: https://tools.ietf.org/html/rfc3550#section-6.4.2

		0                   1                   2                   3
		0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
header |V=2|P|    RC   |   PT=RR=201   |             length            |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                     SSRC of packet sender                     |
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_1 (SSRC of first source)                 |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  1    | fraction lost |       cumulative number of packets lost       |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |           extended highest sequence number received           |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      interarrival jitter                      |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                         last SR (LSR)                         |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                   delay since last SR (DLSR)                  |
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
report |                 SSRC_2 (SSRC of second source)                |
block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  2    :                               ...                             :
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	   |                  profile-specific extensions                  |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
		 
		if (!buffer->require(nb_bytes())) 
		{
			WARNING_EX_LOG("requires %d bytes", nb_bytes());
			return false;
		}

		m_header.rc = 1;
		if (! encode_header(buffer))
		{
			WARNING_EX_LOG("encode header");
			return false;
		}

		buffer->write_4bytes(m_rb.ssrc);
		buffer->write_1bytes(m_rb.fraction_lost);
		buffer->write_3bytes(m_rb.lost_packets);
		buffer->write_4bytes(m_rb.highest_sn);
		buffer->write_4bytes(m_rb.jitter);
		buffer->write_4bytes(m_rb.lsr);
		buffer->write_4bytes(m_rb.dlsr);

		return true;
	}
	 
	crtcp_twcc::crtcp_twcc(uint32_t sender_ssrc)
		: crtcp_common()
		, m_media_ssrc(0)
		, m_base_sn(0)
		, m_reference_time(0)
		, m_fb_pkt_count(0)
		, m_encoded_chucks()
		, m_pkt_deltas()
		, m_recv_packets()
		, m_recv_sns()
		, m_pkt_len(0)
		, m_next_base_sn(0) 
	{
		m_header.padding = 0;
		m_header.type = ERtcpType_rtpfb;
		m_header.rc = 15;
		m_header.version = kRtcpVersion;
		m_ssrc = sender_ssrc;
	}

	crtcp_twcc::~crtcp_twcc()
	{
	}

	uint32 crtcp_twcc::get_media_ssrc() const
	{
		return m_media_ssrc;
	}

	uint16 crtcp_twcc::get_base_sn() const
	{
		return m_base_sn;
	}

	uint32 crtcp_twcc::get_reference_time() const
	{
		return m_reference_time;
	}

	uint8 crtcp_twcc::get_feedback_count() const
	{
		return m_fb_pkt_count;
	}

	std::vector<uint16> crtcp_twcc::get_packet_chucks() const
	{
		return m_encoded_chucks;
	}

	std::vector<uint16> crtcp_twcc::get_recv_deltas() const
	{
		return m_pkt_deltas;
	}

	void crtcp_twcc::set_media_ssrc(uint32 ssrc)
	{
		m_media_ssrc = ssrc;
	}

	void crtcp_twcc::set_base_sn(uint16 sn)
	{
		m_base_sn = sn;
	}

	void crtcp_twcc::set_reference_time(uint32 time)
	{
		m_reference_time = time;
	}

	void crtcp_twcc::set_feedback_count(uint8 count)
	{
		m_fb_pkt_count = count;
	}

	void crtcp_twcc::add_packet_chuck(uint16 chuck)
	{
		m_encoded_chucks.push_back(chuck);
	}

	void crtcp_twcc::add_recv_delta(uint16 delta)
	{
		m_pkt_deltas.push_back(delta);
	}

	bool crtcp_twcc::recv_packet(uint16 sn, uint64 ts)
	{
		std::map<uint16, uint64>::iterator it = m_recv_packets.find(sn);
		if (it != m_recv_packets.end()) 
		{
			WARNING_EX_LOG("TWCC dup seq: %d", sn);
			return false;
		 
		}

		m_recv_packets[sn] = ts;
		m_recv_sns.insert(sn);
		return true;
	}

	bool crtcp_twcc::need_feedback()
	{
		return m_recv_packets.size() > 0;
	}

	void crtcp_twcc::clear()
	{
		m_encoded_chucks.clear();
		m_pkt_deltas.clear();
		m_recv_packets.clear();
		m_recv_sns.clear();
		m_next_base_sn = 0;
	}

	uint64 crtcp_twcc::calculate_delta_us(uint64 ts, uint64 last)
	{
		int64 divisor = kTwccFbReferenceTimeDivisor;
		int64 delta_us = (ts - last) % divisor;

		if (delta_us > (divisor >> 1))
		{
			delta_us -= divisor;
		}

		delta_us += (delta_us < 0) ? (-kTwccFbDeltaUnit / 2) : (kTwccFbDeltaUnit / 2);
		delta_us /= kTwccFbDeltaUnit;

		return delta_us;
	}

	bool crtcp_twcc::process_pkt_chunk(crtcp_twcc_chunk & chunk, int delta_size)
	{
		size_t needed_chunk_size = chunk.size == 0 ? kTwccFbChunkBytes : 0;

		size_t might_occupied = m_pkt_len + needed_chunk_size + delta_size;
		if (might_occupied > (size_t)kRtcpPacketSize) 
		{
			WARNING_EX_LOG("might_occupied %zu", might_occupied);
			return false;
		}

		if (can_add_to_chunk(chunk, delta_size))
		{
			//pkt_len += needed_chunk_size;
			add_to_chunk(chunk, delta_size);
			return true;
		}
		if (!encode_chunk(chunk) ) 
		{
			WARNING_EX_LOG("encode chunk, delta_size %u", delta_size);
			return false; 
		}
		add_to_chunk(chunk, delta_size);
		return true;
	}

	bool crtcp_twcc::can_add_to_chunk(crtcp_twcc_chunk & chunk, int delta_size)
	{
		NORMAL_EX_LOG("can_add %d chunk->size %u delta_sizes %d %d %d %d %d %d %d %d %d %d %d %d %d %d all_same %d has_large_delta %d",
			delta_size, chunk.size, chunk.delta_sizes[0], chunk.delta_sizes[1], chunk.delta_sizes[2], chunk.delta_sizes[3],
			chunk.delta_sizes[4], chunk.delta_sizes[5], chunk.delta_sizes[6], chunk.delta_sizes[7], chunk.delta_sizes[8],
			chunk.delta_sizes[9], chunk.delta_sizes[10], chunk.delta_sizes[11], chunk.delta_sizes[12], chunk.delta_sizes[13],
			(int)chunk.all_same, (int)chunk.has_large_delta);

		if (chunk.size < kTwccFbTwoBitElements) 
		{
			return true;
		}

		if (chunk.size < kTwccFbOneBitElements && !chunk.has_large_delta && delta_size != kTwccFbLargeRecvDeltaBytes) 
		{
			return true;
		}

		if (chunk.size < kTwccFbMaxRunLength && chunk.all_same && chunk.delta_sizes[0] == delta_size) 
		{
			NORMAL_EX_LOG("< %d && all_same && delta_size[0] %d == %d", kTwccFbMaxRunLength, chunk.delta_sizes[0], delta_size);
			return true;
		}

		return false;
	}

	void crtcp_twcc::add_to_chunk(crtcp_twcc_chunk & chunk, int delta_size)
	{
		if (chunk.size < kTwccFbMaxBitElements) 
		{
			chunk.delta_sizes[chunk.size] = delta_size;
		}

		chunk.size += 1;
		chunk.all_same = chunk.all_same && delta_size == chunk.delta_sizes[0];
		chunk.has_large_delta = chunk.has_large_delta || delta_size >= kTwccFbLargeRecvDeltaBytes;
	}

	bool crtcp_twcc::encode_chunk(crtcp_twcc_chunk & chunk)
	{
		if (can_add_to_chunk(chunk, 0) && can_add_to_chunk(chunk, 1) && can_add_to_chunk(chunk, 2)) 
		{
			WARNING_EX_LOG("TWCC chunk");
			return false;
		}

		if (chunk.all_same) 
		{
			if (! encode_chunk_run_length(chunk) ) 
			{
				WARNING_EX_LOG("encode run");

				return false;
			}
			reset_chunk(chunk);
			return true;
		}

		if (chunk.size == kTwccFbOneBitElements)
		{
			if (!encode_chunk_one_bit(chunk) ) 
			{
				WARNING_EX_LOG("encode chunk");
				return false;
			}
			reset_chunk(chunk);
			return true;
		}

		if (! encode_chunk_two_bit(chunk, kTwccFbTwoBitElements, true) ) 
		{
			WARNING_EX_LOG("encode chunk");
			return false;
		}

		return true;
	}

	bool crtcp_twcc::encode_chunk_run_length(crtcp_twcc_chunk & chunk)
	{
		if (!chunk.all_same || chunk.size > kTwccFbMaxRunLength)
		{
			WARNING_EX_LOG("invalid run all_same:%d, size:%d", chunk.all_same, chunk.size);
			return false;
		}

		uint16 encoded_chunk = (chunk.delta_sizes[0] << 13) | chunk.size;

		m_encoded_chucks.push_back(encoded_chunk);
		m_pkt_len += sizeof(encoded_chunk);
		return true;
	}

	bool crtcp_twcc::encode_chunk_one_bit(crtcp_twcc_chunk & chunk)
	{
		int32 i = 0;
		if (chunk.has_large_delta) 
		{
			WARNING_EX_LOG("invalid large delta");
			return  false;
		}

		uint16 encoded_chunk = 0x8000;
		for (i = 0; i < chunk.size; ++i) 
		{
			encoded_chunk |= (chunk.delta_sizes[i] << (kTwccFbOneBitElements - 1 - i));
		}

		m_encoded_chucks.push_back(encoded_chunk);
		m_pkt_len += sizeof(encoded_chunk);

		// 1 0 symbol_list
		return true;
	}

	bool crtcp_twcc::encode_chunk_two_bit(crtcp_twcc_chunk & chunk, size_t size, bool shift)
	{
		uint32 i = 0;
		uint8 delta_size = 0;

		uint16 encoded_chunk = 0xc000;
		// 1 1 symbol_list
		for (i = 0; i < size; ++i) 
		{
			encoded_chunk |= (chunk.delta_sizes[i] << (2 * (kTwccFbTwoBitElements - 1 - i)));
		}
		m_encoded_chucks.push_back(encoded_chunk);
		m_pkt_len += sizeof(encoded_chunk);

		if (shift) 
		{
			chunk.size -= size;
			chunk.all_same = true;
			chunk.has_large_delta = false;
			for (i = 0; i < chunk.size; ++i) 
			{
				delta_size = chunk.delta_sizes[i + size];
				chunk.delta_sizes[i] = delta_size;
				chunk.all_same = (chunk.all_same && delta_size == chunk.delta_sizes[0]);
				chunk.has_large_delta = chunk.has_large_delta || delta_size == kTwccFbLargeRecvDeltaBytes;
			}
		}

		return true;
		//return int32();
	}

	void crtcp_twcc::reset_chunk(crtcp_twcc_chunk & chunk)
	{
		chunk.size = 0;

		chunk.all_same = true;
		chunk.has_large_delta = false;
	}

	bool crtcp_twcc::encode_remaining_chunk(crtcp_twcc_chunk & chunk)
	{
		if (chunk.all_same) 
		{
			return encode_chunk_run_length(chunk);
		}
		else if (chunk.size <= kTwccFbTwoBitElements) 
		{
			// FIXME, TRUE or FALSE
			return encode_chunk_two_bit(chunk, chunk.size, false);
		}
		return encode_chunk_one_bit(chunk);
	}

	bool crtcp_twcc::decode(cbuffer * buffer)
	{
		/*
	@doc: https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01#section-3.1
			0                   1                   2                   3
		0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |V=2|P|  FMT=15 |    PT=205     |           length              |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                     SSRC of packet sender                     |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      SSRC of media source                     |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |      base sequence number     |      packet status count      |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                 reference time                | fb pkt. count |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |          packet chunk         |         packet chunk          |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   .                                                               .
	   .                                                               .
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |         packet chunk          |  recv delta   |  recv delta   |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   .                                                               .
	   .                                                               .
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |           recv delta          |  recv delta   | zero padding  |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
		 
		m_data = buffer->head();
		m_nb_data = buffer->left();

		if (! decode_header(buffer))
		{
			WARNING_EX_LOG("decode header");
			return false;
		}

		m_payload_len = (m_header.length + 1) * 4 - sizeof(crtcp_header) - 4;
		buffer->read_bytes((char *)m_payload, m_payload_len);

		return true;
	}

	uint64 crtcp_twcc::nb_bytes()
	{
		return kMaxUDPDataSize;
	}

	bool crtcp_twcc::encode(cbuffer * buffer)
	{
		bool ret = do_encode(buffer);

		if (!ret || m_next_base_sn == 0)
		{
			clear();
		}

		return ret;
	}

	bool crtcp_twcc::do_encode(cbuffer * buffer)
	{
		/*
	@doc: https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01#section-3.1
			0                   1                   2                   3
		0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |V=2|P|  FMT=15 |    PT=205     |           length              |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                     SSRC of packet sender                     |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      SSRC of media source                     |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |      base sequence number     |      packet status count      |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                 reference time                | fb pkt. count |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |          packet chunk         |         packet chunk          |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   .                                                               .
	   .                                                               .
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |         packet chunk          |  recv delta   |  recv delta   |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   .                                                               .
	   .                                                               .
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |           recv delta          |  recv delta   | zero padding  |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
		 
		if (!buffer->require(nb_bytes()))
		{
			WARNING_EX_LOG("requires %d bytes", nb_bytes());
			return false;
		}

		m_pkt_len = kTwccFbPktHeaderSize;

		std::set<uint16, cseq_compare_less>::iterator it_sn = m_recv_sns.begin();
		if (!m_next_base_sn) 
		{
			m_base_sn = *it_sn;
		}
		else 
		{
			m_base_sn = m_next_base_sn;
			it_sn = m_recv_sns.find(m_base_sn);
		}

		std::map<uint16, uint64>::iterator it_ts = m_recv_packets.find(m_base_sn);
		uint64 ts = it_ts->second;

		m_reference_time = (ts % kTwccFbReferenceTimeDivisor) / kTwccFbTimeMultiplier;
		uint64 last_ts = (uint64)(m_reference_time)* kTwccFbTimeMultiplier;

		uint16 last_sn = m_base_sn;
		uint16 packet_count = 0;

		// encode chunk
		crtcp_twcc::crtcp_twcc_chunk chunk;
		for (; it_sn != m_recv_sns.end(); ++it_sn)
		{
			// check whether exceed buffer len
			// max recv_delta_size = 2
			if (m_pkt_len + 2 >= buffer->left()) 
			{
				break;
			}

			uint16 current_sn = *it_sn;
			// calculate delta
			it_ts = m_recv_packets.find(current_sn);
			if (it_ts == m_recv_packets.end()) 
			{
				continue;
			}

			packet_count++;
			uint64 delta_us = calculate_delta_us(it_ts->second, last_ts);
			int16 delta = delta_us;
			if (delta != delta_us)
			{
				WARNING_EX_LOG("twcc: delta:%" PRId64 ", exceeds the 16bits", delta_us);
				return false;
			}

			if (current_sn > (last_sn + 1)) 
			{
				// lost packet
				for (uint16_t lost_sn = last_sn + 1; lost_sn < current_sn; ++lost_sn) 
				{
					process_pkt_chunk(chunk, 0);
					packet_count++;
				}
			}

			// FIXME 24-bit base receive delta not supported
			int32 recv_delta_size = (delta >= 0 && delta <= 0xff) ? 1 : 2;
			if (! process_pkt_chunk(chunk, recv_delta_size) ) 
			{
				WARNING_EX_LOG("delta_size %d, failed to append_recv_delta", recv_delta_size);
				return false;
			}

			m_pkt_deltas.push_back(delta);
			last_ts += delta * kTwccFbDeltaUnit;
			m_pkt_len += recv_delta_size;
			last_sn = current_sn;

			m_recv_packets.erase(it_ts);
		}

		m_next_base_sn = 0;
		if (it_sn != m_recv_sns.end()) 
		{
			m_next_base_sn = *it_sn;
		}

		if (0 < chunk.size) 
		{
			if (!encode_remaining_chunk(chunk))
			{
				WARNING_EX_LOG("encode chunk");
				return false;
			}
		}

		// encode rtcp twcc packet
		if ((m_pkt_len % 4) == 0) 
		{
			m_header.length = m_pkt_len / 4;
		}
		else 
		{
			m_header.length = (m_pkt_len + 4 - (m_pkt_len % 4)) / 4;
		}
		m_header.length -= 1;

		if (! encode_header(buffer))
		{
			WARNING_EX_LOG("encode header");
			return false;
		}

		buffer->write_4bytes(m_media_ssrc);
		buffer->write_2bytes(m_base_sn);
		buffer->write_2bytes(packet_count);
		buffer->write_3bytes(m_reference_time);
		buffer->write_1bytes(m_fb_pkt_count);

		int32 required_size = m_encoded_chucks.size() * 2;
		if (!buffer->require(required_size)) 
		{
			WARNING_EX_LOG("encoded_chucks_[%d] requires %d bytes", (int32)m_encoded_chucks.size(), required_size);
			return false;
		}

		for (std::vector<uint16>::iterator it = m_encoded_chucks.begin(); it != m_encoded_chucks.end(); ++it) 
		{
			buffer->write_2bytes(*it);
		}

		required_size = m_pkt_deltas.size() * 2;
		if (!buffer->require(required_size)) 
		{
			WARNING_EX_LOG("pkt_deltas_[%d] requires %d bytes", (int32)m_pkt_deltas.size(), required_size);
			return false;
		}

		for (std::vector<uint16 >::iterator it = m_pkt_deltas.begin(); it != m_pkt_deltas.end(); ++it)
		{
			if (0 <= *it && 0xFF >= *it) 
			{
				// small delta
				uint8_t delta = *it;
				buffer->write_1bytes(delta);
			}
			else
			{
				// large or negative delta
				buffer->write_2bytes(*it);
			}
		}

		while ((m_pkt_len % 4) != 0)
		{
			buffer->write_1bytes(0);
			m_pkt_len++;
		}

		m_encoded_chucks.clear();
		m_pkt_deltas.clear();

		return true;
	}

	

	crtcp_nack::crtcp_nack(uint32 sender_ssrc)
		: crtcp_common()
		, m_media_ssrc(0)
		, m_lost_sns()
	{
		m_header.padding = 0;
		m_header.type = ERtcpType_rtpfb;
		m_header.rc = 1;
		m_header.version = kRtcpVersion;
		m_ssrc = sender_ssrc;
		m_media_ssrc = 0;
	}

	crtcp_nack::~crtcp_nack()
	{
	}

	uint32 crtcp_nack::get_media_ssrc() const
	{
		return m_media_ssrc;
	}

	std::vector<uint16> crtcp_nack::get_lost_sns() const
	{
		std::vector<uint16> sn;
		for (std::set<uint16, cseq_compare_less>::iterator it = m_lost_sns.begin(); it != m_lost_sns.end(); ++it)
		{
			sn.push_back(*it);
		}
		return sn;
	}

	bool crtcp_nack::empty()
	{
		return m_lost_sns.empty();
	}

	void crtcp_nack::set_media_ssrc(uint32 ssrc)
	{
		m_media_ssrc = ssrc;
	}

	void crtcp_nack::add_lost_sn(uint16 sn)
	{
		m_lost_sns.insert(sn);
	}

	bool crtcp_nack::decode(cbuffer * buffer)
	{
		/*
   @doc: https://tools.ietf.org/html/rfc4585#section-6.1
	   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |V=2|P|   FMT   |       PT      |          length               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  SSRC of packet sender                        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  SSRC of media source                         |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  :            Feedback Control Information (FCI)                 :
  :                                                               :

   Generic NACK
   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |            PID                |             BLP               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   */
		 
		m_data = buffer->head();
		m_nb_data = buffer->left();

		if (!decode_header(buffer) )
		{
			WARNING_EX_LOG("decode header");
			return false;
		}

		m_media_ssrc = buffer->read_4bytes();
		char bitmask[20];
		for (int32 i = 0; i < (m_header.length - 2); i++)
		{
			uint16 pid = buffer->read_2bytes();
			uint16 blp = buffer->read_2bytes();
			m_lost_sns.insert(pid);
			memset(bitmask, 0, 20);
			for (int32 j = 0; j < 16; j++) 
			{
				bitmask[j] = (blp & (1 << j)) >> j ? '1' : '0';
				if ((blp & (1 << j)) >> j)
				{
					m_lost_sns.insert(pid + j + 1);
				}
			}
			bitmask[16] = '\n';
			NORMAL_EX_LOG("[%d] %d / %s", i, pid, bitmask);
		}

		return true;
	}

	uint64_t crtcp_nack::nb_bytes()
	{
		return kRtcpPacketSize;
	}

	bool crtcp_nack::encode(cbuffer * buffer)
	{
		/*
   @doc: https://tools.ietf.org/html/rfc4585#section-6.1
	   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |V=2|P|   FMT   |       PT      |          length               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  SSRC of packet sender                        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  SSRC of media source                         |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  :            Feedback Control Information (FCI)                 :
  :                                                               :

   Generic NACK
   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |            PID                |             BLP               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   */
		 
		if (!buffer->require(nb_bytes())) 
		{
			WARNING_EX_LOG("requires %d bytes", nb_bytes());
			return false;
		}

		std::vector<cpid_blp> chunks;
		do {
			cpid_blp chunk;
			chunk.in_use = false;
			uint16_t pid = 0;
			for (std::set<uint16_t, cseq_compare_less>::iterator it = m_lost_sns.begin(); it != m_lost_sns.end(); ++it) 
			{
				uint16_t sn = *it;
				if (!chunk.in_use) 
				{
					chunk.pid = sn;
					chunk.blp = 0;
					chunk.in_use = true;
					pid = sn;
					continue;
				}
				if ((sn - pid) < 1) 
				{
					NORMAL_EX_LOG("skip seq %d", sn);
				}
				else if ((sn - pid) > 16)
				{
					// append full chunk
					chunks.push_back(chunk);

					// start new chunk
					chunk.pid = sn;
					chunk.blp = 0;
					chunk.in_use = true;
					pid = sn;
				}
				else 
				{
					chunk.blp |= 1 << (sn - pid - 1);
				}
			}
			if (chunk.in_use)
			{
				chunks.push_back(chunk);
			}

			m_header.length = 2 + chunks.size();
			if (!encode_header(buffer))
			{
				WARNING_EX_LOG("encode header");
				break;
			}

			buffer->write_4bytes(m_media_ssrc);
			for (std::vector<cpid_blp>::iterator it_chunk = chunks.begin(); it_chunk != chunks.end(); it_chunk++) {
				buffer->write_2bytes(it_chunk->pid);
				buffer->write_2bytes(it_chunk->blp);
			}
		} while (0);

		return true;
	}

	crtcp_psfb_common::crtcp_psfb_common()
		: crtcp_common()
		, m_media_ssrc(0)
	{
		m_header.padding = 0;
		m_header.type = ERtcpType_psfb;
		m_header.rc = 1;
		m_header.version = kRtcpVersion;
	}

	crtcp_psfb_common::~crtcp_psfb_common()
	{
	}

	uint32 crtcp_psfb_common::get_media_ssrc() const
	{
		return m_media_ssrc;
	}

	void crtcp_psfb_common::set_media_ssrc(uint32 ssrc)
	{
		m_media_ssrc = ssrc;
	}

	bool crtcp_psfb_common::decode(cbuffer * buffer)
	{
		/*
   @doc: https://tools.ietf.org/html/rfc4585#section-6.1
	   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |V=2|P|   FMT   |       PT      |          length               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  SSRC of packet sender                        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  SSRC of media source                         |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  :            Feedback Control Information (FCI)                 :
  :                                                               :
  */
		 
		m_data = buffer->head();
		m_nb_data = buffer->left();

		if (! decode_header(buffer)) 
		{
			WARNING_EX_LOG("decode header");
			return false;
		}

		m_media_ssrc = buffer->read_4bytes();
		int32 len = (m_header.length + 1) * 4 - 12;
		buffer->skip(len);
		return true;
	}

	uint64 crtcp_psfb_common::nb_bytes()
	{
		return kRtcpPacketSize;
	}

	bool crtcp_psfb_common::encode(cbuffer * buffer)
	{
		WARNING_EX_LOG("not support");
		return false;
	}

	crtcp_pli::crtcp_pli(uint32_t sender_ssrc)
		: crtcp_psfb_common()
	{
		m_header.padding = 0;
		m_header.type = ERtcpType_psfb;
		m_header.rc = kPLI;
		m_header.version = kRtcpVersion;
		m_ssrc = sender_ssrc;
	}

	crtcp_pli::~crtcp_pli()
	{
	}

	bool crtcp_pli::decode(cbuffer * buffer)
	{
		/*
  @doc: https://tools.ietf.org/html/rfc4585#section-6.1
	  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |V=2|P|   FMT   |       PT      |          length               |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                  SSRC of packet sender                        |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                  SSRC of media source                         |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 :            Feedback Control Information (FCI)                 :
 :                                                               :
 */

		//srs_error_t err = srs_success;
		m_data = buffer->head();
		m_nb_data = buffer->left();

		if (! decode_header(buffer))
		{
			WARNING_EX_LOG("decode header");
			return false;
		}

		m_media_ssrc = buffer->read_4bytes();
		return true;
	}

	uint64_t crtcp_pli::nb_bytes()
	{
		return 12;
	}

	bool crtcp_pli::encode(cbuffer * buffer)
	{
		/*
   @doc: https://tools.ietf.org/html/rfc4585#section-6.1
	   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |V=2|P|   FMT   |       PT      |          length               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  SSRC of packet sender                        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  SSRC of media source                         |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  :            Feedback Control Information (FCI)                 :
  :                                                               :
   */
		 
		if (!buffer->require(nb_bytes())) 
		{
			WARNING_EX_LOG("requires %d bytes", nb_bytes());
			return false;
		}

		m_header.length = 2;
		if (! encode_header(buffer)) 
		{
			WARNING_EX_LOG("encode header");
			return false;
		}

		buffer->write_4bytes(m_media_ssrc);
		return true;
	}

	crtcp_sli::crtcp_sli(uint32 sender_ssrc)
		: crtcp_psfb_common()
		, m_first(0)
		, m_number(0)
		, m_picture(0) 
	{
		m_header .padding = 0;
		m_header .type = ERtcpType_psfb;
		m_header .rc = kSLI;
		m_header .version = kRtcpVersion;
		m_ssrc = sender_ssrc;
	}

	crtcp_sli::~crtcp_sli()
	{
	}

	bool crtcp_sli::decode(cbuffer * buffer)
	{
		/*
	@doc: https://tools.ietf.org/html/rfc4585#section-6.1
		0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|   FMT   |       PT      |          length               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of packet sender                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of media source                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :            Feedback Control Information (FCI)                 :
   :                                                               :


	@doc: https://tools.ietf.org/html/rfc4585#section-6.3.2
	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |            First        |        Number           | PictureID |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   */

		 
		m_data = buffer->head();
		m_nb_data = buffer->left();

		if (! decode_header(buffer)) 
		{
			WARNING_EX_LOG("decode header");
			return false;
		}

		m_media_ssrc = buffer->read_4bytes();
		int32 len = (m_header.length + 1) * 4 - 12;
		buffer->skip(len);
		return true;
	}

	uint64 crtcp_sli::nb_bytes()
	{
		return kRtcpPacketSize;
	}

	bool crtcp_sli::encode(cbuffer * buffer)
	{
		NORMAL_EX_LOG("warn");
		return true;
	}

	crtcp_rpsi::crtcp_rpsi(uint32 sender_ssrc)
		: crtcp_psfb_common()
		, m_pb(0)
		, m_payload_type(0)
		, m_native_rpsi(NULL)
		, m_nb_native_rpsi(0) 
	{
		m_header.padding = 0;
		m_header.type = ERtcpType_psfb;
		m_header.rc = kRPSI;
		m_header.version = kRtcpVersion;
		m_ssrc = sender_ssrc;
	}

	crtcp_rpsi::~crtcp_rpsi()
	{
	}

	bool crtcp_rpsi::decode(cbuffer * buffer)
	{
		/*
	@doc: https://tools.ietf.org/html/rfc4585#section-6.1
		0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|   FMT   |       PT      |          length               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of packet sender                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of media source                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :            Feedback Control Information (FCI)                 :
   :                                                               :


	@doc: https://tools.ietf.org/html/rfc4585#section-6.3.3
	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      PB       |0| Payload Type|    Native RPSI bit string     |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |   defined per codec          ...                | Padding (0) |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   */

		 
		m_data = buffer->head();
		m_nb_data = buffer->left();

		if (! decode_header(buffer)) 
		{
			WARNING_EX_LOG("decode header");
			return false;
		}

		m_media_ssrc = buffer->read_4bytes();
		int32 len = (m_header.length + 1) * 4 - 12;
		buffer->skip(len);
		return true;
	}

	uint64 crtcp_rpsi::nb_bytes()
	{
		return kRtcpPacketSize;
	}

	bool crtcp_rpsi::encode(cbuffer * buffer)
	{
		NORMAL_EX_LOG("warn");
		return true;
	}

	crtcp_xr::crtcp_xr(uint32 ssrc)
		:crtcp_common()
	{
		m_header.padding = 0;
		m_header.type = ERtcpType_xr;
		m_header.rc = 0;
		m_header.version = kRtcpVersion;
		m_ssrc = ssrc;
	}

	crtcp_xr::~crtcp_xr()
	{
	}

	bool crtcp_xr::decode(cbuffer * buffer)
	{
		/*
	@doc: https://tools.ietf.org/html/rfc3611#section-2
	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|reserved |   PT=XR=207   |             length            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                              SSRC                             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :                         report blocks                         :
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   */

		 
		m_data = buffer->head();
		m_nb_data = buffer->left();

		if (!decode_header(buffer)) 
		{
			WARNING_EX_LOG("decode header");
			return false; 
		}

		int32 len = (m_header.length + 1) * 4 - 8;
		buffer->skip(len);
		return true;
	}

	uint64 crtcp_xr::nb_bytes()
	{
		return kRtcpPacketSize;
	}

	bool crtcp_xr::encode(cbuffer * buffer)
	{
		WARNING_EX_LOG("not support");
		return false;
	}

}