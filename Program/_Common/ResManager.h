#ifndef __RESMANAGER_H
#define __RESMANAGER_H

#include "data.h"
#include "lang.h"

#define DEF_CTRL_CHECK "ButtCheck.tga"
#define DEF_CTRL_RADIO "ButtRadio.tga"
#define DEF_CTRL_GROUP "WndEditTile200.tga"
#define DEF_CTRL_TAB1  "WndTabTile00.bmp"
#define DEF_CTRL_TAB2  "WndTabTile10.bmp"
#define DEF_WNDBASE    "WndTile00.tga"
#define DEF_CTRL_TEXT  "WndEditTile00.tga"


typedef struct tagWndCtrl
{
	DWORD dwWndId;
	DWORD dwWndType;
	DWORD dwWndStyle;
	BOOL bTile;

	CString strDefine;
	CString strTitle;
	CString strToolTip;	
	CString strTexture;
	CRect rect;
	BOOL m_bVisible;
	BOOL m_bDisabled;
	BOOL m_bTabStop;
	BOOL m_bGroup;

#ifdef __19_INTERFACE_UPDATE
	BOOL m_bColorR;
	BOOL m_bColorG;
	BOOL m_bColorB;
#endif//__19_INTERFACE_UPDATE

} WNDCTRL,* LPWNDCTRL; 

typedef struct tagWndApplet
{
	CWndBase* pWndBase;
	DWORD dwWndId;
	DWORD dwWndStyle;
	BOOL bTile;
	
	CString strTitle;
	CString strToolTip;	
	D3DFORMAT d3dFormat;
	CString strDefine;
	CString strTexture;
#ifdef __19_INTERFACE_UPDATE
	CString strTextureIcon;
#endif//__19_INTERFACE_UPDATE
	CPtrArray ptrCtrlArray;
	CSize size;

	LPWNDCTRL GetAt( DWORD dwWndId );

} WNDAPPLET,* LPWNDAPPLET;

class CResManager
{
public:
	CResManager();
	~CResManager();
		
private:
	CMapPtrToPtr m_mapWndApplet;

public:
	void Free();
	BOOL Load( LPCTSTR lpszName );
	LPWNDAPPLET GetAt( DWORD dwAppletId );
	LPWNDCTRL GetAtControl( DWORD dwAppletId, DWORD dwCtrlId );

private:
	CString GetLangApplet( CScript& scanner, LPWNDAPPLET lpWndApplet, BOOL bToolTip );
	CString GetLangCtrl( CScript& scanner, LPWNDCTRL lpWndCtrl, BOOL bToolTip );
};


#endif