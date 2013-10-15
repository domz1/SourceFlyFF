#include "../include/ffl_dump_common.h"
#include "../include/ffl_c_string.h"

#include <tchar.h>


void	ffl_get_win_version( ffl_tchar_t * version, size_t version_count, ffl_tchar_t * build_num, size_t build_num_count )
{
	// winbase.h
	#ifndef	VER_PLATFORM_WIN32s
	#define	VER_PLATFORM_WIN32s             0
	#endif

	#ifndef	VER_PLATFORM_WIN32_WINDOWS
	#define	VER_PLATFORM_WIN32_WINDOWS      1
	#endif

	#ifndef	VER_PLATFORM_WIN32_NT
	#define	VER_PLATFORM_WIN32_NT           2
	#endif

	#ifndef	VER_PLATFORM_WIN32_CE
	#define	VER_PLATFORM_WIN32_CE           3
	#endif

	/*
	dwPlatFormID  dwMajorVersion  dwMinorVersion  dwBuildNumber
	95             1              4               0             950
	95 SP1         1              4               0        >950 && <=1080
	95 OSR2        1              4             <10           >1080
	98             1              4              10            1998
	98 SP1         1              4              10       >1998 && <2183
	98 SE          1              4              10          >=2183
	ME             1              4              90            3000

	NT 3.51        2              3              51
	NT 4           2              4               0            1381
	2000           2              5               0            2195
	XP             2              5               1            2600
	2003 Server    2              5               2            3790

	CE             3
	*/

	if( version != NULL && version_count > 0 && build_num != NULL && build_num_count > 0 )
	{
		static const ffl_tchar_t	win_unknown[]	= FFL_T( "Unknown Windows version" );

		static const ffl_tchar_t win95[]			= FFL_T( "Windows 95" );
		static const ffl_tchar_t win95_sp1[]		= FFL_T( "Windows 95 SP1" );
		static const ffl_tchar_t win95_osr2[]		= FFL_T( "Windows 95 OSR2" );
		static const ffl_tchar_t win98[]			= FFL_T( "Windows 98" );
		static const ffl_tchar_t win98_sp1[]		= FFL_T( "Windows 98 SP1" );
		static const ffl_tchar_t win98_se[]			= FFL_T( "Windows 98 SE" );
		static const ffl_tchar_t winme[]			= FFL_T( "Windows ME" );

		static const ffl_tchar_t winnt351[]			= FFL_T( "Windows NT 3.51" );
		static const ffl_tchar_t winnt4[]			= FFL_T( "Windows NT 4" );
		static const ffl_tchar_t win2000[]			= FFL_T( "Windows 2000 Professional" );
		static const ffl_tchar_t win2000_server[]	= FFL_T( "Windows 2000 Server" );
		static const ffl_tchar_t winxp[]			= FFL_T( "Windows XP" );
		static const ffl_tchar_t winxp_64[]			= FFL_T( "Windows XP(x64)" );
		static const ffl_tchar_t win2003_server[]	= FFL_T( "Windows Server 2003" );
		static const ffl_tchar_t winvista[]			= FFL_T( "Windows Vista" );
		static const ffl_tchar_t win2008_server[]	= FFL_T( "Windows Server 2008" );

		static const ffl_tchar_t wince[]			= FFL_T( "Windows CE" );


		ffl_strncpy( version, version_count, win_unknown, FFL_COUNTOF(win_unknown) );

		OSVERSIONINFOEX	os_info;
		::memset( &os_info, 0, sizeof(os_info) );
		os_info.dwOSVersionInfoSize	= sizeof(os_info);

		SYSTEM_INFO	sys_info;
		::memset( &sys_info, 0, sizeof(sys_info) );
		::GetSystemInfo( &sys_info );

		if( ::GetVersionEx( reinterpret_cast< OSVERSIONINFO * >( &os_info ) ) != FALSE )
		{
			DWORD platform_id	= os_info.dwPlatformId;
			DWORD major_version	= os_info.dwMajorVersion;
			DWORD minor_version	= os_info.dwMinorVersion;
			DWORD build_number	= os_info.dwBuildNumber & 0xFFFF;	// Win 95 needs this

			ffl_snprintf( build_num, build_num_count, FFL_T( "%u.%u.%u" ), major_version, minor_version, build_number );

			if( (platform_id == VER_PLATFORM_WIN32_WINDOWS) && (major_version == 4) )
			{
				if( (minor_version < 10) && (build_number == 950) )
				{
					ffl_strncpy( version, version_count, win95, FFL_COUNTOF( win95 ) );
				}
				else if( (minor_version < 10) && ((build_number > 950) && (build_number <= 1080)) )
				{
					ffl_strncpy( version, version_count, win95_sp1, FFL_COUNTOF( win95_sp1 ) );
				}
				else if( (minor_version < 10) && (build_number > 1080) )
				{
					ffl_strncpy( version, version_count, win95_osr2, FFL_COUNTOF( win95_osr2 ) );
				}
				else if( (minor_version == 10) && (build_number == 1998) )
				{
					ffl_strncpy( version, version_count, win98, FFL_COUNTOF( win98 ) );
				}
				else if( (minor_version == 10) && ((build_number > 1998) && (build_number < 2183)) )
				{
					ffl_strncpy( version, version_count, win98_sp1, FFL_COUNTOF( win98_sp1 ) );
				}
				else if( (minor_version == 10) && (build_number >= 2183) )
				{
					ffl_strncpy( version, version_count, win98_se, FFL_COUNTOF( win98_se ) );
				}
				else if( minor_version == 90 )
				{
					ffl_strncpy( version, version_count, winme, FFL_COUNTOF( winme ) );
				}
			}
			else if( platform_id == VER_PLATFORM_WIN32_NT )
			{
				if( (major_version == 3) && (minor_version == 51) )
				{
					ffl_strncpy( version, version_count, winnt351, FFL_COUNTOF( winnt351 ) );
				}
				else if( (major_version == 4) && (minor_version == 0) )
				{
					ffl_strncpy( version, version_count, winnt4, FFL_COUNTOF( winnt4 ) );
				}
				else if( (major_version == 5) && (minor_version == 0) )
				{
					if( os_info.wProductType == VER_NT_WORKSTATION )
					{
						ffl_strncpy( version, version_count, win2000, FFL_COUNTOF( win2000 ) );
					}
					else
					{
						ffl_strncpy( version, version_count, win2000_server, FFL_COUNTOF( win2000_server ) );
					}
				}
				else if( (major_version == 5) && (minor_version == 1) )
				{
					ffl_strncpy( version, version_count, winxp, FFL_COUNTOF( winxp ) );
				}
				else if( (major_version == 5) && (minor_version == 2) )
				{
					if( os_info.wProductType == VER_NT_WORKSTATION && sys_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
					{
						ffl_strncpy( version, version_count, winxp_64, FFL_COUNTOF( winxp_64 ) );
					}
					else
					{
						ffl_strncpy( version, version_count, win2003_server, FFL_COUNTOF( win2003_server ) );
					}
				}
				else if( (major_version == 6) && (minor_version == 0) )
				{
					if( os_info.wProductType == VER_NT_WORKSTATION )
					{
						ffl_strncpy( version, version_count, winvista, FFL_COUNTOF( winvista ) );
					}
					else
					{
						ffl_strncpy( version, version_count, win2008_server, FFL_COUNTOF( win2008_server ) );
					}
				}
			}
			else if( platform_id == VER_PLATFORM_WIN32_CE )
			{
				ffl_strncpy( version, version_count, wince, FFL_COUNTOF( wince ) );
			}
		}
	}

	return;
}

void	ffl_formating_time( ffl_tchar_t * output, size_t output_count, FILETIME * time )
{
	if( output != NULL && output_count > 0 && time != NULL )
	{
		output[0] = FFL_T( '\0' );
		WORD fat_date = 0, fat_time = 0;

		if( ::FileTimeToLocalFileTime( time, time ) && ::FileTimeToDosDateTime( time, &fat_date, &fat_time ) )
		{
			ffl_snprintf( output, output_count, FFL_T( "%d/%d/%d %02d:%02d:%02d" ),
				(fat_date / 512) + 1980, (fat_date / 32) & 15, fat_date & 31,
				(fat_time >> 11), (fat_time >> 5) & 0x3F, (fat_time & 0x1F) * 2 );
		}
	}
}

const ffl_tchar_t *	ffl_get_exception_string( DWORD code )
{
	#define	EXCEPTION( x )	case EXCEPTION_##x: return FFL_T( #x );

	switch( code )
	{
		EXCEPTION( ACCESS_VIOLATION )
		EXCEPTION( DATATYPE_MISALIGNMENT )
		EXCEPTION( BREAKPOINT )
		EXCEPTION( SINGLE_STEP )
		EXCEPTION( ARRAY_BOUNDS_EXCEEDED )
		EXCEPTION( FLT_DENORMAL_OPERAND )
		EXCEPTION( FLT_DIVIDE_BY_ZERO )
		EXCEPTION( FLT_INEXACT_RESULT )
		EXCEPTION( FLT_INVALID_OPERATION )
		EXCEPTION( FLT_OVERFLOW )
		EXCEPTION( FLT_STACK_CHECK )
		EXCEPTION( FLT_UNDERFLOW )
		EXCEPTION( INT_DIVIDE_BY_ZERO )
		EXCEPTION( INT_OVERFLOW )
		EXCEPTION( PRIV_INSTRUCTION )
		EXCEPTION( IN_PAGE_ERROR )
		EXCEPTION( ILLEGAL_INSTRUCTION )
		EXCEPTION( NONCONTINUABLE_EXCEPTION )
		EXCEPTION( STACK_OVERFLOW )
		EXCEPTION( INVALID_DISPOSITION )
		EXCEPTION( GUARD_PAGE )
		EXCEPTION( INVALID_HANDLE )
	}

	// If not one of the "known" exceptions, try to get the string
	// from NTDLL.DLL's message table.

	static ffl_tchar_t buffer[1024] = { 0 };

	::FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
		::GetModuleHandle( FFL_T( "ntdll.dll" ) ),
		code, 0, buffer, FFL_COUNTOF( buffer ), NULL );

	return	buffer;
}

void	ffl_dump_print( ffl_text_file * file, const ffl_tchar_t * format, ... )
{
	if( file != NULL && file->opened() == true && format != NULL )
	{
		ffl_tchar_t buffer[8*1024] = { 0, };

		va_list arglist;
		va_start( arglist, format );
		ffl_vsnprintf( buffer, FFL_COUNTOF( buffer ), format, arglist );
		va_end( arglist );

		FFL_VERIFY( file->write_text( buffer, ::_tcslen( buffer ) ) == ::_tcslen( buffer ) );
	}

	return;
}
