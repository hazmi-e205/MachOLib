#include "stream_memory.h"
#include <cstdlib>

namespace io 
{
	stream_memory::stream_memory()
		:m_buffer(NULL),
		m_size(0),
		m_position(0)
	{
		return;
	}
	stream_memory::stream_memory(bstreamsize size)
		:m_buffer(NULL),
		m_size(0),
		m_position(0)
	{
		m_buffer = static_cast<char*>(malloc(static_cast<size_t>(size)));
		m_size = (m_buffer == NULL) ? (0) : (size);
		return;
	}
	stream_memory::stream_memory(const char* data, bstreamsize size)
		:m_buffer(NULL),
		m_size(0),
		m_position(0)
	{
		m_buffer = static_cast<char*>(malloc(static_cast<size_t>(size)));
		if(m_buffer != NULL) {
			memcpy(m_buffer,data,static_cast<size_t>(size));
			m_size = size;
		}
		return;
	}
	stream_memory::stream_memory(io::stream_base::ptr data, bstreamsize size)
		:m_buffer(NULL),
		m_size(0),
		m_position(0)
	{
		set_encoding(data->encoding());
		m_buffer = static_cast<char*>(malloc(static_cast<size_t>(size)));
		if(m_buffer != NULL) {
			data->read(m_buffer,size);
			m_size = size;
		}
		return;
	}
	stream_memory::stream_memory(const stream_memory& other)
		:m_buffer(NULL),
		m_size(0),
		m_position(0)
	{
		set_encoding(other.encoding());
		if(other.m_size > 0) {
			m_buffer = static_cast<char*>(malloc(static_cast<size_t>(other.m_size)));
			if(m_buffer != NULL) {
				memcpy(m_buffer,other.m_buffer,static_cast<size_t>(other.m_size));
				m_size = other.m_size;
			}
		}
		return;
	}
	stream_memory::~stream_memory()
	{
		free(m_buffer);
		return;
	}
	bstreamsize stream_memory::size() const
	{
		return m_size;
	}
	bstreamsize stream_memory::seek(bstreamsize offset)
	{
		if(offset < m_size) {
			m_position = offset;
		}
		return m_position;
	}
	bstreamsize stream_memory::position()
	{
		return m_position;
	}
	bstreamsize stream_memory::read(char* dst,bstreamsize size)
	{
		if (size)
		{
			size = std::min(m_size - m_position, size);
			memcpy(dst, &m_buffer[m_position], static_cast<size_t>(size));
			m_position += size;
		}
		return size;
	}
	bstreamsize stream_memory::write(const char* src,bstreamsize size)
	{
		if (size)
		{
			size = std::min(m_size - m_position,size);
			memcpy(&m_buffer[m_position], src, static_cast<size_t>(size));
			m_position += size;
		}
		return size;
	}
	stream_memory& stream_memory::operator =(const stream_memory& other)
	{
		if(&other != this) {
			stream_memory temp(other);
			std::swap(*this,temp);
		}
		return *this;
	}
	const char* stream_memory::data() const
	{
		return m_buffer;
	}
}
