/***********************************************************************************************
created: 		2022-09-19

author:			peer_connection

purpose:		log
************************************************************************************************/
#ifndef _C_PEER_CONNECTION_H_
#define _C_PEER_CONNECTION_H_
#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
//#include "cclient.h"
namespace chen {
	class cclient;
	class cpeer_connection :public webrtc::PeerConnectionObserver /*好玩东西给webrtc封装这个里面   */,
		public webrtc::CreateSessionDescriptionObserver/*,
		public PeerConnectionClientObserver*/
	{
	public:
		cpeer_connection(cclient * ptr)
			: m_client_ptr(ptr)
			, peer_connection_factory_(NULL)
			, peer_connection_(NULL)
		{

		}
		//~cpeer_connection();

		bool init();
		void update();
		void destroy();


		bool create_answer( const std::string & sdp);
	public:
		//
		// PeerConnectionObserver implementation.
		//

		void OnSignalingChange(
			webrtc::PeerConnectionInterface::SignalingState new_state) override {}


		// 好家伙  webrtc封装太好 ^_^  接口定义 PeerConnectionObserver
		void OnAddTrack(
			rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
			const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
			streams) override;
		void OnRemoveTrack(
			rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;
		void OnDataChannel(
			rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {}
		void OnRenegotiationNeeded() override {}
		void OnIceConnectionChange(
			webrtc::PeerConnectionInterface::IceConnectionState new_state) override {}
		void OnIceGatheringChange(
			webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}
		void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
		void OnIceConnectionReceivingChange(bool receiving) override {}



		//  // CreateSessionDescriptionObserver implementation.
		void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
		void OnFailure(webrtc::RTCError error) override;
	private:


		bool _add_track();
	private:
		cclient						*									m_client_ptr;
		rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>		peer_connection_factory_;
		rtc::scoped_refptr<webrtc::PeerConnectionInterface>				peer_connection_;
		
	};
}

#endif // _C_PEER_CONNECTION_H_

