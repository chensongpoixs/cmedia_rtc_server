
#include "cmedia_server.h"
#include <signal.h>
//------------------------------------------------------------------------------
void Stop(int i)
{
	chen::g_media_server.stop();
}

void RegisterSignal()
{
	signal(SIGINT, Stop);
	signal(SIGTERM, Stop);

}

int main(int argc, char* argv[])
{
	/*std::string offer_sdp = "v=0\r\no=mediasoup-client 10000 4 IN IP4 0.0.0.0\r\ns=-\r\nt=0 0\r\na=ice-lite\r\na=fingerprint:sha-512 45:DA:1C:EC:D0:8B:B6:B9:F8:3D:34:DE:70:BE:92:A3:FA:27:7F:D8:FE:37:B4:2D:1A:CC:21:4D:C9:C6:13:B7:26:11:19:BA:65:57:11:1E:83:C8:3F:53:1D:44:AA:1B:70:72:96:39:74:25:A8:A3:2F:06:BA:DD:1B:F3:76:F1\r\na=msid-semantic: WMS *\r\na=group:BUNDLE datachannel 0 probator 1\r\nm=application 7 UDP/DTLS/SCTP webrtc-datachannel\r\nc=IN IP4 127.0.0.1\r\na=setup:actpass\r\na=mid:datachannel\r\na=ice-ufrag:d7it3k20q92fgfz3\r\na=ice-pwd:vl0ftesqwr0h61tdc61sej1b9kt2zame\r\na=candidate:udpcandidate 1 udp 1076302079 192.168.0.109 49229 typ host\r\na=end-of-candidates\r\na=ice-options:renomination\r\na=sctp-port:5000\r\na=max-message-size:262144\r\nm=video 7 UDP/TLS/RTP/SAVPF 100 101\r\nc=IN IP4 127.0.0.1\r\na=rtpmap:100 H264/90000\r\na=rtpmap:101 rtx/90000\r\na=fmtp:100 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f\r\na=fmtp:101 apt=100\r\na=rtcp-fb:100 transport-cc \r\na=rtcp-fb:100 ccm fir\r\na=rtcp-fb:100 nack \r\na=rtcp-fb:100 nack pli\r\na=extmap:1 urn:ietf:params:rtp-hdrext:sdes:mid\r\na=extmap:4 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:5 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=extmap:11 urn:3gpp:video-orientation\r\na=extmap:12 urn:ietf:params:rtp-hdrext:toffset\r\na=setup:actpass\r\na=mid:0\r\na=msid:38296e8c a4f74987-2d2b-4cb1-9ded-04c8d6af32b1\r\na=sendonly\r\na=ice-ufrag:d7it3k20q92fgfz3\r\na=ice-pwd:vl0ftesqwr0h61tdc61sej1b9kt2zame\r\na=candidate:udpcandidate 1 udp 1076302079 192.168.0.109 49229 typ host\r\na=end-of-candidates\r\na=ice-options:renomination\r\na=ssrc:738168580 cname:38296e8c\r\na=ssrc:738168581 cname:38296e8c\r\na=ssrc-group:FID 738168580 738168581\r\na=rtcp-mux\r\na=rtcp-rsize\r\nm=video 7 UDP/TLS/RTP/SAVPF 127\r\nc=IN IP4 127.0.0.1\r\na=rtpmap:127 H264/90000\r\na=fmtp:127 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f\r\na=rtcp-fb:127 transport-cc \r\na=rtcp-fb:127 ccm fir\r\na=rtcp-fb:127 nack \r\na=rtcp-fb:127 nack pli\r\na=extmap:1 urn:ietf:params:rtp-hdrext:sdes:mid\r\na=extmap:4 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:5 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=extmap:11 urn:3gpp:video-orientation\r\na=extmap:12 urn:ietf:params:rtp-hdrext:toffset\r\na=setup:actpass\r\na=mid:probator\r\na=msid:probator probator\r\na=sendonly\r\na=ice-ufrag:d7it3k20q92fgfz3\r\na=ice-pwd:vl0ftesqwr0h61tdc61sej1b9kt2zame\r\na=candidate:udpcandidate 1 udp 1076302079 192.168.0.109 49229 typ host\r\na=end-of-candidates\r\na=ice-options:renomination\r\na=ssrc:1234 cname:probator\r\na=rtcp-mux\r\na=rtcp-rsize\r\nm=video 7 UDP/TLS/RTP/SAVPF 100 101\r\nc=IN IP4 127.0.0.1\r\na=rtpmap:100 H264/90000\r\na=rtpmap:101 rtx/90000\r\na=fmtp:100 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f\r\na=fmtp:101 apt=100\r\na=rtcp-fb:100 transport-cc \r\na=rtcp-fb:100 ccm fir\r\na=rtcp-fb:100 nack \r\na=rtcp-fb:100 nack pli\r\na=extmap:1 urn:ietf:params:rtp-hdrext:sdes:mid\r\na=extmap:4 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:5 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=extmap:11 urn:3gpp:video-orientation\r\na=extmap:12 urn:ietf:params:rtp-hdrext:toffset\r\na=setup:actpass\r\na=mid:1\r\na=msid:3468df6d bfa88a30-ef74-490f-a820-bb81d5c2e680\r\na=sendonly\r\na=ice-ufrag:d7it3k20q92fgfz3\r\na=ice-pwd:vl0ftesqwr0h61tdc61sej1b9kt2zame\r\na=candidate:udpcandidate 1 udp 1076302079 192.168.0.109 49229 typ host\r\na=end-of-candidates\r\na=ice-options:renomination\r\na=ssrc:751214125 cname:3468df6d\r\na=ssrc:751214126 cname:3468df6d\r\na=ssrc-group:FID 751214125 751214126\r\na=rtcp-mux\r\na=rtcp-rsize\r\n";

	FILE * out_file_ptr = fopen("./webrtc_offer_2.sdp", "wb+");
	fprintf(out_file_ptr, "%s", offer_sdp.c_str());
	fflush(out_file_ptr);

	return 0;*/
	RegisterSignal();

	const char* config_filename = "server.cfg";
	if (argc > 1)
	{
		config_filename = argv[1];
	}
	const char* log_path = "./log";
	if (argc > 2)
	{
		log_path = argv[2];
	}
	bool init = chen::g_media_server.init(log_path, config_filename);

	if (init)
	{
		init = chen::g_media_server.Loop();
	}
	 

	chen::g_media_server.destroy();
	if (!init)
	{
		return 1;
	}
	return 0;

	 
}
