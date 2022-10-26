#ifndef __CFAT_ARCH_H__
#define __CFAT_ARCH_H__

#include <types.h>
#include <base/base.h>
#include <io/source_of_stream.h>
#include "ECPUTypes.h"
#include "ECPUSubTypes.h"

namespace macho
{
	class CFATArch
	{
	private:
		io::source_of_stream	m_source;

		ECPU_TYPE				m_cputype;
		ECPU_SUB_TYPE			m_cpusubtype;
		macho::t_uint32			m_macho_off;
		macho::t_uint32			m_macho_size;
		macho::t_uint32			m_align;
	public:
		CFATArch();
		~CFATArch();
		base::ERESULT_CODE deserialize(io::stream_base::ptr stream);
		ECPU_TYPE getTypeOfCPU() const;
		ECPU_SUB_TYPE getSubTypeOfCPU() const;
		macho::t_uint32 getOffset() const;
		macho::t_uint32 getSize() const;
		macho::t_uint32 getAlign() const;
	};
}

#endif //__CFAT_ARCH_H__