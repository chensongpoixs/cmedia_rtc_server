/***********************************************************************************************
created: 		2023-01-17

author:			chensong

purpose:		_C_DTLS_ _H_


************************************************************************************************/

#ifndef _C_DTLS_H_
#define _C_DTLS_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "cdtls_session.h"
namespace chen {

	class cdtls 
	{
	public:

		cdtls()
			:m_dtls_session_ptr(NULL)
		{}
	 
		~cdtls();
		
	public:
		//static bool init();


	//	static void destory();
	public:

		bool init(const std::string  & role);

		void destroy();
	public:
		// As DTLS client, start handshake actively, send the ClientHello packet.
		int32 start_active_handshake();
		// When got DTLS packet, may handshake packets or application data.
		// @remark When we are passive(DTLS server), we start handshake when got DTLS packet.
		int32 on_dtls(char* data, int32 nb_data);
	public:
		int32 get_srtp_key(std::string& recv_key, std::string& send_key);
	protected:
	private:

		cdtls_session    *     m_dtls_session_ptr;
		
	};
}


#endif // _C_DTLS_IMPL_H_