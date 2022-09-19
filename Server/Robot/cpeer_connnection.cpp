/***********************************************************************************************
created: 		2022-08-24

author:			chensong

purpose:		log
************************************************************************************************/
#include "cpeer_connnection.h"
#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "api/audio/audio_mixer.h"
#include "api/audio_codecs/audio_decoder_factory.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_options.h"
#include "api/create_peerconnection_factory.h"
#include "api/rtp_sender_interface.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"
#include "p2p/base/port_allocator.h"
#include "pc/video_track_source.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/ref_counted_object.h"
#include "rtc_base/rtc_certificate_generator.h"
//#include "rtc_base/strings/json.h"
#include "clog.h"
#include "cclient.h"
#include "ccapturer_tracksource.h"
namespace chen {


	static const char * RTC_AUDIO_LABEL = "audio";
	static const char * RTC_VIDEO_LABEL = "video";
	static const char * RTC_STREAM = "rtc_chen_stream";


	class DummySetSessionDescriptionObserver
		: public webrtc::SetSessionDescriptionObserver {
	public:
		static DummySetSessionDescriptionObserver* Create() {
			return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
		}
		virtual void OnSuccess() { RTC_LOG(INFO) << __FUNCTION__; }
		virtual void OnFailure(webrtc::RTCError error) {
			RTC_LOG(INFO) << __FUNCTION__ << " " << ToString(error.type()) << ": "
				<< error.message();
		}
	};

	/*cpeer_connection::cpeer_connection()
		: peer_connection_factory_(NULL)
		, peer_connection_(NULL)
	{
	}
	cpeer_connection::~cpeer_connection()
	{
	}
*/
	bool cpeer_connection::init()
	{
		 auto networkThread = rtc::Thread::CreateWithSocketServer();
		 auto signalingThread = rtc::Thread::Create();
		 auto workerThread = rtc::Thread::Create();
		if (! networkThread->Start() || ! signalingThread->Start() || ! workerThread->Start())
		{
			ERROR_EX_LOG("thread start errored");
			return false;
		}
		peer_connection_factory_ = webrtc::CreatePeerConnectionFactory(
			networkThread.get(),
			signalingThread.get(),
			workerThread.get() /* signaling_thread */, nullptr /* default_adm */,
			webrtc::CreateBuiltinAudioEncoderFactory(),
			webrtc::CreateBuiltinAudioDecoderFactory(),
			webrtc::CreateBuiltinVideoEncoderFactory(),
			webrtc::CreateBuiltinVideoDecoderFactory(), nullptr /* audio_mixer */,
			nullptr /* audio_processing */);

		if (!peer_connection_factory_)
		{
			ERROR_EX_LOG("create rtc peer connection factory failed !!!");
			 
			return false;
		}

		webrtc::PeerConnectionInterface::RTCConfiguration config;
		config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan; //这个 
		//config.enable_dtls_srtp = dtls; //是否加密
		//webrtc::PeerConnectionInterface::IceServer server;
		//server.uri = GetPeerConnectionString();
		//config.servers.push_back(server);

		peer_connection_ = peer_connection_factory_->CreatePeerConnection( config, nullptr, nullptr, this);


		if (!peer_connection_)
		{
			ERROR_EX_LOG("create peer connection failed !!!");
			return false;
		}
		if (!_add_track())
		{
			return false;
		}
		peer_connection_->CreateOffer(
			this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
		
		return true;
	}

	void cpeer_connection::update()
	{
	}

	void cpeer_connection::destroy()
	{
	}

	bool cpeer_connection::create_answer(const std::string & sdp)
	{
		/*webrtc::SdpType type = *type_maybe;
		std::string sdp;
		if (!rtc::GetStringFromJsonObject(jmessage, kSessionDescriptionSdpName,
			&sdp)) {
			RTC_LOG(WARNING) << "Can't parse received session description message.";
			return;
		}*/
		webrtc::SdpParseError error;
		std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
			webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdp, &error);
		if (!session_description)
		{
			/*RTC_LOG(WARNING) << "Can't parse received session description message. "
				<< "SdpParseError was: " << error.description;*/
			WARNING_EX_LOG("Can't parse received session description message. SdpParseError was: = %s ", error.description.c_str());
			return false;
		}
		NORMAL_EX_LOG("Received session description : %s", sdp.c_str());
	//	RTC_LOG(INFO) << " Received session description :";// << message;
		peer_connection_->SetRemoteDescription( DummySetSessionDescriptionObserver::Create(), session_description.release());
		peer_connection_->CreateAnswer(
			this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
		return true;
	}

	void cpeer_connection::OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver, const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>& streams)
	{
	}

	void cpeer_connection::OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
	{
	}

	void cpeer_connection::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
	{

	}

	void cpeer_connection::OnSuccess(webrtc::SessionDescriptionInterface* desc)
	{
		// 得到本地视频基本信息 先设置本地 SDP 鸭
		peer_connection_->SetLocalDescription(DummySetSessionDescriptionObserver::Create(), desc);

		std::string sdp;
		desc->ToString(&sdp);
		NORMAL_EX_LOG("[sdp = %s]", sdp.c_str());
		if (m_client_ptr)
		{
			m_client_ptr->set_load_offer(sdp);
		}
		
		// send server peer sdp info 
	}

	void cpeer_connection::OnFailure(webrtc::RTCError error)
	{
	}

	bool cpeer_connection::_add_track()
	{
		if (!peer_connection_->GetSenders().empty()) 
		{
			WARNING_EX_LOG("Already added tracks.");
			return false;  // Already added tracks.
		}

		/*rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
			peer_connection_factory_->CreateAudioTrack(
				RTC_AUDIO_LABEL, peer_connection_factory_->CreateAudioSource(
					cricket::AudioOptions())));
		auto result_or_error = peer_connection_->AddTrack(audio_track, { RTC_STREAM });
		if (!result_or_error.ok()) {
			RTC_LOG(LS_ERROR) << "Failed to add audio track to PeerConnection: "
				<< result_or_error.error().message();
		}*/

		rtc::scoped_refptr<ProxyVideoTrackSource> video_device = ProxyVideoTrackSource::Create();
		if (video_device)
		{
			rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track_(
				peer_connection_factory_->CreateVideoTrack(RTC_VIDEO_LABEL, video_device));
			//main_wnd_->StartLocalRenderer(video_track_);

			auto result_or_error = peer_connection_->AddTrack(video_track_, { RTC_STREAM });
			if (!result_or_error.ok()) 
			{
				//result_or_error.value();
				ERROR_EX_LOG( "Failed to add video track to PeerConnection:");
				return false;
			}

		}
		else 
		{
			ERROR_EX_LOG( "OpenVideoCaptureDevice failed");
			return false;
		}
		return true;
	}

}