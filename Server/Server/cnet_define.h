#ifndef C_NET_DEFINE_H
#define C_NET_DEFINE_H

#ifdef _MSC_VER  
#ifndef _WIN32_WINNT
#define _WIN32_WINNT    0X0601  //windows7
#endif
#endif
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#endif //C_NET_DEFINE_H
