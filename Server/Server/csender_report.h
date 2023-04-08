/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_SENDER_REPORT_H_
#define _C_SENDER_REPORT_H_
#include "cnet_type.h"
#include "clog.h"
#include "cfunction_view.h"
#include "carray_view.h"
#include "buffer.h"
#include "crtcp_packet.h"
#include "cntp_time.h"
#include "creport_block.h"
#include <vector>

namespace chen {

	namespace rtcp {
		class  CommonHeader;
		class csender_report : public crtcp_packet
		{
		public:
			static const  uint8 kPacketType = 200;
			static const  size_t kMaxNumberOfReportBlocks = 0x1f;
		public:
			csender_report();
			csender_report(const csender_report&);
			csender_report(csender_report&&);
			csender_report& operator=(const csender_report&);
			csender_report& operator=(csender_report&&);
			~csender_report() override;

			// Parse assumes header is already parsed and validated.
			bool Parse(const CommonHeader& packet);

			void SetSenderSsrc(uint32_t ssrc) { sender_ssrc_ = ssrc; }
			void SetNtp(NtpTime ntp) { ntp_ = ntp; }
			void SetRtpTimestamp(uint32_t rtp_timestamp) {
				rtp_timestamp_ = rtp_timestamp;
			}
			void SetPacketCount(uint32_t packet_count) {
				sender_packet_count_ = packet_count;
			}
			void SetOctetCount(uint32_t octet_count) {
				sender_octet_count_ = octet_count;
			}
			bool AddReportBlock(const creport_block& block);
			bool SetReportBlocks(std::vector<creport_block> blocks);
			void ClearReportBlocks() { report_blocks_.clear(); }

			uint32_t sender_ssrc() const { return sender_ssrc_; }
			NtpTime ntp() const { return ntp_; }
			uint32_t rtp_timestamp() const { return rtp_timestamp_; }
			uint32_t sender_packet_count() const { return sender_packet_count_; }
			uint32_t sender_octet_count() const { return sender_octet_count_; }

			const std::vector<creport_block>& report_blocks() const {
				return report_blocks_;
			}

			size_t BlockLength() const override;

			bool Create(uint8_t* packet,
				size_t* index,
				size_t max_length,
				PacketReadyCallback callback) const override;

		private:
			static constexpr size_t kSenderBaseLength = 24;

			uint32_t sender_ssrc_;
			NtpTime ntp_;
			uint32_t rtp_timestamp_;
			uint32_t sender_packet_count_;
			uint32_t sender_octet_count_;
			std::vector<creport_block> report_blocks_;
		};
	}
}

#endif