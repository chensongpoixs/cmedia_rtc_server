/***********************************************************************************************
					created: 		2019-05-13

					author:			chensong

					purpose:		msg_base_id
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


#ifndef _C_MSG_BASE_ID_H_
#define _C_MSG_BASE_ID_H_

namespace chen {
	enum EMsgBaseID
	{

		// ROOM
		C2S_BeatHeart = 200,
		S2C_BeatHeart,
		C2S_Login,
		S2C_Login,
		C2S_JoinRoom,
		S2C_JoinRoom,
		C2S_DestroyRoom ,
		S2C_DestroyRoom,

		C2S_WebrtcMessage,
		S2C_WebrtcMessage,
		C2S_RtcDataChannel,
		S2C_RtcDataChannel,
		S2C_RtcDataChannelUpdate,
		
		//C2S_CreateAnswar,
		//S2C_CreateAnswar,
		//C2S_Candidate,
		//S2C_Candidate,




		////////////////////////////////////////////////////////////////////////////////////////////
		S2C_WebrtcMessageUpdate = 500,
		S2C_JoinRoomUpdate,
		S2C_LevalRoomUpdate,

		S2C_RtpCapabilitiesUpdate,
		S2C_RtpP2pUpdate,
		//S2C_CreateAnswarUpdate,
		//S2C_CandidateUpdate,


		//////// WebRTC
		C2S_CreateRtc = 1000,
		S2C_CreateRtc,


		C2S_RtcConnect,
		S2C_RtcConnect,
		
		C2S_RtcProduce,
		S2C_RtcProduce,


		C2S_rtc_restart_ice, 
		S2C_rtc_restart_ice,
		
		C2S_rtc_info,  
		S2C_rtc_info,


		C2S_rtc_get_stats,
		S2C_rtc_get_stats,

		C2S_rtc_set_max_incoming_bitrate,
		S2C_rtc_set_max_incoming_bitrate,

		C2S_rtc_set_max_outgoing_bitrate,
		S2C_rtc_set_max_outgoing_bitrate,

		C2S_rtc_produce,
		S2C_rtc_produce,

		C2S_RtcConsume,
		S2C_RtcConsume,

		C2S_rtc_produce_data,
		S2C_rtc_produce_data,

		C2S_rtc_consume_data,
		S2C_rtc_consume_data,


		C2S_rtc_enable_trace_event,
		S2C_rtc_enable_trace_event,

		C2S_rtc_produce_close,
		S2C_rtc_produce_close,

		C2S_rtc_consumer_close,
		S2C_rtc_consumer_close,


		
		C2S_rtc_produce_info,
		S2C_rtc_produce_info,

		C2S_rtc_produce_get_stats,
		S2C_rtc_produce_get_stats,
		
		C2S_rtc_produce_pause,
		S2C_rtc_produce_pause,

		C2S_rtc_produce_resume,
		S2C_rtc_produce_resume,

		C2S_rtc_produce_enable_trace_event,
		S2C_rtc_produce_enable_trace_event,
		
		C2S_rtc_consumer_info,
		S2C_rtc_consumer_info,


		C2S_rtc_consumer_get_stats,
		S2C_rtc_consumer_get_stats,


		C2S_rtc_consumer_pause,
		S2C_rtc_consumer_pause,

		C2S_rtc_consumer_resume,
		S2C_rtc_consumer_resume,

		C2S_rtc_consumer_set_preferred_layers,
		S2C_rtc_consumer_set_preferred_layers,

		C2S_rtc_consumer_set_priority,
		S2C_rtc_consumer_set_priority,

		C2S_rtc_consumer_request_key_frame,
		S2C_rtc_consumer_request_key_frame,

		C2S_rtc_consumer_enable_trace_event,
		S2C_rtc_consumer_enable_trace_event,



		C2S_rtc_data_producer_close,
		S2C_rtc_data_producer_close,

		C2S_rtc_data_consumer_close,
		S2C_rtc_data_consumer_close,

		//////////////////		////
		C2S_rtc_data_producer_info,
		S2C_rtc_data_producer_info,


		C2S_rtc_data_producer_get_stats,
		S2C_rtc_data_producer_get_stats,


		//////////////////		/////
		C2S_rtc_data_consumer_info,
		S2C_rtc_data_consumer_info,

		C2S_rtc_data_consumer_get_stats,
		S2C_rtc_data_consumer_get_stats,

		C2S_rtc_data_consumer_get_buffered_amount,
		S2C_rtc_data_consumer_get_buffered_amount,
		
		C2S_rtc_data_consumer_set_buffered_amount_low_threshold,
		S2C_rtc_data_consumer_set_buffered_amount_low_threshold,



		C2S_rtc_publisher,
		S2C_rtc_publisher,

		C2S_rtc_player,
		S2C_rtc_player,
		C2S_rtc_requestframe,
		S2C_rtc_requestframe,
	
		Msg_Client_Max = 125000,

		
		Msg_Max,
	};

}// namespace chen

#endif // !#define _C_MSG_BASE_ID_H_