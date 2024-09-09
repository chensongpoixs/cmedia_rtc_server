/***********************************************************************************************
created: 		2023-12-28

author:			chensong

purpose:		Íø¿¨ÐÅÏ¢
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
#include "cjwt_create_token.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <iostream>
namespace chen {


	namespace jwt {
	

		//java 从最后 删除'='个数  url-unfriendly base64 char u
		int remove_padding( char*  data, size_t len )
		{
			int paddingCount = 0;
			//int encoded_payload_len = strlen(encoded_payload);
			for (int i = len - 1; i > 0; --i)
			{
				if (data[i] == '=')
				{
					++paddingCount;
				}
				else
				{
					break;
				}
			}
			//replace URL-unfriendly Base64 chars to url-friendly ones:
			for (int i = 0; i < len; ++i)
			{
				if (data[i] == '+')
				{
					data[i] = '-';
				}
				else if (data[i] == '/')
				{
					data[i] = '_';
				}
			}

			return len - paddingCount;
		}



		std::string create_token( const std::string& secret_key,  const std::string& payload)
		{
			// 构建头部// \"typ\":\"JWT\",
			static const char* header = "{\"typ\":\"JWT\",\"alg\":\"HS256\"}";
			char buffer[10240] = {0};

			// 计算 Base64 编码后头部的长度
			int header_len = EVP_ENCODE_LENGTH(strlen(header));

			// 计算 Base64 编码后负载的长度
			int payload_len =   payload.length();// = EVP_ENCODE_LENGTH(strlen(payload));

			// 分配内存存储 Base64 编码后的头部和负载
			 
			char  encoded_header[4096] = {0};
			char  encoded_payload[4096] = {0};
			 
			// Base64 编码头部和负载
			EVP_EncodeBlock((unsigned char*)encoded_header , (const unsigned char*)header, strlen(header));
			EVP_EncodeBlock((unsigned char*)encoded_payload , (const unsigned char*)payload.c_str(), payload.length());

 


			// 构建待签名字符串
			int unsigned_data_len = remove_padding( encoded_header , strlen(encoded_header) ) + remove_padding(encoded_payload, strlen(encoded_payload) ) + 2; // 加上两个点号
			 
			snprintf(buffer, unsigned_data_len, "%s.%s", std::string(encoded_header, remove_padding((char*)encoded_header , strlen(encoded_header) )).c_str(), std::string(encoded_payload, remove_padding((char*)encoded_payload ,  strlen(encoded_payload))).c_str());
			 
			// 使用 HMAC SHA-512 对待签名字符串进行签名
			signed char hmac_result[EVP_MAX_MD_SIZE];// = { 0 };
			unsigned int hmac_len;// = 0;
			HMAC(EVP_sha256(), secret_key.c_str(), secret_key.length(), (const unsigned char*)buffer, strlen(buffer), (unsigned char*)hmac_result, &hmac_len);

			//HMAC(EVP_sha512(), secret_key.c_str(), secret_key.length(), (const unsigned char*)buffer, strlen(buffer), (unsigned char*)hmac_result, &hmac_len);

			// Base64 编码签名结果
			int signature_len = EVP_ENCODE_LENGTH(hmac_len);
			char encoded_signature[4096 * 2] = {0}; 
			EVP_EncodeBlock((unsigned char*)encoded_signature , (unsigned char*)hmac_result, hmac_len);

			// 构建 JWT token
			int token_len = unsigned_data_len + remove_padding((char *)encoded_signature , strlen(encoded_signature) ) + 1; // 加上两个点号和结尾的 null 字符
			 
			 snprintf(buffer, token_len, "%s.%s.%s", std::string(encoded_header, remove_padding((char*)encoded_header, strlen(encoded_header))).c_str(), std::string(encoded_payload, remove_padding((char*)encoded_payload, strlen(encoded_payload))).c_str(), std::string(encoded_signature, remove_padding((char*)encoded_signature, strlen(encoded_signature))).c_str());

			 
		
			return buffer;
		}
		void test_create_token()
		{
			std::string key = "syz.20120328";
			std::string payload = "{\"syz.20120328\":\"{\\\"userId\\\":21}\",\"sub\":\"syz.20120328\"}";
			std::string token = create_token(key, payload);
			std::cout << "jwt token = " << token << std::endl;
		}
		void test_siample_create_token()
		{	// eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJhdWQiOiJhZG1pbiJ9.uPchyky8YSNmaJjxTHPIM_CnS9X8hcjzLp9H55aohag
			// eyJhbGciOiJIUzUxMiJ9.YWRtaW4.GpUM8DOkNOKFv7U7FGocvzGrJqVRTlVBfTAm5ZcGFLM
			// eyJhbGciOiJIUzI1NiJ9.eyJhdWQiOiJhZG1pbiJ9.57_yU-DN8JUik8E-3Mz-SYyaB-lbuBIfw2eGJw8yPoc
			// eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJhdWQiOiJhZG1pbiJ9.uPchyky8YSNmaJjxTHPIM_CnS9X8hcjzLp9H55aohag
			// eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJhdWQiOiJhZG1pbiJ9.uPchyky8YSNmaJjxTHPIM_CnS9X8hcjzLp9H55aohag
			// eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.YWRtaW4.3oX27EpiWv0SXdPCgPRToX4-OvU7u68Y3RguGyI_EyE
			std::string key = "syz@8015";
			std::string payload = "{\"aud\":\"admin\"}";
			std::string token = create_token(key, payload);
			std::cout << "jwt token = " << token << std::endl;
		}
	}
}