/***********************************************************************************************
created: 		2024-04-06

author:			chensong

purpose:		cplayer
************************************************************************************************/
#include "cmain_wnd.h"
#include "clog.h"
namespace chen {
	ATOM cmain_wnd::m_global_wnd_class = 0;
	static const wchar_t   kClassName[] = L"Rtc_Player";



	/////////////////////////////////////////////////////


	HFONT GetDefaultFont() 
	{
		static HFONT font = reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
		return font;
	}

	void CalculateWindowSizeForText(HWND wnd, const wchar_t* text, size_t* width, size_t* height)
	{
		HDC dc = ::GetDC(wnd);
		RECT text_rc = { 0 };
		::DrawTextW(dc, text, -1, &text_rc, DT_CALCRECT | DT_SINGLELINE);
		::ReleaseDC(wnd, dc);
		RECT client, window;
		::GetClientRect(wnd, &client);
		::GetWindowRect(wnd, &window);

		*width = text_rc.right - text_rc.left;
		*width += (window.right - window.left) - (client.right - client.left);
		*height = text_rc.bottom - text_rc.top;
		*height += (window.bottom - window.top) - (client.bottom - client.top);
	}
	//////////////////////////////////////////////////


	cmain_wnd::cmain_wnd()
	{
	}
	cmain_wnd::~cmain_wnd()
	{
	}
	bool cmain_wnd::init()
	{
		if (!register_window_class())
		{
			WARNING_EX_LOG("register window failed !!!");
			return false;
		}

		m_main_wnd =
			::CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, kClassName, L"Rtc",
				WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), this);
		::SendMessage(m_main_wnd, WM_SETFONT, reinterpret_cast<WPARAM>(GetDefaultFont()), TRUE);
		//::ShowWindow(m_main_wnd, SW_SHOW);
		_create_child_windows();
		if (false)
		{
			struct Windows {
				HWND wnd;
				const wchar_t* text;
				size_t width;
				size_t height;
			} windows[] = {
				{m_label1, L"Server"},  {m_edit1, L"XXXyyyYYYgggXXXyyyYYYggg"},
				{m_label2, L":"},       {m_edit2, L"XyXyX"},
				{m_button, L"Connect"},
			};

			const size_t kSeparator = 5;
			size_t total_width = (ARRAYSIZE(windows) - 1) * kSeparator;

			for (size_t i = 0; i < ARRAYSIZE(windows); ++i) 
			{
				CalculateWindowSizeForText(windows[i].wnd, windows[i].text,
					&windows[i].width, &windows[i].height);
				total_width += windows[i].width;
			}

			RECT rc;
			::GetClientRect(m_main_wnd, &rc);
			size_t x = (rc.right / 2) - (total_width / 2);
			size_t y = rc.bottom * 0.8;
			for (size_t i = 0; i < ARRAYSIZE(windows); ++i) 
			{
				size_t top = y - (windows[i].height / 2);
				::MoveWindow(windows[i].wnd, static_cast<int>(x), static_cast<int>(top),
					static_cast<int>(windows[i].width),
					static_cast<int>(windows[i].height), TRUE);
				x += kSeparator + windows[i].width;
				if (windows[i].text[0] != 'X')
				{
					::SetWindowTextW(windows[i].wnd, windows[i].text);
				}
				::ShowWindow(windows[i].wnd, SW_SHOWNA);
			}
			::SetFocus(m_edit1);
		}
		
		RECT rc;
		::GetClientRect(m_main_wnd, &rc);
		::MoveWindow(m_listbox, 0, 0, rc.right, rc.bottom, TRUE);
		::ShowWindow(m_listbox, SW_SHOWNA);
		return true;
	}
	void cmain_wnd::update()
	{
	}
	void cmain_wnd::destroy()
	{
	}
	LRESULT cmain_wnd::main_wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		cmain_wnd* me = reinterpret_cast<cmain_wnd*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (!me && WM_CREATE == msg)
		{
			CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lp);
			me = reinterpret_cast<cmain_wnd*>(cs->lpCreateParams);
			me->m_main_wnd = hwnd;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(me));
		}

		LRESULT result = 0;
		//if (me) {
		///*	void* prev_nested_msg = me->nested_msg_;
		//	me->nested_msg_ = &msg;*/

		//	bool handled = me->OnMessage(msg, wp, lp, &result);
		//	if (WM_NCDESTROY == msg) {
		//		me->destroyed_ = true;
		//	}
		//	else if (!handled) {
		//		result = ::DefWindowProc(hwnd, msg, wp, lp);
		//	}

		//	if (me->destroyed_ && prev_nested_msg == NULL) {
		//		me->OnDestroyed();
		//		me->wnd_ = NULL;
		//		me->destroyed_ = false;
		//	}

		//	me->nested_msg_ = prev_nested_msg;
		//}
		//else 
		{
			result = ::DefWindowProc(hwnd, msg, wp, lp);
		}

		return result;
	}
	bool cmain_wnd::register_window_class()
	{
		if (m_global_wnd_class)
		{
			return true;
		}
		WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_DBLCLKS;
		wcex.hInstance = GetModuleHandle(NULL);
		wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wcex.lpfnWndProc = &main_wnd_proc;
		wcex.lpszClassName = kClassName;
		m_global_wnd_class = ::RegisterClassExW(&wcex);
		//RTC_DCHECK(wnd_class_ != 0);
		return m_global_wnd_class != 0;
		 
	}
	void cmain_wnd::_create_child_windows()
	{
		// Create the child windows in tab order.
		_create_child_window(&m_label1, 1, L"Static", ES_CENTER | ES_READONLY, 0);
		_create_child_window(&m_edit1, 2, L"Edit", ES_LEFT | ES_NOHIDESEL | WS_TABSTOP, WS_EX_CLIENTEDGE);
		_create_child_window(&m_label2, 3, L"Static", ES_CENTER | ES_READONLY, 0);
		_create_child_window(&m_edit2, 4, L"Edit", ES_LEFT | ES_NOHIDESEL | WS_TABSTOP, WS_EX_CLIENTEDGE);
		_create_child_window(&m_button, 5, L"Button", BS_CENTER | WS_TABSTOP, 0);

		_create_child_window(&m_listbox, 6, L"ListBox", LBS_HASSTRINGS | LBS_NOTIFY, WS_EX_CLIENTEDGE);

		::SetWindowTextA(m_edit1,  "127.0.0.1");
		::SetWindowTextA(m_edit2, "9090");
	}
	void cmain_wnd::_create_child_window(HWND * wnd, int32 id, const wchar_t * class_name, DWORD control_style, DWORD ex_style)
	{
		if (::IsWindow(*wnd))
		{
			WARNING_EX_LOG("not is window !!!");
			return;
		}
		// Child windows are invisible at first, and shown after being resized.
		DWORD style = WS_CHILD | control_style;
		*wnd = ::CreateWindowExW(ex_style, class_name, L"", style, 100, 100, 100, 100,
			m_main_wnd, reinterpret_cast<HMENU>(id),
			GetModuleHandle(NULL), NULL);
		//RTC_DCHECK(::IsWindow(*wnd) != FALSE);
		::SendMessage(*wnd, WM_SETFONT, reinterpret_cast<WPARAM>(GetDefaultFont()), TRUE);

	}
}