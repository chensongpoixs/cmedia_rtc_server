/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/

#ifndef _C_RTP_STREAM_RECV_H_
#define _C_RTP_STREAM_RECV_H_
#include <string>
#include "cnet_type.h"
#include "crtp_rtcp.h"
#include "RtpPacket.hpp"
#include "SenderReport.hpp"
#include "crate_calculator.h"
#include "crtp_stream.h"
#include "ReceiverReport.hpp"
namespace chen {
	class crtp_stream_recv : public crtp_stream
	{
	public:
		explicit crtp_stream_recv(const crtp_stream::crtp_stream_params & params);
		
		virtual ~crtp_stream_recv();

	public:
		 
	public:
		bool  receive_packet(RTC::RtpPacket* packet);
		bool  receive_rtx_packet(RTC::RtpPacket* packet);
		crtcp_rr get_rtcp_receiver_report();
		crtcp_rr get_rtx_rtcp_receiver_report();
		 void receive_rtcp_sender_report(crtcp_sr* report);
		 void receive_rtx_rtcp_sender_report(crtcp_sr* report);
		//void ReceiveRtcpXrDelaySinceLastRr(RTC::RTCP::DelaySinceLastRr::SsrcInfo* ssrcInfo);



		 void request_key_frame();
		/* void pause();
		 void pesume();*/
		 virtual void pause() ;
		 virtual void resume()  ;
		// virtual uint32  get_bitrate(uint64  nowMs)  ;
		 uint32  get_bitrate(uint64  nowMs)  
		 {
			 return m_media_transmission_counter .GetBitrate(nowMs);
		 }
		 /* uint32  get_bitrate(uint64  nowMs, uint8  spatialLayer, uint8  temporalLayer) override
		  {
			  return m_transmission_counter.GetBitrate(nowMs, spatialLayer, temporalLayer);
		  }*/
		 /*uint32  get_spatialLayerBitrate(uint64_t nowMs, uint8_t spatialLayer) override
		 {
			 return this->transmissionCounter.GetSpatialLayerBitrate(nowMs, spatialLayer);
		 }*/
		 /*uint32  GetLayerBitrate(uint64_t nowMs, uint8_t spatialLayer, uint8_t temporalLayer) override
		 {
			 return this->transmissionCounter.GetLayerBitrate(nowMs, spatialLayer, temporalLayer);
		 }*/

	public:
		//virtual void set_rtx(uint8 payload_type, uint32 ssrc);
		void del_rtx();
	protected:
		void onnack_generator_nack_required(const std::vector<uint16>& seqNumbers)  ;
		void onnack_generator_key_frame_required()  ;
	private:

		void _calculate_jitter(uint32 rtpTimestamp);

		 
		void update_score();
 
		

	private:
		uint32			m_expected_prior{ 0u };      // Packets expected at last interval.
		uint32			m_expected_prior_score{ 0u }; // Packets expected at last interval for score calculation.
		uint32			m_received_prior{ 0u };      // Packets received at last interval.
		uint32			m_received_prior_score{ 0u }; // Packets received at last interval for score calculation.
		uint32			m_last_sr_timestamp{ 0u };    // The middle 32 bits out of 64 in the NTP
										   // timestamp received in the most recent
										   // sender report.
		uint64			m_last_sr_received{ 0u };     // Wallclock time representing the most recent
										   // sender report arrival.
		int32			m_transit{ 0u };             // Relative transit time for prev packet.
		uint32			m_jitter{ 0u };
		uint8			m_fir_seq_number{ 0u };
		uint32			m_reported_packet_lost{ 0u };
		//std::unique_ptr<RTC::NackGenerator> nackGenerator;
		//Timer* inactivityCheckPeriodicTimer{ nullptr };
		bool			m_inactive{ false };
		//TransmissionCounter transmissionCounter;      // Valid media + valid RTX.
		RtpDataCounter m_media_transmission_counter; // Just valid media.


		 
	};
}

#endif // _C_RTP_STREAM_RECV_H_