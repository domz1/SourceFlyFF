#ifndef __FFL_FILE_H__
#define __FFL_FILE_H__


#include "ffl_common.h"


class	ffl_file
{
	public:
		ffl_file();
		~ffl_file();

		bool		create( const ffl_tchar_t * name, unsigned int access_mode = GENERIC_READ | GENERIC_WRITE, unsigned int share_mode = FILE_SHARE_READ );
		bool		open( const ffl_tchar_t * name, unsigned int access_mode = GENERIC_READ | GENERIC_WRITE, unsigned int share_mode = FILE_SHARE_READ );

		void		close();

		bool		opened();

		size_t		read( void * buffer, size_t buffer_size );
		size_t		write( const void * buffer, size_t write_size );

		ffl_int64_t	seek( ffl_int64_t offset, int origin );
		bool		flush();

		ffl_uint64_t	size();


	private:
		HANDLE		handle_;
};


#endif	//__FFL_FILE_H__
