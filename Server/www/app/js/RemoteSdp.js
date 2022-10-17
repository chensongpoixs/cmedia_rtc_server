(function (root, factory) {
    if (typeof define === 'function' && define.amd) {
        // AMD. Register as an anonymous module.
        define(["./adapter"], factory);
    } else if (typeof exports === 'object') {
        // Node. Does not work with strict CommonJS, but
        // only CommonJS-like environments that support module.exports,
        // like Node.
        module.exports = factory(require("./adapter"));
    } else {
        // Browser globals (root is window)
        root.RemoteSdp = factory(root.adapter);
    }
}(this, function ( ) {
/**
TODO@chensong 2022-10-17 
iceParameters = {iceLite: true, password: 'zs5484zxgjhi0a07oh65lxoehc279j1w', usernameFragment: 'xqzyquhv6tv8m0qh'}
iceCandidates = {foundation: 'udpcandidate', ip: '192.168.1.73', port: 41382, priority: 1076302079, protocol: 'udp', type : 'host'}
dtlsParameters = fingerprints[{algorithm: 'sha-1', value: 'B6:97:F0:66:BC:1A:63:95:80:0B:DB:8F:FE:6A:81:57:D2:BD:E8:6B'},
								{algorithm: 'sha-224', value: '1D:85:0A:7E:D7:6D:EC:D0:15:1B:70:BE:C1:FD:2D:E1:87:49:5F:76:C5:6E:C8:CD:3F:94:AA:FA'},
								{algorithm: 'sha-256', value: '1C:CC:83:52:4A:96:52:7F:D2:94:BB:C0:40:96:AE:71:BB:98:81:55:F5:0B:B8:B6:5C:2D:4E:2F:44:2F:09:2F'},
								{algorithm: 'sha-384', value: '37:8E:F6:CA:AE:69:F5:6A:21:72:90:6F:40:13:9A:33:F0…6:09:BC:D1:FB:5F:B3:6F:7A:76:88:1F:68:99:24:6A:F0'},
								{algorithm: 'sha-512', value: '54:32:E9:9B:1E:AB:CA:F5:2B:61:80:B4:6B:60:70:1E:24…A:D9:49:18:56:59:23:60:48:BD:FB:2E:32:CF:F2:FA:61'}],
									role: "client"}
sctpParameters = {MIS: 1024, OS: 1024, isDataChannel: true, maxMessageSize: 262144, port: 5000, sctpBufferedAmount:0, sendBufferSize:262144}

*/

	function RemoteSdp({ iceParameters, iceCandidates, dtlsParameters, sctpParameters, plainRtpParameters, planB = false })
	{
		// MediaSection instances with same order as in the SDP.
	        this._mediaSections = [];
	        // MediaSection indices indexed by MID.
	        this._midToIndex = new Map();
	        this._iceParameters = iceParameters;
	        this._iceCandidates = iceCandidates;
	        this._dtlsParameters = dtlsParameters;
	        this._sctpParameters = sctpParameters;
	        this._plainRtpParameters = plainRtpParameters;
	        this._planB = planB;
	       // this.logger = new Logger();

	        this._sdpObject =
	            {
	                version: 0,
	                origin: {
	                    address: '0.0.0.0',
	                    ipVer: 4,
	                    netType: 'IN',
	                    sessionId: 10000,
	                    sessionVersion: 0,
	                    username: 'mediasoup-client'
	                },
	                name: '-',
	                timing: { start: 0, stop: 0 },
	                media: []
	            };
	        // If ICE parameters are given, add ICE-Lite indicator.
	        if (iceParameters && iceParameters.iceLite) 
	        {
	            this._sdpObject.icelite = 'ice-lite';
	        }
	        // If DTLS parameters are given, assume WebRTC and BUNDLE.
	        if (dtlsParameters) 
	        {
	            this._sdpObject.msidSemantic = { semantic: 'WMS', token: '*' };
	            // NOTE: We take the latest fingerprint.
	            const numFingerprints = this._dtlsParameters.fingerprints.length;
	            this._sdpObject.fingerprint =
	                {
	                    type: dtlsParameters.fingerprints[numFingerprints - 1].algorithm,
	                    hash: dtlsParameters.fingerprints[numFingerprints - 1].value
	                };
	            this._sdpObject.groups = [{ type: 'BUNDLE', mids: '' }];
	        }
	        // If there are plain RPT parameters, override SDP origin.
	        if (plainRtpParameters) 
	        {
	            this._sdpObject.origin.address = plainRtpParameters.ip;
	            this._sdpObject.origin.ipVer = plainRtpParameters.ipVersion;
	        }
			
			/////////////////////////////////////////////////////////////////
			/// JS event 驱动
			 // 维护事件及监听者
	        this.listeners = {};

			/////////////////////////////////////////////////////////////////

			console.log('RemoteSdp constructor !!!');







	        //************************************************
	        // Functions
	        //************************************************
	        this.updateIceParameters = function(iceParameters)
	        {
	        	console.log('[updateIceParameters] iceParameters = ', iceParameters);
			 	this._iceParameters = iceParameters;
		        this._sdpObject.icelite = iceParameters.iceLite ? 'ice-lite' : undefined;
		        for (const mediaSection of this._mediaSections) 
		        {
		            mediaSection.setIceParameters(iceParameters);
		        }
	        }
	        this.updateDtlsRole = function(role) 
	        {
		        console.log('updateDtlsRole() [role:%s]', role);
		        this._dtlsParameters.role = role;
		        for (const mediaSection of this._mediaSections) 
		        {
		            mediaSection.setDtlsRole(role);
		        }
		    }

		    this.getNextMediaSectionIdx = function()
		    {
		        // If a closed media section is found, return its index.
		        for (let idx = 0; idx < this._mediaSections.length; ++idx) 
		        {
		            const mediaSection = this._mediaSections[idx];
		            if (mediaSection.closed)
		            {
		            	return { idx, reuseMid: mediaSection.mid };
		            }
		        }
		        // If no closed media section is found, return next one.
		        return { idx: this._mediaSections.length };
		    }

		 	this.send = function({ offerMediaObject, reuseMid, offerRtpParameters, answerRtpParameters, codecOptions, extmapAllowMixed = false }) 
		 	{


		        const mediaSection = new AnswerMediaSection({
		            iceParameters: this._iceParameters,
		            iceCandidates: this._iceCandidates,
		            dtlsParameters: this._dtlsParameters,
		            plainRtpParameters: this._plainRtpParameters,
		            planB: this._planB,
		            offerMediaObject,
		            offerRtpParameters,
		            answerRtpParameters,
		            codecOptions,
		            extmapAllowMixed
		        });
		        // Unified-Plan with closed media section replacement.
		        if (reuseMid) 
		        {
		            this._replaceMediaSection(mediaSection, reuseMid);
		        }
		        // Unified-Plan or Plan-B with different media kind.
		        else if (!this._midToIndex.has(mediaSection.mid())) 
		        {
		            this._addMediaSection(mediaSection);
		        }
		        // Plan-B with same media kind.
		        else
		        {
		            this._replaceMediaSection(mediaSection);
		        }
		    }


		    this.receive = function({ mid, kind, offerRtpParameters, streamId, trackId }) 
		    {
		        const idx = this._midToIndex.get(mid);
		        let mediaSection;
		        if (idx !== undefined)
		        {
		        	mediaSection = this._mediaSections[idx];
		        }
		        // Unified-Plan or different media kind.
		        if (!mediaSection) 
		        {
		            mediaSection = new OfferMediaSection({
		                iceParameters: this._iceParameters,
		                iceCandidates: this._iceCandidates,
		                dtlsParameters: this._dtlsParameters,
		                plainRtpParameters: this._plainRtpParameters,
		                planB: this._planB,
		                mid,
		                kind,
		                offerRtpParameters,
		                streamId,
		                trackId
		            });
		            // Let's try to recycle a closed media section (if any).
		            // NOTE: Yes, we can recycle a closed m=audio section with a new m=video.
		            const oldMediaSection = this._mediaSections.find((m) => (m.closed));
		            if (oldMediaSection) 
		            {
		                this._replaceMediaSection(mediaSection, oldMediaSection.mid());
		            }
		            else {
		                this._addMediaSection(mediaSection);
		            }
		        }
		        // Plan-B.
		        else {
		            mediaSection.planBReceive({ offerRtpParameters, streamId, trackId });
		            this._replaceMediaSection(mediaSection);
		        }
		    }

		    this.disableMediaSection = function(mid)
		    {
		        const idx = this._midToIndex.get(mid);
		        if (idx === undefined) 
		        {
		            throw new Error(`no media section found with mid '${mid}'`);
		        }
		        const mediaSection = this._mediaSections[idx];
		        mediaSection.disable();
		    }
		    this.closeMediaSection = function(mid) 
		    {
		        const idx = this._midToIndex.get(mid);
		        if (idx === undefined) 
		        {
		            throw new Error(`no media section found with mid '${mid}'`);
		        }
		        const mediaSection = this._mediaSections[idx];
		        // NOTE: Closing the first m section is a pain since it invalidates the
		        // bundled transport, so let's avoid it.
		        if (mid === this._firstMid) 
		        {
		            logger.debug('closeMediaSection() | cannot close first media section, disabling it instead [mid:%s]', mid);
		            this.disableMediaSection(mid);
		            return;
		        }
		        mediaSection.close();
		        // Regenerate BUNDLE mids.
		        this._regenerateBundleMids();
		    }
		    this.planBStopReceiving = function({ mid, offerRtpParameters }) 
		    {
		        const idx = this._midToIndex.get(mid);
		        if (idx === undefined) 
		        {
		            throw new Error(`no media section found with mid '${mid}'`);
		        }
		        const mediaSection = this._mediaSections[idx];
		        mediaSection.planBStopReceiving({ offerRtpParameters });
		        this._replaceMediaSection(mediaSection);
		    }
		    this.sendSctpAssociation = function({ offerMediaObject }) 
		    {
		        const mediaSection = new AnswerMediaSection({
		            iceParameters: this._iceParameters,
		            iceCandidates: this._iceCandidates,
		            dtlsParameters: this._dtlsParameters,
		            sctpParameters: this._sctpParameters,
		            plainRtpParameters: this._plainRtpParameters,
		            offerMediaObject
		        });
		        this._addMediaSection(mediaSection);
		    }
		    this.receiveSctpAssociation = function({ oldDataChannelSpec = false } = {}) 
		    {
		        const mediaSection = new OfferMediaSection({
		            iceParameters: this._iceParameters,
		            iceCandidates: this._iceCandidates,
		            dtlsParameters: this._dtlsParameters,
		            sctpParameters: this._sctpParameters,
		            plainRtpParameters: this._plainRtpParameters,
		            mid: 'datachannel',
		            kind: 'application',
		            oldDataChannelSpec
		        });
		        this._addMediaSection(mediaSection);
		    }
		    this.getSdp = function() 
		    {
		        // Increase SDP version.
		        this._sdpObject.origin.sessionVersion++;
		        return sdpTransform.write(this._sdpObject);
		    }




		    this.test = function()
		    {
		    	const mediaSection = new AnswerMediaSection();
		    	console.log('AnswerMediaSection');
		    }

	        ////////////////////////////////////////////////////////
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
	 
	 
			///////////////////////////////////////////////////////////////////
			// private function 
			///////////////////////////////////////////////////////////////////

		    _addMediaSection = function(newMediaSection) 
		    {
		        if (!this._firstMid)
		        {
		        	this._firstMid = newMediaSection.mid();
		        }
		        // Add to the vector.
		        this._mediaSections.push(newMediaSection);
		        // Add to the map.
		        this._midToIndex.set(newMediaSection.mid(), this._mediaSections.length - 1);
		        // Add to the SDP object.
		        this._sdpObject.media.push(newMediaSection.getObject());
		        // Regenerate BUNDLE mids.
		        this._regenerateBundleMids();
		    }
			 _replaceMediaSection = function(newMediaSection, reuseMid) 
			 {
		        // Store it in the map.
		        if (typeof reuseMid === 'string') 
		        {
		            const idx = this._midToIndex.get(reuseMid);
		            if (idx === undefined) 
		            {
		                throw new Error(`no media section found for reuseMid '${reuseMid}'`);
		            }
		            const oldMediaSection = this._mediaSections[idx];
		            // Replace the index in the vector with the new media section.
		            this._mediaSections[idx] = newMediaSection;
		            // Update the map.
		            this._midToIndex.delete(oldMediaSection.mid);
		            this._midToIndex.set(newMediaSection.mid(), idx);
		            // Update the SDP object.
		            this._sdpObject.media[idx] = newMediaSection.getObject();
		            // Regenerate BUNDLE mids.
		            this._regenerateBundleMids();
		        }
		        else 
		        {
		            const idx = this._midToIndex.get(newMediaSection.mid);
		            if (idx === undefined)
		            {
		                throw new Error(`no media section found with mid '${newMediaSection.mid}'`);
		            }
		            // Replace the index in the vector with the new media section.
		            this._mediaSections[idx] = newMediaSection;
		            // Update the SDP object.
		            this._sdpObject.media[idx] = newMediaSection.getObject();
		        }
		    }

		    _regenerateBundleMids = function() 
		    {
		        if (!this._dtlsParameters)
		        {
		        	return;
		        }
		        this._sdpObject.groups[0].mids = this._mediaSections
		            .filter((mediaSection) => !mediaSection.closed)
		            .map((mediaSection) => mediaSection.mid)
		            .join(' ');
		    }
	};

	return RemoteSdp;
}));