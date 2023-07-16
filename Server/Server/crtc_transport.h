/***********************************************************************************************
created: 		2023-02-03

author:			chensong

purpose:		crtc_transport
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

#ifndef _C_RTC_TRANSPORT_H_
#define _C_RTC_TRANSPORT_H_
#include "cnet_type.h"
#include "crtc_sdp.h"
#include "crtc_source_description.h"
#include "crtc_transport_define.h"
#include "cudp_socket.h"
#include "crtc_stun_packet.h"
#include "crtc_dtls.h"
#include "crtp_rtcp.h"
#include "crtc_player_stream.h"
#include "cremote_estimator_proxy.h"
#include "FeedbackRtpTransport.hpp"
#include "crtp_listener.h"
#include "TransportCongestionControlServer.hpp"
#include "TransportCongestionControlServer.hpp"
#include "TransportCongestionControlClient.hpp"
#include "csctp_association.h"
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


	struct crtc_room_master
	{
		std::string m_room_name;
		std::string m_user_name;
		crtc_room_master () 
			: m_room_name("")
			, m_user_name(""){}
	};

	class crtc_transport : public cudp_socket::Listener, public crtc_transportlinster,
		public RTC::TransportCongestionControlClient::Listener,
		public RTC::TransportCongestionControlServer::Listener, 
		public chen::ctimer::Listener, public csctp_association::Listener
	{
	public:
		explicit crtc_transport(const crtc_room_master & master)
		: m_local_sdp ()
		, m_remote_sdp ()
		, m_rtc_net_state(ERtcNetworkStateInit)
		, m_udp_sockets()
		, m_current_socket_ptr(NULL)
		, m_dtls_ptr(NULL)
		, m_srtp_send_session_ptr(NULL)
		, m_srtp_recv_session_ptr(NULL)
			, m_players_ssrc_map()
			/*, m_all_audio_ssrc(0)
			, m_all_rtx_audio_ssrc(0)
			, m_all_video_ssrc(0)
			, m_all_rtx_video_ssrc(0)*/
			, m_key_frame_ssrc(0)
			, m_ssrc_media_type_map()
			, m_rtc_client_type(ERtcClientNone)
			, m_request_keyframe(0)
			, m_time_out_ms(uv_util::GetTimeMs())
			, m_remote_estimator(this)
			, m_all_rtp_listener()
			, m_server_ssrc_map()
			, m_tcc_client(NULL)
			, m_tcc_server(NULL)
			, m_rtc_master(master)
			, m_transportWideCcSeq(0u)
			, m_udp_ports()
			, m_tcp_ports()
			, m_timer_ptr(NULL)
			, m_sctp_association_ptr(NULL)
			, m_rtp_header_extension_ids()
			//, m_feedback_rtp_transport_packet()
			//, m_srtp()
		 {}

		virtual ~crtc_transport();
	public:
		using onSendCallback = const std::function<void(bool sent)>;
	public:
		bool init(ERtcClientType rtc_client_type, const crtc_sdp & remote_sdp, const crtc_sdp & local_sdp, const crtc_source_description& stream_desc);

		bool create_players(const std::map<uint32_t, crtc_track_description*>& sub_relations);
		void update(uint32 uDeltaTime);

		void destroy();
		bool is_active() const;
	public:

		void request_key_frame();

		const crtc_sdp & get_rtp_sdp() const {
			return m_local_sdp;
		}
		  crtc_sdp   get_rtp_sdp()   {
			return m_local_sdp;
		}
		  std::string  get_stream_name() { return m_rtc_master.m_room_name + "/" + m_rtc_master.m_user_name; }
		  cremote_estimator_proxy* get_remote_estimator() { return &m_remote_estimator; }
		const ERtcClientType get_rtc_type() { return m_rtc_client_type; }

	public:
		void OnTransportConsumerKeyFrameRequested();

		
	public:
		/* Pure virtual methods inherited from RTC::Consumer::Listener. */
	public:
		//void OnConsumerSendRtpPacket(crtc_consumer* consumer, RTC::RtpPacket* packet) override;
		void OnConsumerRetransmitRtpPacket(crtc_consumer* consumer, RTC::RtpPacket* packet)  ;
		/*void OnConsumerKeyFrameRequested(crtc_consumer* consumer, uint32_t mappedSsrc) override;
		void OnConsumerNeedBitrateChange(crtc_consumer* consumer) override;
		void OnConsumerNeedZeroBitrate(crtc_consumer* consumer) override;
		void OnConsumerProducerClosed(crtc_consumer* consumer) override;*/
	public:
		void send_rtp_data(void * data, int32 size);
		void send_rtp_data(RTC::RtpPacket* packet);

		void send_rtp_packet(RTC::RtpPacket* packet, cudp_socket_handler::onSendCallback *cb);
		void send_rtp_audio_data(RTC::RtpPacket* packet);
		void send_rtp_video_data(RTC::RtpPacket* packet);
		void send_rtp_rtx_video_data(RTC::RtpPacket* packet);


		void send_consumer(RTC::RtpPacket* packet);
		// rtcp  
		void send_rtcp_packet(RTC::RTCP::Packet* packet);
		bool send_rtcp(const uint8 * data, size_t len);

		void send_rtcp_compound_packet(RTC::RTCP::CompoundPacket* packet);

	public:
		bool send_sctp_data(const uint8* data, size_t len);

		void send_sctp_data(uint16_t streamId,
			uint32_t ppid,
			const uint8* msg,
			size_t len);
	public:
		// virtual
		virtual int32 write_dtls_data(void* data, int size);
		virtual int32 on_dtls_handshake_done();
		virtual void  on_dtls_application_data(const uint8* data, int32 size);
		virtual void  on_dtls_transport_connected(ECRYPTO_SUITE srtp_crypto_suite, uint8* srtp_local_key, size_t srtp_local_key_len, uint8* srtp_remote_key, size_t srtp_remote_key_len);
	public:
		void set_state_as_waiting_stun() { m_rtc_net_state = ERtcNetworkStateWaitingStun; };
		bool get_dtls_connected_ok() const { return m_rtc_net_state == ERtcNetworkStateEstablished; }
	protected:
	public:

		void OnPacketReceived(cudp_socket* socket, const uint8_t* data, size_t len, const sockaddr * remoteAddr);
	public:
		virtual void OnUdpSocketPacketReceived(
			cudp_socket* socket, const uint8_t* data, size_t len, const struct sockaddr* remoteAddr);


	public:
		void OnTransportCongestionControlServerSendRtcpPacket(
			RTC::TransportCongestionControlServer* tccServer, RTC::RTCP::Packet* packet);

	public:
		virtual void OnTransportCongestionControlClientBitrates(
			RTC::TransportCongestionControlClient* tccClient,
			RTC::TransportCongestionControlClient::Bitrates& bitrates) ;
		virtual void OnTransportCongestionControlClientSendRtpPacket(
			RTC::TransportCongestionControlClient* tccClient,
			RTC::RtpPacket* packet,
			const webrtc::PacedPacketInfo& pacingInfo)  ;


		public:
			virtual void OnSctpAssociationConnecting(csctp_association* sctpAssociation)  ;
			virtual void OnSctpAssociationConnected(csctp_association* sctpAssociation)  ;
			virtual void OnSctpAssociationFailed(csctp_association* sctpAssociation)  ;
			virtual void OnSctpAssociationClosed(csctp_association* sctpAssociation)  ;
			virtual void OnSctpAssociationSendData(
				csctp_association* sctpAssociation, const uint8_t* data, size_t len)  ;
			virtual void OnSctpAssociationMessageReceived(
				csctp_association* sctpAssociation,
				uint16_t streamId,
				uint32_t ppid,
				const uint8_t* msg,
				size_t len) ;
			virtual void OnSctpAssociationBufferedAmount(
				csctp_association* sctpAssociation, uint32_t len)  ;
	public:
		
	private:

		void _on_stun_data_received(cudp_socket* socket, const uint8_t* data, size_t len, const sockaddr * remoteAddr);
		void _on_dtls_data_received(cudp_socket* socket, const uint8_t* data, size_t len, const sockaddr * remoteAddr);
	
		void _on_rtp_data_received(cudp_socket* socket, const uint8* data, size_t len, const sockaddr*remoteAddr );
		void _on_rtcp_data_received(cudp_socket* socket, const uint8* data, size_t len, const sockaddr*remoteAddr );
		// data channel 
		void  _on_application_data_receviced(const uint8* data, size_t len);
		//void _on_application_data_receviced(cudp_socket* socket, const uint8* data, size_t len, const sockaddr*remoteAddr);

	private:
		// publish -> remote sdp 
		/*bool _negotiate_publish_capability(crtc_source_description * stream_desc);
		bool _generate_publish_local_sdp(  crtc_sdp& local_sdp, crtc_source_description* stream_desc, bool unified_plan, bool audio_before_video);

		bool _generate_publish_local_sdp_for_audio(crtc_sdp& local_sdp, crtc_source_description* stream_desc);
		bool _generate_publish_local_sdp_for_video(crtc_sdp& local_sdp, crtc_source_description* stream_desc, bool unified_plan);
*/
	private:
		void _handler_rtcp_packet(RTC::RTCP::Packet* packet);


		bool _dispatch_rtcp(crtcp_common* rtcp);


		public:
			void OnTimer(ctimer* timer)  ;
	private:
		// p2p net 
		bool _on_rtcp_feedback_twcc(char * data, int32 nb_data);
		// 老版本的代码 不需要处理了
		bool _on_rtcp_feedback_remb(crtcp_psfb_common * rtcp);



		inline bool _mangle_rtp_packet(RTC::RtpPacket * packet, bool audio_video = false);
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

		///// key: stream id
		//std::map<std::string, SrsRtcPlayStream*> players_;
		//key: player track's ssrc
		std::map<uint32, crtc_player_stream*>	m_players_ssrc_map;

		//////////////////////
		/*uint32									m_all_audio_ssrc;
		uint32									m_all_rtx_audio_ssrc;*/
		////////////////////////////////////////////////////////////////
		/*uint32									m_all_video_ssrc;
		uint32									m_all_rtx_video_ssrc;*/


		uint32									m_key_frame_ssrc;

		std::map<uint32, uint32 >				m_ssrc_media_type_map;


		ERtcClientType							m_rtc_client_type;
		uint32									m_request_keyframe;
	
		uint64									m_time_out_ms;
		cremote_estimator_proxy					m_remote_estimator;
		uint32									m_feedback_gcc_timer;
		//RTC::RTCP::FeedbackRtpTransportPacket   m_feedback_rtp_transport_packet;

		crtp_listener							m_all_rtp_listener;
		std::map<uint32, uint32>				m_server_ssrc_map;
		RTC::TransportCongestionControlClient*	m_tcc_client;
		RTC::TransportCongestionControlServer*	m_tcc_server;

		crtc_room_master						m_rtc_master;
		uint32									m_transportWideCcSeq;
		std::vector<uint32>									m_udp_ports;
		std::vector< uint32>									m_tcp_ports;
		ctimer				*					m_timer_ptr;
		csctp_association *						m_sctp_association_ptr;
		RTC::RtpHeaderExtensionIds				m_rtp_header_extension_ids;
	};


}

#endif // _C_RTC_TRANSPORT_H_