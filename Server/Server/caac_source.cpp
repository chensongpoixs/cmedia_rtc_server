/***********************************************************************************************
created: 		2023-07-16

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
#include "caac_source.h"
namespace chen {
	static uint32_t AACSampleRate[16] =
	{
		96000, 88200, 64000, 48000,
		44100, 32000, 24000, 22050,
		16000, 12000, 11025, 8000,
		7350, 0, 0, 0 /*reserved */
	};
	caac_source * caac_source::construct()
	{
		return new caac_source();
	}
	void caac_source::destroy(caac_source * source)
	{
		delete source;
	}

	void caac_source::destroy()
	{

	}

	std::string caac_source::get_media_description(uint16_t port)
	{
		char buf[100] = { 0 };
		sprintf(buf, "m=audio %hu RTP/AVP 97", port); // \r\nb=AS:64
		return std::string(buf);
	}

	std::string caac_source::get_attribute()
	{
		char buf[500] = { 0 };
		sprintf(buf, "a=rtpmap:97 MPEG4-GENERIC/%u/%u\r\n", m_samplerate, m_channels);

		uint8_t index = 0;
		for (index = 0; index < 16; index++) {
			if (AACSampleRate[index] == m_samplerate) {
				break;
			}
		}

		if (index == 16) {
			return ""; // error
		}

		uint8_t profile = 1;
		char config[10] = { 0 };

		sprintf(config, "%02x%02x", (uint8_t)((profile + 1) << 3) | (index >> 1), (uint8_t)((index << 7) | (m_channels << 3)));
		sprintf(buf + strlen(buf),
			"a=fmtp:97 profile-level-id=1;"
			"mode=AAC-hbr;"
			"sizelength=13;indexlength=3;indexdeltalength=3;"
			"config=%04u",
			atoi(config));

		return std::string(buf);
	}

	bool caac_source::handler_frame(MediaChannelId channel_id, AVFrame frame)
	{
		if (frame.size > (MAX_RTP_PAYLOAD_SIZE - AU_SIZE)) {
			return false;
		}

		int adts_size = 0;
		if (m_has_adts) {
			adts_size = ADTS_SIZE;
		}

		uint8_t *frame_buf = frame.buffer.get() + adts_size;
		uint32_t frame_size = frame.size - adts_size;

		char AU[AU_SIZE] = { 0 };
		AU[0] = 0x00;
		AU[1] = 0x10;
		AU[2] = (frame_size & 0x1fe0) >> 5;
		AU[3] = (frame_size & 0x1f) << 3;

		RtpPacket rtp_pkt;
		rtp_pkt.type = frame.type;
		rtp_pkt.timestamp = frame.timestamp;
		rtp_pkt.size = frame_size + RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE + AU_SIZE;
		rtp_pkt.last = 1;

		rtp_pkt.data.get()[RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE + 0] = AU[0];
		rtp_pkt.data.get()[RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE + 1] = AU[1];
		rtp_pkt.data.get()[RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE + 2] = AU[2];
		rtp_pkt.data.get()[RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE + 3] = AU[3];

		memcpy(rtp_pkt.data.get() + RTP_TCP_HEAD_SIZE + RTP_HEADER_SIZE + AU_SIZE, frame_buf, frame_size);

		if (m_send_frame_callback_ptr) {
			m_send_frame_callback_ptr(channel_id, rtp_pkt);
		}

		return true;
	}

	uint32_t caac_source::get_timestamp(uint32_t samplerate /*= 44100*/)
	{
		//auto time_point = chrono::time_point_cast<chrono::milliseconds>(chrono::high_resolution_clock::now());
		//return (uint32_t)(time_point.time_since_epoch().count() * sampleRate / 1000);

		auto time_point = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now());
		return (uint32_t)((time_point.time_since_epoch().count() + 500) / 1000 * samplerate / 1000);
	}

	bool caac_source::init(uint32 samplerate, uint32 channels, bool has_adts)
	{
		m_samplerate = samplerate;
		m_channels = channels;
		m_has_adts = has_adts;

		m_payload = 97;
		m_media_type = AAC;
		m_clock_rate = samplerate;
		return true;
	}
}