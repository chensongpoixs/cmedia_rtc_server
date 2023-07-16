#include "crtsp_request.h"
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
#include "crtsp_request.h"
#include "cmedia_type.h" 
#include "crtp.h"
namespace chen {
	crtsp_request2::crtsp_request2()
	{
	}
	crtsp_request2::~crtsp_request2()
	{
	}
	bool crtsp_request2::ParseRequest(cbuffer_reader * buffer)
	{
		if (buffer->Peek()[0] == '$') {
			method_ = RTCP;
			return true;
		}

		bool ret = true;
		while (1) {
			if (state_ == kParseRequestLine) {
				const char* firstCrlf = buffer->FindFirstCrlf();
				if (firstCrlf != nullptr)
				{
					ret = ParseRequestLine(buffer->Peek(), firstCrlf);
					buffer->RetrieveUntil(firstCrlf + 2);
				}

				if (state_ == kParseHeadersLine) {
					continue;
				}
				else {
					break;
				}
			}
			else if (state_ == kParseHeadersLine) {
				const char* lastCrlf = buffer->FindLastCrlf();
				if (lastCrlf != nullptr) {
					ret = ParseHeadersLine(buffer->Peek(), lastCrlf);
					buffer->RetrieveUntil(lastCrlf + 2);
				}
				break;
			}
			else if (state_ == kGotAll) {
				buffer->RetrieveAll();
				return true;
			}
		}

		return ret;
	}
	uint32_t crtsp_request2::GetCSeq() const
	{
		uint32_t cseq = 0;
		auto iter = header_line_param_.find("cseq");
		if (iter != header_line_param_.end()) {
			cseq = iter->second.second;
		}

		return cseq;
	}
	std::string crtsp_request2::GetRtspUrl() const
	{
		auto iter = request_line_param_.find("url");
		if (iter != request_line_param_.end()) {
			return iter->second.first;
		}

		return "";
	}
	std::string crtsp_request2::GetRtspUrlSuffix() const
	{
		auto iter = request_line_param_.find("url_suffix");
		if (iter != request_line_param_.end()) {
			return iter->second.first;
		}

		return "";
	}
	std::string crtsp_request2::GetIp() const
	{
		auto iter = request_line_param_.find("url_ip");
		if (iter != request_line_param_.end()) {
			return iter->second.first;
		}

		return "";
	}
	std::string crtsp_request2::GetAuthResponse() const
	{
		return auth_response_;
	}
	uint8_t crtsp_request2::GetRtpChannel() const
	{
		auto iter = header_line_param_.find("rtp_channel");
		if (iter != header_line_param_.end()) {
			return iter->second.second;
		}

		return 0;
	}
	uint8_t crtsp_request2::GetRtcpChannel() const
	{
		auto iter = header_line_param_.find("rtcp_channel");
		if (iter != header_line_param_.end()) {
			return iter->second.second;
		}

		return 0;
	}
	uint16_t crtsp_request2::GetRtpPort() const
	{
		auto iter = header_line_param_.find("rtp_port");
		if (iter != header_line_param_.end()) {
			return iter->second.second;
		}

		return 0;
	}
	uint16_t crtsp_request2::GetRtcpPort() const
	{
		auto iter = header_line_param_.find("rtcp_port");
		if (iter != header_line_param_.end()) {
			return iter->second.second;
		}

		return 0;
	}
	int crtsp_request2::BuildOptionRes(const char * buf, int buf_size)
	{
		memset((void*)buf, 0, buf_size);
		snprintf((char*)buf, buf_size,
			"RTSP/1.0 200 OK\r\n"
			"CSeq: %u\r\n"
			"Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY\r\n"
			"\r\n",
			this->GetCSeq());

		return (int)strlen(buf);
	}
	int crtsp_request2::BuildDescribeRes(const char * buf, int buf_size, const char * sdp)
	{
		memset((void*)buf, 0, buf_size);
		snprintf((char*)buf, buf_size,
			"RTSP/1.0 200 OK\r\n"
			"CSeq: %u\r\n"
			"Content-Length: %d\r\n"
			"Content-Type: application/sdp\r\n"
			"\r\n"
			"%s",
			this->GetCSeq(),
			(int)strlen(sdp),
			sdp);

		return (int)strlen(buf);
	}
	int crtsp_request2::BuildSetupMulticastRes(const char * buf, int buf_size, const char * multicast_ip, uint16_t port, uint32_t session_id)
	{
		memset((void*)buf, 0, buf_size);
		snprintf((char*)buf, buf_size,
			"RTSP/1.0 200 OK\r\n"
			"CSeq: %u\r\n"
			"Transport: RTP/AVP;multicast;destination=%s;source=%s;port=%u-0;ttl=255\r\n"
			"Session: %u\r\n"
			"\r\n",
			this->GetCSeq(),
			multicast_ip,
			this->GetIp().c_str(),
			port,
			session_id);

		return (int)strlen(buf);
	}
	int crtsp_request2::BuildSetupTcpRes(const char * buf, int buf_size, uint16_t rtp_chn, uint16_t rtcp_chn, uint32_t session_id)
	{
		memset((void*)buf, 0, buf_size);
		snprintf((char*)buf, buf_size,
			"RTSP/1.0 200 OK\r\n"
			"CSeq: %u\r\n"
			"Transport: RTP/AVP/TCP;unicast;interleaved=%d-%d\r\n"
			"Session: %u\r\n"
			"\r\n",
			this->GetCSeq(),
			rtp_chn, rtcp_chn,
			session_id);

		return (int)strlen(buf);
	}
	int crtsp_request2::BuildSetupUdpRes(const char * buf, int buf_size, uint16_t rtp_chn, uint16_t rtcp_chn, uint32_t session_id)
	{
		memset((void*)buf, 0, buf_size);
		snprintf((char*)buf, buf_size,
			"RTSP/1.0 200 OK\r\n"
			"CSeq: %u\r\n"
			"Transport: RTP/AVP;unicast;client_port=%hu-%hu;server_port=%hu-%hu\r\n"
			"Session: %u\r\n"
			"\r\n",
			this->GetCSeq(),
			this->GetRtpPort(),
			this->GetRtcpPort(),
			rtp_chn,
			rtcp_chn,
			session_id);

		return (int)strlen(buf);
	}
	int crtsp_request2::BuildPlayRes(const char * buf, int buf_size, const char * rtp_info, uint32_t session_id)
	{
		memset((void*)buf, 0, buf_size);
		snprintf((char*)buf, buf_size,
			"RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Range: npt=0.000-\r\n"
			"Session: %u; timeout=60\r\n",
			this->GetCSeq(),
			session_id);

		if (rtp_info != nullptr) {
			snprintf((char*)buf + strlen(buf), buf_size - strlen(buf), "%s\r\n", rtp_info);
		}

		snprintf((char*)buf + strlen(buf), buf_size - strlen(buf), "\r\n");
		return (int)strlen(buf);
	}
	int crtsp_request2::BuildTeardownRes(const char * buf, int buf_size, uint32_t session_id)
	{
		memset((void*)buf, 0, buf_size);
		snprintf((char*)buf, buf_size,
			"RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Session: %u\r\n"
			"\r\n",
			this->GetCSeq(),
			session_id);

		return (int)strlen(buf);
	}
	int crtsp_request2::BuildGetParamterRes(const char * buf, int buf_size, uint32_t session_id)
	{
		memset((void*)buf, 0, buf_size);
		snprintf((char*)buf, buf_size,
			"RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Session: %u\r\n"
			"\r\n",
			this->GetCSeq(),
			session_id);

		return (int)strlen(buf);
	}
	int crtsp_request2::BuildNotFoundRes(const char * buf, int buf_size)
	{
		memset((void*)buf, 0, buf_size);
		snprintf((char*)buf, buf_size,
			"RTSP/1.0 404 Stream Not Found\r\n"
			"CSeq: %u\r\n"
			"\r\n",
			this->GetCSeq());

		return (int)strlen(buf);
	}
	int crtsp_request2::BuildServerErrorRes(const char * buf, int buf_size)
	{
		memset((void*)buf, 0, buf_size);
		snprintf((char*)buf, buf_size,
			"RTSP/1.0 500 Internal Server Error\r\n"
			"CSeq: %u\r\n"
			"\r\n",
			this->GetCSeq());

		return (int)strlen(buf);
	}
	int crtsp_request2::BuildUnsupportedRes(const char * buf, int buf_size)
	{
		memset((void*)buf, 0, buf_size);
		snprintf((char*)buf, buf_size,
			"RTSP/1.0 461 Unsupported transport\r\n"
			"CSeq: %d\r\n"
			"\r\n",
			this->GetCSeq());

		return (int)strlen(buf);
	}
	int crtsp_request2::BuildUnauthorizedRes(const char * buf, int buf_size, const char * realm, const char * nonce)
	{
		memset((void*)buf, 0, buf_size);
		snprintf((char*)buf, buf_size,
			"RTSP/1.0 401 Unauthorized\r\n"
			"CSeq: %d\r\n"
			"WWW-Authenticate: Digest realm=\"%s\", nonce=\"%s\"\r\n"
			"\r\n",
			this->GetCSeq(),
			realm,
			nonce);

		return (int)strlen(buf);
	}
	bool crtsp_request2::ParseRequestLine(const char * begin, const char * end)
	{
		std::string message(begin, end);
		char method[64] = { 0 };
		char url[512] = { 0 };
		char version[64] = { 0 };

		if (sscanf(message.c_str(), "%s %s %s", method, url, version) != 3) {
			return true;
		}

		std::string method_str(method);
		if (method_str == "OPTIONS") {
			method_ = OPTIONS;
		}
		else if (method_str == "DESCRIBE") {
			method_ = DESCRIBE;
		}
		else if (method_str == "SETUP") {
			method_ = SETUP;
		}
		else if (method_str == "PLAY") {
			method_ = PLAY;
		}
		else if (method_str == "TEARDOWN") {
			method_ = TEARDOWN;
		}
		else if (method_str == "GET_PARAMETER") {
			method_ = GET_PARAMETER;
		}
		else {
			method_ = NONE;
			return false;
		}

		if (strncmp(url, "rtsp://", 7) != 0) {
			return false;
		}

		// parse url
		uint16_t port = 0;
		char ip[64] = { 0 };
		char suffix[64] = { 0 };

		if (sscanf(url + 7, "%[^:]:%hu/%s", ip, &port, suffix) == 3) {

		}
		else if (sscanf(url + 7, "%[^/]/%s", ip, suffix) == 2) {
			port = 554;
		}
		else {
			return false;
		}

		request_line_param_.emplace("url", std::make_pair(std::string(url), 0));
		request_line_param_.emplace("url_ip", std::make_pair(std::string(ip), 0));
		request_line_param_.emplace("url_port", std::make_pair("", (uint32_t)port));
		request_line_param_.emplace("url_suffix", std::make_pair(std::string(suffix), 0));
		request_line_param_.emplace("version", std::make_pair(std::string(version), 0));
		request_line_param_.emplace("method", std::make_pair(std::move(method_str), 0));

		state_ = kParseHeadersLine;
		return true;
	}
	bool crtsp_request2::ParseHeadersLine(const char * begin, const char * end)
	{
		std::string message(begin, end);
		if (!ParseCSeq(message)) {
			if (header_line_param_.find("cseq") == header_line_param_.end()) {
				return false;
			}
		}

		if (method_ == DESCRIBE || method_ == SETUP || method_ == PLAY) {
			ParseAuthorization(message);
		}

		if (method_ == OPTIONS) {
			state_ = kGotAll;
			return true;
		}

		if (method_ == DESCRIBE) {
			if (ParseAccept(message)) {
				state_ = kGotAll;
			}
			return true;
		}

		if (method_ == SETUP) {
			if (ParseTransport(message)) {
				ParseMediaChannel(message);
				state_ = kGotAll;
			}

			return true;
		}

		if (method_ == PLAY) {
			if (ParseSessionId(message)) {
				state_ = kGotAll;
			}
			return true;
		}

		if (method_ == TEARDOWN) {
			state_ = kGotAll;
			return true;
		}

		if (method_ == GET_PARAMETER) {
			state_ = kGotAll;
			return true;
		}

		return true;
	}
	bool crtsp_request2::ParseCSeq(std::string & message)
	{
		std::size_t pos = message.find("CSeq");
		if (pos != std::string::npos) {
			uint32_t cseq = 0;
			sscanf(message.c_str() + pos, "%*[^:]: %u", &cseq);
			header_line_param_.emplace("cseq", std::make_pair("", cseq));
			return true;
		}

		return false;
	}
	bool crtsp_request2::ParseAccept(std::string & message)
	{
		if ((message.rfind("Accept") == std::string::npos)
			|| (message.rfind("sdp") == std::string::npos)) {
			return false;
		}

		return true;
	}
	bool crtsp_request2::ParseTransport(std::string & message)
	{
		std::size_t pos = message.find("Transport");
		if (pos != std::string::npos) {
			if ((pos = message.find("RTP/AVP/TCP")) != std::string::npos) {
				transport_ = RTP_OVER_TCP;
				uint16_t rtpChannel = 0, rtcpChannel = 0;
				if (sscanf(message.c_str() + pos, "%*[^;];%*[^;];%*[^=]=%hu-%hu", &rtpChannel, &rtcpChannel) != 2) {
					return false;
				}
				header_line_param_.emplace("rtp_channel", std::make_pair("", rtpChannel));
				header_line_param_.emplace("rtcp_channel", std::make_pair("", rtcpChannel));
			}
			else if ((pos = message.find("RTP/AVP")) != std::string::npos) {
				uint16_t rtp_port = 0, rtcpPort = 0;
				if (((message.find("unicast", pos)) != std::string::npos)) {
					transport_ = RTP_OVER_UDP;
					if (sscanf(message.c_str() + pos, "%*[^;];%*[^;];%*[^=]=%hu-%hu",
						&rtp_port, &rtcpPort) != 2)
					{
						return false;
					}

				}
				else if ((message.find("multicast", pos)) != std::string::npos) {
					transport_ = RTP_OVER_MULTICAST;
				}
				else {
					return false;
				}

				header_line_param_.emplace("rtp_port", std::make_pair("", rtp_port));
				header_line_param_.emplace("rtcp_port", std::make_pair("", rtcpPort));
			}
			else {
				return false;
			}

			return true;
		}

		return false;
	}
	bool crtsp_request2::ParseSessionId(std::string & message)
	{
		std::size_t pos = message.find("Session");
		if (pos != std::string::npos) {
			uint32_t session_id = 0;
			if (sscanf(message.c_str() + pos, "%*[^:]: %u", &session_id) != 1) {
				return false;
			}
			return true;
		}

		return false;
		return false;
	}
	bool crtsp_request2::ParseMediaChannel(std::string & message)
	{
		channel_id_ = channel_0;

		auto iter = request_line_param_.find("url");
		if (iter != request_line_param_.end()) 
		{
			std::size_t pos = iter->second.first.find("track1");
			if (pos != std::string::npos) 
			{
				channel_id_ = channel_1;
			}
		}

		return true;
	}
	bool crtsp_request2::ParseAuthorization(std::string & message)
	{
		std::size_t pos = message.find("Authorization");
		if (pos != std::string::npos) 
		{
			if ((pos = message.find("response=")) != std::string::npos)
			{
				auth_response_ = message.substr(pos + 10, 32);
				if (auth_response_.size() == 32) 
				{
					return true;
				}
			}
		}

		auth_response_.clear();
		return false;
	}
}

 