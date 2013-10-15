#ifndef __FFL_DUMP_UNEXCEPTION_FILTER_H__
#define __FFL_DUMP_UNEXCEPTION_FILTER_H__


#include "ffl_dump_common.h"


class	ffl_dump_unexception_filter
{
	public:
		static void		install( int level = ffl_dump_level_light );
		static void		snapping( int level = ffl_dump_level_light );


	private:
		static LONG WINAPI	top_level_filter( EXCEPTION_POINTERS * exception_ptr );
		static void			do_snapping( int level, EXCEPTION_POINTERS * exception_ptr );

	private:
		static int			dump_level_;

	public:
		ffl_dump_unexception_filter( int level = ffl_dump_level_light );
		~ffl_dump_unexception_filter();
};


#endif	//__FFL_DUMP_UNEXCEPTION_FILTER_H__

