#ifndef __substream_H__
#define __substream_H__

#include "stream_base.h"

namespace io
{
	class substream : public stream_base
	{
	private:
		io::stream_base::ptr m_parent;
		bstreamsize m_position;
		bstreamsize m_start;
		bstreamsize m_size;
	public:
		substream();
		substream(io::stream_base::ptr data,bstreamsize start, bstreamsize size);
		virtual bstreamsize		size() const;
		virtual bstreamsize		seek(bstreamsize offset);
		virtual bstreamsize		position();
		virtual bstreamsize		read(char* dst,bstreamsize size);
		virtual bstreamsize		write(const char* src,bstreamsize size);
	};
}
#endif //