/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/
#include "crtp_stream_recv.h"
#include "cuv_util.h"
namespace chen {
	crtp_stream_recv::~crtp_stream_recv()
	{
	}
	bool crtp_stream_recv::receive_packet(RTC::RtpPacket * packet)
	{
		_calculate_jitter(packet->GetTimestamp());
		if (m_inactive)
		{
			m_inactive = false;
		}
		return false;
	}
	bool crtp_stream_recv::receive_rtx_packet(RTC::RtpPacket * packet)
	{
		return false;
	}
	void crtp_stream_recv::receive_rtcp_sender_report(RTC::RTCP::SenderReport * report)
	{
	}
	void crtp_stream_recv::receive_rtx_rtcp_sender_report(RTC::RTCP::SenderReport * report)
	{
	}
	void crtp_stream_recv::_calculate_jitter(uint32 rtpTimestamp)
	{
		int32 transit = static_cast<int32>(uv_util::GetTimeMs() - (rtpTimestamp * 1000 / 9000/*this->params.clockRate*/));
		int32 d = transit - m_transit;

		// First transit calculation, save and return.
		if (m_transit == 0)
		{
			m_transit = transit;

			return;
		}

		m_transit = transit;

		if (d < 0)
		{
			d = -d;
		}

		m_jitter += (1. / 16.) * (static_cast<double>(d) - m_jitter);
	}
}