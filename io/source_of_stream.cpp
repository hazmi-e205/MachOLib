#include "source_of_stream.h"
#include "stream_file.h"

namespace io
{
	source_of_stream::source_of_stream()
		:m_path(),
		m_encoding(EENCODING_UNKNOWN),
		m_sourse()
	{
		return;
	}
	source_of_stream::source_of_stream(const char* resource, EENCODING encoding)
		:m_path(),
		m_encoding(encoding),
		m_sourse()
	{
		std::string res(resource);
		m_path = std::wstring(res.begin(), res.end());
		return;
	}
	source_of_stream::source_of_stream(const wchar_t* resource, EENCODING encoding)
		:m_path(resource),
		m_encoding(encoding),
		m_sourse()
	{
		return;
	}
	source_of_stream::source_of_stream(const io::stream_base::ptr& resource, EENCODING encoding)
		:m_path(),
		m_encoding(encoding),
		m_sourse(resource)
	{
		return;
	}
	stream_base::ptr source_of_stream::open() const
	{
		bool error = false;
		stream_base::ptr p;
		if(m_path.empty() && m_sourse.get() != NULL)
		{
			p = m_sourse;
		}
		else
		{
			p.reset(new stream_file(m_path.c_str(),true,false,error));
		}
		if(m_encoding != EENCODING_UNKNOWN) 
		{
			p->set_encoding(m_encoding);
		}
		return error?stream_base::ptr():p;
	}
	std::wstring source_of_stream::path() const
	{
		return m_path;
	}
	void source_of_stream::set_encoding(EENCODING encoding)
	{
		m_encoding = encoding;
		return;
	}
}