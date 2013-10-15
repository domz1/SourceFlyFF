#include "stdafx.h"
#include "BillingMgr.h"
#include "BillingMgrJP.h"
#include "BillingMgrJP2.h"
#include "BillingMgrTW.h"
#include "BillingMgrTH.h"
#include "query.h"

///////////////////////////////////////////////////////////////////////
// global variable
///////////////////////////////////////////////////////////////////////

static			CBillingMgr* g_pBillingMgr = NULL;	
static char		g_szBillingPWD[256];

///////////////////////////////////////////////////////////////////////
// global function
///////////////////////////////////////////////////////////////////////

// strcpy와 같은기능 하지만, NULL이전의 포인터를 리턴한다.
char* StrCpyExcludeNull( char* dst, const char* src )
{
	char *cp = dst;

	while( *cp++ = *src++ )
		   ; 
	return( cp - 1 );
}

// pCur에서 pEnd까지 공백(0x20)을 붙인다.
void AppendSpace( char* pCur, char* pEnd )
{
	if( pEnd - pCur )
		memset( pCur, 0x20, pEnd - pCur );
}

char* GetBillingPWD()
{
	return g_szBillingPWD;
}

// TODO: LoadLibrary
BOOL CreateBillingMgr()
{
#ifdef __BILLING2_041021						// 디비방식 빌링 
	#if defined(__BILLING_TW)					
		g_pBillingMgr = new CBillingMgrTW;		// 대만 	
	#elif defined(__BILLING_JP)
		g_pBillingMgr = new CBillingMgrJP2;		// 일본 	
	#else
		#error BILLING TARGET MUST BE DEFINED.	// 빌링은 나라별로 디파인을 꼭 넣어야한다.
	#endif
#else
	#if defined(__BILLING_TH)
		g_pBillingMgr = new CBillingMgrTH;		// 태국 
	#elif defined(__BILLING_JP)
		g_pBillingMgr = new CBillingMgrJP;	
	#endif
#endif

	return TRUE;
}

CBillingMgr* GetBillingMgr()
{
	ASSERT( g_pBillingMgr );
	return g_pBillingMgr;
}
