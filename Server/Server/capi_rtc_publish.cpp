/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/
#include "capi_rtc_publish.h"
#include "crtc_user_config.h"
#include "cshare_proto_error.h"
namespace chen {
	capi_rtc_publish::~capi_rtc_publish()
	{
	}
	int32 capi_rtc_publish::do_serve_client(const std::string & remote_sdp)
	{
		int32 err = 0;
		crtc_user_config ruc;

		ruc.m_api = "";

		ruc.m_eip = "127.0.0.1"; // candidate eip  of server 

		ruc.m_codec = ""; // 
		ruc.m_publish = true;
		ruc.m_dtls = true;

		ruc.m_remote_sdp_str = remote_sdp;


		if ((err = ruc.m_remote_sdp.parse(remote_sdp)) != 0)
		{
			ERROR_EX_LOG("parse sdp failed: %s", remote_sdp.c_str());
			return err;
		}



		return err;
	}
	int32 capi_rtc_publish::_serve_client(crtc_user_config * ruc)
	{

		int32 err = 0;
		if ((err = _check_remote_sdp(ruc->m_remote_sdp)) != 0) 
		{
			WARNING_EX_LOG("remote sdp check failed");
			return err;
			//return error_wrap(err, "remote sdp check failed");
		}

		crtc_sdp local_sdp;


		// TODO: FIXME: move to create_session.
		// Config for SDP and session.
		/*
		a = setup 主要是表示dtls的协商过程中角色的问题，谁是客户端，谁是服务器
		a = setup : actpass 既可以是客户端，也可以是服务器
		a = setup : active 客户端
		a = setup : passive 服务器
		由客户端先发起client hello

		*/
		local_sdp.m_session_config.m_dtls_role =  "passive";
		local_sdp.m_session_config.m_dtls_version = "auto";



		return err;
	}

	int32 capi_rtc_publish::_check_remote_sdp(const crtc_sdp & remote_sdp)
	{
		int32 err = 0;

		if (remote_sdp.m_group_policy != "BUNDLE")
		{
			ERROR_EX_LOG("now only support BUNDLE, group policy=%s", remote_sdp.m_group_policy.c_str());
			return EShareRtcSdpExchange;
			//return error_new(ERROR_RTC_SDP_EXCHANGE, "now only support BUNDLE, group policy=%s", remote_sdp.m_group_policy.c_str());
		}

		if (remote_sdp.m_media_descs.empty()) 
		{
			ERROR_EX_LOG("no media descriptions");
			return EShareRtcSdpExchange;
			//return  error_new(ERROR_RTC_SDP_EXCHANGE, "no media descriptions");
		}

		for (std::vector<cmedia_desc>::const_iterator iter = remote_sdp.m_media_descs.begin(); iter != remote_sdp.m_media_descs.end(); ++iter) 
		{
			if (iter->m_type != "audio" && iter->m_type != "video") 
			{
				ERROR_EX_LOG("unsupport media type=%s", iter->m_type.c_str());
				return EShareRtcSdpExchange;
				//return error_new(ERROR_RTC_SDP_EXCHANGE, "unsupport media type=%s", iter->type_.c_str());
			}

			if (!iter->m_rtcp_mux) 
			{
				ERROR_EX_LOG("now only suppor rtcp-mux");
				return EShareRtcSdpExchange;
				//return  error_new(ERROR_RTC_SDP_EXCHANGE, "now only suppor rtcp-mux");
			}

			if (iter->m_recvonly)
			{
				ERROR_EX_LOG("publish API only support sendrecv/sendonly");
				return EShareRtcSdpExchange;
				//return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "publish API only support sendrecv/sendonly");
			}
		}

		return err;
		 
	}

}