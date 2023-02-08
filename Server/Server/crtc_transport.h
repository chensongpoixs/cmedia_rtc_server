/***********************************************************************************************
created: 		2023-02-03

author:			chensong

purpose:		crtc_transport


************************************************************************************************/

#ifndef _C_RTC_TRANSPORT_H_
#define _C_RTC_TRANSPORT_H_
#include "cnet_type.h"
#include "crtc_sdp.h"
#include "crtc_source_description.h"
namespace chen {


	class crtc_transport
	{
	public:
		explicit crtc_transport()
		: m_local_sdp ()
		, m_remote_sdp (){}
		virtual ~crtc_transport();

	public:
		bool init(const std::string & remote_sdp, std::string & local_sdp);

		void update(uint32 uDeltaTime);

		void destroy();
	protected:


	private:
		// publish -> remote sdp 
		bool _negotiate_publish_capability(crtc_source_description * stream_desc);
		bool _generate_publish_local_sdp(  crtc_sdp& local_sdp, crtc_source_description* stream_desc, bool unified_plan, bool audio_before_video);
	
		bool _generate_publish_local_sdp_for_audio(crtc_sdp& local_sdp, crtc_source_description* stream_desc);
		bool _generate_publish_local_sdp_for_video(crtc_sdp& local_sdp, crtc_source_description* stream_desc, bool unified_plan);


	private:
		crtc_sdp			 			m_local_sdp ;
		crtc_sdp			 			m_remote_sdp ;
	};


}

#endif // _C_RTC_TRANSPORT_H_