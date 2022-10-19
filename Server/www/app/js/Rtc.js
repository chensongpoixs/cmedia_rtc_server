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
        root.Rtc = factory(root.adapter);
    }
}(this, function ( ) {

        function Rtc()
        {

            // Map of RTCTransceivers indexed by MID.
            this._mapMidTransceiver = new Map();
            // Local stream for sending.
            this._sendStream = new MediaStream();
            // Whether a DataChannel m=application section has been created.
            this._hasDataChannelMediaSection = false;
            // Sending DataChannel id value counter. Incremented for each new DataChannel.
            this._nextSendSctpStreamId = 0;
            // Got transport local and remote parameters.
            this._transportReady = false;

            this._pc = null;

            /////////////////////////////////////////////////////////////////////////////////////////////////

            this._extendedRtpCapabilities = undefined;
            this._recvRtpCapabilities = undefined;
            this._canProduceByKind =
                {
                    audio: false,
                    video: false
                };
            this._sctpCapabilities = undefined;
            this.RtcSdpObj = new RtcSdp();
            this.commonSdpUtilsobj = new commonSdpUtils();
            this.ref_count = 0;
            console.log('[Rtc][constructor]');

            ///////////////////////////////////////////////////////////////////////////////////////////////////
            // public function 
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            this.close = function ()
            {
                console.log('[Rtc] close pc !!!');
                if (this._pc)
                {
                    try 
                    {
                        this._pc.close();
                    }
                    catch(error)
                    {
                        console.log('[Rtc] pc close catch failed !!!');
                    }
                }
            }


            this.CreateNativeRtpCapabilities = function()
            {
                console.log('[Rtc][CreateNativeRtpCapabilities]');
                 this._pc = new RTCPeerConnection({
                            iceServers: [],
                            iceTransportPolicy: 'all',
                            bundlePolicy: 'max-bundle',
                            rtcpMuxPolicy: 'require',
                            sdpSemantics: 'unified-plan'
                        });
                try {
                    this.ref_count++;
                    console.log('ref_count = ', this.ref_count);
                    this._pc.addTransceiver('audio');
                    this._pc.addTransceiver('video');
                    console.log('[Rtc][CreateNativeRtpCapabilities][addTransceiver]' );
                    /*const offer = await*/ this._pc.createOffer().then(this._handlerGetOffer).catch(this._handlerOfferError);
                    console.log('[Rtc][CreateNativeRtpCapabilities][createOffer]' );
                    /*
                    pc.createOffer(offerOptions)
                            .then(getOffer)
                            .catch(handleOfferError);
                    }
                    */
                    
                    // try {
                    //     pc.close();
                    // }
                    // catch (error) { }
                    // logger.debug('-------------------native  offer sdp ------------------------------------------');
                    // logger.debug(offer.sdp);
                    // const sdpObject = sdpTransform.parse(offer.sdp);
                    // logger.debug('-------------------native  sdpTransform sdp ------------------------------------------');
                    // logger.debug(JSON.stringify(sdpObject));
                    // logger.debug('-------------------native  sdpTransform sdp end ------------------------------------------');
                    // const nativeRtpCapabilities = sdpCommonUtils.extractRtpCapabilities({ sdpObject });
                    // logger.debug('-------------------native  sdpCommonUtils  extractRtpCapabilities sdp   ------------------------------------------');
                    // logger.debug(JSON.stringify(nativeRtpCapabilities));
                    // return nativeRtpCapabilities;
                }
                catch (error) {
                    try 
                    {
                        pc.close();
                        console.log('[Rtc][CreateNativeRtpCapabilities] catch pc close failed !!!');
                    }
                    catch (error2) 
                    { 
                         console.log('[Rtc][CreateNativeRtpCapabilities]catch pc close catch failed !!!');
                    }
                    // throw error;
                }
            }


            //////////////////////////////////////////////////////////////////////
            // private function
            //////////////////////////////////////////////////////////////////////
            /**
             *  pc.createOffer SDP --->
             */
           this. _handlerGetOffer = function (desc)
            {
                console.log('[Rtc][_handlerGetOffer] offer = ', desc);
                console.log('Rtc::_handlerGetOffer . ref_count = ', this.ref_count);
                // TODO@chensong 2022-10-18  特别好玩地方 居然在类构造函数中居然new对象没有用 ^_^ !!!
                if (!this.RtcSdpObj)
                {
                    this.RtcSdpObj = new RtcSdp();
                    //this.RtcSdpObj.test();
                    console.log('RtcSdp new ok !!!');
                    
                }
                const sdpObject =  this.RtcSdpObj.parse(desc.sdp);

                console.log('[Rtc][_handlerGetOffer][parse]sdpObject = ', sdpObject);
                if (!this.commonSdpUtilsobj)
                {
                    this.commonSdpUtilsobj = new commonSdpUtils();
                }
                 this._extendedRtpCapabilities =  this.commonSdpUtilsobj.extractRtpCapabilities({ sdpObject });
                 console.log('[Rtc][_handlerGetOffer][parse]this._extendedRtpCapabilities = ', this._extendedRtpCapabilities);
                 this._canProduceByKind.audio = this.canSend('audio', this._extendedRtpCapabilities); //this._extendedRtpCapabilities.codecs.some((codec) => codec.kind === 'audio');
                 this._canProduceByKind.video = this.canSend('video', this._extendedRtpCapabilities);//this._extendedRtpCapabilities.codecs.some((codec) => codec.kind === 'video');
                 // console.log('audio = ', audio);
                 // Generate our receiving RTP capabilities for receiving media.
                this._recvRtpCapabilities = getRecvRtpCapabilities(this._extendedRtpCapabilities);
                console.log('load() | got receiving RTP capabilities:%o', this._recvRtpCapabilities);

                //{ OS: 1024, MIS: 1024 }
                // Generate our SCTP capabilities.
                this._sctpCapabilities = {numStreams : { OS: 1024, MIS: 1024}};
                console.log('load() | got native SCTP capabilities:%o', this._sctpCapabilities);
            }

            this._handlerOfferError = function (err)
            {
                console.log('[Rtc][_handlerOfferError] err = ', err);
            }


             /*
              Whether media can be sent based on the given RTP capabilities.
             */
            canSend = function(kind, extendedRtpCapabilities) 
            {
                return extendedRtpCapabilities.codecs.
                    some((codec) => codec.kind === kind);
            }

            getRecvRtpCapabilities = function (extendedRtpCapabilities) 
            {
                // body...
                const rtpCapabilities = {
                    codecs: [],
                    headerExtensions: []
                };
                for (const extendedCodec of extendedRtpCapabilities.codecs) {
                    const codec = {
                        mimeType: extendedCodec.mimeType,
                        kind: extendedCodec.kind,
                        preferredPayloadType: extendedCodec.remotePayloadType,
                        clockRate: extendedCodec.clockRate,
                        channels: extendedCodec.channels,
                        parameters: extendedCodec.localParameters,
                        rtcpFeedback: extendedCodec.rtcpFeedback
                    };
                    rtpCapabilities.codecs.push(codec);
                    // Add RTX codec.
                    if (!extendedCodec.remoteRtxPayloadType)
                        continue;
                    const rtxCodec = {
                        mimeType: `${extendedCodec.kind}/rtx`,
                        kind: extendedCodec.kind,
                        preferredPayloadType: extendedCodec.remoteRtxPayloadType,
                        clockRate: extendedCodec.clockRate,
                        parameters: {
                            apt: extendedCodec.remotePayloadType
                        },
                        rtcpFeedback: []
                    };
                    rtpCapabilities.codecs.push(rtxCodec);
                    // TODO: In the future, we need to add FEC, CN, etc, codecs.
                }
                for (const extendedExtension of extendedRtpCapabilities.headerExtensions) {
                    // Ignore RTP extensions not valid for receiving.
                    if (extendedExtension.direction !== 'sendrecv' &&
                        extendedExtension.direction !== 'recvonly') {
                        continue;
                    }
                    const ext = {
                        kind: extendedExtension.kind,
                        uri: extendedExtension.uri,
                        preferredId: extendedExtension.recvId,
                        preferredEncrypt: extendedExtension.encrypt,
                        direction: extendedExtension.direction
                    };
                    rtpCapabilities.headerExtensions.push(ext);
                }
                return rtpCapabilities;
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

        return Rtc;
}));