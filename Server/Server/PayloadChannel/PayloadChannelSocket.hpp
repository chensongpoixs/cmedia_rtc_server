#ifndef MS_PAYLOAD_CHANNEL_UNIX_STREAM_SOCKET_HPP
#define MS_PAYLOAD_CHANNEL_UNIX_STREAM_SOCKET_HPP

#include "common.hpp"
#include "PayloadChannel/Notification.hpp"
#include "PayloadChannel/PayloadChannelRequest.hpp"
#include "handles/UnixStreamSocket.hpp"
#include <nlohmann/json.hpp>

namespace PayloadChannel
{
	class ConsumerSocket : public ::UnixStreamSocket
	{
	public:
		class Listener
		{
		public:
			virtual ~Listener() = default;

		public:
			virtual void OnConsumerSocketMessage(ConsumerSocket* consumerSocket, char* msg, size_t msgLen) = 0;
			virtual void OnConsumerSocketClosed(ConsumerSocket* consumerSocket) = 0;
		};

	public:
		ConsumerSocket(int fd, size_t bufferSize, Listener* listener);
		~ConsumerSocket();

		/* Pure virtual methods inherited from ::UnixStreamSocket. */
	public:
		void UserOnUnixStreamRead() override;
		void UserOnUnixStreamSocketClosed() override;

	private:
		// Passed by argument.
		Listener* listener{ nullptr };
		// Allocated by this.
		uint8_t* readBuffer{ nullptr };
		// Others.
		size_t msgStart{ 0u }; // Where the latest message starts.
	};

	class ProducerSocket : public ::UnixStreamSocket
	{
	public:
		ProducerSocket(int fd, size_t bufferSize);

		/* Pure virtual methods inherited from ::UnixStreamSocket. */
	public:
		void UserOnUnixStreamRead() override
		{
		}
		void UserOnUnixStreamSocketClosed() override
		{
		}
	};

	class PayloadChannelSocket : public ConsumerSocket::Listener
	{
	public:
		class Listener
		{
		public:
			virtual ~Listener() = default;

		public:
			virtual void OnPayloadChannelNotification(
			  PayloadChannel::PayloadChannelSocket* payloadChannel,
			  PayloadChannel::Notification* notification) = 0;
			virtual void OnPayloadChannelRequest(
			  PayloadChannel::PayloadChannelSocket* payloadChannel,
			  PayloadChannel::PayloadChannelRequest* request)                                         = 0;
			virtual void OnPayloadChannelClosed(PayloadChannel::PayloadChannelSocket* payloadChannel) = 0;
		};

	public:
		explicit PayloadChannelSocket(int consumerFd, int producerFd);
		virtual ~PayloadChannelSocket();

	public:
		void Close();
		void SetListener(Listener* listener);
		void Send(json& jsonMessage, const uint8_t* payload, size_t payloadLen);
		void Send(json& jsonMessage);

	private:
		void SendImpl(const void* nsPayload, size_t nsPayloadLen);

		/* Pure virtual methods inherited from ConsumerSocket::Listener. */
	public:
		void OnConsumerSocketMessage(ConsumerSocket* consumerSocket, char* msg, size_t msgLen) override;
		void OnConsumerSocketClosed(ConsumerSocket* consumerSocket) override;

	private:
		// Passed by argument.
		Listener* listener{ nullptr };
		// Others.
		bool closed{ false };
		ConsumerSocket consumerSocket;
		ProducerSocket producerSocket;
		PayloadChannel::Notification* ongoingNotification{ nullptr };
		PayloadChannel::PayloadChannelRequest* ongoingRequest{ nullptr };
		uint8_t* writeBuffer;
	};
} // namespace PayloadChannel

#endif
