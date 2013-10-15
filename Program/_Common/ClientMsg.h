#ifndef __CLIENTMSG_H
#define __CLIENTMSG_H

//#include "graphic.h"
//#include "hanstr.h"
#include "timer.h"

typedef struct tagCLIENTMSG
{
	CEditString m_string;
	DWORD   m_dwRGB;
	int m_nAlpha;
	CD3DFont*  m_pFont;
	CSize m_size;
} CLIENTMSG,* LPCLIENTMSG;

class CClientMsg : public CPtrArray
{
	struct Image 
	{
		COLORREF    m_RGB;
		CString     m_string;
		int         m_nOverlapCnt;
		int         m_nEffectCnt;
		int         m_nEffectAdd;
		BOOL        m_bEffect;
		//CDibBitmap* m_pDibBitmap; 
		//LPVOID m_pDibBitmap; 
	};
	CTimer m_timer; 
	CRect  m_rectPage;
	int    m_nLinePos;
	int    m_nLineSpace;
	int    m_nBeginLine;
	//int    m_nAddLineNum;
	void  Scroll();
public:
	CD3DFont* m_pFont;
	CD3DFont* m_pFontEffect;

	CPtrArray m_textArray;

	CClientMsg(); 
	~CClientMsg();

	void Render( CPoint point, C2DRender* p2DRender );

	void SetClientMessage(int nSize,int nLineSpace);
	void ClearAllMessage();
	void SetLinePos(C2DRender* p2DRender,int nLinePos);
	void SetRectPage(C2DRender* p2DRender,CRect rect);
	void SetLineNum(C2DRender* p2DRender,int nLineNum);
//	void MakeImageString(C2DRender* p2DRender);
	void AddMessage( LPCTSTR lpszMessage, DWORD RGB, BOOL bEffect = FALSE );

    // Initializing and destroying device-dependent objects
    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT DeleteDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
};
extern CClientMsg g_ClientMsg;

#endif

