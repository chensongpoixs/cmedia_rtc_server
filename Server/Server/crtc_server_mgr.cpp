/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		rtc_servr_mgr


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
			if ((err = session->add_publisher(ruc, local_sdp)) != 0) 
			{
				ERROR_EX_LOG("add publisher");
				return err;
				//return  error_wrap(err, "add publisher");
			}
		}
		else
		{
			if ((err = session->add_player(ruc, local_sdp)) != 0) 
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
		session->set_remote_sdp(ruc->m_remote_sdp);
		// We must setup the local SDP, then initialize the session object.
		session->set_local_sdp(local_sdp);
		session->set_state_as_waiting_stun();

		return err;
	}
}