/***********************************************************************************************
created: 		2022-10-17

author:			chensong

purpose:		cglobal_rtc


************************************************************************************************/
#include "cglobal_rtc.h"
#include "DtlsTransport.hpp"
#include "csctp_association_mgr.h"
#include "clog.h"
#include <json/json.h>
namespace chen {
	cglobal_rtc g_global_rtc;
	cglobal_rtc::cglobal_rtc()
	{
	}
	cglobal_rtc::~cglobal_rtc()
	{
	}
	bool cglobal_rtc::init()
	{
		if (!g_sctp_association_mgr.init())
		{
			return false;
		}
		SYSTEM_LOG("sctp_association_mgr init OK !!!");
		RTC::DtlsTransport::ClassInit();
		SYSTEM_LOG("DtlsTransport Init OK !!!");
		return true;
	}
	void cglobal_rtc::destory()
	{
		RTC::DtlsTransport::ClassDestroy();
		SYSTEM_LOG("DtlsTransport destroy OK !!!");
		g_sctp_association_mgr.destroy();
		SYSTEM_LOG("sctp_association_mrg destory OK !!!");
		 
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

	Json::Value  cglobal_rtc::all_rtp_capabilities()
	{
		Json::Value all_rtp;


		Json::Value codecs;
		
		{
			Json::Value codec;
			codec["kind"] = "video";
			codec["mimeType"] = "video/H264";
			codec["clockRate"]  = 90000;
			codec["parameters"]["packetization-mode"] = 1;
			codec["parameters"]["level-asymmetry-allowed"] = 1;
			codec["parameters"]["profile-level-id"] = "42e01f";
			codec["parameters"]["x-google-start-bitrate"] = 1000;
			codec["parameters"]["x-google-max-bitrate"] = 28000;
			codec["parameters"]["x-google-min-bitrate"] = 5500;
			codec["preferredPayloadType"] = 100;
			{
				Json::Value rtcpFeedback;
				Json::Value typeobj;
				{
					typeobj["type"] = "nack";
					typeobj["parameter"] = "";
				}
				rtcpFeedback.append(typeobj);

				{
					typeobj["type"] = "nack";
					typeobj["parameter"] = "pli";
				}
				rtcpFeedback.append(typeobj);
				{
					typeobj["type"] = "ccm";
					typeobj["parameter"] = "fir";
				}
				rtcpFeedback.append(typeobj);

				{
					typeobj["type"] = "goog-remb";
					typeobj["parameter"] = "";
				}
				rtcpFeedback.append(typeobj);
				{
					typeobj["type"] = "transport-cc";
					typeobj["parameter"] = "";
				}
				rtcpFeedback.append(typeobj);
				codec["rtcpFeedback"] = rtcpFeedback;
			}
			codecs.append(codec);
		}
		{
			Json::Value codec;
			codec["kind"] = "video";
			codec["mimeType"] = "video/rtx";
			codec["preferredPayloadType"] = 101;
			codec["clockRate"] = 90000;
			codec["parameters"]["apt"] = 100;
			codec["rtcpFeedback"] = Json::Value();
			codecs.append(codec);
		}
		
		Json::Value headerExtensions;

		{
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
		
		all_rtp["codecs"] = codecs;
		all_rtp["headerExtensions"] = headerExtensions;
		return all_rtp;
	}

}