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
        root.commonSdpUtils = factory(root.adapter);
    }
}(this, function ( ) {

        function commonSdpUtils()
        {
            this.RtcSdpObj = new RtcSdp();
           

           this.extractRtpCapabilities = function({ sdpObject })
           {
                // Map of RtpCodecParameters indexed by payload type.
                const codecsMap = new Map();
                // Array of RtpHeaderExtensions.
                const headerExtensions = [];
                // Whether a m=audio/video section has been already found.
                let gotAudio = false;
                let gotVideo = false;
                for (const m of sdpObject.media) {
                    const kind = m.type;
                    switch (kind) {
                        case 'audio':
                            {
                                if (gotAudio)
                                    continue;
                                gotAudio = true;
                                break;
                            }
                        case 'video':
                            {
                                if (gotVideo)
                                    continue;
                                gotVideo = true;
                                break;
                            }
                        default:
                            {
                                continue;
                            }
                    }
                    // Get codecs.
                    for (const rtp of m.rtp) {
                        const codec = {
                            kind: kind,
                            mimeType: `${kind}/${rtp.codec}`,
                            preferredPayloadType: rtp.payload,
                            clockRate: rtp.rate,
                            channels: rtp.encoding,
                            parameters: {},
                            rtcpFeedback: []
                        };
                        codecsMap.set(codec.preferredPayloadType, codec);
                    }
                    // Get codec parameters.
                    for (const fmtp of m.fmtp || []) 
                    {
                        if (null === this.RtcSdpObj )
                        {
                            this.RtcSdpObj = new RtcSdp();
                        }
                        const parameters = this.RtcSdpObj.parseParams(fmtp.config);
                        const codec = codecsMap.get(fmtp.payload);
                        if (!codec)
                            continue;
                        // Specials case to convert parameter value to string.
                        if (parameters && parameters.hasOwnProperty('profile-level-id'))
                            parameters['profile-level-id'] = String(parameters['profile-level-id']);
                        codec.parameters = parameters;
                    }
                    // Get RTCP feedback for each codec.
                    for (const fb of m.rtcpFb || []) {
                        const codec = codecsMap.get(fb.payload);
                        if (!codec)
                            continue;
                        const feedback = {
                            type: fb.type,
                            parameter: fb.subtype
                        };
                        if (!feedback.parameter)
                            delete feedback.parameter;
                        codec.rtcpFeedback.push(feedback);
                    }
                    // Get RTP header extensions.
                    for (const ext of m.ext || []) {
                        // Ignore encrypted extensions (not yet supported in mediasoup).
                        if (ext['encrypt-uri'])
                            continue;
                        const headerExtension = {
                            kind: kind,
                            uri: ext.uri,
                            preferredId: ext.value
                        };
                        headerExtensions.push(headerExtension);
                    }
                }
                const rtpCapabilities = {
                    codecs: Array.from(codecsMap.values()),
                    headerExtensions: headerExtensions
                };
                return rtpCapabilities;
            } 
        }
        

        return commonSdpUtils;
}));