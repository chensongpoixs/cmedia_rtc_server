/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/


#include "crtcp_receiver.h"
#include "crtp_rtcp_define.h"
#include "ctransport_feedback.h"
#include "cvideo_bitrate_allocation.h"
#include "ccommon_header.h"
#include "cuv_util.h"
#include "csender_report.h"
#include "ctime_util.h"
namespace chen {

	struct crtcp_receiver::cpacket_information
	{
		// TODO@chensong 2023-03-29 每一位代表一种RTCP的消息
		uint32						packet_type_flags = 0;  // RTCPPacketTypeFlags bit field. 

		uint32_t					remote_ssrc = 0; // 远端的ssrc
		std::vector<uint16_t>		nack_sequence_numbers;  // 保存掉包的seq
		std::list<crtc_report_block>			report_blocks; // 
		int64						rtt_ms = 0;// 数据包周期时长 一个包一去一回的时间长
		uint32						receiver_estimated_max_bitrate_bps = 0; //接受端评估最大的带宽
		std::unique_ptr<rtcp::TransportFeedback> transport_feedback;
		VideoBitrateAllocation                    target_bitrate_allocation;
		//absl::optional<VideoBitrateAllocation> target_bitrate_allocation;
		//std::unique_ptr<rtcp::LossNotification> loss_notification;
	};

	struct crtcp_receiver::creport_block_with_rtt {
		crtc_report_block report_block;

		int64_t last_rtt_ms = 0;
		int64_t min_rtt_ms = 0;
		int64_t max_rtt_ms = 0;
		int64_t sum_rtt_ms = 0;
		size_t num_rtts = 0;
	};

	crtcp_receiver::crtcp_receiver()
	{
	}
	crtcp_receiver::~crtcp_receiver()
	{
	}
	bool crtcp_receiver::_parse_compound_packet(uint8 * packet_begin, const uint8 * packet_end, cpacket_information * packet_information)
	{
		rtcp::CommonHeader rtcp_block;
		for (const uint8_t* next_block = packet_begin; next_block != packet_end; next_block = rtcp_block.NextPacket())
		{
			ptrdiff_t remaining_blocks_size = packet_end - next_block;
			RTC_DCHECK_GT(remaining_blocks_size, 0);
			// rtcp包有统一头格式读取方法
			if (!rtcp_block.Parse(next_block, remaining_blocks_size))
			{
				if (next_block == packet_begin)
				{
					// Failed to parse 1st header, nothing was extracted from this packet.
					 WARNING_EX_LOG( "Incoming invalid RTCP packet");
					return false;
				}
				++m_num_skipped_packets;
				break;
			}

			 if (m_packet_type_counter.first_packet_time_ms == -1)
			 {
				 m_packet_type_counter.first_packet_time_ms = uv_util::GetTimeMs(); /*clock_->TimeInMilliseconds()*/;
			} 

			switch (rtcp_block.type())
			{
			case rtcp::csender_report::kPacketType:
				//HandleSenderReport(rtcp_block, packet_information);
				//break;
			//case rtcp::ReceiverReport::kPacketType: {
			//	HandleReceiverReport(rtcp_block, packet_information);
			//	break;
			//}
			//case rtcp::Sdes::kPacketType:
			//	HandleSdes(rtcp_block, packet_information);
			//	break;
			//case rtcp::ExtendedReports::kPacketType: // TODO@chensong 2022-12-20  反馈target_bitrate带宽 kbps/s ==> 意义是什么呢
			//	HandleXr(rtcp_block, packet_information);
			//	break;
			//case rtcp::Bye::kPacketType:
			//	HandleBye(rtcp_block);
			//	break;
			//case rtcp::Rtpfb::kPacketType:
			//	switch (rtcp_block.fmt()) {
			//	case rtcp::Nack::kFeedbackMessageType: {
			//		// TODO@chensong 处理Nack信息包 丢包的处理
			//		HandleNack(rtcp_block, packet_information);
			//		break;
			//	}
			//	case rtcp::Tmmbr::kFeedbackMessageType:
			//		HandleTmmbr(rtcp_block, packet_information);
			//		break;
			//	case rtcp::Tmmbn::kFeedbackMessageType:
			//		HandleTmmbn(rtcp_block, packet_information);
			//		break;
			//	case rtcp::RapidResyncRequest::kFeedbackMessageType:
			//		HandleSrReq(rtcp_block, packet_information);
			//		break;
			//	case rtcp::TransportFeedback::kFeedbackMessageType:
			//		HandleTransportFeedback(rtcp_block, packet_information);
			//		break;
			//	default:
			//		++num_skipped_packets_;
			//		break;
			//	}
			//	break;
			//case rtcp::Psfb::kPacketType:
			//	switch (rtcp_block.fmt()) {
			//	case rtcp::Pli::kFeedbackMessageType:
			//		HandlePli(rtcp_block, packet_information);
			//		break;
			//	case rtcp::Fir::kFeedbackMessageType:
			//		HandleFir(rtcp_block, packet_information);
			//		break;
			//	case rtcp::Psfb::kAfbMessageType:  // TODO@chensong 2022-11-28
			//									   // 接受网络带宽bps [goole old net ]
			//	{
			//		HandlePsfbApp(rtcp_block, packet_information);
			//		break;
			//	}
			//	default:
			//		++num_skipped_packets_;
			//		break;
			//	}
			//	break;
			default:
				++m_num_skipped_packets;
				break;
			}
		}

		/*if (packet_type_counter_observer_)
		{
			packet_type_counter_observer_->RtcpPacketTypesCounterUpdated(main_ssrc_, packet_type_counter_);
		}*/

		/*int64_t now_ms = clock_->TimeInMilliseconds();
		if (now_ms - last_skipped_packets_warning_ms_ >= kMaxWarningLogIntervalMs && num_skipped_packets_ > 0)
		{
			last_skipped_packets_warning_ms_ = now_ms;
			RTC_LOG(LS_WARNING)
				<< num_skipped_packets_
				<< " RTCP blocks were skipped due to being malformed or of "
				"unrecognized/unsupported type, during the past "
				<< (kMaxWarningLogIntervalMs / 1000) << " second period.";
		}*/

		return true;
	}

	void crtcp_receiver::_trigger_callbacks_from_rtcp_packet(const cpacket_information & packet_information)
	{
		// Process TMMBR and REMB first to avoid multiple callbacks
		// to OnNetworkChanged.
		//if (packet_information.packet_type_flags & kRtcpTmmbr)
		//{
		//	// Might trigger a OnReceivedBandwidthEstimateUpdate.
		//	// TODO@chensong 2022-12-20 根据接收端反馈网络带宽 更新带宽模块 bandwidth ？？？ [现在抛弃？]
		//	NotifyTmmbrUpdated();
		//}
		//uint32_t local_ssrc;
		//std::set<uint32_t> registered_ssrcs;
		//{
		//	// We don't want to hold this critsect when triggering the callbacks below.
		//	rtc::CritScope lock(&rtcp_receiver_lock_);
		//	local_ssrc = main_ssrc_;
		//	registered_ssrcs = registered_ssrcs_;
		//}
		// TODO@chensong 2022-12-20 receiver_only_ default false 
		/*if (!receiver_only_ && (packet_information.packet_type_flags & kRtcpSrReq))
		{
			rtp_rtcp_->OnRequestSendReport();
		}*/
		// TODO@chensong 发送RTX丢包信息
		//if (!receiver_only_ && (packet_information.packet_type_flags & kRtcpNack))
		//{
		//	if (!packet_information.nack_sequence_numbers.empty())
		//	{
		//		RTC_LOG(LS_VERBOSE) << "Incoming NACK length: " << packet_information.nack_sequence_numbers.size();
		//		// 请求重新发送seq的包   ModuleRtpRtcpImpl->OnReceivedNack
		//		rtp_rtcp_->OnReceivedNack(packet_information.nack_sequence_numbers);
		//	}
		//}

		// We need feedback that we have received a report block(s) so that we
		// can generate a new packet in a conference relay scenario, one received
		// report can generate several RTCP packets, based on number relayed/mixed
		// a send report block should go out to all receivers.
		// TODO@chensong 2022-12-20 接受端请求立即刷新帧 (sps、pps信息)
		//if (rtcp_intra_frame_observer_)
		//{
		//	RTC_DCHECK(!receiver_only_);
		//	if ((packet_information.packet_type_flags & kRtcpPli) || (packet_information.packet_type_flags & kRtcpFir))
		//	{
		//		if (packet_information.packet_type_flags & kRtcpPli)
		//		{
		//			RTC_LOG(LS_VERBOSE) << "Incoming PLI from SSRC " << packet_information.remote_ssrc;
		//		}
		//		else
		//		{
		//			RTC_LOG(LS_VERBOSE) << "Incoming FIR from SSRC " << packet_information.remote_ssrc;
		//		}
		//		// TODO@chensong 2022-12-20  EncoderRtcpFeedback::OnReceivedIntraFrameRequest
		//		rtcp_intra_frame_observer_->OnReceivedIntraFrameRequest(local_ssrc);
		//	}
		//}
		/*if (rtcp_loss_notification_observer_ && (packet_information.packet_type_flags & kRtcpLossNotification))
		{
			rtcp::LossNotification* loss_notification = packet_information.loss_notification.get();
			RTC_DCHECK(loss_notification);
			if (loss_notification->media_ssrc() == local_ssrc)
			{
				rtcp_loss_notification_observer_->OnReceivedLossNotification(
					loss_notification->media_ssrc(), loss_notification->last_decoded(),
					loss_notification->last_received(), loss_notification->decodability_flag());
			}
		}*/
		/*if (rtcp_bandwidth_observer_)
		{
			RTC_DCHECK(!receiver_only_);
			if (packet_information.packet_type_flags & kRtcpRemb)
			{
				RTC_LOG(LS_VERBOSE)
					<< "Incoming REMB: "
					<< packet_information.receiver_estimated_max_bitrate_bps;
				rtcp_bandwidth_observer_->OnReceivedEstimatedBitrate(packet_information.receiver_estimated_max_bitrate_bps);
			}
			if ((packet_information.packet_type_flags & kRtcpSr) || (packet_information.packet_type_flags & kRtcpRr))
			{
				int64_t now_ms = clock_->TimeInMilliseconds();
				rtcp_bandwidth_observer_->OnReceivedRtcpReceiverReport(packet_information.report_blocks, packet_information.rtt_ms, now_ms);
			}
		}*/
		// TODO@chensong 2022-12-20 接受sr或者rr信息做ack确认 没有看懂啥意思？？？ 感觉啥好像都没有干是的
		if ((packet_information.packet_type_flags & kRtcpSr) || (packet_information.packet_type_flags & kRtcpRr))
		{
			//rtp_rtcp_->OnReceivedRtcpReportBlocks(packet_information.report_blocks);
		}

		//if (transport_feedback_observer_ && (packet_information.packet_type_flags & kRtcpTransportFeedback))
		//{
		//	uint32_t media_source_ssrc = packet_information.transport_feedback->media_ssrc();
		//	if (media_source_ssrc == local_ssrc || registered_ssrcs.find(media_source_ssrc) != registered_ssrcs.end())
		//	{
		//		// TODO@chensong 2022-12-05    接受端反馈过来的接受包seq和时间戳统计数据  
		//		// remb
		//		//RtpTransportControllerSend::OnTransportFeedback 这个代码带宽评估的非常重要一步是根据对端反馈网络带宽 带宽评估条件之一 
		//		transport_feedback_observer_->OnTransportFeedback(*packet_information.transport_feedback);
		//	}
		//}
		// TODO@chensong 2022-12-20  bitrate 对象没有 RtpRtcp::Configuration配置中默认是没有该bitrate_allocation_observer_对象的 所以一般下面的逻辑不走了
		/*if (bitrate_allocation_observer_ && packet_information.target_bitrate_allocation)
		{
			bitrate_allocation_observer_->OnBitrateAllocationUpdated(*packet_information.target_bitrate_allocation);
		}*/

		//// TODO@chensong 2022-12-20 数据统计模块 
		//if (!receiver_only_)
		//{
		//	rtc::CritScope cs(&feedbacks_lock_);
		//	if (stats_callback_)
		//	{
		//		for (const auto& report_block : packet_information.report_blocks)
		//		{
		//			RtcpStatistics stats;
		//			stats.packets_lost = report_block.packets_lost;
		//			stats.extended_highest_sequence_number = report_block.extended_highest_sequence_number;
		//			stats.fraction_lost = report_block.fraction_lost;
		//			stats.jitter = report_block.jitter;

		//			stats_callback_->StatisticsUpdated(stats, report_block.source_ssrc);
		//		}
		//	}
		//}
	}

	void crtcp_receiver::_handle_sender_report(const rtcp::CommonHeader& rtcp_block, cpacket_information* packet_information)
	{
		rtcp::csender_report sender_report;
		if (!sender_report.Parse(rtcp_block))
		{
			++m_num_skipped_packets;
			return;
		}

		const uint32_t remote_ssrc = sender_report.sender_ssrc();

		packet_information->remote_ssrc = remote_ssrc;

		//UpdateTmmbrRemoteIsAlive(remote_ssrc);

		// Have I received RTP packets from this party?
		//if (remote_ssrc_ == remote_ssrc)
		//{
		//	// Only signal that we have received a SR when we accept one.
		//	packet_information->packet_type_flags |= kRtcpSr;

		//	remote_sender_ntp_time_ = sender_report.ntp();
		//	remote_sender_rtp_time_ = sender_report.rtp_timestamp();
		//	last_received_sr_ntp_ = TimeMicrosToNtp(clock_->TimeInMicroseconds());
		//}
		//else
		{
			// We will only store the send report from one source, but
			// we will store all the receive blocks.
			packet_information->packet_type_flags |= kRtcpRr;
		}

		for (const rtcp::creport_block& report_block : sender_report.report_blocks())
		{
			_handle_report_block(report_block, packet_information, remote_ssrc);
		}
	}

	void crtcp_receiver::_handle_receiver_report(const rtcp::CommonHeader & rtcp_block, cpacket_information * packet_information)
	{
	}

	void crtcp_receiver::_handle_report_block(const rtcp::creport_block & report_block, cpacket_information * packet_information, uint32 remote_ssrc)
	{
		// This will be called once per report block in the RTCP packet.
		// We filter out all report blocks that are not for us.
		// Each packet has max 31 RR blocks.
		//
		// We can calc RTT if we send a send report and get a report block back.

		// |report_block.source_ssrc()| is the SSRC identifier of the source to
		// which the information in this reception report block pertains.

		// Filter out all report blocks that are not for us.
		// TODO@chensong 2022-12-26 
		//这将在RTCP数据包中的每个报告块调用一次。 
		//我们过滤掉所有不适合我们的报告块。 
		//每个数据包最多有31个RR块。 
		//如果我们发送发送报告并得到报告块，我们可以计算RTT。 
		//|report_block.source_ssrc（）|是源的ssrc标识符 
		//该接收报告块中的信息与之相关。 
		//过滤掉所有不适合我们的报告块。
		if (m_registered_ssrcs.count(report_block.source_ssrc()) == 0)
		{
			return;
		}

		m_last_received_rb_ms = uv_util::GetTimeMs();;
		// TODO@chensong 2022-12-26  没有该ssrc在received_report_blocks_中map中正好插入
		creport_block_with_rtt* report_block_info = &m_received_report_blocks[report_block.source_ssrc()][remote_ssrc];
		report_block_info->report_block.sender_ssrc = remote_ssrc;
		report_block_info->report_block.source_ssrc = report_block.source_ssrc();
		report_block_info->report_block.fraction_lost = report_block.fraction_lost();
		report_block_info->report_block.packets_lost = report_block.cumulative_lost_signed();
		if (report_block.extended_high_seq_num() > report_block_info->report_block.extended_highest_sequence_number)
		{
			// We have successfully delivered new RTP packets to the remote side after
			// the last RR was sent from the remote side.
			m_last_increased_sequence_number_ms = uv_util::GetTimeMs();
		}
		report_block_info->report_block.extended_highest_sequence_number = report_block.extended_high_seq_num();
		report_block_info->report_block.jitter = report_block.jitter();
		report_block_info->report_block.delay_since_last_sender_report = report_block.delay_since_last_sr();
		report_block_info->report_block.last_sender_report_timestamp = report_block.last_sr();

		int64_t rtt_ms = 0;
		uint32_t send_time_ntp = report_block.last_sr();
		// RFC3550, section 6.4.1, LSR field discription states:
		// If no SR has been received yet, the field is set to zero.
		// Receiver rtp_rtcp module is not expected to calculate rtt using
		// Sender Reports even if it accidentally can.

		// TODO(nisse): Use this way to determine the RTT only when |receiver_only_|
		// is false. However, that currently breaks the tests of the
		// googCaptureStartNtpTimeMs stat for audio receive streams. To fix, either
		// delete all dependencies on RTT measurements for audio receive streams, or
		// ensure that audio receive streams that need RTT and stats that depend on it
		// are configured with an associated audio send stream.
		if (send_time_ntp != 0)
		{
			uint32_t delay_ntp = report_block.delay_since_last_sr();
			// Local NTP time.
			// 微妙 
			uint32_t receive_time_ntp = CompactNtp(TimeMicrosToNtp(uv_util::GetTimeMs()));

			// RTT in 1/(2^16) seconds.
			uint32_t rtt_ntp = receive_time_ntp - delay_ntp /*发送时间与接收到时间差值*/ - send_time_ntp;
			// Convert to 1/1000 seconds (milliseconds).
			// 微妙转换 毫秒级
			rtt_ms = CompactNtpRttToMs(rtt_ntp);
			if (rtt_ms > report_block_info->max_rtt_ms)
			{
				report_block_info->max_rtt_ms = rtt_ms;
			}

			if (report_block_info->num_rtts == 0 || rtt_ms < report_block_info->min_rtt_ms)
			{
				report_block_info->min_rtt_ms = rtt_ms;
			}

			report_block_info->last_rtt_ms = rtt_ms;
			report_block_info->sum_rtt_ms += rtt_ms;
			++report_block_info->num_rtts;

			packet_information->rtt_ms = rtt_ms;
		}

		packet_information->report_blocks.push_back(report_block_info->report_block);
	}

}