/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/

#ifndef _C_RTX_STREAM_H_
#define _C_RTX_STREAM_H_
#include <string>
#include "cnet_type.h"
#include "crtp_rtcp.h"
#include "RtpPacket.hpp"
#include "SenderReport.hpp"
#include "crate_calculator.h"
#include "ReceiverReport.hpp"
namespace chen {
	class crtx_stream 
	{
	public:
		struct crtx_params
		{
			uint8		payload_type;
			uint32		ssrc;
			std::string type;
			std::string subtype;
			uint32		clock_rate;
		};
	public:
		explicit crtx_stream(const crtx_params& params) 
		: m_params (params)
		, m_max_seq(0u)
		, m_cycles(0u)
		, m_base_seq(0u)
		, m_bad_seq(0u)
		, m_max_packet_ts(0u)
		, m_max_packet_ms(0u)
		, m_packets_lost(0u)
		, m_fraction_lost(0u)
		, m_packets_discarded(0u)
		, m_packets_count(0u)
		, m_expected_prior(0u)
		, m_received_prior(0u)
		, m_started(false)
		, m_last_sr_timestamp(0u)
		, m_last_sr_received(0u)
		, m_reported_packet_lost(0u){}
		~crtx_stream();

	public:
		uint32 get_ssrc() const
		{
			return m_params.ssrc;
		}
		uint8  get_payload_type() const
		{
			return m_params.payload_type;
		}
		const std::string& get_mime_type() const
		{
			return m_params.subtype;
		}
		uint32   get_clock_rate() const
		{
			return m_params.clock_rate;
		}
		 
		 
		uint8  get_fraction_lost() const
		{
			return m_fraction_lost;
		}
		float get_loss_percentage() const
		{
			return static_cast<float>(m_fraction_lost) * 100 / 256;
		}
		size_t get_packets_discarded() const
		{
			return m_packets_discarded;
		} 
		bool receive_packet(RTC::RtpPacket* packet);
		RTC::RTCP::ReceiverReport* get_rtcp_receiver_report();
		void receive_rtcp_sender_report(RTC::RTCP::SenderReport* report);
	protected:
		bool update_seq(RTC::RtpPacket* packet);

		uint32  get_expected_packets() const
		{
			return (m_cycles + m_max_seq) - m_base_seq + 1;
		}
	private:

		void _init_seq(uint16 seq);
	private:
		crtx_params			m_params;


		uint16				m_max_seq;
		uint32				m_cycles;
		uint32				m_base_seq;
		uint32				m_bad_seq;


		/////////////////////////////////////////
		uint32				m_max_packet_ts;
		uint64				m_max_packet_ms;


		uint32				m_packets_lost;
		uint8				m_fraction_lost;

		size_t				m_packets_discarded;
		size_t              m_packets_count ;

		bool				m_started;


		// Fields for generating Receiver Reports.
		uint32				m_expected_prior;
		uint32				m_received_prior;
		/// SR 反馈信息
		uint32				m_last_sr_timestamp{ 0u };
		uint64				m_last_sr_received{ 0u };
		uint32				m_reported_packet_lost{ 0u };
	};
}
#endif //_C_RTX_STREAM_H_