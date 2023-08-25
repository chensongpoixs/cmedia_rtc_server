/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		api_rtc_publish

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
#include "capi_rtc_player.h"
#include "csdp_util.h"
#include "ccfg.h"
#include "capi_rtc_publish.h"
#include "crtc_user_config.h"
#include "cshare_proto_error.h"
#include "crtc_source_description.h"
#include "crtc_track_description.h"
#include "csdp_util.h"
#include "ccfg.h"
#include "cdigit2str.h"
#include "ctransport_mgr.h"
#include "crandom.h"
#include "cdtls_certificate.h"
#include "cglobal_config.h"
#include "crtc_ssrc_generator.h"
#include "crtc_constants.h"
#include "crtp_header_extension_uri.h"
#include "cglobal_rtc_config.h"
#include "cglobal_rtc_port.h"
namespace chen {
	capi_rtc_player::~capi_rtc_player()
	{
	}
	bool capi_rtc_player::do_serve_client(const cclient_player_message & player_message/*const std::string & remote_sdp, const std::string & roomname,
		const std::string & peerid, const std::string & video_peerid*/, std::string & local_sdp)
	{
		crtc_source_description stream_desc;
		crtc_sdp rtc_remote_sdp;
		crtc_sdp rtc_local_sdp;
		// RFC 4145 - http://tools.ietf.org/html/rfc4145#section-4
		// 'active':  The endpoint will initiate an outgoing connection.
		// 'passive': The endpoint will accept an incoming connection.
		// 'actpass': The endpoint is willing to accept an incoming
		//            connection or to initiate an outgoing connection.
		/*a = setup 主要是表示dtls的协商过程中角色的问题，谁是客户端，谁是服务器
		a = setup:actpass 既可以是客户端，也可以是服务器
		a = setup : active 客户端
		a = setup : passive 服务器
		由客户端先发起client hello*/
		rtc_local_sdp.m_session_config.m_dtls_role = "passive";
		rtc_local_sdp.m_session_config.m_dtls_version = "auto";
		std::string planname = player_message.m_room_name + "/" + player_message.m_peer_id;// roomname + "/" + peerid;
		std::string media_stream_url = player_message.m_room_name + "/" + player_message.m_video_peer_id;
		crtc_ssrc_info * rtc_ssrc_info_ptr = g_global_rtc_config.get_stream_uri(media_stream_url/*roomname + "/" + video_peerid*/);
		if (!rtc_ssrc_info_ptr)
		{
			WARNING_EX_LOG("create media ssrc failed !!![ media name = %s ]", media_stream_url.c_str());

			return false;
		}
		rtc_remote_sdp.parse(player_message.m_remote_sdp);
		std::map<uint32_t, crtc_track_description*> play_sub_relations;
		// audio track description
		if (true) {
			crtc_track_description* audio_track_desc = new crtc_track_description();
			stream_desc.m_audio_track_desc_ptr = audio_track_desc;

			audio_track_desc->m_type = "audio";
			audio_track_desc->m_id = "audio-" + c_rand.rand_str(8);
			audio_track_desc->m_msid = planname;//c_rand.rand_str(36);
			//audio_track_desc->m_msid_tracker = c_rand.rand_str(36);;
			//audio_track_desc->m_mid = "0";
			uint32_t audio_ssrc = rtc_ssrc_info_ptr->m_audio_ssrc;// c_rtc_ssrc_generator.generate_ssrc();
			audio_track_desc->m_ssrc = c_rtc_ssrc_generator.generate_ssrc(); ;// audio_ssrc;
			audio_track_desc->m_direction = "sendonly";

			audio_track_desc->m_media_ptr = new caudio_payload(kAudioPayloadType, "opus", kAudioSamplerate, kAudioChannel);
		}

		// video track description
		if (true) {
			crtc_track_description* video_track_desc = new crtc_track_description();
			stream_desc.m_video_track_descs.push_back(video_track_desc);

			video_track_desc->m_type = "video";
			video_track_desc->m_id = "video-" + c_rand.rand_str(8);
			video_track_desc->m_msid = planname;// c_rand.rand_str(36);
			//video_track_desc->m_mid = "1";
			uint32_t video_ssrc = c_rtc_ssrc_generator.generate_ssrc(); ;// rtc_ssrc_info_ptr->m_video_ssrc;// c_rtc_ssrc_generator.generate_ssrc();
			video_track_desc->m_ssrc = video_ssrc;
			video_track_desc->m_direction = "sendonly";

			if (player_message.m_codec == "AV1")
			{
				cvideo_payload* video_payload = new cvideo_payload(kVideoPayloadType, "AV1", kVideoSamplerate);
				video_track_desc->m_media_ptr = video_payload;
			}
			else
			{
				cvideo_payload* video_payload = new cvideo_payload(kVideoPayloadType, "H264", kVideoSamplerate);
				video_track_desc->m_media_ptr = video_payload;

				video_payload->set_h264_param_desc("level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f");
				video_payload->set_bitrate_param_desc("x-google-max-bitrate=" + std::to_string(g_cfg.get_uint32(ECI_RtcMaxBitrate)) + ";x-google-min-bitrate=" + std::to_string(g_cfg.get_uint32(ECI_RtcMinBitrate)) + ";x-google-start-bitrate=" + std::to_string(g_cfg.get_uint32(ECI_RtcStartBitrate)));

			}
			crtx_payload_des *rtx_video_payload = new crtx_payload_des(kRtxVideoPayloadType, kVideoPayloadType);
			video_track_desc->m_rtx_ptr = rtx_video_payload;
			video_track_desc->m_rtx_ssrc = c_rtc_ssrc_generator.generate_ssrc();// rtc_ssrc_info_ptr->m_rtx_video_ssrc;// c_rtc_ssrc_generator.generate_ssrc();
		}
		///////////////////////////////////////////


		_negotiate_play_capability(player_message, rtc_remote_sdp, &stream_desc, play_sub_relations);
		if (!play_sub_relations.size())
		{
			WARNING_EX_LOG("no play relations");
			return false;
		}
		{
			if (stream_desc.m_audio_track_desc_ptr)
			{
				delete stream_desc.m_audio_track_desc_ptr;
				stream_desc.m_audio_track_desc_ptr = NULL;
			}
			for (crtc_track_description* trak_ptr : stream_desc.m_video_track_descs)
			{
				delete trak_ptr;
			}
			stream_desc.m_video_track_descs.clear();
		}
		std::map<uint32_t, crtc_track_description*>::iterator it = play_sub_relations.begin();
		
		while (it != play_sub_relations.end()) {
			crtc_track_description* track_desc = it->second;
			
			// TODO: FIXME: we only support one audio track.
			if (track_desc->m_type == "audio" /*&& !stream_desc.m_audio_track_desc_ptr*/)
			{
				
				stream_desc.m_audio_track_desc_ptr = track_desc->copy();
			}

			if (track_desc->m_type == "video" /*&& stream_desc.m_video_track_descs.empty()*/) {
				stream_desc.m_video_track_descs.push_back(track_desc->copy());
			}
			++it;
		}

		_generate_play_local_sdp(media_stream_url, rtc_local_sdp, &stream_desc, rtc_remote_sdp.is_unified(), true);

		// datachannel -->
		const cmedia_desc & application_media =  rtc_remote_sdp.m_media_descs.back();

		if (application_media.m_type == "application")
		{
			//m_media_descs.push_back(cmedia_desc(media));
			rtc_local_sdp.m_media_descs.push_back(application_media);
			//rtc_local_sdp.m_media_descs.back().m_max_message_size = 10000;
			rtc_local_sdp.m_groups.push_back(std::to_string(rtc_local_sdp.m_groups.size()));
		}


		// All tracks default as inactive, so we must enable them.
	//	session->set_all_tracks_status(req->get_stream_url(), ruc->publish_, true);

		std::string local_pwd = c_rand.rand_str(32);
		std::string local_ufrag = "";
		// TODO: FIXME: Rename for a better name, it's not an username.
		std::string username = "";
		while (true)
		{
			local_ufrag = c_rand.rand_str(8);

			username = local_ufrag + ":" + rtc_remote_sdp.get_ice_ufrag();
			if (!g_transport_mgr.find_username(username))
			{
				break;
			}
		}

		rtc_local_sdp.set_ice_ufrag(local_ufrag);
		rtc_local_sdp.set_ice_pwd(local_pwd);
		rtc_local_sdp.set_fingerprint_algo("sha-256");
		rtc_local_sdp.set_fingerprint(g_dtls_certificate.get_fingerprint());
		crtc_room_master master;
		master.m_room_name = player_message.m_room_name;
		master.m_user_name = player_message.m_video_peer_id;
		master.m_cur_user = player_message.m_peer_id;
		crtc_transport * transport_ptr = new crtc_transport(master);
		// We allows to mock the eip of server.
		if (true)
		{
			//static  int32 udp_port = 4000;//_srs_config->get_rtc_server_listen();
			int32 tcp_port = 0;//_srs_config->get_rtc_server_tcp_listen();
			std::string protocol = "tcp";//_srs_config->get_rtc_server_protocol();
			std::string wan_ip = "0.0.0.0";
		   //std::set<std::string> candidates = {"192.168.1.175"};;// = discover_candidates(ruc);
			//for (std::set<std::string>::iterator it = candidates.begin(); it != candidates.end(); ++it) 
			for (std::set<std::string>::const_iterator iter = g_global_config.get_all_ips().begin(); iter != g_global_config.get_all_ips().end(); ++iter)
			{
				std::string hostname = *iter;/*"172.20.10.2";*/
				int32 uport = g_global_rtc_port.get_new_port();//;udp_port++;
				//parse_hostport(*it, hostname, uport);
				//int32 tport = tcp_port; 
				//parse_hostport(*it, hostname, tport);
//
				 if (player_message.m_rtc_protocol == "udp")
				{
				/*	rtc_local_sdp.add_candidate("udp", hostname, uport, "host");
				}
				else  if (player_message.m_rtc_protocol == "tcp")
				{
					rtc_local_sdp.add_candidate("tcp", hostname, uport, "host");
				}*/
					 cudp_socket * socket_ptr = new   cudp_socket(transport_ptr, wan_ip);
					 uport = socket_ptr->GetLocalPort();
					 rtc_local_sdp.add_candidate("udp", hostname, socket_ptr->GetLocalPort(), "host");
					 transport_ptr->insert_udp_socket(socket_ptr);
				 }
				 else if ("tcp" == player_message.m_rtc_protocol)
				 {
					 ctcp_server * socket_ptr = new   ctcp_server(transport_ptr, transport_ptr, wan_ip);
					 uport = socket_ptr->GetLocalPort();
					 rtc_local_sdp.add_candidate("tcp", hostname, socket_ptr->GetLocalPort(), "host");
					 transport_ptr->insert_tcp_server(socket_ptr);
				 }
				else
				{
					WARNING_EX_LOG("[protocol = %s][hostname = %s][uport = %u]", player_message.m_rtc_protocol.c_str(), hostname.c_str(), uport);
					/*m_local_sdp.add_candidate("udp", hostname, uport, "host");
					m_local_sdp.add_candidate("tcp", hostname, tport, "host");*/
				}
			}

			//std::vector<std::string> v = std::vector<std::string>(candidates.begin(), candidates.end());
			NORMAL_EX_LOG("RTC: Use candidates  = , protocol=%s", /*srs_join_vector_string(v, ", ").c_str(),*/ player_message.m_rtc_protocol.c_str());
		}

		// Setup the negotiate DTLS by config.
		rtc_local_sdp.m_session_negotiate = rtc_local_sdp.m_session_config;

		// Setup the negotiate DTLS role.
		if (rtc_remote_sdp.get_dtls_role() == "active")
		{
			rtc_local_sdp.m_session_negotiate.m_dtls_role = "passive";
		}
		else if (rtc_remote_sdp.get_dtls_role() == "passive")
		{
			rtc_local_sdp.m_session_negotiate.m_dtls_role = "active";
		}
		else if (rtc_remote_sdp.get_dtls_role() == "actpass")
		{
			rtc_local_sdp.m_session_negotiate.m_dtls_role = rtc_local_sdp.m_session_config.m_dtls_role;
		}
		else
		{
			// @see: https://tools.ietf.org/html/rfc4145#section-4.1
			// The default value of the setup attribute in an offer/answer exchange
			// is 'active' in the offer and 'passive' in the answer.
			rtc_local_sdp.m_session_negotiate.m_dtls_role = "passive";
		}
		//rtc_local_sdp.m_session_negotiate.m_dtls_role = "active";
		rtc_local_sdp.set_dtls_role(rtc_local_sdp.m_session_negotiate.m_dtls_role);

		//session->set_remote_sdp(ruc->remote_sdp_);
		// We must setup the local SDP, then initialize the session object.
		//session->set_local_sdp(local_sdp);
	//	session->set_state_as_waiting_stun();
		
		transport_ptr->create_players(play_sub_relations);
		transport_ptr->init( ERtcClientPlayer, rtc_remote_sdp, rtc_local_sdp, stream_desc);
		transport_ptr->set_state_as_waiting_stun();
		// Before session initialize, we must setup the local SDP.
		//if ((err = session->initialize(req, ruc->dtls_, ruc->srtp_, username)) != 0) 
		{
			//	return srs_error_wrap(err, "init");
		}

		// We allows username is optional, but it never empty here.
		//_srs_rtc_manager->add_with_name(username, session);
		g_transport_mgr.insert_username(username, transport_ptr);
		//g_transport_mgr.insert_stream_name(roomname + "/" + peerid, transport_ptr);
		g_transport_mgr.m_all_consumer_map[media_stream_url].insert(transport_ptr);
		ctransport_mgr::STREAM_URL_MAP::iterator iter =  g_transport_mgr.m_all_stream_url_map.find(media_stream_url);
		if (iter != g_transport_mgr.m_all_stream_url_map.end())
		{
			iter->second->register_consumer(transport_ptr);
			transport_ptr->register_producer(iter->second);
		}

		std::ostringstream    sdp;
		rtc_local_sdp.encode(sdp);


		local_sdp = sdp.str();



		//NORMAL_EX_LOG("sdp info = %s", sdp.str().c_str());
		return true;
	}
	bool capi_rtc_player::_negotiate_play_capability(const cclient_player_message & player_message, crtc_sdp& remote_sdp, crtc_source_description * stream_desc, std::map<uint32_t, crtc_track_description*>& sub_relations)
	{
		if (!stream_desc)
		{
			ERROR_EX_LOG("stream description is NULL");
			return false;
			//	return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "stream description is NULL");
		}

		bool nack_enabled = true;
		bool twcc_enabled = true;
		// TODO: FIME: Should check packetization-mode=1 also.
		bool has_42e01f = sdp_has_h264_profile(remote_sdp, "42e01f");

		// How many video descriptions we have parsed.
		int32 nn_any_video_parsed = 0;
		for (int i = 0; i < (int)remote_sdp.m_media_descs.size(); ++i) {
			const cmedia_desc& remote_media_desc = remote_sdp.m_media_descs.at(i);
			std::vector<cmedia_payload_type> rtx_payloads = remote_media_desc.find_media_with_encoding_name("rtx");
			//std::vector<cmedia_payload_type> ulpfec_payloads = remote_media_desc.find_media_with_encoding_name("ulpfec");
			
			if (remote_media_desc.is_video())
			{
				nn_any_video_parsed++;
			}

			// Whether feature enabled in remote extmap.
			int remote_twcc_id = 0;
			std::map<int32, std::string> extmaps = remote_media_desc.get_extmaps();
			if (true) {
				
				for (std::map<int32, std::string>::iterator it = extmaps.begin(); it != extmaps.end(); ++it) 
				{
					if (it->second == kTWCCExt) 
					{
						remote_twcc_id = it->first;
						//remote_twcc_id = get_rtp_header_extension_uri_type(kTWCCExt);
						break;
					}
				}
			}

			std::vector<crtc_track_description*> track_descs;
			cmedia_payload_type remote_payload(0);
			if (remote_media_desc.is_audio()) {
				// Update the ruc, which is about user specified configuration.
				//ruc->audio_before_video_ = !nn_any_video_parsed;

				// TODO: check opus format specific param
				std::vector<cmedia_payload_type> payloads = remote_media_desc.find_media_with_encoding_name("opus");
				if (payloads.empty()) 
				{
					WARNING_EX_LOG("no valid found opus payload type");
					return false;
					//return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "no valid found opus payload type");
				}

				remote_payload = payloads.at(0);
			   track_descs = _get_track_desc(stream_desc, "audio", "opus");
				//if (true)
				//{
				//	crtc_track_description track_desc;
				//	track_desc.set_direction("sendonly");
				//	track_desc.set_mid(remote_media_desc.m_mid);
				//	if (twcc_enabled && remote_twcc_id)
				//	{
				//		track_desc.add_rtp_extension_desc(remote_twcc_id, kTWCCExt);
				//	}

				//	// TODO: check opus format specific param
				//	std::vector<cmedia_payload_type> payloads = remote_media_desc.find_media_with_encoding_name("opus");
				//	if (payloads.empty())
				//	{
				//		ERROR_EX_LOG("no valid found opus payload type");
				//		return false;
				//	}

				//	for (int32 j = 0; j < (int32)payloads.size(); j++)
				//	{
				//		const cmedia_payload_type& payload = payloads.at(j);

				//		// if the payload is opus, and the encoding_param_ is channel
				//		caudio_payload* audio_payload = new caudio_payload(payload.m_payload_type, payload.m_encoding_name, payload.m_clock_rate, ::atol(payload.m_encoding_param.c_str()));
				//		audio_payload->set_opus_param_desc(payload.m_format_specific_param);
				//		// AudioPayload* audio_payload = new SrsAudioPayload(payload.m_payload_type, payload.m_encoding_name, payload.m_clock_rate, ::atol(payload.m_encoding_param.c_str()));
				//		//audio_payload->set_opus_param_desc(payload.format_specific_param_);

				//		// TODO: FIXME: Only support some transport algorithms.
				//		for (int32 k = 0; k < (int32)payload.m_rtcp_fb.size(); ++k)
				//		{
				//			const std::string& rtcp_fb = payload.m_rtcp_fb.at(k);

				//			// 掉包重传的协议
				//			if (nack_enabled)
				//			{
				//				if (rtcp_fb == "nack" || rtcp_fb == "nack pli")
				//				{
				//					audio_payload->m_rtcp_fbs.push_back(rtcp_fb);
				//				}
				//			}
				//			// 网络带宽评估的协议
				//			if (twcc_enabled && remote_twcc_id)
				//			{
				//				if (rtcp_fb == "transport-cc")
				//				{
				//					audio_payload->m_rtcp_fbs.push_back(rtcp_fb);
				//				}
				//			}
				//		}

				//		track_desc.m_type = "audio";
				//		track_desc.set_codec_payload(audio_payload);
				//	}

				//	

				//	// TODO: FIXME: use one parse payload from sdp.
				//	track_desc.create_auxiliary_payload(remote_media_desc.find_media_with_encoding_name("red"));
				//	track_desc.create_auxiliary_payload(remote_media_desc.find_media_with_encoding_name("rtx"));
				//	track_desc.create_auxiliary_payload(remote_media_desc.find_media_with_encoding_name("ulpfec"));
				//	track_descs.push_back(track_desc.copy());
				//}
			}
			else if (remote_media_desc.is_video() && player_message.m_codec == "AV1")
			{
				//std::vector<SrsMediaPayloadType> payloads = remote_media_desc.find_media_with_encoding_name("AV1");
				std::vector<cmedia_payload_type> payloads = remote_media_desc.find_media_with_encoding_name("AV1");

				if (payloads.empty()) {
					// Be compatible with the Chrome M96, still check the AV1X encoding name
					// @see https://bugs.chromium.org/p/webrtc/issues/detail?id=13166
					payloads = remote_media_desc.find_media_with_encoding_name("AV1X");
				}
				if (payloads.empty())
				{
					WARNING_EX_LOG("no valid found AV1 and AV1X payload type");
					return false;
					//return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "no found valid AV1 payload type");
				}

				remote_payload = payloads.at(0);
				track_descs = _get_track_desc(stream_desc, "video", "AV1");
				//track_descs = source->get_track_desc("video", "AV1");
				//if (track_descs.empty())
				//{
				//	// Be compatible with the Chrome M96, still check the AV1X encoding name
				//	// @see https://bugs.chromium.org/p/webrtc/issues/detail?id=13166
				//	track_descs = source->get_track_desc("video", "AV1X");
				//}
			}
			else if (remote_media_desc.is_video()) 
			{
				// TODO: check opus format specific param
				std::vector<cmedia_payload_type> payloads = remote_media_desc.find_media_with_encoding_name("H264");
				
				if (payloads.empty()) 
				{
					WARNING_EX_LOG("no valid found h264 payload type");
					return false;
					//return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "no valid found h264 payload type");
				}

				remote_payload = payloads.at(0);
				for (int j = 0; j < (int)payloads.size(); j++) {
					const cmedia_payload_type& payload = payloads.at(j);

					// If exists 42e01f profile, choose it; otherwise, use the first payload.
					// TODO: FIME: Should check packetization-mode=1 also.
					if (!has_42e01f || sdp_has_h264_profile(payload, "42e01f")) {
						remote_payload = payload;
						break;
					}
				}

				track_descs = _get_track_desc(stream_desc, "video", "H264");
			}

			for (int j = 0; j < (int)track_descs.size(); ++j) {
				crtc_track_description* track = track_descs.at(j)->copy();

				// We should clear the extmaps of source(publisher).
				// @see https://github.com/ossrs/srs/issues/2370
				track->m_extmaps.clear();

				// We should clear the rtcp_fbs of source(publisher).
				// @see https://github.com/ossrs/srs/issues/2371
				track->m_media_ptr->m_rtcp_fbs.clear();

				// Use remote/source/offer PayloadType.
				track->m_media_ptr->m_pt_of_publisher = track->m_media_ptr->m_pt;
				track->m_media_ptr->m_pt = remote_payload.m_payload_type;

				std::vector<cmedia_payload_type> red_pts = remote_media_desc.find_media_with_encoding_name("red");
				if (!red_pts.empty() && track->m_red_ptr) {
					cmedia_payload_type red_pt = red_pts.at(0);

					track->m_red_ptr->m_pt_of_publisher = track->m_red_ptr->m_pt;
					track->m_red_ptr->m_pt = red_pt.m_payload_type;
				}

				track->m_mid = remote_media_desc.m_mid;
				uint32 publish_ssrc = track->m_ssrc;

				std::vector<std::string> rtcp_fb;
				remote_payload.m_rtcp_fb.swap(rtcp_fb);
				for (int j = 0; j < (int)rtcp_fb.size(); j++) {
					if (nack_enabled) {
						if (rtcp_fb.at(j) == "nack" || rtcp_fb.at(j) == "nack pli") {
							track->m_media_ptr->m_rtcp_fbs.push_back(rtcp_fb.at(j));
						}
					}
					if (twcc_enabled && remote_twcc_id) {
						if (rtcp_fb.at(j) == "transport-cc") {
							track->m_media_ptr->m_rtcp_fbs.push_back(rtcp_fb.at(j));
						}
						track->add_rtp_extension_desc(remote_twcc_id, kTWCCExt);
					}
				}

				//track->m_ssrc = c_rtc_ssrc_generator.generate_ssrc();
				if (true)
				{
					for (std::map<int32, std::string>::iterator it = extmaps.begin(); it != extmaps.end(); ++it)
					{

						if (it->second == RtpExtension_kMidUri)
						{//get_rtp_header_extension_uri_type(RtpExtension_kMidUri);
							track->add_rtp_extension_desc( it->first , RtpExtension_kMidUri);
						}
						else if (it->second == RtpExtension_kAbsSendTimeUri) 
						{
							track->add_rtp_extension_desc(it->first, RtpExtension_kAbsSendTimeUri);
						}
					}
				}
				if (track->m_type == "audio")
				{
					 
					for (std::map<int32, std::string>::iterator iter = extmaps.begin(); iter != extmaps.end(); ++iter)
					{
						if (iter->second == RtpExtension_kAudioLevelUri)
						{
							track->add_rtp_extension_desc(iter->first, RtpExtension_kAudioLevelUri);
							break;
						}
					} 
				}
				else if (track->m_type == "video")
				{
					for (std::map<int32, std::string>::iterator iter = extmaps.begin(); iter != extmaps.end(); ++iter)
					{
						if (iter->second == RtpExtension_kTimestampOffsetUri)
						{
							track->add_rtp_extension_desc(iter->first, RtpExtension_kTimestampOffsetUri);

						}
						else if (iter->second == RtpExtension_kVideoRotationUri)
						{
							track->add_rtp_extension_desc(iter->first, RtpExtension_kVideoRotationUri);

						}
						else if (iter->second == RtpExtension_kRidUri)
						{
							track->add_rtp_extension_desc(iter->first, RtpExtension_kRidUri);

						}
						else if (iter->second == RtpExtension_kRepairedRidUri)
						{
							track->add_rtp_extension_desc(iter->first, RtpExtension_kRepairedRidUri);

						}
						else if (iter->second == RtpExtension_kRepairedRidUri)
						{
							track->add_rtp_extension_desc(iter->first, RtpExtension_kRepairedRidUri);

						}
						else if (iter->second == RtpExtension_kFrameMarking07Uri)
						{
							track->add_rtp_extension_desc(iter->first, RtpExtension_kFrameMarking07Uri);
						}
						else if (iter->second == RtpExtension_kFrameMarkingUri)
						{
							track->add_rtp_extension_desc(iter->first, RtpExtension_kFrameMarkingUri);
						}

					}
			 		
					if (track->m_rtx_ptr)
					{
						for (size_t rtx_i = 0; rtx_i < rtx_payloads.size(); ++rtx_i)
						{
							if (remote_payload.m_rtx == rtx_payloads[rtx_i].m_payload_type)
							{
								((crtx_payload_des*)(track->m_rtx_ptr))->m_pt = rtx_payloads[rtx_i].m_payload_type;
								((crtx_payload_des*)(track->m_rtx_ptr))->m_apt = remote_payload.m_payload_type;// ((crtx_payload_des*)&rtx_payloads[rtx_i])->m_apt;
								break;
							}
						}
					}
					/* if (!ulpfec_payloads.empty())
					{
						 track->m_fec_ssrc = c_rtc_ssrc_generator.generate_ssrc();
						 track->m_ulpfec_ptr = new ccodec_payload(ulpfec_payloads[0].m_payload_type, "ulpfec", ulpfec_payloads[0].m_clock_rate);
					} */
				}
				// TODO: FIXME: set audio_payload rtcp_fbs_,
				// according by whether downlink is support transport algorithms.
				// TODO: FIXME: if we support downlink RTX, MUST assign rtx_ssrc_, rtx_pt, rtx_apt
				// not support rtx
				/*if (true)
				{
					delete track->m_rtx_ptr;
					track->m_rtx_ssrc = 0;
				}
*/
				track->set_direction("sendonly");
				sub_relations.insert(std::make_pair(publish_ssrc, track));
			}
		}

		//for (int32 i = 0; i < (int32)remote_sdp.m_media_descs.size(); ++i)
		//{
		//	const cmedia_desc& remote_media_desc = remote_sdp.m_media_descs.at(i);

		//	if (remote_media_desc.is_video())
		//	{
		//		nn_any_video_parsed++;
		//	}

		//	/*	SrsRtcTrackDescription* track_desc = new SrsRtcTrackDescription();
		//		SrsAutoFree(SrsRtcTrackDescription, track_desc);*/
		//	crtc_track_description track_desc;
		//	track_desc.set_direction("sendonly");
		//	track_desc.set_mid(remote_media_desc.m_mid);
		//	// Whether feature enabled in remote extmap.
		//	int32 remote_twcc_id = 0;
		//	if (true)
		//	{
		//		std::map<int32, std::string> extmaps = remote_media_desc.get_extmaps();
		//		for (std::map<int32, std::string>::iterator it = extmaps.begin(); it != extmaps.end(); ++it)
		//		{
		//			if (it->second == kTWCCExt)
		//			{
		//				remote_twcc_id = it->first;
		//				break;
		//			}
		//		}
		//	}

		//	if (twcc_enabled && remote_twcc_id)
		//	{
		//		track_desc.add_rtp_extension_desc(remote_twcc_id, kTWCCExt);
		//	}

		//	if (remote_media_desc.is_audio())
		//	{
		//		// Update the ruc, which is about user specified configuration.
		//		//ruc->audio_before_video_ = !nn_any_video_parsed;

		//		// TODO: check opus format specific param
		//		std::vector<cmedia_payload_type> payloads = remote_media_desc.find_media_with_encoding_name("opus");
		//		if (payloads.empty())
		//		{
		//			ERROR_EX_LOG("no valid found opus payload type");
		//			return false;
		//		}

		//		for (int32 j = 0; j < (int32)payloads.size(); j++)
		//		{
		//			const cmedia_payload_type& payload = payloads.at(j);

		//			// if the payload is opus, and the encoding_param_ is channel
		//			caudio_payload* audio_payload = new caudio_payload(payload.m_payload_type, payload.m_encoding_name, payload.m_clock_rate, ::atol(payload.m_encoding_param.c_str()));
		//			audio_payload->set_opus_param_desc(payload.m_format_specific_param);
		//			// AudioPayload* audio_payload = new SrsAudioPayload(payload.m_payload_type, payload.m_encoding_name, payload.m_clock_rate, ::atol(payload.m_encoding_param.c_str()));
		//			//audio_payload->set_opus_param_desc(payload.format_specific_param_);

		//			// TODO: FIXME: Only support some transport algorithms.
		//			for (int32 k = 0; k < (int32)payload.m_rtcp_fb.size(); ++k)
		//			{
		//				const std::string& rtcp_fb = payload.m_rtcp_fb.at(k);

		//				// 掉包重传的协议
		//				if (nack_enabled)
		//				{
		//					if (rtcp_fb == "nack" || rtcp_fb == "nack pli")
		//					{
		//						audio_payload->m_rtcp_fbs.push_back(rtcp_fb);
		//					}
		//				}
		//				// 网络带宽评估的协议
		//				if (twcc_enabled && remote_twcc_id)
		//				{
		//					if (rtcp_fb == "transport-cc")
		//					{
		//						audio_payload->m_rtcp_fbs.push_back(rtcp_fb);
		//					}
		//				}
		//			}

		//			track_desc.m_type = "audio";
		//			track_desc.set_codec_payload(audio_payload);
		//			// Only choose one match opus codec.
		//			break;
		//		}
		//	}
		//	//else if (remote_media_desc.is_video() && ruc->codec_ == "av1") {
		//	//	std::vector<SrsMediaPayloadType> payloads = remote_media_desc.find_media_with_encoding_name("AV1");
		//	//	if (payloads.empty()) {
		//	//		// Be compatible with the Chrome M96, still check the AV1X encoding name
		//	//		// @see https://bugs.chromium.org/p/webrtc/issues/detail?id=13166
		//	//		payloads = remote_media_desc.find_media_with_encoding_name("AV1X");
		//	//	}
		//	//	if (payloads.empty()) {
		//	//		return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "no found valid AV1 payload type");
		//	//	}

		//	//	for (int j = 0; j < (int)payloads.size(); j++) {
		//	//		const SrsMediaPayloadType& payload = payloads.at(j);

		//	//		// Generate video payload for av1.
		//	//		SrsVideoPayload* video_payload = new SrsVideoPayload(payload.payload_type_, payload.encoding_name_, payload.clock_rate_);

		//	//		// TODO: FIXME: Only support some transport algorithms.
		//	//		for (int k = 0; k < (int)payload.rtcp_fb_.size(); ++k) {
		//	//			const string& rtcp_fb = payload.rtcp_fb_.at(k);

		//	//			if (nack_enabled) {
		//	//				if (rtcp_fb == "nack" || rtcp_fb == "nack pli") {
		//	//					video_payload->rtcp_fbs_.push_back(rtcp_fb);
		//	//				}
		//	//			}
		//	//			if (twcc_enabled && remote_twcc_id) {
		//	//				if (rtcp_fb == "transport-cc") {
		//	//					video_payload->rtcp_fbs_.push_back(rtcp_fb);
		//	//				}
		//	//			}
		//	//		}

		//	//		track_desc->type_ = "video";
		//	//		track_desc->set_codec_payload((SrsCodecPayload*)video_payload);
		//	//		break;
		//	//	}
		//	//}
		//	else if (remote_media_desc.is_video())
		//	{
		//		std::vector<cmedia_payload_type> payloads = remote_media_desc.find_media_with_encoding_name("H264");
		//		if (payloads.empty())
		//		{
		//			ERROR_EX_LOG("no found valid H.264 payload type");
		//			return false;
		//			//return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "no found valid H.264 payload type");
		//		}

		//		std::deque<cmedia_payload_type> backup_payloads;
		//		for (int32 j = 0; j < (int32)payloads.size(); j++)
		//		{
		//			const cmedia_payload_type& payload = payloads.at(j);

		//			if (payload.m_format_specific_param.empty())
		//			{
		//				backup_payloads.push_front(payload);
		//				continue;
		//			}
		//			ch264_specific_param h264_param;
		//			if ((parse_h264_fmtp(payload.m_format_specific_param, h264_param)) != 0)
		//			{

		//				//srs_error_reset(err); 
		//				WARNING_EX_LOG(" h264 specific param  parse h264 fmtp failed !!!");
		//				continue;
		//			}

		//			// If not exists 42e01f, we pick up any profile such as 42001f.
		//			bool profile_matched = (!has_42e01f || h264_param.m_profile_level_id == "42e01f");

		//			// Try to pick the "best match" H.264 payload type.
		//			if (profile_matched && h264_param.m_packetization_mode == "1" && h264_param.m_level_asymmerty_allow == "1")
		//			{
		//				// if the playload is opus, and the encoding_param_ is channel

		//				cvideo_payload * video_payload = new cvideo_payload(payload.m_payload_type, payload.m_encoding_name, payload.m_clock_rate);
		//				//SrsVideoPayload* video_payload = new SrsVideoPayload(payload.payload_type_, payload.encoding_name_, payload.clock_rate_);
		//				video_payload->set_h264_param_desc(payload.m_format_specific_param);

		//				// Set the codec parameter for H.264, to make Unity happy.
		//				video_payload->m_h264_param = h264_param;

		//				// TODO: FIXME: Only support some transport algorithms.
		//				for (int32 k = 0; k < (int32)payload.m_rtcp_fb.size(); ++k)
		//				{
		//					const std::string& rtcp_fb = payload.m_rtcp_fb.at(k);

		//					if (nack_enabled)
		//					{
		//						if (rtcp_fb == "nack" || rtcp_fb == "nack pli")
		//						{
		//							video_payload->m_rtcp_fbs.push_back(rtcp_fb);
		//						}
		//					}
		//					if (twcc_enabled && remote_twcc_id)
		//					{
		//						if (rtcp_fb == "transport-cc")
		//						{
		//							video_payload->m_rtcp_fbs.push_back(rtcp_fb);
		//						}
		//					}
		//				}

		//				track_desc.m_type = "video";
		//				track_desc.set_codec_payload(video_payload);
		//				// Only choose first match H.264 payload type.
		//				break;
		//			}

		//			backup_payloads.push_back(payload);
		//		}

		//		// Try my best to pick at least one media payload type.
		//		if (!track_desc.m_media_ptr && !backup_payloads.empty())
		//		{
		//			const cmedia_payload_type& payload = backup_payloads.front();

		//			// if the playload is opus, and the encoding_param_ is channel
		//			cvideo_payload *  video_payload = new cvideo_payload(payload.m_payload_type, payload.m_encoding_name, payload.m_clock_rate);

		//			//SrsVideoPayload* video_payload = new SrsVideoPayload(payload.payload_type_, payload.encoding_name_, payload.clock_rate_);

		//			// TODO: FIXME: Only support some transport algorithms.
		//			for (int k = 0; k < (int)payload.m_rtcp_fb.size(); ++k)
		//			{
		//				const std::string& rtcp_fb = payload.m_rtcp_fb.at(k);

		//				if (nack_enabled)
		//				{
		//					if (rtcp_fb == "nack" || rtcp_fb == "nack pli")
		//					{
		//						video_payload->m_rtcp_fbs.push_back(rtcp_fb);
		//					}
		//				}

		//				if (twcc_enabled && remote_twcc_id)
		//				{
		//					if (rtcp_fb == "transport-cc")
		//					{
		//						video_payload->m_rtcp_fbs.push_back(rtcp_fb);
		//					}
		//				}
		//			}

		//			track_desc.m_type = "video";
		//			track_desc.set_codec_payload(video_payload);
		//			NORMAL_EX_LOG("choose backup H.264 payload type=%d", payload.m_payload_type);
		//		}

		//		// TODO: FIXME: Support RRTR?
		//		//local_media_desc.payload_types_.back().rtcp_fb_.push_back("rrtr");
		//	}

		//	// Error if track desc is invalid, that is failed to match SDP, for example, we require H264 but no H264 found.
		//	if (track_desc.m_type.empty() || !track_desc.m_media_ptr)
		//	{
		//		ERROR_EX_LOG("no match for track=%s, mid=%s, tracker=%s", remote_media_desc.m_type.c_str(), remote_media_desc.m_mid.c_str(), remote_media_desc.m_msid_tracker.c_str());
		//		return false;
		//		//return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "no match for track=%s, mid=%s, tracker=%s", remote_media_desc.type_.c_str(), remote_media_desc.mid_.c_str(), remote_media_desc.msid_tracker_.c_str());
		//	}

		//	// TODO: FIXME: use one parse payload from sdp.
		//	track_desc.create_auxiliary_payload(remote_media_desc.find_media_with_encoding_name("red"));
		//	track_desc.create_auxiliary_payload(remote_media_desc.find_media_with_encoding_name("rtx"));
		//	track_desc.create_auxiliary_payload(remote_media_desc.find_media_with_encoding_name("ulpfec"));

		//	std::string track_id;
		//	for (int32 j = 0; j < (int32)remote_media_desc.m_ssrc_infos.size(); ++j)
		//	{
		//		const cssrc_info& ssrc_info = remote_media_desc.m_ssrc_infos.at(j);

		//		// ssrc have same track id, will be description in the same track description.
		//		if (track_id != ssrc_info.m_msid_tracker)
		//		{
		//			//SrsRtcTrackDescription* track_desc_copy = track_desc->copy();
		//			crtc_track_description * track_desc_copy = track_desc.copy();
		//			track_desc_copy->m_ssrc = ssrc_info.m_ssrc;
		//			track_desc_copy->m_id = ssrc_info.m_msid_tracker;
		//			track_desc_copy->m_msid = ssrc_info.m_msid;

		//			if (remote_media_desc.is_audio() && !stream_desc->m_audio_track_desc_ptr)
		//			{
		//				stream_desc->m_audio_track_desc_ptr = track_desc_copy;
		//			}
		//			else if (remote_media_desc.is_video())
		//			{
		//				stream_desc->m_video_track_descs.push_back(track_desc_copy);
		//			}
		//			else
		//			{
		//				delete track_desc_copy;
		//				track_desc_copy = NULL;
		//				NORMAL_EX_LOG("not find track_id  delete  track_desc_copy object [track_id = %s][ssrc_info.m_msid_tracker = %s]", track_id.c_str(), ssrc_info.m_msid_tracker.c_str());
		//				//	srs_freep(track_desc_copy);
		//			}
		//		}
		//		track_id = ssrc_info.m_msid_tracker;
		//	}

		//	// set track fec_ssrc and rtx_ssrc
		//	for (int32 j = 0; j < (int32)remote_media_desc.m_ssrc_groups.size(); ++j)
		//	{
		//		const cssrc_group& ssrc_group = remote_media_desc.m_ssrc_groups.at(j);
		//		crtc_track_description *track_desc = stream_desc->find_track_description_by_ssrc(ssrc_group.m_ssrcs[0]);
		//		//SrsRtcTrackDescription* track_desc = stream_desc->find_track_description_by_ssrc(ssrc_group.ssrcs_[0]);
		//		if (!track_desc)
		//		{
		//			continue;
		//		}

		//		if (ssrc_group.m_semantic == "FID")
		//		{
		//			track_desc->set_rtx_ssrc(ssrc_group.m_ssrcs[1]);
		//		}
		//		else if (ssrc_group.m_semantic == "FEC")
		//		{
		//			track_desc->set_fec_ssrc(ssrc_group.m_ssrcs[1]);
		//		}
		//	}
		//}
		return true;
	}
	bool capi_rtc_player::_generate_play_local_sdp(const std::string & media_stream_url, crtc_sdp & local_sdp, crtc_source_description * stream_desc, bool unified_plan, bool audio_before_video)
	{
		int32 err = 0;
		if (!stream_desc)
		{
			ERROR_EX_LOG("stream description is NULL");
			return false;
			//return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "stream description is NULL");
		}

		local_sdp.m_version = "0";

		local_sdp.m_username = MEDIA_RTC_SERVER;
		local_sdp.m_session_id = digit2str_dec((int64_t)this);
		local_sdp.m_session_version = "2";
		local_sdp.m_nettype = "IN";
		local_sdp.m_addrtype = "IP4";
		local_sdp.m_unicast_address = "0.0.0.0";

		local_sdp.m_session_name = "RTCPlaySession";

		local_sdp.m_msid_semantic = "WMS";

		// TODO@chensong 2023-03-08   default -> video stream address url  
		// url [roomname + username]
		//std::string stream_id = roomname + "/" + peerid; //"test";;// req->app + "/" + req->stream;

		local_sdp.m_msids.push_back(media_stream_url);

		local_sdp.m_group_policy = "BUNDLE";

		std::string cname = c_rand.rand_str(16);
		if (audio_before_video)
		{
			if (!_generate_play_local_sdp_for_audio(local_sdp, stream_desc, cname))
			{
				ERROR_EX_LOG("_generate_play_local_sdp_for_audio failed !!!");
				return false;
			}
			if (!_generate_play_local_sdp_for_video(local_sdp, stream_desc, unified_plan, cname))
			{
				ERROR_EX_LOG("_generate_play_local_sdp_for_video failed !!!");
				return false;
			}
		}
		else
		{
			if (!_generate_play_local_sdp_for_video(local_sdp, stream_desc, unified_plan, cname))
			{
				ERROR_EX_LOG("_generate_play_local_sdp_for_video failed !!!");
				return false;
			}
			if (!_generate_play_local_sdp_for_audio(local_sdp, stream_desc, cname))
			{
				ERROR_EX_LOG("_generate_play_local_sdp_for_audio failed !!!");
				return false;
				//return srs_error_wrap(err, "audio");
			}
		}

		return true;
		return true;
	}
	bool capi_rtc_player::_generate_play_local_sdp_for_audio(crtc_sdp & local_sdp, crtc_source_description * stream_desc, std::string cname)
	{
		// generate audio media desc 
		if (stream_desc->m_audio_track_desc_ptr)
		{
			crtc_track_description* audio_track = stream_desc->m_audio_track_desc_ptr;

			local_sdp.m_media_descs.push_back(cmedia_desc("audio"));
			cmedia_desc& local_media_desc = local_sdp.m_media_descs.back();

			local_media_desc.m_port = 9;
			local_media_desc.m_protos = "UDP/TLS/RTP/SAVPF";
			local_media_desc.m_rtcp_mux = true;
			local_media_desc.m_rtcp_rsize = true;

			local_media_desc.m_extmaps = audio_track->m_extmaps;

			local_media_desc.m_mid = audio_track->m_mid;
			local_media_desc.m_msid = audio_track->m_msid;
			local_media_desc.m_msid_tracker = audio_track->m_id;
			local_sdp.m_groups.push_back(local_media_desc.m_mid);

			 
			
			if (audio_track->m_direction == "recvonly")
			{
				local_media_desc.m_recvonly = true;
			}
			else if (audio_track->m_direction == "sendonly")
			{
				local_media_desc.m_sendonly = true;
			}
			else if (audio_track->m_direction == "sendrecv")
			{
				local_media_desc.m_sendrecv = true;
			}
			else if (audio_track->m_direction == "inactive")
			{
				local_media_desc.m_inactive = true;
			}
			if (audio_track->m_red_ptr) {
				cred_paylod* red_payload = (cred_paylod*)audio_track->m_red_ptr;
				local_media_desc.m_payload_types .push_back(red_payload->generate_media_payload_type());
			}
			caudio_payload* payload = dynamic_cast<caudio_payload*>(audio_track->m_media_ptr);
			local_media_desc.m_payload_types.push_back(payload->generate_media_payload_type());

			//TODO: FIXME: add red, rtx, ulpfec..., payload_types_.
		//local_media_desc.payload_types_.push_back(payload->generate_media_payload_type());

			local_media_desc.m_ssrc_infos.push_back(cssrc_info(audio_track->m_ssrc, cname, audio_track->m_msid, audio_track->m_id));

			if (audio_track->m_rtx_ptr) {
				std::vector<uint32_t> group_ssrcs;
				group_ssrcs.push_back(audio_track->m_ssrc);
				group_ssrcs.push_back(audio_track->m_rtx_ssrc);

				local_media_desc.m_ssrc_groups.push_back(cssrc_group("FID", group_ssrcs));
				local_media_desc.m_ssrc_infos.push_back(cssrc_info(audio_track->m_rtx_ssrc, cname, audio_track->m_msid, audio_track->m_id));
			}

			if (audio_track->m_ulpfec_ptr) 
			{
				std::vector<uint32_t> group_ssrcs;
				group_ssrcs.push_back(audio_track->m_ssrc);
				group_ssrcs.push_back(audio_track->m_fec_ssrc);
				local_media_desc.m_ssrc_groups.push_back(cssrc_group("FEC", group_ssrcs));

				local_media_desc.m_ssrc_infos.push_back(cssrc_info(audio_track->m_fec_ssrc, cname, audio_track->m_msid, audio_track->m_id));
			}
		}
		return true;
	}


	void video_track_generate_play_offer(crtc_track_description* track, std::string mid, crtc_sdp& local_sdp)
	{
		local_sdp.m_media_descs.push_back(cmedia_desc("video"));
		cmedia_desc& local_media_desc = local_sdp.m_media_descs.back();

		local_media_desc.m_port = 9;
		local_media_desc.m_protos = "UDP/TLS/RTP/SAVPF";
		local_media_desc.m_rtcp_mux = true;
		local_media_desc.m_rtcp_rsize = true;
		local_media_desc.m_msid = "";
		local_media_desc.m_msid_tracker = "";
		local_media_desc.m_extmaps = track->m_extmaps;

		// If mid not duplicated, use mid_ of track. Otherwise, use transformed mid.
		if (true) {
			bool mid_duplicated = false;
			for (int i = 0; i < (int)local_sdp.m_groups.size(); ++i) {
				std::string& existed_mid = local_sdp.m_groups.at(i);
				if (existed_mid == track->m_mid) {
					mid_duplicated = true;
					break;
				}
			}
			if (mid_duplicated) {
				local_media_desc.m_mid = mid;
			}
			else {
				local_media_desc.m_mid = track->m_mid;
			}
			local_sdp.m_groups.push_back(local_media_desc.m_mid);
		}

		if (track->m_direction == "recvonly") {
			local_media_desc.m_recvonly = true;
		}
		else if (track->m_direction == "sendonly") {
			local_media_desc.m_sendonly = true;
		}
		else if (track->m_direction == "sendrecv") {
			local_media_desc.m_sendrecv = true;
		}
		else if (track->m_direction == "inactive") {
			local_media_desc.m_inactive = true;
		}

		cvideo_payload* payload = (cvideo_payload*)track->m_media_ptr;

		local_media_desc.m_payload_types.push_back(payload->generate_media_payload_type());

		if (track->m_red_ptr) {
			cred_paylod* red_payload = (cred_paylod*)track->m_red_ptr;
			local_media_desc.m_payload_types.push_back(red_payload->generate_media_payload_type());
		}
		if (track->m_rtx_ptr) {
			crtx_payload_des* rtx_payload = (crtx_payload_des*)track->m_rtx_ptr;
			local_media_desc.m_payload_types.push_back(rtx_payload->generate_media_payload_type());
		}
		if (track->m_ulpfec_ptr) {
			ccodec_payload* ulpfec_payload = (ccodec_payload*)track->m_ulpfec_ptr;
			local_media_desc.m_payload_types.push_back(ulpfec_payload->generate_media_payload_type());
		}
	}


	bool capi_rtc_player::_generate_play_local_sdp_for_video(crtc_sdp & local_sdp, crtc_source_description * stream_desc, bool unified_plan, std::string cname)
	{
		for (int i = 0; i < (int)stream_desc->m_video_track_descs.size(); ++i) {
			crtc_track_description* track = stream_desc->m_video_track_descs[i];

			if (!unified_plan) {
				// for plan b, we only add one m= for video track.
				if (i == 0) 
				{
					video_track_generate_play_offer(track, "video-" + std::to_string(i), local_sdp);
				}
			}
			else {
				// unified plan SDP, generate a m= for each video track.
				video_track_generate_play_offer(track, "video-" + std::to_string(i), local_sdp);
			}

			cmedia_desc& local_media_desc = local_sdp.m_media_descs.back();
			local_media_desc.m_msid = track->m_msid;
			local_media_desc.m_msid_tracker = track->m_id;
			local_media_desc.m_ssrc_infos.push_back(cssrc_info(track->m_ssrc, cname, track->m_msid, track->m_id));

			if (track->m_rtx_ptr && track->m_rtx_ssrc)
			{
				std::vector<uint32_t> group_ssrcs;
				group_ssrcs.push_back(track->m_ssrc);
				group_ssrcs.push_back(track->m_rtx_ssrc);

				local_media_desc.m_ssrc_groups.push_back(cssrc_group("FID", group_ssrcs));
				local_media_desc.m_ssrc_infos.push_back(cssrc_info(track->m_rtx_ssrc, cname, track->m_msid, track->m_id));
			}

			if (track->m_ulpfec_ptr && track->m_fec_ssrc) {
				std::vector<uint32_t> group_ssrcs;
				group_ssrcs.push_back(track->m_ssrc);
				group_ssrcs.push_back(track->m_fec_ssrc);
				local_media_desc.m_ssrc_groups.push_back(cssrc_group("FEC", group_ssrcs));

				local_media_desc.m_ssrc_infos.push_back(cssrc_info(track->m_fec_ssrc, cname, track->m_msid, track->m_id));
			}
		}
		return true;
	}
	bool capi_rtc_player::_create_player(std::map<uint32, crtc_track_description*> sub_relations)
	{
		return true;
	}
	std::vector<crtc_track_description*> capi_rtc_player::_get_track_desc(crtc_source_description * stream_desc, std::string type, std::string media_name)
	{
		std::vector<crtc_track_description*> track_descs;
		if (!stream_desc) {
			return track_descs;
		}

		if (type == "audio") {
			if (!stream_desc->m_audio_track_desc_ptr) {
				return track_descs;
			}
			if (stream_desc->m_audio_track_desc_ptr->m_media_ptr->m_name == media_name) {
				track_descs.push_back(stream_desc->m_audio_track_desc_ptr);
			}
		}

		if (type == "video") {
			std::vector<crtc_track_description*>::iterator it = stream_desc->m_video_track_descs.begin();
			while (it != stream_desc->m_video_track_descs.end()) {
				track_descs.push_back(*it);
				++it;
			}
		}

		return track_descs;
	}
}