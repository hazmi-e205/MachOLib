#ifndef __stream_cache_H__
#define __stream_cache_H__

#include "stream_base.h"
#include <utility/CConsole.h>
#include <string.h>

namespace io
{
	template<unsigned int SIZE>
	class stream_cache : public stream_base
	{
	public:
		enum { kSIZE = SIZE };
	private:
		io::stream_base::ptr m_parent;
		bstreamsize m_position;
		bstreamsize m_cache_position;
		bool		m_has_cache;
		char m_buffer[kSIZE];
	public:
		stream_cache(io::stream_base::ptr data)
			:m_parent(data),
			m_position(0),
			m_cache_position(0),
			m_has_cache(false)
		{
			memset(m_buffer,0,kSIZE);
			return;
		}
		virtual bstreamsize size() const
		{
			return m_parent->size();
		}
		virtual bstreamsize seek(bstreamsize offset)
		{
			m_position = std::min(offset, m_parent->size());
			return m_position;
		}
		virtual bstreamsize position()
		{
			return m_position;
		}
		virtual bstreamsize read(char* dst,bstreamsize size)
		{
			if(size <= kSIZE)
			{
				bstreamsize ret = size;
				if(!is_hit(size))
				{
					//do read into cache
					m_cache_position = m_parent->seek(m_position);
					ret = std::min(do_read(&m_buffer[0], kSIZE), size);
					m_has_cache = true;
					//TRACE("[stream_cache] not hit");
				}
				memcpy(dst,&m_buffer[m_position - m_cache_position], static_cast<size_t>(size));
				seek(m_position + ret);
				return ret;
			}
			else
			{
				bstreamsize ret = size;
				m_parent->seek(m_position);
				m_position += ret = do_read(dst, size);
				return ret;
			}
		}
		virtual bstreamsize write(const char* src,bstreamsize size)
		{
			reset_cache();
			return do_write(src, size);
		}
	private:
		void reset_cache()
		{
			m_has_cache = false;
		}
		bool is_hit(bstreamsize size) const
		{
			return (m_has_cache 
					&& (m_position >= m_cache_position)
					&& (m_position + size) < (m_cache_position + kSIZE));
		}
		bstreamsize do_read(char* dst,bstreamsize size)
		{
			m_parent->seek(m_position);
			return m_parent->read(dst, size);
		}
		bstreamsize do_write(const char* src,bstreamsize size)
		{
			m_parent->seek(m_position);
			return m_parent->write(src, size);
		}
	};

	template<unsigned int SIZE>
	inline io::stream_base::ptr make_cache(io::stream_base::ptr stream)
	{
		return io::stream_base::ptr(new stream_cache<SIZE>(stream));
	}
}
#endif //__stream_cache_H__
