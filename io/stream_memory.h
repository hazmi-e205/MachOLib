#ifndef __stream_memory_H__
#define __stream_memory_H__

#include "stream_base.h"

namespace io
{
	struct stream_memory : public stream_base
	{
	protected:
		char*		m_buffer;
		bstreamsize m_size;
		bstreamsize m_position;
	public:
		stream_memory();
		stream_memory(bstreamsize size);
		stream_memory(const char* data, bstreamsize size);
		stream_memory(io::stream_base::ptr data,bstreamsize size);
		stream_memory(const stream_memory& other);
		virtual	~stream_memory();
		virtual bstreamsize		size() const;
		virtual bstreamsize		seek(bstreamsize offset);
		virtual bstreamsize		position();
		virtual bstreamsize		read(char* dst,bstreamsize size);
		virtual bstreamsize		write(const char* src,bstreamsize size);
		virtual const char*		data() const;
		stream_memory& operator =(const stream_memory& other);
	};
}

#endif //__stream_memory_H__