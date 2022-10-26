#ifndef __SOURCE_OF_STREAM_H__
#define __SOURCE_OF_STREAM_H__

#include "stream_base.h"
#include <string>

namespace io 
{
	class source_of_stream
	{
	private:
		std::wstring m_path;
		EENCODING m_encoding;
		io::stream_base::ptr m_sourse;
	public:
		source_of_stream();
		source_of_stream(const char* resource, EENCODING encoding);
		source_of_stream(const wchar_t* resource, EENCODING encoding);
		/*for possible to fake source_of_stream*/
		source_of_stream(const io::stream_base::ptr& resource, EENCODING encoding);
		stream_base::ptr open() const;
		std::wstring path() const;
		void set_encoding(EENCODING encoding);
	};
}
#endif //__SOURCE_OF_STREAM_H__