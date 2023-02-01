/***********************************************************************************************
created: 		2023-02-01

author:			chensong

purpose:		 rtc——stun packet 
************************************************************************************************/


#ifndef _C_RTC_STUN_PACKET_H_
#define _C_RTC_STUN_PACKET_H_
#include "cnet_type.h"
#include "crtc_stun_define.h"



namespace chen {

	class crtc_stun_packet
	{
	public:
		explicit crtc_stun_packet()
		: m_message_type(0)
		, m_username("")
		, m_password("")
		, m_local_ufrag("")
		, m_remote_ufrag("")
		, m_transcation_id("")
		, m_mapped_address(0)
		, m_mapped_port(0)
		, m_use_candidate(false)
		, m_ice_controlled(false)
		, m_ice_controlling(false)
		{}
		virtual ~crtc_stun_packet(); 

	public:
		static bool is_stun(const uint8* data, size_t len)
		{
			// clang-format off
			return (
				// STUN headers are 20 bytes.
				(len >= 20) &&
				// DOC: https://tools.ietf.org/html/draft-ietf-avtcore-rfc5764-mux-fixes
				(data[0] < 3) &&
				// Magic cookie must match.
				(data[4] == kStunMagicCookie[0]) && (data[5] == kStunMagicCookie[1]) &&
				(data[6] == kStunMagicCookie[2]) && (data[7] == kStunMagicCookie[3])
				);
			// clang-format on
		}
	public:
		bool is_binding_request() const 
		{
			return m_message_type == EBindingRequest;
		};
		bool is_binding_response() const 
		{
			return m_message_type == EBindingResponse;
		};
		uint16  get_message_type() const { return m_message_type; };
		std::string get_username() const { return m_username; };
		std::string get_local_ufrag() const { return m_local_ufrag; };
		std::string get_remote_ufrag() const { return m_remote_ufrag; };
		std::string get_transcation_id() const { return m_transcation_id;  };
		uint32  get_mapped_address() const { return m_mapped_address; };
		uint16  get_mapped_port() const { return m_mapped_port; };
		bool get_ice_controlled() const { return m_ice_controlled; };
		bool get_ice_controlling() const { return m_ice_controlling; };
		bool get_use_candidate() const { return m_use_candidate; };
		void set_message_type(const uint16& m) { m_message_type = m; };
		void set_local_ufrag(const std::string& u) { m_local_ufrag = u; };
		void set_remote_ufrag(const std::string& u) { m_remote_ufrag = u; };
		void set_transcation_id(const std::string& t) { m_transcation_id = t; };
		void set_mapped_address(const uint32 & addr) { m_mapped_address = addr; };
		void set_mapped_port(const uint32 & port) { m_mapped_port = port; };
		int32 decode(const char* buf, const int32 nb_buf);
		int32 encode(const std::string& pwd,  const char * buffer, const int32 nb_buf);
	private:
		int32 encode_binding_response(const std::string& pwd, const char * buffer, const int32 nb_buf);
		std::string encode_username();
		std::string encode_mapped_address();
		std::string encode_hmac(char* hamc_buf, const int32 hmac_buf_len);
		std::string encode_fingerprint(uint32  crc32);
	protected:
	private:


	private:
		//// STUN attributes.
		uint16					m_message_type;		// 2 bytes.
		std::string				m_username;			// Less than 513 bytes.
		std::string				m_password;
		std::string				m_local_ufrag;
		std::string				m_remote_ufrag;
		std::string				m_transcation_id;   // 12 bytes.
		uint32					m_mapped_address;   // /*8 or 20 bytes.
		uint16					m_mapped_port;      //                 */
		bool					m_use_candidate;    // 0 bytes.
		bool					m_ice_controlled;   // 8 bytes unsigned integer.
		bool					m_ice_controlling;  // 8 bytes unsigned integer.
	};

}

#endif // _C_RTC_STUN_PACKET_H_