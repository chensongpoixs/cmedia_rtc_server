/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_TRACK_DESCRIPTION_H_

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
#include "cselect_scheduler.h"
namespace chen {
	cselect_scheduler::cselect_scheduler()
	{
		static std::once_flag flag;
		std::call_once(flag, [] {
#if defined(WIN32) || defined(_WIN32) 
			WSADATA wsa_data;
			if (WSAStartup(MAKEWORD(2, 2), &wsa_data)) {
				WSACleanup();
			}
#endif
		});
		FD_ZERO(&fd_read_backup_);
		FD_ZERO(&fd_write_backup_);
		FD_ZERO(&fd_exp_backup_);
	}
	cselect_scheduler::~cselect_scheduler()
	{
	}
	void cselect_scheduler::UpdateChannel(cnet_session * session)
	{
		std::lock_guard<std::mutex> lock(mutex_);

		SOCKET socket = session->GetSocket();

		if (m_sessions.find(socket) != m_sessions.end()) {
			if (session->IsNoneEvent()) {
				is_fd_read_reset_ = true;
				is_fd_write_reset_ = true;
				is_fd_exp_reset_ = true;
				m_sessions.erase(socket);
			}
			else {
				//is_fd_read_reset_ = true;
				is_fd_write_reset_ = true;
			}
		}
		else {
			if (!session->IsNoneEvent()) {
				m_sessions.emplace(socket, session);
				is_fd_read_reset_ = true;
				is_fd_write_reset_ = true;
				is_fd_exp_reset_ = true;
			}
		}
	}
	void cselect_scheduler::RemoveChannel(cnet_session * session)
	{
		std::lock_guard<std::mutex> lock(mutex_);

		SOCKET fd = session->GetSocket();

		if (m_sessions.find(fd) != m_sessions.end()) {
			is_fd_read_reset_ = true;
			is_fd_write_reset_ = true;
			is_fd_exp_reset_ = true;
			m_sessions.erase(fd);
		}
	}
	bool cselect_scheduler::HandleEvent(int32 timeout)
	{
		if (m_sessions.empty()) 
		{
			if (timeout <= 0) 
			{
				timeout = 10;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
			//Timer::Sleep(timeout);
			return true;
		}


		fd_set fd_read;
		fd_set fd_write;
		fd_set fd_exp;
		FD_ZERO(&fd_read);
		FD_ZERO(&fd_write);
		FD_ZERO(&fd_exp);
		bool fd_read_reset = false;
		bool fd_write_reset = false;
		bool fd_exp_reset = false;

		if (is_fd_read_reset_ || is_fd_write_reset_ || is_fd_exp_reset_) {
			if (is_fd_exp_reset_) {
				maxfd_ = 0;
			}

			std::lock_guard<std::mutex> lock(mutex_);
			for (const std::pair<SOCKET, cnet_session*>&iter : m_sessions) {
				int events = iter.second->GetEvents();
				SOCKET fd = iter.second->GetSocket();

				if (is_fd_read_reset_ && (events&EVENT_IN)) {
					FD_SET(fd, &fd_read);
				}

				if (is_fd_write_reset_ && (events&EVENT_OUT)) {
					FD_SET(fd, &fd_write);
				}

				if (is_fd_exp_reset_) {
					FD_SET(fd, &fd_exp);
					if (fd > maxfd_) {
						maxfd_ = fd;
					}
				}
			}

			fd_read_reset = is_fd_read_reset_;
			fd_write_reset = is_fd_write_reset_;
			fd_exp_reset = is_fd_exp_reset_;
			is_fd_read_reset_ = false;
			is_fd_write_reset_ = false;
			is_fd_exp_reset_ = false;
		}

		if (fd_read_reset) {
			FD_ZERO(&fd_read_backup_);
			memcpy(&fd_read_backup_, &fd_read, sizeof(fd_set));
		}
		else {
			memcpy(&fd_read, &fd_read_backup_, sizeof(fd_set));
		}


		if (fd_write_reset) {
			FD_ZERO(&fd_write_backup_);
			memcpy(&fd_write_backup_, &fd_write, sizeof(fd_set));
		}
		else {
			memcpy(&fd_write, &fd_write_backup_, sizeof(fd_set));
		}


		if (fd_exp_reset) {
			FD_ZERO(&fd_exp_backup_);
			memcpy(&fd_exp_backup_, &fd_exp, sizeof(fd_set));
		}
		else {
			memcpy(&fd_exp, &fd_exp_backup_, sizeof(fd_set));
		}

		if (timeout <= 0) {
			timeout = 10;
		}

		struct timeval tv = { timeout / 1000, timeout % 1000 * 1000 };
		int ret = select((int)maxfd_ + 1, &fd_read, &fd_write, &fd_exp, &tv);
		if (ret < 0) {
#if defined(__linux) || defined(__linux__) 
			if (errno == EINTR) {
				return true;
			}
#endif 
			return false;
		}

		//std::forward_list<std::pair<ChannelPtr, int>> event_list;
		if (ret > 0) {
			std::lock_guard<std::mutex> lock(mutex_);
			for (std::pair<const SOCKET, cnet_session*>& iter : m_sessions) {
				int32 events = 0;
				SOCKET socket = iter.second->GetSocket();

				if (FD_ISSET(socket, &fd_read)) {
					events |= EVENT_IN;
				}

				if (FD_ISSET(socket, &fd_write)) {
					events |= EVENT_OUT;
				}

				if (FD_ISSET(socket, &fd_exp)) {
					events |= (EVENT_HUP); // close
				}

				if (events != 0) 
				{
					//event_list.emplace_front(iter.second, events);
					iter.second->HandleEvent(events);
				}
			}
		}

		/*for (auto& iter : event_list) {
			iter.first->HandleEvent(iter.second);
		}*/

		return true;


		return true;
	}
}