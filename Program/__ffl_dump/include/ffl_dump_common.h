#ifndef __FFL_DUMP_COMMON_H__
#define __FFL_DUMP_COMMON_H__


#include "ffl_common.h"
#include "ffl_text_file.h"
#include "ffl_assert.h"
#include "ffl_module_info.h"

#include <DbgHelp.h>


enum
{
	ffl_dump_level_none
	, ffl_dump_level_light
	, ffl_dump_level_medium
	, ffl_dump_level_heavy
};

struct	ffl_dump_param_t
{
	int						level;
	HANDLE					dump_thread;
	DWORD					dump_thread_id;
	EXCEPTION_POINTERS *	exception_ptr;
};

void	ffl_get_win_version( ffl_tchar_t * version, size_t version_count, ffl_tchar_t * build_num, size_t build_num_count );
void	ffl_formating_time( ffl_tchar_t * output, size_t output_count, FILETIME * time );
const ffl_tchar_t *	ffl_get_exception_string( DWORD code );
void	ffl_dump_print( ffl_text_file * file, const ffl_tchar_t * format, ... );

#endif	//__FFL_DUMP_COMMON_H__
