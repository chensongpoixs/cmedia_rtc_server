/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		rtc_source

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
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

