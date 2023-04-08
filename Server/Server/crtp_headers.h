/***********************************************************************************************
created: 		2023-04-08

author:			chensong

purpose:		ccfg
************************************************************************************************/
#ifndef _C_RTP_HEADERS_H_
#define _C_RTP_HEADERS_H_
#include "cnet_type.h"
namespace chen {
	/*class crtp_headers
	{
	public:
	protected:
	private:
	};*/
	struct FeedbackRequest {
		// Determines whether the recv delta as specified in
		// https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01
		// should be included.
		bool m_include_timestamps;
		// Include feedback of received packets in the range [sequence_number -
		// sequence_count + 1, sequence_number]. That is, no feedback will be sent if
		// sequence_count is zero.
		int32 m_sequence_count;
	};


	//struct RTPHeaderExtension {
	//	RTPHeaderExtension();
	//	RTPHeaderExtension(const RTPHeaderExtension& other);
	//	RTPHeaderExtension& operator=(const RTPHeaderExtension& other);

	//	bool hasTransmissionTimeOffset;
	//	int32_t transmissionTimeOffset;
	//	bool hasAbsoluteSendTime;
	//	uint32_t absoluteSendTime;
	//	bool hasTransportSequenceNumber;
	//	uint16_t transportSequenceNumber;
	//	absl::optional<FeedbackRequest> feedback_request;

	//	// Audio Level includes both level in dBov and voiced/unvoiced bit. See:
	//	// https://datatracker.ietf.org/doc/draft-lennox-avt-rtp-audio-level-exthdr/
	//	bool hasAudioLevel;
	//	bool voiceActivity;
	//	uint8_t audioLevel;

	//	// For Coordination of Video Orientation. See
	//	// http://www.etsi.org/deliver/etsi_ts/126100_126199/126114/12.07.00_60/
	//	// ts_126114v120700p.pdf
	//	bool hasVideoRotation;
	//	VideoRotation videoRotation;

	//	// TODO(ilnik): Refactor this and one above to be absl::optional() and remove
	//	// a corresponding bool flag.
	//	bool hasVideoContentType;
	//	VideoContentType videoContentType;

	//	bool has_video_timing;
	//	VideoSendTiming video_timing;

	//	bool has_frame_marking;
	//	FrameMarking frame_marking;

	//	PlayoutDelay playout_delay = { -1, -1 };

	//	// For identification of a stream when ssrc is not signaled. See
	//	// https://tools.ietf.org/html/draft-ietf-avtext-rid-09
	//	// TODO(danilchap): Update url from draft to release version.
	//	std::string stream_id;
	//	std::string repaired_stream_id;

	//	// For identifying the media section used to interpret this RTP packet. See
	//	// https://tools.ietf.org/html/draft-ietf-mmusic-sdp-bundle-negotiation-38
	//	std::string mid;

	//	//absl::optional<ColorSpace> color_space;
	//};

	struct RTPHeader {
		RTPHeader();
		RTPHeader(const RTPHeader& other);
		RTPHeader& operator=(const RTPHeader& other);

		bool markerBit;   // 是否是一帧结束包
		uint8_t payloadType;
		uint16_t sequenceNumber;
		uint32_t timestamp;
		uint32_t ssrc;
		uint8_t numCSRCs;
		uint32_t arrOfCSRCs[/*kRtpCsrcSize*/15];
		size_t paddingLength;
		size_t headerLength;
		int payload_type_frequency;
		//RTPHeaderExtension extension;
	};



	/* Struct for RTP header. */
	struct RtpHeader
	{
#if defined(WIN32 )
		uint8_t csrcCount : 4;
		uint8_t extension : 1;
		uint8_t padding : 1;
		uint8_t version : 2;
		uint8_t payloadType : 7;
		uint8_t marker : 1;
#elif defined(MS_BIG_ENDIAN)
		uint8_t version : 2;
		uint8_t padding : 1;
		uint8_t extension : 1;
		uint8_t csrcCount : 4;
		uint8_t marker : 1;
		uint8_t payloadType : 7;
#endif
		uint16_t sequenceNumber;
		uint32_t timestamp;
		uint32_t ssrc;
	};
	// RTCP mode to use. Compound mode is described by RFC 4585 and reduced-size
	// RTCP mode is described by RFC 5506.
	// TODO@chensong 2022-10-30   中文翻译 https://rfc2cn.com/rfc5506.html
	enum class RtcpMode { kOff, kCompound, kReducedSize };

	enum NetworkState {
		kNetworkUp,
		kNetworkDown,
	};
}

#endif // _C_RTP_HEADERS_H_