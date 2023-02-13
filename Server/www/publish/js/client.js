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

	//io.set('origins', 'http://localhost:8082'); 
	// 'joined' 信息处理函数
	//socket.on('joined', (roomid, id) => {
	//	console.log('receive joined message!  --》', roomid, id);
	//	//状态机变更'joined'
	//	state = 'joined';
	//	/**
	//	 如果是Mesh方案， 第一个人不该在这里创建
	//	 peerConnection，而是要等到所有端都收到
	//	 一个'otherjoin' 信息时在创建
	//	*/
	//	
	//	// 创建PeerConnection 并绑定音视频轨
	//	createPeerConnection();
	//	bindTracks();
	//	
	//	//设置button状态
	//	btnConn.disabled = true;
	//	btnLeave.disabled = false;
	//	console.log('receive joined message , state=', state);
	//});
	//
	//
	//
	//// otherjoin 信息处理函数
	//socket.on('other_join', (roomid) => {
	//	console.log('recevice joined message: ', roomid, state);
	//	
	//	// 如果是多人， 每加入一个人都要创建一个新的PeerConnection
	//	if (state === 'joined_unbind')
	//	{
	//		createPeerConnection();
	//		bindTracks();
	//	}
	//	
	//	//状态机变更为 joined_conn
	//	state = 'joined_conn';
	//	
	//	// 开始 '呼叫'对方
	//	call();
	//	
	//	console.log('receive other_join message , state = ', state);
	//});
	//
	//// full 信息处理函数
	//socket.on('full', (roomid, id) => {
	//	console.log('receive full message ', roomid, id);
	//	
	//	//关闭socket.io 连接
	//	socket.disconnect();
	//	
	//	//挂断 "呼叫"
	//	hangup();
	//	
	//	//关闭本地媒体
	//	closeLocalMedia();
	//	
	//	//状态机变更为 leavled
	//	state = 'leaved';
	//	
	//	console.log('receive full message , state = ', state);
	//	
	//	alert('the room is full !!!');
	//});
	//
	//
	//// leaved 信息处理函数
	//socket.on('left', (roomid, id) => {
	//	console.log('receive leaved message, state =', state);
	//	
	//	//改变button状态
	//	btnConn.disabled = false;
	//	btnLeave.disabled = true;
	//	
	//});
	//
	//// bye 信息处理函数
	//socket.on('bye', (roomid, id) => {
	//	console.log('recive bye message ', roomid, id);
	//	
	//	/**
	//	 当是Mesh方案时，应该带上当前房间的用户数，
	//	 如果当前房间用户数不小于2，则不用修改状态，
	//	 并且关闭的应该是对应用户的PeerConnection。
	//	 在客户端应该维护一张PeerConnection表，它是
	//	 key:value的格式， key=userid, value=peerConnection 
	//	*/
	//	//状态机变更为joined_unbind
	//	state = 'joined_unbind';
	//	
	//	//挂断 '呼叫'
	//	hangup();
	//	
	//	offer.value = '';
	//	answer.value = '';
	//	console.log('receive bye message, state = ', state );
	//});
	//
	//// socket.io 连接断开处理函数
	//socket.on('disconnect', (socket) => {
	//	console.log('receive disabled message !', roomid);
	//	
	//	if (!(state === 'leaved'))
	//	{
	//		//挂断 "呼叫"
	//		hangup();
	//		
	//		///关闭本地媒体
	//		closeLocalMedia();
	//	}
	//	
	//	// 状态机变更为 leaved
	//	state = 'leaved';
	//});
	//
	////收到对端信息处理函数
	//socket.on('message', (roomid, data) => {
	//	console.log('  receive message ===> !', roomid, data);
	//	
	//	if (data === null || data === undefined)
	//	{
	//		console.log('the message is invalid!!!');
	//		return;
	//	}
	//	
	//	//如果收到的SDP是ｏｆｆｅｒ
	//	if(data.hasOwnProperty('typ.e') && datatype === 'offer')
	//	{
	//		offer.value = data.sdp;
	//		
	//		//进行媒体协商
	//		pc.setRemoteDescription(new RTCSessionDescription(data));
	//		
	//		// 创建answer
	//		pc.createAnswer()
	//			.then(getAnswer)
	//			.catch(handleAnswerError);
	//		
	//		
	//	}
	//	else if (data.hasOwnProperty('type') && data.type === 'answer')
	//	{   // 如果收到的SDP是Answer
	//		answer.value = data;
	//		
	//		//进行媒体协商
	//		pc.setRemoteDescription(new RTCSessionDescription(data));
	//		
	//	}
	//	else if (data.hasOwnProperty('type') && data.type === 'candidate')
	//	{
	//		//如果收到是Candidate信息
	//		 var candidate = new RTCIceCandidate({
	//			 sdpMLineIndex :data.label,
	//			 candidate:data.candidate
	//		 });
	//		 
	//		 //将远端Candidate信息添加到PeerConnection
	//		 pc.addIceCandidate(candidate);
	//	}
	//	else 
	//	{
	//		console.log('the message is invalid!!!', data);
	//	}
	//});
	

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


/**
 功能： 打开音视频设备成功时的回调用
 返回值: true 

*/
function getMediaStream(stream)
{
	//将从设备上获取到的音视频track添加到localStream中
	if (localStream)
	{
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
		//pc.onicecandidate = (e) => {
		//	if (e.candidate)
		//	{
		//		console.log("candidate" + JSON.stringify(e.candidate.toJSON()));
		//		
		//		//将Candidate发送个对端
		//		sendMessage( {
		//			msg_id: 208,
		//			data: {
		//				type : 'candidate',
		//				label :event.candidate.sdpMLineIndex,
		//				id : event.candidate.sdpMid,
		//				candidate : event.candidate.candidate
		//			}
		//		 });
		//	}
		//	else 
		//	{
		//		console.log('this is the end candidate !!!');
		//	}
		//}
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