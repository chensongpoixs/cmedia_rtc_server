/***********************************************************************************************
created: 		2023-04-08

author:			chensong

purpose:		ccfg
WebRTC 中对应rtp的扩展头信息的作用
************************************************************************************************/
#include "crtp_header_extension_uri.h"
#include <unordered_map>
#include <string>
#include "crtc_sdp_define.h"
namespace chen {


	static std::unordered_map<std::string, ERtpHeaderExtensionUri> g_rtp_header_extension_uri  =
	{
		//  在 unified SDP 描述中 ‘a=mid’ 是每个 audio/video line 的必要元素，这个 header extension 将 SDP 中 ‘a=mid’ 后信息保存，
		// 用于标识一个 RTP packet 的 media 信息，可以作为一个 media 的唯一标识 
		{ RtpExtension_kMidUri,                                       EMID                    },
		// 1. Media Source 等同于 WebRTC 中 Track 的概念，在 SDP 描述中可以使用 mid 作为唯一标识
		// 2. RTP Stream 是 RTP 流传输的最小流单位，例如在 Simulcast 或 SVC 场景中，一个 Media Source 中包含多个 RTP Stream，这时 SDP 中使用 ‘a=rid’ 来描述每个 RTP Stream
		{ RtpExtension_kRidUri,                             ERTP_STREAM_ID          },
		// 用于声明重传时使用的 rid 标识	
		{ RtpExtension_kRepairedRidUri,                    EREPAIRED_RTP_STREAM_ID },
		// 1. abs-send-time 为 一个 3 bytes 的时间数据
		// 2. REMB 计算需要 RTP 报文扩展头部 abs-send-time 的支持，用以记录 RTP 数据包在发送端的绝对发送时间
		{ RtpExtension_kAbsSendTimeUri,                EABS_SEND_TIME          },
		//Transport-wide Congestion Control

		///////////////////////////////////////////////////////////////////////////////////////////////////
		// 有效数据只有 16 bit，记录了一个 sequence number 称为 transport-wide sequence number
		// 发送端在发送 RTP 数据包时，在 RTP 头部扩展中设置传输层序列号 TransportSequenceNumber；
		// 数据包到达接收端后记录该序列号和包到达时间，然后接收端基于此构造 TransportCC 报文返回到发送端；
		// 发送端解析该报文并执行 SendSide-BWE 算法，计算得到基于延迟的码率；
		// 最终和基于丢包率的码率进行比较得到最终目标码率
		{ kTWCCExt, ETRANSPORT_WIDE_CC_01   },
		// NOTE: Remove this once framemarking draft becomes RFC.


		{ RtpExtension_kFrameMarking07Uri,              EFRAME_MARKING_07       },


		// 由于 WebRTC 中 RTP payload 通过 SRTP 进行加密，这样导致 RTP packet 在经过交换节点或转发节点时，
		// 有些场景下需要知道当前 RTP packet 的编码信息，framemarking 用于给定该编码信息 
		{ RtpExtension_kFrameMarkingUri,                                   EFRAME_MARKING          },

		// 用于音量调节
		{ RtpExtension_kAudioLevelUri,                               ESSRC_AUDIO_LEVEL       },

		// 接收方收到以后，也会在响应中加入该 tag 属性，这样就完成了该功能的协商。Tag 中的 7 是扩展数据帧的扩展 id，
		// 可以是 1-15 中的任何一个 , 用来标识该方向数据的位置，具体参考 RFC5285。更多描述参考 RCC.07- 2.7.1.2.2。
		// 以上所述的视频方向其实包含了数据的方向和发送方 camera 的选项（前置或者后置），为了方便起见，一下都称为视频方向数据。
		{ RtpExtension_kVideoRotationUri,                                                EVIDEO_ORIENTATION      },

		// 传输时间偏移 (Transmission Time Offset)，offset 为 RTP packet 中 timestamp 与 实际发送时间的偏移
		{ RtpExtension_kTimestampOffsetUri,                                        ETOFFSET                }

	};
	ERtpHeaderExtensionUri get_rtp_header_extension_uri_type(const std::string & uri)
	{
		std::unordered_map<std::string, ERtpHeaderExtensionUri>::iterator iter =  g_rtp_header_extension_uri.find(uri);
		if (iter != g_rtp_header_extension_uri.end())
		{
			return iter->second;
		}
		return EUNKNOWN;
	}
}