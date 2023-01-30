/***********************************************************************************************
created: 		2023-01-30

author:			chensong

purpose:		_C_DTLS_CERTIFICATE_H_


************************************************************************************************/

#ifndef _C_DTLS_SESSION_H_
#define _C_DTLS_SESSION_H_
#include "cnet_type.h"
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cdtls_define.h"
#include "cmedia_desc.h"
namespace chen {



	class cdtls_session
	{
	public:
		//************************************
		// Method:    cdtls_session
		// FullName:  chen::cdtls_session::cdtls_session
		// Access:    public 
		// Returns:   
		// Qualifier:
		//************************************
		explicit cdtls_session() 
			: m_dtls_ctx_ptr(NULL)
			, m_dtls_ptr(NULL)
			, m_bio_in_ptr(NULL)
			, m_bio_out_ptr(NULL)
			, m_handshake_done_for_us(false)
			, m_nn_arq_packets(0) {}
		//************************************
		// Method:    ~cdtls_session
		// FullName:  chen::cdtls_session::~cdtls_session
		// Access:    virtual public 
		// Returns:   
		// Qualifier:
		//************************************
		virtual ~cdtls_session();


	public:
		//************************************
		// Method:    init
		// FullName:  chen::cdtls::init
		// Access:    public static 
		// Returns:   chen::int32
		// Qualifier:
		//************************************
		virtual bool init();


		//************************************
		// Method:    destroy
		// FullName:  chen::cdtls_session::destroy
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		//************************************
		virtual void destroy();

		//************************************
		// Method:    start_active_handshake
		// FullName:  chen::cdtls::start_active_handshake
		// Access:    virtual public 
		// Returns:   chen::int32
		// Qualifier:
		//************************************
		virtual int32 start_active_handshake() = 0;
		//************************************
		// Method:    should_reset_timer
		// FullName:  chen::cdtls::should_reset_timer
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		//************************************
		virtual bool should_reset_timer() = 0;
		//************************************
		// Method:    on_dtls
		// FullName:  chen::cdtls::on_dtls
		// Access:    virtual public 
		// Returns:   chen::int32
		// Qualifier:
		// Parameter: char * data
		// Parameter: int32 nb_data
		//************************************
		virtual int32 on_dtls(char* data, int32 nb_data);
	protected:
		// callback 回调用应用层的api 就是传输transport中的api
		int32 do_on_dtls(char* data, int32 nb_data);
		int32 do_handshake();
		void state_trace(uint8* data, int32 length, bool incoming, int32 r0, int32 r1, bool arq);
	public:
		int32 get_srtp_key(std::string& recv_key, std::string& send_key);
		// global function  -------- ???
		void callback_by_ssl(std::string type, std::string desc);
	protected:
		virtual int32 on_final_out_data(uint8* data, int32 size) = 0;
		virtual int32 on_handshake_done() = 0;
		virtual bool is_dtls_client() = 0;

	protected:


		SSL_CTX*		m_dtls_ctx_ptr;
		SSL*			m_dtls_ptr;
		BIO*			m_bio_in_ptr;
		BIO*			m_bio_out_ptr;
		// Whether the handshake is done, for us only.
	// @remark For us only, means peer maybe not done, we also need to handle the DTLS packet.
		bool			m_handshake_done_for_us;

		// The stat for ARQ packets.
		int32			m_nn_arq_packets;
	private:
	};






	class cdtls_server : public cdtls_session
	{
	public:

		 
		explicit cdtls_server()
			: cdtls_session() {}

		 
		virtual ~cdtls_server();
	protected:
	private:
	};
	class cdtls_client : public cdtls_session
	{
	public:

		 
		explicit cdtls_client()
			: cdtls_session()
		, m_state(EDtlsStateInit)
		, m_arq_max_retry(12* 2)   // the max dtls retry num is 12 in openssl.// Max ARQ limit shared for ClientHello and Certificate.
		, m_reset_timer(true){}

		 
		virtual ~cdtls_client();
	public:
	public:
		virtual bool init();
		virtual void update(uint32 uDeltaTime); 
		virtual void destroy();
		virtual int32 start_active_handshake();
		virtual bool should_reset_timer();
	protected:
		virtual int32 on_final_out_data(uint8* data, int size);
		virtual int32 on_handshake_done();
		virtual bool is_dtls_client();
	private:
		int32 start_arq();
		void stop_arq();
	protected:
	private:
		// The DTLS-client state to drive the ARQ thread.
		EDtlsState				m_state;
		// The max ARQ retry.
		int32					m_arq_max_retry;
		// Should we reset the timer?
		// It's true when init, or in state ServerHello.
		bool					m_reset_timer ;
	};

}

#endif // _C_DTLS_SESSION_H_