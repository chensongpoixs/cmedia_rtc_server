//#define MS_CLASS "RTC::RtpHeaderExtensionUri"
// #define MS_LOG_DEV_LEVEL 3

//#include "Logger.hpp"
//#include "Utils.hpp"
#include "RtpDictionaries.hpp"
#include "clog.h"
#include "crtc_util.h"
using namespace chen;
namespace RTC
{
	/* Class variables. */

	// clang-format off
	std::unordered_map<std::string, RtpHeaderExtensionUri::Type> RtpHeaderExtensionUri::string2Type =
	{
		//  �� unified SDP ������ ��a=mid�� ��ÿ�� audio/video line �ı�ҪԪ�أ���� header extension �� SDP �� ��a=mid�� ����Ϣ���棬
		// ���ڱ�ʶһ�� RTP packet �� media ��Ϣ��������Ϊһ�� media ��Ψһ��ʶ 
		{ "urn:ietf:params:rtp-hdrext:sdes:mid",                                       RtpHeaderExtensionUri::Type::MID                    },
		// 1. Media Source ��ͬ�� WebRTC �� Track �ĸ���� SDP �����п���ʹ�� mid ��ΪΨһ��ʶ
		// 2. RTP Stream �� RTP ���������С����λ�������� Simulcast �� SVC �����У�һ�� Media Source �а������ RTP Stream����ʱ SDP ��ʹ�� ��a=rid�� ������ÿ�� RTP Stream
		{ "urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id",                             RtpHeaderExtensionUri::Type::RTP_STREAM_ID          },
		// ���������ش�ʱʹ�õ� rid ��ʶ	
		{ "urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id",                    RtpHeaderExtensionUri::Type::REPAIRED_RTP_STREAM_ID },
		// 1. abs-send-time Ϊ һ�� 3 bytes ��ʱ������
		// 2. REMB ������Ҫ RTP ������չͷ�� abs-send-time ��֧�֣����Լ�¼ RTP ���ݰ��ڷ��Ͷ˵ľ��Է���ʱ��
		{ "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time",                RtpHeaderExtensionUri::Type::ABS_SEND_TIME          },
		//Transport-wide Congestion Control

		///////////////////////////////////////////////////////////////////////////////////////////////////
		// ��Ч����ֻ�� 16 bit����¼��һ�� sequence number ��Ϊ transport-wide sequence number
		// ���Ͷ��ڷ��� RTP ���ݰ�ʱ���� RTP ͷ����չ�����ô�������к� TransportSequenceNumber��
		// ���ݰ�������ն˺��¼�����кźͰ�����ʱ�䣬Ȼ����ն˻��ڴ˹��� TransportCC ���ķ��ص����Ͷˣ�
		// ���Ͷ˽����ñ��Ĳ�ִ�� SendSide-BWE �㷨������õ������ӳٵ����ʣ�
		// ���պͻ��ڶ����ʵ����ʽ��бȽϵõ�����Ŀ������
		{ "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01", RtpHeaderExtensionUri::Type::TRANSPORT_WIDE_CC_01   },
		// NOTE: Remove this once framemarking draft becomes RFC.


		{ "http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07",              RtpHeaderExtensionUri::Type::FRAME_MARKING_07       },


		// ���� WebRTC �� RTP payload ͨ�� SRTP ���м��ܣ��������� RTP packet �ھ��������ڵ��ת���ڵ�ʱ��
		// ��Щ��������Ҫ֪����ǰ RTP packet �ı�����Ϣ��framemarking ���ڸ����ñ�����Ϣ 
		{ "urn:ietf:params:rtp-hdrext:framemarking",                                   RtpHeaderExtensionUri::Type::FRAME_MARKING          },

		// ������������
		{ "urn:ietf:params:rtp-hdrext:ssrc-audio-level",                               RtpHeaderExtensionUri::Type::SSRC_AUDIO_LEVEL       },

		// ���շ��յ��Ժ�Ҳ������Ӧ�м���� tag ���ԣ�����������˸ù��ܵ�Э�̡�Tag �е� 7 ����չ����֡����չ id��
		// ������ 1-15 �е��κ�һ�� , ������ʶ�÷������ݵ�λ�ã�����ο� RFC5285�����������ο� RCC.07- 2.7.1.2.2��
		// ������������Ƶ������ʵ���������ݵķ���ͷ��ͷ� camera ��ѡ�ǰ�û��ߺ��ã���Ϊ�˷��������һ�¶���Ϊ��Ƶ�������ݡ�
		{ "urn:3gpp:video-orientation",                                                RtpHeaderExtensionUri::Type::VIDEO_ORIENTATION      },

		// ����ʱ��ƫ�� (Transmission Time Offset)��offset Ϊ RTP packet �� timestamp �� ʵ�ʷ���ʱ���ƫ��
		{ "urn:ietf:params:rtp-hdrext:toffset",                                        RtpHeaderExtensionUri::Type::TOFFSET                }

	};
	// clang-format on

	/* Class methods. */

	RtpHeaderExtensionUri::Type RtpHeaderExtensionUri::GetType(std::string& uri)
	{
		//MS_TRACE();

		// Force lowcase names.
		std::transform(uri.begin(), uri.end(), uri.begin(), ::tolower);
		//Utils::String::ToLowerCase(uri);

		auto it = RtpHeaderExtensionUri::string2Type.find(uri);

		if (it != RtpHeaderExtensionUri::string2Type.end())
		{
			return it->second;
		}

		return RtpHeaderExtensionUri::Type::UNKNOWN;
	}
} // namespace RTC
