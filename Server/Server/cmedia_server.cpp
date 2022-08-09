/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		cmedia_server


************************************************************************************************/

#include "cmedia_server.h"
namespace chen {
	cmedia_server g_media_server;

	cmedia_server::cmedia_server() : m_stop(false)
	{
	}

	cmedia_server::~cmedia_server()
	{
	}

	bool cmedia_server::init(const char* log_path, const char* config_file)
	{
		return true;
	}

	bool cmedia_server::Loop()
	{
		return true;
	}

	void cmedia_server::destroy()
	{
	}

	void cmedia_server::stop()
	{
	}

}