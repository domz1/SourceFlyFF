//Dem Projekt Neuz hinzufügen



#include "StdAfx.h"
#include "WndChangeJobEx.h"
#include "DPClient.h"
#include "DPLoginClient.h"
#include "Network.h"
#include "ResData.h"
#include "defineText.h"
#include "mover.h"

extern CDPClient g_DPlay;

CWndJobChangeEx::CWndJobChangeEx()
{
}

CWndJobChangeEx::~CWndJobChangeEx()
{
	DeleteDeviceObjects();
}

BOOL CWndJobChangeEx::Initialize( CWndBase* pWndParent, DWORD dwType )
{

	if( g_pPlayer->GetLevel() == MAX_JOB_LEVEL || g_pPlayer->GetLevel() == MAX_EXP_LEVEL+MAX_JOB_LEVEL || g_pPlayer->GetLevel() == MAX_LEGEND_LEVEL || ( ( g_pPlayer->GetLevel() == MAX_GENERAL_LEVEL) && g_pPlayer->GetExpPercent() == 9999 ) )
	{
		if(  g_pPlayer->GetJob() == 0 )
		{
			nJobMin = 1;
			nJobMax = 4;
		}
		else if( g_pPlayer->GetJob() < MAX_EXPERT )
		{
			nJobMin = (g_pPlayer->GetJob() + 2) * 2;
			nJobMax = nJobMin + 1;
		}
		else if( g_pPlayer->GetJob() < MAX_PROFESSIONAL )
		{
			nJobMin = g_pPlayer->GetJob() + 10;
			nJobMax = nJobMin;
		}
		else if( g_pPlayer->GetJob() < MAX_HERO )
		{
			nJobMin = g_pPlayer->GetJob() + 8;
			nJobMax = nJobMin;
		}
		nCurJob = nJobMin;
	}else{
		nCurJob = 0;
	}
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_FASTJOBCHANGE, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndJobChangeEx::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_BUTTON1:
		g_DPlay.UpdateJob( nCurJob );
		Destroy();
		break;
	case WIDC_BUTTON2:
		if( nCurJob < nJobMax )
			nCurJob++;
		break;
	case WIDC_BUTTON3:
		if( nCurJob > nJobMin )
			nCurJob--;
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndJobChangeEx::OnDraw( C2DRender* p2DRender )
{
	CWndButton* nJobPic = (CWndButton*)GetDlgItem( WIDC_BUTTON4 );
	CWndStatic* nJobTit = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	CWndButton* nApply = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	CWndButton* chk1 = (CWndButton*) GetDlgItem( WIDC_BUTTON2 );
	CWndButton* chk2 = (CWndButton*) GetDlgItem( WIDC_BUTTON3 );
	switch( nCurJob )
	{
	case 0:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotVan.bmp" ), TRUE );
		nJobPic->EnableWindow( FALSE );
		nJobTit->SetToolTip( "You don't have the right level" );
		nApply->EnableWindow( FALSE );
		nJobTit->SetTitle( "Error" );
		chk1->EnableWindow( FALSE );
		chk2->EnableWindow( FALSE );
		return;
		break;
	case 1:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotMer.bmp" ), TRUE );
		break;
	case 2:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotArc.bmp" ), TRUE );
		break;
	case 3:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotAs.bmp" ), TRUE );
		break;
	case 4:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotMag.bmp" ), TRUE );
		break;
	case 6: case 16: case 24:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotKnight.bmp" ), TRUE );
		break;
	case 7: case 17: case 25:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotBlad.bmp" ), TRUE );
		break;
	case 8: case 18: case 26:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotJes.bmp" ), TRUE );
		break;
	case 9: case 19: case 27:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotRan.bmp" ), TRUE );
		break;
	case 10: case 20: case 28:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotRingm.bmp" ), TRUE );
		break;
	case 11: case 21: case 29:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotBillfor.bmp" ), TRUE );
		break;
	case 12: case 22: case 30:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotPsy.bmp" ), TRUE );
		break;
	case 13: case 23: case 31:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotElem.bmp" ), TRUE );
		break;
	case 32:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotLord.bmp" ), TRUE );
		break;
	case 33:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotStormb.bmp" ), TRUE );
		break;
	case 34:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotWindI.bmp" ), TRUE );
		break;
	case 35:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotCracks.bmp" ), TRUE );
		break;
	case 36:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotFlor.bmp" ), TRUE );
		break;
	case 37:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotForcem.bmp" ), TRUE );
		break;
	case 38:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotMent.bmp" ), TRUE );
		break;
	case 39:
		nJobPic->SetTexture(D3DDEVICE, MakePath( DIR_THEME, "SlotElel.bmp" ), TRUE );
		break;
	default:
		return;
		break;
	}
	nJobPic->SetToolTip( prj.m_aJob[nCurJob].szName );
	nJobTit->SetTitle( prj.m_aJob[nCurJob].szName );
	( nCurJob == nJobMax ) ? chk1->EnableWindow( FALSE ) : chk1->EnableWindow( TRUE );
	( nCurJob == nJobMin ) ? chk2->EnableWindow( FALSE ):chk2->EnableWindow( TRUE );
}

void CWndJobChangeEx::OnInitialUpdate( )
{
	CWndNeuz::OnInitialUpdate(); 
	RestoreDeviceObjects();

	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndJobChangeEx::OnCommand( UINT nID, DWORD dwMessage, CWndBase *pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

HRESULT CWndJobChangeEx::RestoreDeviceObjects()
{
	CWndNeuz::RestoreDeviceObjects();
	return S_OK;
}
HRESULT CWndJobChangeEx::InvalidateDeviceObjects()
{
	CWndNeuz::InvalidateDeviceObjects();
    return S_OK;
}
HRESULT CWndJobChangeEx::DeleteDeviceObjects()
{
	CWndNeuz::DeleteDeviceObjects();
	InvalidateDeviceObjects();
	return S_OK;
}