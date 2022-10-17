(function (root, factory) {
    if (typeof define === 'function' && define.amd) 
    {
        // AMD. Register as an anonymous module.
        define(["./adapter"], factory);
    } 
    else if (typeof exports === 'object') 
    {
        // Node. Does not work with strict CommonJS, but
        // only CommonJS-like environments that support module.exports,
        // like Node.
        module.exports = factory(require("./adapter"));
    } 
    else 
    {
        // Browser globals (root is window)
        root.Transport = factory(root.adapter);
    }
}(this, function ( ) {

    function Transport({ direction, id, iceParameters, iceCandidates, dtlsParameters, sctpParameters, iceServers, iceTransportPolicy, additionalSettings, proprietaryConstraints, appData, handlerFactory, extendedRtpCapabilities, canProduceByKind }) 
    {
       // super();
       // Closed flag.
        this._closed = false;
        // Transport connection state.
        this._connectionState = 'new';
        // Map of Producers indexed by id.
        this._producers = new Map();
        // Map of Consumers indexed by id.
        this._consumers = new Map();
        // Map of DataProducers indexed by id.
        this._dataProducers = new Map();
        // Map of DataConsumers indexed by id.
        this._dataConsumers = new Map();
        // Whether the Consumer for RTP probation has been created.
        this._probatorConsumerCreated = false;
        // AwaitQueue instance to make async tasks happen sequentially.
     //   this._awaitQueue = new awaitqueue_1.AwaitQueue({ ClosedErrorClass: errors_1.InvalidStateError });
        // Observer instance.
      //  this._observer = new EnhancedEventEmitter_1.EnhancedEventEmitter();
       // logger.debug('constructor() [id:%s, direction:%s]', id, direction);
        this._id = id;
        this._direction = direction;
        this._extendedRtpCapabilities = extendedRtpCapabilities;
        this._canProduceByKind = canProduceByKind;
        this._maxSctpMessageSize =  sctpParameters ? sctpParameters.maxMessageSize : null;
        this._appData = appData;
        this._remoteSdp = null;
        this._pc = null;


        ///////////////////////////////////

         this._remoteSdp = new RemoteSdp({
                iceParameters,
                iceCandidates,
                dtlsParameters,
                sctpParameters
            });
            console.log('RTCPeerConnection new --------->');
            this._pc = new RTCPeerConnection(Object.assign({ iceServers: iceServers || [], iceTransportPolicy: iceTransportPolicy || 'all', 
                bundlePolicy: 'max-bundle', rtcpMuxPolicy: 'require', sdpSemantics: 'unified-plan' }, additionalSettings), proprietaryConstraints);

            // Handle RTCPeerConnection connection status.
            this._pc.addEventListener('iceconnectionstatechange', () => {
                console.log(' +++ mediasoup core debugger  iceconnectionstatechange ---> [' +  this._pc.iceConnectionState + '] ^_^ !!!');
                console.log('RTCPeerConnection addEventListener  --------->');
                // 分布订阅模式   
                switch (this._pc.iceConnectionState) {
                     case 'checking':
                      //  console.log('iceconnectionstatechange ---> [' +  this._pc.iceConnectionState + '] ^_^ !!!');
                        this.emit('@connectionstatechange', 'connecting');
                        
                        break;
                    case 'connected':
                    case 'completed':
                        //console.log('iceconnectionstatechange ---> [' +  this._pc.iceConnectionState + '] ^_^ !!!');
                        this.emit('@connectionstatechange', 'connected');
                        break;
                    case 'failed':
                    //console.log('iceconnectionstatechange ---> [' +  this._pc.iceConnectionState + '] ^_^ !!!');
                        this.emit('@connectionstatechange', 'failed');
                        break;
                    case 'disconnected':
                    //console.log('iceconnectionstatechange ---> [' +  this._pc.iceConnectionState + '] ^_^ !!!');
                        this.emit('@connectionstatechange', 'disconnected');
                        break;
                    case 'closed':
                    //console.log('iceconnectionstatechange ---> [' +  this._pc.iceConnectionState + '] ^_^ !!!');
                        this.emit('@connectionstatechange', 'closed');
                        break;
                }
            });
      

       // this._test();
        // this._work({
        //     direction,
        //     iceParameters,
        //     iceCandidates,
        //     dtlsParameters,
        //     sctpParameters,
        //     iceServers,
        //     iceTransportPolicy,
        //     additionalSettings,
        //     proprietaryConstraints,
        //     extendedRtpCapabilities
        // });
        // Clone and sanitize additionalSettings.
       // additionalSettings = utils.clone(additionalSettings, {});
        //delete additionalSettings.iceServers;
        //delete additionalSettings.iceTransportPolicy;
       // delete additionalSettings.bundlePolicy;
       // delete additionalSettings.rtcpMuxPolicy;
       // delete additionalSettings.sdpSemantics;
        //this._handler = handlerFactory();
       // this._handler.run({
       //     direction,
       /*     iceParameters,
            iceCandidates,
            dtlsParameters,
            sctpParameters,
            iceServers,
            iceTransportPolicy,
            additionalSettings,
            proprietaryConstraints,
            extendedRtpCapabilities
        });
        this._appData = appData;
        this._handleHandler();
        */


        ///////////////////////////////////////////////////////////////////////////////////////
        // private function
        ///////////////////////////////////////////////////////////////////////////////////////

       this. _test = function () 
        {
            console.log('_sendTrasnport test !!!');
            
        }
        
        this._work = function({ direction, iceParameters, iceCandidates, dtlsParameters, sctpParameters, iceServers, iceTransportPolicy, additionalSettings, proprietaryConstraints, extendedRtpCapabilities = {} })
        {
            this._remoteSdp = new RemoteSdp({
                iceParameters,
                iceCandidates,
                dtlsParameters,
                sctpParameters
            });

            this._pc = new RTCPeerConnection(Object.assign({ iceServers: iceServers || [], iceTransportPolicy: iceTransportPolicy || 'all', 
                bundlePolicy: 'max-bundle', rtcpMuxPolicy: 'require', sdpSemantics: 'unified-plan' }, additionalSettings), proprietaryConstraints);

            // Handle RTCPeerConnection connection status.
            this._pc.addEventListener('iceconnectionstatechange', () => {
                console.log(' +++ mediasoup core debugger  iceconnectionstatechange ---> [' +  this._pc.iceConnectionState + '] ^_^ !!!');
                // 分布订阅模式   
                switch (this._pc.iceConnectionState) {
                     case 'checking':
                      //  console.log('iceconnectionstatechange ---> [' +  this._pc.iceConnectionState + '] ^_^ !!!');
                        this.emit('@connectionstatechange', 'connecting');
                        
                        break;
                    case 'connected':
                    case 'completed':
                        //console.log('iceconnectionstatechange ---> [' +  this._pc.iceConnectionState + '] ^_^ !!!');
                        this.emit('@connectionstatechange', 'connected');
                        break;
                    case 'failed':
                    //console.log('iceconnectionstatechange ---> [' +  this._pc.iceConnectionState + '] ^_^ !!!');
                        this.emit('@connectionstatechange', 'failed');
                        break;
                    case 'disconnected':
                    //console.log('iceconnectionstatechange ---> [' +  this._pc.iceConnectionState + '] ^_^ !!!');
                        this.emit('@connectionstatechange', 'disconnected');
                        break;
                    case 'closed':
                    //console.log('iceconnectionstatechange ---> [' +  this._pc.iceConnectionState + '] ^_^ !!!');
                        this.emit('@connectionstatechange', 'closed');
                        break;
                }
            });
      
        }







        ////////////////////JS envent 驱动////////////////////////////

            /**
             * 注册事件监听者
             * @param {String} type 事件类型
             * @param {Function} callback 回调函数
             */
            this.on   = function(type, callback) 
            {
                if (!this.listeners[type]) 
                {  // 如果该事件类型不存在
                    this.listeners[type] = [] // 为该事件类型设置数组，存放回调函数
                }
                this.listeners[type].push(callback) // 将回调函数放入该事件类型数组
            }
            /**
             * 发布事件
             * @param {String} type 事件类型
             * @param  {...any} args 参数列表，把emit传递的参数赋给回调函数
             */
            this.emit = function(type, ...args) 
            {
                if (this.listeners[type]) 
                { // 如果该事件类型存在
                    this.listeners[type].forEach(callback => {
                        callback(...args) // 调用该事件类型数组中的每一个回调函数，并传入参数
                    })
                }
            }
            /**
             * 移除某个事件的一个监听者
             * @param {String} type 事件类型
             * @param {Function} callback 回调函数
             */
            this.off = function(type, callback) 
            {
                if (this.listeners[type]) {
                    // 查询传入回调函数在该事件类型数组中的下标，并将其下标用targetIndex存储
                    const targetIndex = this.listeners[type].findIndex(item => item === callback)
                    if (targetIndex !== -1) { // 说明该回调函数存在于事件类型数组中
                        this.listeners[type].splice(targetIndex, 1) // 删除该回调函数
                    }
                    if (this.listeners[type].length === 0) { // 该事件类型数组为空
                        delete this.listeners[type] // 删除该事件类型
                    }
                }
            }
            /**
             * 移除某个事件的所有监听者
             * @param {String} type 事件类型
             */
            this.offAll = function(type) 
            {
                if (this.listeners[type]) 
                { // 如果该事件类型数组存在
                    delete this.listeners[type] // 直接删除该事件类型
                }
            }
     
    };
    return Transport;
}));