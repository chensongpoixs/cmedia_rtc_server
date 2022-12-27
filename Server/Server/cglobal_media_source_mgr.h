/***********************************************************************************************
created: 		2022-12-27

author:			chensong

purpose:		ip_address


************************************************************************************************/


#ifndef _C_GLOBAL_MEDIA_SOURCE_MGR_H_
#define _C_GLOBAL_MEDIA_SOURCE_MGR_H_
#include "cnet_type.h"
#include <map>
#include <unordered_map>
#include <string>
#include "cmedia_source.h"
namespace chen {
	class cmedia_source_mgr
	{
	private:
		typedef     std::unordered_map<std::string, cmedia_source*>         ALL_MEDIA_SOURCE;

	public:
		explicit cmedia_source_mgr()
			: m_media_source() {}
		~cmedia_source_mgr() {}

	public:
		bool init();
		void update(uint32 uDeltaTime);
		void destroy();
	
	public:

	protected:
	private:
		ALL_MEDIA_SOURCE					m_media_source;
	};
}

#endif // _C_GLOBAL_MEDIA_SOURCE_MGR_H_