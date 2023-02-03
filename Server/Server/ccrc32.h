/***********************************************************************************************
created: 		2023-02-02

author:			chensong

purpose:		 buffer
************************************************************************************************/


#ifndef _C_CRC32_H_
#define _C_CRC32_H_
#include "cnet_type.h"

namespace chen {
	// Calc the crc32 of bytes in buf by IEEE, for zip.
	uint32  srs_crc32_ieee(const void* buf, int32 size, uint32  previous = 0);
}

#endif // _C_CRC32_H_