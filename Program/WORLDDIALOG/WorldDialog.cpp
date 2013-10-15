#include "stdafx.h"
#include "NpcScript.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
/*	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		if( InitDLL( "WorldDialog.txt" ) == FALSE )
		{
		#ifdef _DEBUG
			MessageBox( NULL, "InitDLL return FALSE", "error", MB_OK );
		#else
			OutputDebugString( "InitDLL(WorldDialog.txt) return FALSE" );
		#endif
			return FALSE;
		}
	}
*/
    return TRUE;
}

INT APIENTRY RunDialog( LPCTSTR szNpc, LPCTSTR szKey, NPCDIALOG_INFO* pInfo )
{
#ifdef _DEBUG
	char szBuffer[256];
	wsprintf( szBuffer, "RunDialog(%s, %s)\n", szNpc, szKey );
	OutputDebugString( szBuffer );
#endif

	CNpcScript obj;
	obj.m_pInfo = pInfo;
	obj.DoExec( szNpc, szKey );
	return 1;
}

INT APIENTRY FindScriptKey( LPCTSTR szNpc, LPCTSTR szKey )
{
	CNpcScript obj;
	obj.m_pInfo = NULL;
	return obj.Find( szNpc, szKey );
}

void APIENTRY SetLanguage(INT nLanguage)
{
	CNpcScript script;
	script.SetLang(nLanguage);
}

BOOL APIENTRY InitWorldDialog()
{
	if( InitDLL( "WorldDialog.txt" ) == FALSE )
	{
	#ifdef _DEBUG
		MessageBox( NULL, "InitDLL return FALSE", "error", MB_OK );
	#else
		OutputDebugString( "InitDLL(WorldDialog.txt) return FALSE" );
	#endif
		return FALSE;
	}

	return TRUE;
}