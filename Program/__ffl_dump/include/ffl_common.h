#ifndef __FFL_COMMON_H__
#define __FFL_COMMON_H__


#ifndef	_WIN32_WINNT

#define	_WIN32_WINNT	0x0500

#else

#if _WIN32_WINNT < 0x0403
#error	Windows 2000 or later required.
#endif

#endif

#ifndef	WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <new>

#include "ffl_assert.h"

//////////////////////////////////////////////////////////////////////////

typedef	__int64				ffl_int64_t;
typedef	unsigned __int64	ffl_uint64_t;


#if	defined(_UNICODE) || defined(UNICODE)

typedef	wchar_t				ffl_tchar_t;

#ifndef	FFL_WIDEN2
#define FFL_WIDEN2( x )		L ## x
#endif

#ifndef	FFL_T
#define FFL_T( x )			FFL_WIDEN2( x )
#endif

#else

typedef	char				ffl_tchar_t;

#ifndef	FFL_T
#define	FFL_T( x )			x
#endif

#endif


#ifndef	FFL_FILE
#define FFL_FILE			FFL_T( __FILE__ )
#endif

#ifndef	FFL_LINE
#define FFL_LINE			__LINE__
#endif

#ifndef	FFL_FUNCTION
#define FFL_FUNCTION		FFL_T( __FUNCTION__ )
#endif

//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
#ifndef	FFL_UNARG
#define	FFL_UNARG( arg )	(arg);
#endif
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#ifndef	FFL_COUNTOF
#define	FFL_COUNTOF( a )	(sizeof( (a) )/sizeof( (a)[0] ))
#endif
//////////////////////////////////////////////////////////////////////////


#endif	//__FFL_COMMON_H__
