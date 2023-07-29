/***********************************************************************************************
created: 		2023-02-13

author:			chensong

purpose:		 rtc——stun define
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

#include "cice_server.h"
#include "ctransport_tuple.h"
#include "crtc_transport.h"
namespace chen {
	 
	bool cice_server::init(const std::string& username, const std::string &password)
	{
		m_username_fragment = username;
		m_password = password;
		return true;
	}
	void cice_server::update(uint32 uDeltaTime)
	{
	}
	void cice_server::destroy()
	{
	}

	void cice_server::ProcessStunPacket(const uint8* data, size_t len, ctransport_tuple* tuple)
	{
		crtc_stun_packet stun_packet;
		if (0 != stun_packet.decode((const char *)(data), len))
		{
			WARNING_EX_LOG("stun decode packet failed !!!");
			return;
		}
		if (!stun_packet.is_binding_request())
		{
			WARNING_EX_LOG("stun not binding request failed !!!");
			return;
		}
		char buf[1500] = { 0 };
		cbuffer  stream(buf, sizeof(buf));
		{
			crtc_stun_packet stun_response;
			stun_response.set_message_type(EBindingResponse);
			stun_response.set_local_ufrag(stun_packet.get_remote_ufrag());
			stun_response.set_remote_ufrag(stun_packet.get_local_ufrag());
			stun_response.set_transcation_id(stun_packet.get_transcation_id());
			// FIXME: inet_addr is deprecated, IPV6 support
			int32 family;
			uint16_t port;
			std::string ip;

			uv_ip::GetAddressInfo(tuple->GetRemoteAddress(), family, ip, port);
			//NORMAL_EX_LOG("[remote_ip = %s][remote_port = %u]",  ip.c_str(), port);
			//NORMAL_EX_LOG(" [localip = %s][localport = %u]",  m_current_socket_ptr->GetLocalIp().c_str(), m_current_socket_ptr->GetLocalPort());
			stun_response.set_mapped_address(be32toh(inet_addr(ip.c_str())));
			stun_response.set_mapped_port(port);
			stun_response.encode(m_password/*m_local_sdp.get_ice_pwd()*/, &stream);
			//m_current_socket_ptr->Send((const uint8_t *)stream.data(), stream.pos(), remoteAddr, nullptr);
			//m_tcp_connection_ptr->Send((const uint8_t *)stream.data(), stream.pos(), nullptr);
			//tuple->Send((const uint8_t *)stream.data(), stream.pos());
			m_listenter_ptr->OnIceServerSendStunPacket(  tuple, (const uint8_t *)stream.data(), stream.pos());
			HandleTuple(tuple, stun_packet.get_use_candidate());
		}
		//NORMAL_EX_LOG("[m_time_out_ms = %u]", uv_util::GetTimeMs() - m_time_out_ms);
		//m_time_out_ms = uv_util::GetTimeMs();
		///////////////////////////////////////////////////////////////////////////////////////
		//if (m_rtc_net_state == ERtcNetworkStateWaitingStun)
		{
			//m_rtc_net_state = ERtcNetworkStateDtls;
			//cdtls_client * dtls_client_ptr = dynamic_cast<cdtls_client*>(m_dtls_ptr);
			//dtls_client_ptr->init();
			//m_dtls_ptr->start_active_handshake("server");
		}
		// Handle the tuple.
		// HandleTuple(tuple, stun_packet.get_use_candidate());

	}

	bool cice_server::IsValidTuple(const ctransport_tuple * tuple) const
	{
		return HasTuple(tuple) != nullptr;
	}

	void cice_server::RemoveTuple(ctransport_tuple * tuple)
	{
		ctransport_tuple* removedTuple{ nullptr };

		// Find the removed tuple.
		auto it = this->m_tuples.begin();

		for (; it != this->m_tuples.end(); ++it)
		{
			ctransport_tuple* storedTuple = std::addressof(*it);

			if (storedTuple->Compare(tuple))
			{
				removedTuple = storedTuple;

				break;
			}
		}

		// If not found, ignore.
		if (!removedTuple)
		{
			return;
		}

		// Remove from the list of tuples.
		this->m_tuples.erase(it);

		// If this is not the selected tuple, stop here.
		if (removedTuple != this->m_selected_tuple_ptr)
			return;

		// Otherwise this was the selected tuple.
		this->m_selected_tuple_ptr = nullptr;

		// Mark the first tuple as selected tuple (if any).
		if (this->m_tuples.begin() != this->m_tuples.end())
		{
			//INFO_EX_LOG("");
			SetSelectedTuple(std::addressof(*this->m_tuples.begin()));
		}
		// Or just emit 'disconnected'.
		else
		{
			// Update state.
			this->m_ice_state = EIceDisConnected;
			// Notify the listener.
			this->m_listenter_ptr->OnIceServerDisconnected( );
		}
	}

	void cice_server::ForceSelectedTuple(const ctransport_tuple * tuple)
	{
		auto* storedTuple = HasTuple(tuple);

		/*MS_ASSERT(
			storedTuple,
			"cannot force the selected tuple if the given tuple was not already a valid tuple");
		INFO_EX_LOG("");*/
		// Mark it as selected tuple.
		SetSelectedTuple(storedTuple);
	}

	void cice_server::HandleTuple(ctransport_tuple * tuple, bool hasUseCandidate)
	{
		switch (this->m_ice_state)
		{
		case EIceNew:
		{
			// There should be no tuples.
			//MS_ASSERT(
			//	this->tuples.empty(), "state is 'new' but there are %zu tuples", this->tuples.size());

			// There shouldn't be a selected tuple.
			//MS_ASSERT(!this->selectedTuple, "state is 'new' but there is selected tuple");

			if (!hasUseCandidate)
			{
				//MS_DEBUG_TAG(ice, "transition from state 'new' to 'connected'");

				// Store the tuple.
				auto* storedTuple = AddTuple(tuple);
				//INFO_EX_LOG("");
				// Mark it as selected tuple.
				SetSelectedTuple(storedTuple);
				// Update state.
				this->m_ice_state = EIceConnected;
				// Notify the listener.
				this->m_listenter_ptr->OnIceServerConnected( );
			}
			else
			{
				//MS_DEBUG_TAG(ice, "transition from state 'new' to 'completed'");

				// Store the tuple.
				auto* storedTuple = AddTuple(tuple);
				//INFO_EX_LOG("");
				// Mark it as selected tuple.
				SetSelectedTuple(storedTuple);
				// Update state.
				this->m_ice_state = EIceCompleted;
				// Notify the listener.
				this->m_listenter_ptr->OnIceServerCompleted();
			}

			break;
		}

		case EIceDisConnected:
		{
			// There should be no tuples.
			//MS_ASSERT(
			//	this->tuples.empty(),
			//	"state is 'disconnected' but there are %zu tuples",
			//	this->tuples.size());

			// There shouldn't be a selected tuple.
			//MS_ASSERT(!this->selectedTuple, "state is 'disconnected' but there is selected tuple");

			if (!hasUseCandidate)
			{
				//MS_DEBUG_TAG(ice, "transition from state 'disconnected' to 'connected'");

				// Store the tuple.
				auto* storedTuple = AddTuple(tuple);
				//INFO_EX_LOG("");
				// Mark it as selected tuple.
				SetSelectedTuple(storedTuple);
				// Update state.
				this->m_ice_state = EIceConnected;
				// Notify the listener.
				this->m_listenter_ptr->OnIceServerConnected( );
			}
			else
			{
				//MS_DEBUG_TAG(ice, "transition from state 'disconnected' to 'completed'");

				// Store the tuple.
				auto* storedTuple = AddTuple(tuple);
				//INFO_EX_LOG("");
				// Mark it as selected tuple.
				SetSelectedTuple(storedTuple);
				// Update state.
				this->m_ice_state = EIceCompleted;
				// Notify the listener.
				this->m_listenter_ptr->OnIceServerCompleted( );
			}

			break;
		}

		case EIceConnected:
		{
			// There should be some tuples.
			//MS_ASSERT(!this->tuples.empty(), "state is 'connected' but there are no tuples");

			// There should be a selected tuple.
			//MS_ASSERT(this->selectedTuple, "state is 'connected' but there is not selected tuple");

			if (!hasUseCandidate)
			{
				// If a new tuple store it.
				if (!HasTuple(tuple))
					AddTuple(tuple);
			}
			else
			{
				//MS_DEBUG_TAG(ice, "transition from state 'connected' to 'completed'");

				auto* storedTuple = HasTuple(tuple);

				// If a new tuple store it.
				if (!storedTuple)
					storedTuple = AddTuple(tuple);
				//INFO_EX_LOG("");
				// Mark it as selected tuple.
				SetSelectedTuple(storedTuple);
				// Update state.
				this->m_ice_state = EIceCompleted;
				// Notify the listener.
				this->m_listenter_ptr->OnIceServerCompleted();
			}

			break;
		}

		case EIceCompleted:
		{
			// There should be some tuples.
			//MS_ASSERT(!this->tuples.empty(), "state is 'completed' but there are no tuples");

			// There should be a selected tuple.
			//MS_ASSERT(this->selectedTuple, "state is 'completed' but there is not selected tuple");

			if (!hasUseCandidate)
			{
				// If a new tuple store it.
				if (!HasTuple(tuple))
					AddTuple(tuple);
			}
			else
			{
				auto* storedTuple = HasTuple(tuple);

				// If a new tuple store it.
				if (!storedTuple)
					storedTuple = AddTuple(tuple);
				//INFO_EX_LOG("");
				// Mark it as selected tuple.
				SetSelectedTuple(storedTuple);
			}

			break;
		}
		}
	}

	ctransport_tuple * cice_server::AddTuple(ctransport_tuple * tuple)
	{
		// Add the new tuple at the beginning of the list.
		this->m_tuples.push_front(*tuple);

		auto* storedTuple = std::addressof(*this->m_tuples.begin());

		// If it is UDP then we must store the remote address (until now it is
		// just a pointer that will be freed soon).
		if (storedTuple->GetProtocol() == ctransport_tuple::ERtcUdp)
			storedTuple->StoreUdpRemoteAddress();

		// Return the address of the inserted tuple.
		return storedTuple;
	}

	ctransport_tuple * cice_server::HasTuple(const ctransport_tuple * tuple) const
	{
		// If there is no selected tuple yet then we know that the tuples list
		// is empty.
		if (!this->m_selected_tuple_ptr)
			return nullptr;

		// Check the current selected tuple.
		if (this->m_selected_tuple_ptr->Compare(tuple))
			return this->m_selected_tuple_ptr;

		// Otherwise check other stored tuples.
		for (const auto& it : this->m_tuples)
		{
			auto* storedTuple = const_cast<ctransport_tuple*>(std::addressof(it));

			if (storedTuple->Compare(tuple))
				return storedTuple;
		}

		return nullptr;
		return nullptr;
	}

	void cice_server::SetSelectedTuple(ctransport_tuple * storedTuple)
	{

		// If already the selected tuple do nothing.
		if (storedTuple == this->m_selected_tuple_ptr)
			return;

		this->m_selected_tuple_ptr = storedTuple;

		// Notify the listener.
		this->m_listenter_ptr->OnIceServerSelectedTuple(  m_selected_tuple_ptr);
	}

}