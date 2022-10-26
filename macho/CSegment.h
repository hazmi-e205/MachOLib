#ifndef __CSEGMENT_H__
#define __CSEGMENT_H__

#include <map>
#include <types.h>
#include <shared_ptr_alias.hpp>
#include <base/base.h>
#include <io/source_of_stream.h>
#include <io/stream_base.h>
#include "CSection.h"

namespace macho
{
	class CSegment
	{
	public:
		typedef tools::shared_ptr<CSegment> Ptr;
		typedef std::map<std::string,CSection::Ptr> SectionsTable;
	public:
		static const char kSEGMENT_TEXT[];
		static const char kSEGMENT_DWARF[];
	private:
		bool							m_isloaded;
		bool							m_isx64;
		SectionsTable					m_sections;
		t_uint32						m_start_off;
		t_uint32						m_offset_of_macho;
		io::source_of_stream			m_source;
		t_uint64						m_vmaddr;
		t_uint64						m_vmsize;
		std::string						m_name;
	private:
		base::ERESULT_CODE	preload();
	public:
		CSegment();
		CSegment(const io::source_of_stream& source,t_uint32 offset,t_uint32 offset_of_macho,bool is_x64);
		t_uint64		getVmAddress();
		t_uint64		getVmSize();
		std::string		getName();
		bool			isExistSection(const char* name);
		CSection::Ptr	getSection(const char* name);
		CSection::Ptr	getSectionAt(t_int32 idx);
		t_uint32		getSectionCount();
	};
}

#endif //__CSEGMENT_H__