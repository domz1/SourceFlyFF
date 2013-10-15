#include "../include/ffl_dump_mini.h"
#include "../include/ffl_c_string.h"

#include <tchar.h>


void	ffl_dump_mini::dump( const ffl_tchar_t * name, ffl_dump_param_t * param )
{
	if( param != NULL && name != NULL )
	{
		HANDLE file = ::CreateFile( name,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
			NULL );

		if( file != INVALID_HANDLE_VALUE )
		{
			write( file, param );

			::CloseHandle( file );
			file = INVALID_HANDLE_VALUE;
		}
	}

	return;
}

void	ffl_dump_mini::write( HANDLE file, ffl_dump_param_t * param )
{
	if( file != INVALID_HANDLE_VALUE && param != NULL )
	{
		HMODULE dbghelp = find_dll();
		if( dbghelp != NULL )
		{
			typedef	BOOL (WINAPI * mini_dump_write_dump_f)( HANDLE,
				DWORD,
				HANDLE,
				MINIDUMP_TYPE,
				const PMINIDUMP_EXCEPTION_INFORMATION,
				const PMINIDUMP_USER_STREAM_INFORMATION,
				const PMINIDUMP_CALLBACK_INFORMATION );

			mini_dump_write_dump_f dump_f = (mini_dump_write_dump_f) ::GetProcAddress( dbghelp, "MiniDumpWriteDump" );
			if( dump_f != NULL )
			{
				int dump_type = MiniDumpNormal;// | MiniDumpWithHandleData;
				if( param->level == ffl_dump_level_medium )
				{
					dump_type |= MiniDumpWithDataSegs;
				}
				if( param->level == ffl_dump_level_heavy )
				{
					dump_type |= MiniDumpWithFullMemory;
				}

				MINIDUMP_EXCEPTION_INFORMATION info;
				info.ThreadId			= param->dump_thread_id;
				info.ExceptionPointers	= param->exception_ptr;
				info.ClientPointers		= FALSE;

				dump_f(	::GetCurrentProcess(),
					::GetCurrentProcessId(),
					file,
					static_cast< MINIDUMP_TYPE >( dump_type ),
					param->exception_ptr ? &info : NULL,
					NULL,
					NULL );
			}

			::FreeLibrary( dbghelp );
			dbghelp = NULL;
		}
	}

	return;
}

HMODULE	ffl_dump_mini::find_dll()
{
	static const ffl_tchar_t dll_name[] = FFL_T( "dbghelp.dll" );

	HMODULE dbghelp = NULL;
	ffl_tchar_t path[MAX_PATH] = { 0, };

	// 실행파일이 있는 곳의 dbghelp.dll을 먼저 로드한다.
	if( ::GetModuleFileName( NULL, path, FFL_COUNTOF( path ) ) != 0 )
	{
		ffl_tchar_t * slash = NULL;
		if( (slash = ::_tcsrchr( path, FFL_T( '\\' ) )) != NULL )
		{
			size_t count = FFL_COUNTOF( path ) - (slash-path);
			ffl_strncpy( slash + 1, count - 1, dll_name, FFL_COUNTOF( dll_name ) );
			dbghelp = ::LoadLibrary( path );
		}
	}

	// 실패하면 기본 dll을 로드한다.
	if( dbghelp == NULL )
	{
		dbghelp = ::LoadLibrary( dll_name );
	}

	return	dbghelp;
}