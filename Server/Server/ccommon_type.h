/***********************************************************************************************
created: 		2023-03-08

author:			chensong

purpose:		ctransport_util


************************************************************************************************/


#ifndef C_COMMON_TYPE_H_
#define C_COMMON_TYPE_H_
#include "cnet_type.h"
#include <stddef.h>  // For size_t
#include <cstdint>

namespace chen {

	// Statistics for RTCP packet types.
	struct RtcpPacketTypeCounter {
		RtcpPacketTypeCounter()
			: first_packet_time_ms(-1),
			nack_packets(0),
			fir_packets(0),
			pli_packets(0),
			nack_requests(0),
			unique_nack_requests(0) {}

		void Add(const RtcpPacketTypeCounter& other) {
			nack_packets += other.nack_packets;
			fir_packets += other.fir_packets;
			pli_packets += other.pli_packets;
			nack_requests += other.nack_requests;
			unique_nack_requests += other.unique_nack_requests;
			if (other.first_packet_time_ms != -1 &&
				(other.first_packet_time_ms < first_packet_time_ms ||
					first_packet_time_ms == -1)) {
				// Use oldest time.
				first_packet_time_ms = other.first_packet_time_ms;
			}
		}

		void Subtract(const RtcpPacketTypeCounter& other) {
			nack_packets -= other.nack_packets;
			fir_packets -= other.fir_packets;
			pli_packets -= other.pli_packets;
			nack_requests -= other.nack_requests;
			unique_nack_requests -= other.unique_nack_requests;
			if (other.first_packet_time_ms != -1 &&
				(other.first_packet_time_ms > first_packet_time_ms ||
					first_packet_time_ms == -1)) {
				// Use youngest time.
				first_packet_time_ms = other.first_packet_time_ms;
			}
		}

		int64_t TimeSinceFirstPacketInMs(int64_t now_ms) const {
			return (first_packet_time_ms == -1) ? -1 : (now_ms - first_packet_time_ms);
		}

		int UniqueNackRequestsInPercent() const {
			if (nack_requests == 0) {
				return 0;
			}
			return static_cast<int>((unique_nack_requests * 100.0f / nack_requests) +
				0.5f);
		}

		int64_t first_packet_time_ms;   // Time when first packet is sent/received.
		uint32_t nack_packets;          // Number of RTCP NACK packets.
		uint32_t fir_packets;           // Number of RTCP FIR packets.
		uint32_t pli_packets;           // Number of RTCP PLI packets.
		uint32_t nack_requests;         // Number of NACKed RTP packets.
		uint32_t unique_nack_requests;  // Number of unique NACKed RTP packets.
	};

}
#endif //