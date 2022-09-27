(function (root, factory) {
    if (typeof define === 'function' && define.amd) {
        // AMD. Register as an anonymous module.
		//console.log('++++========++');
        define(["./adapter"], factory);
    } else if (typeof exports === 'object') {
        // Node. Does not work with strict CommonJS, but
        // only CommonJS-like environments that support module.exports,
        // like Node.
		//console.log('+++-------++++++');
        module.exports = factory(require("./adapter"));
    } else {
        // Browser globals (root is window)
		//console.log('+++++++++');
        root.device = factory(root.adapter);
    }
}(this, function (adapter) {

    function device() 
	{
    	 console.log('device');
        var self = this;
		self.client = null;
		
		///////////////////////////////////////////////////////////////////////////////////////
		// TODO@chensong 20220926 js中异步方法的使用同步问题
		// await  操作符用于等待一个Promise 对象。它只能在异步函数 async function 中使用。
		// 
		// 
		// 
		// 语法
		// [return_value] = await expression;
		// 表达式
		// 一个 Promise 对象或者任何要等待的值。
		// 
		// 返回值
		// 返回 Promise 对象的处理结果。如果等待的不是 Promise 对象，则返回该值本身。
		// 
		// 描述
		// await 表达式会暂停当前 async function 的执行，等待 Promise 处理完成。若 Promise 正常处理(fulfilled)，其回调的resolve函数参数作为 await 表达式的值，继续执行 async function。
		// 若 Promise 处理异常(rejected)，await 表达式会把 Promise 的异常原因抛出。
		// 另外，如果 await 操作符后的表达式的值不是一个 Promise，则返回该值本身。
		// 例子
		// 如果一个 Promise 被传递给一个 await 操作符，await 将等待 Promise 正常处理完成并返回其处理结果。
		// 
		// function resolveAfter2Seconds(x) {
		// return new Promise(resolve => {
		/// 	/setTimeout(() => {
		// 	/resolve(x);
		// 	/}, 2000);
		// });
		// }
		// 
		// async function f1() {
		// var x = await resolveAfter2Seconds(10);
		// console.log(x); // 10
		// }
		// f1();
		// 如果该值不是一个 Promise，await 会把该值转换为已正常处理的Promise，然后等待其处理结果。
		// 
		// async function f2() {
		// 
		// var y = await 20;
		// console.log(y); // 20
		// 
		// }
		// 
		// f2();
		// 如果 Promise 处理异常，则异常值被抛出。
		// 
		// async function f3() {
		// 
		// try {
		// 	/var z = await Promise.reject(30);
		// } catch (e) {
		// 	/console.log(e); // 30
		// }
		// }
		// 
		// f3();
		// 
		//////////////////////////////////////////////////////////////////////////////////////
		
        this.get_webrtc_sdp = async function()
		{
			console.log('device.get_webrtc_sdp');
			const pc  = new RTCPeerConnection( {iceServers: [],
						iceTransportPolicy: 'all',
						bundlePolicy: 'max-bundle',
						rtcpMuxPolicy: 'require',
						sdpSemantics: 'unified-plan' });
			try 
			{
				pc.addTransceiver('audio');
				pc.addTransceiver('video');
				const offer = await pc.createOffer();
				try 
				{
					pc.close();
				}
				catch (error) { }
				console.log('-------------------native  offer sdp ------------------------------------------');
				       //     offer.sdp = offer.sdp.replace('useinbandfec=1', 'useinbandfec=1;stereo=1;sprop-maxcapturerate=48000');

				console.log(offer.sdp);
             
				return offer.sdp;
			}		
			catch (error) 
			{
				console.log('--device throw !!! --- ');
				try 
				{
					pc.close();
				}
				catch (error2) { }
				throw error;
			}
		};
          
    };

    return device;
  
}));