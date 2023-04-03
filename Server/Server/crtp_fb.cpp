/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/
#include "crtp_fb.h"
#include "cbyte_io.h"
// RFC 4585, Section 6.1: Feedback format.
//
// Common packet format:
//
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |V=2|P|   FMT   |       PT      |          length               |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// 0 |                  SSRC of packet sender                        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// 4 |                  SSRC of media source                         |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   :            Feedback Control Information (FCI)                 :
//   :                                                               :

namespace chen {
	void crtp_fb::ParseCommonFeedback(const uint8 * payload)
	{
		// 发送的ssrc源
		sender_ssrc_ = ByteReader<uint32>::ReadBigEndian(&payload[0]);
		// media ssrc 源
		media_ssrc_ = ByteReader<uint32>::ReadBigEndian(&payload[4]);
	}
	void crtp_fb::CreateCommonFeedback(uint8 * payload) const
	{
		ByteWriter<uint32>::WriteBigEndian(&payload[0], sender_ssrc_);
		ByteWriter<uint32>::WriteBigEndian(&payload[4], media_ssrc_);
	}
}