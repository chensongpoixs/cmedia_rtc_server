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

#ifndef _C_RTP_CONNECTION_H_
#define _C_RTP_CONNECTION_H_
#include "cnet_type.h"
#include <vector>
#include "cmedia_type.h"
#include "crtp.h"
#include <random>
//#include ""
namespace chen {
	class crtp_connection
	{
	public:
		explicit crtp_connection()
		{
			std::random_device rd;

			for (int chn = 0; chn < MAX_MEDIA_CHANNEL; chn++) {
				rtpfd_[chn] = 0;
				rtcpfd_[chn] = 0;
				memset(&media_channel_info_[chn], 0, sizeof(media_channel_info_[chn]));
				media_channel_info_[chn].rtp_header.version = RTP_VERSION;
				media_channel_info_[chn].packet_seq = rd() & 0xffff;
				media_channel_info_[chn].rtp_header.seq = 0; //htons(1);
				media_channel_info_[chn].rtp_header.ts = htonl(rd());
				media_channel_info_[chn].rtp_header.ssrc = htonl(rd());
			}

			//auto conn = rtsp_connection_.lock();
			//rtsp_ip_ = conn->GetIp();
			//rtsp_port_ = conn->GetPort();
		}
		virtual ~crtp_connection()
		{
			for (int chn = 0; chn < MAX_MEDIA_CHANNEL; chn++) {
				if (rtpfd_[chn] > 0) {
					//SocketUtil::Close(rtpfd_[chn]);
				}

				if (rtcpfd_[chn] > 0) {
					//SocketUtil::Close(rtcpfd_[chn]);
				}
			}
		}

	public:
		void SetClockRate(MediaChannelId channel_id, uint32_t clock_rate)
		{
			media_channel_info_[channel_id].clock_rate = clock_rate;
		}

		void SetPayloadType(MediaChannelId channel_id, uint32_t payload)
		{
			media_channel_info_[channel_id].rtp_header.payload = payload;
		}

		bool SetupRtpOverTcp(MediaChannelId channel_id, uint16_t rtp_channel, uint16_t rtcp_channel);
		bool SetupRtpOverUdp(MediaChannelId channel_id, uint16_t rtp_port, uint16_t rtcp_port);
		bool SetupRtpOverMulticast(MediaChannelId channel_id, std::string ip, uint16_t port);

		uint32_t GetRtpSessionId() const
		{
			return (uint32_t)((size_t)(this));
		}

		uint16_t GetRtpPort(MediaChannelId channel_id) const
		{
			return local_rtp_port_[channel_id];
		}

		uint16_t GetRtcpPort(MediaChannelId channel_id) const
		{
			return local_rtcp_port_[channel_id];
		}

		SOCKET GetRtpSocket(MediaChannelId channel_id) const
		{
			return rtpfd_[channel_id];
		}

		SOCKET GetRtcpSocket(MediaChannelId channel_id) const
		{
			return rtcpfd_[channel_id];
		}

		std::string GetIp()
		{
			return rtsp_ip_;
		}

		uint16_t GetPort()
		{
			return rtsp_port_;
		}

		bool IsMulticast() const
		{
			return is_multicast_;
		}

		bool IsSetup(MediaChannelId channel_id) const
		{
			return media_channel_info_[channel_id].is_setup;
		}

		std::string GetMulticastIp(MediaChannelId channel_id) const;

		void Play();
		void Record();
		void Teardown();

		std::string GetRtpInfo(const std::string& rtsp_url);
		int SendRtpPacket(MediaChannelId channel_id, RtpPacket pkt);

		bool IsClosed() const
		{
			return is_closed_;
		}

		int GetId() const;

		bool HasKeyFrame() const
		{
			return has_key_frame_;
		}
	private:
		void SetFrameType(uint8_t frameType = 0);
		void SetRtpHeader(MediaChannelId channel_id, RtpPacket pkt);
		int  SendRtpOverTcp(MediaChannelId channel_id, RtpPacket pkt);
		int  SendRtpOverUdp(MediaChannelId channel_id, RtpPacket pkt);
	protected:
	private:
		std::string rtsp_ip_;
		uint16_t rtsp_port_;

		TransportMode transport_mode_;
		bool is_multicast_ = false;

		bool is_closed_ = false;
		bool has_key_frame_ = false;

		uint8_t  frame_type_ = 0;
		uint16_t local_rtp_port_[MAX_MEDIA_CHANNEL];
		uint16_t local_rtcp_port_[MAX_MEDIA_CHANNEL];
		SOCKET rtpfd_[MAX_MEDIA_CHANNEL];
		SOCKET rtcpfd_[MAX_MEDIA_CHANNEL];

		struct sockaddr_in peer_addr_;
		struct sockaddr_in peer_rtp_addr_[MAX_MEDIA_CHANNEL];
		struct sockaddr_in peer_rtcp_sddr_[MAX_MEDIA_CHANNEL];
		MediaChannelInfo media_channel_info_[MAX_MEDIA_CHANNEL];
	};
}

#endif //