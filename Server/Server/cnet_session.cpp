#include "cnet_session.h"
namespace chen {

	static const uint32 MAX_NET_BYTE = 1024 * 64;

	 cnet_session::cnet_session(SOCKET socket)
		 :m_socket(socket)
		 , m_data_ptr(NULL)
	 {
		 m_data_ptr = new uint8[MAX_NET_BYTE];
	 }
	cnet_session::~cnet_session()
	{
		if (m_data_ptr)
		{
			delete m_data_ptr;
			m_data_ptr = NULL;
		}
	}
	void cnet_session::HandleEvent(int32 events)
	{
		if (events & (EVENT_PRI | EVENT_IN)) 
		{
			//read_callback_();
			//recvfrom();
			if (true)
			{
				/*int
					WSAAPI
					recvfrom(
						_In_ SOCKET s,
						_Out_writes_bytes_to_(len, return) __out_data_source(NETWORK) char FAR * buf,
						_In_ int len,
						_In_ int flags,
						_Out_writes_bytes_to_opt_(*fromlen, *fromlen) struct sockaddr FAR * from,
						_Inout_opt_ int FAR * fromlen
					)*/
				//recvfrom();
				struct sockaddr_storage from;
				int from_len;
				memset(&from, 0, sizeof from);
				from_len = sizeof from;
				int32 bytes_read = recvfrom(m_socket, (char *)( m_data_ptr), MAX_NET_BYTE, 0, (struct sockaddr*)&from, &from_len);
				if (bytes_read > 0)
				{
					 read_callback_(m_data_ptr, bytes_read, &from);
				}
				/*else
				{
					break;
				}*/
			}
		}

		if (events & EVENT_OUT) 
		{
			//write_callback_();
			write_callback_();
		}

		if (events & EVENT_HUP) 
		{
			//close_callback_();
			return;
		}

		if (events & (EVENT_ERR)) 
		{
			//error_callback_();
		}
	}
}