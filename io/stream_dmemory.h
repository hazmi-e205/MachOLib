#ifndef __STREAM_DMEMORY_H__
#define __STREAM_DMEMORY_H__

#include "stream_memory.h"

namespace io
{
	class stream_dmemory : public stream_memory
	{
	public:
		stream_dmemory();
		stream_dmemory(bstreamsize size);
		stream_dmemory(const char* data, bstreamsize size);
		stream_dmemory(io::stream_base::ptr data, bstreamsize size);
		stream_dmemory(const stream_dmemory& other);
		virtual	~stream_dmemory();
		virtual bstreamsize read(char* dst, bstreamsize size);
		virtual bstreamsize write(const char* src, bstreamsize size);
		stream_dmemory& operator =(const stream_dmemory& other);
	private:
		bstreamsize m_reserved;
	};
}

#endif //__STREAM_MEMORY_H__