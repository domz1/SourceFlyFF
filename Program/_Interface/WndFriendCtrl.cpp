// WndBase.cpp: implementation of the CWndBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "DPClient.h"
#include "party.h"
#include "WndFriendCtrl.h"
#include "WndMessenger.h"
//#include "company.h"
extern	CDPClient	g_DPlay;
extern CParty g_Party;
//extern	CCompanyMng		g_CompanyMng;

#include "WndManager.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndFriendCtrl
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndFriendCtrl::CWndFriendCtrl() 
{
	m_uParent = 0;
	m_nCurSelect = -1;

	m_bDrag = FALSE;
	m_pVBGauge = NULL;

	memset( m_uServerPlayerId, 0, sizeof( m_uServerPlayerId ) );
	memset( m_nServerCount, 0, sizeof( m_nServerCount) );
}
CWndFriendCtrl::~CWndFriendCtrl()
{
	DeleteDeviceObjects();
}

void CWndFriendCtrl::Create( DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID )
{
	m_dwListCtrlStyle = dwListCtrlStyle;
	CWndBase::Create( WBS_CHILD, rect, pParentWnd, nID );
}

void CWndFriendCtrl::SetSearch( int nSelectServer, int nSex, LPCTSTR strId )
{
	m_nSelectServer = nSelectServer;
	m_nSex = nSex;
	strcpy( m_szId, strId );
}

void CWndFriendCtrl::LoadListBoxScript(LPCTSTR lpFileName) 
{
}	
void CWndFriendCtrl::InterpriteScript(CScanner& scanner,CPtrArray& ptrArray) 
{
}
HRESULT CWndFriendCtrl::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndFriendCtrl::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndFriendCtrl::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
void CWndFriendCtrl::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);
	m_nFontHeight = 20;

	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauFillSmall.bmp" ), 0xffff00ff, TRUE );

	m_menu.CreateMenu( this );	
}

#ifdef __RT_1025
void CWndFriendCtrl::OnDraw( C2DRender* p2DRender ) 
{
	CPlayerDataCenter* pPlayerDataCenter	= CPlayerDataCenter::GetInstance();
	CPoint pt( 3, 3 );
	m_nDrawCount = 0;
	memset( m_nServerCount, 0, sizeof( m_nServerCount ) );

	if( NULL == g_pPlayer )
		return;

	// Draw Max Count
	int nMax = GetDrawCount();

	CWndWorld*		pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ 6 * 3 * nMax ];
	TEXTUREVERTEX2* pVertices = pVertex;
	
//	for( int i = 0; i < m_wndScrollBar.GetScrollPos() && iter != g_WndMng.m_Messenger.m_aFriend.end(); i++, iter++, m_nDrawCount++ );
	int i = 0;
	m_nDrawCount = m_wndScrollBar.GetScrollPos();
	// Draw Select
	for( map<u_long, Friend>::iterator	i1	= g_WndMng.m_RTMessenger.begin(); i1 != g_WndMng.m_RTMessenger.end(); ++i1 )
	{
		u_long idFriend	= i1->first;
		Friend* pFriend		= &i1->second;
		u_long uFriendLogin		= CPlayerDataCenter::GetInstance()->GetPlayerData( idFriend )->data.uLogin;
		int nMulti	=  uFriendLogin - 1;

		if( uFriendLogin == 100 || pFriend->dwState == FRS_OFFLINE )
		{	
			m_uServerPlayerId[10][m_nServerCount[10]]	= idFriend;
			++m_nServerCount[10];
		}
		else
		{
			m_uServerPlayerId[nMulti][m_nServerCount[nMulti]]		= idFriend;
			++m_nServerCount[nMulti];
		}
	}

	// My Multi Draw
	int nMyMuilti	= g_Neuz.m_uIdofMulti - 1;

	for( int j = 0 ; j < m_nServerCount[nMyMuilti] && i < nMax; ++j, ++i )
	{
		if( i < m_nDrawCount )
			continue;
		u_long idFriend		= m_uServerPlayerId[nMyMuilti][j];
		Friend* pFriend		= g_WndMng.m_RTMessenger.GetFriend( idFriend );
		DWORD dwState	= 0;
		u_long uFriendLogin		= CPlayerDataCenter::GetInstance()->GetPlayerData( idFriend )->data.uLogin; 

		DWORD dwColor = 0xffffffff;//ADEILSON 0xff000000
		if( i == m_nCurSelect )
			dwColor = 0xff00ffff;//ADEILSON 0xff6060ff
		
		int x = 0, nWidth = m_rectClient.Width();	// - 1;
		
		CRect rect( x, pt.y, x + nWidth, pt.y + m_nFontHeight );
		rect.SetRect( x + 3, pt.y + 6, x + 3 + 32, pt.y + 6 + 32 ); 
		if( pFriend->dwState	== FRS_AUTOABSENT )
		{
			dwState		= FRS_ABSENT;
		}
		else
		{
			if( pFriend->dwState == FRS_OFFLINEBLOCK )
				dwState		= FRS_OFFLINE;
			else
				dwState		= pFriend->dwState;
		}

		CString string;
		if( pFriend->dwState == FRS_ONLINE || pFriend->dwState == FRS_OFFLINE || pFriend->dwState == FRS_OFFLINEBLOCK )
		{
			if( pFriend->dwState == FRS_OFFLINEBLOCK )
			{
				string.Format( "%s(%s)", pPlayerDataCenter->GetPlayerString( idFriend ), prj.GetText( TID_FRS_ONLINE + 2 ) );
				dwColor = 0xffff0000;//ADEILSON 0xff000000
			}
			else
			{
				string	= pPlayerDataCenter->GetPlayerString( idFriend );
			}
		}
		else
		{	
			string.Format( "%s(%s)", pPlayerDataCenter->GetPlayerString( idFriend ), prj.GetText( TID_FRS_ONLINE + dwState ) );
		}
		
		p2DRender->TextOut( x + 40, pt.y + 3, string, 0xff00ff00 );//ADEILSON 0xff000000 				

		PlayerData* pPlayerData	= pPlayerDataCenter->GetPlayerData( idFriend );
		LONG nJob	= pPlayerData->data.nJob;
		BYTE nSex	= pPlayerData->data.nSex;
		if( dwState == FRS_OFFLINE )
		{
			if( MAX_EXPERT <= nJob )
			{
#if __VER >= 10 // __LEGEND
				if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 16 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
				else if( MAX_MASTER <= nJob )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 24 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
				else
#ifdef __3RD_LEGEND16
					if ( MAX_HERO <= nJob )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 32 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
				else
#endif //__3RD_LEGEND16
#endif //__LEGEND
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 66 + nJob - 6 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
			}
			else
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  12 + nJob + ( 6 * nSex ), &pVertices, 0xffff6464 );
			}
		}
		else
		{
			if( MAX_EXPERT <= nJob )
			{
#if __VER >= 10 // __LEGEND
				if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 16 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
				else if( MAX_MASTER <= nJob )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 24 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
				else
#ifdef __3RD_LEGEND16
					if ( MAX_HERO <= nJob )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 32 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
				else
#endif //__3RD_LEGEND16
#endif //__LEGEND
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 6 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
			}
			else
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  12 + nJob + ( 6 * nSex ), &pVertices, 0xffffffff );
			}

			if( dwState != FRS_ONLINE )
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 12, pt.y + 10 ), 33 + ( dwState - 2 ), &pVertices, 0xffffffff );
			}
			if( uFriendLogin != 100 )
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 21, pt.y ), 38 + uFriendLogin, &pVertices, 0xffffffff );
			}
		}
		pt.y += m_nFontHeight;// + 3;
	}

	// Differ Multi Draw
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == nMyMuilti )
			continue;

		for( int k = 0 ; k < m_nServerCount[j] && i < nMax ; ++k , ++i )
		{
			if( i < m_nDrawCount )
				continue;

			u_long idFriend	= m_uServerPlayerId[j][k];
			Friend* pFriend		= g_WndMng.m_RTMessenger.GetFriend( idFriend );
			u_long uFriendLogin		= CPlayerDataCenter::GetInstance()->GetPlayerData( idFriend )->data.uLogin; 
			DWORD dwState	= 0;
			
			DWORD dwColor = 0xffffffff;//ADEILSON 0xff000000
			
			if( i == m_nCurSelect )
				dwColor = 0xff00ffff;//ADEILSON 0xff6060ff 
			
			int x = 0, nWidth = m_rectClient.Width();	// - 1;
			CRect rect( x, pt.y, x + nWidth, pt.y + m_nFontHeight );
			rect.SetRect( x + 3, pt.y + 6, x + 3 + 32, pt.y + 6 + 32 ); 

			if( pFriend->dwState == FRS_AUTOABSENT )
			{
				dwState		= FRS_ABSENT;
			}
			else
			{
				if( pFriend->dwState == FRS_OFFLINEBLOCK )
					dwState		= FRS_OFFLINE;
				else
					dwState		= pFriend->dwState;
			}

			CString string;
			if( pFriend->dwState == FRS_ONLINE || pFriend->dwState == FRS_OFFLINE || pFriend->dwState == FRS_OFFLINEBLOCK )
			{
				if( pFriend->dwState == FRS_OFFLINEBLOCK )
				{
					string.Format( "%s(%s)", pPlayerDataCenter->GetPlayerString( idFriend ), prj.GetText( TID_FRS_ONLINE + 2 ) );
					dwColor = 0xffff0000;//ADEILSON 0xff000000 
				}
				else
				{
					string	= pPlayerDataCenter->GetPlayerString( idFriend );
				}
			}
			else
			{	
				string.Format( "%s(%s)", pPlayerDataCenter->GetPlayerString( idFriend ), prj.GetText( TID_FRS_ONLINE + dwState ) );
			}
			
			p2DRender->TextOut( x + 40, pt.y + 3, string, 0xff00ff00 ); //ADEILSON 0xff000000				

		PlayerData* pPlayerData	= pPlayerDataCenter->GetPlayerData( idFriend );
		LONG nJob	= pPlayerData->data.nJob;
		BYTE nSex	= pPlayerData->data.nSex;
		
			if( dwState == FRS_OFFLINE )
			{
				if( MAX_EXPERT <= nJob )
				{
#if __VER >= 10 // __LEGEND
				if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 16 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
				else if( MAX_MASTER <= nJob )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 24 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
				else
#ifdef __3RD_LEGEND16
					if ( MAX_HERO <= nJob )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 32 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
				else
#endif //__3RD_LEGEND16
#endif //__LEGEND
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 6 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
				}
				else
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  12 + nJob + ( 6 * nSex ), &pVertices, 0xffff6464 );
				}
			}
			else
			{
				if( MAX_EXPERT <= nJob )
				{
#if __VER >= 10 // __LEGEND
				if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 16 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
				else if( MAX_MASTER <= nJob )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 24 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
				else
#ifdef __3RD_LEGEND16
					if ( MAX_HERO <= nJob )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 32 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
				else
#endif //__3RD_LEGEND16
#endif //__LEGEND
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 6 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
				}
				else
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  12 + nJob + ( 6 * nSex ), &pVertices, 0xffffffff );
				}
				if( dwState != FRS_ONLINE )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 12, pt.y + 10 ), 33 + ( dwState - 2 ), &pVertices, 0xffffffff );
				}
				if( uFriendLogin != 100 )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 21, pt.y ), 38 + uFriendLogin, &pVertices, 0xffffffff );
				}
			}
			pt.y += m_nFontHeight;// + 3;
		}
	}
	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	SAFE_DELETE_ARRAY( pVertex );
}
#else	// __RT_1025
void CWndFriendCtrl::OnDraw( C2DRender* p2DRender ) 
{
#if __VER >= 11 // __SYS_PLAYER_DATA
	CPlayerDataCenter* pPlayerDataCenter	= CPlayerDataCenter::GetInstance();
#endif	// __SYS_PLAYER_DATA
	CPoint pt( 3, 3 );
	m_nDrawCount = 0;
	memset( m_nServerCount, 0, sizeof( m_nServerCount ) );

	if( NULL == g_pPlayer )
		return;

	// Draw Max Count
	int nMax = GetDrawCount();

	CWndWorld*		pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ 6 * 3 * nMax ];
	TEXTUREVERTEX2* pVertices = pVertex;

	C2FriendPtr::iterator iter = g_WndMng.m_Messenger.m_aFriend.begin();
//	for( int i = 0; i < m_wndScrollBar.GetScrollPos() && iter != g_WndMng.m_Messenger.m_aFriend.end(); i++, iter++, m_nDrawCount++ );
	int i = 0;
	m_nDrawCount = m_wndScrollBar.GetScrollPos();
	// Draw Select
	for( ; iter != g_WndMng.m_Messenger.m_aFriend.end() ; ++iter )
	{
		LPFRIEND lpFriend = (LPFRIEND)iter->second;
		int nMuilti = lpFriend->m_uIdofMulti -1;
		
		if( lpFriend->m_uIdofMulti == 100 || lpFriend->dwState == FRS_OFFLINE ) // logOff Last Draw
		{	
			m_uServerPlayerId[10][m_nServerCount[10]] = lpFriend->dwUserId;
			++m_nServerCount[10];
		}
		else	// Multi Server
		{
			m_uServerPlayerId[nMuilti][m_nServerCount[nMuilti]] = lpFriend->dwUserId;
			++m_nServerCount[nMuilti];
		}
	}

	// My Multi Draw
	int nMyMuilti	= g_Neuz.m_uIdofMulti - 1;

	for( int j = 0 ; j < m_nServerCount[nMyMuilti] && i < nMax; ++j, ++i )
	{
		if( i < m_nDrawCount )
			continue;

		LPFRIEND lpFriend	= g_WndMng.m_Messenger.GetFriend( m_uServerPlayerId[nMyMuilti][j] );
		DWORD dwState;

		// 상태에 따라 색 변경

		DWORD dwColor = 0xffffffff;//ADEILSON 0xff000000
		if( i == m_nCurSelect )
			dwColor = 0xff00ffff;//ADEILSON 0xff6060ff 
		
		int x = 0, nWidth = m_rectClient.Width();// - 1;
		
		CRect rect( x, pt.y, x + nWidth, pt.y + m_nFontHeight );
		rect.SetRect( x + 3, pt.y + 6, x + 3 + 32, pt.y + 6 + 32 ); 
		if( lpFriend->dwState == FRS_AUTOABSENT )
		{
			dwState = FRS_ABSENT;
		}
		else
		{
			if( lpFriend->dwState == FRS_OFFLINEBLOCK )
				dwState = FRS_OFFLINE;
			else
				dwState = lpFriend->dwState;
		}

		CString string;
		if( lpFriend->dwState == FRS_ONLINE || lpFriend->dwState == FRS_OFFLINE || lpFriend->dwState == FRS_OFFLINEBLOCK )
		{
			if( lpFriend->dwState == FRS_OFFLINEBLOCK )
			{
#if __VER >= 11 // __SYS_PLAYER_DATA
				string.Format( "%s(%s)", pPlayerDataCenter->GetPlayerString( lpFriend->dwUserId ), prj.GetText( TID_FRS_ONLINE + 2 ) );
#else	// __SYS_PLAYER_DATA
				string.Format( "%s(%s)", lpFriend->szName, prj.GetText( TID_FRS_ONLINE + 2 ) );
#endif	// __SYS_PLAYER_DATA
				dwColor = 0xffff0000;//ADEILSON 0xff000000
			}
			else
			{
#if __VER >= 11 // __SYS_PLAYER_DATA
				string	= pPlayerDataCenter->GetPlayerString( lpFriend->dwUserId );
#else	// __SYS_PLAYER_DATA
				string = lpFriend->szName;
#endif	// __SYS_PLAYER_DATA
			}
		}
		else
		{	
#if __VER >= 11 // __SYS_PLAYER_DATA
			string.Format( "%s(%s)", pPlayerDataCenter->GetPlayerString( lpFriend->dwUserId ), prj.GetText( TID_FRS_ONLINE + dwState ) );
#else	// __SYS_PLAYER_DATA
			string.Format( "%s(%s)", lpFriend->szName, prj.GetText( TID_FRS_ONLINE + dwState ) );
#endif	// __SYS_PLAYER_DATA
		}
		
		p2DRender->TextOut( x + 40, pt.y + 3, string, 0xff00ff00 ); 				

#if __VER >= 11 // __SYS_PLAYER_DATA
		PlayerData* pPlayerData	= pPlayerDataCenter->GetPlayerData( lpFriend->dwUserId );
		LONG nJob	= pPlayerData->nJob;
		BYTE nSex	= pPlayerData->nSex;
#else	// __SYS_PLAYER_DATA
		LONG nJob	= lpFriend->nJob;
		BYTE nSex	= lpFriend->nSex;
#endif	// __SYS_PLAYER_DATA
		if( dwState == FRS_OFFLINE )
		{
			if( MAX_EXPERT <= nJob )
			{
#if __VER >= 10 // __LEGEND
				if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 16 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
				else if( MAX_MASTER <= nJob )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 24 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
				else
#endif //__LEGEND
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 66 + nJob - 6 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
			}
			else
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  12 + nJob + ( 6 * nSex ), &pVertices, 0xffff6464 );
			}
		}
		else
		{
			if( MAX_EXPERT <= nJob )
			{
#if __VER >= 10 // __LEGEND
				if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 16 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
				else if( MAX_MASTER <= nJob )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 24 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
				else
#endif //__LEGEND
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 6 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
			}
			else
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  12 + nJob + ( 6 * nSex ), &pVertices, 0xffffffff );
			}

			if( dwState != FRS_ONLINE )
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 12, pt.y + 10 ), 33 + ( dwState - 2 ), &pVertices, 0xffffffff );
			}
			if( lpFriend->m_uIdofMulti != 100 )
			{
				pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 21, pt.y ), 38 + lpFriend->m_uIdofMulti, &pVertices, 0xffffffff );
			}
		}
		pt.y += m_nFontHeight;// + 3;
	}

	// Differ Multi Draw
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == nMyMuilti )
			continue;

		for( int k = 0 ; k < m_nServerCount[j] && i < nMax ; ++k , ++i )
		{
			if( i < m_nDrawCount )
				continue;
			
			u_long uFriendId = m_uServerPlayerId[j][k];
			LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
			DWORD dwState;
			
			// 상태에 따라 색 변경
			DWORD dwColor = 0xffffffff;//ADEILSON 0xff000000
			
			if( i == m_nCurSelect )
				dwColor = 0xff00ffff;//ADEILSON 0xff6060ff 
			
			int x = 0, nWidth = m_rectClient.Width();// - 1;
			CRect rect( x, pt.y, x + nWidth, pt.y + m_nFontHeight );
			rect.SetRect( x + 3, pt.y + 6, x + 3 + 32, pt.y + 6 + 32 ); 

			if( lpFriend->dwState == FRS_AUTOABSENT )
			{
				dwState = FRS_ABSENT;
			}
			else
			{
				if( lpFriend->dwState == FRS_OFFLINEBLOCK )
					dwState = FRS_OFFLINE;
				else
					dwState = lpFriend->dwState;
			}

			CString string;
			if( lpFriend->dwState == FRS_ONLINE || lpFriend->dwState == FRS_OFFLINE || lpFriend->dwState == FRS_OFFLINEBLOCK )
			{
				if( lpFriend->dwState == FRS_OFFLINEBLOCK )
				{
#if __VER >= 11 // __SYS_PLAYER_DATA
					string.Format( "%s(%s)", pPlayerDataCenter->GetPlayerString( lpFriend->dwUserId ), prj.GetText( TID_FRS_ONLINE + 2 ) );
#else	// __SYS_PLAYER_DATA
					string.Format( "%s(%s)", lpFriend->szName, prj.GetText( TID_FRS_ONLINE + 2 ) );
#endif	// __SYS_PLAYER_DATA
					dwColor = 0xffff0000;//ADEILSON 0xff000000 
				}
				else
				{
#if __VER >= 11 // __SYS_PLAYER_DATA
					string	= pPlayerDataCenter->GetPlayerString( lpFriend->dwUserId );
#else	// __SYS_PLAYER_DATA
					string	= lpFriend->szName;
#endif	// __SYS_PLAYER_DATA
				}
			}
			else
			{	
#if __VER >= 11 // __SYS_PLAYER_DATA
				string.Format( "%s(%s)", pPlayerDataCenter->GetPlayerString( lpFriend->dwUserId ), prj.GetText( TID_FRS_ONLINE + dwState ) );
#else	// __SYS_PLAYER_DATA
				string.Format( "%s(%s)", lpFriend->szName, prj.GetText( TID_FRS_ONLINE + dwState ) );
#endif	// __SYS_PLAYER_DATA
			}
			
			p2DRender->TextOut( x + 40, pt.y + 3, string, 0xff00ff00 ); 				

#if __VER >= 11 // __SYS_PLAYER_DATA
		PlayerData* pPlayerData	= pPlayerDataCenter->GetPlayerData( lpFriend->dwUserId );
		LONG nJob	= pPlayerData->nJob;
		BYTE nSex	= pPlayerData->nSex;
#else	// __SYS_PLAYER_DATA
		LONG nJob	= lpFriend->nJob;
		BYTE nSex	= lpFriend->nSex;
#endif	// __SYS_PLAYER_DATA
			
			if( dwState == FRS_OFFLINE )
			{
				if( MAX_EXPERT <= nJob )
				{
#if __VER >= 10 // __LEGEND
				if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 16 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
				else if( MAX_MASTER <= nJob )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 24 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
				else
#endif //__LEGEND
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 6 ) + ( 8 * nSex ), &pVertices, 0xffff6464 );
				}
				else
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  12 + nJob + ( 6 * nSex ), &pVertices, 0xffff6464 );
				}
			}
			else
			{
				if( MAX_EXPERT <= nJob )
				{
#if __VER >= 10 // __LEGEND
				if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 16 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
				else if( MAX_MASTER <= nJob )
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 24 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
				else
#endif //__LEGEND
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  ( 70 + nJob - 6 ) + ( 8 * nSex ), &pVertices, 0xffffffff );
				}
				else
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 2, pt.y ),  12 + nJob + ( 6 * nSex ), &pVertices, 0xffffffff );
				}
				if( dwState != FRS_ONLINE )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 12, pt.y + 10 ), 33 + ( dwState - 2 ), &pVertices, 0xffffffff );
				}
				if( lpFriend->m_uIdofMulti != 100 )
				{
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( 21, pt.y ), 38 + lpFriend->m_uIdofMulti, &pVertices, 0xffffffff );
				}
			}
			pt.y += m_nFontHeight;// + 3;
		}
	}
	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
	SAFE_DELETE_ARRAY( pVertex );
}
#endif	// __RT_1025

void CWndFriendCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bDrag = FALSE;

	CPoint pt( 3, 3 );
	CRect rect;

	int count = m_nDrawCount;	
	
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
	{
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			m_nCurSelect = count;
			return;
		}
		pt.y += m_nFontHeight;
	}
	
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
		{
			continue;
		}
		for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
		{
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				m_nCurSelect = count;
				return;
			}
			pt.y += m_nFontHeight;
		}
	}
	g_WorldMng()->SetObjFocus(NULL);
}
void CWndFriendCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
}

#ifdef __RT_1025
void CWndFriendCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	u_long idFriend;
	Friend* pFriend;
	int nSelect		= GetSelect( point, idFriend, &pFriend );
	if( nSelect != -1 )
	{
		CPlayerDataCenter* pPlayerDataCenter	= CPlayerDataCenter::GetInstance();
#ifdef __RT_1025
		if( pFriend->dwState != FRS_OFFLINE && !pFriend->bBlock )
#else	// __RT_1025
		if( pFriend->dwState != FRS_OFFLINE && pFriend->dwState != FRS_BLOCK && pFriend->dwState != FRS_OFFLINEBLOCK )
#endif	// __RT_1025
		{
			m_nCurSelect	= nSelect;
			CWndMessage* pWndMessage	= g_WndMng.OpenMessage( pPlayerDataCenter->GetPlayerString( idFriend ) );
		}
		else
		{
			CString szMessage;
			if( pFriend->dwState == FRS_OFFLINE )
				szMessage	= prj.GetText(TID_GAME_NOTLOGIN);	//szMessage += "님은 접속되어 있지 않습니다";
			else
				szMessage.Format( prj.GetText( TID_GAME_MSGBLOCKCHR ), pPlayerDataCenter->GetPlayerString( idFriend ) );  //szMessage += "님은 차단되어 있어 메세지를 보낼수 없습니다";
			g_WndMng.PutString( szMessage, NULL, prj.GetTextColor(TID_GAME_NOTLOGIN) );
		}
	}	
}
#else	// __RT_1025
//{{AFX
void CWndFriendCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	LPFRIEND lpFriend = NULL;
	int nSelect = GetSelect( point, &lpFriend );
	if( nSelect != -1 )
	{
#ifdef __SYS_PLAYER_DATA
		CPlayerDataCenter* pPlayerDataCenter	= CPlayerDataCenter::GetInstance();
#endif	// __SYS_PLAYER_DATA
		if( lpFriend->dwState != FRS_OFFLINE && lpFriend->dwState != FRS_BLOCK && lpFriend->dwState != FRS_OFFLINEBLOCK )
		{
			m_nCurSelect = nSelect;
#ifdef __SYS_PLAYER_DATA
			CWndMessage* pWndMessage	= g_WndMng.OpenMessage( pPlayerDataCenter->GetPlayerString( lpFriend->dwUserId ) );
#else	// __SYS_PLAYER_DATA
			CWndMessage* pWndMessage = g_WndMng.OpenMessage( lpFriend->szName );
#endif	// __SYS_PLAYER_DATA
		}
		else
		{
			CString szMessage;
			if( lpFriend->dwState == FRS_OFFLINE )
				szMessage = prj.GetText(TID_GAME_NOTLOGIN);                               //szMessage += "님은 접속되어 있지 않습니다";
			else
#ifdef __SYS_PLAYER_DATA
				szMessage.Format( prj.GetText( TID_GAME_MSGBLOCKCHR ), pPlayerDataCenter->GetPlayerString( lpFriend->dwUserId ) );  //szMessage += "님은 차단되어 있어 메세지를 보낼수 없습니다";
#else	// __SYS_PLAYER_DATA
				szMessage.Format( prj.GetText(TID_GAME_MSGBLOCKCHR), lpFriend->szName );  //szMessage += "님은 차단되어 있어 메세지를 보낼수 없습니다";
#endif	// __SYS_PLAYER_DATA
			g_WndMng.PutString( szMessage, NULL, prj.GetTextColor(TID_GAME_NOTLOGIN) );
			
		}
	}	
}
//}}AFX
#endif	// __RT_1025

#ifdef __RT_1025
void	CWndFriendCtrl::GetSelectFriend( int SelectCount, u_long & idPlayer, Friend** ppFriend )
{
	int count	= 0;
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
	{
		if( count < m_nDrawCount )
			continue;
		
		if( SelectCount == count )
		{
			u_long idFriend		= m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j]; 
			Friend* pFriend		= g_WndMng.m_RTMessenger.GetFriend( idFriend );
			if( pFriend )
			{
				idPlayer	= idFriend;
				*ppFriend	= pFriend;
				return;
			}
		}
	}
	
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
			continue;
		for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
		{
			if( count < m_nDrawCount )
				continue;
			
			if( SelectCount == count )
			{
				u_long idFriend		= m_uServerPlayerId[j][k];
				Friend* pFriend		= g_WndMng.m_RTMessenger.GetFriend( idFriend );
				if( pFriend )
				{
					idPlayer	= idFriend;
					*ppFriend	= pFriend;
					return;
				}
			}
		}
	}
	idPlayer	= 0;
	*ppFriend	= NULL;
}
#else	// __RT_1025
//{{AFX
void CWndFriendCtrl::GetSelectFriend( int SelectCount, LPFRIEND* lppFriend )
{
	int count = 0;
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
	{
		if( count < m_nDrawCount )
		{
			continue;
		}
		
		if( SelectCount == count )
		{
			u_long uFriendId = m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j];
			LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
			if( lpFriend )
			{
				*lppFriend = lpFriend;
				return;
			}
		}
	}
	
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
		{
			continue;
		}
		for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
		{
			if( count < m_nDrawCount )
			{
				continue;
			}
			
			if( SelectCount == count )
			{
				u_long uFriendId = m_uServerPlayerId[j][k];
				LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
				if( lpFriend )
				{
					*lppFriend = lpFriend;
					return;
				}
			}
		}
	}
	
	*lppFriend = NULL;
}
//}}AFX
#endif	// __RT_1025

u_long CWndFriendCtrl::GetSelectId( int SelectCount )
{
	int count = 0;
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
	{
		if( count < m_nDrawCount )
		{
			continue;
		} 
		if( SelectCount == count )
		{
#ifdef __RT_1025
			u_long idFriend	= m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j];
			Friend* pFriend		= g_WndMng.m_RTMessenger.GetFriend( idFriend );
			if( pFriend )
				return idFriend;
#else	// __RT_1025
			u_long uFriendId = m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j];
			LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
			if( lpFriend )
				return lpFriend->dwUserId;
#endif	// __RT_1025
		}
	}
	
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
			continue;

		for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
		{
			if( j == g_Neuz.m_uIdofMulti -1 )
				continue;

			if( SelectCount == count )
			{
#ifdef __RT_1025
				u_long idFriend		= m_uServerPlayerId[j][k];
				Friend* pFriend		= g_WndMng.m_RTMessenger.GetFriend( idFriend );
				if( pFriend )
					return idFriend;
#else	// __RT_1025
				u_long uFriendId = m_uServerPlayerId[j][k];
				LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
				if( lpFriend )
					return lpFriend->dwUserId;
#endif	// __RT_1025
			}
		}
	}
	return -1;
}

#ifdef __RT_1025
int CWndFriendCtrl::GetSelect( CPoint point, u_long & idPlayer, Friend** ppFriend )
#else	// __RT_1025
int CWndFriendCtrl::GetSelect( CPoint point, LPFRIEND* lppFriend )
#endif	// __RT_1025
{
	CPoint pt( 3, 3 );
	CRect rect;
	int count = 0;	
	for( int j = 0 ; j < m_nServerCount[g_Neuz.m_uIdofMulti-1] ; ++j, ++count )
	{
		if( count < m_nDrawCount )
		{
			continue;
		} 
		
		rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
		if( rect.PtInRect( point ) )
		{
			u_long uFriendId = m_uServerPlayerId[g_Neuz.m_uIdofMulti-1][j];
#ifdef __RT_1025
			Friend* pFriend		= g_WndMng.m_RTMessenger.GetFriend( uFriendId );
			if( pFriend )
			{
				idPlayer	= uFriendId;
				if( pFriend )
					*ppFriend	= pFriend;
			}				
#else	// __RT_1025
			LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
			if( lpFriend )
				*lppFriend = lpFriend;
#endif	// __RT_1025
			return count;
		}
		pt.y += m_nFontHeight;
	}
	
	for( j = 0 ; j < 11 ; ++j )
	{
		if( j == g_Neuz.m_uIdofMulti -1 )
		{
			continue;
		}
		for( int k = 0 ; k < m_nServerCount[j] ; ++k, ++count )
		{
			if( count < m_nDrawCount )
			{
				continue;
			} 
			
			rect.SetRect( pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight );
			if( rect.PtInRect( point ) )
			{
				u_long uFriendId = m_uServerPlayerId[j][k];
#ifdef __RT_1025
				Friend* pFriend		= g_WndMng.m_RTMessenger.GetFriend( uFriendId );
				if( pFriend )
				{
					idPlayer	= uFriendId;
					*ppFriend	= pFriend;
				}
#else	// __RT_1025
				LPFRIEND lpFriend = g_WndMng.m_Messenger.GetFriend( uFriendId );
				if( lpFriend )
					*lppFriend = lpFriend;
#endif	// __RT_1025
				return count;
			}
			pt.y += m_nFontHeight;
		}
	}

#ifdef __RT_1025
	idPlayer	= 0;
	*ppFriend	= NULL;
#else	// __RT_1025
	if( lppFriend )
		*lppFriend = NULL;
#endif	// __RT_1025
	return -1;
}


BOOL CWndFriendCtrl::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
#if __VER >= 11 // __SYS_PLAYER_DATA
	CPlayerDataCenter*	pPlayerDataCenter	= CPlayerDataCenter::GetInstance();
#endif	// __SYS_PLAYER_DATA
	m_menu.SetVisible(FALSE);

	switch( nID )
	{
	case 0:		// 메시지
		{
#ifdef __RT_1025
			u_long idPlayer	= 0;
			Friend* pFriend;
			GetSelectFriend( m_nCurSelect, idPlayer, &pFriend );
			if( idPlayer )
				CWndMessage* pWndMessage	= g_WndMng.OpenMessage( pPlayerDataCenter->GetPlayerString( idPlayer ) );
#else	// __RT_1025
			LPFRIEND lpFriend = NULL;
			GetSelectFriend( m_nCurSelect, &lpFriend );
			if( lpFriend )
				CWndMessage* pWndMessage = g_WndMng.OpenMessage( lpFriend->szName );
#endif	// __RT_1025
		}
		break;
	case 1:		// 차단 / 차단해제
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
				g_DPlay.SendFriendInterceptState( uidPlayer );		
		}
		break;
	case 2:		// 삭제
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
				g_DPlay.SendRemoveFriend( uidPlayer );
		}
		break;
	case 3: // 이동
		{
#ifdef __RT_1025
			u_long idPlayer;
			Friend* pFriend;
			GetSelectFriend( m_nCurSelect, idPlayer, &pFriend );
			if( pFriend )
			{
				CString string;
				string.Format( "/teleport \"%s\"", pPlayerDataCenter->GetPlayerString( idPlayer ) );
				ParsingCommand( string.LockBuffer(), g_pPlayer );
				string.UnlockBuffer();
			}
#else	// __RT_1025
			LPFRIEND lpFriend = NULL;
			GetSelectFriend( m_nCurSelect, &lpFriend );
			if( lpFriend )
			{
				CString string;
				string.Format( "/teleport \"%s\"", lpFriend->szName );
				ParsingCommand( string.LockBuffer(), g_pPlayer );
				string.UnlockBuffer();			
			}
#endif	// __RT_1025
		}
		break;
	case 4:	// 극단 초청
		{
			u_long uidPlayer = GetSelectId( m_nCurSelect );
			if( uidPlayer != -1 )
			{
				CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
				if( pWndWorld )
					pWndWorld->InviteParty( uidPlayer );
			}
		}
		break;
	case 5: // 컴퍼니 초청
		{
		}
		break;
	case 6: // 쪽지 보내기
		{
#ifdef __RT_1025
			u_long idPlayer;
			Friend* pFriend;
			GetSelectFriend( m_nCurSelect, idPlayer, &pFriend );
			if( pFriend )
			{
				SAFE_DELETE( g_WndMng.m_pWndMessageNote );
				g_WndMng.m_pWndMessageNote	= new CWndMessageNote;
				strcpy( g_WndMng.m_pWndMessageNote->m_szName, pPlayerDataCenter->GetPlayerString( idPlayer ) );
				g_WndMng.m_pWndMessageNote->m_dwUserId	= idPlayer;
				g_WndMng.m_pWndMessageNote->Initialize();		
			}
#else	// __RT_1025
			LPFRIEND lpFriend = NULL;
			GetSelectFriend( m_nCurSelect, &lpFriend );
			if( lpFriend )
			{
				SAFE_DELETE( g_WndMng.m_pWndMessageNote );
				g_WndMng.m_pWndMessageNote = new CWndMessageNote;
				strcpy( g_WndMng.m_pWndMessageNote->m_szName, lpFriend->szName );
				g_WndMng.m_pWndMessageNote->m_dwUserId = lpFriend->dwUserId;
				g_WndMng.m_pWndMessageNote->Initialize();		
			}
#endif	// __RT_1025
		}
		break;
	}
	return -1; 
} 

void CWndFriendCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
}

void CWndFriendCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
	u_long idPlayer;
#ifdef __RT_1025
	Friend* pFriend;
	int nSelect		= GetSelect( point, idPlayer, &pFriend );
#else	// __RT_1025
	LPFRIEND lpFriend = NULL;
	int nSelect		= GetSelect( point, &lpFriend );
#endif	// __RT_1025
	if( nSelect != -1 )
	{
#ifdef __RT_1025
		idPlayer		= idPlayer;
		DWORD dwState	= pFriend->dwState;
#else	// __RT_1025
		idPlayer		= lpFriend->dwUserId;
		DWORD dwState	= lpFriend->dwState;
#endif	// __RT_1025
		m_nCurSelect	= nSelect;
		ClientToScreen( &point );
		m_menu.DeleteAllMenu();
#ifdef __RT_1025
		if( dwState != FRS_OFFLINE && !pFriend->bBlock )
#else	// __RT_1025
		if( dwState != FRS_OFFLINE && dwState != FRS_BLOCK && dwState != FRS_OFFLINEBLOCK )
#endif	// __RT_1025
			m_menu.AppendMenu( 0, 0 ,_T( prj.GetText( TID_APP_MESSAGE ) ) );
#ifdef __RT_1025
		if( pFriend->bBlock )
#else	// __RT_1025
		if( dwState == FRS_BLOCK || dwState == FRS_OFFLINEBLOCK )
#endif	// __RT_1025
			m_menu.AppendMenu( 0, 1 ,_T( prj.GetText( TID_FRS_BLOCKRESTORE ) ) );
		else
			m_menu.AppendMenu( 0, 1 ,_T( prj.GetText( TID_FRS_BLOCK ) ) );
		m_menu.AppendMenu( 0, 2 ,_T( prj.GetText( TID_FRS_DELETE ) ) );
#ifdef __RT_1025
		if( !pFriend->bBlock )
#else	// __RT_1025
		if( dwState != FRS_OFFLINE && dwState != FRS_OFFLINEBLOCK )
#endif	// __RT_1025
		{					
			if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
				m_menu.AppendMenu( 0, 3 ,_T( prj.GetText( TID_FRS_MOVE2 ) ) );	
#ifndef __RT_1025
			if( dwState != FRS_BLOCK )
#endif	// __RT_1025
			{
				if( g_Party.IsMember( idPlayer ) == FALSE )
					m_menu.AppendMenu( 0, 4 ,_T( prj.GetText( TID_MMI_INVITE_PARTY ) ) );
			}
		}
#ifdef __RT_1025
		if( dwState == FRS_OFFLINE || pFriend->bBlock )
#else	// __RT_1025
		if( dwState == FRS_OFFLINE || dwState == FRS_BLOCK || dwState == FRS_OFFLINEBLOCK )
#endif	// __RT_1025
			m_menu.AppendMenu( 0, 6 , _T( prj.GetText( TID_GAME_TAGSEND ) ) );
		m_menu.Move( point );
		m_menu.SetVisible( TRUE );
		m_menu.SetFocus();
	}
}
void CWndFriendCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
}
void CWndFriendCtrl::OnRButtonDblClk( UINT nFlags, CPoint point)
{
}
void CWndFriendCtrl::SetScrollBar()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
#ifdef __RT_1025
	nRange	= g_WndMng.m_RTMessenger.size();
#else	// __RT_1025
	nRange = g_WndMng.m_Messenger.m_aFriend.size();//m_pItemContainer->m_dwIndexNum;// - nPage;
#endif	// __RT_1025
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}

void CWndFriendCtrl::OnSize( UINT nType, int cx, int cy )
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect( rect );

	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
#ifdef __RT_1025
	nRange	= g_WndMng.m_RTMessenger.size();
#else	// __RT_1025
	nRange = g_WndMng.m_Messenger.m_aFriend.size();//m_pItemContainer->m_dwIndexNum;// - nPage;
#endif	// __RT_1025
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	
	CWndBase::OnSize( nType, cx, cy);
}
void CWndFriendCtrl::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect( 3, 3 );

	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndFriendCtrl::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	p2DRender->RenderFillRect ( rect, dwColor1 );
	p2DRender->RenderRoundRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor2 );
	rect.DeflateRect( 1 , 1 );
	p2DRender->RenderRect( rect, dwColor3 );

	int nPage, nRange;
	if( 1 ) //m_dwListCtrlStyle == WLVS_REPORT )
	{
		m_nFontHeight = 20;//GetFontHeight( &g_2DRender );
		nPage = GetClientRect().Height() / ( m_nFontHeight );
#ifdef __RT_1025
		nRange	= g_WndMng.m_RTMessenger.size();
#else	// __RT_1025
		nRange = g_WndMng.m_Messenger.m_aFriend.size();
#endif	// __RT_1025
	}
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
}
BOOL CWndFriendCtrl::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;
}
BOOL CWndFriendCtrl::SetItem( const LVITEM* pItem )
{
	return TRUE;
}
int CWndFriendCtrl::InsertItem( const LVITEM* pItem )
{
	return TRUE;
}
int CWndFriendCtrl::GetItemCount() const
{
	return 1;//m_aItems.GetSize();
}

void CWndFriendCtrl::ScrollBarPos( int nPos )
{
	m_wndScrollBar.SetScrollPos( nPos );
}

int CWndFriendCtrl::GetDrawCount( void )
{
#ifdef __RT_1025
	int nMax	= g_WndMng.m_RTMessenger.size();
#else	// __RT_1025
	int nMax =  g_WndMng.m_Messenger.m_aFriend.size();
#endif	// __RT_1025
	if( nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage() )
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if( nMax < m_wndScrollBar.GetScrollPos() )
		nMax = 0;

	return nMax;
}