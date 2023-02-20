/***********************************************************************************************
created: 		2023-02-20

author:			chensong

purpose:		_C_DTLS_ _H_


************************************************************************************************/

#ifndef _C_ENCODER_H_
#define _C_ENCODER_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>

namespace chen {
	class cbuffer;
	// Encoder.
	class icencoder
	{
	public:
		explicit icencoder();
		virtual ~icencoder();
	public:
		/**
		 * get the number of bytes to code to.
		 */
		virtual uint64_t nb_bytes() = 0;
		/**
		 * encode object to bytes in SrsBuffer.
		 */
		virtual bool encode(cbuffer* buf) = 0;
	};



	// TODO: FIXME: protocol, amf0, json should implements it.
	class iccodec : public icencoder
	{
	public:
		explicit iccodec();
		virtual ~iccodec();
	public:
		// Get the number of bytes to code to.
		virtual uint64_t nb_bytes() = 0;
		// Encode object to bytes in SrsBuffer.
		virtual bool encode(cbuffer* buf) = 0;
	public:
		// Decode object from bytes in SrsBuffer.
		virtual bool decode(cbuffer* buf) = 0;
	};


}

#endif // _C_ENCODER_H_