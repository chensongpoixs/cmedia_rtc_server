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
namespace chen {


	class crtc_transport : public cudp_socket::Listener
	{
	public:
		explicit crtc_transport()
		: m_local_sdp ()
		, m_remote_sdp ()
		, m_rtc_net_state(ERtcNetworkStateInit)
		, m_update_socket_ptr(NULL)
		 {}

		virtual ~crtc_transport();

	public:
		bool init(const crtc_sdp & remote_sdp, const crtc_sdp & local_sdp);

		void update(uint32 uDeltaTime);

		void destroy();

		
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
		cudp_socket		*				m_update_socket_ptr;


		//crtc_stun_packet				m_rtc_stun_packet;


		// 1000000LL * 30
	};


}

#endif // _C_RTC_TRANSPORT_H_