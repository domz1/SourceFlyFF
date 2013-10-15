#ifndef __FFL_DUMP_REPORT_H__
#define __FFL_DUMP_REPORT_H__


#include "ffl_dump_common.h"


class	ffl_dump_report
{
	public:
		ffl_dump_report();
		~ffl_dump_report();

		void	dump( const ffl_tchar_t * name, ffl_dump_param_t * param );

	private:

#if	defined( _X86_ ) && defined( _M_IX86 )

		void	report( ffl_text_file * file, ffl_dump_param_t * param );

		void	generate_exception( ffl_text_file * file, ffl_dump_param_t * param );
		bool	get_logical_address( void * addr, ffl_tchar_t * module, DWORD len, DWORD * section, DWORD * offset );
		bool	inti_imagehlp_func( HMODULE imagehlp );
		void	intel_stack_walk( ffl_text_file * file, PCONTEXT context );
		void	imagehlp_stack_walk( ffl_text_file * file, PCONTEXT context, HANDLE dump_thread );

	private:

		//////////////////////////////////////////////////////////////////////////
		typedef	BOOL	(__stdcall * sym_initialize_f)( HANDLE, LPSTR, BOOL );
		typedef	BOOL	(__stdcall * sym_cleanup_f)( HANDLE );

		typedef	BOOL	(__stdcall * stack_walk_f)( DWORD, HANDLE, HANDLE, LPSTACKFRAME, LPVOID,
			PREAD_PROCESS_MEMORY_ROUTINE, PFUNCTION_TABLE_ACCESS_ROUTINE,
			PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE );

		typedef	LPVOID	(__stdcall * sym_function_table_access_f)( HANDLE, DWORD );
		typedef	DWORD	(__stdcall * sym_get_module_base_f)( HANDLE, DWORD );
		typedef	BOOL	(__stdcall * sym_get_sym_from_addr_f)( HANDLE, DWORD, PDWORD, PIMAGEHLP_SYMBOL );
		//////////////////////////////////////////////////////////////////////////

#endif

		sym_initialize_f			sym_initialize_;
		sym_cleanup_f				sym_cleanup_;
		stack_walk_f				stack_walk_;
		sym_function_table_access_f	sym_function_table_access_;
		sym_get_module_base_f		sym_get_module_base_;
		sym_get_sym_from_addr_f		sym_get_sym_from_addr_;

};


#endif	//__FFL_DUMP_REPORT_H__
