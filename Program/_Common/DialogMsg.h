#ifndef __DIALOGMSG_H
#define __DIALOGMSG_H

#define DLGEMOT_ATTACK     0
#define DLGEMOT_SUMMON     1 
#define DLGEMOT_EVADE      2
#define DLGEMOT_HELPER     3 
#define DLGEMOT_BERSERK    4 
#define DLGEMOT_LOOT       5
#define DLGEMOT_STUN       6
#define DLGEMOT_LOVE       7
#define DLGEMOT_SAD        8
#define DLGEMOT_TEAR       9

#ifdef __CLIENT
#include "timer.h"

typedef struct tagCUSTOMTEXT
{
	CEditString m_string;
	DWORD       m_dwRGB;
	CD3DFont*   m_pFont;
	CSize       m_size;
	CObj*       m_pObj;
	CRect       m_rect;
	CTimer      m_timer; 
	BOOL	    m_bInfinite;
	CTexture*   m_pTexture;
	int			m_nKind;
} CUSTOMTEXT,* LPCUSTOMTEXT;

#define CHAT_NORMAL		0
#define CHAT_PARTY		1
#define CHAT_GUILD		2
#define CHAT_SHOUT		3

#define MAX_EMOTICON_STR		32		// 이모티콘 명령글자 최대길이
#define MAX_EMOTICON_NUM		100		// 이모티콘 갯수

struct EmticonCmd
{
	DWORD m_dwIndex;
	TCHAR m_szCommand[MAX_EMOTICON_STR];  
};


class CDialogMsg : public CPtrArray
{
	struct Image 
	{
		COLORREF    m_RGB;
		CString     m_string;
		CObj*       m_pObj;
	};
	CRect  m_rectPage;
	int    m_nLinePos;
	int    m_nLineSpace;
	int    m_nBeginLine;
	int    m_nAddLineNum;
	void  Scroll();
public:
	
	CTexturePack m_texPack_ex[3];

	CTexturePack m_texEmoticonUser;
	EmticonCmd	m_EmiticonCmd[MAX_EMOTICON_NUM];
	void AddEmoticonUser( CObj* pObj, int nEmoticonIdx );
		
	bool LoadEmotion();
	void ClearVendorObjMsg();
	void RemoveVendorMessage(CObj* pObj);

	CTexturePack m_texEmoticon;
	void AddTexture( CObj* pObj, CTexture* pTexture );
	void AddEmoticon( CObj* pObj, int nEmoticonIdx );
	CD3DFont* m_pFont;
	CD3DFont* m_pFontEffect;

	CPtrArray m_textArray;
	
	CPtrArray m_VendortextArray;
	
	CTexture*		   m_pTex[3];
	
	CDialogMsg(); 
	~CDialogMsg();

	void ClearAllMessage();
	void ClearMessage( CObj* pObj );
	void Render( C2DRender* p2DRender );
	void SetLinePos( C2DRender* p2DRender, int nLinePos );
	void SetRectPage( C2DRender* p2DRender, CRect rect );
	void SetLineNum( C2DRender* p2DRender, int nLineNum );

	void AddMessage( CObj* pObj, LPCTSTR lpszMessage, DWORD = 0xffffffff, int nKind = 0, DWORD dwPStyle = 0x00000001 );
	void AddVendorMessage( CObj* pObj, LPCTSTR lpszMessage, DWORD RGB = 0xffffffff );
	void RemoveDeleteObjMsg();
	void RemoveMessage( CObj* pObj );

    // Initializing and destroying device-dependent objects
    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT DeleteDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
};
extern CDialogMsg g_DialogMsg;
#endif

#endif

