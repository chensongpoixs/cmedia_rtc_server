/***********************************************************************************************
created: 		2022-08-12

author:			chensong

purpose:		cudp_socket_handler
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
#include "ctcp_conection.h"
#include "clog.h"
#include "ctcp_connection_handler.h"
#include "crtc_util.h"
namespace chen {

	/* Static. */

	static constexpr size_t ReadBufferSize{ 65536 };
	thread_local static uint8_t ReadBuffer[ReadBufferSize];

	/* Instance methods. */

	ctcp_connection::ctcp_connection(Listener* listener, size_t bufferSize)
		: ctcp_connection_handler::ctcp_connection_handler(bufferSize), listener(listener)
	{
		//MS_TRACE();
	}

	ctcp_connection::~ctcp_connection()
	{
		//MS_TRACE();
	}

	void ctcp_connection::UserOnTcpConnectionRead()
	{
		//MS_TRACE();

		/*NORMAL_EX_LOG(
			"data received [local:%s :%" PRIu16 ", remote:%s :%" PRIu16 "]",
			GetLocalIp().c_str(),
			GetLocalPort(),
			GetPeerIp().c_str(),
			GetPeerPort());*/

		/*
		 * Framing RFC 4571
		 *
		 *     0                   1                   2                   3
		 *     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		 *     ---------------------------------------------------------------
		 *     |             LENGTH            |  STUN / DTLS / RTP / RTCP   |
		 *     ---------------------------------------------------------------
		 *
		 * A 16-bit unsigned integer LENGTH field, coded in network byte order
		 * (big-endian), begins the frame.  If LENGTH is non-zero, an RTP or
		 * RTCP packet follows the LENGTH field.  The value coded in the LENGTH
		 * field MUST equal the number of octets in the RTP or RTCP packet.
		 * Zero is a valid value for LENGTH, and it codes the null packet.
		 */

		 // Be ready to parse more than a single frame in a single TCP chunk.
		while (true)
		{
			// We may receive multiple packets in the same TCP chunk. If one of them is
			// a DTLS Close Alert this would be closed (Close() called) so we cannot call
			// our listeners anymore.
			if (IsClosed())
				return;
			
			size_t dataLen = this->bufferDataLen - this->frameStart;
			//NORMAL_EX_LOG("[buffer = %s][datalen  = %u]", buffer, dataLen);
			size_t packetLen;

			 if (dataLen >= 2)
			 	packetLen = size_t{ rtc_byte::get2bytes(this->buffer + this->frameStart, 0) };

			// We have packetLen bytes.
			if ( dataLen >= 2 && dataLen >= 2 + packetLen)
			{
				const uint8_t* packet = this->buffer + this->frameStart  +2   ;

				// Update received bytes and notify the listener.
				if (packetLen != 0)
				{
					// Copy the received packet into the static buffer so it can be expanded
					// later.
					std::memcpy(ReadBuffer, packet, packetLen);

					this->listener->OnTcpConnectionPacketReceived(this, ReadBuffer, packetLen);
				}

				// If there is no more space available in the buffer and that is because
				// the latest parsed frame filled it, then empty the full buffer.
				if ((this->frameStart + 2 + packetLen) == this->bufferSize)
				{
					//NORMAL_EX_LOG("no more space in the buffer, emptying the buffer data");

					this->frameStart = 0;
					this->bufferDataLen = 0;
				}
				// If there is still space in the buffer, set the beginning of the next
				// frame to the next position after the parsed frame.
				else
				{
					this->frameStart += 2 + packetLen;
				}

				// If there is more data in the buffer after the parsed frame then
				// parse again. Otherwise break here and wait for more data.
				if (this->bufferDataLen > this->frameStart)
				{
					//NORMAL_EX_LOG("there is more data after the parsed frame, continue parsing");

					continue;
				}

				break;
			}

			// Incomplete packet.

			// Check if the buffer is full.
			if (this->bufferDataLen == this->bufferSize)
			{
				// First case: the incomplete frame does not begin at position 0 of
				// the buffer, so move the frame to the position 0.
				if (this->frameStart != 0)
				{
					//NORMAL_EX_LOG(
						//"no more space in the buffer, moving parsed bytes to the beginning of "
						//"the buffer and wait for more data");

					std::memmove(
						this->buffer, this->buffer + this->frameStart, this->bufferSize - this->frameStart);
					this->bufferDataLen = this->bufferSize - this->frameStart;
					this->frameStart = 0;
				}
				// Second case: the incomplete frame begins at position 0 of the buffer.
				// The frame is too big.
				else
				{
					WARNING_EX_LOG(
						"no more space in the buffer for the unfinished frame being parsed, closing the "
						"connection");

					ErrorReceiving();

					// And exit fast since we are supposed to be deallocated.
					return;
				}
			}
			// The buffer is not full.
			else
			{
				NORMAL_EX_LOG("frame not finished yet, waiting for more data");
			}

			// Exit the parsing loop.
			break;
		}
	}

	void ctcp_connection::Send(const uint8_t* data, size_t len, ctcp_connection_handler::onSendCallback* cb)
	{
		//MS_TRACE();

		// Write according to Framing RFC 4571.

		uint8_t frameLen[2];
		rtc_byte::set2bytes(frameLen, 0, len);
		ctcp_connection_handler::Write(frameLen, 2, data, len, cb);
	}
}