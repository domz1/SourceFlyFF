#include "../include/ffl_text_file.h"


#if defined(_UNICODE) || defined(UNICODE)
	static const unsigned char ffl_utf16_little_endian_code[]	= { 0xFF, 0xFE, 0x00 };
#endif

ffl_text_file::ffl_text_file()
{
}

ffl_text_file::~ffl_text_file()
{
	close();
}

bool	ffl_text_file::create( const ffl_tchar_t * name, unsigned int access_mode /* = GENERIC_READ | GENERIC_WRITE */, unsigned int share_mode /* = FILE_SHARE_READ */ )
{
	FFL_ASSERT( name != NULL );

	if( name != NULL )
	{
		if( file_.create( name, access_mode, share_mode ) == false )
		{
			return	false;
		}

#if defined(_UNICODE) || defined(UNICODE)
		FFL_VERIFY( file_.write( ffl_utf16_little_endian_code, sizeof(ffl_utf16_little_endian_code) - sizeof(unsigned char) ) == ( sizeof(ffl_utf16_little_endian_code) - sizeof(unsigned char) ) );
#endif

		return	true;
	}

	return	false;
}

bool	ffl_text_file::open( const ffl_tchar_t * name, unsigned int access_mode /* = GENERIC_READ | GENERIC_WRITE */, unsigned int share_mode /* = FILE_SHARE_READ */ )
{
	FFL_ASSERT( name != NULL );

	if( name != NULL )
	{
		if( file_.open( name, access_mode, share_mode ) == false )
		{
			return	false;
		}

#if defined(_UNICODE) || defined(UNICODE)
		unsigned char	endian_code[ sizeof(ffl_utf16_little_endian_code) ] = { 0, };

		FFL_VERIFY( file_.read( endian_code, sizeof(ffl_utf16_little_endian_code) - sizeof(unsigned char) ) == ( sizeof(ffl_utf16_little_endian_code) - sizeof(unsigned char) ) );
		if( ::memcmp( endian_code, ffl_utf16_little_endian_code, sizeof(ffl_utf16_little_endian_code) - sizeof(unsigned char) ) != 0 )
		{
			file_.seek( 0, FILE_BEGIN );
		}
#endif

		return	true;
	}

	return	false;
}

void	ffl_text_file::close()
{
	file_.close();

	return;
}

bool	ffl_text_file::opened()
{
	return	( file_.opened() );
}

size_t	ffl_text_file::write_text( const ffl_tchar_t * text, size_t count )
{
	FFL_ASSERT( text != NULL );
	FFL_ASSERT( count > 0 );
	FFL_ASSERT( opened() == true );

	size_t	written_size = 0;

	if( text != NULL && count > 0 && opened() == true )
	{
		written_size	= file_.write( text, (count * sizeof(ffl_tchar_t)) );
	}

	return	( written_size / sizeof(ffl_tchar_t) );
}

size_t	ffl_text_file::read_text( ffl_tchar_t * text, size_t count )
{
	FFL_ASSERT( text != NULL );
	FFL_ASSERT( count > 0 );
	FFL_ASSERT( opened() == true );

	size_t	read_size = 0;

	if( text != NULL && count > 0 && opened() == true )
	{
		read_size	= file_.read( text, (count * sizeof(ffl_tchar_t)) );
	}

	return	( read_size / sizeof(ffl_tchar_t) );
}

size_t	ffl_text_file::read_line( ffl_tchar_t * text, size_t count )
{
	FFL_ASSERT( text != NULL );
	FFL_ASSERT( count > 0 );
	FFL_ASSERT( opened() == true );

	size_t	read_count = 0;

	if( text != NULL && count > 0 && opened() == true )
	{
		ffl_tchar_t	one_char = 0;

		while( file_.read( &one_char, sizeof(one_char) ) == sizeof(one_char) )
		{
			if( one_char != FFL_T( '\r' ) && one_char != FFL_T( '\n' ) )
			{
				if( count > read_count )
				{
					text[ read_count ] = one_char;
					++read_count;
				}
			}
			else
			{
				file_.read( &one_char, sizeof(one_char) );

				if( one_char != FFL_T( '\r' ) && one_char != FFL_T( '\n' ) )
				{
					file_.seek( (static_cast< ffl_int64_t >( -1 ) ) * sizeof(one_char), FILE_CURRENT );
				}

				break;
			}
		}

		if( count > read_count )
		{
			text[ read_count ] = FFL_T( '\0' );
		}
	}

	return	( read_count );
}

bool	ffl_text_file::flush()
{
	return	( file_.flush() );
}

ffl_uint64_t	ffl_text_file::size()
{
	return	( file_.size() );
}


