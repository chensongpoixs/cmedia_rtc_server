#define MS_CLASS "RTC::RTCP::ReceiverReport"
// #define MS_LOG_DEV_LEVEL 3

#include "RTC/RTCP/ReceiverReport.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include <cstring>

namespace RTC
{
	namespace RTCP
	{
		/* Class methods. */

		ReceiverReport* ReceiverReport::Parse(const uint8_t* data, size_t len)
		{
			MS_TRACE();

			// Get the header.
			auto* header = const_cast<Header*>(reinterpret_cast<const Header*>(data));

			// Packet size must be >= header size.
			if (len < sizeof(Header))
			{
				MS_WARN_TAG(rtcp, "not enough space for receiver report, packet discarded");

				return nullptr;
			}

			return new ReceiverReport(header);
		}

		/* Instance methods. */

		void ReceiverReport::Dump() const
		{
			MS_TRACE();

			MS_DUMP("<ReceiverReport>");
			MS_DUMP("  ssrc          : %" PRIu32, GetSsrc());
			MS_DUMP("  fraction lost : %" PRIu8, GetFractionLost());
			MS_DUMP("  total lost    : %" PRIu32, GetTotalLost());
			MS_DUMP("  last seq      : %" PRIu32, GetLastSeq());
			MS_DUMP("  jitter        : %" PRIu32, GetJitter());
			MS_DUMP("  lsr           : %" PRIu32, GetLastSenderReport());
			MS_DUMP("  dlsr          : %" PRIu32, GetDelaySinceLastSenderReport());
			MS_DUMP("</ReceiverReport>");
		}

		size_t ReceiverReport::Serialize(uint8_t* buffer)
		{
			MS_TRACE();

			// Copy the header.
			std::memcpy(buffer, this->header, sizeof(Header));

			return sizeof(Header);
		}

		/* Class methods. */

		/**
		 * ReceiverReportPacket::Parse()
		 * @param  data   - Points to the begining of the incoming RTCP packet.
		 * @param  len    - Total length of the packet.
		 * @param  offset - points to the first Receiver Report in the incoming packet.
		 */
		ReceiverReportPacket* ReceiverReportPacket::Parse(const uint8_t* data, size_t len, size_t offset)
		{
			MS_TRACE();
			// RTCP receiver report (RFC 3550).			//			//   0                   1                   2                   3			//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1			//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+			//  |V=2|P|    RC   |   PT=RR=201   |             length            |			//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+			//  |                     SSRC of packet sender                     |			//  +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+			//  |                         report block(s)                       |			//  |                            ....                               |
			// Get the header.
			auto* header = const_cast<CommonHeader*>(reinterpret_cast<const CommonHeader*>(data));

			// Ensure there is space for the common header and the SSRC of packet sender.
			if (len < sizeof(CommonHeader) + 4u /* ssrc */)
			{
				MS_WARN_TAG(rtcp, "not enough space for receiver report packet, packet discarded");

				return nullptr;
			}

			std::unique_ptr<ReceiverReportPacket> packet(new ReceiverReportPacket(header));

			uint32_t ssrc =
			  Utils::Byte::Get4Bytes(reinterpret_cast<uint8_t*>(header), sizeof(CommonHeader));

			packet->SetSsrc(ssrc);

			if (offset == 0)
			{
				offset = sizeof(Packet::CommonHeader) + 4u /* ssrc */;
			}
			// RC ===> report block����
			uint8_t count = header->count;

			while ((count-- != 0u) && (len > offset))
			{
//<<<<<<< HEAD
				// RR�ķ�����Ϣ
//=======
				// �����ж��Դ count��Դ�ĸ������� ��Ϣ����Ҫ�ռ��Ĺ�
//>>>>>>> 20adb84c0b9b03c2ea608d143e974cef7a4a3e62
				ReceiverReport* report = ReceiverReport::Parse(data + offset, len - offset);

				if (report != nullptr)
				{
					packet->AddReport(report);
					offset += report->GetSize();
				}
				else
				{
					return packet.release();
				}
			}

			return packet.release();
		}

		/* Instance methods. */

		size_t ReceiverReportPacket::Serialize(uint8_t* buffer)
		{
			MS_TRACE();

			size_t offset = Packet::Serialize(buffer);

			// Copy the SSRC.
			Utils::Byte::Set4Bytes(buffer, sizeof(Packet::CommonHeader), this->ssrc);
			offset += 4u;

			// Serialize reports.
			for (auto* report : this->reports)
			{
				offset += report->Serialize(buffer + offset);
			}

			return offset;
		}

		void ReceiverReportPacket::Dump() const
		{
			MS_TRACE();

			MS_DUMP("<ReceiverReportPacket>");
			MS_DUMP("  ssrc: %" PRIu32, this->ssrc);
			for (auto* report : this->reports)
			{
				report->Dump();
			}
			MS_DUMP("</ReceiverReportPacket>");
		}
	} // namespace RTCP
} // namespace RTC
