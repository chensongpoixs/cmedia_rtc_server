/*

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
*/
#include "cmedia_server.h"
#include <signal.h>
#include "crtc_sdp.h"
#include "csocket_util.h"
#include "cnet_adapter_test.h"
#include "cdtls.h"
#include "cdtls_test.h"
#include "cdtls_certificate.h"
#include "crtc_transport.h"
#include "capi_rtc_publish.h"
#include "crtc_sdp_test.h"
// standard includes
#include <csignal>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <Windows.h>
extern "C" {
 
#ifdef _WIN32
#include <iphlpapi.h>
#endif
}
#include <shellapi.h>
//------------------------------------------------------------------------------
void Stop(int i)
{
	chen::g_media_server.stop();
	std::exit(1);
}

void RegisterSignal()
{
	signal(SIGINT, Stop);
	signal(SIGTERM, Stop);

}

static FILE * out_file_ptr = NULL;
void moudle_test()
{
	chen::net_adapter_test();
	//chen::cdtls::init();

	/*chen::cdtls_certificate cdtls_certificate;
	cdtls_certificate.init();*/
	//chen::g_dtls_certificate.init();



	//chen::test::test_rtc_sdp();
}


void test_cassert()
{
	cassert((5==9));
	cassert_desc((5 == 9), "(5 == 9)");
}



namespace service_ctrl {
	class service_controller {
	public:
		/**
		 * @brief Constructor for service_controller class.
		 * @param service_desired_access SERVICE_* desired access flags.
		 */
		service_controller(DWORD service_desired_access) {
			scm_handle = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_CONNECT);
			if (!scm_handle) {
				auto winerr = GetLastError();
				//BOOST_LOG(error) << "OpenSCManager() failed: "sv << winerr;
				if (out_file_ptr)
				{
					fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
					fflush(out_file_ptr);
				}
				return;
			}

			service_handle = OpenServiceA(scm_handle, "RtcMediaService", service_desired_access);
			if (!service_handle) {
				auto winerr = GetLastError();
				//BOOST_LOG(error) << "OpenService() failed: "sv << winerr;
				if (out_file_ptr)
				{
					fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
					fflush(out_file_ptr);
				}
				return;
			}
		}

		~service_controller() {
			if (service_handle) {
				CloseServiceHandle(service_handle);
			}

			if (scm_handle) {
				CloseServiceHandle(scm_handle);
			}
		}

		/**
		 * @brief Asynchronously starts the Sunshine service.
		 */
		bool
			start_service() {
			if (!service_handle) {
				if (out_file_ptr)
				{
					fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
					fflush(out_file_ptr);
				}
				return false;
			}
			if (out_file_ptr)
			{
				fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
				fflush(out_file_ptr);
			}
			if (!StartServiceA(service_handle, 0, nullptr)) {
				auto winerr = GetLastError();
				if (winerr != ERROR_SERVICE_ALREADY_RUNNING) {
					//BOOST_LOG(error) << "StartService() failed: "sv << winerr;
					if (out_file_ptr)
					{
						fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
						fflush(out_file_ptr);
					}
					return false;
				}
			}
			if (out_file_ptr)
			{
				fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
				fflush(out_file_ptr);
			}
			return true;
		}

		/**
		 * @brief Query the service status.
		 * @param status The SERVICE_STATUS struct to populate.
		 */
		bool
			query_service_status(SERVICE_STATUS &status) {
			if (!service_handle) 
			{
				if (out_file_ptr)
				{
					fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
					fflush(out_file_ptr);
				}
				return false;
			}
			if (out_file_ptr)
			{
				fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
				fflush(out_file_ptr);
			}
			if (!QueryServiceStatus(service_handle, &status)) {
				auto winerr = GetLastError();
				//BOOST_LOG(error) << "QueryServiceStatus() failed: "sv << winerr;
				if (out_file_ptr)
				{
					fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
					fflush(out_file_ptr);
				}
				return false;
			}
			if (out_file_ptr)
			{
				fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
				fflush(out_file_ptr);
			}
			return true;
		}

	private:
		SC_HANDLE scm_handle = NULL;
		SC_HANDLE service_handle = NULL;
	};

	/**
	 * @brief Check if the service is running.
	 *
	 * EXAMPLES:
	 * ```cpp
	 * is_service_running();
	 * ```
	 */
	bool is_service_running() {
		service_controller sc{ SERVICE_QUERY_STATUS };

		SERVICE_STATUS status;
		if (!sc.query_service_status(status)) 
		{
			return false;
		}

		return status.dwCurrentState == SERVICE_RUNNING;
	}

	/**
	 * @brief Start the service and wait for startup to complete.
	 *
	 * EXAMPLES:
	 * ```cpp
	 * start_service();
	 * ```
	 */
	bool
		start_service() {
		service_controller sc{ SERVICE_QUERY_STATUS | SERVICE_START };

		std::cout << "Starting Rtc ...";
		if (out_file_ptr)
		{
			fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
			fflush(out_file_ptr);
		}
		// This operation is asynchronous, so we must wait for it to complete
		if (!sc.start_service()) {
			if (out_file_ptr)
			{
				fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
				fflush(out_file_ptr);
			}
			return false;
		}
		if (out_file_ptr)
		{
			fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
			fflush(out_file_ptr);
		}
		SERVICE_STATUS status;
		do {
			Sleep(1000);
			std::cout << '.';
		} while (sc.query_service_status(status) && status.dwCurrentState == SERVICE_START_PENDING);

		if (status.dwCurrentState != SERVICE_RUNNING) {
		//	BOOST_LOG(error) << SERVICE_NAME " failed to start: "sv << status.dwWin32ExitCode;
			if (out_file_ptr)
			{
				fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
				fflush(out_file_ptr);
			}
			return false;
		}
		if (out_file_ptr)
		{
			fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
			fflush(out_file_ptr);
		}
		std::cout << std::endl;
		return true;
	}

	/**
	 * @brief Wait for the UI to be ready after Sunshine startup.
	 *
	 * EXAMPLES:
	 * ```cpp
	 * wait_for_ui_ready();
	 * ```
	 */
	//bool
	//	wait_for_ui_ready() {
	//	std::cout << "Waiting for Web UI to be ready...";

	//	// Wait up to 30 seconds for the web UI to start
	//	for (int i = 0; i < 30; i++) {
	//		PMIB_TCPTABLE tcp_table = nullptr;
	//		ULONG table_size = 0;
	//		ULONG err;

	//		auto fg = util::fail_guard([&tcp_table]() {
	//			free(tcp_table);
	//		});

	//		do {
	//			// Query all open TCP sockets to look for our web UI port
	//			err = GetTcpTable(tcp_table, &table_size, false);
	//			if (err == ERROR_INSUFFICIENT_BUFFER) {
	//				free(tcp_table);
	//				tcp_table = (PMIB_TCPTABLE)malloc(table_size);
	//			}
	//		} while (err == ERROR_INSUFFICIENT_BUFFER);

	//		if (err != NO_ERROR) {
	//			BOOST_LOG(error) << "Failed to query TCP table: "sv << err;
	//			return false;
	//		}

	//		uint16_t port_nbo = htons(map_port(confighttp::PORT_HTTPS));
	//		for (DWORD i = 0; i < tcp_table->dwNumEntries; i++) {
	//			auto &entry = tcp_table->table[i];

	//			// Look for our port in the listening state
	//			if (entry.dwLocalPort == port_nbo && entry.dwState == MIB_TCP_STATE_LISTEN) {
	//				std::cout << std::endl;
	//				return true;
	//			}
	//		}

	//		Sleep(1000);
	//		std::cout << '.';
	//	}

	//	std::cout << "timed out"sv << std::endl;
	//	return false;
	//}
}  // namespace service_ctrl

//static std::atomic_int desired_exit_code;
LRESULT CALLBACK
SessionMonitorWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_ENDSESSION: {
		// Terminate ourselves with a blocking exit call
		std::cout << "Received WM_ENDSESSION"  << std::endl;
		 
	//	int zero = 0;
		//desired_exit_code.compare_exchange_strong(zero, exit_code);
		//Stop(15);
		std::raise(SIGTERM);
		std::raise(SIGINT);
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		return 0;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}


void check_file(const char * file)
{
	FILE *f = fopen(file, "r");
	if (!f)
	{
		f = fopen(file, "wb+");
		
	}
	fclose(f);
	f = NULL;
}


int doadmin(int argc, char* argv[])
{
	if (!out_file_ptr)
	{
		std::string path = "./log/" + std::to_string(::time(NULL)) + "_rtc.log";
		out_file_ptr = fopen(path.c_str(), "wb+");
	}
	using namespace chen;
	SetProcessShutdownParameters(0x100, SHUTDOWN_NORETRY);
	std::thread window_thread([]() {
		WNDCLASSA wnd_class{};
		wnd_class.lpszClassName = "RtcSessionMonitorClass";
		wnd_class.lpfnWndProc = SessionMonitorWindowProc;
		if (!RegisterClassA(&wnd_class)) {
			std::cout << "Failed to register session monitor window class"  << std::endl;
			if (out_file_ptr)
			{
				fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
				fflush(out_file_ptr);
			}
			return -1;
		}

		auto wnd = CreateWindowExA(
			0,
			wnd_class.lpszClassName,
			"Rtc Session Monitor Window",
			0,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			nullptr,
			nullptr,
			nullptr,
			nullptr);
		if (!wnd) {
			std::cout << "Failed to create session monitor window"  << std::endl;
			WARNING_EX_LOG("Failed to create session monitor window");
			if (out_file_ptr)
			{
				fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
				fflush(out_file_ptr);
			}
			return -1;
		}

		ShowWindow(wnd, SW_HIDE);

		// Run the message loop for our window
		MSG msg{};
		while (GetMessage(&msg, nullptr, 0, 0) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	});
	window_thread.detach();
	if (out_file_ptr)
	{
		fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
		fflush(out_file_ptr);
	}
	bool shortcut_launch = false;
	bool service_admin_launch = false;

	if (argc == 1  && !service_ctrl::is_service_running())
	{
		/*if (service_ctrl::is_service_running())
		{
			fprintf(out_file_ptr, "[%s][%d][argc = %u]", __FUNCTION__, __LINE__, argc);
			fflush(out_file_ptr);
			return 1;
		}*/
		// cur check 
		// -admin
		WCHAR executable[MAX_PATH];
		GetModuleFileNameW(NULL, executable, ARRAYSIZE(executable));
		if (out_file_ptr)
		{
			fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
			fflush(out_file_ptr);
		}
		SHELLEXECUTEINFOW shell_exec_info{};
		shell_exec_info.cbSize = sizeof(shell_exec_info);
		shell_exec_info.fMask = SEE_MASK_NOASYNC | SEE_MASK_NO_CONSOLE | SEE_MASK_NOCLOSEPROCESS;
		shell_exec_info.lpVerb = L"runas";
		shell_exec_info.lpFile = executable;
		shell_exec_info.lpParameters = L"--admin";
		shell_exec_info.nShow = SW_NORMAL;
		if (!ShellExecuteExW(&shell_exec_info)) {
			auto winerr = GetLastError();
			std::cout << "Error: ShellExecuteEx() failed:"  << winerr << std::endl;
			WARNING_EX_LOG("Error: ShellExecuteEx() failed:");
			if (out_file_ptr)
			{
				fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
				fflush(out_file_ptr);
			}
			return 1;
		}
		NORMAL_EX_LOG("  ShellExecuteEx() failed:");
		if (out_file_ptr)
		{
			fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
			fflush(out_file_ptr);
		}
		// Wait for the elevated process to finish starting the service
		WaitForSingleObject(shell_exec_info.hProcess, INFINITE);
		CloseHandle(shell_exec_info.hProcess);

		/// 启动进程中信息退出
		return 0;
	}
	else if (argc == 2 && std::string(argv[1]) == "--admin")
	{
		NORMAL_EX_LOG("---");
		if (out_file_ptr)
		{
			fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
			fflush(out_file_ptr);
		}
		service_ctrl::start_service();
	}
	else
	{
		WARNING_EX_LOG("[warr] argc = %u\n", argc);
		if (out_file_ptr)
		{
			fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
			fflush(out_file_ptr);
		}
		/*printf("[warr] argc = %u\n", argc);*/
	}
	return 1;
}
 
int main(int argc, char* argv[])
{
	
	printf("LOG init ...");
	if (!LOG::init("./log", "media_rtc_server"))
	{
		return false;
	}
	 
	using namespace chen;
	if (doadmin(argc, argv) != 0)
	{
		NORMAL_EX_LOG("---");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		LOG::destroy();
		return 0;
	}
	 
		RegisterSignal();
		const char* config_filename = "media_server.cfg";
		check_file(config_filename);
		NORMAL_EX_LOG("---");
		const char* log_path = "./log";
	/*	if (argc > 2)
		{
			log_path = argv[2];
		}*/
		NORMAL_EX_LOG("---");
		if (out_file_ptr)
		{
			fprintf(out_file_ptr, "[%s][%d] ", __FUNCTION__, __LINE__);
			fflush(out_file_ptr);
		}
		if (out_file_ptr)
		{
			fclose(out_file_ptr);
			out_file_ptr = NULL;
		}
		bool init = chen::g_media_server.init(log_path, config_filename);
		NORMAL_EX_LOG("---");
		//moudle_test();
		if (init)
		{
			NORMAL_EX_LOG("---");
			init = chen::g_media_server.Loop();
		}
		NORMAL_EX_LOG("---");

		chen::g_media_server.destroy();
		if (!init)
		{
			return 1;
		}
	 
	
	 
	return 0;

	
}
