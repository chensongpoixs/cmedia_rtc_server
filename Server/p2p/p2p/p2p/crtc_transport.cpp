/***********************************************************************************************
created: 		2022-11-10

author:			chensong

purpose:		crtc_player
************************************************************************************************/

#include "crtc_transport.h"
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/create_peerconnection_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include "crtc_player.h"
namespace chen {

	class DummySetSessionDescriptionObserver
		: public webrtc::SetSessionDescriptionObserver {
	public:
		static DummySetSessionDescriptionObserver* Create() {
			return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
		}
		virtual void OnSuccess() { RTC_LOG(LS_INFO) << __FUNCTION__; }
		virtual void OnFailure(webrtc::RTCError error) {
			RTC_LOG(LS_INFO) << __FUNCTION__ << " " << ToString(error.type()) << ": "
				<< error.message();
		}
	};
	crtc_transport::crtc_transport()
		 
	{
	}

	bool crtc_transport::init(crtc_player * ptr)
	{
		m_rtc_player = ptr;
		m_networkThread = rtc::Thread::CreateWithSocketServer();
		m_signalingThread = rtc::Thread::Create();
		m_workerThread = rtc::Thread::Create();
		 

		m_networkThread->SetName("network_thread", nullptr);
		m_signalingThread->SetName("signaling_thread", nullptr);
		m_workerThread->SetName("worker_thread", nullptr);

		if (!m_workerThread->Start() || !m_signalingThread->Start() || !m_networkThread->Start())
		{
			ERROR_EX_LOG(" webrtc network signalig  worker start failed !!!");
			return false;
		}

		m_peer_connection_factory = webrtc::CreatePeerConnectionFactory(
			m_networkThread.get(),
			m_workerThread.get(),
			m_signalingThread.get(),
			nullptr /*default_adm*/,
			webrtc::CreateBuiltinAudioEncoderFactory(),
			webrtc::CreateBuiltinAudioDecoderFactory(),
			webrtc::CreateBuiltinVideoEncoderFactory(),
			// CreateBuiltinExternalVideoEncoderFactory() ,
			webrtc::CreateBuiltinVideoDecoderFactory(),
			nullptr /*audio_mixer*/,
			nullptr /*audio_processing*/);

		webrtc::PeerConnectionInterface::RTCConfiguration config;
		//config.enable_dtls_srtp = true; //是否加密
		// Set SDP semantics to Unified Plan.
		config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
		m_peer_connection = m_peer_connection_factory->CreatePeerConnection(config, nullptr, nullptr, this);
		webrtc::RtpTransceiverInit transceiverInit;
		transceiverInit.direction = webrtc::RtpTransceiverDirection::kRecvOnly;
		m_peer_connection->AddTransceiver(cricket::MediaType::MEDIA_TYPE_AUDIO, transceiverInit);
		m_peer_connection->AddTransceiver(cricket::MediaType::MEDIA_TYPE_VIDEO, transceiverInit);
		m_peer_connection->AddTransceiver(cricket::MediaType::MEDIA_TYPE_DATA, transceiverInit);
		 
		m_peer_connection->CreateOffer(
			this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
		return true;
	}

	void crtc_transport::destroy()
	{
		SYSTEM_LOG("recv transport stop !!!");
		//m_data_consumsers.clear();
		if (m_peer_connection_factory)
		{
			m_peer_connection_factory->StopAecDump();
		}
		m_peer_connection_factory = nullptr;
		m_peer_connection = nullptr;
		SYSTEM_LOG("recv factory ok null !!!");
	}

	bool crtc_transport::set_remote_description(const std::string & sdp)
	{
		if (!m_peer_connection)
		{
			WARNING_EX_LOG("");
			return false;
		}
		std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
			webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdp);
		m_peer_connection->SetRemoteDescription(
			DummySetSessionDescriptionObserver::Create(),
			session_description.release());
		return true;
	}

	bool crtc_transport::set_remoter_iceCandidate(const std::string& mid, uint32 sdpindex, std::string candidate)
	{
		/*if (!peer_connection_)
		{
			create_offer();
		}*/
		//std::string sdp;
		//std::string sdp_mid;
		//int sdp_mlineindex = 0;
		//if (!rtc::GetStringFromJsonObject(candidate, "sdpMid", &sdp_mid) ||
		//	!rtc::GetIntFromJsonObject(candidate, "sdpMLineIndex",
		//		&sdp_mlineindex) ||
		//	!rtc::GetStringFromJsonObject(candidate, "candidate", &sdp)) {
		//	//RTC_LOG(WARNING) << "Can't parse received message.";
		//	WARNING_EX_LOG("Can't parse received message.");
		//	return;
		//}
		webrtc::SdpParseError error;
		std::unique_ptr<webrtc::IceCandidateInterface> candidate_ptr(webrtc::CreateIceCandidate(mid, sdpindex, candidate, &error));
		if (!candidate_ptr) {
			/*RTC_LOG(WARNING) << "Can't parse received candidate message. "
				<< "SdpParseError was: " << error.description;*/
			WARNING_EX_LOG("Can't parse received candidate message. SdpParseError was: %s", error.description.c_str());
			return false;
		}
		if (!m_peer_connection->AddIceCandidate(candidate_ptr.get())) {
			//RTC_LOG(WARNING) << "Failed to apply the received candidate";
			WARNING_EX_LOG("Failed to apply the received candidate");
			return false;
		}
		NORMAL_EX_LOG("ok ");
		return true;
	}

	//
	// PeerConnectionObserver implementation.
	//



	void crtc_transport::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
	{
		NORMAL_EX_LOG("OnSignalingChange new_state = %d", new_state);
	/*	if (m_client_ptr && new_state == webrtc::PeerConnectionInterface::kClosed)
		{
			m_client_ptr->webrtc_connect_failed_callback();
		}*/
	}

	// 好家伙  webrtc封装太好 ^_^  接口定义 PeerConnectionObserver
	void crtc_transport::OnAddTrack(
		rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
		const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
		streams)
	{
		NORMAL_EX_LOG("OnAddTrack");
		auto* track = reinterpret_cast<webrtc::MediaStreamTrackInterface*>(receiver->track().release());
		if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind) 
		{
			webrtc::VideoTrackInterface* video_track = static_cast<webrtc::VideoTrackInterface*>(track);
			// 好家伙  终于找到你 哈哈 ^_^
			if (video_track)
			{
				m_remote_renderer.reset(new  cvideo_render(1, 1, video_track));
			}
			
			//main_wnd_->StartRemoteRenderer(video_track);
		}
		track->Release();
		//auto* track = reinterpret_cast<webrtc::MediaStreamTrackInterface*>(receiver);
		//m_remote_renderer->reset( D3dRenderer::Create("Rtc_Player", 1, 1, receiver->track().release()))
	}
	void crtc_transport::OnRemoveTrack(
		rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
	{
		NORMAL_EX_LOG("OnRemoveTrack");
		// Remote peer stopped sending a track.
		auto* track = reinterpret_cast<webrtc::MediaStreamTrackInterface*>(receiver->track().release());
		track->Release();
	}


	void crtc_transport::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
	{
		NORMAL_EX_LOG("OnSignalingChange new_state = %d", new_state);
		/*if (m_client_ptr && new_state == webrtc::PeerConnectionInterface::kClosed)
		{
			m_client_ptr->webrtc_connect_failed_callback();
		}*/
	}

	void crtc_transport::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
	{
		NORMAL_EX_LOG("OnIceCandidate");
		std::string sdp;
		if (!candidate->ToString(&sdp)) {
			//RTC_LOG(LS_ERROR) << "Failed to serialize candidate";
			ERROR_EX_LOG("Failed to serialize candidate");
			return;
		}
		NORMAL_EX_LOG("ice candidate : %s", sdp.c_str());
		//return;
		 if (m_rtc_player)
		{
			m_rtc_player->send_ice_candidate(0, candidate->sdp_mid(), candidate->sdp_mline_index(), sdp);
		}
		//m_callback_ptr->send_candidate(0, candidate->sdp_mid(), candidate->sdp_mline_index(), sdp);
	}


	// CreateSessionDescriptionObserver implementation.
	void crtc_transport::OnSuccess(webrtc::SessionDescriptionInterface* desc)
	{
		std::string sdp;

		desc->ToString(&sdp);
		m_peer_connection->SetLocalDescription(DummySetSessionDescriptionObserver::Create(), desc);
		NORMAL_EX_LOG("offer sdp = %s", sdp.c_str());
		if (m_rtc_player)
		{
			m_rtc_player->send_offer(sdp);
		}
		//m_offer = sdp;
		//nlohmann::json localsdpobject =  sdptransform::parse(sdp);
		//m_client_ptr->transportofferasner(m_send, true);
	}
	void crtc_transport::OnFailure(webrtc::RTCError error)
	{
		/*if (m_client_ptr)
		{
			m_client_ptr->transportofferasner(false, false);
		}*/

	}
}