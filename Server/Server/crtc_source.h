/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		rtc_source


************************************************************************************************/

#ifndef _C_RTC_SOURCE_H_
#define _C_RTC_SOURCE_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "crtc_source_description.h"
#include "crtc_consumer.h"
namespace chen {
	class crtc_source
	{
	public:
		crtc_source()
			: m_pli_elapsed(0)
			, m_stream_desc_ptr(NULL)
			, m_consumers()
			, m_is_created(false)
			, m_is_delivering_packets(false)
			 
		{}
		~crtc_source();


	public:
		int32 init();
		//void update();
		//int32 on_timer();


	private:
		void _init_for_play_before_publishing();

	public:
		// Create consumer
		// @param consumer, output the create consumer.
		virtual int32 create_consumer(crtc_consumer*& consumer);
		// Dumps packets in cache to consumer.
		// @param ds, whether dumps the sequence header.
		// @param dm, whether dumps the metadata.
		// @param dg, whether dumps the gop cache.
		virtual int32 consumer_dumps(crtc_consumer* consumer, bool ds = true, bool dm = true, bool dg = true);
		virtual void on_consumer_destroy(crtc_consumer* consumer);
		// Whether we can publish stream to the source, return false if it exists.
		// @remark Note that when SDP is done, we set the stream is not able to publish.
		virtual bool can_publish();
		// For RTC, the stream is created when SDP is done, and then do DTLS
		virtual void set_stream_created();
		//// When start publish stream.
		//virtual int32 on_publish();
		//// When stop publish stream.
		//virtual void on_unpublish();

	public:

		// Get and set the publisher, passed to consumer to process requests such as PLI.
		//IRtcPublishStream* publish_stream();
		//void set_publish_stream(IRtcPublishStream* v);
		// Consume the shared RTP packet, user must free it.
		//int32 on_rtp(RtpPacket* pkt);
		// Set and get stream description for souce
		bool has_stream_desc();
		void set_stream_desc(crtc_source_description* stream_desc);
		std::vector<crtc_track_description*> get_track_desc(std::string type, std::string media_type);
	protected:
	private:


		// The PLI for RTC 
		int32									m_pli_elapsed;

		// Steam description for this steam.
		crtc_source_description *				m_stream_desc_ptr;

		// To delivery stream to clients.
		std::vector<crtc_consumer*>				m_consumers;
		// Whether stream is created, that is, SDP is done.
		bool									m_is_created;
		// Whether stream is delivering data, that is, DTLS is done.
		bool									m_is_delivering_packets;
		// Notify stream event to event handler
		//std::vector<IRtcSourceEventHandler*>	m_event_handlers_;
	};

}

#endif/// _C_RTC_SOURCE_H_

