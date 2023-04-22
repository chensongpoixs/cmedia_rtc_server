/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		csctp_association_mgr
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
#include "csctp_association_mgr.h"
#include "clog.h"
#include "csctp_association.h"
namespace chen {
	csctp_association_mgr g_sctp_association_mgr;
	/* Static methods for usrsctp global callbacks. */

	inline static int onSendSctpData(void* addr, void* data, size_t len, uint8_t /*tos*/, uint8_t /*setDf*/)
	{
		csctp_association * sctpAssociation = g_sctp_association_mgr.RetrieveSctpAssociation(reinterpret_cast<uintptr_t>(addr));

		if (!sctpAssociation)
		{
			//MS_WARN_TAG(sctp, "no SctpAssociation found");
			WARNING_EX_LOG("no SctpAssociation found");
			return -1;
		}
		// TODO@chensong 20220810  sctp send data 
		sctpAssociation->OnUsrSctpSendSctpData(data, len);

		// NOTE: Must not free data, usrsctp lib does it.

		return 0;
	}

	// Static method for printing usrsctp debug.
	inline static void sctpDebug(const char* format, ...)
	{
		char buffer[10000];
		va_list ap;

		va_start(ap, format);
		vsprintf(buffer, format, ap);

		// Remove the artificial carriage return set by usrsctp.
		buffer[std::strlen(buffer) - 1] = '\0';

		DEBUG_EX_LOG("sctp = %s", buffer);

		va_end(ap);
	}


	csctp_association_mgr::csctp_association_mgr()
		: m_num_sctp_associations(0u)
		, m_next_sctpassociationid(0u)
		, m_global_instances(0)
		, m_sctp_association_map()
	{
	}
	csctp_association_mgr::~csctp_association_mgr()
	{
	}
	bool csctp_association_mgr::init()
	{
		{
			clock_guard lock(m_global_sync_mutex);
			if (m_global_instances == 0)
			{
				usrsctp_init_nothreads(0, onSendSctpData, sctpDebug);
				// Disable explicit congestion notifications (ecn).
				usrsctp_sysctl_set_sctp_ecn_enable(0);

#ifdef SCTP_DEBUG
				usrsctp_sysctl_set_sctp_debug_on(SCTP_DEBUG_ALL);
#endif
			}
			++m_global_instances;
		}
		return true;
	}
	void csctp_association_mgr::update(uint32 uDeltaTime)
	{
		static size_t CheckerInterval{ 10u }; // In ms.
	}
	void csctp_association_mgr::destroy()
	{
		{
			clock_guard lock(m_global_sync_mutex);
			--m_global_instances;
			if (0 == m_global_instances)
			{
				usrsctp_finish();
				//numSctpAssociations = 0u;
				m_num_sctp_associations = 0u;
				m_next_sctpassociationid = 0u;
				m_sctp_association_map.clear();
			}
		}
	}
	uintptr_t csctp_association_mgr::GetNextSctpAssociationId()
	{
		clock_guard lock(m_global_sync_mutex);
		// NOTE: usrsctp_connect() fails with a value of 0.
		if (0u == m_next_sctpassociationid)
		{
			++m_next_sctpassociationid;
		}

		// In case we've wrapped around and need to find an empty spot from a removed
		// SctpAssociation. Assumes we'll never be full.

		while (m_sctp_association_map.find(m_next_sctpassociationid) != m_sctp_association_map.end())
		{
			++m_next_sctpassociationid;

			if (m_next_sctpassociationid == 0u)
			{
				++m_next_sctpassociationid;
			}
		}

		return m_next_sctpassociationid++;
	}
	void csctp_association_mgr::RegisterSctpAssociation(csctp_association * sctpAssociation)
	{
		//std::lock_guard<std::mutex> lock(globalSyncMutex);
		clock_guard lock(m_global_sync_mutex);
		//MS_ASSERT(DepUsrSCTP::checker != nullptr, "Checker not created");

		auto it = m_sctp_association_map.find(sctpAssociation->get_id());

		/*MS_ASSERT(
			it == DepUsrSCTP::mapIdSctpAssociation.end(),
			"the id of the SctpAssociation is already in the map");*/

		m_sctp_association_map[sctpAssociation->get_id()] = sctpAssociation;

		if (++m_num_sctp_associations == 1u)
		{ }
			//DepUsrSCTP::checker->Start();
	}
	void csctp_association_mgr::DeregisterSctpAssociation(csctp_association * sctpAssociation)
	{
		clock_guard lock(m_global_sync_mutex);
	    m_sctp_association_map.erase(sctpAssociation->get_id());


		if (--m_num_sctp_associations == 0u)
		{ }
			//DepUsrSCTP::checker->Stop();
	}
	csctp_association * csctp_association_mgr::RetrieveSctpAssociation(uintptr_t id)
	{
		clock_guard lock(m_global_sync_mutex);

		C_SCTP_ASSOCIATION_MAP::iterator iter = m_sctp_association_map.find(id);

		if (iter == m_sctp_association_map.end())
		{
			return nullptr;
		}

		return iter->second;
		 
	}
}