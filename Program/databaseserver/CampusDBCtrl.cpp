// CampusDBCtrl.cpp: implementation of the CCampusDBCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CampusDBCtrl.h"

#if __VER >= 15 // __CAMPUS
#include "dptrans.h"
extern	APP_INFO	g_appInfo;
#include "DPCoreSrvr.h"
extern	CDPCoreSrvr	g_dpCoreSrvr;

//////////////////////////////////////////////////////////////////////
// CCampusDBCtrl Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCampusDBCtrl::CCampusDBCtrl()
:m_pLogQuery( NULL )
{

}

CCampusDBCtrl::~CCampusDBCtrl()
{
	SAFE_DELETE( m_pLogQuery );
}

void CCampusDBCtrl::Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey )
{
	CAr ar( pov->lpBuf, pov->uBufSize );
	
	switch( pov->nQueryMode )
	{
	case CAMPUS_LOAD:
		CreateLogQuery();
		LoadAllCampus();
		break;

	case CAMPUS_SEND:
		SendAllCampus( dwCompletionKey );
		break;

	case CAMPUS_ADD_MEMBER:
		AddCampusMember( ar );
		break;

	case CAMPUS_REMOVE_MEMBER:
		RemoveCampusMember( ar );
		break;

	case CAMPUS_UPDATE_POINT:
		UpdateCampusPoint( ar );
		break;
	}
}

void CCampusDBCtrl::CreateLogQuery()
{
	m_pLogQuery = new CQuery;

	const char* pass	= CDbManager::GetInstance().DB_ADMIN_PASS_LOG;
	if( FALSE == m_pLogQuery->Connect( 3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, pass ) )
	{
		::AfxMessageBox( "Can't connect db: CCampusDBCtrl.CreateLogQuery" );
		SAFE_DELETE( m_pLogQuery );
		ASSERT( 0 );
	}
}

void CCampusDBCtrl::LoadAllCampus()
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	sprintf( szQuery, "usp_Campus_Load @serverindex = '%02d'", g_appInfo.dwSys );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
	
	while( pQuery->Fetch() )
	{
		CCampus* pCampus = new CCampus;
		pCampus->SetCampusId( pQuery->GetInt( "idCampus" ) );
		CCampusHelper::GetInstance()->AddCampus( pCampus );
	}
	
	sprintf( szQuery, "usp_CampusMember_Load @serverindex = '%02d'", g_appInfo.dwSys );
	if( pQuery->Exec( szQuery ) == FALSE )
	{
		CCampusHelper::GetInstance()->Clear();
		WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery );
		return;
	}
	
	u_long idCampus;
	while( pQuery->Fetch() )
	{
		CCampusMember* pMember = new CCampusMember;
		pMember->SetPlayerId( pQuery->GetInt( "m_idPlayer" ) );
		pMember->SetLevel( pQuery->GetInt( "nMemberLv" ) );
		idCampus = pQuery->GetInt( "idCampus" );
		CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus( idCampus );
		if( pCampus )
		{
			pCampus->AddMember( pMember );
			if( pMember->GetLevel() == CAMPUS_MASTER )
				pCampus->SetMaster( pMember->GetPlayerId() );
			CCampusHelper::GetInstance()->AddPlayerId2CampusId( pMember->GetPlayerId(), idCampus );
		}
		else
		{
			WriteLog( "LoadAllCampus(): Player's campus not found - %d, %d", pMember->GetPlayerId(), idCampus );
			SAFE_DELETE( pMember );
		}
	}
}

void CCampusDBCtrl::AddCampusMember( CAr & ar )
{
	u_long idMaster, idPupil;
	int nMasterPoint, nPupilPoint;
	ar >> idMaster >> nMasterPoint >> idPupil >> nPupilPoint;

	if( nMasterPoint < 0 || nPupilPoint < 0
		|| CCampusHelper::GetInstance()->GetCampus( CCampusHelper::GetInstance()->GetCampusIdByPlayerId( idPupil ) ) )
		return;

	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus( CCampusHelper::GetInstance()->GetCampusIdByPlayerId( idMaster ) );
	if( pCampus )
	{
		if( pCampus->GetPupilNum() >= CCampusHelper::GetInstance()->GetMaxPupilNum( nMasterPoint ) )
			return;

		CCampusMember* pCM = new CCampusMember;
		pCM->SetLevel( CAMPUS_PUPIL );
		pCM->SetPlayerId( idPupil );
		if( pCampus->AddMember( pCM )  )
		{
			if( CCampusHelper::GetInstance()->AddPlayerId2CampusId( idPupil, pCampus->GetCampusId() ) )
			{
				UpdateCampusId( idPupil, pCampus->GetCampusId() );
				InsertCampusMember( pCampus->GetCampusId(), idPupil, CAMPUS_PUPIL );
			}
			else
			{
				Error( "AddPlayerId2CampusId() fail" );
				pCampus->RemoveMember( pCM->GetPlayerId() );
				SAFE_DELETE( pCM );
				return;
			}
		}
		else
		{
			SAFE_DELETE( pCM );
			return;
		}
	}
	else
	{
		pCampus = new CCampus;
		pCampus->SetMaster( idMaster );
		u_long idCampus = CCampusHelper::GetInstance()->AddCampus( pCampus );
		if( idCampus > 0 )
		{
			CCampusMember* pMaster = new CCampusMember;
			CCampusMember* pPupil = new CCampusMember;

			pMaster->SetLevel( CAMPUS_MASTER );
			pMaster->SetPlayerId( idMaster );
			pPupil->SetLevel( CAMPUS_PUPIL );
			pPupil->SetPlayerId( idPupil );

			if( pCampus->AddMember( pMaster ) && pCampus->AddMember( pPupil )
				&& CCampusHelper::GetInstance()->AddPlayerId2CampusId( idMaster, idCampus )
				&& CCampusHelper::GetInstance()->AddPlayerId2CampusId( idPupil, idCampus ) )
			{
				UpdateCampusId( idMaster, idCampus );
				UpdateCampusId( idPupil, idCampus );
				InsertCampus( idCampus );
				InsertCampusMember( idCampus, idMaster, CAMPUS_MASTER );
				InsertCampusMember( idCampus, idPupil, CAMPUS_PUPIL );
			}
			else
			{
				Error( "AddMember failed!" );
				SAFE_DELETE( pMaster );
				SAFE_DELETE( pPupil );
				CCampusHelper::GetInstance()->RemoveCampus( idCampus );
				return;
			}
		}
		else
		{
			Error( "new campus failed! campus exist" );
			SAFE_DELETE( pCampus );
			return;
		}
	}
	LogUpdateCampusMember( pCampus->GetCampusId(), idMaster, idPupil, 'T' );
	CDPTrans::GetInstance()->SendAddCampusMember( pCampus->GetCampusId(), idMaster, idPupil );
}

void CCampusDBCtrl::RemoveCampusMember( CAr & ar )
{
	u_long idCampus, idPlayer;
	ar >> idCampus >> idPlayer;

	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus( idCampus );
	if( pCampus && pCampus->IsMember( idPlayer ) )
	{
		if( pCampus->IsMaster( idPlayer ) )
		{
			vector<u_long> vecMember = pCampus->GetAllMemberPlayerId();
			for( vector<u_long>::iterator it = vecMember.begin(); it != vecMember.end(); ++it )
			{
				CCampusHelper::GetInstance()->RemovePlayerId2CampusId( *it );
				UpdateCampusId( *it, 0 );
				if( *it != idPlayer )
				{
					DeleteCampusMember( *it, CAMPUS_PUPIL );
					LogUpdateCampusMember( idCampus, idPlayer, *it, 'F' );
				}
			}
			CCampusHelper::GetInstance()->RemoveCampus( idCampus );
			DeleteCampus( idCampus );
		}
		else
		{
			CCampusHelper::GetInstance()->RemovePlayerId2CampusId( idPlayer );
			pCampus->RemoveMember( idPlayer );
			UpdateCampusId( idPlayer, 0 );
			DeleteCampusMember( idPlayer, CAMPUS_PUPIL );
			LogUpdateCampusMember( idCampus, pCampus->GetMaster(), idPlayer, 'F' );
			if( pCampus->GetMemberSize() < 2 )
			{
				CCampusHelper::GetInstance()->RemovePlayerId2CampusId( pCampus->GetMaster() );
				UpdateCampusId( pCampus->GetMaster(), 0 );
				CCampusHelper::GetInstance()->RemoveCampus( idCampus );
				DeleteCampus( idCampus );
			}
		}
		CDPTrans::GetInstance()->SendRemoveCampusMember( idCampus, idPlayer );
	}
	else
		Error( "RemoveCampusMember fail" );
}

void CCampusDBCtrl::UpdateCampusPoint( CAr & ar )
{
	u_long idPlayer;
	int nCampusPoint;
	BOOL bAdd;
	char chState;
	ar >> idPlayer >> nCampusPoint >> bAdd >> chState;
	
	int nPrevPoint, nCurrPoint;
	nPrevPoint = nCurrPoint = 0;
	if( bAdd )
	{
		nCurrPoint = UpdateCampusPoint( idPlayer, nCampusPoint );
		nPrevPoint = nCurrPoint - nCampusPoint;
	}
	else
	{
		nCurrPoint = UpdateCampusPoint( idPlayer, -(nCampusPoint) );
		nPrevPoint = nCurrPoint + nCampusPoint;
	}
	LogUpdateCampusPoint( idPlayer, nPrevPoint, nCurrPoint, chState );
	CDPTrans::GetInstance()->SendUpdateCampusPoint( idPlayer, nCurrPoint );
}

void CCampusDBCtrl::SendAllCampus( DPID dpId )
{
	if( !CCampusHelper::GetInstance()->IsEmpty() )
		CDPTrans::GetInstance()->SendAllCampus( dpId );
}

void CCampusDBCtrl::InsertCampus( u_long idCampus )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	sprintf( szQuery, "usp_Campus_Insert @idCampus = %d, @serverindex = '%02d'", idCampus, g_appInfo.dwSys );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

void CCampusDBCtrl::DeleteCampus( u_long idCampus )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	sprintf( szQuery, "usp_Campus_Delete @idCampus = %d, @serverindex = '%02d'", idCampus, g_appInfo.dwSys );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

void CCampusDBCtrl::InsertCampusMember( u_long idCampus, u_long idPlayer, int nMemberLv )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	sprintf( szQuery, "usp_CampusMember_Insert @idCampus = %d, @serverindex = '%02d', @m_idPlayer = '%07d', @nMemberLv = %d", 
		idCampus, g_appInfo.dwSys, idPlayer, nMemberLv );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

void CCampusDBCtrl::DeleteCampusMember( u_long idPlayer, int nMemberLv )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	sprintf( szQuery, "usp_CampusMember_Delete  @serverindex = '%02d', @m_idPlayer = '%07d', @nMemberLv = %d", 
		g_appInfo.dwSys, idPlayer, nMemberLv );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

int CCampusDBCtrl::UpdateCampusPoint( u_long idPlayer, int nCampusPoint )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	g_DbManager.DBQryCharacter( szQuery, "U5", idPlayer, g_appInfo.dwSys, '\0', "", nCampusPoint );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return INT_MAX; }

	int nTotalPoint = INT_MAX;
	if( pQuery->Fetch() )
		nTotalPoint = pQuery->GetInt( "m_nCampusPoint" );
	
	return nTotalPoint;
}

void CCampusDBCtrl::UpdateCampusId( u_long idPlayer, u_long idCampus )
{
	CQuery* pQuery = GetQueryObject();
	char szQuery[QUERY_SIZE] = {0, };
	
	g_DbManager.DBQryCharacter( szQuery, "U6", idPlayer, g_appInfo.dwSys, '\0', "", idCampus );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

void CCampusDBCtrl::LogUpdateCampusMember( u_long idCampus, u_long idMaster, u_long idPupil, char chState )
{
	char szQuery[QUERY_SIZE] = {0, };

	sprintf( szQuery, "usp_CampusLog_Insert  @m_idMaster = '%07d', @serverindex = '%02d', @idCampus = %d, @m_idPupil = '%07d', @chState = '%c'", 
		idMaster, g_appInfo.dwSys, idCampus, idPupil, chState );
	if( m_pLogQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

void CCampusDBCtrl::LogUpdateCampusPoint( u_long idPlayer, int nPrevPoint, int nCurrPoint, char chState )
{
	char szQuery[QUERY_SIZE] = {0, };

	sprintf( szQuery, "usp_CampusPointLog_Insert  @m_idPlayer = '%07d', @serverindex = '%02d', @chState = '%c', @nPrevPoint = %0d, @nCurrPoint = %d", 
		idPlayer, g_appInfo.dwSys, chState, nPrevPoint, nCurrPoint );
	if( m_pLogQuery->Exec( szQuery ) == FALSE )
	{ WriteLog( "%s, %d\t%s", __FILE__, __LINE__, szQuery ); return; }
}

//////////////////////////////////////////////////////////////////////
// CCampusHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCampusHelper::CCampusHelper()
{
	m_pCampusMng = new CCampusMng;
	if( !m_CampusDBCtrl.CreateDbHandler() )
		Error( "CCampusHelper - m_CampusDBCtrl.CreateDbHandler()" );
}

CCampusHelper::~CCampusHelper()
{
	SAFE_DELETE( m_pCampusMng );
	m_CampusDBCtrl.CloseDbHandler();
}

CCampusHelper* CCampusHelper::GetInstance()
{
	static CCampusHelper sCH;
	return & sCH;
}

int CCampusHelper::GetMaxPupilNum( int nCampusPoint )
{
	if( nCampusPoint >= 0 && nCampusPoint < MIN_LV2_POINT )
		return 1;
	else if( nCampusPoint >= MIN_LV2_POINT && nCampusPoint < MIN_LV3_POINT )
		return 2;
	else if( nCampusPoint >= MIN_LV3_POINT )
		return 3;
	
	return 0;
}

#endif // __CAMPUS