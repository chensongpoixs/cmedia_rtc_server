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


#include "crtp_connection.h"
#include "csocket_util.h"
namespace chen {

 
	bool  crtp_connection::SetupRtpOverTcp(MediaChannelId channel_id, uint16_t rtp_channel, uint16_t rtcp_channel)
	{
		//auto conn = rtsp_connection_.lock();
		//if (!conn) {
		//	return false;
		//}

		media_channel_info_[channel_id].rtp_channel = rtp_channel;
		media_channel_info_[channel_id].rtcp_channel = rtcp_channel;
		//rtpfd_[channel_id] = conn->GetSocket();
		//rtcpfd_[channel_id] = conn->GetSocket();
		media_channel_info_[channel_id].is_setup = true;
		transport_mode_ = RTP_OVER_TCP;

		return true;
	}
	bool crtp_connection::SetupRtpOverUdp(MediaChannelId channel_id,const  struct sockaddr_in *addr, uint16_t rtp_port, uint16_t rtcp_port)
	{
		//auto conn = rtsp_connection_.lock();
		//if (!conn) {
		//	return false;
		//}
		//
		//if (SocketUtil::GetPeerAddr(conn->GetSocket(), &peer_addr_) < 0) {
		//	return false;
		//}
		if (addr)
		{
			memcpy(&peer_addr_, addr, sizeof(*addr));
		}
		media_channel_info_[channel_id].rtp_port = rtp_port;
		media_channel_info_[channel_id].rtcp_port = rtcp_port;

		std::random_device rd;
		for (int n = 0; n <= 10; n++) {
			if (n == 10) {
				return false;
			}

			local_rtp_port_[channel_id] = rd() & 0xfffe;
			local_rtcp_port_[channel_id] = local_rtp_port_[channel_id] + 1;

			rtpfd_[channel_id] = ::socket(AF_INET, SOCK_DGRAM, 0);
			if (!socket_util::bind(rtpfd_[channel_id], "0.0.0.0", local_rtp_port_[channel_id]))
			{
				socket_util::close(rtpfd_[channel_id]);
				continue;
			}

			rtcpfd_[channel_id] = ::socket(AF_INET, SOCK_DGRAM, 0);
			if (!socket_util::bind(rtcpfd_[channel_id], "0.0.0.0", local_rtcp_port_[channel_id])) 
			{
				socket_util::close(rtpfd_[channel_id]);
				socket_util::close(rtcpfd_[channel_id]);
				continue;
			}

			break;
		}

		socket_util::setsendbufsize(rtpfd_[channel_id], 50 * 1024);

		peer_rtp_addr_[channel_id].sin_family = AF_INET;
		peer_rtp_addr_[channel_id].sin_addr.s_addr = peer_addr_.sin_addr.s_addr;
		peer_rtp_addr_[channel_id].sin_port = htons(media_channel_info_[channel_id].rtp_port);

		peer_rtcp_sddr_[channel_id].sin_family = AF_INET;
		peer_rtcp_sddr_[channel_id].sin_addr.s_addr = peer_addr_.sin_addr.s_addr;
		peer_rtcp_sddr_[channel_id].sin_port = htons(media_channel_info_[channel_id].rtcp_port);

		media_channel_info_[channel_id].is_setup = true;
		transport_mode_ = RTP_OVER_UDP;

		return true;
	}
	bool crtp_connection::SetupRtpOverMulticast(MediaChannelId channel_id, std::string ip, uint16_t port)
	{

		std::random_device rd;
		for (int n = 0; n <= 10; n++) {
			if (n == 10) {
				return false;
			}

			local_rtp_port_[channel_id] = rd() & 0xfffe;
			rtpfd_[channel_id] = ::socket(AF_INET, SOCK_DGRAM, 0);
			if (!socket_util::bind(rtpfd_[channel_id], "0.0.0.0", local_rtp_port_[channel_id])) 
			{
				socket_util::close(rtpfd_[channel_id]);
				continue;
			}

			break;
		}

		media_channel_info_[channel_id].rtp_port = port;

		peer_rtp_addr_[channel_id].sin_family = AF_INET;
		peer_rtp_addr_[channel_id].sin_addr.s_addr = inet_addr(ip.c_str());
		peer_rtp_addr_[channel_id].sin_port = htons(port);

		media_channel_info_[channel_id].is_setup = true;
		transport_mode_ = RTP_OVER_MULTICAST;
		is_multicast_ = true;
		return true;
	}
	std::string crtp_connection::GetMulticastIp(MediaChannelId channel_id) const
	{
		return std::string(::inet_ntoa(peer_rtp_addr_[channel_id].sin_addr));
	}
	void crtp_connection::Play()
	{
		for (int chn = 0; chn < MAX_MEDIA_CHANNEL; chn++)
		{
			if (media_channel_info_[chn].is_setup) {
				media_channel_info_[chn].is_play = true;
			}
		}
	}
	void crtp_connection::Record()
	{
		for (int chn = 0; chn < MAX_MEDIA_CHANNEL; chn++) {
			if (media_channel_info_[chn].is_setup) {
				media_channel_info_[chn].is_record = true;
				media_channel_info_[chn].is_play = true;
			}
		}
	}
	void crtp_connection::Teardown()
	{
		if (!is_closed_) {
			is_closed_ = true;
			for (int chn = 0; chn < MAX_MEDIA_CHANNEL; chn++) {
				media_channel_info_[chn].is_play = false;
				media_channel_info_[chn].is_record = false;
			}
		}
	}
	std::string crtp_connection::GetRtpInfo(const std::string & rtsp_url)
	{
		char buf[2048] = { 0 };
		snprintf(buf, 1024, "RTP-Info: ");

		int num_channel = 0;

		auto time_point = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
		auto ts = time_point.time_since_epoch().count();
		for (int chn = 0; chn < MAX_MEDIA_CHANNEL; chn++) {
			uint32_t rtpTime = (uint32_t)(ts*media_channel_info_[chn].clock_rate / 1000);
			if (media_channel_info_[chn].is_setup) {
				if (num_channel != 0) {
					snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), ",");
				}

				snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
					"url=%s/track%d;seq=0;rtptime=%u",
					rtsp_url.c_str(), chn, rtpTime);
				num_channel++;
			}
		}

		return std::string(buf);
	}
	int crtp_connection::SendRtpPacket(MediaChannelId channel_id, RtpPacket pkt)
	{
		if (is_closed_) {
			return -1;
		}

		//auto conn = rtsp_connection_.lock();
		//if (!conn) {
		//	return -1;
		//}
		//
		//RtspConnection *rtsp_conn = (RtspConnection *)conn.get();

		bool ret = true;// rtsp_conn->task_scheduler_->AddTriggerEvent([this, channel_id, pkt]
		{
			this->SetFrameType(pkt.type);
			this->SetRtpHeader(channel_id, pkt);
			if ((media_channel_info_[channel_id].is_play || media_channel_info_[channel_id].is_record) && has_key_frame_) {
				if (transport_mode_ == RTP_OVER_TCP) {
					SendRtpOverTcp(channel_id, pkt);
				}
				else {
					SendRtpOverUdp(channel_id, pkt);
				}

				media_channel_info_[channel_id].octet_count  += pkt.size;
				media_channel_info_[channel_id].packet_count += 1;
			}
		}
		//);

		return ret ? 0 : -1;
	}
	int crtp_connection::GetId() const
	{
		//auto conn = rtsp_connection_.lock();
		//if (!conn) {
		//	return -1;
		//}
		//RtspConnection *rtspConn = (RtspConnection *)conn.get();
		return 1;// rtspConn->GetId();
	}
	void crtp_connection::SetFrameType(uint8_t frame_type)
	{
		frame_type_ = frame_type;
		if (!has_key_frame_ && (frame_type == 0 || frame_type == VIDEO_FRAME_I)) {
			has_key_frame_ = true;
		}
	}
	void crtp_connection::SetRtpHeader(MediaChannelId channel_id, RtpPacket pkt)
	{
		if ((media_channel_info_[channel_id].is_play || media_channel_info_[channel_id].is_record) && has_key_frame_) {
			media_channel_info_[channel_id].rtp_header.marker = pkt.last;
			media_channel_info_[channel_id].rtp_header.ts = htonl(pkt.timestamp);
			media_channel_info_[channel_id].rtp_header.seq = htons(media_channel_info_[channel_id].packet_seq++);
			memcpy(pkt.data.get() + 4, &media_channel_info_[channel_id].rtp_header, RTP_HEADER_SIZE);
		}
	}
	int crtp_connection::SendRtpOverTcp(MediaChannelId channel_id, RtpPacket pkt)
	{
		//auto conn = rtsp_connection_.lock();
		//if (!conn) {
		//	return -1;
		//}

		uint8_t* rtpPktPtr = pkt.data.get();
		rtpPktPtr[0] = '$';
		rtpPktPtr[1] = (char)media_channel_info_[channel_id].rtp_channel;
		rtpPktPtr[2] = (char)(((pkt.size - 4) & 0xFF00) >> 8);
		rtpPktPtr[3] = (char)((pkt.size - 4) & 0xFF);

		//conn->Send((char*)rtpPktPtr, pkt.size);
		return pkt.size;
	}
	int crtp_connection::SendRtpOverUdp(MediaChannelId channel_id, RtpPacket pkt)
	{
		int ret = sendto(rtpfd_[channel_id], (const char*)pkt.data.get() + 4, pkt.size - 4, 0,
			(struct sockaddr *)&(peer_rtp_addr_[channel_id]), sizeof(struct sockaddr_in));

		if (ret < 0) {
			Teardown();
			return -1;
		}

		return ret;
	}
}
