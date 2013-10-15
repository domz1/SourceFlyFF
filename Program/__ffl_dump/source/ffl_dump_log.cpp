#include "../include/ffl_dump_log.h"
#include "../include/ffl_module_info.h"

#include "../include/ffl_c_string.h"


#define ONEKILO			1024
#define SIXTYFOURKILO	( 64 * ONEKILO )
#define ONEMEGA			( ONEKILO * ONEKILO )
#define ONEGIGA			( ONEKILO * ONEKILO * ONEKILO )


void	ffl_dump_log::dump( const ffl_tchar_t * name, ffl_dump_param_t * param )
{
#if defined( _X86_ ) && defined( _M_IX86 )

	if( name != NULL && param != NULL )
	{
		ffl_text_file file;
		if( file.create( name, GENERIC_WRITE ) == true )
		{
			log( &file, param );
		}
	}

#endif

	return;
}

#if	defined( _X86_ ) && defined( _M_IX86 )

void	ffl_dump_log::log( ffl_text_file * file, ffl_dump_param_t * param )
{
	if( file != NULL && param != NULL )
	{
		static const ffl_tchar_t	unknown[] = FFL_T( "Unknown" );

		PEXCEPTION_RECORD	exception = param->exception_ptr->ExceptionRecord;
		PCONTEXT			context = param->exception_ptr->ContextRecord;

		ffl_tchar_t	module_name[MAX_PATH] = { 0, };
		if( ::GetModuleFileName( NULL, module_name, FFL_COUNTOF(module_name) ) <= 0 )
		{
			ffl_strncpy( module_name, FFL_COUNTOF(module_name), unknown, FFL_COUNTOF(unknown) );
		}

		ffl_tchar_t	crash_module_path[MAX_PATH] = { 0, };
		MEMORY_BASIC_INFORMATION	memory_info;
		::memset( &memory_info, 0, sizeof(memory_info) );
		if( ::VirtualQuery( reinterpret_cast< void * >( context->Eip ), &memory_info, sizeof(memory_info) ) != 0 )
		{
			if( ::GetModuleFileName( reinterpret_cast< HINSTANCE >( memory_info.AllocationBase ), crash_module_path, FFL_COUNTOF(crash_module_path) ) <= 0 )
			{
				ffl_strncpy( crash_module_path, FFL_COUNTOF(crash_module_path), unknown, FFL_COUNTOF(unknown) );
			}
		}

		ffl_dump_print( file,
			FFL_T( "%s caused %s (0x%08x) \r\nin module %s at %04x:%08x.\r\n\r\n" ),
			module_name,
			ffl_get_exception_string( exception->ExceptionCode ),
			exception->ExceptionCode,
			crash_module_path,
			context->SegCs,
			context->Eip );

		system_information_log( file );

		if( exception->ExceptionCode == STATUS_ACCESS_VIOLATION && exception->NumberParameters >= 2 )
		{
			const ffl_tchar_t *	read_from = FFL_T( "Read from" );
			if( exception->ExceptionInformation[0] )
			{
				read_from = FFL_T( "Write to" );
			}

			ffl_dump_print( file, FFL_T( "%s location %08x caused an access violation.\r\n" ),
				read_from, exception->ExceptionInformation[1] );
		}

		register_log( file, context );

		ffl_dump_print( file, FFL_T( "\r\nBytes at CS:EIP:\r\n" ) );

		BYTE *	code = reinterpret_cast< BYTE * >( context->Eip );

		const int	NUM_CODE_BYTES = 16;
		for( int nCodebyte = 0; nCodebyte < NUM_CODE_BYTES; ++nCodebyte )
		{
			__try
			{
				ffl_dump_print( file, FFL_T( "%02x " ), code[nCodebyte] );
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
				ffl_dump_print( file, FFL_T( "?? " ) );
			}
		}

		DWORD *	stack = reinterpret_cast< DWORD * >( context->Esp );
		stack_log( file, stack );

		module_list_log( file );

		ffl_dump_print( file, FFL_T( "\r\n===== [ end of file ] =====\r\n" ) );
	}

	return;
}

void	ffl_dump_log::system_information_log( ffl_text_file * file )
{
	if( file != NULL )
	{
		static const ffl_tchar_t	unknown[] = FFL_T( "Unknown" );

		FILETIME	current_time;
		::GetSystemTimeAsFileTime( &current_time );

		ffl_tchar_t	time_string[100] = { 0, };
		ffl_formating_time( time_string, FFL_COUNTOF(time_string), &current_time );

		ffl_dump_print( file, FFL_T( "Error occurred at %s.\r\n" ), time_string );

		ffl_tchar_t	module_name[MAX_PATH] = { 0, };
		if( ::GetModuleFileName( 0, module_name, FFL_COUNTOF(module_name) ) <= 0 )
		{
			ffl_strncpy( module_name, FFL_COUNTOF(module_name), unknown, FFL_COUNTOF(unknown) );
		}

		ffl_tchar_t	user_name[200] = { 0, };
		DWORD	user_name_count = FFL_COUNTOF(user_name);
		if( ::GetUserName( user_name, &user_name_count ) == FALSE )
		{
			ffl_strncpy( user_name, FFL_COUNTOF(user_name), unknown, FFL_COUNTOF(unknown) );
		}

		ffl_dump_print( file, FFL_T( "%s, run by %s.\r\n" ), module_name, user_name );


		ffl_tchar_t	win_version[50] = { 0, };
		ffl_tchar_t	build[50] = { 0, };
		ffl_get_win_version( win_version, FFL_COUNTOF(win_version), build, FFL_COUNTOF(build) );

		ffl_dump_print( file, FFL_T( "Operation system : %s (%s).\r\n" ), win_version, build );

		SYSTEM_INFO	system_info;
		::memset( &system_info, 0, sizeof(system_info) );
		::GetSystemInfo( &system_info );
		ffl_dump_print( file, FFL_T( "%d processor(s), type %d.\r\n" ), system_info.dwNumberOfProcessors, system_info.dwProcessorType );

		MEMORYSTATUS	memory_info;
		::memset( &memory_info, 0, sizeof(memory_info) );
		memory_info.dwLength = sizeof(memory_info);
		::GlobalMemoryStatus( &memory_info );

		ffl_dump_print( file, FFL_T( "%d%% memory in use.\r\n" ), memory_info.dwMemoryLoad );
		ffl_dump_print( file, FFL_T( "%d MBytes physical memory.\r\n" ), (memory_info.dwTotalPhys + ONEMEGA - 1) / ONEMEGA );
		ffl_dump_print( file, FFL_T( "%d MBytes physical memory free.\r\n" ), (memory_info.dwAvailPhys + ONEMEGA - 1) / ONEMEGA );
		ffl_dump_print( file, FFL_T( "%d MBytes paging file.\r\n" ), (memory_info.dwTotalPageFile + ONEMEGA - 1) / ONEMEGA );
		ffl_dump_print( file, FFL_T( "%d MBytes paging file free.\r\n" ), (memory_info.dwAvailPageFile + ONEMEGA - 1) / ONEMEGA );
		ffl_dump_print( file, FFL_T( "%d MBytes user address space.\r\n" ), (memory_info.dwTotalVirtual + ONEMEGA - 1) / ONEMEGA );
		ffl_dump_print( file, FFL_T( "%d MBytes user address space free.\r\n" ), (memory_info.dwAvailVirtual + ONEMEGA - 1) / ONEMEGA );
	}

	return;
}

void	ffl_dump_log::register_log( ffl_text_file * file, PCONTEXT context )
{
	if( file != NULL && context != NULL )
	{
		ffl_dump_print( file, FFL_T( "\r\n" ) );
		ffl_dump_print( file, FFL_T( "Context:\r\n" ) );
		ffl_dump_print( file, FFL_T( "EDI:    0x%08x  ESI: 0x%08x  EAX:   0x%08x\r\n" ), context->Edi, context->Esi, context->Eax );
		ffl_dump_print( file, FFL_T( "EBX:    0x%08x  ECX: 0x%08x  EDX:   0x%08x\r\n" ), context->Ebx, context->Ecx, context->Edx );
		ffl_dump_print( file, FFL_T( "EIP:    0x%08x  EBP: 0x%08x  SegCs: 0x%08x\r\n" ), context->Eip, context->Ebp, context->SegCs );
		ffl_dump_print( file, FFL_T( "EFlags: 0x%08x  ESP: 0x%08x  SegSs: 0x%08x\r\n" ), context->EFlags, context->Esp, context->SegSs );
	}

	return;
}

void	ffl_dump_log::stack_log( ffl_text_file * file, DWORD * stack )
{
	if( file != NULL && stack != NULL )
	{
		static const int max_stack_dump	= 3072;
		static const int stack_columns	= 4;

		ffl_dump_print( file, FFL_T( "\r\n\r\nStack:\r\n" ) );

		__try
		{
			DWORD * top = NULL;

			__asm
			{
				mov	eax, fs:[4]
				mov top, eax
			}

			if( top > stack + max_stack_dump )
			{
				top = stack + max_stack_dump;
			}

			int count = 0;
			DWORD * start = stack;
			int dwords_printed = 0;

			while( stack + 1 <= top )
			{
				if( (count % stack_columns) == 0 )
				{
					start = stack;
					dwords_printed = 0;
					ffl_dump_print( file, FFL_T( "0x%08x: " ), stack );
				}

				if( (++count % stack_columns) == 0 || stack + 2 > top )
				{
					ffl_dump_print( file, FFL_T( "%08x " ), *stack );
					++dwords_printed;

					int n = dwords_printed;
					while( n < 4 )
					{
						ffl_dump_print( file, FFL_T( "         " ) );
						++n;
					}

					for( int i = 0; i < dwords_printed; ++i )
					{
						DWORD stack_value = *start;
						for( int j = 0; j < 4; ++j )
						{
							char c = static_cast< char >( (stack_value & 0xFF) );
							if( c < 0x20 || c > 0x7E )
							{
								c = '.';
							}

#if	defined(_UNICODE) || defined(UNICODE)
							wchar_t w = static_cast< wchar_t >( c );
							ffl_dump_print( file, L"%c", w );
#else
							ffl_dump_print( file, "%c", c );
#endif
							stack_value = stack_value >> 8;
						}

						++start;
					}

					ffl_dump_print( file, FFL_T( "\r\n" ) );
				}
				else
				{
					ffl_dump_print( file, FFL_T( "%08x " ), *stack );
					++dwords_printed;
				}
				++stack;
			}

			ffl_dump_print( file, FFL_T( "\r\n" ) );
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
			ffl_dump_print( file, FFL_T( "Exception encountered during stack dump.\r\n" ) );
		}
	}

	return;
}

void	ffl_dump_log::module_list_log( ffl_text_file * file )
{
	if( file != NULL )
	{
		SYSTEM_INFO	system_info;
		::GetSystemInfo( &system_info );

		const size_t page_size = system_info.dwPageSize;
		const size_t num_pages = 4 * size_t( ONEGIGA / page_size );

		size_t page_num = 0;
		void * last_alloc_base = NULL;
		int module_num = 1;

		while( page_num < num_pages )
		{
			MEMORY_BASIC_INFORMATION memory_info;
			if( ::VirtualQuery( (void *)(page_num * page_size), &memory_info, sizeof( memory_info ) ) )
			{
				if( memory_info.RegionSize > 0 )
				{
					page_num += memory_info.RegionSize / page_size;
					if( memory_info.State == MEM_COMMIT && memory_info.AllocationBase > last_alloc_base )
					{
						last_alloc_base = memory_info.AllocationBase;
						if( module_info_log( file, reinterpret_cast< HINSTANCE >( last_alloc_base ), module_num ) == true )
						{
							++module_num;
						}
					}

					continue;
				}
			}

			page_num += SIXTYFOURKILO / page_size;
		}
	}

	return;
}

bool	ffl_dump_log::module_info_log( ffl_text_file * file, HINSTANCE module, int module_num )
{
	__try
	{
		ffl_tchar_t module_name[MAX_PATH] = { 0, };
		if( ::GetModuleFileName( module, module_name, FFL_COUNTOF( module_name ) ) > 0 )
		{
			IMAGE_DOS_HEADER * dos_header = reinterpret_cast< IMAGE_DOS_HEADER * >( module );
			if( IMAGE_DOS_SIGNATURE != dos_header->e_magic )
			{
				return false;
			}

			IMAGE_NT_HEADERS * nt_header = reinterpret_cast< IMAGE_NT_HEADERS * >( ((reinterpret_cast< char * >( dos_header )) + dos_header->e_lfanew) );
			if( IMAGE_NT_SIGNATURE != nt_header->Signature )
			{
				return false;
			}

			HANDLE module_file = ::CreateFile( module_name,
				GENERIC_READ,
				FILE_SHARE_READ,
				0,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				0 );

			ffl_tchar_t time_string[100] = { 0, };
			DWORD file_size = 0;

			if( module_file != INVALID_HANDLE_VALUE )
			{
				file_size = ::GetFileSize( module_file, 0 );
				FILETIME last_write_time;
				if( ::GetFileTime( module_file, 0, 0, &last_write_time ) )
				{
					ffl_formating_time( time_string, FFL_COUNTOF( time_string ), &last_write_time );
				}

				::CloseHandle( module_file );
				module_file = INVALID_HANDLE_VALUE;
			}

			ffl_dump_print( file, FFL_T( "Module %d\r\n" ), module_num );
			ffl_dump_print( file, FFL_T( "%s\r\n" ), module_name );
			ffl_dump_print( file, FFL_T( "Image Base: 0x%08x  Image Size: 0x%08x\r\n" ), 
				nt_header->OptionalHeader.ImageBase,
				nt_header->OptionalHeader.SizeOfImage );

			ffl_dump_print( file, FFL_T( "Checksum:   0x%08x  Time Stamp: 0x%08x\r\n" ),
				nt_header->OptionalHeader.CheckSum,
				nt_header->FileHeader.TimeDateStamp );

			ffl_dump_print( file, FFL_T( "File Size:  %-10d  File Time:  %s\r\n" ),
				file_size, time_string );

			print_module_info( file, module_name );

			return true;
		}
	}

	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return false;
}

void	ffl_dump_log::print_module_info( ffl_text_file * file, const ffl_tchar_t * module_path )
{
	if( file != NULL && module_path != NULL )
	{
		ffl_module_info module_info;
		if( module_info.load_info( module_path ) == true )
		{
			ffl_dump_print( file, FFL_T( "Version Information:\r\n" ) );

			ffl_dump_print( file, FFL_T( "   Company:    %s\r\n" ), module_info.company_name() );
			ffl_dump_print( file, FFL_T( "   Product:    %s\r\n" ), module_info.product_name() );
			ffl_dump_print( file, FFL_T( "   FileDesc:   %s\r\n" ), module_info.file_description() );

			const WORD * file_verion = module_info.file_version();
			ffl_dump_print( file, FFL_T( "   FileVer:    %d.%d.%d.%d\r\n" ), 
				file_verion[0], file_verion[1], file_verion[2], file_verion[3] );

			const WORD * prod_verion = module_info.file_version();
			ffl_dump_print( file, FFL_T( "   ProdVer:    %d.%d.%d.%d\r\n" ),
				prod_verion[0], prod_verion[1], prod_verion[2], prod_verion[3] );

			ffl_dump_print( file, FFL_T( "\r\n" ) );
		}
	}

	return;
}

#endif