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

