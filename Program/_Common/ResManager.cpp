#include "stdafx.h"
#include "ResManager.h"

LPWNDCTRL WNDAPPLET::GetAt( DWORD dwWndId )
{
	LPWNDCTRL pWndCtrl = NULL;
	for( int i = 0; i < ptrCtrlArray.GetSize(); i++ )
	{
		pWndCtrl = (LPWNDCTRL) ptrCtrlArray.GetAt( i );
		if( pWndCtrl->dwWndId == dwWndId )
			return pWndCtrl;
	}
	return NULL;
}

CResManager::CResManager()
{
}

CResManager::~CResManager()
{
	Free();
}

void CResManager::Free()
{
	DWORD dwAppletId;
	LPWNDAPPLET LPWNDAPPLET;
	POSITION pos = m_mapWndApplet.GetStartPosition();
	while(pos)
	{ 
		m_mapWndApplet.GetNextAssoc( pos, (void*&)dwAppletId, (void*&)LPWNDAPPLET );
		for( int i = 0; i < LPWNDAPPLET->ptrCtrlArray.GetSize(); i++ )
			safe_delete( (LPWNDCTRL)LPWNDAPPLET->ptrCtrlArray.GetAt( i ) );
		SAFE_DELETE( LPWNDAPPLET );
	}
}

LPWNDAPPLET CResManager::GetAt( DWORD dwAppletId )
{
	LPWNDAPPLET LPWNDAPPLET = NULL;
	m_mapWndApplet.Lookup( (void*)dwAppletId, (void*&) LPWNDAPPLET );
	return LPWNDAPPLET;
}

LPWNDCTRL CResManager::GetAtControl( DWORD dwAppletId, DWORD dwCtrlId )
{
	LPWNDAPPLET lpWndApplet = GetAt( dwAppletId );
	if( lpWndApplet )
		return lpWndApplet->GetAt ( dwCtrlId );
	return NULL;
}

CString CResManager::GetLangApplet( CScript& scanner, LPWNDAPPLET lpWndApplet, BOOL bToolTip )
{
	CString string;
	scanner.GetToken();	// {
	scanner.GetToken();
	string	= scanner.token;
	scanner.GetToken();	// }

	return string;
}
CString CResManager::GetLangCtrl( CScript& scanner, LPWNDCTRL lpWndCtrl, BOOL bToolTip )
{

	CString string;
	scanner.GetToken();	// {
	scanner.GetToken();
	string	= scanner.token;
	scanner.GetToken();	// }

	return string;
}

BOOL CResManager::Load( LPCTSTR lpszName )
{
	CScript scanner;
	if( scanner.Load( lpszName, FALSE ) == FALSE )
		return FALSE;

	DWORD dwWndType;
	scanner.GetToken_NoDef();

	while( scanner.tok != FINISHED )
	{
		LPWNDAPPLET pWndApplet = new WNDAPPLET;
		pWndApplet->pWndBase = NULL;
		pWndApplet->strDefine = scanner.token;


		pWndApplet->dwWndId = CScript::GetDefineNum( scanner.token );

		scanner.GetToken();
		pWndApplet->strTexture = scanner.token;

#ifdef __19_INTERFACE_UPDATE
		scanner.GetToken();
	    pWndApplet->strTextureIcon = scanner.token;
#else
		scanner.GetToken();
#endif//__19_INTERFACE_UPDATE



		pWndApplet->bTile = scanner.GetNumber();

		CSize size;
		pWndApplet->size.cx = scanner.GetNumber();
		pWndApplet->size.cy = scanner.GetNumber();
		pWndApplet->dwWndStyle = scanner.GetNumber();
		pWndApplet->d3dFormat = (D3DFORMAT)scanner.GetNumber();

		// 타이틀 
		pWndApplet->strTitle = GetLangApplet( scanner, pWndApplet, FALSE );
		// 핼프 키 
		pWndApplet->strToolTip = GetLangApplet( scanner, pWndApplet, TRUE );

		
		// HelpKey
		m_mapWndApplet.SetAt( (void*)pWndApplet->dwWndId, pWndApplet );
		scanner.GetToken(); // skip {
		dwWndType = scanner.GetNumber(); 
		while( *scanner.token != '}' )
		{
			LPWNDCTRL pWndCtrl = new WNDCTRL;
			pWndCtrl->dwWndType = dwWndType;
			scanner.GetToken_NoDef();
			pWndCtrl->strDefine = scanner.token;///Char;
			for( int z = 0; z < pWndApplet->ptrCtrlArray.GetSize(); z++ )
			{
				if( ((LPWNDCTRL)pWndApplet->ptrCtrlArray.GetAt( z ) )->strDefine == pWndCtrl->strDefine )
				{
					CString string;
					string.Format( "%s에서 ID 충돌 %s ", pWndApplet->strDefine, pWndCtrl->strDefine );
					AfxMessageBox( string );
			//		assert(false);
				}
			}
			pWndCtrl->dwWndId = CScript::GetDefineNum( scanner.token );
			scanner.GetToken();
			pWndCtrl->strTexture = scanner.token;//token;
			pWndCtrl->bTile = scanner.GetNumber();
			pWndCtrl->rect.left = scanner.GetNumber();
			pWndCtrl->rect.top = scanner.GetNumber();
			pWndCtrl->rect.right = scanner.GetNumber();
			pWndCtrl->rect.bottom = scanner.GetNumber();
			pWndCtrl->dwWndStyle = scanner.GetNumber();

			// visible, Group, Disabled, TabStop
			pWndCtrl->m_bVisible = scanner.GetNumber();
			pWndCtrl->m_bGroup = scanner.GetNumber();
			pWndCtrl->m_bDisabled = scanner.GetNumber();
			pWndCtrl->m_bTabStop = scanner.GetNumber();
			
#ifdef __19_INTERFACE_UPDATE
			// color R, color G, color B
			pWndCtrl->m_bColorR = scanner.GetNumber();
			pWndCtrl->m_bColorG = scanner.GetNumber();
			pWndCtrl->m_bColorB = scanner.GetNumber();
#else //
			scanner.GetNumber();
			scanner.GetNumber();
			scanner.GetNumber();
#endif//__19_INTERFACE_UPDATE
			// 타이틀 
			pWndCtrl->strTitle = GetLangCtrl( scanner, pWndCtrl, FALSE );
			// 핼프 키 
			pWndCtrl->strToolTip = GetLangCtrl( scanner, pWndCtrl, TRUE );
			
			pWndApplet->ptrCtrlArray.Add( pWndCtrl );
			dwWndType = scanner.GetNumber(); 
		} 
		scanner.GetToken_NoDef();
	} 

	return TRUE;
}
