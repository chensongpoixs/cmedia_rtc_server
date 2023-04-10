//
///***********************************************************************************************
//created: 		2023-04-08
//
//author:			chensong
//
//purpose:		ccfg
//************************************************************************************************/
#ifndef _C_RTP_HEADER_EXTENSIONS_H_
#define _C_RTP_HEADER_EXTENSIONS_H_
#include "cnet_type.h"
#include "crtp_rtcp_define.h"
#include "clog.h"
#include "crtp_headers.h"
namespace chen {



	//// clang-format off
	//std::unordered_map<std::string, RtpHeaderExtensionUri::Type> RtpHeaderExtensionUri::string2Type =
	//{
	//	//  在 unified SDP 描述中 ‘a=mid’ 是每个 audio/video line 的必要元素，这个 header extension 将 SDP 中 ‘a=mid’ 后信息保存，
	//	// 用于标识一个 RTP packet 的 media 信息，可以作为一个 media 的唯一标识 
	//	{ "urn:ietf:params:rtp-hdrext:sdes:mid",                                       RtpHeaderExtensionUri::Type::MID                    },
	//	// 1. Media Source 等同于 WebRTC 中 Track 的概念，在 SDP 描述中可以使用 mid 作为唯一标识
	//	// 2. RTP Stream 是 RTP 流传输的最小流单位，例如在 Simulcast 或 SVC 场景中，一个 Media Source 中包含多个 RTP Stream，这时 SDP 中使用 ‘a=rid’ 来描述每个 RTP Stream
	//	{ "urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id",                             RtpHeaderExtensionUri::Type::RTP_STREAM_ID          },
	//	// 用于声明重传时使用的 rid 标识	
	//	{ "urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id",                    RtpHeaderExtensionUri::Type::REPAIRED_RTP_STREAM_ID },
	//	// 1. abs-send-time 为 一个 3 bytes 的时间数据
	//	// 2. REMB 计算需要 RTP 报文扩展头部 abs-send-time 的支持，用以记录 RTP 数据包在发送端的绝对发送时间
	//	{ "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",                RtpHeaderExtensionUri::Type::ABS_SEND_TIME          },
	//	//Transport-wide Congestion Control

	//	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	// 有效数据只有 16 bit，记录了一个 sequence number 称为 transport-wide sequence number
	//	// 发送端在发送 RTP 数据包时，在 RTP 头部扩展中设置传输层序列号 TransportSequenceNumber；
	//	// 数据包到达接收端后记录该序列号和包到达时间，然后接收端基于此构造 TransportCC 报文返回到发送端；
	//	// 发送端解析该报文并执行 SendSide-BWE 算法，计算得到基于延迟的码率；
	//	// 最终和基于丢包率的码率进行比较得到最终目标码率
	//	{ "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01", RtpHeaderExtensionUri::Type::TRANSPORT_WIDE_CC_01   },
	//	// NOTE: Remove this once framemarking draft becomes RFC.


	//	{ "http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07",              RtpHeaderExtensionUri::Type::FRAME_MARKING_07       },


	//	// 由于 WebRTC 中 RTP payload 通过 SRTP 进行加密，这样导致 RTP packet 在经过交换节点或转发节点时，
	//	// 有些场景下需要知道当前 RTP packet 的编码信息，framemarking 用于给定该编码信息 
	//	{ "urn:ietf:params:rtp-hdrext:framemarking",                                   RtpHeaderExtensionUri::Type::FRAME_MARKING          },

	//	// 用于音量调节
	//	{ "urn:ietf:params:rtp-hdrext:ssrc-audio-level",                               RtpHeaderExtensionUri::Type::SSRC_AUDIO_LEVEL       },

	//	// 接收方收到以后，也会在响应中加入该 tag 属性，这样就完成了该功能的协商。Tag 中的 7 是扩展数据帧的扩展 id，
	//	// 可以是 1-15 中的任何一个 , 用来标识该方向数据的位置，具体参考 RFC5285。更多描述参考 RCC.07- 2.7.1.2.2。
	//	// 以上所述的视频方向其实包含了数据的方向和发送方 camera 的选项（前置或者后置），为了方便起见，一下都称为视频方向数据。
	//	{ "urn:3gpp:video-orientation",                                                RtpHeaderExtensionUri::Type::VIDEO_ORIENTATION      },

	//	// 传输时间偏移 (Transmission Time Offset)，offset 为 RTP packet 中 timestamp 与 实际发送时间的偏移
	//	{ "urn:ietf:params:rtp-hdrext:toffset",                                        RtpHeaderExtensionUri::Type::TOFFSET                }

	//};
	enum ERtpHeaderExtensionUri
	{
			EUNKNOWN = 0, 
 	//  在 unified SDP 描述中 ‘a=mid’ 是每个 audio/video line 的必要元素，这个 header extension 将 SDP 中 ‘a=mid’ 后信息保存，
    // 用于标识一个 RTP packet 的 media 信息，可以作为一个 media 的唯一标识 
			EMID = 1, // urn:ietf:params:rtp-hdrext:sdes:mid
    // 1. Media Source 等同于 WebRTC 中 Track 的概念，在 SDP 描述中可以使用 mid 作为唯一标识
    // 2. RTP Stream 是 RTP 流传输的最小流单位，例如在 Simulcast 或 SVC 场景中，一个 Media Source 中包含多个 RTP Stream，这时 SDP 中使用 ‘a=rid’ 来描述每个 RTP Stream

			ERTP_STREAM_ID = 2, //urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id
	// 用于声明重传时使用的 rid 标识	
			EREPAIRED_RTP_STREAM_ID = 3, //urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id
	 	// 1. abs-send-time 为 一个 3 bytes 的时间数据
	   // 2. REMB 计算需要 RTP 报文扩展头部 abs-send-time 的支持，用以记录 RTP 数据包在发送端的绝对发送时间
			EABS_SEND_TIME = 4, //http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
			//	// 有效数据只有 16 bit，记录了一个 sequence number 称为 transport-wide sequence number
	//	// 发送端在发送 RTP 数据包时，在 RTP 头部扩展中设置传输层序列号 TransportSequenceNumber；
	//	// 数据包到达接收端后记录该序列号和包到达时间，然后接收端基于此构造 TransportCC 报文返回到发送端；
	//	// 发送端解析该报文并执行 SendSide-BWE 算法，计算得到基于延迟的码率；
	//	// 最终和基于丢包率的码率进行比较得到最终目标码率
			ETRANSPORT_WIDE_CC_01 = 5, //http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
			EFRAME_MARKING_07 = 6, // NOTE: Remove once RFC. http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07
		//	// 由于 WebRTC 中 RTP payload 通过 SRTP 进行加密，这样导致 RTP packet 在经过交换节点或转发节点时，
	//	// 有些场景下需要知道当前 RTP packet 的编码信息，framemarking 用于给定该编码信息 
			EFRAME_MARKING = 7, //urn:ietf:params:rtp-hdrext:framemarking
	// 用于音量调节
			ESSRC_AUDIO_LEVEL = 10, //urn:ietf:params:rtp-hdrext:ssrc-audio-level
		//	// 接收方收到以后，也会在响应中加入该 tag 属性，这样就完成了该功能的协商。Tag 中的 7 是扩展数据帧的扩展 id，
	//	// 可以是 1-15 中的任何一个 , 用来标识该方向数据的位置，具体参考 RFC5285。更多描述参考 RCC.07- 2.7.1.2.2。
	//	// 以上所述的视频方向其实包含了数据的方向和发送方 camera 的选项（前置或者后置），为了方便起见，一下都称为视频方向数据。
			EVIDEO_ORIENTATION = 11, //urn:3gpp:video-orientation
	// 传输时间偏移 (Transmission Time Offset)，offset 为 RTP packet 中 timestamp 与 实际发送时间的偏移
			ETOFFSET = 12 //urn:ietf:params:rtp-hdrext:toffset
	};
	/* Struct for RTP header extension. */
	struct RtpHeaderExtension
	{
		uint16 id;
		uint16 length; // Size of value in multiples of 4 bytes.
		uint8 value[1];
	};
	/* Struct for One-Byte extension. */
	struct OneByteExtension
	{
#if defined(WIN32 )
		uint8 len : 4;
		uint8 id : 4;
#elif defined(MS_BIG_ENDIAN)
		uint8 id : 4;
		uint8 len : 4;
#endif
		uint8 value[1];
	};

	/* Struct for Two-Bytes extension. */
	struct TwoBytesExtension
	{
		uint8 id : 8;
		uint8 len : 8;
		uint8 value[1];
	};

//	class AbsoluteSendTime {
//	public:
//		using value_type = uint32;
//		static const   RTPExtensionType kId = kRtpExtensionAbsoluteSendTime;
//		static const   uint8_t kValueSizeBytes = 3;
//		const char kUri[] = "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time";
//
//		static bool Parse(uint8 * data, size_t size, uint32* time_24bits);
//		static size_t ValueSize(uint32 time_24bits) { return kValueSizeBytes; }
//		static bool Write(uint8* data, size_t size, uint32 time_24bits);
//
//		static   uint32 MsTo24Bits(int64 time_ms) 
//		{
//			return static_cast<uint32>(((time_ms << 18) + 500) / 1000) & 0x00FFFFFF;
//		}
//	};
//
//	class AudioLevel {
//	public:
//		  static const   RTPExtensionType kId = kRtpExtensionAudioLevel;
//		  static const   uint8_t kValueSizeBytes = 1;
//		    const char kUri[] = "urn:ietf:params:rtp-hdrext:ssrc-audio-level";
//
//		static bool Parse(uint8* data, size_t size, bool* voice_activity, uint8* audio_level);
//		static size_t ValueSize(bool voice_activity, uint8_t audio_level) 
//		{
//			return kValueSizeBytes;
//		}
//		static bool Write(uint8* data, size_t size, bool voice_activity, uint8  audio_level);
//	};
//
//	class TransmissionOffset {
//	public:
//		using value_type = int32;
//		static const   RTPExtensionType kId = kRtpExtensionTransmissionTimeOffset;
//		static const    uint8_t kValueSizeBytes = 3;
//		const char kUri[] = "urn:ietf:params:rtp-hdrext:toffset";
//
//		static bool Parse(uint8* data, size_t size, int32* rtp_time);
//		static size_t ValueSize(int32_t rtp_time) { return kValueSizeBytes; }
//		static bool Write(uint8* data, size_t size, int32 rtp_time);
//	};
//
//	class TransportSequenceNumber {
//	public:
//		using value_type = uint16_t;
//		static const     RTPExtensionType kId = kRtpExtensionTransportSequenceNumber;
//		static const     uint8  kValueSizeBytes = 2;
//		const char kUri[] = "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01";
//		static bool Parse(const uint8* data, size_t size, uint16* transport_sequence_number);
//		static size_t ValueSize(uint16_t /*transport_sequence_number*/) {
//			return kValueSizeBytes;
//		}
//		static bool Write(  uint8* data, size_t size, uint16  transport_sequence_number);
//	};
//
//	class TransportSequenceNumberV2 {
//	public:
//		static const      RTPExtensionType kId = kRtpExtensionTransportSequenceNumber02;
//		static const      uint8  kValueSizeBytes = 4;
//		static const      uint8  kValueSizeBytesWithoutFeedbackRequest = 2; 
//
//		  const char kUri[] = "http://www.webrtc.org/experiments/rtp-hdrext/transport-wide-cc-02";
//
//		static bool Parse(const uint8* data, size_t size, uint16_t* transport_sequence_number,
//			 FeedbackRequest * feedback_request, bool & feedback);
//		static size_t ValueSize( uint16_t /*transport_sequence_number*/,
//			const  FeedbackRequest & feedback_request, bool feedback) 
//		{
//			return feedback ? kValueSizeBytes : kValueSizeBytesWithoutFeedbackRequest;
//		}
//		static bool Write(  uint8* data, size_t size, uint16_t transport_sequence_number,
//			const  FeedbackRequest & feedback_request, bool feedback);
//
//	private:
//		 static const    uint16 kIncludeTimestampsBit = 1 << 15;
//	};
//
//	class VideoOrientation {
//	public:
//		using value_type = VideoRotation;
//		 static const    RTPExtensionType kId = kRtpExtensionVideoRotation;
//		 static const   uint8_t kValueSizeBytes = 1;
//		    const char kUri[] = "urn:3gpp:video-orientation";
//
//		static bool Parse(const uint8*data, size_t size, VideoRotation* value);
//		static size_t ValueSize(VideoRotation) { return kValueSizeBytes; }
//		static bool Write(rtc::ArrayView<uint8_t> data, VideoRotation value);
//		static bool Parse(rtc::ArrayView<const uint8_t> data, uint8_t* value);
//		static size_t ValueSize(uint8_t value) { return kValueSizeBytes; }
//		static bool Write(rtc::ArrayView<uint8_t> data, uint8_t value);
//	};
//
//	class PlayoutDelayLimits {
//	public:
//		using value_type = PlayoutDelay;
//		static const   RTPExtensionType kId = kRtpExtensionPlayoutDelay;
//		static const   uint8_t kValueSizeBytes = 3;
//		    const char kUri[] = "http://www.webrtc.org/experiments/rtp-hdrext/playout-delay";
//
//		// Playout delay in milliseconds. A playout delay limit (min or max)
//		// has 12 bits allocated. This allows a range of 0-4095 values which
//		// translates to a range of 0-40950 in milliseconds.
//		    int kGranularityMs = 10;
//		// Maximum playout delay value in milliseconds.
//		    int kMaxMs = 0xfff * kGranularityMs;  // 40950.
//
//		static bool Parse(rtc::ArrayView<const uint8_t> data,
//			PlayoutDelay* playout_delay);
//		static size_t ValueSize(const PlayoutDelay&) {
//			return kValueSizeBytes;
//		}
//		static bool Write(rtc::ArrayView<uint8_t> data,
//			const PlayoutDelay& playout_delay);
//	};
//
//	class VideoContentTypeExtension {
//	public:
//		using value_type = VideoContentType;
//		    RTPExtensionType kId = kRtpExtensionVideoContentType;
//		    uint8_t kValueSizeBytes = 1;
//		    const char kUri[] =
//			"http://www.webrtc.org/experiments/rtp-hdrext/video-content-type";
//
//		static bool Parse(rtc::ArrayView<const uint8_t> data,
//			VideoContentType* content_type);
//		static size_t ValueSize(VideoContentType) {
//			return kValueSizeBytes;
//		}
//		static bool Write(rtc::ArrayView<uint8_t> data,
//			VideoContentType content_type);
//	};
//
//	class VideoTimingExtension {
//	public:
//		using value_type = VideoSendTiming;
//		    RTPExtensionType kId = kRtpExtensionVideoTiming;
//		    uint8_t kValueSizeBytes = 13;
//		    const char kUri[] =
//			"http://www.webrtc.org/experiments/rtp-hdrext/video-timing";
//
//		static bool Parse(rtc::ArrayView<const uint8_t> data,
//			VideoSendTiming* timing);
//		static size_t ValueSize(const VideoSendTiming&) { return kValueSizeBytes; }
//		static bool Write(rtc::ArrayView<uint8_t> data,
//			const VideoSendTiming& timing);
//
//		static size_t ValueSize(uint16_t time_delta_ms, uint8_t idx) {
//			return kValueSizeBytes;
//		}
//		// Writes only single time delta to position idx.
//		static bool Write(rtc::ArrayView<uint8_t> data,
//			uint16_t time_delta_ms,
//			uint8_t idx);
//	};
//
//	class FrameMarkingExtension {
//	public:
//		using value_type = FrameMarking;
//		    RTPExtensionType kId = kRtpExtensionFrameMarking;
//		    const char kUri[] =
//			"http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07";
//
//		static bool Parse(rtc::ArrayView<const uint8_t> data,
//			FrameMarking* frame_marking);
//		static size_t ValueSize(const FrameMarking& frame_marking);
//		static bool Write(rtc::ArrayView<uint8_t> data,
//			const FrameMarking& frame_marking);
//
//	private:
//		static bool IsScalable(uint8_t temporal_id, uint8_t layer_id);
//	};
//
//	class ColorSpaceExtension {
//	public:
//		using value_type = ColorSpace;
//		    RTPExtensionType kId = kRtpExtensionColorSpace;
//		    uint8_t kValueSizeBytes = 28;
//		    uint8_t kValueSizeBytesWithoutHdrMetadata = 4;
//		    const char kUri[] =
//			"http://www.webrtc.org/experiments/rtp-hdrext/color-space";
//
//		static bool Parse(rtc::ArrayView<const uint8_t> data,
//			ColorSpace* color_space);
//		static size_t ValueSize(const ColorSpace& color_space) {
//			return color_space.hdr_metadata() ? kValueSizeBytes
//				: kValueSizeBytesWithoutHdrMetadata;
//		}
//		static bool Write(rtc::ArrayView<uint8_t> data,
//			const ColorSpace& color_space);
//
//	private:
//		    int kChromaticityDenominator = 50000;  // 0.00002 resolution.
//		    int kLuminanceMaxDenominator = 1;      // 1 resolution.
//		    int kLuminanceMinDenominator = 10000;  // 0.0001 resolution.
//
//		static uint8_t CombineRangeAndChromaSiting(
//			ColorSpace::RangeID range,
//			ColorSpace::ChromaSiting chroma_siting_horizontal,
//			ColorSpace::ChromaSiting chroma_siting_vertical);
//		static size_t ParseHdrMetadata(rtc::ArrayView<const uint8_t> data,
//			HdrMetadata* hdr_metadata);
//		static size_t ParseChromaticity(const uint8_t* data,
//			HdrMasteringMetadata::Chromaticity* p);
//		static size_t ParseLuminance(const uint8_t* data, float* f, int denominator);
//		static size_t WriteHdrMetadata(rtc::ArrayView<uint8_t> data,
//			const HdrMetadata& hdr_metadata);
//		static size_t WriteChromaticity(uint8_t* data,
//			const HdrMasteringMetadata::Chromaticity& p);
//		static size_t WriteLuminance(uint8_t* data, float f, int denominator);
//	};
//
//	// Base extension class for RTP header extensions which are strings.
//	// Subclasses must defined kId and kUri static constexpr members.
//	class BaseRtpStringExtension {
//	public:
//		using value_type = std::string;
//		// String RTP header extensions are limited to 16 bytes because it is the
//		// maximum length that can be encoded with one-byte header extensions.
//		    uint8_t kMaxValueSizeBytes = 16;
//
//		static bool Parse(rtc::ArrayView<const uint8_t> data, std::string* str);
//		static size_t ValueSize(const std::string& str) { return str.size(); }
//		static bool Write(rtc::ArrayView<uint8_t> data, const std::string& str);
//	};
//
//	class RtpStreamId : public BaseRtpStringExtension {
//	public:
//		    RTPExtensionType kId = kRtpExtensionRtpStreamId;
//		    const char kUri[] =
//			"urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id";
//	};
//
//	class RepairedRtpStreamId : public BaseRtpStringExtension {
//	public:
//		    RTPExtensionType kId = kRtpExtensionRepairedRtpStreamId;
//		    const char kUri[] =
//			"urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id";
//	};
//
//	class RtpMid : public BaseRtpStringExtension {
//	public:
//		    RTPExtensionType kId = kRtpExtensionMid;
//		    const char kUri[] = "urn:ietf:params:rtp-hdrext:sdes:mid";
//	};
}

#endif // _C_RTP_HEADER_EXTENSIONS_H_
//
//
//
//
//
