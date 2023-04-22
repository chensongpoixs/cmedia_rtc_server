/***********************************************************************************************
created: 		2023-01-30

author:			chensong

purpose:		_C_DTLS_CERTIFICATE_H_
输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。

************************************************************************************************/

#ifndef _C_DTLS_SESSION_H_
#define _C_DTLS_SESSION_H_
#include "cnet_type.h"
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cdtls_define.h"
#include "cmedia_desc.h"
#include "crtc_transport.h"
namespace chen {



	class cdtls_session
	{
	public:
		//************************************
		// Method:    cdtls_session
		// FullName:  chen::cdtls_session::cdtls_session
		// Access:    public 
		// Returns:   
		// Qualifier:
		//************************************
		explicit cdtls_session(crtc_transportlinster * callback)
			: m_dtls_ctx_ptr(NULL)
			, m_dtls_ptr(NULL)
			, m_bio_in_ptr(NULL)
			, m_bio_out_ptr(NULL)
			, m_handshake_done_for_us(false)
			, m_nn_arq_packets(0)
			, m_callback_ptr(callback){}
		//************************************
		// Method:    ~cdtls_session
		// FullName:  chen::cdtls_session::~cdtls_session
		// Access:    virtual public 
		// Returns:   
		// Qualifier:
		//************************************
		virtual ~cdtls_session();


	public:
		//************************************
		// Method:    init
		// FullName:  chen::cdtls::init
		// Access:    public static 
		// Returns:   chen::int32
		// Qualifier:
		//************************************
		virtual bool init();


		//************************************
		// Method:    destroy
		// FullName:  chen::cdtls_session::destroy
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		//************************************
		virtual void destroy();

		//************************************
		// Method:    start_active_handshake
		// FullName:  chen::cdtls::start_active_handshake
		// Access:    virtual public 
		// Returns:   chen::int32
		// Qualifier:
		//************************************
		virtual int32 start_active_handshake() = 0;
		//************************************
		// Method:    should_reset_timer
		// FullName:  chen::cdtls::should_reset_timer
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		//************************************
		virtual bool should_reset_timer() = 0;
		//************************************
		// Method:    on_dtls
		// FullName:  chen::cdtls::on_dtls
		// Access:    virtual public 
		// Returns:   chen::int32
		// Qualifier:
		// Parameter: char * data
		// Parameter: int32 nb_data
		//************************************
		virtual int32 on_dtls(char* data, int32 nb_data);
	protected:
		// callback 回调用应用层的api 就是传输transport中的api
		int32 do_on_dtls(char* data, int32 nb_data);
		int32 do_handshake();
		void state_trace(uint8* data, int32 length, bool incoming, int32 r0, int32 r1, bool arq);
	public:
		int32 get_srtp_key(std::string& recv_key, std::string& send_key);
		// global function  -------- ???
		void callback_by_ssl(std::string type, std::string desc);
	protected:
		virtual int32 on_final_out_data(uint8* data, int32 size) = 0;
		virtual int32 on_handshake_done() = 0;
		virtual bool is_dtls_client() = 0;

	protected:


		SSL_CTX*		m_dtls_ctx_ptr;
		SSL*			m_dtls_ptr;
		BIO*			m_bio_in_ptr;
		BIO*			m_bio_out_ptr;
		// Whether the handshake is done, for us only.
	// @remark For us only, means peer maybe not done, we also need to handle the DTLS packet.
		bool			m_handshake_done_for_us;

		// The stat for ARQ packets.
		int32			m_nn_arq_packets;

		crtc_transportlinster *				m_callback_ptr;
	private:
	};






	class cdtls_server : public cdtls_session
	{
	public:

		 
		explicit cdtls_server(crtc_transportlinster *callback)
			: cdtls_session(callback) {}

		 
		virtual ~cdtls_server();

	public:
		virtual bool init();
		virtual void update(uint32 uDeltaTime);
		virtual void destroy();
		virtual int32 start_active_handshake();
		virtual bool should_reset_timer();
	protected:
		virtual int32 on_final_out_data(uint8* data, int size);
		virtual int32 on_handshake_done();
		virtual bool is_dtls_client();
	private:
		int32 start_arq();
		void stop_arq();
	protected:
	protected:
	private:
	};
	class cdtls_client : public cdtls_session
	{
	public:

		 
		explicit cdtls_client(crtc_transportlinster *callback)
			: cdtls_session(callback)
		, m_state(EDtlsStateInit)
		, m_arq_max_retry(12* 2)   // the max dtls retry num is 12 in openssl.// Max ARQ limit shared for ClientHello and Certificate.
		, m_reset_timer(true){}

		 
		virtual ~cdtls_client();
	public:
	public:
		virtual bool init();
		virtual void update(uint32 uDeltaTime); 
		virtual void destroy();
		virtual int32 start_active_handshake();
		virtual bool should_reset_timer();
	protected:
		virtual int32 on_final_out_data(uint8* data, int size);
		virtual int32 on_handshake_done();
		virtual bool is_dtls_client();
	private:
		int32 start_arq();
		void stop_arq();
	protected:
	private:
		// The DTLS-client state to drive the ARQ thread.
		EDtlsState				m_state;
		// The max ARQ retry.
		int32					m_arq_max_retry;
		// Should we reset the timer?
		// It's true when init, or in state ServerHello.
		bool					m_reset_timer ;
	};

}

#endif // _C_DTLS_SESSION_H_