/***********************************************************************************************
created: 		2023-02-13

author:			chensong

purpose:		 rtc——stun define
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


#ifndef _C_ICE_SERVER_H_
#define _C_ICE_SERVER_H_
#include "cnet_type.h"
#include <list>
#include "ctransport_tuple.h"
//#include "crtc_transport.h"
namespace chen {

	//class ctransport_tuple;
	class cice_server_istener;
	class cice_server
	{
	public:
		enum EIceState
		{
			EIceNew = 1,
			EIceConnected,
			EIceCompleted,
			EIceDisConnected
		};
	
	public:
		
	public:
		 cice_server(cice_server_istener* listener)
		: m_listenter_ptr(listener)
		, m_username_fragment("")
		, m_password("")
		, m_old_username_fragment("")
		, m_old_password("")
		, m_ice_state(EIceNew)
		, m_tuples()
		, m_selected_tuple_ptr(NULL){}
		virtual ~cice_server() {}

	public:
		bool init(const std::string& username, const std::string &password);
		void update(uint32 uDeltaTime);
		void destroy();

	public:
		void ProcessStunPacket(const uint8* data, size_t len, ctransport_tuple* tuple);
		const std::string& GetUsernameFragment() const
		{
			return this->m_username_fragment;
		}
		const std::string& GetPassword() const
		{
			return this->m_password;
		}
		EIceState GetState() const
		{
			return this->m_ice_state;
		}
		ctransport_tuple* GetSelectedTuple() const
		{
			return this->m_selected_tuple_ptr;
		}
		void SetUsernameFragment(const std::string& usernameFragment)
		{
			this->m_old_username_fragment = this->m_username_fragment;
			this->m_username_fragment    = usernameFragment;
		}
		void SetPassword(const std::string& password)
		{
			this->m_old_password = this->m_password;
			this->m_password    = password;
		}
		bool IsValidTuple(const ctransport_tuple* tuple) const;
		void RemoveTuple(ctransport_tuple* tuple);
		// This should be just called in 'connected' or completed' state
		// and the given tuple must be an already valid tuple.
		void ForceSelectedTuple(const ctransport_tuple* tuple);

	private:
		void HandleTuple(ctransport_tuple* tuple, bool hasUseCandidate);
		/**
		* Store the given tuple and return its stored address.
		*/
		ctransport_tuple* AddTuple(ctransport_tuple* tuple);
		/**
		* If the given tuple exists return its stored address, nullptr otherwise.
		*/
		ctransport_tuple* HasTuple(const ctransport_tuple* tuple) const;
		/**
		* Set the given tuple as the selected tuple.
		* NOTE: The given tuple MUST be already stored within the list.
		*/
		void SetSelectedTuple(ctransport_tuple* storedTuple);

	protected:
	private:
		cice_server_istener *  m_listenter_ptr;
		std::string		m_username_fragment; 
		std::string		m_password;
		std::string		m_old_username_fragment;
		std::string		m_old_password;

		EIceState       m_ice_state;

		std::list<ctransport_tuple>   m_tuples; //元组
		ctransport_tuple *			m_selected_tuple_ptr;
	};
}


#endif // _C_ICE_SERVER_H_