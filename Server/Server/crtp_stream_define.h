/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/

#ifndef _C_RTP_STREAM_DEFINE_H_
#define _C_RTP_STREAM_DEFINE_H_

#include "cnet_type.h"
namespace chen {

	static const  uint16  MaxDropout{ 3000 };
	static const  uint16  MaxMisorder{ 1500 };
	static const  uint32  RtpSeqMod{ 1 << 16 };
	static const  size_t ScoreHistogramLength{ 24 };
}

#endif //_C_RTP_STREAM_DEFINE_H_