/***********************************************************************************************
					created: 		2019-05-13
					author:			chensong
					purpose:		msg_dipatch
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