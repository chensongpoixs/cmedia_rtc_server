/***********************************************************************************************
created: 		2023-03-12

author:			chensong

purpose:		rtc_listener


************************************************************************************************/

#ifndef _C_RTC_LISTENER_H_
#define _C_RTC_LISTENER_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "crtc_producer.h"
#include "crtc_consumer.h"
namespace chen {

	class crtp_listener
	{
	public:
		explicit crtp_listener() 
		: m_ssrcTable()
		, m_midTable()
		, m_ridTable(){}

		virtual ~crtp_listener();
	public:
		bool add_producer(uint32 ssrc,  crtc_producer * producer);
		void remove_producer(uint32 ssrc);


		crtc_producer * get_producer(  RTC::RtpPacket * packet )   ;
		crtc_producer * get_producer(uint32 ssrc)   ;


		////////////////////////////////////////////////////////////////////////


		bool add_consumer(uint32 ssrc, crtc_consumer* consumer);
		void remote_consumer(uint32 ssrc);
		crtc_consumer * get_consumer( RTC::RtpPacket * packet);
		crtc_consumer * get_consumer(uint32 ssrc)  ;
	public:
		std::mutex											m_ssrc_mutex;
		// Table of SSRC / Producer pairs.
		std::unordered_map<uint32, crtc_producer*>			m_ssrcTable;
		//  Table of MID / Producer pairs.
		std::unordered_map<std::string, crtc_producer*>		m_midTable;
		//  Table of RID / Producer pairs.
		std::unordered_map<std::string, crtc_producer*>		m_ridTable;


		std::unordered_map<uint32, crtc_consumer*>			m_ssrc_consumer_table;


	protected:
	private:
	};

}
#endif // 