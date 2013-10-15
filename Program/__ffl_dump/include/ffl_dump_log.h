#ifndef __FFL_DUMP_LOG_H__
#define __FFL_DUMP_LOG_H__


#include "ffl_dump_common.h"


class	ffl_dump_log
{
	public:
		void	dump( const ffl_tchar_t * name, ffl_dump_param_t * param );

	private:

#if	defined( _X86_ ) && defined( _M_IX86 )
		void	log( ffl_text_file * file, ffl_dump_param_t * param );

		void	system_information_log( ffl_text_file * file );
		void	register_log( ffl_text_file * file, PCONTEXT context );
		void	stack_log( ffl_text_file * file, DWORD * stack );
		void	module_list_log( ffl_text_file * file );
		bool	module_info_log( ffl_text_file * file, HINSTANCE module, int module_num );
		void	print_module_info( ffl_text_file * file, const ffl_tchar_t * module_path );

#endif

};


#endif	//__FFL_DUMP_LOG_H__
