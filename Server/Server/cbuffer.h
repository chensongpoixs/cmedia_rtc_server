/***********************************************************************************************
created: 		2023-02-02

author:			chensong

purpose:		 buffer
************************************************************************************************/


#ifndef _C_BUFFER_H_
#define _C_BUFFER_H_
#include "cnet_type.h"

namespace chen {

	class cbuffer
	{
	public:
		explicit cbuffer(char * p, int32 size)
		: m_position_ptr(p)
		, m_bytes_ptr(p)
		, m_nb_bytes(size){}
		 ~cbuffer();
	public:
		 
		// Get the data and head of buffer.
		//      current-bytes = head() = data() + pos()
		char* data();
		char* head();
		// Get the total size of buffer.
		//      left-bytes = size() - pos()
		int32 size();
		void set_size(int32 v);
		// Get the current buffer position.
		int32 pos();
		// Left bytes in buffer, total size() minus the current pos().
		int32 left();
		// Whether buffer is empty.
		bool empty();
		// Whether buffer is able to supply required size of bytes.
		// @remark User should check buffer by require then do read/write.
		// @remark Assert the required_size is not negative.
		bool require(int32 required_size);
	public:
		// Skip some size.
		// @param size can be any value. positive to forward; negative to backward.
		// @remark to skip(pos()) to reset buffer.
		// @remark assert initialized, the data() not NULL.
		void skip(int32 size);
	public:
		// Read 1bytes char from buffer.
		int8 read_1bytes();
		// Read 2bytes int from buffer.
		int16  read_2bytes();
		int16  read_le2bytes();
		// Read 3bytes int from buffer.
		int32  read_3bytes();
		int32  read_le3bytes();
		// Read 4bytes int from buffer.
		int32  read_4bytes();
		int32  read_le4bytes();
		// Read 8bytes int from buffer.
		int64  read_8bytes();
		int64  read_le8bytes();
		// Read string from buffer, length specifies by param len.
		std::string read_string(int32 len);
		// Read bytes from buffer, length specifies by param len.
		void read_bytes(char* data, int32 size);
	public:
		// Write 1bytes char to buffer.
		void write_1bytes(int8  value);
		// Write 2bytes int to buffer.
		void write_2bytes(int16  value);
		void write_le2bytes(int16  value);
		// Write 4bytes int to buffer.
		void write_4bytes(int32  value);
		void write_le4bytes(int32  value);
		// Write 3bytes int to buffer.
		void write_3bytes(int32  value);
		void write_le3bytes(int32  value);
		// Write 8bytes int to buffer.
		void write_8bytes(int64  value);
		void write_le8bytes(int64  value);
		// Write string to buffer
		void write_string(std::string value);
		// Write bytes to buffer
		void write_bytes(char* data, int32 size);
	protected:
	private: 
		// current position at bytes.
		char*							m_position_ptr;
		// the bytes data for buffer to read or write.
		char*							m_bytes_ptr;
		// the total number of bytes.
		int32							m_nb_bytes;

	};

}
#endif // _C_BUFFER_H_