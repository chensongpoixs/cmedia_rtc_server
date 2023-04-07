/***********************************************************************************************
created: 		2024-04-06

author:			chensong

purpose:		cplayer
************************************************************************************************/
#ifndef _C_MAIN_WND_H_
#define _C_MAIN_WND_H_

#include "cnet_types.h"
#include <map>
#include <memory>
#include <string>

#include <Windows.h>
namespace chen
{
	class cmain_wnd
	{ 
	public:
		 explicit  cmain_wnd();
		~cmain_wnd();
	public:
		bool init();
		void update();

		void destroy();

	public:
		static LRESULT CALLBACK main_wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
		static bool register_window_class();

	private:
		void _create_child_windows();
		void _create_child_window(HWND * wnd, int32 id, const wchar_t* class_name, DWORD control_style, DWORD ex_style);
	protected:
	private:
		static ATOM     m_global_wnd_class;
		HWND			m_main_wnd;

		HWND			m_edit1;
		HWND			m_edit2;
		HWND			m_label1;
		HWND			m_label2;
		HWND			m_button;
		HWND			m_listbox;
		
	};
}


#endif // _C_MAIN_WND_H_