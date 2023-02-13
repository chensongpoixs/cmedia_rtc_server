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
#include "ccrc32.h"

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


		
		uint32 magic_cookie = rtc_byte::get4bytes((const uint8_t*)buf, 4);
		m_transcation_id = rtc_byte::getbytes((const uint8_t*)buf, 8, 12);

		int32 pos = 20; 
		const char * p = buf + pos;
		// Ensure there are at least 4 remaining bytes (attribute with 0 length).
		while (pos +4  <= nb_buf) 
		{
			uint16  type = rtc_byte::get2bytes((const uint8_t*)p, 0); //  stream->read_2bytes();
			uint16  len = rtc_byte::get2bytes((const uint8_t*)p, 2);// stream->read_2bytes();

			//if (stream->left() < nb_buf)
			//{
			//	WARNING_EX_LOG("invalid stun packet");
			//	return -1;
			//	//return   error_new(ERROR_RTC_STUN, "invalid stun packet");
			//}

			std::string val = rtc_byte::getbytes((const uint8_t*)p, 4, len);// stream->read_string(len);
			// padding
			if (len % 4 != 0) 
			{
				//stream->read_string(4 - (len % 4));
			}

			switch (type) {
			case EUsername: 
			{
				//username = val;
				m_username = val;
				size_t p = val.find(":");
				if (p != std::string::npos) 
				{
					m_local_ufrag = val.substr(0, p);
					m_remote_ufrag = val.substr(p + 1);
					NORMAL_EX_LOG("stun packet local_ufrag=%s, remote_ufrag=%s", m_local_ufrag.c_str(), m_remote_ufrag.c_str());
				}
				break;
			}

			case EUseCandidate: 
			{
				m_use_candidate = true;
				 NORMAL_EX_LOG("stun use-candidate");
				break;
			}

							   // @see: https://tools.ietf.org/html/draft-ietf-ice-rfc5245bis-00#section-5.1.2
							   // One agent full, one lite:  The full agent MUST take the controlling
							   // role, and the lite agent MUST take the controlled role.  The full
							   // agent will form check lists, run the ICE state machines, and
							   // generate connectivity checks.
			case EIceControlled: 
			{
				m_ice_controlled = true;
				 NORMAL_EX_LOG("stun ice-controlled");
				break;
			}

			case EIceControlling: 
			{
				m_ice_controlling = true;
				NORMAL_EX_LOG("stun ice-controlling");
				break;
			}
			/*case EPriority:
			{
				break;
			}*/

			default:
			{
				NORMAL_EX_LOG("stun type=%u, no process", type);
				break;
			}
			}

			// Set next attribute position.
			pos = static_cast<size_t>( pos + 4 + len);
			
		}
		// Ensure current position matches the total length.
		if (pos != nb_buf)
		{
			WARNING_EX_LOG("ice, computed packet size does not match total size, packet discarded");

			return -1;
		}
		//return err;

		return 0;
	}

	int32 crtc_stun_packet::encode(const std::string & pwd,  cbuffer * buffer)
	{
		if (is_binding_response()) 
		{
			return encode_binding_response(pwd, buffer);
		}

		WARNING_EX_LOG("unknown stun type=%d", get_message_type());
		return -1;
	}

	int32 crtc_stun_packet::encode_binding_response(const std::string & pwd,  cbuffer* stream)
	{
		uint32 err = 0;
		std::string property_username = encode_username();
		std::string mapped_address = encode_mapped_address();

		stream->write_2bytes(EBindingResponse);
		//rtc_byte::set2bytes((uint8 * )buffer, 0, EBindingResponse);
		stream->write_2bytes(property_username.size() + mapped_address.size());
		//rtc_byte::set2bytes((uint8*)buffer, 2, property_username.size() + mapped_address.size());
		stream->write_4bytes(KStunMagicCookie);
		//rtc_byte::set4bytes((uint8*)buffer, 4, KStunMagicCookie);
		stream->write_string(m_transcation_id);
		//rtc_byte::setbytes((uint8*)buffer, 8, (uint8*)(m_transcation_id.c_str()), m_transcation_id.size());

		stream->write_string(property_username);
		//rtc_byte::setbytes((uint8*)buffer, 8+m_transcation_id.size(), (uint8*)property_username.c_str(), property_username.size());
		
		stream->write_string(mapped_address);
		//rtc_byte::setbytes((uint8*)buffer, 8 + m_transcation_id.size() + property_username.size(), 
		//	(uint8*)mapped_address.c_str(), mapped_address.size());

		stream->data()[2] = ((stream->pos() - 20 + 20 + 4) & 0x0000FF00) >> 8;
		stream->data()[3] = ((stream->pos() - 20 + 20 + 4) & 0x000000FF);

		char hmac_buf[20] = { 0 };
		unsigned int hmac_buf_len = 0;
		if ((err = hmac_encode("sha1", pwd.c_str(), pwd.size(), stream->data(), stream->pos(), hmac_buf, hmac_buf_len)) != 0)
		{
			WARNING_EX_LOG("hmac encode failed");
			return err;
			//return  error_wrap(err, "hmac encode failed");
		}

		std::string hmac = encode_hmac(hmac_buf, hmac_buf_len);

		stream->write_string(hmac);
		stream->data()[2] = ((stream->pos() - 20 + 8) & 0x0000FF00) >> 8;
		stream->data()[3] = ((stream->pos() - 20 + 8) & 0x000000FF);

		uint32_t crc32 = srs_crc32_ieee(stream->data(), stream->pos(), 0) ^ 0x5354554E;

		std::string fingerprint = encode_fingerprint(crc32);

		stream->write_string(fingerprint);

		stream->data()[2] = ((stream->pos() - 20) & 0x0000FF00) >> 8;
		stream->data()[3] = ((stream->pos() - 20) & 0x000000FF);
		return err;
	}

	std::string crtc_stun_packet::encode_username()
	{
		char buf[1460] = {0};
		 
		std::string username = m_remote_ufrag + ":" + m_local_ufrag;
		rtc_byte::set2bytes((unsigned char *)&buf[0], 0, EUsername);
		//stream->write_2bytes(EUsername);
		rtc_byte::set2bytes((unsigned char *)&buf[0], 2, username.size());
		//stream->write_2bytes(username.size());
		memcpy(&buf[0] + 4, username.c_str(), username.size());
		// stream->write_string(username);

		if ((4 + username.size()) % 4 != 0) 
		{
			 char padding[4] = { 0 };
			 memcpy(&buf[0] + 4 + username.size(), &padding[0], 4 - (4 + username.size()) %4);
			//stream->write_bytes(padding, 4 - (stream->pos() % 4));
		}

		return buf;
		//return std::string();
	}

	std::string crtc_stun_packet::encode_mapped_address()
	{
		char buf[1460] = {0};
		 
		
		//stream->write_2bytes(EXorMappedAddress);
		rtc_byte::set2bytes((unsigned char *)&buf[0], 0, EXorMappedAddress);
		//stream->write_2bytes(8);
		rtc_byte::set2bytes((unsigned char *)&buf[0], 2, 8);
		//stream->write_1bytes(0); // ignore this bytes
		rtc_byte::set1byte((unsigned char *)&buf[0], 4, 0);
		//stream->write_1bytes(1); // ipv4 family
		rtc_byte::set1byte((unsigned char *)&buf[0], 5, 1);
		//stream->write_2bytes(m_mapped_port ^ (kStunMagicCookie >> 16));
		rtc_byte::set2bytes((unsigned char *)&buf[0], 6, m_mapped_port ^ (KStunMagicCookie >> 16));
		//stream->write_4bytes(m_mapped_address ^ kStunMagicCookie);
		rtc_byte::set4bytes((unsigned char *)&buf[0], 8, m_mapped_address ^ KStunMagicCookie);
		return buf;
		//return std::string();
	}

	std::string crtc_stun_packet::encode_hmac(char * hamc_buf, const int32 hmac_buf_len)
	{
		char buffer[1460] = {0};
		 

		//stream->write_2bytes(EMessageIntegrity);
		rtc_byte::set2bytes((uint8  *)&buffer[0], 0, EMessageIntegrity);
		//stream->write_2bytes(hmac_buf_len);
		rtc_byte::set2bytes((uint8  *)&buffer[0], 2, hmac_buf_len);
		//stream->write_bytes(hmac_buf, hmac_buf_len);
		memcpy((uint8  *)&buffer[0] +4, hamc_buf, hmac_buf_len);
		//rtc_byte::set2bytes((uint8  *)&buffer[0], 2, hmac_buf_len);
		return buffer;
	}

	std::string crtc_stun_packet::encode_fingerprint(uint32 crc32)
	{
		char buffer[1460] = {0};
		 
		
		///stream->write_2bytes(EFingerprint);
		rtc_byte::set2bytes((uint8*)&buffer[0], 0, EFingerprint);
		//stream->write_2bytes(4);
		rtc_byte::set2bytes((uint8*)&buffer[0], 2, 4);
		//stream->write_4bytes(crc32);
		rtc_byte::set4bytes((uint8*)&buffer[0], 4, crc32);
		return buffer;
	}

}