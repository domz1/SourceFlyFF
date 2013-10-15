#include "stdafx.h"
#include "globalgiftbox.h"
#include "dpmng.h"
#include "msghdr.h"

CGlobalGiftbox::CGlobalGiftbox()
{
	m_bLoad	= FALSE;
}

CGlobalGiftbox::~CGlobalGiftbox()
{
	m_map.clear();
}

CGlobalGiftbox*	CGlobalGiftbox::GetInstance( void )
{
	static CGlobalGiftbox sGlobalGiftbox;
	return	&sGlobalGiftbox;
}

void CGlobalGiftbox::OnUpload( CAr & ar )
{
	if( m_bLoad )
		return;

	// recv
	int nSize;
	ar >> nSize;
	for( int i = 0; i < nSize; i++ )
	{
		DWORD dwGiftbox, dwItem;
		int cbItem, nNum;
		ar >> dwGiftbox;
		ar >> cbItem;
		for( int j = 0; j < cbItem; j++ )
		{
			ar >> dwItem >> nNum;
			DWORD dwKey	= MAKELONG( (WORD)dwGiftbox, (WORD)dwItem );
			bool bResult	= m_map.insert( map<DWORD, int>::value_type( dwKey, nNum ) ).second;
			if( !bResult )
				Error( "global giftbox error: %d, %d, %d", dwGiftbox, dwItem, nNum );
		}
	}
	// load script
	CScanner s;
	DWORD dwGiftbox, dwItem;
	int nNum;
	if( s.Load( "../globalgiftbox.txt" ) )
	{
		dwGiftbox	= (DWORD)s.GetNumber();
		while( s.tok != FINISHED )
		{
			dwItem	= (DWORD)s.GetNumber();
			nNum	= s.GetNumber();

			map<DWORD, int>::iterator it	= m_map.find( MAKELONG( (WORD)dwGiftbox, (WORD)dwItem ) );
			if( it == m_map.end() )
			{
				char szString[100];
				sprintf( szString, "globalgiftbox load error: key not found, g: %d, i: %d", dwGiftbox, dwItem );
				AfxMessageBox( szString );
			}
//			it->second	-= nNum;
			it->second	= nNum;
			if( it->second < 0 )
			{
				Error( "global giftbox error: number is negative, %d, %d", dwGiftbox, dwItem );
				it->second	= 0;
			}
			dwGiftbox	= (DWORD)s.GetNumber();
		}
	}

	m_bLoad	= TRUE;
}

void CGlobalGiftbox::OnQuery( CDPMng<CBuffer>* pdp, CAr & ar, DPID dpid )	// recv & check & save
{
	u_long idPlayer;
	DWORD dwGiftbox, dwItem, dwObjId;
	int nNum, nQueryGiftbox;
	ar >> idPlayer >> dwGiftbox >> dwItem >> nNum >> dwObjId >> nQueryGiftbox;

	map<DWORD, int>::iterator i	= m_map.find( MAKELONG( (WORD)dwGiftbox, (WORD)dwItem ) );
	if( i == m_map.end() )
	{
		Error( "CGlobalGiftbox::OnQuery: key not found, g: %d, i: %d", dwGiftbox, dwItem );
		return;
	}
	BOOL bResult	= FALSE;
	if( i->second > 0 )
	{
		i->second--;
		bResult	= TRUE;
	}

	CAr arResult;
	arResult << PACKETTYPE_QUERYGLOBALGIFTBOX;
	arResult << idPlayer << dwGiftbox << dwItem << nNum << dwObjId << nQueryGiftbox << bResult;
	int nBufSize;
	LPBYTE lpBuf	= arResult.GetBuffer( &nBufSize );
	pdp->Send( lpBuf, nBufSize, dpid );

	Save();
}

void CGlobalGiftbox::OnRestore( CAr & ar )
{
	DWORD dwGiftbox, dwItem;
	ar >> dwGiftbox >> dwItem;
	map<DWORD, int>::iterator i	= m_map.find( MAKELONG( (WORD)dwGiftbox, (WORD)dwItem ) );
	if( i == m_map.end() )
	{
		Error( "CGlobalGiftbox::OnRestore: key not found, g: %d, i: %d", dwGiftbox, dwItem );
		return;
	}
	i->second++;

	Save();
}

BOOL CGlobalGiftbox::Save( void )
{
	FILE *f	= fopen( "../globalgiftbox.txt", "w" );
	if( f )
	{
		for( map<DWORD, int>::iterator i = m_map.begin(); i != m_map.end(); ++i )
		{
			DWORD dwGiftbox, dwItem;
			dwGiftbox	= (DWORD)LOWORD( i->first );
			dwItem	= (DWORD)HIWORD( i->first );
			int nNum	= i->second;
			fprintf( f, "%d\t%d\t%d\n", dwGiftbox, dwItem, nNum );
		}
		fclose( f );
		return TRUE;
	}
	return FALSE;
}
