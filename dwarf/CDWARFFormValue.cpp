#include "CDWARFFormValue.h"
#include <assert.h>

namespace dwarf
{

	CDWARFFormValue::CDWARFFormValue()
		:m_storage(),
		m_type(DW_FORM_Invalid)
	{
		return;
	}
	CDWARFFormValue::CDWARFFormValue(DW_FORM form)
		:m_storage(),
		m_type(form)
	{
		eValueType t = ConvertFormToType(form,-1,NULL);
		if(install(t) == eValueTypeInvalid)
		{
			//assert(false);
		}
		return;
	}
	CDWARFFormValue::CDWARFFormValue(const CDWARFFormValue& other)
		:m_storage(),
		m_type(other.m_type)
	{
		if(m_type != DW_FORM_Invalid)
		{
			eValueType t = type();
			if(install(t) == eValueTypeInvalid)
			{
				//assert(false);
			}
			else
			{
				copy(other);
			}
		}
	}
	CDWARFFormValue& CDWARFFormValue::operator = (const CDWARFFormValue& other)
	{
		if(this != &other)
		{
			uninstall();
			m_type = other.m_type;
			eValueType t = type();
			if(t != eValueTypeInvalid && install(t) != eValueTypeInvalid)
			{
				copy(other);
			}
			else
			{
				m_type = DW_FORM_Invalid;
				//assert(false);
			}
		}
		return *this;
	}
	CDWARFFormValue::~CDWARFFormValue()
	{
		if(m_type != DW_FORM_Invalid)
		{
			uninstall();
		}
		return;
	}

	CDWARFFormValue::eValueType CDWARFFormValue::ConvertFormToType(DW_FORM form,t_int32 addr_size,t_int32* osize)
	{
		t_int32 idle;
		t_int32& size = (osize)?*osize:idle;
		enum { UnknownSize = -1 };
		CDWARFFormValue::eValueType type = eValueTypeInvalid;
		switch(form)
		{
		case DW_FORM_indirect:
			//type of this form storing in dwarf section
			size = UnknownSize;
			break;
		case DW_FORM_addr:
			type = eValueTypeUnsigned;
			size = addr_size;
			break;
		case DW_FORM_data1:
			type = eValueTypeUnsigned;
			size = 1;
			break;
		case DW_FORM_data2:
			type = eValueTypeUnsigned;
			size = 2;
			break;
		case DW_FORM_data4:
			type = eValueTypeUnsigned;
			size = 4;
			break;
		case DW_FORM_data8:
			type = eValueTypeUnsigned;
			size = 8;
			break;
        case DW_FORM_flag:
			type = eValueTypeUnsigned;
			size = 1;
			break;
        case DW_FORM_strp:
			type = eValueTypeUnsigned;
			size = 4;
			break;
		case DW_FORM_udata:
			type = eValueTypeUnsigned;
			size = UnknownSize;
			break;
        case DW_FORM_ref_addr:
			type = eValueTypeUnsigned;
			size = addr_size;
			break;
        case DW_FORM_ref1:
			type = eValueTypeUnsigned;
			size = 1;
			break;
        case DW_FORM_ref2:
			type = eValueTypeUnsigned;
			size = 2;
			break;
        case DW_FORM_ref4:
			type = eValueTypeUnsigned;
			size = 4;
			break;
        case DW_FORM_ref8:
			type = eValueTypeUnsigned;
			size = 8;
			break;
        case DW_FORM_ref_udata:
			type = eValueTypeUnsigned;
			size = UnknownSize;
			break;
		case DW_FORM_sec_offset:
			type = eValueTypeUnsigned;
			size = 4;
			break;
        case DW_FORM_flag_present:
			type = eValueTypeUnsigned;
			size = 0;
			break;
        case DW_FORM_ref_sig8:
			type = eValueTypeUnsigned;
			size = 8;
			break;
		case DW_FORM_sdata:
			type = eValueTypeSigned;
			size = UnknownSize;
			break;
		case DW_FORM_string:
			type = eValueTypeCStr;
			size = UnknownSize;
			break;
		case DW_FORM_exprloc:
			type = eValueTypeBlock;
			size = UnknownSize;
			break;
		case DW_FORM_block:
			type = eValueTypeBlock;
			size = UnknownSize;
			break;
        case DW_FORM_block1:
			type = eValueTypeBlock;
			size = UnknownSize;
			break;
		case DW_FORM_block2:
			type = eValueTypeBlock;
			size = UnknownSize;
			break;
        case DW_FORM_block4:
			type = eValueTypeBlock;
			size = UnknownSize;
			break;
		default:
			//assert(false);
			break;
		}
		return type;
	}
	CDWARFFormValue::eValueType CDWARFFormValue::install(eValueType type)
	{
		/*not needed now*/
		//data().clear();
		/*not needed now*/
		switch(type)
		{
		case eValueTypeSigned:
			sval() = 0;
			break;
		case eValueTypeUnsigned:
			uval() = 0;
			break;
		case eValueTypeCStr:
			new (m_storage.un.bf) FormStringData();
			break;
		case eValueTypeBlock:
			uval() = 0;
			break;
		default:
			type = eValueTypeInvalid;
			//assert(false);
			break;
		}
		return type;
	}
	void CDWARFFormValue::copy(const CDWARFFormValue& other)
	{
		/*not needed now*/
		//data() = other.data();
		//m_str = other.m_str;
		/*not needed now*/
		switch(type())
		{
		case eValueTypeSigned:
			sval() = other.sval();
			break;
		case eValueTypeUnsigned:
			uval() = other.uval();
			break;
		case eValueTypeCStr:
			cstr() = other.cstr();
			break;
		case eValueTypeBlock:
			uval() = other.uval();
			break;
		default:
			break;
		}
		return;
	}
	CDWARFFormValue::eValueType CDWARFFormValue::uninstall()
	{
		switch(type())
		{
		case eValueTypeInvalid:
			break;
		case eValueTypeSigned:
			m_storage.un.sval = 0;
			break;
		case eValueTypeUnsigned:
			m_storage.un.uval = 0;
			break;
		case eValueTypeCStr:
			pcstr()->~FormStringData();
			break;
		case eValueTypeBlock:
			m_storage.un.uval = 0;
			/*not needed now*/
			//data().clear();
			/*not needed now*/
			break;
		default:
			break;
		}
		return eValueTypeInvalid;
	}
	t_uint64& CDWARFFormValue::uval()
	{
		//assert(type() == eValueTypeUnsigned || type() == eValueTypeBlock);
		return m_storage.un.uval;
	}
	t_int64& CDWARFFormValue::sval()
	{
		//assert(type() == eValueTypeSigned);
		return m_storage.un.sval;
	}
	CDWARFFormValue::FormStringData* CDWARFFormValue::pcstr()
	{
		//assert(type() == eValueTypeCStr);
		FormStringData* str = reinterpret_cast<FormStringData*>(&m_storage.un.bf[0]);
		return str;
	}
	CDWARFFormValue::FormStringData& CDWARFFormValue::cstr()
	{
		return *pcstr();
	}
	/*not needed now*/
	/*CDWARFFormValue::FormData& CDWARFFormValue::data()
	{
		return m_storage.data;
	}*/
	/*not needed now*/

	const t_uint64& CDWARFFormValue::uval() const
	{
		//assert(type() == eValueTypeUnsigned || type() == eValueTypeBlock);
		return m_storage.un.uval;
	}
	const t_int64& CDWARFFormValue::sval() const
	{
		//assert(type() == eValueTypeSigned);
		return m_storage.un.sval;
	}
	const CDWARFFormValue::FormStringData* CDWARFFormValue::pcstr() const
	{
		//assert(type() == eValueTypeCStr);
		const FormStringData* str = reinterpret_cast<const FormStringData*>(&m_storage.un.bf[0]);
		return str;
	}
	const CDWARFFormValue::FormStringData& CDWARFFormValue::cstr() const
	{
		return *pcstr();
	}
	/*not needed now*/
	/*const CDWARFFormValue::FormData& CDWARFFormValue::data() const
	{
		return m_storage.data;
	}*/
	/*not needed now*/


	CDWARFFormValue::eValueType CDWARFFormValue::type() const
	{
		return ConvertFormToType(static_cast<DW_FORM>(m_type),-1,NULL);
	}
	DW_FORM CDWARFFormValue::form() const
	{
		return static_cast<DW_FORM>(m_type);
	}
	CDWARFFormValue::eFormClass CDWARFFormValue::formClass() const
	{
		switch(m_type)
		{
			case DW_FORM_addr: return eFormClassAddress;
			case DW_FORM_block2: return eFormClassBlock;
			case DW_FORM_block4: return eFormClassBlock;
			case DW_FORM_data2: return eFormClassConstant;
			case DW_FORM_data4: return eFormClassConstant;
			case DW_FORM_data8: return eFormClassConstant;
			case DW_FORM_string: return eFormClassString;
			case DW_FORM_block: return eFormClassBlock;
			case DW_FORM_block1: return eFormClassBlock;
			case DW_FORM_data1: return eFormClassConstant;
			case DW_FORM_flag: return eFormClassFlag;
			case DW_FORM_sdata: return eFormClassConstant;
			case DW_FORM_strp: return eFormClassString;
			case DW_FORM_udata: return eFormClassConstant;
			case DW_FORM_ref_addr: return eFormClassReference;
			case DW_FORM_ref1: return eFormClassReference;
			case DW_FORM_ref2: return eFormClassReference;
			case DW_FORM_ref4: return eFormClassReference;
			case DW_FORM_ref8: return eFormClassReference;
			case DW_FORM_ref_udata: return eFormClassReference;
			case DW_FORM_exprloc: return eFormClassExprloc;
			case DW_FORM_flag_present: return eFormClassFlag;
			case DW_FORM_ref_sig8: return eFormClassReference;
		}
		//assert(false);
		return  eFormClassInvalid;
	}

	bool CDWARFFormValue::parse(t_uint8 addr_size/*4 or 8*/, t_uint8 dwarf_format /*32 or 64*/, t_uint16 dwarf_version, io::stream_base::ptr debug_str,io::stream_base::ptr stream)
	{
		bool ret = true;
		bool indirect = false;
		bool is_block = false;
		/*not needed now*/
		//m_storage.data.clear();
		/*not needed now*/
		// Read the value for the form into value and follow and DW_FORM_indirect instances we run into
		do
		{
			indirect = false;
			switch (m_type)
			{
			case DW_FORM_addr:
				{
					t_uint32 addr32 = 0;
					t_uint64 addr64 = 0;
					if(addr_size == 4) { 
						stream->t_read(addr32);
						m_storage.un.uval = addr32;
					}
					else {
						stream->t_read(addr64);
						m_storage.un.uval = addr64;
					}
					break;
				}
			case DW_FORM_block2:
				{
					t_uint16 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;
					is_block = true;
					break;
				}
			case DW_FORM_block4:
				{
					t_uint32 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;
					is_block = true;
					break;
				}
			case DW_FORM_data2:
				{
					t_uint16 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;
					break;
				}
			case DW_FORM_data4:
				{
					t_uint32 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;
					break;
				}
			case DW_FORM_data8:
				{
					t_uint64 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;
					break;
				}
			case DW_FORM_string:
				{
					char val[8192];
					io::bstreamsize s = stream->read_sz(val,sizeof(8192) - 1);
					if(s >= 0ULL)
					{
						val[s] = 0;
						cstr() = val;
					}
					else
					{
						ret = false;
						//assert(false);
					}
					break;
				}
			case DW_FORM_exprloc:
			case DW_FORM_block:
				{
					stream->read_uleb128(m_storage.un.uval);
					is_block = true;
					break;
				}
			case DW_FORM_block1:
				{
					t_uint8 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val; 
					is_block = true;           
					break;
				}
			case DW_FORM_data1:
				{
					t_uint8 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;         
					break;
				}
			case DW_FORM_flag:
				{
					t_uint8 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;         
					break;
				}
			case DW_FORM_sdata:
				{
					stream->read_sleb128(m_storage.un.sval);
					break;
				}
			case DW_FORM_strp://pointer of section __debug_str
				{
					t_uint32 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;
					break;
				}
			case DW_FORM_udata:
				{
					stream->read_uleb128(m_storage.un.uval);
					break;
				}
			case DW_FORM_ref_addr:
				{
					t_uint32 addr32 = 0;
					t_uint64 addr64 = 0;
					if (dwarf_version > 2)
					{
						if (dwarf_format == 32) {
							stream->t_read(addr32);
							m_storage.un.uval = addr32;
						}
						else {
							stream->t_read(addr64);
							m_storage.un.uval = addr64;
						}
					}
					else
					{
						if (addr_size == 4) {
							stream->t_read(addr32);
							m_storage.un.uval = addr32;
						}
						else {
							stream->t_read(addr64);
							m_storage.un.uval = addr64;
						}
					}
					break;
				}
			case DW_FORM_ref1:
				{
					t_uint8 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;         
					break;
				}
			case DW_FORM_ref2:
				{
					t_uint16 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;         
					break;
				}
			case DW_FORM_ref4:
				{
					t_uint32 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;         
					break;
				}
			case DW_FORM_ref8:
				{
					t_uint64 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;         
					break;
				}
			case DW_FORM_ref_udata:
				{
					stream->read_uleb128(m_storage.un.uval);
					break;
				}
			case DW_FORM_indirect:
				{
					t_uint64 f;
					stream->read_uleb128(f);
					uninstall();
					install(ConvertFormToType(static_cast<DW_FORM>(f),-1,NULL));
					m_type = static_cast<DW_FORM>(f);
					indirect = true;
				}
				break;

			case DW_FORM_sec_offset:
				{
					t_uint32 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;         
					break;
				}
			case DW_FORM_flag_present:
				{
					m_storage.un.uval = 1;         
					break;
				}
			case DW_FORM_ref_sig8:
				{
					t_uint64 val = 0;
					stream->t_read(val);
					m_storage.un.uval = val;         
					break;
				}
			default:
				//assert(false);
				ret = false;
				break;
			}
		} while (indirect && ret);

		if (is_block && ret)
		{
			if((stream->position() + m_storage.un.uval) <= stream->size())
			{
				/*not needed now*/
				/*m_storage.data.resize(static_cast<size_t>(m_storage.un.uval));
				/*not needed now*/
				std::vector<char> temp(static_cast<size_t>(m_storage.un.uval),0);
				//assert(temp.size() == m_storage.un.uval);
				if(!temp.empty()) {
					stream->read(&temp[0],temp.size());
				}
			}
			else
			{
				//assert(false);
				ret = false;
			}
		}
		return ret;
	}
	bool CDWARFFormValue::Skip(DW_FORM form,t_uint8 addr_size/*4 or 8*/, t_uint8 dwarf_format /*32 or 64*/, t_uint16 dwarf_version, io::stream_base::ptr stream)
	{
		bool ret = true;
		bool indirect = false;
		bool is_block = false;
		t_uint64 uval = 0;
		// Read the value for the form into value and follow and DW_FORM_indirect instances we run into
		do
		{
			indirect = false;
			switch (form)
			{
			case DW_FORM_addr:
				{
					stream->seek(stream->position() + addr_size);
					break;
				}
			case DW_FORM_block2:
				{
					t_uint16 val;
					stream->t_read(val);
					uval = val;
					is_block = true;
					break;
				}
			case DW_FORM_block4:
				{
					t_uint32 val;
					stream->t_read(val);
					uval = val;
					is_block = true;
					break;
				}
			case DW_FORM_data2:
				{
					stream->seek(stream->position() + sizeof(t_uint16));
					break;
				}
			case DW_FORM_data4:
				{
					stream->seek(stream->position() + sizeof(t_uint32));
					break;
				}
			case DW_FORM_data8:
				{
					stream->seek(stream->position() + sizeof(t_uint64));
					break;
				}
			case DW_FORM_string:
				{
					char val[8192];
					io::bstreamsize s = stream->read_sz(val,sizeof(8192) - 1);
					if(s < 0ULL)
					{
						ret = false;
						//assert(false);
					}
					break;
				}
			case DW_FORM_exprloc:
			case DW_FORM_block:
				{
					t_uint64 t;
					stream->read_uleb128(t);
					uval = t;
					is_block = true;
					break;
				}
			case DW_FORM_block1:
				{
					t_uint8 val;
					stream->t_read(val);
					uval = val;
					is_block = true;           
					break;
				}
			case DW_FORM_data1:
				{
					stream->seek(stream->position() + sizeof(t_uint8));     
					break;
				}
			case DW_FORM_flag:
				{
					stream->seek(stream->position() + sizeof(t_uint8));
					break;
				}
			case DW_FORM_sdata:
				{
					t_int64 t;
					stream->read_sleb128(t);
					break;
				}
			case DW_FORM_strp://pointer of section __debug_str
				{
					stream->seek(stream->position() + sizeof(t_uint32));   
					break;
				}
			case DW_FORM_udata:
				{
					t_uint64 t;
					stream->read_uleb128(t);
					break;
				}
			case DW_FORM_ref_addr:
				{
					if (dwarf_version > 2)
					{
						stream->seek(stream->position() + dwarf_format/8);
					}
					else
					{
						stream->seek(stream->position() + addr_size);
					}
					break;
				}
			case DW_FORM_ref1:
				{
					stream->seek(stream->position() + sizeof(t_uint8));
					break;
				}
			case DW_FORM_ref2:
				{ 
					stream->seek(stream->position() + sizeof(t_uint16));      
					break;
				}
			case DW_FORM_ref4:
				{
					stream->seek(stream->position() + sizeof(t_uint32));
					break;
				}
			case DW_FORM_ref8:
				{ 
					stream->seek(stream->position() + sizeof(t_uint64));
					break;
				}
			case DW_FORM_ref_udata:
				{
					t_uint64 t;
					stream->read_uleb128(t);
					break;
				}
			case DW_FORM_indirect:
				{
					t_uint64 f;
					stream->read_uleb128(f);
					form = static_cast<DW_FORM>(f);
					indirect = true;
				}
				break;

			case DW_FORM_sec_offset:
				{
					stream->seek(stream->position() + sizeof(t_uint32));
					break;
				}
			case DW_FORM_flag_present:
				{      
					break;
				}
			case DW_FORM_ref_sig8:
				{
					stream->seek(stream->position() + sizeof(t_uint64));
					break;
				}
			default:
				//assert(false);
				ret = false;
				break;
			}
		} while (indirect && ret);

		if (is_block && ret)
		{
			if((stream->position() + uval) <= stream->size())
			{
				stream->seek(stream->position() + uval);
			}
			else
			{
				//assert(false);
				ret = false;
			}
		}
		return ret;
	}
	/*not needed now*/
	/*CDWARFFormValue::FormStringData CDWARFFormValue::asString() const
	{
		return m_str;
	}*/
	/*not needed now*/
	CDWARFFormValue::FormStringData CDWARFFormValue::asString(io::stream_base::ptr debug_str) const
	{
		std::string ret;
		switch(m_type)
		{
		case DW_FORM_strp:
			{
				enum { kSIZE = 8192 };
				char buffer[kSIZE] = {0};
				t_uint64 off = asUInt64();
				debug_str->seek(off);
				debug_str->read_sz(buffer,kSIZE);
				ret = buffer;
			}
			break;
		case DW_FORM_string:
			{
				ret = cstr();
			}
			break;
		default:
			break;
		}
		return ret;
	}
	t_uint64 CDWARFFormValue::asUInt64() const
	{
		return uval();
	}
	t_int64 CDWARFFormValue::asInt64() const
	{
		return sval();
	}
}// namespace dwarf