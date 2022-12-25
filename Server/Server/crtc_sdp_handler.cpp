
/***********************************************************************************************
created: 		2022-09-10

author:			chensong

purpose:		crtc_sdp_handler


************************************************************************************************/
#include "crtc_sdp.h"
#include "crtc_sdp_util.h"
#include "crtc_sdp_define.h"
#include <absl/utility/utility.h>
#include <absl/types/optional.h>
#include <sstream>
//#include "RtcSdpDefine.pb.h"
#include "clog.h"
#if defined(_MSC_VER)
#include <ws2spi.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <in6addr.h>
#endif // 
#include "crtc_sdp.h"

//namespace chen {
//	void crtc_sdp::_config_media()
//	{
//		////////////////////////匹配支持编码/////////////////////////////////
//		/*	{
//					kind       : 'video',
//					mimeType   : 'video/h264',
//					clockRate  : 90000,
//					parameters :
//					{
//						'packetization-mode'      : 1,
//						'profile-level-id'        : '42e01f',
//						'level-asymmetry-allowed' : 1,
//						'x-google-start-bitrate'  : 1000,
//						'x-google-max-bitrate'    : 28000,
//						'x-google-min-bitrate'    : 5500,
//					}
//				}
//		*/
//		size_t index = 0;
//		for (size_t i = 0; i < m_media_datas.size(); ++i)
//		{
//			if (m_media_datas[i].m_codec_type == RTC::RtpCodecMimeType::Type::VIDEO)
//			{
//				index = i;
//				break;
//			}
//		}
//		std::vector<RTC::RtpCodecParameters> rtp_codec_parameters;
//		// 匹配编码算法
//		for (size_t i = 0; i < m_media_datas[index].codecs.size(); ++i)
//		{
//			if (m_media_datas[index].codecs[i].clockRate == 90000 && RTC::RtpCodecMimeType::Subtype::H264 == m_media_datas[index].codecs[i].mimeType.subtype)
//			{
//				rtp_codec_parameters.push_back(m_media_datas[index].codecs[i]);
//				for (size_t rtx_i = 0; rtx_i < m_media_datas[index].codecs.size(); ++rtx_i)
//				{
//					if (m_media_datas[index].codecs[rtx_i].payloadType == m_media_datas[index].codecs[i].rtx_payloadType)
//					{
//						rtp_codec_parameters.push_back(m_media_datas[index].codecs[rtx_i]);
//						break;
//						 
//					}
//				}
//				break;
//			}
//		}
//		m_media_datas[index].codecs = rtp_codec_parameters;
//		//RTC::RtpParameters rtp_parameter = m_media_datas[index];
//
//
//
//	}
//	bool crtc_sdp::_handler_sdp_v(const uint8_t * line_data, size_t line_data_size)
//	{
//		return true;
//	}
//	bool crtc_sdp::_handler_sdp_o(const uint8_t * line_data, size_t line_data_size)
//	{
//		return true;
//	}
//	bool crtc_sdp::_handler_sdp_t(const uint8_t * line_data, size_t line_data_size)
//	{
//		return true;
//	}
//	bool crtc_sdp::_handler_sdp_a(const uint8_t * line_data, size_t line_data_size)
//	{
//		// a=group:BUNDLE 0 1
//		// a=msid-semantic: WMS stream_id
//		//std::vector<
//		std::string key;
//		size_t cur_read_index = 2;
//
//		while (cur_read_index < line_data_size &&  line_data[cur_read_index] != ':')
//		{
//			key.push_back(line_data[cur_read_index++]);
//		}
//		
//		// line end
//		if (cur_read_index >= line_data_size)
//		{
//			//  rtcp-mux
//			//  sendrecv
//			//  rtcp-rsize
//			return true;
//		}
//		if (line_data[cur_read_index] == ':')
//		{
//			++cur_read_index;
//		}
//		std::vector<std::string> fields;
//		_parse_line_value_array(line_data, cur_read_index, line_data_size, fields);
//		
//		// 1. group:BUNDLE 
//		if (key == "group")
//		{
//			if (fields.size() <= 1)
//			{
//				WARNING_EX_LOG("parse sdp not find group:BUNDLE  !!!");
//				return false;
//			}
//			for (size_t i = 1; i < fields.size(); ++i)
//			{
//				//int32 mid = std::atoi(fields[i].c_str());
//				//RTC::RtpParameters rtpParameter;
//				//rtpParameter.mid = std::atoi(fields[i].c_str());
//				//m_media_datas.push_back(rtpParameter);
//			}
//		}
//		else if ("msid-semantic" == key)
//		{
//
//		}
//		else if ("ice-ufrag" == key)
//		{
//
//		}
//		else if ("ice-pwd" == key)
//		{
//
//		}
//		else if ("ice-option" == key)
//		{
//
//		}
//		else if ("fingerprint" == key)
//		{
//			if (fields.size() != 2)
//			{
//				WARNING_EX_LOG("fingerprint size = %u", fields.size());
//				return false;
//			}
//
//			m_finger_print.algorithm = RTC::DtlsTransport::GetFingerprintAlgorithm(fields[0]);
//			m_finger_print.value = fields[1];
//		}
//		else if ("setup" == key)
//		{
//
//		}
//		else if ("mid" == key)
//		{
//			if (fields.size() != 1)
//			{
//				WARNING_EX_LOG("mid not size = %u", fields.size());
//				return false;
//			}
//			m_rtp_parameter.mid = fields[0];
//		}
//		else if ("extmap" == key)
//		{
//			if (fields.size() != 2)
//			{
//				WARNING_EX_LOG("extamp size != 2 size = %u", fields.size());
//				return false;
//			}
//			RTC::RtpHeaderExtensionParameters rtp_header_extension_parameter;
//			rtp_header_extension_parameter.uri = fields[1];
//			rtp_header_extension_parameter.id = std::atoi(fields[0].c_str());
//			rtp_header_extension_parameter.encrypt = false;
//			rtp_header_extension_parameter.type = RTC::RtpHeaderExtensionUri::GetType(rtp_header_extension_parameter.uri);
//			m_rtp_parameter.headerExtensions.push_back(rtp_header_extension_parameter);
//		}
//		else if ("sendrecv" == key)
//		{
//
//		}
//		else if ("msid" == key)
//		{
//
//		}
//		else if ("rtpmap" == key)
//		{
//			if (fields.size() != 2)
//			{
//				WARNING_EX_LOG("rtpmap size = %u !!!", fields.size());
//				return false;
//			}
//
//			////////////////////////匹配支持编码/////////////////////////////////
//		/*	{
//					kind       : 'video',
//					mimeType   : 'video/h264',
//					clockRate  : 90000,
//					parameters :
//					{
//						'packetization-mode'      : 1,
//						'profile-level-id'        : '42e01f',
//						'level-asymmetry-allowed' : 1,
//						'x-google-start-bitrate'  : 1000,
//						'x-google-max-bitrate'    : 28000,
//						'x-google-min-bitrate'    : 5500,
//					}
//				}*/
//			/// 
//			uint8_t payloadType = std::atoi(fields[0].c_str());
//			size_t index = 0;
//			for (size_t i = 0; i < m_rtp_parameter.codecs.size(); ++i)
//			{
//				if (m_rtp_parameter.codecs[i].payloadType == payloadType)
//				{
//					index = i;
//					break;
//				}
//			}
//			m_rtp_parameter.codecs[index].mimeType.mimeType = fields[1];
//
//			auto slashPos = fields[1].find('/');
//
//			if (slashPos == std::string::npos || slashPos == 0 || slashPos == fields[1].length() - 1)
//			{
//				ERROR_EX_LOG("wrong codec MIME");
//				return false;
//			}
//
//				std::string type    = fields[1].substr(0, slashPos);
//				std::string subtype = fields[1].substr(slashPos + 1);
//
//			//	// Force lowcase names.
//			//	//Utils::String::ToLowerCase(type);
//				std::transform(type.begin(), type.end(), type.begin(), ::tolower);
//				//std::transform(subtype.begin(), subtype.end(), subtype.begin(), ::tolower);
//				//Utils::String::ToLowerCase(subtype);
//
//			//	// Set MIME type.
//			{
//				 
//					m_rtp_parameter.codecs[index].mimeType.type = m_rtp_parameter.m_codec_type; // it->second;
//					//this->type = it->second;
//			}
//
//			//	// Set MIME subtype.
//				{
//					auto it = RTC::RtpCodecMimeType::string2Subtype.find(type);
//
//					if (it == RTC::RtpCodecMimeType::string2Subtype.end())
//					{
//						ERROR_EX_LOG("unknown codec MIME type '%s'", type.c_str());
//						return false;
//					}
//
//					m_rtp_parameter.codecs[index].mimeType.subtype = it->second;
//					
//				}
//				// // opus/48000/2
//				auto subtypePos = subtype.find('/');
//				if (subtypePos == std::string::npos || subtypePos == 0 || subtypePos == subtype.length() - 1)
//				{
//					m_rtp_parameter.codecs[index].clockRate = std::atoi(subtype.c_str());
//				}
//				else
//				{
//					std::string clock_rate = subtype.substr(0, subtypePos);
//					m_rtp_parameter.codecs[index].clockRate = std::atoi(clock_rate.c_str());
//				}
//			//	// Set mimeType.
//			/*	this->mimeType = RtpCodecMimeType::type2String[this->type] + "/" +
//				                 RtpCodecMimeType::subtype2String[this->subtype];*/
//			//	//if ("opus" == subtype) // audio webrtc bug  opus 2 channel 
//			//	//{
//			//	//	this->mimeType += "/" + std::to_string(2);
//			//	//}
//		}
//		else if ("rtcp-fb" == key)
//		{
//			if (fields.size() != 2)
//			{
//				WARNING_EX_LOG("rtcp-fb size = %u !!!", fields.size());
//				return false;
//			}
//			uint8_t payloadType = std::atoi(fields[0].c_str());
//			size_t index = 0;
//			for (size_t i = 0; i < m_rtp_parameter.codecs.size(); ++i)
//			{
//				if (m_rtp_parameter.codecs[i].payloadType == payloadType)
//				{
//					index = i;
//					break;
//				}
//			}
//			RTC::RtcpFeedback rtcp_feedback;
//			rtcp_feedback.type = fields[1];
//			m_rtp_parameter.codecs[index].rtcpFeedbacks.push_back(rtcp_feedback);
//		}
//		else if ("fmtp" == key)
//		{
//
//			if (fields.size() != 2)
//			{
//				WARNING_EX_LOG("fmtp size = %u !!!", fields.size());
//				return false;
//			}
//			uint8_t payloadType = std::atoi(fields[0].c_str());
//			size_t index = 0;
//			for (size_t i = 0; i < m_rtp_parameter.codecs.size(); ++i)
//			{
//				if (m_rtp_parameter.codecs[i].payloadType == payloadType)
//				{
//					index = i;
//					break;
//				}
//			}
//			/**
//			*   RTX --> [ apt <--> rtp]
//			*	a=rtpmap:96 H264/90000
//				a=rtcp-fb:96 goog-remb
//				a=rtcp-fb:96 transport-cc
//				a=rtcp-fb:96 ccm fir
//				a=rtcp-fb:96 nack
//				a=rtcp-fb:96 nack pli
//				a=fmtp:96 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42001f;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000
//				a=rtpmap:97 rtx/90000
//				a=fmtp:97 apt=96;x-google-max-bitrate=100000;x-google-min-bitrate=4000;x-google-start-bitrate=8000
//			*
//			*/
//			// 
//		//	RTC::Parameters parameter;
//			_parse_value_array(fields[1], m_rtp_parameter.codecs[index].parameters);
//			if (RTC::RtpCodecMimeType::Subtype::RTX == m_rtp_parameter.codecs[index].mimeType.subtype)
//			{
//				// video ----> RTX
//				// apt 
//				std::unordered_map<std::string, RTC::Parameters::Value>::iterator iter = m_rtp_parameter.codecs[index].parameters.mapKeyValues.find("apt");
//				if (iter != m_rtp_parameter.codecs[index].parameters.mapKeyValues.end())
//				{ 
//					for (size_t rtx_i = 0; rtx_i < m_rtp_parameter.codecs.size(); ++rtx_i)
//					{
//						if (m_rtp_parameter.codecs[rtx_i].payloadType == std::atoi(iter->second.stringValue.c_str()))
//						{
//							m_rtp_parameter.codecs[rtx_i].rtx_payloadType = payloadType;
//							break;
//						}
//					}
//				}
//
//			}
//
//		}
//		else if ("ssrc-group" == key)
//		{
//			// ssrc // RTX ssrc
//			if (fields.size() < 2)
//			{
//				WARNING_EX_LOG("ssrc-group size = %u !!!", fields.size());
//				return false;
//			}
//
//			RTC::RtpEncodingParameters rtp_encoding_parameter;
//			rtp_encoding_parameter.ssrc = std::atoi(fields[0].c_str());
//			if (fields.size() > 2)
//			{
//				rtp_encoding_parameter.hasRtx = true; // video
//				rtp_encoding_parameter.rtx.ssrc = std::atoi(fields[1].c_str());;
//			}
//			m_rtp_parameter.encodings.push_back(rtp_encoding_parameter);
//
//		}
//		else if ("ssrc" == key)
//		{
//			if (fields.size() != 2)
//			{
//				WARNING_EX_LOG("ssrc size = %u !!!", fields.size());
//				return false;
//			}
//			m_rtp_parameter.rtcp.ssrc = std::atoi(fields[0].c_str());
//			m_rtp_parameter.rtcp.reducedSize = true;
//			char cname[1024] = { 0 };
//			int32 cname_index = 6;
//			int32 cname_size = 0;
//			while (fields[1].size() > cname_index)
//			{
//				cname[cname_size++] = fields[1][cname_index++];
//			}
//			m_rtp_parameter.rtcp.cname = cname;
//			//m_media_datas.push_back(m_rtp_parameter);
//		}
//		else  
//		{
//			WARNING_EX_LOG("key = %s", key.c_str());
//		}
//		// 
//		NORMAL_EX_LOG("==================================a= %s ===================================================================", key.c_str());
//		std::ostringstream cmd;
//		for (const std::string & field : fields)
//		{
//			cmd << "," << field;
//		}
//		NORMAL_EX_LOG("[fileds size = %u][%s]", fields.size(), cmd.str().c_str());
//		fields.clear();
//
//		return true;
//	}
//	bool crtc_sdp::_handler_sdp_m(const uint8_t * line_data, size_t line_data_size)
//	{
//		// 1. audio, video , application
//		std::string key;
//		size_t cur_read_index = 2;
//
//		while (cur_read_index < line_data_size &&  line_data[cur_read_index] != ' ')
//		{
//			key.push_back(line_data[cur_read_index++]);
//		}
//
//		std::vector<std::string> fields;
//		_parse_line_value_array(line_data, cur_read_index, line_data_size, fields);
//		NORMAL_EX_LOG("==================================m= %s ===================================================================", key.c_str());
//
//		if (fields.size() < 3)
//		{
//			WARNING_EX_LOG(" m line [key = %s][size = %lu]", key.c_str(), fields.size());
//			return false;
//		}
//		
//		RTC::RtpParameters _temp_rtp_parameter;
//		m_rtp_parameter = _temp_rtp_parameter; // std::swap(_temp_rtp_parameter);
//
//		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
//
//		auto it = RTC::RtpCodecMimeType::string2Type.find(key);
//
//		if (it == RTC::RtpCodecMimeType::string2Type.end())
//		{
//			ERROR_EX_LOG("unknown m line type MIME key '%s'", key.c_str());
//			return false;
//		}
//
//		m_rtp_parameter.m_codec_type = it->second;
//		for (size_t i = 2; i < fields.size(); ++i)
//		{
//			RTC::RtpCodecParameters rtpcodecparameter;
//			rtpcodecparameter.payloadType = std::atoi(fields[i].c_str());
//			m_rtp_parameter.codecs.push_back(rtpcodecparameter);
//
//		}
//		std::ostringstream cmd;
//		for (const std::string & field : fields)
//		{
//			cmd << "," << field;
//		}
//		NORMAL_EX_LOG("[fileds size = %u][%s]", fields.size(), cmd.str().c_str());
//		fields.clear();
//
//		return true;
//		return true;
//	}
//	bool crtc_sdp::_handler_sdp_c(const uint8_t * line_data, size_t line_data_size)
//	{
//		return false;
//	}
//	bool crtc_sdp::_parse_line_value_array(const uint8_t * line_data, size_t cur_index, size_t line_data_size, std::vector<std::string>& data)
//	{
//		//DEBUG_EX_LOG("[data = %s][index = %lu][data_size = %lu]", line_data, cur_index, line_data_size);
//		std::string value;
//		for (size_t i = cur_index; i < line_data_size; ++i)
//		{
//			if (line_data[i] != ' ')
//			{
//				value.push_back(line_data[i]);
//			}
//			else if (!value.empty())
//			{
//				data.push_back(value);
//				value.clear();
//			}
//		}
//		if (!value.empty())
//		{
//			data.push_back(value);
//			value.clear();
//		}
//		return true;
//	}
//	bool crtc_sdp::_parse_value_array(const std::string & data_value, RTC::Parameters & data)
//	{
//		std::string key;
//		std::string value;
//		for (size_t i = 0; i < data_value.size(); ++i)
//		{
//			if (data_value[i] == '=')
//			{
//				
//				for (i += 1; i < data_value.size(); ++i)
//				{
//					if (data_value[i] == ';')
//					{
//						data.mapKeyValues.emplace(key, RTC::Parameters::Value(value));
//						key.clear();
//						value.clear();
//						break;
//					}
//					else
//					{
//						value.push_back(data_value[i]);
//					}
//				}
//			}
//			else
//			{
//				key.push_back(data_value[i]);
//			}
//			//else if (!value.empty())
//			//{
//			//	//data.push_back(value);
//			//	//value.clear();
//			//}
//		}
//		if (!value.empty() && !key.empty())
//		{
//			data.mapKeyValues.emplace(key, RTC::Parameters::Value(value));
//			//data.push_back(value);
//			//value.clear();
//		}
//		return true;
//	}
//}