/*
*	author:		Andrey Simiklit
*	mail:		andrey.simiklit@gmail.com
*/
#ifndef __BIN_SON_BASE_STREAM_H__
#define __BIN_SON_BASE_STREAM_H__

#include <boost/shared_ptr.hpp>
#include "io_betype.h"

namespace io
{
	struct stream_base
	{
	public:
		typedef boost::shared_ptr<stream_base> ptr;
	private:
		EENCODING m_be;
	public:
		stream_base() :m_be(EENCODING_LITTLE_ENDIAN){}
		virtual ~stream_base()	{}
		virtual bstreamsize		size() const = 0;
		virtual bstreamsize		seek(bstreamsize offset) = 0;
		virtual bstreamsize		position() = 0;
		virtual bstreamsize		read(char* dst, bstreamsize size) = 0;
		virtual bstreamsize		write(const char* src, bstreamsize size) = 0;
		/*
		* other stream
		*/
		virtual bstreamsize swrite(const ptr& src, bstreamsize size);
		/*
		* setting
		*/
		void enable_big_endian(bool be_or_le);
		void set_encoding(EENCODING encoding);
		bool is_big_endian() const;
		EENCODING encoding() const;
		/*
		* read zero terminated string
		*/
		bstreamsize	read_sz(char* dst, bstreamsize max_size);
		/*
		* write zero terminated string
		*/
		bstreamsize	write_sz(const char* dst);
		bstreamsize read_uleb128(t_uint64& out_);
		bstreamsize read_sleb128(t_int64& out_);
		/*
		* big-endian write
		*/
		bstreamsize be_write(t_int8 val);
		bstreamsize be_write(t_int16 val);
		bstreamsize be_write(t_int32 val);
		bstreamsize be_write(t_int64 val);
		bstreamsize be_write(t_uint8 val);
		bstreamsize be_write(t_uint16 val);
		bstreamsize be_write(t_uint32 val);
		bstreamsize be_write(t_uint64 val);
		bstreamsize be_write(t_float32 val);
		bstreamsize be_write(t_float64 val);
		/*
		* big-endian read
		*/
		bstreamsize be_read(t_int8& val);
		bstreamsize be_read(t_int16& val);
		bstreamsize be_read(t_int32& val);
		bstreamsize be_read(t_int64& val);
		bstreamsize be_read(t_uint8& val);
		bstreamsize be_read(t_uint16& val);
		bstreamsize be_read(t_uint32& val);
		bstreamsize be_read(t_uint64& val);
		bstreamsize be_read(t_float32& val);
		bstreamsize be_read(t_float64& val);

		/*
		* little-endian write
		*/
		bstreamsize le_write(t_int8 val);
		bstreamsize le_write(t_int16 val);
		bstreamsize le_write(t_int32 val);
		bstreamsize le_write(t_int64 val);
		bstreamsize le_write(t_uint8 val);
		bstreamsize le_write(t_uint16 val);
		bstreamsize le_write(t_uint32 val);
		bstreamsize le_write(t_uint64 val);
		/*
		* little-endian read
		*/
		bstreamsize le_read(t_int8& val);
		bstreamsize le_read(t_int16& val);
		bstreamsize le_read(t_int32& val);
		bstreamsize le_read(t_int64& val);
		bstreamsize le_read(t_uint8& val);
		bstreamsize le_read(t_uint16& val);
		bstreamsize le_read(t_uint32& val);
		bstreamsize le_read(t_uint64& val);
		/*
		* write
		*/
		virtual bstreamsize t_write(t_int8 val);
		virtual bstreamsize t_write(t_int16 val);
		virtual bstreamsize t_write(t_int32 val);
		virtual bstreamsize t_write(t_int64 val);
		virtual bstreamsize t_write(t_uint8 val);
		virtual bstreamsize t_write(t_uint16 val);
		virtual bstreamsize t_write(t_uint32 val);
		virtual bstreamsize t_write(t_uint64 val);


		bstreamsize t_write8i(char val);
		bstreamsize t_write16i(short val);
		bstreamsize t_write32i(int val);
		bstreamsize t_write64i(long long int val);
		bstreamsize t_write8u(unsigned char val);
		bstreamsize t_write16u(unsigned short val);
		bstreamsize t_write32u(unsigned int val);
		bstreamsize t_write64u(unsigned long long int val);
		/*
		* read
		*/
		virtual bstreamsize t_read(t_int8& val);
		virtual bstreamsize t_read(t_int16& val);
		virtual bstreamsize t_read(t_int32& val);
		virtual bstreamsize t_read(t_int64& val);
		virtual bstreamsize t_read(t_uint8& val);
		virtual bstreamsize t_read(t_uint16& val);
		virtual bstreamsize t_read(t_uint32& val);
		virtual bstreamsize t_read(t_uint64& val);

		bstreamsize t_read8i(char& val);
		bstreamsize t_read16i(short& val);
		bstreamsize t_read32i(int& val);
		bstreamsize t_read64i(long long int& val);
		bstreamsize t_read8u(unsigned char& val);
		bstreamsize t_read16u(unsigned short& val);
		bstreamsize t_read32u(unsigned int& val);
		bstreamsize t_read64u(unsigned long long int& val);
	};
}

#endif //__BIN_SON_BASE_STREAM_H__