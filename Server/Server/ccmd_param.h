/********************************************************************
created:	2019-05-13

author:		chensong

purpose:	×Ö·û´®¶ÁÈ¡¹¤¾ß

*********************************************************************/
#ifndef _C_CMD_PARAM_H_
#define _C_CMD_PARAM_H_

#include <boost/lexical_cast.hpp>
#include "cnoncopyable.h"
#include <string>
#include <vector>
#include "cnet_type.h"

namespace chen {

	class ccmd_param :private cnoncopyable
	{
	public:
		ccmd_param() : m_index(-1) {}
		~ccmd_param() {}

		void parse_cmd(const std::string& code);

		void set_cmd(const std::string& cmd) { m_cmd = cmd; }

		const std::string& get_cmd() const { return m_cmd; }

		void push_param(const std::string& tokens) { m_tokens.push_back(tokens); }

		int get_param_num() const { return static_cast<int>(m_tokens.size()); }

		void get_all_params(std::string& cmd) const;

		void get_params(std::string& cmd, int start_index) const;

		void get_remain_paras(std::string& cmd) const;

		template<typename T>
		bool get_param(int index, T& param) const
		{
			try
			{
				if (index >= get_param_num())
				{
					return false;
				}

				param = boost::lexical_cast<T>(m_tokens[index]);
				return true;
			}
			catch (boost::bad_lexical_cast&)
			{
				return false;
			}
			return true;
		}

		template<typename T>
		bool get_next_param(T& param) const
		{
			++m_index;
			return get_param(m_index, param);
		}
		const std::string & get_param(int32 index) const { return  m_tokens[index]; }

	private:
		std::string				m_cmd;
		std::vector<std::string> m_tokens;
		mutable int m_index;
	};

} // namespace chen

#endif // _C_CMD_PARAM_H_
