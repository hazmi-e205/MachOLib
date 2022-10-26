#include "CDWARFRangeTable.h"


namespace dwarf
{
	bool CDWARFRangeTable::parse(t_uint8 addressSize, io::stream_base::ptr stream)
	{
		bool ret = false;
		m_table.clear();
		while(stream->position() < stream->size())
		{
			const RangeSet* set = extract(addressSize, stream);
			if(set == NULL)
			{
				assert(false && "Error on extart range set");
				break;
			}
		}
		std::sort(m_table.begin(), m_table.end());
		return ret;
	}
	const CDWARFRangeTable::RangeSet* CDWARFRangeTable::getRangeBy(t_uint64 offset) const
	{
		const RangeSet* ret = NULL;
		RangeTable::const_iterator i = std::lower_bound(m_table.begin(), m_table.end(), offset, RangeSetComparator());
		if(i != m_table.end())
		{
			if((*i).Offset == offset)
			{
				ret = &(*i);
			}
		}
		return ret;
	}
	const CDWARFRangeTable::RangeSet* CDWARFRangeTable::extractRangeSetBy(t_uint8 addressSize, io::stream_base::ptr stream, t_uint64 offset)
	{
		const RangeSet* ret = getRangeBy(offset);
		if(ret == NULL)
		{
			stream->seek(offset);
			ret = extract(addressSize, stream);
		}
		return ret;
	}
	const CDWARFRangeTable::RangeSet* CDWARFRangeTable::extract(t_uint8 addressSize, io::stream_base::ptr stream)
	{
		const RangeSet* ret = NULL;
		switch(addressSize)
		{
		case 4:
			{
				RangeSet curset(stream->position());
				while(stream->position() < stream->size())
				{
					t_uint32 begin = 0, end = 0; 
					if(stream->t_read(begin) != sizeof(begin)){ ret = false; break; }
					if(stream->t_read(end) != sizeof(end)){ ret = false; break; }
					curset.Collection.push_back(CDWARFRangeListEntry(begin, end));
					if(curset.Collection.back().isEndOfListEntry())
					{
						curset.Collection.pop_back();
						break;
					}
				}
				RangeTable::const_iterator i = std::lower_bound(m_table.begin(), m_table.end(), curset);
				ret = &(*m_table.insert(i, curset));
				break;
			}
		case 8:
			{
				RangeSet curset(stream->position());
				while(stream->position() < stream->size())
				{
					t_uint64 begin = 0, end = 0; 
					if(stream->t_read(begin) != sizeof(begin)){ ret = false; break; }
					if(stream->t_read(end) != sizeof(end)){ ret = false; break; }
					curset.Collection.push_back(CDWARFRangeListEntry(begin, end));
					if(curset.Collection.back().isEndOfListEntry())
					{
						curset.Collection.pop_back();
						break;
					}
				}
				RangeTable::const_iterator i = std::lower_bound(m_table.begin(), m_table.end(), curset);
				ret = &(*m_table.insert(i, curset));
				break;
			}
		default:
			assert(false && "wrong address size!!!!");
			break;
		}
		return ret;
	}
}