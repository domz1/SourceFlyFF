#include "../include/ffl_c_string.h"

#include <cstdio>
#include <cstdlib>


int		ffl_vsnprintf_a( char * buffer, size_t count, const char * format, va_list args )
{
	FFL_ASSERT( buffer != NULL );
	FFL_ASSERT( count > 0 );

	int	ret = -1;

	if( buffer != NULL && count > 0 && format != NULL )
	{
#if	defined(_MSC_VER) && (_MSC_VER > 1310)
		ret = ::vsnprintf_s( buffer, count, _TRUNCATE, format, args );
#else
		ret = ::_vsnprintf( buffer, count, format, args );
#endif
	}

	return	ret;
}

int		ffl_snprintf_a( char * buffer, size_t count, const char * format, ... )
{
	FFL_ASSERT( buffer != NULL );
	FFL_ASSERT( count > 0 );

	int	ret = -1;

	if( buffer != NULL && count > 0 && format != NULL )
	{
		va_list args;
		va_start( args, format );
		ret = ffl_vsnprintf_a( buffer, count, format, args );
		va_end( args );
	}

	return	ret;
}

int		ffl_strncpy_a( char * dest, size_t dest_count, const char * src, size_t src_count )
{
	FFL_ASSERT( dest != NULL );
	FFL_ASSERT( dest_count > 0 );
	FFL_ASSERT( src != NULL );
	FFL_ASSERT( src_count > 0 );

	if( dest != NULL && dest_count > 0 && src != NULL && src_count > 0 )
	{
#if	defined(_MSC_VER) && (_MSC_VER > 1310)
		if( ::strncpy_s( dest, dest_count, src, src_count ) == 0 )
		{
			return	0;
		}
#else
		::strncpy( dest, src, src_count );

		return	0;
#endif
	}

	return	-1;
}

int		ffl_splitpath_a( const char * path, char * drive, size_t drive_count, char * dir, size_t dir_count, char * fname, size_t fname_count, char * ext, size_t ext_count )
{
	FFL_ASSERT( path != NULL );

	if( path != NULL )
	{
#if	defined(_MSC_VER) && (_MSC_VER > 1310)
		if( ::_splitpath_s( path, drive, drive_count, dir, dir_count, fname, fname_count, ext, ext_count ) == 0 )
		{
			return	0;
		}
#else
		FFL_UNARG( drive_count );
		FFL_UNARG( dir_count );
		FFL_UNARG( fname_count );
		FFL_UNARG( ext_count );

		::_splitpath( path, drive, dir, fname, ext );

		return	0;
#endif
	}

	return	-1;
}

int		ffl_makepath_a( char * path, size_t count, const char * drive, const char * dir, const char * fname, const char * ext )
{
	if( path != NULL && count > 0 )
	{
#if	defined(_MSC_VER) && (_MSC_VER > 1310)
		if( ::_makepath_s( path, count, drive, dir, fname, ext ) == 0 )
		{
			return	0;
		}
#else
		::_makepath( path, drive, dir, fname, ext );

		return	0;
#endif
	}

	return	-1;
}


int		ffl_vsnprintf_w( wchar_t * buffer, size_t count, const wchar_t * format, va_list args )
{
	FFL_ASSERT( buffer != NULL );
	FFL_ASSERT( count > 0 );

	int	ret = -1;

	if( buffer != NULL && count > 0 && format != NULL )
	{
#if	defined(_MSC_VER) && (_MSC_VER > 1310)
		ret = ::_vsnwprintf_s( buffer, count, _TRUNCATE, format, args );
#else
		ret = ::_vsnwprintf( buffer, count, format, args );
#endif
	}

	return	ret;
}

int		ffl_snprintf_w( wchar_t * buffer, size_t count, const wchar_t * format, ... )
{
	FFL_ASSERT( buffer != NULL );
	FFL_ASSERT( count > 0 );

	int	ret = -1;

	if( buffer != NULL && count > 0 && format != NULL )
	{
		va_list args;
		va_start( args, format );
		ret = ffl_vsnprintf_w( buffer, count, format, args );
		va_end( args );
	}

	return	ret;
}

int		ffl_strncpy_w( wchar_t * dest, size_t dest_count, const wchar_t * src, size_t src_count )
{
	FFL_ASSERT( dest != NULL );
	FFL_ASSERT( dest_count > 0 );
	FFL_ASSERT( src != NULL );
	FFL_ASSERT( src_count > 0 );

	if( dest != NULL && dest_count > 0 && src != NULL )
	{
#if	defined(_MSC_VER) && (_MSC_VER > 1310)
		if( ::wcsncpy_s( dest, dest_count, src, src_count ) == 0 )
		{
			return	0;
		}
#else
		::wcsncpy( dest, src, src_count );

		return	0;
#endif
	}

	return	-1;
}

int		ffl_splitpath_w( const wchar_t * path, wchar_t * drive, size_t drive_count, wchar_t * dir, size_t dir_count, wchar_t * fname, size_t fname_count, wchar_t * ext, size_t ext_count )
{
	FFL_ASSERT( path != NULL );

	if( path != NULL )
	{
#if	defined(_MSC_VER) && (_MSC_VER > 1310)
		if( ::_wsplitpath_s( path, drive, drive_count, dir, dir_count, fname, fname_count, ext, ext_count ) == 0 )
		{
			return	0;
		}
#else
		FFL_UNARG( drive_count );
		FFL_UNARG( dir_count );
		FFL_UNARG( fname_count );
		FFL_UNARG( ext_count );

		::_wsplitpath( path, drive, dir, fname, ext );

		return	0;
#endif
	}

	return	-1;
}

int		ffl_makepath_w( wchar_t * path, size_t count, const wchar_t * drive, const wchar_t * dir, const wchar_t * fname, const wchar_t * ext )
{
	if( path != NULL && count > 0 )
	{
#if	defined(_MSC_VER) && (_MSC_VER > 1310)
		if( ::_wmakepath_s( path, count, drive, dir, fname, ext ) == 0 )
		{
			return	0;
		}
#else
		::_wmakepath( path, drive, dir, fname, ext );

		return	0;
#endif
	}

	return	-1;
}

