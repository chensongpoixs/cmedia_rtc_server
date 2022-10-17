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
        root.OfferMediaSection = factory(root.adapter);
    }
}(this, function ( ) {

            function OfferMediaSection({ iceParameters, iceCandidates, dtlsParameters, sctpParameters, plainRtpParameters, planB = false, mid, kind, offerRtpParameters, streamId, trackId, oldDataChannelSpec = false }) 
            {
                this._mediaObject = {};
                this._planB = planB;
                if (iceParameters) {
                    this.setIceParameters(iceParameters);
                }
                if (iceCandidates) {
                    this._mediaObject.candidates = [];
                    for (const candidate of iceCandidates) {
                        const candidateObject = {};
                        // mediasoup does mandates rtcp-mux so candidates component is always
                        // RTP (1).
                        candidateObject.component = 1;
                        candidateObject.foundation = candidate.foundation;
                        candidateObject.ip = candidate.ip;
                        candidateObject.port = candidate.port;
                        candidateObject.priority = candidate.priority;
                        candidateObject.transport = candidate.protocol;
                        candidateObject.type = candidate.type;
                        if (candidate.tcpType)
                            candidateObject.tcptype = candidate.tcpType;
                        this._mediaObject.candidates.push(candidateObject);
                    }
                    this._mediaObject.endOfCandidates = 'end-of-candidates';
                    this._mediaObject.iceOptions = 'renomination';
                }
                if (dtlsParameters) {
                    this.setDtlsRole(dtlsParameters.role);
                }

                this._mediaObject.mid = String(mid);
                this._mediaObject.type = kind;
                if (!plainRtpParameters) {
                    this._mediaObject.connection = { ip: '127.0.0.1', version: 4 };
                    if (!sctpParameters)
                        this._mediaObject.protocol = 'UDP/TLS/RTP/SAVPF';
                    else
                        this._mediaObject.protocol = 'UDP/DTLS/SCTP';
                    this._mediaObject.port = 7;
                }
                else {
                    this._mediaObject.connection =
                        {
                            ip: plainRtpParameters.ip,
                            version: plainRtpParameters.ipVersion
                        };
                    this._mediaObject.protocol = 'RTP/AVP';
                    this._mediaObject.port = plainRtpParameters.port;
                }
                switch (kind) {
                    case 'audio':
                    case 'video':
                        {
                            this._mediaObject.direction = 'sendonly';
                            this._mediaObject.rtp = [];
                            this._mediaObject.rtcpFb = [];
                            this._mediaObject.fmtp = [];
                            if (!this._planB)
                                this._mediaObject.msid = `${streamId || '-'} ${trackId}`;
                            for (const codec of offerRtpParameters.codecs) {
                                const rtp = {
                                    payload: codec.payloadType,
                                    codec: getCodecName(codec),
                                    rate: codec.clockRate
                                };
                                if (codec.channels > 1)
                                    rtp.encoding = codec.channels;
                                this._mediaObject.rtp.push(rtp);
                                const fmtp = {
                                    payload: codec.payloadType,
                                    config: ''
                                };
                                for (const key of Object.keys(codec.parameters)) {
                                    if (fmtp.config)
                                        fmtp.config += ';';
                                    fmtp.config += `${key}=${codec.parameters[key]}`;
                                }
                                if (fmtp.config)
                                    this._mediaObject.fmtp.push(fmtp);
                                for (const fb of codec.rtcpFeedback) {
                                    this._mediaObject.rtcpFb.push({
                                        payload: codec.payloadType,
                                        type: fb.type,
                                        subtype: fb.parameter
                                    });
                                }
                            }
                            this._mediaObject.payloads = offerRtpParameters.codecs
                                .map((codec) => codec.payloadType)
                                .join(' ');
                            this._mediaObject.ext = [];
                            for (const ext of offerRtpParameters.headerExtensions) {
                                this._mediaObject.ext.push({
                                    uri: ext.uri,
                                    value: ext.id
                                });
                            }
                            this._mediaObject.rtcpMux = 'rtcp-mux';
                            this._mediaObject.rtcpRsize = 'rtcp-rsize';
                            const encoding = offerRtpParameters.encodings[0];
                            const ssrc = encoding.ssrc;
                            const rtxSsrc = (encoding.rtx && encoding.rtx.ssrc)
                                ? encoding.rtx.ssrc
                                : undefined;
                            this._mediaObject.ssrcs = [];
                            this._mediaObject.ssrcGroups = [];
                            if (offerRtpParameters.rtcp.cname) {
                                this._mediaObject.ssrcs.push({
                                    id: ssrc,
                                    attribute: 'cname',
                                    value: offerRtpParameters.rtcp.cname
                                });
                            }
                            if (this._planB) {
                                this._mediaObject.ssrcs.push({
                                    id: ssrc,
                                    attribute: 'msid',
                                    value: `${streamId || '-'} ${trackId}`
                                });
                            }
                            if (rtxSsrc) {
                                if (offerRtpParameters.rtcp.cname) {
                                    this._mediaObject.ssrcs.push({
                                        id: rtxSsrc,
                                        attribute: 'cname',
                                        value: offerRtpParameters.rtcp.cname
                                    });
                                }
                                if (this._planB) {
                                    this._mediaObject.ssrcs.push({
                                        id: rtxSsrc,
                                        attribute: 'msid',
                                        value: `${streamId || '-'} ${trackId}`
                                    });
                                }
                                // Associate original and retransmission SSRCs.
                                this._mediaObject.ssrcGroups.push({
                                    semantics: 'FID',
                                    ssrcs: `${ssrc} ${rtxSsrc}`
                                });
                            }
                            break;
                        }
                    case 'application':
                        {
                            // New spec.
                            if (!oldDataChannelSpec) {
                                this._mediaObject.payloads = 'webrtc-datachannel';
                                this._mediaObject.sctpPort = sctpParameters.port;
                                this._mediaObject.maxMessageSize = sctpParameters.maxMessageSize;
                            }
                            // Old spec.
                            else {
                                this._mediaObject.payloads = sctpParameters.port;
                                this._mediaObject.sctpmap =
                                    {
                                        app: 'webrtc-datachannel',
                                        sctpmapNumber: sctpParameters.port,
                                        maxMessageSize: sctpParameters.maxMessageSize
                                    };
                            }
                            break;
                        }
                }



                //////////////////////////////////////////////////////////////////////////////////////////////
                // public function
                //////////////////////////////////////////////////////////////////////////////////////////////
                this.mid = function() 
                {
                    return String(this._mediaObject.mid);
                }
                this. closed = fucntion() 
                {
                    return this._mediaObject.port === 0;
                }
                this.getObject = function() 
                {
                    return this._mediaObject;
                }
                this.setIceParameters = function(iceParameters) 
                {
                    this._mediaObject.iceUfrag = iceParameters.usernameFragment;
                    this._mediaObject.icePwd = iceParameters.password;
                }
                this.disable = function()
                {
                    this._mediaObject.direction = 'inactive';
                    delete this._mediaObject.ext;
                    delete this._mediaObject.ssrcs;
                    delete this._mediaObject.ssrcGroups;
                    delete this._mediaObject.simulcast;
                    delete this._mediaObject.simulcast_03;
                    delete this._mediaObject.rids;
                }
                this.close = function() 
                {
                    this._mediaObject.direction = 'inactive';
                    this._mediaObject.port = 0;
                    delete this._mediaObject.ext;
                    delete this._mediaObject.ssrcs;
                    delete this._mediaObject.ssrcGroups;
                    delete this._mediaObject.simulcast;
                    delete this._mediaObject.simulcast_03;
                    delete this._mediaObject.rids;
                    delete this._mediaObject.extmapAllowMixed;
                }

                 // eslint-disable-next-line @typescript-eslint/no-unused-vars
                this.setDtlsRole = function(role) 
                {
                    // Always 'actpass'.
                    this._mediaObject.setup = 'actpass';
                }
                this.planBReceive = function({ offerRtpParameters, streamId, trackId }) 
                {
                    const encoding = offerRtpParameters.encodings[0];
                    const ssrc = encoding.ssrc;
                    const rtxSsrc = (encoding.rtx && encoding.rtx.ssrc)
                        ? encoding.rtx.ssrc
                        : undefined;
                    const payloads = this._mediaObject.payloads.split(' ');
                    for (const codec of offerRtpParameters.codecs) {
                        if (payloads.includes(String(codec.payloadType))) {
                            continue;
                        }
                        const rtp = {
                            payload: codec.payloadType,
                            codec: getCodecName(codec),
                            rate: codec.clockRate
                        };
                        if (codec.channels > 1)
                            rtp.encoding = codec.channels;
                        this._mediaObject.rtp.push(rtp);
                        const fmtp = {
                            payload: codec.payloadType,
                            config: ''
                        };
                        for (const key of Object.keys(codec.parameters)) {
                            if (fmtp.config)
                                fmtp.config += ';';
                            fmtp.config += `${key}=${codec.parameters[key]}`;
                        }
                        if (fmtp.config)
                            this._mediaObject.fmtp.push(fmtp);
                        for (const fb of codec.rtcpFeedback) {
                            this._mediaObject.rtcpFb.push({
                                payload: codec.payloadType,
                                type: fb.type,
                                subtype: fb.parameter
                            });
                        }
                    }
                    this._mediaObject.payloads += ` ${offerRtpParameters
                        .codecs
                        .filter((codec) => !this._mediaObject.payloads.includes(codec.payloadType))
                        .map((codec) => codec.payloadType)
                        .join(' ')}`;
                    this._mediaObject.payloads = this._mediaObject.payloads.trim();
                    if (offerRtpParameters.rtcp.cname) {
                        this._mediaObject.ssrcs.push({
                            id: ssrc,
                            attribute: 'cname',
                            value: offerRtpParameters.rtcp.cname
                        });
                    }
                    this._mediaObject.ssrcs.push({
                        id: ssrc,
                        attribute: 'msid',
                        value: `${streamId || '-'} ${trackId}`
                    });
                    if (rtxSsrc) {
                        if (offerRtpParameters.rtcp.cname) {
                            this._mediaObject.ssrcs.push({
                                id: rtxSsrc,
                                attribute: 'cname',
                                value: offerRtpParameters.rtcp.cname
                            });
                        }
                        this._mediaObject.ssrcs.push({
                            id: rtxSsrc,
                            attribute: 'msid',
                            value: `${streamId || '-'} ${trackId}`
                        });
                        // Associate original and retransmission SSRCs.
                        this._mediaObject.ssrcGroups.push({
                            semantics: 'FID',
                            ssrcs: `${ssrc} ${rtxSsrc}`
                        });
                    }
                }
                this.planBStopReceiving = function({ offerRtpParameters }) 
                {
                    const encoding = offerRtpParameters.encodings[0];
                    const ssrc = encoding.ssrc;
                    const rtxSsrc = (encoding.rtx && encoding.rtx.ssrc)
                        ? encoding.rtx.ssrc
                        : undefined;
                    this._mediaObject.ssrcs = this._mediaObject.ssrcs
                        .filter((s) => s.id !== ssrc && s.id !== rtxSsrc);
                    if (rtxSsrc) {
                        this._mediaObject.ssrcGroups = this._mediaObject.ssrcGroups
                            .filter((group) => group.ssrcs !== `${ssrc} ${rtxSsrc}`);
                    }
                }
     
            };
            return OfferMediaSection;
}));