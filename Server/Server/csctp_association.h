/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		csctp_association

 
************************************************************************************************/

#ifndef _C_SCTP_ASSOCIATION_H_
#define _C_SCTP_ASSOCIATION_H_
#include <string>
#include "cnet_type.h"
#include <map>
#include <usrsctp.h>
#include "cwebrtc_transport.h"
#include <json/json.h>
namespace chen
{

	enum  ESCTPSTATE
	{
		ESCTP_NEW = 1,
		ESCTP_CONNECTING,
		ESCTP_CONNECTED,
		ESCTP_FAILED,
		ESCTP_CLOSED
	};
	enum   EStreamDirection
	{
		ESTREAM_INCOMING = 1,
		ESTREAM_OUTGOING
	};
	class csctp_association
	{
	public:
		class Listener
		{
		public:
			virtual ~Listener() = default;

		public:
			virtual void OnSctpAssociationConnecting(csctp_association* sctpAssociation) = 0;
			virtual void OnSctpAssociationConnected(csctp_association* sctpAssociation) = 0;
			virtual void OnSctpAssociationFailed(csctp_association* sctpAssociation) = 0;
			virtual void OnSctpAssociationClosed(csctp_association* sctpAssociation) = 0;
			virtual void OnSctpAssociationSendData(
				csctp_association* sctpAssociation, const uint8_t* data, size_t len) = 0;
			virtual void OnSctpAssociationMessageReceived(
				csctp_association* sctpAssociation,
				uint16_t streamId,
				uint32_t ppid,
				const uint8_t* msg,
				size_t len) = 0;
			virtual void OnSctpAssociationBufferedAmount(
				csctp_association* sctpAssociation, uint32_t len) = 0;
		protected:
		private:
		};
	public:
		csctp_association(csctp_association::Listener * transport, uint16_t os, uint16_t mis, size_t max_sctp_message_size, size_t sctp_send_buffer_size);
			
		~csctp_association();



	public:
		static bool issctp(const uint8_t * data, size_t len);
		uintptr_t get_id() { return m_id; }
		
	public:
		void TransportConnected();
		ESCTPSTATE get_state() const { return m_state; }
		size_t get_sctp_buffered_amount() const { return m_sctp_buffered_amount; }

		void ProcessSctpData(const uint8_t* data, size_t len);
		void SendSctpMessage(uint16_t streamId, uint32_t ppid, const uint8_t* msg, size_t len );
	public:
		/// Json
		void reply(Json::Value& value);
	private:
		void ResetSctpStream(uint16_t streamId, EStreamDirection direction);
		void AddOutgoingStreams(bool force = false);

	public:

		/* Callbacks fired by usrsctp events. */
		void OnUsrSctpSendSctpData(void* buffer, size_t len);
		void OnUsrSctpReceiveSctpData(
			uint16_t streamId, uint16_t ssn, uint32_t ppid, int flags, const uint8_t* data, size_t len);
		void OnUsrSctpReceiveSctpNotification(union sctp_notification* notification, size_t len);
		void OnUsrSctpSentData(uint32_t freeBuffer);
	private:
		csctp_association::Listener *			m_transport_ptr;
		uintptr_t					m_id;// { 0u };
		uint16_t					m_os;  // default [1024u]
		uint16_t					m_mis; // default [1024u]
		size_t						m_max_sctp_message_size; // default [262144u]
		size_t						m_sctp_send_buffer_size; // default [262144u]
		size_t						m_sctp_buffered_amount; // default [0u]
		// Allocated by this
		uint8_t	*					m_message_buffer;
		ESCTPSTATE					m_state; // default [ESCTP_NEW]
		struct socket*				m_socket;
		uint16_t					m_desiredos; // default [0u]
		size_t						m_message_buffer_len;
		uint16_t					m_last_ssn_received; // Valid for us since no SCTP I-DATA support.
	};
}

#endif // _C_SCTP_ASSOCIATION_H_