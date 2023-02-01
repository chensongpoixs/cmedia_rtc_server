/***********************************************************************************************
created: 		2023-02-01

author:			chensong

purpose:		 rtc stun packet
************************************************************************************************/
#include "crtc_stun_packet.h"
#include <openssl/dh.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/ssl.h>
#include "clog.h"
#include "cuv_util.h"

#include "crtc_util.h"


namespace chen {

	static int32 hmac_encode(const std::string& algo, const char* key, const int32& key_length,
		const char* input, const int32 input_length, char* output, uint32& output_length)
	{
		int32 err = 0;

		const EVP_MD* engine = NULL;
		if (algo == "sha512") 
		{
			engine = EVP_sha512();
		}
		else if (algo == "sha256")
		{
			engine = EVP_sha256();
		}
		else if (algo == "sha1")
		{
			engine = EVP_sha1();
		}
		else if (algo == "md5") 
		{
			engine = EVP_md5();
		}
		else if (algo == "sha224") 
		{
			engine = EVP_sha224();
		}
		else if (algo == "sha384")
		{
			engine = EVP_sha384();
		}
		else 
		{
			WARNING_EX_LOG("stun unknown algo=%s", algo.c_str());
			return -1;
		}

		HMAC_CTX* ctx = HMAC_CTX_new();
		if (ctx == NULL) 
		{
			WARNING_EX_LOG("hmac init faied" );
			return -1;
		}

		if (HMAC_Init_ex(ctx, key, key_length, engine, NULL) < 0) 
		{
			HMAC_CTX_free(ctx);
			WARNING_EX_LOG("hmac init faied");
			 
		}

		if (HMAC_Update(ctx, (const unsigned char*)input, input_length) < 0) 
		{
			HMAC_CTX_free(ctx);
			WARNING_EX_LOG("hmac update faied");
			return -1;
		}

		if (HMAC_Final(ctx, (unsigned char*)output, &output_length) < 0) 
		{
			HMAC_CTX_free(ctx);
			WARNING_EX_LOG("hmac final faied");
			return -1;
			 
		}

		HMAC_CTX_free(ctx);

		return err;
	}

	crtc_stun_packet::~crtc_stun_packet()
	{

	}

	int32 crtc_stun_packet::decode(const char * buf, const int32 nb_buf)
	{

		if (!is_stun((const uint8*)buf, nb_buf))
		{
			WARNING_EX_LOG("parse packet not is stun !!!");
			return -1;
		}
		 
		m_message_type		=  rtc_byte::get2bytes((const uint8*)buf, 0);
		uint16 message_len  =  rtc_byte::get2bytes((const uint8*)buf, 2);
		// length field must be total size minus header's 20 bytes, and must be multiple of 4 Bytes.
		if ((static_cast<size_t>(message_len) != nb_buf - 20) || ((message_len & 0x03) != 0))
		{ 
			WARNING_EX_LOG("ice, length field + 20 does not match total size (or it is not multiple of 4 bytes),  packet discarded ,invalid stun packet, message_len = %d, nb_buf = %d", message_len, nb_buf);
			return -1;
		}


		/*std::string magic_cookie = rtc_byte::get4bytes((const uint8_t*)buf, 4);
		transcation_id = stream->read_string(12);*/


		while (stream->left() >= 4) 
		{
			uint16_t type = stream->read_2bytes();
			uint16_t len = stream->read_2bytes();

			if (stream->left() < len) 
			{
				return   error_new(ERROR_RTC_STUN, "invalid stun packet");
			}

			std::string val = stream->read_string(len);
			// padding
			if (len % 4 != 0) 
			{
				stream->read_string(4 - (len % 4));
			}

			switch (type) {
			case EUsername: 
			{
				username = val;
				size_t p = val.find(":");
				if (p != string::npos) {
					local_ufrag = val.substr(0, p);
					remote_ufrag = val.substr(p + 1);
					 verbose("stun packet local_ufrag=%s, remote_ufrag=%s", local_ufrag.c_str(), remote_ufrag.c_str());
				}
				break;
			}

			case EUseCandidate: 
			{
				use_candidate = true;
				 verbose("stun use-candidate");
				break;
			}

							   // @see: https://tools.ietf.org/html/draft-ietf-ice-rfc5245bis-00#section-5.1.2
							   // One agent full, one lite:  The full agent MUST take the controlling
							   // role, and the lite agent MUST take the controlled role.  The full
							   // agent will form check lists, run the ICE state machines, and
							   // generate connectivity checks.
			case EIceControlled: {
				ice_controlled = true;
				 verbose("stun ice-controlled");
				break;
			}

			case EIceControlling: {
				ice_controlling = true;
				 verbose("stun ice-controlling");
				break;
			}

			default: {
				 verbose("stun type=%u, no process", type);
				break;
			}
			}
		}

		//return err;

		return int32();
	}

	int32 crtc_stun_packet::encode(const std::string & pwd, const char * buffer, const int32 nb_buf)
	{
		return int32();
	}

	int32 crtc_stun_packet::encode_binding_response(const std::string & pwd, const char * buffer, const int32 nb_buf)
	{
		return int32();
	}

	std::string crtc_stun_packet::encode_username()
	{
		return std::string();
	}

	std::string crtc_stun_packet::encode_mapped_address()
	{
		return std::string();
	}

	std::string crtc_stun_packet::encode_hmac(char * hamc_buf, const int32 hmac_buf_len)
	{
		return std::string();
	}

	std::string crtc_stun_packet::encode_fingerprint(uint32 crc32)
	{
		return std::string();
	}

}