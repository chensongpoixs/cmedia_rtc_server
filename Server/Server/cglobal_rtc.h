/***********************************************************************************************
created: 		2022-10-17

author:			chensong

purpose:		cglobal_rtc


************************************************************************************************/

#ifndef _C_GLOBAL_RTC_H_
#define _C_GLOBAL_RTC_H_
#include "cnoncopyable.h"
#include <json/json.h>
#include <map>
#include <vector>
#include "RtpDictionaries.hpp"
namespace chen 
{
	
	// g_rtp_capabilities
	
	struct crtp_capabilities
	{
		
		std::vector<RTC::RtpCodecParameters> 			 m_codecs;
		std::vector<RTC::RtpHeaderExtensionParameters>   m_header_extensions;
		crtp_capabilities()
		: m_codecs()
		, m_header_extensions(){}
	};
	
	
	class cglobal_rtc : public cnoncopyable
	{
	public :
		explicit cglobal_rtc();
		virtual ~cglobal_rtc();

		
	public:
		  	bool init();
		  void destory();

	public:
		/*
		const DynamicPayloadTypes = [
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
    111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
    122, 123, 124, 125, 126, 127, 96, 97, 98, 99
];
		static std::map<uint32, uint32>      PayloadTypeMap = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112};*/
	public:
		//RtpCapabilities
		Json::Value  & all_rtp_capabilities() { return m_json_rtp_capabilities; }
		const 	Json::Value&  all_rtp_capabilities() const { return m_json_rtp_capabilities; }
		  	
		crtp_capabilities & get_rtp_capabilities() { return m_rtp_capabilities; }
		const crtp_capabilities & get_rtp_capabilities() const { return m_rtp_capabilities; }
	private:
		bool _init_rtp_capabilitie();
		bool _config_rtp_info();
	private:
		Json::Value  				m_json_rtp_capabilities;
		crtp_capabilities			m_rtp_capabilities;
	};
	extern cglobal_rtc g_global_rtc;
}

#endif // _C_GLOBAL_RTC_H_