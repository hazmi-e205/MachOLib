#include "substream.h"

namespace io 
{
	substream::substream()
		:m_parent(),
		m_position(0),
		m_start(0),
		m_size(0)
	{
		return;
	}
	substream::substream(io::stream_base::ptr data,bstreamsize start, bstreamsize size)
		:m_parent(data),
		m_position(0),
		m_start(start),
		m_size(size)
	{
		return;
	}
	bstreamsize substream::size() const
	{
		return m_size;
	}
	bstreamsize substream::seek(bstreamsize offset)
	{
		if(offset < m_size) {
			m_position = offset;
		}
		return m_position;
	}
	bstreamsize substream::position()
	{
		return m_position;
	}
	bstreamsize substream::read(char* dst,bstreamsize size)
	{
		bstreamsize ret = 0;
		if(m_parent)
		{
			size = std::min(m_size - m_position,size);
			if(m_parent->seek(m_start + m_position) == (m_start + m_position)) {
				m_position += ret = m_parent->read(dst, size);
			}
		}
		return ret;
	}
	bstreamsize substream::write(const char* src,bstreamsize size)
	{
		bstreamsize ret = 0;
		if(m_parent)
		{
			size = std::min(m_size - m_position,size);
			if(m_parent->seek(m_start + m_position) == (m_start + m_position)) {
				m_position += ret = m_parent->write(src, size);
			}
		}
		return ret;
	}
}