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
namespace chen {

	class ctransport_mgr
	{
	private:
		typedef std::unordered_map<std::string, crtc_transport*>   TRANSPORT_MAP;
									
		typedef std::unordered_map<std::string, crtc_transport*>   STREAM_URL_MAP;
		////////////////////////////////////////////////////////////////////////////////


	public:

		explicit ctransport_mgr()
		: m_all_transport_map()
		, m_all_stream_url_map(){}

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
		crtc_transport * find_stream_name(const std::string & stream_url);
		const crtc_transport* find_stream_name(const std::string & stream_url)const;
		bool   insert_stream_name(const std::string & stream_url, crtc_transport* transport);

		bool   remove_stream_name(const std::string& stream_url);


	protected:
	private:

		//crtc_transport*    _find_transport(const std::string &username);
		//const crtc_transport* _find_transport(const std::string & username) const;


		

	private:
		//typedef TRANSPORT_MAP::iterator					iterator
	private:

		TRANSPORT_MAP										m_all_transport_map;
		STREAM_URL_MAP										m_all_stream_url_map;
	};


	extern ctransport_mgr   g_transport_mgr;
}


#endif // _C_TRANSPORT_MGR_H_