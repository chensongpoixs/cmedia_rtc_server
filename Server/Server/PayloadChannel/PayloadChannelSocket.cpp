#define MS_CLASS "PayloadChannel::PayloadChannelSocket"
// #define MS_LOG_DEV_LEVEL 3

#include "PayloadChannel/PayloadChannelSocket.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include "MediaSoupErrors.hpp"
#include "PayloadChannel/PayloadChannelRequest.hpp"
#include <cmath>   // std::ceil()
#include <cstdio>  // sprintf()
#include <cstring> // std::memcpy(), std::memmove()
extern "C"
{
#include <netstring.h>
}

namespace PayloadChannel
{
	/* Static. */

	// netstring length for a 4194304 bytes payload.
	static constexpr size_t NsMessageMaxLen{ 4194313 };
	static constexpr size_t NsPayloadMaxLen{ 4194304 };

	/* Instance methods. */
	PayloadChannelSocket::PayloadChannelSocket(int consumerFd, int producerFd)
	  : consumerSocket(consumerFd, NsMessageMaxLen, this), producerSocket(producerFd, NsMessageMaxLen)
	{
		MS_TRACE();

		this->writeBuffer = (uint8_t*)std::malloc(NsMessageMaxLen);
	}

	PayloadChannelSocket::~PayloadChannelSocket()
	{
		MS_TRACE();

		std::free(this->writeBuffer);
		delete this->ongoingNotification;

		if (!this->closed)
			Close();
	}

	void PayloadChannelSocket::Close()
	{
		MS_TRACE_STD();

		if (this->closed)
			return;

		this->closed = true;

		this->consumerSocket.Close();
		this->producerSocket.Close();
	}

	void PayloadChannelSocket::SetListener(Listener* listener)
	{
		MS_TRACE();

		this->listener = listener;
	}

	void PayloadChannelSocket::Send(json& jsonMessage, const uint8_t* payload, size_t payloadLen)
	{
		MS_TRACE();

		if (this->closed)
			return;

		std::string message = jsonMessage.dump();
		DEBUG_EX_LOG("[message = %s][payload= %s]", message.c_str(), payload);
		if (message.length() > NsPayloadMaxLen)
		{
			MS_ERROR("mesage too big");

			return;
		}
		else if (payloadLen > NsPayloadMaxLen)
		{
			MS_ERROR("payload too big");

			return;
		}

		SendImpl(message.c_str(), message.length());
		SendImpl(payload, payloadLen);
	}

	void PayloadChannelSocket::Send(json& jsonMessage)
	{
		MS_TRACE_STD();

		if (this->closed)
			return;

		std::string message = jsonMessage.dump();
		DEBUG_EX_LOG("message = %s", message.c_str());
		if (message.length() > NsPayloadMaxLen)
		{
			MS_ERROR_STD("mesage too big");

			return;
		}

		SendImpl(message.c_str(), message.length());
	}

	inline void PayloadChannelSocket::SendImpl(const void* nsPayload, size_t nsPayloadLen)
	{
		MS_TRACE();

		size_t nsNumLen;
		DEBUG_EX_LOG("[nsPayload = %s]", nsPayload);
		if (nsPayloadLen == 0)
		{
			nsNumLen             = 1;
			this->writeBuffer[0] = '0';
			this->writeBuffer[1] = ':';
			this->writeBuffer[2] = ',';
		}
		else
		{
			nsNumLen = static_cast<size_t>(std::ceil(std::log10(static_cast<double>(nsPayloadLen) + 1)));
			std::sprintf(reinterpret_cast<char*>(this->writeBuffer), "%zu:", nsPayloadLen);
			std::memcpy(this->writeBuffer + nsNumLen + 1, nsPayload, nsPayloadLen);
			this->writeBuffer[nsNumLen + nsPayloadLen + 1] = ',';
		}

		size_t nsLen = nsNumLen + nsPayloadLen + 2;

		this->producerSocket.Write(this->writeBuffer, nsLen);
	}

	void PayloadChannelSocket::OnConsumerSocketMessage(
	  ConsumerSocket* /*consumerSocket*/, char* msg, size_t msgLen)
	{
		MS_TRACE();

		if (!this->ongoingNotification && !this->ongoingRequest)
		{
			json jsonData = json::parse(msg, msg + msgLen);
			if (PayloadChannelRequest::IsRequest(jsonData))
			{
				try
				{
					json jsonMessage     = json::parse(msg, msg + msgLen);
					this->ongoingRequest = new PayloadChannel::PayloadChannelRequest(this, jsonMessage);
				}
				catch (const json::parse_error& error)
				{
					MS_ERROR_STD("JSON parsing error: %s", error.what());
				}
				catch (const MediaSoupError& error)
				{
					MS_ERROR("discarding wrong Payload Channel notification");
				}
			}

			else if (Notification::IsNotification(jsonData))
			{
				try
				{
					json jsonMessage          = json::parse(msg, msg + msgLen);
					this->ongoingNotification = new PayloadChannel::Notification(jsonMessage);
				}
				catch (const json::parse_error& error)
				{
					MS_ERROR_STD("JSON parsing error: %s", error.what());
				}
				catch (const MediaSoupError& error)
				{
					MS_ERROR("discarding wrong Payload Channel notification");
				}
			}

			else
			{
				MS_ERROR("discarding wrong Payload Channel data");
			}
		}
		else if (this->ongoingNotification)
		{
			this->ongoingNotification->SetPayload(reinterpret_cast<const uint8_t*>(msg), msgLen);

			// Notify the listener.
			try
			{
				this->listener->OnPayloadChannelNotification(this, this->ongoingNotification);
			}
			catch (const MediaSoupError& error)
			{
				MS_ERROR("notification failed: %s", error.what());
			}

			// Delete the Notification.
			delete this->ongoingNotification;
			this->ongoingNotification = nullptr;
		}
		else if (this->ongoingRequest)
		{
			this->ongoingRequest->SetPayload(reinterpret_cast<const uint8_t*>(msg), msgLen);

			// Notify the listener.
			try
			{
				this->listener->OnPayloadChannelRequest(this, this->ongoingRequest);
			}
			catch (const MediaSoupTypeError& error)
			{
				this->ongoingRequest->TypeError(error.what());
			}
			catch (const MediaSoupError& error)
			{
				this->ongoingRequest->Error(error.what());
			}

			// Delete the Request.
			delete this->ongoingRequest;
			this->ongoingRequest = nullptr;
		}
	}

	void PayloadChannelSocket::OnConsumerSocketClosed(ConsumerSocket* /*consumerSocket*/)
	{
		MS_TRACE();

		this->listener->OnPayloadChannelClosed(this);
	}

	ConsumerSocket::ConsumerSocket(int fd, size_t bufferSize, Listener* listener)
	  : ::UnixStreamSocket(fd, bufferSize, ::UnixStreamSocket::Role::CONSUMER), listener(listener)
	{
		MS_TRACE();

		this->readBuffer = static_cast<uint8_t*>(std::malloc(NsMessageMaxLen));
	}

	ConsumerSocket::~ConsumerSocket()
	{
		MS_TRACE();

		std::free(this->readBuffer);
	}

	void ConsumerSocket::UserOnUnixStreamRead()
	{
		MS_TRACE();

		// Be ready to parse more than a single message in a single chunk.
		while (true)
		{
			if (IsClosed())
				return;

			size_t readLen = this->bufferDataLen - this->msgStart;
			char* msgStart = nullptr;
			size_t msgLen;
			int nsRet = netstring_read(
			  reinterpret_cast<char*>(this->buffer + this->msgStart), readLen, &msgStart, &msgLen);

			if (nsRet != 0)
			{
				switch (nsRet)
				{
					case NETSTRING_ERROR_TOO_SHORT:
					{
						// Check if the buffer is full.
						if (this->bufferDataLen == this->bufferSize)
						{
							// First case: the incomplete message does not begin at position 0 of
							// the buffer, so move the incomplete message to the position 0.
							if (this->msgStart != 0)
							{
								std::memmove(this->buffer, this->buffer + this->msgStart, readLen);
								this->msgStart      = 0;
								this->bufferDataLen = readLen;
							}
							// Second case: the incomplete message begins at position 0 of the buffer.
							// The message is too big, so discard it.
							else
							{
								MS_ERROR(
								  "no more space in the buffer for the unfinished message being parsed, "
								  "discarding it");

								this->msgStart      = 0;
								this->bufferDataLen = 0;
							}
						}

						// Otherwise the buffer is not full, just wait.
						return;
					}

					case NETSTRING_ERROR_TOO_LONG:
					{
						MS_ERROR("NETSTRING_ERROR_TOO_LONG");

						break;
					}

					case NETSTRING_ERROR_NO_COLON:
					{
						MS_ERROR("NETSTRING_ERROR_NO_COLON");

						break;
					}

					case NETSTRING_ERROR_NO_COMMA:
					{
						MS_ERROR("NETSTRING_ERROR_NO_COMMA");

						break;
					}

					case NETSTRING_ERROR_LEADING_ZERO:
					{
						MS_ERROR("NETSTRING_ERROR_LEADING_ZERO");

						break;
					}

					case NETSTRING_ERROR_NO_LENGTH:
					{
						MS_ERROR("NETSTRING_ERROR_NO_LENGTH");

						break;
					}
				}

				// Error, so reset and exit the parsing loop.
				this->msgStart      = 0;
				this->bufferDataLen = 0;

				return;
			}

			// If here it means that msgStart points to the beginning of a message
			// with msgLen bytes length, so recalculate readLen.
			readLen =
			  reinterpret_cast<const uint8_t*>(msgStart) - (this->buffer + this->msgStart) + msgLen + 1;

			std::memcpy(this->readBuffer, msgStart, msgLen);
			this->listener->OnConsumerSocketMessage(this, reinterpret_cast<char*>(this->readBuffer), msgLen);

			// If there is no more space available in the buffer and that is because
			// the latest parsed message filled it, then empty the full buffer.
			if ((this->msgStart + readLen) == this->bufferSize)
			{
				this->msgStart      = 0;
				this->bufferDataLen = 0;
			}
			// If there is still space in the buffer, set the beginning of the next
			// parsing to the next position after the parsed message.
			else
			{
				this->msgStart += readLen;
			}

			// If there is more data in the buffer after the parsed message
			// then parse again. Otherwise break here and wait for more data.
			if (this->bufferDataLen > this->msgStart)
			{
				continue;
			}

			break;
		}
	}

	void ConsumerSocket::UserOnUnixStreamSocketClosed()
	{
		MS_TRACE();

		// Notify the listener.
		this->listener->OnConsumerSocketClosed(this);
	}

	ProducerSocket::ProducerSocket(int fd, size_t bufferSize)
	  : ::UnixStreamSocket(fd, bufferSize, ::UnixStreamSocket::Role::PRODUCER)
	{
		MS_TRACE();
	}
} // namespace PayloadChannel
