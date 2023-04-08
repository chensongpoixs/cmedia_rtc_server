//
///***********************************************************************************************
//created: 		2023-04-08
//
//author:			chensong
//
//purpose:		ccfg
//************************************************************************************************/
//#include "crtp_header_extensions.h"
//#include "cbyte_io.h"
//#include "clog.h"
//namespace chen {
//
//	/////////////////////////////////////////////////////////////////////////////////////
//	// Absolute send time in RTP streams.
//	//
//	// The absolute send time is signaled to the receiver in-band using the
//	// general mechanism for RTP header extensions [RFC8285]. The payload
//	// of this extension (the transmitted value) is a 24-bit unsigned integer
//	// containing the sender's current time in seconds as a fixed point number
//	// with 18 bits fractional part.
//	//
//	// The form of the absolute send time extension block:
//	//
//	//    0                   1                   2                   3
//	//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	//   |  ID   | len=2 |              absolute send time               |
//	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	bool AbsoluteSendTime::Parse(uint8 * data, size_t size, uint32 * time_24bits)
//	{
//		if (size != 3)
//		{
//			return false;
//		}
//		*time_24bits = ByteReader<uint32, 3>::ReadBigEndian(data );
//		return true; 
//	}
//	bool AbsoluteSendTime::Write(uint8 * data, size_t size, uint32 time_24bits)
//	{
//		RTC_DCHECK_EQ(size, 3);
//		RTC_DCHECK_LE(time_24bits, 0x00FFFFFF);
//		ByteWriter<uint32_t, 3>::WriteBigEndian(data, time_24bits);
//		return true; 
//	}
//
//	/////////////////////////////////////////////////////////////////////////////////
//	// An RTP Header Extension for Client-to-Mixer Audio Level Indication
//	//
//	// https://datatracker.ietf.org/doc/draft-lennox-avt-rtp-audio-level-exthdr/
//	//
//	// The form of the audio level extension block:
//	//
//	//    0                   1
//	//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
//	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	//   |  ID   | len=0 |V|   level     |
//	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	//
//
//	bool AudioLevel::Parse(uint8* data, size_t size, bool* voice_activity, uint8* audio_level)
//	{
//		if (size != 1)
//		{
//			return false;
//		}
//		*voice_activity = (data[0] & 0x80) != 0;
//		*audio_level = data[0] & 0x7F;
//		return true;
//	}
//
//	bool AudioLevel::Write(uint8 * data, size_t size, bool voice_activity, uint8 audio_level)
//	{
//		RTC_DCHECK_EQ(size, 1);
//		RTC_CHECK_LE(audio_level, 0x7f);
//		data[0] = (voice_activity ? 0x80 : 0x00) | audio_level;
//		return true;
//	}
//	///////////////////////////////////////////////////////////////////////////////
//	// From RFC 5450: Transmission Time Offsets in RTP Streams.
//	//
//	// The transmission time is signaled to the receiver in-band using the
//	// general mechanism for RTP header extensions [RFC8285]. The payload
//	// of this extension (the transmitted value) is a 24-bit signed integer.
//	// When added to the RTP timestamp of the packet, it represents the
//	// "effective" RTP transmission time of the packet, on the RTP
//	// timescale.
//	//
//	// The form of the transmission offset extension block:
//	//
//	//    0                   1                   2                   3
//	//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	//   |  ID   | len=2 |              transmission offset              |
//	//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//	bool TransmissionOffset::Parse(uint8 * data, size_t size, int32 * rtp_time)
//	{
//		if (size != 3)
//		{
//			return false;
//		}
//		*rtp_time = ByteReader<int32, 3>::ReadBigEndian(data );
//		return true;
//	}
//
//	bool TransmissionOffset::Write(uint8 * data, size_t size, int32  rtp_time)
//	{
//		RTC_DCHECK_EQ(size , 3);
//		RTC_DCHECK_LE(rtp_time, 0x00ffffff);
//		ByteWriter<int32 , 3>::WriteBigEndian(data , rtp_time);
//		return true;
//	}
//	/////////////////////////////////////////////////////////////////////////////////////////
//	// TransportSequenceNumber
//	//
//	//   0                   1                   2
//	//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
//	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	//  |  ID   | L=1   |transport-wide sequence number |
//	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//	bool TransportSequenceNumber::Parse(const uint8 * data, size_t size, uint16 * transport_sequence_number)
//	{
//		if (size != kValueSizeBytes)
//		{
//			return false;
//		}
//		*transport_sequence_number = ByteReader<uint16_t>::ReadBigEndian(data );
//		return true;
//		return false;
//	}
//
//	bool TransportSequenceNumber::Write( uint8 * data, size_t size, uint16 transport_sequence_number)
//	{
//		RTC_DCHECK_EQ(size , ValueSize(transport_sequence_number));
//		ByteWriter<uint16_t>::WriteBigEndian(data , transport_sequence_number);
//		return true;
//		return false;
//	}
//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	//const char TransportSequenceNumberV2::kUri[];
//	// TransportSequenceNumberV2
//	//
//	// In addition to the format used for TransportSequencNumber, V2 also supports
//	// the following packet format where two extra bytes are used to specify that
//	// the sender requests immediate feedback.
//	//   0                   1                   2                   3
//	//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	//  |  ID   | L=3   |transport-wide sequence number |T|  seq count  |
//	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	//  |seq count cont.|
//	//  +-+-+-+-+-+-+-+-+
//	//
//	// The bit |T| determines whether the feedback should include timing information
//	// or not and |seq_count| determines how many packets the feedback packet should
//	// cover including the current packet. If |seq_count| is zero no feedback is
//	// requested.
//	bool TransportSequenceNumberV2::Parse(const uint8 * data, size_t size, uint16_t * transport_sequence_number, FeedbackRequest * feedback_request, bool & feedback)
//	{
//		if (size != kValueSizeBytes &&
//			size != kValueSizeBytesWithoutFeedbackRequest)
//		{
//			return false;
//		}
//
//		*transport_sequence_number = ByteReader<uint16_t>::ReadBigEndian(data );
//
//		feedback = false;
//		if (size == kValueSizeBytes)
//		{
//			uint16  feedback_request_raw = ByteReader<uint16_t>::ReadBigEndian(data  + 2);
//			bool include_timestamps = (feedback_request_raw & kIncludeTimestampsBit) != 0;
//			uint16 sequence_count = feedback_request_raw & ~kIncludeTimestampsBit;
//
//			// If |sequence_count| is zero no feedback is requested.
//			if (sequence_count != 0)
//			{
//				*feedback_request = { include_timestamps, sequence_count };
//				feedback = true;
//			}
//		}
//		return true;
//		return false;
//	}
//
//	bool TransportSequenceNumberV2::Write(uint8 * data, size_t size, uint16_t transport_sequence_number, const FeedbackRequest & feedback_request, bool feedback)
//	{
//		RTC_DCHECK_EQ(size,
//			ValueSize(transport_sequence_number, feedback_request, feedback));
//
//		ByteWriter<uint16_t>::WriteBigEndian(data , transport_sequence_number);
//
//		if (feedback)
//		{
//			RTC_DCHECK_GE(feedback_request.m_sequence_count, 0);
//			RTC_DCHECK_LT(feedback_request.m_sequence_count, kIncludeTimestampsBit);
//			uint16  feedback_request_raw =
//				feedback_request.m_sequence_count |
//				(feedback_request.m_include_timestamps ? kIncludeTimestampsBit : 0);
//			ByteWriter<uint16_t>::WriteBigEndian(data  + 2, feedback_request_raw);
//		}
//		return true;
//	}
//
//}
//
//
