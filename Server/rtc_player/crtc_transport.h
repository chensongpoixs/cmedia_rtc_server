/***********************************************************************************************
created: 		2022-11-10

author:			chensong

purpose:		crtc_player
************************************************************************************************/
#ifndef _C_RTC_TRANSPORT_H_
#define _C_RTC_TRANSPORT_H_
#include "api/create_peerconnection_factory.h"
#include "clog.h"
#include "cd3d_renderer.h"
namespace chen {
	class crtc_player;
	class crtc_transport : public webrtc::PeerConnectionObserver  , public webrtc::CreateSessionDescriptionObserver
	{

	public:
		crtc_transport();

		bool init(crtc_player * ptr);


		void destroy();


		bool set_remote_description(const std::string &sdp);
	public:

		//
		// PeerConnectionObserver implementation.
		//

		void OnSignalingChange(
			webrtc::PeerConnectionInterface::SignalingState new_state) override;



		// 好家伙  webrtc封装太好 ^_^  接口定义 PeerConnectionObserver
		void OnAddTrack(
			rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
			const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
			streams) override;
		void OnRemoveTrack(
			rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;
		void OnDataChannel(
			rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override
		{
			NORMAL_EX_LOG("OnDataChannel");
		}
		void OnRenegotiationNeeded() override
		{
			NORMAL_EX_LOG("OnRenegotiationNeeded");
		}
		void OnIceConnectionChange(
			webrtc::PeerConnectionInterface::IceConnectionState new_state) override;

		void OnIceGatheringChange(
			webrtc::PeerConnectionInterface::IceGatheringState new_state) override
		{
			NORMAL_EX_LOG("OnIceGatheringChange");
		}
		void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
		void OnIceConnectionReceivingChange(bool receiving) override
		{
			NORMAL_EX_LOG("OnIceConnectionReceivingChange");
		}
	public:
		// CreateSessionDescriptionObserver implementation.
		void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
		void OnFailure(webrtc::RTCError error) override;
	protected:
	private:
		rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_peer_connection;
		rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_peer_connection_factory;
		std::unique_ptr<rtc::Thread> m_networkThread;
		std::unique_ptr<rtc::Thread> m_signalingThread;
		std::unique_ptr<rtc::Thread> m_workerThread;
		crtc_player *				 m_rtc_player;
		std::unique_ptr<cvideo_render> m_remote_renderer;
	};
}

#endif // _C_RTC_TRANSPORT_H_