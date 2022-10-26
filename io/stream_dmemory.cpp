#include "stream_dmemory.h"
#include <cstdlib>

namespace io
{
	stream_dmemory::stream_dmemory()
		:stream_memory(),
		m_reserved(m_size)
	{
		return;
	}
	stream_dmemory::stream_dmemory(bstreamsize size)
		:stream_memory(size),
		m_reserved(m_size)
	{
		return;
	}
	stream_dmemory::stream_dmemory(const char* data, bstreamsize size)
		: stream_memory(data, size),
		m_reserved(m_size)
	{
		return;
	}
	stream_dmemory::stream_dmemory(io::stream_base::ptr data, bstreamsize size)
		:stream_memory(data, size),
		m_reserved(m_size)
	{
		return;
	}
	stream_dmemory::stream_dmemory(const stream_dmemory& other)
		:stream_memory(other),
		m_reserved(other.m_reserved)
	{
		return;
	}
	stream_dmemory::~stream_dmemory()
	{
		return;
	}
	bstreamsize stream_dmemory::read(char* dst, bstreamsize size)
	{
		if (size)
		{
			size = std::min(m_size - m_position, size);
			memcpy(dst, &m_buffer[m_position], static_cast<size_t>(size));
			m_position += size;
		}
		return size;
	}
	bstreamsize stream_dmemory::write(const char* src, bstreamsize size)
	{
		if (size)
		{
			if ((m_position + size) > m_size)
			{
				if ((m_position + size) > m_reserved)
				{
					//need alloc
					//need update reserved
					m_buffer = static_cast<char*>(realloc(m_buffer, (size_t)((m_position + size) << 1)));
					m_reserved = (m_position + size) << 1;
				}
				//need updata size
				memcpy(&m_buffer[m_position], src, static_cast<size_t>(size));
				m_position = m_size = (m_position + size);
			}
			else
			{
				memcpy(&m_buffer[m_position], src, static_cast<size_t>(size));
				m_position += size;
			}
		}
		return size;
	}
	stream_dmemory& stream_dmemory::operator =(const stream_dmemory& other)
	{
		if (&other != this) {
			stream_dmemory temp(other);
			std::swap(*this, temp);
		}
		return *this;
	}
}
