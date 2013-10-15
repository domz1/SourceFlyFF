#include "stdafx.h"
#include "NpcProperty.h"
#if defined(__REMOVE_SCIRPT_060712)
	#include "WorldDialog.h"
	#include "ScriptHelper.h"
	extern Functions* GetDialogFunctions();
#endif

CNpcProperty::CNpcProperty()
{
	m_dwTick = ::GetTickCount() + MIN( 1 ) + xRandom( SEC( 30 ) );
#if defined(__REMOVE_SCIRPT_060712)
	m_szName[0] = '\0';
#endif
}

CNpcProperty::~CNpcProperty()
{
}

BOOL CNpcProperty::IsTimeOut()
{
	return ::GetTickCount() > m_dwTick;
}

void CNpcProperty::ResetTimer()
{
	m_dwTick = ::GetTickCount() + SEC( 15 ) + SEC( xRandom( 10 ) );
}

BOOL CNpcProperty::IsDialogLoaded()
{
#if defined(__REMOVE_SCIRPT_060712)
	return m_szName[0] != '\0';
#else
	return m_Dialog.IsEmpty() == FALSE;
#endif
}

BOOL CNpcProperty::LoadDialog( LPCHARACTER lpCharacter )
{
	if( lpCharacter->m_szDialog[0] == '\0' )
		return FALSE;

#if defined(__REMOVE_SCIRPT_060712)
	int n = strlen( lpCharacter->m_szDialog ) - 4;
	if( n > 0 )
	{
		strcpy( m_szName, lpCharacter->m_szDialog );
		m_szName[n] = '\0';
		_tcslwr( m_szName );
		return TRUE;
	}
	else
		return FALSE;
#else	// __REMOVE_SCIRPT_060712

	char lpOutputString[100]	= { 0, };
	sprintf( lpOutputString, "dialog file: %s\n", lpCharacter->m_szDialog );
	OutputDebugString( lpOutputString );

	m_Dialog.Free();
	return m_Dialog.LoadScript( MakePath( DIR_DIALOG, ::GetLanguage(), lpCharacter->m_szDialog ) );
#endif	// not __REMOVE_SCIRPT_060712
}

BOOL CNpcProperty::RunDialog( LPCTSTR szKey, int* pResult, int nValue, int nDstId, int nSrcId, int nQuestId )
{
#if! defined(__REMOVE_SCIRPT_060712)
	return m_Dialog.Run( szKey, pResult, nValue, nDstId, nSrcId, nQuestId );
#else
	if( m_szName[0] == 0 )		// character.inc에서 dialog 이름을 등록하지 않은 경우 대사가 없다고 간주.
		return TRUE;

	if( pResult )
		*pResult = 1;

	CNpcDialogInfo info;
	info.m_nValue    = nValue;
	info.m_nSrcId    = nSrcId;
	info.m_nDstId    = nDstId;
	info.m_nQuestId  = nQuestId;
	info.m_pszName   = m_szName; 
	info.m_pnGlobal  = &m_nGlobal;
	info.m_pFuctions = GetDialogFunctions();

	if( RunPredefineKey( szKey, info ) == TRUE )
		return TRUE;

	CWorldDialog::GetInstance().Run( m_szName, szKey, &info );
	return TRUE;
#endif
}

