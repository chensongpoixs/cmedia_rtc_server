'use strict'

//本地视频窗口

var localVideo = document.querySelector('video#localvideo');

//对端视频窗口
var remoteVideo = document.querySelector('video#remotevideo');

// connserver 连接服务器的标签
var btnConn = document.querySelector('button#connserver');


//与信令服务器断开连接Button
var btnLeave = document.querySelector('button#leave');

// 查看Offer文本窗口
var offer = document.querySelector('textarea#offer');


// 查看Answer文本窗口
var answer  = document.querySelector('textarea#answer');

var pcConfig = {
	'iceServer' : [{
		//TURN服务器地址
		'urls': 'stun:stun.l.google.com:19302'
		// TURN服务器用户名
		//'username': 'xxx',
		//TURN服务器密码
		//'credential': "xxx"
	}],
	// 默认使用relay方式传输数据 turn 参数 哈哈 ^_^
	"iceTransportPolicy": "all",
	//"iceTransportPolicy": "relay",
	// 音视频同一个通道哈
	"bundlePolicy": "max-bundle",
	// 同一个rtcp和rtp走 公用一个Candidate 就是同一个通道哈 ^_^
	"rtcpMuxPolicy": "require",
	"sdpSemantics": "unified-plan",
	"iceCandiatePoolSize": "0"
};


const MessageType = {

     // ROOM
		C2S_BeatHeart:200,
		S2C_BeatHeart:201,
		C2S_Login:202,
		S2C_Login:203,
		C2S_JoinRoom:204,
		S2C_JoinRoom:205,
		C2S_DestroyRoom:206,
		S2C_DestroyRoom:207,

		C2S_WebrtcMessage:208,
		S2C_WebrtcMessage:209,
		
		//C2S_CreateAnswar,
		//S2C_CreateAnswar,
		//C2S_Candidate,
		//S2C_Candidate,




		////////////////////////////////////////////////////////////////////////////////////////////
		S2C_WebrtcMessageUpdate:500,
		S2C_JoinRoomUpdate:501,
		S2C_LevalRoomUpdate:502
};
//本地视频流
var localStream = null;

//对端视频流
var remoteStream = null;


// PeerConnection
var pc = null;

//房间号
var roomid;
var username;
var socket = null;

//offer描述
var offerdesc = null;

//状态机 初始为init
var state = 'init';


let ws ;

const WS_OPEN_STATE = 1;


//////////////////////////////SDP /////////////////////////


var session_sdp ;


var remove_sdpObject =
            {
                version: 0,
                origin: {
                    address: '0.0.0.0',
                    ipVer: 4,
                    netType: 'IN',
                    sessionId: 10000,
                    sessionVersion: 0,
                    username: 'media_rtc_client'
                },
                name: '-',
                timing: { start: 0, stop: 0 }, 
				msidSemantic: { semantic: 'WMS', token: '*' },
				groups: [{ type: 'BUNDLE', mids: '' }],
				icelite: 'ice-lite',
				//fingerprint :
                //{
                //    type: dtlsParameters.fingerprints[numFingerprints - 1].algorithm,
                //    hash: dtlsParameters.fingerprints[numFingerprints - 1].value
                //},
                media: []
				
            };

var remove_session;


/**
 功能： 判断此浏览器是在PC端，还是移动端。
 返回值: false 说明当前操作系统是移动端；
		 true  说明当前的操作系统是PC端

*/
function IsPC()
{
	var userAgentInfo = navigator.userAgent;
	var Agents = ["Android", "iPhone", "SymbianOS", "Windows Phone", "iPad", "iPod"];
	var flag = true;
	
	
	for (var v = 0; v < Agents.length; ++v)
	{
		if (userAgentInfo.indexOf(Agents[v])> 0)
		{
			flag = false;
			break;
		}
	}
	return flag;
}


/**
 功能： 判断是Android端还是IOS端
 返回： true 说明是Android端
		false 说明是IOS端
*/
function IsAndroid()
{
	var u = navigator.userAgent;
	var app = navigator.appVersion;
	var isAndroid = u.indexOf('Android')> -1 || u.indexOf('Linux')> -1;
	var isIOS = !!u.match(/\(i[^;]+;( U;)? CPU.+Mac OS X/);
	
	if (isAndroid)
	{
		// 这个是Android系统
		return true;
	}
	if (isIOS)
	{
		// IOS 系统
		return false;
	}
}



/**
 功能：从url 中获取指定的值
 返回值：指定的值或者false
*/

function getQueryVariable(variable)
{
	var query = window.location.search.substring(1);
	
	var vars = query.split("&");
	
	for (var i = 0; i < vars.length; ++i)
	{
		var pair = vars[i].split("=");
		if (pair[0] == variable)
		{
			return pair[1];
		}
	}
	return  false;
}



/**
 功能： 向对端发送信息
 返回值: 无
*/
function sendMessage(message)
{
	console.log('send message to other and ', message);
	
	if (!ws)
	{
		console.log('socket is null');
	}
	// 208  message 



	ws.send(JSON.stringify(message));

	//ws.send();
	//socket.emit('message', roomid, data);
}


/**
 功能： 与信令服务器连接socket.io 连接
		并根据信令更新状态机
		
 返回值 无

*/

function conn()
{
	//连接信令服务器
	//socket = io.connect( );

	 window.WebSocket = window.WebSocket || window.MozWebSocket;

    if (!window.WebSocket) 
    {
        alert('Your browser doesn\'t support WebSocket');
        return;
    }

     var ws_url = "ws://127.0.0.1:9500/?roomId=20220927&peerId=chensong";
	console.log('ws_url = ', ws_url);
     ws = new WebSocket(ws_url );

	

////////////////////////////////////////////////////////////////////////////////////////////////////

	ws.onmessage = function(event)
    {
        
        console.log(`<- S2C : ${event.data}`);

        let msg = JSON.parse(event.data);
        // 
        if (msg.msg_id === 203) // S2C_Login
        {
        	if(msg.result != 0)
        	{
        		console.log("login failed!!! roomid = ", roomid);
        	}
        	else 
        	{
        		console.log("login ok !!! roomid = ", roomid);
        		//状态机变更'joined'
				state = 'joined';
				/**
				 如果是Mesh方案， 第一个人不该在这里创建
				 peerConnection，而是要等到所有端都收到
				 一个'otherjoin' 信息时在创建
				*/
				
				// 创建PeerConnection 并绑定音视频轨
				createPeerConnection();
				bindTracks();
				
				//设置button状态
				btnConn.disabled = true;
				btnLeave.disabled = false;
				console.log('receive joined message , state=', state);
        	}
        }
        else if (msg.msg_id === 209) // S2C_WebrtcMessage
        {
        	console.log("webrtc mssage ~~~");
        }
        else if (msg.msg_id === 500) //S2C_WebrtcMessageUpdate
        {
        	// 1. config

        	// 2. answer
        	
        	// 3. icecandidate

        	//	//如果收到的SDP是ｏｆｆｅｒ
			if(msg.data.hasOwnProperty('type') && msg.data.type === 'offer')
			{
				offer.value = msg.data.sdp;
				
				//进行媒体协商
				pc.setRemoteDescription(new RTCSessionDescription(msg.data));
				
				// 创建answer
				pc.createAnswer()
					.then(getAnswer)
					.catch(handleAnswerError);
				
				
			}
			else if (msg.data.hasOwnProperty('type') && msg.data.type === 'answer')
			{   // 如果收到的SDP是Answer
				answer.value = msg.data.sdp;
				
				//进行媒体协商
				pc.setRemoteDescription(new RTCSessionDescription(msg.data));
				
			}
			else if (msg.data.hasOwnProperty('type') && msg.data.type === 'candidate')
			{
				//如果收到是Candidate信息
				var candidate = new RTCIceCandidate({
					sdpMLineIndex :msg.data.label,
					candidate: msg.data.candidate
				});
				
				//将远端Candidate信息添加到PeerConnection
				pc.addIceCandidate(candidate);
			}
			else 
			{
				console.log('the message is invalid!!!', msg.data);
			}
        	
        }
        else if (msg.msg_id === 501) //S2C_JoinRoomUpdate
        {
        	console.log('new peer !!!');

        		console.log('receive joined message!  --》', roomid);
				// 如果是多人， 每加入一个人都要创建一个新的PeerConnection
			if (state === 'joined_unbind')
			{
				createPeerConnection();
				bindTracks();
			}
	//	
			//状态机变更为 joined_conn
			state = 'joined_conn';
		
			// 开始 '呼叫'对方
			call();
		
			console.log('receive other_join message , state = ', state);

        }
        else if (msg.msg_id === 502) // S2C_LevalRoomUpdate
        {
        	console.log('leval room update ---->>>>> ');
        	//	//改变button状态
		//	btnConn.disabled = true;
			//btnLeave.disabled = false;
        }



       /* let msg = JSON.parse(event.data);
        if (msg.type === 'config') {
            onConfig(msg);
        } else if (msg.type === 'playerCount') {
            updateKickButton(msg.count - 1);
        } else if (msg.type === 'answer') {
            onWebRtcAnswer(msg);
        } else if (msg.type === 'iceCandidate') {
            onWebRtcIce(msg.candidate);
        } else {
            console.log(`invalid SS message type: ${msg.type}`);
        }*/
    };
	 ws.onerror = function(event) 
    {
        console.log(`WS error: ${JSON.stringify(event)}`);
        console.log('receive disabled message !', roomid);
		
		if (!(state === 'leaved'))
		{
			//挂断 "呼叫"
			hangup();
			
			///关闭本地媒体
			closeLocalMedia();
		}
		
		// 状态机变更为 leaved
		state = 'leaved';
    };

   ws.onclose = function(event) 
   {
        console.log(`WS closed: ${JSON.stringify(event.code)} - ${event.reason}`);
       ws = null;
          console.log('receive disabled message !', roomid);
		
		if (!(state === 'leaved'))
		{
			//挂断 "呼叫"
			hangup();
			
			///关闭本地媒体
			closeLocalMedia();
		}
		
		// 状态机变更为 leaved
		state = 'leaved';
    };

	ws.onopen = function(event)
    {
        console.log(`C2S -> Login`);
         if (ws && ws.readyState === WS_OPEN_STATE) 
         {
         	//从URL中获取roomid
			roomid = getQueryVariable('roomId');
			username = getQueryVariable('username');
			ws.send(JSON.stringify({ 
                msg_id : 202/*C2S_Login*/, 
                data: {
                	user_name : username,
                	room_name : roomid
                } }));
            //let offerStr = JSON.stringify(offer);
            //console.log(`-> SS: offer:\n${offerStr}`);
          //  ws.send(offerStr);
          // console.log(`C2S -> getRouterRtpCapabilities OK !!!`);
            
             //   self.ws.send(JSON.stringify({
            //    request: true,
            //    id : self.msg_id,
             //   method:'getRouterRtpCapabilities',
             //   data: {} }));
        }
        
       
    }
	
	
	//发送'join'消息
	//socket.emit('join', roomid);
	return true;
}


/**
 功能: 打开有视频设备，并连接信令服务器
 返回值: true 

*/


function connSignalServer() 
{
	// 开启本地视频
	console.log('connSignalServer ------>>>>>>>> ');
	start();
	
	return true;
}
///////////////////////////////////////////// SDP ////////////////////////////////////////////////////////////////////////////////////////
var validLine = RegExp.prototype.test.bind(/^([a-z])=(.*)/);
var grammar =  {
  v: [{
    name: 'version',
    reg: /^(\d*)$/
  }],
  o: [{
    // o=- 20518 0 IN IP4 203.0.113.1
    // NB: sessionId will be a String in most cases because it is huge
    name: 'origin',
    reg: /^(\S*) (\d*) (\d*) (\S*) IP(\d) (\S*)/,
    names: ['username', 'sessionId', 'sessionVersion', 'netType', 'ipVer', 'address'],
    format: '%s %s %d %s IP%d %s'
  }],
  // default parsing of these only (though some of these feel outdated)
  s: [{ name: 'name' }],
  i: [{ name: 'description' }],
  u: [{ name: 'uri' }],
  e: [{ name: 'email' }],
  p: [{ name: 'phone' }],
  z: [{ name: 'timezones' }], // TODO: this one can actually be parsed properly...
  r: [{ name: 'repeats' }],   // TODO: this one can also be parsed properly
  // k: [{}], // outdated thing ignored
  t: [{
    // t=0 0
    name: 'timing',
    reg: /^(\d*) (\d*)/,
    names: ['start', 'stop'],
    format: '%d %d'
  }],
  c: [{
    // c=IN IP4 10.47.197.26
    name: 'connection',
    reg: /^IN IP(\d) (\S*)/,
    names: ['version', 'ip'],
    format: 'IN IP%d %s'
  }],
  b: [{
    // b=AS:4000
    push: 'bandwidth',
    reg: /^(TIAS|AS|CT|RR|RS):(\d*)/,
    names: ['type', 'limit'],
    format: '%s:%s'
  }],
  m: [{
    // m=video 51744 RTP/AVP 126 97 98 34 31
    // NB: special - pushes to session
    // TODO: rtp/fmtp should be filtered by the payloads found here?
    reg: /^(\w*) (\d*) ([\w/]*)(?: (.*))?/,
    names: ['type', 'port', 'protocol', 'payloads'],
    format: '%s %d %s %s'
  }],
  a: [
    {
      // a=rtpmap:110 opus/48000/2
      push: 'rtp',
      reg: /^rtpmap:(\d*) ([\w\-.]*)(?:\s*\/(\d*)(?:\s*\/(\S*))?)?/,
      names: ['payload', 'codec', 'rate', 'encoding'],
      format: function (o) {
        return (o.encoding)
          ? 'rtpmap:%d %s/%s/%s'
          : o.rate
            ? 'rtpmap:%d %s/%s'
            : 'rtpmap:%d %s';
      }
    },
    {
      // a=fmtp:108 profile-level-id=24;object=23;bitrate=64000
      // a=fmtp:111 minptime=10; useinbandfec=1
      push: 'fmtp',
      reg: /^fmtp:(\d*) ([\S| ]*)/,
      names: ['payload', 'config'],
      format: 'fmtp:%d %s'
    },
    {
      // a=control:streamid=0
      name: 'control',
      reg: /^control:(.*)/,
      format: 'control:%s'
    },
    {
      // a=rtcp:65179 IN IP4 193.84.77.194
      name: 'rtcp',
      reg: /^rtcp:(\d*)(?: (\S*) IP(\d) (\S*))?/,
      names: ['port', 'netType', 'ipVer', 'address'],
      format: function (o) {
        return (o.address != null)
          ? 'rtcp:%d %s IP%d %s'
          : 'rtcp:%d';
      }
    },
    {
      // a=rtcp-fb:98 trr-int 100
      push: 'rtcpFbTrrInt',
      reg: /^rtcp-fb:(\*|\d*) trr-int (\d*)/,
      names: ['payload', 'value'],
      format: 'rtcp-fb:%s trr-int %d'
    },
    {
      // a=rtcp-fb:98 nack rpsi
      push: 'rtcpFb',
      reg: /^rtcp-fb:(\*|\d*) ([\w-_]*)(?: ([\w-_]*))?/,
      names: ['payload', 'type', 'subtype'],
      format: function (o) {
        return (o.subtype != null)
          ? 'rtcp-fb:%s %s %s'
          : 'rtcp-fb:%s %s';
      }
    },
    {
      // a=extmap:2 urn:ietf:params:rtp-hdrext:toffset
      // a=extmap:1/recvonly URI-gps-string
      // a=extmap:3 urn:ietf:params:rtp-hdrext:encrypt urn:ietf:params:rtp-hdrext:smpte-tc 25@600/24
      push: 'ext',
      reg: /^extmap:(\d+)(?:\/(\w+))?(?: (urn:ietf:params:rtp-hdrext:encrypt))? (\S*)(?: (\S*))?/,
      names: ['value', 'direction', 'encrypt-uri', 'uri', 'config'],
      format: function (o) {
        return (
          'extmap:%d' +
          (o.direction ? '/%s' : '%v') +
          (o['encrypt-uri'] ? ' %s' : '%v') +
          ' %s' +
          (o.config ? ' %s' : '')
        );
      }
    },
    {
      // a=extmap-allow-mixed
      name: 'extmapAllowMixed',
      reg: /^(extmap-allow-mixed)/
    },
    {
      // a=crypto:1 AES_CM_128_HMAC_SHA1_80 inline:PS1uQCVeeCFCanVmcjkpPywjNWhcYD0mXXtxaVBR|2^20|1:32
      push: 'crypto',
      reg: /^crypto:(\d*) ([\w_]*) (\S*)(?: (\S*))?/,
      names: ['id', 'suite', 'config', 'sessionConfig'],
      format: function (o) {
        return (o.sessionConfig != null)
          ? 'crypto:%d %s %s %s'
          : 'crypto:%d %s %s';
      }
    },
    {
      // a=setup:actpass
      name: 'setup',
      reg: /^setup:(\w*)/,
      format: 'setup:%s'
    },
    {
      // a=connection:new
      name: 'connectionType',
      reg: /^connection:(new|existing)/,
      format: 'connection:%s'
    },
    {
      // a=mid:1
      name: 'mid',
      reg: /^mid:([^\s]*)/,
      format: 'mid:%s'
    },
    {
      // a=msid:0c8b064d-d807-43b4-b434-f92a889d8587 98178685-d409-46e0-8e16-7ef0db0db64a
      name: 'msid',
      reg: /^msid:(.*)/,
      format: 'msid:%s'
    },
    {
      // a=ptime:20
      name: 'ptime',
      reg: /^ptime:(\d*(?:\.\d*)*)/,
      format: 'ptime:%d'
    },
    {
      // a=maxptime:60
      name: 'maxptime',
      reg: /^maxptime:(\d*(?:\.\d*)*)/,
      format: 'maxptime:%d'
    },
    {
      // a=sendrecv
      name: 'direction',
      reg: /^(sendrecv|recvonly|sendonly|inactive)/
    },
    {
      // a=ice-lite
      name: 'icelite',
      reg: /^(ice-lite)/
    },
    {
      // a=ice-ufrag:F7gI
      name: 'iceUfrag',
      reg: /^ice-ufrag:(\S*)/,
      format: 'ice-ufrag:%s'
    },
    {
      // a=ice-pwd:x9cml/YzichV2+XlhiMu8g
      name: 'icePwd',
      reg: /^ice-pwd:(\S*)/,
      format: 'ice-pwd:%s'
    },
    {
      // a=fingerprint:SHA-1 00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF:00:11:22:33
      name: 'fingerprint',
      reg: /^fingerprint:(\S*) (\S*)/,
      names: ['type', 'hash'],
      format: 'fingerprint:%s %s'
    },
    {
      // a=candidate:0 1 UDP 2113667327 203.0.113.1 54400 typ host
      // a=candidate:1162875081 1 udp 2113937151 192.168.34.75 60017 typ host generation 0 network-id 3 network-cost 10
      // a=candidate:3289912957 2 udp 1845501695 193.84.77.194 60017 typ srflx raddr 192.168.34.75 rport 60017 generation 0 network-id 3 network-cost 10
      // a=candidate:229815620 1 tcp 1518280447 192.168.150.19 60017 typ host tcptype active generation 0 network-id 3 network-cost 10
      // a=candidate:3289912957 2 tcp 1845501695 193.84.77.194 60017 typ srflx raddr 192.168.34.75 rport 60017 tcptype passive generation 0 network-id 3 network-cost 10
      push:'candidates',
      reg: /^candidate:(\S*) (\d*) (\S*) (\d*) (\S*) (\d*) typ (\S*)(?: raddr (\S*) rport (\d*))?(?: tcptype (\S*))?(?: generation (\d*))?(?: network-id (\d*))?(?: network-cost (\d*))?/,
      names: ['foundation', 'component', 'transport', 'priority', 'ip', 'port', 'type', 'raddr', 'rport', 'tcptype', 'generation', 'network-id', 'network-cost'],
      format: function (o) {
        var str = 'candidate:%s %d %s %d %s %d typ %s';

        str += (o.raddr != null) ? ' raddr %s rport %d' : '%v%v';

        // NB: candidate has three optional chunks, so %void middles one if it's missing
        str += (o.tcptype != null) ? ' tcptype %s' : '%v';

        if (o.generation != null) {
          str += ' generation %d';
        }

        str += (o['network-id'] != null) ? ' network-id %d' : '%v';
        str += (o['network-cost'] != null) ? ' network-cost %d' : '%v';
        return str;
      }
    },
    {
      // a=end-of-candidates (keep after the candidates line for readability)
      name: 'endOfCandidates',
      reg: /^(end-of-candidates)/
    },
    {
      // a=remote-candidates:1 203.0.113.1 54400 2 203.0.113.1 54401 ...
      name: 'remoteCandidates',
      reg: /^remote-candidates:(.*)/,
      format: 'remote-candidates:%s'
    },
    {
      // a=ice-options:google-ice
      name: 'iceOptions',
      reg: /^ice-options:(\S*)/,
      format: 'ice-options:%s'
    },
    {
      // a=ssrc:2566107569 cname:t9YU8M1UxTF8Y1A1
      push: 'ssrcs',
      reg: /^ssrc:(\d*) ([\w_-]*)(?::(.*))?/,
      names: ['id', 'attribute', 'value'],
      format: function (o) {
        var str = 'ssrc:%d';
        if (o.attribute != null) {
          str += ' %s';
          if (o.value != null) {
            str += ':%s';
          }
        }
        return str;
      }
    },
    {
      // a=ssrc-group:FEC 1 2
      // a=ssrc-group:FEC-FR 3004364195 1080772241
      push: 'ssrcGroups',
      // token-char = %x21 / %x23-27 / %x2A-2B / %x2D-2E / %x30-39 / %x41-5A / %x5E-7E
      reg: /^ssrc-group:([\x21\x23\x24\x25\x26\x27\x2A\x2B\x2D\x2E\w]*) (.*)/,
      names: ['semantics', 'ssrcs'],
      format: 'ssrc-group:%s %s'
    },
    {
      // a=msid-semantic: WMS Jvlam5X3SX1OP6pn20zWogvaKJz5Hjf9OnlV
      name: 'msidSemantic',
      reg: /^msid-semantic:\s?(\w*) (\S*)/,
      names: ['semantic', 'token'],
      format: 'msid-semantic: %s %s' // space after ':' is not accidental
    },
    {
      // a=group:BUNDLE audio video
      push: 'groups',
      reg: /^group:(\w*) (.*)/,
      names: ['type', 'mids'],
      format: 'group:%s %s'
    },
    {
      // a=rtcp-mux
      name: 'rtcpMux',
      reg: /^(rtcp-mux)/
    },
    {
      // a=rtcp-rsize
      name: 'rtcpRsize',
      reg: /^(rtcp-rsize)/
    },
    {
      // a=sctpmap:5000 webrtc-datachannel 1024
      name: 'sctpmap',
      reg: /^sctpmap:([\w_/]*) (\S*)(?: (\S*))?/,
      names: ['sctpmapNumber', 'app', 'maxMessageSize'],
      format: function (o) {
        return (o.maxMessageSize != null)
          ? 'sctpmap:%s %s %s'
          : 'sctpmap:%s %s';
      }
    },
    {
      // a=x-google-flag:conference
      name: 'xGoogleFlag',
      reg: /^x-google-flag:([^\s]*)/,
      format: 'x-google-flag:%s'
    },
    {
      // a=rid:1 send max-width=1280;max-height=720;max-fps=30;depend=0
      push: 'rids',
      reg: /^rid:([\d\w]+) (\w+)(?: ([\S| ]*))?/,
      names: ['id', 'direction', 'params'],
      format: function (o) {
        return (o.params) ? 'rid:%s %s %s' : 'rid:%s %s';
      }
    },
    {
      // a=imageattr:97 send [x=800,y=640,sar=1.1,q=0.6] [x=480,y=320] recv [x=330,y=250]
      // a=imageattr:* send [x=800,y=640] recv *
      // a=imageattr:100 recv [x=320,y=240]
      push: 'imageattrs',
      reg: new RegExp(
        // a=imageattr:97
        '^imageattr:(\\d+|\\*)' +
        // send [x=800,y=640,sar=1.1,q=0.6] [x=480,y=320]
        '[\\s\\t]+(send|recv)[\\s\\t]+(\\*|\\[\\S+\\](?:[\\s\\t]+\\[\\S+\\])*)' +
        // recv [x=330,y=250]
        '(?:[\\s\\t]+(recv|send)[\\s\\t]+(\\*|\\[\\S+\\](?:[\\s\\t]+\\[\\S+\\])*))?'
      ),
      names: ['pt', 'dir1', 'attrs1', 'dir2', 'attrs2'],
      format: function (o) {
        return 'imageattr:%s %s %s' + (o.dir2 ? ' %s %s' : '');
      }
    },
    {
      // a=simulcast:send 1,2,3;~4,~5 recv 6;~7,~8
      // a=simulcast:recv 1;4,5 send 6;7
      name: 'simulcast',
      reg: new RegExp(
        // a=simulcast:
        '^simulcast:' +
        // send 1,2,3;~4,~5
        '(send|recv) ([a-zA-Z0-9\\-_~;,]+)' +
        // space + recv 6;~7,~8
        '(?:\\s?(send|recv) ([a-zA-Z0-9\\-_~;,]+))?' +
        // end
        '$'
      ),
      names: ['dir1', 'list1', 'dir2', 'list2'],
      format: function (o) {
        return 'simulcast:%s %s' + (o.dir2 ? ' %s %s' : '');
      }
    },
    {
      // old simulcast draft 03 (implemented by Firefox)
      //   https://tools.ietf.org/html/draft-ietf-mmusic-sdp-simulcast-03
      // a=simulcast: recv pt=97;98 send pt=97
      // a=simulcast: send rid=5;6;7 paused=6,7
      name: 'simulcast_03',
      reg: /^simulcast:[\s\t]+([\S+\s\t]+)$/,
      names: ['value'],
      format: 'simulcast: %s'
    },
    {
      // a=framerate:25
      // a=framerate:29.97
      name: 'framerate',
      reg: /^framerate:(\d+(?:$|\.\d+))/,
      format: 'framerate:%s'
    },
    {
      // RFC4570
      // a=source-filter: incl IN IP4 239.5.2.31 10.1.15.5
      name: 'sourceFilter',
      reg: /^source-filter: *(excl|incl) (\S*) (IP4|IP6|\*) (\S*) (.*)/,
      names: ['filterMode', 'netType', 'addressTypes', 'destAddress', 'srcList'],
      format: 'source-filter: %s %s %s %s %s'
    },
    {
      // a=bundle-only
      name: 'bundleOnly',
      reg: /^(bundle-only)/
    },
    {
      // a=label:1
      name: 'label',
      reg: /^label:(.+)/,
      format: 'label:%s'
    },
    {
      // RFC version 26 for SCTP over DTLS
      // https://tools.ietf.org/html/draft-ietf-mmusic-sctp-sdp-26#section-5
      name: 'sctpPort',
      reg: /^sctp-port:(\d+)$/,
      format: 'sctp-port:%s'
    },
    {
      // RFC version 26 for SCTP over DTLS
      // https://tools.ietf.org/html/draft-ietf-mmusic-sctp-sdp-26#section-6
      name: 'maxMessageSize',
      reg: /^max-message-size:(\d+)$/,
      format: 'max-message-size:%s'
    },
    {
      // RFC7273
      // a=ts-refclk:ptp=IEEE1588-2008:39-A7-94-FF-FE-07-CB-D0:37
      push:'tsRefClocks',
      reg: /^ts-refclk:([^\s=]*)(?:=(\S*))?/,
      names: ['clksrc', 'clksrcExt'],
      format: function (o) {
        return 'ts-refclk:%s' + (o.clksrcExt != null ? '=%s' : '');
      }
    },
    {
      // RFC7273
      // a=mediaclk:direct=963214424
      name:'mediaClk',
      reg: /^mediaclk:(?:id=(\S*))? *([^\s=]*)(?:=(\S*))?(?: *rate=(\d+)\/(\d+))?/,
      names: ['id', 'mediaClockName', 'mediaClockValue', 'rateNumerator', 'rateDenominator'],
      format: function (o) {
        var str = 'mediaclk:';
        str += (o.id != null ? 'id=%s %s' : '%v%s');
        str += (o.mediaClockValue != null ? '=%s' : '');
        str += (o.rateNumerator != null ? ' rate=%s' : '');
        str += (o.rateDenominator != null ? '/%s' : '');
        return str;
      }
    },
    {
      // a=keywds:keywords
      name: 'keywords',
      reg: /^keywds:(.+)$/,
      format: 'keywds:%s'
    },
    {
      // a=content:main
      name: 'content',
      reg: /^content:(.+)/,
      format: 'content:%s'
    },
    // BFCP https://tools.ietf.org/html/rfc4583
    {
      // a=floorctrl:c-s
      name: 'bfcpFloorCtrl',
      reg: /^floorctrl:(c-only|s-only|c-s)/,
      format: 'floorctrl:%s'
    },
    {
      // a=confid:1
      name: 'bfcpConfId',
      reg: /^confid:(\d+)/,
      format: 'confid:%s'
    },
    {
      // a=userid:1
      name: 'bfcpUserId',
      reg: /^userid:(\d+)/,
      format: 'userid:%s'
    },
    {
      // a=floorid:1
      name: 'bfcpFloorId',
      reg: /^floorid:(.+) (?:m-stream|mstrm):(.+)/,
      names: ['id', 'mStream'],
      format: 'floorid:%s mstrm:%s'
    },
    {
      // any a= that we don't understand is kept verbatim on media.invalid
      push: 'invalid',
      names: ['value']
    }
  ]
};

// set sensible defaults to avoid polluting the grammar with boring details
Object.keys(grammar).forEach(function (key) {
  var objs = grammar[key];
  objs.forEach(function (obj) {
    if (!obj.reg) {
      obj.reg = /(.*)/;
    }
    if (!obj.format) {
      obj.format = '%s';
    }
  });
});




function toIntIfInt(v)
{
  return String(Number(v)) === v ? Number(v) : v;
};

function attachProperties(match, location, names, rawName) {
  if (rawName && !names) {
    location[rawName] = toIntIfInt(match[1]);
  }
  else {
    for (var i = 0; i < names.length; i += 1) {
      if (match[i+1] != null) {
        location[names[i]] = toIntIfInt(match[i+1]);
      }
    }
  }
};


function parseReg(obj, location, content) 
  {
  var needsBlank = obj.name && obj.names;
  if (obj.push && !location[obj.push]) {
    location[obj.push] = [];
  }
  else if (needsBlank && !location[obj.name]) {
    location[obj.name] = {};
  }
  var keyLocation = obj.push ?
    {} :  // blank object that will be pushed
    needsBlank ? location[obj.name] : location; // otherwise, named location or root

  attachProperties(content.match(obj.reg), keyLocation, obj.names, obj.name);

  if (obj.push) {
    location[obj.push].push(keyLocation);
  }
};

function parse_sdp(sdp)
{
	var session = {}
    , media = []
    , location = session; // points at where properties go under (one of the above)
	console.log('-------------------------');
	console.log(grammar);
	console.log('---------------------------');
  // parse lines we understand
  sdp.split(/(\r\n|\r|\n)/).filter(validLine).forEach(function (l) {
    var type = l[0];
    var content = l.slice(2);
    if (type === 'm') {
      media.push({rtp: [], fmtp: []});
      location = media[media.length-1]; // point at latest media line
    }

    for (var j = 0; j < (grammar[type] || []).length; j += 1) {
      var obj = grammar[type][j];
      if (obj.reg.test(content)) {
        return parseReg(obj, location, content);
      }
    }
  });

  session.media = media; // link it up
  return session;
}



function extractDtlsParameters({ sdpObject }) 
{
    const mediaObject = (sdpObject.media || [])
        .find((m) => (m.iceUfrag && m.port !== 0));
    if (!mediaObject)
        throw new Error('no active media section found');
    const fingerprint = mediaObject.fingerprint || sdpObject.fingerprint;
    let role;
    switch (mediaObject.setup) {
        case 'active':
            role = 'client';
            break;
        case 'passive':
            role = 'server';
            break;
        case 'actpass':
            role = 'auto';
            break;
    }
    const dtlsParameters = {
        role,
        fingerprints: [
            {
                algorithm: fingerprint.type,
                value: fingerprint.hash
            }
        ]
    };
    return dtlsParameters;
}






////////////////////////////////////////////////////SDP Write/////////////////////////////////////////////////////////////////////////////////////////





// customized util.format - discards excess arguments and can void middle ones
var formatRegExp = /%[sdv%]/g;
var format = function (formatStr) {
  var i = 1;
  var args = arguments;
  var len = args.length;
  return formatStr.replace(formatRegExp, function (x) {
    if (i >= len) {
      return x; // missing argument
    }
    var arg = args[i];
    i += 1;
    switch (x) {
    case '%%':
      return '%';
    case '%s':
      return String(arg);
    case '%d':
      return Number(arg);
    case '%v':
      return '';
    }
  });
  // NB: we discard excess arguments - they are typically undefined from makeLine
};

var makeLine = function (type, obj, location) {
  var str = obj.format instanceof Function ?
    (obj.format(obj.push ? location : location[obj.name])) :
    obj.format;

  var args = [type + '=' + str];
  if (obj.names) {
    for (var i = 0; i < obj.names.length; i += 1) {
      var n = obj.names[i];
      if (obj.name) {
        args.push(location[obj.name][n]);
      }
      else { // for mLine and push attributes
        args.push(location[obj.names[i]]);
      }
    }
  }
  else {
    args.push(location[obj.name]);
  }
  return format.apply(null, args);
};

// RFC specified order
// TODO: extend this with all the rest
var defaultOuterOrder = [
  'v', 'o', 's', 'i',
  'u', 'e', 'p', 'c',
  'b', 't', 'r', 'z', 'a'
];
var defaultInnerOrder = ['i', 'c', 'b', 'a'];


function sdp_write(session, opts) 
{
	console.log('session.origin.sessionVersion = '+ session.origin.sessionVersion);
	session.origin.sessionVersion++;
	console.log('session.origin.sessionVersion = '+ session.origin.sessionVersion);
  opts = opts || {};
  // ensure certain properties exist
  if (session.version == null) {
    session.version = 0; // 'v=0' must be there (only defined version atm)
  }
  if (session.name == null) {
    session.name = ' '; // 's= ' must be there if no meaningful name set
  }
  session.media.forEach(function (mLine) {
    if (mLine.payloads == null) {
      mLine.payloads = '';
    }
  });

  var outerOrder = opts.outerOrder || defaultOuterOrder;
  var innerOrder = opts.innerOrder || defaultInnerOrder;
  var sdp = [];

  // loop through outerOrder for matching properties on session
  outerOrder.forEach(function (type) {
    grammar[type].forEach(function (obj) {
      if (obj.name in session && session[obj.name] != null) {
        sdp.push(makeLine(type, obj, session));
      }
      else if (obj.push in session && session[obj.push] != null) {
        session[obj.push].forEach(function (el) {
          sdp.push(makeLine(type, obj, el));
        });
      }
    });
  });

  // then for each media line, follow the innerOrder
  session.media.forEach(function (mLine) {
    sdp.push(makeLine('m', grammar.m[0], mLine));

    innerOrder.forEach(function (type) {
      grammar[type].forEach(function (obj) {
        if (obj.name in mLine && mLine[obj.name] != null) {
          sdp.push(makeLine(type, obj, mLine));
        }
        else if (obj.push in mLine && mLine[obj.push] != null) {
          mLine[obj.push].forEach(function (el) {
            sdp.push(makeLine(type, obj, el));
          });
        }
      });
    });
  });

  return sdp.join('\r\n') + '\r\n';
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////















/////////////////////////////////////////////////////////////////////////////////////////////////
/**
 功能： 打开音视频设备成功时的回调用
 返回值: true 

*/
function getMediaStream(stream)
{
	//将从设备上获取到的音视频track添加到localStream中
	console.log('++++++++++++++++getMediaStream+++++++++++++++++++++++');
	console.log(stream);
	if (localStream)
	{
		console.log('audio');
		stream.getAudioTracks().forEach((track) => {
			localStream.addTrack(track);
			stream.removeTrack(track);
			
		});
	}
	else 
	{
		localStream = stream;
	}
	
	//本地视频标签与本地流绑定
	localVideo.srcObject = localStream;
	
	/**
	 调用 conn() 函数的位置特别重要，一定要在
	 getMediaStream调用之后再调用它，否则就
	 会出现绑定失败的情况
	*/
	
	// setup connection
	conn();
}

/**
 功能：错误处理函数
 返回值: 无
*/

function handleError(err)
{
	console.log('Failed to get Media Stream !', err);
}


/**
 功能： 打开有音视频设备，
 
*/

function start()
{
	if (!navigator.mediaDevices || !navigator.mediaDevices.getUserMedia)
	{
		console.log('the getUserMedia is not supported !!!');
		return;
	}
	
	var constraints;
	constraints = {
		video : true,
		audio :{
			echoCancellation : true,
			noiseSuppression: true,
			autoGainControl: true
		}
	};
	// 底层的api 设置回调函数 getMediaStream
	navigator.mediaDevices.getUserMedia(constraints)
					.then(getMediaStream)
					.catch(handleError);
	
}


/**
 功能： 获得对端媒体流
*/

function getRemoteStream(e)
{
	//存放远端视频流
	remoteStream = e.streams[0];
	
	// 远端视频标签与远端视频流绑定
	remoteVideo.srcObject = e.streams[0];
}



/**
 功能: 处理Offer错误

*/

function handleOfferError(err)
{
	console.log('Failed ot carete offer:', err);
}

/**
 功能： 处理Answer错误
*/
function handleAnswerError(err)
{
	console.log('Failed to create answer: ', err);
}

/**
 功能: 获取Answer SDP描述符的回调函数

*/
function getAnswer(desc)
{
	//设置Answer
	pc.setLocalDescription(desc);
	
	// 将Answer显示出来
	answer.value = desc.sdp;
	
	// 将Answer SDP 发送给对端
	sendMessage({
					msg_id: 208,
					data: desc
				});
}


/**
 功能： 获取Offer SDP 描述符的

*/
function getOffer(desc)
{
	console.log('offer desc ==> ', desc );
	//设置Offer
	pc.setLocalDescription(desc);
	
	//将Offer显示在网页中出来
	offer.value = desc.sdp;
	offerdesc = desc;
	
	console.log('-------------SDP------------');
	
	session_sdp = parse_sdp(offer.value);
	console.log(session_sdp);
	console.log('session_sdp.origin.sessionVersion = '+ session_sdp.origin.sessionVersion);
	// Get our local DTLS parameters.
    const dtlsParameters =  extractDtlsParameters({ sdpObject: session_sdp });
        // Set our DTLS role.
    dtlsParameters.role = 'client';
	
	
	console.log('------------dtlsParameters-------------');
	console.log(dtlsParameters);
        // Update the remote DTLS role in the SDP.
		
		
		
	const test_write_sdp = 	sdp_write(session_sdp);
	console.log('-------------test_write_sdp------------');
	console.log(test_write_sdp);
	console.log('-------------SDP------------');
	//将Offer SDP 发送给对端
	
	sendMessage(
				{
					msg_id: 208,
					data: offerdesc
				}
			 );
}



/**
 功能: 创建PeerConnection对象
 

*/
function createPeerConnection()
{
	/**
	   如果是多人的话， 在这里要创建一个新的连接
	   新创建好得要放到一个映射表中
	*/
	// key=userid, value=peerConnection
	console.log('create RTCPeerConnection !!!');
	
	if (!pc)
	{
		//创建PeerConnection对象
		pc = new RTCPeerConnection(pcConfig);
		
		// 当收集到Candidate后
		pc.onicecandidate = (e) => {
			if (e.candidate)
			{
				console.log("candidate" + JSON.stringify(e.candidate.toJSON()));
				
				//将Candidate发送个对端
				sendMessage( {
					msg_id: 208,
					data: {
						type : 'candidate',
						label :event.candidate.sdpMLineIndex,
						id : event.candidate.sdpMid,
						candidate : event.candidate.candidate
					}
				 });
			}
			else 
			{
				console.log('this is the end candidate !!!');
			}
		}
		/**
		 当PeerConnection 对象收到远端音视频流时
		 触发ontrack事件
		 并回调getRemoteStream函数
		
		*/
		pc.ontrack = getRemoteStream;
	}
	else 
	{
		console.log('the pc have be careted !!!');
	}
	
	return;
}


/**
	功能： 将音视频track绑定到PeerConnection对象中
*/
function bindTracks()
{
	console.log('bind tracks into RTCPeerConnection !!!');
	
	if (pc === null && localStream === undefined)
	{
		console.log('pc is null or undefined!!!');
		return;
	}
	
	if (localStream === null && localStream === undefined)
	{
		console.log('localStream is null or undefinded !!!');
		return;
	}
	
	//将本地音视频流中所有tranck添加到PeerConnection对象中
	localStream.getTracks().forEach((track) => {
		pc.addTrack(track, localStream);
	});
}



/**
 功能： 开启 "呼叫"
*/

function call()
{
	if (state === 'joined_conn')
	{
		var offerOptions = {
			offerToReceiveAudio: 1,
			offerToReceiveVideo: 1
		};
		
		/**
		 创建Offer
		 如果成功: 则返回getOffer()方法
		 如果失败：则返回handleOfferError方法
		 
		*/
		pc.createOffer(offerOptions)
			.then(getOffer)
			.catch(handleOfferError);
	}
}



/**
 功能： 挂断呼叫
 

*/

function hangup()
{
	if (!pc)
	{
		return;
	}
	
	offerdesc= null;
	//将PeerConnection 连接关闭
	pc.close();
	pc = null;
}


/**
 功能： 关闭本地媒体
*/
function closeLocalMedia()
{
	if (!(localStream === null) || 
	localStream === undefined)
	{
		// 遍历每个track， 并将其关闭
		localStream.getTracks().forEach((track) => {
			track.stop();
		});
	}
	
	localStream = null;
}


/**
 功能： 离开房间
*/
function leave()
{
	//向信令服务器发送leave信息
	//socket.emit('leave', roomid);
	ws.close();
	//挂断 '呼叫'
	hangup();
	
	//关闭本地媒体
	closeLocalMedia();
	
	offer.value = '';
	answer.value = '';
	btnConn.disabled = false;
	btnLeave.disabled = true;
	
	
}


//为Button设置单击事件
btnConn.onclick = connSignalServer;
btnLeave.onclick = leave;