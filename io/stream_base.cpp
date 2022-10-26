#include "stream_base.h"

#define RC_C(x) reinterpret_cast<const char*>(x)
#define RC(x) reinterpret_cast<char*>(x)

namespace io
{
	void stream_base::enable_big_endian(bool be_or_le)
	{
		set_encoding(be_or_le?(EENCODING_BIG_ENDIAN):(EENCODING_LITTLE_ENDIAN));
		return;
	}
	void stream_base::set_encoding(EENCODING encoding)
	{
		m_be = encoding;
		return;
	}
	bool stream_base::is_big_endian() const
	{
		return m_be == EENCODING_BIG_ENDIAN;
	}
	EENCODING stream_base::encoding() const
	{
		return m_be;
	}
	/*
	* other stream
	*/
	bstreamsize stream_base::swrite(const stream_base::ptr& src, bstreamsize size)
	{
		enum { kSIZE = 1024 };
		char temporary[kSIZE];
		bstreamsize writenall = 0;
		while((size - writenall) > 0) {
			bstreamsize readen = src->read(temporary, std::min((size - writenall), (bstreamsize)kSIZE));
			if(readen > 0) {
				bstreamsize writen = write(temporary, readen);
				if(writen > 0) { 
					writenall += writen;
				}
				else {
					return 0ULL;
				}
			}
			else {
				return 0ULL;
			}
		}
		return writenall;
	}
	bstreamsize	stream_base::read_sz(char* dst,bstreamsize max_size)
	{
		bstreamsize i = 0;
		t_int8 cur = '\0';
		do
		{
			if(be_read(cur) == 0ULL){ break; }
			dst[i] = cur;
			++i;
		} while(i < max_size && cur != '\0');
		return i;
	}
	bstreamsize	stream_base::write_sz(const char* src)
	{
		bstreamsize size = strlen(src);
		return write(src, size + 1);
	}
	bstreamsize stream_base::read_uleb128(t_uint64& out_)
	{
		bstreamsize chk = 0;
		unsigned shift = 0;
		t_uint8 byte = 0;
		out_ = 0;
		while (position() < size()) {
			if(t_read(byte) > 0){ ++chk; }
			out_ |= (byte & 0x7fU) << shift;
			shift += 7;
			if ((byte & 0x80) == 0)
				break;
		}
		return chk;
	}
	bstreamsize stream_base::read_sleb128(t_int64& out_)
	{
		bstreamsize chk = 0;
		unsigned int shift = 0;
		t_uint8 byte = 0;
		out_ = 0;

		while(position() < size()) {
			if(t_read(byte) > 0) { ++chk; }
			out_ |= (byte & 0x7fU) << shift;
			shift += 7;
			if ((byte & 0x80) == 0)
				break;
		}
		if (shift < 64 && (byte & 0x40)) {
			out_ |= -(1 << shift);
		}
		return chk;
	}
	bstreamsize stream_base::be_write(t_int8 val)
	{
		return write(reinterpret_cast<const char*>(&val),sizeof(t_int8));
	}
	bstreamsize stream_base::be_write(t_int16 val)
	{
		t_int16 be = be_int16::encode(val);
		return write(reinterpret_cast<const char*>(&be),sizeof(t_int16));
	}
	bstreamsize stream_base::be_write(t_int32 val)
	{
		t_int32 be = be_int32::encode(val);
		return write(reinterpret_cast<const char*>(&be),sizeof(t_int32));
	}
	bstreamsize stream_base::be_write(t_int64 val)
	{
		t_int64 be = be_int64::encode(val);
		return write(reinterpret_cast<const char*>(&be),sizeof(t_int64));
	}
	bstreamsize stream_base::be_write(t_uint8 val)
	{
		return write(reinterpret_cast<const char*>(&val),sizeof(t_uint8));
	}
	bstreamsize stream_base::be_write(t_uint16 val)
	{
		t_uint16 be = be_uint16::encode(val);
		return write(reinterpret_cast<const char*>(&be),sizeof(t_uint16));
	}
	bstreamsize stream_base::be_write(t_uint32 val)
	{
		t_uint32 be = be_uint32::encode(val);
		return write(reinterpret_cast<const char*>(&be),sizeof(t_uint32));
	}
	bstreamsize stream_base::be_write(t_uint64 val)
	{
		t_uint64 be = be_uint64::encode(val);
		return write(reinterpret_cast<const char*>(&be),sizeof(t_uint64));
	}
	bstreamsize stream_base::be_write(t_float32 val)
	{
		union
		{
			t_float32 f;
			t_uint32 be;
		};
		f = val;
		be = be_uint32::encode(be);
		return write(reinterpret_cast<const char*>(&be),sizeof(t_uint32));
	}
	bstreamsize stream_base::be_write(t_float64 val)
	{
		union
		{
			t_float64 f;
			t_uint64 be;
		};
		f = val;
		be = be_uint64::encode(be);
		return write(reinterpret_cast<const char*>(&be),sizeof(t_uint64));
	}

	bstreamsize stream_base::be_read(t_int8& val)
	{
		return read(reinterpret_cast<char*>(&val),sizeof(t_int8));
	}
	bstreamsize stream_base::be_read(t_int16& val)
	{
		t_int16 tmp;
		bstreamsize size = read(reinterpret_cast<char*>(&tmp),sizeof(t_int16));
		if(size == sizeof(t_int16)) {
			val = be_int16::decode(tmp);
		}
		return size;
	}
	bstreamsize stream_base::be_read(t_int32& val)
	{
		t_int32 tmp;
		bstreamsize size = read(reinterpret_cast<char*>(&tmp),sizeof(t_int32));
		if(size == sizeof(t_int32)) {
			val = be_int32::decode(tmp);
		}
		return size;
	}
	bstreamsize stream_base::be_read(t_int64& val)
	{
		t_int64 tmp;
		bstreamsize size = read(reinterpret_cast<char*>(&tmp),sizeof(t_int64));
		if(size == sizeof(t_int64)) {
			val = be_int64::decode(tmp);
		}
		return size;
	}
	bstreamsize stream_base::be_read(t_uint8& val)
	{
		return read(reinterpret_cast<char*>(&val),sizeof(t_uint8));
	}
	bstreamsize stream_base::be_read(t_uint16& val)
	{
		t_uint16 tmp;
		bstreamsize size = read(reinterpret_cast<char*>(&tmp),sizeof(t_uint16));
		if(size == sizeof(t_uint16)) {
			val = be_uint16::decode(tmp);
		}
		return size;
	}
	bstreamsize stream_base::be_read(t_uint32& val)
	{
		t_uint32 tmp;
		bstreamsize size = read(reinterpret_cast<char*>(&tmp),sizeof(t_uint32));
		if(size == sizeof(t_uint32)) {
			val = be_uint32::decode(tmp);
		}
		return size;
	}
	bstreamsize stream_base::be_read(t_uint64& val)
	{
		t_uint64 tmp;
		bstreamsize size = read(reinterpret_cast<char*>(&tmp),sizeof(t_uint64));
		if(size == sizeof(t_uint64)) {
			val = be_uint64::decode(tmp);
		}
		return size;
	}
	bstreamsize stream_base::be_read(t_float32& val)
	{
		typedef union { t_uint32 u32; t_float32 f32; } T;
		T& tmp = reinterpret_cast<T&>(val);
		bstreamsize size = read(reinterpret_cast<char*>(&tmp.u32),sizeof(t_uint32));
		if(size == sizeof(t_uint32)) {
			tmp.u32 = be_uint32::decode(tmp.u32);
		}
		return size;
	}
	bstreamsize stream_base::be_read(t_float64& val)
	{
		typedef union { t_uint64 u64; t_float64 f64; } T;
		T& tmp = reinterpret_cast<T&>(val);
		bstreamsize size = read(reinterpret_cast<char*>(&tmp.u64),sizeof(t_uint64));
		if(size == sizeof(t_uint64)) {
			tmp.u64 = be_uint64::decode(tmp.u64);
		}
		return size;
	}


	/*little endian write INT8*/
	bstreamsize stream_base::le_write(t_int8 val)
	{
		return write(reinterpret_cast<const char*>(&val),sizeof(t_int8));
	}
	/*little endian write INT16*/
	bstreamsize stream_base::le_write(t_int16 val)
	{
		t_int16 le = le_int16::encode(val);
		return write(reinterpret_cast<const char*>(&le),sizeof(t_int16));
	}
	/*little endian write INT32*/
	bstreamsize stream_base::le_write(t_int32 val)
	{
		t_int32 le = le_int32::encode(val);
		return write(reinterpret_cast<const char*>(&le),sizeof(t_int32));
	}
	/*little endian write INT64*/
	bstreamsize stream_base::le_write(t_int64 val)
	{
		t_int64 le = le_int64::encode(val);
		return write(reinterpret_cast<const char*>(&le),sizeof(t_int64));
	}
	/*little endian write UINT8*/
	bstreamsize stream_base::le_write(t_uint8 val)
	{
		return write(reinterpret_cast<const char*>(&val),sizeof(t_uint8));
	}
	/*little endian write UINT16*/
	bstreamsize stream_base::le_write(t_uint16 val)
	{
		t_uint16 le = le_uint16::encode(val);
		return write(reinterpret_cast<const char*>(&le),sizeof(t_uint16));
	}
	/*little endian write UINT32*/
	bstreamsize stream_base::le_write(t_uint32 val)
	{
		t_uint32 le = le_uint32::encode(val);
		return write(reinterpret_cast<const char*>(&le),sizeof(t_uint32));
	}
	/*little endian write UINT64*/
	bstreamsize stream_base::le_write(t_uint64 val)
	{
		t_uint64 le = le_uint64::encode(val);
		return write(reinterpret_cast<const char*>(&le),sizeof(t_uint64));
	}
	/*little endian read INT8*/
	bstreamsize stream_base::le_read(t_int8& val)
	{
		return read(reinterpret_cast<char*>(&val),sizeof(t_int8));
	}
	/*little endian read INT16*/
	bstreamsize stream_base::le_read(t_int16& val)
	{
		t_int16 tmp;
		bstreamsize size = read(reinterpret_cast<char*>(&tmp),sizeof(t_int16));
		if(size == sizeof(t_int16)) {
			val = le_int16::decode(tmp);
		}
		return size;
	}
	/*little endian read INT32*/
	bstreamsize stream_base::le_read(t_int32& val)
	{
		t_int32 tmp;
		bstreamsize size = read(reinterpret_cast<char*>(&tmp),sizeof(t_int32));
		if(size == sizeof(t_int32)) {
			val = le_int32::decode(tmp);
		}
		return size;
	}
	/*little endian read INT64*/
	bstreamsize stream_base::le_read(t_int64& val)
	{
		t_int64 tmp;
		bstreamsize size = read(reinterpret_cast<char*>(&tmp),sizeof(t_int64));
		if(size == sizeof(t_int64)) {
			val = le_int64::decode(tmp);
		}
		return size;
	}
	/*little endian read UINT8*/
	bstreamsize stream_base::le_read(t_uint8& val)
	{
		return read(reinterpret_cast<char*>(&val),sizeof(t_uint8));
	}
	/*little endian read UINT16*/
	bstreamsize stream_base::le_read(t_uint16& val)
	{
		t_uint16 tmp;
		bstreamsize size = read(reinterpret_cast<char*>(&tmp),sizeof(t_uint16));
		if(size == sizeof(t_uint16)) {
			val = le_uint16::decode(tmp);
		}
		return size;
	}
	/*little endian read UINT32*/
	bstreamsize stream_base::le_read(t_uint32& val)
	{
		t_uint32 tmp;
		bstreamsize size = read(reinterpret_cast<char*>(&tmp),sizeof(t_uint32));
		if(size == sizeof(t_uint32)) {
			val = le_uint32::decode(tmp);
		}
		return size;
	}
	/*little endian read UINT64*/
	bstreamsize stream_base::le_read(t_uint64& val)
	{
		t_uint64 tmp;
		bstreamsize size = read(reinterpret_cast<char*>(&tmp),sizeof(t_uint64));
		if(size == sizeof(t_uint64)) {
			val = le_uint64::decode(tmp);
		}
		return size;
	}
	/*
	* write
	*/
	bstreamsize stream_base::t_write(t_int8 val){ return is_big_endian()?(be_write(val)):(le_write(val)); }
	bstreamsize stream_base::t_write(t_int16 val){ return is_big_endian()?(be_write(val)):(le_write(val)); }
	bstreamsize stream_base::t_write(t_int32 val){ return is_big_endian()?(be_write(val)):(le_write(val)); }
	bstreamsize stream_base::t_write(t_int64 val){ return is_big_endian()?(be_write(val)):(le_write(val)); }
	bstreamsize stream_base::t_write(t_uint8 val){ return is_big_endian()?(be_write(val)):(le_write(val)); }
	bstreamsize stream_base::t_write(t_uint16 val){ return is_big_endian()?(be_write(val)):(le_write(val)); }
	bstreamsize stream_base::t_write(t_uint32 val){ return is_big_endian()?(be_write(val)):(le_write(val)); }
	bstreamsize stream_base::t_write(t_uint64 val){ return is_big_endian()?(be_write(val)):(le_write(val)); }
	
	bstreamsize stream_base::t_write8i(char val){ return t_write(static_cast<t_int8>(val)); }
	bstreamsize stream_base::t_write16i(short val){ return t_write(static_cast<t_int16>(val)); }
	bstreamsize stream_base::t_write32i(int val){ return t_write(static_cast<t_int32>(val)); }
	bstreamsize stream_base::t_write64i(long long int val){ return t_write(static_cast<t_int64>(val)); }
	bstreamsize stream_base::t_write8u(unsigned char val){ return t_write(static_cast<t_uint8>(val)); }
	bstreamsize stream_base::t_write16u(unsigned short val){ return t_write(static_cast<t_uint16>(val)); }
	bstreamsize stream_base::t_write32u(unsigned int val){ return t_write(static_cast<t_uint32>(val)); }
	bstreamsize stream_base::t_write64u(unsigned long long int val){ return t_write(static_cast<t_uint64>(val)); }
	/*
	* read
	*/
	bstreamsize stream_base::t_read(t_int8& val){ return is_big_endian()?(be_read(val)):(le_read(val)); }
	bstreamsize stream_base::t_read(t_int16& val){ return is_big_endian()?(be_read(val)):(le_read(val)); }
	bstreamsize stream_base::t_read(t_int32& val){ return is_big_endian()?(be_read(val)):(le_read(val)); }
	bstreamsize stream_base::t_read(t_int64& val){ return is_big_endian()?(be_read(val)):(le_read(val)); }
	bstreamsize stream_base::t_read(t_uint8& val){ return is_big_endian()?(be_read(val)):(le_read(val)); }
	bstreamsize stream_base::t_read(t_uint16& val){ return is_big_endian()?(be_read(val)):(le_read(val)); }
	bstreamsize stream_base::t_read(t_uint32& val){ return is_big_endian()?(be_read(val)):(le_read(val)); }
	bstreamsize stream_base::t_read(t_uint64& val){ return is_big_endian()?(be_read(val)):(le_read(val)); }

	bstreamsize stream_base::t_read8i(char& val) { return t_read(reinterpret_cast<t_int8&>(val)); }
	bstreamsize stream_base::t_read16i(short& val) { return t_read(reinterpret_cast<t_int16&>(val)); }
	bstreamsize stream_base::t_read32i(int& val) { return t_read(reinterpret_cast<t_int32&>(val)); }
	bstreamsize stream_base::t_read64i(long long int& val) { return t_read(reinterpret_cast<t_int64&>(val)); }
	bstreamsize stream_base::t_read8u(unsigned char& val) { return t_read(reinterpret_cast<t_uint8&>(val)); }
	bstreamsize stream_base::t_read16u(unsigned short& val) { return t_read(reinterpret_cast<t_uint16&>(val)); }
	bstreamsize stream_base::t_read32u(unsigned int& val) { return t_read(reinterpret_cast<t_uint32&>(val)); }
	bstreamsize stream_base::t_read64u(unsigned long long int& val) { return t_read(reinterpret_cast<t_uint64&>(val)); }
}