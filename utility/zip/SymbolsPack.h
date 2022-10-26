#ifndef __SYMBOLS_PACK_H__
#define __SYMBOLS_PACK_H__

#include <map>
#include <string>
#include <vector>
#include <io/stream_base.h>
#include <io/source_of_stream.h>
#include <shared_ptr_alias.hpp>

/*
//big-endian encoding

0. magic (unsigned int) 0xF1C3545F
1. number of records (unsigned int)
2. offset of zipped strings (unsigned int)
3. size of zipped strings (unsigned int)

4. offset of first record (unsigned int)
5. number of nlist structures in first record (unsigned int)
6. UUID of first record (16 byte)

7. offset of second record (unsigned int)
8. number of nlist structures in second record (unsigned int)
9. UUID of second record (16 byte)

10. offset of N record (unsigned int)
11. number of nlist structures in N record (unsigned int)
12. UUID of N record (16 byte)


*********SECTIONS*******
header
first record of nlist array
second record of nlist array
N record of nlist array
zipped strings
*/



struct MachOPackageHeader
{
	static const unsigned int kMAGIC = 0xF1C3545F;

	MachOPackageHeader()
		:magic(0),
		nrec(0),
		zsoff(0),
		zssize(0)
	{
		return;
	}

	MachOPackageHeader(unsigned int mg, unsigned int number_rec, unsigned int stroff, unsigned int strsize)
		:magic(mg),
		nrec(number_rec),
		zsoff(stroff),
		zssize(strsize)
	{
		return;
	}

	unsigned int magic;
	unsigned int nrec;
	unsigned int zsoff;
	unsigned int zssize;

	enum { size = 4 * 4 };

	io::bstreamsize write(io::stream_base::ptr& stream) const
	{
		io::bstreamsize oret = 0;
		oret += stream->t_write32u(magic);
		oret += stream->t_write32u(nrec);
		oret += stream->t_write32u(zsoff);
		oret += stream->t_write32u(zssize);
		return oret;
	}
	io::bstreamsize read(io::stream_base::ptr& stream)
	{
		io::bstreamsize oret = 0;
		oret += stream->t_read32u(magic);
		oret += stream->t_read32u(nrec);
		oret += stream->t_read32u(zsoff);
		oret += stream->t_read32u(zssize);
		return oret;
	}
};
struct MachODescription
{
	MachODescription()
		:offset(0),
		zsize(0),
		type(0)
	{
		memset(UUID, 0, sizeof(UUID));
	}
	MachODescription(unsigned int off, unsigned int zs, const char uuid[], unsigned int type)
		:offset(off),
		zsize(zs),
		type(type)
	{
		memset(UUID, 0, sizeof(UUID));
		if (uuid) {
			memcpy(UUID, uuid, sizeof(UUID));
		}
	}
	unsigned int offset;
	unsigned int zsize;
	unsigned int type;//x64 or x32 low bit
	char UUID[16];

	enum { size = (4 * 3) + 16 };
	enum 
	{
		type_none = 0,
		type_x64 = 1
	};

	io::bstreamsize write(io::stream_base::ptr& stream) const
	{
		io::bstreamsize oret = 0;
		oret += stream->t_write32u(offset);
		oret += stream->t_write32u(zsize);
		oret += stream->t_write32u(type);
		oret += stream->write(UUID, sizeof(UUID));
		return oret;
	}
	io::bstreamsize read(io::stream_base::ptr& stream)
	{
		io::bstreamsize oret = 0;
		oret += stream->t_read32u(offset);
		oret += stream->t_read32u(zsize);
		oret += stream->t_read32u(type);
		oret += stream->read(UUID, sizeof(UUID));
		return oret;
	}
};
struct SymbolNode32
{
	SymbolNode32()
		:stridx(0),
		value(0)
	{
	}
	SymbolNode32(unsigned int si, unsigned int addr)
		:stridx(si),
		value(addr)
	{
	}

	unsigned int stridx;
	unsigned int value;

	enum { size = 2 * 4 };
	io::bstreamsize write(io::stream_base::ptr& stream) const
	{
		return write(*stream);
	}
	io::bstreamsize write(io::stream_base& stream) const
	{
		io::bstreamsize oret = 0;
		oret += stream.t_write32u(stridx);
		oret += stream.t_write32u(value);
		return oret;
	}
	io::bstreamsize read(io::stream_base::ptr& stream)
	{
		return read(*stream);
	}
	io::bstreamsize read(io::stream_base& stream)
	{
		io::bstreamsize oret = 0;
		oret += stream.t_read32u(stridx);
		oret += stream.t_read32u(value);
		return oret;
	}
};
struct SymbolNode64
{
	SymbolNode64()
		:stridx(0),
		value(0)
	{
		return;
	}
	SymbolNode64(const SymbolNode32& other)
		:stridx(other.stridx),
		value(other.value)
	{
		return;
	}
	SymbolNode64(unsigned int si, unsigned long long int addr)
		:stridx(si),
		value(addr)
	{
		return;
	}

	unsigned int stridx;
	unsigned long long int value;

	enum { size = 4 + 8 };

	io::bstreamsize write(io::stream_base::ptr& stream) const
	{
		return write(*stream);
	}
	io::bstreamsize write(io::stream_base& stream) const
	{
		io::bstreamsize oret = 0;
		oret += stream.t_write32u(stridx);
		oret += stream.t_write64u(value);
		return oret;
	}
	io::bstreamsize read(io::stream_base::ptr& stream)
	{
		return read(*stream);
	}
	io::bstreamsize read(io::stream_base& stream)
	{
		io::bstreamsize oret = 0;
		oret += stream.t_read32u(stridx);
		oret += stream.t_read64u(value);
		return oret;
	}

	bool operator < (const SymbolNode64& other) const
	{
		return (value < other.value);
	}
};

class SymbolPackage
{
public:
	typedef tools::shared_ptr<SymbolPackage> Ptr;
	typedef std::map<std::string, MachODescription> RecordTable;
	typedef std::vector<char> StringsArr;
public:
	SymbolPackage(const io::source_of_stream& pack);
	std::string getSymbol(const std::string& uuid, unsigned long long int address);
private:
	RecordTable& getRecords(io::stream_base::ptr& stream);
	StringsArr& getStrings(io::stream_base::ptr& stream);
private:
	io::source_of_stream	m_source;
	StringsArr				m_str;
	RecordTable				m_offsets;
};


#endif //__SYMBOLS_PACK_H__