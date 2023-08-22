#define MS_CLASS "Logger"
// #define MS_LOG_DEV_LEVEL 3

#include "Logger.hpp"
#include <uv.h>

/* Class variables. */

const int64_t Logger::pid{ static_cast<int64_t>(uv_os_getpid()) };
thread_local Channel::ChannelSocket* Logger::channel{ nullptr };
thread_local char Logger::buffer[Logger::bufferSize];

/* Class methods. */

void Logger::ClassInit(Channel::ChannelSocket* channel)
{
	Logger::channel = channel;

	MS_TRACE();
}
