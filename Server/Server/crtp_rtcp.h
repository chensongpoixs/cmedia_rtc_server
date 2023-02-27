/***********************************************************************************************
created: 		2023-02-20

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_RTP_RTCP_H_
#define _C_RTP_RTCP_H_
#include "cnet_type.h"
#include <sstream>
#include "cencoder.h"
#include <vector>
#include "crtp_rtcp_define.h"
#include <map>
#include <set>

namespace chen {

	class crtcp_common : public iccodec
	{
	public:
		explicit crtcp_common()
		: iccodec()
		, m_header()
		, m_ssrc(0)
		, m_payload()
		, m_payload_len(0)
		, m_data(NULL)
		, m_nb_data(0){}
		virtual ~crtcp_common();
	protected:
	protected:
		bool decode_header(cbuffer *buffer);
		bool encode_header(cbuffer *buffer);
	public:
		/*crtcp_common();
		virtual ~crtcp_common();*/
		virtual uint8_t type() const;
		virtual uint8_t get_rc() const;

		uint32_t get_ssrc();
		void set_ssrc(uint32_t ssrc);

		char* data();
		int32 size();
		// interface ISrsCodec
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64_t nb_bytes();
		virtual bool encode(cbuffer *buffer);
	protected:

		crtcp_header		m_header;
		uint32				m_ssrc;
		uint8				m_payload[kRtcpPacketSize];
		int32				m_payload_len;

		char*				m_data;
		int32				m_nb_data;
	};

	









	class SrsRtcpApp : public crtcp_common
	{
	
	public:
		SrsRtcpApp();
		
		virtual ~SrsRtcpApp();

		static bool is_rtcp_app(uint8 *data, int32 nb_data);

		virtual uint8 type() const;

		uint8 get_subtype() const;
		std::string get_name() const;
		bool get_payload(uint8*& payload, int32& len);

		bool set_subtype(uint8 type);
		bool set_name(std::string name);
		bool set_payload(uint8* payload, int32 len);
		// interface ISrsCodec
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64 nb_bytes();
		virtual bool encode(cbuffer *buffer);
	private:
		uint8		m_name[4];
	};


	struct crtcp_rb
	{
		uint32_t ssrc;
		uint8_t fraction_lost;
		uint32_t lost_packets;
		uint32_t highest_sn;
		uint32_t jitter;
		uint32_t lsr;
		uint32_t dlsr;

		crtcp_rb() {
			ssrc = 0;
			fraction_lost = 0;
			lost_packets = 0;
			highest_sn = 0;
			jitter = 0;
			lsr = 0;
			dlsr = 0;
		}
	};


	class crtcp_sr : public crtcp_common
	{
	public:
		explicit crtcp_sr();
		virtual ~crtcp_sr();

		uint8 get_rc() const;
		// overload SrsRtcpCommon
		virtual uint8 type() const;
		uint64 get_ntp() const;
		uint32 get_rtp_ts() const;
		uint32 get_rtp_send_packets() const;
		uint32 get_rtp_send_bytes() const;

		void set_ntp(uint64 ntp);
		void set_rtp_ts(uint32 ts);
		void set_rtp_send_packets(uint32 packets);
		void set_rtp_send_bytes(uint32 bytes);
		// interface ISrsCodec
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64 nb_bytes();
		virtual bool encode(cbuffer *buffer);
	private:
		uint64		m_ntp;
		uint32		m_rtp_ts; //rtp 网络时间戳
		uint32		m_send_rtp_packets; //发送的总包数
		uint32		m_send_rtp_bytes; //总共发送数据包量
	};



	class crtcp_rr : public crtcp_common
	{
	
	public:
		explicit crtcp_rr(uint32  sender_ssrc = 0);
		virtual ~crtcp_rr();

		// overload SrsRtcpCommon
		virtual uint8  type() const;

		uint32  get_rb_ssrc() const;
		float get_lost_rate() const;
		uint32  get_lost_packets() const;
		uint32  get_highest_sn() const;
		uint32  get_jitter() const;
		uint32  get_lsr() const;
		uint32  get_dlsr() const;

		void set_rb_ssrc(uint32  ssrc);
		void set_lost_rate(float rate);
		void set_lost_packets(uint32 count);
		void set_highest_sn(uint32  sn);
		void set_jitter(uint32  jitter);
		void set_lsr(uint32  lsr);
		void set_dlsr(uint32  dlsr);
		void set_sender_ntp(uint64  ntp);
		// interface ISrsCodec
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64  nb_bytes();
		virtual bool encode(cbuffer *buffer);
	private:
		crtcp_rb  m_rb;
	};


	// The "distance" between two uint16 number, for example:
//      distance(prev_value=3, value=5) === (int16_t)(uint16_t)((uint16_t)3-(uint16_t)5) === -2
//      distance(prev_value=3, value=65534) === (int16_t)(uint16_t)((uint16_t)3-(uint16_t)65534) === 5
//      distance(prev_value=65532, value=65534) === (int16_t)(uint16_t)((uint16_t)65532-(uint16_t)65534) === -2
// For RTP sequence, it's only uint16 and may flip back, so 3 maybe 3+0xffff.
// @remark Note that srs_rtp_seq_distance(0, 32768)>0 is TRUE by https://mp.weixin.qq.com/s/JZTInmlB9FUWXBQw_7NYqg
//      but for WebRTC jitter buffer it's FALSE and we follow it.
// @remark For srs_rtp_seq_distance(32768, 0)>0, it's FALSE definitely.
	inline int16   rtp_seq_distance(const uint16 & prev_value, const uint16 & value)
	{
		return (int16 )(value - prev_value);
	}
	// For map to compare the sequence of RTP.
	struct cseq_compare_less {
		bool operator()(const uint16& pre_value, const uint16& value) const 
		{
			return  rtp_seq_distance(pre_value, value) > 0;
		}
	};

	class crtcp_twcc : public crtcp_common
	{
	private:
		struct crtcp_twcc_chunk {
			uint8 delta_sizes[kTwccFbMaxBitElements];
			uint16 size;
			bool all_same;
			bool has_large_delta;
			crtcp_twcc_chunk()
				: delta_sizes()
				, size(0)
				, all_same(true)
				, has_large_delta(false) {}
		};
	public:
		explicit crtcp_twcc(uint32_t sender_ssrc = 0);
		virtual ~crtcp_twcc();

		uint32  get_media_ssrc() const;
		uint16  get_base_sn() const;
		uint32  get_reference_time() const;
		uint8  get_feedback_count() const;
		std::vector<uint16 > get_packet_chucks() const;
		std::vector<uint16 > get_recv_deltas() const;

		void set_media_ssrc(uint32  ssrc);
		void set_base_sn(uint16  sn);
		void set_reference_time(uint32  time);
		void set_feedback_count(uint8  count);
		void add_packet_chuck(uint16  chuck);
		void add_recv_delta(uint16  delta);

		bool recv_packet(uint16  sn, uint64 ts);
		bool need_feedback();

		// interface ISrsCodec
	private:
		void clear();
		uint64 calculate_delta_us(uint64 ts, uint64 last);
		bool process_pkt_chunk(crtcp_twcc_chunk& chunk, int delta_size);
		bool can_add_to_chunk(crtcp_twcc_chunk& chunk, int delta_size);
		void add_to_chunk(crtcp_twcc_chunk& chunk, int delta_size);
		bool encode_chunk(crtcp_twcc_chunk& chunk);
		bool encode_chunk_run_length(crtcp_twcc_chunk& chunk);
		bool encode_chunk_one_bit(crtcp_twcc_chunk& chunk);
		bool encode_chunk_two_bit(crtcp_twcc_chunk& chunk, size_t size, bool shift);
		void reset_chunk(crtcp_twcc_chunk& chunk);
		bool encode_remaining_chunk(crtcp_twcc_chunk& chunk);
	
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64  nb_bytes();
		virtual bool encode(cbuffer *buffer);
	private:
		bool do_encode(cbuffer *buffer);

	private:
	private:
		uint32														m_media_ssrc;
		uint16													    m_base_sn;
		int32														m_reference_time;
		uint8														m_fb_pkt_count;
		std::vector<uint16>											m_encoded_chucks;
		std::vector<uint16>											m_pkt_deltas;

		std::map<uint16, uint64>									m_recv_packets;
		std::set<uint16, cseq_compare_less>							m_recv_sns; 

		int32														m_pkt_len;
		uint16														m_next_base_sn;
	};


	class crtcp_nack : public crtcp_common
	{
	private:
		struct cpid_blp 
		{
			uint16 pid;
			uint16 blp;
			bool in_use;
		};

		
	public:
		explicit crtcp_nack(uint32 sender_ssrc = 0);
		virtual ~crtcp_nack();

		uint32  get_media_ssrc() const;
		std::vector<uint16 > get_lost_sns() const;
		bool empty();

		void set_media_ssrc(uint32  ssrc);
		void add_lost_sn(uint16  sn);
		// interface ISrsCodec
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64_t nb_bytes();
		virtual bool encode(cbuffer *buffer);
	private:
		uint32									m_media_ssrc;
		std::set<uint16, cseq_compare_less>		m_lost_sns;
	};

	class crtcp_psfb_common : public crtcp_common
	{
	
	public:
		explicit crtcp_psfb_common();
		virtual ~crtcp_psfb_common();

		uint32 get_media_ssrc() const;
		void set_media_ssrc(uint32 ssrc);

		// interface ISrsCodec
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64 nb_bytes();
		virtual bool encode(cbuffer *buffer);
	protected:
		uint32		m_media_ssrc;
	};


	class crtcp_pli : public crtcp_psfb_common
	{
	public:
		explicit crtcp_pli(uint32_t sender_ssrc = 0);
		virtual ~crtcp_pli();

		// interface ISrsCodec
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64_t nb_bytes();
		virtual bool encode(cbuffer *buffer);
	};



	class crtcp_sli : public crtcp_psfb_common
	{
	
	public:
		explicit crtcp_sli(uint32 sender_ssrc = 0);
		virtual ~crtcp_sli();

		// interface ISrsCodec
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64 nb_bytes();
		virtual bool encode(cbuffer *buffer);
	private:
		uint16				m_first;
		uint16				m_number;
		uint8				m_picture;
	};


	class crtcp_rpsi : public crtcp_psfb_common
	{
	

	public:
		explicit crtcp_rpsi(uint32 sender_ssrc = 0);
		virtual ~crtcp_rpsi();

		// interface ISrsCodec
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64 nb_bytes();
		virtual bool encode(cbuffer *buffer);

	private:
		uint8			m_pb;
		uint8			m_payload_type;
		char*			m_native_rpsi;
		int32			m_nb_native_rpsi;
	};
	class crtcp_xr : public crtcp_common
	{
	public:
		explicit crtcp_xr(uint32  ssrc = 0);
		virtual ~crtcp_xr();

		// interface ISrsCodec
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64 nb_bytes();
		virtual bool encode(cbuffer *buffer);
	};
	class crtcp_compound : public iccodec
	{
	public:
		explicit crtcp_compound()
			: m_rtcps()
			, m_nb_bytes()
			, m_data(NULL)
			, m_nb_data(0) {}
		virtual ~crtcp_compound();
	public:
		// TODO: FIXME: Should rename it to pop(), because it's not a GET method.
		crtcp_common* get_next_rtcp();
		bool add_rtcp(crtcp_common *rtcp);
		void clear();

		char* data();
		int32 size();

		// interface ISrsCodec
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64_t nb_bytes();
		virtual bool encode(cbuffer *buffer);
	protected:

	private:
		std::vector<crtcp_common*> m_rtcps;
		int32			m_nb_bytes;
		char*			m_data;
		int32			m_nb_data;
	};
}

#endif // _C_RTP_RTCP_H_