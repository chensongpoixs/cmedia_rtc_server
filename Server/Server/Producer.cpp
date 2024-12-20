﻿/*
输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
*/
#include "Producer.hpp"
//#include "DepLibUV.hpp"
//#include "Logger.hpp"
//#include "MediaSoupErrors.hpp"
//#include "Utils.hpp"
//#include "Channel/ChannelNotifier.hpp"
#include "Tools.hpp"
#include "FeedbackPs.hpp"
#include "FeedbackRtp.hpp"
#include "XrReceiverReferenceTime.hpp"
#include <cstring>  // std::memcpy()
#include <iterator> // std::ostream_iterator
#include <sstream>  // std::ostringstream
#include "Producer.hpp"
#include "cuv_util.h"
#include "crtc_sdp.h"
//#define  MS_TRACE 
namespace RTC
{
	/* Instance methods. */
	//using namespace chen;
	Producer::Producer( RTC::Producer::Listener* listener,       chen::cmedia_desc   media_desc)
	  :  listener(listener)
		, m_media_desc(media_desc)
	{
	//	MS_TRACE();
		/*
		{
					"kind":"video",
					"paused":false,
					"rtpMapping":{
					"codecs":[
					{
						"mappedPayloadType":101,
						"payloadType":108
					},
					{
						"mappedPayloadType":102,
						"payloadType":109
					}
					],
					"encodings":[
					{
						"mappedSsrc":806764358,
						"rid":"r0",
						"scalabilityMode":"S1T3"
					},
					{
						"mappedSsrc":806764359,
						"rid":"r1",
						"scalabilityMode":"S1T3"
					},
					{
						"mappedSsrc":806764360,
						"rid":"r2",
						"scalabilityMode":"S1T3"
					}
					]
					},
					"rtpParameters":{
						"codecs":[
						{
						"clockRate":90000,
						"mimeType":"video/H264",
						"parameters":{
						"level-asymmetry-allowed":1,
						"packetization-mode":1,
						"profile-level-id":"42e01f"
					},
						"payloadType":108,
					"rtcpFeedback":[
					{
						"parameter":"",
						"type":"goog-remb"
					},
					{
						"parameter":"",
						"type":"transport-cc"
					},
					{
						"parameter":"fir",
						"type":"ccm"
					},
					{
						"parameter":"",
						"type":"nack"
					},
					{
						"parameter":"pli",
						"type":"nack"
					}
					]
					},
					{
						"clockRate":90000,
						"mimeType":"video/rtx",
						"parameters":{
						"apt":108
						},
						"payloadType":109,
						"rtcpFeedback":[

					]
					}
					],
					"encodings":[
					{
						"active":true,
						"dtx":false,
						"maxBitrate":500000,
						"rid":"r0",
						"scalabilityMode":"S1T3",
						"scaleResolutionDownBy":4
					},
					{
						"active":true,
						"dtx":false,
						"maxBitrate":1000000,
						"rid":"r1",
						"scalabilityMode":"S1T3",
						"scaleResolutionDownBy":2
					},
					{
						"active":true,
						"dtx":false,
						"maxBitrate":5000000,
						"rid":"r2",
						"scalabilityMode":"S1T3",
						"scaleResolutionDownBy":1
					}
					],
					"headerExtensions":[
					{
						"encrypt":false,
						"id":4,
						"parameters":{

						},
						"uri":"urn:ietf:params:rtp-hdrext:sdes:mid"
					},
					{
						"encrypt":false,
						"id":10,
						"parameters":{

						},
						"uri":"urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id"
					},
					{
						"encrypt":false,
						"id":11,
						"parameters":{

						},
						"uri":"urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id"
					},
					{
						"encrypt":false,
						"id":2,
						"parameters":{

						},
					"uri":"http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time"
					},
					{
						"encrypt":false,
						"id":3,
						"parameters":{

						},
					"uri":"http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01"
					},
					{
						"encrypt":false,
						"id":13,
						"parameters":{

						},
						"uri":"urn:3gpp:video-orientation"
					},
					{
						"encrypt":false,
						"id":14,
						"parameters":{

						},
					"uri":"urn:ietf:params:rtp-hdrext:toffset"
					}
						],
						"mid":"2",
						"rtcp":{
						"cname":"UgYi3789TL6C/8Zx",
						"reducedSize":true
						}
					}
				}
		
		*/
		/*auto jsonKindIt = data.find("kind");

		if (jsonKindIt == data.end() || !jsonKindIt->is_string())
		{
			MS_THROW_TYPE_ERROR("missing kind");
		}*/

		// This may throw.
		this->kind = RTC::Media::GetKind(media_desc.m_type.c_str());

		if (this->kind == RTC::Media::Kind::ALL)
		{
			ERROR_EX_LOG("invalid empty kind");
		}

		 

		// This may throw.
		//this->rtpParameters = RTC::RtpParameters(*jsonRtpParametersIt);

		// Evaluate type.
		this->type = RtpParameters::Type::SIMPLE; // RTC::RtpParameters::GetType(this->rtpParameters);

		// Reserve a slot in rtpStreamByEncodingIdx and rtpStreamsScores vectors
		// for each RTP stream.
		/*this->rtpStreamByEncodingIdx.resize(this->rtpParameters.encodings.size(), nullptr);
		this->rtpStreamScores.resize(this->rtpParameters.encodings.size(), 0u);*/

		//auto& encoding   = this->rtpParameters.encodings[0];
		//auto* mediaCodec = this->rtpParameters.GetCodecForEncoding(encoding);

		///*if (!RTC::Codecs::Tools::IsValidTypeForCodec(this->type, mediaCodec->mimeType))
		//{
		//	MS_THROW_TYPE_ERROR(
		//	  "%s codec not supported for %s",
		//	  mediaCodec->mimeType.ToString().c_str(),
		//	  RTC::RtpParameters::GetTypeString(this->type).c_str());
		//}*/

		////auto jsonRtpMappingIt = data.find("rtpMapping");

		////if (jsonRtpMappingIt == data.end() || !jsonRtpMappingIt->is_object())
		////{
		////	MS_THROW_TYPE_ERROR("missing rtpMapping");
		////}

		///*auto jsonCodecsIt = jsonRtpMappingIt->find("codecs");

		//if (jsonCodecsIt == jsonRtpMappingIt->end() || !jsonCodecsIt->is_array())
		//{
		//	MS_THROW_TYPE_ERROR("missing rtpMapping.codecs");
		//}*/

		//for (auto& codec : *jsonCodecsIt)
		//{
		//	if (!codec.is_object())
		//	{
		//		MS_THROW_TYPE_ERROR("wrong entry in rtpMapping.codecs (not an object)");
		//	}

		//	auto jsonPayloadTypeIt = codec.find("payloadType");

		//	// clang-format off
		//	if (
		//		jsonPayloadTypeIt == codec.end() ||
		//		!Utils::Json::IsPositiveInteger(*jsonPayloadTypeIt)
		//	)
		//	// clang-format on
		//	{
		//		MS_THROW_TYPE_ERROR("wrong entry in rtpMapping.codecs (missing payloadType)");
		//	}

		//	auto jsonMappedPayloadTypeIt = codec.find("mappedPayloadType");

		//	// clang-format off
		//	if (
		//		jsonMappedPayloadTypeIt == codec.end() ||
		//		!Utils::Json::IsPositiveInteger(*jsonMappedPayloadTypeIt)
		//	)
		//	// clang-format on
		//	{
		//		MS_THROW_TYPE_ERROR("wrong entry in rtpMapping.codecs (missing mappedPayloadType)");
		//	}

		//	this->rtpMapping.codecs[jsonPayloadTypeIt->get<uint8_t>()] =
		//	  jsonMappedPayloadTypeIt->get<uint8_t>();
		//}

		//auto jsonEncodingsIt = jsonRtpMappingIt->find("encodings");

		//if (jsonEncodingsIt == jsonRtpMappingIt->end() || !jsonEncodingsIt->is_array())
		//{
		//	MS_THROW_TYPE_ERROR("missing rtpMapping.encodings");
		//}

		//this->rtpMapping.encodings.reserve(jsonEncodingsIt->size());

		//for (auto& encoding : *jsonEncodingsIt)
		//{
		//	if (!encoding.is_object())
		//	{
		//		MS_THROW_TYPE_ERROR("wrong entry in rtpMapping.encodings");
		//	}

		//	this->rtpMapping.encodings.emplace_back();

		//	auto& encodingMapping = this->rtpMapping.encodings.back();

		//	// ssrc is optional.
		//	auto jsonSsrcIt = encoding.find("ssrc");

		//	// clang-format off
		//	if (
		//		jsonSsrcIt != encoding.end() &&
		//		Utils::Json::IsPositiveInteger(*jsonSsrcIt)
		//	)
		//	// clang-format on
		//	{
		//		encodingMapping.ssrc = jsonSsrcIt->get<uint32_t>();
		//	}

		//	// rid is optional.
		//	auto jsonRidIt = encoding.find("rid");

		//	if (jsonRidIt != encoding.end() && jsonRidIt->is_string())
		//	{
		//		encodingMapping.rid = jsonRidIt->get<std::string>();
		//	}

		//	// However ssrc or rid must be present (if more than 1 encoding).
		//	// clang-format off
		//	if (
		//		jsonEncodingsIt->size() > 1 &&
		//		jsonSsrcIt == encoding.end() &&
		//		jsonRidIt == encoding.end()
		//	)
		//	// clang-format on
		//	{
		//		MS_THROW_TYPE_ERROR("wrong entry in rtpMapping.encodings (missing ssrc or rid)");
		//	}

		//	// If there is no mid and a single encoding, ssrc or rid must be present.
		//	// clang-format off
		//	if (
		//		this->rtpParameters.mid.empty() &&
		//		jsonEncodingsIt->size() == 1 &&
		//		jsonSsrcIt == encoding.end() &&
		//		jsonRidIt == encoding.end()
		//	)
		//	// clang-format on
		//	{
		//		MS_THROW_TYPE_ERROR(
		//		  "wrong entry in rtpMapping.encodings (missing ssrc or rid, or rtpParameters.mid)");
		//	}

		//	// mappedSsrc is mandatory.
		//	auto jsonMappedSsrcIt = encoding.find("mappedSsrc");

		//	// clang-format off
		//	if (
		//		jsonMappedSsrcIt == encoding.end() ||
		//		!Utils::Json::IsPositiveInteger(*jsonMappedSsrcIt)
		//	)
		//	// clang-format on
		//	{
		//		MS_THROW_TYPE_ERROR("wrong entry in rtpMapping.encodings (missing mappedSsrc)");
		//	}

		//	encodingMapping.mappedSsrc = jsonMappedSsrcIt->get<uint32_t>();
		//}

		/*auto jsonPausedIt = data.find("paused");

		if (jsonPausedIt != data.end() && jsonPausedIt->is_boolean())
		{
			this->paused = jsonPausedIt->get<bool>();
		}*/
		this->paused = true;

		// The number of encodings in rtpParameters must match the number of encodings
		// in rtpMapping.
		/*if (this->rtpParameters.encodings.size() != this->rtpMapping.encodings.size())
		{
			MS_THROW_TYPE_ERROR("rtpParameters.encodings size does not match rtpMapping.encodings size");
		}*/

		// Fill RTP header extension ids.
		// This may throw.
		//for (auto& exten : this->rtpParameters.headerExtensions)
		//{
		//	/*if (exten.id == 0u)
		//	{
		//		MS_THROW_TYPE_ERROR("RTP extension id cannot be 0");
		//	}*/

		//	if (this->rtpHeaderExtensionIds.mid == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::MID)
		//	{
		//		this->rtpHeaderExtensionIds.mid = exten.id;
		//	}

		//	if (this->rtpHeaderExtensionIds.rid == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::RTP_STREAM_ID)
		//	{
		//		this->rtpHeaderExtensionIds.rid = exten.id;
		//	}

		//	if (this->rtpHeaderExtensionIds.rrid == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::REPAIRED_RTP_STREAM_ID)
		//	{
		//		this->rtpHeaderExtensionIds.rrid = exten.id;
		//	}

		//	if (this->rtpHeaderExtensionIds.absSendTime == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::ABS_SEND_TIME)
		//	{
		//		this->rtpHeaderExtensionIds.absSendTime = exten.id;
		//	}

		//	if (this->rtpHeaderExtensionIds.transportWideCc01 == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::TRANSPORT_WIDE_CC_01)
		//	{
		//		this->rtpHeaderExtensionIds.transportWideCc01 = exten.id;
		//	}

		//	// NOTE: Remove this once framemarking draft becomes RFC.
		//	if (this->rtpHeaderExtensionIds.frameMarking07 == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::FRAME_MARKING_07)
		//	{
		//		this->rtpHeaderExtensionIds.frameMarking07 = exten.id;
		//	}

		//	if (this->rtpHeaderExtensionIds.frameMarking == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::FRAME_MARKING)
		//	{
		//		this->rtpHeaderExtensionIds.frameMarking = exten.id;
		//	}

		//	if (this->rtpHeaderExtensionIds.ssrcAudioLevel == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::SSRC_AUDIO_LEVEL)
		//	{
		//		this->rtpHeaderExtensionIds.ssrcAudioLevel = exten.id;
		//	}

		//	if (this->rtpHeaderExtensionIds.videoOrientation == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::VIDEO_ORIENTATION)
		//	{
		//		this->rtpHeaderExtensionIds.videoOrientation = exten.id;
		//	}

		//	if (this->rtpHeaderExtensionIds.toffset == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::TOFFSET)
		//	{
		//		this->rtpHeaderExtensionIds.toffset = exten.id;
		//	}
		//}

		// Set the RTCP report generation interval.
		if (this->kind == RTC::Media::Kind::AUDIO)
		{
			this->maxRtcpInterval = RTC::RTCP::MaxAudioIntervalMs;
		}
		else
		{
			this->maxRtcpInterval = RTC::RTCP::MaxVideoIntervalMs;
		}

		// Create a KeyFrameRequestManager.
		if (this->kind == RTC::Media::Kind::VIDEO)
		{
			//auto jsonKeyFrameRequestDelayIt = data.find("keyFrameRequestDelay");
			uint32_t keyFrameRequestDelay   = 0u;

			// clang-format off
			//if (
			//	jsonKeyFrameRequestDelayIt != data.end() &&
			//	jsonKeyFrameRequestDelayIt->is_number_integer()
			//)
			//// clang-format on
			//{
			//	keyFrameRequestDelay = jsonKeyFrameRequestDelayIt->get<uint32_t>();
			//}

			this->keyFrameRequestManager = new RTC::KeyFrameRequestManager(this, keyFrameRequestDelay);
		}
	}

	Producer::~Producer()
	{
		//MS_TRACE();

		// Delete all streams.
		for (auto& kv : this->mapSsrcRtpStream)
		{
			auto* rtpStream = kv.second;

			delete rtpStream;
		}

		this->mapSsrcRtpStream.clear();
		this->rtpStreamByEncodingIdx.clear();
		this->rtpStreamScores.clear();
		this->mapRtxSsrcRtpStream.clear();
		this->mapRtpStreamMappedSsrc.clear();
		this->mapMappedSsrcSsrc.clear();

		// Delete the KeyFrameRequestManager.
		delete this->keyFrameRequestManager;
	}

	
	
	
	Producer::ReceiveRtpPacketResult Producer::ReceiveRtpPacket(RTC::RtpPacket* packet)
	{
		//MS_TRACE();

		// Reset current packet.
		this->currentRtpPacket = nullptr;

		// Count number of RTP streams.
		auto numRtpStreamsBefore = this->mapSsrcRtpStream.size();

		auto* rtpStream = GetRtpStream(packet);

		if (!rtpStream)
		{
			WARNING_EX_LOG("rtp, no stream found for received packet [ssrc:%" PRIu32 "]", packet->GetSsrc());

			return ReceiveRtpPacketResult::DISCARDED;
		}

		// Pre-process the packet.
		PreProcessRtpPacket(packet);

		ReceiveRtpPacketResult result;
		bool isRtx{ false };

		// Media packet.
		if (packet->GetSsrc() == rtpStream->GetSsrc())
		{
			result = ReceiveRtpPacketResult::MEDIA;

			// Process the packet.
			if (!rtpStream->ReceivePacket(packet))
			{
				// May have to announce a new RTP stream to the listener.
				if (this->mapSsrcRtpStream.size() > numRtpStreamsBefore)
					NotifyNewRtpStream(rtpStream);

				return result;
			}
		}
		// RTX packet.
		else if (packet->GetSsrc() == rtpStream->GetRtxSsrc())
		{
			result = ReceiveRtpPacketResult::RETRANSMISSION;
			isRtx  = true;

			// Process the packet.
			if (!rtpStream->ReceiveRtxPacket(packet))
				return result;
		}
		// Should not happen.
		else
		{
			cassert("found stream does not match received packet");
		}

		if (packet->IsKeyFrame())
		{
			DEBUG_EX_LOG(" rtp, key frame received [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
			  packet->GetSsrc(),
			  packet->GetSequenceNumber());

			// Tell the keyFrameRequestManager.
			if (this->keyFrameRequestManager)
				this->keyFrameRequestManager->KeyFrameReceived(packet->GetSsrc());
		}

		// May have to announce a new RTP stream to the listener.
		if (this->mapSsrcRtpStream.size() > numRtpStreamsBefore)
		{
			// Request a key frame for this stream since we may have lost the first packets
			// (do not do it if this is a key frame).
			if (this->keyFrameRequestManager && !this->paused && !packet->IsKeyFrame())
				this->keyFrameRequestManager->ForceKeyFrameNeeded(packet->GetSsrc());

			// Update current packet.
			this->currentRtpPacket = packet;

			NotifyNewRtpStream(rtpStream);

			// Reset current packet.
			this->currentRtpPacket = nullptr;
		}

		// If paused stop here.
		if (this->paused)
			return result;

		// May emit 'trace' event.
		EmitTraceEventRtpAndKeyFrameTypes(packet, isRtx);

		// Mangle the packet before providing the listener with it.
		if (!MangleRtpPacket(packet, rtpStream))
			return ReceiveRtpPacketResult::DISCARDED;

		// Post-process the packet.
		PostProcessRtpPacket(packet);

		this->listener->OnProducerRtpPacketReceived(this, packet);

		return result;
	}

	void Producer::ReceiveRtcpSenderReport(RTC::RTCP::SenderReport* report)
	{
		//MS_TRACE();

		auto it = this->mapSsrcRtpStream.find(report->GetSsrc());

		if (it != this->mapSsrcRtpStream.end())
		{
			auto* rtpStream = it->second;
			bool first      = rtpStream->GetSenderReportNtpMs() == 0;

			rtpStream->ReceiveRtcpSenderReport(report);

			this->listener->OnProducerRtcpSenderReport(this, rtpStream, first);

			return;
		}

		// If not found, check with RTX.
		auto it2 = this->mapRtxSsrcRtpStream.find(report->GetSsrc());

		if (it2 != this->mapRtxSsrcRtpStream.end())
		{
			auto* rtpStream = it2->second;

			rtpStream->ReceiveRtxRtcpSenderReport(report);

			return;
		}

		DEBUG_EX_LOG("rtcp, RtpStream not found [ssrc:%" PRIu32 "]", report->GetSsrc());
	}

	void Producer::ReceiveRtcpXrDelaySinceLastRr(RTC::RTCP::DelaySinceLastRr::SsrcInfo* ssrcInfo)
	{
		//MS_TRACE();

		auto it = this->mapSsrcRtpStream.find(ssrcInfo->GetSsrc());

		if (it == this->mapSsrcRtpStream.end())
		{
			WARNING_EX_LOG("rtcp, RtpStream not found [ssrc:%" PRIu32 "]", ssrcInfo->GetSsrc());

			return;
		}

		auto* rtpStream = it->second;

		rtpStream->ReceiveRtcpXrDelaySinceLastRr(ssrcInfo);
	}

	void Producer::GetRtcp(RTC::RTCP::CompoundPacket* packet, uint64_t nowMs)
	{
		//MS_TRACE();

		if (static_cast<float>((nowMs - this->lastRtcpSentTime) * 1.15) < this->maxRtcpInterval)
			return;

		for (auto& kv : this->mapSsrcRtpStream)
		{
			auto* rtpStream = kv.second;
			auto* report    = rtpStream->GetRtcpReceiverReport();

			packet->AddReceiverReport(report);

			auto* rtxReport = rtpStream->GetRtxRtcpReceiverReport();

			if (rtxReport)
				packet->AddReceiverReport(rtxReport);
		}

		// Add a receiver reference time report if no present in the packet.
		if (!packet->HasReceiverReferenceTime())
		{
			auto ntp     =  rtc_time::TimeMs2Ntp(nowMs);
			auto* report = new RTC::RTCP::ReceiverReferenceTime();

			report->SetNtpSec(ntp.seconds);
			report->SetNtpFrac(ntp.fractions);
			packet->AddReceiverReferenceTime(report);
		}

		this->lastRtcpSentTime = nowMs;
	}

	void Producer::RequestKeyFrame(uint32_t mappedSsrc)
	{
		//MS_TRACE();

		if (!this->keyFrameRequestManager || this->paused)
			return;

		auto it = this->mapMappedSsrcSsrc.find(mappedSsrc);

		if (it == this->mapMappedSsrcSsrc.end())
		{
			WARNING_EX_LOG("rtcp, rtx, given mappedSsrc not found, ignoring");

			return;
		}

		uint32_t ssrc = it->second;

		// If the current RTP packet is a key frame for the given mapped SSRC do
		// nothing since we are gonna provide Consumers with the requested key frame
		// right now.
		//
		// NOTE: We know that this may only happen before calling MangleRtpPacket()
		// so the SSRC of the packet is still the original one and not the mapped one.
		//
		// clang-format off
		if (
			this->currentRtpPacket &&
			this->currentRtpPacket->GetSsrc() == ssrc &&
			this->currentRtpPacket->IsKeyFrame()
		)
		// clang-format on
		{
			return;
		}

		this->keyFrameRequestManager->KeyFrameNeeded(ssrc);
	}

	RTC::RtpStreamRecv* Producer::GetRtpStream(RTC::RtpPacket* packet)
	{
		//MS_TRACE();

		uint32_t ssrc       = packet->GetSsrc();
		uint8_t payloadType = packet->GetPayloadType();

		// If stream found in media ssrcs map, return it.
		{
			auto it = this->mapSsrcRtpStream.find(ssrc);

			if (it != this->mapSsrcRtpStream.end())
			{
				auto* rtpStream = it->second;

				return rtpStream;
			}
		}

		// If stream found in RTX ssrcs map, return it.
		{
			auto it = this->mapRtxSsrcRtpStream.find(ssrc);

			if (it != this->mapRtxSsrcRtpStream.end())
			{
				auto* rtpStream = it->second;

				return rtpStream;
			}
		}

		// Otherwise check our encodings and, if appropriate, create a new stream.

		for (const cssrc_group & ssrc_group : m_media_desc.m_ssrc_groups)
		{
			if (ssrc_group.m_semantic == "FID")
			{

			}
		}

		// First, look for an encoding with matching media or RTX ssrc value.
		for (size_t i{ 0 }; i < this->rtpParameters.encodings.size(); ++i)
		{
			auto& encoding         = this->rtpParameters.encodings[i];
			const auto* mediaCodec = this->rtpParameters.GetCodecForEncoding(encoding);
			const auto* rtxCodec   = this->rtpParameters.GetRtxCodecForEncoding(encoding);
			bool isMediaPacket     = (mediaCodec->payloadType == payloadType);
			bool isRtxPacket       = (rtxCodec && rtxCodec->payloadType == payloadType);

			if (isMediaPacket && encoding.ssrc == ssrc)
			{
				auto* rtpStream = CreateRtpStream(packet, *mediaCodec, i);

				return rtpStream;
			}
			else if (isRtxPacket && encoding.hasRtx && encoding.rtx.ssrc == ssrc)
			{
				auto it = this->mapSsrcRtpStream.find(encoding.ssrc);

				// Ignore if no stream has been created yet for the corresponding encoding.
				if (it == this->mapSsrcRtpStream.end())
				{
					DEBUG_EX_LOG("rtp, rtx, ignoring RTX packet for not yet created RtpStream (ssrc lookup)");

					return nullptr;
				}

				auto* rtpStream = it->second;

				// Ensure no RTX ssrc was previously detected.
				if (rtpStream->HasRtx())
				{
					DEBUG_EX_LOG("rtp, rtx, ignoring RTX packet with new ssrc (ssrc lookup)");

					return nullptr;
				}

				// Update the stream RTX data.
				rtpStream->SetRtx(payloadType, ssrc);

				// Insert the new RTX ssrc into the map.
				this->mapRtxSsrcRtpStream[ssrc] = rtpStream;

				return rtpStream;
			}
		}

		// If not found, look for an encoding matching the packet RID value.
		std::string rid;

		if (packet->ReadRid(rid))
		{
			for (size_t i{ 0 }; i < this->rtpParameters.encodings.size(); ++i)
			{
				auto& encoding = this->rtpParameters.encodings[i];

				if (encoding.rid != rid)
					continue;

				const auto* mediaCodec = this->rtpParameters.GetCodecForEncoding(encoding);
				const auto* rtxCodec   = this->rtpParameters.GetRtxCodecForEncoding(encoding);
				bool isMediaPacket     = (mediaCodec->payloadType == payloadType);
				bool isRtxPacket       = (rtxCodec && rtxCodec->payloadType == payloadType);

				if (isMediaPacket)
				{
					// Ensure no other stream already exists with same RID.
					for (auto& kv : this->mapSsrcRtpStream)
					{
						auto* rtpStream = kv.second;

						if (rtpStream->GetRid() == rid)
						{
							WARNING_EX_LOG("rtp,  ignoring packet with unknown ssrc but already handled RID (RID lookup)");

							return nullptr;
						}
					}

					auto* rtpStream = CreateRtpStream(packet, *mediaCodec, i);

					return rtpStream;
				}
				else if (isRtxPacket)
				{
					// Ensure a stream already exists with same RID.
					for (auto& kv : this->mapSsrcRtpStream)
					{
						auto* rtpStream = kv.second;

						if (rtpStream->GetRid() == rid)
						{
							// Ensure no RTX ssrc was previously detected.
							if (rtpStream->HasRtx())
							{
								DEBUG_EX_LOG("rtp, rtx, ignoring RTX packet with new SSRC (RID lookup)");

								return nullptr;
							}

							// Update the stream RTX data.
							rtpStream->SetRtx(payloadType, ssrc);

							// Insert the new RTX ssrc into the map.
							this->mapRtxSsrcRtpStream[ssrc] = rtpStream;

							return rtpStream;
						}
					}

					DEBUG_EX_LOG("rtp, rtx, ignoring RTX packet for not yet created RtpStream (RID lookup)");

					return nullptr;
				}
			}

			WARNING_EX_LOG("rtp, ignoring packet with unknown RID (RID lookup)");

			return nullptr;
		}

		// If not found, and there is a single encoding without ssrc and RID, this
		// may be the media or RTX stream.
		//
		// clang-format off
		if (
			this->rtpParameters.encodings.size() == 1 &&
			!this->rtpParameters.encodings[0].ssrc &&
			this->rtpParameters.encodings[0].rid.empty()
		)
		// clang-format on
		{
			auto& encoding         = this->rtpParameters.encodings[0];
			const auto* mediaCodec = this->rtpParameters.GetCodecForEncoding(encoding);
			const auto* rtxCodec   = this->rtpParameters.GetRtxCodecForEncoding(encoding);
			bool isMediaPacket     = (mediaCodec->payloadType == payloadType);
			bool isRtxPacket       = (rtxCodec && rtxCodec->payloadType == payloadType);

			if (isMediaPacket)
			{
				// Ensure there is no other RTP stream already.
				if (!this->mapSsrcRtpStream.empty())
				{
					WARNING_EX_LOG(" rtp, ignoring packet with unknown ssrc not matching the already existing stream (single RtpStream lookup)");

					return nullptr;
				}

				auto* rtpStream = CreateRtpStream(packet, *mediaCodec, 0);

				return rtpStream;
			}
			else if (isRtxPacket)
			{
				// There must be already a media RTP stream.
				auto it = this->mapSsrcRtpStream.begin();

				if (it == this->mapSsrcRtpStream.end())
				{
					DEBUG_EX_LOG(" rtp, rtx,  ignoring RTX packet for not yet created RtpStream (single stream lookup)");

					return nullptr;
				}

				auto* rtpStream = it->second;

				// Ensure no RTX SSRC was previously detected.
				if (rtpStream->HasRtx())
				{
					DEBUG_EX_LOG("rtp, rtx, ignoring RTX packet with new SSRC (single stream lookup)");

					return nullptr;
				}

				// Update the stream RTX data.
				rtpStream->SetRtx(payloadType, ssrc);

				// Insert the new RTX SSRC into the map.
				this->mapRtxSsrcRtpStream[ssrc] = rtpStream;

				return rtpStream;
			}
		}

		return nullptr;
	}

	RTC::RtpStreamRecv* Producer::CreateRtpStream(
	  RTC::RtpPacket* packet, const RTC::RtpCodecParameters& mediaCodec, size_t encodingIdx)
	{
		//MS_TRACE();

		uint32_t ssrc = packet->GetSsrc();

		cassert_desc(
		  this->mapSsrcRtpStream.find(ssrc) == this->mapSsrcRtpStream.end(),
		  "RtpStream with given SSRC already exists");
		cassert_desc(
		  !this->rtpStreamByEncodingIdx[encodingIdx],
		  "RtpStream for given encoding index already exists");

		auto& encoding        = this->rtpParameters.encodings[encodingIdx];
		auto& encodingMapping = this->rtpMapping.encodings[encodingIdx];

		DEBUG_EX_LOG(" rtp, [encodingIdx:%zu, ssrc:%" PRIu32 ", rid:%s, payloadType:%" PRIu8 "]",
		  encodingIdx,
		  ssrc,
		  encoding.rid.c_str(),
		  mediaCodec.payloadType);

		// Set stream params.
		RTC::RtpStream::Params params;

		params.encodingIdx    = encodingIdx;
		params.ssrc           = ssrc;
		params.payloadType    = mediaCodec.payloadType;
		params.mimeType       = mediaCodec.mimeType;
		params.clockRate      = mediaCodec.clockRate;
		params.rid            = encoding.rid;
		params.cname          = this->rtpParameters.rtcp.cname;
		params.spatialLayers  = encoding.spatialLayers;
		params.temporalLayers = encoding.temporalLayers;

		// Check in band FEC in codec parameters.
		if (mediaCodec.parameters.HasInteger("useinbandfec") && mediaCodec.parameters.GetInteger("useinbandfec") == 1)
		{
			DEBUG_EX_LOG("rtcp, in band FEC enabled");

			params.useInBandFec = true;
		}

		// Check DTX in codec parameters.
		if (mediaCodec.parameters.HasInteger("usedtx") && mediaCodec.parameters.GetInteger("usedtx") == 1)
		{
			DEBUG_EX_LOG("rtp, DTX enabled");

			params.useDtx = true;
		}

		// Check DTX in the encoding.
		if (encoding.dtx)
		{
			DEBUG_EX_LOG("rtp, DTX enabled");

			params.useDtx = true;
		}

		for (const auto& fb : mediaCodec.rtcpFeedbacks)
		{
			if (!params.useNack && fb.type == "nack" && fb.parameter.empty())
			{
				DEBUG_EX_LOG("rtp, rtcp, NACK supported");

				params.useNack = true;
			}
			else if (!params.usePli && fb.type == "nack" && fb.parameter == "pli")
			{
				DEBUG_EX_LOG("rtp, rtcp, PLI supported");

				params.usePli = true;
			}
			else if (!params.useFir && fb.type == "ccm" && fb.parameter == "fir")
			{
				DEBUG_EX_LOG("rtp, rtcp, FIR supported");

				params.useFir = true;
			}
		}

		// Create a RtpStreamRecv for receiving a media stream.
		auto* rtpStream = new RTC::RtpStreamRecv(this, params);

		// Insert into the maps.
		this->mapSsrcRtpStream[ssrc]              = rtpStream;
		this->rtpStreamByEncodingIdx[encodingIdx] = rtpStream;
		this->rtpStreamScores[encodingIdx]        = rtpStream->GetScore();

		// Set the mapped SSRC.
		this->mapRtpStreamMappedSsrc[rtpStream]             = encodingMapping.mappedSsrc;
		this->mapMappedSsrcSsrc[encodingMapping.mappedSsrc] = ssrc;

		// If the Producer is paused tell it to the new RtpStreamRecv.
		if (this->paused)
			rtpStream->Pause();

		// Emit the first score event right now.
		EmitScore();

		return rtpStream;
	}

	void Producer::NotifyNewRtpStream(RTC::RtpStreamRecv* rtpStream)
	{
		//MS_TRACE();

		auto mappedSsrc = this->mapRtpStreamMappedSsrc.at(rtpStream);

		// Notify the listener.
		this->listener->OnProducerNewRtpStream(this, static_cast<RTC::RtpStream*>(rtpStream), mappedSsrc);
	}

	inline void Producer::PreProcessRtpPacket(RTC::RtpPacket* packet)
	{
		//MS_TRACE();

		if (this->kind == RTC::Media::Kind::VIDEO)
		{
			// NOTE: Remove this once framemarking draft becomes RFC.
			packet->SetFrameMarking07ExtensionId(this->rtpHeaderExtensionIds.frameMarking07);
			packet->SetFrameMarkingExtensionId(this->rtpHeaderExtensionIds.frameMarking);
		}
	}

	inline bool Producer::MangleRtpPacket(RTC::RtpPacket* packet, RTC::RtpStreamRecv* rtpStream) const
	{
		//MS_TRACE();

		// Mangle the payload type.
		{
			uint8_t payloadType = packet->GetPayloadType();
			auto it             = this->rtpMapping.codecs.find(payloadType);

			if (it == this->rtpMapping.codecs.end())
			{
				WARNING_EX_LOG("rtp, unknown payload type [payloadType:%" PRIu8 "]", payloadType);

				return false;
			}

			uint8_t mappedPayloadType = it->second;

			packet->SetPayloadType(mappedPayloadType);
		}

		// Mangle the SSRC.
		{
			uint32_t mappedSsrc = this->mapRtpStreamMappedSsrc.at(rtpStream);

			packet->SetSsrc(mappedSsrc);
		}

		// Mangle RTP header extensions.
		{
			thread_local static uint8_t buffer[4096];
			thread_local static std::vector<RTC::RtpPacket::GenericExtension> extensions;

			// This happens just once.
			if (extensions.capacity() != 24)
				extensions.reserve(24);

			extensions.clear();

			uint8_t* extenValue;
			uint8_t extenLen;
			uint8_t* bufferPtr{ buffer };

			// Add urn:ietf:params:rtp-hdrext:sdes:mid.
			{
				extenLen = RTC::MidMaxLength;

				extensions.emplace_back(
				  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::MID), extenLen, bufferPtr);

				bufferPtr += extenLen;
			}

			if (this->kind == RTC::Media::Kind::AUDIO)
			{
				// Proxy urn:ietf:params:rtp-hdrext:ssrc-audio-level.
				extenValue = packet->GetExtension(this->rtpHeaderExtensionIds.ssrcAudioLevel, extenLen);

				if (extenValue)
				{
					std::memcpy(bufferPtr, extenValue, extenLen);

					extensions.emplace_back(
					  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::SSRC_AUDIO_LEVEL),
					  extenLen,
					  bufferPtr);

					// Not needed since this is the latest added extension.
					// bufferPtr += extenLen;
				}
			}
			else if (this->kind == RTC::Media::Kind::VIDEO)
			{
				// Add http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time.
				{
					extenLen = 3u;

					// NOTE: Add value 0. The sending Transport will update it.
					uint32_t absSendTime{ 0u };

					rtc_byte::set3bytes(bufferPtr, 0, absSendTime);

					extensions.emplace_back(
					  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::ABS_SEND_TIME), extenLen, bufferPtr);

					bufferPtr += extenLen;
				}

				// Add http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01.
				{
					extenLen = 2u;

					// NOTE: Add value 0. The sending Transport will update it.
					uint16_t wideSeqNumber{ 0u };

					rtc_byte::set2bytes(bufferPtr, 0, wideSeqNumber);

					extensions.emplace_back(
					  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::TRANSPORT_WIDE_CC_01),
					  extenLen,
					  bufferPtr);

					bufferPtr += extenLen;
				}

				// NOTE: Remove this once framemarking draft becomes RFC.
				// Proxy http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07.
				extenValue = packet->GetExtension(this->rtpHeaderExtensionIds.frameMarking07, extenLen);

				if (extenValue)
				{
					std::memcpy(bufferPtr, extenValue, extenLen);

					extensions.emplace_back(
					  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::FRAME_MARKING_07),
					  extenLen,
					  bufferPtr);

					bufferPtr += extenLen;
				}

				// Proxy urn:ietf:params:rtp-hdrext:framemarking.
				extenValue = packet->GetExtension(this->rtpHeaderExtensionIds.frameMarking, extenLen);

				if (extenValue)
				{
					std::memcpy(bufferPtr, extenValue, extenLen);

					extensions.emplace_back(
					  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::FRAME_MARKING), extenLen, bufferPtr);

					bufferPtr += extenLen;
				}

				// Proxy urn:3gpp:video-orientation.
				extenValue = packet->GetExtension(this->rtpHeaderExtensionIds.videoOrientation, extenLen);

				if (extenValue)
				{
					std::memcpy(bufferPtr, extenValue, extenLen);

					extensions.emplace_back(
					  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::VIDEO_ORIENTATION),
					  extenLen,
					  bufferPtr);

					bufferPtr += extenLen;
				}

				// Proxy urn:ietf:params:rtp-hdrext:toffset.
				extenValue = packet->GetExtension(this->rtpHeaderExtensionIds.toffset, extenLen);

				if (extenValue)
				{
					std::memcpy(bufferPtr, extenValue, extenLen);

					extensions.emplace_back(
					  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::TOFFSET), extenLen, bufferPtr);

					// Not needed since this is the latest added extension.
					// bufferPtr += extenLen;
				}
			}

			// Set the new extensions into the packet using One-Byte format.
			packet->SetExtensions(1, extensions);

			// Assign mediasoup RTP header extension ids (just those that mediasoup may
			// be interested in after passing it to the Router).
			packet->SetMidExtensionId(static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::MID));
			packet->SetAbsSendTimeExtensionId(
			  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::ABS_SEND_TIME));
			packet->SetTransportWideCc01ExtensionId(
			  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::TRANSPORT_WIDE_CC_01));
			// NOTE: Remove this once framemarking draft becomes RFC.
			packet->SetFrameMarking07ExtensionId(
			  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::FRAME_MARKING_07));
			packet->SetFrameMarkingExtensionId(
			  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::FRAME_MARKING));
			packet->SetSsrcAudioLevelExtensionId(
			  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::SSRC_AUDIO_LEVEL));
			packet->SetVideoOrientationExtensionId(
			  static_cast<uint8_t>(RTC::RtpHeaderExtensionUri::Type::VIDEO_ORIENTATION));
		}

		return true;
	}

	inline void Producer::PostProcessRtpPacket(RTC::RtpPacket* packet)
	{
		//MS_TRACE();

		if (this->kind == RTC::Media::Kind::VIDEO)
		{
			bool camera;
			bool flip;
			uint16_t rotation;

			if (packet->ReadVideoOrientation(camera, flip, rotation))
			{
				// If video orientation was not yet detected or any value has changed,
				// emit event.
				// clang-format off
				if (
					!this->videoOrientationDetected ||
					camera != this->videoOrientation.camera ||
					flip != this->videoOrientation.flip ||
					rotation != this->videoOrientation.rotation
				)
				// clang-format on
				{
					this->videoOrientationDetected  = true;
					this->videoOrientation.camera   = camera;
					this->videoOrientation.flip     = flip;
					this->videoOrientation.rotation = rotation;

					/*json data = json::object();

					data["camera"]   = this->videoOrientation.camera;
					data["flip"]     = this->videoOrientation.flip;
					data["rotation"] = this->videoOrientation.rotation;

					Channel::ChannelNotifier::Emit(this->id, "videoorientationchange", data);*/
				}
			}
		}
	}

	inline void Producer::EmitScore() const
	{
		//MS_TRACE();

		/*json data = json::array();

		for (auto* rtpStream : this->rtpStreamByEncodingIdx)
		{
			if (!rtpStream)
				continue;

			data.emplace_back(json::value_t::object);

			auto& jsonEntry = data[data.size() - 1];

			jsonEntry["encodingIdx"] = rtpStream->GetEncodingIdx();
			jsonEntry["ssrc"]        = rtpStream->GetSsrc();

			if (!rtpStream->GetRid().empty())
				jsonEntry["rid"] = rtpStream->GetRid();

			jsonEntry["score"] = rtpStream->GetScore();
		}

		Channel::ChannelNotifier::Emit(this->id, "score", data);*/
	}

	inline void Producer::EmitTraceEventRtpAndKeyFrameTypes(RTC::RtpPacket* packet, bool isRtx) const
	{
		//MS_TRACE();

		if (this->traceEventTypes.keyframe && packet->IsKeyFrame())
		{
			/*json data = json::object();

			data["type"]      = "keyframe";
			data["timestamp"] = DepLibUV::GetTimeMs();
			data["direction"] = "in";

			packet->FillJson(data["info"]);

			if (isRtx)
				data["info"]["isRtx"] = true;

			Channel::ChannelNotifier::Emit(this->id, "trace", data);*/
		}
		else if (this->traceEventTypes.rtp)
		{
			/*json data = json::object();

			data["type"]      = "rtp";
			data["timestamp"] = DepLibUV::GetTimeMs();
			data["direction"] = "in";

			packet->FillJson(data["info"]);

			if (isRtx)
				data["info"]["isRtx"] = true;

			Channel::ChannelNotifier::Emit(this->id, "trace", data);*/
		}
	}

	inline void Producer::EmitTraceEventPliType(uint32_t ssrc) const
	{
		//MS_TRACE();

		if (!this->traceEventTypes.pli)
			return;

		/*json data = json::object();

		data["type"]         = "pli";
		data["timestamp"]    = DepLibUV::GetTimeMs();
		data["direction"]    = "out";
		data["info"]["ssrc"] = ssrc;

		Channel::ChannelNotifier::Emit(this->id, "trace", data);*/
	}

	inline void Producer::EmitTraceEventFirType(uint32_t ssrc) const
	{
		//MS_TRACE();

		if (!this->traceEventTypes.fir)
			return;

		/*json data = json::object();

		data["type"]         = "fir";
		data["timestamp"]    = DepLibUV::GetTimeMs();
		data["direction"]    = "out";
		data["info"]["ssrc"] = ssrc;

		Channel::ChannelNotifier::Emit(this->id, "trace", data);*/
	}

	inline void Producer::EmitTraceEventNackType() const
	{
		//MS_TRACE();

		if (!this->traceEventTypes.nack)
			return;

		/*json data = json::object();

		data["type"]      = "nack";
		data["timestamp"] = DepLibUV::GetTimeMs();
		data["direction"] = "out";
		data["info"]      = json::object();

		Channel::ChannelNotifier::Emit(this->id, "trace", data);*/
	}

	inline void Producer::OnRtpStreamScore(RTC::RtpStream* rtpStream, uint8_t score, uint8_t previousScore)
	{
		//MS_TRACE();

		// Update the vector of scores.
		this->rtpStreamScores[rtpStream->GetEncodingIdx()] = score;

		// Notify the listener.
		this->listener->OnProducerRtpStreamScore(this, rtpStream, score, previousScore);

		// Emit the score event.
		EmitScore();
	}

	inline void Producer::OnRtpStreamSendRtcpPacket(
	  RTC::RtpStreamRecv* /*rtpStream*/, RTC::RTCP::Packet* packet)
	{
		switch (packet->GetType())
		{
			case RTC::RTCP::Type::PSFB:
			{
				auto* feedback = static_cast<RTC::RTCP::FeedbackPsPacket*>(packet);

				switch (feedback->GetMessageType())
				{
					case RTC::RTCP::FeedbackPs::MessageType::PLI:
					{
						// May emit 'trace' event.
						EmitTraceEventPliType(feedback->GetMediaSsrc());

						break;
					}

					case RTC::RTCP::FeedbackPs::MessageType::FIR:
					{
						// May emit 'trace' event.
						EmitTraceEventFirType(feedback->GetMediaSsrc());

						break;
					}

					default:;
				}
			}

			case RTC::RTCP::Type::RTPFB:
			{
				auto* feedback = static_cast<RTC::RTCP::FeedbackRtpPacket*>(packet);

				switch (feedback->GetMessageType())
				{
					case RTC::RTCP::FeedbackRtp::MessageType::NACK:
					{
						// May emit 'trace' event.
						EmitTraceEventNackType();

						break;
					}

					default:;
				}
			}

			default:;
		}

		// Notify the listener.
		this->listener->OnProducerSendRtcpPacket(this, packet);
	}

	inline void Producer::OnRtpStreamNeedWorstRemoteFractionLost(
	  RTC::RtpStreamRecv* rtpStream, uint8_t& worstRemoteFractionLost)
	{
		auto mappedSsrc = this->mapRtpStreamMappedSsrc.at(rtpStream);

		// Notify the listener.
		this->listener->OnProducerNeedWorstRemoteFractionLost(this, mappedSsrc, worstRemoteFractionLost);
	}

	inline void Producer::OnKeyFrameNeeded(
	  RTC::KeyFrameRequestManager* /*keyFrameRequestManager*/, uint32_t ssrc)
	{
		//MS_TRACE();

		auto it = this->mapSsrcRtpStream.find(ssrc);

		if (it == this->mapSsrcRtpStream.end())
		{
			WARNING_EX_LOG("rtcp, rtx, no associated RtpStream found [ssrc:%" PRIu32 "]", ssrc);

			return;
		}

		auto* rtpStream = it->second;

		rtpStream->RequestKeyFrame();
	}
} // namespace RTC
