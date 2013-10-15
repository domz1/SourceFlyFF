#ifndef __FFL_DUMP_MINI_H__
#define __FFL_DUMP_MINI_H__


#include "ffl_dump_common.h"


class	ffl_dump_mini
{
	public:
		void	dump( const ffl_tchar_t * name, ffl_dump_param_t * param );


	private:
		HMODULE	find_dll();
		void	write( HANDLE file, ffl_dump_param_t * param );
};

#endif	//__FFL_DUMP_MINI_H__
