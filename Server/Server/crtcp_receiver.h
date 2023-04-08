/***********************************************************************************************
created: 		2023-04-08

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_RTCP_RECEIVER_H_
#define _C_RTCP_RECEIVER_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <set>

#include "ccommon_type.h"
#include "ccommon_header.h"
#include "crtp_fb.h"
#include "creport_block.h"
namespace chen {
	class crtcp_receiver
	{
	public:
		crtcp_receiver();
		~crtcp_receiver();
	public:
	private:
		struct cpacket_information;
		struct creport_block_with_rtt;
		// RTCP report blocks mapped by remote SSRC.
		using ReportBlockInfoMap = std::map<uint32_t, creport_block_with_rtt>;
		// RTCP report blocks map mapped by source SSRC.
		using ReportBlockMap = std::map<uint32_t, ReportBlockInfoMap>;
	private:

		bool _parse_compound_packet(uint8* packet_begin, const uint8* packet_end, cpacket_information * packet_information);
	
		void _trigger_callbacks_from_rtcp_packet(const cpacket_information& packet_information);
	private:
		void _handle_sender_report(const rtcp::CommonHeader& rtcp_block, cpacket_information* packet_information);
	




		void _handle_receiver_report(const rtcp::CommonHeader& rtcp_block, cpacket_information* packet_information);

		void _handle_report_block(const rtcp::creport_block& report_block, cpacket_information* packet_information, uint32  remote_ssrc);
	protected:
	private:
		std::set<uint32>		m_registered_ssrcs;


		ReportBlockMap			m_received_report_blocks;

		// The time we last received an RTCP RR telling we have successfully
		// delivered RTP packet to the remote side.
		int64					m_last_increased_sequence_number_ms;
		// The last time we received an RTCP Report block for this module.
		int64					m_last_received_rb_ms  ;
		RtcpPacketTypeCounter   m_packet_type_counter;

		size_t			m_num_skipped_packets;
		int64			m_last_skipped_packets_warning_ms;
	};
}


#endif // _C_RTCP_RECEIVER_H_