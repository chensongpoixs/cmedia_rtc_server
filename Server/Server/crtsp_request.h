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

#ifndef _C_RTSP_REQUEST_H_
#define _C_RTSP_REQUEST_H_
#include "cnet_type.h"
#include "crtsp_define.h"
#include <unordered_map>
#include "cbuffer_reader.h"
#include "crtsp.h"
#include "crtp.h"
namespace chen {

	class crtsp_request2
	{
	public:
		enum Method
		{
			OPTIONS = 0, 
			DESCRIBE, 
			SETUP, 
			PLAY, 
			TEARDOWN, 
			GET_PARAMETER,
			RTCP, 
			NONE,
		};
		const char* MethodToString[8] =
		{
			"OPTIONS", "DESCRIBE", "SETUP", "PLAY", "TEARDOWN", "GET_PARAMETER",
			"RTCP", "NONE"
		};
		enum RtspRequestParseState
		{
			kParseRequestLine,
			kParseHeadersLine,
			//kParseBody,	
			kGotAll,
		};
	public:
		explicit crtsp_request2();

		virtual ~crtsp_request2();
	
	public:
		bool ParseRequest(cbuffer_reader *buffer);

		bool GotAll() const
		{
			return state_ == kGotAll;
		}

		void Reset()
		{
			state_ = kParseRequestLine;
			request_line_param_.clear();
			header_line_param_.clear();
		}

		Method GetMethod() const
		{
			return method_;
		}

		uint32_t GetCSeq() const;

		std::string GetRtspUrl() const;

		std::string GetRtspUrlSuffix() const;

		std::string GetIp() const;

		std::string GetAuthResponse() const;

		TransportMode GetTransportMode() const
		{
			return transport_;
		}

		MediaChannelId GetChannelId() const
		{
			return channel_id_;
		}

		uint8_t  GetRtpChannel() const;
		uint8_t  GetRtcpChannel() const;
		uint16_t GetRtpPort() const;
		uint16_t GetRtcpPort() const;

		int BuildOptionRes(const char* buf, int buf_size);
		int BuildDescribeRes(const char* buf, int buf_size, const char* sdp);
		int BuildSetupMulticastRes(const char* buf, int buf_size, const char* multicast_ip, uint16_t port, uint32_t session_id);
		int BuildSetupTcpRes(const char* buf, int buf_size, uint16_t rtp_chn, uint16_t rtcp_chn, uint32_t session_id);
		int BuildSetupUdpRes(const char* buf, int buf_size, uint16_t rtp_chn, uint16_t rtcp_chn, uint32_t session_id);
		int BuildPlayRes(const char* buf, int buf_size, const char* rtp_info, uint32_t session_id);
		int BuildTeardownRes(const char* buf, int buf_size, uint32_t session_id);
		int BuildGetParamterRes(const char* buf, int buf_size, uint32_t session_id);
		int BuildNotFoundRes(const char* buf, int buf_size);
		int BuildServerErrorRes(const char* buf, int buf_size);
		int BuildUnsupportedRes(const char* buf, int buf_size);
		int BuildUnauthorizedRes(const char* buf, int buf_size, const char* realm, const char* nonce);

	private:
		bool ParseRequestLine(const char* begin, const char* end);
		bool ParseHeadersLine(const char* begin, const char* end);
		bool ParseCSeq(std::string& message);
		bool ParseAccept(std::string& message);
		bool ParseTransport(std::string& message);
		bool ParseSessionId(std::string& message);
		bool ParseMediaChannel(std::string& message);
		bool ParseAuthorization(std::string& message);


	protected:
	private:
		Method method_;
		MediaChannelId channel_id_;
		TransportMode transport_;
		std::string auth_response_;
		std::unordered_map<std::string, std::pair<std::string, uint32_t>> request_line_param_;
		std::unordered_map<std::string, std::pair<std::string, uint32_t>> header_line_param_;

		RtspRequestParseState state_ = kParseRequestLine;
	};




}

#endif //_C_RTSP_REQUEST_H_