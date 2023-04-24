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
#include "csctp_association.h"
#include "crtc_util.h"
#include "csctp_association_mgr.h"
#include "clog.h"
namespace chen {



	// Free send buffer threshold (in bytes) upon which send_cb will be executed.
	static uint32_t SendBufferThreshold{ 256u };
	/* Static. */

	static const   size_t SctpMtu = 1200 ;
	static const   uint16_t MaxSctpStreams = 65535 ;
	/* SCTP events to which we are subscribing. */

	/* clang-format off */
	uint16_t EventTypes[] =
	{
		SCTP_ADAPTATION_INDICATION,
		SCTP_ASSOC_CHANGE,
		SCTP_ASSOC_RESET_EVENT,
		SCTP_REMOTE_ERROR,
		SCTP_SHUTDOWN_EVENT,
		SCTP_SEND_FAILED_EVENT,
		SCTP_STREAM_RESET_EVENT,
		SCTP_STREAM_CHANGE_EVENT
	};
	/* clang-format on */

	/* Static methods for usrsctp callbacks. */

	inline static int onRecvSctpData(
		struct socket* /*sock*/,
		union sctp_sockstore /*addr*/,
		void* data,
		size_t len,
		struct sctp_rcvinfo rcv,
		int flags,
		void* ulpInfo)
	{
		csctp_association* sctpAssociation = g_sctp_association_mgr. RetrieveSctpAssociation(reinterpret_cast<uintptr_t>(ulpInfo));

		if (!sctpAssociation)
		{
			WARNING_EX_LOG("sctp, no SctpAssociation found");

			std::free(data);

			return 0;
		}

		if (flags & MSG_NOTIFICATION)
		{
			sctpAssociation->OnUsrSctpReceiveSctpNotification( static_cast<union sctp_notification*>(data), len);
		}
		else
		{
			uint16_t streamId = rcv.rcv_sid;
			uint32_t ppid = ntohl(rcv.rcv_ppid);
			uint16_t ssn = rcv.rcv_ssn;

			//DEBUG_EX_LOG("sctp, data chunk received [length:%zu, streamId:%hu, SSN:%hu, TSN:%u, PPID:%u, context:%u, flags:%d]",
			/*	len,
				rcv.rcv_sid,
				rcv.rcv_ssn,
				rcv.rcv_tsn,
				ntohl(rcv.rcv_ppid),
				rcv.rcv_context,
				flags); */

			sctpAssociation->OnUsrSctpReceiveSctpData( streamId, ssn, ppid, flags, static_cast<uint8_t*>(data), len);
		}

		std::free(data);

		return 1;
	}

	inline static int onSendSctpData(struct socket* /*sock*/, uint32_t freeBuffer, void* ulpInfo)
	{
		csctp_association* sctpAssociation = g_sctp_association_mgr. RetrieveSctpAssociation(reinterpret_cast<uintptr_t>(ulpInfo));

		if (!sctpAssociation)
		{
			WARNING_EX_LOG("sctp, no SctpAssociation found");

			return 0;
		}

		sctpAssociation->OnUsrSctpSentData(freeBuffer);

		return 1;
	}

	csctp_association::csctp_association(csctp_association::Listener * transport, uint16_t os, uint16_t mis, size_t max_sctp_message_size, size_t sctp_send_buffer_size)
		: m_transport_ptr(transport)
		, m_id(0u)
		, m_os(os)
		, m_mis(mis)
		, m_max_sctp_message_size(max_sctp_message_size)
		, m_sctp_send_buffer_size(sctp_send_buffer_size)
		, m_sctp_buffered_amount(0u)
		, m_message_buffer(NULL)
		, m_state(ESCTP_NEW)
		, m_socket(NULL)
		, m_desiredos(0u)
		, m_message_buffer_len(0u)
		, m_last_ssn_received(0u)
	{
		// Get a id for this SctpAssociation.
		//m_id = DepUsrSCTP::GetNextSctpAssociationId();
		m_id = g_sctp_association_mgr.GetNextSctpAssociationId();

		// Register ourselves in usrsctp.
		// NOTE: This must be done before calling usrsctp_bind().
		usrsctp_register_address(reinterpret_cast<void*>(m_id));

		int ret;

		m_socket = usrsctp_socket( AF_CONN, SOCK_STREAM, IPPROTO_SCTP,
			onRecvSctpData, onSendSctpData, SendBufferThreshold, reinterpret_cast<void*>(m_id));

		if (!m_socket)
		{
			usrsctp_deregister_address(reinterpret_cast<void*>(m_id));

			ERROR_EX_LOG("usrsctp_socket() failed: %s", ::strerror(errno));
		}

		usrsctp_set_ulpinfo(m_socket, reinterpret_cast<void*>(m_id));

		// Make the socket non-blocking.
		ret = usrsctp_set_non_blocking(m_socket, 1);

		if (ret < 0)
		{
			usrsctp_deregister_address(reinterpret_cast<void*>(m_id));

			ERROR_EX_LOG("usrsctp_set_non_blocking() failed: %s", std::strerror(errno));
		}

		// Set SO_LINGER.
		// This ensures that the usrsctp close call deletes the association. This
		// prevents usrsctp from calling the global send callback with references to
		// this class as the address.
		struct linger lingerOpt; // NOLINT(cppcoreguidelines-pro-type-member-init)

		lingerOpt.l_onoff = 1;
		lingerOpt.l_linger = 0;

		ret = usrsctp_setsockopt(m_socket, SOL_SOCKET, SO_LINGER, &lingerOpt, sizeof(lingerOpt));

		if (ret < 0)
		{
			usrsctp_deregister_address(reinterpret_cast<void*>(m_id));

			ERROR_EX_LOG("usrsctp_setsockopt(SO_LINGER) failed: %s", std::strerror(errno));
		}

		// Set SCTP_ENABLE_STREAM_RESET.
		struct sctp_assoc_value av; // NOLINT(cppcoreguidelines-pro-type-member-init)

		av.assoc_value = SCTP_ENABLE_RESET_STREAM_REQ | SCTP_ENABLE_RESET_ASSOC_REQ | SCTP_ENABLE_CHANGE_ASSOC_REQ;

		ret = usrsctp_setsockopt(m_socket, IPPROTO_SCTP, SCTP_ENABLE_STREAM_RESET, &av, sizeof(av));

		if (ret < 0)
		{
			usrsctp_deregister_address(reinterpret_cast<void*>(m_id));

			ERROR_EX_LOG("usrsctp_setsockopt(SCTP_ENABLE_STREAM_RESET) failed: %s", std::strerror(errno));
		}

		// Set SCTP_NODELAY.
		uint32_t noDelay = 1;

		ret = usrsctp_setsockopt(m_socket, IPPROTO_SCTP, SCTP_NODELAY, &noDelay, sizeof(noDelay));

		if (ret < 0)
		{
			usrsctp_deregister_address(reinterpret_cast<void*>(m_id));

			ERROR_EX_LOG("usrsctp_setsockopt(SCTP_NODELAY) failed: %s", std::strerror(errno));
		}

		// Enable events.
		struct sctp_event event; // NOLINT(cppcoreguidelines-pro-type-member-init)

		std::memset(&event, 0, sizeof(event));
		event.se_on = 1;

		for (size_t i{ 0 }; i < sizeof(EventTypes) / sizeof(uint16_t); ++i)
		{
			event.se_type = EventTypes[i];

			ret = usrsctp_setsockopt(m_socket, IPPROTO_SCTP, SCTP_EVENT, &event, sizeof(event));

			if (ret < 0)
			{
				usrsctp_deregister_address(reinterpret_cast<void*>(m_id));

				ERROR_EX_LOG("usrsctp_setsockopt(SCTP_EVENT) failed: %s", std::strerror(errno));
			}
		}

		// Init message.
		struct sctp_initmsg initmsg; // NOLINT(cppcoreguidelines-pro-type-member-init)

		std::memset(&initmsg, 0, sizeof(initmsg));
		initmsg.sinit_num_ostreams = m_os;
		initmsg.sinit_max_instreams = m_mis;

		ret = usrsctp_setsockopt(m_socket, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));

		if (ret < 0)
		{
			usrsctp_deregister_address(reinterpret_cast<void*>(m_id));

			ERROR_EX_LOG("usrsctp_setsockopt(SCTP_INITMSG) failed: %s", std::strerror(errno));
		}

		// Server side.
		struct sockaddr_conn sconn; // NOLINT(cppcoreguidelines-pro-type-member-init)

		std::memset(&sconn, 0, sizeof(sconn));
		sconn.sconn_family = AF_CONN;
		sconn.sconn_port = htons(5000);
		sconn.sconn_addr = reinterpret_cast<void*>(m_id);
#ifdef HAVE_SCONN_LEN
		sconn.sconn_len = sizeof(sconn);
#endif

		ret = usrsctp_bind(m_socket, reinterpret_cast<struct sockaddr*>(&sconn), sizeof(sconn));

		if (ret < 0)
		{
			usrsctp_deregister_address(reinterpret_cast<void*>(m_id));

			ERROR_EX_LOG("usrsctp_bind() failed: %s", std::strerror(errno));
		}

		auto bufferSize = static_cast<int>(m_sctp_send_buffer_size);

		if (usrsctp_setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(int)) < 0)
		{
			usrsctp_deregister_address(reinterpret_cast<void*>(m_id));

			ERROR_EX_LOG("usrsctp_setsockopt(SO_SNDBUF) failed: %s", std::strerror(errno));
		}

		// Register the SctpAssociation into the global map.
		 g_sctp_association_mgr. RegisterSctpAssociation(this);

	}
	/*csctp_association::csctp_association()
	{
	}*/
	csctp_association::~csctp_association()
	{
		usrsctp_set_ulpinfo(m_socket, nullptr);
		usrsctp_close(m_socket);

		// Deregister ourselves from usrsctp.
		usrsctp_deregister_address(reinterpret_cast<void*>(m_id));

		// Register the SctpAssociation from the global map.
		g_sctp_association_mgr.  DeregisterSctpAssociation(this);

		delete[] m_message_buffer;
	}
	bool csctp_association::issctp(const uint8_t * data, size_t len)
	{
		// clang-format off
		return (
			(len >= 12) &&
			// Must have Source Port Number and Destination Port Number set to 5000 (hack).
			(rtc_byte::get2bytes(data, 0) == 5000) &&
			(rtc_byte::get2bytes(data, 2) == 5000)
			);
		// clang-format on
	}
	void csctp_association::TransportConnected()
	{
		// Just run the SCTP stack if our state is 'new'.
		if (m_state != ESCTPSTATE::ESCTP_NEW)
		{
			return;
		}

		try
		{
			int ret;
			struct sockaddr_conn rconn; // NOLINT(cppcoreguidelines-pro-type-member-init)

			std::memset(&rconn, 0, sizeof(rconn));
			rconn.sconn_family = AF_CONN;
			rconn.sconn_port = htons(5000);
			rconn.sconn_addr = reinterpret_cast<void*>(m_id);
#ifdef HAVE_SCONN_LEN
			rconn.sconn_len = sizeof(rconn);
#endif

			ret = usrsctp_connect(m_socket, reinterpret_cast<struct sockaddr*>(&rconn), sizeof(rconn));

			if (ret < 0 && errno != EINPROGRESS)
			{
				ERROR_EX_LOG("usrsctp_connect() failed: %s", std::strerror(errno));
			}

			// Disable MTU discovery.
			sctp_paddrparams peerAddrParams; // NOLINT(cppcoreguidelines-pro-type-member-init)

			std::memset(&peerAddrParams, 0, sizeof(peerAddrParams));
			std::memcpy(&peerAddrParams.spp_address, &rconn, sizeof(rconn));
			peerAddrParams.spp_flags = SPP_PMTUD_DISABLE;

			// The MTU value provided specifies the space available for chunks in the
			// packet, so let's subtract the SCTP header size.
			peerAddrParams.spp_pathmtu = SctpMtu - sizeof(struct sctp_common_header);

			ret = usrsctp_setsockopt(
				m_socket, IPPROTO_SCTP, SCTP_PEER_ADDR_PARAMS, &peerAddrParams, sizeof(peerAddrParams));

			if (ret < 0)
			{
				ERROR_EX_LOG("usrsctp_setsockopt(SCTP_PEER_ADDR_PARAMS) failed: %s", std::strerror(errno));
			}

			// Announce connecting state.
			m_state = ESCTPSTATE::ESCTP_CONNECTING;
			
			m_transport_ptr->OnSctpAssociationConnecting(this);
		}
		catch (.../*const MediaSoupError&*/ /*error*/)
		{
			m_state = ESCTPSTATE::ESCTP_FAILED;
			m_transport_ptr->OnSctpAssociationFailed(this);
		}
	}
	void csctp_association::ProcessSctpData(const uint8_t * data, size_t len)
	{
		usrsctp_conninput(reinterpret_cast<void*>(m_id), data, len, 0);
	}
	void csctp_association::SendSctpMessage(uint16_t streamId, uint32_t ppid, const uint8_t * msg, size_t len)
	{
		// This must be controlled by the DataConsumer.
		cassert_desc(
			len <= this->m_max_sctp_message_size,
			"given message exceeds max allowed message size [message size:%zu, max message size:%zu]",
			len,
			this->m_max_sctp_message_size);

		//const auto& parameters = dataConsumer->GetSctpStreamParameters();

		// Fill stcp_sendv_spa.
		struct sctp_sendv_spa spa; // NOLINT(cppcoreguidelines-pro-type-member-init)

		std::memset(&spa, 0, sizeof(spa));
		spa.sendv_flags = SCTP_SEND_SNDINFO_VALID;
		spa.sendv_sndinfo.snd_sid = streamId;
		spa.sendv_sndinfo.snd_ppid = htonl(ppid);
		spa.sendv_sndinfo.snd_flags = SCTP_EOR;

		// If ordered it must be reliable.
		//if (parameters.ordered)
		//{
		//	spa.sendv_prinfo.pr_policy = SCTP_PR_SCTP_NONE;
		//	spa.sendv_prinfo.pr_value = 0;
		//}
		//// Configure reliability: https://tools.ietf.org/html/rfc3758
		//else
		{
			spa.sendv_flags |= SCTP_SEND_PRINFO_VALID;
			spa.sendv_sndinfo.snd_flags |= SCTP_UNORDERED;

			/*if (parameters.maxPacketLifeTime != 0)
			{
				spa.sendv_prinfo.pr_policy = SCTP_PR_SCTP_TTL;
				spa.sendv_prinfo.pr_value = parameters.maxPacketLifeTime;
			}
			else if (parameters.maxRetransmits != 0)*/
			{
				spa.sendv_prinfo.pr_policy = SCTP_PR_SCTP_RTX;
				spa.sendv_prinfo.pr_value = 1000;/*parameters.maxRetransmits*/;
			}
		}

		this->m_sctp_buffered_amount += len;

		// Notify the listener about the buffered amount increase regardless
		// usrsctp_sendv result.
		// In case of failure the correct value will be later provided by usrsctp
		// via onSendSctpData.
		m_transport_ptr->OnSctpAssociationBufferedAmount(this, this->m_sctp_buffered_amount);

		int ret = usrsctp_sendv(
			this->m_socket, msg, len, nullptr, 0, &spa, static_cast<socklen_t>(sizeof(spa)), SCTP_SENDV_SPA, 0);

		if (ret < 0)
		{
			bool sctpSendBufferFull = errno == EWOULDBLOCK || errno == EAGAIN;

			// SCTP send buffer being full is legit, not an error.
			if (sctpSendBufferFull)
			{

				DEBUG_EX_LOG( "sctp, error sending SCTP message [sid:%" PRIu16 ", ppid:%" PRIu32 ", message size:%zu]: %s",   streamId, ppid, len, std::strerror(errno));

			}
			else
			{
				WARNING_EX_LOG(
					"sctp, error sending SCTP message [sid:%" PRIu16 ", ppid:%" PRIu32 ", message size:%zu]: %s",
					 streamId,
					ppid,
					len,
					std::strerror(errno));
			}

			 
		}
		 
	}
	void csctp_association::reply(Json::Value & value)
	{
		// Add port (always 5000). 
		// TODO@chensong     20220821
		value["port"] = 5000;

		// Add OS
		value["OS"] = m_os;

		// Add MIS.
		value["MIS"] = m_mis;

		// Add maxMessageSize.
		value["maxMessageSize"] = m_max_sctp_message_size; //this->maxSctpMessageSize;

		// Add sendBufferSize.
		value["sendBufferSize"] = m_sctp_send_buffer_size;// //this->sctpSendBufferSize;

		// Add sctpBufferedAmountLowThreshold.
		value["sctpBufferedAmount"] = m_sctp_buffered_amount;////this->sctpBufferedAmount;

		// Add isDataChannel.
		// value["sctpParameters"]["isDataChannel"] = this->isDataChannel;
	}
	void csctp_association::ResetSctpStream(uint16_t streamId, EStreamDirection direction)
	{
		// Do nothing if an outgoing stream that could not be allocated by us.
		if (direction == EStreamDirection::ESTREAM_OUTGOING && streamId > m_os - 1)
		{
			return;
		}

		int ret;
		struct sctp_assoc_value av; // NOLINT(cppcoreguidelines-pro-type-member-init)
		socklen_t len = sizeof(av);

		ret = usrsctp_getsockopt(m_socket, IPPROTO_SCTP, SCTP_RECONFIG_SUPPORTED, &av, &len);

		if (ret == 0)
		{
			if (av.assoc_value != 1)
			{
				DEBUG_EX_LOG("sctp  stream reconfiguration not negotiated");

				return;
			}
		}
		else
		{
			WARNING_EX_LOG(
				"sctp could not retrieve whether stream reconfiguration has been negotiated: %s ",
				std::strerror(errno));

			return;
		}

		// As per spec: https://tools.ietf.org/html/rfc6525#section-4.1
		len = sizeof(sctp_assoc_t) + (2 + 1) * sizeof(uint16_t);

		auto* srs = static_cast<struct sctp_reset_streams*>(std::malloc(len));

		switch (direction)
		{
		case EStreamDirection::ESTREAM_INCOMING:
			srs->srs_flags = SCTP_STREAM_RESET_INCOMING;
			break;

		case EStreamDirection::ESTREAM_OUTGOING:
			srs->srs_flags = SCTP_STREAM_RESET_OUTGOING;
			break;
		}

		srs->srs_number_streams = 1;
		srs->srs_stream_list[0] = streamId; // No need for htonl().

		ret = usrsctp_setsockopt(m_socket, IPPROTO_SCTP, SCTP_RESET_STREAMS, srs, len);

		if (ret == 0)
		{
			DEBUG_EX_LOG( "SCTP_RESET_STREAMS sent [streamId:%u]", streamId);
		}
		else
		{
			WARNING_EX_LOG(  "usrsctp_setsockopt(SCTP_RESET_STREAMS) failed: %s", std::strerror(errno));
		}

		std::free(srs);
	}
	void csctp_association::AddOutgoingStreams(bool force)
	{
		uint16_t additionalOs{ 0 };

		if (MaxSctpStreams - m_os >= 32)
		{
			additionalOs = 32;
		}
		else
		{
			additionalOs = MaxSctpStreams - m_os;
		}

		if (additionalOs == 0)
		{
			WARNING_EX_LOG("sctp, cannot add more outgoing streams [OS:%u]", m_os);

			return;
		}

		auto nextDesiredOs = m_os + additionalOs;

		// Already in progress, ignore (unless forced).
		if (!force && nextDesiredOs == m_desiredos)
		{
			return;
		}

		// Update desired value.
		 m_desiredos = nextDesiredOs;

		// If not connected, defer it.
		if (m_state != ESCTPSTATE::ESCTP_CONNECTED)
		{
			DEBUG_EX_LOG("sctp, SCTP not connected, deferring OS increase");

			return;
		}

		struct sctp_add_streams sas; // NOLINT(cppcoreguidelines-pro-type-member-init)

		std::memset(&sas, 0, sizeof(sas));
		sas.sas_instrms = 0;
		sas.sas_outstrms = additionalOs;

		DEBUG_EX_LOG("sctp, adding %u outgoing streams", additionalOs);

		int ret = usrsctp_setsockopt(
			m_socket, IPPROTO_SCTP, SCTP_ADD_STREAMS, &sas, static_cast<socklen_t>(sizeof(sas)));

		if (ret < 0)
		{
			WARNING_EX_LOG("sctp, usrsctp_setsockopt(SCTP_ADD_STREAMS) failed: %s", std::strerror(errno));
		}
	}
	void csctp_association::OnUsrSctpSendSctpData(void * buffer, size_t len)
	{
		const uint8_t* data = static_cast<uint8_t*>(buffer);

#if MS_LOG_DEV_LEVEL == 3
		MS_DUMP_DATA(data, len);
#endif

		m_transport_ptr->OnSctpAssociationSendData(this, data, len);
	}
	void csctp_association::OnUsrSctpReceiveSctpData(uint16_t streamId, uint16_t ssn, uint32_t ppid, int flags, const uint8_t * data, size_t len)
	{
		// Ignore WebRTC DataChannel Control DATA chunks.
		if (ppid == 50)
		{
			WARNING_EX_LOG("sctp, ignoring SCTP data with ppid:50 (WebRTC DataChannel Control)");

			return;
		}

		if (m_message_buffer_len   != 0 && ssn != m_last_ssn_received  )
		{
			WARNING_EX_LOG(" sctp, message chunk received with different SSN while buffer not empty, buffer discarded [ssn:%u, last ssn received:%u]",
				ssn, m_last_ssn_received);

			m_message_buffer_len = 0;
		}

		// Update last SSN received.
		m_last_ssn_received = ssn;

		auto eor = static_cast<bool>(flags & MSG_EOR);

		if (m_message_buffer_len + len > m_max_sctp_message_size  )
		{
			WARNING_EX_LOG("sctp, ongoing received message exceeds max allowed message size [message size:%zu, max message size:%zu, eor:%u]",
				m_message_buffer_len + len, m_max_sctp_message_size, eor ? 1 : 0);

			m_last_ssn_received = 0;

			return;
		}

		// If end of message and there is no buffered data, notify it directly.
		if (eor && m_message_buffer_len == 0)
		{
			//DEBUG_EX_LOG("directly notifying listener [eor:1, buffer len:0]");

			m_transport_ptr->OnSctpAssociationMessageReceived(this, streamId, ppid, data, len);
		}
		// If end of message and there is buffered data, append data and notify buffer.
		else if (eor && m_message_buffer_len != 0)
		{
			std::memcpy(m_message_buffer + m_message_buffer_len, data, len);
			m_message_buffer_len += len;

			//DEBUG_EX_LOG("notifying listener [eor:1, buffer len:%zu]", m_message_buffer_len);

			m_transport_ptr->OnSctpAssociationMessageReceived(
				this, streamId, ppid, m_message_buffer, m_message_buffer_len);

			m_message_buffer_len = 0;
		}
		// If non end of message, append data to the buffer.
		else if (!eor)
		{
			// Allocate the buffer if not already done.
			if (!m_message_buffer)
			{
				m_message_buffer = new uint8_t[m_max_sctp_message_size];
			}

			std::memcpy(m_message_buffer + m_message_buffer_len, data, len);
			m_message_buffer_len += len;

			//DEBUG_EX_LOG("data buffered [eor:0, buffer len:%zu]", m_message_buffer_len);
		}
	}
	void csctp_association::OnUsrSctpReceiveSctpNotification(sctp_notification * notification, size_t len)
	{
		if (notification->sn_header.sn_length != (uint32_t)len)
			return;

		switch (notification->sn_header.sn_type)
		{
		case SCTP_ADAPTATION_INDICATION:
		{
			DEBUG_EX_LOG("sctp, SCTP adaptation indication [%x]",
				notification->sn_adaptation_event.sai_adaptation_ind);

			break;
		}

		case SCTP_ASSOC_CHANGE:
		{
			switch (notification->sn_assoc_change.sac_state)
			{
			case SCTP_COMM_UP:
			{
				DEBUG_EX_LOG("sctp, SCTP association connected, streams [out:%u, in:%hu]",
					notification->sn_assoc_change.sac_outbound_streams,
					notification->sn_assoc_change.sac_inbound_streams);

				// Update our OS.
				m_os = notification->sn_assoc_change.sac_outbound_streams;

				// Increase if requested before connected.
				if (m_desiredos > m_os)
				{
					AddOutgoingStreams(/*force*/ true);
				}

				if (m_state != ESCTPSTATE::ESCTP_CONNECTED)
				{
					m_state = ESCTPSTATE::ESCTP_CONNECTED;
					m_transport_ptr->OnSctpAssociationConnected(this);
				}

				break;
			}

			case SCTP_COMM_LOST:
			{
				if (notification->sn_header.sn_length > 0)
				{
					static const size_t BufferSize{ 1024 };
					thread_local static char buffer[BufferSize];

					uint32_t len = notification->sn_header.sn_length;

					for (uint32_t i{ 0 }; i < len; ++i)
					{
						std::snprintf(
							buffer, BufferSize, " 0x%02x", notification->sn_assoc_change.sac_info[i]);
					}

					DEBUG_EX_LOG("sctp, SCTP communication lost [info:%s]", buffer);
				}
				else
				{
					DEBUG_EX_LOG("sctp, SCTP communication lost");
				}

				if (m_state != ESCTPSTATE::ESCTP_CLOSED)
				{
					m_state = ESCTPSTATE::ESCTP_CLOSED;
					m_transport_ptr->OnSctpAssociationClosed(this);
				}

				break;
			}

			case SCTP_RESTART:
			{
				DEBUG_EX_LOG( "sctp, SCTP remote association restarted, streams [out:%u, int:%u]",
					notification->sn_assoc_change.sac_outbound_streams,
					notification->sn_assoc_change.sac_inbound_streams);

				// Update our OS.
				m_os = notification->sn_assoc_change.sac_outbound_streams;

				// Increase if requested before connected.
				if (m_desiredos > m_os)
				{
					AddOutgoingStreams(/*force*/ true);
				}

				if (m_state != ESCTPSTATE::ESCTP_CONNECTED)
				{
					m_state = ESCTPSTATE::ESCTP_CONNECTED;
					m_transport_ptr->OnSctpAssociationConnected(this);
				}

				break;
			}

			case SCTP_SHUTDOWN_COMP:
			{
				DEBUG_EX_LOG("sctp, SCTP association gracefully closed");

				if (m_state != ESCTPSTATE::ESCTP_CLOSED)
				{
					m_state = ESCTPSTATE::ESCTP_CLOSED;
					m_transport_ptr->OnSctpAssociationClosed(this);
				}

				break;
			}

			case SCTP_CANT_STR_ASSOC:
			{
				if (notification->sn_header.sn_length > 0)
				{
					static const size_t BufferSize{ 1024 };
					thread_local static char buffer[BufferSize];

					uint32_t len = notification->sn_header.sn_length;

					for (uint32_t i{ 0 }; i < len; ++i)
					{
						std::snprintf(
							buffer, BufferSize, " 0x%02x", notification->sn_assoc_change.sac_info[i]);
					}

					WARNING_EX_LOG("sctp, SCTP setup failed: %s", buffer);
				}

				if (m_state != ESCTPSTATE::ESCTP_FAILED)
				{
					 m_state = ESCTPSTATE::ESCTP_FAILED;
					m_transport_ptr->OnSctpAssociationFailed(this);
				}

				break;
			}

			default:;
			}

			break;
		}

		// https://tools.ietf.org/html/rfc6525#section-6.1.2.
		case SCTP_ASSOC_RESET_EVENT:
		{
			DEBUG_EX_LOG("sctp,  SCTP association reset event received");

			break;
		}

		// An Operation Error is not considered fatal in and of itself, but may be
		// used with an ABORT chunk to report a fatal condition.
		case SCTP_REMOTE_ERROR:
		{
			static const size_t BufferSize{ 1024 };
			thread_local static char buffer[BufferSize];

			uint32_t len = notification->sn_remote_error.sre_length - sizeof(struct sctp_remote_error);

			for (uint32_t i{ 0 }; i < len; i++)
			{
				std::snprintf(buffer, BufferSize, "0x%02x", notification->sn_remote_error.sre_data[i]);
			}

			WARNING_EX_LOG("sctp, remote SCTP association error [type:0x%04x, data:%s]",
				notification->sn_remote_error.sre_error,
				buffer);

			break;
		}

		// When a peer sends a SHUTDOWN, SCTP delivers this notification to
		// inform the application that it should cease sending data.
		case SCTP_SHUTDOWN_EVENT:
		{
			DEBUG_EX_LOG("sctp,  remote SCTP association shutdown");

			if (m_state != ESCTPSTATE::ESCTP_CLOSED)
			{
				m_state = ESCTPSTATE::ESCTP_CLOSED;
				m_transport_ptr->OnSctpAssociationClosed(this);
			}

			break;
		}

		case SCTP_SEND_FAILED_EVENT:
		{
			static const size_t BufferSize{ 1024 };
			thread_local static char buffer[BufferSize];

			uint32_t len =
				notification->sn_send_failed_event.ssfe_length - sizeof(struct sctp_send_failed_event);

			for (uint32_t i{ 0 }; i < len; ++i)
			{
				std::snprintf(buffer, BufferSize, "0x%02x", notification->sn_send_failed_event.ssfe_data[i]);
			}

			WARNING_EX_LOG(" sctp, SCTP message sent failure [streamId:%u, ppid:%u, sent:%s, error:0x%08x, info:%s]",
				notification->sn_send_failed_event.ssfe_info.snd_sid,
				ntohl(notification->sn_send_failed_event.ssfe_info.snd_ppid),
				(notification->sn_send_failed_event.ssfe_flags & SCTP_DATA_SENT) ? "yes" : "no",
				notification->sn_send_failed_event.ssfe_error,
				buffer);

			break;
		}

		case SCTP_STREAM_RESET_EVENT:
		{
			bool incoming{ false };
			bool outgoing{ false };
			uint16_t numStreams =
				(notification->sn_strreset_event.strreset_length - sizeof(struct sctp_stream_reset_event)) /
				sizeof(uint16_t);

			if (notification->sn_strreset_event.strreset_flags & SCTP_STREAM_RESET_INCOMING_SSN)
			{
				incoming = true;
			}

			if (notification->sn_strreset_event.strreset_flags & SCTP_STREAM_RESET_OUTGOING_SSN)
			{
				outgoing = true;
			}

			//if (MS_HAS_DEBUG_TAG(sctp))
			//{
			//	std::string streamIds;

			//	for (uint16_t i{ 0 }; i < numStreams; ++i)
			//	{
			//		auto streamId = notification->sn_strreset_event.strreset_stream_list[i];

			//		// Don't log more than 5 stream ids.
			//		if (i > 4)
			//		{
			//			streamIds.append("...");

			//			break;
			//		}

			//		if (i > 0)
			//			streamIds.append(",");

			//		streamIds.append(std::to_string(streamId));
			//	}

			//	DEBUG_EX_LOG("sctp, SCTP stream reset event [flags:%x, i|o:%s|%s, num streams:%u, stream ids:%s]",
			//		notification->sn_strreset_event.strreset_flags,
			//		incoming ? "true" : "false",
			//		outgoing ? "true" : "false",
			//		numStreams,
			//		streamIds.c_str());
			//}

			// Special case for WebRTC DataChannels in which we must also reset our
			// outgoing SCTP stream.
			/*if (incoming && !outgoing && this->isDataChannel)
			{
				for (uint16_t i{ 0 }; i < numStreams; ++i)
				{
					auto streamId = notification->sn_strreset_event.strreset_stream_list[i];

					ResetSctpStream(streamId, EStreamDirection::ESTREAM_OUTGOING);
				}
			}*/

			break;
		}

		case SCTP_STREAM_CHANGE_EVENT:
		{
			if (notification->sn_strchange_event.strchange_flags == 0)
			{
				DEBUG_EX_LOG("sctp, SCTP stream changed, streams [out:%u, in:%u, flags:%x]",
					notification->sn_strchange_event.strchange_outstrms,
					notification->sn_strchange_event.strchange_instrms,
					notification->sn_strchange_event.strchange_flags);
			}
			else if (notification->sn_strchange_event.strchange_flags & SCTP_STREAM_RESET_DENIED)
			{
				WARNING_EX_LOG(" sctp, SCTP stream change denied, streams [out:%u, in:%u, flags:%x]",
					notification->sn_strchange_event.strchange_outstrms,
					notification->sn_strchange_event.strchange_instrms,
					notification->sn_strchange_event.strchange_flags);

				break;
			}
			else if (notification->sn_strchange_event.strchange_flags & SCTP_STREAM_RESET_FAILED)
			{
				WARNING_EX_LOG("sctp, SCTP stream change failed, streams [out:%u, in:%u, flags:%x]",
					notification->sn_strchange_event.strchange_outstrms,
					notification->sn_strchange_event.strchange_instrms,
					notification->sn_strchange_event.strchange_flags);

				break;
			}

			// Update OS.
			m_os = notification->sn_strchange_event.strchange_outstrms;

			break;
		}

		default:
		{
			WARNING_EX_LOG("sctp, unhandled SCTP event received [type:%u]", notification->sn_header.sn_type);
		}
		}
	}
	void csctp_association::OnUsrSctpSentData(uint32_t freeBuffer)
	{
		auto previousSctpBufferedAmount = m_sctp_buffered_amount;

		m_sctp_buffered_amount = m_sctp_send_buffer_size   - freeBuffer;

		if (m_sctp_buffered_amount != previousSctpBufferedAmount)
		{
			m_transport_ptr->OnSctpAssociationBufferedAmount(this, m_sctp_buffered_amount);
		}
	}
}