/*
*	author:		Andrey Simiklit
*	mail:		andrey.simiklit@gmail.com
*/
#ifndef __BIN_SON_TYPE_H__
#define __BIN_SON_TYPE_H__

#include "../types.h"
#include "becodec.hpp"
#include "lecodec.hpp"

namespace io
{
	typedef macho::t_bool			t_bool;
	typedef macho::t_int8			t_int8;
	typedef macho::t_int16			t_int16;
	typedef macho::t_int32			t_int32;
	typedef macho::t_int64			t_int64;
	typedef macho::t_uint8			t_uint8;
	typedef macho::t_uint16			t_uint16;
	typedef macho::t_uint32			t_uint32;
	typedef macho::t_uint64			t_uint64;
	typedef macho::t_float32		t_float32;
	typedef macho::t_float64		t_float64;
    
	typedef unsigned int			t_size_type;
	typedef t_uint32                t_serializable_value_size_type;
	typedef t_uint8                 t_serializable_type;
	typedef t_uint64				bstreamsize;

	typedef t_int8					be_int8;
	typedef becodec<t_int16>		be_int16;
	typedef becodec<t_int32>		be_int32;
	typedef becodec<t_int64>		be_int64;
	typedef t_uint8					be_uint8;
	typedef becodec<t_uint16>		be_uint16;
	typedef becodec<t_uint32>		be_uint32;
	typedef becodec<t_uint64>		be_uint64;
    
	typedef be_int32				be_size_type;
	typedef be_uint32				be_value_size_type;
	typedef t_uint8					be_serializable_type;

	
	typedef t_int8					le_int8;
	typedef lecodec<t_int16>		le_int16;
	typedef lecodec<t_int32>		le_int32;
	typedef lecodec<t_int64>		le_int64;
	typedef t_uint8					le_uint8;
	typedef lecodec<t_uint16>		le_uint16;
	typedef lecodec<t_uint32>		le_uint32;
	typedef lecodec<t_uint64>		le_uint64;
}

#endif //__BIN_SON_TYPE_H__