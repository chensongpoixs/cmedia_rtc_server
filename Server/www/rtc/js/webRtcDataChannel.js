// Copyright Epic Games, Inc. All Rights Reserved.
// universal module definition - read https://www.davidbcalhoun.com/2014/what-is-amd-commonjs-and-umd/

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
        root.webRtcDataChannel = factory(root.adapter);
    }
}(this, function (adapter) {

    function webRtcDataChannel(parOptions) {
    	parOptions = typeof parOptions !== 'undefined' ? parOptions : {};
    	
        var self = this;

        //**********************
        //Config setup
        //**********************
		this.cfg = typeof parOptions.peerConnectionOptions !== 'undefined' ? parOptions.peerConnectionOptions : {};
		this.cfg.sdpSemantics = 'unified-plan';
        // this.cfg.rtcAudioJitterBufferMaxPackets = 10;
        // this.cfg.rtcAudioJitterBufferFastAccelerate = true;
        // this.cfg.rtcAudioJitterBufferMinDelayMs = 0;

		// If this is true in Chrome 89+ SDP is sent that is incompatible with UE Pixel Streaming 4.26 and below.
        // However 4.27 Pixel Streaming does not need this set to false as it supports `offerExtmapAllowMixed`.
        // tdlr; uncomment this line for older versions of Pixel Streaming that need Chrome 89+.
        this.cfg.offerExtmapAllowMixed = false;

        //**********************
        //Variables
        //**********************
        this.pcClient = null;
        this.dcClient = null;
        this.tnClient = null;

        this.sdpConstraints = {
          offerToReceiveAudio: 0, //Note: if you don't need audio you can get improved latency by turning this off.
          offerToReceiveVideo: 0,
          voiceActivityDetection: false
        };

        // See https://www.w3.org/TR/webrtc/#dom-rtcdatachannelinit for values (this is needed for Firefox to be consistent with Chrome.)
        this.dataChannelOptions = {ordered: true};

        // This is useful if the video/audio needs to autoplay (without user input) as browsers do not allow autoplay non-muted of sound sources without user interaction.
        this.startVideoMuted = typeof parOptions.startVideoMuted !== 'undefined' ? parOptions.startVideoMuted : false;
        this.autoPlayAudio = typeof parOptions.autoPlayAudio !== 'undefined' ? parOptions.autoPlayAudio : false;

        

        //**********************
        //Functions
        //**********************

        

       
        onsignalingstatechange = function(state) {
            console.info('signaling state change:', state)
        };

        oniceconnectionstatechange = function(state) {
            console.info('ice connection state change:', state)
        };

        onicegatheringstatechange = function(state) {
            console.info('ice gathering state change:', state)
        };

        
        setupDataChannel = function(pc, label, options) {
            try {
                let datachannel = pc.createDataChannel(label, options);
                console.log(`Created datachannel (${label})`)

                // Inform browser we would like binary data as an ArrayBuffer (FF chooses Blob by default!)
                datachannel.binaryType = "arraybuffer";
                
                datachannel.onopen = function (e) {
                  console.log(`data channel (${label}) connect`)
                  if(self.onDataChannelDataChannelOConnected){
                    self.onDataChannelDataChannelOConnected();
                  }
                }

                datachannel.onclose = function (e) {
                  console.log(`data channel (${label}) closed`)
                }

                datachannel.onmessage = function (e) {
                  //console.log(`Got message (${label})`, e.data)
                 // if (self.onDataChannelMessage)
                   // self.onDataChannelMessage(e.data);
                }

                return datachannel;
            } catch (e) { 
                console.warn('No data channel', e);
                return null;
            }
        }

        onicecandidate = function (e) {
			console.log('ICE candidate', e)
			if (e.candidate && e.candidate.candidate) {
                self.onWebRtcDataChannelOCandidate(1, e.candidate);
            }
        };

        handleCreateOffer = function (pc) {
            pc.createOffer(self.sdpConstraints).then(function (offer) {
                
                // Munging is where we modifying the sdp string to set parameters that are not exposed to the browser's WebRTC API
                mungeSDPOffer(offer);

                // Set our munged SDP on the local peer connection so it is "set" and will be send across
            	pc.setLocalDescription(offer);
            	if (self.onWebRtcDataChannelOffer) {
                    console.log('rtctype == 1-');
            		self.onWebRtcDataChannelOffer(1, offer);
                }
                else
                {
                    console.warn('not find DataChannel Offer callback send msg function ');
                }
            },
            function () { console.warn("Couldn't create offer") });
        }

        mungeSDPOffer = function (offer) {

            // turn off video-timing sdp sent from browser
            //offer.sdp = offer.sdp.replace("http://www.webrtc.org/experiments/rtp-hdrext/playout-delay", "");

            // this indicate we support stereo (Chrome needs this)
            offer.sdp = offer.sdp.replace('useinbandfec=1', 'useinbandfec=1;stereo=1;sprop-maxcapturerate=48000');

        }
        
        setupPeerConnection = function (pc) {
        	if (pc.SetBitrate)
        		console.log("Hurray! there's RTCPeerConnection.SetBitrate function");

            //Setup peerConnection events
            pc.onsignalingstatechange = onsignalingstatechange;
            pc.oniceconnectionstatechange = oniceconnectionstatechange;
            pc.onicegatheringstatechange = onicegatheringstatechange;

            pc.ontrack = handleOnTrack;
            pc.onicecandidate = onicecandidate;
        };

        
        setupTracksToSendAsync = async function(pc){
            
            // Setup a transceiver for getting UE video
            pc.addTransceiver("video", { direction: "recvonly" });

            // Setup a transceiver for sending mic audio to UE and receiving audio from UE
            if(!self.useMic)
            {
                pc.addTransceiver("audio", { direction: "recvonly" });
            }
            else
            {
                let audioSendOptions = self.useMic ? 
                {
                    autoGainControl: false,
                    channelCount: 1,
                    echoCancellation: false,
                    latency: 0,
                    noiseSuppression: false,
                    sampleRate: 16000,
                    volume: 1.0
                } : false;

                // Note using mic on android chrome requires SSL or chrome://flags/ "unsafely-treat-insecure-origin-as-secure"
                const stream = await navigator.mediaDevices.getUserMedia({video: false, audio: audioSendOptions});
                if(stream)
                {
                    for (const track of stream.getTracks()) {
                        if(track.kind && track.kind == "audio")
                        {
                            pc.addTransceiver(track, { direction: "sendrecv" });
                        }
                    }
                }
                else
                {
                    pc.addTransceiver("audio", { direction: "recvonly" });
                }
            }
        };


        //**********************
        //Public functions
        //**********************

        this.setVideoEnabled = function(enabled) {
            self.video.srcObject.getTracks().forEach(track => track.enabled = enabled);
        }

       //This is called when revceiving new ice candidates individually instead of part of the offer
        //This is currently not used but would be called externally from this class
        this.handleCandidateFromServer = function(iceCandidate) {
            console.log("ICE candidate: ", iceCandidate);
            
            //?????Candidate??
            // var candidate = new RTCIceCandidate({
            //   sdpMLineIndex :data.label,
            //   candidate:data.candidate
            // });
            //
            let candidate = new RTCIceCandidate(iceCandidate);
            self.pcClient.addIceCandidate(candidate).then(_=>{
                console.log('ICE candidate successfully added');
            });
        };

        //Called externaly to create an offer for the server
        this.createOffer = function() {
            if(self.pcClient){
                console.log("Closing existing PeerConnection")
                self.pcClient.close();
                self.pcClient = null;
            }
            self.pcClient = new RTCPeerConnection(self.cfg);
			setupPeerConnection(self.pcClient);
			self.dcClient = setupDataChannel(self.pcClient, 'cirrus', self.dataChannelOptions);
			handleCreateOffer(self.pcClient);
            //setupTracksToSendAsync(self.pcClient).finally(function()
            //{
            //    setupPeerConnection(self.pcClient);
            //    self.dcClient = setupDataChannel(self.pcClient, 'cirrus', self.dataChannelOptions);
            //    handleCreateOffer(self.pcClient);
            //});
            
        };

        //Called externaly when an answer is received from the server
        this.receiveAnswer = function(answer) {
            console.log('Received answer:');
            console.log(answer);
            var answerDesc = new RTCSessionDescription(answer);
            self.pcClient.setRemoteDescription(answerDesc);

            let receivers = self.pcClient.getReceivers();
            for(let receiver of receivers)
            {
                receiver.playoutDelayHint = 0;
            }
        };

        this.close = function(){
            if(self.pcClient){
                console.log("Closing existing peerClient")
                self.pcClient.close();
                self.pcClient = null;
            }
            if(self.aggregateStatsIntervalId)
                clearInterval(self.aggregateStatsIntervalId);
        }

        //Sends data across the datachannel
        this.send = function(data){
            if(self.dcClient && self.dcClient.readyState == 'open'){
                //console.log('Sending data on dataconnection', self.dcClient)
                self.dcClient.send(data);
            }
        };

        this.getStats = function(onStats){
            if(self.pcClient && onStats){
                self.pcClient.getStats(null).then((stats) => { 
                    onStats(stats); 
                });
            }
        }

        this.aggregateStats = function(checkInterval){
            let calcAggregatedStats = generateAggregatedStatsFunction();
            let printAggregatedStats = () => { self.getStats(calcAggregatedStats); }
            self.aggregateStatsIntervalId = setInterval(printAggregatedStats, checkInterval);
        }
    };

    return webRtcDataChannel;
  
}));
