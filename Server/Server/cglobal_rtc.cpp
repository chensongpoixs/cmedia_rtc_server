/***********************************************************************************************
created: 		2022-10-17

author:			chensong

purpose:		cglobal_rtc

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
************************************************************************************************/
#include "cglobal_rtc.h"
#include "DtlsTransport.hpp"
#include "csctp_association_mgr.h"
#include "clog.h"
#include <json/json.h>
#include  <map>
#include "RtpDictionaries.hpp"
namespace chen 
{
	//std::map<uint32, uint32>          PayloadTypeMap = {};
	cglobal_rtc g_global_rtc;
	
	cglobal_rtc::cglobal_rtc()
	{
	}
	cglobal_rtc::~cglobal_rtc()
	{
	}
	
	bool cglobal_rtc::init()
	{
		_init_rtp_capabilitie();

		if (!_config_rtp_info())
		{
			WARNING_EX_LOG("config rtp info failed !!!");
			return false;
		}
		SYSTEM_LOG("config rtp capabilities OK !!!");

		csrtp_session::init();
		SYSTEM_LOG("sctp session init OK !!!");

		

		if (!g_sctp_association_mgr.init())
		{
			return false;
		}
		 
		/*SYSTEM_LOG("sctp_association_mgr init OK !!!");
		RTC::DtlsTransport::ClassInit();
		SYSTEM_LOG("DtlsTransport Init OK !!!");*/
		return true;
	}
	void cglobal_rtc::destory()
	{
		/*RTC::DtlsTransport::ClassDestroy();
		SYSTEM_LOG("DtlsTransport destroy OK !!!");*/
		g_sctp_association_mgr.destroy();
		SYSTEM_LOG("sctp_association_mrg destory OK !!!");
		csrtp_session::destroy();
		SYSTEM_LOG("srtp_session destroy OK !!!"); 
		m_json_rtp_capabilities.clear();
		SYSTEM_LOG("config rtp capabilities clear OK !!!");
	}

	bool cglobal_rtc::_init_rtp_capabilitie()
	{

		int32 StartPayloadType = 100;
		/*
		
		  codecs: [
        {
            kind: 'audio',
            mimeType: 'audio/opus',
            clockRate: 48000,
            channels: 2,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/PCMU',
            preferredPayloadType: 0,
            clockRate: 8000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/PCMA',
            preferredPayloadType: 8,
            clockRate: 8000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/ISAC',
            clockRate: 32000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/ISAC',
            clockRate: 16000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/G722',
            preferredPayloadType: 9,
            clockRate: 8000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/iLBC',
            clockRate: 8000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/SILK',
            clockRate: 24000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/SILK',
            clockRate: 16000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/SILK',
            clockRate: 12000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/SILK',
            clockRate: 8000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/CN',
            preferredPayloadType: 13,
            clockRate: 32000
        },
        {
            kind: 'audio',
            mimeType: 'audio/CN',
            preferredPayloadType: 13,
            clockRate: 16000
        },
        {
            kind: 'audio',
            mimeType: 'audio/CN',
            preferredPayloadType: 13,
            clockRate: 8000
        },
        {
            kind: 'audio',
            mimeType: 'audio/telephone-event',
            clockRate: 48000
        },
        {
            kind: 'audio',
            mimeType: 'audio/telephone-event',
            clockRate: 32000
        },
        {
            kind: 'audio',
            mimeType: 'audio/telephone-event',
            clockRate: 16000
        },
        {
            kind: 'audio',
            mimeType: 'audio/telephone-event',
            clockRate: 8000
        },
        {
            kind: 'video',
            mimeType: 'video/VP8',
            clockRate: 90000,
            rtcpFeedback: [
                { type: 'nack' },
                { type: 'nack', parameter: 'pli' },
                { type: 'ccm', parameter: 'fir' },
                { type: 'goog-remb' },
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'video',
            mimeType: 'video/VP9',
            clockRate: 90000,
            rtcpFeedback: [
                { type: 'nack' },
                { type: 'nack', parameter: 'pli' },
                { type: 'ccm', parameter: 'fir' },
                { type: 'goog-remb' },
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'video',
            mimeType: 'video/H264',
            clockRate: 90000,
            parameters: {
                'packetization-mode': 1,
                'level-asymmetry-allowed': 1
            },
            rtcpFeedback: [
                { type: 'nack' },
                { type: 'nack', parameter: 'pli' },
                { type: 'ccm', parameter: 'fir' },
                { type: 'goog-remb' },
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'video',
            mimeType: 'video/H264',
            clockRate: 90000,
            parameters: {
                'packetization-mode': 0,
                'level-asymmetry-allowed': 1
            },
            rtcpFeedback: [
                { type: 'nack' },
                { type: 'nack', parameter: 'pli' },
                { type: 'ccm', parameter: 'fir' },
                { type: 'goog-remb' },
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'video',
            mimeType: 'video/H265',
            clockRate: 90000,
            parameters: {
                'packetization-mode': 1,
                'level-asymmetry-allowed': 1
            },
            rtcpFeedback: [
                { type: 'nack' },
                { type: 'nack', parameter: 'pli' },
                { type: 'ccm', parameter: 'fir' },
                { type: 'goog-remb' },
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'video',
            mimeType: 'video/H265',
            clockRate: 90000,
            parameters: {
                'packetization-mode': 0,
                'level-asymmetry-allowed': 1
            },
            rtcpFeedback: [
                { type: 'nack' },
                { type: 'nack', parameter: 'pli' },
                { type: 'ccm', parameter: 'fir' },
                { type: 'goog-remb' },
                { type: 'transport-cc' }
            ]
        }
    ],
    headerExtensions: [
        {
            kind: 'audio',
            uri: 'urn:ietf:params:rtp-hdrext:sdes:mid',
            preferredId: 1,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'video',
            uri: 'urn:ietf:params:rtp-hdrext:sdes:mid',
            preferredId: 1,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'video',
            uri: 'urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id',
            preferredId: 2,
            preferredEncrypt: false,
            direction: 'recvonly'
        },
        {
            kind: 'video',
            uri: 'urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id',
            preferredId: 3,
            preferredEncrypt: false,
            direction: 'recvonly'
        },
        {
            kind: 'audio',
            uri: 'http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time',
            preferredId: 4,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'video',
            uri: 'http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time',
            preferredId: 4,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        // NOTE: For audio we just enable transport-wide-cc-01 when receiving media.
        {
            kind: 'audio',
            uri: 'http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01',
            preferredId: 5,
            preferredEncrypt: false,
            direction: 'recvonly'
        },
        {
            kind: 'video',
            uri: 'http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01',
            preferredId: 5,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        // NOTE: Remove this once framemarking draft becomes RFC.
        {
            kind: 'video',
            uri: 'http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07',
            preferredId: 6,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'video',
            uri: 'urn:ietf:params:rtp-hdrext:framemarking',
            preferredId: 7,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'audio',
            uri: 'urn:ietf:params:rtp-hdrext:ssrc-audio-level',
            preferredId: 10,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'video',
            uri: 'urn:3gpp:video-orientation',
            preferredId: 11,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'video',
            uri: 'urn:ietf:params:rtp-hdrext:toffset',
            preferredId: 12,
            preferredEncrypt: false,
            direction: 'sendrecv'
        }
    ]
		*/
		{
			RTC::RtpCodecParameters  rtpCodecParameter;
			rtpCodecParameter.mimeType.type = RTC::RtpCodecMimeType::Type::VIDEO;
			rtpCodecParameter.mimeType.subtype = RTC::RtpCodecMimeType::Subtype::H264;
			rtpCodecParameter.mimeType.mimeType = "video/H264";
			rtpCodecParameter.clockRate = 90000;
			rtpCodecParameter.payloadType = StartPayloadType++;
			rtpCodecParameter.parameters.insert("packetization-mode", 1);
			rtpCodecParameter.parameters.insert("level-asymmetry-allowed", 1);
			rtpCodecParameter.parameters.insert("profile-level-id", std::string("42e01f"));
			rtpCodecParameter.parameters.insert("x-google-max-bitrate", 28000);
			rtpCodecParameter.parameters.insert("x-google-min-bitrate", 5500);
			rtpCodecParameter.parameters.insert("x-google-start-bitrate", 1000);
			 
			 


			 
			
			rtpCodecParameter.rtcpFeedbacks.push_back(RTC::RtcpFeedback("nack", ""));
			rtpCodecParameter.rtcpFeedbacks.push_back(RTC::RtcpFeedback("nack", "pli"));
			rtpCodecParameter.rtcpFeedbacks.push_back(RTC::RtcpFeedback("ccm", "fir"));
			rtpCodecParameter.rtcpFeedbacks.push_back(RTC::RtcpFeedback("goog_remb", ""));
			rtpCodecParameter.rtcpFeedbacks.push_back(RTC::RtcpFeedback("transport-cc", ""));
			m_rtp_capabilities.m_codecs.push_back(rtpCodecParameter);
		}
		{
			RTC::RtpCodecParameters  rtpCodecParameter;
			rtpCodecParameter.mimeType.type = RTC::RtpCodecMimeType::Type::VIDEO;
			rtpCodecParameter.mimeType.subtype = RTC::RtpCodecMimeType::Subtype::RTX;
			rtpCodecParameter.mimeType.mimeType = "video/rtx";
			rtpCodecParameter.clockRate = 90000;
			rtpCodecParameter.parameters.insert("apt", StartPayloadType - 1);
			/*if (!rtpCodecParameter.parameters.mapKeyValues.insert(std::make_pair("apt", RTC::Parameters::Value(StartPayloadType -1))).second)
			{
				WARNING_EX_LOG("rtp codec parameter install failed (key = `apt`)");
			}*/
			rtpCodecParameter.payloadType = StartPayloadType++;

			m_rtp_capabilities.m_codecs.push_back(rtpCodecParameter);
		}

		// rtp header extension
		/*{
			for (std::pair<const std::string, RTC::RtpHeaderExtensionUri::Type>& pi : RTC::RtpHeaderExtensionUri::string2Type)
			{
				RTC::RtpHeaderExtensionParameters rtp_header_extension;
				rtp_header_extension.id = static_cast<uint8>(pi.second);
				rtp_header_extension.uri = pi.first;
				rtp_header_extension.type = RTC::RtpHeaderExtensionUri::GetType(rtp_header_extension.uri);
				rtp_header_extension.parameters.mapKeyValues.insert(std::make_pair("direction", "sendrecv"));
				m_rtp_capabilities.m_header_extensions.push_back(rtp_header_extension);
			}
		}*/
		////////////////////////////////////////////////////////////////////////////////////////////////



		return true;
	}

	bool cglobal_rtc::_config_rtp_info()
	{
		m_json_rtp_capabilities.clear();
		Json::Value codecs;

		{
			//Json::Value codec;

			for (const RTC::RtpCodecParameters& rtp_codec : m_rtp_capabilities.m_codecs)
			{
				Json::Value codec;
				if (rtp_codec.mimeType.type == RTC::RtpCodecMimeType::Type::VIDEO)
				{
					codec["kind"] = "video";
					//codec["mineType"] = "video";
				}
				else if (rtp_codec.mimeType.type == RTC::RtpCodecMimeType::Type::AUDIO)
				{
					codec["kind"] = "audio";
					//codec["mineType"] = "audio";
					//codec[""]
				}
				else
				{
					ERROR_EX_LOG("not mimType Type = %d", rtp_codec.mimeType.type);
				}

				codec["mimeType"] = rtp_codec.mimeType.mimeType; ;
				codec["clockRate"] = rtp_codec.clockRate;
				codec["preferredPayloadType"] = rtp_codec.payloadType;

				for (const  std::pair<const std::string,   RTC::Parameters::Value> &pi : rtp_codec.parameters.mapKeyValues)
				{
					if (pi.second.type == RTC::Parameters::Value::Type::ARRAY_OF_INTEGERS)
					{
						std::string value;
						for (const uint32 & num : pi.second.arrayOfIntegers)
						{
							value += " " + num;
						}
						codec["parameters"][pi.first] = value;
					}
					else if (pi.second.type == RTC::Parameters::Value::Type::BOOLEAN)
					{
						codec["parameters"][pi.first] = pi.second.booleanValue;
					}
					else if (pi.second.type == RTC::Parameters::Value::Type::INTEGER)
					{
						codec["parameters"][pi.first] = pi.second.integerValue;
					}
					else if (pi.second.type == RTC::Parameters::Value::Type::DOUBLE)
					{
						codec["parameters"][pi.first] = pi.second.doubleValue;
					}
					else if (pi.second.type == RTC::Parameters::Value::Type::STRING)
					{
						codec["parameters"][pi.first] = pi.second.stringValue;
					}
					
				}

				Json::Value rtcpFeedback;
				for (const RTC::RtcpFeedback & rtcp_feedback: rtp_codec.rtcpFeedbacks)
				{
					Json::Value typeobj;
					{
						typeobj["type"] = rtcp_feedback.type;
						typeobj["parameter"] = rtcp_feedback.parameter;
					}
					rtcpFeedback.append(typeobj);
				}
				codec["rtcpFeedback"] = rtcpFeedback;
				codecs.append(codec);
			}

			

		Json::Value headerExtensions;
		/*
		  codecs: [
        {
            kind: 'audio',
            mimeType: 'audio/opus',
            clockRate: 48000,
            channels: 2,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/PCMU',
            preferredPayloadType: 0,
            clockRate: 8000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/PCMA',
            preferredPayloadType: 8,
            clockRate: 8000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/ISAC',
            clockRate: 32000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/ISAC',
            clockRate: 16000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/G722',
            preferredPayloadType: 9,
            clockRate: 8000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/iLBC',
            clockRate: 8000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/SILK',
            clockRate: 24000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/SILK',
            clockRate: 16000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/SILK',
            clockRate: 12000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/SILK',
            clockRate: 8000,
            rtcpFeedback: [
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'audio',
            mimeType: 'audio/CN',
            preferredPayloadType: 13,
            clockRate: 32000
        },
        {
            kind: 'audio',
            mimeType: 'audio/CN',
            preferredPayloadType: 13,
            clockRate: 16000
        },
        {
            kind: 'audio',
            mimeType: 'audio/CN',
            preferredPayloadType: 13,
            clockRate: 8000
        },
        {
            kind: 'audio',
            mimeType: 'audio/telephone-event',
            clockRate: 48000
        },
        {
            kind: 'audio',
            mimeType: 'audio/telephone-event',
            clockRate: 32000
        },
        {
            kind: 'audio',
            mimeType: 'audio/telephone-event',
            clockRate: 16000
        },
        {
            kind: 'audio',
            mimeType: 'audio/telephone-event',
            clockRate: 8000
        },
        {
            kind: 'video',
            mimeType: 'video/VP8',
            clockRate: 90000,
            rtcpFeedback: [
                { type: 'nack' },
                { type: 'nack', parameter: 'pli' },
                { type: 'ccm', parameter: 'fir' },
                { type: 'goog-remb' },
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'video',
            mimeType: 'video/VP9',
            clockRate: 90000,
            rtcpFeedback: [
                { type: 'nack' },
                { type: 'nack', parameter: 'pli' },
                { type: 'ccm', parameter: 'fir' },
                { type: 'goog-remb' },
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'video',
            mimeType: 'video/H264',
            clockRate: 90000,
            parameters: {
                'packetization-mode': 1,
                'level-asymmetry-allowed': 1
            },
            rtcpFeedback: [
                { type: 'nack' },
                { type: 'nack', parameter: 'pli' },
                { type: 'ccm', parameter: 'fir' },
                { type: 'goog-remb' },
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'video',
            mimeType: 'video/H264',
            clockRate: 90000,
            parameters: {
                'packetization-mode': 0,
                'level-asymmetry-allowed': 1
            },
            rtcpFeedback: [
                { type: 'nack' },
                { type: 'nack', parameter: 'pli' },
                { type: 'ccm', parameter: 'fir' },
                { type: 'goog-remb' },
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'video',
            mimeType: 'video/H265',
            clockRate: 90000,
            parameters: {
                'packetization-mode': 1,
                'level-asymmetry-allowed': 1
            },
            rtcpFeedback: [
                { type: 'nack' },
                { type: 'nack', parameter: 'pli' },
                { type: 'ccm', parameter: 'fir' },
                { type: 'goog-remb' },
                { type: 'transport-cc' }
            ]
        },
        {
            kind: 'video',
            mimeType: 'video/H265',
            clockRate: 90000,
            parameters: {
                'packetization-mode': 0,
                'level-asymmetry-allowed': 1
            },
            rtcpFeedback: [
                { type: 'nack' },
                { type: 'nack', parameter: 'pli' },
                { type: 'ccm', parameter: 'fir' },
                { type: 'goog-remb' },
                { type: 'transport-cc' }
            ]
        }
    ],
    headerExtensions: [
        {
            kind: 'audio',
            uri: 'urn:ietf:params:rtp-hdrext:sdes:mid',
            preferredId: 1,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'video',
            uri: 'urn:ietf:params:rtp-hdrext:sdes:mid',
            preferredId: 1,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'video',
            uri: 'urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id',
            preferredId: 2,
            preferredEncrypt: false,
            direction: 'recvonly'
        },
        {
            kind: 'video',
            uri: 'urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id',
            preferredId: 3,
            preferredEncrypt: false,
            direction: 'recvonly'
        },
        {
            kind: 'audio',
            uri: 'http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time',
            preferredId: 4,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'video',
            uri: 'http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time',
            preferredId: 4,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        // NOTE: For audio we just enable transport-wide-cc-01 when receiving media.
        {
            kind: 'audio',
            uri: 'http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01',
            preferredId: 5,
            preferredEncrypt: false,
            direction: 'recvonly'
        },
        {
            kind: 'video',
            uri: 'http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01',
            preferredId: 5,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        // NOTE: Remove this once framemarking draft becomes RFC.
        {
            kind: 'video',
            uri: 'http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07',
            preferredId: 6,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'video',
            uri: 'urn:ietf:params:rtp-hdrext:framemarking',
            preferredId: 7,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'audio',
            uri: 'urn:ietf:params:rtp-hdrext:ssrc-audio-level',
            preferredId: 10,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'video',
            uri: 'urn:3gpp:video-orientation',
            preferredId: 11,
            preferredEncrypt: false,
            direction: 'sendrecv'
        },
        {
            kind: 'video',
            uri: 'urn:ietf:params:rtp-hdrext:toffset',
            preferredId: 12,
            preferredEncrypt: false,
            direction: 'sendrecv'
        }
    ]
		
		*/
		{
			//for (const RTC::RtpHeaderExtensionParameters & rtp_header_extension : m_rtp_capabilities.m_header_extensions)
			//{
			//	Json::Value extension;
			//	//extension["kind"] = rtp_header_extension.type;
			//	extension["uri"] = rtp_header_extension.uri;
			//	extension["preferredId"] = rtp_header_extension.id;
			//	extension["preferredEncrypt"] = rtp_header_extension.encrypt;
			//	extension["direction"] = "sendrecv";
			//	headerExtensions.append(extension);

			//}
			Json::Value extension;
			extension["kind"] = "video";
			extension["uri"] = "urn:ietf:params:rtp-hdrext:sdes:mid";
			extension["preferredId"] = 1;
			extension["preferredEncrypt"] = false;
			extension["direction"] = "sendrecv";
			headerExtensions.append(extension);

			//Json::Value extension;
			extension["kind"] = "video";
			extension["uri"] = "urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id";
			extension["preferredId"] = 2;
			extension["preferredEncrypt"] = false;
			extension["direction"] = "recvonly";
			headerExtensions.append(extension);


			extension["kind"] = "video";
			extension["uri"] = "urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id";
			extension["preferredId"] = 3;
			extension["preferredEncrypt"] = false;
			extension["direction"] = "recvonly";
			headerExtensions.append(extension);


			extension["kind"] = "video";
			extension["uri"] = "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time";
			extension["preferredId"] = 4;
			extension["preferredEncrypt"] = false;
			extension["direction"] = "sendrecv";
			headerExtensions.append(extension);


			extension["kind"] = "video";
			extension["uri"] = "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01";
			extension["preferredId"] = 5;
			extension["preferredEncrypt"] = false;
			extension["direction"] = "sendrecv";
			headerExtensions.append(extension);


			extension["kind"] = "video";
			extension["uri"] = "http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07";
			extension["preferredId"] = 6;
			extension["preferredEncrypt"] = false;
			extension["direction"] = "sendrecv";
			headerExtensions.append(extension);


			extension["kind"] = "video";
			extension["uri"] = "urn:ietf:params:rtp-hdrext:framemarking";
			extension["preferredId"] = 7;
			extension["preferredEncrypt"] = false;
			extension["direction"] = "sendrecv";
			headerExtensions.append(extension);

			extension["kind"] = "video";
			extension["uri"] = "urn:3gpp:video-orientation";
			extension["preferredId"] = 11;
			extension["preferredEncrypt"] = false;
			extension["direction"] = "sendrecv";
			headerExtensions.append(extension);

			extension["kind"] = "video";
			extension["uri"] = "urn:ietf:params:rtp-hdrext:toffset";
			extension["preferredId"] = 12;
			extension["preferredEncrypt"] = false;
			extension["direction"] = "sendrecv";
			headerExtensions.append(extension);

		}

		m_json_rtp_capabilities["codecs"] = codecs;
		m_json_rtp_capabilities["headerExtensions"] = headerExtensions;
		 
		return true;
	}



	/*
	{
	"codecs": [{
		"kind": "video",
		"mimeType": "video/H264",
		"clockRate": 90000,
		"parameters": {
			"packetization-mode": 1,
			"level-asymmetry-allowed": 1,
			"profile-level-id": "42e01f",
			"x-google-start-bitrate": 1000,
			"x-google-max-bitrate": 28000,
			"x-google-min-bitrate": 5500
		},
		"rtcpFeedback": [{
			"type": "nack",
			"parameter": ""
		}, {
			"type": "nack",
			"parameter": "pli"
		}, {
			"type": "ccm",
			"parameter": "fir"
		}, {
			"type": "goog-remb",
			"parameter": ""
		}, {
			"type": "transport-cc",
			"parameter": ""
		}],
		"preferredPayloadType": 100
	}, {
		"kind": "video",
		"mimeType": "video/rtx",
		"preferredPayloadType": 101,
		"clockRate": 90000,
		"parameters": {
			"apt": 100
		},
		"rtcpFeedback": []
	}, {
		"kind": "video",
		"mimeType": "video/H264",
		"clockRate": 90000,
		"parameters": {
			"packetization-mode": 1,
			"level-asymmetry-allowed": 1,
			"profile-level-id": "4d0032",
			"x-google-start-bitrate": 1000
		},
		"rtcpFeedback": [{
			"type": "nack",
			"parameter": ""
		}, {
			"type": "nack",
			"parameter": "pli"
		}, {
			"type": "ccm",
			"parameter": "fir"
		}, {
			"type": "goog-remb",
			"parameter": ""
		}, {
			"type": "transport-cc",
			"parameter": ""
		}],
		"preferredPayloadType": 102
	}, {
		"kind": "video",
		"mimeType": "video/rtx",
		"preferredPayloadType": 103,
		"clockRate": 90000,
		"parameters": {
			"apt": 102
		},
		"rtcpFeedback": []
	}, {
		"kind": "video",
		"mimeType": "video/VP8",
		"clockRate": 90000,
		"rtcpFeedback": [{
			"type": "nack",
			"parameter": ""
		}, {
			"type": "nack",
			"parameter": "pli"
		}, {
			"type": "ccm",
			"parameter": "fir"
		}, {
			"type": "goog-remb",
			"parameter": ""
		}, {
			"type": "transport-cc",
			"parameter": ""
		}],
		"parameters": {
			"x-google-start-bitrate": 1000
		},
		"preferredPayloadType": 104
	}, {
		"kind": "video",
		"mimeType": "video/rtx",
		"preferredPayloadType": 105,
		"clockRate": 90000,
		"parameters": {
			"apt": 104
		},
		"rtcpFeedback": []
	}, {
		"kind": "video",
		"mimeType": "video/VP9",
		"clockRate": 90000,
		"rtcpFeedback": [{
			"type": "nack",
			"parameter": ""
		}, {
			"type": "nack",
			"parameter": "pli"
		}, {
			"type": "ccm",
			"parameter": "fir"
		}, {
			"type": "goog-remb",
			"parameter": ""
		}, {
			"type": "transport-cc",
			"parameter": ""
		}],
		"parameters": {
			"profile-id": 2,
			"x-google-start-bitrate": 1000
		},
		"preferredPayloadType": 106
	}, {
		"kind": "video",
		"mimeType": "video/rtx",
		"preferredPayloadType": 107,
		"clockRate": 90000,
		"parameters": {
			"apt": 106
		},
		"rtcpFeedback": []
	}],
	"headerExtensions": [{
		"kind": "audio",
		"uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
		"preferredId": 1,
		"preferredEncrypt": false,
		"direction": "sendrecv"
	}, {
		"kind": "video",
		"uri": "urn:ietf:params:rtp-hdrext:sdes:mid",
		"preferredId": 1,
		"preferredEncrypt": false,
		"direction": "sendrecv"
	}, {
		"kind": "video",
		"uri": "urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id",
		"preferredId": 2,
		"preferredEncrypt": false,
		"direction": "recvonly"
	}, {
		"kind": "video",
		"uri": "urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id",
		"preferredId": 3,
		"preferredEncrypt": false,
		"direction": "recvonly"
	}, {
		"kind": "audio",
		"uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
		"preferredId": 4,
		"preferredEncrypt": false,
		"direction": "sendrecv"
	}, {
		"kind": "video",
		"uri": "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",
		"preferredId": 4,
		"preferredEncrypt": false,
		"direction": "sendrecv"
	}, {
		"kind": "audio",
		"uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01",
		"preferredId": 5,
		"preferredEncrypt": false,
		"direction": "recvonly"
	}, {
		"kind": "video",
		"uri": "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01",
		"preferredId": 5,
		"preferredEncrypt": false,
		"direction": "sendrecv"
	}, {
		"kind": "video",
		"uri": "http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07",
		"preferredId": 6,
		"preferredEncrypt": false,
		"direction": "sendrecv"
	}, {
		"kind": "video",
		"uri": "urn:ietf:params:rtp-hdrext:framemarking",
		"preferredId": 7,
		"preferredEncrypt": false,
		"direction": "sendrecv"
	}, {
		"kind": "audio",
		"uri": "urn:ietf:params:rtp-hdrext:ssrc-audio-level",
		"preferredId": 10,
		"preferredEncrypt": false,
		"direction": "sendrecv"
	}, {
		"kind": "video",
		"uri": "urn:3gpp:video-orientation",
		"preferredId": 11,
		"preferredEncrypt": false,
		"direction": "sendrecv"
	}, {
		"kind": "video",
		"uri": "urn:ietf:params:rtp-hdrext:toffset",
		"preferredId": 12,
		"preferredEncrypt": false,
		"direction": "sendrecv"
	}]
}
	*/

	

}
}