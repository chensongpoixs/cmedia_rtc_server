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
        root.Logger = factory(root.adapter);
    }
}(this, function ( ) {

        function Logger(prefix)
        {
            console.log('<=====================================>[][][][][][Logger]');
            // if(prefix)
            // {
            //     this._debug = debug.default(`cmedia_rtc_client:${prefix}`);
            //     this._warn = debug.default(`cmedia_rtc_client:WARN:${prefix}`);
            //     this._error = debug.default(`cmedia_rtc_client:ERROR:${prefix}`);
            // }
            // else 
            // {
                this._debug =  'cmedia_rtc_client' ;
                this._warn =  `cmedia_rtc_client:WARN` ;
                this._error =  `cmedia_rtc_client:ERROR` ;
            // }

            /* eslint-disable no-console */
            this._debug.log = console.info.bind(console);
            this._warn.log = console.warn.bind(console);
            this._error.log = console.error.bind(console);
            /* eslint-enable no-console */
            this. debug = function() 
            {
                return this._debug;
            }
            this. warn = function() 
            {
                return this._warn;
            }
            this. error = function() 
            {
                return this._error;
            }
        };
        return Logger;
}));