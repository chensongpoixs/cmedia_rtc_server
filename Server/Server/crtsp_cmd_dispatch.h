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

#ifndef _C_RTSP_CMD_DIPATCH_H_
#define _C_RTSP_CMD_DIPATCH_H_

#include "cnet_type.h"
#include <atomic>
#include "cwebrtc_mgr.h"
#include "cwebrtc_transport.h"
#include <json/json.h>
#include <unordered_map>
#include "cmsg_base_id.h"
#include "cparse_rtsp_request.h"
#include "crtsp_session.h"
#include <unordered_map>
namespace chen {
	//typedef void (cwebrtc_mgr::*handler_webrtc_type)(Json::Value & value);
	typedef void (crtsp_session::*rtsp_handler_type)(crtsp_request *request);
#pragma pack(push, 4)

	struct crtsp_cmd_handler
	{
		std::string							msg_name;
		std::atomic<long>					handle_count;
		rtsp_handler_type						handler;

		crtsp_cmd_handler() : msg_name(""), handle_count(0), handler(NULL) {}
	};

	//handler_type

	class crtsp_cmd_dispatch :private cnoncopyable
	{
	public:

	public:
		explicit crtsp_cmd_dispatch()
			: m_rtsp_cmd_handlers(){}
		~crtsp_cmd_dispatch();
	public:

		bool init();
		void destroy();

		crtsp_cmd_handler* get_msg_handler(const std::string & cmd_name);

	private:
		void _register_msg_handler(const std::string & cmd, const std::string& msg_name, rtsp_handler_type handler);
		//void _register_webrtc_transport_msg_handler(uint16 msg_id, const std::string& msg_name, handler_type handler);
	private:
		//crtsp_cmd_handler		m_msg_handler[Msg_Client_Max];
		std::unordered_map<std::string, crtsp_cmd_handler*>  m_rtsp_cmd_handlers;

	};

#pragma pack(pop)
	//extern cmsg_dispatch<handler_webrtc_type> g_webrtc_mgr_msg_dispatch;
	extern crtsp_cmd_dispatch g_rtsp_cmd_dispatch;
} //namespace chen

#endif //!#define _C_MSG_DIPATCH_H_