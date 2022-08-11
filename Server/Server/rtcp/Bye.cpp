#include "Bye.hpp"
//#include "Logger.hpp"
//#include "Utils.hpp"
#include <cstring>
#include "clog.h"
#include "crtc_util.h"

using namespace chen;


namespace RTC
{
	namespace RTCP
	{
		/* Class methods. */

		ByePacket* ByePacket::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			// Get the header.
			auto* header = const_cast<CommonHeader*>(reinterpret_cast<const CommonHeader*>(data));
			std::unique_ptr<ByePacket> packet(new ByePacket(header));
			// RTCP Header 头大偏移量
			size_t offset = sizeof(Packet::CommonHeader);
			// Report Block 的个数
			uint8_t count = header->count;

			// 4个的字节就是一个 ssrc 名称
			while (((count--) != 0u) && (len > offset))
			{
				if (len - offset < 4u)
				{
					WARNING_EX_LOG("rtcp, not enough space for SSRC in RTCP Bye message");

					return nullptr;
				}

				packet->AddSsrc(rtc_byte::get4bytes(data, offset));
				offset += 4u;
			}

			if (len > offset)
			{
				auto length = size_t{ rtc_byte::get1byte(data, offset) };

				offset += 1u;

				if (length <= len - offset)
					packet->SetReason(std::string(reinterpret_cast<const char*>(data) + offset, length));
			}

			return packet.release();
		}

		/* Instance methods. */

		size_t ByePacket::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			size_t offset = Packet::Serialize(buffer);

			// SSRCs.
			for (auto ssrc : this->ssrcs)
			{
				rtc_byte::set4bytes(buffer, offset, ssrc);
				offset += 4u;
			}

			if (!this->reason.empty())
			{
				// Length field.
				rtc_byte::set1byte(buffer, offset, this->reason.length());
				offset += 1u;

				// Reason field.
				std::memcpy(buffer + offset, this->reason.c_str(), this->reason.length());
				offset += this->reason.length();
			}

			// 32 bits padding.
			size_t padding = (-offset) & 3;

			for (size_t i{ 0 }; i < padding; ++i)
			{
				buffer[offset + i] = 0;
			}

			return offset + padding;
		}

		void ByePacket::Dump() const
		{
			//MS_TRACE();

			NORMAL_LOG("<ByePacket>");
			for (auto ssrc : this->ssrcs)
			{
				NORMAL_LOG("  ssrc   : %u", ssrc);
			}
			if (!this->reason.empty())
				NORMAL_LOG("  reason : %s", this->reason.c_str());
			NORMAL_LOG("</ByePacket>");
		}
	} // namespace RTCP
} // namespace RTC
