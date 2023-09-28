var ws = require('nodejs-websocket');
var userMap = {};//服务器维护的客户端连接映射表
var customer = [];//所有客户端
var server = ws.createServer(function(socket){
    if(!dealConnect(socket)){
        console.log("客户端连接失败,没有填写必要的参数[name]")
        socket.sendText('连接失败,请输入昵称后重试');//给客户端返回消息
        socket.close();//关闭连接
        return;
    }
　  console.log("新连接到了");
    customer.push(socket);

　　// 前端传来的消息处理  事件名称为text，接受文本信息
    socket.on('text', function(str) {
        var data = JSON.parse(str);
        if(data.toUesr&&data.toUesr.length>0){
            socket.sendText("服务器端收到["+data.name+"]给["+data.toUesr+"]发送的消息："+data.txt);
            //向指定用户发
            sendMsgToUser(data);
        }else{
            console.log("收到前端消息:"+str)
            //广播
            server.connections.forEach(client=>{
                client.sendText("服务器端收到["+data.name+"]发送的消息："+data.txt);
            })
        }
    });
    // socket.on('connect', function(code) {
    //     console.log('开启连接', code)
    // })
    socket.on('error', function(code) {
        console.log('客户端异常关闭', code)
        socket.close();
        socket=null
    })
    // socket.on('close', function(code) {
    //     console.log('关闭连接', code)
    //     for (const key in customer) {
           
    //     }
    // })
    
}).listen(3000);//监听端口3000

//处理连接请求
function dealConnect(socket){
    console.log('new connect --> ', socket.path);
	if(socket.path&&socket.path.length>0)
	{
		
        let path = socket.path;
        let name = getValFromPathByKey(path,"peerId");
        //将拥有用户名的socket添加到用户连接表
        if(name){
            userMap[name] = socket;
            return true;//连接成功
        }
    }
    return false;//连接失败
}

//将消息发给指定用户
function sendMsgToUser(data){
    if(data){
        for (const user in userMap) {
            if(user==data.toUesr){
                userMap[user].sendText("用户"+data.name+"给你发消息了:"+data.txt);
            }
        }
    }
}

//根据key获取path中的参数
function getValFromPathByKey(path,key){
    if(path&&path.length>0){
        let paramsStr = path.substr(path.indexOf("?")+1);
        let KVStrArr = paramsStr.split("&");
        let params = {};
        if(KVStrArr&&KVStrArr.length>0){
            for (let i = 0; i < KVStrArr.length; i++) {
                const item = KVStrArr[i];
                let k = item.split("=")[0];
                let v = item.split("=")[1];
                params[""+k+""] = v;
            }
        }
        return params[key];
    }
    return null;
}
