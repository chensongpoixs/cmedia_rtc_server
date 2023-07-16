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
#include "cglobal_rtsp.h"
#include "ch264_source.h"
#include "ch264_file.h"
#include "clog.h"
#include "crtsp_server.h"
#include "ccfg.h"
namespace chen 
{

	void SendFrameThread( uint32 session_id, ch264_file* h264_file)
	{
		int buf_size = 2000000;
		std::unique_ptr<uint8_t> frame_buf(new uint8_t[buf_size]);

		while (1) {
			bool end_of_frame = false;
			int frame_size = h264_file->ReadFrame((char*)frame_buf.get(), buf_size, &end_of_frame);
			if (frame_size > 0) {
				AVFrame videoFrame = { 0 };
				videoFrame.type = 0;
				videoFrame.size = frame_size;
				videoFrame.timestamp = ch264_source::get_timestamp();
				videoFrame.buffer.reset(new uint8_t[videoFrame.size]);
				memcpy(videoFrame.buffer.get(), frame_buf.get(), videoFrame.size);
				g_rtsp_server.push_frame(session_id, channel_0, videoFrame);
			}
			else {
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		};
	}
	bool init_rtsp_global()
	{


		ch264_file* h264_file = new ch264_file();
		if (!h264_file->Open(g_cfg.get_string(ECI_RtspTestH264File).c_str())) 
		{
			ERROR_EX_LOG("Open %s failed.\n", "test.h264");
			return 0;
		}

		cmedia_session* session_ptr = cmedia_session::construct();
		session_ptr->init("live");
		ch264_source* h264_source_ptr = ch264_source::construct();
		h264_source_ptr->init(25);
		session_ptr->add_source(channel_0, h264_source_ptr);
		uint32 session_id = g_rtsp_server.add_session(session_ptr);

		std::thread t1(SendFrameThread, session_id, h264_file);
		t1.detach();

		return true;
	}
	void destroy_rtsp_global()
	{
	}
}