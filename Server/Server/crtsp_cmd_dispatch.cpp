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
#include "crtsp_cmd_dispatch.h"
#include <map>
#include "crtsp_session.h"
namespace chen {
	/*crtsp_cmd_dispatch::crtsp_cmd_dispatch()
	{
	}*/
	crtsp_cmd_dispatch::~crtsp_cmd_dispatch()
	{
	}
	bool crtsp_cmd_dispatch::init()
	{

		_register_msg_handler("OPTIONS",			"OPTIONS",				&crtsp_session::handler_options);
		_register_msg_handler("DESCRIBE",			"DESCRIBE",				&crtsp_session::handler_describe);
		_register_msg_handler("SETUP",				"SETUP",				&crtsp_session::handler_setup);
		_register_msg_handler("TEARDOWN",			"TEARDOWN",				&crtsp_session::handler_teardown);
		_register_msg_handler("PLAY",				"PLAY",					&crtsp_session::handler_play);
		_register_msg_handler("PAUSE",				"PAUSE",				&crtsp_session::handler_pause);
		_register_msg_handler("GET_PARAMETER",		"GET_PARAMETER",		&crtsp_session::handler_get_parameter);
		_register_msg_handler("SET_PARAMETER",		"SET_PARAMETER",		&crtsp_session::handler_set_parameter);
		  

		return true;
	}
	void crtsp_cmd_dispatch::destroy()
	{
		for (std::unordered_map < std::string, crtsp_cmd_handler*>::iterator iter = m_rtsp_cmd_handlers.begin(); iter != m_rtsp_cmd_handlers.end(); ++iter)
		{
			if (iter->second)
			{
				delete iter->second;
			}
		}
		
		m_rtsp_cmd_handlers.clear();
	}
	crtsp_cmd_handler * crtsp_cmd_dispatch::get_msg_handler(const std::string & cmd_name)
	{
		/*if (static_cast<int> (msg_id) >= Msg_Client_Max || static_cast<int> (msg_id) <= S2C_WebrtcMessageUpdate)
		{
			return NULL;
		}

		return &(m_msg_handler[msg_id]);*/
		std::unordered_map < std::string, crtsp_cmd_handler*>::iterator iter = m_rtsp_cmd_handlers.find(cmd_name);
		if (iter != m_rtsp_cmd_handlers.end())
		{
			return iter->second;
		}
		WARNING_EX_LOG("not find rtsp cmd_name = %s", cmd_name.c_str());
		return NULL;

	}
	void crtsp_cmd_dispatch::_register_msg_handler(const std::string & cmd, const std::string & msg_name, rtsp_handler_type handler)
	{
		//if (static_cast<int> (msg_id) >= Msg_Client_Max || m_msg_handler[msg_id].handler)
		//{
		//	ERROR_LOG("[%s] register msg error, msg_id = %u, msg_name = %s", __FUNCTION__, msg_id, msg_name.c_str());
		//	return;
		//}

		//m_msg_handler[msg_id].msg_name = msg_name;//   数据统计
		//m_msg_handler[msg_id].handle_count = 0;
		//m_msg_handler[msg_id].handler = handler;
		crtsp_cmd_handler* cmd_handler = new crtsp_cmd_handler();

		cmd_handler->handle_count = 0;
		cmd_handler->msg_name = msg_name;
		cmd_handler->handler = handler;
		if (!m_rtsp_cmd_handlers.insert(std::make_pair(cmd, cmd_handler)).second)
		{ 
			delete cmd_handler;
			cmd_handler = NULL;
			WARNING_EX_LOG(" register [cmd = %s] failed !!!", cmd.c_str());
			return;
		}
	}


	crtsp_cmd_dispatch g_rtsp_cmd_dispatch;
} //namespace chen