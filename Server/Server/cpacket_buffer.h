/***********************************************************************************************
created: 		2023-07-23

author:			chensong

purpose:		vmc packet

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



#ifndef _C_PACKET_BUFFER_H_
#define _C_PACKET_BUFFER_H_
#include "cnetwork.h"
#include "cnet_type.h"
#include "crtp_video_header.h"
#include "cvcm_encoded_frame.h"
#include "cvcm_packet.h"
namespace chen {

	class cpacket_buffer
	{
	private:
		// Since we want the packet buffer to be as packet type agnostic
		// as possible we extract only the information needed in order
		// to determine whether a sequence of packets is continuous or not.
		struct ContinuityInfo {
			// The sequence number of the packet.
			uint16 seq_num = 0;

			// If this is the first packet of the frame.
			bool frame_begin = false;

			// If this is the last packet of the frame.
			bool frame_end = false;

			// If this slot is currently used.
			bool used = false;

			// If all its previous packets have been inserted into the packet buffer.
			bool continuous = false;

			// If this packet has been used to create a frame already.
			bool frame_created = false;
		};
	public:
		explicit cpacket_buffer() 
		: m_size(0)
		, m_max_size(2048)
		, m_first_seq_num(0)
		, m_first_packet_received(false)
		, m_is_cleared_to_first_seq_num(false)
		, m_data_buffer()
		, m_sequence_buffer(){}
			virtual ~cpacket_buffer();

	public:
		bool init(uint32 start_buffer_size, uint32 max_buffer_size  = 2048 );
		void destroy();

	public:

		bool insert_packet(cvcm_packet * packet);


	private:
		bool _expand_buffer_size();

		std::vector<cvcm_encoded_frame> _find_frames(uint16 seq_num);

		bool  _potential_new_frame(uint16 seq_num) const;


		bool  _get_bitstream(uint16 first_seq_num, uint16 last_seq_num, uint32 timestamp, cvcm_encoded_frame & vcm_frame);
	
		void  _return_frame(uint16 first_seq_num, uint16 last_seq_num, uint32 timestamp);
	protected:
	private:

		uint32		 m_size;
		uint32       m_max_size; 

		uint16       m_first_seq_num;

		bool         m_first_packet_received;

		bool		 m_is_cleared_to_first_seq_num;


		std::vector<cvcm_packet>  m_data_buffer;

		std::vector<ContinuityInfo> m_sequence_buffer;

	};


}

#endif // 