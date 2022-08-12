/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		cwebrtc_transport_handler
************************************************************************************************/

#include "cwebrtc_transport.h"

namespace chen {


	bool cwebrtc_transport::handler_connect(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_restart_ice(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_info(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_get_stats(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_set_max_incoming_bitrate(Json::Value & value)
	{
		return false;
	}


	bool cwebrtc_transport::handler_set_max_outgoing_bitrate(Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consume(Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_data(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consume_data(Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_enable_trace_event(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_close(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_close(Json::Value& value)
	{
		return false;
	}

	/////////////////////produce info /////////////////////////
	bool cwebrtc_transport::handler_produce_info(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_get_stats(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_pause(Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_resume(Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_enable_trace_event(Json::Value & value)
	{
		return false;
	}

	/////////////////////consumer info////////////////////////////////
	bool cwebrtc_transport::handler_consumer_info(Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_get_stats(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_pause(Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_resume(Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_set_preferred_layers(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_set_priority(Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_request_key_frame(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_enable_trace_event(Json::Value & value)
	{
		return false;
	}

	///////////////////////////////////////////////////////////
	bool cwebrtc_transport::handler_data_producer_close(Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_data_consumer_close(Json::Value& value)
	{
		return false;
	}

	bool cwebrtc_transport::handler_data_producer_info(Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_data_producer_get_stats(Json::Value & value)
	{
		return false;
	}


	bool cwebrtc_transport::handler_data_consumer_info(Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_data_consumer_get_stats(Json::Value & value)
	{
		return false;
	}

	bool cwebrtc_transport::handler_data_consumer_get_buffered_amount(Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_data_consumer_set_buffered_amount_low_threshold(Json::Value& value)
	{
		return false;
	}
}