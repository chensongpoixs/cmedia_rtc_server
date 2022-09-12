#ifndef C_NET_DEFINE_H
#define C_NET_DEFINE_H
 
#ifdef _MSC_VER  
#ifndef _WIN32_WINNT
#define _WIN32_WINNT    0x0501  //windows7
#endif
#endif
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffer.hpp>
#endif //C_NET_DEFINE_H
