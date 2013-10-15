#include "../include/ffl_dump_report.h"


ffl_dump_report::ffl_dump_report()
{
	sym_initialize_				= NULL;
	sym_cleanup_				= NULL;
	stack_walk_					= NULL;
	sym_function_table_access_	= NULL;
	sym_get_module_base_		= NULL;
	sym_get_sym_from_addr_		= NULL;
}

ffl_dump_report::~ffl_dump_report()
{
}

void	ffl_dump_report::dump( const ffl_tchar_t * name, ffl_dump_param_t * param )
{
#if	defined( _X86_ ) && defined( _M_IX86 )

	if( name != NULL && param != NULL )
	{
		ffl_text_file file;

		if( file.create( name, GENERIC_WRITE ) == true )
		{
			report( &file, param );
		}
	}

#endif

	return;
}


#if	defined( _X86_ ) && defined( _M_IX86 )

void	ffl_dump_report::report( ffl_text_file * file, ffl_dump_param_t * param )
{
	if( file != NULL && param != NULL )
	{
		FILETIME	current_time;
		::GetSystemTimeAsFileTime( &current_time );

		ffl_tchar_t	time_buffer[100] = { 0, };
		ffl_formating_time( time_buffer, FFL_COUNTOF( time_buffer ), &current_time );

		ffl_dump_print( file, FFL_T( "\r\n\r\n==================   logged at %s    ==========================\r\n\r\n" ), time_buffer );

		generate_exception( file, param );

		ffl_dump_print( file, FFL_T( "\r\n===== [end of file] =====\r\n" ) );
	}

	return;
}

void	ffl_dump_report::generate_exception( ffl_text_file * file, ffl_dump_param_t * param )
{
	if( file != NULL && param != NULL )
	{
		PEXCEPTION_RECORD	exception_record = param->exception_ptr->ExceptionRecord;

		ffl_dump_print( file, FFL_T( "Exception code : %08X %s\r\n" ), exception_record->ExceptionCode, ffl_get_exception_string( exception_record->ExceptionCode ) );

		ffl_tchar_t	faulting_module[MAX_PATH] = { 0, };
		DWORD		section = 0;
		DWORD		offset = 0;
		get_logical_address( exception_record->ExceptionAddress, faulting_module, FFL_COUNTOF(faulting_module), &section, &offset );

		ffl_dump_print( file, FFL_T( "Fault address :%08X %02X:%08X %s\r\n" ), exception_record->ExceptionAddress, section, offset, faulting_module );

		PCONTEXT	context = param->exception_ptr->ContextRecord;

		//	Show the registers
#ifdef	_M_IX86		//	Intel Only!
		ffl_dump_print( file, FFL_T( "\r\nRegisters:\r\n" ) );

		ffl_dump_print( file, FFL_T( "EAX:%08X\r\nEBX:%08X\r\nECX:%08X\r\nEDX:%08X\r\nESI:%08X\r\nEDI:%08X\r\n" ),
			context->Eax, context->Ebx, context->Ecx, context->Edx, context->Esi, context->Edi );

		ffl_dump_print( file, FFL_T( "CS:EIP:%04X:%08X\r\n" ), context->SegCs, context->Eip );
		ffl_dump_print( file, FFL_T( "SS:ESP:%04X:%08X  EBP:%08X\r\n" ), context->SegSs, context->Esp, context->Ebp );
		ffl_dump_print( file, FFL_T( "DS:%04X  ES:%04X  FS:%04X  GS:%04X\r\n" ), context->SegDs, context->SegEs, context->SegFs, context->SegGs );
		ffl_dump_print( file, FFL_T( "Flags:%08X\r\n" ), context->EFlags );
#endif	//_M_IX86

		static const ffl_tchar_t	dll_name[] = FFL_T( "imagehlp.dll" );
		HMODULE	imagehlp = ::LoadLibrary( dll_name );

		if( imagehlp != NULL )
		{
			if( inti_imagehlp_func( imagehlp ) == true )
			{
				imagehlp_stack_walk( file, context, param->dump_thread );
				sym_cleanup_( ::GetCurrentProcess() );
			}

			::FreeLibrary( imagehlp );
			imagehlp = NULL;
		}
		else
		{
#ifdef	_M_IX86		// Intel Only!
			//	Walk the stack using x86 specific code
			intel_stack_walk( file, context );
#endif
		}

		ffl_dump_print( file, FFL_T( "\r\n" ) );
	}

	return;
}

bool	ffl_dump_report::get_logical_address( void * addr, ffl_tchar_t * module, DWORD len, DWORD * section, DWORD * offset )
{
	MEMORY_BASIC_INFORMATION	memory_info;
	if( ::VirtualQuery( addr, &memory_info, sizeof(memory_info) ) == 0 )
	{
		return	false;
	}

	HMODULE	mod = reinterpret_cast< HMODULE >( memory_info.AllocationBase );
	if( ::GetModuleFileName( mod, module, len ) == 0 )
	{
		return	false;
	}

	IMAGE_DOS_HEADER * dos_header = reinterpret_cast< IMAGE_DOS_HEADER * >( mod );

	IMAGE_NT_HEADERS * nt_header = reinterpret_cast< IMAGE_NT_HEADERS * >( ((reinterpret_cast< char * >( dos_header )) + dos_header->e_lfanew) );

	PIMAGE_SECTION_HEADER section_header = IMAGE_FIRST_SECTION( nt_header );

	DWORD rva = static_cast< DWORD >( reinterpret_cast< char *>( addr ) - reinterpret_cast< char * >( mod ) ); // RVA is offset from module load address

	for( unsigned int i = 0; i < nt_header->FileHeader.NumberOfSections; ++i, ++section_header )
	{
		DWORD section_start = section_header->VirtualAddress;
		DWORD section_end = section_start + max( section_header->SizeOfRawData, section_header->Misc.VirtualSize );

		if( (rva >= section_start) && (rva <= section_end) )
		{
			*section = i+1;
			*offset = rva - section_start;
			return true;
		}
	}

	return false;   // Should never get here!
}

bool	ffl_dump_report::inti_imagehlp_func( HMODULE imagehlp )
{
	sym_initialize_ = (sym_initialize_f) ::GetProcAddress( imagehlp, "SymInitialize" );
	if( sym_initialize_ == NULL )
	{
		return false;
	}

	sym_cleanup_ = (sym_cleanup_f) ::GetProcAddress( imagehlp, "SymCleanup" );
	if( sym_cleanup_ == NULL )
	{
		return false;
	}

	stack_walk_ = (stack_walk_f) ::GetProcAddress( imagehlp, "StackWalk" );
	if( stack_walk_ == NULL )
	{
		return false;
	}

	sym_function_table_access_ = (sym_function_table_access_f) ::GetProcAddress( imagehlp, "SymFunctionTableAccess" );
	if( sym_function_table_access_ == NULL )
	{
		return false;
	}

	sym_get_module_base_ = (sym_get_module_base_f) ::GetProcAddress( imagehlp, "SymGetModuleBase" );
	if( sym_get_module_base_ == NULL )
	{
		return false;
	}

	sym_get_sym_from_addr_ = (sym_get_sym_from_addr_f) ::GetProcAddress( imagehlp, "SymGetSymFromAddr" );
	if( sym_get_sym_from_addr_ == NULL )
	{
		return false;
	}

	if( sym_initialize_( ::GetCurrentProcess(), 0, TRUE ) == FALSE )
	{
		return false;
	}

	return true;
}

void	ffl_dump_report::intel_stack_walk( ffl_text_file * file, PCONTEXT context )
{
	ffl_dump_print( file, FFL_T( "\r\nCall stack:\r\n" ) );
	ffl_dump_print( file, FFL_T( "Address   Frame     Logical addr  Module\r\n" ) );

	DWORD pc = context->Eip;
	PDWORD frame = NULL, prev_frame = NULL;
	frame = reinterpret_cast< PDWORD >( context->Ebp );

	do
	{
		ffl_tchar_t module[MAX_PATH] = { 0, };
		DWORD section = 0, offset = 0;

		get_logical_address( reinterpret_cast< void *>( pc ), module, FFL_COUNTOF( module ), &section, &offset );

		ffl_dump_print( file, FFL_T( "%08X  %08X  %04X:%08X %s\r\n" ), pc, frame, section, offset, module );

		pc = frame[1];
		prev_frame = frame;
		frame = reinterpret_cast< PDWORD >( frame[0] );

		if( reinterpret_cast< DWORD >( frame ) & 3 )
		{
			break;
		}

		if( frame <= prev_frame )
		{
			break;
		}

		if( ::IsBadWritePtr( frame, sizeof( void * )*2 ) )
		{
			break;
		}

#pragma	warning( push )
#pragma warning( disable : 4127 )	// 조건식이 상수입니다.
	} while( true );
#pragma	warning( pop )
}

void	ffl_dump_report::imagehlp_stack_walk( ffl_text_file * file, PCONTEXT context, HANDLE dump_thread )
{
	ffl_dump_print( file, FFL_T( "\r\nCall stack:\r\n" ) );
	ffl_dump_print( file, FFL_T( "Address   Frame\r\n" ) );


	STACKFRAME stack_frame;
	::memset( &stack_frame, 0, sizeof( stack_frame ) );

	stack_frame.AddrPC.Offset       = context->Eip;
	stack_frame.AddrPC.Mode         = AddrModeFlat;
	stack_frame.AddrStack.Offset    = context->Esp;
	stack_frame.AddrStack.Mode      = AddrModeFlat;
	stack_frame.AddrFrame.Offset    = context->Ebp;
	stack_frame.AddrFrame.Mode      = AddrModeFlat;

#pragma	warning( push )
#pragma warning( disable : 4127 )	// 조건식이 상수입니다.
	while( true )
#pragma	warning( pop )
	{
		if( stack_walk_( IMAGE_FILE_MACHINE_I386, ::GetCurrentProcess(), dump_thread, &stack_frame,
			context, 0, sym_function_table_access_, sym_get_module_base_, 0 ) == FALSE )
		{
			break;
		}

		if( 0 == stack_frame.AddrFrame.Offset )
		{
			break;
		}

		ffl_dump_print( file, FFL_T( "%08X  %08X  " ), stack_frame.AddrPC.Offset, stack_frame.AddrFrame.Offset );


		BYTE symbol_buffer[ sizeof( IMAGEHLP_SYMBOL ) + 512 ] = { 0, };
		PIMAGEHLP_SYMBOL symbol = reinterpret_cast< PIMAGEHLP_SYMBOL >( symbol_buffer );
		symbol->SizeOfStruct = sizeof( symbol_buffer );
		symbol->MaxNameLength = 512;

		DWORD sym_displacement = 0;

		if( sym_get_sym_from_addr_( ::GetCurrentProcess(), stack_frame.AddrPC.Offset, &sym_displacement, symbol ) == TRUE )
		{
			ffl_dump_print( file, FFL_T( "%hs+%X\r\n" ), symbol->Name, sym_displacement );
		}
		else
		{
			ffl_tchar_t module[MAX_PATH] = { 0, };
			DWORD section = 0, offset = 0;

			get_logical_address( reinterpret_cast< void * >( stack_frame.AddrPC.Offset ), module, FFL_COUNTOF( module ), &section, &offset );

			ffl_dump_print( file, FFL_T( "%04X:%08X %s\r\n" ), section, offset, module );
		}
	}
}

#endif