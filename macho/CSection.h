#ifndef __CSECTION_H__
#define __CSECTION_H__

#include <map>
#include <types.h>
#include <shared_ptr_alias.hpp>
#include <base/base.h>
#include <io/source_of_stream.h>
#include <io/stream_base.h>

namespace macho
{
	class CSection
	{
	public:
		typedef tools::shared_ptr<CSection> Ptr;
	public:
		static const char kSECTION_DEBUG_LINE[];
		static const char kSECTION_DEBUG_INFO[];
		static const char kSECTION_DEBUG_ABBR[];
		static const char kSECTION_DEBUG_STR[];
		static const char kSECTION_DEBUG_ARANGES[];
		static const char kSECTION_DEBUG_RANGES[];
		static const char kSECTION_TEXT[];
	public:
		CSection();
		CSection(const io::source_of_stream& source,t_uint32 offset,t_uint32 offset_of_macho,bool is_x64);
		base::ERESULT_CODE		preload_if_notloaded();
		//load section to memory
		io::stream_base::ptr	load();
		//return substream of full stream
		io::stream_base::ptr	read();
		t_uint64				getAddress();
		t_uint64				getSize();
		std::string				getName();
	private:
		base::ERESULT_CODE	preload();
	private:
		struct section { /* for 32-bit architectures */
			char		sectname[16];	/* name of this section */
			char		segname[16];	/* segment this section goes in */
			t_uint32	addr;		/* memory address of this section */
			t_uint32	size;		/* size in bytes of this section */
			t_uint32	offset;		/* file offset of this section */
			t_uint32	align;		/* section alignment (power of 2) */
			t_uint32	reloff;		/* file offset of relocation entries */
			t_uint32	nreloc;		/* number of relocation entries */
			t_uint32	flags;		/* flags (section type and attributes)*/
			t_uint32	reserved1;	/* reserved (for offset or index) */
			t_uint32	reserved2;	/* reserved (for count or sizeof) */
		};
		struct section_64 { /* for 64-bit architectures */
			char		sectname[16];	/* name of this section */
			char		segname[16];	/* segment this section goes in */
			t_uint64	addr;		/* memory address of this section */
			t_uint64	size;		/* size in bytes of this section */
			t_uint32	offset;		/* file offset of this section */
			t_uint32	align;		/* section alignment (power of 2) */
			t_uint32	reloff;		/* file offset of relocation entries */
			t_uint32	nreloc;		/* number of relocation entries */
			t_uint32	flags;		/* flags (section type and attributes)*/
			t_uint32	reserved1;	/* reserved (for offset or index) */
			t_uint32	reserved2;	/* reserved (for count or sizeof) */
			t_uint32	reserved3;	/* reserved */
		};
	private:
		bool							m_isloaded;
		bool							m_isx64;
		t_uint32						m_start_off;
		t_uint32						m_offset_of_macho;
		io::source_of_stream			m_source;
		t_uint32						m_offset_of_section_data;
		t_uint32						m_size_of_section_data;
		t_uint64						m_addr;		/* memory address of this section */
		t_uint64						m_size;		/* size in bytes of this section */
		std::string						m_sec;
	};
}

#endif //__CSECTION_H__