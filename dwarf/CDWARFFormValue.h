#ifndef __CDWARF_FORM_VALUE_H__
#define __CDWARF_FORM_VALUE_H__

#include <string>
#include <vector>
#include <io/stream_base.h>
#include <io/source_of_stream.h>
#include "DWARFTypes.h"
#include "EDWARF.h"

namespace dwarf
{
	class CDWARFFormValue
	{
	public:
		enum eValueType
		{
			eValueTypeInvalid = 0,
			eValueTypeUnsigned,
			eValueTypeSigned,
			eValueTypeCStr,
			eValueTypeBlock
		};
		enum eFormClass
		{
			eFormClassInvalid = 0,
			eFormClassAddress,
			eFormClassBlock,
			eFormClassConstant,
			eFormClassExprloc,
			eFormClassFlag,
			eFormClassReference,
			eFormClassString,
		};
	private:
		typedef std::string			FormStringData;
		typedef std::vector<char>	FormData;
	private:

		struct SBaseEl
		{
			/*not needed now*/
			/*SBaseEl() : data() {}*/
			/*not needed now*/
			union 
			{
				t_uint64	uval;
				t_int64		sval;
				char		bf[sizeof(FormStringData)];
			} un;
			/*not needed now*/
			//FormData data;
			/*not needed now*/
		};
		enum eValueSize
		{
			eValueSizeFixed,
			eValueSizeVariable
		};
	public:
		CDWARFFormValue();
		explicit CDWARFFormValue(DW_FORM form);
		CDWARFFormValue(const CDWARFFormValue& other);
		CDWARFFormValue& operator = (const CDWARFFormValue& other);
		~CDWARFFormValue();
		eFormClass          formClass() const;
		eValueType			type() const;
		DW_FORM				form() const;
		bool				parse(t_uint8 addr_size/*4 or 8*/, t_uint8 dwarf_format /*32 or 64*/, t_uint16 dwarf_version,io::stream_base::ptr debug_str,io::stream_base::ptr stream);
		/*not needed now*/
		//FormStringData		asString() const;
		/*not needed now*/
		FormStringData		asString(io::stream_base::ptr debug_str) const;
		t_uint64			asUInt64() const;
		t_int64				asInt64() const;

		static bool			Skip(DW_FORM form,t_uint8 addr_size/*4 or 8*/, t_uint8 dwarf_format /*32 or 64*/, t_uint16 dwarf_version, io::stream_base::ptr stream);
		static eValueType	ConvertFormToType(DW_FORM form,t_int32 addr_size,t_int32* osize);
	private:
		eValueType		install(eValueType type);
		void			copy(const CDWARFFormValue& other);
		eValueType		uninstall();
		t_uint64&		uval();
		t_int64&		sval();
		FormStringData*	pcstr();
		FormStringData&	cstr();
		/*not needed now*/
		//FormData&		data();
		/*not needed now*/

		
		const t_uint64&			uval() const;
		const t_int64&			sval() const;
		const FormStringData*	pcstr() const;
		const FormStringData&	cstr() const;
		/*not needed now*/
		//const FormData&			data() const;
		/*not needed now*/
	private:
		SBaseEl			m_storage;
		DW_FORM			m_type;
	};
}//namespace dwarf

#endif //__CDWARF_FORM_VALUE_H__