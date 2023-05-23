/***********************************************************************************************
created: 		2023-05-11

author:			chensong

purpose:		_C_DTLS_ _H_
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
#include "crtsp_session.h"
#include <iostream>
#include <sstream>
#include "ctime_api.h"
#include "crtsp_define.h"
#include "ccfg.h"
namespace chen {
	 
	crtsp_session::~crtsp_session()
	{
	}
	void crtsp_session::set_cseq(uint32 cseq)
	{
	}

	void crtsp_session::set_session(ctcp_connection * session)
	{
		 // TODO@chensong 2023-05-23 单线程 没有问题 多线程是有问题哈 ^_^
		//memcpy(m_session_ptr, session, sizeof(ctcp_connection));
		m_session_ptr = session;
	}

	void crtsp_session::handler_options(crtsp_request* request)
	{
		NORMAL_EX_LOG("");
		std::stringstream cmd;

		cmd << "RTSP/1.0 200 OK" << kCRLF;
		cmd << "CSeq: " << request->get_cseq() << kCRLF;
		cmd << crtsp_api::date_header();
		cmd << "Public: %s" << RTSP_ALLOWED_COMMAND <<  kCRLF << kCRLF;
		
		_send_msg((const uint8*)cmd.str().c_str(), cmd.str().length());
	}

	void crtsp_session::handler_describe(crtsp_request* request)
	{
		NORMAL_EX_LOG("");
		std::stringstream cmd;
		cmd << "RTSP/1.0 200 OK" << kCRLF;
		cmd << "CSeq: " << request->get_cseq() << kCRLF;
		cmd << crtsp_api::date_header();

		/*
		Content-Base: rtsp://127.0.0.1/input.264/
		Content-Type: application/sdp
		Content-Length: 515
		*/
		cmd << "Content-Base: " << request->get_rtsp_url() << "/" << kCRLF;
		cmd << "Content-Type: application/sdp" << kCRLF;
		cmd << "Content-Length: " << kCRLF;


		std::stringstream sdp_session;

		sdp_session << "v=0" << kCRLF;
		sdp_session << "o=- " << (this) << this << " 1 IN IP4 " << g_cfg.get_string(ECI_RtspWanIp) << kCRLF;
		sdp_session << "s=H.264 Video, Media RTSP Server v1.0" << kCRLF;
		sdp_session << "i=" << kCRLF;
		sdp_session << "t=0 0" << kCRLF;
		sdp_session << "a=tool: Media RTSP Server v1.0" << kCRLF;
		sdp_session << "a=type:broadcast" << kCRLF;
		sdp_session << "a=control:*" << kCRLF;
		/*
		 "v=0\r\n"
		  "o=- %ld%06ld %d IN IP4 %s\r\n"
		  "s=%s\r\n"
		  "i=%s\r\n"
		  "t=0 0\r\n"
		  "a=tool: Media RTSP Server v1.0\r\n"
		  "a=type:broadcast\r\n"
		  "a=control:*\r\n"
		  "%s"
		  "%s"
		  "a=x-qt-text-nam:%s\r\n"
		  "a=x-qt-text-inf:%s\r\n"
		  "%s";
		
		*/


		_send_msg((const uint8*)cmd.str().c_str(), cmd.str().length());
	}

	void crtsp_session::handler_register(crtsp_request* request)
	{
		NORMAL_EX_LOG("");
	}

	void crtsp_session::handler_setup(crtsp_request* request)
	{
		NORMAL_EX_LOG("");
	}

	void crtsp_session::handler_teardown(crtsp_request* request)
	{
		NORMAL_EX_LOG("");
	}

	void crtsp_session::handler_play(crtsp_request* request)
	{
		NORMAL_EX_LOG("");
	}

	void crtsp_session::handler_pause(crtsp_request* request)
	{
		NORMAL_EX_LOG("");
	}



	void crtsp_session::handler_get_parameter(crtsp_request* request)
	{
		NORMAL_EX_LOG("");
	}
	void crtsp_session::handler_set_parameter(crtsp_request* request)
	{
		NORMAL_EX_LOG("");
	}

	bool crtsp_session::_send_msg(const uint8 * data, size_t len)
	{
		if (!m_session_ptr)
		{
			WARNING_EX_LOG(" session_ptr == NULL !!!");
			return false;
		}
		NORMAL_EX_LOG("[data = %s]", data);
		  m_session_ptr->Send(data, len, NULL);
		  return true;
	}

}