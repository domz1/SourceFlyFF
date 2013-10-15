#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildTabMember.h"
#include "WndManager.h"
#include "DPClient.h"
#include "MsgHdr.h"
#include "defineQuest.h"
extern	CDPClient	g_DPlay;

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#include "guild.h"
extern	CGuildMng	g_GuildMng;

#include "eveschool.h"
extern CGuildCombat g_GuildCombatMng;


/****************************************************
  WndId : APP_GUILD_TABMEMBER 
****************************************************/
#define		MAX_MEMBER_LIST_DRAW	14		// 한화면에 보이는 리스트 수.
#define		MEMBER_LIST_HEIGHT		18		// 리스트의 한라인 폭 y += MEMBER_LIST_HEIGHT 로 쓰라.

#if __VER >= 12 // __CSC_VER12_2
//////////////////////////////////////////////////////////////////////////
// Common Local Func.
//////////////////////////////////////////////////////////////////////////
bool prMemberLevelAsce(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;

	if(player1.nMemberLv > player2.nMemberLv)
		rtn_val = true;
	else if(player1.nMemberLv == player2.nMemberLv)
	{
		if(player1.nClass < player2.nClass)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prMemberLevelDesc(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;

	if(player1.nMemberLv < player2.nMemberLv)
		rtn_val = true;
	else if(player1.nMemberLv == player2.nMemberLv)
	{
		if(player1.nClass > player2.nClass)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prJobAsce(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;

	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[ player1.nJob ].dwJobType;
	nPlayer2JobType = prj.m_aJob[ player2.nJob ].dwJobType;

	if(nPlayer1JobType > nPlayer2JobType)
		rtn_val = true;
	else if(nPlayer1JobType == nPlayer2JobType)
	{
		if(player1.nJob > player2.nJob)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prJobDesc(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;
	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[ player1.nJob ].dwJobType;
	nPlayer2JobType = prj.m_aJob[ player2.nJob ].dwJobType;

	if(nPlayer1JobType < nPlayer2JobType)
		rtn_val = true;
	else if(nPlayer1JobType == nPlayer2JobType)
	{
		if(player1.nJob < player2.nJob)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prLevelAsce(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;
	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[ player1.nJob ].dwJobType;
	nPlayer2JobType = prj.m_aJob[ player2.nJob ].dwJobType;

	if(nPlayer1JobType > nPlayer2JobType)
		rtn_val = true;
	else if(nPlayer1JobType == nPlayer2JobType)
	{
		if(player1.nLevel > player2.nLevel)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prLevelDesc(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;
	int nPlayer1JobType, nPlayer2JobType;

	nPlayer1JobType = prj.m_aJob[ player1.nJob ].dwJobType;
	nPlayer2JobType = prj.m_aJob[ player2.nJob ].dwJobType;

	if(nPlayer1JobType < nPlayer2JobType)
		rtn_val = true;
	else if(nPlayer1JobType == nPlayer2JobType)
	{
		if(player1.nLevel < player2.nLevel)
			rtn_val = true;
	}
	
	return rtn_val;
}

bool prNameAsce(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;
	CString strplayer1Name, strplayer2Name;

	strplayer1Name.Format("%s", player1.szName);
	strplayer2Name.Format("%s", player2.szName);

	if(strplayer1Name > strplayer2Name)
		rtn_val = true;
	
	return rtn_val;
}

bool prNameDesc(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;
	CString strplayer1Name, strplayer2Name;

	strplayer1Name.Format("%s", player1.szName);
	strplayer2Name.Format("%s", player2.szName);

	if(strplayer1Name < strplayer2Name)
		rtn_val = true;
	
	return rtn_val;
}

bool prAliasAsce(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;
	CString strplayer1Name, strplayer2Name;

	strplayer1Name.Format("%s", player1.szAlias);
	strplayer2Name.Format("%s", player2.szAlias);

	if(strplayer1Name > strplayer2Name)
		rtn_val = true;
	
	return rtn_val;
}

bool prAliasDesc(MEMBERLIST player1, MEMBERLIST player2)
{
	bool rtn_val = false;
	CString strplayer1Name, strplayer2Name;

	strplayer1Name.Format("%s", player1.szAlias);
	strplayer2Name.Format("%s", player2.szAlias);

	if(strplayer1Name < strplayer2Name)
		rtn_val = true;
	
	return rtn_val;
}

#endif //__CSC_VER12_2
CWndGuildTabMember::CWndGuildTabMember() 
{ 
//	m_pListCtrl = NULL;
	m_nMax = 0;
	m_nCurrentList = 0;
	m_nSelect = 0;
	memset( m_aList, 0, sizeof(MEMBERLIST) * MAX_MEMBER );
	m_nMxOld = m_nMyOld = 0;
#if __VER >= 12 // __CSC_VER12_2
	m_bSortbyMemberLevel = FALSE;
	m_bSortbyLevel = TRUE;
	m_bSortbyJob = TRUE;
	m_bSortbyName = TRUE;
	m_bSortbyAlias = TRUE;
#endif //__CSC_VER12_2
} 

CWndGuildTabMember::~CWndGuildTabMember() 
{ 
//	SAFE_DELETE( m_pListCtrl );
} 

void CWndGuildTabMember::OnDraw( C2DRender* p2DRender ) 
{ 
	int nPage, nRange;
	nPage = MAX_MEMBER_LIST_DRAW;
	nRange = m_nMax;
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	if( m_nMax - m_wndScrollBar.GetScrollPos() + 1 > m_wndScrollBar.GetScrollPage() )
		m_nCurrentList = m_wndScrollBar.GetScrollPos(); 
	if( m_nMax < m_wndScrollBar.GetScrollPos() )
		m_nCurrentList = 0;
	int		i, sx, sy;
	int		nData;
	MEMBERLIST *pMember;
	DWORD	dwColor;
	
	sx = 8;
	sy = 32;	
	
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

#if __VER >= 11 // __CSC_VER11_4
	// 영웅, 마스터 아이콘 추가됬으니 vertex 하나 늘임
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ ((m_nMax > MAX_MEMBER_LIST_DRAW) ? MAX_MEMBER_LIST_DRAW: m_nMax) * 6 * 5 ];
#else //__CSC_VER11_4
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[ ((m_nMax > MAX_MEMBER_LIST_DRAW) ? MAX_MEMBER_LIST_DRAW: m_nMax) * 6 * 4 ];
#endif //__CSC_VER11_4

	TEXTUREVERTEX2* pVertices = pVertex;

	for( i = m_nCurrentList; i < m_nCurrentList + MAX_MEMBER_LIST_DRAW; i ++ )
	{

		if( i >= m_nMax )	break;
		pMember = &(m_aList[i]);
		nData = pMember->nMemberLv;			
		if( i == m_nSelect )
		{
			dwColor = D3DCOLOR_ARGB( 255, 0, 255, 255 );/*adeilson*/
		}
		else
		{
			if( pMember->bIsOnLine )
				dwColor = 0xffffffff;//adeilson 0xff000000
			else
				dwColor = 0xFF909090;
		}

		int j;
		int nClass = pMember->nClass;
		++nClass;

#if __VER >= 11 // __CSC_VER11_4
		int nposx = sx + 14;
		if(nClass == 2)
			nposx -= 5;
		else if(nClass == 3)
			nposx -= 10;
#endif //__CSC_VER11_4

		switch( pMember->nMemberLv )
		{
		case 0:
#if __VER >= 11 // __CSC_VER11_4
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( nposx, sy - 4 ), 44, &pVertices, 0xffffffff );
#else //__CSC_VER11_4
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx - 5, sy - 4 ), 43 + 0 + ( 6 * 1 ), &pVertices, 0xffffffff );
#endif //__CSC_VER11_4
			break;
		case 1:
			{
				for( j = 0 ; j < nClass ; ++j )
				{
#if __VER >= 11 // __CSC_VER11_4
					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( nposx + j * 10, sy - 4 ), 45, &pVertices, 0xffffffff );
#else //__CSC_VER11_4
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx - 5 + j * 17, sy - 4 ), 43 + 1 + ( 6 * 1 ), &pVertices, 0xffffffff );
#endif //__CSC_VER11_4
				}
			}
			break;
		case 2:
			{
				for( j = 0 ; j < nClass ; ++j )
				{
#if __VER >= 11 // __CSC_VER11_4
					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( nposx + j * 10, sy - 4 ), 46, &pVertices, 0xffffffff );
#else //__CSC_VER11_4
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx - 5 + j * 17, sy - 4 ), 43 + 2 + ( 6 * 1 ), &pVertices, 0xffffffff );
#endif //__CSC_VER11_4
				}
			}
			break;
		case 3:
			{
				for( j = 0 ; j < nClass ; ++j )
				{
#if __VER >= 11 // __CSC_VER11_4
					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( nposx + j * 10, sy - 4 ), 47, &pVertices, 0xffffffff );
#else //__CSC_VER11_4
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx - 5 + j * 17, sy - 4 ), 43 + 3 + ( 6 * 1 ), &pVertices, 0xffffffff );
#endif //__CSC_VER11_4
				}
			}
			break;
		case 4:
			{
				for( j = 0 ; j < nClass ; ++j )
				{
#if __VER >= 11 // __CSC_VER11_4
					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( nposx + j * 10, sy - 4 ), 48, &pVertices, 0xffffffff );
#else //__CSC_VER11_4
					pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx - 5 + j * 17, sy - 4 ), 43 + 4 + ( 6 * 1 ), &pVertices, 0xffffffff );
#endif //__CSC_VER11_4
				}
			}
			break;
		default:
			break;
		}

		DWORD Color;

		if( pMember->bIsOnLine )
			Color = 0xffffffff;
		else
			Color = 0xffff6464;
#if __VER >= 11 // __CSC_VER11_4
		// Draw Job Icon
		if( prj.m_aJob[ pMember->nJob ].dwJobType == JTYPE_PRO )
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( sx + 84, sy-3 ),  ( 19 + pMember->nJob - 6 ), &pVertices, Color );
		else if( prj.m_aJob[ pMember->nJob ].dwJobType == JTYPE_MASTER )
		{
			int nMasterIndex = 27;
			if(/*m_nLevel >= 60 && */pMember->nLevel < 70) //Level Down될 경우를 생각해서 주석처리.
				nMasterIndex = 27;
			else if(pMember->nLevel>= 70 && pMember->nLevel < 80)
				nMasterIndex = 28;
			else if(pMember->nLevel >= 80 && pMember->nLevel < 90)
				nMasterIndex = 29;
			else if(pMember->nLevel >= 90 && pMember->nLevel < 100)
				nMasterIndex = 30;
			else if(pMember->nLevel >= 100 && pMember->nLevel < 110)
				nMasterIndex = 31;
			else if(pMember->nLevel>= 110 && pMember->nLevel <= 120)
				nMasterIndex = 32;
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( sx + 64, sy-3 ),  nMasterIndex, &pVertices, Color );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( sx + 84, sy-3 ),  ( 19 + pMember->nJob- 16 ), &pVertices, Color );
		}
		else if( prj.m_aJob[ pMember->nJob ].dwJobType == JTYPE_HERO  )
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( sx + 64, sy-3 ),  33, &pVertices, Color );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( sx + 84, sy-3 ),  ( 19 + pMember->nJob - 24 ), &pVertices, Color );
		}
#ifdef __3RD_LEGEND16
		else if( prj.m_aJob[ pMember->nJob ].dwJobType == JTYPE_LEGEND_HERO  )
		{
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( sx + 64, sy-3 ),  33, &pVertices, Color );
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( sx + 84, sy-3 ),  ( 19 + pMember->nJob - 32 ), &pVertices, Color );
		}
#endif // __3RD_LEGEND16
		else
			pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, CPoint( sx + 84, sy-3 ),  14 + pMember->nJob, &pVertices, Color );
#else //__CSC_VER11_4
		if( prj.m_aJob[ pMember->nJob ].dwJobType == JTYPE_PRO )
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx +  50, sy-3 ), ( 70 + pMember->nJob - 6 ) + ( 8 * pMember->nSex ), &pVertices, Color );
#if __VER >= 10 // __LEGEND
		else if( prj.m_aJob[ pMember->nJob ].dwJobType == JTYPE_MASTER )
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx +  50, sy-3 ), ( 70 + pMember->nJob - 16 ) + ( 8 * pMember->nSex ), &pVertices, Color );
		else if( prj.m_aJob[ pMember->nJob ].dwJobType == JTYPE_HERO )
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx +  50, sy-3 ), ( 70 + pMember->nJob - 24 ) + ( 8 * pMember->nSex ), &pVertices, Color );
#endif //__LEGEND
		else
			pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, CPoint( sx +  50, sy-3 ), 12 + pMember->nJob + ( 6 * pMember->nSex ), &pVertices, Color );
#endif //__CSC_VER11_4

#if __VER >= 10 // __LEGEND
#if __VER < 11 // __CSC_VER11_4
		CString strLevel;
		if(prj.m_aJob[ pMember->nJob ].dwJobType == JTYPE_MASTER)
		{
			strLevel.Format("%d-M", pMember->nLevel);
			p2DRender->TextOut( sx + 90, sy, strLevel, dwColor );
		}
		else if(prj.m_aJob[ pMember->nJob ].dwJobType == JTYPE_HERO)
		{
			strLevel = "HERO";
			p2DRender->TextOut( sx + 90, sy, strLevel, dwColor );
		}
		else
#endif //__CSC_VER11_4
#if __VER >= 12 // __CSC_VER12_2
			p2DRender->TextOut( sx + 126, sy, pMember->nLevel, dwColor );
#else //__CSC_VER12_2
			p2DRender->TextOut( sx + 95, sy, pMember->nLevel, dwColor );
#endif //__CSC_VER12_2
#else //__LEGEND
		p2DRender->TextOut( sx + 95, sy, pMember->nLevel, dwColor );
#endif //__LEGEND
		
		CString strFormat;
		strFormat.Format("%s", pMember->szName);

		if( strFormat.GetLength() > 10 ) 
		{
			int	nReduceCount = 0;

			for( nReduceCount=0; nReduceCount<10; )
			{
				if( IsDBCSLeadByte( strFormat[ nReduceCount ] ) )
					nReduceCount+=2;
				else
					nReduceCount++;
			}

			strFormat = strFormat.Left( nReduceCount );
			strFormat += "...";
		}

#if __VER >= 12 // __CSC_VER12_2
		p2DRender->TextOut( sx + 168, sy, strFormat, dwColor );
		p2DRender->TextOut( sx + 264, sy, pMember->szAlias , dwColor );
#else //__CSC_VER12_2
		p2DRender->TextOut( sx + 135, sy, strFormat, dwColor );
		p2DRender->TextOut( sx + 218, sy, pMember->szAlias , dwColor );
#endif //__CSC_VER12_2

		sy += MEMBER_LIST_HEIGHT;
	}

#if __VER >= 11 // __CSC_VER11_4
	pWndWorld->m_texPlayerDataIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
#else //__CSC_VER11_4
	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
#endif //__CSC_VER11_4
	
	//	delete pVertex;
	SAFE_DELETE_ARRAY( pVertex );
} 
void CWndGuildTabMember::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_wndMenu.CreateMenu( this );	
	
//	CString strMember;
	CRect rect = GetWindowRect();
	rect.top += 40;
	rect.bottom	-= 10 ;
	rect.left -= 30;
	rect.right -= 5;
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );//,m_pSprPack,-1);

	int nPage, nRange;
	nPage = MAX_MEMBER_LIST_DRAW;
	nRange = m_nMax;//m_pItemContainer->m_dwIndexNum;// - nPage;
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );

#if __VER >= 12 // __CSC_VER12_2
	// 버튼 이미지 세팅
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_MLEVEL);
	if(pWndButton)
	{
//		if(::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_TWN )
//		else
#ifdef __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#else // __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#endif // __NO_SUB_LANG
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal05.tga" ), 0xffff00ff );
		else
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), "ButtNormal05.tga" ), 0xffff00ff );
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_JOB);
	if(pWndButton)
	{
//		if(::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_TWN )
//		else
#ifdef __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#else // __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#endif // __NO_SUB_LANG
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal05.tga" ), 0xffff00ff );
		else
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), "ButtNormal05.tga" ), 0xffff00ff );
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_LEVEL);
	if(pWndButton)
	{
//		if(::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_TWN )
//		else
#ifdef __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#else // __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#endif // __NO_SUB_LANG
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal05.tga" ), 0xffff00ff );
		else
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), "ButtNormal05.tga" ), 0xffff00ff );
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_NAME);
	if(pWndButton)
	{
//		if(::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_TWN )
//		else
#ifdef __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#else // __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#endif // __NO_SUB_LANG
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal00.tga" ), 0xffff00ff );
		else
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), "ButtNormal00.tga" ), 0xffff00ff );
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_ALIAS);
	if(pWndButton)
	{
//		if(::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_TWN )
//		else
#ifdef __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#else // __NO_SUB_LANG
		if(::GetLanguage() == LANG_USA || ::GetLanguage() == LANG_VTN)
#endif // __NO_SUB_LANG
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtNormal00.tga" ), 0xffff00ff );
		else
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", ::GetLanguage(), "ButtNormal00.tga" ), 0xffff00ff );
	}
#endif //__CSC_VER12_2

	UpdateData();

#if __VER >= 12 // __CSC_VER12_2
	SortbyMemberLevel();	// Default Sort
#endif //__CSC_VER12_2

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndGuildTabMember::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
#if __VER >= 12 // __CSC_VER12_2
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_TABMEMBER_EX, 0, CPoint( 0, 0 ), pWndParent );
#else //__CSC_VER12_2
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_TABMEMBER, 0, CPoint( 0, 0 ), pWndParent );
#endif //__CSC_VER12_2
} 
BOOL CWndGuildTabMember::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	if( nID == 1000 && dwMessage == 514 )	// Scroll Bar
	{
		int adf = m_wndScrollBar.GetScrollPos();
		int bbf = m_wndScrollBar.GetScrollRange();
		return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
	}
	int a = 0;

	CMover* pMover	= (CMover*)CMover::GetActiveObj();
	if( !pMover )
		return FALSE;
	CGuild* pGuild	= pMover->GetGuild();
	if( !pGuild )
		return FALSE;
	if( m_nSelect < 0 || m_nSelect >= m_nMax )
		return FALSE;

	if( g_pPlayer->m_idWar )
	{
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDWARERRORMEMBER ) );
		return FALSE;
	}

	if( pWndBase == &m_wndMenu )
	{
		switch( nID )
		{
		case MGI_APPELL_UP:		// 호칭 업.
			g_DPlay.SendGuildMemberLv( pMover->m_idPlayer,
				m_aList[m_nSelect].idPlayer, m_aList[m_nSelect].nMemberLv - 1 );
			break;
		case MGI_APPELL_DOWN:
			g_DPlay.SendGuildMemberLv( pMover->m_idPlayer,
				m_aList[m_nSelect].idPlayer, m_aList[m_nSelect].nMemberLv + 1 );
			break;

		case MGI_CLASS_UP:		// 등급 업
			g_DPlay.SendGuildClass( pMover->m_idPlayer,
				m_aList[m_nSelect].idPlayer, 1 );
			break;
		case MGI_CLASS_DOWN:	
			g_DPlay.SendGuildClass( pMover->m_idPlayer,
				m_aList[m_nSelect].idPlayer, 0 );
			break;
		case MGI_CHG_MASTER:
			{
				if( m_aList[m_nSelect].idPlayer != g_pPlayer->m_idPlayer )
				{
					if( m_aList[m_nSelect].bIsOnLine )
						g_DPlay.SendChgMaster( pMover->m_idPlayer, m_aList[m_nSelect].idPlayer );
					else
						g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_NOTLOGIN ), MB_OK, this );
				}
			}
			break;
		case MGI_NICKNAME:		// 별칭 부여
			{
				CGuild* pGuild = g_pPlayer->GetGuild();
				if( pGuild && pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
				{
					if( 10 > pGuild->m_nLevel )
					{
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_GUILDNOTLEVEL) ) );	
					}
					else
					{
						SAFE_DELETE( g_WndMng.m_pWndGuildNickName );
						g_WndMng.m_pWndGuildNickName = new CWndGuildNickName;
						g_WndMng.m_pWndGuildNickName->m_idPlayer = m_aList[m_nSelect].idPlayer;
						g_WndMng.m_pWndGuildNickName->Initialize( &g_WndMng, APP_GUILD_NICKNAME );
					}
				}
				else
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDONLYMASTER ), MB_OK, this );
				}
				
			}
			break;

		case MGI_GUILD_LEAVE:	// 탈퇴
			{
				if(  pGuild->GetQuest( QUEST_WARMON_LV1 ) != NULL && pGuild->GetQuest( QUEST_WARMON_LV1 )->nState == QS_BEGIN )	// 클락워크 퀘스트면 길드 해체 안됨
				{
					QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( QUEST_WARMON_LV1 );
					if( pQuestProp )
					{
						g_WndMng.OpenMessageBox( prj.GetText( TID_GUILD_QUEST_LEAVEERROR ), MB_OK, this );
					}
					return FALSE;
				}
				else
				{
					if( g_GuildCombatMng.m_bRequest && g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE && g_GuildCombatMng.m_nGCState != CGuildCombat::WAR_CLOSE_STATE )
					//if( g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE && g_GuildCombatMng.m_nGCState != CGuildCombat::WAR_CLOSE_STATE )
						g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDCOMBAT_NOT_LEAVE_GUILD ) );		//"수정해야함 : 길드대전에 신청한길드는 대전중에는 길드 탈퇴를 할수 없습니다" );
#if __VER >= 11 // __GUILD_COMBAT_1TO1
					else if( g_GuildCombat1to1Mng.m_nState != CGuildCombat1to1Mng::GC1TO1_CLOSE )
						g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDCOMBAT1TO1_NOTLEAVEGUILD ) );
#endif //__GUILD_COMBAT_1TO1
					else
					{
						g_DPlay.SendRemoveGuildMember( pMover->m_idPlayer, m_aList[m_nSelect].idPlayer );
					}
				}
			}
			break;
		} // switch nID
	}
		
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildTabMember::OnSize( UINT nType, int cx, int cy ) \
{ 
	CRect rect = GetWindowRect();
	rect.top = rect.top + 40;
	rect.bottom	= rect.bottom - 10 ;
	rect.left = rect.right - 40;
	rect.right = rect.right - 25;
	m_wndScrollBar.SetWndRect( rect );
	
	int nPage, nRange;
	nPage = MAX_MEMBER_LIST_DRAW;
	nRange = m_nMax;//m_pItemContainer->m_dwIndexNum;// - nPage;
	m_wndScrollBar.SetScrollRange( 0, nRange );
	m_wndScrollBar.SetScrollPage( nPage );
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildTabMember::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildTabMember::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	int mx = point.x - 8;
	int my = point.y - 32;
	m_nSelect = m_nCurrentList + my / MEMBER_LIST_HEIGHT;
	if( m_nSelect < 0 )
		m_nSelect = 0;
	if( m_nSelect >= m_nMax )
		m_nSelect = m_nMax - 1;
} 

void CWndGuildTabMember::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
	OnLButtonDown( nFlags, point );
} 

void CWndGuildTabMember::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
	m_wndMenu.DeleteAllMenu();
	m_wndMenu.AppendMenu( 0, MGI_APPELL_UP,		prj.GetText( TID_GAME_GUILD_APPELLATIONUP ) );
	m_wndMenu.AppendMenu( 0, MGI_APPELL_DOWN,	prj.GetText( TID_GAME_GUILD_APPELLATIONDOWN ) );
	m_wndMenu.AppendMenu( 0, MGI_CLASS_UP,		prj.GetText( TID_GAME_GUILD_CLASSUP ) );
	m_wndMenu.AppendMenu( 0, MGI_CLASS_DOWN,	prj.GetText( TID_GAME_GUILD_CLASSDOWN ) );
	m_wndMenu.AppendMenu( 0, MGI_NICKNAME,		prj.GetText( TID_GAME_GUILD_NICKNAME ) );
	m_wndMenu.AppendMenu( 0, MGI_GUILD_LEAVE,	prj.GetText( TID_GAME_GUILD_LEAVE ) );
	CMover* pMover	= CMover::GetActiveMover();
	if( pMover )
	{
		CGuild* pGuild	= pMover->GetGuild();
		if( pGuild && pGuild->IsMaster( pMover->m_idPlayer ) )
			m_wndMenu.AppendMenu( 0, MGI_CHG_MASTER,	prj.GetText( TID_GAME_CHG_MASTER ) );
	}

	m_wndMenu.Move( CPoint( m_rectCurrentWindow.left, m_rectCurrentWindow.top ) + point );
	m_wndMenu.SetVisible( TRUE );//!m_wndMenuMover.IsVisible() );
	m_wndMenu.SetFocus();
	
} 

BOOL CWndGuildTabMember::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	m_wndMenu.SetVisible( FALSE );

#if __VER >= 12 // __CSC_VER12_2
	switch(nID)
	{
		case WIDC_MLEVEL:
			SortbyMemberLevel();
			break;
		case WIDC_JOB:
			SortbyJob();
			break;
		case WIDC_LEVEL:
			SortbyLevel();
			break;
		case WIDC_NAME:
			SortbyName();
			break;
		case WIDC_ALIAS:
			SortbyAlias();
			break;
	}
#endif //__CSC_VER12_2
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildTabMember::OnMouseMove(UINT nFlags, CPoint point )
{
	if( nFlags & MK_LBUTTON )
	{
		int		nDistY = (m_nMyOld - point.y) / 5;		// 과거 좌표와의 차이.

		m_nCurrentList += nDistY;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
		if( (m_nCurrentList + MAX_MEMBER_LIST_DRAW - 1) >= m_nMax )
		{
			m_nCurrentList = m_nMax - MAX_MEMBER_LIST_DRAW;
			if( m_nCurrentList < 0 )
				m_nCurrentList = 0;
		}
	
	}

	m_nMxOld = point.x;
	m_nMyOld = point.y;
}
void CWndGuildTabMember::OnMouseWndSurface( CPoint point )
{
	int mx = point.x - 8;
	int my = point.y - 32;
	int nSelect = m_nCurrentList + my / MEMBER_LIST_HEIGHT;

	if( nSelect < 0 )
		nSelect = 0;
	if( nSelect >= m_nMax )
		nSelect = m_nMax - 1;
	
	int nMax = (m_nMax > MAX_MEMBER_LIST_DRAW) ? MAX_MEMBER_LIST_DRAW : m_nMax;

	int offset_y = 30;

	for( int i=0; i<nMax; i++ )
	{
		CRect rect = CRect( 4,offset_y,m_rectLayout.right - m_rectLayout.left, (offset_y+15) );
		offset_y+=MEMBER_LIST_HEIGHT;

		if( PtInRect( &rect, point ) )
		{
			CRect rect2 = CRect( point.x, point.y, point.x+200, point.y+20 );
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rect2 );

			CString str;

			str.Format( "%s\n%s %u\n%s %d\n%s %d", m_aList[ nSelect ].szName,
											   prj.GetText(TID_GAME_TOOLTIP_EXPMERIT ), m_aList[ nSelect ].dwGivePxpCount,
				                               prj.GetText(TID_GAME_TOOLTIP_PENYAMERIT ), m_aList[ nSelect ].nGiveGold,
											   prj.GetText(TID_GAME_TOOLTIP_GIVEUPNUM ), m_aList[ nSelect ].nLose );

			g_toolTip.PutToolTip( m_nIdWnd, str, rect2, point2 );
			break;
		}
	}
}	

BOOL CWndGuildTabMember::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	int		nZoom = 64;
	
//    if( nFlags & MK_SHIFT )
//		nZoom /= 8;
	if( zDelta > 0 )
		m_nCurrentList -= 3;
	else
		m_nCurrentList += 3;

	if( m_nCurrentList < 0 )
		m_nCurrentList = 0;
	if( (m_nCurrentList + MAX_MEMBER_LIST_DRAW - 1) >= m_nMax )
	{
		m_nCurrentList = m_nMax - MAX_MEMBER_LIST_DRAW;
		if( m_nCurrentList < 0 )
			m_nCurrentList = 0;
	}

	m_wndScrollBar.SetScrollPos( m_nCurrentList );
	return TRUE;
}


void CWndGuildTabMember::UpdateData()
{
	CGuild* pGuild = g_pPlayer->GetGuild();
	if( pGuild )
	{
		m_nMax = 0;
		CGuildMember* pMember;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;

			m_aList[ m_nMax ].idPlayer       = pMember->m_idPlayer;
			m_aList[ m_nMax ].nMemberLv      = pMember->m_nMemberLv;			// 호칭
#if __VER >= 11 // __SYS_PLAYER_DATA
			PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
			m_aList[ m_nMax ].nJob	= pPlayerData->data.nJob;
			m_aList[ m_nMax ].nSex	= pPlayerData->data.nSex;
			m_aList[ m_nMax ].nLevel	= pPlayerData->data.nLevel;	// 레벨
			m_aList[ m_nMax ].bIsOnLine      = ( pPlayerData->data.uLogin > 0 );
#else	// __SYS_PLAYER_DATA
			m_aList[ m_nMax ].nJob           = pMember->m_nJob;	// 직업
			m_aList[ m_nMax ].nSex           = pMember->m_dwSex;	// 성
			m_aList[ m_nMax ].nLevel = pMember->m_nLevel;			// 레벨
			m_aList[ m_nMax ].bIsOnLine      = pMember->m_nLogin;
#endif	// __SYS_PLAYER_DATA

			m_aList[ m_nMax ].nGiveGold      = pMember->m_nGiveGold;					// 길드에 기부한 페냐
			m_aList[ m_nMax ].dwGivePxpCount = pMember->m_dwGivePxpCount;				// 길드에 기부한 PXP횟수( 스킬 경험치 )
			m_aList[ m_nMax ].nWin           = pMember->m_nWin;							// 무엇을 이겼지?
			m_aList[ m_nMax ].nLose          = pMember->m_nLose;						// 무엇을 졌을까나?
		
#if __VER >= 11 // __SYS_PLAYER_DATA
			LPCSTR pszPlayer	= pPlayerData->szPlayer;
			lstrcpy( m_aList[m_nMax].szName, pszPlayer );
#else	//__SYS_PLAYER_DATA
			LPCSTR pszPlayer	= prj.GetPlayerString( pMember->m_idPlayer );
			if( pszPlayer )
				lstrcpy( m_aList[m_nMax].szName, pszPlayer );
			else
				g_DPlay.SendQueryPlayerString( pMember->m_idPlayer, QPS_GUILD_MEMBER );
#endif	// __SYS_PLAYER_DATA
			strcpy( m_aList[ m_nMax ].szAlias, pMember->m_szAlias );
			m_aList[ m_nMax ].nClass		= pMember->m_nClass;

			m_nMax ++;
		}
	}
	else
	{
		m_nMax = 0;
		m_nCurrentList = 0;
		m_nSelect = 0;
		memset( m_aList, 0, sizeof(m_aList) );
		m_nMxOld = m_nMyOld = 0;
	}		
}

#if __VER >= 12 // __CSC_VER12_2

void CWndGuildTabMember::SortbyMemberLevel()
{
	if(m_nMax > 1)
	{
		if(m_bSortbyMemberLevel)
		{
			std::sort( m_aList, m_aList + m_nMax, prMemberLevelAsce );
			m_bSortbyMemberLevel = FALSE;
		}
		else
		{
			std::sort( m_aList, m_aList + m_nMax, prMemberLevelDesc );
			m_bSortbyMemberLevel = TRUE;
		}
	}
}

void CWndGuildTabMember::SortbyJob()
{
	if(m_nMax > 1)
	{
		if(m_bSortbyJob)
		{
			std::sort( m_aList, m_aList + m_nMax, prJobAsce );
			m_bSortbyJob = FALSE;
		}
		else
		{
			std::sort( m_aList, m_aList + m_nMax, prJobDesc );
			m_bSortbyJob = TRUE;
		}
	}
}

void CWndGuildTabMember::SortbyLevel()
{
	if(m_nMax > 1)
	{
		if(m_bSortbyLevel)
		{
			std::sort( m_aList, m_aList + m_nMax, prLevelAsce );
			m_bSortbyLevel = FALSE;
		}
		else
		{
			std::sort( m_aList, m_aList + m_nMax, prLevelDesc );
			m_bSortbyLevel = TRUE;
		}
	}
}

void CWndGuildTabMember::SortbyName()
{
	if(m_nMax > 1)
	{
		if(m_bSortbyName)
		{
			std::sort( m_aList, m_aList + m_nMax, prNameAsce );
			m_bSortbyName = FALSE;
		}
		else
		{
			std::sort( m_aList, m_aList + m_nMax, prNameDesc );
			m_bSortbyName = TRUE;
		}
	}
}

void CWndGuildTabMember::SortbyAlias()
{
	if(m_nMax > 1)
	{
		if(m_bSortbyAlias)
		{
			std::sort( m_aList, m_aList + m_nMax, prAliasAsce );
			m_bSortbyAlias = FALSE;
		}
		else
		{
			std::sort( m_aList, m_aList + m_nMax, prAliasDesc );
			m_bSortbyAlias = TRUE;
		}
	}
}

#endif //__CSC_VER12_2
