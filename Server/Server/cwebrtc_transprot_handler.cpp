/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		cwebrtc_transport_handler
************************************************************************************************/

#include "cwebrtc_transport.h"

namespace chen {


	bool cwebrtc_transport::handler_connect(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_restart_ice(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_info(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_get_stats(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_set_max_incoming_bitrate(uint64 session_id, Json::Value & value)
	{
		return false;
	}


	bool cwebrtc_transport::handler_set_max_outgoing_bitrate(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consume(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_data(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consume_data(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_enable_trace_event(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_close(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_close(uint64 session_id, Json::Value& value)
	{
		return false;
	}

	/////////////////////produce info /////////////////////////
	bool cwebrtc_transport::handler_produce_info(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_get_stats(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_pause(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_resume(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_enable_trace_event(uint64 session_id, Json::Value & value)
	{
		return false;
	}

	/////////////////////consumer info////////////////////////////////
	bool cwebrtc_transport::handler_consumer_info(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_get_stats(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_pause(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_resume(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_set_preferred_layers(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_set_priority(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_request_key_frame(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_enable_trace_event(uint64 session_id, Json::Value & value)
	{
		return false;
	}

	///////////////////////////////////////////////////////////
	bool cwebrtc_transport::handler_data_producer_close(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_data_consumer_close(uint64 session_id, Json::Value& value)
	{
		return false;
	}

	bool cwebrtc_transport::handler_data_producer_info(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_data_producer_get_stats(uint64 session_id, Json::Value & value)
	{
		return false;
	}


	bool cwebrtc_transport::handler_data_consumer_info(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_data_consumer_get_stats(uint64 session_id, Json::Value & value)
	{
		return false;
	}

	bool cwebrtc_transport::handler_data_consumer_get_buffered_amount(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_data_consumer_set_buffered_amount_low_threshold(uint64 session_id, Json::Value& value)
	{
		return false;
	}
}