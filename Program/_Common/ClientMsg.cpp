#include "stdafx.h"
#include "clientMsg.h"
#include "timer.h"


CClientMsg g_ClientMsg;

/////////////////

CClientMsg::CClientMsg()
{
//	m_textArray.SetSize( 10 );
	m_nLineSpace = 1;
	m_nBeginLine =  0;
	//m_nAddLineNum = 0;
	m_timer.Set(5000);
	m_pFont = NULL;
	m_pFontEffect = NULL;
}
CClientMsg::~CClientMsg()
{
	ClearAllMessage();
	for(int i = 0; i < m_textArray.GetSize(); i++)
		safe_delete( (LPCLIENTMSG)m_textArray.GetAt(i) );
}
void CClientMsg::SetClientMessage( int nSize, int nLineSpace )
{
//	m_textArray.SetSize( nSize );
	m_nLineSpace = nLineSpace;
	m_nBeginLine =  0;
	//m_nAddLineNum = 0;
	m_timer.Set(3000);
}
void CClientMsg::ClearAllMessage()
{
	LPCLIENTMSG lpClientMsg;
	for( int i = 0; i < m_textArray.GetSize(); i++ )
	{
		lpClientMsg = (LPCLIENTMSG)m_textArray.GetAt(i);
		//if(pImage->m_pDibBitmap)
			//delete pImage->m_pDibBitmap;
		safe_delete( lpClientMsg );
	}
	m_textArray.RemoveAll();
//	m_nAddLineNum = 0;  
}
void CClientMsg::Render( CPoint point, C2DRender* p2DRender )
{
	//point.x = 1024 / 2;
	//point.y = 768 / 2 + 100;
	CSize size;	
	m_nLineSpace = -3;
	LPCLIENTMSG lpClientMsg;
	int nLine = 0;//(p2DRender->m_clipRect.bottom - 30) - p2DRender->m_clipRect.top; 
	for( int i = m_textArray.GetSize() - 1; i >= 0 ; i-- )
	{
		lpClientMsg = (LPCLIENTMSG) m_textArray.GetAt( i );
		if(lpClientMsg)
		{
			LPCTSTR lpStr = lpClientMsg->m_string;
			p2DRender->m_pFont->GetTextExtent((TCHAR*)lpStr,&size);
			nLine -= size.cy + m_nLineSpace;

			CPoint pt = point;
			pt.x = pt.x - size.cx / 2;
			pt.y = pt.y + ( ( m_textArray.GetSize() * ( size.cy + m_nLineSpace ) ) / 2 ) + nLine;
			lpClientMsg->m_string.SetColor( ( lpClientMsg->m_nAlpha << 24 ) | ( lpClientMsg->m_dwRGB & 0x00ffffff ) );
			//p2DRender->TextOut_EditString( pt.x - 1, pt.y - 1, lpClientMsg->m_string );
			p2DRender->TextOut_EditString( pt.x, pt.y, lpClientMsg->m_string );
			
			if( lpClientMsg->m_nAlpha != 255 )
			{
				lpClientMsg->m_nAlpha -= 10;
				if( lpClientMsg->m_nAlpha < 0 )
				{
					safe_delete( (LPCLIENTMSG)m_textArray.GetAt( 0 ) );
					m_textArray.RemoveAt( 0 );
					i--;
				}
			}
		}
	}
	if(m_timer.Over() && m_textArray.GetSize())
	{
		lpClientMsg = (LPCLIENTMSG)m_textArray.GetAt(0);
		if( lpClientMsg )
		{
			m_timer.Reset();
			if( lpClientMsg->m_nAlpha == 255 )
				lpClientMsg->m_nAlpha = 254;
		}
	}
}
void CClientMsg::SetLinePos(C2DRender* p2DRender,int nLinePos)
{
//	MakeImageString(p2DRender);
}
void CClientMsg::SetRectPage(C2DRender* p2DRender,CRect rect)
{
	ClearAllMessage();
	m_rectPage = rect;
	//SetSize(rect.Height() / ( p2DRender->GetFontManager()->GetHanFontSize().cy + m_nLineSpace));
//	MakeImageString(p2DRender);
}
void CClientMsg::SetLineNum(C2DRender* p2DRender,int nLineNum)
{
	ClearAllMessage();
	//rectPage = rect;
	SetSize(nLineNum);
//	MakeImageString(p2DRender);
}
void CClientMsg::Scroll()
{
	/*
	if( m_textArray.GetSize() == 0 )
		return;
	LPCLIENTMSG lpClientMsg = (LPCLIENTMSG)m_textArray.GetAt( 0 );
	if(lpClientMsg)
	{
		delete lpClientMsg;
		m_textArray.SetAt(0,NULL);
	}
	m_nAddLineNum--;
	for(int i = 0; i < m_textArray.GetSize(); i++)
		m_textArray.SetAt(i,m_textArray.GetAt(i+1));
*/
	/*
	Image* pImage = (Image*)GetAt(0);
	if(pImage)
	{
		delete pImage->m_pDibBitmap;
		delete pImage;
		SetAt(0,NULL);
	}
	m_nAddLineNum--;
	for(int i = 0; i < m_nAddLineNum; i++)
		SetAt(i,GetAt(i+1));
		*/
}
void CClientMsg::AddMessage( LPCTSTR lpszMessage, DWORD RGB, BOOL bEffect )
{
	LPCLIENTMSG lpClientMsg = new CLIENTMSG;
	lpClientMsg->m_dwRGB = RGB;
	lpClientMsg->m_string.SetParsingString( lpszMessage, 0xffffffff );
	lpClientMsg->m_pFont = bEffect ? m_pFontEffect : m_pFont;
	lpClientMsg->m_nAlpha = 255;
	//m_textArray.SetAt( m_nAddLineNum, lpClientMsg );
	m_textArray.Add(lpClientMsg);
	int nMax = 6;
	if( m_textArray.GetSize() > nMax )
	{
		safe_delete( (LPCLIENTMSG)m_textArray.GetAt( 0 ) );
		m_textArray.RemoveAt( 0 );
	}
	//m_nAddLineNum++;
	m_timer.Set( 3000 );
}
