/***********************************************************************************************
created: 		2023-04-08

author:			chensong

purpose:		ccfg
************************************************************************************************/

#include "crtp_headers.h"


namespace chen {
	RTPHeader::RTPHeader()
		: markerBit(false),
		payloadType(0),
		sequenceNumber(0),
		timestamp(0),
		ssrc(0),
		numCSRCs(0),
		arrOfCSRCs(),
		paddingLength(0),
		headerLength(0),
		payload_type_frequency(0)
		/*, extension()*/ {}

	RTPHeader::RTPHeader(const RTPHeader& other) = default;

	RTPHeader& RTPHeader::operator=(const RTPHeader& other) = default;
}