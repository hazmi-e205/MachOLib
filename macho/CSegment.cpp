#include "CSegment.h"
#include "MachO.h"
#include <utility/CConsole.h>
#include <io/stream_memory.h>

namespace macho
{
	const char CSegment::kSEGMENT_TEXT[] = "__TEXT";
	const char CSegment::kSEGMENT_DWARF[] = "__DWARF";

	CSegment::CSegment()
		:m_isloaded(false),
		m_isx64(false),
		m_sections(),
		m_start_off(0),
		m_offset_of_macho(0),
		m_source(),
		m_vmaddr(0),
		m_vmsize(0),
		m_name()
	{
		return;
	}
	CSegment::CSegment(const io::source_of_stream& source,t_uint32 offset,t_uint32 offset_of_macho,bool is_x64)
		:m_isloaded(false),
		m_isx64(is_x64),
		m_sections(),
		m_start_off(offset),
		m_offset_of_macho(offset_of_macho),
		m_source(source),
		m_vmaddr(0),
		m_vmsize(0),
		m_name()
	{
		return;
	}
	base::ERESULT_CODE CSegment::preload()
	{
		base::ERESULT_CODE ret_code = base::ERESULT_CODE_FAIL;
		io::stream_base::ptr stream = m_source.open();
		if(stream.get())
		{
			stream->seek(m_start_off);
			if(m_isx64)
			{
				char segname[17] = {0};
				t_uint64 vmaddr, vmsize, fileoff, filesize;
				t_uint32 maxprot, initprot, nsects, flags;
				stream->read(segname,sizeof(segname)-1);
				stream->t_read(vmaddr);
				stream->t_read(vmsize);
				stream->t_read(fileoff);
				stream->t_read(filesize);
				stream->t_read(maxprot);
				stream->t_read(initprot);
				stream->t_read(nsects);
				stream->t_read(flags);
				m_vmaddr = vmaddr;
				m_vmsize = vmsize;
				m_name = segname;

				t_uint32 offset_of_sections = static_cast<t_uint32>(stream->position());
				io::stream_memory sections(stream, sizeof(macho::section_64) * nsects);
				for(t_uint32 i = 0; i < nsects; ++i)
				{
					
					static const int size_of_other_part64 = (sizeof(char)*16) //segname
															+ sizeof(t_uint64) //addr
															+ sizeof(t_uint64) //size
															+ sizeof(t_uint32) //offset
															+ sizeof(t_uint32) //align
															+ sizeof(t_uint32) //reloff
															+ sizeof(t_uint32) //nreloc
															+ sizeof(t_uint32) //flags
															+ sizeof(t_uint32) //reserved1
															+ sizeof(t_uint32) //reserved2
															+ sizeof(t_uint32); //reserved3
					char sectname[17] = {0};
					t_uint32 offset_of_cur_section = static_cast<t_uint32>(sections.position());
					sections.read(sectname,sizeof(sectname) - 1);
					sections.seek(sections.position() + size_of_other_part64);
					CSection::Ptr sec(new CSection(m_source,offset_of_sections + offset_of_cur_section,m_offset_of_macho,true));
					m_sections.insert(std::make_pair(sectname,sec));
				}
				m_isloaded = true;
				ret_code = base::ERESULT_CODE_SUCCESS;
			}
			else
			{
				char segname[17] = {0};
				t_uint32 vmaddr, vmsize, fileoff, filesize, 
						maxprot, initprot, nsects, flags;
				stream->read(segname,sizeof(segname) - 1);
				stream->t_read(vmaddr);
				stream->t_read(vmsize);
				stream->t_read(fileoff);
				stream->t_read(filesize);
				stream->t_read(maxprot);
				stream->t_read(initprot);
				stream->t_read(nsects);
				stream->t_read(flags);
				m_vmaddr = vmaddr;
				m_vmsize = vmsize;
				m_name = segname;

				t_uint32 offset_of_sections = static_cast<t_uint32>(stream->position());
				io::stream_memory sections(stream, sizeof(macho::section) * nsects);
				for(t_uint32 i = 0; i < nsects; ++i)
				{
					static const int size_of_other_part = (sizeof(char)*16) //segname
															+ sizeof(t_uint32) //addr
															+ sizeof(t_uint32) //size
															+ sizeof(t_uint32) //offset
															+ sizeof(t_uint32) //align
															+ sizeof(t_uint32) //reloff
															+ sizeof(t_uint32) //nreloc
															+ sizeof(t_uint32) //flags
															+ sizeof(t_uint32) //reserved1
															+ sizeof(t_uint32); //reserved2
					char sectname[17] = {0};
					t_uint32 offset_of_cur_section = static_cast<t_uint32>(sections.position());
					sections.read(sectname,sizeof(sectname) - 1);
					sections.seek(sections.position() + size_of_other_part);
					CSection::Ptr sec(new CSection(m_source,offset_of_sections + offset_of_cur_section,m_offset_of_macho,false));
					m_sections.insert(std::make_pair(sectname,sec));
				}
				m_isloaded = true;
				ret_code = base::ERESULT_CODE_SUCCESS;
			}
		}
		else
		{
			TRACE("[CSegment] preload error (stream is not opened)!!!");
		}
		return ret_code;
	}
	t_uint64 CSegment::getVmAddress()
	{
		if(!m_isloaded) {
			base::ERESULT_CODE ret_code = preload();
			//assert(ret_code == base::ERESULT_CODE_SUCCESS);
			if(!base::is_successful_only(ret_code))
			{
				TRACE("[CSegment (%s)] getVmAddress",m_name.c_str());
			}
		}
		return m_vmaddr;
	}
	t_uint64 CSegment::getVmSize()
	{
		if(!m_isloaded) {
			base::ERESULT_CODE ret_code = preload();
			//assert(ret_code == base::ERESULT_CODE_SUCCESS);
			if(!base::is_successful_only(ret_code))
			{
				TRACE("[CSegment (%s)] ERROR getVmSize",m_name.c_str());
			}
		}
		return m_vmsize;
	}
	bool CSegment::isExistSection(const char* name)
	{
		if(!m_isloaded) {
			base::ERESULT_CODE ret_code = preload();
			//assert(ret_code == base::ERESULT_CODE_SUCCESS);
			if(!base::is_successful_only(ret_code))
			{
				TRACE("[CSegment (%s)] ERROR isExistSection name = %s",m_name.c_str(),name);
			}
		}
		return (m_sections.find(name) != m_sections.end());
	}
	CSection::Ptr CSegment::getSection(const char* name)
	{
		if(!m_isloaded) {
			base::ERESULT_CODE ret_code = preload();
			//assert(ret_code == base::ERESULT_CODE_SUCCESS);
			if(!base::is_successful_only(ret_code))
			{
				TRACE("[CSegment (%s)] ERROR getSection name = %s",m_name.c_str(),name);
			}
		}
		return m_sections[name];
	}
	CSection::Ptr CSegment::getSectionAt(t_int32 idx)
	{
		if(!m_isloaded) {
			base::ERESULT_CODE ret_code = preload();
			//assert(ret_code == base::ERESULT_CODE_SUCCESS);
		}
		t_int32 chk = 0;
		for(SectionsTable::const_iterator i = m_sections.begin(), e = m_sections.end();
			i != e;
			++i)
		{
			if(chk == idx)
			{
				return (*i).second;
			}
			++chk;
		}
		//assert(false);
		return CSection::Ptr();
	}
	t_uint32 CSegment::getSectionCount()
	{
		return m_sections.size();
	}
	std::string CSegment::getName()
	{
		if(!m_isloaded) {
			base::ERESULT_CODE ret_code = preload();
			//assert(ret_code == base::ERESULT_CODE_SUCCESS);
		}
		return m_name;
	}
}