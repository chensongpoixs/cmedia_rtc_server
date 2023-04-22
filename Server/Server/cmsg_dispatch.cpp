/***********************************************************************************************
					created: 		2019-05-13
					author:			chensong
					purpose:		msg_dipatch
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

#include "cmsg_dispatch.h"
#include "clog.h"
#include "cwebrtc_transport.h"
namespace chen {
	cmsg_dispatch::cmsg_dispatch()
	{
	}
	cmsg_dispatch::~cmsg_dispatch()
	{
	}
	bool cmsg_dispatch::init()
	{
		
		//_register_msg_handler(C2S_RtcConnect,			"webrtc_connect",		&cwebrtc_transport::handler_connect);
		//_register_msg_handler(C2S_rtc_restart_ice,		"webrtc_restart_ice",	&cwebrtc_transport::handler_restart_ice);
		//_register_msg_handler(C2S_rtc_info,			"webrtc_info",			&cwebrtc_transport::handler_info);
		//_register_msg_handler(C2S_rtc_get_stats,		"webrtc_get_stats",		&cwebrtc_transport::handler_get_stats);
		//_register_msg_handler(C2S_rtc_set_max_incoming_bitrate,			"webrtc_set_max_incoming_bitrate",		&cwebrtc_transport::handler_set_max_incoming_bitrate);
		//_register_msg_handler(C2S_rtc_set_max_outgoing_bitrate,			"webrtc_set_max_outgoing_bitrate",		&cwebrtc_transport::handler_set_max_outgoing_bitrate);
		//_register_msg_handler(C2S_rtc_produce,			"webrtc_produce",		&cwebrtc_transport::handler_produce);
		//_register_msg_handler(C2S_RtcConsume,			"webrtc_consume",		&cwebrtc_transport::handler_consume);
		//_register_msg_handler(C2S_rtc_produce_data,			"webrtc_produce_data",		&cwebrtc_transport::handler_produce_data);
		//_register_msg_handler(C2S_rtc_consume_data,			"webrtc_consume_data",		&cwebrtc_transport::handler_consume_data);
		//_register_msg_handler(C2S_rtc_enable_trace_event,			"webrtc_enable_trace_event",		&cwebrtc_transport::handler_enable_trace_event);
		//_register_msg_handler(C2S_rtc_produce_close,			"webrtc_produce_close",		&cwebrtc_transport::handler_produce_close);
		//_register_msg_handler(C2S_rtc_consumer_close,			"webrtc_consumer_close",		&cwebrtc_transport::handler_consumer_close);
		///////

		///////////////////////produce info //////		//////////////////		/
		//_register_msg_handler(C2S_rtc_produce_info,			"webrtc_produce_info",		&cwebrtc_transport::handler_produce_info);
		//_register_msg_handler(C2S_rtc_produce_get_stats,			"webrtc_produce_get_stats",		&cwebrtc_transport::handler_produce_get_stats);
		//_register_msg_handler(C2S_rtc_produce_pause,			"webrtc_produce_pause",		&cwebrtc_transport::handler_produce_pause);
		//_register_msg_handler(C2S_rtc_produce_resume,			"webrtc_produce_resume",		&cwebrtc_transport::handler_produce_resume);
		//_register_msg_handler(C2S_rtc_produce_enable_trace_event,			"webrtc_produce_enable_trace_event",		&cwebrtc_transport::handler_produce_enable_trace_event);

		///////////////////////consumer info//////		//////////////////		////////
		//_register_msg_handler(C2S_rtc_consumer_info,			"webrtc_consumer_info",		&cwebrtc_transport::handler_consumer_info);
		//_register_msg_handler(C2S_rtc_consumer_get_stats,			"webrtc_consumer_get_stats",		&cwebrtc_transport::handler_consumer_get_stats);
		//_register_msg_handler(C2S_rtc_consumer_pause,			"webrtc_consumer_pause",		&cwebrtc_transport::handler_consumer_pause);
		//_register_msg_handler(C2S_rtc_consumer_resume,			"webrtc_consumer_resume",		&cwebrtc_transport::handler_consumer_resume);
		//_register_msg_handler(C2S_rtc_consumer_set_preferred_layers,			"webrtc_consumer_set_preferred_layers",		&cwebrtc_transport::handler_consumer_set_preferred_layers);
		//_register_msg_handler(C2S_rtc_consumer_set_priority,			"webrtc_consumer_set_priority",		&cwebrtc_transport::handler_consumer_set_priority);
		//_register_msg_handler(C2S_rtc_consumer_request_key_frame,			"webrtc_consumer_request_key_frame",		&cwebrtc_transport::handler_consumer_request_key_frame);
		//_register_msg_handler(C2S_rtc_consumer_enable_trace_event,			"webrtc_consumer_enable_trace_event",		&cwebrtc_transport::handler_consumer_enable_trace_event);



		//_register_msg_handler(C2S_rtc_data_producer_close,			"webrtc_data_producer_close",		&cwebrtc_transport::handler_data_producer_close);
		//_register_msg_handler(C2S_rtc_data_consumer_close,			"webrtc_data_consumer_close",		&cwebrtc_transport::handler_data_consumer_close);

		//////////////////////////////////////////		////
		//_register_msg_handler(C2S_rtc_data_producer_info,			"webrtc_data_producer_info",		&cwebrtc_transport::handler_data_producer_info);
		//_register_msg_handler(C2S_rtc_data_producer_get_stats,			"webrtc_data_producer_get_stats",		&cwebrtc_transport::handler_data_producer_get_stats);


		//////////////////////////////////////////		/////
		//_register_msg_handler(C2S_rtc_data_consumer_info,			"webrtc_data_consumer_info",		&cwebrtc_transport::handler_data_consumer_info);
		//_register_msg_handler(C2S_rtc_data_consumer_get_stats,			"webrtc_data_consumer_get_stats",		&cwebrtc_transport::handler_data_consumer_get_stats);
		//_register_msg_handler(C2S_rtc_data_consumer_get_buffered_amount,			"webrtc_data_consumer_get_buffered_amount",		&cwebrtc_transport::handler_data_consumer_get_buffered_amount);
		//_register_msg_handler(C2S_rtc_data_consumer_set_buffered_amount_low_threshold,			"webrtc_data_consumer_set_buffered_amount_low_threshold",		&cwebrtc_transport::handler_data_consumer_set_buffered_amount_low_threshold);






		return true;
	}
	void cmsg_dispatch::destroy()
	{
		/*for (std::unordered_map < std::string, cmsg_handler*>::iterator iter = m_msg_handler_map.begin(); iter != m_msg_handler_map.end(); ++iter)
		{
			if (iter->second)
			{
				delete iter->second;
			}
		}
		m_msg_handler_map.clear();*/
	}
	cmsg_handler * cmsg_dispatch::get_msg_handler(uint16 msg_id)
	{
		if (static_cast<int> (msg_id) >= Msg_Client_Max || static_cast<int> (msg_id) <= S2C_WebrtcMessageUpdate)
		{
			return NULL;
		}

		return &(m_msg_handler[msg_id]);
	}
	void cmsg_dispatch::_register_msg_handler(uint16 msg_id, const std::string & msg_name, handler_type handler)
	{
		if (static_cast<int> (msg_id) >= Msg_Client_Max || m_msg_handler[msg_id].handler)
		{
			ERROR_LOG("[%s] register msg error, msg_id = %u, msg_name = %s", __FUNCTION__, msg_id, msg_name.c_str());
			return;
		}

		m_msg_handler[msg_id].msg_name = msg_name;//   数据统计
		m_msg_handler[msg_id].handle_count = 0;
		m_msg_handler[msg_id].handler = handler;
	}


	cmsg_dispatch g_msg_dispatch;
} //namespace chen