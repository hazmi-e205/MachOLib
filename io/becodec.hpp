/*
*	author:		Буков Антон
*	account:	http://habrahabr.ru/users/k06a/
*	source:		http://habrahabr.ru/post/121811/
*
*	Мной были внесены небольшие изменения которые не должны влиять на функционирование.
*	Спасибо Букову Антону за отличный код.
*/
#ifndef __BIN_SON_BIG_ENDIAN_BYTE_ORDER_H__
#define __BIN_SON_BIG_ENDIAN_BYTE_ORDER_H__

#include "../types.h"
using namespace macho;

namespace io
{
	#pragma pack(push,1)
	template<typename T,T _NULL_VALUE = 0>
	struct becodec
	{
	public:
		typedef T		value_type;
		enum { value_size = sizeof(value_type) };
	public:
		template<typename IN_TYPE>
		inline static T encode(const IN_TYPE& t_in)
		{
			T value = static_cast<value_type>(t_in);
			union
			{
				t_uint8	buffer[value_size];
				T		result;
			} temp;
			for(unsigned int i = 0; i < value_size; ++i)
			{
				temp.buffer[((value_size - 1) - i)] = static_cast<t_uint8>((value >> (i << 3)));
			}
			return temp.result;
		}
		inline static T decode(const T& t_in)
		{
			union
			{
				t_uint8	buffer[value_size];
				T		result;
			} temp;
			temp.result = t_in;
			T tmp = T(_NULL_VALUE);
			for(unsigned int i = 0; i < value_size; ++i)
			{
					tmp |= (T(temp.buffer[((value_size - 1) - i)]) << (i << 3));
			}
			return tmp;
		}
	};
	#pragma pack(pop)
}

#endif //__BIN_SON_BIG_ENDIAN_BYTE_ORDER_H__
