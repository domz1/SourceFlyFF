#include "../include/ffl_file.h"

ffl_file::ffl_file()
{
	handle_	= INVALID_HANDLE_VALUE;
}

ffl_file::~ffl_file()
{
	close();
}

bool	ffl_file::create( const ffl_tchar_t * name, unsigned int access_mode /* = GENERIC_READ | GENERIC_WRITE */, unsigned int share_mode /* = FILE_SHARE_READ */ )
{
	FFL_ASSERT( opened() == false );

	handle_	= ::CreateFile( name, access_mode, share_mode, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( handle_ == INVALID_HANDLE_VALUE )
	{
		return	false;
	}

	return	true;
}

bool	ffl_file::open( const ffl_tchar_t * name, unsigned int access_mode /* = GENERIC_READ | GENERIC_WRITE */, unsigned int share_mode /* = FILE_SHARE_READ */ )
{
	FFL_ASSERT( opened() == false );

	handle_	= ::CreateFile( name, access_mode, share_mode, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( handle_ == INVALID_HANDLE_VALUE )
	{
		return	false;
	}

	return	true;
}

void	ffl_file::close()
{
	if( handle_ != INVALID_HANDLE_VALUE )
	{
		FFL_VERIFY( ::CloseHandle( handle_ ) != FALSE );
		handle_	= INVALID_HANDLE_VALUE;
	}

	return;
}

bool	ffl_file::opened()
{
	return	( handle_ != INVALID_HANDLE_VALUE );
}

size_t	ffl_file::read( void * buffer, size_t buffer_size )
{
	FFL_ASSERT( opened() == true );
	FFL_ASSERT( buffer_size != NULL );

	DWORD	read_size = 0;

	if( opened() == true && buffer != NULL )
	{
		if( ::ReadFile( handle_, buffer, static_cast< DWORD >( buffer_size ), &read_size, NULL ) == FALSE )
		{
			read_size	= 0;
		}
	}

	return	( read_size );
}

size_t	ffl_file::write( const void * buffer, size_t write_size )
{
	FFL_ASSERT( opened() == true );
	FFL_ASSERT( buffer != NULL );

	DWORD	written_size = 0;

	if( opened() == true )
	{
		if( ::WriteFile( handle_, buffer, static_cast< DWORD >( write_size ), &written_size, NULL ) == FALSE )
		{
			written_size	= 0;
		}
	}

	return	( written_size );
}

ffl_int64_t	ffl_file::seek( ffl_int64_t offset, int origin )
{
	FFL_ASSERT( opened() == true );

	LARGE_INTEGER	large;
	large.QuadPart	= 0;

	if( opened() == true )
	{
		large.QuadPart	= offset;
		large.LowPart	= ::SetFilePointer( handle_, large.LowPart, &large.HighPart, origin );

		if( large.LowPart == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR )
		{
			large.QuadPart	= INVALID_SET_FILE_POINTER;
		}
	}

	return	( large.QuadPart );
}

bool	ffl_file::flush()
{
	FFL_ASSERT( opened() == true );

	if( opened() == true )
	{
		if( ::FlushFileBuffers( handle_ ) == FALSE )
		{
			return	false;
		}

		return	true;
	}

	return	false;
}

ffl_uint64_t	ffl_file::size()
{
	FFL_ASSERT( opened() == true );

	ULARGE_INTEGER	large;
	large.QuadPart	= 0;

	if( opened() == true )
	{
		large.LowPart	= ::GetFileSize( handle_, &large.HighPart );

		if( large.LowPart == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR )
		{
			large.QuadPart	= 0;
		}
	}

	return	( large.QuadPart );
}