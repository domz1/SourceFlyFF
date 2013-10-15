#ifndef __FFL_TEXT_FILE_H__
#define __FFL_TEXT_FILE_H__


#include "ffl_common.h"
#include "ffl_file.h"

class	ffl_text_file
{
	public:
		ffl_text_file();
		~ffl_text_file();

		bool		create( const ffl_tchar_t * name, unsigned int access_mode = GENERIC_READ | GENERIC_WRITE, unsigned int share_mode = FILE_SHARE_READ );
		bool		open( const ffl_tchar_t * name, unsigned int access_mode = GENERIC_READ | GENERIC_WRITE, unsigned int share_mode = FILE_SHARE_READ );

		void		close();

		bool		opened();

		size_t		write_text( const ffl_tchar_t * text, size_t count );

		size_t		read_text( ffl_tchar_t * text, size_t count );
		size_t		read_line( ffl_tchar_t * text, size_t count );

		bool		flush();
		ffl_uint64_t	size();


	private:
		ffl_file		file_;
};

#endif	//__FFL_TEXT_FILE_H__
