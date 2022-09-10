
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
	/*std::string offer_sdp = "v=0\r\no=- 766148587023367303 2 IN IP4 127.0.0.1\r\ns=-\r\nt=0 0\r\na=group:BUNDLE 0 1\r\na=msid-semantic: WMS stream_id\r\nm=audio 9 UDP/TLS/RTP/SAVPF 111 103 104 9 102 0 8 106 105 13 110 112 113 126\r\nc=IN IP4 0.0.0.0\r\na=rtcp:9 IN IP4 0.0.0.0\r\na=ice-ufrag:Z277\r\na=ice-pwd:LfBLct9Nhr4NZtbmk1rcdxI7\r\na=ice-options:trickle\r\na=fingerprint:sha-256 E2:E5:CF:5A:F8:B6:6E:7E:98:04:7D:78:38:2C:D1:E1:12:2B:88:39:BE:5F:53:2F:CE:FE:09:81:4E:AB:D5:CD\r\na=setup:active\r\na=mid:0\r\na=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level\r\na=extmap:2 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=extmap:3 urn:ietf:params:rtp-hdrext:sdes:mid\r\na=extmap:4 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id\r\na=extmap:5 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id\r\na=sendrecv\r\na=msid:stream_id audio_label\r\na=rtcp-mux\r\na=rtpmap:111 opus/48000/2\r\na=rtcp-fb:111 transport-cc\r\na=fmtp:111 minptime=10;useinbandfec=1\r\na=rtpmap:103 ISAC/16000\r\na=rtpmap:104 ISAC/32000\r\na=rtpmap:9 G722/8000\r\na=rtpmap:102 ILBC/8000\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:8 PCMA/8000\r\na=rtpmap:106 CN/32000\r\na=rtpmap:105 CN/16000\r\na=rtpmap:13 CN/8000\r\na=rtpmap:110 telephone-event/48000\r\na=rtpmap:112 telephone-event/32000\r\na=rtpmap:113 telephone-event/16000\r\na=rtpmap:126 telephone-event/8000\r\na=ssrc:900375463 cname:EZHghqMyS5bNTKUd\r\nm=video 9 UDP/TLS/RTP/SAVPF 96 97 98 99 100 101 127 121 125 120 124 107 108 109 123 119 122\r\nc=IN IP4 0.0.0.0\r\na=rtcp:9 IN IP4 0.0.0.0\r\na=ice-ufrag:Z277\r\na=ice-pwd:LfBLct9Nhr4NZtbmk1rcdxI7\r\na=ice-options:trickle\r\na=fingerprint:sha-256 E2:E5:CF:5A:F8:B6:6E:7E:98:04:7D:78:38:2C:D1:E1:12:2B:88:39:BE:5F:53:2F:CE:FE:09:81:4E:AB:D5:CD\r\na=setup:active\r\na=mid:1\r\na=extmap:14 urn:ietf:params:rtp-hdrext:toffset\r\na=extmap:13 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:12 urn:3gpp:video-orientation\r\na=extmap:2 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=extmap:11 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay\r\na=extmap:6 http://www.webrtc.org/experiments/rtp-hdrext/video-content-type\r\na=extmap:7 http://www.webrtc.org/experiments/rtp-hdrext/video-timing\r\na=extmap:8 http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07\r\na=extmap:9 http://www.webrtc.org/experiments/rtp-hdrext/color-space\r\na=extmap:3 urn:ietf:params:rtp-hdrext:sdes:mid\r\na=extmap:4 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id\r\na=extmap:5 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id\r\na=sendrecv\r\na=msid:stream_id video_label\r\na=rtcp-mux\r\na=rtcp-rsize\r\na=rtpmap:96 H264/90000\r\na=rtcp-fb:96 goog-remb\r\na=rtcp-fb:96 transport-cc\r\na=rtcp-fb:96 ccm fir\r\na=rtcp-fb:96 nack\r\na=rtcp-fb:96 nack pli\r\na=fmtp:96 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42001f;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:97 rtx/90000\r\na=fmtp:97 apt=96;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:98 H264/90000\r\na=rtcp-fb:98 goog-remb\r\na=rtcp-fb:98 transport-cc\r\na=rtcp-fb:98 ccm fir\r\na=rtcp-fb:98 nack\r\na=rtcp-fb:98 nack pli\r\na=fmtp:98 level-asymmetry-allowed=1;packetization-mode=0;profile-level-id=42001f;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:99 rtx/90000\r\na=fmtp:99 apt=98;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:100 H264/90000\r\na=rtcp-fb:100 goog-remb\r\na=rtcp-fb:100 transport-cc\r\na=rtcp-fb:100 ccm fir\r\na=rtcp-fb:100 nack\r\na=rtcp-fb:100 nack pli\r\na=fmtp:100 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:101 rtx/90000\r\na=fmtp:101 apt=100;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:127 H264/90000\r\na=rtcp-fb:127 goog-remb\r\na=rtcp-fb:127 transport-cc\r\na=rtcp-fb:127 ccm fir\r\na=rtcp-fb:127 nack\r\na=rtcp-fb:127 nack pli\r\na=fmtp:127 level-asymmetry-allowed=1;packetization-mode=0;profile-level-id=42e01f;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:121 rtx/90000\r\na=fmtp:121 apt=127;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:125 VP8/90000\r\na=rtcp-fb:125 goog-remb\r\na=rtcp-fb:125 transport-cc\r\na=rtcp-fb:125 ccm fir\r\na=rtcp-fb:125 nack\r\na=rtcp-fb:125 nack pli\r\na=fmtp:125 x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:120 rtx/90000\r\na=fmtp:120 apt=125;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:124 VP9/90000\r\na=rtcp-fb:124 goog-remb\r\na=rtcp-fb:124 transport-cc\r\na=rtcp-fb:124 ccm fir\r\na=rtcp-fb:124 nack\r\na=rtcp-fb:124 nack pli\r\na=fmtp:124 profile-id=0;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:107 rtx/90000\r\na=fmtp:107 apt=124;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:108 VP9/90000\r\na=rtcp-fb:108 goog-remb\r\na=rtcp-fb:108 transport-cc\r\na=rtcp-fb:108 ccm fir\r\na=rtcp-fb:108 nack\r\na=rtcp-fb:108 nack pli\r\na=fmtp:108 profile-id=2;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:109 rtx/90000\r\na=fmtp:109 apt=108;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:123 red/90000\r\na=fmtp:123 x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:119 rtx/90000\r\na=fmtp:119 apt=123;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=rtpmap:122 ulpfec/90000\r\na=fmtp:122 x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000\r\na=ssrc-group:FID 3656806274 1960953279\r\na=ssrc:3656806274 cname:EZHghqMyS5bNTKUd\r\na=ssrc:1960953279 cname:EZHghqMyS5bNTKUd\r\n";


	FILE * out_file_ptr = fopen("./webrtc.sdp", "wb+");
	fprintf(out_file_ptr, "%s", answer_sdp.c_str());
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
