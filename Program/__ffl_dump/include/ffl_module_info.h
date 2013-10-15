#ifndef __FFL_MODULE_INFO_H__
#define __FFL_MODULE_INFO_H__


#include "ffl_common.h"

#pragma comment( lib, "Version.lib" )


class	ffl_module_info
{
	public:
		ffl_module_info();

		bool	load_info( const ffl_tchar_t * path );

		const ffl_tchar_t *	company_name()		const;
		const ffl_tchar_t *	product_name()		const;
		const ffl_tchar_t *	file_description()	const;

		const WORD *	file_version()		const;
		const WORD *	product_version()	const;

	private:
		bool	query_fixed_info( void * data, VS_FIXEDFILEINFO * fixed_info );
		bool	query_string_info( void * data, const ffl_tchar_t * key, ffl_tchar_t * value, size_t count );

	private:
		WORD	file_version_[4];
		WORD	product_version_[4];

		ffl_tchar_t	company_name_[128];
		ffl_tchar_t	product_name_[128];
		ffl_tchar_t	file_description_[512];

};



#endif	//__FFL_MODULE_INFO_H__
