#ifndef __MACHO_TYPES_H__
#define __MACHO_TYPES_H__

namespace macho
{
	typedef bool					t_bool;
	typedef signed char				t_int8;
	typedef signed short			t_int16;
	typedef signed long int			t_int32;
	typedef signed long long int	t_int64;
	typedef unsigned char			t_uint8;
	typedef unsigned short			t_uint16;
	typedef unsigned long int		t_uint32;
	typedef unsigned long long int	t_uint64;
	typedef float					t_float32;
	typedef double					t_float64;
}

namespace base
{
	typedef macho::t_bool		t_bool;
	typedef macho::t_int8		t_int8;
	typedef macho::t_int16		t_int16;
	typedef macho::t_int32		t_int32;
	typedef macho::t_int64		t_int64;
	typedef macho::t_uint8		t_uint8;
	typedef macho::t_uint16		t_uint16;
	typedef macho::t_uint32		t_uint32;
	typedef macho::t_uint64		t_uint64;
	typedef macho::t_float32	t_float32;
	typedef macho::t_float64	t_float64;
}

#endif //__MACHO_TYPES_H__
