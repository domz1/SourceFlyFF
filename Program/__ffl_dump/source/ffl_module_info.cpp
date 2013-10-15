
#include "../include/ffl_module_info.h"
#include "../include/ffl_c_string.h"

#include <cstdio>
#include <cstdlib>


ffl_module_info::ffl_module_info()
{
	::memset( file_version_, 0, sizeof( file_version_ ) );
	::memset( product_version_, 0, sizeof( product_version_ ) );

	::memset( company_name_, 0, sizeof( company_name_ ) );
	::memset( product_name_, 0, sizeof( product_name_ ) );
	::memset( file_description_, 0, sizeof( file_description_ ) );
}

bool	ffl_module_info::load_info( const ffl_tchar_t * path )
{
	if( path == NULL )
	{
		return	false;
	}

	DWORD	handle = 0;
	DWORD	info_size = 0;

	info_size = ::GetFileVersionInfoSize( path, &handle );
	if( info_size == 0 )
	{
		return	false;
	}

	unsigned char *	data = reinterpret_cast< unsigned char * >( ::malloc( info_size+1 ) );
	if( data == NULL )
	{
		return	false;
	}

	::memset( data, 0, info_size+1 );
	if( ::GetFileVersionInfo( path, handle, info_size, data ) == FALSE )
	{
		return	false;
	}

	VS_FIXEDFILEINFO	fixed_info;
	if( query_fixed_info( data, &fixed_info ) == false )
	{
		return	false;
	}

	file_version_[0]	= HIWORD( fixed_info.dwFileVersionMS );
	file_version_[1]	= LOWORD( fixed_info.dwFileVersionMS );
	file_version_[2]	= HIWORD( fixed_info.dwFileVersionLS );
	file_version_[3]	= LOWORD( fixed_info.dwFileVersionLS );

	product_version_[0]	= HIWORD( fixed_info.dwProductVersionMS );
	product_version_[1]	= LOWORD( fixed_info.dwProductVersionMS );
	product_version_[2]	= HIWORD( fixed_info.dwProductVersionLS );
	product_version_[3]	= LOWORD( fixed_info.dwProductVersionLS );

	// get string info
	if( query_string_info( data, FFL_T( "CompanyName" ), company_name_, FFL_COUNTOF( company_name_ ) ) == false )
	{
		::free( data );
		data = NULL;
		return false;
	}

	if( query_string_info( data, FFL_T( "FileDescription" ), product_name_, FFL_COUNTOF( product_name_ ) ) == false )
	{
		::free( data );
		data = NULL;
		return false;
	}

	if( query_string_info( data, FFL_T( "ProductName" ), file_description_, FFL_COUNTOF( file_description_ ) ) == false )
	{
		::free( data );
		data = NULL;
		return false;
	}

	if( data != NULL )
	{
		::free( data );
		data = NULL;
	}

	return true;
}

const ffl_tchar_t *	ffl_module_info::company_name()		const
{
	return	company_name_;
}

const ffl_tchar_t *	ffl_module_info::product_name()		const
{
	return	product_name_;
}

const ffl_tchar_t *	ffl_module_info::file_description()	const
{
	return	file_description_;
}

const WORD *	ffl_module_info::file_version()		const
{
	return	file_version_;
}

const WORD *	ffl_module_info::product_version()	const
{
	return	product_version_;
}

bool	ffl_module_info::query_fixed_info( void * data, VS_FIXEDFILEINFO * fixed_info )
{
	if( data == NULL || fixed_info == NULL )
	{
		return false;
	}

	UINT len = 0;
	VS_FIXEDFILEINFO * temp_info = NULL;

	if( ::VerQueryValue( data, FFL_T( "\\" ), reinterpret_cast< void ** >( &temp_info ), &len ) == FALSE )
	{
		return false;
	}

	if( len == sizeof( VS_FIXEDFILEINFO ) )
	{
		::memcpy( fixed_info, temp_info, len );
	}

	return true;
}

bool	ffl_module_info::query_string_info( void * data, const ffl_tchar_t * key, ffl_tchar_t * value, size_t count )
{
	if( data == NULL || key == NULL || value == NULL || count <= 0 )
	{
		return false;
	}

	DWORD * translation = NULL;
	UINT len = 0;
	ffl_tchar_t * temp_value = NULL;

	if( ::VerQueryValue( data, FFL_T( "\\VarFileInfo\\Translation" ), reinterpret_cast< void ** >( &translation ), &len ) == FALSE )
	{
		return false;
	}

	ffl_tchar_t full_key[1024] = { 0, };
	ffl_snprintf( full_key, FFL_COUNTOF( full_key ), FFL_T( "\\StringFileInfo\\%04x%04x\\%s" ), LOWORD( *translation ), HIWORD( *translation ), key );
	if( ::VerQueryValue( data, full_key, reinterpret_cast< void ** >( &temp_value ), &len ) == FALSE )
	{
		return false;
	}

	if( count >= len )
	{
		if( len != 0 )
		{
			ffl_strncpy( value, count, temp_value, len );
		}
		else
		{
			value[0] = 0;
		}
	}
	else
	{
		return false;
	}

	return true;
}