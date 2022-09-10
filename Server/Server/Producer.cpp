//#define MS_CLASS "RTC::Producer"
// #define MS_LOG_DEV_LEVEL 3

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
#include <cassert>
#include <vector>
#include <algorithm>


namespace RTC
{
	/* Instance methods. */

	Producer::Producer(const std::string& id, RTC::Producer::Listener* listener, const RTC::RtpParameters& rtp_Parameter/*, Json::Value& data*/)
		: id(id)
		, listener(listener)
	{

		if (rtp_Parameter.m_codec_type == RtpCodecMimeType::Type::VIDEO)
		{
			kind = RTC::Media::Kind::VIDEO;
		}
		else
		{
			kind = RTC::Media::Kind::AUDIO;
		}
		paused = false;

		rtpParameters = rtp_Parameter;
		////////////////////////////////////////////////mid////////////////////////////////////////////////////////
		//
		// default video h264 RtpParameters
		// 1. 
		//rtpParameters.mid = "0";

		///////////////////////////////////////////////////RTCP///////////////////////////////////////////////////////////
		//// 2. rtcp 
		//rtpParameters.rtcp.cname = "UgYi3789TL6C/8Zx";
		//rtpParameters.rtcp.reducedSize = true;

		//////////////////////////////////////////////////codecs///////////////////////////////////////////////////////////
		//// 3. codecs
		////	3.1 RTP
		//RtpCodecParameters  rtpcodecparameter;
		//rtpcodecparameter.clockRate = 90000;
		//rtpcodecparameter.mimeType.type = RtpCodecMimeType::Type::VIDEO;
		//rtpcodecparameter.mimeType.subtype = RtpCodecMimeType::Subtype::H264;
		//rtpcodecparameter.mimeType.mimeType = "video/H264";
		//rtpcodecparameter.payloadType = 108; 
		//rtpcodecparameter.parameters.mapKeyValues.emplace("level-asymmetry-allowed", Parameters::Value(1));
		//rtpcodecparameter.parameters.mapKeyValues.emplace("packetization-mode", Parameters::Value(1));
		//rtpcodecparameter.parameters.mapKeyValues.emplace("profile-level-id", Parameters::Value("42e01f"));
		////// 服务质量Qos RTCPfeedback
		//RtcpFeedback rtcpfeedback;

		//rtcpfeedback.type = "goog-remb";
		//rtpcodecparameter.rtcpFeedbacks.push_back(rtcpfeedback);

		//rtcpfeedback.type = "transport-cc";
		//rtpcodecparameter.rtcpFeedbacks.push_back(rtcpfeedback);

		//rtcpfeedback.type = "ccm";
		//rtcpfeedback.parameter = "fir";
		//rtpcodecparameter.rtcpFeedbacks.push_back(rtcpfeedback);

		//rtcpfeedback.type = "nack";
		//rtcpfeedback.parameter = "";
		//rtpcodecparameter.rtcpFeedbacks.push_back(rtcpfeedback);


		//rtcpfeedback.type = "nack";
		//rtcpfeedback.parameter = "pli";
		//rtpcodecparameter.rtcpFeedbacks.push_back(rtcpfeedback);
		// 

		//rtpParameters.codecs.push_back(rtpcodecparameter);
		//// RTX RtpCodecParameters  rtpcodecparameter;
		//RtpCodecParameters  rtx_rtpcodecparameter;
		//rtx_rtpcodecparameter.clockRate = 90000;
		//rtx_rtpcodecparameter.mimeType.type = RtpCodecMimeType::Type::VIDEO;
		//rtx_rtpcodecparameter.mimeType.subtype = RtpCodecMimeType::Subtype::RTX;
		//rtx_rtpcodecparameter.mimeType.mimeType = "video/rtx";
		//rtx_rtpcodecparameter.payloadType = 109;

		//rtx_rtpcodecparameter.parameters.mapKeyValues.emplace("apt", Parameters::Value(108));
		//rtpParameters.codecs.push_back(rtx_rtpcodecparameter);

		///////////////////////////////////encodings////////////////////////////////////////////
		//RtpEncodingParameters rtpencodingparameter;
		//rtpencodingparameter.ssrc = 899346955;
		//rtpencodingparameter.rtx.ssrc = 28746816;
		//rtpParameters.encodings.push_back(rtpencodingparameter);


		//////////////////////////////////////RtpHeaderExtensionParameters///////////////////////////////////////////////////////
		//RtpHeaderExtensionParameters rtpheaderextensionparameter;
		//rtpheaderextensionparameter.uri = "urn:ietf:params:rtp-hdrext:sdes:mid";
		//rtpheaderextensionparameter.type = RTC::RtpHeaderExtensionUri::GetType(rtpheaderextensionparameter.uri);
		//rtpheaderextensionparameter.id = 3;
		//rtpheaderextensionparameter.encrypt = false;

		//rtpParameters.headerExtensions.push_back(rtpheaderextensionparameter);
		//rtpheaderextensionparameter.uri = "urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id";
		//rtpheaderextensionparameter.type = RTC::RtpHeaderExtensionUri::GetType(rtpheaderextensionparameter.uri);
		//rtpheaderextensionparameter.id = 10;
		//rtpheaderextensionparameter.encrypt = false;

		//rtpParameters.headerExtensions.push_back(rtpheaderextensionparameter);

		//////
		//rtpheaderextensionparameter.uri = "urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id";
		//rtpheaderextensionparameter.type = RTC::RtpHeaderExtensionUri::GetType(rtpheaderextensionparameter.uri);
		//rtpheaderextensionparameter.id = 11;
		//rtpheaderextensionparameter.encrypt = false;

		//rtpParameters.headerExtensions.push_back(rtpheaderextensionparameter);
		//
		//rtpheaderextensionparameter.uri = "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time";
		//rtpheaderextensionparameter.type = RTC::RtpHeaderExtensionUri::GetType(rtpheaderextensionparameter.uri);
		//rtpheaderextensionparameter.id = 2;
		//rtpheaderextensionparameter.encrypt = false;

		//rtpParameters.headerExtensions.push_back(rtpheaderextensionparameter);


		//rtpheaderextensionparameter.uri = "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01";
		//rtpheaderextensionparameter.type = RTC::RtpHeaderExtensionUri::GetType(rtpheaderextensionparameter.uri);
		//rtpheaderextensionparameter.id = 3;
		//rtpheaderextensionparameter.encrypt = false;

		//rtpParameters.headerExtensions.push_back(rtpheaderextensionparameter);

		//rtpheaderextensionparameter.uri = "urn:3gpp:video-orientation";
		//rtpheaderextensionparameter.type = RTC::RtpHeaderExtensionUri::GetType(rtpheaderextensionparameter.uri);
		//rtpheaderextensionparameter.id = 13;
		//rtpheaderextensionparameter.encrypt = false;

		//rtpParameters.headerExtensions.push_back(rtpheaderextensionparameter);

		//rtpheaderextensionparameter.uri = "urn:ietf:params:rtp-hdrext:toffset";
		//rtpheaderextensionparameter.type = RTC::RtpHeaderExtensionUri::GetType(rtpheaderextensionparameter.uri);
		//rtpheaderextensionparameter.id = 14;
		//rtpheaderextensionparameter.encrypt = false;

		//rtpParameters.headerExtensions.push_back(rtpheaderextensionparameter);

		// Fill RTP header extension ids.
		// This may throw.
		for (auto& exten : this->rtpParameters.headerExtensions)
		{
			if (exten.id == 0u)
			{
				ERROR_EX_LOG("RTP extension id cannot be 0");
			}

			if (this->rtpHeaderExtensionIds.mid == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::MID)
			{
				this->rtpHeaderExtensionIds.mid = exten.id;
			}

			if (this->rtpHeaderExtensionIds.rid == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::RTP_STREAM_ID)
			{
				this->rtpHeaderExtensionIds.rid = exten.id;
			}

			if (this->rtpHeaderExtensionIds.rrid == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::REPAIRED_RTP_STREAM_ID)
			{
				this->rtpHeaderExtensionIds.rrid = exten.id;
			}

			if (this->rtpHeaderExtensionIds.absSendTime == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::ABS_SEND_TIME)
			{
				this->rtpHeaderExtensionIds.absSendTime = exten.id;
			}

			if (this->rtpHeaderExtensionIds.transportWideCc01 == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::TRANSPORT_WIDE_CC_01)
			{
				this->rtpHeaderExtensionIds.transportWideCc01 = exten.id;
			}

			// NOTE: Remove this once framemarking draft becomes RFC.
			if (this->rtpHeaderExtensionIds.frameMarking07 == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::FRAME_MARKING_07)
			{
				this->rtpHeaderExtensionIds.frameMarking07 = exten.id;
			}

			if (this->rtpHeaderExtensionIds.frameMarking == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::FRAME_MARKING)
			{
				this->rtpHeaderExtensionIds.frameMarking = exten.id;
			}

			if (this->rtpHeaderExtensionIds.ssrcAudioLevel == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::SSRC_AUDIO_LEVEL)
			{
				this->rtpHeaderExtensionIds.ssrcAudioLevel = exten.id;
			}

			if (this->rtpHeaderExtensionIds.videoOrientation == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::VIDEO_ORIENTATION)
			{
				this->rtpHeaderExtensionIds.videoOrientation = exten.id;
			}

			if (this->rtpHeaderExtensionIds.toffset == 0u && exten.type == RTC::RtpHeaderExtensionUri::Type::TOFFSET)
			{
				this->rtpHeaderExtensionIds.toffset = exten.id;
			}
		}


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
			uint32_t keyFrameRequestDelay = 100u;

			// clang-format off
			//if (
			//	data.isMember("keyFrameRequestDelay") && data["keyFrameRequestDelay"].isUInt64()
			//	)
			//	// clang-format on
			//{
			//	keyFrameRequestDelay = data["keyFrameRequestDelay"].asUInt();// jsonKeyFrameRequestDelayIt->get<uint32_t>();
			//}
			//keyFrameRequestDelay = 500;
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

	//void Producer::FillJson(json& jsonObject) const
	//{
	//	MS_TRACE();

	//	// Add id.
	//	jsonObject["id"] = this->id;

	//	// Add kind.
	//	jsonObject["kind"] = RTC::Media::GetString(this->kind);

	//	// Add rtpParameters.
	//	this->rtpParameters.FillJson(jsonObject["rtpParameters"]);

	//	// Add type.
	//	jsonObject["type"] = RTC::RtpParameters::GetTypeString(this->type);

	//	// Add rtpMapping.
	//	jsonObject["rtpMapping"] = json::object();
	//	auto jsonRtpMappingIt    = jsonObject.find("rtpMapping");

	//	// Add rtpMapping.codecs.
	//	{
	//		(*jsonRtpMappingIt)["codecs"] = json::array();
	//		auto jsonCodecsIt             = jsonRtpMappingIt->find("codecs");
	//		size_t idx{ 0 };

	//		for (auto& kv : this->rtpMapping.codecs)
	//		{
	//			jsonCodecsIt->emplace_back(json::value_t::object);

	//			auto& jsonEntry        = (*jsonCodecsIt)[idx];
	//			auto payloadType       = kv.first;
	//			auto mappedPayloadType = kv.second;

	//			jsonEntry["payloadType"]       = payloadType;
	//			jsonEntry["mappedPayloadType"] = mappedPayloadType;

	//			++idx;
	//		}
	//	}

	//	// Add rtpMapping.encodings.
	//	{
	//		(*jsonRtpMappingIt)["encodings"] = json::array();
	//		auto jsonEncodingsIt             = jsonRtpMappingIt->find("encodings");

	//		for (size_t i{ 0 }; i < this->rtpMapping.encodings.size(); ++i)
	//		{
	//			jsonEncodingsIt->emplace_back(json::value_t::object);

	//			auto& jsonEntry             = (*jsonEncodingsIt)[i];
	//			const auto& encodingMapping = this->rtpMapping.encodings[i];

	//			if (!encodingMapping.rid.empty())
	//				jsonEntry["rid"] = encodingMapping.rid;
	//			else
	//				jsonEntry["rid"] = nullptr;

	//			if (encodingMapping.ssrc != 0u)
	//				jsonEntry["ssrc"] = encodingMapping.ssrc;
	//			else
	//				jsonEntry["ssrc"] = nullptr;

	//			jsonEntry["mappedSsrc"] = encodingMapping.mappedSsrc;
	//		}
	//	}

	//	// Add rtpStreams.
	//	jsonObject["rtpStreams"] = json::array();
	//	auto jsonRtpStreamsIt    = jsonObject.find("rtpStreams");

	//	for (auto* rtpStream : this->rtpStreamByEncodingIdx)
	//	{
	//		if (!rtpStream)
	//			continue;

	//		jsonRtpStreamsIt->emplace_back(json::value_t::object);

	//		auto& jsonEntry = (*jsonRtpStreamsIt)[jsonRtpStreamsIt->size() - 1];

	//		rtpStream->FillJson(jsonEntry);
	//	}

	//	// Add paused.
	//	jsonObject["paused"] = this->paused;

	//	// Add traceEventTypes.
	//	std::vector<std::string> traceEventTypes;
	//	std::ostringstream traceEventTypesStream;

	//	if (this->traceEventTypes.rtp)
	//		traceEventTypes.emplace_back("rtp");
	//	if (this->traceEventTypes.keyframe)
	//		traceEventTypes.emplace_back("keyframe");
	//	if (this->traceEventTypes.nack)
	//		traceEventTypes.emplace_back("nack");
	//	if (this->traceEventTypes.pli)
	//		traceEventTypes.emplace_back("pli");
	//	if (this->traceEventTypes.fir)
	//		traceEventTypes.emplace_back("fir");

	//	if (!traceEventTypes.empty())
	//	{
	//		std::copy(
	//		  traceEventTypes.begin(),
	//		  traceEventTypes.end() - 1,
	//		  std::ostream_iterator<std::string>(traceEventTypesStream, ","));
	//		traceEventTypesStream << traceEventTypes.back();
	//	}

	//	jsonObject["traceEventTypes"] = traceEventTypesStream.str();
	//}

	//void Producer::FillJsonStats(json& jsonArray) const
	//{
	//	MS_TRACE();

	//	for (auto* rtpStream : this->rtpStreamByEncodingIdx)
	//	{
	//		if (!rtpStream)
	//			continue;

	//		jsonArray.emplace_back(json::value_t::object);

	//		auto& jsonEntry = jsonArray[jsonArray.size() - 1];

	//		rtpStream->FillJsonStats(jsonEntry);
	//	}
	//}

	//void Producer::HandleRequest(Channel::ChannelRequest* request)
	//{
	//	MS_TRACE();

	//	switch (request->methodId)
	//	{
	//		case Channel::ChannelRequest::MethodId::PRODUCER_DUMP:
	//		{
	//			json data = json::object();

	//			FillJson(data);

	//			request->Accept(data);

	//			break;
	//		}

	//		case Channel::ChannelRequest::MethodId::PRODUCER_GET_STATS:
	//		{
	//			json data = json::array();

	//			FillJsonStats(data);

	//			request->Accept(data);

	//			break;
	//		}

	//		case Channel::ChannelRequest::MethodId::PRODUCER_PAUSE:
	//		{
	//			if (this->paused)
	//			{
	//				request->Accept();

	//				return;
	//			}

	//			// Pause all streams.
	//			for (auto& kv : this->mapSsrcRtpStream)
	//			{
	//				auto* rtpStream = kv.second;

	//				rtpStream->Pause();
	//			}

	//			this->paused = true;

	//			MS_DEBUG_DEV("Producer paused [producerId:%s]", this->id.c_str());

	//			this->listener->OnProducerPaused(this);

	//			request->Accept();

	//			break;
	//		}

	//		case Channel::ChannelRequest::MethodId::PRODUCER_RESUME:
	//		{
	//			if (!this->paused)
	//			{
	//				request->Accept();

	//				return;
	//			}

	//			// Resume all streams.
	//			for (auto& kv : this->mapSsrcRtpStream)
	//			{
	//				auto* rtpStream = kv.second;

	//				rtpStream->Resume();
	//			}

	//			this->paused = false;

	//			MS_DEBUG_DEV("Producer resumed [producerId:%s]", this->id.c_str());

	//			this->listener->OnProducerResumed(this);

	//			if (this->keyFrameRequestManager)
	//			{
	//				MS_DEBUG_2TAGS(rtcp, rtx, "requesting forced key frame(s) after resumed");

	//				// Request a key frame for all streams.
	//				for (auto& kv : this->mapSsrcRtpStream)
	//				{
	//					auto ssrc = kv.first;

	//					this->keyFrameRequestManager->ForceKeyFrameNeeded(ssrc);
	//				}
	//			}

	//			request->Accept();

	//			break;
	//		}

	//		case Channel::ChannelRequest::MethodId::PRODUCER_ENABLE_TRACE_EVENT:
	//		{
	//			auto jsonTypesIt = request->data.find("types");

	//			// Disable all if no entries.
	//			if (jsonTypesIt == request->data.end() || !jsonTypesIt->is_array())
	//				MS_THROW_TYPE_ERROR("wrong types (not an array)");

	//			// Reset traceEventTypes.
	//			struct TraceEventTypes newTraceEventTypes;

	//			for (const auto& type : *jsonTypesIt)
	//			{
	//				if (!type.is_string())
	//					MS_THROW_TYPE_ERROR("wrong type (not a string)");

	//				std::string typeStr = type.get<std::string>();

	//				if (typeStr == "rtp")
	//					newTraceEventTypes.rtp = true;
	//				else if (typeStr == "keyframe")
	//					newTraceEventTypes.keyframe = true;
	//				else if (typeStr == "nack")
	//					newTraceEventTypes.nack = true;
	//				else if (typeStr == "pli")
	//					newTraceEventTypes.pli = true;
	//				else if (typeStr == "fir")
	//					newTraceEventTypes.fir = true;
	//			}

	//			this->traceEventTypes = newTraceEventTypes;

	//			request->Accept();

	//			break;
	//		}

	//		default:
	//		{
	//			MS_THROW_ERROR("unknown method '%s'", request->method.c_str());
	//		}
	//	}
	//}

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
			WARNING_EX_LOG("rtp, no stream found for received packet [ssrc:%u]", packet->GetSsrc());

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
			//aserrt("found stream does not match received packet");
		}

		if (packet->IsKeyFrame())
		{
			DEBUG_EX_LOG("rtp, key frame received [ssrc:%u, seq:%hu]",
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

		DEBUG_EX_LOG("rtcp, RtpStream not found [ssrc:%u]", report->GetSsrc());
	}

	void Producer::ReceiveRtcpXrDelaySinceLastRr(RTC::RTCP::DelaySinceLastRr::SsrcInfo* ssrcInfo)
	{
		//MS_TRACE();

		auto it = this->mapSsrcRtpStream.find(ssrcInfo->GetSsrc());

		if (it == this->mapSsrcRtpStream.end())
		{
			WARNING_EX_LOG("rtcp, RtpStream not found [ssrc:%u]", ssrcInfo->GetSsrc());

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
			auto ntp     = rtc_time::TimeMs2Ntp(nowMs);
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
							WARNING_EX_LOG("rtp, ignoring packet with unknown ssrc but already handled RID (RID lookup)");

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
					WARNING_EX_LOG("rtp, ignoring packet with unknown ssrc not matching the already existing stream (single RtpStream lookup)");

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
					DEBUG_EX_LOG("rtp, rtx, ignoring RTX packet for not yet created RtpStream (single stream lookup)");

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

		assert(
		  this->mapSsrcRtpStream.find(ssrc) == this->mapSsrcRtpStream.end(),
		  "RtpStream with given SSRC already exists");
		assert(
		  !this->rtpStreamByEncodingIdx[encodingIdx],
		  "RtpStream for given encoding index already exists");

		auto& encoding        = this->rtpParameters.encodings[encodingIdx];
		auto& encodingMapping = this->rtpMapping.encodings[encodingIdx];

		DEBUG_EX_LOG("rtp, [encodingIdx:%zu, ssrc:%u, rid:%s, payloadType:%hhu]",
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

		/*for (const auto& fb : mediaCodec.rtcpFeedback)
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
		}*/

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
				WARNING_EX_LOG("rtp, unknown payload type [payloadType:%hhu]", payloadType);

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

					//json data = json::object();

					///data["camera"]   = this->videoOrientation.camera;
					//data["flip"]     = this->videoOrientation.flip;
					//data["rotation"] = this->videoOrientation.rotation;

					//Channel::ChannelNotifier::Emit(this->id, "videoorientationchange", data);
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
	//	MS_TRACE();

		//if (this->traceEventTypes.keyframe && packet->IsKeyFrame())
		//{
		//	json data = json::object();

		//	data["type"]      = "keyframe";
		//	data["timestamp"] = DepLibUV::GetTimeMs();
		//	data["direction"] = "in";

		//	packet->FillJson(data["info"]);

		//	if (isRtx)
		//		data["info"]["isRtx"] = true;

		//	Channel::ChannelNotifier::Emit(this->id, "trace", data);
		//}
		//else if (this->traceEventTypes.rtp)
		//{
		//	json data = json::object();

		//	data["type"]      = "rtp";
		//	data["timestamp"] = DepLibUV::GetTimeMs();
		//	data["direction"] = "in";

		//	packet->FillJson(data["info"]);

		//	if (isRtx)
		//		data["info"]["isRtx"] = true;

		//	Channel::ChannelNotifier::Emit(this->id, "trace", data);
		//}
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
			WARNING_EX_LOG("rtcp, rtx, no associated RtpStream found [ssrc:%u]", ssrc);

			return;
		}

		auto* rtpStream = it->second;

		rtpStream->RequestKeyFrame();
	}
} // namespace RTC
