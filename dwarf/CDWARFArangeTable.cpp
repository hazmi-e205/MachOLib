#include "CDWARFArangeTable.h"

namespace dwarf
{
	/*
	* CDWARFArangeTable
	*/
	CDWARFArangeTable::CDWARFArangeTable()
		:m_ranges()
	{
		return;
	}
	bool CDWARFArangeTable::parse_set(io::stream_base::ptr stream)
	{
		bool ret = false;
		size_t counter = m_ranges.size();
		CDWARFArangesHeader header;
		t_uint64 start = stream->position();
		stream->t_read(header.length);
		t_uint64 start_of_length = stream->position();
		stream->t_read(header.version);
		stream->t_read(header.cu_offset);
		stream->t_read(header.addr_size);
		stream->t_read(header.seg_size);
		if((start_of_length + header.length) <= stream->size()
			&& (header.addr_size == 4 || header.addr_size == 8))
		{
			const t_uint64 header_size = stream->position() - start;
			const t_uint64 tuple_size = (header.addr_size<<1);
			t_uint64 first_tuple_offset = 0;

			while (first_tuple_offset < header_size) {
				first_tuple_offset += tuple_size;
			}

			stream->seek(start + first_tuple_offset);

			ret = true;
			switch(header.addr_size)
			{
			case 4:
				{
					while(stream->position() < stream->size() && ret)
					{
						t_uint32 address = 0,length = 0;
						if(stream->t_read(address) != sizeof(address)) { 
							ret = false; 
						}
						if(stream->t_read(length) != sizeof(length)) { 
							ret = false; 
						}
						if((address || length) && ret)
						{
							m_ranges.push_back(CDWARFRange(address,(address + length), header.cu_offset));
						}
						else
						{
							break;
						}
					}
					break;
				}
			case 8:
				{
					while(stream->position() < stream->size() && ret)
					{
						t_uint64 address = 0,length = 0;
						if(stream->t_read(address) != sizeof(address)){ 
							ret = false; 
						}
						if(stream->t_read(length) != sizeof(length)){ 
							ret = false; 
						}
						if((address || length) && ret)
						{
							m_ranges.push_back(CDWARFRange(address,(address + length), header.cu_offset));
						}
						else
						{
							break;
						}
					}
					break;
				}
			default:
				assert(false && "bad address size of arange set");
				ret = false;
				break;
			}
		}
		else
		{
			assert(false && "invalid arange set");
		}
		return ret && (m_ranges.size() - counter) > 0;
	}
	bool CDWARFArangeTable::parse(io::stream_base::ptr stream)
	{
		bool ret = false;
		m_ranges.clear();
		while(stream->position() < stream->size())
		{
			if(!parse_set(stream))
			{
				ret = false;
				break;
			}
			else
			{
				ret = true;
			}
		}
		std::sort(m_ranges.begin(), m_ranges.end());
		return ret;
	}
	const CDWARFArangeTable::CDWARFRange* CDWARFArangeTable::getRangesBy(t_uint64 address) const
	{
		const CDWARFRange* range = NULL;
		RangeTable::const_iterator it = std::lower_bound(m_ranges.begin(), m_ranges.end(), address, RangeComparator());
		if(it != m_ranges.end())
		{
			if(address >= (*it).begin && address < (*it).end)
			{
				range = &(*it);
			}
			else
			{
				if(it != m_ranges.begin()) {
					--it;
				}
				if(address >= (*it).begin && address < (*it).end)
				{
					range = &(*it);
				}
			}
		}
		return range;
	}
}