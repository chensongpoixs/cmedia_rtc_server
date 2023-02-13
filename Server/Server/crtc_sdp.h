/***********************************************************************************************
created: 		2022-09-03

author:			chensong

purpose:		crtc_sdp


************************************************************************************************/

#ifndef _C_RTC_SDP_H_
#define _C_RTC_SDP_H_
#include <iostream>
#include <cstdbool>
#include <cstdint>
#include <string>
#include "Producer.hpp"
//#include "RtcSdp.pb.h"
#include "crtc_sdp_define.h"
#include "DtlsTransport.hpp"
#include "cmedia_desc.h"
namespace chen {
	//class crtc_sdp
	//{
	//private:
	//	
	//public:
	//	crtc_sdp()
	//		/*: m_session_td()
	//		, m_session_description()*/
	//		: m_client_sdp("")
	//		, m_current_pos(0)
	//		, m_media_datas()
	//		, m_rtp_parameter()
	//		, m_finger_print()
	//	{}
	//	~crtc_sdp();
	//	bool init(const std::string & sdp);


	//	void destroy();
	//public:
	//	const std::vector< RTC::RtpParameters> & get_rtp_parameters() const { return m_media_datas; }
	//	const RTC::DtlsTransport::Fingerprint  get_finger_print() const { return  m_finger_print; }
	//public:
	//	std::string get_webrtc_sdp() const ;
	//protected:
	//private:
	//	//bool _parse_session_description(const std::string & session_sdp_description);
	//	//bool _parse_media_description(const std::string & media_sdp_description);

	//	bool  _get_line_data(size_t & read_size);
	//	
	//private:
	//	void _config_media();
	//private:
	//	bool  _handler_sdp_v(const uint8_t * line_data, size_t line_data_size);
	//	bool  _handler_sdp_o(const uint8_t * line_data, size_t line_data_size);
	//	bool  _handler_sdp_t(const uint8_t * line_data, size_t line_data_size);
	//	bool  _handler_sdp_a(const uint8_t * line_data, size_t line_data_size);
	//	bool  _handler_sdp_m(const uint8_t * line_data, size_t line_data_size);
	//	bool  _handler_sdp_c(const uint8_t * line_data, size_t line_data_size);
	//	//bool  _handler_sdp_a(const uint8_t * line_data, size_t line_data_size);
	//private:
	//	bool _parse_line_value_array(const uint8_t * line_data, size_t cur_index, size_t line_data_size, std::vector<std::string> & data);

	//	// key=value; key=value;...
	//	bool _parse_value_array(const std::string & value, RTC::Parameters & data);
	//	//bool _handler_sdp_a_group(const uint8_t * line_data, size_t cur_index, size_t line_data_size);
	//	//bool _handler_sdp_a_msid_semantic(const uint8_t * line_data, size_t cur_index, size_t line_data_size);
	//	//bool _parse_connection_data(const std::string& line, Socket_Address & addr/*, rtc::SocketAddress* addr*/);


	//	 
	//private:
	//	//Transport_Description			m_session_td;
	//	//Session_Description				m_session_description;
	//	std::string							m_client_sdp;
	//	size_t								m_current_pos;
	//	std::vector< RTC::RtpParameters>	m_media_datas;
	//	RTC::RtpParameters					m_rtp_parameter;

	//	// crypto
	//	RTC::DtlsTransport::Fingerprint	    m_finger_print;
	//};


	class crtc_sdp
	{
	public:
		explicit  crtc_sdp()
			: m_in_media_session(false)
			, m_version("")
			, m_username("")
			, m_session_id("")
			, m_session_version("")
			, m_nettype("")
			, m_addrtype("")
			, m_unicast_address("")
			, m_session_name("")
			, m_start_time(0)
			, m_end_time(0)
			, m_connection("")
			, m_session_info()
			, m_session_config()
			, m_session_negotiate()
			, m_groups()
			, m_group_policy("")
			, m_ice_lite("a=ice-lite")
			, m_msid_semantic("")
			, m_msids()
			, m_media_descs()
		{}
	  virtual	~crtc_sdp(){}


	public:
		int32 parse(const std::string sdp_str);


		int32 encode(std::ostringstream& os);
	public:
		std::vector<cmedia_desc*> find_media_descs(const std::string& type);
	public:
		bool is_unified() const;
		// TODO: FIXME: will be fixed when use single pc.
		int32 update_msid(std::string id);
	public:
		void set_ice_ufrag(const std::string& ufrag);
		void set_ice_pwd(const std::string& pwd);
		void set_dtls_role(const std::string& dtls_role);
		void set_fingerprint_algo(const std::string& algo);
		void set_fingerprint(const std::string& fingerprint);
		void add_candidate(const std::string& protocol, const std::string& ip, const int& port, const std::string& type);

		std::string get_ice_ufrag() const;
		std::string get_ice_pwd() const;
		std::string get_dtls_role() const;
		ccandidate get_candidate() const;

	private:
	private:
		int32 _handler_parse_line(const std::string& line);
		int32 _handler_parse_origin(const std::string& content);
		int32 _handler_parse_version(const std::string& content);
		int32 _handler_parse_session_name(const std::string& content);
		int32 _handler_parse_timing(const std::string& content);
		int32 _handler_parse_attribute(const std::string& content);
		int32 _handler_parse_gb28181_ssrc(const std::string& content);
		int32 _handler_parse_media_description(const std::string& content);
		int32 _handler_parse_attr_group(const std::string& content);
	private:
		bool							m_in_media_session;
	public:
		// version
		std::string						m_version;

		// origin
		std::string						m_username;
		std::string						m_session_id;
		std::string						m_session_version;
		std::string						m_nettype;
		std::string						m_addrtype	;
		std::string						m_unicast_address;

		// session_name
		std::string						m_session_name;

		// timing
		int64							m_start_time;
		int64							m_end_time;

		// Connection data, see https://www.ietf.org/rfc/rfc4566.html#section-5.7
		std::string						m_connection;

		csession_info					m_session_info;
		csession_config					m_session_config;
		csession_config					m_session_negotiate;

		std::vector<std::string>		m_groups;
		std::string						m_group_policy;

		std::string						m_ice_lite;
		std::string						m_msid_semantic;
		std::vector<std::string>		m_msids;

		// m-line, media sessions
		std::vector<cmedia_desc>		m_media_descs;
	};

/*
   


   1. 判断客户端没有连接或者断开  主的update检查心跳包
   2.  source_mgr ->   source 
   3.  
 

 视频超分辨率技术
 一、 视频超分算法: 总结了近年来基于深度学习的视频超分方法的特点

 1. MEMC表示运动估计和补偿方法
 2. DC表示可变形卷积方法
 3. 3D Conv表示3D卷积方法
 4. RCNN表示循环卷积神经网络方法





 ### 实不相瞒，macOS和ubuntu下我都安装成功了，Windows没有成功（老报错），感兴趣且运气爆表的同学可以再试试。

安装
1 创建环境
   
conda create -n realbasic python=3.8

2 安装pytorch

 https://pytorch.org/get-started/locally/

 具体版本需根据自己的电脑配置，如有GPU，需与注意自己的cuda版本，可以使用nvidia-smi查看

 conda install pytorch==1.7.1 torchvision==0.8.2 torchaudio==0.7.2 cudatoolkit=10.1 -c pytorch


3 安装mim 和 mmcv-full

MMCV 是一个面向计算机视觉的基础库，它支持了很多开源项目。建议安装完整版:mmcv-full ，包含所有的特性以及丰富的开箱即用的 CUDA 算子。

但是直接pip安装需注意cuda和torch版本，容易安装失败。建议使用MIM安装，它会自动检查 CUDA 和 PyTorch 环境并尽量帮我们安装和环境匹配的预编译版本的 MMCV-full，从而省去编译的耗时。


pip install openmim
mim install mmcv-full

4 安装MMEditing

MMEditing 是基于 PyTorch 的图像&视频编辑开源工具箱, 提供修复/抠图/超分辨率/生成等任务最先进的算法。用pip我试了也失败了几次，最后只能本地安装：

git clone https://github.com/open-mmlab/mmediting.git
cd mmediting
pip install -r requirements.txt
# 如报错，忽略即可，继续执行下指令
pip install -v -e .



使用
1 克隆项目

https://github.com/ckkelvinchan/RealBasicVSR
1.
2 下载预训练模型

下载地址：



在RealBasicVSR项目下新建checkpoints文件夹


3 运行

将需要转换的视频放到data目录下，运行即可

python inference_realbasicvsr.py configs/realbasicvsr_x4.py checkpoints/RealBasicVSR_x4.pth data/demo_001.mp4 results/demo_001.mp4 --fps=12.5


4 结果
运行比较耗时，需耐心等待，如果你有钞能力，当我没说。

 
*/

}
#endif // _C_RTC_SDP_H_