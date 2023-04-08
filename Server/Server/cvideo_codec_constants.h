/***********************************************************************************************
created: 		2023-03-08

author:			chensong

purpose:		ctransport_util


************************************************************************************************/


#ifndef C_VIDEO_CODEC_CONSTANTS_H_
#define C_VIDEO_CODEC_CONSTANTS_H_
#include "cnet_type.h"
namespace chen {

enum : int32 { kMaxSimulcastStreams = 4 };
enum : int32 { kMaxSpatialLayers = 5 };
enum : int32 { kMaxTemporalStreams = 4 };

}  // namespace chen

#endif  // C_VIDEO_CODEC_CONSTANTS_H_
