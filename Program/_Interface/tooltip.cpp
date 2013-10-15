// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h" 


#include "DialogMsg.h"
extern CDialogMsg g_DialogMsg;

#if __VER >= 9 // __CSC_VER9_1
#include "defineText.h"
#endif //__CSC_VER9_1

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
CToolTip::CToolTip() : 
m_rectRender( 0, 0, 0, 0 ), 
m_strToolTip( _T( "" ) ), 
m_nSubToolTipFlag( CWndMgr::TOOL_TIP_SWITCH_MAIN ), 
m_nRevisedRect( 0, 0, 0, 0 ), 
m_nSubToolTipNumber( CWndMgr::TOOL_TIP_SWITCH_MAIN )
#else // __IMPROVE_SYSTEM_VER15
CToolTip::CToolTip()
#endif // __IMPROVE_SYSTEM_VER15
{
	m_bReadyToopTip = FALSE;
	m_dwToolTipId   = 0;
	m_ptToolTip     = 0;
	m_ptOldToolTip  = 0;
	m_bPutToolTip = FALSE;
	m_bEnable = TRUE;
	m_nPosition = 0;
	m_bToolTip = FALSE;
	m_nAlpha = 255;
#if __VER >= 9 // __CSC_VER9_1
	m_nAdded = 0;
	m_nSlot = 0;
#if __VER >= 12 // __EXT_PIERCING
	m_pUltimateTexture = NULL;
	m_pJewelBgTexture = NULL;
#ifdef __NEW_ITEM_VARUNA
	m_pBarunaTexture = NULL;
#endif // __NEW_ITEM_VARUNA
#endif //__EXT_PIERCING
#endif //__CSC_VER9_1
#ifndef __IMPROVE_MAP_SYSTEM
#if __VER >= 13 // __CSC_VER13_1
	m_nMonInfoCnt = 0;
	for(int i=0; i<5; i++)
		m_pDwMonId[i] = 0;
#endif //__CSC_VER13_1
#endif // __IMPROVE_MAP_SYSTEM
#ifdef __IMPROVE_MAP_SYSTEM
	m_vecMapMonsterID.clear();
#endif // __IMPROVE_MAP_SYSTEM
}
CToolTip::~CToolTip()
{
	Delete();
}
void CToolTip::Delete()
{
	int nloadTexture = 0;
	for( int i = 0 ; i < MAX_TT ; ++i )
	{
		for( int j = 0 ; j < 9 ; ++ j )
		{
			m_apTextureToolTip[j].DeleteDeviceObjects();
			++nloadTexture;
		}
	}
}

void CToolTip::InitTexture()
{
	CString szTextName;
	CString szTextNamebuf;
	szTextNamebuf = "WndTooltipTile";

	char szBuf[32];
	int nloadTexture = 0;
	for( int i = 0 ; i < MAX_TT ; ++i )
	{
		for( int j = 0 ; j < 9 ; ++ j )
		{
			szTextName = szTextNamebuf;
			sprintf( szBuf, "%02d", ( i * 10 ) + j );
			szTextName += szBuf;
			szTextName += ".tga";
			m_apTextureToolTip[nloadTexture].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, szTextName ), 0xffff00ff, TRUE );
			++nloadTexture;
		}
	}

#if __VER >= 12 // __EXT_PIERCING
	//Ultimate Icon & Bg Load
	CString strPath;
	
	if(::GetLanguage() == LANG_FRE)
		strPath = MakePath( "Theme\\", ::GetLanguage(), "Icon_Ultimate.dds");
	else
		strPath = MakePath( DIR_ICON, "Icon_Ultimate.dds");

	m_pUltimateTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
#ifdef __NEW_ITEM_VARUNA
	strPath = MakePath( DIR_ICON, "icon_Baruna01.dds" );
	m_pBarunaTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
#endif // __NEW_ITEM_VARUNA
	m_pJewelBgTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "back_Slotitem.tga"), 0xffff00ff );
#endif //__EXT_PIERCING
}
/////////////////////////////////////////////////////////////////////////////
// Parameters :
// Return     :
// Remarks    :
//   현재 열려진 툴팁을 켄슬한다.(사라지게 한다.)
//
void CToolTip::CancelToolTip()
{
	if(m_bEnable == FALSE)
		return;
	m_bReadyToopTip = FALSE;
	//m_dwToolTipId = 0;
	m_bToolTip = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Func. Name : 
// Parameters :
// Return     :
// Remarks    :
//   툴팁을 입력한다. 0.5초가 지나면 알아서 출력한다.
//

void CToolTip::PutToolTip( DWORD dwToolTipId, CString& string, CRect rect, CPoint pt, int nToolTipPos )
{
	PutToolTip( dwToolTipId, (LPCTSTR)string, rect, pt, nToolTipPos );
}

void CToolTip::PutToolTip( DWORD dwToolTipId, LPCTSTR lpszString, CRect rect, CPoint pt, int nToolTipPos )
{
	if(m_bEnable == FALSE)
		return;
	if(!rect.PtInRect(pt))
		return;
//if(m_bReadyToopTip == FALSE && dwToolTipId != m_dwToolTipId)
	if( dwToolTipId != m_dwToolTipId || m_rect != rect )
	{
		m_nAlpha = 0;
		m_rect = rect;
		m_bReadyToopTip = TRUE;
		m_dwToolTipId = dwToolTipId;
		//m_timerToopTip.Set(100);
		m_timerToopTip.Set(0);
		if(m_bToolTip)
			m_bToolTip = FALSE;
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
		m_nSubToolTipNumber = CWndMgr::TOOL_TIP_SWITCH_MAIN;
#endif // __IMPROVE_SYSTEM_VER15
	}
	m_strToolTip = lpszString;
	m_strToolTip.Init( CWndBase::m_Theme.m_pFontText, &CRect( 0, 0, 200, 0 ) );
	CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent_EditString( m_strToolTip );
	m_rectRender 
		= CRect( 
			0, 
			0, 
			size.cx + 6, 
			size.cy + 3 + ( 2 * ( size.cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight() ) ) 
		);
	m_rect = rect;
	m_nPosition = nToolTipPos;
	m_bPutToolTip = TRUE;
#if __VER >= 9 // __CSC_VER9_1
	m_nAdded = 0;
	m_nSlot = 0;
#endif //__CSC_VER9_1
#ifndef __IMPROVE_MAP_SYSTEM
#if __VER >= 13 // __CSC_VER13_1
	m_nMonInfoCnt = 0;
	for(int i=0; i<5; i++)
		m_pDwMonId[i] = 0;
#endif //__CSC_VER13_1
#endif // __IMPROVE_MAP_SYSTEM
#ifdef __IMPROVE_MAP_SYSTEM
	m_vecMapMonsterID.clear();
#endif // __IMPROVE_MAP_SYSTEM
}

void CToolTip::PutToolTip( DWORD dwToolTipId, CEditString& string, CRect rect, CPoint pt, int nToolTipPos )
{
	if(m_bEnable == FALSE)
		return;
	if(!rect.PtInRect(pt))
		return;
//if(m_bReadyToopTip == FALSE && dwToolTipId != m_dwToolTipId)
	if( dwToolTipId != m_dwToolTipId)
	{
		m_nAlpha = 0;
		m_rect = rect;
		m_bReadyToopTip = TRUE;
		m_dwToolTipId = dwToolTipId;
		//m_timerToopTip.Set(100);
		m_timerToopTip.Set(0);
		if(m_bToolTip)
			m_bToolTip = FALSE;
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
		m_nSubToolTipNumber = CWndMgr::TOOL_TIP_SWITCH_MAIN;
#endif // __IMPROVE_SYSTEM_VER15
	}
	m_strToolTip = string;
	m_strToolTip.Init( CWndBase::m_Theme.m_pFontText, &CRect( 0, 0, 200, 0 ) );
	CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent_EditString( m_strToolTip );
	m_rectRender 
		= CRect( 
			0, 
			0, 
			size.cx + 6, 
			size.cy + 3 + ( 2 * ( size.cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight() ) ) 
		);
	m_rect = rect;
	m_nPosition = nToolTipPos;
	m_bPutToolTip = TRUE;
#if __VER >= 9 // __CSC_VER9_1
	m_nAdded = 0;
	m_nSlot = 0;
#endif //__CSC_VER9_1
#ifndef __IMPROVE_MAP_SYSTEM
#if __VER >= 13 // __CSC_VER13_1
	m_nMonInfoCnt = 0;
	for(int i=0; i<5; i++)
		m_pDwMonId[i] = 0;
#endif //__CSC_VER13_1
#endif // __IMPROVE_MAP_SYSTEM
#ifdef __IMPROVE_MAP_SYSTEM
	m_vecMapMonsterID.clear();
#endif // __IMPROVE_MAP_SYSTEM
}

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
void CToolTip::PutToolTipEx( DWORD dwToolTipId, CEditString& string, CRect rect, CPoint pt, int nToolTipPos, int nSubToolTipFlag )
{
	if(m_bEnable == FALSE)
		return;
	if(!rect.PtInRect(pt))
		return;

	if( m_rect == rect )
	{
		if( nSubToolTipFlag == 0 )
		{
			if( m_bPutToolTip == FALSE )
			{
				m_nAlpha = 0;
				m_rect = rect;
				m_bReadyToopTip = TRUE;
				m_dwToolTipId = dwToolTipId;
				m_timerToopTip.Set(0);
				if(m_bToolTip)
					m_bToolTip = FALSE;
			}
		}
		else if( nSubToolTipFlag == 1 || nSubToolTipFlag == 2 )
		{
			if( g_toolTip.GetReadyToolTipSwitch() == TRUE )
				m_bReadyToopTip = TRUE;
		}
	}
	else
	{
		m_nAlpha = 0;
		m_rect = rect;
		m_bReadyToopTip = TRUE;
		m_dwToolTipId = dwToolTipId;
		m_timerToopTip.Set(0);
		if(m_bToolTip)
			m_bToolTip = FALSE;
	}
	m_strToolTip = string;
	m_strToolTip.Init( CWndBase::m_Theme.m_pFontText, &CRect( 0, 0, 200, 0 ) );
	CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent_EditString( m_strToolTip );
	m_rectRender = CRect( 0, 0, size.cx + 6, size.cy + 3 + ( 2 * ( size.cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight() ) ) );
	m_rect = rect;
	m_nPosition = nToolTipPos;
	m_bPutToolTip = TRUE;
	m_nAdded = 0;
	m_nSlot = 0;
	m_nSubToolTipFlag = nSubToolTipFlag;
#ifndef __IMPROVE_MAP_SYSTEM
	m_nMonInfoCnt = 0;
	for(int i=0; i<5; i++)
		m_pDwMonId[i] = 0;
#endif // __IMPROVE_MAP_SYSTEM
#ifdef __IMPROVE_MAP_SYSTEM
	m_vecMapMonsterID.clear();
#endif // __IMPROVE_MAP_SYSTEM
}
#endif // __IMPROVE_SYSTEM_VER15

/////////////////////////////////////////////////////////////////////////////
// Func. Name : 
// Parameters :
// Return     :
// Remarks    :
//   툴팁처리를 한다. 시간이 지나면 입력된 것을 출력한다.
//
void CToolTip::Process(CPoint pt,C2DRender* p2DRender)
{
	CD3DFont* pFont = p2DRender->m_pFont;
	if(m_bEnable == FALSE)
		return;
	// Put을 호출하지 못했다. 그건 툴립 출력할일이 없다는 뜻.
	if(m_bPutToolTip == FALSE)
	{
		m_dwToolTipId = -1;
		CancelToolTip();
		return;
	}
	if(m_bPutToolTip == TRUE && m_bReadyToopTip == TRUE && m_timerToopTip.Over())
	{
		if(m_bToolTip == NULL)
			m_ptToolTip = pt;
		else
			m_bToolTip = FALSE;

		if(0) //m_ptToolTip != pt)
		{
			m_bReadyToopTip = FALSE;
		}
		else
		{
			m_bToolTip = TRUE;
			/*
			CD3DFont* pFont = p2DRender->m_pFont;
			LPCTSTR string = m_strToolTip;
			DWORD dwHeight = pFont->GetMaxHeight();//string);//lpszString,nCount GetStringPixel((CHanString)str);memDC.GetStringPixel((CHanString)string);
			int nStrMax, nLength = strlen(string);
			CSize sizeString;
			if(nLength > 20)
				nStrMax = 20;
			else
				nStrMax = nLength;
			CStringArray strArray;
			CString str;
			char chr = ' ';
			int c = 0;
			CSize sizeLine;
			int nMaxLen = 0;
			do
			{
				chr = string[c];
				str = "";
				do 
				{
					chr = string[c++];
					if(chr != '\r' && chr != '\0')
						str += chr;
				} 
				while((c < nStrMax || chr != ' ') && chr != '\0' && chr != '\r');

				nStrMax = c + 20;
				strArray.Add(str);
				sizeLine = pFont->GetTextExtent( str );
				if(nMaxLen < sizeLine.cx)
					nMaxLen = sizeLine.cx;
			} 
			while(chr != '\0');
			int nWidth = nMaxLen + 6;
			int nHeight = sizeString.cy * strArray.GetSize() + 6;

			//m_bToolTip = new CDibBitmap;
			//m_bToolTip->CreateDIBSection(&memDC,NULL,nWidth,nHeight,16);
			//nWidth = m_bToolTip->GetWidth();
			//memDC.SelectBitmap_(m_bToolTip);
			//memDC.Clear(0xffff);
			//memDC.SetTextColor(MKHIGHRGB(0x1a,0x1a,0x1a));
			for(c = 0; c < strArray.GetSize(); c++)
			{
				//if(strArray[c][0] != '\r')
					p2DRender->TextOut( 3, 3 + c * sizeString.cy, strArray[c] );
			}
			//memDC.SelectObject(pOldFont);
			*/
		}
	}
	if(m_rect.PtInRect(pt) == FALSE)
		m_bPutToolTip = FALSE;
	/*
	else
	{
	//	m_rectRender.InflateRect( 1, 1 );

		if( m_rectRender.left < m_rectRender.left )
			m_rectRender.left = m_rectRender.left;
		if( m_rectRender.right > m_rectRender.right )
			m_rectRender.right = m_rectRender.right;
		if( m_rectRender.top < m_rectRender.top )
			m_rectRender.top = m_rectRender.top;
		if( m_rectRender.bottom > m_rectRender.bottom )
			m_rectRender.bottom = m_rectRender.bottom;
	}
	*/
	m_nAlpha += 15;
	if( m_nAlpha > 255 ) m_nAlpha = 255;
}
void CToolTip::Paint(C2DRender* p2DRender)
{
	if(m_bEnable == FALSE)
		return;
	if( m_bToolTip )
	{
		CPoint pt = m_ptToolTip;
		int nPostion = m_nPosition;
		if(nPostion == 0) // top
		{
			pt = m_rect.TopLeft();
			pt.y -= m_rectRender.Height() + 10;
		}
		else
		if(nPostion == 1) // bottom
		{
//			pt = CPoint(m_rect.left, m_rect.bottom);
			pt = CPoint(m_rect.left, m_rect.bottom + 20 );		// 2006/7/5 -xuzhu-
			//rect.SetRect(pt.x,pt.y,pt.x+m_bToolTip->GetWidth(),pt.y+m_bToolTip->GetHeight());
		}
		else
		if(nPostion == 2) // left
		{
			pt = m_rect.TopLeft();
			pt.x -= m_rectRender.Width() + 2;
		}
		else
		if(nPostion == 3) // right
		{
			pt = CPoint(m_rect.right,m_rect.top);
			//pt.x += m_bToolTip->GetHeight() + 2;
		}
		//CRect rect(pt.x,pt.y,pt.x+m_bToolTip->GetWidth(),pt.y+m_bToolTip->GetHeight());
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
		switch( m_nSubToolTipFlag )
		{
		case CWndMgr::TOOL_TIP_SWITCH_SUB1:
			{
				const CRect rectMain = g_toolTip.GetRevisedRect();
				const CRect rectRender = g_toolTip.GetRenderRect();
				pt.x = rectMain.left + rectRender.Width() + 16;
				break;
			}
		case CWndMgr::TOOL_TIP_SWITCH_SUB2:
			{
				const CRect rectSub1 = g_toolTipSub1.GetRevisedRect();
				const CRect rectRender = g_toolTipSub1.GetRenderRect();
				pt.x = rectSub1.left + rectRender.Width() + 16;
				break;
			}
		}
		static const int TOOL_TIP_WIDTH_FOR_SLOTS = 194; // 아이템 보석 슬롯 5개가 렌더링되기 위한 툴팁 길이
		if( m_nSlot > 0 && m_rectRender.right < TOOL_TIP_WIDTH_FOR_SLOTS )
			m_rectRender.right = TOOL_TIP_WIDTH_FOR_SLOTS;
		CRect rect( pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height() );
#else // __IMPROVE_SYSTEM_VER15
		CRect rect( pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height() );
#endif // __IMPROVE_SYSTEM_VER15

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
		switch( m_nSubToolTipFlag )
		{
		case CWndMgr::TOOL_TIP_SWITCH_MAIN:
			{
				static const int TOOL_TIP_MARGIN = 16;
				switch( m_nSubToolTipNumber )
				{
				case CWndMgr::TOOL_TIP_SWITCH_MAIN:
					{
						if( rect.right + TOOL_TIP_MARGIN > p2DRender->m_clipRect.right )
							pt.x = p2DRender->m_clipRect.Width() - rect.Width() - 8;
						break;
					}
				case CWndMgr::TOOL_TIP_SWITCH_SUB1:
					{
						const CRect rectToolTipSub1 = g_toolTipSub1.GetRevisedRect();
						if( rect.right + rectToolTipSub1.Width() + ( TOOL_TIP_MARGIN * 2 ) > p2DRender->m_clipRect.right )
							pt.x = p2DRender->m_clipRect.Width() - rect.Width() - 224;
						break;
					}
				case CWndMgr::TOOL_TIP_SWITCH_SUB2:
					{
						const CRect rectToolTipSub1 = g_toolTipSub1.GetRevisedRect();
						const CRect rectToolTipSub2 = g_toolTipSub2.GetRevisedRect();
						if( rect.right + rectToolTipSub1.Width() + rectToolTipSub2.Width() + ( TOOL_TIP_MARGIN * 3 ) > p2DRender->m_clipRect.right )
							pt.x = p2DRender->m_clipRect.Width() - rect.Width() - 470;
						break;
					}
				}
				if( rect.top < p2DRender->m_clipRect.top )
					pt.y = p2DRender->m_clipRect.top + 8;
				m_nRevisedRect = CRect( pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height() );
				break;
			}
		case CWndMgr::TOOL_TIP_SWITCH_SUB1:
		case CWndMgr::TOOL_TIP_SWITCH_SUB2:
			{
				if( rect.top < p2DRender->m_clipRect.top )
					pt.y = p2DRender->m_clipRect.top + 8;
				m_nRevisedRect = CRect( pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height() );
				break;
			}
			}
#else // __IMPROVE_SYSTEM_VER15
		if( rect.right > p2DRender->m_clipRect.right )
		{
//			pt.x -= m_rectRender.Width() - m_rect.Width();			// 버그인거 같애서 아래처럼 수정함 2006/6/30 -xuzhu-
			pt.x = p2DRender->m_clipRect.Width() - rect.Width() - 8;
		}
		if( rect.top < p2DRender->m_clipRect.top )
		{
//			pt.y += m_rectRender.Height() + m_rect.Height();
//			pt.y = p2DRender->m_clipRect.Height() - rect.Height() - 8;
			pt.y = p2DRender->m_clipRect.top + 8;
		}
#endif // __IMPROVE_SYSTEM_VER15

#if __VER >= 9 // __CSC_VER9_1
		if(m_nSlot > 0)
		{
//			CItemElem* pItemElem = (CItemElem*)m_pUltimateItemBase;
//			if( pItemElem->GetAbilityOption() < 10 )
//				rect.SetRect( pt.x, pt.y, pt.x + m_rectRender.Width() + 16, pt.y + m_rectRender.Height() + (m_nSlot * 26) );
//			else
//				rect.SetRect( pt.x, pt.y, pt.x + m_rectRender.Width() + 10, pt.y + m_rectRender.Height() + (m_nSlot * 26) );
#if __VER >= 12 // __EXT_PIERCING
#if __VER < 15 // __IMPROVE_SYSTEM_VER15
			if(m_rectRender.right < 194)    // Min Width Size
				m_rectRender.right = 194;
#endif // __IMPROVE_SYSTEM_VER15

			rect.SetRect( pt.x, pt.y - (36), pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height() );
			if(rect.top < p2DRender->m_clipRect.top)
			{
				pt.y = p2DRender->m_clipRect.top + 8;
				rect.SetRect( pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height() + (36) );
			}
#else //__EXT_PIERCING
			if(m_rectRender.right < 190)    // Min Width Size
				m_rectRender.right = 190;

			rect.SetRect( pt.x, pt.y - (m_nSlot * 26), pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height() );
			if(rect.top < p2DRender->m_clipRect.top)
			{
				pt.y = p2DRender->m_clipRect.top + 8;
				rect.SetRect( pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height() + (m_nSlot * 26) );
			}
#endif //__EXT_PIERCING
		}
		else
			rect.SetRect( pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height() );
#else
		rect.SetRect( pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height() );
#endif //__CSC_VER9_1

		int nPlusLow = 8;
		int nPlusColumn = 8;
		int nSetting = g_Option.m_nToolTipTexture * 9;

		float fNa = float( rect.bottom - rect.top + nPlusLow * 2 ) / 16.0f;
		int nlowDraw = (int)fNa;
		if( nlowDraw == 0 && 0 < fNa )
			nlowDraw = 1;

		fNa = float( rect.right - rect.left + nPlusColumn * 2 ) / 16.0f;
		int nColumnDraw = (int)fNa;
		
		if( nColumnDraw == 0 && 0 < fNa )
			nColumnDraw = 1;

		CRect PlusRect = rect;
		PlusRect.left = rect.left - nPlusLow;
		PlusRect.top = rect.top - nPlusColumn;
		PlusRect.right = rect.right- 16 + nPlusLow;
		PlusRect.bottom = rect.bottom - 16 + nPlusColumn;

		for( int i = 0 ; i < nlowDraw ; ++i )
		{
			for( int j = 0 ; j < nColumnDraw ; ++j )
			{
				// 테두리 처리
				if( i == 0 || j == 0 )
				{
					if( j == 0 && i != 0 )
					{
						if( i + 1 == nlowDraw )
						{
							m_apTextureToolTip[ nSetting + 3 ].Render( p2DRender, CPoint( PlusRect.left, PlusRect.top + ( i * 16 ) ), CPoint( 16, PlusRect.bottom - ( PlusRect.top + ( i * 16 ) ) ) );
							m_apTextureToolTip[ nSetting + 5 ].Render( p2DRender, CPoint( PlusRect.right, PlusRect.top + ( i * 16 ) ), CPoint( 16, PlusRect.bottom - ( PlusRect.top + ( i * 16 ) ) ) );
						}
						else 
						{
							m_apTextureToolTip[ nSetting + 3 ].Render( p2DRender, CPoint( PlusRect.left, PlusRect.top + ( i * 16 ) ) );
							m_apTextureToolTip[ nSetting + 5 ].Render( p2DRender, CPoint( PlusRect.right, PlusRect.top + ( i * 16 ) ) );
						}
						
					}
					else if( i == 0 && j != 0 )
					{
						if( j + 1 == nColumnDraw )
						{
							m_apTextureToolTip[ nSetting + 1 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top ), CPoint( PlusRect.right - ( PlusRect.left + ( j * 16 ) ), 16 ) );
							m_apTextureToolTip[ nSetting + 7 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.bottom ), CPoint( PlusRect.right - ( PlusRect.left + ( j * 16 ) ), 16 ) );
						}
						else
						{
							m_apTextureToolTip[ nSetting + 1 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top ) );
							m_apTextureToolTip[ nSetting + 7 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.bottom ) );
						}
					}		
					continue;
				}
					
				// 가운데 처리
				if( i == nlowDraw - 1 || j == nColumnDraw - 1 )
				{
					// 끝에 맞지 않은 구조 처리 
					if( i == nlowDraw - 1 && j == nColumnDraw - 1 )
					{
						m_apTextureToolTip[ nSetting + 4 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top + ( i * 16 ) ),
							CPoint( PlusRect.right - ( PlusRect.left + ( j * 16 ) ), PlusRect.bottom - ( PlusRect.top + ( i * 16 ) ) ) );
						
					}
					else if( i == nlowDraw - 1 )
					{
						m_apTextureToolTip[ nSetting + 4 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top + ( i * 16 ) ),
							CPoint( 16, PlusRect.bottom - ( PlusRect.top + ( i * 16 ) ) ) );

					}
					else
					{
						m_apTextureToolTip[ nSetting + 4 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top + ( i * 16 ) ),
							CPoint( PlusRect.right - ( PlusRect.left + ( j * 16 ) ), 16 ) );
					}
				}
				else
				{
					m_apTextureToolTip[ nSetting + 4 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top + ( i * 16 ) ) );
				}
			}
		}

		m_apTextureToolTip[ nSetting + 0 ].Render( p2DRender, CPoint( PlusRect.left, PlusRect.top ) );
		m_apTextureToolTip[ nSetting + 2 ].Render( p2DRender, CPoint( PlusRect.right, PlusRect.top ) );
		m_apTextureToolTip[ nSetting + 6 ].Render( p2DRender, CPoint( PlusRect.left, PlusRect.bottom ) );
		m_apTextureToolTip[ nSetting + 8 ].Render( p2DRender, CPoint( PlusRect.right, PlusRect.bottom ) );

		p2DRender->TextOut_EditString( rect.TopLeft().x + 3, rect.TopLeft().y + 3, m_strToolTip, 0, 0, 2 );//, D3DCOLOR_ARGB( m_nAlpha * 255 / 255, 0, 0, 0 ) );

#ifndef __IMPROVE_MAP_SYSTEM
#if __VER >= 13 // __CSC_VER13_1
		if(m_nMonInfoCnt > 0)
		{
			int nMonElementYPos = 0;
			int nStrLine = 0;

			for(int i=0; i<m_nMonInfoCnt; i++)
			{
				if( (int)( m_strToolTip.GetLineCount() ) > i )
				{
					CString strTemp = m_strToolTip.GetLine(nStrLine);
					
					// Check Line.
					CString strLv, strEnd;
					strEnd = strTemp.GetAt( strTemp.GetLength() - 1 );
					if( strTemp.Find( prj.GetText( TID_GAME_MONSTER_INFORMATION_LEVEL ) ) == 0 )
					{
						int nLine = 1;
						if(strEnd != "\n")
						{
							nLine = 2;
							nStrLine++;
							strTemp = m_strToolTip.GetLine(nStrLine);
						}

						strTemp.TrimRight();
						CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent(strTemp);
						MoverProp* pMoverProp = prj.GetMoverProp(m_pDwMonId[i]);
						
						if(pMoverProp)
						{
							if( g_WndMng.m_pWndWorld && pMoverProp->eElementType )
							{
								if(i==0)
									nMonElementYPos = PlusRect.top + 8 + (size.cy + 2) * (nLine - 1);
								else
									nMonElementYPos += (size.cy + 2) * nLine;

								g_WndMng.m_pWndWorld->m_texAttrIcon.Render( p2DRender, CPoint(PlusRect.left + size.cx + 20, nMonElementYPos), pMoverProp->eElementType-1, 255, 1.0f, 1.0f );
							}
						}

						nStrLine++;
					}
				}
			}
		}
#endif //__CSC_VER13_1
#endif // __IMPROVE_MAP_SYSTEM

#ifdef __IMPROVE_MAP_SYSTEM
		if( static_cast< int >( m_vecMapMonsterID.size() ) > 0 )
		{
			int nMonElementYPos = 0;
			int nStringLine = 0;
			for( vector< DWORD >::iterator Iterator = m_vecMapMonsterID.begin(); Iterator != m_vecMapMonsterID.end(); ++Iterator )
			{
				CString strTemp = m_strToolTip.GetLine( nStringLine );
				CString strLevel = _T( "" );
				CString strEnd = strTemp.GetAt( strTemp.GetLength() - 1 );
				if( strTemp.Find( prj.GetText( TID_GAME_MONSTER_INFORMATION_LEVEL ) ) == 0 )
				{
					int nLine = 1;
					if( strEnd != "\n" )
					{
						nLine = 2;
						++nStringLine;
						strTemp = m_strToolTip.GetLine( nStringLine );
					}

					strTemp.TrimRight();
					CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent( strTemp );
					MoverProp* pMoverProp = prj.GetMoverProp( *Iterator );
					if( pMoverProp )
					{
						if( g_WndMng.m_pWndWorld && pMoverProp->eElementType )
						{
							if( Iterator == m_vecMapMonsterID.begin() )
								nMonElementYPos = PlusRect.top + 8 + ( size.cy + 2 ) * ( nLine - 1 );
							else
								nMonElementYPos += ( size.cy + 2 ) * nLine;

							g_WndMng.m_pWndWorld->m_texAttrIcon.Render( p2DRender, CPoint( PlusRect.left + size.cx + 20, nMonElementYPos ), pMoverProp->eElementType - 1, 255, 1.0f, 1.0f );
						}
					}

					++nStringLine;
				}
			}
		}
#endif // __IMPROVE_MAP_SYSTEM

#if __VER >= 9 // __CSC_VER9_1
		if(m_nAdded == 1)
		{
#if __VER >= 12 // __EXT_PIERCING
			CTexture* pTexture;
			
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
			if(m_pUltimateTexture != NULL)
			{
				if( m_nSubToolTipFlag == CWndMgr::TOOL_TIP_SWITCH_MAIN )
					m_pUltimateTexture->Render( p2DRender, CPoint( PlusRect.left + 10, PlusRect.top + 8) );
				else
					m_pUltimateTexture->Render( p2DRender, CPoint( PlusRect.left + 10, PlusRect.top + 24) );
			}
#else // __IMPROVE_SYSTEM_VER15
			if(m_pUltimateTexture != NULL)
				m_pUltimateTexture->Render( p2DRender, CPoint( PlusRect.left + 10, PlusRect.top + 8) );
#endif // __IMPROVE_SYSTEM_VER15

			//Jewel Icon Added
			CPoint point;
			point.x = PlusRect.left + 14 + ((PlusRect.Width() - 194) / 2);
			point.y = PlusRect.bottom - 24;

			for(int i=0; i<m_nSlot; i++)
			{
				if(m_pJewelBgTexture != NULL)
					m_pJewelBgTexture->RenderScal( p2DRender, point, 255, 1.0f, 1.0f );

				//Jewel Render
				if(m_nAddedJewel[i] != 0)
				{
					ItemProp* pItemProp;
					pItemProp = prj.GetItemProp( m_nAddedJewel[i] );
					if(pItemProp != NULL)
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->RenderScal( p2DRender, point, 255, 1.0f, 1.0f );
					}
				}

				point.x += 38;
			}
#else //__EXT_PIERCING
			//Ultimate Icon Added
			CString strPath;
			CTexture* pTexture;
			
			if(::GetLanguage() == LANG_FRE)
				strPath = MakePath( "Theme\\", ::GetLanguage(), "Icon_Ultimate.dds");
			else
				strPath = MakePath( DIR_ICON, "Icon_Ultimate.dds");

			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( PlusRect.left + 10, PlusRect.top + 8) );

			//Jewel Icon Added
			for(int i=0; i<m_nSlot; i++)
			{
				CPoint point;
				point.x = PlusRect.left + 8;
				point.y = PlusRect.bottom - 16 - (26*i);
				//Slot Render
				pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "back_Slotitem.tga"), 0xffff00ff );
				if(pTexture != NULL)
					pTexture->RenderScal( p2DRender, point, 255, 0.7f, 0.7f );

				point.x += 36;
				pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "Window02.tga"), 0xffff00ff );
				if(pTexture != NULL)
					pTexture->RenderScal( p2DRender, point, 255, 0.9f, 0.7f );
				point.x -= 36;
				//Jewel Render
				if(m_nAddedJewel[i] != 0)
				{
					ItemProp* pItemProp;
					pItemProp = prj.GetItemProp( m_nAddedJewel[i] );
					if(pItemProp != NULL)
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->RenderScal( p2DRender, point, 255, 0.7f, 0.7f );
					}
					//Text Render
					CString DstText;
					DWORD dwColorbuf;
					
					if(m_nAddedJewel[i] >= II_GEN_MAT_DIAMOND01 && m_nAddedJewel[i] <= II_GEN_MAT_TOPAZ01) //HP 상승
					{
						dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt1;
						DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_HP_MAX) );
					}
					else if(m_nAddedJewel[i] >= II_GEN_MAT_DIAMOND02 && m_nAddedJewel[i] <= II_GEN_MAT_TOPAZ02) //공격력 상승
					{
						dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt2;
						DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_ATKPOWER) );
					}
					else if(m_nAddedJewel[i] >= II_GEN_MAT_DIAMOND03 && m_nAddedJewel[i] <= II_GEN_MAT_TOPAZ03) //방어력 상승
					{
						dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt3;
						DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_ADJDEF) );
					}
					else if(m_nAddedJewel[i] >= II_GEN_MAT_DIAMOND04 && m_nAddedJewel[i] <= II_GEN_MAT_TOPAZ04) //흡혈
					{
						dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt4;
						DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_MELEE_STEALHP) );
					}
					else if(m_nAddedJewel[i] >= II_GEN_MAT_DIAMOND05 && m_nAddedJewel[i] <= II_GEN_MAT_TOPAZ05) //PvP시 대미지 증가
					{
						dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt5;
						DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_PVP_DMG) );
					}
					else if(m_nAddedJewel[i] >= II_GEN_MAT_DIAMOND06 && m_nAddedJewel[i] <= II_GEN_MAT_TOPAZ06) //힘증가
					{
						dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt6;
						DstText.Format( "%s", prj.GetText(TID_TOOLTIP_STR) );
					}
					else if(m_nAddedJewel[i] >= II_GEN_MAT_DIAMOND07 && m_nAddedJewel[i] <= II_GEN_MAT_TOPAZ07) //체력증가
					{
						dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt7;
						DstText.Format( "%s", prj.GetText(TID_TOOLTIP_STA) );
					}
					else if(m_nAddedJewel[i] >= II_GEN_MAT_DIAMOND08 && m_nAddedJewel[i] <= II_GEN_MAT_TOPAZ08) //민첩증가
					{
						dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt8;
						DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DEX) );
					}
					else if(m_nAddedJewel[i] >= II_GEN_MAT_DIAMOND09 && m_nAddedJewel[i] <= II_GEN_MAT_TOPAZ09) //지능증가
					{
						dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt9;
						DstText.Format( "%s", prj.GetText(TID_TOOLTIP_INT) );
					}

					point.x += 50;
					point.y += 6;
					p2DRender->TextOut( point.x, point.y, DstText, dwColorbuf );
				}
			}
#endif //__EXT_PIERCING
		}
#ifdef __NEW_ITEM_VARUNA
		else if ( m_nAdded == 2 )
		{
			if(m_pBarunaTexture != NULL)
			{
				if( m_nSubToolTipFlag == CWndMgr::TOOL_TIP_SWITCH_MAIN )
					m_pBarunaTexture->Render( p2DRender, CPoint( PlusRect.left + 10, PlusRect.top + 8) );
				else
					m_pBarunaTexture->Render( p2DRender, CPoint( PlusRect.left + 10, PlusRect.top + 24) );
			}
		}
#endif // __NEW_ITEM_VARUNA
#endif //__CSC_VER9_1
	}
}
#if __VER >= 9 // __CSC_VER9_1
void CToolTip::SetUltimateToolTip(CItemBase* pItemBase)
{
	m_nAdded = 1;
#if __VER >= 12 // __EXT_PIERCING
	m_nSlot = ((CItemElem*)pItemBase)->GetUltimatePiercingSize();
	for(int i=0; i<m_nSlot; i++)
		m_nAddedJewel[i] = ((CItemElem*)pItemBase)->GetUltimatePiercingItem(i);
#else // __EXT_PIERCING
	m_nSlot = ((CItemElem*)pItemBase)->GetPiercingSize();
	for(int i=0; i<m_nSlot; i++)
		m_nAddedJewel[i] = ((CItemElem*)pItemBase)->GetPiercingItem( m_nSlot - i - 1 );
#endif // __EXT_PIERCING

	m_pUltimateItemBase = pItemBase;
}
#endif //__CSC_VER9_1
#ifdef __NEW_ITEM_VARUNA
void CToolTip::SetBarunaToolTip(CItemBase* pItemBase)
{
	m_nAdded = 2;
}
#endif // __NEW_ITEM_VARUNA
#ifndef __IMPROVE_MAP_SYSTEM
#if __VER >= 13 // __CSC_VER13_1
void CToolTip::SetWorldMapMonsterInfo(int nMonCnt, DWORD* pDwMonId)
{
	m_nMonInfoCnt = nMonCnt;
	for(int i=0; i<nMonCnt; i++)
		m_pDwMonId[i] = pDwMonId[i];

	m_rectRender.right += 18;
}
#endif //__CSC_VER13_1
#endif // __IMPROVE_MAP_SYSTEM

#ifdef __IMPROVE_MAP_SYSTEM
//-----------------------------------------------------------------------------
void CToolTip::ResizeMapMonsterToolTip( void )
{
	m_rectRender.right += 18;
}
//-----------------------------------------------------------------------------
void CToolTip::InsertMonsterID( DWORD dwMonsterID )
{
	m_vecMapMonsterID.push_back( dwMonsterID );
}
//-----------------------------------------------------------------------------
#endif // __IMPROVE_MAP_SYSTEM

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
//-----------------------------------------------------------------------------
const CPoint& CToolTip::GetPointToolTip( void ) const
{
	return m_ptToolTip;
}
//-----------------------------------------------------------------------------
const CRect& CToolTip::GetRect( void ) const
{
	return m_rect;
}
//-----------------------------------------------------------------------------
void CToolTip::SetRenderRect( const CRect& rectRender )
{
	m_rectRender = rectRender;
}
//-----------------------------------------------------------------------------
const CRect& CToolTip::GetRenderRect( void ) const
{
	return m_rectRender;
}
//-----------------------------------------------------------------------------
const CRect& CToolTip::GetRevisedRect( void ) const
{
	return m_nRevisedRect;
}
//-----------------------------------------------------------------------------
void CToolTip::SetSubToolTipNumber( int nSubToolTipNumber )
{
	m_nSubToolTipNumber = nSubToolTipNumber;
}
//-----------------------------------------------------------------------------
int CToolTip::GetSubToolTipNumber( void ) const
{
	return m_nSubToolTipNumber;
}
//-----------------------------------------------------------------------------
BOOL CToolTip::GetReadyToolTipSwitch( void ) const
{
	return m_bReadyToopTip;
}
//-----------------------------------------------------------------------------
#endif // __IMPROVE_SYSTEM_VER15