/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		cmedia_server


************************************************************************************************/
#ifndef _C_MEDIA_SERVER_H_
#define _C_MEDIA_SERVER_H_
#include "cnoncopyable.h"
#include "ctimer.h"
namespace chen {
	class cmedia_server : public ctimer::Listener /*: public  cnoncopyable*/
	{
	public:
		explicit cmedia_server();
	    virtual	~cmedia_server();


	public:
		bool init(const char* log_path, const char* config_file);
		bool Loop();
		void destroy();
		void stop();
	public:
		virtual void OnTimer(ctimer * timer);
	private:
		volatile bool m_stop;
		ctimer * m_server_intaval;
	};
	extern cmedia_server g_media_server;
}

#endif // _C_MEDIA_SERVER_H_