/***********************************************************************************************
					created: 		2019-05-13

					author:			chensong

					purpose:		msg_base_id
************************************************************************************************/


#ifndef _C_MSG_BASE_ID_H_
#define _C_MSG_BASE_ID_H_

namespace chen {
	enum EMsgBaseID
	{

		// ROOM
		C2S_BeatHeart = 200,
		S2C_BeatHeart,
		C2S_create_room,
		S2C_create_room,
		C2S_destroy_room ,
		S2S_destroy_room,

		//////// WebRTC
		
		C2S_rtc_connect = 1000,
		S2C_rtc_connect,
		
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

		C2S_rtc_consume,
		S2C_rtc_consume,

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

		Msg_Client_Max = 125000,

		
		Msg_Max,
	};

}// namespace chen

#endif // !#define _C_MSG_BASE_ID_H_