(function (root, factory) 
{
   if (typeof define === 'function' && define.amd) {
       // AMD. Register as an anonymous module.
		//console.log('++++========++');
       define(["./grammar"], factory);
   } else if (typeof exports === 'object') {
       // Node. Does not work with strict CommonJS, but
       // only CommonJS-like environments that support module.exports,
       // like Node.
		//console.log('+++-------++++++');
       module.exports = factory(require("./grammar"));
   } else {
       // Browser globals (root is window)
		//console.log('+++++++++');
       root.webrtc_sdp = factory(root.grammar);
   }
  // root.webrtc_sdp = factory(root.adapter);
}(this, function (adapter) {
	
	function webrtc_sdp() 
	{
    	 console.log('webrtc_sdp');
        var self = this;
		//self.client = null;
		var validLine = RegExp.prototype.test.bind(/^([a-z])=(.*)/);
		 
		
 
		 
        this.parse = function(sdp)
		    //function parse(sdp)
		{
			
			
		 
			 console.log('webrtc_sdp.parse');
			var session = {}
				, media = []
				, location = session; // points at where properties go under (one of the above)
				console.log(grammar.length);
			//return session;
			// parse lines we understand
			   sdp.split(/(\r\n|\r|\n)/).filter(validLine).forEach(function (l) 
			  {
				var type = l[0];
				var content = l.slice(2);
				if (type === 'm') 
				{
				  media.push({rtp: [], fmtp: []});
				  location = media[media.length-1]; // point at latest media line
				}
				// 根据type的类型进行解析格式
				for (var j = 0; j < (grammar[type] || []).length; j += 1) 
				{
				  var obj = grammar[type][j];
				  if (obj.reg.test(content)) 
				  {
					 return parseReg(obj, location, content);
				  }
				}
			  });

			  session.media = media; // link it up
			  return session;
			 
		};
          
    };

    return webrtc_sdp;
  
}));