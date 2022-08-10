/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		csctp_association_mgr


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
		//sctpAssociation->OnUsrSctpSendSctpData(data, len);

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