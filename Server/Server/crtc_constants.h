﻿/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_CONSTANTS_H_

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/

#ifndef _C_RTC_CONSTANTS_H_
#define _C_RTC_CONSTANTS_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "crtx_paylod_des.h"
#include "cred_payload.h"
#include "cvideo_payload.h"
#include "caudio_payload.h"
#include "cred_payload.h"
namespace chen {





	// The RTP packet max size, should never exceed this size.
	const int kRtpPacketSize = 1500;

	const int kRtpHeaderFixedSize = 12;
	const uint8_t kRtpMarker = 0x80;

	// H.264 nalu header type mask.
	const uint8_t kNalTypeMask = 0x1F;

	// @see: https://tools.ietf.org/html/rfc6184#section-5.2
	const uint8_t kStapA = 24;

	// @see: https://tools.ietf.org/html/rfc6184#section-5.2
	const uint8_t kFuA = 28;

	// @see: https://tools.ietf.org/html/rfc6184#section-5.8
	const uint8_t kStart = 0x80; // Fu-header start bit
	const uint8_t kEnd = 0x40; // Fu-header end bit
	//////////////////




	// Firefox defaults as 109, Chrome is 111.
	const int kAudioPayloadType = 101;
	const int kAudioChannel = 2;
	const int kAudioSamplerate = 48000;

	// Firefox defaults as 126, Chrome is 102.
	const int kVideoPayloadType = 108;
	const int kVideoSamplerate = 90000;


	const int kRtxVideoPayloadType = 109;

	// The RTP payload max size, reserved some paddings for SRTP as such:
	//      kRtpPacketSize = kRtpMaxPayloadSize + paddings
	// For example, if kRtpPacketSize is 1500, recommend to set kRtpMaxPayloadSize to 1400,
	// which reserves 100 bytes for SRTP or paddings.
	// otherwise, the kRtpPacketSize must less than MTU, in webrtc source code, 
	// the rtp max size is assigned by kVideoMtu = 1200.
	// so we set kRtpMaxPayloadSize = 1200.
	// see @doc https://groups.google.com/g/discuss-webrtc/c/gH5ysR3SoZI
	const int kRtpMaxPayloadSize = kRtpPacketSize - 300;

}

#endif // _C_RTC_CONSTANTS_H_