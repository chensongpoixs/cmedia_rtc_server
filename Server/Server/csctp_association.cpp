/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		csctp_association


************************************************************************************************/
#include "csctp_association.h"
#include "crtc_util.h"
namespace chen {
	csctp_association::csctp_association(uint16_t os, uint16_t mis, size_t max_sctp_message_size, size_t sctp_send_buffer_size)
		: m_id(0u)
		, m_os(os)
		, m_mis(mis)
		, m_max_sctp_message_size(max_sctp_message_size)
		, m_sctp_send_buffer_size(sctp_send_buffer_size)
		, m_sctp_buffered_amount(0u)
		, m_message_buffer(NULL)
		, m_state(ESCTP_NEW)
		, m_socket(NULL)
		, m_desiredos(0u)
		, m_message_buffer_len(0u)
		, m_last_ssn_received(0u)
	{
		// Get a id for this SctpAssociation.
		//m_id = DepUsrSCTP::GetNextSctpAssociationId();
	}
	/*csctp_association::csctp_association()
	{
	}*/
	csctp_association::~csctp_association()
	{
	}
	bool csctp_association::issctp(const uint8_t * data, size_t len)
	{
		// clang-format off
		return (
			(len >= 12) &&
			// Must have Source Port Number and Destination Port Number set to 5000 (hack).
			(rtc_byte::get2bytes(data, 0) == 5000) &&
			(rtc_byte::get2bytes(data, 2) == 5000)
			);
		// clang-format on
	}
}