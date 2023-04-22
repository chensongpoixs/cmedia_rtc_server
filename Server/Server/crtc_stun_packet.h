/***********************************************************************************************
created: 		2023-02-01

author:			chensong

purpose:		 rtc——stun packet 
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


#ifndef _C_RTC_STUN_PACKET_H_
#define _C_RTC_STUN_PACKET_H_
#include "cnet_type.h"
#include "crtc_stun_define.h"
#include "cbuffer.h"


namespace chen {

	class crtc_stun_packet
	{
	public:
		explicit crtc_stun_packet()
		: m_message_type(0)
		, m_username("")
		, m_password("")
		, m_local_ufrag("")
		, m_remote_ufrag("")
		, m_transcation_id("")
		, m_mapped_address(0)
		, m_mapped_port(0)
		, m_use_candidate(false)
		, m_ice_controlled(false)
		, m_ice_controlling(false)
		{}
		virtual ~crtc_stun_packet(); 

	public:
		static bool is_stun(const uint8* data, size_t len)
		{
			// clang-format off
			return (
				// STUN headers are 20 bytes.
				(len >= 20) &&
				// DOC: https://tools.ietf.org/html/draft-ietf-avtcore-rfc5764-mux-fixes
				(data[0] < 3) &&
				// Magic cookie must match.
				(data[4] == kStunMagicCookie[0]) && (data[5] == kStunMagicCookie[1]) &&
				(data[6] == kStunMagicCookie[2]) && (data[7] == kStunMagicCookie[3])
				);
			// clang-format on
		}
	public:
		bool is_binding_request() const 
		{
			return m_message_type == EBindingRequest;
		};
		bool is_binding_response() const 
		{
			return m_message_type == EBindingResponse;
		};
		uint16  get_message_type() const { return m_message_type; };
		std::string get_username() const { return m_username; };
		std::string get_local_ufrag() const { return m_local_ufrag; };
		std::string get_remote_ufrag() const { return m_remote_ufrag; };
		std::string get_transcation_id() const { return m_transcation_id;  };
		uint32  get_mapped_address() const { return m_mapped_address; };
		uint16  get_mapped_port() const { return m_mapped_port; };
		bool get_ice_controlled() const { return m_ice_controlled; };
		bool get_ice_controlling() const { return m_ice_controlling; };
		bool get_use_candidate() const { return m_use_candidate; };
		void set_message_type(const uint16& m) { m_message_type = m; };
		void set_local_ufrag(const std::string& u) { m_local_ufrag = u; };
		void set_remote_ufrag(const std::string& u) { m_remote_ufrag = u; };
		void set_transcation_id(const std::string& t) { m_transcation_id = t; };
		void set_mapped_address(const uint32 & addr) { m_mapped_address = addr; };
		void set_mapped_port(const uint32 & port) { m_mapped_port = port; };
		int32 decode(const char* buf, const int32 nb_buf);
		int32 encode(const std::string& pwd,   cbuffer* buffer);
	private:
		int32 encode_binding_response(const std::string& pwd,  cbuffer* buffer);
		std::string encode_username();
		std::string encode_mapped_address();
		std::string encode_hmac(char* hamc_buf, const int32 hmac_buf_len);
		std::string encode_fingerprint(uint32  crc32);
	protected:
	private:


	private:
		//// STUN attributes.
		uint16					m_message_type;		// 2 bytes.
		std::string				m_username;			// Less than 513 bytes.
		std::string				m_password;
		std::string				m_local_ufrag;
		std::string				m_remote_ufrag;
		std::string				m_transcation_id;   // 12 bytes.
		uint32					m_mapped_address;   // /*8 or 20 bytes.
		uint16					m_mapped_port;      //                 */
		bool					m_use_candidate;    // 0 bytes.
		bool					m_ice_controlled;   // 8 bytes unsigned integer.
		bool					m_ice_controlling;  // 8 bytes unsigned integer.
	};

}

#endif // _C_RTC_STUN_PACKET_H_