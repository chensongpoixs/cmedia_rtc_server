﻿#ifndef MS_RTC_ICE_SERVER_HPP
#define MS_RTC_ICE_SERVER_HPP

#include "common.hpp"
#include "RTC/StunPacket.hpp"
#include "RTC/TransportTuple.hpp"
#include <list>
#include <string>

namespace RTC
{
	class IceServer
	{
	public:
		enum class IceState
		{
			NEW = 1,
			CONNECTED,
			COMPLETED,
			DISCONNECTED
		};

	public:
		class Listener
		{
		public:
			virtual ~Listener() = default;

		public:
			/**
			 * These callbacks are guaranteed to be called before ProcessStunPacket()
			 * returns, so the given pointers are still usable.
			 */
			virtual void OnIceServerSendStunPacket(
			  const RTC::IceServer* iceServer, const RTC::StunPacket* packet, RTC::TransportTuple* tuple) = 0;
			virtual void OnIceServerSelectedTuple(
			  const RTC::IceServer* iceServer, RTC::TransportTuple* tuple)        = 0;
			virtual void OnIceServerConnected(const RTC::IceServer* iceServer)    = 0;
			virtual void OnIceServerCompleted(const RTC::IceServer* iceServer)    = 0;
			virtual void OnIceServerDisconnected(const RTC::IceServer* iceServer) = 0;
		};

	public:
		IceServer(Listener* listener, const std::string& usernameFragment, const std::string& password);

	public:
		void ProcessStunPacket(RTC::StunPacket* packet, RTC::TransportTuple* tuple);
		const std::string& GetUsernameFragment() const
		{
			return this->usernameFragment;
		}
		const std::string& GetPassword() const
		{
			return this->password;
		}
		IceState GetState() const
		{
			return this->state;
		}
		RTC::TransportTuple* GetSelectedTuple() const
		{
			return this->selectedTuple;
		}
		void SetUsernameFragment(const std::string& usernameFragment)
		{
			this->oldUsernameFragment = this->usernameFragment;
			this->usernameFragment    = usernameFragment;
		}
		void SetPassword(const std::string& password)
		{
			this->oldPassword = this->password;
			this->password    = password;
		}
		bool IsValidTuple(const RTC::TransportTuple* tuple) const;
		void RemoveTuple(RTC::TransportTuple* tuple);
		// This should be just called in 'connected' or completed' state
		// and the given tuple must be an already valid tuple.
		void ForceSelectedTuple(const RTC::TransportTuple* tuple);

	private:
		void HandleTuple(RTC::TransportTuple* tuple, bool hasUseCandidate);
		/**
		 * Store the given tuple and return its stored address.
		 */
		RTC::TransportTuple* AddTuple(RTC::TransportTuple* tuple);
		/**
		 * If the given tuple exists return its stored address, nullptr otherwise.
		 */
		RTC::TransportTuple* HasTuple(const RTC::TransportTuple* tuple) const;
		/**
		 * Set the given tuple as the selected tuple.
		 * NOTE: The given tuple MUST be already stored within the list.
		 */
		void SetSelectedTuple(RTC::TransportTuple* storedTuple);

	private:
		// Passed by argument.
		Listener* listener{ nullptr };
		// Others.
		std::string usernameFragment; // 用户名
		std::string password;		  // 密码
		std::string oldUsernameFragment;   // ICE 从新启动后 用户名和密码修改这边报错上一次用户名和密码
		std::string oldPassword;
		IceState state{ IceState::NEW };
		std::list<RTC::TransportTuple> tuples;
		RTC::TransportTuple* selectedTuple{ nullptr };
	};
} // namespace RTC

#endif
