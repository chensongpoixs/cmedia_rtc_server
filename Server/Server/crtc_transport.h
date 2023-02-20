/***********************************************************************************************
created: 		2023-02-03

author:			chensong

purpose:		crtc_transport


************************************************************************************************/

#ifndef _C_RTC_TRANSPORT_H_
#define _C_RTC_TRANSPORT_H_
#include "cnet_type.h"
#include "crtc_sdp.h"
#include "crtc_source_description.h"
#include "crtc_transport_define.h"
#include "cudp_socket.h"
#include "crtc_stun_packet.h"
#include "crtc_dtls.h"
namespace chen {

	class cdtls_session;

	class crtc_transportlinster
	{
	public:

		virtual int32 write_dtls_data(void* data, int32 size) = 0;
		virtual int32 on_dtls_handshake_done() = 0;
		virtual void  on_dtls_application_data(const uint8* data, int32 size) = 0;
		virtual void  on_dtls_transport_connected(ECRYPTO_SUITE srtp_crypto_suite, uint8* srtp_local_key, size_t srtp_local_key_len, uint8* srtp_remote_key, size_t srtp_remote_key_len) = 0;
	};

	class crtc_transport : public cudp_socket::Listener, public crtc_transportlinster
	{
	public:
		explicit crtc_transport()
		: m_local_sdp ()
		, m_remote_sdp ()
		, m_rtc_net_state(ERtcNetworkStateInit)
		, m_udp_sockets()
		, m_current_socket_ptr(NULL)
		, m_dtls_ptr(NULL)
		, m_srtp_send_session_ptr(NULL)
		, m_srtp_recv_session_ptr(NULL)
			//, m_srtp()
		 {}

		virtual ~crtc_transport();

	public:
		bool init(const crtc_sdp & remote_sdp, const crtc_sdp & local_sdp);

		void update(uint32 uDeltaTime);

		void destroy();

	public:
		// virtual
		virtual int32 write_dtls_data(void* data, int size);
		virtual int32 on_dtls_handshake_done();
		virtual void  on_dtls_application_data(const uint8* data, int32 size);
		virtual void  on_dtls_transport_connected(ECRYPTO_SUITE srtp_crypto_suite, uint8* srtp_local_key, size_t srtp_local_key_len, uint8* srtp_remote_key, size_t srtp_remote_key_len);
	public:
		void set_state_as_waiting_stun() { m_rtc_net_state = ERtcNetworkStateWaitingStun; };
	protected:
	public:

		void OnPacketReceived(cudp_socket* socket, const uint8_t* data, size_t len, const sockaddr * remoteAddr);
	public:
		virtual void OnUdpSocketPacketReceived(
			cudp_socket* socket, const uint8_t* data, size_t len, const struct sockaddr* remoteAddr);
	public:

	private:

		void _on_stun_data_received(cudp_socket* socket, const uint8_t* data, size_t len, const sockaddr * remoteAddr);
		void _on_dtls_data_received(cudp_socket* socket, const uint8_t* data, size_t len, const sockaddr * remoteAddr);
	
		void _on_rtp_data_received(cudp_socket* socket, const uint8* data, size_t len, const sockaddr*remoteAddr );
		void _on_rtcp_data_received(cudp_socket* socket, const uint8* data, size_t len, const sockaddr*remoteAddr );
		// data channel 
		//void _on_application_data_receviced(cudp_socket* socket, const uint8* data, size_t len, const sockaddr*remoteAddr);

	private:
		// publish -> remote sdp 
		/*bool _negotiate_publish_capability(crtc_source_description * stream_desc);
		bool _generate_publish_local_sdp(  crtc_sdp& local_sdp, crtc_source_description* stream_desc, bool unified_plan, bool audio_before_video);

		bool _generate_publish_local_sdp_for_audio(crtc_sdp& local_sdp, crtc_source_description* stream_desc);
		bool _generate_publish_local_sdp_for_video(crtc_sdp& local_sdp, crtc_source_description* stream_desc, bool unified_plan);
*/
		
	private:
		crtc_sdp			 			m_local_sdp ;
		crtc_sdp			 			m_remote_sdp ;

		ERtcNetworkState				m_rtc_net_state;

		// ice server

		// 
		
		std::vector<cudp_socket*>		m_udp_sockets;
		
		sockaddr 						m_remote_addr;
		cudp_socket		*				m_current_socket_ptr;
		//crtc_stun_packet				m_rtc_stun_packet;
		//cdtls_session *					m_dtls_ptr;
		crtc_dtls	*					m_dtls_ptr;
		//csrtp							m_srtp;
		csrtp_session *					m_srtp_send_session_ptr;
		csrtp_session*					m_srtp_recv_session_ptr;
		// 1000000LL * 30
	};


}

#endif // _C_RTC_TRANSPORT_H_