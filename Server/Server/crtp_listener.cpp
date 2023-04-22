/***********************************************************************************************
created: 		2023-03-12

author:			chensong

purpose:		rtc_listener


************************************************************************************************/
#include "crtp_listener.h"
#include "clog.h"
namespace chen {
	crtp_listener::~crtp_listener()
	{
	}
	bool crtp_listener::add_producer(uint32 ssrc, crtc_producer * producer)
	{ 
		if (!m_ssrcTable.insert(std::make_pair(ssrc, producer)).second)
		{
			WARNING_EX_LOG("insert producer rtp listener failed (ssrc = %u)", ssrc);
			return false;
		}
		return true;
	}

	void crtp_listener::remove_producer(uint32 ssrc)
	{ 
		m_ssrcTable.erase(ssrc);
	}

	chen::crtc_producer * crtp_listener::get_producer( RTC::RtpPacket * packet)
	{ 
		std::unordered_map<uint32, crtc_producer*>::iterator iter = m_ssrcTable.find(packet->GetSsrc());
		if (iter != m_ssrcTable.end())
		{
			return iter->second;
		}
		return NULL;
	}

	crtc_producer * crtp_listener::get_producer(uint32 ssrc)  
	{ 
		std::unordered_map<uint32, crtc_producer*>::const_iterator iter = m_ssrcTable.find(ssrc);
		if (iter != m_ssrcTable.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	bool crtp_listener::add_consumer(uint32 ssrc, crtc_consumer * consumer)
	{ 
		if (!m_ssrc_consumer_table.insert(std::make_pair(ssrc, consumer)).second)
		{
			WARNING_EX_LOG("insert producer rtp listener failed (ssrc = %u)", ssrc);
			return false;
		}
		return true;
	}

	void crtp_listener::remote_consumer(uint32 ssrc)
	{
		 
		m_ssrc_consumer_table.erase(ssrc);
	}

	crtc_consumer * crtp_listener::get_consumer( RTC::RtpPacket * packet)
	{
		 
		std::unordered_map<uint32, crtc_consumer*>::iterator iter = m_ssrc_consumer_table.find(packet->GetSsrc());
		if (iter != m_ssrc_consumer_table.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	crtc_consumer * crtp_listener::get_consumer(uint32 ssrc)  
	{ 
		std::unordered_map<uint32, crtc_consumer*>::const_iterator iter = m_ssrc_consumer_table.find(ssrc);
		if (iter != m_ssrc_consumer_table.end())
		{
			return iter->second;
		}
		return nullptr;
	}

}