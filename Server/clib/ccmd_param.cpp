/********************************************************************
created:	2019-05-13

author:		chensong

purpose:	×Ö·û´®¶ÁÈ¡¹¤¾ß

*********************************************************************/
#include "ccmd_param.h"
#include "clog.h"
#include <string>
namespace chen {



	void ccmd_param::parse_cmd(const std::string & code)
	{
		//std::string::size_type end;
		//int32 index = 0;
		//int32 end = 0;
		//
		//if (code[0] == ' ')
		//{
		//	++index;
		//	//end = code.find_first_of(index, ' ');
		//}
		//for (size_t i = index; i < code.length(); ++i)
		//{
		//	if (code[i] == ' ')
		//	{
		//		end = i;
		//		break;
		//	}
		//}
		//if (end == 0)
		//{
		//	end = code.length();
		//}
		////end = code.find_first_of(index, ' ');
		////ERROR_EX_LOG("index = %d, end = %d", index, end);
		//m_cmd = code.substr(index, end - index);
		////std::string::size_type start = code.find_first_of(' ');
		////std::string temp_str = code.substr(start, code.length());
		////ERROR_EX_LOG("tempstr = %s", temp_str.c_str());
		////std::string::size_type end = temp_str.find_first_of(' ');
		////m_cmd = code.substr(start, temp_str.find_first_of(' ') == std::string::npos ? temp_str.length(): temp_str.find_first_of(' '));
		////ERROR_EX_LOG("m_cmd = %s", m_cmd.c_str());
		////m_cmd = code;
		std::string str;
		for (std::string::const_iterator it = code.begin(); it != code.end(); ++it)
		{
			if (*it == ' ')
			{
				if (str.empty())
				{
					continue;
				}
				if (m_cmd.empty())
				{
					m_cmd = str;
				}
				else
				{
					m_tokens.push_back(str);
				}
				str.clear();
			}
			else
			{
				str.append(1, *it);
			}
		}

		if (!str.empty())
		{
			if (m_cmd.empty())
			{
				m_cmd = str;
			}
			else
			{
				m_tokens.push_back(str);
			}
		}
	}

	void ccmd_param::get_all_params(std::string & cmd) const
	{
		for (std::vector<std::string>::const_iterator iter = m_tokens.begin(); iter != m_tokens.end(); ++iter)
		{
			cmd += *iter + " ";
		}
	}

	void ccmd_param::get_params(std::string & cmd, int start_index) const
	{
	}

	void ccmd_param::get_remain_paras(std::string & cmd) const
	{
	}

} // namespace chen


