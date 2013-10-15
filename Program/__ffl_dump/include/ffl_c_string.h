#ifndef __FFL_C_STRING_H__
#define __FFL_C_STRING_H__


#include "ffl_dump_common.h"


int		ffl_vsnprintf_a( char * buffer, size_t count, const char * format, va_list args );
int		ffl_snprintf_a( char * buffer, size_t count, const char * format, ... );
int		ffl_strncpy_a( char * dest, size_t dest_count, const char * src, size_t src_count );

int		ffl_splitpath_a( const char * path, char * drive, size_t drive_count, char * dir, size_t dir_count, char * fname, size_t fname_count, char * ext, size_t ext_count );
int		ffl_makepath_a( char * path, size_t count, const char * drive, const char * dir, const char * fname, const char * ext );


int		ffl_vsnprintf_w( wchar_t * buffer, size_t count, const wchar_t * format, va_list args );
int		ffl_snprintf_w( wchar_t * buffer, size_t count, const wchar_t * format, ... );
int		ffl_strncpy_w( wchar_t * dest, size_t dest_count, const wchar_t * src, size_t src_count );

int		ffl_splitpath_w( const wchar_t * path, wchar_t * drive, size_t drive_count, wchar_t * dir, size_t dir_count, wchar_t * fname, size_t fname_count, wchar_t * ext, size_t ext_count );
int		ffl_makepath_w( wchar_t * path, size_t count, const wchar_t * drive, const wchar_t * dir, const wchar_t * fname, const wchar_t * ext );



#if	defined(_UNICODE) || defined(UNICODE)

#define	ffl_vsnprintf	ffl_vsnprintf_w
#define	ffl_snprintf		ffl_snprintf_w
#define	ffl_strncpy		ffl_strncpy_w

#if	defined(_MSC_VER) && (_MSC_VER > 1310)
#define	ffl_sscanf		::swscanf_s
#else
#define	ffl_sscanf		::swscanf
#endif

#define	ffl_splitpath	ffl_splitpath_w
#define	ffl_makepath		ffl_makepath_w

#else

#define	ffl_vsnprintf	ffl_vsnprintf_a
#define	ffl_snprintf		ffl_snprintf_a
#define	ffl_strncpy		ffl_strncpy_a

#if	defined(_MSC_VER) && (_MSC_VER > 1310)
#define	ffl_sscanf		::scanf_s
#else
#define	ffl_sscanf		::scanf
#endif

#define	ffl_splitpath	ffl_splitpath_a
#define	ffl_makepath		ffl_makepath_a

#endif



#endif	//__FFL_C_STRING_H__
