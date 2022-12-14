//#define MS_CLASS "RTC::RTCP::Packet"
// #define MS_LOG_DEV_LEVEL 3

#include "Packet.hpp"
//#include "Logger.hpp"
#include "Bye.hpp"
#include "Feedback.hpp"
#include "ReceiverReport.hpp"
#include "Sdes.hpp"
#include "SenderReport.hpp"
#include "XR.hpp"
#include "clog.h"

using namespace chen;

namespace RTC
{
	namespace RTCP
	{
		/* Namespace variables. */

		uint8_t Buffer[BufferSize];

		/* Class variables. */

		// clang-format off
		std::map<Type, std::string> Packet::type2String =
		{
			{ Type::SR,    "SR"    },
			{ Type::RR,    "RR"    },
			{ Type::SDES,  "SDES"  },
			{ Type::BYE,   "BYE"   },
			{ Type::APP,   "APP"   },
			{ Type::RTPFB, "RTPFB" },
			{ Type::PSFB,  "PSFB"  },
			{ Type::XR,    "XR"    }
		};
		// clang-format on

		/* Class methods. */

		Packet* Packet::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			// First, Currently parsing and Last RTCP packets in the compound packet.
			Packet* first{ nullptr };
			Packet* current{ nullptr };
			Packet* last{ nullptr };

			while (len > 0u)
			{
				if (!Packet::IsRtcp(data, len))
				{
					WARNING_EX_LOG("rtcp, data is not a RTCP packet");

					return first;
				}

				auto* header     = const_cast<CommonHeader*>(reinterpret_cast<const CommonHeader*>(data));
				// 这一步 ？？？ 是不是很有问题啊     ========>  数据的 要加上头的大小的哈
				size_t packetLen = static_cast<size_t>(ntohs(header->length) + 1) * 4;

				if (len < packetLen)
				{
					WARNING_EX_LOG("rtcp, packet length exceeds remaining data [len:%zu,  packet len:%zu]",
					  len,   packetLen);

					return first;
				}

				switch (Type(header->packetType))
				{
					case Type::SR:
					{
						// 1. PT= 200 发送反馈包
						current = SenderReportPacket::Parse(data, packetLen);

						if (!current)
							break;

						if (header->count > 0)
						{
							Packet* rr = ReceiverReportPacket::Parse(data, packetLen, current->GetSize());

							if (!rr)
								break;

							current->SetNext(rr);
						}

						break;
					}

					case Type::RR:
					{
						//2. PT = 201  接受多少包发送给对端
						current = ReceiverReportPacket::Parse(data, packetLen);

						break;
					}

					case Type::SDES:
					{
						// 3. PT = 202  对媒体源的描述
						current = SdesPacket::Parse(data, packetLen);

						break;
					}

					case Type::BYE:
					{
						// 4. PT = 203 不需要传输的数据
						current = ByePacket::Parse(data, packetLen);

						break;
					}

					case Type::APP:
					{
						// 5. PT = 204 应用自定义信息
						current = nullptr;

						break;
					}

					case Type::RTPFB:
					{
						// 6. PT = 205 反馈信息
						current = FeedbackRtpPacket::Parse(data, packetLen);

						break;
					}

					case Type::PSFB:
					{
						// 7. PT= 206  负载情况 反馈信息
						current = FeedbackPsPacket::Parse(data, packetLen);

						break;
					}

					case Type::XR:
					{
						// 8. PT = 207  扩展头
						current = ExtendedReportPacket::Parse(data, packetLen);

						break;
					}

					default:
					{
						WARNING_EX_LOG("rtcp, unknown RTCP packet type [packetType:%hhu]", header->packetType);

						current = nullptr;
					}
				}

				if (!current)
				{
					std::string packetType = Type2String(Type(header->packetType));

					if (Type(header->packetType) == Type::PSFB)
					{
						packetType +=
						  " " + FeedbackPsPacket::MessageType2String(FeedbackPs::MessageType(header->count));
					}
					else if (Type(header->packetType) == Type::RTPFB)
					{
						packetType +=
						  " " + FeedbackRtpPacket::MessageType2String(FeedbackRtp::MessageType(header->count));
					}

					WARNING_EX_LOG("rtcp, error parsing %s Packet", packetType.c_str());

					return first;
				}

				data += packetLen;
				len -= packetLen;

				if (!first)
					first = current;
				else
					last->SetNext(current);

				last = current->GetNext() != nullptr ? current->GetNext() : current;
			}

			return first;
		}

		const std::string& Packet::Type2String(Type type)
		{
			//MS_TRACE();

			static const std::string Unknown("UNKNOWN");

			auto it = Packet::type2String.find(type);

			if (it == Packet::type2String.end())
				return Unknown;

			return it->second;
		}

		/* Instance methods. */

		size_t Packet::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			this->header = reinterpret_cast<CommonHeader*>(buffer);

			size_t length = (GetSize() / 4) - 1;

			// Fill the common header.
			this->header->version    = 2;
			this->header->padding    = 0;
			this->header->count      = static_cast<uint8_t>(GetCount());
			this->header->packetType = static_cast<uint8_t>(GetType());
			this->header->length     = uint16_t{ htons(length) };

			return sizeof(CommonHeader);
		}
	} // namespace RTCP
} // namespace RTC
