#include "CFATArch.h"

namespace macho
{
	CFATArch::CFATArch()
		:m_cputype(ECPU_TYPE_ANY),
		m_cpusubtype(ECPU_SUB_TYPE_ALL),
		m_macho_off(0),
		m_macho_size(0),
		m_align(0)
	{
		return;
	}
	CFATArch::~CFATArch()
	{
		return;
	}
	ECPU_TYPE CFATArch::getTypeOfCPU() const
	{
		return m_cputype;
	}
	ECPU_SUB_TYPE CFATArch::getSubTypeOfCPU() const
	{
		return m_cpusubtype;
	}
	t_uint32 CFATArch::getOffset() const
	{
		return m_macho_off;
	}
	t_uint32 CFATArch::getSize() const
	{
		return m_macho_size;
	}
	t_uint32 CFATArch::getAlign() const
	{
		return m_align;
	}
	base::ERESULT_CODE CFATArch::deserialize(io::stream_base::ptr stream)
	{
		base::ERESULT_CODE ret_code = base::ERESULT_CODE_FAIL;
		t_uint32	cputype;	/* cpu specifier (int) */
		t_uint32	cpusubtype;	/* machine specifier (int) */
		t_uint32	offset;		/* file offset to this object file */
		t_uint32	size;		/* size of this object file */
		t_uint32	align;		/* alignment as a power of 2 */

		stream->t_read(cputype);
		stream->t_read(cpusubtype);
		stream->t_read(offset);
		stream->t_read(size);
		stream->t_read(align);
		ret_code = base::ERESULT_CODE_SUCCESS;

		m_cputype = static_cast<ECPU_TYPE>(cputype);
		m_cpusubtype = static_cast<ECPU_SUB_TYPE>(cpusubtype);
		m_macho_off = offset;
		m_macho_size = size;
		m_align = align;
		return ret_code;
	}
}