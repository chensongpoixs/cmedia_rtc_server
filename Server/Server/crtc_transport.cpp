/***********************************************************************************************
created: 		2023-02-03

author:			chensong

purpose:		crtc_transport

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
#include "crtc_transport.h"
#include "clog.h"
#include "ccrypto_random.h"
#include "ctransport_util.h"
#include "crtc_track_description.h"
#include "ccfg.h"
#include "cstr2digit.h"
#include "cdigit2str.h"
#include "crandom.h"
#include "cdtls_certificate.h"
#include <set>
#include "csocket_util.h"
#include  "ctransport_mgr.h"
#include "crtc_stun_packet.h"
#include "cdtls_session.h"
#include "crtp_rtcp.h"
#include "cstr2digit.h"
#include "H264.hpp"
#include "crtp_header_extensions.h"
#include "crtc_producer.h"
#include "cuv_ip.h"

#include "cglobal_rtc_config.h"
#include "FeedbackPsAfb.hpp"
#include "FeedbackPsRemb.hpp"
#include <unordered_map>
#include "cglobal_rtc_port.h"
namespace chen {

	static const char * wan_ip = "0.0.0.0";
	crtc_transport::~crtc_transport()
	{
		int32  count = 34;
		NORMAL_EX_LOG("%" PRIu32 "", count);
	}
	bool crtc_transport::init(ERtcClientType rtc_client_type, const crtc_sdp & remote_sdp, const crtc_sdp & local_sdp, const crtc_source_description& stream_desc)
	{
		m_rtc_client_type = rtc_client_type;
		m_remote_sdp = remote_sdp;
		m_local_sdp = local_sdp;
		



		 
		for (size_t m_i = 0; m_i < m_remote_sdp.m_media_descs.size(); ++m_i)
		{
			for (std::map<int32, std::string>::const_iterator it = m_remote_sdp.m_media_descs[m_i].get_extmaps().begin(); it != m_remote_sdp.m_media_descs[m_i].get_extmaps().end(); ++it)
			{
				if (it->second == kTWCCExt)
				{  
					m_rtp_header_extension_ids.transportWideCc01 = it->first;
				}
				else if (it->second == RtpExtension_kRidUri)
				{
					m_rtp_header_extension_ids.rid = it->first;
				}
				else if (it->second == RtpExtension_kRepairedRidUri)
				{
					m_rtp_header_extension_ids.rrid = it->first;
				}
				else if (it->second == RtpExtension_kMidUri)
				{
					m_rtp_header_extension_ids.mid = it->first; 
				}
				else if (it->second == RtpExtension_kAbsSendTimeUri)
				{
					m_rtp_header_extension_ids.absSendTime = it->first;
				}
				else if (it->second == RtpExtension_kFrameMarkingUri)
				{
					m_rtp_header_extension_ids.frameMarking = it->first;
				}
				else if (it->second == RtpExtension_kFrameMarking07Uri)
				{
					m_rtp_header_extension_ids.frameMarking07 = it->first;
				}
				else if (it->second == RtpExtension_kAudioLevelUri)
				{
					m_rtp_header_extension_ids.ssrcAudioLevel = it->first;
				}
				else if (it->second == RtpExtension_kVideoRotationUri)
				{
					m_rtp_header_extension_ids.videoOrientation = it->first;
				}
				else if (it->second == RtpExtension_kTimestampOffsetUri)
				{
					m_rtp_header_extension_ids.toffset = it->first;
				}
			}
		}
		
		 



		std::vector<ccandidate> candidates =  m_local_sdp.get_candidate();

		//m_update_socket_ptr = new cudp_socket(this, candidate.m_ip, candidate.m_port);

		/*for (std::vector<ccandidate>::iterator iter = candidates.begin(); iter != candidates.end(); ++iter)
		{
			std::string ip = wan_ip;
			cudp_socket * socket_ptr = new   cudp_socket(this,  ip, (*iter).m_port);
			m_udp_ports.push_back((*iter).m_port);
			m_udp_sockets.push_back(socket_ptr);
			socket_ptr = NULL;
		}*/

		for (std::vector<ccandidate>::iterator iter = candidates.begin(); iter != candidates.end(); ++iter)
		{
			std::string ip = wan_ip ;
			ctcp_server * socket_ptr = new   ctcp_server(this, this, ip, (*iter).m_port);
			m_udp_ports.push_back((*iter).m_port);
			m_tcp_servers.push_back(socket_ptr);
			socket_ptr = NULL;
		}
		//m_dtls_ptr = new cdtls_client(this);

		//m_dtls_ptr->init();
		m_dtls_ptr = new crtc_dtls(this);
		m_dtls_ptr->init();

		crtc_ssrc_info * rtc_ssrc_info_ptr = g_global_rtc_config.get_stream_uri(m_local_sdp.m_msids[0]);
		if (!rtc_ssrc_info_ptr)
		{
			WARNING_EX_LOG("create media ssrc failed !!![ media name = %s]", m_local_sdp.m_msids[0].c_str()); 
		}

		if (m_rtc_client_type == ERtcClientPlayer)
		{
			if (stream_desc.m_audio_track_desc_ptr)
			{
				crtc_producer::crtp_params params;
				params.mid = stream_desc.m_audio_track_desc_ptr->m_mid;
				params.params.payload_type = stream_desc.m_audio_track_desc_ptr->m_media_ptr->m_pt;
				params.params.ssrc = stream_desc.m_audio_track_desc_ptr->m_ssrc;
				params.params.type = stream_desc.m_audio_track_desc_ptr->m_media_ptr->m_type;
				params.params.subtype = stream_desc.m_audio_track_desc_ptr->m_media_ptr->m_name;
				//if (stream_desc.m_audio_track_desc_ptr-)
				/*if (rtc_ssrc_info_ptr)
				{
					if (!m_server_ssrc_map.insert(std::make_pair(params.params.ssrc, rtc_ssrc_info_ptr->m_audio_ssrc)).second)
					{
						WARNING_EX_LOG("audio insert server ssrc table failed !!! (client ssrc = %u)(server ssrc= %u)", params.params.ssrc, rtc_ssrc_info_ptr->m_audio_ssrc);
					}
				}*/

				crtc_consumer * consumer_ptr = new crtc_consumer(this, "audio", params);
				if (consumer_ptr) 
				{
					if (!m_all_rtp_listener.add_consumer(stream_desc.m_audio_track_desc_ptr->m_ssrc, consumer_ptr))
					{
						WARNING_EX_LOG("add consumer_ptr audio failed (ssrc = %u)", stream_desc.m_audio_track_desc_ptr->m_ssrc);
					}
				}

			}

			{
				for (const crtc_track_description * rtc_track : stream_desc.m_video_track_descs)
				{
					crtc_producer::crtp_params params;
					params.mid = rtc_track->m_mid;
					params.params.payload_type = rtc_track->m_media_ptr->m_pt;
					params.params.subtype = rtc_track->m_media_ptr->m_name;
					params.params.ssrc = rtc_track->m_ssrc;
					params.params.type = rtc_track->m_type;
					params.params.use_nack = true;
					params.params.use_fir = true;
					params.params.use_pli = true;
					crtc_consumer * consumer_ptr = NULL;
					 
					if (rtc_track->m_rtx_ptr)
					{
						params.params.rtx_payload_type = rtc_track->m_rtx_ptr->m_pt;
						params.params.rtx_ssrc = rtc_track->m_rtx_ssrc;
						if (rtc_ssrc_info_ptr)
						{
							if (!m_server_ssrc_map.insert(std::make_pair(params.params.ssrc, rtc_ssrc_info_ptr->m_video_ssrc)).second)
							{
								WARNING_EX_LOG("video rtx  insert server ssrc table failed !!! (client ssrc = %u)(server ssrc= %u)", params.params.ssrc, rtc_ssrc_info_ptr->m_rtx_video_ssrc);
							}
						}
						consumer_ptr = new crtc_consumer(this, "video", params );;
						if (consumer_ptr)
						{
							if (!m_all_rtp_listener.add_rtx_consumer(rtc_track->m_rtx_ssrc, consumer_ptr))
							{
								WARNING_EX_LOG("add producer video failed (ssrc = %u)", rtc_track->m_rtx_ssrc);
							}
						}
						else
						{
							WARNING_EX_LOG("alloc failed !!!");
						}
					}
					else
					{
						consumer_ptr = new crtc_consumer(this, "video", params );;
					}

					if (consumer_ptr)
					{
						if (!m_all_rtp_listener.add_consumer(rtc_track->m_ssrc, consumer_ptr))
						{
							WARNING_EX_LOG("add producer video failed (ssrc = %u)", rtc_track->m_ssrc);
						}
					}
					else
					{
						WARNING_EX_LOG("alloc failed !!!");
					}
					
				}
			}


			//for (std::vector<cmedia_desc>::iterator iter = m_local_sdp.m_media_descs.begin(); iter != m_local_sdp.m_media_descs.end(); ++iter)
			//{
			//	for (std::vector<cssrc_info>::iterator ssrc_iter = iter->m_ssrc_infos.begin(); ssrc_iter != iter->m_ssrc_infos.end(); ++ssrc_iter)
			//	{
			//		if (iter->is_audio())
			//		{
			//			if (m_all_audio_ssrc == 0)
			//			{
			//				m_all_audio_ssrc = ssrc_iter->m_ssrc;
			//			}
			//			else if (m_all_rtx_audio_ssrc == 0)
			//			{
			//				m_all_rtx_audio_ssrc = ssrc_iter->m_ssrc;
			//			}
			//			else
			//			{
			//				WARNING_EX_LOG("audio rtx --> ssrc = %u", ssrc_iter->m_ssrc);
			//			}
			//		}
			//		if (iter->is_video())
			//		{
			//			//m_all_video_ssrcs.push_back(ssrc_iter->m_ssrc);
			//			if (m_all_video_ssrc == 0)
			//			{
			//				m_all_video_ssrc = ssrc_iter->m_ssrc;
			//			}
			//			else if (m_all_rtx_video_ssrc == 0)
			//			{
			//				m_all_rtx_video_ssrc = ssrc_iter->m_ssrc;
			//			}
			//			else
			//			{
			//				WARNING_EX_LOG("video rtx --> ssrc = %u", ssrc_iter->m_ssrc);
			//			}
			//		}
			//		if (iter->m_payload_types.size())
			//		{
			//			m_ssrc_media_type_map[ssrc_iter->m_ssrc] = iter->m_payload_types[0].m_payload_type;
			//		} 
			//	}
			//}
		}
		else  if (m_rtc_client_type == ERtcClientPublisher)
		{

			if (stream_desc.m_audio_track_desc_ptr)
			{
				crtc_producer::crtp_params params;
				params.mid = stream_desc.m_audio_track_desc_ptr->m_mid;
				params.params.payload_type = stream_desc.m_audio_track_desc_ptr->m_media_ptr->m_pt;
				params.params.ssrc = stream_desc.m_audio_track_desc_ptr->m_ssrc;
				params.params.type = stream_desc.m_audio_track_desc_ptr->m_media_ptr->m_type;
				params.params.subtype = stream_desc.m_audio_track_desc_ptr->m_media_ptr->m_name;
				if (rtc_ssrc_info_ptr)
				{
					if (!m_server_ssrc_map.insert(std::make_pair(params.params.ssrc, rtc_ssrc_info_ptr->m_audio_ssrc)).second)
					{
						WARNING_EX_LOG("audio insert server ssrc table failed !!! (client ssrc = %u)(server ssrc= %u)", params.params.ssrc, rtc_ssrc_info_ptr->m_audio_ssrc);
					}
				}

				crtc_producer * producer_ptr = new crtc_producer(this, "audio", params, m_server_ssrc_map);
				if (!m_all_rtp_listener.add_producer(stream_desc.m_audio_track_desc_ptr->m_ssrc, producer_ptr))
				{
					WARNING_EX_LOG("add producer audio failed (ssrc = %u)", stream_desc.m_audio_track_desc_ptr->m_ssrc);
				}

			}
			
			{
				for (const crtc_track_description * rtc_track : stream_desc.m_video_track_descs)
				{
					crtc_producer::crtp_params params;
					params.mid = rtc_track->m_mid;
					params.params.payload_type = rtc_track->m_media_ptr->m_pt;
					params.params.ssrc = rtc_track->m_ssrc;
					params.params.type = rtc_track->m_type;
					params.params.subtype = rtc_track->m_media_ptr->m_name;
					params.params.use_nack = true;
					params.params.use_fir = true;
					params.params.use_pli = true;
					m_key_frame_ssrc = params.params.ssrc;
					crtc_producer * producer_ptr = NULL; 
					if (rtc_ssrc_info_ptr)
					{
						if (!m_server_ssrc_map.insert(std::make_pair(params.params.ssrc, rtc_ssrc_info_ptr->m_video_ssrc)).second)
						{
							WARNING_EX_LOG("video insert server ssrc table failed !!! (client ssrc = %u)(server ssrc= %u)", params.params.ssrc, rtc_ssrc_info_ptr->m_video_ssrc);
						}
					}
					if (rtc_track->m_rtx_ptr)
					{
						params.params.rtx_payload_type = rtc_track->m_rtx_ptr->m_pt;
						params.params.rtx_ssrc = rtc_track->m_rtx_ssrc;
						if (rtc_ssrc_info_ptr)
						{
							if (!m_server_ssrc_map.insert(std::make_pair(params.params.rtx_ssrc, rtc_ssrc_info_ptr->m_rtx_video_ssrc)).second)
							{
								WARNING_EX_LOG("video rtx  insert server ssrc table failed !!! (client ssrc = %u)(server ssrc= %u)", params.params.ssrc, rtc_ssrc_info_ptr->m_rtx_video_ssrc);
							}
						}
						producer_ptr = new crtc_producer(this, "video", params, m_server_ssrc_map);;
						if (!m_all_rtp_listener.add_rtx_add_producer(rtc_track->m_rtx_ssrc, producer_ptr))
						{
							WARNING_EX_LOG("add producer video failed (ssrc = %u)", rtc_track->m_rtx_ssrc);
						}
					}
					else
					{
						producer_ptr = new crtc_producer(this, "video", params, m_server_ssrc_map);;
					}
				
					
					if (!m_all_rtp_listener.add_producer(rtc_track->m_ssrc, producer_ptr))
					{
						WARNING_EX_LOG("add producer video failed (ssrc = %u)", rtc_track->m_ssrc);
					}
				}
			}
			//for (std::vector<cmedia_desc>::iterator iter = m_remote_sdp.m_media_descs.begin(); iter != m_remote_sdp.m_media_descs.end(); ++iter)
			//{
			//	for (std::vector<cssrc_info>::iterator ssrc_iter = iter->m_ssrc_infos.begin(); ssrc_iter != iter->m_ssrc_infos.end(); ++ssrc_iter)
			//	{
			//		if (iter->is_audio())
			//		{
			//			//m_all_audio_ssrcs.push_back(ssrc_iter->m_ssrc);
			//			if(m_all_audio_ssrc == 0)
			//			{
			//				m_all_audio_ssrc = ssrc_iter->m_ssrc;
			//			}
			//			else if (m_all_rtx_audio_ssrc == 0)
			//			{
			//				m_all_rtx_audio_ssrc = ssrc_iter->m_ssrc;
			//			}
			//			else
			//			{
			//				WARNING_EX_LOG("audio rtx --> ssrc = %u", ssrc_iter->m_ssrc);
			//			}
			//		}
			//		if (iter->is_video())
			//		{
			//			//m_all_video_ssrcs.push_back(ssrc_iter->m_ssrc);
			//			if (m_all_video_ssrc == 0)
			//			{
			//				m_all_video_ssrc = ssrc_iter->m_ssrc;
			//			}
			//			else if (m_all_rtx_video_ssrc == 0)
			//			{
			//				m_all_rtx_video_ssrc = ssrc_iter->m_ssrc;
			//			}
			//			else
			//			{
			//				WARNING_EX_LOG("video rtx --> ssrc = %u", ssrc_iter->m_ssrc);
			//			}
			//		}
			//		if (iter->m_payload_types.size())
			//		{
			//			m_ssrc_media_type_map[ssrc_iter->m_ssrc] = iter->m_payload_types[0].m_payload_type;
			//		}

			//	}
			//}



		}


		 
		//TODO@chensong 2023-04-20 datachannel -->
		const cmedia_desc & application_media = m_remote_sdp.m_media_descs.back();

		if (application_media.m_type == "application")
		{
			//m_media_descs.push_back(cmedia_desc(media));
			m_sctp_association_ptr = new csctp_association(this, 10, 10, application_media.m_max_message_size, 21000);
		}
		

		m_timer_ptr = new ctimer(this);
		
		return true;
	}
	bool crtc_transport::create_players(const std::map<uint32_t, crtc_track_description*>& sub_relations)
	{


		//for (m_local)


		//crtc_player_stream * player_ptr = new crtc_player_stream();
		//player_ptr->init(sub_relations);
		//// 
		//// make map between ssrc and player for fastly searching
		//for (std::map<uint32 , crtc_track_description*>::const_iterator it = sub_relations.begin(); it != sub_relations.end(); ++it) {
		//	crtc_track_description* track_desc = it->second;
		//	if (track_desc->m_type == "audio")
		//	{
		//		m_all_audio_ssrcs.push_back(track_desc->m_ssrc);
		//	}
		//	else if (track_desc->m_type == "video")
		//	{
		//		m_all_video_ssrcs.push_back(track_desc->m_ssrc);
		//	}
		//	std::map<uint32 , crtc_player_stream*>::iterator it_player = m_players_ssrc_map.find(track_desc->m_ssrc);
		//	if ((m_players_ssrc_map.end() != it_player) && (player_ptr != it_player->second)) 
		//	{
		//		WARNING_EX_LOG( "duplicate ssrc %d, track id: %s",
		//			track_desc->m_ssrc, track_desc->m_id.c_str());
		//		return false;
		//	}
		//	m_players_ssrc_map[track_desc->m_ssrc] = player_ptr;

		//	if (0 != track_desc->m_fec_ssrc) {
		//		if (m_players_ssrc_map.end() != m_players_ssrc_map.find(track_desc->m_fec_ssrc)) {
		//			WARNING_EX_LOG( "duplicate fec ssrc %d, track id: %s",
		//				track_desc->m_fec_ssrc, track_desc->m_id.c_str());
		//			return false;
		//		}
		//		m_players_ssrc_map[track_desc->m_fec_ssrc] = player_ptr;
		//	}

		//	if (0 != track_desc->m_rtx_ssrc) {
		//		if (m_players_ssrc_map.end() != m_players_ssrc_map.find(track_desc->m_rtx_ssrc)) {
		//			WARNING_EX_LOG( "duplicate rtx ssrc %d, track id: %s",
		//				track_desc->m_rtx_ssrc, track_desc->m_id.c_str());
		//			return false;
		//		}
		//		m_players_ssrc_map[track_desc->m_rtx_ssrc] = player_ptr;
		//	}
		//}

		return true;
	}
	void crtc_transport::update(uint32 uDeltaTime)
	{
		//m_feedback_gcc_timer += uDeltaTime;
		//if (m_feedback_gcc_timer > 100)
		//{
		//	m_feedback_gcc_timer -= 100;
		//	m_remote_estimator.send_periodic_Feedbacks();
		//	//NORMAL_EX_LOG("[m_rtc_net_state = %u]", m_rtc_net_state);
		//}
		
	}
	void crtc_transport::destroy()
	{
		DEBUG_EX_LOG("");
		//m_current_socket_ptr = NULL;
		m_all_rtp_listener.destroy();
		if (m_dtls_ptr)
		{
			m_dtls_ptr->destroy();
			delete m_dtls_ptr;
			m_dtls_ptr = NULL;
		}
		if (m_timer_ptr)
		{
			m_timer_ptr->Stop();
			delete m_timer_ptr;
			m_timer_ptr = NULL;
		}
		for (std::vector<cudp_socket*>::iterator iter = m_udp_sockets.begin(); iter != m_udp_sockets.end(); ++iter)
		{
			cudp_socket* ptr = *iter;
			delete ptr;
		}
		m_udp_sockets.clear();

		for (std::vector<ctcp_server*>::iterator iter = m_tcp_servers.begin(); iter != m_tcp_servers.end(); ++iter)
		{
			ctcp_server* ptr = *iter;
			delete ptr;
		}
		m_udp_sockets.clear();
		for (const uint32 &port : m_udp_ports)
		{
			g_global_rtc_port.brack_port(port);
		}
		if (m_srtp_send_session_ptr)
		{
			delete m_srtp_send_session_ptr;
			m_srtp_send_session_ptr = NULL;
		}
		if (m_srtp_recv_session_ptr)
		{
			delete m_srtp_recv_session_ptr;
			m_srtp_recv_session_ptr = NULL;
		}
		if (m_tcc_client)
		{
			m_tcc_client->TransportDisconnected();
			delete m_tcc_client;
			m_tcc_client = NULL;
		}
		if (m_tcc_server )
		{
			m_tcc_server->TransportDisconnected();

			delete m_tcc_server;

			m_tcc_server = NULL;
		}
		if (m_sctp_association_ptr)
		{
			delete m_sctp_association_ptr;
			m_sctp_association_ptr = NULL;
		}
		
	}
	bool crtc_transport::is_active() const
	{
		bool ret = (m_time_out_ms + g_cfg.get_uint32(ECI_StunTimeOut)) > uv_util::GetTimeMs();
		if (!ret)
		{
			WARNING_EX_LOG("[cur_ms = %u][pre_ms = %u][diff = %u]", uv_util::GetTimeMs(), m_time_out_ms, uv_util::GetTimeMs() - m_time_out_ms);;
		}
		return ret;
	}
	void crtc_transport::request_key_frame()
	{
		++m_request_keyframe;
		///////////////////////////////////////////////////////////////////////////
		////                         IDR Request

		//     关键帧也叫做即时刷新帧，简称IDR帧。对视频来说，IDR帧的解码无需参考之前的帧，因此在丢包严重时可以通过发送关键帧请求进行画面的恢复。
		// 关键帧的请求方式分为三种：RTCP FIR反馈（Full intra frame request）、RTCP PLI 反馈（Picture Loss Indictor）或SIP Info消息，
		//							具体使用哪种可通过协商确定.

		///////////////////////////////////////////////////////////////////////////
		//if (this->params.usePli)
		//if (m_request_keyframe > 0)
		{
			DEBUG_EX_LOG("rtcp, rtx, sending PLI [ssrc:%" PRIu32 "], [m_request_keyframe = %" PRIu32 "]", m_key_frame_ssrc, m_request_keyframe);
			m_request_keyframe = 0;
			// Sender SSRC should be 0 since there is no media sender involved, but
			// some implementations like gstreamer will fail to process it otherwise.

			 
			RTC::RTCP::FeedbackPsPliPacket packet(m_key_frame_ssrc, m_key_frame_ssrc);

			packet.Serialize(RTC::RTCP::Buffer);
			send_rtcp_packet(&packet);
			//this->pliCount++;

			// Notify the listener.
			//static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
		}
		//else if (this->params.useFir)
		//{
		//	DEBUG_EX_LOG(" rtcp, rtx,  sending FIR [ssrc:%" PRIu32 "]", m_all_video_ssrc);

		//	// Sender SSRC should be 0 since there is no media sender involved, but
		//	// some implementations like gstreamer will fail to process it otherwise.
		//	RTC::RTCP::FeedbackPsFirPacket packet(m_all_video_ssrc, m_all_video_ssrc);
		//	auto* item = new RTC::RTCP::FeedbackPsFirItem(GetSsrc(), ++this->firSeqNumber);

		//	packet.AddItem(item);
		//	packet.Serialize(RTC::RTCP::Buffer);

		//	this->firCount++;

		//	// Notify the listener.
		//	static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
		//}
	}
	void crtc_transport::OnTransportConsumerKeyFrameRequested()
	{
		if (!get_dtls_connected_ok())
		{
			WARNING_EX_LOG("not dtls connected ok !!!");
			return;
		}
		ctransport_mgr:: STREAM_URL_MAP::iterator iter =   g_transport_mgr.m_all_stream_url_map.find(m_rtc_master.m_room_name + "/" + m_rtc_master.m_user_name);
	
		if (iter == g_transport_mgr.m_all_stream_url_map.end())
		{
			WARNING_EX_LOG("not find stream url = %s", std::string(m_rtc_master.m_room_name + "/" + m_rtc_master.m_room_name).c_str());
			return;
		}
		iter->second->request_key_frame();
	}
	void crtc_transport::OnConsumerRetransmitRtpPacket(crtc_consumer * consumer, RTC::RtpPacket * packet)
	{
		// Update abs-send-time if present.
		//packet->UpdateAbsSendTime(uv_util::GetTimeMs());

		// Update transport wide sequence number if present.
		// clang-format off
		if (
			this->m_tcc_client &&
			this->m_tcc_client->GetBweType() == RTC::BweType::TRANSPORT_CC &&
			packet->UpdateTransportWideCc01(this->m_transportWideCcSeq + 1)
			)
			// clang-format on
		{
			this->m_transportWideCcSeq++;

			auto* tccClient = this->m_tcc_client;
			webrtc::RtpPacketSendInfo packetInfo;

			packetInfo.ssrc = packet->GetSsrc();
			packetInfo.transport_sequence_number = this->m_transportWideCcSeq;
			packetInfo.has_rtp_sequence_number = true;
			packetInfo.rtp_sequence_number = packet->GetSequenceNumber();
			packetInfo.length = packet->GetSize();
			packetInfo.pacing_info = this->m_tcc_client->GetPacingInfo();

			// Indicate the pacer (and prober) that a packet is to be sent.
			this->m_tcc_client->InsertPacket(packetInfo);

#ifdef ENABLE_RTC_SENDER_BANDWIDTH_ESTIMATOR
			auto* senderBwe = this->senderBwe;
			RTC::SenderBandwidthEstimator::SentInfo sentInfo;

			sentInfo.wideSeq = this->transportWideCcSeq;
			sentInfo.size = packet->GetSize();
			sentInfo.sendingAtMs = DepLibUV::GetTimeMs();

			auto* cb = new onSendCallback([tccClient, &packetInfo, senderBwe, &sentInfo](bool sent) {
				if (sent)
				{
					tccClient->PacketSent(packetInfo, DepLibUV::GetTimeMsInt64());

					sentInfo.sentAtMs = DepLibUV::GetTimeMs();

					senderBwe->RtpPacketSent(sentInfo);
				}
			});

			SendRtpPacket(consumer, packet, cb);
#else
			  cudp_socket_handler::onSendCallback* cb = new cudp_socket_handler::onSendCallback([tccClient, &packetInfo](bool sent) {
				if (sent)
				{
					tccClient->PacketSent(packetInfo, uv_util::GetTimeMsInt64());
				}
			});
			  //WARNING_EX_LOG("RTX [ssrc = %u][payload_type = %u][seq=%u]", packet->GetSsrc(), packet->GetPayloadType(), packet->GetSequenceNumber());
			send_rtp_packet(  packet, cb);
#endif
		}
		else
		{
			//WARNING_EX_LOG("RTX [ssrc = %u][payload_type = %u][seq=%u]", packet->GetSsrc(), packet->GetPayloadType(), packet->GetSequenceNumber());

			send_rtp_data(packet);
			//SendRtpPacket(consumer, packet);
		}

		//this->sendRtxTransmission.Update(packet);
	}
	void crtc_transport::send_rtp_data(void * data, int32 size)
	{
		/*if (m_rtc_net_state != ERtcNetworkStateEstablished)
		{
			WARNING_EX_LOG("");
			return  ;
		}*/
		if (m_tcp_connection_ptr && m_srtp_send_session_ptr)
		{
			if (m_srtp_send_session_ptr->EncryptRtp((const uint8_t**)&data, (size_t *)&size))
			{
				WARNING_EX_LOG("rtp encrypt rtp failed !!!");
				return;
			}
			//NORMAL_EX_LOG("rtp data size = %u", size);
			//m_current_socket_ptr->Send((const uint8_t *)data, size, &m_remote_addr, NULL);
			m_tcp_connection_ptr->Send((const uint8_t *)data, size, NULL);
		}
	}
	void crtc_transport::send_rtp_data(RTC::RtpPacket * packet)
	{
	/*	if (m_rtc_net_state != ERtcNetworkStateEstablished)
		{
			WARNING_EX_LOG("");
			return;
		}*/
		if (m_tcp_connection_ptr && m_srtp_send_session_ptr)
		{
			//{
			//	for (const cmedia_desc& media : m_local_sdp.m_media_descs)
			//	{
			//		if (media.m_type != "audio")
			//		{
			//			continue;
			//		}
			//		//for (const  cssrc_group & ssrc_group : media.m_ssrc_groups)
			//		if (media.m_ssrc_groups.size())
			//		{
			//			DEBUG_EX_LOG("audio ssrc = %u, --> find ssrc = %u", packet->GetSsrc(), media.m_ssrc_groups[0].m_ssrcs);
			//			packet->SetSsrc(media.m_ssrc_groups[0].m_ssrcs[0]);
			//			break;
			//		}
			//		//if (media)
			//	}
			//}
			//
			//packet->UpdateAbsSendTime(uv_util::GetTimeMs());
			const uint8_t* data = packet->GetData();
			size_t len = packet->GetSize();
			 
			if (len != 512 && !m_srtp_send_session_ptr->EncryptRtp( &data,  &len))
			{
				WARNING_EX_LOG("rtp encrypt rtp failed !!!");
				return;
			}
			//NORMAL_EX_LOG("rtp data size = %u", len);
			//m_current_socket_ptr->Send( data, len, &m_remote_addr, NULL);
			m_tcp_connection_ptr->Send(data, len,  NULL);
		}
	}
	void crtc_transport::send_rtp_packet(RTC::RtpPacket * packet, cudp_socket_handler::onSendCallback * cb)
	{
		if (m_tcp_connection_ptr && m_srtp_send_session_ptr)
		{
			//{
			//	for (const cmedia_desc& media : m_local_sdp.m_media_descs)
			//	{
			//		if (media.m_type != "audio")
			//		{
			//			continue;
			//		}
			//		//for (const  cssrc_group & ssrc_group : media.m_ssrc_groups)
			//		if (media.m_ssrc_groups.size())
			//		{
			//			DEBUG_EX_LOG("audio ssrc = %u, --> find ssrc = %u", packet->GetSsrc(), media.m_ssrc_groups[0].m_ssrcs);
			//			packet->SetSsrc(media.m_ssrc_groups[0].m_ssrcs[0]);
			//			break;
			//		}
			//		//if (media)
			//	}
			//}
			//
			//packet->UpdateAbsSendTime(uv_util::GetTimeMs());
			const uint8_t* data = packet->GetData();
			size_t len = packet->GetSize();

			if (/*len != 512 &&*/ !m_srtp_send_session_ptr->EncryptRtp(&data, &len))
			{
				if (cb)
				{
					(*cb)(false);
					delete cb;
				}
				WARNING_EX_LOG("rtp encrypt rtp failed !!!");
				return;
			}
			//NORMAL_EX_LOG("rtp data size = %u", len);
			//m_current_socket_ptr->Send(data, len, &m_remote_addr, NULL);
			m_tcp_connection_ptr->Send(data, len, NULL);
			if (cb)
			{
				(*cb)(true);
				delete cb;
			}
		}
		else
		{
			if (cb)
			{
				(*cb)(false);
				delete cb;
			}
		}
	}
	void crtc_transport::send_rtp_audio_data(RTC::RtpPacket * packet)
	{
		/*if (m_rtc_net_state != ERtcNetworkStateEstablished)
		{
			WARNING_EX_LOG("");
			return;
		}*/
		//if (m_current_socket_ptr && m_srtp_send_session_ptr && m_all_audio_ssrc != 0)
		//{
		//	packet->SetSsrc(m_all_audio_ssrc);
		//	std::map<uint32, uint32 >::const_iterator iter = m_ssrc_media_type_map.find(m_all_audio_ssrc);

		//	if (iter != m_ssrc_media_type_map.end())
		//	{
		//		packet->SetPayloadType(iter->second);
		//	}
		//	 
		//	const uint8_t* data = packet->GetData();
		//	size_t len = packet->GetSize();
		//	if (!m_srtp_send_session_ptr->EncryptRtp(&data, &len))
		//	{
		//		WARNING_EX_LOG("rtp encrypt rtp failed !!!");
		//		return;
		//	}
		////	NORMAL_EX_LOG("rtp data size = %u", len);
		//	m_current_socket_ptr->Send(data, len, &m_remote_addr, NULL);
		//}
	}
	void crtc_transport::send_rtp_video_data(RTC::RtpPacket * packet)
	{
		/*if (m_rtc_net_state != ERtcNetworkStateEstablished)
		{
			WARNING_EX_LOG("");
			return;
		}*/
		//std::ostringstream cmd;
		//cmd << "[ssrc_map]: ";
		/*for (const std::pair<uint32, uint32> & pi: m_ssrc_media_type_map)
		{
			cmd << "[first = " << pi.first << "][ second = " << pi.second << "]";
		}*/
		//NORMAL_EX_LOG("[-->%s]", cmd.str().c_str());
		//if (m_current_socket_ptr && m_srtp_send_session_ptr &&  m_all_video_ssrc != 0)
		//{
		//	packet->SetSsrc(m_all_video_ssrc);
		//	//size_t size = m_ssrc_media_type_map.size();



		//	std::map<uint32, uint32 >::const_iterator iter =  m_ssrc_media_type_map.find(m_all_video_ssrc);
		//
		//	if (iter != m_ssrc_media_type_map.end())
		//	{
		//		packet->SetPayloadType(iter->second);
		//	}
		//	 
		//	const uint8_t* data = packet->GetData();
		//	size_t len = packet->GetSize();
		//	if (!m_srtp_send_session_ptr->EncryptRtp(&data, &len))
		//	{
		//		WARNING_EX_LOG("rtp encrypt rtp failed !!!");
		//		return;
		//	}
		//	//NORMAL_EX_LOG("rtp data size = %u", len);
		//	m_current_socket_ptr->Send(data, len, &m_remote_addr, NULL);
		//}
	}
	void crtc_transport::send_rtp_rtx_video_data(RTC::RtpPacket * packet)
	{
		//if (m_current_socket_ptr && m_srtp_send_session_ptr &&  m_all_rtx_video_ssrc != 0)
		//{
		//	packet->SetSsrc(m_all_rtx_video_ssrc);
		//	std::map<uint32, uint32 >::const_iterator iter = m_ssrc_media_type_map.find(m_all_rtx_video_ssrc);

		//	if (iter != m_ssrc_media_type_map.end())
		//	{
		//		packet->SetPayloadType(iter->second);
		//	}
		//	 
		//	const uint8_t* data = packet->GetData();
		//	size_t len = packet->GetSize();
		//	if (!m_srtp_send_session_ptr->EncryptRtp(&data, &len))
		//	{
		//		WARNING_EX_LOG("rtp encrypt rtp failed !!!");
		//		return;
		//	}
		//	//NORMAL_EX_LOG("rtp data size = %u", len);
		//	m_current_socket_ptr->Send(data, len, &m_remote_addr, NULL);
		//}
	}

	bool crtc_transport::send_rtcp(const uint8 * data, size_t len)
	{
		if (m_tcp_connection_ptr && m_srtp_send_session_ptr)
		{
			if (!m_srtp_send_session_ptr->EncryptRtcp(&data, &len))
			{
				WARNING_EX_LOG("rtcp encrypt reqest key frame failed !!!");
				return false;
			}
			//NORMAL_EX_LOG("rtp data size = %u", len);
			//m_current_socket_ptr->Send(data, len, &m_remote_addr, NULL);
			m_tcp_connection_ptr->Send(data, len, NULL);
			return true;
		}
		return false;
	}
	void crtc_transport::send_rtcp_compound_packet(RTC::RTCP::CompoundPacket * packet)
	{
		if (m_tcp_connection_ptr && m_srtp_send_session_ptr)
		{
			const uint8_t* data = packet->GetData();
			size_t len = packet->GetSize();
			if (!m_srtp_send_session_ptr->EncryptRtcp(&data, &len))
			{
				WARNING_EX_LOG("rtcp encrypt reqest key frame failed !!!");
				return;
			}
			//NORMAL_EX_LOG("rtp data size = %u", len);
			//m_current_socket_ptr->Send(data, len, &m_remote_addr, NULL);
			m_tcp_connection_ptr->Send((const uint8_t *)data, len, NULL);
		}
	}
	bool crtc_transport::send_sctp_data(const uint8 * data, size_t len)
	{
		if (m_dtls_ptr)
		{
			m_dtls_ptr->send_application_data(data, len);
		}
		return true;
	}
	void crtc_transport::send_sctp_data(uint16_t streamId, uint32_t ppid, const uint8_t * msg, size_t len)
	{
		if (m_rtc_net_state != ERtcNetworkStateEstablished)
		{
			WARNING_EX_LOG("rtc status = %u", m_rtc_net_state);
			return;
		}
		if (!m_sctp_association_ptr)
		{
			WARNING_EX_LOG("not open data channel !!!");
			return;
		}
		m_sctp_association_ptr->SendSctpMessage(streamId, ppid, msg, len);
	}
	void crtc_transport::send_consumer(RTC::RtpPacket * packet)
	{
		crtc_consumer * consumer_ptr =  m_all_rtp_listener.get_consumer(packet);
		if (!consumer_ptr)
		{
			WARNING_EX_LOG("not find consumer ssrc = %u", packet->GetSsrc());
			return;
		}
		consumer_ptr->send_rtp_packet(packet);
	}
	void crtc_transport::send_rtcp_packet(RTC::RTCP::Packet * packet)
	{
		if (m_tcp_connection_ptr && m_srtp_send_session_ptr)
		{
			const uint8_t* data = packet->GetData();
			size_t len = packet->GetSize();
			if (!m_srtp_send_session_ptr->EncryptRtcp(&data, &len))
			{
				WARNING_EX_LOG("rtcp encrypt reqest key frame failed !!!");
				return;
			}
			//NORMAL_EX_LOG("rtp data size = %u", len);
			//m_current_socket_ptr->Send(data, len, &m_remote_addr, NULL);
			m_tcp_connection_ptr->Send((const uint8_t *)data, len, NULL);
		}
		

		//// Ensure there is sending SRTP session.
		//if (!this->srtpSendSession)
		//{
		//	MS_WARN_DEV("ignoring RTCP packet due to non sending SRTP session");

		//	return;
		//}

		//if (!this->srtpSendSession->EncryptRtcp(&data, &len))
		//	return;

		//this->iceServer->GetSelectedTuple()->Send(data, len);

		//// Increase send transmission.
		//RTC::Transport::DataSent(len);
	}
	int32 crtc_transport::write_dtls_data(void * data, int size)
	{
		if (m_tcp_connection_ptr)
		{
			//m_current_socket_ptr->Send((const uint8_t *)data, size, &m_remote_addr, NULL);
			m_tcp_connection_ptr->Send((const uint8_t *)data, size, NULL);
		}
		
		return 0;
	}

	chen::int32 crtc_transport::on_dtls_handshake_done()
	{
		NORMAL_EX_LOG("");
		return 0;
	}

	void crtc_transport::on_dtls_application_data(const uint8 * data, int32 size)
	{
		//NORMAL_EX_LOG("application data --> data channel -->>>>>>>");
		_on_application_data_receviced(data, size);
	}

	void crtc_transport::on_dtls_transport_connected(ECRYPTO_SUITE srtp_crypto_suite, uint8 * srtp_local_key, size_t srtp_local_key_len, uint8 * srtp_remote_key, size_t srtp_remote_key_len)
	{
		//std::string local_key((char *)srtp_local_key, srtp_local_key_len);
		/*if (!m_srtp.init(std::string((char *)srtp_local_key, srtp_local_key_len), std::string((char *)srtp_remote_key, srtp_remote_key_len)))
		{
			WARNING_EX_LOG("srtp send recv init failed !!!");
		}
		NORMAL_EX_LOG("srtp send recv create init OK ---->");*/
		// Close it if it was already set and update it.
		if (m_srtp_send_session_ptr)
		{
			delete m_srtp_send_session_ptr;
			m_srtp_send_session_ptr = nullptr;
		}
		
		if (m_srtp_recv_session_ptr)
		{
			delete m_srtp_recv_session_ptr;
			m_srtp_recv_session_ptr = nullptr;
		}
		 
		 

		try
		{
			m_srtp_send_session_ptr = new csrtp_session(
				EOUTBOUND, srtp_crypto_suite, srtp_local_key, srtp_local_key_len);
		}
		catch (const std::exception& error)
		{
			ERROR_EX_LOG("error creating SRTP sending session: %s", error.what());
		}

		try
		{
			m_srtp_recv_session_ptr = new csrtp_session(
				EINBOUND, srtp_crypto_suite, srtp_remote_key, srtp_remote_key_len);

			// Notify the Node WebRtcTransport.
			/*json data = json::object();

			data["dtlsState"] = "connected";
			data["dtlsRemoteCert"] = remoteCert;

			Channel::ChannelNotifier::Emit(this->id, "dtlsstatechange", data);
			DEBUG_EX_LOG("data = %s", data.dump().c_str());*/
			// Tell the parent class.
			//RTC::Transport::Connected();
		}
		catch (const std::exception& error)
		{
			ERROR_EX_LOG("error creating SRTP receiving session: %s", error.what());

			if (m_srtp_send_session_ptr)
			{
				delete m_srtp_send_session_ptr;
				m_srtp_send_session_ptr = nullptr;
			}
		}
		m_rtc_net_state = ERtcNetworkStateEstablished;
		if (!m_tcc_server && ERtcClientPublisher == m_rtc_client_type)
		{
			m_tcc_server = new RTC::TransportCongestionControlServer(this, RTC::BweType::TRANSPORT_CC, RTC::MtuSize);
			m_tcc_server->SetMaxIncomingBitrate(g_cfg.get_uint32(ECI_RtcMaxBitrate));
			m_tcc_server->TransportConnected();
		}
		if (!m_tcc_client && ERtcClientPlayer == m_rtc_client_type)
		{
			m_tcc_client   = new RTC::TransportCongestionControlClient(
				this, RTC::BweType::TRANSPORT_CC, g_cfg.get_uint32(ECI_RtcStartBitrate), g_cfg.get_uint32(ECI_RtcMaxBitrate));
			m_tcc_client->TransportConnected();

		}
		if (m_sctp_association_ptr)
		{
			m_sctp_association_ptr->TransportConnected();
		}
		if (m_timer_ptr)
		{
			m_timer_ptr->Start(static_cast<uint64_t>(RTC::RTCP::MaxVideoIntervalMs / 2));
		}
	}

	void crtc_transport::OnPacketReceived(cudp_socket * socket, const uint8_t * data, size_t len, const sockaddr * remoteAddr)
	{
		//NORMAL_EX_LOG("---->");
		// Increase receive transmission.
		//RTC::Transport::DataReceived(len);


		memcpy(&m_remote_addr, remoteAddr, sizeof(*remoteAddr));
		/*for ( cudp_socket * temp : m_udp_sockets)
		{
			if (temp == socket)
			{
				m_current_socket_ptr = temp;
				DEBUG_EX_LOG(" find cur dup socket Ok !!");
			}
		}*/
		// TODO@chensong 2023-05-23 单线程 没有问题 多线程是有问题哈 ^_^
		//m_current_socket_ptr  = socket;
		// TODO@chensong 2023-05-11 firefox浏览器的适配   不知道firefox 修改webrtc的stun进行优化操作
		if (m_rtc_net_state == ERtcNetworkStateEstablished)
		{
			m_time_out_ms = uv_util::GetTimeMs();
		}
		// Check if it's STUN.
		if (crtc_stun_packet::is_stun(data, len))
		{
			//NORMAL_EX_LOG("is_stun");
			 
			//OnStunDataReceived(tuple, data, len);
			/*m_rtc_stun_packet.decode((const char *)(data), len);
			if (!m_rtc_stun_packet.is_binding_request())
			{
				WARNING_EX_LOG("stun not binding request failed !!!");
				return;
			}*/
			//uint64 ms = uv_util::GetTimeMs();
			_on_stun_data_received(socket, data, len, remoteAddr);
			//uint64 diff_ms = uv_util::GetTimeMs();
			//NORMAL_EX_LOG("media stun --> ms = %u", diff_ms - ms);
		}
		// Check if it's RTCP.
		else if (RTC::RTCP::Packet::IsRtcp(data, len))
		{
			//NORMAL_EX_LOG("IsRtcp>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
			 
			//OnRtcpDataReceived(tuple, data, len);
			_on_rtcp_data_received(socket, data, len, remoteAddr);
			
		}
		// Check if it's RTP.
		else if (RTC::RtpPacket::IsRtp(data, len))
		{
			//NORMAL_EX_LOG("IsRtp");
			 
			//OnRtpDataReceived(tuple, data, len);
			_on_rtp_data_received(socket, data, len, remoteAddr);
		}
		// Check if it's DTLS.
		else if (RTC::DtlsTransport::IsDtls(data, len))
		{
			//NORMAL_EX_LOG("IsDtls");
			//OnDtlsDataReceived(tuple, data, len);
			_on_dtls_data_received(socket, data, len, remoteAddr);
		}
		else
		{
			 
			WARNING_EX_LOG("ignoring received packet of unknown type");
		}
		
	}
	void crtc_transport::OnUdpSocketPacketReceived(cudp_socket * socket, const uint8_t * data, size_t len, const sockaddr * remoteAddr)
	{
		/*std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::microseconds ms;*/
		OnPacketReceived(socket, data, len, remoteAddr);
		/*cur_time_ms = std::chrono::steady_clock::now();
		dur = cur_time_ms - pre_time;
		ms = std::chrono::duration_cast<std::chrono::microseconds>(dur);*/
		//NORMAL_EX_LOG("udp recv packet [microseconds = %u]", ms.count());
		/*elapse = static_cast<uint32_t>(ms.count());
		if (elapse < TICK_TIME)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME - elapse));
		}*/
	}
	void crtc_transport::OnRtcTcpConnectionNew(ctcp_server * tcpServer, ctcp_connection * connection)
	{
		NORMAL_EX_LOG("new [address=%s:%u]", connection->GetPeerIp().c_str(), connection->GetPeerPort());
	}
	void crtc_transport::OnTcpConnectionPacketReceived(ctcp_connection * connection, const uint8_t * data, size_t len)
	{
		
		// TODO@chensong 2023-05-11 firefox浏览器的适配   不知道firefox 修改webrtc的stun进行优化操作
		//if (m_rtc_net_state == ERtcNetworkStateEstablished)
		{
			m_time_out_ms = uv_util::GetTimeMs();
		}
		//NORMAL_EX_LOG("[%s]", data);

		m_tcp_connection_ptr = connection;
		// Check if it's STUN.
		if (crtc_stun_packet::is_stun(data, len))
		{
			//NORMAL_EX_LOG("is_stun");

			//OnStunDataReceived(tuple, data, len);
			/*m_rtc_stun_packet.decode((const char *)(data), len);
			if (!m_rtc_stun_packet.is_binding_request())
			{
				WARNING_EX_LOG("stun not binding request failed !!!");
				return;
			}*/
			//uint64 ms = uv_util::GetTimeMs();
			_on_stun_data_received(NULL, data, len, connection->GetPeerAddress());
			//uint64 diff_ms = uv_util::GetTimeMs();
			//NORMAL_EX_LOG("media stun --> ms = %u", diff_ms - ms);
		}
		// Check if it's RTCP.
		else if (RTC::RTCP::Packet::IsRtcp(data, len))
		{
			//NORMAL_EX_LOG("IsRtcp>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

			//OnRtcpDataReceived(tuple, data, len);
			_on_rtcp_data_received(NULL, data, len, connection->GetPeerAddress());

		}
		// Check if it's RTP.
		else if (RTC::RtpPacket::IsRtp(data, len))
		{
			//NORMAL_EX_LOG("IsRtp");

			//OnRtpDataReceived(tuple, data, len);
			_on_rtp_data_received(NULL, data, len, connection->GetPeerAddress());
		}
		// Check if it's DTLS.
		else if (RTC::DtlsTransport::IsDtls(data, len))
		{
			//NORMAL_EX_LOG("IsDtls");
			//OnDtlsDataReceived(tuple, data, len);
			_on_dtls_data_received(NULL, data, len, connection->GetPeerAddress());
		}
		else
		{

			WARNING_EX_LOG("ignoring received packet of unknown type");
		}
	}
	void crtc_transport::OnRtcTcpConnectionClosed(ctcp_server * tcpServer, ctcp_connection * connection)
	{
		NORMAL_EX_LOG("close [address=%s:%u]", connection->GetPeerIp().c_str(), connection->GetPeerPort());
	}
	void crtc_transport::OnTransportCongestionControlServerSendRtcpPacket(RTC::TransportCongestionControlServer * tccServer, RTC::RTCP::Packet * packet)
	{
		packet->Serialize(RTC::RTCP::Buffer);
		//NORMAL_EX_LOG("--->");
		send_rtcp_packet(packet);
	}
	void crtc_transport::OnTransportCongestionControlClientBitrates(RTC::TransportCongestionControlClient * tccClient, RTC::TransportCongestionControlClient::Bitrates & bitrates)
	{
		if (!m_tcc_client)
		{
			WARNING_EX_LOG(" tcc ptr = NULL");
			return;
		}
		//uint32_t availableBitrate = this->tccClient->GetAvailableBitrate();

		m_tcc_client->RescheduleNextAvailableBitrateEvent();

		uint32_t totalDesiredBitrate{ 0u };

		for (auto& kv : m_all_rtp_listener.m_ssrc_consumer_table)
		{
			//auto* consumer = kv.second;
			auto desiredBitrate = kv.second->get_desired_bitrate();

			totalDesiredBitrate += desiredBitrate;
		}

		NORMAL_EX_LOG("total desired bitrate: %" PRIu32, totalDesiredBitrate);

		m_tcc_client->SetDesiredBitrate(totalDesiredBitrate, false);
	}
	void crtc_transport::OnTransportCongestionControlClientSendRtpPacket(RTC::TransportCongestionControlClient * tccClient, RTC::RtpPacket * packet, const webrtc::PacedPacketInfo & pacingInfo)
	{
		// Update abs-send-time if present.
		//packet->UpdateAbsSendTime(uv_util::GetTimeMs());

		// Update transport wide sequence number if present.
		// clang-format off
		if (
			m_tcc_client->GetBweType() == RTC::BweType::TRANSPORT_CC &&
			packet->UpdateTransportWideCc01(m_transportWideCcSeq + 1)
			)
			// clang-format on
		{
			m_transportWideCcSeq++;

			// May emit 'trace' event.
			//EmitTraceEventProbationType(packet);

			webrtc::RtpPacketSendInfo packetInfo;

			packetInfo.ssrc = packet->GetSsrc();
			packetInfo.transport_sequence_number = m_transportWideCcSeq;
			packetInfo.has_rtp_sequence_number = true;
			packetInfo.rtp_sequence_number = packet->GetSequenceNumber();
			packetInfo.length = packet->GetSize();
			packetInfo.pacing_info = pacingInfo;

			// Indicate the pacer (and prober) that a packet is to be sent.
			this->m_tcc_client->InsertPacket(packetInfo);

			//NORMAL_EX_LOG("====================>");
			/*	const auto* cb = new onSendCallback([m_tcc_client, &packetInfo](bool sent) {
					if (sent)
						m_tcc_client->PacketSent(packetInfo, uv_util::GetTimeMsInt64());
				});

				SendRtpPacket(nullptr, packet, cb);*/
			send_rtp_data(packet);

		}
		else
		{
			// May emit 'trace' event.
			//EmitTraceEventProbationType(packet);

			send_rtp_data( packet);
		}

		//this->sendProbationTransmission.Update(packet);

		//MS_DEBUG_DEV(
		//	"probation sent [seq:%" PRIu16 ", wideSeq:%" PRIu16 ", size:%zu, bitrate:%" PRIu32 "]",
		//	packet->GetSequenceNumber(),
		//	this->transportWideCcSeq,
		//	packet->GetSize(),
		//	this->sendProbationTransmission.GetBitrate(uv_util::GetTimeMs()));
	}
	void crtc_transport::OnSctpAssociationConnecting(csctp_association * sctpAssociation)
	{
		//WARNING_EX_LOG("");
	}

	void crtc_transport::OnSctpAssociationConnected(csctp_association* sctpAssociation)
	{
		//WARNING_EX_LOG("");
		// Tell all DataConsumers.
		/*for (auto& kv : this->mapDataConsumers)
		{
			auto* dataConsumer = kv.second;

			if (dataConsumer->GetType() == RTC::DataConsumer::Type::SCTP)
			{
				dataConsumer->SctpAssociationConnected();
			}
		}*/
	}

	void crtc_transport::OnSctpAssociationFailed(csctp_association * sctpAssociation)
	{
		//WARNING_EX_LOG("");
		/*for (auto& kv : this->mapDataConsumers)
		{
			auto* dataConsumer = kv.second;

			if (dataConsumer->GetType() == RTC::DataConsumer::Type::SCTP)
			{
				dataConsumer->SctpAssociationClosed();
			}
		}*/
	}

	void crtc_transport::OnSctpAssociationClosed(csctp_association * sctpAssociation)
	{
		//WARNING_EX_LOG("");
		// Tell all DataConsumers.
		/*for (auto& kv : this->mapDataConsumers)
		{
			auto* dataConsumer = kv.second;

			if (dataConsumer->GetType() == RTC::DataConsumer::Type::SCTP)
			{
				dataConsumer->SctpAssociationClosed();
			}
		}*/
	}

	void crtc_transport::OnSctpAssociationSendData(csctp_association * sctpAssociation, const uint8_t * data, size_t len)
	{
		//WARNING_EX_LOG("");
		// Ignore if destroying.
		// NOTE: This is because when the child class (i.e. WebRtcTransport) is deleted,
		// its destructor is called first and then the parent Transport's destructor,
		// and we would end here calling SendSctpData() which is an abstract method.
		/*if (this->destroying)
			return;*/

		if (this->m_sctp_association_ptr)
		{
			send_sctp_data(data, len);
		}
	}

	void crtc_transport::OnSctpAssociationMessageReceived(csctp_association * sctpAssociation, uint16_t streamId, uint32_t ppid, const uint8_t * msg, size_t len)
	{
		//WARNING_EX_LOG("[hex = %s]", str2hex((const char *)msg, len).c_str());
		
		// TODO@chensong 2023-04-20
		// data channel -->>> >>
		/*if (m_rtc_net_state != ERtcNetworkStateEstablished)
		{
			WARNING_EX_LOG();
			return;
		}*/


		ctransport_mgr::STREAM_URL_MAP::iterator iter = g_transport_mgr.m_all_stream_url_map.find(m_rtc_master.m_room_name + "/" + m_rtc_master.m_user_name);
		
		if (iter != g_transport_mgr.m_all_stream_url_map.end())
		{
			if (iter->second)
			{
				iter->second->send_sctp_data(0, ppid, msg, len);
			}
			
		}
		/*if (m_sctp_association_ptr)
		{
			m_sctp_association_ptr->SendSctpMessage(streamId, ppid, msg, len);
		}*/
		//send_sctp_data(msg, len);
		//RTC::DataProducer* dataProducer = this->sctpListener.GetDataProducer(streamId);

		//if (!dataProducer)
		//{
		//	MS_WARN_TAG(
		//		sctp, "no suitable DataProducer for received SCTP message [streamId:%" PRIu16 "]", streamId);

		//	return;
		//}

		//// Pass the SCTP message to the corresponding DataProducer.
		//try
		//{
		//	dataProducer->ReceiveMessage(ppid, msg, len);
		//}
		//catch (std::exception& error)
		//{
		//	// Nothing to do.
		//}
	}

	void crtc_transport::OnSctpAssociationBufferedAmount(csctp_association * sctpAssociation, uint32_t len)
	{
		//WARNING_EX_LOG("");
	}

	void crtc_transport::_on_stun_data_received(cudp_socket * socket, const uint8_t * data, size_t len, const sockaddr * remoteAddr)
	{
		//NORMAL_EX_LOG(" [address=%s:%u]", m_tcp_connection_ptr->GetPeerIp().c_str(), m_tcp_connection_ptr->GetPeerPort());
		crtc_stun_packet stun_packet;
		if (0 != stun_packet.decode((const char *)(data), len) )
		{
			WARNING_EX_LOG("stun decode packet failed !!!");
			return ;
		}
		if (!stun_packet.is_binding_request())
		{
			WARNING_EX_LOG("stun not binding request failed !!!");
			return;
		} 

		char buf[1500] = {0};
		cbuffer  stream(buf, sizeof(buf));
		{
			crtc_stun_packet stun_response;
			stun_response.set_message_type(EBindingResponse);
			stun_response.set_local_ufrag(stun_packet.get_remote_ufrag());
			stun_response.set_remote_ufrag(stun_packet.get_local_ufrag());
			stun_response.set_transcation_id(stun_packet.get_transcation_id());
			// FIXME: inet_addr is deprecated, IPV6 support
			int32 family;
			uint16_t port;
			std::string ip;

			uv_ip::GetAddressInfo(remoteAddr, family, ip, port);
			//NORMAL_EX_LOG("[remote_ip = %s][remote_port = %u]",  ip.c_str(), port);
			//NORMAL_EX_LOG(" [localip = %s][localport = %u]",  m_current_socket_ptr->GetLocalIp().c_str(), m_current_socket_ptr->GetLocalPort());
			stun_response.set_mapped_address(be32toh(inet_addr(ip.c_str())));
			stun_response.set_mapped_port(port);
			stun_response.encode(m_local_sdp.get_ice_pwd(), &stream);
			//m_current_socket_ptr->Send((const uint8_t *)stream.data(), stream.pos(), remoteAddr, nullptr);
			m_tcp_connection_ptr->Send((const uint8_t *)stream.data(), stream.pos() , nullptr);
			
		}
		//NORMAL_EX_LOG("[m_time_out_ms = %u]", uv_util::GetTimeMs() - m_time_out_ms);
		m_time_out_ms = uv_util::GetTimeMs();
		
		if (m_rtc_net_state != ERtcNetworkStateDtls && m_rtc_net_state != ERtcNetworkStateEstablished)
		{
			m_rtc_net_state = ERtcNetworkStateDtls;
			//cdtls_client * dtls_client_ptr = dynamic_cast<cdtls_client*>(m_dtls_ptr);
			//dtls_client_ptr->init();
			m_dtls_ptr->start_active_handshake("server");
		}
		


	}

	void crtc_transport::_on_dtls_data_received(cudp_socket* socket, const uint8_t* data, size_t len, const sockaddr * remoteAddr)
	{
		// TODO@chensong 2023-02-17 
		 // DTLS 的状态是否在连接中   connecting -> connected
		//NORMAL_EX_LOG(" [address=%s:%u]", m_tcp_connection_ptr->GetPeerIp().c_str(), m_tcp_connection_ptr->GetPeerPort());
		if (true/* connected  conecting*/)
		{
			m_dtls_ptr->process_dtls_data(data, len);
		}
	}

	void crtc_transport::_on_rtp_data_received(cudp_socket * socket, const uint8 * data, size_t len, const sockaddr * remoteAddr)
	{
		//NORMAL_EX_LOG(" [address=%s:%u]", m_tcp_connection_ptr->GetPeerIp().c_str(), m_tcp_connection_ptr->GetPeerPort());
		if (!m_srtp_recv_session_ptr)
		{
			WARNING_EX_LOG("srtp recv session ptr = null !!!");
			return;
		}
		// TODO@chensong 2023-2-23 native rtc 客户端推流  会有一个522长度数据 srtp 解码崩溃问题 --> 
		if (len == 522 /*|| len == 512*/)
		{
			//WARNING_EX_LOG("rtp %u data [%s]",len, str2hex((const char *)data, len).c_str());
			static uint32 count = 0;
		//	++count;
			std::string file_name = "./rtp/rtp.core."+std::to_string(len)+"._" + std::to_string(++count) + ".yuv";
			FILE * out_rtp_core_ptr = fopen(file_name.c_str(), "wb+");
			if (out_rtp_core_ptr)
			{
				fwrite(data, len, 1, out_rtp_core_ptr);
				fflush(out_rtp_core_ptr);
				fclose(out_rtp_core_ptr);
				out_rtp_core_ptr = NULL;
			}
			
			return;
		}

		if (!m_srtp_recv_session_ptr->DecryptSrtp((uint8_t *)data, &len))
		{
			WARNING_EX_LOG("rtp unprotect rtp failed !!!-------->>>>>>>");
		}
		else
		{
			//NORMAL_EX_LOG("rtp unprotect rtp OK !!!-------->>>>>>>");
			RTC::RtpPacket* packet = RTC::RtpPacket::Parse(data, len);

			if (!packet)
			{
				WARNING_EX_LOG("rtp, received data is not a valid RTP packet");

				return;
			}


			// Apply the Transport RTP header extension ids so the RTP listener can use them.
			packet->SetMidExtensionId(m_rtp_header_extension_ids.mid);
			 packet->SetRidExtensionId(m_rtp_header_extension_ids.rid);
			 packet->SetRepairedRidExtensionId(m_rtp_header_extension_ids.rrid);
			packet->SetAbsSendTimeExtensionId(m_rtp_header_extension_ids.absSendTime);
			packet->SetTransportWideCc01ExtensionId(m_rtp_header_extension_ids.transportWideCc01);

			auto nowMs = uv_util::GetTimeMs();
			//packet->SetTimestamp(nowMs);
			//NORMAL_EX_LOG("[pakcet timestamp = %u][cur nowms = %u][ms = %u]", packet->GetTimestamp(), ::time(NULL), time(NULL) - packet->GetTimestamp());
			// Feed the TransportCongestionControlServer.
			if (m_tcc_server )
			{
				m_tcc_server->IncomingPacket(nowMs, packet);
			}
			//NORMAL_EX_LOG("[payload_type = %u]", packet->GetPayloadType());

			crtc_producer * producer_ptr = m_all_rtp_listener.get_producer(packet );
			if (!producer_ptr)
			{
				WARNING_EX_LOG("not find ssrc =%u failed !!!", packet->GetSsrc());
				return;
			}


			//if (m_all_video_ssrc == packet->GetSsrc())
			//{
			//	m_remote_estimator.on_packet_arrival(packet->GetSequenceNumber(), packet->GetSsrc(), packet->GetTimestamp());
			//	RTC::Codecs::H264::ProcessRtpPacket(packet);
			//	//packet->Dump();
			//	//NORMAL_EX_LOG("[video][rtp ][ssrc = %u][size = %u][GetSequenceNumber = %u][GetPayloadType = %u][timestamp = %u][marker = %u]", packet->GetSsrc(), len, packet->GetSequenceNumber(), packet->GetPayloadType(), packet->GetTimestamp(), packet->HasMarker());
			//}
			//else  if (m_all_rtx_video_ssrc == packet->GetSsrc())
			//{
			//	//NORMAL_EX_LOG("[video_rtx][rtp ][ssrc = %u][GetSequenceNumber = %u][GetPayloadType = %u]", packet->GetSsrc(), packet->GetSequenceNumber(), packet->GetPayloadType());

			//}
			//bool send_audio = true;
			//crtp_stream::crtp_stream_params params;
			//params.type = "auido";
			//for (const cmedia_desc & media_ : m_remote_sdp.m_media_descs)
			//{
			//	for (const cssrc_group& ssrc_group : media_.m_ssrc_groups)
			//	{
			//		for (const uint32 ssrc : ssrc_group.m_ssrcs)
			//		{
			//			if (ssrc == packet->GetSsrc())
			//			{
			//				if (media_.m_type == "audio")
			//				{


			//				}
			//				else
			//				{
			//					params.type = "video";
			//					//send_audio = false;
			//				}
			//				break;
			//			}
			//		}
			//	}
			//}
			
			//crtc_producer::mangle_rtp_packet(packet, producer_ptr->get_rtcp_params().params/*params*/);
			producer_ptr->receive_rtp_packet(packet);
			 
			
			
		}
	}

	void crtc_transport::_on_rtcp_data_received(cudp_socket * socket, const uint8 * data, size_t len, const sockaddr * remoteAddr)
	{
		//if (!m_srtp.unprotect_rtcp(/*static_cast<void*>*//*(void *)(data)*/const_cast<uint8_t*>(data), reinterpret_cast<int32*>(&len)))
		//{
		//	WARNING_EX_LOG("rtcp unprotect rtcp failed !!!-------->>>>>>>");
		//}
		//else
		//{
		//	WARNING_EX_LOG("rtcp unprotect rtcp OK !!!-------->>>>>>>");
		//}
		//NORMAL_EX_LOG(" [address=%s:%u]", m_tcp_connection_ptr->GetPeerIp().c_str(), m_tcp_connection_ptr->GetPeerPort());
		if (!m_srtp_recv_session_ptr->DecryptSrtcp((uint8_t *)data, &len))
		{
			WARNING_EX_LOG("rtcp unprotect rtp failed !!!-------->>>>>>>");
		}
		else

		{
			//NORMAL_EX_LOG("rtcp unprotect rtp OK !!!-------->>>>>>>");
			RTC::RTCP::Packet* packet = RTC::RTCP::Packet::Parse(data, len);

			if (!packet)
			{
				WARNING_EX_LOG( "rtcp received data is not a valid RTCP compound or single packet");

				return;
			}
			// Handle each RTCP packet.
			while (packet)
			{
				_handler_rtcp_packet(packet);

				auto* previousPacket = packet;

				packet = packet->GetNext();

				delete previousPacket;
			}
			return;
			cbuffer buffer((char *)data, len);

			crtcp_compound rtcp_compound;
			if (!rtcp_compound.decode(&buffer))
			{
				WARNING_EX_LOG("decode rtcp plaintext=%u, bytes=[%s], at=%s", len,
					str2hex((  char *)data, len, 8).c_str(),
					str2hex((  char *)buffer.head(), buffer.left(), 8).c_str());
				return;
			}

			crtcp_common* rtcp = NULL;
			while (NULL != (rtcp = rtcp_compound.get_next_rtcp()))
			{
				bool success  = _dispatch_rtcp(rtcp);
			//	SrsAutoFree(SrsRtcpCommon, rtcp);
				if (rtcp)
				{
					delete rtcp;
					rtcp = NULL;
				}
				if (success != true)
				{
					WARNING_EX_LOG("plaintext=%u, bytes=[%s], rtcp=(%u,%u,%u,%u)", len,
						str2hex(rtcp->data(), rtcp->size(), rtcp->size()).c_str(),
						rtcp->get_rc(), rtcp->type(), rtcp->get_ssrc(), rtcp->size());
					return;
				}
			}

			 
		}
	}

	void crtc_transport::_on_application_data_receviced(const uint8 * data, size_t len)
	{
		if (!m_sctp_association_ptr)
		{
			DEBUG_EX_LOG("ignoring SCTP packet (SCTP not enabled)");
			return;
		}
		m_sctp_association_ptr->ProcessSctpData(data, len);
	}

	void crtc_transport::_handler_rtcp_packet(RTC::RTCP::Packet * packet)
	{
		switch (packet->GetType())
		{
			case RTC::RTCP::Type::RR:
			{
				//DEBUG_EX_LOG("RTC::RTCP::Type::RR");
				RTC::RTCP::ReceiverReportPacket* rr = static_cast<RTC::RTCP::ReceiverReportPacket*>(packet);
				//rr->Dump();
				for (auto it = rr->Begin(); it != rr->End(); ++it)
				{
					//RTC::RTCP::ReceiverReport& report = *it;
					auto* consumer = m_all_rtp_listener.get_consumer((*it)->GetSsrc());

					if (!consumer)
					{
						// Special case for the RTP probator.
						if ((*it)->GetSsrc() == RTC::RtpProbationSsrc)
						{
							continue;
						}

						DEBUG_EX_LOG( "rtcp no Consumer found for received Receiver Report [ssrc:%" PRIu32 "]",
							(*it)->GetSsrc());

						continue;
					}

					consumer->receive_rtcp_receiver_report((*it));
				}

				{
					 
					if (m_tcc_client && !m_all_rtp_listener.m_ssrc_consumer_table.empty())
					{
						float rtt = 0;

						// Retrieve the RTT from the first active consumer.
						for (auto& kv : m_all_rtp_listener.m_ssrc_consumer_table)
						{
							auto* consumer = kv.second;

							//if (consumer->IsActive())
							{
								rtt = consumer->get_rtt();

								break;
							}
						}

						m_tcc_client->ReceiveRtcpReceiverReport(rr, rtt, uv_util::GetTimeMsInt64());
					}

				}
				break;
			}

			case RTC::RTCP::Type::PSFB:
			{
				//DEBUG_EX_LOG("RTC::RTCP::Type::PSFB");
				auto* feedback = static_cast<RTC::RTCP::FeedbackPsPacket*>(packet);

				switch (feedback->GetMessageType())
				{
				case RTC::RTCP::FeedbackPs::MessageType::PLI:
				{
					//DEBUG_EX_LOG("RTC::RTCP::FeedbackPs::MessageType::PLI");
					auto* consumer = m_all_rtp_listener.get_consumer(feedback ->GetMediaSsrc());
					if (!consumer)
					{
						WARNING_EX_LOG( "rtcp no Consumer found for received PLI Feedback packet "
							"[sender ssrc:%" PRIu32 ", media ssrc:%" PRIu32 "]",
							feedback->GetSenderSsrc(),
							feedback->GetMediaSsrc());

						break;
					}
					
					NORMAL_EX_LOG(
						"rtcp PLI received, requesting key frame for Consumer "
						"[sender ssrc:%" PRIu32 ", media ssrc:%" PRIu32 "]",
						feedback->GetSenderSsrc(),
						feedback->GetMediaSsrc());

					 consumer->receive_key_frame_request( RTC::RTCP::FeedbackPs::MessageType::PLI, feedback->GetMediaSsrc());

					break;
				}

				case RTC::RTCP::FeedbackPs::MessageType::FIR:
				{
				//	DEBUG_EX_LOG("RTC::RTCP::FeedbackPs::MessageType::FIR");
					// Must iterate FIR items.
					auto* fir = static_cast<RTC::RTCP::FeedbackPsFirPacket*>(packet);

					for (auto it = fir->Begin(); it != fir->End(); ++it)
					{
						crtc_consumer* consumer = m_all_rtp_listener.get_consumer((*it)->GetSsrc());
						if (!consumer)
						{
							DEBUG_EX_LOG( "no Consumer found for received FIR Feedback packet "
								"[sender ssrc:%" PRIu32 ", media ssrc:%" PRIu32 ", item ssrc:%" PRIu32 "]",
								feedback->GetSenderSsrc(),
								feedback->GetMediaSsrc(),
								(*it)->GetSsrc());
							continue;;
						}
						 /*auto& item = *it;
						auto* consumer = GetConsumerByMediaSsrc(item->GetSsrc());

						if (item->GetSsrc() == RTC::RtpProbationSsrc)
						{
							continue;
						}
						else if (!consumer)
						{
							MS_DEBUG_TAG(
								rtcp,
								"no Consumer found for received FIR Feedback packet "
								"[sender ssrc:%" PRIu32 ", media ssrc:%" PRIu32 ", item ssrc:%" PRIu32 "]",
								feedback->GetSenderSsrc(),
								feedback->GetMediaSsrc(),
								item->GetSsrc());

							continue;
						}*/

						NORMAL_EX_LOG(" rtcp, FIR received, requesting key frame for Consumer "
							"[sender ssrc:%" PRIu32 ", media ssrc:%" PRIu32 ", item ssrc: "  PRIu32 "]",
							feedback->GetSenderSsrc(),
							feedback->GetMediaSsrc() ,
						(*it)->GetSsrc()) ;

						 consumer->receive_key_frame_request(feedback->GetMessageType(), (*it)->GetSsrc());
					}

					break;
				}

				case RTC::RTCP::FeedbackPs::MessageType::AFB:
				{
				//	DEBUG_EX_LOG("RTC::RTCP::FeedbackPs::MessageType::AFB");
					RTC::RTCP::FeedbackPsAfbPacket* afb = static_cast<RTC::RTCP::FeedbackPsAfbPacket*>(feedback);

					// Store REMB info.
					if (afb->GetApplication() == RTC::RTCP::FeedbackPsAfbPacket::Application::REMB)
					{
						auto* remb = static_cast<RTC::RTCP::FeedbackPsRembPacket*>(afb);

						// Pass it to the TCC client.
						// clang-format off
						if (
							this->m_tcc_client &&
							this->m_tcc_client->GetBweType() == RTC::BweType::REMB
							)
							// clang-format on
						{
							this->m_tcc_client->ReceiveEstimatedBitrate(remb->GetBitrate());
						}

						break;
					}
					else
					{
						DEBUG_EX_LOG( "rtcp ignoring unsupported %s Feedback PS AFB packet "
							"[sender ssrc:%" PRIu32 ", media ssrc:%" PRIu32 "]",
							RTC::RTCP::FeedbackPsPacket::MessageType2String(feedback->GetMessageType()).c_str(),
							feedback->GetSenderSsrc(),
							feedback->GetMediaSsrc());

						break;
					}
				}

				default:
				{
					NORMAL_EX_LOG("rtcp, ignoring unsupported %s Feedback packet "
						"[sender ssrc:%" PRIu32 ", media ssrc:%" PRIu32 "]",
						RTC::RTCP::FeedbackPsPacket::MessageType2String(feedback->GetMessageType()).c_str(),
						feedback->GetSenderSsrc(),
						feedback->GetMediaSsrc());
				}
				}

				break;
			}

			case RTC::RTCP::Type::RTPFB:
			{
				//DEBUG_EX_LOG("RTC::RTCP::Type::RTPFB");
				auto* feedback = static_cast<RTC::RTCP::FeedbackRtpPacket*>(packet);
				//auto* consumer = GetConsumerByMediaSsrc(feedback->GetMediaSsrc());
				crtc_consumer* consumer = m_all_rtp_listener.get_consumer(feedback->GetMediaSsrc());
				// If no Consumer is found and this is not a Transport Feedback for the
				// probation SSRC or any Consumer RTX SSRC, ignore it.
				//
				// clang-format off
				if (!consumer && feedback->GetMessageType() != RTC::RTCP::FeedbackRtp::MessageType::TCC &&
					( feedback->GetMediaSsrc() != RTC::RtpProbationSsrc ||
						!m_all_rtp_listener.get_consumer(feedback->GetMediaSsrc()) ) )
					// clang-format on
				{
					DEBUG_EX_LOG( "rtcp , no Consumer found for received Feedback packet "
						"[sender ssrc:%" PRIu32 ", media ssrc:%" PRIu32 "]",
						feedback->GetSenderSsrc(),
						feedback->GetMediaSsrc());

					break;
				}

				switch (feedback->GetMessageType())
				{
					case RTC::RTCP::FeedbackRtp::MessageType::NACK:
					{
						//DEBUG_EX_LOG("[%s] RTC::RTCP::FeedbackRtp::MessageType::NACK, ", m_local_sdp.m_msids[0].c_str());
						 if (!consumer)
						{
							DEBUG_EX_LOG( "rtcp , no Consumer found for received NACK Feedback packet "
								"[sender ssrc:%" PRIu32 ", media ssrc:%" PRIu32 "]",
								feedback->GetSenderSsrc(),
								feedback->GetMediaSsrc());

							break;
						}

						auto* nackPacket = static_cast<RTC::RTCP::FeedbackRtpNackPacket*>(packet);

						consumer->receive_nack(nackPacket); 

						break;
					}

					case RTC::RTCP::FeedbackRtp::MessageType::TCC:
					{
					//	DEBUG_EX_LOG("RTC::RTCP::FeedbackRtp::MessageType::TCC");
						auto* feedback = static_cast<RTC::RTCP::FeedbackRtpTransportPacket*>(packet);

						if (m_tcc_client)
						{
							m_tcc_client->ReceiveRtcpTransportFeedback(feedback);
						}

		//#ifdef ENABLE_RTC_SENDER_BANDWIDTH_ESTIMATOR
		//				// Pass it to the SenderBandwidthEstimator client.
		//				if (this->senderBwe)
		//					this->senderBwe->ReceiveRtcpTransportFeedback(feedback);
		//#endif

						break;
					}

					default:
					{
						NORMAL_EX_LOG(" rtcp, ignoring unsupported %s Feedback packet "
							"[sender ssrc:%" PRIu32 ", media ssrc:%" PRIu32 "]",
							RTC::RTCP::FeedbackRtpPacket::MessageType2String(feedback->GetMessageType()).c_str(),
							feedback->GetSenderSsrc(),
							feedback->GetMediaSsrc());
					}
				}

				break;
			}

			case RTC::RTCP::Type::SR:
			{
				RTC::RTCP::SenderReportPacket* sr = static_cast<RTC::RTCP::SenderReportPacket*>(packet);
			//	DEBUG_EX_LOG("RTC::RTCP::Type::SR");
				//sr->Dump();
				// Even if Sender Report packet can only contains one report.
				for (auto it = sr->Begin(); it != sr->End(); ++it)
				{
					auto& report = *it;
					auto* producer = m_all_rtp_listener.get_producer(report->GetSsrc());//this->rtpListener.GetProducer(report->GetSsrc());

					if (!producer)
					{
						DEBUG_EX_LOG( "no Producer found for received Sender Report [ssrc:%" PRIu32 "]",
							report->GetSsrc());

						continue;
					}

					producer->receive_rtcp_sender_report(report);
				} 

				break;
			}

			case RTC::RTCP::Type::SDES:
			{
				//DEBUG_EX_LOG("RTC::RTCP::Type::SDES");
				// According to RFC 3550 section 6.1 "a CNAME item MUST be included in
				// in each compound RTCP packet". So this is true even for compound
				// packets sent by endpoints that are not sending any RTP stream to us
				// (thus chunks in such a SDES will have an SSCR does not match with
				// any Producer created in this Transport).
				// Therefore, and given that we do nothing with SDES, just ignore them.

				break;
			}

			case RTC::RTCP::Type::BYE:
			{
				//DEBUG_EX_LOG("RTC::RTCP::Type::BYE");
				//NORMAL_EX_LOG("rtcp, ignoring received RTCP BYE");

				break;
			}

			case RTC::RTCP::Type::XR:
			{
			//	DEBUG_EX_LOG("RTC::RTCP::Type::XR");
				auto* xr = static_cast<RTC::RTCP::ExtendedReportPacket*>(packet);

				for (auto it = xr->Begin(); it != xr->End(); ++it)
				{
					auto& report = *it;

					switch (report->GetType())
					{
					case RTC::RTCP::ExtendedReportBlock::Type::DLRR:
					{
						//DLRR(Delay Last Receiver Report) 是 RTCP(Real - Time Control Protocol) 中的一种报告类型，
						//用于反映接收方在上一次发送 RTCP 报告后收到的最后一个 RTP 包的时间戳和序列号。
						//DLRR 信息可以被发送方用来计算网络延迟以及推断音视频质量情况。在 WebRTC 中，RTCP 报告可以帮助调整编码参数，
						// 控制发包速率，以提高通信质量。webrtc中rtcp中DLRR
						auto* dlrr = static_cast<RTC::RTCP::DelaySinceLastRr*>(report);

						for (auto it2 = dlrr->Begin(); it2 != dlrr->End(); ++it2)
						{
							auto& ssrcInfo = *it2;

							// SSRC should be filled in the sub-block.
							if (ssrcInfo->GetSsrc() == 0)
							{
								ssrcInfo->SetSsrc(xr->GetSsrc());

							}
							auto iter = this->m_all_rtp_listener.m_ssrcTable.find((ssrcInfo->GetSsrc()));

							if (iter == this->m_all_rtp_listener.m_ssrcTable.end())
							{
								DEBUG_EX_LOG(
									"rtcp, no Producer found for received Sender Extended Report [ssrc:%" PRIu32 "]",
									ssrcInfo->GetSsrc());

								continue;
							} 

							iter->second ->receive_rtcp_xrdelay_since_lastrr(ssrcInfo);
						}

						break;
					}

					default:;
					}
				}

				break;
			}

			default:
			{
				NORMAL_EX_LOG("rtcp, unhandled RTCP type received [type:%"  "]",
					static_cast<uint8_t>(packet->GetType()));
			}
		}
	}

	bool crtc_transport::_dispatch_rtcp(crtcp_common * rtcp)
	{
		// The feedback packet for specified SSRC.
		// For example, if got SR packet, we required a publisher to handle it.
		uint32 required_publisher_ssrc = 0;
		uint32 required_player_ssrc = 0;
		switch (rtcp->type())
		{
			case ERtcpType_rr:
			{
				DEBUG_EX_LOG("RR");
				crtcp_rr* rr = dynamic_cast<crtcp_rr*>(rtcp);
				if (rr->get_rb_ssrc() == 0)
				{ //for native client
					return true;
				}
				break;
			}
			case ERtcpType_psfb:
			{
				DEBUG_EX_LOG("ERtcpType_psfb");
				crtcp_psfb_common* psfb = dynamic_cast<crtcp_psfb_common*>(rtcp);
				switch (psfb->get_rc())
				{
					case EPSAFB:
					{
						return _on_rtcp_feedback_remb(psfb);
						break;
					}
					default:
					{
						WARNING_EX_LOG("psfb type = %u", psfb->get_rc());
						break;
					}
				}
				break;
			}
			case ERtcpType_rtpfb:
			{
				DEBUG_EX_LOG("ERtcpType_rtpfb");
				switch (rtcp->get_rc())
				{
					case ERtpfbTCC:
					{
						return _on_rtcp_feedback_twcc(rtcp->data(), rtcp->size());
						break;
					}
					case ERtpfbNACK:
					{
						crtcp_nack* nack = dynamic_cast<crtcp_nack*>(rtcp);
						required_player_ssrc = nack->get_media_ssrc();
						//DEBUG_EX_LOG("ERtcpType_rtpfb[ftpfb][required_player_ssrc = %u]", required_player_ssrc);
						break;
					}
					default:
					{
						WARNING_EX_LOG("rtcp ftpfb subtype = %u", rtcp->get_rc());
						break;
					}
				}
				break;
			}
			case ERtcpType_sr:
			{
				crtcp_sr* sr = dynamic_cast<crtcp_sr*>(rtcp);
				/*crtc_producer* producer_ptr = m_all_rtp_listener.get_producer(sr->get_ssrc());
				if (producer_ptr)
				{
					producer_ptr->receive_rtcp_sender_report(sr);
				}*/
				required_publisher_ssrc = sr->get_ssrc();
				DEBUG_EX_LOG("ERtcpType_sr [required_publisher_ssrc = %u]", required_publisher_ssrc);
				
				break;
			}
			case ERtcpType_sdes:
			{
				// According to RFC 3550 section 6.1 "a CNAME item MUST be included in
				// in each compound RTCP packet". So this is true even for compound
				// packets sent by endpoints that are not sending any RTP stream to us
				// (thus chunks in such a SDES will have an SSCR does not match with
				// any Producer created in this Transport).
				// Therefore, and given that we do nothing with SDES, just ignore them.

				break;
			}
			case ERtcpType_bye:
			{ 
				//NORMAL_EX_LOG("ignoring received RTCP BYE");
				break;
			}
			case ERtcpType_xr:
			{
				DEBUG_EX_LOG("ERtcpType_xr");
				switch (rtcp->get_rc())
				{
					case EExtendedDLRR:
					{
						break;
					}
					default:
					{
						WARNING_EX_LOG("rtcp xr sub type = %u", rtcp->get_rc());
						break;
					}
				}
				break;
			}
			default:
			{
				WARNING_EX_LOG("rtcp type = %u", rtcp->type());
				break;
			}
		}

		(void)required_publisher_ssrc;
		(void)required_player_ssrc;



		// For TWCC packet.
		//if (ERtcpType_rtpfb == rtcp->type() && ETCC == rtcp->get_rc())
		//{
		//	return _on_rtcp_feedback_twcc(rtcp->data(), rtcp->size());
		//}

		//// For REMB packet.
		//if (ERtcpType_psfb == rtcp->type())
		//{
		//	crtcp_psfb_common* psfb = dynamic_cast<crtcp_psfb_common*>(rtcp);
		//	if (EAFB == psfb->get_rc())
		//	{
		//		return _on_rtcp_feedback_remb(psfb);
		//	}
		//}

		//// Ignore special packet.
		//if (ERtcpType_rr == rtcp->type()) 
		//{
		//	crtcp_rr* rr = dynamic_cast<crtcp_rr*>(rtcp);
		//	if (rr->get_rb_ssrc() == 0) 
		//	{ //for native client
		//		return true;
		//	}
		//}

		//
		//if (ERtcpType_sr == rtcp->type()) 
		//{
		//	required_publisher_ssrc = rtcp->get_ssrc();
		//}
		//else if (ERtcpType_rr == rtcp->type())
		//{
		//	crtcp_rr* rr = dynamic_cast<crtcp_rr*>(rtcp);
		//	required_player_ssrc = rr->get_rb_ssrc();
		//}
		//else if (ERtcpType_rtpfb == rtcp->type()) 
		//{
		//	if (1 == rtcp->get_rc()) 
		//	{
		//		crtcp_nack* nack = dynamic_cast<crtcp_nack*>(rtcp);
		//		required_player_ssrc = nack->get_media_ssrc();
		//	}
		//}
		//else if (ERtcpType_psfb == rtcp->type()) 
		//{
		//	crtcp_psfb_common* psfb = dynamic_cast<crtcp_psfb_common*>(rtcp);
		//	required_player_ssrc = psfb->get_media_ssrc();
		//}

		// Find the publisher or player by SSRC, always try to got one.
		//RtcPlayStream* player = NULL;
		//RtcPublishStream* publisher = NULL;
		//if (true) 
		//{
		//	uint32  ssrc = required_publisher_ssrc ? required_publisher_ssrc : rtcp->get_ssrc();
		//	std::map<uint32, RtcPublishStream*>::iterator it = publishers_ssrc_map_.find(ssrc);
		//	if (it != publishers_ssrc_map_.end()) 
		//	{
		//		publisher = it->second;
		//	}
		//}

		//if (true) {
		//	uint32 ssrc = required_player_ssrc ? required_player_ssrc : rtcp->get_ssrc();
		//	std::map<uint32, RtcPlayStream*>::iterator it = players_ssrc_map_.find(ssrc);
		//	if (it != players_ssrc_map_.end()) {
		//		player = it->second;
		//	}
		//}

		//// Ignore if packet is required by publisher or player.
		//if (required_publisher_ssrc && !publisher)
		//{
		//	WARNING_EX_LOG("no ssrc %u in publishers. rtcp type:%u", required_publisher_ssrc, rtcp->type());
		//	return err;
		//}
		//if (required_player_ssrc && !player) 
		//{
		//	WARNING_EX_LOG("no ssrc %u in players. rtcp type:%u", required_player_ssrc, rtcp->type());
		//	return err;
		//}

		//// Handle packet by publisher or player.
		//if (publisher && srs_success != (err = publisher->on_rtcp(rtcp))) 
		//{
		//	return srs_error_wrap(err, "handle rtcp");
		//}
		//if (player && srs_success != (err = player->on_rtcp(rtcp))) {
		//	return srs_error_wrap(err, "handle rtcp");
		//}
		return true;
	}

	void crtc_transport::OnTimer(ctimer * timer)
	{
		// RTCP timer.
		if (timer == m_timer_ptr)
		{
			auto interval = static_cast<uint64_t>(RTC::RTCP::MaxVideoIntervalMs);
			uint64_t nowMs = uv_util::GetTimeMs();
		//	m_remote_estimator.send_periodic_Feedbacks();
			//SendRtcp(nowMs);
			std::unique_ptr<RTC::RTCP::CompoundPacket> packet{ nullptr };

			for (std::pair<const uint32, crtc_consumer*> & p : m_all_rtp_listener.m_ssrc_consumer_table)
			{
				// Reset the Compound packet.
				packet.reset(new RTC::RTCP::CompoundPacket());
				p.second->get_rtcp(packet.get(), nowMs);

				// Send the RTCP compound packet if there is a sender report.
				if (packet->HasSenderReport())
				{
					packet->Serialize(RTC::RTCP::Buffer);
					send_rtcp_compound_packet(packet.get());
					//SendRtcpCompoundPacket(packet.get());
				}
			}
			// Reset the Compound packet.
			packet.reset(new RTC::RTCP::CompoundPacket());
			for (std::pair<const uint32, crtc_producer*> & p : m_all_rtp_listener.m_ssrcTable)
			{
				// Reset the Compound packet.
				packet.reset(new RTC::RTCP::CompoundPacket());
				p.second->get_rtcp(packet.get(), nowMs);

				// Send the RTCP compound packet if there is a sender report.
				if (packet->HasSenderReport())
				{
					packet->Serialize(RTC::RTCP::Buffer);
					send_rtcp_compound_packet(packet.get());
					//SendRtcpCompoundPacket(packet.get());
				}
			}
			if (packet->GetReceiverReportCount() != 0u)
			{
				packet->Serialize(RTC::RTCP::Buffer);
				send_rtcp_compound_packet(packet.get());
			}
			// Recalculate next RTCP interval.
			//if (!this->mapConsumers.empty())
			//{
			//	// Transmission rate in kbps.
			//	uint32_t rate{ 0 };

			//	// Get the RTP sending rate.
			//	for (auto& kv : this->mapConsumers)
			//	{
			//		auto* consumer = kv.second;

			//		rate += consumer->GetTransmissionRate(nowMs) / 1000;
			//	}

			//	// Calculate bandwidth: 360 / transmission bandwidth in kbit/s.
			//	if (rate != 0u)
			//		interval = 360000 / rate;

			//	if (interval > RTC::RTCP::MaxVideoIntervalMs)
			//		interval = RTC::RTCP::MaxVideoIntervalMs;
			//}
			interval = RTC::RTCP::MaxVideoIntervalMs;
			/*
			 * The interval between RTCP packets is varied randomly over the range
			 * [0.5,1.5] times the calculated interval to avoid unintended synchronization
			 * of all participants.
			 */
			interval *= static_cast<float>(s_crypto_random.GetRandomUInt(5, 15)) / 10;

			m_timer_ptr->Start(interval);
		}
	}

	bool crtc_transport::_on_rtcp_feedback_twcc(char * data, int32 nb_data)
	{
		return true;
	}

	bool crtc_transport::_on_rtcp_feedback_remb(crtcp_psfb_common * rtcp)
	{
		//ignore REMB
		return true;
	}

	inline bool crtc_transport::_mangle_rtp_packet(  RTC::RtpPacket * packet, bool audio_video)
	{
		// Mangle the payload type.
 
		// Mangle the SSRC.
		//{
		//	// 2.  修改为服务端ssrc   这个可能是因为不同端可能会冲突的问题吧
		//	 
		//	packet->SetSsrc(mappedSsrc);
		//}
		 
		// Mangle RTP header extensions.
		{
			thread_local static uint8_t buffer[4096];
			thread_local static std::vector<RTC::RtpPacket::GenericExtension> extensions;

			// This happens just once.
			if (extensions.capacity() != 24)
			{
				extensions.reserve(24);
			}

			extensions.clear();

			uint8_t* extenValue;
			uint8_t extenLen;
			uint8_t* bufferPtr{ buffer };

			// Add urn:ietf:params:rtp-hdrext:sdes:mid.
			{
				extenLen = RTC::MidMaxLength;

				extensions.emplace_back(
					static_cast<uint8_t>(EMID), extenLen, bufferPtr);

				bufferPtr += extenLen;
			}

			if (!audio_video)
			{
				// Proxy urn:ietf:params:rtp-hdrext:ssrc-audio-level.
				extenValue = packet->GetExtension(ESSRC_AUDIO_LEVEL, extenLen);

				if (extenValue)
				{
					std::memcpy(bufferPtr, extenValue, extenLen);

					extensions.emplace_back(
						static_cast<uint8_t>(ESSRC_AUDIO_LEVEL),
						extenLen,
						bufferPtr);

					// Not needed since this is the latest added extension.
					// bufferPtr += extenLen;
				}
			}
			else if (audio_video)
			{
				// Add http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time.
				{
					extenLen = 3u;

					// NOTE: Add value 0. The sending Transport will update it.
					uint32_t absSendTime{ 0u };

					rtc_byte::set3bytes(bufferPtr, 0, absSendTime);

					extensions.emplace_back(
						static_cast<uint8_t>(EABS_SEND_TIME), extenLen, bufferPtr);

					bufferPtr += extenLen;
				}

				// Add http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01.
				{
					extenLen = 2u;

					// NOTE: Add value 0. The sending Transport will update it.
					uint16_t wideSeqNumber{ 0u };

					rtc_byte::set3bytes(bufferPtr, 0, wideSeqNumber);

					extensions.emplace_back(
						static_cast<uint8_t>(ETRANSPORT_WIDE_CC_01),
						extenLen,
						bufferPtr);

					bufferPtr += extenLen;
				}

				// NOTE: Remove this once framemarking draft becomes RFC.
				// Proxy http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07.
				extenValue =   packet->GetExtension(EFRAME_MARKING_07, extenLen);

				if (extenValue)
				{
					std::memcpy(bufferPtr, extenValue, extenLen);

					extensions.emplace_back(
						static_cast<uint8_t>(EFRAME_MARKING_07),
						extenLen,
						bufferPtr);

					bufferPtr += extenLen;
				}

				// Proxy urn:ietf:params:rtp-hdrext:framemarking.
				extenValue =   packet->GetExtension(EFRAME_MARKING, extenLen);

				if (extenValue)
				{
					std::memcpy(bufferPtr, extenValue, extenLen);

					extensions.emplace_back(
						static_cast<uint8_t>(EFRAME_MARKING), extenLen, bufferPtr);

					bufferPtr += extenLen;
				}

				// Proxy urn:3gpp:video-orientation.
				extenValue = packet->GetExtension(EVIDEO_ORIENTATION, extenLen);

				if (extenValue)
				{
					std::memcpy(bufferPtr, extenValue, extenLen);

					extensions.emplace_back(
						static_cast<uint8_t>(EVIDEO_ORIENTATION),
						extenLen,
						bufferPtr);

					bufferPtr += extenLen;
				}

				// Proxy urn:ietf:params:rtp-hdrext:toffset.
				extenValue = packet->GetExtension(ETOFFSET, extenLen);

				if (extenValue)
				{
					std::memcpy(bufferPtr, extenValue, extenLen);

					extensions.emplace_back(
						static_cast<uint8_t>(ETOFFSET), extenLen, bufferPtr);

					// Not needed since this is the latest added extension.
					// bufferPtr += extenLen;
				}
			}

			// Set the new extensions into the packet using One-Byte format.
			packet->SetExtensions(1, extensions);

			// Assign mediasoup RTP header extension ids (just those that mediasoup may
			// be interested in after passing it to the Router).
			packet->SetMidExtensionId(static_cast<uint8_t>(EMID));
			packet->SetAbsSendTimeExtensionId(
				static_cast<uint8_t>(EABS_SEND_TIME));
			packet->SetTransportWideCc01ExtensionId(
				static_cast<uint8_t>(ETRANSPORT_WIDE_CC_01));
			// NOTE: Remove this once framemarking draft becomes RFC.
			packet->SetFrameMarking07ExtensionId(
				static_cast<uint8_t>(EFRAME_MARKING_07));
			packet->SetFrameMarkingExtensionId(
				static_cast<uint8_t>(EFRAME_MARKING));
			packet->SetSsrcAudioLevelExtensionId(
				static_cast<uint8_t>(ESSRC_AUDIO_LEVEL));
			packet->SetVideoOrientationExtensionId(
				static_cast<uint8_t>(EVIDEO_ORIENTATION));
		}

		return true;
		return true;
	}

	//bool crtc_transport::_negotiate_publish_capability(crtc_source_description * stream_desc)
	//{
	// 
	//	 
	//	if (!stream_desc) 
	//	{
	//		ERROR_EX_LOG("stream description is NULL");
	//		return false;
	//	//	return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "stream description is NULL");
	//	}

	//	bool nack_enabled = true;
	//	bool twcc_enabled = true;
	//	// TODO: FIME: Should check packetization-mode=1 also.
	//	bool has_42e01f =  sdp_has_h264_profile(m_remote_sdp, "42e01f");

	//	// How many video descriptions we have parsed.
	//	int32 nn_any_video_parsed = 0;

	//	for (int32 i = 0; i < (int32)m_remote_sdp.m_media_descs.size(); ++i) 
	//	{
	//		const cmedia_desc& remote_media_desc = m_remote_sdp.m_media_descs.at(i);

	//		if (remote_media_desc.is_video())
	//		{
	//			nn_any_video_parsed++;
	//		}

	//	/*	SrsRtcTrackDescription* track_desc = new SrsRtcTrackDescription();
	//		SrsAutoFree(SrsRtcTrackDescription, track_desc);*/
	//		crtc_track_description track_desc;
	//		 track_desc.set_direction("recvonly");
	//		 track_desc.set_mid(remote_media_desc.m_mid);
	//		// Whether feature enabled in remote extmap.
	//		int32 remote_twcc_id = 0;
	//		if (true) 
	//		{
	//			std::map<int32, std::string> extmaps = remote_media_desc.get_extmaps();
	//			for (std::map<int32, std::string>::iterator it = extmaps.begin(); it != extmaps.end(); ++it) 
	//			{
	//				if (it->second == kTWCCExt)
	//				{
	//					remote_twcc_id = it->first;
	//					break;
	//				}
	//			}
	//		}

	//		if (twcc_enabled && remote_twcc_id) 
	//		{
	//			track_desc.add_rtp_extension_desc(remote_twcc_id, kTWCCExt);
	//		}

	//		if (remote_media_desc.is_audio())
	//		{
	//			// Update the ruc, which is about user specified configuration.
	//			//ruc->audio_before_video_ = !nn_any_video_parsed;

	//			// TODO: check opus format specific param
	//			std::vector<cmedia_payload_type> payloads = remote_media_desc.find_media_with_encoding_name("opus");
	//			if (payloads.empty())
	//			{
	//				ERROR_EX_LOG("no valid found opus payload type");
	//				return false; 
	//			}

	//			for (int32 j = 0; j < (int32)payloads.size(); j++) 
	//			{
	//				const cmedia_payload_type& payload = payloads.at(j);

	//				// if the payload is opus, and the encoding_param_ is channel
	//				caudio_payload* audio_payload = new caudio_payload(payload.m_payload_type, payload.m_encoding_name, payload.m_clock_rate, ::atol(payload.m_encoding_param.c_str()));
	//				audio_payload->set_opus_param_desc(payload.m_format_specific_param);
	//				// AudioPayload* audio_payload = new SrsAudioPayload(payload.m_payload_type, payload.m_encoding_name, payload.m_clock_rate, ::atol(payload.m_encoding_param.c_str()));
	//				//audio_payload->set_opus_param_desc(payload.format_specific_param_);

	//				// TODO: FIXME: Only support some transport algorithms.
	//				for (int32 k = 0; k < (int32)payload.m_rtcp_fb.size(); ++k)
	//				{
	//					const std::string& rtcp_fb = payload.m_rtcp_fb.at(k);

	//					// 掉包重传的协议
	//					if (nack_enabled) 
	//					{
	//						if (rtcp_fb == "nack" || rtcp_fb == "nack pli") 
	//						{
	//							audio_payload->m_rtcp_fbs.push_back(rtcp_fb);
	//						}
	//					}
	//					// 网络带宽评估的协议
	//					if (twcc_enabled && remote_twcc_id) 
	//					{
	//						if (rtcp_fb == "transport-cc")
	//						{
	//							audio_payload->m_rtcp_fbs.push_back(rtcp_fb);
	//						}
	//					}
	//				}

	//				track_desc.m_type = "audio";
	//				track_desc.set_codec_payload(audio_payload);
	//				// Only choose one match opus codec.
	//				break;
	//			}
	//		}
	//		//else if (remote_media_desc.is_video() && ruc->codec_ == "av1") {
	//		//	std::vector<SrsMediaPayloadType> payloads = remote_media_desc.find_media_with_encoding_name("AV1");
	//		//	if (payloads.empty()) {
	//		//		// Be compatible with the Chrome M96, still check the AV1X encoding name
	//		//		// @see https://bugs.chromium.org/p/webrtc/issues/detail?id=13166
	//		//		payloads = remote_media_desc.find_media_with_encoding_name("AV1X");
	//		//	}
	//		//	if (payloads.empty()) {
	//		//		return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "no found valid AV1 payload type");
	//		//	}

	//		//	for (int j = 0; j < (int)payloads.size(); j++) {
	//		//		const SrsMediaPayloadType& payload = payloads.at(j);

	//		//		// Generate video payload for av1.
	//		//		SrsVideoPayload* video_payload = new SrsVideoPayload(payload.payload_type_, payload.encoding_name_, payload.clock_rate_);

	//		//		// TODO: FIXME: Only support some transport algorithms.
	//		//		for (int k = 0; k < (int)payload.rtcp_fb_.size(); ++k) {
	//		//			const string& rtcp_fb = payload.rtcp_fb_.at(k);

	//		//			if (nack_enabled) {
	//		//				if (rtcp_fb == "nack" || rtcp_fb == "nack pli") {
	//		//					video_payload->rtcp_fbs_.push_back(rtcp_fb);
	//		//				}
	//		//			}
	//		//			if (twcc_enabled && remote_twcc_id) {
	//		//				if (rtcp_fb == "transport-cc") {
	//		//					video_payload->rtcp_fbs_.push_back(rtcp_fb);
	//		//				}
	//		//			}
	//		//		}

	//		//		track_desc->type_ = "video";
	//		//		track_desc->set_codec_payload((SrsCodecPayload*)video_payload);
	//		//		break;
	//		//	}
	//		//}
	//		else if (remote_media_desc.is_video()) 
	//		{
	//			std::vector<cmedia_payload_type> payloads = remote_media_desc.find_media_with_encoding_name("H264");
	//			if (payloads.empty())
	//			{
	//				ERROR_EX_LOG("no found valid H.264 payload type");
	//				return false;
	//				//return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "no found valid H.264 payload type");
	//			}

	//			std::deque<cmedia_payload_type> backup_payloads;
	//			for (int32 j = 0; j < (int32)payloads.size(); j++) 
	//			{
	//				const cmedia_payload_type& payload = payloads.at(j);

	//				if (payload.m_format_specific_param.empty())
	//				{
	//					backup_payloads.push_front(payload);
	//					continue;
	//				}
	//				ch264_specific_param h264_param;
	//				if (( parse_h264_fmtp(payload.m_format_specific_param, h264_param)) != 0) 
	//				{

	//					//srs_error_reset(err); 
	//					WARNING_EX_LOG(" h264 specific param  parse h264 fmtp failed !!!");
	//					continue;
	//				}

	//				// If not exists 42e01f, we pick up any profile such as 42001f.
	//				bool profile_matched = (!has_42e01f || h264_param.m_profile_level_id == "42e01f");

	//				// Try to pick the "best match" H.264 payload type.
	//				if (profile_matched && h264_param.m_packetization_mode == "1" && h264_param.m_level_asymmerty_allow == "1") 
	//				{
	//					// if the playload is opus, and the encoding_param_ is channel

	//					cvideo_payload * video_payload = new cvideo_payload(payload.m_payload_type, payload.m_encoding_name, payload.m_clock_rate);
	//					//SrsVideoPayload* video_payload = new SrsVideoPayload(payload.payload_type_, payload.encoding_name_, payload.clock_rate_);
	//					video_payload->set_h264_param_desc(payload.m_format_specific_param);

	//					// Set the codec parameter for H.264, to make Unity happy.
	//					video_payload->m_h264_param = h264_param;

	//					// TODO: FIXME: Only support some transport algorithms.
	//					for (int32 k = 0; k < (int32)payload.m_rtcp_fb.size(); ++k) 
	//					{
	//						const std::string& rtcp_fb = payload.m_rtcp_fb.at(k);

	//						if (nack_enabled) 
	//						{
	//							if (rtcp_fb == "nack" || rtcp_fb == "nack pli") 
	//							{
	//								video_payload->m_rtcp_fbs.push_back(rtcp_fb);
	//							}
	//						}
	//						if (twcc_enabled && remote_twcc_id) 
	//						{
	//							if (rtcp_fb == "transport-cc") 
	//							{
	//								video_payload->m_rtcp_fbs.push_back(rtcp_fb);
	//							}
	//						}
	//					}

	//					track_desc.m_type = "video";
	//					track_desc.set_codec_payload(video_payload);
	//					// Only choose first match H.264 payload type.
	//					break;
	//				}

	//				backup_payloads.push_back(payload);
	//			}

	//			// Try my best to pick at least one media payload type.
	//			if (!track_desc.m_media_ptr && !backup_payloads.empty())
	//			{
	//				const cmedia_payload_type& payload = backup_payloads.front();

	//				// if the playload is opus, and the encoding_param_ is channel
	//				cvideo_payload *  video_payload = new cvideo_payload(payload.m_payload_type, payload.m_encoding_name, payload.m_clock_rate);

	//				//SrsVideoPayload* video_payload = new SrsVideoPayload(payload.payload_type_, payload.encoding_name_, payload.clock_rate_);

	//				// TODO: FIXME: Only support some transport algorithms.
	//				for (int k = 0; k < (int)payload.m_rtcp_fb.size(); ++k)
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
	//				track_desc.set_codec_payload( video_payload);
	//				NORMAL_EX_LOG("choose backup H.264 payload type=%d", payload.m_payload_type);
	//			}

	//			// TODO: FIXME: Support RRTR?
	//			//local_media_desc.payload_types_.back().rtcp_fb_.push_back("rrtr");
	//		}

	//		// Error if track desc is invalid, that is failed to match SDP, for example, we require H264 but no H264 found.
	//		if (track_desc.m_type.empty() || !track_desc.m_media_ptr) 
	//		{
	//			ERROR_EX_LOG("no match for track=%s, mid=%s, tracker=%s", remote_media_desc.m_type.c_str(), remote_media_desc.m_mid.c_str(), remote_media_desc.m_msid_tracker.c_str());
	//			return false;
	//			//return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "no match for track=%s, mid=%s, tracker=%s", remote_media_desc.type_.c_str(), remote_media_desc.mid_.c_str(), remote_media_desc.msid_tracker_.c_str());
	//		}

	//		// TODO: FIXME: use one parse payload from sdp.
	//		track_desc.create_auxiliary_payload(remote_media_desc.find_media_with_encoding_name("red"));
	//		track_desc.create_auxiliary_payload(remote_media_desc.find_media_with_encoding_name("rtx"));
	//		track_desc.create_auxiliary_payload(remote_media_desc.find_media_with_encoding_name("ulpfec"));

	//		std::string track_id;
	//		for (int32 j = 0; j < (int32)remote_media_desc.m_ssrc_infos.size(); ++j) 
	//		{
	//			const cssrc_info& ssrc_info = remote_media_desc.m_ssrc_infos.at(j);

	//			// ssrc have same track id, will be description in the same track description.
	//			if (track_id != ssrc_info.m_msid_tracker) 
	//			{
	//				//SrsRtcTrackDescription* track_desc_copy = track_desc->copy();
	//				crtc_track_description * track_desc_copy = track_desc.copy();
	//				 track_desc_copy->m_ssrc = ssrc_info.m_ssrc;
	//				 track_desc_copy->m_id = ssrc_info.m_msid_tracker;
	//				 track_desc_copy->m_msid = ssrc_info.m_msid;

	//				if (remote_media_desc.is_audio()  && !stream_desc->m_audio_track_desc_ptr )
	//				{
	//					stream_desc->m_audio_track_desc_ptr = track_desc_copy;
	//				}
	//				else if (remote_media_desc.is_video())
	//				{
	//					stream_desc->m_video_track_descs.push_back(track_desc_copy);
	//				}
	//				else
	//				{
	//					delete track_desc_copy;
	//					track_desc_copy = NULL;
	//					NORMAL_EX_LOG("not find track_id  delete  track_desc_copy object [track_id = %s][ssrc_info.m_msid_tracker = %s]", track_id.c_str(), ssrc_info.m_msid_tracker.c_str());
	//				//	srs_freep(track_desc_copy);
	//				}
	//			}
	//			track_id = ssrc_info.m_msid_tracker;
	//		}

	//		// set track fec_ssrc and rtx_ssrc
	//		for (int32 j = 0; j < (int32)remote_media_desc.m_ssrc_groups.size(); ++j) 
	//		{
	//			const cssrc_group& ssrc_group = remote_media_desc.m_ssrc_groups.at(j);
	//			crtc_track_description *track_desc = stream_desc->find_track_description_by_ssrc(ssrc_group.m_ssrcs[0]);
	//			//SrsRtcTrackDescription* track_desc = stream_desc->find_track_description_by_ssrc(ssrc_group.ssrcs_[0]);
	//			if (!track_desc) 
	//			{
	//				continue;
	//			}

	//			if (ssrc_group.m_semantic == "FID") 
	//			{
	//				track_desc->set_rtx_ssrc(ssrc_group.m_ssrcs[1]);
	//			}
	//			else if (ssrc_group.m_semantic == "FEC")
	//			{
	//				track_desc->set_fec_ssrc(ssrc_group.m_ssrcs[1]);
	//			}
	//		}
	//	}

	//	return true;
	//}
	//bool crtc_transport::_generate_publish_local_sdp(crtc_sdp & local_sdp, crtc_source_description * stream_desc, bool unified_plan, bool audio_before_video)
	//{
	//	//srs_error_t err = srs_success;
	//	int32 err = 0;
	//	if (!stream_desc) 
	//	{
	//		ERROR_EX_LOG("stream description is NULL");
	//		return false;
	//		//return srs_error_new(ERROR_RTC_SDP_EXCHANGE, "stream description is NULL");
	//	}

	//	local_sdp.m_version = "0";

	//	local_sdp.m_username = MEDIA_RTC_SERVER;
	//	local_sdp.m_session_id = digit2str_dec((int64_t)this);
	//	local_sdp.m_session_version = "2";
	//	local_sdp.m_nettype = "IN";
	//	local_sdp.m_addrtype = "IP4";
	//	local_sdp.m_unicast_address = "0.0.0.0";

	//	local_sdp.m_session_name = "RTCPublishSession";

	//	local_sdp.m_msid_semantic = "WMS";
	//	// TODO@chensong 2023-03-08   default -> video stream address url  
	//	std::string stream_id = "test";;// req->app + "/" + req->stream;
	//	local_sdp.m_msids.push_back(stream_id);

	//	local_sdp.m_group_policy = "BUNDLE";

	//	if (audio_before_video) 
	//	{
	//		if ((err = _generate_publish_local_sdp_for_audio(local_sdp, stream_desc)) != 0) 
	//		{
	//			ERROR_EX_LOG("_generate_publish_local_sdp_for_audio failed !!!");
	//			return false;
	//		}
	//		if ((err = _generate_publish_local_sdp_for_video(local_sdp, stream_desc, unified_plan)) != 0) 
	//		{
	//			ERROR_EX_LOG("_generate_publish_local_sdp_for_video failed !!!");
	//			return false;
	//		}
	//	}
	//	else 
	//	{
	//		if ((err = _generate_publish_local_sdp_for_video(local_sdp, stream_desc, unified_plan)) != 0) 
	//		{
	//			ERROR_EX_LOG("_generate_publish_local_sdp_for_video failed !!!");
	//			return false;
	//		}
	//		if ((err = _generate_publish_local_sdp_for_audio(local_sdp, stream_desc)) != 0) 
	//		{
	//			ERROR_EX_LOG("_generate_publish_local_sdp_for_audio failed !!!");
	//			return false;
	//			//return srs_error_wrap(err, "audio");
	//		}
	//	}

	//	return true;
	//}
	//bool crtc_transport::_generate_publish_local_sdp_for_audio(crtc_sdp & local_sdp, crtc_source_description * stream_desc)
	//{

	//	//srs_error_t err = srs_success;

	//	// generate audio media desc
	//	if (stream_desc->m_audio_track_desc_ptr) 
	//	{
	//		crtc_track_description* audio_track = stream_desc->m_audio_track_desc_ptr;

	//		local_sdp.m_media_descs.push_back(cmedia_desc("audio"));
	//		cmedia_desc& local_media_desc = local_sdp.m_media_descs.back();

	//		local_media_desc.m_port = 9;
	//		local_media_desc.m_protos = "UDP/TLS/RTP/SAVPF";
	//		local_media_desc.m_rtcp_mux = true;
	//		local_media_desc.m_rtcp_rsize = true;

	//		local_media_desc.m_mid = audio_track->m_mid;
	//		local_sdp.m_groups.push_back(local_media_desc.m_mid);

	//		// anwer not need set stream_id and track_id;
	//		// local_media_desc.msid_ = stream_id;
	//		// local_media_desc.msid_tracker_ = audio_track->id_;
	//		local_media_desc.m_extmaps = audio_track->m_extmaps;

	//		if (audio_track->m_direction == "recvonly") 
	//		{
	//			local_media_desc.m_recvonly = true;
	//		}
	//		else if (audio_track->m_direction == "sendonly") 
	//		{
	//			local_media_desc.m_sendonly = true;
	//		}
	//		else if (audio_track->m_direction == "sendrecv") 
	//		{
	//			local_media_desc.m_sendrecv  = true;
	//		}
	//		else if (audio_track->m_direction == "inactive") 
	//		{
	//			local_media_desc.m_inactive  = true;
	//		}

	//		caudio_payload* payload = dynamic_cast<caudio_payload*>(audio_track->m_media_ptr);
	//		local_media_desc.m_payload_types.push_back(payload->generate_media_payload_type());
	//	}

	//	return true; 
	//	//return false;
	//}
	//bool crtc_transport::_generate_publish_local_sdp_for_video(crtc_sdp & local_sdp, crtc_source_description * stream_desc, bool unified_plan)
	//{
	//	//srs_error_t err = srs_success;

	//	for (int32 i = 0; i < (int32)stream_desc->m_video_track_descs.size(); ++i) {
	//		crtc_track_description* video_track = stream_desc->m_video_track_descs.at(i);

	//		local_sdp.m_media_descs.push_back(cmedia_desc("video"));
	//		cmedia_desc& local_media_desc = local_sdp.m_media_descs.back();

	//		local_media_desc.m_port = 9;
	//		local_media_desc.m_protos = "UDP/TLS/RTP/SAVPF";
	//		local_media_desc.m_rtcp_mux = true;
	//		local_media_desc.m_rtcp_rsize = true;

	//		local_media_desc.m_mid = video_track->m_mid;
	//		local_sdp.m_groups.push_back(local_media_desc.m_mid);

	//		// anwer not need set stream_id and track_id;
	//		//local_media_desc.msid_ = stream_id;
	//		//local_media_desc.msid_tracker_ = video_track->id_;
	//		local_media_desc.m_extmaps = video_track->m_extmaps;

	//		if (video_track->m_direction == "recvonly") 
	//		{
	//			local_media_desc.m_recvonly = true;
	//		}
	//		else if (video_track->m_direction == "sendonly")
	//		{
	//			local_media_desc.m_sendonly = true;
	//		}
	//		else if (video_track->m_direction == "sendrecv")
	//		{
	//			local_media_desc.m_sendrecv = true;
	//		}
	//		else if (video_track->m_direction == "inactive")
	//		{
	//			local_media_desc.m_inactive = true;
	//		}

	//		cvideo_payload* payload =  dynamic_cast<cvideo_payload*>(video_track->m_media_ptr);
	//		local_media_desc.m_payload_types.push_back(payload->generate_media_payload_type());

	//		if (video_track->m_red_ptr) 
	//		{
	//			cred_paylod* payload =  dynamic_cast<cred_paylod*>(video_track->m_red_ptr);
	//			local_media_desc.m_payload_types.push_back(payload->generate_media_payload_type());
	//		}

	//		if (!unified_plan) 
	//		{
	//			// For PlanB, only need media desc info, not ssrc info;
	//			break;
	//		}
	//	}
	//	return true;
	//}





}