/********************************************************************
created:	2019-05-07

author:		chensong

level:		网络层

purpose:	网络数据的收发
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
*********************************************************************/
#include "cwan_session.h"

#include "clog.h"
#include "cmsg_dispatch.h"
#include <json/json.h>
#include "cwan_server.h"
#include "croom_mgr.h"
#include "cclient_msg_dispatch.h"
#include "cmsg_dispatch.h"
#include "cshare_proto_error.h"
#include "cglobal_rtc.h"
#include "capi_rtc_publish.h"
#include "cglobal_rtc_config.h"
#include "capi_rtc_player.h"
#include "ctransport_mgr.h"
namespace chen {

	void cwan_session::handler_msg(uint64_t session_id, const void* p, uint32 size)
	{
		// 1. 登录状态判断
		//NORMAL_EX_LOG("[session_id = %u][p = %s]", session_id, p);
		if (!m_json_reader.parse((const char *)p, (const char *)p + size, m_json_response))
		{
			ERROR_EX_LOG("parse json failed !!! [session_id = %llu][json = %s]", session_id, p);
			return;
		}
		if (!m_json_response.isMember("msg_id"))
		{
			WARNING_EX_LOG("[session_id = %llu]not find cmd type !!!", session_id);
			return;
		}
		// TODO@chensong 20220812 管理的比较漏
		const uint32 msg_id = m_json_response["msg_id"].asUInt();
			cclient_msg_handler * client_msg_handler = g_client_msg_dispatch.get_msg_handler(msg_id);
			if (!client_msg_handler || !client_msg_handler->handler)
			{
				WARNING_EX_LOG("[session_id = %llu]not find msg_id= %u callback !!!", session_id, msg_id);
				return;
			}
			m_session_id = session_id;
			++client_msg_handler->handle_count;
			(this->*(client_msg_handler->handler))(m_json_response);
		//if ((msg_id < S2C_WebrtcMessageUpdate) || (msg_id < Msg_Client_Max && msg_id > S2C_LevalRoomUpdate))
		//{

		//	cclient_msg_handler * client_msg_handler = g_client_msg_dispatch.get_msg_handler(msg_id);
		//	if (!client_msg_handler || !client_msg_handler->handler)
		//	{
		//		WARNING_EX_LOG("[session_id = %llu]not find msg_id= %u callback !!!", session_id, msg_id);
		//		return;
		//	}
		//	m_session_id = session_id;
		//	++client_msg_handler->handle_count;
		//	(this->*(client_msg_handler->handler))(m_json_response);

		//}
		//else if (msg_id < C2S_CreateRtc)
		//{
		//	Json::Value reply;
		//	if (!m_json_response["data"].isMember("room_name") || !m_json_response["data"]["room_name"].isString())
		//	{
		//		WARNING_EX_LOG("[session_id = %llu]not find room_name  , [value = %s] !!! ", m_session_id, m_json_response.asCString());
		//		send_msg(msg_id+1, EShareProtoRoomName, reply);
		//		return  ;
		//	}
		//	cmsg_handler *msg_handler = g_msg_dispatch.get_msg_handler(msg_id);
		//	if (!msg_handler || !msg_handler->handler)
		//	{
		//		
		//		WARNING_EX_LOG("[session_id = %llu]not find msg_id= %u callback !!!", session_id, msg_id);
		//		return;
		//	}
		//	++msg_handler->handle_count;

		//	g_room_mgr.join_room(session_id,  m_json_response["data"]["room_name"].asCString(), m_user_name);


		//	//(this->*(msg_handler->handler))(session_id, m_json_response);
		//} 
		//else
		//{
		//	WARNING_EX_LOG("not find [msg_id = %u][msg = %s]", msg_id, p);
		//}

		 
	}
	bool    cwan_session::handler_login(Json::Value & value)
	{
		Json::Value reply;
		if (!value.isMember("data") || !value["data"].isObject())
		{
			WARNING_EX_LOG("[session_id = %llu]not find cmd type, [value = %s] !!! ", m_session_id, value.asCString());
			send_msg(S2C_Login, EShareProtoData, reply);
			return false;
		}
		if (!value["data"].isMember("user_name") || !value["data"]["user_name"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find user_name      !!! ", m_session_id );
			send_msg(S2C_Login, EShareProtoUserName, reply);
			return false;
		}
		if (!value["data"].isMember("room_name") || !value["data"]["room_name"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find room_name    !!! ", m_session_id );
			send_msg(S2C_Login, EShareProtoRoomName, reply);
			return false;
		}
		
		if (g_room_mgr.room_has_username(value["data"]["room_name"].asCString(), value["data"]["user_name"].asCString()))
		{
			WARNING_EX_LOG("[session_id = %llu] total roomname failed  [room_name = %s] !!! ", m_session_id, value["data"]["room_name"].asCString()
			);
			send_msg(S2C_Login, EShareLoginRoomUsername, reply);
			close();
			return false;
		}
		uint32 type = 0;
		if (value["data"].isMember("type") && value["data"]["type"].isInt64())
		{
			type = value["data"]["type"].asInt64() > 0 ? 1 : 0;
		}
		/*if (g_room_mgr.room_user_type(value["data"]["room_name"].asCString(), type))
		{
			WARNING_EX_LOG("[session_id = %llu] total roomname failed  [room_name = %s] !!! ", m_session_id, value["data"]["room_name"].asCString()
			);
			send_msg(S2C_Login, EShareLoginRoomType, reply);
			close();
			return false;
		}*/
		
		m_user_name = value["data"]["user_name"].asCString();
		m_room_name = value["data"]["room_name"].asCString();
		if (!g_room_mgr.join_room(m_session_id, m_room_name, m_user_name, type))
		{
			//EShareProtoCreateRoomFailed
			WARNING_EX_LOG("[session_id = %llu] create roomname failed   !!! ", m_session_id );
			send_msg(S2C_Login, EShareProtoCreateRoomFailed, reply);
			return false;
		}
		//NORMAL_EX_LOG("[sdp = %s]", value["data"]["sdp"].asCString());
		//if (!m_rtc_sdp.init(value["data"]["sdp"].asCString()))
		//{
		//	WARNING_EX_LOG("rtc session_id = %lu, sdp = %s", value["data"]["sdp"].asCString());
		//	//EShareProtoParseSdpFailed
		//	send_msg(S2C_Login, EShareProtoParseSdpFailed, reply);
		//	return false;
		//}

		m_client_connect_type = EClientConnectSession;

		NORMAL_EX_LOG("%s Login OK!!", m_user_name.c_str());
		send_msg(S2C_Login, EShareProtoOk, reply);
		 send_msg(S2C_RtpCapabilitiesUpdate, 0, g_global_rtc.all_rtp_capabilities());
		 g_room_mgr.build_client_p2p(m_room_name);
		return true;
	}



	bool	cwan_session::handler_join_room(Json::Value &value)
	{
		 
		return true;
	}
	bool	cwan_session::handler_destroy_room(Json::Value& value)
	{
		return true;
	}

	bool	cwan_session::handler_webrtc_message(Json::Value& value)
	{
		if (m_room_name.empty())
		{
			return false;
		}
		Json::Value reply;
		if (!value.isMember("data") || !value["data"].isObject())
		{
			WARNING_EX_LOG("[session_id = %llu]not find cmd type, [value = %s] !!! ", m_session_id, value.asCString());
			send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
			return false;
		}
		g_room_mgr.webrtc_message(m_room_name, m_session_id, value["data"]);
		send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
		/*Json::Value reply;
		if (!value.isMember("data") || !value["data"].isObject())
		{
			WARNING_EX_LOG("[session_id = %llu]not find cmd type, [value = %s] !!! ", m_session_id, value.asCString());
			send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
			return false;
		}*/
		/*if (!value["data"].isMember("type") || !value["data"]["type"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find type  , [value = %s] !!! ", m_session_id, value.asCString());
			send_msg(S2C_WebrtcMessage, EShareProtoUserName, reply);
			return false;
		}*/
		//if (!value["data"].isMember("room_name") || !value["data"]["room_name"].isString())
		/*{
			WARNING_EX_LOG("[session_id = %llu]not find room_name  , [value = %s] !!! ", m_session_id, value.asCString());
			send_msg(S2C_Login, EShareProtoRoomName, reply);
			return false;
		}*/


		return true;
	}
	bool cwan_session::handler_rtc_datachannel(Json::Value & value)
	{
		if (!m_room_name.empty() && g_room_mgr.m_master[m_room_name] != 0)
		{
			Json::Value reply;
			if (!value.isMember("datachannel") || !value["datachannel"].isObject())
			{
				WARNING_EX_LOG("[session_id = %llu]not find cmd type, [value = %s] !!! ", m_session_id, value.toStyledString().c_str());
				send_msg(S2C_RtcDataChannel, EShareProtoData, reply);
				return false;
			}
			cwan_session * session_ptr = g_wan_server.get_session(g_room_mgr.m_master[m_room_name]);
			if (!session_ptr)
			{
				//EShareProtoOk
				send_msg(S2C_RtcDataChannel, EShareProtoOk, reply);
				return false;
			}
			/*	Json::Value msg;
				msg["datachannel"] = value["datachannel"];*/
			session_ptr->send_msg(S2C_RtcDataChannelUpdate, EShareProtoOk, value["datachannel"]);
			send_msg(S2C_RtcDataChannel, EShareProtoOk, reply);
		}
		return true;
	}
	/*bool	cwan_session::handler_create_answer(Json::Value& value)
	{
		return true;
	}

	bool	cwan_session::handler_candidate(Json::Value& value)
	{
		return true;
	}*/
	/*bool cwan_session::handler_create_room(Json::Value & value)
	{

		return true;
	}

	bool cwan_session::handler_destroy_room(Json::Value & value)
	{
		return true;
	}*/



	bool   cwan_session::handler_create_rtc(Json::Value& value)
	{
		//g_global_webrtc_mgr.handler_create_webrtc(m_session_id, value);
		return true;
	}

	bool   cwan_session::handler_connect_rtc(Json::Value& value)
	{
		//g_global_webrtc_mgr.handler_connect_webrtc(m_session_id, value);
		return true;
	}
	bool   cwan_session::handler_rtc_produce(Json::Value& value)
	{
		return true;// g_global_webrtc_mgr.handler_webrtc_produce(m_session_id, value);;
	}
	bool   cwan_session::handler_rtc_consume(Json::Value& value)
	{
		return   true; // g_global_webrtc_mgr.handler_webrtc_consume(m_session_id, value);;;
	}
	bool cwan_session::handler_rtc_publisher(Json::Value & value)
	{
		if (m_room_name.empty())
		{
			return false;
		}
		Json::Value reply;
		if (!value.isMember("data") || !value["data"].isObject())
		{
			WARNING_EX_LOG("[session_id = %llu]not find data type, [value = %s] !!! ", m_session_id, value.toStyledString().c_str());
			send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
			return false;
		}

		if (!value["data"].isMember("offer") || !value["data"]["offer"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find offer type, [value = %s] failed !!! ", m_session_id, value.toStyledString().c_str() );
			send_msg(S2C_rtc_publisher, EShareProtoData, reply);
			return false;
		}
		if ( !value["data"].isMember("roomname") || !value["data"]["roomname"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find roomname type, [value = %s] failed !!! ", m_session_id, value.toStyledString().c_str());
			send_msg(S2C_rtc_publisher, EShareProtoData, reply);
			return false;
		}
		if (  !value["data"].isMember("peerid") || !value["data"]["peerid"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find peerid type, [value = %s] failed !!! ", m_session_id, value.toStyledString().c_str());
			send_msg(S2C_rtc_publisher, EShareProtoData, reply);
			return false;
		}
		//g_room_mgr.webrtc_message(m_room_name, m_session_id, value["data"]);
		//send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
		{
			capi_rtc_publish publisher;

			cclient_publish_message publish_message;
			publish_message.m_remote_sdp = value["data"]["offer"].asCString();
			publish_message.m_room_name = value["data"]["roomname"].asCString();
			publish_message.m_peer_id = value["data"]["peerid"].asCString();
			if (value["data"].isMember("codec") && value["data"]["codec"].isString())
			{
				publish_message.m_codec = value["data"]["codec"].asCString();
			}
			if (value["data"].isMember("rtc_protocol") && value["data"]["rtc_protocol"].isString())
			{
				publish_message.m_rtc_protocol = value["data"]["rtc_protocol"].asCString();
			}
			std::string media_stream_url = publish_message.m_room_name + "/" + publish_message.m_peer_id;
			if (!g_global_rtc_config.get_stream_uri(media_stream_url /*roomname + "/" + peerid*/))
			{
				WARNING_EX_LOG("create media ssrc failed !!![ media name = %s ]", media_stream_url.c_str());
				send_msg(S2C_rtc_publisher, EShareRtcCreateMediaSsrcInfo, reply);
				return true;
			}
			std::string local_sdp;
			auto iter = g_transport_mgr.m_all_stream_url_map.find(media_stream_url);
			if (iter == g_transport_mgr.m_all_stream_url_map.end())
			{
				publisher.do_serve_client(publish_message/*sdp, roomname, peerid*/, local_sdp);
				reply["sdp"] = local_sdp;
				reply["type"] = "answer";
				//if (m_master)
				{
					m_room_name = publish_message.m_room_name; //roomname;
					m_user_name = publish_message.m_peer_id;// //peerid;
					m_master = true;
					g_room_mgr.m_master[m_room_name] = m_session_id;
				}
				send_msg(S2C_rtc_publisher, EShareProtoOk, reply);
			}
			else
			{
				send_msg(S2C_rtc_publisher, EShareRtcCreateMediaChannel, reply);
			}
			
			//send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
		}
		return true;
	}

	bool cwan_session::handler_rtc_player(Json::Value & value)
	{
		if (m_room_name.empty())
		{
			return false;
		}
		Json::Value reply;
		if (!value.isMember("data") || !value["data"].isObject())
		{
			WARNING_EX_LOG("[session_id = %llu]not find data type, [value = %s] !!! ", m_session_id, value.toStyledString().c_str());
			send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
			return false;
		}

		if (!value["data"].isMember("offer") || !value["data"]["offer"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find offer type, [value = %s] failed !!! ", m_session_id, value.toStyledString().c_str());
			send_msg(S2C_rtc_publisher, EShareProtoData, reply);
			return false;
		}
		if (!value["data"].isMember("roomname") || !value["data"]["roomname"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find roomname type, [value = %s] failed !!! ", m_session_id, value.toStyledString().c_str());
			send_msg(S2C_rtc_publisher, EShareProtoData, reply);
			return false;
		}
		if (!value["data"].isMember("video_peerid") || !value["data"]["video_peerid"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find video_peerid type, [value = %s] failed !!! ", m_session_id, value.toStyledString().c_str());
			send_msg(S2C_rtc_publisher, EShareProtoData, reply);
			return false;
		}
		if (!value["data"].isMember("peerid") || !value["data"]["peerid"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find peerid type, [value = %s] failed !!! ", m_session_id, value.toStyledString().c_str());
			send_msg(S2C_rtc_publisher, EShareProtoData, reply);
			return false;
		}
		//g_room_mgr.webrtc_message(m_room_name, m_session_id, value["data"]);
		//send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
		{
			capi_rtc_player player;

			cclient_player_message  player_message;
			player_message.m_remote_sdp = value["data"]["offer"].asCString();
			player_message.m_room_name = value["data"]["roomname"].asCString();
			player_message.m_video_peer_id = value["data"]["video_peerid"].asCString();
			player_message.m_peer_id = value["data"]["peerid"].asCString();
			//std::string codec = "H264"; // default H264 编码
			if (value["data"].isMember("codec") && value["data"]["codec"].isString())
			{
				player_message.m_codec = value["data"]["codec"].asCString();
			}
			if (value["data"].isMember("rtc_protocol") && value["data"]["rtc_protocol"].isString())
			{
				player_message.m_rtc_protocol = value["data"]["rtc_protocol"].asCString();
			}
			if (!g_global_rtc_config.get_stream_uri(player_message.m_room_name + "/" + player_message.m_video_peer_id))
			{
				//EShareRtcCreateMediaSsrcInfo
				WARNING_EX_LOG("create media ssrc failed !!![ media name = %s/%s]", player_message.m_room_name.c_str(), player_message.m_video_peer_id.c_str());
				send_msg(S2C_rtc_player, EShareRtcCreateMediaSsrcInfo, reply);
				return true;
			}
			std::string local_sdp;

			player.do_serve_client(player_message,  local_sdp);
			reply["sdp"] = local_sdp;
			reply["type"] = "answer";
			m_room_name = player_message.m_room_name;
			m_user_name = player_message.m_peer_id;
			send_msg(S2C_rtc_player, EShareProtoOk, reply);
			//send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
		}
		return true;
	}
	bool cwan_session::handler_rtc_request_frame(Json::Value & value)
	{
		if (m_room_name.empty())
		{
			return false;
		}
		Json::Value reply;
		if (!value.isMember("data") || !value["data"].isObject())
		{
			WARNING_EX_LOG("[session_id = %llu]not find data type, [value = %s] !!! ", m_session_id, value.toStyledString().c_str());
			send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
			return false;
		}

		 
		if (!value["data"].isMember("roomname") || !value["data"]["roomname"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find roomname type, [value = %s] failed !!! ", m_session_id, value.toStyledString().c_str());
			send_msg(S2C_rtc_publisher, EShareProtoData, reply);
			return false;
		}
		if (!value["data"].isMember("video_peerid") || !value["data"]["video_peerid"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find video_peerid type, [value = %s] failed !!! ", m_session_id, value.toStyledString().c_str());
			send_msg(S2C_rtc_publisher, EShareProtoData, reply);
			return false;
		}
		 
		//g_room_mgr.webrtc_message(m_room_name, m_session_id, value["data"]);
		//send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
		{
			 
			std::string roomname = value["data"]["roomname"].asCString();
			std::string video_peerid = value["data"]["video_peerid"].asCString();
			 
			auto iter  = g_transport_mgr.m_all_stream_url_map.find(roomname + "/" + video_peerid);
		 
			if (iter == g_transport_mgr.m_all_stream_url_map.end())
			{
				send_msg(S2C_rtc_requestframe, EShareRtcRequestFrame, reply);
				return false;
			}
			iter->second ->request_key_frame();
			send_msg(S2C_rtc_requestframe, EShareProtoOk, reply);
			//send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
		}
		return true;
	}
}