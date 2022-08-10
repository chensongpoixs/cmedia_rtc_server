/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		csctp_association_mgr


************************************************************************************************/

#ifndef _C_SCTP_ASSOCIATION_MGR_H_
#define _C_SCTP_ASSOCIATION_MGR_H_
#include <string>
#include "cnet_type.h"
#include <map>
#include <usrsctp.h>
#include <mutex>
#include <unordered_map>
namespace chen
{
	class csctp_association;
	class csctp_association_mgr
	{
	private:
		typedef std::unordered_map<uint64, csctp_association*>       C_SCTP_ASSOCIATION_MAP;

		typedef std::mutex											 clock_type;
		typedef std::lock_guard<clock_type>							 clock_guard;
	public:
		csctp_association_mgr();
		~csctp_association_mgr();

	public:
		bool init();
		void update(uint32 uDeltaTime);
		void destroy();


	public:
		uintptr_t GetNextSctpAssociationId();
		void RegisterSctpAssociation(csctp_association* sctpAssociation);
		void DeregisterSctpAssociation(csctp_association* sctpAssociation);
		csctp_association* RetrieveSctpAssociation(uintptr_t id);
	private:

	private:
		uint64					m_num_sctp_associations;
		uintptr_t				m_next_sctpassociationid;
		clock_type				m_global_sync_mutex;
		size_t					m_global_instances;
		C_SCTP_ASSOCIATION_MAP  m_sctp_association_map;
	};

	extern csctp_association_mgr g_sctp_association_mgr;
}

#endif // _C_SCTP_ASSOCIATION_MGR_H_