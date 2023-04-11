/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/

#ifndef _C_RTP_STREAM_H_
#define _C_RTP_STREAM_H_
#include <string>
#include "cnet_type.h"
#include "crtp_rtcp.h"
#include "RtpPacket.hpp"
#include "SenderReport.hpp"
#include "crate_calculator.h"
#include "ReceiverReport.hpp"
#include "crtx_stream.h"
namespace chen {
	class crtp_stream
	{
	public:
		struct crtp_stream_params
		{
			uint8			 payload_type;
			uint32			 ssrc;
			std::string		 type; // audio, video, datachannel
			std::string		 subtype; // opus, h264, ...
			uint32			 clock_rate;
							 
			uint8			 rtx_payload_type;
			uint32			 rtx_ssrc;
			//////////////////////////////////////////////////////////////////////////
			bool			use_nack;
			bool			use_pli;
			bool			use_fir;
			bool			use_inband_fec;
			bool			use_dtx;
			//crtp_stream_params() : {}
		};
	public:
		explicit crtp_stream(const crtp_stream_params & params)
			: m_params(params)
			, m_started(false)
			, m_max_seq(0u)
			, m_cycles(0u)
			, m_base_seq(0u)
			, m_bad_seq(0u)
			, m_max_packet_ts(0u)
			, m_max_packet_ms(0u)
			, m_packets_lost(0u)
			, m_fraction_lost(0u)
			, m_packets_discarded(0u)
			, m_packets_retransmitted(0u)
			, m_packets_repaired(0u)
			, m_nack_count(0u)
			, m_nack_packet_count(0u)
			, m_pli_count(0u)
			, m_fir_count(0u)
			, m_repaired_prior_score(0u)
			, m_retransmitted_prior_score(0u)
			, m_last_sender_report_ntp_ms(0u)
			, m_last_sender_repor_ts(0u)
			, m_rtt(0)
			, m_has_rtt(false)
			, m_rtx_stream_ptr(NULL)
			, m_score(0u)
			, m_scores()
			, m_active_since_ms(0u)
		{}
		~crtp_stream();

	public:

		uint32  get_ssrc() const
		{
			return m_params.ssrc;
		}
		uint8_t get_payload_type() const
		{
			return m_params.payload_type;
		}
		const std::string& get_type() const
		{
			return m_params.type;
		}
		uint32  get_clock_rate() const
		{
			return m_params.clock_rate;
		}
		 
		 
		bool has_rtx() const
		{
			return m_rtx_stream_ptr != nullptr;
		}
		virtual void set_rtx(uint8 payload_type, uint32 ssrc);
		uint32 get_rtx_ssrc() const
		{
			return m_params.rtx_ssrc;
		}
		uint8 get_rtx_payload_type() const
		{
			return m_params.rtx_payload_type;
		}
		 
		 
		 
		virtual bool receive_packet(RTC::RtpPacket* packet);
		virtual void pause() = 0;
		virtual void resume() = 0;
		virtual uint32  get_bitrate(uint64  nowMs) = 0;
		//virtual uint32  get_bitrate(uint64  nowMs, uint8_t spatialLayer, uint8_t temporalLayer) = 0;
		 void reset_score(uint8  score, bool notify);
		uint8  get_fraction_lost() const
		{
			return m_fraction_lost;
		}
		float get_loss_percentage() const
		{
			return static_cast<float>(m_fraction_lost) * 100 / 256;
		}
		float get_rtt() const
		{
			return m_rtt;
		}
		uint64  get_max_packet_ms() const
		{
			return m_max_packet_ms;
		}
		uint32  get_max_packet_ts() const
		{
			return m_max_packet_ts;
		}
		uint64  get_sender_report_ntp_ms() const
		{
			return m_last_sender_report_ntp_ms;
		}
		uint32   get_sender_report_ts() const
		{
			return m_last_sender_repor_ts;
		}
		uint8  get_score() const
		{
			return m_score;
		}
		uint64  get_active_ms() const
		{
			return uv_util::GetTimeMs() - m_active_since_ms;
		}

	protected:
	protected:
		bool update_seq(RTC::RtpPacket* packet);
		void update_score(uint8 score);
		void packet_retransmitted(RTC::RtpPacket* packet);
		void packet_repaired(RTC::RtpPacket* packet);

		uint32  get_expected_packets() const
		{
			return (m_cycles + m_max_seq) - m_base_seq + 1;
		}

	private:
		void _init_seq(uint16 seq);
		protected:

		crtp_stream_params				m_params;
		bool							m_started;
		//////////////////////////////////////////////////
		uint16							m_max_seq;
		uint32							m_cycles;
		uint32							m_base_seq;
		uint32							m_bad_seq;


		/////////////////////////////////////////
		uint32							m_max_packet_ts;
		uint64							m_max_packet_ms;


		uint32							m_packets_lost;
		uint8							m_fraction_lost;


		size_t							m_packets_discarded;

		size_t							m_packets_retransmitted{ 0u };
		size_t							m_packets_repaired{ 0u };
		size_t							m_nack_count{ 0u };
		size_t							m_nack_packet_count{ 0u };
		size_t							m_pli_count{ 0u };
		size_t							m_fir_count{ 0u };
		size_t							m_repaired_prior_score{ 0u }; // Packets repaired at last interval for score calculation.
		size_t							m_retransmitted_prior_score{ 0u }; // Packets retransmitted at last interval for score calculation.
		uint64 	    					m_last_sender_report_ntp_ms{ 0u }; // NTP timestamp in last Sender Report (in ms).
		uint32 	     					m_last_sender_repor_ts{ 0u };     // RTP timestamp in last Sender Report.
		float							m_rtt{ 0 };
		bool							m_has_rtt{ false };

	   crtx_stream*						m_rtx_stream_ptr;


	   // Score related.
	   uint8							m_score{ 0u };
	   std::vector<uint8 >				m_scores;
	   // Last time since the stream is active.
	   uint64							m_active_since_ms{ 0u };
	private:
		
	};
}

#endif // _C_RTP_STREAM_H_