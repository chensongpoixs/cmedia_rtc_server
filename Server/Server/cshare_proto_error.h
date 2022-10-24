/***********************************************************************************************
					created: 		2019-05-13

					author:			chensong

					purpose:		msg_base_id
************************************************************************************************/


#ifndef _C_SHARE_PROTO_ERROR_H_
#define _C_SHARE_PROTO_ERROR_H_

namespace chen {
	enum ESharePtotoType
	{
		EShareProtoOk = 0,
		EShareProtoData = 300, // data数据没有
		EShareProtoUserName , // 用户名字段
		EShareProtoRoomName, // 房间
		EShareProtoSdp, // SDP info 
		EShareProtoParseSdpFailed, // 解析SDP失败 
		EShareProtoCreateRoomFailed,
		EShareProtoNotTransportId,
		EShareProtoNotTransportIdRtcObject,
		EShareProtoRtcConnectStatusError,
		EShareProtoDtlsParametersRoleTypeError,
		EShareProtoNotFindDtlsFingerprints,
		EShareProtoMediaNotKind,
		EShareProtoNotRtpParameters,
		EShareProtoParseProduceFailed,
	};
}

#endif // _C_SHARE_PROTO_ERROR_H_