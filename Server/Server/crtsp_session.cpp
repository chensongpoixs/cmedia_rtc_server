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
#include "crtsp_server.h"

namespace chen {
	static const uint32  MAX_RTSP_MESSAGE_SIZE = 2048;
	crtsp_session::~crtsp_session()
	{
	}
	void crtsp_session::set_cseq(uint32 cseq)
	{
		m_cseq = cseq;
	}

	void crtsp_session::set_session(ctcp_connection * session)
	{
		 // TODO@chensong 2023-05-23 单线程 没有问题 多线程是有问题哈 ^_^
		//memcpy(m_session_ptr, session, sizeof(ctcp_connection));
		m_session_ptr = session;
	}

	bool crtsp_session::on_received(uint8 * data, size_t size)
	{

		//KeepAlive();
		cbuffer_reader buffer;
		//std::string rtsp_data((char *)data, size);
		buffer.Read( data, size);
		int32 len = buffer.ReadableBytes();
		if (len <= 0) {
			return false; //close
		}

		//if (conn_mode_ == RTSP_SERVER)
		{
			if (!HandleRtspRequest(buffer)) 
			{
				return false;
			}
		}
		/*else if (conn_mode_ == RTSP_PUSHER) {
			if (!HandleRtspResponse(buffer)) {
				return false;
			}
		}*/

		if (buffer.ReadableBytes() > MAX_RTSP_MESSAGE_SIZE) {
			buffer.RetrieveAll();
		}

		return true;
	}

	bool crtsp_session::HandleRtspRequest(cbuffer_reader & buffer)
	{
#if RTSP_DEBUG
		std::string str(buffer.Peek(), buffer.ReadableBytes());
		if (str.find("rtsp") != std::string::npos || str.find("RTSP") != std::string::npos)
		{
			NORMAL_EX_LOG("%s", str.c_str() );
			//std::cout << str << std::endl;
		}
#endif

		if (m_rtsp_request.ParseRequest(&buffer)) {
			crtsp_request2::Method method = m_rtsp_request.GetMethod();
			if (method == crtsp_request2::RTCP) {
				HandleRtcp(buffer);
				return true;
			}
			else if (!m_rtsp_request.GotAll()) {
				return true;
			}

			switch (method)
			{
			case crtsp_request2::OPTIONS:
				HandleCmdOption();
				break;
			case crtsp_request2::DESCRIBE:
				HandleCmdDescribe();
				break;
			case crtsp_request2::SETUP:
				HandleCmdSetup();
				break;
			case crtsp_request2::PLAY:
				HandleCmdPlay();
				break;
			case crtsp_request2::TEARDOWN:
				HandleCmdTeardown();
				break;
			case crtsp_request2::GET_PARAMETER:
				HandleCmdGetParamter();
				break;
			default:
				break;
			}

			if (m_rtsp_request.GotAll()) {
				m_rtsp_request.Reset();
			}
		}
		else {
			return false;
		}

		return true;
	}

	void crtsp_session::HandleRtcp(cbuffer_reader & buffer)
	{
		char *peek = buffer.Peek();
		if (peek[0] == '$' &&  buffer.ReadableBytes() > 4) {
			uint32_t pkt_size = peek[2] << 8 | peek[3];
			if (pkt_size + 4 >= buffer.ReadableBytes()) {
				buffer.Retrieve(pkt_size + 4);
			}
		}
	}

	void crtsp_session::HandleCmdOption()
	{
		std::shared_ptr<char> res(new char[2048], std::default_delete<char[]>());
		int size = m_rtsp_request.BuildOptionRes(res.get(), 2048);
		this->SendRtspMessage(res, size);
	}

	void crtsp_session::HandleCmdDescribe()
	{
		if (/*auth_info_ != nullptr &&*/ !HandleAuthentication()) 
		{
			return;
		}

		/*if (rtp_conn_ == nullptr) {
			rtp_conn_.reset(new RtpConnection(shared_from_this()));
		}*/

		int size = 0;
		std::shared_ptr<char> res(new char[4096], std::default_delete<char[]>());
		//MediaSession::Ptr media_session = nullptr;

		/*auto rtsp = rtsp_.lock();
		if (rtsp) {
			media_session = rtsp->LookMediaSession(m_rtsp_request.GetRtspUrlSuffix());
		}*/
		cmedia_session * media_session_ptr = g_rtsp_server.find_media_session(m_rtsp_request.GetRtspUrlSuffix());

		if (!media_session_ptr) 
		{
			size = m_rtsp_request.BuildNotFoundRes(res.get(), 4096);
		}
		else {
			m_session_id = media_session_ptr->get_media_session_id();
			media_session_ptr->add_client(&m_rtp_connection);

			for (int chn = 0; chn < MAX_MEDIA_CHANNEL; chn++) 
			{
				cmedia_source* source = media_session_ptr->get_media_source((MediaChannelId)chn);
				if (source != nullptr) {
					m_rtp_connection.SetClockRate((MediaChannelId)chn, source->GetClockRate());
					m_rtp_connection.SetPayloadType((MediaChannelId)chn, source->GetPayloadType());
				}
			}

			std::string sdp = media_session_ptr->get_sdp_message("127.0.0.1"/*SocketUtil::GetSocketIp(this->GetSocket())*/, g_rtsp_server.GetVersion());
			if (sdp == "") {
				size = m_rtsp_request.BuildServerErrorRes(res.get(), 4096);
			}
			else {
				size = m_rtsp_request.BuildDescribeRes(res.get(), 4096, sdp.c_str());
			}
		}

		SendRtspMessage(res, size);
	}

	void crtsp_session::HandleCmdSetup()
	{
		if (/*auth_info_ != nullptr &&*/ !HandleAuthentication()) 
		{
			return;
		}

		int size = 0;
		std::shared_ptr<char> res(new char[4096], std::default_delete<char[]>());
		MediaChannelId channel_id = m_rtsp_request.GetChannelId();
		/*MediaSession::Ptr media_session = nullptr;

		auto rtsp = rtsp_.lock();
		if (rtsp) {
			media_session = rtsp->LookMediaSession(session_id_);
		}
*/
		cmedia_session * media_session_ptr = g_rtsp_server.find_media_session(m_session_id);
		if (/*!rtsp ||*/ !media_session_ptr) {
			goto server_error;
		}

		if (media_session_ptr->is_multicast()) 
		{
			std::string multicast_ip = media_session_ptr->get_multicast_ip();
			if (m_rtsp_request.GetTransportMode() == RTP_OVER_MULTICAST) 
			{
				uint16_t port = media_session_ptr->get_multicast_port(channel_id);
				uint16_t session_id = m_rtp_connection.GetRtpSessionId();
				if (!m_rtp_connection.SetupRtpOverMulticast(channel_id, multicast_ip.c_str(), port))
				{
					goto server_error;
				}

				size = m_rtsp_request.BuildSetupMulticastRes(res.get(), 4096, multicast_ip.c_str(), port, session_id);
			}
			else {
				goto transport_unsupport;
			}
		}
		else {
			if (m_rtsp_request.GetTransportMode() == RTP_OVER_TCP) 
			{
				uint16_t rtp_channel = m_rtsp_request.GetRtpChannel();
				uint16_t rtcp_channel = m_rtsp_request.GetRtcpChannel();
				uint32 session_id = m_rtp_connection.GetRtpSessionId();

				m_rtp_connection.SetupRtpOverTcp(channel_id, rtp_channel, rtcp_channel);
				size = m_rtsp_request.BuildSetupTcpRes(res.get(), 4096, rtp_channel, rtcp_channel, session_id);
			}
			else if (m_rtsp_request.GetTransportMode() == RTP_OVER_UDP) {
				uint16_t peer_rtp_port = m_rtsp_request.GetRtpPort();
				uint16_t peer_rtcp_port = m_rtsp_request.GetRtcpPort();
				uint16_t session_id = m_rtp_connection.GetRtpSessionId();

				if (m_rtp_connection.SetupRtpOverUdp(channel_id, peer_rtp_port, peer_rtcp_port)) {
					//SOCKET rtcp_fd = m_rtp_connection.GetRtcpSocket(channel_id);
					//rtcp_channels_[channel_id].reset(new Channel(rtcp_fd));
					//rtcp_channels_[channel_id]->SetReadCallback([rtcp_fd, this]() { this->HandleRtcp(rtcp_fd); });
					//rtcp_channels_[channel_id]->EnableReading();
					//task_scheduler_->UpdateChannel(rtcp_channels_[channel_id]);
				}
				else {
					goto server_error;
				}

				uint16_t serRtpPort = m_rtp_connection.GetRtpPort(channel_id);
				uint16_t serRtcpPort = m_rtp_connection.GetRtcpPort(channel_id);
				size = m_rtsp_request.BuildSetupUdpRes(res.get(), 4096, serRtpPort, serRtcpPort, session_id);
			}
			else {
				goto transport_unsupport;
			}
		}

		SendRtspMessage(res, size);
		return;

	transport_unsupport:
		size = m_rtsp_request.BuildUnsupportedRes(res.get(), 4096);
		SendRtspMessage(res, size);
		return;

	server_error:
		size = m_rtsp_request.BuildServerErrorRes(res.get(), 4096);
		SendRtspMessage(res, size);
		return;
	}

	void crtsp_session::HandleCmdPlay()
	{
		//if (auth_info_ != nullptr) 
		{
			if (!HandleAuthentication()) 
			{
				return;
			}
		}

		/*if (rtp_conn_ == nullptr)
		{
			return;
		}*/

		m_conn_state = START_PLAY;
		m_rtp_connection. Play();

		uint16_t session_id = m_rtp_connection.GetRtpSessionId();
		std::shared_ptr<char> res(new char[2048], std::default_delete<char[]>());

		int size = m_rtsp_request.BuildPlayRes(res.get(), 2048, nullptr, session_id);
		SendRtspMessage(res, size);
	}

	void crtsp_session::HandleCmdTeardown()
	{
		//if (rtp_conn_ == nullptr) 
		{
			//return;
		}

		m_rtp_connection.Teardown();

		uint16_t session_id = m_rtp_connection.GetRtpSessionId();
		std::shared_ptr<char> res(new char[2048], std::default_delete<char[]>());
		int size = m_rtsp_request.BuildTeardownRes(res.get(), 2048, session_id);
		SendRtspMessage(res, size);
	}

	void crtsp_session::HandleCmdGetParamter()
	{
		//if (rtp_conn_ == nullptr) {
			//return;
		//}

		uint16_t session_id = m_rtp_connection.GetRtpSessionId();
		std::shared_ptr<char> res(new char[2048], std::default_delete<char[]>());
		int size = m_rtsp_request.BuildGetParamterRes(res.get(), 2048, session_id);
		SendRtspMessage(res, size);
	}

	bool crtsp_session::HandleAuthentication()
	{
		if ( !m_has_auth)
		{
			std::string cmd = m_rtsp_request.MethodToString[m_rtsp_request.GetMethod()];
			std::string url = m_rtsp_request.GetRtspUrl();

			if (m_nonce.size() > 0 && (m_auth_info.GetResponse(m_nonce, cmd, url) == m_rtsp_request.GetAuthResponse())) 
			{
				m_nonce.clear();
				m_has_auth = true;
			}
			else 
			{
				std::shared_ptr<char> res(new char[4096], std::default_delete<char[]>());
				m_nonce = m_auth_info.GetNonce();
				int32 size = m_rtsp_request.BuildUnauthorizedRes(res.get(), 4096, m_auth_info.GetRealm().c_str(), m_nonce.c_str());
				SendRtspMessage(res, size);
				return false;
			}
		}

		return true;
	}

	void crtsp_session::handler_options(crtsp_request* request)
	{
		NORMAL_EX_LOG("");
		std::stringstream cmd;

		cmd << "RTSP/1.0 200 OK" << kCRLF;
		cmd << "CSeq: " << request->get_cseq() << kCRLF;
		cmd << crtsp_api::date_header();
		cmd << "Public: " << RTSP_ALLOWED_COMMAND <<  kCRLF << kCRLF;
		
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
		NORMAL_EX_LOG(" sdp_session = %s", sdp_session.str().c_str());
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

	void crtsp_session::SendRtspMessage(std::shared_ptr<char> buf, uint32_t size)
	{
#if RTSP_DEBUG
		NORMAL_EX_LOG("%s", buf.get() );
		//cout << buf.get() << endl;
#endif

		this->_send_msg((const uint8*)(buf.get()), size);
		return;
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