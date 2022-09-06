# 问题


```
// TODO@chensong 202208011  
// .cpp(55): error C4146: 一元负运算符应用于无符号类型，结果仍为无符号类型  --> 解决办法——关闭SDL检查
```

# openssl 1.1.0 vs上编译指令


x64

```
perl Configure VC-WIN64A no-asm  no-shared  --prefix=D:/dep/openssl_1.1.0
```


# 流程

1. Create_WebRTC

C2S --> forceTcp, producing, consuming, sctpCapabilities 


返回消息

	1. iceParameters ->用户名, 密码、 icelite
	2. iceCandidates -> foundation、 priority、ip、protocol， port、type(HOST)、tcpType
    3. iceState
	4. iceSelectedTuple ->localIp、localPort、remoteIp、remotePort、protocol
	5. dtlsParameters  -> fingerprints[{algorithm, value}, {algorithm, value}]、role
	6. sctpParameters  -> MIS、OS、isDataChannel、maxMessageSize、port、sctpBufferedAmount、sendBufferSize



2. WebRTC_connect

C2S -> TRansportId、dtlsParameters{role, fingerprints[algorithm, value ]} ==> 发送自己本地使用fingerprints加密密钥


S2C->


3. WebRTC_produce


# SDP

a： 属性
c: 连接
v： 版本
s： 一个会话
o： 谁拥有这个会话





v=0
o=chensong 10000 1 IN IP4 0.0.0.0
s=-
t=0 0
a=ice-lite
a=fingerprint:sha-512 3D:9A:9C:39:15:B1:1F:B4:43:F8:F0:95:D1:F6:DE:C1:3C:09:C1:B9:83:14:A0:E8:31:11:92:69:56:7F:6D:DD:9E:76:C9:9B:CD:82:10:DF:FB:64:B5:0B:7C:FB:DF:E0:83:54:5C:2F:0E:D7:1A:A7:E6:0C:AC:6D:FD:9B:AE:75
a=msid-semantic: WMS *
a=group:BUNDLE 0
m=video 7 UDP/TLS/RTP/SAVPF 100 101
c=IN IP4 127.0.0.1
a=rtpmap:100 H264/90000
a=rtpmap:101 rtx/90000
a=fmtp:100 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f;x-google-max-bitrate=28000;x-google-min-bitrate=5500;x-google-start-bitrate=1000
a=fmtp:101 apt=100
a=rtcp-fb:100 transport-cc
a=rtcp-fb:100 ccm fir
a=rtcp-fb:100 nack
a=rtcp-fb:100 nack pli
a=extmap:3 urn:ietf:params:rtp-hdrext:sdes:mid
a=extmap:4 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id
a=extmap:5 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id
a=extmap:13 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
a=extmap:2 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
a=extmap:8 http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07
a=extmap:12 urn:3gpp:video-orientation
a=extmap:14 urn:ietf:params:rtp-hdrext:toffset
a=setup:active
a=mid:0
a=recvonly
a=ice-ufrag:xjg2o2h2v8bfzqkv
a=ice-pwd:z5ywy0b4le11t42tcqzpz5kdmr22i9rm
a=candidate:udpcandidate 1 udp 1076302079 192.168.1.73 43717 typ host
a=end-of-candidates
a=ice-options:renomination
a=rtcp-mux
a=rtcp-rsize