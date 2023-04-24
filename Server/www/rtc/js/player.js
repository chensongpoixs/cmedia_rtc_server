'use strict'

//本地视频窗口

var localVideo = document.querySelector('video#localvideo');

//对端视频窗口
var remoteVideo = document.querySelector('video#remotevideo');

// connserver 连接服务器的标签
var btnConn = document.querySelector('button#connserver');


var btnRequestFrame = document.querySelector('button#RequestFrame');

//与信令服务器断开连接Button
var btnLeave = document.querySelector('button#leave');

// 查看Offer文本窗口
var offer = document.querySelector('textarea#offer');



//var message;
	//message.desc = desc.sdp;
	//offerdesc.offer_desc = desc.sdp;
	//offer_desc.roomname = "chensong";
	//offer_desc.peerid = "chensong";
	//将Offer SDP 发送给对端
	var room_name = getQueryVariable('roomname');
	var videopeerid	= getQueryVariable('videopeerid');
	var rtc_ip = getQueryVariable('rtc_ip');
var rtc_port = getQueryVariable('rtc_port');
// 查看Answer文本窗口
var answer  = document.querySelector('textarea#answer');

 

 
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

     var ws_url = "ws://"+rtc_ip +":"+rtc_port+"/?roomId=20220927&peerId=chensong";
	console.log('ws_url = ', ws_url);
     ws = new WebSocket(ws_url );

	 
	ws.onmessage = function(event)
    {
        
        console.log(`<- S2C : ${event.data}`);

        let msg = JSON.parse(event.data);
        // 
		console.log('recv message =  ', msg);
        if (msg.msg_id === 1075) // S2C_Login
        {
        	 answer.value = msg.data.sdp;
				
				//进行媒体协商
				//{type: 'answer', sdp: session.sdp}
		     	pc.setRemoteDescription(new RTCSessionDescription({type: 'answer', sdp: msg.data.sdp}));
			
			//console.log(ww);
			
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
			 
			console.log(' ws connect ok !!!');
			// 创建PeerConnection 并绑定音视频轨
			createPeerConnection();
			bindTracks();
			//setupDataChannel();
			//设置button状态
			btnConn.disabled = true;
			btnLeave.disabled = false;
			btnRequestFrame.disabled = false;
			call();
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
	//start();
	conn();
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
	/*
	if (!navigator.mediaDevices || !navigator.mediaDevices.getUserMedia)
	{
		console.log('the getUserMedia is not supported !!!');
		return;
	}
	*/
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
	/*navigator.mediaDevices.getUserMedia(constraints)
					.then(getMediaStream)
					.catch(handleError);
	*/
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
	
	
	
	
	sendMessage(
				{
					msg_id: 1074,
					data:  {
								offer : desc.sdp,
								roomname : room_name.toString(),
								video_peerid: videopeerid.toString(),
								peerid : Math.round(Math.random()*10000).toString()
						   } 
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
		pc = new RTCPeerConnection(null);
		
		 console.log(' new rtc sdk  core debugger  iceconnectionstatechange ---> [' +  pc.iceConnectionState + '] ^_^ !!!');
		 
		 pc.addEventListener('iceconnectionstatechange', () => {
			console.log(' +++ rtc sdk core debugger  iceconnectionstatechange ---> [' +  pc.iceConnectionState + '] ^_^ !!!');
               
            switch (pc.iceConnectionState) {
				 case 'checking':
                  console.log('iceconnectionstatechange ---> [' +  pc.iceConnectionState + '] ^_^ !!!');
					//this.emit('@connectionstatechange', 'connecting');
					
                    break;
                case 'connected':
                case 'completed':
					console.log('iceconnectionstatechange ---> [' +  pc.iceConnectionState + '] ^_^ !!!');
                    //this.emit('@connectionstatechange', 'connected');
                    break;
                case 'failed':
				console.log('iceconnectionstatechange ---> [' +  pc.iceConnectionState + '] ^_^ !!!');
                //    this.emit('@connectionstatechange', 'failed');
                    break;
                case 'disconnected':
				console.log('iceconnectionstatechange ---> [' +  pc.iceConnectionState + '] ^_^ !!!');
                  //  this.emit('@connectionstatechange', 'disconnected');
                    break;
                case 'closed':
				console.log('iceconnectionstatechange ---> [' +  pc.iceConnectionState + '] ^_^ !!!');
                //    this.emit('@connectionstatechange', 'closed');
                    break;
            }
        });
		 
		 
		  pc.addEventListener('icegatheringstatechange', (state) => {
			console.log(' +++ rtc sdk core debugger  icegatheringstatechange ---> [' +  pc.iceConnectionState + '] ^_^ !!!' , state);
               
            switch (pc.iceConnectionState) {
				 case 'checking':
                  console.log('iceconnectionstatechange ---> [' +  pc.iceConnectionState + '] ^_^ !!!');
					//this.emit('@connectionstatechange', 'connecting');
					
                    break;
                case 'connected':
                case 'completed':
					console.log('iceconnectionstatechange ---> [' +  pc.iceConnectionState + '] ^_^ !!!');
                    //this.emit('@connectionstatechange', 'connected');
                    break;
                case 'failed':
				console.log('iceconnectionstatechange ---> [' +  pc.iceConnectionState + '] ^_^ !!!');
                //    this.emit('@connectionstatechange', 'failed');
                    break;
                case 'disconnected':
				console.log('iceconnectionstatechange ---> [' +  pc.iceConnectionState + '] ^_^ !!!');
                  //  this.emit('@connectionstatechange', 'disconnected');
                    break;
                case 'closed':
				console.log('iceconnectionstatechange ---> [' +  pc.iceConnectionState + '] ^_^ !!!');
                //    this.emit('@connectionstatechange', 'closed');
                    break;
            }
        });
		 
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
	
	if (pc === null && remoteStream	=== undefined)
	{
		console.log('pc is null or undefined!!!');
		return;
	}
	
	 	pc.addTransceiver("audio", {direction: "recvonly"});
      pc.addTransceiver("video", {direction: "recvonly"});
	
	 
}



function setupDataChannel()
{
	console.log(' create datachannel ..... !!!');
	if (pc === null  )
	{
		console.log('pc is null or undefined!!!');
		return;
	}
	let datachannel = pc.createDataChannel('chat', {ordered: true});
    console.log(`Created datachannel `);
    // Inform browser we would like binary data as an ArrayBuffer (FF chooses Blob by default!)
    datachannel.binaryType = "arraybuffer";
    datachannel.onopen = function (e) {
      console.log(`data channel ( connect`);
      if(self.onDataChannelConnected){
        self.onDataChannelConnected();
      }
    }

    datachannel.onclose = function (e) {
      console.log(`data channel  closed`);
    }

    datachannel.onmessage = function (e) {
      console.log(`Got message  `, e.data);
       
    }

}




/**
 功能： 开启 "呼叫"
*/

function call()
{
	//if (state === 'joined_conn')
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
		pc.createOffer()
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
	btnRequestFrame.disabled	= false;
	
	
}



function RequestFrame()
{
	//var room_name = getQueryVariable('roomname');
	//var videopeerid	= getQueryVariable('videopeerid');
	sendMessage({
					msg_id: 1076,
					data: {
						roomname : room_name.toString(),
								video_peerid: videopeerid.toString()
					}
				});
	
}


//为Button设置单击事件
btnConn.onclick = connSignalServer;
btnLeave.onclick = leave;
btnRequestFrame.onclick = RequestFrame;