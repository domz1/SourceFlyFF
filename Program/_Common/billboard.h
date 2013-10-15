// BeastView.h : interface of the CBeastView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BILLBOARD_H__25FE7788_56AD_4D50_8D9E_E69C969DE0F9__INCLUDED_)
#define AFX_BILLBOARD_H__25FE7788_56AD_4D50_8D9E_E69C969DE0F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBillboard; 

typedef struct tagBILLBOARD
{
	TCHAR m_lpszFileName [32];
	TCHAR m_lpszName [32];
	CRect rect;
	CPoint ptCenter;
	D3DCOLOR dwColor;
	BOOL bAnimation;
	DWORD dwAniMethod;
	DWORD dwFrame;
	BOOL  bEffect;
	FLOAT fScale;
	CBillboard* m_pBillboard;

}* LPBILLBOARD, BILLBOARD;

class CBillboard : public CModel 
{
	LPBILLBOARD  m_pBillboard;
	CTexture*    m_pTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	DWORD                   m_nVertexNum;
public:
	float m_fScale;

	CBillboard();
	~CBillboard();

	void AddFrame( FLOAT fAddFrame ) { }
	void SetFrame( FLOAT fFrame ) { }
	CTexture* GetTexture(void) { return m_pTexture; }
	virtual BOOL Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmatWorld = NULL );
	virtual void FrameMove() { }
	// dx 초기화 관련 
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, LPBILLBOARD lpBillboard, CTexture* pTexture );
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BILLBOARD_H__25FE7788_56AD_4D50_8D9E_E69C969DE0F9__INCLUDED_)






