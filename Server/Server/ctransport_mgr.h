/***********************************************************************************************
created: 		2023-03-10

author:			chensong

purpose:		ctransport_mgr


************************************************************************************************/

#ifndef _C_TRANSPORT_MGR_H_
#define _C_TRANSPORT_MGR_H_
#include "cnet_type.h"
#include "crtc_sdp.h"
#include <unordered_map>
#include "crtc_source_description.h"
#include <crtc_transport.h>
#include "crtp_stream.h"
namespace chen {

	class ctransport_mgr
	{
	public:
		typedef std::unordered_map<std::string, crtc_transport*>   TRANSPORT_MAP;
									
		typedef std::unordered_map<std::string, crtc_transport*>   STREAM_URL_MAP;
		////////////////////////////////////////////////////////////////////////////////
		// key-->
		typedef std::unordered_map<std::string, std::set<crtc_transport*>> CONSUMER_MAP;

		//  key ---> 0: audio 1: video 2: datachannel
		typedef std::unordered_map < std::string, std::vector<std::set<crtp_stream*>>>  ALL_RTP_STREAM_MAP;


		// key -->
		//typedef std::unordered_map < std::string, std::vector<std::set<crtc_transport*>>>  ALL_CONSUMER_MAP;
	public:

		explicit ctransport_mgr()
		: m_all_transport_map()
	    , m_all_stream_url_map()
		, m_all_consumer_map()
		{}

		virtual ~ctransport_mgr();

	public:

		bool init();
		void update(uint32 uDeltaTime);

		void destroy();

	public:


		// ice username
		crtc_transport * find_username(const std::string & username);
		const crtc_transport* find_username(const std::string & username)const ;
		bool   insert_username(const std::string & username, crtc_transport* transport);

		bool   remove_username(const std::string& username);


	public:
		// stream url 
	 
		bool   remove_stream_name(const std::string& stream_url);


	protected:
	private:

		//crtc_transport*    _find_transport(const std::string &username);
		//const crtc_transport* _find_transport(const std::string & username) const;


		

	private:
		//typedef TRANSPORT_MAP::iterator					iterator
	private:

		TRANSPORT_MAP										m_all_transport_map;
	public:

		STREAM_URL_MAP										m_all_stream_url_map;
	public:
		CONSUMER_MAP										m_all_consumer_map;



		ALL_RTP_STREAM_MAP									m_all_recv_rtp_stream_map;
		ALL_RTP_STREAM_MAP									m_all_send_rtp_stream_map;
	};


	extern ctransport_mgr   g_transport_mgr;
}


#endif // _C_TRANSPORT_MGR_H_