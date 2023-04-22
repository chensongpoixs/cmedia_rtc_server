/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		rtc_servr_mgr
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
#include "crtc_server_mgr.h"
#include "crtc_source.h"
#include "crtc_source_mgr.h"
#include "cshare_proto_error.h"
#include "clog.h"
#include "crtc_user_config.h"
#include "crtc_connection.h"
#include "crandom.h"
namespace chen {
	crtc_server_mgr::~crtc_server_mgr()
	{
	}
	int32 crtc_server_mgr::create_session(crtc_user_config * ruc, crtc_sdp & local_sdp, crtc_connection ** psession)
	{

		int32 err = 0;


		crtc_source * source = NULL;
		if ((err = g_rtc_source_mgr.fetch_or_create(ruc, &source)) != 0)
		{
			WARNING_EX_LOG("create source ");
			return err;
		}
		std::string stream_url;
		if (ruc->m_publish   && !source->can_publish()) 
		{
			ERROR_EX_LOG("stream %s busy", stream_url.c_str());
			return EShareRtcSourceBusy;
			//return error_new(ERROR_RTC_SOURCE_BUSY, "stream %s busy", req->get_stream_url().c_str());
		}


		// TODO: FIXME: add do_create_session to error process.
		crtc_connection* session = new crtc_connection( );
		if ((err = _do_create_session(ruc, local_sdp, session)) != 0)
		{
			if (session)
			{
				delete session;
				session = NULL;
			}
			ERROR_EX_LOG("create session");
			//srs_freep(session);
			return err;
			//return error_wrap(err, "create session");
		}

		*psession = session;

		return err;
	}
	int32 crtc_server_mgr::_do_create_session(crtc_user_config * ruc, crtc_sdp & local_sdp, crtc_connection * session)
	{
		int32 err = 0;
		// first add publisher/player for negotiate sdp media info
		if (ruc->m_publish) 
		{
			//if ((err = session->add_publisher(ruc, local_sdp)) != 0) 
			{
				ERROR_EX_LOG("add publisher");
				return err;
				//return  error_wrap(err, "add publisher");
			}
		}
		else
		{
			//if ((err = session->add_player(ruc, local_sdp)) != 0) 
			{
				ERROR_EX_LOG("add player");
				return err;
				//return  error_wrap(err, "add player");
			}
		}
		// All tracks default as inactive, so we must enable them.
		//session->set_all_tracks_status(req->get_stream_url(), ruc->publish_, true);


		std::string local_pwd = c_rand.rand_str(32);
		std::string local_ufrag = "";
		// TODO: FIXME: Rename for a better name, it's not an username.
		std::string username = "";
		while (true) 
		{
			local_ufrag = c_rand.rand_str(8);

			username = local_ufrag + ":" + ruc->m_remote_sdp.get_ice_ufrag();
			/*if (!g_rtc_source_mgr.find_by_name(username)) {
				break;
			}*/
		}
		local_sdp.set_ice_ufrag(local_ufrag);
		local_sdp.set_ice_pwd(local_pwd);
		local_sdp.set_fingerprint_algo("sha-256");
		local_sdp.set_fingerprint("_rtc_dtls_certificate->get_fingerprint()");



		// We allows to mock the eip of server.
		if (true) 
		{
			int udp_port;
			int tcp_port;
			std::string protocol = "UDP";

			std::set<std::string> candidates; // = ips;//discover_candidates(ruc);
			for (std::set<std::string>::iterator it = candidates.begin(); it != candidates.end(); ++it)
			{
				std::string hostname;
				int32 uport = udp_port;  
				//parse_hostport(*it, hostname, uport);
				int32 tport = tcp_port;  
				//parse_hostport(*it, hostname, tport);

				if (protocol == "udp") 
				{
					local_sdp.add_candidate("udp", hostname, uport, "host");
				}
				else if (protocol == "tcp")
				{
					local_sdp.add_candidate("tcp", hostname, tport, "host");
				}
				else 
				{
					local_sdp.add_candidate("udp", hostname, uport, "host");
					local_sdp.add_candidate("tcp", hostname, tport, "host");
				}
			}

			std::vector<std::string> v = std::vector<std::string>(candidates.begin(), candidates.end());


			std::ostringstream cmd;
			for (const std::string & str : v)
			{
				cmd << ", " << str;
			}

			NORMAL_EX_LOG("RTC: Use candidates %s, protocol=%s",  cmd.str().c_str(), protocol.c_str());
		}



		// Setup the negotiate DTLS by config.
		local_sdp.m_session_negotiate = local_sdp.m_session_config;

		// Setup the negotiate DTLS role.
		if (ruc->m_remote_sdp.get_dtls_role() == "active") 
		{
			local_sdp.m_session_negotiate.m_dtls_role = "passive";
		}
		else if (ruc->m_remote_sdp.get_dtls_role() == "passive") 
		{
			local_sdp.m_session_negotiate.m_dtls_role = "active";
		}
		else if (ruc->m_remote_sdp.get_dtls_role() == "actpass") 
		{
			local_sdp.m_session_negotiate.m_dtls_role = local_sdp.m_session_config.m_dtls_role;
		}
		else 
		{
			// @see: https://tools.ietf.org/html/rfc4145#section-4.1
			// The default value of the setup attribute in an offer/answer exchange
			// is 'active' in the offer and 'passive' in the answer.
			local_sdp.m_session_negotiate.m_dtls_role = "passive";
		}
		local_sdp.set_dtls_role(local_sdp.m_session_negotiate.m_dtls_role);


		// TODO@chensong 2022-01-15 
		//session->set_remote_sdp(ruc->m_remote_sdp);
		// We must setup the local SDP, then initialize the session object.
		//session->set_local_sdp(local_sdp);
		//session->set_state_as_waiting_stun();

		return err;
	}
}