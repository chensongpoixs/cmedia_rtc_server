//#define MS_CLASS "RTC::RTCP::Sdes"
// #define MS_LOG_DEV_LEVEL 3

#include "Sdes.hpp"
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
		/* Item Class variables. */

		// clang-format off
		std::map<SdesItem::Type, std::string> SdesItem::type2String =
		{
			{ SdesItem::Type::END,   "END"   },
			{ SdesItem::Type::CNAME, "CNAME" },
			{ SdesItem::Type::NAME,  "NAME"  },
			{ SdesItem::Type::EMAIL, "EMAIL" },
			{ SdesItem::Type::PHONE, "PHONE" },
			{ SdesItem::Type::LOC,   "LOC"   },
			{ SdesItem::Type::TOOL,  "TOOL"  },
			{ SdesItem::Type::NOTE,  "NOTE"  },
			{ SdesItem::Type::PRIV,  "PRIV"  }
		};
		// clang-format on

		/* Class methods. */

		SdesItem* SdesItem::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			// Get the header.
			auto* header = const_cast<Header*>(reinterpret_cast<const Header*>(data));

			// data size must be >= header + length value.
			if (len < sizeof(Header) || len < (1u * 2) + header->length)
			{
				WARNING_EX_LOG("rtcp, not enough space for SDES item, discarded");

				return nullptr;
			}

			if (header->type == SdesItem::Type::END)
				return nullptr;

			return new SdesItem(header);
		}

		const std::string& SdesItem::Type2String(SdesItem::Type type)
		{
			static const std::string Unknown("UNKNOWN");

			auto it = SdesItem::type2String.find(type);

			if (it == SdesItem::type2String.end())
				return Unknown;

			return it->second;
		}

		/* Instance methods. */

		SdesItem::SdesItem(SdesItem::Type type, size_t len, const char* value)
		{
			//MS_TRACE();

			// Allocate memory.
			this->raw.reset(new uint8_t[2 + len]);

			// Update the header pointer.
			this->header = reinterpret_cast<Header*>(this->raw.get());

			this->header->type   = type;
			this->header->length = len;

			// Copy the value into raw.
			std::memcpy(this->header->value, value, this->header->length);
		}

		void SdesItem::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<SdesItem>");
			DEBUG_LOG("  type   : %s", SdesItem::Type2String(this->GetType()).c_str());
			DEBUG_LOG("  length : %hhu" , this->header->length);
			DEBUG_LOG("  value  : %.*s", this->header->length, this->header->value);
			DEBUG_LOG("</SdesItem>");
		}

		size_t SdesItem::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			// Add minimum header.
			std::memcpy(buffer, this->header, 2);

			// Copy the content.
			std::memcpy(buffer + 2, this->header->value, this->header->length);

			return 2 + this->header->length;
		}

		/* Class methods. */

		SdesChunk* SdesChunk::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			// data size must be > SSRC field.
			if (len < 4u /* ssrc */)
			{
				WARNING_EX_LOG("rtcp, not enough space for SDES chunk, discarded");

				return nullptr;
			}

			uint32_t ssrc = rtc_byte::get4bytes(data, 0);

			std::unique_ptr<SdesChunk> chunk(new SdesChunk(ssrc));

			size_t offset = 4u /* ssrc */;

			while (len > offset)
			{
				SdesItem* item = SdesItem::Parse(data + offset, len - offset);

				if (!item)
					break;

				chunk->AddItem(item);
				offset += item->GetSize();
			}

			return chunk.release();
		}

		/* Instance methods. */

		size_t SdesChunk::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			// Copy the SSRC.
			rtc_byte::set4bytes(buffer, 0, this->ssrc);

			size_t offset{ 4u }; // ssrc.

			for (auto* item : this->items)
			{
				offset += item->Serialize(buffer + offset);
			}

			// 32 bits padding.
			size_t padding = (-offset) & 3;

			for (size_t i{ 0 }; i < padding; ++i)
			{
				buffer[offset + i] = 0;
			}

			return offset + padding;
		}

		void SdesChunk::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<SdesChunk>");
			DEBUG_LOG("  ssrc : %u"  , this->ssrc);
			for (auto* item : this->items)
			{
				item->Dump();
			}
			DEBUG_LOG("</SdesChunk>");
		}

		/* Class methods. */

		SdesPacket* SdesPacket::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			// Get the header.
			auto* header = const_cast<CommonHeader*>(reinterpret_cast<const CommonHeader*>(data));
			std::unique_ptr<SdesPacket> packet(new SdesPacket(header));
			size_t offset = sizeof(Packet::CommonHeader);
			uint8_t count = header->count;

			while ((count-- != 0u) && (len > offset))
			{
				SdesChunk* chunk = SdesChunk::Parse(data + offset, len - offset);

				if (chunk != nullptr)
				{
					packet->AddChunk(chunk);
					offset += chunk->GetSize();
				}
				else
				{
					return packet.release();
				}
			}

			return packet.release();
		}

		/* Instance methods. */

		size_t SdesPacket::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			size_t offset = Packet::Serialize(buffer);

			for (auto* chunk : this->chunks)
			{
				offset += chunk->Serialize(buffer + offset);
			}

			return offset;
		}

		void SdesPacket::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<SdesPacket>");
			for (auto* chunk : this->chunks)
			{
				chunk->Dump();
			}
			DEBUG_LOG("</SdesPacket>");
		}
	} // namespace RTCP
} // namespace RTC
