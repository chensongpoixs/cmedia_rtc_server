/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_REMOTE_ESTIMATOR_PROXY_H_
#define _C_REMOTE_ESTIMATOR_PROXY_H_
#include "cnet_type.h"
#include <sstream>
#include "cencoder.h"
#include <vector>
#include "crtp_rtcp_define.h"
#include <map>
#include <set>
#include "ctransport_feedback.h"


namespace chen {
	class crtc_transport;
	class cremote_estimator_proxy
	{
	public:
		explicit cremote_estimator_proxy(crtc_transport * ptr)
			: m_media_ssrc(0)
			, m_feedback_packet_count(0)
			, m_periodic_window_start_seq(0)
			, m_packet_arrival_times()
			, m_send_interval_ms(100/*default 100*/)
			, m_rtc_transport_ptr(ptr){}
		~cremote_estimator_proxy();

		bool init();
		void update(uint32 uDeltaTime);
		void destroy();
	public:
		void on_packet_arrival(uint16 sequence_number, uint32 media_ssrc, int64 arrival_time);


		void send_periodic_Feedbacks();
	public:
	public:
	private:
		static int64_t BuildFeedbackPacket(uint8_t feedback_packet_count, uint32_t media_ssrc, int64_t base_sequence_number,
			std::map<int64, int64>::const_iterator begin_iterator,  // |begin_iterator| is inclusive.
			std::map<int64, int64>::const_iterator end_iterator,  // |end_iterator| is exclusive.
			rtcp::TransportFeedback* feedback_packet);
	private:
		uint32							m_media_ssrc;
		uint8							m_feedback_packet_count;
		int64							m_periodic_window_start_seq;
		std::map<int64, int64>			m_packet_arrival_times;
		int64							m_send_interval_ms;
		crtc_transport *				m_rtc_transport_ptr;
		
	};
}

#endif //_C_REMOTE_ESTIMATOR_PROXY_H_