

#include "../../include/ffl_dump_unexception_filter.h"

//#include "../../include/Rijndael.h"

#include <tchar.h>


#ifdef	_DEBUG
#pragma comment( lib, "ffl_dumpd.lib" )
#else
#pragma comment( lib, "ffl_dumpr.lib" )
#endif


int		main()
{

	TCHAR		szPassword[ 42 ] = "1fe469c9cbab1e0d6cce772f2d5e14ea";

	char		szPlain[ 16 * 42 ] = {0, };
	char		szEnc[ 16 * 42 ] = {0, };
	char		szDec[ 16 * 42 ] = {0, };


// 	::memcpy( szPlain, szPassword, sizeof(szPassword) );
// 
// 	CRijndael oRijndael;
// 
// 	char	szDefaultKey[32] = "dldhsthvmxm";
// 	oRijndael.MakeKey( szDefaultKey, CRijndael::sm_chain0 );
// 
// 	// iMode = CBC 
// 	oRijndael.ResetChain();
// 	oRijndael.Encrypt( szPlain, szEnc, 16 * 42, CRijndael::CBC );
// 
// 	// iMode = CBC 
// 	CRijndael oRijndael2;
// 
// 	oRijndael2.MakeKey( szDefaultKey, CRijndael::sm_chain0 );
// 
// 	int nLen	= _tcslen( szEnc );
// 	// iMode = CBC 
// 	oRijndael2.ResetChain();
// 	oRijndael2.Decrypt( szEnc, szDec, 16 * 42, CRijndael::CBC );

	ffl_dump_unexception_filter::install( ffl_dump_level_heavy );

	::strcpy( NULL, NULL );



	return 0;
}




// //////////////////////////////////////////////////////////////////////////
// //	BEGINTEST100111	FFL_DUMP
// #include "../__ffl_dump/include/ffl_dump_unexception_filter.h"
// 
// #ifdef	_DEBUG
// #pragma comment( lib, "../__lib/win32/msvc71/ffl_dumpd.lib" )
// #else
// #pragma comment( lib, "../__lib/win32/msvc71/ffl_dumpr.lib" )
// #endif
// //	ENDTEST100111	FFL_DUMP
// //////////////////////////////////////////////////////////////////////////


// 	//////////////////////////////////////////////////////////////////////////
// 	//	BEGINTEST100111	FFL_DUMP
//  	ffl_dump_unexception_filter::install( ffl_dump_level_heavy );
// 	//	ENDTEST100111	FFL_DUMP
// 	//////////////////////////////////////////////////////////////////////////