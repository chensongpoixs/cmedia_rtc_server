/***********************************************************************************************
created: 		2023-02-02

author:			chensong

purpose:		 buffer
************************************************************************************************/
#include "cbuffer.h"
#include "clog.h"
namespace chen {
	cbuffer::~cbuffer()
	{
	}
	char * cbuffer::data()
	{
		return m_bytes_ptr;
	}
	char * cbuffer::head()
	{
		return m_position_ptr;
	}
	int32 cbuffer::size()
	{
		return m_nb_bytes;
	}
	void cbuffer::set_size(int32 v)
	{
		m_nb_bytes = v;
	}
	int32 cbuffer::pos()
	{
		return (int32(m_position_ptr - m_bytes_ptr));
	}
	int32 cbuffer::left()
	{
		return m_nb_bytes - (int32)(m_position_ptr - m_bytes_ptr);
	}
	bool cbuffer::empty()
	{
		return !m_bytes_ptr || (m_position_ptr>= m_bytes_ptr + m_nb_bytes);
	}
	bool cbuffer::require(int32 required_size)
	{
		if (required_size < 0)
		{
			return false;
		}
		 
		return required_size <= m_nb_bytes - (m_position_ptr - m_bytes_ptr);;
	}
	void cbuffer::skip(int32 size)
	{
		cassert(m_position_ptr);
		cassert(m_position_ptr + size >= m_bytes_ptr);
		cassert(m_position_ptr + size <= m_bytes_ptr + m_nb_bytes);

		m_position_ptr += size;
	}
	int8 cbuffer::read_1bytes()
	{

		cassert(require(1));

		return (int8)*m_position_ptr++;
	}
	int16 cbuffer::read_2bytes()
	{
		cassert(require(2));
		int16  value;

		char * pp = (char *)&value;
		pp[1] = *m_position_ptr++;
		pp[0] = *m_position_ptr++;

		return value;
	}
	int16 cbuffer::read_le2bytes()
	{
		cassert(require(2));
		int16  value;

		char * pp = (char *)&value;
		pp[0] = *m_position_ptr++;
		pp[1] = *m_position_ptr++;

		return value;
	}
	int32 cbuffer::read_3bytes()
	{
		cassert(require(3));

		int32  value = 0x00;
		char* pp = (char*)&value;
		pp[2] = *m_position_ptr++;
		pp[1] = *m_position_ptr++;
		pp[0] = *m_position_ptr++;

		return value;
	}
	int32 cbuffer::read_le3bytes()
	{
		cassert(require(3));

		int32  value = 0x00;
		char* pp = (char*)&value;
		pp[0] = *m_position_ptr++;
		pp[1] = *m_position_ptr++;
		pp[2] = *m_position_ptr++;

		return value;
	}
	int32 cbuffer::read_4bytes()
	{
		cassert(require(4));

		int32  value;
		char* pp = (char*)&value;
		pp[3] = *m_position_ptr++;
		pp[2] = *m_position_ptr++;
		pp[1] = *m_position_ptr++;
		pp[0] = *m_position_ptr++;

		return value;
	}
	int32 cbuffer::read_le4bytes()
	{
		cassert(require(4));

		int32 value;
		char* pp = (char*)&value;
		pp[0] = *m_position_ptr++;
		pp[1] = *m_position_ptr++;
		pp[2] = *m_position_ptr++;
		pp[3] = *m_position_ptr++;

		return value;
	}
	int64 cbuffer::read_8bytes()
	{
		cassert(require(8));

		int64  value;
		char* pp = (char*)&value;
		pp[7] = *m_position_ptr++;
		pp[6] = *m_position_ptr++;
		pp[5] = *m_position_ptr++;
		pp[4] = *m_position_ptr++;
		pp[3] = *m_position_ptr++;
		pp[2] = *m_position_ptr++;
		pp[1] = *m_position_ptr++;
		pp[0] = *m_position_ptr++;

		return value;
	}
	int64 cbuffer::read_le8bytes()
	{
		cassert(require(8));

		int64  value;
		char* pp = (char*)&value;
		pp[0] = *m_position_ptr++;
		pp[1] = *m_position_ptr++;
		pp[2] = *m_position_ptr++;
		pp[3] = *m_position_ptr++;
		pp[4] = *m_position_ptr++;
		pp[5] = *m_position_ptr++;
		pp[6] = *m_position_ptr++;
		pp[7] = *m_position_ptr++;

		return value;
	}
	std::string cbuffer::read_string(int32 len)
	{
		cassert(require(len));

		std::string value;
		value.append(m_position_ptr, len);

		m_position_ptr += len;

		return value;
	}
	void cbuffer::read_bytes(char * data, int32 size)
	{
		cassert(require(size));

		memcpy(data, m_position_ptr, size);

		m_position_ptr += size;
	}
	void cbuffer::write_1bytes(int8 value)
	{
		cassert(require(1));

		*m_position_ptr++ = value;
	}
	void cbuffer::write_2bytes(int16 value)
	{
		cassert(require(2));

		char* pp = (char*)&value;
		*m_position_ptr++ = pp[1];
		*m_position_ptr++ = pp[0];
	}
	void cbuffer::write_le2bytes(int16 value)
	{
		cassert(require(2));

		char* pp = (char*)&value;
		*m_position_ptr++ = pp[0];
		*m_position_ptr++ = pp[1];
	}
	void cbuffer::write_4bytes(int32 value)
	{
		cassert(require(4));

		char* pp = (char*)&value;
		*m_position_ptr++ = pp[3];
		*m_position_ptr++ = pp[2];
		*m_position_ptr++ = pp[1];
		*m_position_ptr++ = pp[0];
	}
	void cbuffer::write_le4bytes(int32 value)
	{
		cassert(require(4));

		char* pp = (char*)&value;
		*m_position_ptr++ = pp[0];
		*m_position_ptr++ = pp[1];
		*m_position_ptr++ = pp[2];
		*m_position_ptr++ = pp[3];
	}
	void cbuffer::write_3bytes(int32 value)
	{
		cassert(require(3));

		char* pp = (char*)&value;
		*m_position_ptr++ = pp[2];
		*m_position_ptr++ = pp[1];
		*m_position_ptr++ = pp[0];
	}
	void cbuffer::write_le3bytes(int32 value)
	{
		cassert(require(3));

		char* pp = (char*)&value;
		*m_position_ptr++ = pp[0];
		*m_position_ptr++ = pp[1];
		*m_position_ptr++ = pp[2];
	}
	void cbuffer::write_8bytes(int64 value)
	{
		cassert(require(8));

		char* pp = (char*)&value;
		*m_position_ptr++ = pp[7];
		*m_position_ptr++ = pp[6];
		*m_position_ptr++ = pp[5];
		*m_position_ptr++ = pp[4];
		*m_position_ptr++ = pp[3];
		*m_position_ptr++ = pp[2];
		*m_position_ptr++ = pp[1];
		*m_position_ptr++ = pp[0];
	}
	void cbuffer::write_le8bytes(int64 value)
	{
		cassert(require(8));

		char* pp = (char*)&value;
		*m_position_ptr++ = pp[0];
		*m_position_ptr++ = pp[1];
		*m_position_ptr++ = pp[2];
		*m_position_ptr++ = pp[3];
		*m_position_ptr++ = pp[4];
		*m_position_ptr++ = pp[5];
		*m_position_ptr++ = pp[6];
		*m_position_ptr++ = pp[7];
	}
	void cbuffer::write_string(std::string value)
	{
		if (value.empty()) 
		{
			return;
		}

		cassert(require((int32)value.length()));

		memcpy(m_position_ptr, value.data(), value.length());
		m_position_ptr += value.length();
	}
	void cbuffer::write_bytes(char * data, int32 size)
	{
		if (size <= 0) {
			return;
		}

		cassert(require(size));

		memcpy(m_position_ptr, data, size);
		m_position_ptr += size;
	}
}