#include "stdafx.h"
#include "defineObj.h"
#include "defineSound.h"
#include "defineText.h"
#include "defineItem.h"
#include "AppDefine.h"
#include "DialogMsg.h"  
#include "ClientMsg.h"  
#include "DPClient.h"
#include "MsgHdr.h"
#include "WndMessenger.h"
#include "WndIndirectTalk.h"
#include "..\_UnhandledException\ExceptionHandler.h"
#include "guild.h"
#include "WorldMap.h"
#include "Commonctrl.h"
#include "SecretRoom.h"

extern	CDPClient	g_DPlay;

CWndColosseumComplete::CWndColosseumComplete() 
{  
} 
CWndColosseumComplete::~CWndColosseumComplete() 
{ 
} 
void CWndColosseumComplete::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndStatic* pStatic1	= (CWndStatic *)GetDlgItem( WIDC_STATIC1 );
	CWndStatic* pStatic2	= (CWndStatic *)GetDlgItem( WIDC_STATIC2 );

	CMover* pMover;
	CString Nomes;
	Nomes = pMover->GetName();
	pStatic1->SetTitle(Nomes);

	MoveParentCenter();
}
BOOL CWndColosseumComplete::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TARGETINFORNATION, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndColosseumComplete::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	Destroy();
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
BOOL CWndColosseumComplete::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
