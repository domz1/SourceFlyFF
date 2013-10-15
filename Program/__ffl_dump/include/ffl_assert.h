#ifndef __FFL_ASSERT_H__
#define __FFL_ASSERT_H__


#include "ffl_common.h" 


//////////////////////////////////////////////////////////////////////////
bool	ffl_assert_function_a(	bool			exp,
								const char *	exp_string,
								const char *	description,
								const char *	function_name,
								const char *	file,
								int				line,
								bool *			ignore_always );

bool	ffl_assert_function_w(	bool			exp,
								const wchar_t *	exp_string,
								const wchar_t *	description,
								const wchar_t *	function_name,
								const wchar_t *	file,
								int				line,
								bool *			ignore_always );

#if defined(_UNICODE) || defined(UNICODE)
	#define ffl_assert_function	ffl_assert_function_w
#else
	#define ffl_assert_function	ffl_assert_function_a
#endif
//////////////////////////////////////////////////////////////////////////


//	DebugBreak() function require Windows 2000 or later
#define ffl_assert( exp, description )							\
	{															\
		static bool	ignore_always = false;						\
		if( ignore_always == false )							\
		{														\
			if( ffl_assert_function( (exp),						\
									FFL_T( #exp ),				\
									FFL_T( description ),		\
									FFL_FUNCTION,				\
									FFL_FILE,					\
									FFL_LINE,					\
									&ignore_always ) == false ) \
			{													\
				::DebugBreak();									\
			}													\
		}														\
	}

//////////////////////////////////////////////////////////////////////////
#ifndef	FFL_ASSERT

#ifdef	FFL_DEBUG
	#define FFL_ASSERT( exp )	ffl_assert( !!(exp), #exp )
#else
	#define FFL_ASSERT( exp )	( (void)( 0 ) )
#endif	//FFL_DEBUG

#endif	//FFL_ASSERT
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#ifndef	FFL_VERIFY

#ifdef	FFL_DEBUG
	#define	FFL_VERIFY( exp )	FFL_ASSERT( exp )
#else
	#define	FFL_VERIFY( exp )	if( (exp) )	{}
#endif	//FFL_DEBUG

#endif	//FFL_VERIFY
//////////////////////////////////////////////////////////////////////////


#endif	//__FFL_ASSERT_H__
