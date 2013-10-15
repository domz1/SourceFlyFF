#include "../include/ffl_dump_unexception_filter.h"

#include "../include/ffl_dump_mini.h"
#include "../include/ffl_dump_report.h"
#include "../include/ffl_dump_log.h"

#include "../include/ffl_c_string.h"
#include "../include/ffl_date_time.h"

#include <process.h>
#include <cstdio>
#include <cstdlib>


#if	defined(_MSC_VER) && (_MSC_VER > 1310)
static void	ffl_invalid_parameter( const wchar_t * /* expression */, const wchar_t * /* function */, const wchar_t * /* file */, unsigned int /* line */, uintptr_t /* reserved */ )
{
	::RaiseException( EXCEPTION_ACCESS_VIOLATION, 0, 0, NULL );
}
#endif

static void	ffl_purecall_handler()
{
	::RaiseException( EXCEPTION_ACCESS_VIOLATION, 0, 0, NULL );
}

static void	make_file_name( ffl_tchar_t * output, size_t output_count, const ffl_tchar_t * prefix, const ffl_tchar_t * ext )
{
	if( output != NULL && output_count > 0 && prefix != NULL && ext != NULL )
	{
		ffl_tchar_t module_name[MAX_PATH] = { 0, };

		if( ::GetModuleFileName( NULL, module_name, FFL_COUNTOF( module_name ) ) <= 0 )
		{
			static const ffl_tchar_t unknown[] = FFL_T( "Unknown" );

			ffl_strncpy( module_name, FFL_COUNTOF( module_name ), unknown, FFL_COUNTOF( unknown ) );
		}

		ffl_date_time date;

		ffl_tchar_t drive[_MAX_DRIVE]	= { 0, };
		ffl_tchar_t dir[_MAX_DIR]		= { 0, };
		ffl_tchar_t name[_MAX_FNAME]	= { 0, };
		ffl_tchar_t temp[MAX_PATH]		= { 0, };

		ffl_splitpath( module_name, drive, FFL_COUNTOF( drive ), dir, FFL_COUNTOF( dir ), name, FFL_COUNTOF( name ), NULL, 0 );

		static const ffl_tchar_t file_name_format[] = FFL_T( "%s%s_%d.%d.%d_%d.%d.%d.%s" );

		ffl_snprintf( temp, FFL_COUNTOF( temp ), file_name_format,
			prefix, name,
			date.year(), date.month(), date.day(),
			date.hour(), date.minute(), date.second(),
			ext );
		ffl_makepath( output, output_count, drive, dir, temp, NULL );

		int count = 0;
		DWORD attr = ::GetFileAttributes( output );
		while( attr != INVALID_FILE_ATTRIBUTES )
		{
			static const ffl_tchar_t file_name_format_next[] = FFL_T( "%s%s_%d.%d.%d_%d.%d.%d(%d).%s" );

			++count;

			ffl_snprintf( temp, FFL_COUNTOF( temp ), file_name_format_next,
				prefix, name,
				date.year(), date.month(), date.day(),
				date.hour(), date.minute(), date.second(),
				count,
				ext );
			ffl_makepath( output, output_count, drive, dir, temp, NULL );

			attr = ::GetFileAttributes( output );
		}
	}
}

static void	record( ffl_dump_param_t * param, const ffl_tchar_t * prefix )
{
	if( param != NULL && prefix != NULL )
	{
		static const ffl_tchar_t minidump_ext[]	= FFL_T( "dmp" );
		static const ffl_tchar_t log_ext[]		= FFL_T( "txt" );
		static const ffl_tchar_t report_ext[]	= FFL_T( "rpt" );

		ffl_tchar_t temp_file[MAX_PATH] = { 0, };

		make_file_name( temp_file, FFL_COUNTOF( temp_file ), prefix, minidump_ext );
		ffl_dump_mini minidump;
		minidump.dump( temp_file, param );

		make_file_name( temp_file, FFL_COUNTOF( temp_file ), prefix, log_ext );
		ffl_dump_log dumplog;
		dumplog.dump( temp_file, param );

		make_file_name( temp_file, FFL_COUNTOF( temp_file ), prefix, report_ext );
		ffl_dump_report dumpreport;
		dumpreport.dump( temp_file, param );
	}
}

static unsigned int __stdcall	ffl_dump_recod_function( void * arg )
{
	ffl_dump_param_t * param = reinterpret_cast < ffl_dump_param_t * >( arg );

	if( param != NULL )
	{
		static const ffl_tchar_t crash_prefix[] = FFL_T( "" );

		record( param, crash_prefix );
	}

	return 0;
}




int		ffl_dump_unexception_filter::dump_level_ = ffl_dump_level_light;

ffl_dump_unexception_filter::ffl_dump_unexception_filter( int level /* = ffl_dump_level_light */ )
{
	install( level );
}

ffl_dump_unexception_filter::~ffl_dump_unexception_filter()
{
}

void	ffl_dump_unexception_filter::install( int level /* = ffl_dump_level_light */ )
{
	if( level != ffl_dump_level_none )
	{
		dump_level_ = level;

#if	defined(_MSC_VER) && (_MSC_VER > 1310)
		// crt error handler
		::_set_invalid_parameter_handler( ffl_invalid_parameter );
#endif
		_set_purecall_handler( ffl_purecall_handler );

		// win32 api handler
		::SetUnhandledExceptionFilter( top_level_filter );
	}
}

void	ffl_dump_unexception_filter::snapping( int level /* = ffl_dump_level_light */ )
{
	__try
	{
		::RaiseException( EXCEPTION_ACCESS_VIOLATION, 0, 0, NULL );
	}
	__except( do_snapping( level, GetExceptionInformation() ), EXCEPTION_EXECUTE_HANDLER ) 
	{
	}
}

LONG	ffl_dump_unexception_filter::top_level_filter( EXCEPTION_POINTERS * exception_ptr )
{
	if( exception_ptr != NULL && exception_ptr->ExceptionRecord != NULL )
	{
		ffl_dump_param_t * param = NULL;
		param = reinterpret_cast< ffl_dump_param_t * >( ::malloc( sizeof( ffl_dump_param_t ) ) );
		if( param != NULL )
		{
			param->level = dump_level_;
			param->dump_thread = ::GetCurrentThread();
			param->dump_thread_id = ::GetCurrentThreadId();
			param->exception_ptr = exception_ptr;

			if( exception_ptr->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW )
			{
				unsigned int id = 0;
				HANDLE handle = reinterpret_cast< HANDLE >( ::_beginthreadex( NULL, 0, ffl_dump_recod_function, param, 0, &id ) );
				if( handle != NULL )
				{
					::WaitForSingleObject( handle, INFINITE );
					::CloseHandle( handle );
					handle = NULL;
				}
			}
			else
			{
				static const ffl_tchar_t crash_prefix[] = FFL_T( "" );
				record( param, crash_prefix );
			}

			::free( param );
			param = NULL;

			return EXCEPTION_EXECUTE_HANDLER;
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void	ffl_dump_unexception_filter::do_snapping( int level, EXCEPTION_POINTERS * exception_ptr )
{
	if( level != ffl_dump_level_none && exception_ptr != NULL )
	{
		ffl_dump_param_t * param = NULL;
		param = reinterpret_cast< ffl_dump_param_t * >( ::malloc( sizeof( ffl_dump_param_t ) ) );
		if( param != NULL )
		{
			static const ffl_tchar_t snapping_prefix[] = FFL_T( "snap_" );

			param->level = dump_level_;
			param->dump_thread = ::GetCurrentThread();
			param->dump_thread_id = ::GetCurrentThreadId();
			param->exception_ptr = exception_ptr;

			record( param, snapping_prefix );
		}
	}
}