#include "SymbolsPack.h"
#include <io/stream_memory.h>
#include <macho/CMachO.h>
#include <macho/UUID.h>
#include "ztools.h"
#include <vector>

SymbolPackage::SymbolPackage(const io::source_of_stream& pack)
	:m_source(pack),
	m_str(),
	m_offsets()
{
	return;
}
std::string SymbolPackage::getSymbol(const std::string& uuid, unsigned long long int address)
{
	std::string osymbol;
	io::stream_base::ptr strm = m_source.open();
	if (strm.get())
	{
		MachOPackageHeader header;
		header.read(strm);
		
		RecordTable& records = getRecords(strm);
		StringsArr& strarr = getStrings(strm);

		RecordTable::iterator i = records.find(uuid);
		if (i != records.end())
		{
			strm->seek((*i).second.offset);
			std::vector<char> znodes;
			znodes.resize((*i).second.zsize);
			strm->read(&znodes[0], znodes.size());
			std::vector<char> nodes = uncompressz(znodes);
			if (!nodes.empty())
			{
				std::vector<SymbolNode64> nodestable;
				{
					io::stream_memory memory(&nodes[0], nodes.size());
					if (((*i).second.type & MachODescription::type_x64) != 0)
					{
						int count = nodes.size() / SymbolNode64::size;
						for (int i = 0; i < count; ++i)
						{
							SymbolNode64 node;
							node.read(memory);
							nodestable.push_back(node);
						}
					}
					else
					{
						int count = nodes.size() / SymbolNode32::size;
						for (int i = 0; i < count; ++i)
						{
							SymbolNode32 node;
							node.read(memory);
							nodestable.push_back(node);
						}
					}
				}
				std::sort(nodestable.begin(), nodestable.end());
				std::vector<SymbolNode64>::iterator it = std::upper_bound(nodestable.begin(), nodestable.end(), SymbolNode64(0, address));
				if (it != nodestable.end())
				{
					unsigned int stridx = (it != nodestable.begin()) ? (*(it - 1)).stridx : (*it).stridx;
					if (stridx > 0 && stridx < strarr.size())
					{
						osymbol = std::string(&strarr[stridx]);
					}
				}
				else
				{
					TRACE("[SymbolPackage] Lib %s fount but address not found!\n", uuid.c_str());
				}
			}

		}
		else
		{
			TRACE("[SymbolPackage] Lib %s not found!\n", uuid.c_str());
		}
	}
	return osymbol;
}
SymbolPackage::RecordTable& SymbolPackage::getRecords(io::stream_base::ptr& stream)
{
	if (m_offsets.empty())
	{
		if (stream.get())
		{
			MachOPackageHeader header;
			stream->seek(0);
			header.read(stream);
			for (unsigned int i = 0; i < header.nrec; ++i)
			{
				MachODescription desc;
				desc.read(stream);
				std::string uuid = macho::UUIDMem2Str(desc.UUID);
				m_offsets[uuid] = desc;
			}
		}
	}
	return m_offsets;
}
SymbolPackage::StringsArr& SymbolPackage::getStrings(io::stream_base::ptr& stream)
{
	if (m_str.empty())
	{
		if (stream.get())
		{
			MachOPackageHeader header;
			stream->seek(0);
			header.read(stream);
			stream->seek(header.zsoff);
			std::vector<char> zstr;
			zstr.resize(header.zssize);
			stream->read(&zstr[0], zstr.size());
			m_str = uncompressz(zstr);
		}
	}
	return m_str;
}