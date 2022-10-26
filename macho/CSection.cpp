#include "CSection.h"
#include <utility/CConsole.h>
#include <io/stream_memory.h>
#include <io/substream.h>

namespace macho
{
	const char CSection::kSECTION_DEBUG_LINE[]			= "__debug_line";
	const char CSection::kSECTION_DEBUG_INFO[]			= "__debug_info";
	const char CSection::kSECTION_DEBUG_ABBR[]			= "__debug_abbrev";
	const char CSection::kSECTION_DEBUG_STR[]			= "__debug_str";
	const char CSection::kSECTION_DEBUG_ARANGES[]		= "__debug_aranges";
	const char CSection::kSECTION_DEBUG_RANGES[]		= "__debug_ranges";
	const char CSection::kSECTION_TEXT[]				= "__text";

	CSection::CSection()
		:m_isloaded(false),
		m_isx64(false),
		m_start_off(0),
		m_offset_of_macho(0),
		m_source(),
		m_offset_of_section_data(0),
		m_size_of_section_data(0),
		m_addr(0),
		m_size(0),
		m_sec()
	{
		return;
	}
	CSection::CSection(const io::source_of_stream& source,t_uint32 offset,t_uint32 offset_of_macho,bool is_x64)
		:m_isloaded(false),
		m_isx64(is_x64),
		m_start_off(offset),
		m_offset_of_macho(offset_of_macho),
		m_source(source),
		m_offset_of_section_data(0),
		m_size_of_section_data(0)
	{
		return;
	}
	base::ERESULT_CODE CSection::preload()
	{
		base::ERESULT_CODE ret_code = base::ERESULT_CODE_FAIL;
		io::stream_base::ptr stream = m_source.open();
		if(stream.get())
		{
			stream->seek(m_start_off);
			if(m_isx64)
			{
				section_64 sec64;
				stream->read(sec64.sectname,sizeof(sec64.sectname));
				stream->read(sec64.segname,sizeof(sec64.segname));
				stream->t_read(sec64.addr);
				stream->t_read(sec64.size);
				stream->t_read(sec64.offset);
				stream->t_read(sec64.align);
				stream->t_read(sec64.reloff);
				stream->t_read(sec64.nreloc);
				stream->t_read(sec64.flags);
				stream->t_read(sec64.reserved1);
				stream->t_read(sec64.reserved2);
				stream->t_read(sec64.reserved3);
				m_offset_of_section_data = sec64.offset;
				m_size_of_section_data = static_cast<t_uint32>(sec64.size);
				m_addr = sec64.addr;
				m_size = sec64.size;
				m_sec = std::string(sec64.sectname,strnlen(sec64.sectname,sizeof(sec64.sectname)));
			}
			else
			{
				section sec32;
				stream->read(sec32.sectname,sizeof(sec32.sectname));
				stream->read(sec32.segname,sizeof(sec32.segname));
				stream->t_read(sec32.addr);
				stream->t_read(sec32.size);
				stream->t_read(sec32.offset);
				stream->t_read(sec32.align);
				stream->t_read(sec32.reloff);
				stream->t_read(sec32.nreloc);
				stream->t_read(sec32.flags);
				stream->t_read(sec32.reserved1);
				stream->t_read(sec32.reserved2);
				m_offset_of_section_data = sec32.offset;
				m_size_of_section_data = sec32.size;
				m_addr = sec32.addr;
				m_size = sec32.size;
				m_sec = std::string(sec32.sectname,strnlen(sec32.sectname,sizeof(sec32.sectname)));

			}
			ret_code = base::ERESULT_CODE_SUCCESS;
			m_isloaded = true;
		}
		else
		{
			TRACE("[CSection (%s)] ERROR preload (stream is not opened)",m_sec.c_str());
		}
		return ret_code;
	}
	io::stream_base::ptr CSection::load()
	{
		if(!m_isloaded) {
			base::ERESULT_CODE ret = preload();
			assert(ret == base::ERESULT_CODE_SUCCESS);
			if(!base::is_successful_only(ret))
			{
				TRACE("[CSection (%s)] ERROR load",m_sec.c_str());
			}
		}
		io::stream_base::ptr stream = m_source.open();
		io::stream_base::ptr buffer;
		if(stream.get())
		{
			stream->seek(m_offset_of_macho + m_offset_of_section_data);
			buffer.reset(new io::stream_memory(stream, m_size_of_section_data));
		}
		else
		{
			TRACE("[CSection (%s)] ERROR load (stream is not opened)",m_sec.c_str());
		}
		return buffer;
	}
	io::stream_base::ptr CSection::read()
	{
		if(!m_isloaded) {
			base::ERESULT_CODE ret = preload();
			assert(ret == base::ERESULT_CODE_SUCCESS);
			if(!base::is_successful_only(ret))
			{
				TRACE("[CSection (%s)] ERROR read",m_sec.c_str());
			}
		}
		io::stream_base::ptr stream = m_source.open();
		io::stream_base::ptr buffer;
		if(stream.get())
		{
			stream->seek(m_offset_of_macho + m_offset_of_section_data);
			buffer.reset(new io::substream(stream, m_offset_of_macho + m_offset_of_section_data, m_size_of_section_data));
		}
		else
		{
			TRACE("[CSection (%s)] ERROR read (stream is not opened)",m_sec.c_str());
		}
		return buffer;
	}
	base::ERESULT_CODE CSection::preload_if_notloaded()
	{
		base::ERESULT_CODE ret = base::ERESULT_CODE_SUCCESS;
		if(!m_isloaded)
		{
			ret = preload();
			assert(ret == base::ERESULT_CODE_SUCCESS);
			if(!base::is_successful_only(ret))
			{
				TRACE("[CSection (%s)] ERROR preload_if_notloaded",m_sec.c_str());
			}
		}
		return ret;
	}
	t_uint64 CSection::getAddress()
	{
		if(!m_isloaded)
		{
			base::ERESULT_CODE ret = preload();
			assert(ret == base::ERESULT_CODE_SUCCESS);
			if(!base::is_successful_only(ret))
			{
				TRACE("[CSection (%s)] ERROR getAddress",m_sec.c_str());
			}
		}
		return m_addr;
	}
	t_uint64 CSection::getSize()
	{
		if(!m_isloaded)
		{
			base::ERESULT_CODE ret = preload();
			assert(ret == base::ERESULT_CODE_SUCCESS);
			if(!base::is_successful_only(ret))
			{
				TRACE("[CSection (%s)] ERROR getSize",m_sec.c_str());
			}
		}
		return m_size;
	}
	std::string CSection::getName()
	{
		if(!m_isloaded)
		{
			base::ERESULT_CODE ret = preload();
			assert(ret == base::ERESULT_CODE_SUCCESS);
			if(!base::is_successful_only(ret))
			{
				TRACE("[CSection (%s)] ERROR getName",m_sec.c_str());
			}
		}
		return m_sec;
	}
}