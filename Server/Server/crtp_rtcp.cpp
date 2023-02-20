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

		if (!  decode_header(buffer) ) 
		{
			WARNING_EX_LOG("decode header");
			//return srs_error_wrap(err, "decode header");
		}

		m_payload_len = (m_header.length + 1) * 4 - sizeof(crtcp_common) - 4;
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
				//rtcp = new SrsRtcpSR();
			}
			else if (header->type == ERtcpType_rr) 
			{
				//rtcp = new SrsRtcpRR();
			}
			else if (header->type == ERtcpType_rtpfb) 
			{
				if (1 == header->rc) 
				{
					//nack
					//rtcp = new SrsRtcpNack();
				}
				else if (15 == header->rc)
				{
					//twcc
					//rtcp = new SrsRtcpTWCC();
				}
			}
			else if (header->type == ERtcpType_psfb)
			{
				if (1 == header->rc) 
				{
					// pli
					//rtcp = new SrsRtcpPli();
				}
				else if (2 == header->rc) 
				{
					//sli
					//rtcp = new SrsRtcpSli();
				}
				else if (3 == header->rc)
				{
					//rpsi
					//rtcp = new SrsRtcpRpsi();
				}
				else
				{
					// common psfb
					//rtcp = new SrsRtcpPsfbCommon();
				}
			}
			else if (header->type == ERtcpType_xr) 
			{
				//rtcp = new SrsRtcpXr();
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
}