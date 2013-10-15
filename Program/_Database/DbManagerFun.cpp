#include "StdAfx.h"
#include "defineObj.h"
#include "dbmanager.h"
#include "dploginsrvr.h"
#include "dpcoresrvr.h"
#include "dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include "mytrace.h"
#include "definequest.h"

extern	BOOL CheckValidItem( DWORD dwItemId, short nNum );

#include "party.h"
#include "guild.h"
extern	CGuildMng	g_GuildMng;

#include "guildwar.h"
extern	CGuildWarMng	g_GuildWarMng;

#if __VER >= 13 // __HONORABLE_TITLE
#include "honor.h"
#endif	// __HONORABLE_TITLE

extern const int MAX_TASKBAR; 
extern const int MAX_APPLETTASKBAR; 
extern const int MAX_ITEMTASKBAR;   
extern const char NullStr[2];


#include "eveschool.h"

#if defined( __VERIFY_PLAYER ) || defined( __PROVIDE ) || defined( __S0707_ITEM_CONV ) || defined(__RECOVER0816)
#define	MAX_QUERY_SIZE	1024 * 64
#include "resource.h"
BOOL	CALLBACK	VerifyPlayerDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );
#endif	// __VERIFY_PLAYER OR __PROVIDE

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	CMyTrace			g_MyTrace;

#define	VERIFYSTRING( lpString, lpszPlayer )	\
		if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszPlayer, lpDbOverlappedPlus ) )		return;

#define	VERIFY_GUILD_STRING( lpString, lpszGuild )	\
if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszGuild ) )		return;

// mirchang_100416 VERIFYSTRING use return value
#define	VERIFYSTRING_RETURN( lpString, lpszPlayer )	\
	if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszPlayer, lpDbOverlappedPlus ) )		return FALSE;

#define	VERIFYSTRING_BANK( lpString, lpszPlayer )	\
	if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszPlayer ) )		return FALSE;
// mirchang_100416

void CDbManager::GetStrTime( CTime *time, const char *strbuf )
{
	char cYear[5] = {0,};
	char cMonth[3] = {0,};
	char cDay[3] = {0,};
	char cHour[3] = {0,};
	char cMin[3] = {0,};
	
	strncpy(cYear, strbuf, 4);
	strncpy(cMonth, strbuf + 4, 2 );
	strncpy(cDay, strbuf + 6, 2 );
	strncpy(cHour, strbuf + 8, 2 );
	strncpy(cMin, strbuf + 10, 2 );
	
	CTime Timebuf( atoi(cYear), atoi(cMonth), atoi(cDay), atoi(cHour), atoi(cMin), 0 );
	*time = Timebuf;
}

#ifdef __PET_1024
void CDbManager::GetDBFormatStr( char* szDst, int nMaxLen, const char* szSrc )
{
	char szDigit[3] = {0, };
#ifdef __VS2003
	int ch2;
#else	// __VS2003
	char ch;
#endif // __VS2003
	char* pCur	= const_cast<char*>( szSrc );
	int nLen	= strlen( pCur );
	if( nLen % 2 != 0 )
	{
		pCur[nLen-1]	= '\0';
		nLen--;
	}
	if( nLen >= nMaxLen * 2 )
	{
		Error( "GetDBFormatStr: nLen = %d, nMaxLen = %d", nLen, nMaxLen );
		*szDst	= '\0';
		return;
	}
	while( *pCur )
	{
		szDigit[0]	= pCur[0];
		szDigit[1]	= pCur[1];
		pCur	+= 2;
#ifdef __VS2003
		sscanf( szDigit, "%2X", &ch2 );
		*szDst++	= ch2;
#else // __VS2003
		sscanf( szDigit, "%2X", &ch );
		*szDst++	= ch;
#endif // __VS2003
	}
	*szDst	= '\0';
}

void CDbManager::SetDBFormatStr( char* szDst, int nMaxLen, const char* szSrc )
{
	int nLen	= strlen( szSrc );
	if( nMaxLen < nLen * 2 + 1 )
	{
		Error( "SetDBFormatStr: nLen = %d, nMaxLen = %d", nLen, nMaxLen );
		*szDst	= '\0';
		return;
	}

	char* pCur	= szDst;
	while( *szSrc )
	{
		sprintf( pCur, "%2X",  (BYTE)*szSrc );
		pCur	+= 2;
		szSrc++;
	}
	*pCur	= '\0';
}
#endif	// __PET_1024


// szSrc - 16진수로 변환된 문자열, szDst - ascii문자열, n - szSrc에서 현재작업 index 
void CDbManager::GetStrFromDBFormat( char* szDst, const char* szSrc, int& n )
{
	char szDigit[3] = {0, };
	char ch;
#ifdef __VS2003
	int ch2;
#endif // __VS2003
	
	const char* pCur = szSrc + n;
	while( *pCur != '/' && *pCur )		// 문자열은 '/' 로 끝난다. 안전하게 NULL도 검사 
	{
		szDigit[0] = pCur[0];
		szDigit[1] = pCur[1];
		pCur += 2;
		
#ifdef __VS2003
		sscanf( szDigit, "%2X", &ch2 );
		ch = ch2;
#else // __VS2003
		sscanf( szDigit, "%2X", &ch );
#endif // __VS2003
		*szDst++ = ch;
	}
	*szDst = '\0';
	n = (pCur - szSrc) + 1;				// +1은 '/'를 무시하기 위해서 
}

BOOL CDbManager::GetBank( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, int nSlot )
{
	int CountStr				= 0;
	int itemCount				= 0;
	int	IndexItem				= 0;
	int IndexObjIndex			= 0;
	int nExtBankCount			= 0;
	int nPirecingBankCount		= 0;
	char Bank[7500]				= { 0, };
	char m_apIndexBank[512]		= { 0, };
	char ObjIndexBank[512]		= { 0, };
	char ExtBank[2000]			= {0,};	
	char PirecingBank[4000]		= {0,};
#ifdef __PET_1024
	char szPet[4200]	= { 0,};
#else	// __PET_1024
	char szPet[2688]	= { 0,};
#endif	// __PET_1024

	u_long uPlayerid	= (u_long)qry->GetInt( "m_idPlayer" );
	DWORD dwGold = qry->GetInt( "m_dwGoldBank" );

	qry->GetStr( "m_Bank", Bank );
	qry->GetStr( "m_apIndex_Bank", m_apIndexBank );
	qry->GetStr( "m_dwObjIndex_Bank", ObjIndexBank );
	qry->GetStr( "m_extBank", ExtBank );
	qry->GetStr( "m_BankPiercing", PirecingBank );
	qry->GetStr( "szBankPet", szPet );

	VERIFYSTRING_RETURN( Bank, lpDbOverlappedPlus->AccountInfo.szPlayer );
	VERIFYSTRING_RETURN( m_apIndexBank, lpDbOverlappedPlus->AccountInfo.szPlayer );
	VERIFYSTRING_RETURN( ObjIndexBank, lpDbOverlappedPlus->AccountInfo.szPlayer );
	VERIFYSTRING_RETURN( ExtBank, lpDbOverlappedPlus->AccountInfo.szPlayer );
	VERIFYSTRING_RETURN( PirecingBank, lpDbOverlappedPlus->AccountInfo.szPlayer );
	VERIFYSTRING_RETURN( szPet, lpDbOverlappedPlus->AccountInfo.szPlayer );
	
	pMover->m_idPlayerBank[nSlot] = uPlayerid;
	pMover->m_dwGoldBank[nSlot]	= dwGold;
	
	CountStr	= 0;	IndexItem	= 0;			
	while( '$' != Bank[CountStr] )
	{
		CItemElem BufItemElem;
		IndexItem = GetOneItem( &BufItemElem, Bank, &CountStr );
		if( IndexItem == -1 )
		{
			Error( "Join::Bank : << Not Prop. %d, %d \nBank = %s", uPlayerid, BufItemElem.m_dwItemId, Bank );
		}
		else
		{
			if( IndexItem >= MAX_BANK )
			{
				Error( "Join::Bank : << MAX_BANK %d\nBank = %s", uPlayerid, Bank );
				return FALSE;
			}
			pMover->m_Bank[nSlot].m_apItem[IndexItem] = BufItemElem;
		}
	}
	
	CountStr	= 0;	itemCount	= 0;
	while( '$' != m_apIndexBank[CountStr] )
	{
		pMover->m_Bank[nSlot].m_apIndex[itemCount]						= (DWORD)GetIntFromStr( m_apIndexBank, &CountStr );
		itemCount++;
	}
	
	CountStr	= 0;	IndexObjIndex	= 0;
	while( '$' != ObjIndexBank[CountStr] )
	{
		pMover->m_Bank[nSlot].m_apItem[IndexObjIndex].m_dwObjIndex		= (DWORD)GetIntFromStr( ObjIndexBank, &CountStr );
		IndexObjIndex++;
	}

	CountStr	= 0;	nExtBankCount	= 0;
	while( '$' != ExtBank[CountStr] )
	{
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_bCharged			= (BOOL)GetIntPaFromStr( ExtBank, &CountStr );
		if( pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_bCharged != 1 )
			pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_bCharged	= 0;
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_dwKeepTime		= (DWORD)GetIntPaFromStr( ExtBank, &CountStr );
#if __VER >= 11 // __SYS_IDENTIFY
		__int64 iRandomOptItemId	= GetInt64PaFromStr( ExtBank, &CountStr );
		if( iRandomOptItemId == -102 )
			iRandomOptItemId	= 0;
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].SetRandomOptItemId( iRandomOptItemId );
#else	// __SYS_IDENTIFY
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].SetRandomOptItemId( (DWORD)GetIntPaFromStr( ExtBank, &CountStr ) );
#endif	// __SYS_IDENTIFY
#if __VER >= 15 // __PETVIS
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_bTranformVisPet = static_cast<BOOL>( GetIntPaFromStr( ExtBank, &CountStr ) );
#endif // __PETVIS
		++CountStr;

		++nExtBankCount;
	}

	CountStr	= 0;	nPirecingBankCount = 0;
	while( '$' != PirecingBank[CountStr] )
	{
#if __VER >= 12 // __EXT_PIERCING
		LoadPiercingInfo( pMover->m_Bank[nSlot].m_apItem[nPirecingBankCount], PirecingBank, &CountStr );
#else // __EXT_PIERCING
		int nItemIdCount = 0;
		pMover->m_Bank[nSlot].m_apItem[nPirecingBankCount].SetPiercingSize( GetIntFromStr( PirecingBank, &CountStr ) );
		for( int i = 0 ; i < pMover->m_Bank[nSlot].m_apItem[nPirecingBankCount].GetPiercingSize() ; ++i )
		{
			pMover->m_Bank[nSlot].m_apItem[nPirecingBankCount].SetPiercingItem( nItemIdCount, (DWORD)GetIntFromStr( PirecingBank, &CountStr ) );
			++nItemIdCount;
		}
#endif // __EXT_PIERCING
		++nPirecingBankCount;
	}

	CountStr	= 0;
	int	nId	= 0;
	while( '$' != szPet[CountStr] )
	{
		BOOL bPet	= (BOOL)GetIntFromStr( szPet, &CountStr );
		if( bPet )
		{
			SAFE_DELETE( pMover->m_Bank[nSlot].m_apItem[nId].m_pPet );
			CPet* pPet	= pMover->m_Bank[nSlot].m_apItem[nId].m_pPet		= new CPet;
			BYTE nKind	= (BYTE)GetIntFromStr( szPet, &CountStr );
			pPet->SetKind( nKind );
			BYTE nLevel	= (BYTE)GetIntFromStr( szPet, &CountStr );
			pPet->SetLevel( nLevel );
			DWORD dwExp	= (DWORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetExp( dwExp );
			WORD wEnergy	= (WORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetEnergy( wEnergy );
#ifdef __PET_1024
			WORD wLife	= (WORD)GetIntPaFromStr( szPet, &CountStr );
			pPet->SetLife( wLife );
			for( int i = PL_D; i <= pPet->GetLevel(); i++ )
			{
				BYTE nAvailLevel	= (BYTE)GetIntPaFromStr( szPet, &CountStr );
				pPet->SetAvailLevel( i, nAvailLevel );
			}
			char szFmt[MAX_PET_NAME_FMT]	= { 0,};
			GetStrFromStr( szPet, szFmt, &CountStr );
			char szName[MAX_PET_NAME]	= { 0,};
			GetDBFormatStr( szName, MAX_PET_NAME, szFmt );
			pPet->SetName( szName );
#else	// __PET_1024
			WORD wLife	= (WORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetLife( wLife );
			for( int i = PL_D; i <= pPet->GetLevel(); i++ )
			{
				BYTE nAvailLevel	= (BYTE)GetIntFromStr( szPet, &CountStr );
				pPet->SetAvailLevel( i, nAvailLevel );
			}
#endif	// __PET_1024
		}
		nId++;
	}
	return TRUE;
}

BOOL CDbManager::GetBankMover( CMover* pMover, CQuery *qry, int nSlot )
{
	int CountStr	= 0;
	int itemCount	= 0;
	int	IndexItem	= 0;
	int IndexObjIndex	= 0;
	int nExtBankCount = 0;
	int nPirecingBankCount = 0;
	char Bank[7500]		= { 0, };
	char m_apIndexBank[512]		= { 0, };
	char ObjIndexBank[512]	= { 0, };
	char ExtBank[2000] = {0,};	
	char PirecingBank[4000] = {0,};
#ifdef __PET_1024
	char szPet[4200]	= { 0,};
#else	// __PET_1024
	char szPet[2688]	= { 0,};
#endif	// __PET_1024
	
	qry->GetStr( "m_Bank", Bank );
	qry->GetStr( "m_apIndex_Bank", m_apIndexBank );
	qry->GetStr("m_dwObjIndex_Bank", ObjIndexBank );
	qry->GetStr( "m_extBank", ExtBank );
	qry->GetStr( "m_BankPiercing", PirecingBank );
	qry->GetStr( "szBankPet", szPet );

	CountStr	= 0;	IndexItem	= 0;			
	VERIFYSTRING_BANK( Bank, pMover->m_szName );
	while( '$' != Bank[CountStr] )
	{
		CItemElem BufItemElem;
		IndexItem = GetOneItem( &BufItemElem, Bank, &CountStr );
		if( IndexItem == -1 )
		{
			Error( "Bank = %s", Bank );
		}
		else
		{
			if( IndexItem >= MAX_BANK )
			{
				Error( "Bank = %s", Bank );
				return FALSE;
			}
			pMover->m_Bank[nSlot].m_apItem[IndexItem] = BufItemElem;
		}
	}
	
	CountStr	= 0;	itemCount	= 0;
	VERIFYSTRING_BANK( m_apIndexBank, pMover->m_szName );
	while( '$' != m_apIndexBank[CountStr] )
	{
		pMover->m_Bank[nSlot].m_apIndex[itemCount]		= (DWORD)GetIntFromStr( m_apIndexBank, &CountStr );
		itemCount++;
	}
	
	CountStr	= 0;	IndexObjIndex	= 0;
	VERIFYSTRING_BANK( ObjIndexBank, pMover->m_szName );
	while( '$' != ObjIndexBank[CountStr] )
	{
		pMover->m_Bank[nSlot].m_apItem[IndexObjIndex].m_dwObjIndex		= (DWORD)GetIntFromStr( ObjIndexBank, &CountStr );
		IndexObjIndex++;
	}
	
	CountStr	= 0; 	nExtBankCount	= 0;
	VERIFYSTRING_BANK( ExtBank, pMover->m_szName );
	while( '$' != ExtBank[CountStr] )
	{
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_bCharged			= (BOOL)GetIntPaFromStr( ExtBank, &CountStr );
		if( pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_bCharged != 1 )
			pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_bCharged	= 0;
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_dwKeepTime		= (DWORD)GetIntPaFromStr( ExtBank, &CountStr );
#if __VER >= 11 // __SYS_IDENTIFY
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].SetRandomOptItemId( GetInt64PaFromStr( ExtBank, &CountStr ) );
#else	// __SYS_IDENTIFY
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].SetRandomOptItemId( (DWORD)GetIntPaFromStr( ExtBank, &CountStr ) );
#endif	// __SYS_IDENTIFY
#if __VER >= 15 // __PETVIS
		pMover->m_Bank[nSlot].m_apItem[nExtBankCount].m_bTranformVisPet = static_cast<BOOL>( GetIntPaFromStr( ExtBank, &CountStr ) );
#endif // __PETVIS
		++CountStr;
		++nExtBankCount;
	}
	
	CountStr	= 0;
	nPirecingBankCount = 0;
	VERIFYSTRING_BANK( PirecingBank, pMover->m_szName );
	while( '$' != PirecingBank[CountStr] )
	{
#if __VER >= 12 // __EXT_PIERCING
		LoadPiercingInfo( pMover->m_Bank[nSlot].m_apItem[nPirecingBankCount], PirecingBank, &CountStr );
#else // __EXT_PIERCING
		int nItemIdCount = 0;
		pMover->m_Bank[nSlot].m_apItem[nPirecingBankCount].SetPiercingSize( GetIntFromStr( PirecingBank, &CountStr ) );
		for( int i = 0 ; i < pMover->m_Bank[nSlot].m_apItem[nPirecingBankCount].GetPiercingSize(); ++i )
		{
			pMover->m_Bank[nSlot].m_apItem[nPirecingBankCount].SetPiercingItem( nItemIdCount, (DWORD)GetIntFromStr( PirecingBank, &CountStr ) );
			++nItemIdCount;
		}
#endif // __EXT_PIERCING
		++nPirecingBankCount;
	}

	CountStr	= 0;
	int	nId	= 0;
	VERIFYSTRING_BANK( szPet, pMover->m_szName );
	while( '$' != szPet[CountStr] )
	{
		BOOL bPet	= (BOOL)GetIntFromStr( szPet, &CountStr );
		if( bPet )
		{
			SAFE_DELETE( pMover->m_Bank[nSlot].m_apItem[nId].m_pPet );
			CPet* pPet	= pMover->m_Bank[nSlot].m_apItem[nId].m_pPet		= new CPet;
			BYTE nKind	= (BYTE)GetIntFromStr( szPet, &CountStr );
			pPet->SetKind( nKind );
			BYTE nLevel	= (BYTE)GetIntFromStr( szPet, &CountStr );
			pPet->SetLevel( nLevel );
			DWORD dwExp	= (DWORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetExp( dwExp );
			WORD wEnergy	= (WORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetEnergy( wEnergy );
#ifdef __PET_1024
			WORD wLife	= (WORD)GetIntPaFromStr( szPet, &CountStr );
			pPet->SetLife( wLife );
			for( int i = PL_D; i <= pPet->GetLevel(); i++ )
			{
				BYTE nAvailLevel	= (BYTE)GetIntPaFromStr( szPet, &CountStr );
				pPet->SetAvailLevel( i, nAvailLevel );
			}
			char szFmt[MAX_PET_NAME_FMT]	= { 0,};
			GetStrFromStr( szPet, szFmt, &CountStr );
			char szName[MAX_PET_NAME]	= { 0,};
			GetDBFormatStr( szName, MAX_PET_NAME, szFmt );
			pPet->SetName( szName );
#else	// __PET_1024
			WORD wLife	= (WORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetLife( wLife );
			for( int i = PL_D; i <= pPet->GetLevel(); i++ )
			{
				BYTE nAvailLevel	= (BYTE)GetIntFromStr( szPet, &CountStr );
				pPet->SetAvailLevel( i, nAvailLevel );
			}
#endif	// __PET_1024
		}
		nId++;
	}
	return TRUE;
}

void CDbManager::GetGuildBank( CItemContainer< CItemElem  >*  GuildBank, CQuery *qry )
{
	int CountStr		= 0;
	int IndexItem		= 0;
	char	Bank[7500]		= { 0, };
	qry->GetStr( "m_GuildBank", Bank );
	while( '$' != Bank[CountStr] )
	{
		CItemElem BufItemElem;
		IndexItem = GetOneItem( &BufItemElem, Bank, &CountStr );
		if( IndexItem == -1 )
		{
			
		}
		else
		{
			if( IndexItem >= MAX_GUILDBANK )
			{
				Error( "GuildBank = %s", Bank );
				return;
			}
			GuildBank->m_apItem[IndexItem] = BufItemElem;
		}
	}
	CountStr	= 0;
	int nExtBank = 0;
	char ExtBank[2000] = {0,};
	qry->GetStr( "m_extGuildBank", ExtBank );
	while( '$' != ExtBank[CountStr] )
	{
		GuildBank->m_apItem[nExtBank].m_bCharged				= (BOOL)GetIntPaFromStr( ExtBank, &CountStr );
		if( GuildBank->m_apItem[nExtBank].m_bCharged != 1 )
			GuildBank->m_apItem[nExtBank].m_bCharged	= 0;
		GuildBank->m_apItem[nExtBank].m_dwKeepTime				= (DWORD)GetIntPaFromStr( ExtBank, &CountStr );
#if __VER >= 11 // __SYS_IDENTIFY
		__int64 iRandomOptItemId	= GetInt64PaFromStr( ExtBank, &CountStr );
		if( iRandomOptItemId == -102 )
			iRandomOptItemId	= 0;
		GuildBank->m_apItem[nExtBank].SetRandomOptItemId( iRandomOptItemId );
#else	// __SYS_IDENTIFY
		GuildBank->m_apItem[nExtBank].SetRandomOptItemId( (DWORD)GetIntPaFromStr( ExtBank, &CountStr ) );
#endif	// __SYS_IDENTIFY
#if __VER >= 15 // __PETVIS
		GuildBank->m_apItem[nExtBank].m_bTranformVisPet = static_cast<BOOL>( GetIntPaFromStr( ExtBank, &CountStr ) );
#endif // __PETVIS
		++CountStr; 		++nExtBank;
	}
	
	CountStr	= 0;
	int nPirecingBank = 0;
	char PirecingBank[4000] = {0,};
	qry->GetStr( "m_GuildBankPiercing", PirecingBank );
	while( '$' != PirecingBank[CountStr] )
	{
#if __VER >= 12 // __EXT_PIERCING
		LoadPiercingInfo( GuildBank->m_apItem[nPirecingBank], PirecingBank, &CountStr );
#else // __EXT_PIERCING
		int nItemIdCount = 0;
		GuildBank->m_apItem[nPirecingBank].SetPiercingSize( GetIntFromStr( PirecingBank, &CountStr ) );
		for( int i = 0 ; i < GuildBank->m_apItem[nPirecingBank].GetPiercingSize() ; ++i )
		{
			GuildBank->m_apItem[nPirecingBank].SetPiercingItem( nItemIdCount, (DWORD)GetIntFromStr( PirecingBank, &CountStr ) );
			++nItemIdCount;
		}
#endif // __EXT_PIERCING
		++nPirecingBank;
	}

	CountStr	= 0;
	int	nId	= 0;
#ifdef __PET_1024
	char szPet[4200]	= { 0, };
#else	// __PET_1024
	char szPet[2688]	= { 0, };
#endif	// __PET_1024
	qry->GetStr( "szGuildBankPet", szPet );
	while( '$' != szPet[CountStr] )
	{
		BOOL bPet	= (BOOL)GetIntFromStr( szPet, &CountStr );
		if( bPet )
		{
			SAFE_DELETE( GuildBank->m_apItem[nId].m_pPet );
			CPet* pPet	= GuildBank->m_apItem[nId].m_pPet		= new CPet;
			BYTE nKind	= (BYTE)GetIntFromStr( szPet, &CountStr );
			pPet->SetKind( nKind );
			BYTE nLevel	= (BYTE)GetIntFromStr( szPet, &CountStr );
			pPet->SetLevel( nLevel );
			DWORD dwExp	= (DWORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetExp( dwExp );
			WORD wEnergy	= (WORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetEnergy( wEnergy );
#ifdef __PET_1024
			WORD wLife	= (WORD)GetIntPaFromStr( szPet, &CountStr );
			pPet->SetLife( wLife );
			for( int i = PL_D; i <= pPet->GetLevel(); i++ )
			{
				BYTE nAvailLevel	= (BYTE)GetIntPaFromStr( szPet, &CountStr );
				pPet->SetAvailLevel( i, nAvailLevel );
			}
			char szFmt[MAX_PET_NAME_FMT]	= { 0,};
			GetStrFromStr( szPet, szFmt, &CountStr );
			char szName[MAX_PET_NAME]	= { 0,};
			GetDBFormatStr( szName, MAX_PET_NAME, szFmt );
			pPet->SetName( szName );
#else	// __PET_1024
			WORD wLife	= (WORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetLife( wLife );
			for( int i = PL_D; i <= pPet->GetLevel(); i++ )
			{
				BYTE nAvailLevel	= (BYTE)GetIntFromStr( szPet, &CountStr );
				pPet->SetAvailLevel( i, nAvailLevel );
			}
#endif	// __PET_1024
		}
		nId++;
	}
}

#if __VER >= 11 // __SYS_POCKET
BOOL CDbManager::GetPocket( CMover* pMover, CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	char	szItem[7500]	= { 0,};
	char	szIndex[512]	= { 0,};
	char	szObjIndex[512]	= { 0,};
	char	szExt[2000]		= { 0,};
	char	szPiercing[4000]	= { 0,};
#ifdef __PET_1024
	char	szPet[4200]	= { 0,};
#else	// __PET_1024
	char	szPet[2688]	= { 0,};
#endif	// __PET_1024

	pQuery->GetStr( "szItem", szItem );
	pQuery->GetStr( "szIndex", szIndex );
	pQuery->GetStr( "szObjIndex", szObjIndex );
	pQuery->GetStr( "szExt", szExt );
	pQuery->GetStr( "szPiercing", szPiercing );
	pQuery->GetStr( "szPet", szPet );

	char szPlayerName[64] = {0, };
	if( lpDbOverlappedPlus != NULL )
	{
		_tcscpy( szPlayerName, lpDbOverlappedPlus->AccountInfo.szPlayer );
	}
	else
	{
		_tcscpy( szPlayerName, pMover->m_szName );
	}
	VERIFYSTRING_RETURN( szItem, szPlayerName );
	VERIFYSTRING_RETURN( szIndex, szPlayerName );
	VERIFYSTRING_RETURN( szObjIndex, szPlayerName );
	VERIFYSTRING_RETURN( szExt, szPlayerName );
	VERIFYSTRING_RETURN( szPiercing, szPlayerName );
	VERIFYSTRING_RETURN( szPet, szPlayerName );

	int nPocket	= pQuery->GetInt( "nPocket" );
	BOOL bExpired	= (BOOL)pQuery->GetInt( "bExpired" );

	if( szItem[0] != '$' || !bExpired )
	{
		pMover->m_Pocket.Avail( nPocket );
		CPocket* pPocket	= pMover->m_Pocket.GetPocket( nPocket );
		pPocket->SetExpired( bExpired );
		pPocket->SetExpirationDate( (time_t)pQuery->GetInt( "tExpirationDate" ) );
		int nOffset	= 0, i	= 0;
		while( '$' != szItem[nOffset] )
		{
			CItemElem itemElem;
			i	= GetOneItem( &itemElem, szItem, &nOffset );
			if( i == -1 )
			{
				Error( "Join:Pocket:Property not found" );
			}
			else
			{
				if( i >= POCKET_SLOT_SIZE_1_2 )
				{
					Error( "Join:Pocket:POCKET_SLOT_SIZE_1_2" );
					return FALSE;
				}
				pPocket->m_apItem[i]	= itemElem;
			}
		}
		nOffset		= i	= 0;
		while( '$' != szIndex[nOffset] )
			pPocket->m_apIndex[i++]		= (DWORD)GetIntFromStr( szIndex, &nOffset );
		nOffset		= i	= 0;
		while( '$' != szObjIndex[nOffset] )
			pPocket->m_apItem[i++].m_dwObjIndex	= (DWORD)GetIntFromStr( szObjIndex, &nOffset );
		nOffset		= i	= 0;
		while( '$' != szExt[nOffset] )
		{
			pPocket->m_apItem[i].m_bCharged		= (BOOL)GetIntPaFromStr( szExt, &nOffset );
			if( pPocket->m_apItem[i].m_bCharged != 1 )
				pPocket->m_apItem[i].m_bCharged		= 0;
			pPocket->m_apItem[i].m_dwKeepTime	= (DWORD)GetIntPaFromStr( szExt, &nOffset );
			__int64 iRandomOptItemId	= GetInt64PaFromStr( szExt, &nOffset );
#if __VER >= 15 // __PETVIS
			pPocket->m_apItem[i].m_bTranformVisPet = static_cast<BOOL>( GetIntPaFromStr( szExt, &nOffset ) );
#endif // __PETVIS
			if( iRandomOptItemId == -102 )
				iRandomOptItemId	= 0;
			pPocket->m_apItem[i++].SetRandomOptItemId( iRandomOptItemId );
			nOffset++;
		}
		nOffset		= i	= 0;
		while( '$' != szPiercing[nOffset] )
		{
#if __VER >= 12 // __EXT_PIERCING
			LoadPiercingInfo( pPocket->m_apItem[i], szPiercing, &nOffset );
#else // __EXT_PIERCING
			pPocket->m_apItem[i].SetPiercingSize( GetIntFromStr( szPiercing, &nOffset ) );
			for( int j = 0; j < pPocket->m_apItem[i].GetPiercingSize(); j++ )
				pPocket->m_apItem[i].SetPiercingItem( j, (DWORD)GetIntFromStr( szPiercing, &nOffset ) );
#endif // __EXT_PIERCING
			i++;
		}
		nOffset		= i	= 0;
		while( '$' != szPet[nOffset] )
		{
			BOOL bPet	= (BOOL)GetIntFromStr( szPet, &nOffset );
			if( bPet )
			{
				SAFE_DELETE( pPocket->m_apItem[i].m_pPet );
				CPet* pPet	= pPocket->m_apItem[i].m_pPet	= new CPet;
				BYTE nKind	= (BYTE)GetIntFromStr( szPet, &nOffset );
				pPet->SetKind( nKind );
				BYTE nLevel	= (BYTE)GetIntFromStr( szPet, &nOffset );
				pPet->SetLevel( nLevel );
				DWORD dwExp	= (DWORD)GetIntFromStr( szPet, &nOffset );
				pPet->SetExp( dwExp );
				WORD wEnergy	= (WORD)GetIntFromStr( szPet, &nOffset );
				pPet->SetEnergy( wEnergy );
#ifdef __PET_1024
				WORD wLife	= (WORD)GetIntPaFromStr( szPet, &nOffset );
				pPet->SetLife( wLife );
				for( int j = PL_D; j <= pPet->GetLevel(); j++ )
				{
					BYTE nAvailLevel	= (BYTE)GetIntPaFromStr( szPet, &nOffset );
					pPet->SetAvailLevel( j, nAvailLevel );
				}
				char szFmt[MAX_PET_NAME_FMT]	= { 0,};
				GetStrFromStr( szPet, szFmt, &nOffset );
				char szName[MAX_PET_NAME]	= { 0,};
				GetDBFormatStr( szName, MAX_PET_NAME, szFmt );
				pPet->SetName( szName );
#else	// __PET_1024
				WORD wLife	= (WORD)GetIntFromStr( szPet, &nOffset );
				pPet->SetLife( wLife );
				for( int j = PL_D; j <= pPet->GetLevel(); j++ )
				{
					BYTE nAvailLevel	= (BYTE)GetIntFromStr( szPet, &nOffset );
					pPet->SetAvailLevel( j, nAvailLevel );
				}
#endif	// __PET_1024
			}
			i++;
		}
	}
	return TRUE;
}
#endif	// __SYS_POCKET

BOOL CDbManager::GetInventory( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	int CountStr	= 0;
	int IndexItem	= 0;
	char Inven[6000]	= { 0, };
	char szPlayerName[64] = {0, };
	if( lpDbOverlappedPlus != NULL )
	{
		_tcscpy( szPlayerName, lpDbOverlappedPlus->AccountInfo.szPlayer );
	}
	else
	{
		_tcscpy( szPlayerName, pMover->m_szName );
	}
	qry->GetStr("m_Inventory", Inven);
	VERIFYSTRING_RETURN( Inven, szPlayerName );	// if false, return
	while('$' != Inven[CountStr])
	{
		CItemElem BufItemElem;
		IndexItem = GetOneItem( &BufItemElem, Inven, &CountStr );
		if( IndexItem == -1 )
		{
			Error( "Inventory : << 프로퍼티 없음. %s, %d", pMover->m_szName, BufItemElem.m_dwItemId );
		}
		else
		{
			if( IndexItem >= MAX_INVENTORY + MAX_HUMAN_PARTS)
			{
				Error( "Join::Inventory : << IndexItem %s, %d", pMover->m_szName, IndexItem );
				Error( "Inventory = %s", Inven );
				return FALSE;
			}
			pMover->m_Inventory.m_apItem[IndexItem] = BufItemElem;
		}
	}
	
	CountStr	= 0;
	int itemCount	= 0;
	char m_apIndex[512]		= { 0, };
	qry->GetStr( "m_apIndex", m_apIndex );
	VERIFYSTRING_RETURN( m_apIndex, szPlayerName );
	while( '$' != m_apIndex[CountStr] )
	{
		pMover->m_Inventory.m_apIndex[itemCount]	= (DWORD)GetIntFromStr( m_apIndex, &CountStr );
		itemCount++;
	}
	
	CountStr	= 0;
	int IndexObjIndex	= 0;
	char ObjIndex[512]	= {0,};
	qry->GetStr( "m_dwObjIndex", ObjIndex );
	VERIFYSTRING_RETURN( ObjIndex, szPlayerName );
	while( '$' != ObjIndex[CountStr] )
	{
		pMover->m_Inventory.m_apItem[IndexObjIndex].m_dwObjIndex	= (DWORD)GetIntFromStr( ObjIndex, &CountStr );
		IndexObjIndex++;
	}

	CountStr	= 0;
	int nExtInven = 0;
	char ExtInven[2000] = {0,};
	qry->GetStr( "m_extInventory", ExtInven );
	VERIFYSTRING_RETURN( ExtInven, szPlayerName );
	while( '$' != ExtInven[CountStr] )
	{
		pMover->m_Inventory.m_apItem[nExtInven].m_bCharged					= (BOOL)GetIntPaFromStr( ExtInven, &CountStr );
		if( pMover->m_Inventory.m_apItem[nExtInven].m_bCharged != 1 )
			pMover->m_Inventory.m_apItem[nExtInven].m_bCharged	= 0;
		pMover->m_Inventory.m_apItem[nExtInven].m_dwKeepTime				= (DWORD)GetIntPaFromStr( ExtInven, &CountStr );
#if __VER >= 11 // __SYS_IDENTIFY
		__int64 iRandomOptItemId	= GetInt64PaFromStr( ExtInven, &CountStr );
		if( iRandomOptItemId == -102 )
			iRandomOptItemId	= 0;
		pMover->m_Inventory.m_apItem[nExtInven].SetRandomOptItemId( iRandomOptItemId );
#if __VER >= 15 // __PETVIS
		pMover->m_Inventory.m_apItem[nExtInven].m_bTranformVisPet = static_cast<BOOL>( GetIntPaFromStr( ExtInven, &CountStr ) );
#endif // __PETVIS
#else	// __SYS_IDENTIFY
		pMover->m_Inventory.m_apItem[nExtInven].SetRandomOptItemId( (DWORD)GetIntPaFromStr( ExtInven, &CountStr ) );
#endif	// __SYS_IDENTIFY
		++CountStr; 	++nExtInven;
	}

	CountStr	= 0;
	int nPirecingInven = 0;
	char PirecingInven[4000] = {0,};
	qry->GetStr( "m_InventoryPiercing", PirecingInven );
	VERIFYSTRING_RETURN( PirecingInven, szPlayerName );
	while( '$' != PirecingInven[CountStr] )
	{
#if __VER >= 12 // __EXT_PIERCING
		LoadPiercingInfo( pMover->m_Inventory.m_apItem[nPirecingInven], PirecingInven, &CountStr );
#else // __EXT_PIERCING
		int nItemIdCount = 0;
		pMover->m_Inventory.m_apItem[nPirecingInven].SetPiercingSize( GetIntFromStr( PirecingInven, &CountStr ) );
		for( int i = 0 ; i < pMover->m_Inventory.m_apItem[nPirecingInven].GetPiercingSize(); ++i )
		{
			pMover->m_Inventory.m_apItem[nPirecingInven].SetPiercingItem( nItemIdCount, (DWORD)GetIntFromStr( PirecingInven, &CountStr ) );
			++nItemIdCount;
		}
#endif // __EXT_PIERCING
		++nPirecingInven;
	}

	CountStr	= 0;
	int	nId	= 0;
#ifdef __PET_1024
	char szPet[4200]	= { 0, };
#else	// __PET_1024
	char szPet[2688]	= { 0, };
#endif	// __PET_1024
	qry->GetStr( "szInventoryPet", szPet );
	VERIFYSTRING_RETURN( szPet, szPlayerName );
	while( '$' != szPet[CountStr] )
	{
		BOOL bPet	= (BOOL)GetIntFromStr( szPet, &CountStr );
		if( bPet )
		{
			SAFE_DELETE( pMover->m_Inventory.m_apItem[nId].m_pPet );
			CPet* pPet	= pMover->m_Inventory.m_apItem[nId].m_pPet		= new CPet;
			BYTE nKind	= (BYTE)GetIntFromStr( szPet, &CountStr );
			pPet->SetKind( nKind );
			BYTE nLevel	= (BYTE)GetIntFromStr( szPet, &CountStr );
			pPet->SetLevel( nLevel );
			DWORD dwExp	= (DWORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetExp( dwExp );
			WORD wEnergy	= (WORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetEnergy( wEnergy );
#ifdef __PET_1024
			WORD wLife	= (WORD)GetIntPaFromStr( szPet, &CountStr );
			pPet->SetLife( wLife );
			for( int i = PL_D; i <= pPet->GetLevel(); i++ )
			{
				BYTE nAvailLevel	= (BYTE)GetIntPaFromStr( szPet, &CountStr );
				pPet->SetAvailLevel( i, nAvailLevel );
			}
			char szFmt[MAX_PET_NAME_FMT]	= { 0,};
			GetStrFromStr( szPet, szFmt, &CountStr );
			char szName[MAX_PET_NAME]	= { 0,};
			GetDBFormatStr( szName, MAX_PET_NAME, szFmt );
			pPet->SetName( szName );
#else	// __PET_1024
			WORD wLife	= (WORD)GetIntFromStr( szPet, &CountStr );
			pPet->SetLife( wLife );
			for( int i = PL_D; i <= pPet->GetLevel(); i++ )
			{
				BYTE nAvailLevel	= (BYTE)GetIntFromStr( szPet, &CountStr );
				pPet->SetAvailLevel( i, nAvailLevel );
			}
#endif	// __PET_1024
		}
		nId++;
	}
	return TRUE;
}

#if __VER >= 12 // __EXT_PIERCING
void CDbManager::LoadPiercingInfo( CItemElem & itemElem, char* szPirecingInven, int* pLocation )
{
	ItemProp* itemProp = itemElem.GetProp();
	if( itemProp && itemProp->IsUltimate() )
	{
		itemElem.SetUltimatePiercingSize( GetIntPaFromStr( szPirecingInven, pLocation ) );
		for( int i = 0 ; i < itemElem.GetUltimatePiercingSize(); ++i )
			itemElem.SetUltimatePiercingItem( i, (DWORD)GetIntPaFromStr( szPirecingInven, pLocation ) );
	}
	itemElem.SetPiercingSize( GetIntFromStr( szPirecingInven, pLocation ) );
	for( int i = 0 ; i < itemElem.GetPiercingSize(); ++i )
		itemElem.SetPiercingItem( i, (DWORD)GetIntFromStr( szPirecingInven, pLocation ) );
#if __VER >= 15 // __PETVIS
	if( itemElem.IsVisPet() )
	{
		itemElem.SetVisKeepTimeSize( itemElem.GetPiercingSize() );
		for( i=0; i<itemElem.GetPiercingSize(); i++ )
			itemElem.SetVisKeepTime( i, (time_t)GetIntFromStr( szPirecingInven, pLocation ) );
	}
#endif // __PETVIS
}

// 얼터멋 웨폰의 경우 nPiercedSize가 얼터멋 피어싱 사이즈이고 dwItemId1 ~ 5 까지는 얼터멋 피어싱 아이템(보석)이다.
// nPiercedSize2가 일반 피어싱 사이즈이고 dwItemId6 ~ 15 까지가 일반 피어싱 아이템(카드)이다.
// 얼터멋 웨폰이 아닌 경우에는 nPiercedSize가 피어싱 사이즈이고 dwItemId1~10 까지가 피어싱 아이템(카드)이다.
// 이는 MakeQueryAddMail() 함수에도 동일하게 적용된다.
void CDbManager::GetPiercingInfoFromMail( CQuery* pQuery, CItemElem* pItemElem )
{
	int nAddCount = 1;
	if( pItemElem->GetProp() && pItemElem->GetProp()->IsUltimate() )
	{
		pItemElem->SetUltimatePiercingSize( pQuery->GetInt( "nPiercedSize" ) );
		for( int i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )
		{
			char szItem[32];
			sprintf( szItem, "dwItemId%d", i + nAddCount );
			pItemElem->SetUltimatePiercingItem( i, pQuery->GetInt( szItem ) );
		}
		pItemElem->SetPiercingSize( pQuery->GetInt( "nPiercedSize2" ) );
		nAddCount = 6;
	}
	else
		pItemElem->SetPiercingSize( pQuery->GetInt( "nPiercedSize" ) );

	for( int i=0; i<pItemElem->GetPiercingSize(); i++ )
	{
		char szItem[32];
		sprintf( szItem, "dwItemId%d", i + nAddCount );
		pItemElem->SetPiercingItem( i, pQuery->GetInt( szItem ) );
	}
}
#endif // __EXT_PIERCING

BOOL CDbManager::GetEquipment( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	int CountStr	= 0;
	int IndexEquipment	= 0;
	char Equipment[500]		= {0,};
	qry->GetStr( "m_adwEquipment", Equipment );
	VERIFYSTRING_RETURN( Equipment, lpDbOverlappedPlus->AccountInfo.szPlayer );
	while( '$' != Equipment[CountStr] )
	{
		if( IndexEquipment < MAX_HUMAN_PARTS )
			pMover->m_aEquipInfo[IndexEquipment].dwId	= (DWORD)GetIntFromStr( Equipment, &CountStr );
		IndexEquipment++;
	}
	return TRUE;
}

BOOL CDbManager::GetTaskBar( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	int CountStr	= 0;
	int nIndex	= 0;
	char AppletTaskBar[MAX_APPLETTASKBAR]	= { 0, };
	qry->GetStr( "m_aSlotApplet", AppletTaskBar );
	VERIFYSTRING_RETURN( AppletTaskBar, lpDbOverlappedPlus->AccountInfo.szPlayer );
	while( '$' != AppletTaskBar[CountStr] )
	{
		nIndex	= GetIntFromStr( AppletTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwShortcut	= (DWORD)GetIntFromStr( AppletTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwId	= (DWORD)GetIntFromStr( AppletTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwType	= (DWORD)GetIntFromStr( AppletTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwIndex	= (DWORD)GetIntFromStr( AppletTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwUserId	= (DWORD)GetIntFromStr( AppletTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwData	= (DWORD)GetIntFromStr( AppletTaskBar, &CountStr );
		if( pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_CHAT )
			GetStrFromDBFormat( pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_szString, AppletTaskBar, CountStr );
	}
	
	CountStr	= 0;
	nIndex	= 0;
	int nSlotIndex	= 0;
	char ItemTaskBar[MAX_ITEMTASKBAR]	= { 0, };
	qry->GetStr( "m_aSlotItem", ItemTaskBar );
	VERIFYSTRING_RETURN( ItemTaskBar, lpDbOverlappedPlus->AccountInfo.szPlayer );
	while( '$' != ItemTaskBar[CountStr] )
	{
		nSlotIndex	= GetIntFromStr( ItemTaskBar, &CountStr );
		nIndex	= GetIntFromStr( ItemTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut	= (DWORD)GetIntFromStr( ItemTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwId	= (DWORD)GetIntFromStr( ItemTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwType	= (DWORD)GetIntFromStr( ItemTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwIndex	= (DWORD)GetIntFromStr( ItemTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwUserId	= (DWORD)GetIntFromStr( ItemTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwData	= (DWORD)GetIntFromStr( ItemTaskBar, &CountStr );
		if( pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut == SHORTCUT_CHAT )
			GetStrFromDBFormat( pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString, ItemTaskBar, CountStr );
	}
	
	CountStr	= 0;
	nIndex	= 0;
	char SkillTaskBar[1024]		= { 0, };
	qry->GetStr( "m_aSlotQueue", SkillTaskBar );
	VERIFYSTRING_RETURN( SkillTaskBar, lpDbOverlappedPlus->AccountInfo.szPlayer );
	while( '$' != SkillTaskBar[CountStr] )
	{
		nIndex	= GetIntFromStr( SkillTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwShortcut	= (DWORD)GetIntFromStr( SkillTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwId	= (DWORD)GetIntFromStr( SkillTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwType		= (DWORD)GetIntFromStr( SkillTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwIndex	= (DWORD)GetIntFromStr( SkillTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwUserId	= (DWORD)GetIntFromStr( SkillTaskBar, &CountStr );
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwData		= (DWORD)GetIntFromStr( SkillTaskBar, &CountStr );
	}
	pMover->m_UserTaskBar.m_nActionPoint = qry->GetInt( "m_SkillBar" );
	return TRUE;
}

void CDbManager::GetCardCube( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
}


void CDbManager::GetBaseCharacter( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	qry->GetStr( "m_szName", pMover->m_szName );
	pMover->m_vScale.x		= qry->GetFloat( "m_vScale_x" );
	pMover->m_dwMotion		= (DWORD)qry->GetInt( "m_dwMotion" );
	pMover->m_dwWorldID		= qry->GetInt( "dwWorldID");
	pMover->m_vPos.x		= qry->GetFloat( "m_vPos_x" );
	pMover->m_vPos.y		= qry->GetFloat( "m_vPos_y" );
	pMover->m_vPos.z		= qry->GetFloat( "m_vPos_z" );
	pMover->m_fAngle		= qry->GetFloat( "m_fAngle" );
	qry->GetStr( "m_szCharacterKey", pMover->m_szCharacterKey );
	pMover->m_nHitPoint		= (LONG)qry->GetInt( "m_nHitPoint" );
	pMover->m_nManaPoint	= (LONG)qry->GetInt( "m_nManaPoint" );
	pMover->m_nFatiguePoint		= (LONG)qry->GetInt( "m_nFatiguePoint" );
	pMover->m_dwSkinSet		= (DWORD)qry->GetInt( "m_dwSkinSet" );
	pMover->m_dwHairMesh	= (DWORD)qry->GetInt( "m_dwHairMesh" );
	pMover->m_dwHairColor	= (DWORD)qry->GetInt( "m_dwHairColor" );
	pMover->m_dwHeadMesh	= (DWORD)qry->GetInt( "m_dwHeadMesh" );
	pMover->SetSex( (BYTE)qry->GetInt( "m_dwSex" ) );
	pMover->m_dwRideItemIdx		= (DWORD)qry->GetInt( "m_dwRideItemIdx" );
	pMover->SetGold( qry->GetInt("m_dwGold") );
	pMover->m_nJob			= (LONG)qry->GetInt( "m_nJob" );
	pMover->m_idparty		= (u_long)qry->GetInt( "m_idparty" );
#if __VER >= 8 // __S8_PK
	pMover->m_nPKValue			= qry->GetInt( "m_nPKValue" );
	pMover->m_dwPKPropensity	= qry->GetInt( "m_dwPKPropensity" );
	pMover->m_dwPKExp			= qry->GetInt( "m_dwPKExp" );
#else // __VER >= 8 // __S8_PK
	pMover->m_nNumKill		= qry->GetInt( "m_nNumKill" );
	pMover->m_nSlaughter	= qry->GetInt( "m_nSlaughter" );
#endif // __VER >= 8 // __S8_PK
#if __VER >= 8 // __CSC_VER8_5
	pMover->m_nAngelExp		= qry->GetExpInteger( "m_nAngelExp" );
	pMover->m_nAngelLevel	= qry->GetInt( "m_nAngelLevel" );
#endif // __CSC_VER8_5
	
	pMover->m_idMurderer	= (u_long)qry->GetInt( "m_idMuerderer" );
	pMover->m_nFame			= qry->GetInt( "m_nFame" );
	
	int CountStr	= 0;
	char ActMover[50]	= {0,};
	qry->GetStr( "m_pActMover", ActMover );
	pMover->m_pActMover->m_dwState	= (DWORD)GetIntFromStr( ActMover, &CountStr );
	pMover->m_pActMover->m_dwStateFlag	= (DWORD)GetIntFromStr( ActMover, &CountStr );
	
	CountStr	= 0;
	pMover->m_nStr	= (LONG)qry->GetInt( "m_nStr" );
	pMover->m_nSta	= (LONG)qry->GetInt( "m_nSta" );
	pMover->m_nDex	= (LONG)qry->GetInt( "m_nDex" );
	pMover->m_nInt	= (LONG)qry->GetInt( "m_nInt" );
	pMover->m_nLevel	= (LONG)qry->GetInt( "m_nLevel" );
	pMover->m_nExp1		= qry->GetExpInteger( "m_nExp1" );
	pMover->m_nFuel		= qry->GetInt( "m_nFuel" );
	pMover->m_tmAccFuel	= qry->GetInt( "m_tmAccFuel" );
	
//	pMover->m_dwExpertLv	= (DWORD)qry->GetInt( "m_dwExpertLv");
	pMover->m_idMarkingWorld	= (DWORD)qry->GetInt( "m_idMarkingWorld");
	pMover->m_vMarkingPos.x		= qry->GetFloat( "m_vMarkingPos_x" );
	pMover->m_vMarkingPos.y		= qry->GetFloat( "m_vMarkingPos_y" );
	pMover->m_vMarkingPos.z		= qry->GetFloat( "m_vMarkingPos_z" );
	pMover->m_nRemainGP = (long)qry->GetInt( "m_nRemainGP" );
//	pMover->m_nRemainLP = (long)qry->GetInt( "m_nRemainLP" );
#if __VER >= 12 // __MOD_TUTORIAL
	int nTutorialState	= qry->GetInt( "m_nFlightLv" );
	pMover->SetTutorialState( nTutorialState );
#else	// __MOD_TUTORIAL
	pMover->m_nFlightLv		= (long)qry->GetInt( "m_nFlightLv" );
#endif	// __MOD_TUTORIAL
	pMover->m_nFxp	= (long)qry->GetInt( "m_nFxp" );
//	pMover->m_nTxp	= (long)qry->GetInt( "m_nTxp" );
	pMover->m_dwMode	= (DWORD)qry->GetInt( "m_dwMode" );
	pMover->m_bPlayer	= 1;
	pMover->m_nDeathExp	= qry->GetExpInteger( "m_nDeathExp" );
	pMover->m_nDeathLevel = qry->GetInt( "m_nDeathLevel" );
	pMover->m_nSkillLevel	= qry->GetInt( "m_SkillLv" );
	pMover->m_nSkillPoint	= qry->GetInt( "m_SkillPoint" );
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	pMover->m_nHonor	= qry->GetInt( "m_nHonor" );
#endif	// __HONORABLE_TITLE			// 달인

}
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
void	CDbManager::GetHonor( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	// 내가 등록한 아이디 가지고 오기
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery,
		"usp_Master_Select '%02d','%07d'",
		g_appInfo.dwSys, pMover->m_idPlayer );
	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return;
	}
	
	int aTempHonor[3][50]={0,};
	int nSec = 0;
	while( qry->Fetch() )
	{
		int nSec = qry->GetInt( "sec" );
		nSec--;
		if(nSec > 2 || nSec < 0 )
			return;

		aTempHonor[nSec][0] = qry->GetInt( "c01" );
		aTempHonor[nSec][1] = qry->GetInt( "c02" );
		aTempHonor[nSec][2] = qry->GetInt( "c03" );
		aTempHonor[nSec][3] = qry->GetInt( "c04" );
		aTempHonor[nSec][4] = qry->GetInt( "c05" );
		aTempHonor[nSec][5] = qry->GetInt( "c06" );
		aTempHonor[nSec][6] = qry->GetInt( "c07" );
		aTempHonor[nSec][7] = qry->GetInt( "c08" );
		aTempHonor[nSec][8] = qry->GetInt( "c09" );
		aTempHonor[nSec][9] = qry->GetInt( "c10" );

		aTempHonor[nSec][10] = qry->GetInt( "c11" );
		aTempHonor[nSec][11] = qry->GetInt( "c12" );
		aTempHonor[nSec][12] = qry->GetInt( "c13" );
		aTempHonor[nSec][13] = qry->GetInt( "c14" );
		aTempHonor[nSec][14] = qry->GetInt( "c15" );
		aTempHonor[nSec][15] = qry->GetInt( "c16" );
		aTempHonor[nSec][16] = qry->GetInt( "c17" );
		aTempHonor[nSec][17] = qry->GetInt( "c18" );
		aTempHonor[nSec][18] = qry->GetInt( "c19" );
		aTempHonor[nSec][19] = qry->GetInt( "c20" );

		aTempHonor[nSec][20] = qry->GetInt( "c21" );
		aTempHonor[nSec][21] = qry->GetInt( "c22" );
		aTempHonor[nSec][22] = qry->GetInt( "c23" );
		aTempHonor[nSec][23] = qry->GetInt( "c24" );
		aTempHonor[nSec][24] = qry->GetInt( "c25" );
		aTempHonor[nSec][25] = qry->GetInt( "c26" );
		aTempHonor[nSec][26] = qry->GetInt( "c27" );
		aTempHonor[nSec][27] = qry->GetInt( "c28" );
		aTempHonor[nSec][28] = qry->GetInt( "c29" );
		aTempHonor[nSec][29] = qry->GetInt( "c30" );

		aTempHonor[nSec][30] = qry->GetInt( "c31" );
		aTempHonor[nSec][31] = qry->GetInt( "c32" );
		aTempHonor[nSec][32] = qry->GetInt( "c33" );
		aTempHonor[nSec][33] = qry->GetInt( "c34" );
		aTempHonor[nSec][34] = qry->GetInt( "c35" );
		aTempHonor[nSec][35] = qry->GetInt( "c36" );
		aTempHonor[nSec][36] = qry->GetInt( "c37" );
		aTempHonor[nSec][37] = qry->GetInt( "c38" );
		aTempHonor[nSec][38] = qry->GetInt( "c39" );
		aTempHonor[nSec][39] = qry->GetInt( "c40" );

		aTempHonor[nSec][40] = qry->GetInt( "c41" );
		aTempHonor[nSec][41] = qry->GetInt( "c42" );
		aTempHonor[nSec][42] = qry->GetInt( "c43" );
		aTempHonor[nSec][43] = qry->GetInt( "c44" );
		aTempHonor[nSec][44] = qry->GetInt( "c45" );
		aTempHonor[nSec][45] = qry->GetInt( "c46" );
		aTempHonor[nSec][46] = qry->GetInt( "c47" );
		aTempHonor[nSec][47] = qry->GetInt( "c48" );
		aTempHonor[nSec][48] = qry->GetInt( "c49" );
		aTempHonor[nSec][49] = qry->GetInt( "c50" );
	}

	int nMonster = 0,nItem = 0,nEtc = 0;
	int nType = 0;
	int nCurrentTitleCount =  CTitleManager::Instance()->m_nCurrentTitleCount;
	ASSERT( nCurrentTitleCount <= MAX_HONOR_TITLE );
	for(int i=0;i<nCurrentTitleCount;i++)
	{
		nType = CTitleManager::Instance()->GetIdxType(i);
		if( nType == HI_HUNT_MONSTER)
		{
			pMover->SetHonorCount(i,aTempHonor[2][nMonster]);
			nMonster++;
		}
		else if( nType == HI_USE_ITEM)
		{
			pMover->SetHonorCount(i,aTempHonor[1][nItem]);
			nItem++;
		}
		else
		{
			pMover->SetHonorCount(i,aTempHonor[0][nEtc]);
			nEtc++;
		}
	}
}

#endif	// __HONORABLE_TITLE			// 달인

BOOL CDbManager::GetSkill( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	// 내가 등록한 아이디 가지고 오기
	char szQuery[QUERY_SIZE]	= { 0,};
	sprintf( szQuery,
		"uspLoadCharacterSkill '%02d','%07d'",
		g_appInfo.dwSys, pMover->m_idPlayer );
	if( FALSE == qry->Exec( szQuery ) )
	{
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return FALSE;
	}
	
	while( qry->Fetch() )
	{
		SKILL BufSkill;
		DWORD dwSkillPosition;
		dwSkillPosition = (DWORD)qry->GetInt( "SkillPosition" );
		BufSkill.dwSkill = (DWORD)qry->GetInt( "SkillID" );
		BufSkill.dwLevel = (DWORD)qry->GetInt( "SkillLv" );
		pMover->m_aJobSkill[dwSkillPosition] = BufSkill;
	}
	return TRUE;
}

void CDbManager::GetJobLv( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
}

BOOL CDbManager::GetQuest( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	int CountStr	= 0;
	int IndexQuest	= 0;
	char QuestCnt[3072]		= {0,};
	qry->GetStr( "m_lpQuestCntArray", QuestCnt );
	VERIFYSTRING_RETURN( QuestCnt, pMover->m_szName );
	while( '$' != QuestCnt[CountStr] )
	{
		QUEST BufQuest;
		GetOneQuest( &BufQuest, QuestCnt, &CountStr );
		pMover->m_aQuest[IndexQuest] = BufQuest;
		IndexQuest++;
	}
	pMover->m_nQuestSize = IndexQuest;

	int nQuestSize = IndexQuest;
	CountStr	= 0;
	IndexQuest	= 0;
	char CompleteQuest[1024]		= {0,};
	qry->GetStr( "m_aCompleteQuest", CompleteQuest );
	VERIFYSTRING_RETURN( CompleteQuest, pMover->m_szName );
	while( '$' != CompleteQuest[CountStr] )
	{
		pMover->m_aCompleteQuest[IndexQuest] = (WORD)GetIntFromStr( CompleteQuest, &CountStr ); 
		IndexQuest++;
	}
	pMover->m_nCompleteQuestSize = IndexQuest;

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CountStr = 0;
	IndexQuest = 0;
	char CheckedQuest[100] = {0, };
	qry->GetStr( "m_aCheckedQuest", CheckedQuest );
	VERIFYSTRING_RETURN( CheckedQuest, pMover->m_szName );
	while( '$' != CheckedQuest[CountStr] )
	{
		pMover->m_aCheckedQuest[IndexQuest] = (WORD)GetIntFromStr( CheckedQuest, &CountStr ); 
		IndexQuest++;
	}
	pMover->m_nCheckedQuestSize = IndexQuest;
#endif // __IMPROVE_QUEST_INTERFACE

	// 기존 것중에 완료된것을 완료 배열에 넣는다.
	for( int i = 0; i < nQuestSize; i++ )
	{
		if( pMover->m_aQuest[ i ].m_nState == QS_END )
		{
			for( int j = 0; j < pMover->m_nCompleteQuestSize; j++ )
			{
				if( pMover->m_aCompleteQuest[ j ] == pMover->m_aQuest[ i ].m_wId )
					break;
			}
			if( j == pMover->m_nCompleteQuestSize )
			{
				pMover->m_aCompleteQuest[ pMover->m_nCompleteQuestSize ] = pMover->m_aQuest[ i ].m_wId;
				pMover->m_nCompleteQuestSize++;
			}
			for( j = i; j < nQuestSize - 1; j++ )
			{
				pMover->m_aQuest[ j ] = pMover->m_aQuest[ j + 1 ];
			}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			for( int k = 0; k < pMover->m_nCheckedQuestSize; ++k )
			{
				if( pMover->m_aCheckedQuest[ k ] == pMover->m_aQuest[ i ].m_wId )
				{
					for( int l = k; l < pMover->m_nCheckedQuestSize - 1; ++l )
						pMover->m_aCheckedQuest[ k ] = pMover->m_aCheckedQuest[ k + 1 ];
					pMover->m_aCheckedQuest[ --pMover->m_nCheckedQuestSize ] = 0;
					break;
				}
			}
#endif // __IMPROVE_QUEST_INTERFACE
			nQuestSize--;
			i--;
		}
		else
		{
			int j = 0;
			do
			{
				for( j = i + 1; j < nQuestSize; j++ )
				{
					if( pMover->m_aQuest[ j ].m_wId == pMover->m_aQuest[ i ].m_wId )
						break;
				}
				if( j != nQuestSize )
				{
					for( int k = j; k < nQuestSize - 1; k++ )
					{
						pMover->m_aQuest[ k ] = pMover->m_aQuest[ k + 1 ];
					}
					nQuestSize--;
				}
			} while( j != nQuestSize );
		}
	}
	pMover->m_nQuestSize = nQuestSize;
	return TRUE;
}

BOOL CDbManager::GetSMMode( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	int CountStr	= 0;
	int IndexSM	= 0;
	char SMCnt[3072]		= {0,};
	qry->GetStr( "m_dwSMTime", SMCnt );
	if( 0 != strcmp( SMCnt, "NULL" ) ) 
	{
		VERIFYSTRING_RETURN( SMCnt, pMover->m_szName );
		while( '$' != SMCnt[CountStr] )
		{
			pMover->m_dwSMTime[IndexSM] = (DWORD)GetIntFromStr( SMCnt, &CountStr );
			switch( IndexSM )
			{
			case SM_MAX_HP50:
				{
					pMover->m_nPlusMaxHitPoint = (LONG)GetIntFromStr( SMCnt, &CountStr );
				}
				break;
			case SM_RESIST_ATTACK_LEFT:
				{
					pMover->m_nAttackResistLeft = (BYTE)GetIntFromStr( SMCnt, &CountStr );
				}
				break;
			case SM_RESIST_ATTACK_RIGHT:
				{
					pMover->m_nAttackResistRight = (BYTE)GetIntFromStr( SMCnt, &CountStr );
				}
				break;
			case SM_RESIST_DEFENSE:
				{
					pMover->m_nDefenseResist = (BYTE)GetIntFromStr( SMCnt, &CountStr );
				}
				break;
			}
			++IndexSM;
		}			
	}
	return TRUE;
}

BOOL CDbManager::GetSKillInfluence( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	int CountStr	= 0;
	int iSkillIndex = 0;
	char szSKillInfluence[7500]		= {0,};
	qry->GetStr( "SkillInfluence", szSKillInfluence );
	VERIFYSTRING_RETURN( szSKillInfluence, pMover->m_szName );
	while( '$' != szSKillInfluence[CountStr] )
	{
#ifdef __BUFF_1107
		WORD wType	= (WORD)GetIntFromStr( szSKillInfluence, &CountStr );
		WORD wId	= (WORD)GetIntFromStr( szSKillInfluence, &CountStr );
		DWORD dwLevel	= (DWORD)GetIntFromStr( szSKillInfluence, &CountStr );
		DWORD tmTotal	= GetIntFromStr( szSKillInfluence, &CountStr );
#if __VER >= 13 // __HOUSING
		if( wType == 0 && wId == 0 && dwLevel == 0 && tmTotal == 0 )
			break;
#endif // __HOUSING
		IBuff* pBuff	= pMover->m_buffs.CreateBuff( wType );
		pBuff->SetId( wId );
		pBuff->SetLevel( dwLevel );
		pBuff->SetTotal( tmTotal );
		bool bResult	= pMover->m_buffs.Add( pBuff );
		ASSERT( bResult );
#else	// __BUFF_1107
		pMover->m_SkillState.m_aSkillInfluence[iSkillIndex].wType = (WORD)GetIntFromStr( szSKillInfluence, &CountStr );
		pMover->m_SkillState.m_aSkillInfluence[iSkillIndex].wID = (WORD)GetIntFromStr( szSKillInfluence, &CountStr );
		pMover->m_SkillState.m_aSkillInfluence[iSkillIndex].dwLevel = (DWORD)GetIntFromStr( szSKillInfluence, &CountStr );
		pMover->m_SkillState.m_aSkillInfluence[iSkillIndex].tmCount = GetIntFromStr( szSKillInfluence, &CountStr );
#if __VER >= 13 // __HOUSING
		SKILLINFLUENCE* pSkillInfluenece = &pMover->m_SkillState.m_aSkillInfluence[iSkillIndex];
		if( pSkillInfluenece->wType == 0 && pSkillInfluenece->wID == 0
			&& pSkillInfluenece->dwLevel == 0 && pSkillInfluenece->tmCount == 0 )
			break;
#endif // __HOUSING
#endif	// __BUFF_1107
		
		++iSkillIndex;
	}
	return TRUE;
}

int CDbManager::GetOneItem( CItemElem* pItemElem, char* pstrItem, int *pLocation )
{
	int IndexItem		= 0;
	IndexItem	= GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_dwItemId	= (DWORD)GetIntPaFromStr( pstrItem, pLocation );
	BOOL bEquip	= GetIntPaFromStr( pstrItem, pLocation );
	int nTrade	= GetIntPaFromStr( pstrItem, pLocation );
	GetStrPaFromStr( pstrItem, pItemElem->m_szItemText, pLocation );
	pItemElem->m_nItemNum	= GetIntPaFromStr( pstrItem, pLocation );

	if( CheckValidItem( pItemElem->m_dwItemId, pItemElem->m_nItemNum ) == FALSE )
		pItemElem->m_nItemNum	= 1;

	pItemElem->m_nRepairNumber	= (BYTE)GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_nHitPoint	= GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_nRepair	= GetIntPaFromStr( pstrItem, pLocation );
	int nMaterial	= GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_byFlag		= (BYTE)GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->SetSerialNumber( GetSerialNumberPaFromStr( pstrItem, pLocation ) );
	pItemElem->SetOption( GetIntPaFromStr( pstrItem, pLocation ) );
	pItemElem->m_bItemResist	= (BYTE)GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_nResistAbilityOption	= GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_idGuild = (u_long)GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_nResistSMItemId = GetIntPaFromStr( pstrItem, pLocation );
	pItemElem->m_dwObjId		= IndexItem;
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	if( pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER )
	{
	#if __VER >= 11 // __SYS_PLAYER_DATA
		CPlayerDataCenter::GetInstance()->m_Access.Enter();
		const char*	lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pItemElem->m_nHitPoint );
	#else	// __SYS_PLAYER_DATA
		LPCSTR lpszPlayer	= prj.GetPlayerString( pItemElem->m_nHitPoint );
	#endif	// __SYS_PLAYER_DATA
		if( lpszPlayer != NULL)
			memcpy(pItemElem->m_szItemText,lpszPlayer,sizeof(pItemElem->m_szItemText));
	#if __VER >= 11 // __SYS_PLAYER_DATA
		CPlayerDataCenter::GetInstance()->m_Access.Leave();
	#endif	// __SYS_PLAYER_DATA
	}
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	

	++*pLocation;
	return IndexItem;
}

void CDbManager::GetOneSkill( LPSKILL pSkill, char* pstrSkill, int *pLocation )
{
	(DWORD)GetIntPaFromStr( pstrSkill, pLocation );
	pSkill->dwLevel	= (DWORD)GetIntPaFromStr( pstrSkill, pLocation );
	pSkill->dwSkill	= (DWORD)GetIntPaFromStr( pstrSkill, pLocation );
	(BOOL)GetIntPaFromStr( pstrSkill, pLocation );
	
	++*pLocation;
}

void CDbManager::GetOneQuest( LPQUEST pQuest, char* pstrQuest, int *pLocation )
{
	pQuest->m_wId				= (WORD)GetIntPaFromStr( pstrQuest, pLocation );
	pQuest->m_nState			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );
	pQuest->m_wTime				= (WORD)GetIntPaFromStr( pstrQuest, pLocation );

//	pQuest->m_nKillNPCNum[0]	= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
//	pQuest->m_nKillNPCNum[1]	= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
	pQuest->m_nKillNPCNum[0]	= (WORD)GetIntPaFromStr( pstrQuest, pLocation );	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
	pQuest->m_nKillNPCNum[1]	= (WORD)GetIntPaFromStr( pstrQuest, pLocation );	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
	pQuest->m_bPatrol			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );
	pQuest->m_bReserve2			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );
	pQuest->m_bReserve3			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );
	pQuest->m_bReserve4			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );
	pQuest->m_bReserve5			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );
	pQuest->m_bReserve6			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );
	pQuest->m_bReserve7			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );
	pQuest->m_bReserve8			= (BYTE)GetIntPaFromStr( pstrQuest, pLocation );

	++*pLocation;
}


BOOL CDbManager::GetRemoveItem( CQuery *pQry, int &nNo, char* pOneItem, int &nItem_Count, int &nAbilityOption, int &nItemResist, int &nResistAbilityOption, char &chState )
{
	nNo = pQry->GetInt( "m_nNo" );									// 유니크 넘버
	pQry->GetStr( "Item_Name", pOneItem );							// 이름
	
	nItem_Count	= pQry->GetInt( "Item_count" );						// 갯수
	nAbilityOption = pQry->GetInt( "m_nAbilityOption" );			// AbilityOption ( + ) Option
	chState = pQry->GetChar( "State" );
	nItemResist = pQry->GetInt( "m_bItemResist" );					// 속성값	
	nResistAbilityOption = pQry->GetInt( "m_nResistAbilityOption" );// 속성에 대한 Option값
	
	if( 0 == strcmp( pOneItem, "NULL") )
	{
		WriteLog( "Remove NULL Item_Name : %s", pOneItem );
		return FALSE;
	}

	if( chState != 'I' && chState != 'B'
#ifdef __REMOVEITEM_POCKET
		&& chState != 'P'
#endif // __REMOVEITEM_POCKET
		)
	{
		WriteLog( "Remove Not Data State : %c", chState );
		return FALSE;
	}

	
	if( nItem_Count == -100 || nItem_Count == 0 )
		nItem_Count = 1;
	
	if( nAbilityOption == -100 )
		nAbilityOption = 0;
	
	if( nItemResist == -100 )
		nItemResist = 0;
	
	if( nResistAbilityOption == -100 )
		nResistAbilityOption = 0;

	
	return TRUE;
}

BOOL CDbManager::RemovePenya( CQuery *pQry, char* szSql, CMover* pMover, int nItem_Count, int nNo, char chState )
{
	if( RemoveItemDeleteQuery( pQry, szSql, nNo ) == FALSE )
		return FALSE;

	if( chState == 'I' )	// Inventory
	{
		pMover->AddGold( -nItem_Count, FALSE );
	}
	else	// Bank
	{
		DWORD dwGoldBank	= pMover->m_dwGoldBank[pMover->m_nSlot];
		pMover->m_dwGoldBank[pMover->m_nSlot]	-= nItem_Count;
		if( dwGoldBank < pMover->m_dwGoldBank[pMover->m_nSlot] )
			pMover->m_dwGoldBank[pMover->m_nSlot]	= 0;
	}

	return TRUE;
}

SERIALNUMBER CDbManager::RemoveItem( CQuery *pQry, char* szSql, int nNo, CMover* pMover, char* szItemName, int nItemCount, int nAbilityOption, int nItemResist, int nResistAbilityOption, char chState )
{
	SERIALNUMBER iSerialNumber = 0;

	int nIndex = atoi( szItemName );
	ItemProp* pItemProp = NULL;
	if( nIndex > 0 )
		pItemProp = prj.GetItemProp( nIndex );	
	else
		pItemProp = prj.GetItemProp( szItemName );	

	if( pItemProp )
	{
		if( RemoveItemDeleteQuery( pQry, szSql, nNo ) == FALSE )
			return 0;

#ifdef __REMOVEITEM_POCKET
		switch( chState )
		{
			case 'I' :
				iSerialNumber	= RemoveItemInventory( pItemProp, pMover, nAbilityOption, nItemResist, nResistAbilityOption, nItemCount );
				break;
			
			case 'B' :
				iSerialNumber	= RemoveItemBank( pItemProp, pMover, nAbilityOption, nItemResist, nResistAbilityOption, nItemCount );
				break;

			case 'P' :
				iSerialNumber	= RemoveItemPocket( pItemProp, pMover, nAbilityOption, nItemResist, nResistAbilityOption, nItemCount );
				break;
		}
#else // __REMOVEITEM_POCKET
		if( chState == 'I' )
		{
			iSerialNumber	= RemoveItemInventory( pItemProp, pMover, nAbilityOption, nItemResist, nResistAbilityOption, nItemCount );
		}
		else
		{
			iSerialNumber	= RemoveItemBank( pItemProp, pMover, nAbilityOption, nItemResist, nResistAbilityOption, nItemCount );
		}
#endif // __REMOVEITEM_POCKET

		
		if( iSerialNumber == 0 )
			return 0;
		else
			return iSerialNumber;
	}
	else
	{	// no propItem		
		return 0;
	}
	return 0;
}

SERIALNUMBER CDbManager::RemoveItemInventory( ItemProp* pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount )
{
	SERIALNUMBER iSerialNumber = 0;
	CItemElem *pItemElem = pMover->m_Inventory.GetItem( pItemProp, nAbilityOption, nItemResist, nResistAbilityOption );
	
	if( pItemElem != NULL ) // 인벤토리에 아이템이 있음을 검사.
	{
		iSerialNumber = pItemElem->GetSerialNumber();
		if( pMover->m_Inventory.IsEquip( pItemElem->m_dwObjId ) )	// 장비를 착용하고 있는지 검사
		{
			if( !pMover->m_Inventory.UnEquip( pItemProp->dwParts ) ) // 장비 해제
			{
				WriteLog( "Not UnEquip : %s, %s ", pMover->m_szName, pItemProp->szName );
				return (SERIALNUMBER)0;
			}
			else
				pMover->m_aEquipInfo[pItemProp->dwParts].dwId	= NULL_ID;
		}

		if( pItemElem->m_nItemNum <= nItemCount )
			pMover->m_Inventory.RemoveAt( pItemElem->m_dwObjIndex );
		else
			pItemElem->m_nItemNum -= nItemCount;
		
		return iSerialNumber;
	}
	else
	{
		WriteLog( "Not Inventory : %s, %s ", pMover->m_szName, pItemProp->szName );
		return 0;
	}
	return 0;
}

SERIALNUMBER CDbManager::RemoveItemBank( ItemProp* pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount )
{
	SERIALNUMBER iSerialNumber = 0;
	CItemElem *pItemElem	= pMover->m_Bank[pMover->m_nSlot].GetItem( pItemProp, nAbilityOption, nItemResist, nResistAbilityOption );

	if( pItemElem != NULL )
	{
		iSerialNumber	= pItemElem->GetSerialNumber();

		if( pItemElem->m_nItemNum <= nItemCount )
			pMover->m_Bank[pMover->m_nSlot].RemoveAt( pItemElem->m_dwObjIndex );
		else
			pItemElem->m_nItemNum -= nItemCount;

		return iSerialNumber;
	}
	else
	{
		WriteLog( "item not found - bank: %s, %s ", pMover->m_szName, pItemProp->szName );
		return 0;
	}
	return 0;
}

#ifdef __REMOVEITEM_POCKET
SERIALNUMBER CDbManager::RemoveItemPocket( ItemProp* pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount )
{
	SERIALNUMBER iSerialNumber = 0;
	for( int i=0; i<MAX_POCKET; i++ )
	{
		CPocket* pPocket = pMover->m_Pocket.GetPocket( i );
		if( pPocket )
		{
			CItemElem* pItemElem = pPocket->GetItem( pItemProp, nAbilityOption, nItemResist, nResistAbilityOption );
			if( pItemElem != NULL )
			{
				iSerialNumber	= pItemElem->GetSerialNumber();

				if( pItemElem->m_nItemNum <= nItemCount )
					pPocket->RemoveAt( pItemElem->m_dwObjIndex );
				else
					pItemElem->m_nItemNum -= nItemCount;

				return iSerialNumber;
			}
		}
	}

	WriteLog( "item not found - Pocket: %s, %s ", pMover->m_szName, pItemProp->szName );
	return 0;
}
#endif // __REMOVEITEM_POCKET

BOOL CDbManager::IsAbnormalPlayerData( CMover* pMover )
{
	DWORD dwId;
	int nMax	= pMover->m_Inventory.m_dwItemMax;
	int nNumberofIndex	= pMover->m_Inventory.m_dwIndexNum;

	for( int i = 0; i < nMax; i++ )
	{
		dwId	= pMover->m_Inventory.m_apIndex[i];
		if( dwId != NULL_ID )
		{
			if( pMover->m_Inventory.m_apItem[dwId].m_dwObjIndex != i )
				return TRUE;
		}
	}

	DWORD dwObjIndex;
	for( i = 0; i < nMax; i++ )
	{
		dwObjIndex	= pMover->m_Inventory.m_apItem[i].m_dwObjIndex;
		if( dwObjIndex != NULL_ID )
		{
			if( pMover->m_Inventory.m_apIndex[dwObjIndex] != i )
				return TRUE;
		}
		if( FALSE == pMover->m_Inventory.m_apItem[i].IsEmpty() )
		{
			ItemProp* pItemProp
				= prj.GetItemProp( pMover->m_Inventory.m_apItem[i].m_dwItemId );
			if( !pItemProp	/*It have not property*/
				|| pItemProp->dwItemKind3 == IK3_VIRTUAL/*It is monster's virtual weapon*/ )
				return TRUE;
		}
	}

	DWORD adwEquipment[MAX_HUMAN_PARTS];
	memset( adwEquipment, 0xff, sizeof(DWORD) * MAX_HUMAN_PARTS );
	for( i = nNumberofIndex; i < nMax; i++ )
	{
		dwId	= pMover->m_Inventory.m_apIndex[i];
		if( dwId != NULL_ID )
			adwEquipment[i - nNumberofIndex]	= dwId;
	}
	for( i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( adwEquipment[i] != pMover->m_aEquipInfo[i].dwId )
			return TRUE;
	}

	return FALSE;
}

BOOL CDbManager::NormalizePlayerData( CMover* pMover )
{
	int nMax	= pMover->m_Inventory.m_dwItemMax;
	int nNumberofIndex	= pMover->m_Inventory.m_dwIndexNum;
	DWORD dwId;
	for( int i = 0; i < nMax; i++ )
	{
		dwId	= pMover->m_Inventory.m_apIndex[i];
		if( dwId != NULL_ID )
		{
			if( pMover->m_Inventory.m_apItem[dwId].m_dwObjIndex != i )
			{

				DWORD dwIndex	= NULL_ID;
				for( int j = 0; j < nMax; j++ )
				{	
					if( pMover->m_Inventory.m_apItem[j].m_dwObjIndex == i )
					{
						dwIndex	= j;
						break;
					}
				}
				if( dwIndex != NULL_ID )
				{
					pMover->m_Inventory.m_apIndex[i]	= dwIndex;
				}
				else
				{
					if( i >= nNumberofIndex )
					{
						pMover->m_Inventory.m_apIndex[i]	= NULL_ID;
					}
					else
					{
						for( j = 0; j < nMax; j++ )
						{
							if( pMover->m_Inventory.m_apItem[j].m_dwObjIndex == NULL_ID )
							{
								pMover->m_Inventory.m_apItem[j].m_dwObjIndex	= i;
								pMover->m_Inventory.m_apIndex[i]	= j;
								break;
							}
						}
						if( j == nMax ) {
							ASSERT( 0 );
							return FALSE;
						}
					}
				}
			}
		}
	}

	DWORD dwObjIndex;
	for( i = 0; i < nMax; i++ )
	{
		dwObjIndex	= pMover->m_Inventory.m_apItem[i].m_dwObjIndex;
		if( dwObjIndex != NULL_ID )
		{
			if( pMover->m_Inventory.m_apIndex[dwObjIndex] != i )
				pMover->m_Inventory.m_apItem[i].m_dwObjIndex	= NULL_ID;
		}
	}

	DWORD adwEquipment[MAX_HUMAN_PARTS];
	memset( adwEquipment, 0xff, sizeof(DWORD) * MAX_HUMAN_PARTS );
	for( i = nNumberofIndex; i < nMax; i++ )
	{
		dwId	= pMover->m_Inventory.m_apIndex[i];
		if( dwId != NULL_ID )
			adwEquipment[i - nNumberofIndex]	= dwId;
	}
	for( i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( pMover->m_aEquipInfo[i].dwId != adwEquipment[i] )
			pMover->m_aEquipInfo[i].dwId	= adwEquipment[i];			
	}
	return TRUE;
}

BOOL CDbManager::RemoveQuest( void )
{
	// load - removequest.txt
	CScript s;
	OutputDebugString( "call BOOL CDbManager::RemoveQuest" );
	if( FALSE == s.Load( "removequest.txt" ) )
		return FALSE;
	vector<int>	vecRemoveQuestId;
	int	nQuestId	= s.GetNumber();
	while( s.tok != FINISHED )
	{
		vecRemoveQuestId.push_back( nQuestId );
		nQuestId	= s.GetNumber();
	}

	char lpOutputString[256];
	char aQuest[3072]	= { 0,};
	char aCompleteQuest[1024]	= { 0,};
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	char aCheckedQuest[100]	= {0, };
#endif // __IMPROVE_QUEST_INTERFACE

	CQuery* pQuery	= new CQuery;
	if( FALSE == pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		sprintf( lpOutputString, "fail to connect: dsn=%s, id=%s, pwd=%s", DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 );
		OutputDebugString( lpOutputString );
		SAFE_DELETE( pQuery );
		return FALSE;
	}
	
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	char* pSql	=  new char[4296];	// aQuest + aCompleteQuest + aCheckedQuest + 100
#else // __IMPROVE_QUEST_INTERFACE
	char* pSql	=  new char[4196];	// aQuest + aCompleteQuest + 100
#endif // __IMPROVE_QUEST_INTERFACE


#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	sprintf( pSql, "SELECT m_idPlayer, m_lpQuestCntArray, m_aCompleteQuest, m_aCheckedQuest FROM CHARACTER_TBL WHERE serverindex = '%02d'",
		g_appInfo.dwSys );
#else // __IMPROVE_QUEST_INTERFACE
	sprintf( pSql, "SELECT m_idPlayer, m_lpQuestCntArray, m_aCompleteQuest FROM CHARACTER_TBL WHERE serverindex = '%02d'",
		g_appInfo.dwSys );
#endif // __IMPROVE_QUEST_INTERFACE

	sprintf( pSql, "SELECT m_idPlayer, m_lpQuestCntArray, m_aCompleteQuest FROM CHARACTER_TBL WHERE serverindex = '%02d'",
		g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( pSql ) )
	{
		sprintf( lpOutputString, "query error: %s", pSql );
		SAFE_DELETE_ARRAY( pSql );
		SAFE_DELETE( pQuery );
		return FALSE;
	}
	while( pQuery->Fetch() )
	{
		CMover	mover;
		mover.m_idPlayer	= pQuery->GetInt( "m_idPlayer" );
		//GetQuest( &mover, pQuery, NULL );
		// mirchang_100416 VERIFYSTRING use return value
		if( GetQuest( &mover, pQuery, NULL ) == FALSE )
		{
			SAFE_DELETE_ARRAY( pSql );
			SAFE_DELETE( pQuery );
			return FALSE;
		}
		// mirchang_100416

		BOOL bRemove	= FALSE;
		for( DWORD i = 0; i < vecRemoveQuestId.size(); i++ )
		{
			if( mover.RemoveQuest( vecRemoveQuestId[i] ) )
				bRemove	= TRUE;
		}
		memset( aQuest, 0, sizeof(aQuest) );
		memset( aCompleteQuest, 0, sizeof(aCompleteQuest) );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		memset( aCheckedQuest, 0, sizeof(aCheckedQuest) );
#endif // __IMPROVE_QUEST_INTERFACE
		if( bRemove )
		{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			SaveQuest( &mover, aQuest, aCompleteQuest, aCheckedQuest );
			FILEOUT( "query_removequest.txt", "UPDATE CHARACTER_TBL SET m_lpQuestCntArray = '%s', m_aCompleteQuest = '%s', m_aCheckedQuest = '%s' WHERE m_idPlayer = '%07d' and serverindex = '%02d'",
				aQuest, aCompleteQuest, aCheckedQuest, mover.m_idPlayer, g_appInfo.dwSys );
#else // __IMPROVE_QUEST_INTERFACE
			SaveQuest( &mover, aQuest, aCompleteQuest );
			FILEOUT( "query_removequest.txt", "UPDATE CHARACTER_TBL SET m_lpQuestCntArray = '%s', m_aCompleteQuest = '%s' WHERE m_idPlayer = '%07d' and serverindex = '%02d'",
				aQuest, aCompleteQuest, mover.m_idPlayer, g_appInfo.dwSys );
#endif // __IMPROVE_QUEST_INTERFACE
		}
	}
	OutputDebugString( "RemoveQuest complete" );
	SAFE_DELETE_ARRAY( pSql );
	SAFE_DELETE( pQuery );
	return TRUE;
}

