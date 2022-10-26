#ifndef __stream_file_H__
#define __stream_file_H__

#include "stream_base.h"
#include <windows.h>

namespace io
{
	struct stream_file : public stream_base
	{
	private:
		HANDLE m_hfile;
	public:
		stream_file(const char* path,bool readonly,bool create_always,bool &error);
		stream_file(const wchar_t* path,bool readonly,bool create_always,bool &error);
		virtual	~stream_file();
		virtual bstreamsize		size() const;
		virtual bstreamsize		seek(bstreamsize offset);
		virtual bstreamsize		position();
		virtual bstreamsize		read(char* dst,bstreamsize size);
		virtual bstreamsize		write(const char* src,bstreamsize size);
		void close();
	};
}

#endif //__stream_file_H__
