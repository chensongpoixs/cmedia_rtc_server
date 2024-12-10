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
#include "cweb_http_api_proxy.h"
#include "cwindow_util.h"
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


#include <boost/process.hpp>
#include <boost/filesystem.hpp>
#include <Windows.h>

#define MY_PIPE_BUFFER_SIZE 1024

void StartProcess(/*const char * program, LPCWSTR args*/)
{
	//初始化管道
	HANDLE hPipeRead;
	HANDLE hPipeWrite;
	SECURITY_ATTRIBUTES saOutPipe;
	::ZeroMemory(&saOutPipe, sizeof(saOutPipe));
	saOutPipe.nLength = sizeof(SECURITY_ATTRIBUTES);
	saOutPipe.lpSecurityDescriptor = NULL;
	saOutPipe.bInheritHandle = TRUE;
	if (CreatePipe(&hPipeRead, &hPipeWrite, &saOutPipe, MY_PIPE_BUFFER_SIZE))
	{
		PROCESS_INFORMATION processInfo;
		::ZeroMemory(&processInfo, sizeof(processInfo));
		STARTUPINFO startupInfo;
		::ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(STARTUPINFO);
		startupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		startupInfo.hStdOutput = hPipeWrite;
		startupInfo.hStdError = hPipeWrite;
		startupInfo.wShowWindow = SW_HIDE;
		//L"nvidia-smi", L" -i 0 -q -d UTILIZATION"
		if (::CreateProcess("nvidia-smi.exe", (LPSTR)" -i 0 -q -d UTILIZATION",
			NULL,  // process security
			NULL,  // thread security
			TRUE,  //inheritance
			0,     //no startup flags
			NULL,  // no special environment
			NULL,  //default startup directory
			&startupInfo,
			&processInfo))
		{
			printf("[%s][%d]\n", __FUNCTION__, __LINE__);
			if (WAIT_TIMEOUT != WaitForSingleObject(processInfo.hProcess, 3000))
			{
				printf("[%s][%d]\n", __FUNCTION__, __LINE__);
				DWORD dwReadLen = 0;
				DWORD dwStdLen = 0;
				if (PeekNamedPipe(hPipeRead, NULL, 0, NULL, &dwReadLen, NULL) && dwReadLen > 0)
				{
					printf("[%s][%d]\n", __FUNCTION__, __LINE__);
					char szPipeOut[MY_PIPE_BUFFER_SIZE] = { 0 };
					::ZeroMemory(szPipeOut, sizeof(szPipeOut));
					if (ReadFile(hPipeRead, szPipeOut, dwReadLen, &dwStdLen, NULL))
					{
						printf("[szPipeOut = %s][]\n", szPipeOut);
						// 输出值
						int k = 0;
					}
					int a = 1;
				}
			}
		}
		if (processInfo.hProcess)
		{
			printf("[%s][%d]\n", __FUNCTION__, __LINE__);
			CloseHandle(processInfo.hProcess);
		}
		if (processInfo.hThread)
		{
			printf("[%s][%d]\n", __FUNCTION__, __LINE__);
			CloseHandle(processInfo.hThread);
		}
		printf("[%s][%d]\n", __FUNCTION__, __LINE__);
	}
	printf("[%s][%d]\n", __FUNCTION__, __LINE__);
	CloseHandle(hPipeRead);
	CloseHandle(hPipeWrite);
}

void test_process()
{
	//StartProcess();
	boost::process::environment my_env = boost::this_process::environment();
	//boost::process::ipstream p;
	//
	//boost::process::child c(
	//	std::string("nvidia-smi -i 0 -q -d UTILIZATION"), my_env,
	//	boost::process::std_out > stdout, //forward
	//	boost::process::std_err.close(), //close
	//	boost::process::std_in < boost::process::null //null in
	//);
	//
	//std::string s;
	//std::getline(p, s);

	//c.wait();



	//
	//boost::process::system(
	//	"nvidia-smi -i 0 -q -d UTILIZATION",
	//	boost::process::std_out > stdout, //forward
	//	boost::process::std_err.close(), //close
	//	boost::process::std_in < boost::process::null //null in
	//);
	//nvidia-smi  --list-gpus
	  boost::filesystem::path p = "input.txt";
	 //std::ostringstream cmd;
	  std::string outstr;
	  boost::process::ipstream p2;

	 std::thread reader([  &p2] {
		 std::string line;
		 while (std::getline(p2, line))
			  std::cout << "Received: '" << line << "'" << std::endl;
			// cmd << line;
			// outstr += line;
		 });

	boost::process::child c(
		"nvidia-smi -i 0 -q -d UTILIZATION",
		(boost::process::std_out & boost::process::std_err) > p2, //redirect both to one file
		boost::process::std_in < p //read input from file
	);
	
	c.wait();
	if (reader.joinable())
	{
		reader.join();
	}
	printf("[cmd = %s]\n", outstr .c_str());

	//cmd << std::string(p2.begin(), p2.end());
	//std::cout << p2 << std::endl;
	//printf("p2 = %s\n", p2);
	//{
	//	boost::process::opstream p1;
	//	boost::process::ipstream p2;
	//	boost::process::system(
	//		"nvidia-smi -i 0 -q -d UTILIZATION",
	//		boost::process::std_out > p2,
	//		boost::process::std_in < p1
	//	);
	//	p1 << "my_text";
	//	int i = 0;
	//	p2 >> i;

	//}
	/*{
		std::vector<char> in_buf;
		std::string value = "my_string";
		boost::asio::io_context io_context;
		boost::process::async_pipe p1(io_context);
		boost::process::async_pipe p2(io_context);
		boost::process::system(
			"nvidia-smi -i 0 -q -d UTILIZATION",
			boost::process::std_out > p2,
			boost::process::std_in < p1,
			io_context,
			boost::process::on_exit([&](int exit, const std::error_code& ec_in)
				{
					printf("[out = %s]\n", std::string(in_buf.begin(), in_buf.end()).c_str());
					p1.async_close();
					p2.async_close();
				})
		);
		
		boost::asio::async_write(p1, boost::asio::buffer(value), [](const boost::system::error_code&, std::size_t) {});
		boost::asio::async_read(p2, boost::asio::buffer(in_buf), [](const boost::system::error_code&, std::size_t) {});
	}*/
	//{
	//	boost::asio::io_context io_context;
	//	std::vector<char> in_buf;
	//	std::string value = "my_string";
	//	boost::process::system(
	//		"nvidia-smi -i 0 -q -d UTILIZATION",
	//		boost::process::std_out > boost::process::buffer(in_buf),
	//		boost::process::std_in < boost::process::buffer(value)
	//	);
	//
	//	printf("in_buf = %s\n", std::string(in_buf.begin(), in_buf.end()).c_str());
	//}

	//{
	//	boost::asio::io_context io_context;
	//	std::future<std::vector<char>> in_buf;
	//	std::future<void> write_fut;
	//	std::string value = "my_string";
	//	boost::process::system(
	//		"nvidia-smi -i 0 -q -d UTILIZATION",
	//		boost::process::std_out > in_buf,
	//		boost::process::std_in  < boost::process::buffer(value) > write_fut
	//	);
	//
	//	write_fut.get();
	//	in_buf.get();
	//
	//	//std::cout << in_buf.get() << std::endl;
	//}



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
#if 0
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
int test_http_main();
#endif // #if 0














/*
 *          Copyright Andrey Semashev 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
 /*!
  * \file   main.cpp
  * \author Andrey Semashev
  * \date   30.08.2009
  *
  * \brief  An example of asynchronous logging with bounded log record queue in multiple threads.
  */

  // #define BOOST_LOG_DYN_LINK 1

#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
#include <functional>
#include <boost/core/ref.hpp>
#include <boost/bind/bind.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/barrier.hpp>

#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/record_ordering.hpp>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

using boost::shared_ptr;

enum
{
	LOG_RECORDS_TO_WRITE = 10000,
	THREAD_COUNT = 2
};

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(test_lg, src::logger_mt)



#include <Windows.h>
#include <tchar.h>
#include <ShlObj_core.h>
#include <Shlwapi.h>
#pragma comment( lib, "ShLwApi.Lib" ) //PathRemoveFileSpecA
#define ARRAY_SIZE (2048)



//! This function is executed in multiple threads
void thread_fun(boost::barrier& bar)
{
	// Wait until all threads are created
	bar.wait();

	// Here we go. First, identify the thread.
	BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());

	// Now, do some logging
	for (unsigned int i = 0; i < LOG_RECORDS_TO_WRITE; ++i)
	{
		BOOST_LOG(test_lg::get()) << "Log record " << i;
	}
}
bool create_virtual_desktop(const std::string& desktop_name, PROCESS_INFORMATION& pinfo)
{
	if (desktop_name.size() < 1)
	{
		WARNING_EX_LOG("create desktop  [desktop_name= %s] failed !!!", desktop_name.c_str());
		return false;
	}
	pinfo = { 0 };
	//GetCurrentThreadId();
	HDESK hDesk = OpenDesktop(desktop_name.c_str(), DF_ALLOWOTHERACCOUNTHOOK, TRUE, GENERIC_ALL);

	if (!hDesk)
	{
		SECURITY_ATTRIBUTES sAttribute = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
		hDesk = ::CreateDesktop((desktop_name.c_str()), NULL, NULL, DF_ALLOWOTHERACCOUNTHOOK, GENERIC_ALL, &sAttribute);
		if (!hDesk)
		{
			ERROR_EX_LOG("create desktop failed !!!");
			return false;
		}

		//TODO@chensong 20220723   virtual desktop  mouse keydown 
		TCHAR szExplorerFile[ARRAY_SIZE] = { 0 };

		GetWindowsDirectory(szExplorerFile, ARRAY_SIZE - 1);
		_tcscat_s(szExplorerFile, ARRAY_SIZE - 1, _T("\\Explorer.Exe"));


		TCHAR szDirectoryName[ARRAY_SIZE] = { 0 };
		//TCHAR szExplorerFile[ARRAY_SIZE] = { 0 };

		_tcscpy_s(szDirectoryName, _tcslen(szExplorerFile) + 1, szExplorerFile);

		//PathIsExe(szExplorerFile);


		PathRemoveFileSpec(szDirectoryName);

		STARTUPINFO sInfo = { 0 };
		//PROCESS_INFORMATION pInfo = { 0 };

		sInfo.cb = sizeof(sInfo);
		sInfo.lpDesktop = (LPSTR)desktop_name.c_str();;

		//Lanuching a application into dekstop
		BOOL bCreateProcessReturn = CreateProcess(szExplorerFile,
			NULL,
			NULL,
			NULL,
			TRUE,
			NORMAL_PRIORITY_CLASS,
			NULL,
			szDirectoryName,
			&sInfo,
			&pinfo);
		if (bCreateProcessReturn)
		{
			CloseHandle(pinfo.hThread);
			pinfo.hThread = NULL;
			CloseHandle(pinfo.hProcess);
			pinfo.hProcess = NULL;
		}
		/*TCHAR *pszError = NULL;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0, (LPWSTR)&pszError, 0, NULL);
		OutputDebugString(_T("\n\t\t"));
		OutputDebugString(pszError);*/
	}
	// TODO@chensong 2022-12-26  清理 virtual desktop 资源
	if (hDesk)
	{
		CloseDesktop(hDesk);
		hDesk = NULL;
	}

	
	return true;
}
#include <boost/process.hpp>
#include <boost/process/windows.hpp>
#include <iostream>

#include <windows.h>
void _test_boost_process()
{

	std::string app_path_param = "D:\\Work\\Tools\\yuvplayer.exe";
	boost::process::system(app_path_param, boost::process::windows::hide);
}
void start_rte()
{
	PROCESS_INFORMATION  virtual_app;
	PROCESS_INFORMATION app_info;
	TCHAR szAppName[ARRAY_SIZE] = { 0 };
	//Get the application full path, so that it can be launch into the switching desktop.
	GetModuleFileName(GetModuleHandle(NULL), szAppName, ARRAY_SIZE - 1);
	ACCESS_MASK desired_access = DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
		DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
		DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
		DESKTOP_SWITCHDESKTOP | GENERIC_WRITE;
	std::string new_desktop = std::to_string(::time(NULL));
	create_virtual_desktop(new_desktop, virtual_app);
	STARTUPINFO si = { sizeof(si) };
	//si.lpDesktop = (LPSTR)new_desktop.c_str();
	si.wShowWindow = SW_HIDE;
	std::string app_path_param = "D:\\Work\\Tools\\yuvplayer.exe";
	std::string app_work_path = "D:\\Work\\UE\\builder_V4.5_Release_33_202308280914\\builderexe";
	bool ret = ::CreateProcess(NULL, LPSTR(app_path_param.c_str()), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, LPSTR(app_work_path.c_str()), &si, &app_info);
	if (ret)
	{
		// TODO@chensong 2022-07-27  创建线程关闭  -----后期优化 
		CloseHandle(app_info.hThread);
		app_info.hThread = NULL;
		CloseHandle(app_info.hProcess);
		app_info.hProcess = NULL;

		//process_info.hProcess = virtual_desktop.dwProcessId;
		//process_info.hThread = virtual_desktop.dwThreadId;

	}
	else
	{
		//close_app_project(process_info.virtual_app);
	}

}




void find_Rte_main_window()
{
	long long pid = 50028;
	HWND f = chen::FindMainWindow(pid);

}
BOOL CALLBACK EnumDesktopsProc(LPTSTR lpszDesktop, LPARAM lParam) {
	// 在这里处理桌面的名字，例如输出到控制台
	std::wcout << L"Desktop Name: " << lpszDesktop << std::endl;
	return TRUE;  // 返回 TRUE 继续枚举桌面，返回 FALSE 停止枚举
}


void get_destops()
{
	HDESK hDesk = GetThreadDesktop(GetCurrentThreadId());

	// 调用 EnumDesktops 函数，传递回调函数 EnumDesktopsProc
	EnumDesktops((HWINSTA)hDesk, EnumDesktopsProc, 0);
}
#include <openssl/sha.h>
#include <openssl/hmac.h>
void hmac_sha512(const unsigned char* key, int key_len, const unsigned char* data, int data_len, unsigned char* result) {
	HMAC_CTX* ctx = HMAC_CTX_new();

	HMAC_Init_ex(ctx, key, key_len, EVP_sha512(), NULL);
	HMAC_Update(ctx, data, data_len);
	HMAC_Final(ctx, result, NULL);

	HMAC_CTX_free(ctx);
}


void rte_token()
{

	// 示例密钥和消息
	const char* key = "syz.20120328";
	const char* data = "18";

	// 将密钥和消息转换为字节数组
	unsigned char key_bytes[SHA512_CBLOCK];
	unsigned char data_bytes[SHA512_CBLOCK];
	unsigned char result[SHA512_DIGEST_LENGTH];

	strncpy((char*)key_bytes, key, SHA512_CBLOCK);
	strncpy((char*)data_bytes, data, SHA512_CBLOCK);

	// 计算HMAC-SHA512
	hmac_sha512(key_bytes, strlen((char*)key_bytes), data_bytes, strlen((char*)data_bytes), result);
	printf("result = %s\n", result);
	// 输出结果
	printf("HMAC-SHA512: ");
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) 
	{
		printf("%02x", result[i]);
	}
	printf("\n");
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

// 使用 HMAC SHA-256 签名 JWT token
char* create_jwt_token(const char* key, const char* payload) {
	// 构建头部
	const char* header = "{\"alg\":\"HS512\", \"typ\": \"JWT\" }";

	// 计算 Base64 编码后头部的长度
	int header_len = EVP_ENCODE_LENGTH(strlen(header));

	// 计算 Base64 编码后负载的长度
	int payload_len = EVP_ENCODE_LENGTH(strlen(payload));

	// 分配内存存储 Base64 编码后的头部和负载
	char* encoded_header = (char*)malloc(header_len);
	char* encoded_payload = (char*)malloc(payload_len);

	// Base64 编码头部和负载
	EVP_EncodeBlock((unsigned char*)encoded_header, (const unsigned char*)header, strlen(header));
	EVP_EncodeBlock((unsigned char*)encoded_payload, (const unsigned char*)payload, strlen(payload));

	// 构建待签名字符串
	int unsigned_data_len = strlen(encoded_header) + strlen(encoded_payload) + 2; // 加上两个点号
	char* unsigned_data = (char*)malloc(unsigned_data_len);
	snprintf(unsigned_data, unsigned_data_len, "%s.%s", encoded_header, encoded_payload);

	// 使用 HMAC SHA-256 对待签名字符串进行签名
	unsigned char hmac_result[EVP_MAX_MD_SIZE];
	unsigned int hmac_len;

	HMAC(EVP_sha512(), key, strlen(key), (const unsigned char*)unsigned_data, strlen(unsigned_data), hmac_result, &hmac_len);

	// Base64 编码签名结果
	int signature_len = EVP_ENCODE_LENGTH(hmac_len);
	char* encoded_signature = (char*)malloc(signature_len);
	EVP_EncodeBlock((unsigned char*)encoded_signature, hmac_result, hmac_len);

	// 构建 JWT token
	int token_len = strlen(encoded_header) + strlen(encoded_payload) + strlen(encoded_signature) + 3; // 加上两个点号和结尾的 null 字符
	char* jwt_token = (char*)malloc(token_len);
	snprintf(jwt_token, token_len, "%s.%s.%s", encoded_header, encoded_payload, encoded_signature);

	// 释放分配的内存
	free(encoded_header);
	free(encoded_payload);
	free(unsigned_data);
	free(encoded_signature);

	return jwt_token;
}




//java 从最后 删除'='个数  url-unfriendly base64 char u
int remove_padding(  char *& data, size_t len)
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

// 使用 HMAC SHA-512 签名 JWT token
char* create_jwt_512_token(const char* key, const char* payload) {
	// 构建头部
	const char* header = "{\"alg\":\"HS512\"}";

	// 计算 Base64 编码后头部的长度
	int header_len = EVP_ENCODE_LENGTH(strlen(header));

	// 计算 Base64 编码后负载的长度
	int payload_len = 54;// = EVP_ENCODE_LENGTH(strlen(payload));

	// 分配内存存储 Base64 编码后的头部和负载
	char* encoded_header = (char*)malloc(header_len);
	char* encoded_payload = (char*)malloc(payload_len);

	// Base64 编码头部和负载
	EVP_EncodeBlock((unsigned char*)encoded_header, (const unsigned char*)header, strlen(header));
	EVP_EncodeBlock((unsigned char*)encoded_payload, (const unsigned char*)payload, strlen(payload));


	// java 从最后 删除'='个数
	{
	
		/*int paddingCount = 0;
		int encoded_payload_len = strlen(encoded_payload);
		for (int i = encoded_payload_len - 1; i > 0; --i)
		{
			if (encoded_payload[i] == '=')
			{
				++paddingCount;
			}
			else
			{
				break;
			}
		}*/
	
	}


	// 构建待签名字符串
	int unsigned_data_len = remove_padding(encoded_header, strlen(encoded_header)) + remove_padding(encoded_payload, strlen(encoded_payload)) + 2 ; // 加上两个点号
	char* unsigned_data = (char*)malloc(unsigned_data_len);
	std::string  header_payload(encoded_header, remove_padding(encoded_header, strlen(encoded_header)));
	header_payload += ".";
	header_payload += std::string(encoded_payload, remove_padding(encoded_payload, strlen(encoded_payload)));
	snprintf(unsigned_data, unsigned_data_len, "%s", header_payload.c_str());

	// 使用 HMAC SHA-512 对待签名字符串进行签名
	signed char hmac_result[EVP_MAX_MD_SIZE];// = { 0 };
	unsigned int hmac_len;// = 0;

	HMAC(EVP_sha512(), key, strlen(key), (const unsigned char*)unsigned_data, strlen(unsigned_data),(unsigned char*) hmac_result, &hmac_len);

	// Base64 编码签名结果
	int signature_len = EVP_ENCODE_LENGTH(hmac_len);
	char* encoded_signature = (char*)malloc(signature_len);
	EVP_EncodeBlock((unsigned char*)encoded_signature,(unsigned char*) hmac_result, hmac_len);

	// 构建 JWT token
	int token_len = remove_padding(encoded_header, strlen(encoded_header)) + remove_padding(encoded_payload, strlen(encoded_payload)) + remove_padding(encoded_signature,  strlen(encoded_signature)) + 3; // 加上两个点号和结尾的 null 字符
	char* jwt_token = (char*)malloc(token_len);
	snprintf(jwt_token, token_len, "%s.%s", header_payload.c_str(), encoded_signature);

	// 释放分配的内存
	free(encoded_header);
	free(encoded_payload);
	free(unsigned_data);
	free(encoded_signature);

	return jwt_token;
}


int rte_token_main() {
	const char* secret_key = "syz.20120328";
	/*
	
	{ "syz.20120328": "{\"userId\":18}","sub": "syz.20120328"}
	*/
	Json::Value userid;
	userid["userId"] = 18;
	Json::Value value;
	
	value["sub"] = "syz.20120328";
	value["syz.20120328"] = "{\"userId\":18}";
	Json::StyledWriter swriter;
	swriter.write(value);
	std::string str = swriter.write(value);

	 std::string payload = "{ \
		\"syz.20120328\": \"{\"userId\":18}\",  \
		\"sub\": \"syz.20120328\" \
     }";
	 std::string fff = "{\"syz.20120328\":\"{\\\"userId\\\":18}\",\"sub\":\"syz.20120328\"}";
	printf("payload = %s\n", fff.c_str() );
	/*const char* payload = "{
		"syz.20120328": "{\"userId\":18}",
		"sub" : "syz.20120328"
}";*/

	char* jwt_token = create_jwt_512_token(secret_key, fff.c_str());

	printf("JWT Token: %s\n", jwt_token);

	// 释放分配的内存
	free(jwt_token);

	return 0;
}
#include "cjwt_create_token.h"
int fffjwt_test_main(int argc, char* argv[])
{
	/*boost::process::environment my_env = boost::this_process::environment();

	my_env["PATH"] += "/foo";
	return 0;
	_test_boost_process();
	return 0;*/
	//get_destops();
	//find_Rte_main_window();
	//start_rte();
	//rte_token();
	//rte_token_main();

	chen::jwt::test_create_token();

	return 0;
	test_process();
	return 0;
	try
	{
		// Open a rotating text file
		shared_ptr< std::ostream > strm(new std::ofstream("test.log"));
		if (!strm->good())
			throw std::runtime_error("Failed to open a text log file");

		// Create a text file sink
		typedef sinks::text_ostream_backend backend_t;
		typedef sinks::asynchronous_sink<
			backend_t,
			sinks::bounded_ordering_queue<
			logging::attribute_value_ordering< unsigned int, std::less< unsigned int > >,
			128,                        // queue no more than 128 log records
			sinks::block_on_overflow    // wait until records are processed
			>
		> sink_t;
		shared_ptr< sink_t > sink(new sink_t(
			boost::make_shared< backend_t >(),
			// We'll apply record ordering to ensure that records from different threads go sequentially in the file
			keywords::order = logging::make_attr_ordering< unsigned int >("RecordID", std::less< unsigned int >())));

		sink->locked_backend()->add_stream(strm);

		sink->set_formatter
		(
			expr::format("%1%: [%2%] [%3%] - %4%")
			% expr::attr< unsigned int >("RecordID")
			% expr::attr< boost::posix_time::ptime >("TimeStamp")
			% expr::attr< boost::thread::id >("ThreadID")
			% expr::smessage
		);

		// Add it to the core
		logging::core::get()->add_sink(sink);

		// Add some attributes too
		logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
		logging::core::get()->add_global_attribute("RecordID", attrs::counter< unsigned int >());

		// Create logging threads
		boost::barrier bar(THREAD_COUNT);
		boost::thread_group threads;
		for (unsigned int i = 0; i < THREAD_COUNT; ++i)
			threads.create_thread(boost::bind(&thread_fun, boost::ref(bar)));

		// Wait until all action ends
		threads.join_all();

		// Flush all buffered records
		sink->stop();
		sink->flush();

		return 0;
	}
	catch (std::exception& e)
	{
		std::cout << "FAILURE: " << e.what() << std::endl;
		return 1;
	}
}






int  main(int argc, char* argv[])
{

	//chen::jwt::test_siample_create_token();
	//return 0;
	RegisterSignal();
	const char* config_filename = "media_server.cfg";
	const char* log_path = "./log";
	if (argc > 1)
	{
		config_filename = argv[1];
	}
	if (argc > 2)
	{
		log_path = argv[2];
	}

	bool init = chen::g_media_server.init(log_path, config_filename);
		
	if (init)
	{
		init = chen::g_media_server.Loop();
	}
	chen::g_media_server.destroy();
	if (!init)
	{
		return 1;
	}
	 
	
	 
	return 0;

	
}



//#include "client_http.hpp"
#include "server_http.hpp"
#include <future>

#if 0
// Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// Added for the default_resource example
#include <algorithm>
#include <boost/filesystem.hpp>
#endif // if 0


#include <fstream>
#include <vector>
#ifdef HAVE_OPENSSL
#include "crypto.hpp"
#endif

//using namespace std;
//// Added for the json-example:
//using namespace boost::property_tree;
//
//
////using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;
//
//int test_http_main() {
//	// HTTP-server at port 8080 using 1 thread
//	// Unless you do more heavy non-threaded processing in the resources,
//	// 1 thread is usually faster than several threads
//	SimpleWeb::Server<SimpleWeb::HTTP> server;
//	server.config.port = 8089;
//
//	// Add resources using path-regex and method-string, and an anonymous function
//	// POST-example for the path /string, responds the posted string
//	server.resource["^/string$"]["POST"] = [](shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request) {
//		// Retrieve string:
//		g_web_http_api_proxy.get_all_room();
//		auto content = request->content.string();
//		// request->content.string() is a convenience function for:
//		// stringstream ss;
//		// ss << request->content.rdbuf();
//		// auto content=ss.str();
//
//		*response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n"
//			<< content;
//
//
//		// Alternatively, use one of the convenience functions, for instance:
//		// response->write(content);
//	};
//
//	// POST-example for the path /json, responds firstName+" "+lastName from the posted json
//	// Responds with an appropriate error message if the posted json is not valid, or if firstName or lastName is missing
//	// Example posted json:
//	// {
//	//   "firstName": "John",
//	//   "lastName": "Smith",
//	//   "age": 25
//	// }
//	server.resource["^/json$"]["POST"] = [](shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request) {
//		try {
//			ptree pt;
//			read_json(request->content, pt);
//
//			auto name = pt.get<string>("firstName") + " " + pt.get<string>("lastName");
//
//			*response << "HTTP/1.1 200 OK\r\n"
//				<< "Content-Length: " << name.length() << "\r\n\r\n"
//				<< name;
//		}
//		catch (const exception &e) {
//			*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n"
//				<< e.what();
//		}
//
//
//		// Alternatively, using a convenience function:
//		// try {
//		//     ptree pt;
//		//     read_json(request->content, pt);
//
//		//     auto name=pt.get<string>("firstName")+" "+pt.get<string>("lastName");
//		//     response->write(name);
//		// }
//		// catch(const exception &e) {
//		//     response->write(SimpleWeb::StatusCode::client_error_bad_request, e.what());
//		// }
//	};
//
//	// GET-example for the path /info
//	// Responds with request-information
//	server.resource["^/info$"]["GET"] = [](shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request) {
//		stringstream stream;
//		stream << "<h1>Request from " << request->remote_endpoint().address().to_string() << ":" << request->remote_endpoint().port() << "</h1>";
//
//		stream << request->method << " " << request->path << " HTTP/" << request->http_version;
//
//		stream << "<h2>Query Fields</h2>";
//		auto query_fields = request->parse_query_string();
//		for (auto &field : query_fields)
//			stream << field.first << ": " << field.second << "<br>";
//
//		stream << "<h2>Header Fields</h2>";
//		for (auto &field : request->header)
//			stream << field.first << ": " << field.second << "<br>";
//
//		response->write(stream);
//	};
//
//	// GET-example for the path /match/[number], responds with the matched string in path (number)
//	// For instance a request GET /match/123 will receive: 123
//	server.resource["^/match/([a-zA-Z0-9_]+)$"]["GET"] = [](shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request) 
//	{
//		std::cout << "http thread id  :" << std::this_thread::get_id() << std::endl;
//		g_web_http_api_proxy.get_all_room();
//		response->write(request->path_match[1].str());
//	};
//
//	// GET-example simulating heavy work in a separate thread
//	server.resource["^/work$"]["GET"] = [](shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> /*request*/) {
//		thread work_thread([response] {
//			this_thread::sleep_for(chrono::seconds(5));
//			response->write("Work done");
//		});
//		work_thread.detach();
//	};
//
//	// Default GET-example. If no other matches, this anonymous function will be called.
//	// Will respond with content in the web/-directory, and its subdirectories.
//	// Default file: index.html
//	// Can for instance be used to retrieve an HTML 5 client that uses REST-resources on this server
//	server.default_resource["GET"] = [](shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request) {
//		try {
//			auto web_root_path = boost::filesystem::canonical("web");
//			auto path = boost::filesystem::canonical(web_root_path / request->path);
//			// Check if path is within web_root_path
//			if (distance(web_root_path.begin(), web_root_path.end()) > distance(path.begin(), path.end()) ||
//				!equal(web_root_path.begin(), web_root_path.end(), path.begin()))
//				throw invalid_argument("path must be within root path");
//			if (boost::filesystem::is_directory(path))
//				path /= "index.html";
//
//			SimpleWeb::CaseInsensitiveMultimap header;
//
//			// Uncomment the following line to enable Cache-Control
//			// header.emplace("Cache-Control", "max-age=86400");
//
//#ifdef HAVE_OPENSSL
////    Uncomment the following lines to enable ETag
////    {
////      ifstream ifs(path.string(), ifstream::in | ios::binary);
////      if(ifs) {
////        auto hash = SimpleWeb::Crypto::to_hex_string(SimpleWeb::Crypto::md5(ifs));
////        header.emplace("ETag", "\"" + hash + "\"");
////        auto it = request->header.find("If-None-Match");
////        if(it != request->header.end()) {
////          if(!it->second.empty() && it->second.compare(1, hash.size(), hash) == 0) {
////            response->write(SimpleWeb::StatusCode::redirection_not_modified, header);
////            return;
////          }
////        }
////      }
////      else
////        throw invalid_argument("could not read file");
////    }
//#endif
//
//			auto ifs = make_shared<ifstream>();
//			ifs->open(path.string(), ifstream::in | ios::binary | ios::ate);
//
//			if (*ifs) {
//				auto length = ifs->tellg();
//				ifs->seekg(0, ios::beg);
//
//				header.emplace("Content-Length", to_string(length));
//				response->write(header);
//
//				// Trick to define a recursive function within this scope (for example purposes)
//				class FileServer {
//				public:
//					static void read_and_send(const shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> &response, const shared_ptr<ifstream> &ifs) {
//						// Read and send 128 KB at a time
//						static vector<char> buffer(131072); // Safe when server is running on one thread
//						streamsize read_length;
//						if ((read_length = ifs->read(&buffer[0], static_cast<streamsize>(buffer.size())).gcount()) > 0) {
//							response->write(&buffer[0], read_length);
//							if (read_length == static_cast<streamsize>(buffer.size())) {
//								response->send([response, ifs](const SimpleWeb::error_code &ec) {
//									if (!ec)
//										read_and_send(response, ifs);
//									else
//										cerr << "Connection interrupted" << endl;
//								});
//							}
//						}
//					}
//				};
//				FileServer::read_and_send(response, ifs);
//			}
//			else
//				throw invalid_argument("could not read file");
//		}
//		catch (const exception &e) {
//			response->write(SimpleWeb::StatusCode::client_error_bad_request, "Could not open path " + request->path + ": " + e.what());
//		}
//	};
//
//	server.on_error = [](shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
//		// Handle errors here
//		// Note that connection timeouts will also call this handle with ec set to SimpleWeb::errc::operation_canceled
//	};
//
//	// Start server and receive assigned port when server is listening for requests
//	promise<unsigned short> server_port;
//	thread server_thread([&server, &server_port]() {
//		// Start server
//		server.start([&server_port](unsigned short port) {
//			server_port.set_value(port);
//		});
//	});
//	cout << "Server listening on port " << server_port.get_future().get() << endl
//		<< endl;
//
//	// Client examples
//	string json_string = "{\"firstName\": \"John\",\"lastName\": \"Smith\",\"age\": 25}";
//
//	//// Synchronous request examples
//	//{
//	//  HttpClient client("localhost:8080");
//	//  try {
//	//    cout << "Example GET request to http://localhost:8080/match/123" << endl;
//	//    auto r1 = client.request("GET", "/match/123");
//	//    cout << "Response content: " << r1->content.rdbuf() << endl // Alternatively, use the convenience function r1->content.string()
//	//         << endl;
//
//	//    cout << "Example POST request to http://localhost:8080/string" << endl;
//	//    auto r2 = client.request("POST", "/string", json_string);
//	//    cout << "Response content: " << r2->content.rdbuf() << endl
//	//         << endl;
//	//  }
//	//  catch(const SimpleWeb::system_error &e) {
//	//    cerr << "Client request error: " << e.what() << endl;
//	//  }
//	//}
//
//	//// Asynchronous request example
//	//{
//	//  HttpClient client("localhost:8080");
//	//  cout << "Example POST request to http://localhost:8080/json" << endl;
//	//  client.request("POST", "/json", json_string, [](shared_ptr<HttpClient::Response> response, const SimpleWeb::error_code &ec) {
//	//    if(!ec)
//	//      cout << "Response content: " << response->content.rdbuf() << endl;
//	//  });
//	//  client.io_service->run();
//	//}
//
//	server_thread.join();
//	return 0;
//}

//有办法避免错误C2039：“值”：不是"boost::proto“的成员吗？

//在编写项目时，我使用boost::network::uri::encoded()对请求url.But进行编码，我看到了错误"error C2039：“value:不是”boost：：proto“的成员。其中有四个是boost\proto\generate.hpp(239,20);boost\proto\generate.hpp(239,53);boost\proto\generate.hpp(248,20);boost\proto\generate.hpp(248,53).报道的这是我的测试代码：
//
//#include <iostream>
//#include <string>
//#include "boost/network/uri.hpp"
//using std::string;
//string EncodeURL(string str)
//{
//	return boost::network::uri::encoded(str);
//}
//string DecodeURL(string str)
//{
//	return boost::network::uri::decoded(str);
//}
//int main()
//{
//	EncodeURL("https://test.com/a+a+a.html");
//	return 0;
//}
//复制
//我使用boost和cpp - netlib安装了vcpkg.My IDE is Visual Studio Professional 2019，操作系统是Windows 10 Professional Workstation x64(Ver.2004)。我想知道如何避免这些错误，或者用另一种方法来编码与UNICODE兼容的URL。
//Stack Overflow用户
//回答已采纳
//发布于 2020 - 05 - 15 03:22 : 57
//
//这是一种刺激利巴里的虫子。它可以通过更新boost库到最新版本或编辑boost\proto\generate.hpp来修复。
//
//编辑boost\proto\generate.hpp的方法
//
//将第95行改为
//
//#if BOOST_WORKAROUND(BOOST_MSVC, < 1800)
//	复制
//	将项目233改为
//
//#if BOOST_WORKAROUND(BOOST_MSVC, < 1800)
//	复制
//	这是来自boost libary的按下。