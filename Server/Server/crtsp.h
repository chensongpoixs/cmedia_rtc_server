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

#ifndef _C_RTSP_H_
#define _C_RTSP_H_
#include "cnet_type.h"
#include <vector>
#include "cmedia_type.h"
#include "crtp.h"
#include "cmedia_session.h"
#include <random>
//#include ""
namespace chen {
	struct RtspUrlInfo
	{
		std::string url;
		std::string ip;
		uint16_t port;
		std::string suffix;
	};

	class crtsp
	{
	public:
		crtsp() : has_auth_info_(false) {}
		virtual ~crtsp() {}

		virtual void SetAuthConfig(std::string realm, std::string username, std::string password)
		{
			realm_ = realm;
			username_ = username;
			password_ = password;
			has_auth_info_ = true;

			if (realm_ == "" || username == "") {
				has_auth_info_ = false;
			}
		}

		virtual void SetVersion(std::string version) // SDP Session Name
		{
			version_ = std::move(version);
		}

		virtual std::string GetVersion()
		{
			return version_;
		}

		virtual std::string GetRtspUrl()
		{
			return rtsp_url_info_.url;
		}

		bool ParseRtspUrl(std::string url)
		{
			char ip[100] = { 0 };
			char suffix[100] = { 0 };
			uint16_t port = 0;
#if defined(__linux) || defined(__linux__)
			if (sscanf(url.c_str() + 7, "%[^:]:%hu/%s", ip, &port, suffix) == 3)
#elif defined(WIN32) || defined(_WIN32)
			if (sscanf_s(url.c_str() + 7, "%[^:]:%hu/%s", ip, 100, &port, suffix, 100) == 3)
#endif
			{
				rtsp_url_info_.port = port;
			}
#if defined(__linux) || defined(__linux__)
			else if (sscanf(url.c_str() + 7, "%[^/]/%s", ip, suffix) == 2)
#elif defined(WIN32) || defined(_WIN32)
			else if (sscanf_s(url.c_str() + 7, "%[^/]/%s", ip, 100, suffix, 100) == 2)
#endif
			{
				rtsp_url_info_.port = 554;
			}
			else
			{
				//LOG("%s was illegal.\n", url.c_str());
				return false;
			}

			rtsp_url_info_.ip = ip;
			rtsp_url_info_.suffix = suffix;
			rtsp_url_info_.url = url;
			return true;
		}
	public:
		virtual cmedia_session* find_media_session(const std::string & suffix) { return NULL; }
		virtual cmedia_session* find_media_session(uint32 session_id) { return NULL; }
	protected:
		bool has_auth_info_ = false;
		std::string realm_;
		std::string username_;
		std::string password_;
		std::string version_;
		struct RtspUrlInfo rtsp_url_info_;
	private:
	};
}

#endif //_C_RTSP_H_