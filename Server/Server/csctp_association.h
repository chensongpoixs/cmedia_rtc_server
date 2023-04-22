/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		csctp_association
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

#ifndef _C_SCTP_ASSOCIATION_H_
#define _C_SCTP_ASSOCIATION_H_
#include <string>
#include "cnet_type.h"
#include <map>
#include <usrsctp.h>
#include "cwebrtc_transport.h"
#include <json/json.h>
namespace chen
{

	enum  ESCTPSTATE
	{
		ESCTP_NEW = 1,
		ESCTP_CONNECTING,
		ESCTP_CONNECTED,
		ESCTP_FAILED,
		ESCTP_CLOSED
	};
	enum   EStreamDirection
	{
		ESTREAM_INCOMING = 1,
		ESTREAM_OUTGOING
	};
	class csctp_association
	{
	public:
		class Listener
		{
		public:
			virtual ~Listener() = default;

		public:
			virtual void OnSctpAssociationConnecting(csctp_association* sctpAssociation) = 0;
			virtual void OnSctpAssociationConnected(csctp_association* sctpAssociation) = 0;
			virtual void OnSctpAssociationFailed(csctp_association* sctpAssociation) = 0;
			virtual void OnSctpAssociationClosed(csctp_association* sctpAssociation) = 0;
			virtual void OnSctpAssociationSendData(
				csctp_association* sctpAssociation, const uint8_t* data, size_t len) = 0;
			virtual void OnSctpAssociationMessageReceived(
				csctp_association* sctpAssociation,
				uint16_t streamId,
				uint32_t ppid,
				const uint8_t* msg,
				size_t len) = 0;
			virtual void OnSctpAssociationBufferedAmount(
				csctp_association* sctpAssociation, uint32_t len) = 0;
		protected:
		private:
		};
	public:
		csctp_association(csctp_association::Listener * transport, uint16_t os, uint16_t mis, size_t max_sctp_message_size, size_t sctp_send_buffer_size);
			
		~csctp_association();



	public:
		static bool issctp(const uint8_t * data, size_t len);
		uintptr_t get_id() { return m_id; }
		
	public:
		void TransportConnected();
		ESCTPSTATE get_state() const { return m_state; }
		size_t get_sctp_buffered_amount() const { return m_sctp_buffered_amount; }

		void ProcessSctpData(const uint8_t* data, size_t len);
		void SendSctpMessage(uint16_t streamId, uint32_t ppid, const uint8_t* msg, size_t len );
	public:
		/// Json
		void reply(Json::Value& value);
	private:
		void ResetSctpStream(uint16_t streamId, EStreamDirection direction);
		void AddOutgoingStreams(bool force = false);

	public:

		/* Callbacks fired by usrsctp events. */
		void OnUsrSctpSendSctpData(void* buffer, size_t len);
		void OnUsrSctpReceiveSctpData(
			uint16_t streamId, uint16_t ssn, uint32_t ppid, int flags, const uint8_t* data, size_t len);
		void OnUsrSctpReceiveSctpNotification(union sctp_notification* notification, size_t len);
		void OnUsrSctpSentData(uint32_t freeBuffer);
	private:
		csctp_association::Listener *			m_transport_ptr;
		uintptr_t					m_id;// { 0u };
		uint16_t					m_os;  // default [1024u]
		uint16_t					m_mis; // default [1024u]
		size_t						m_max_sctp_message_size; // default [262144u]
		size_t						m_sctp_send_buffer_size; // default [262144u]
		size_t						m_sctp_buffered_amount; // default [0u]
		// Allocated by this
		uint8_t	*					m_message_buffer;
		ESCTPSTATE					m_state; // default [ESCTP_NEW]
		struct socket*				m_socket;
		uint16_t					m_desiredos; // default [0u]
		size_t						m_message_buffer_len;
		uint16_t					m_last_ssn_received; // Valid for us since no SCTP I-DATA support.
	};
}

#endif // _C_SCTP_ASSOCIATION_H_