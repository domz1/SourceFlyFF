// Quiz.cpp: implementation of the CQuiz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Quiz.h"

#ifdef __QUIZ
#ifdef __WORLDSERVER
#include "User.h"
extern CUserMng g_UserMng;
#include "worldmng.h"
extern CWorldMng g_WorldMng;
#include "DPDatabaseClient.h"
extern CDPDatabaseClient g_dpDBClient;
#include "DPCoreClient.h"
extern CDPCoreClient g_DPCoreClient;
#include "defineobj.h"
#endif // __WORLDSERVER
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuiz::CQuiz()
#if defined(__DBSERVER) || defined(__WORLDSERVER)
:m_bAuto( FALSE ), m_bRun( FALSE ), m_nQuizSize( 0 )
#endif // defined(__DBSERVER) || defined(__WORLDSERVER)
#ifdef __WORLDSERVER
, m_nState( 0 ), m_bSelectLog( FALSE ), m_nQuizCount( 0 )
#endif // __WORLDSERVER
{
/*
#if defined(__CLIENT) || defined(__WORLDSERVER)
	m_pVObj = NULL;
	m_pHObj = NULL;
#endif // defined(__CLIENT) || defined(__WORLDSERVER)
*/
}

CQuiz::~CQuiz()
{
#if defined(__DBSERVER) || defined(__WORLDSERVER)
	Clear();
#endif // defined(__DBSERVER) || defined(__WORLDSERVER)
/*
#if defined(__CLIENT) || defined(__WORLDSERVER)
	SAFE_DELETE( m_pVObj );
	SAFE_DELETE( m_pHObj );
#endif // defined(__CLIENT) || defined(__WORLDSERVER)
*/
}

CQuiz* CQuiz::GetInstance()
{
	static CQuiz sQuiz;
	return & sQuiz;
}

#if defined(__DBSERVER) || defined(__WORLDSERVER)
void CQuiz::Clear()
{
	m_vecQuizList.clear();
}

void CQuiz::LoadScript()
{
	if( IsRun() )
		return;
	
	CLuaBase Lua;
	if( Lua.RunScript( "QuizEvent.lua" ) != 0 )
	{
		Error( "QuizEvent.lua Load Failed!!!" );
		exit(0);
	}
	m_bAuto						= static_cast<BOOL>( Lua.GetGlobalBool( "AutoMode" ) );
	m_nType						= static_cast<int>( Lua.GetGlobalNumber( "QuizType" ) );
	m_dwEntranceTime			= static_cast<DWORD>( Lua.GetGlobalNumber( "EntranceTime" ) );
#ifdef __WORLDSERVER
	m_nChannel					= static_cast<int>( Lua.GetGlobalNumber( "QuizChannel" ) );
	if( !m_nChannel )
		m_nChannel = g_uIdofMulti;

	Lua.GetGloabal( "tNPC" );
	Lua.PushNil();
	while( Lua.TableLoop( -2 ) )
	{
		m_sNPC.dwNPCId			= static_cast<DWORD>( CScript::GetDefineNum( Lua.GetFieldToString( -1, "strNPCId" ) ) );
		m_sNPC.strCharKey		= Lua.GetFieldToString( -1, "strCharKey" );
		m_sNPC.vPos.x			= static_cast<float>( Lua.GetFieldToNumber( -1, "xPos" ) );
		m_sNPC.vPos.y			= static_cast<float>( Lua.GetFieldToNumber( -1, "yPos" ) );
		m_sNPC.vPos.z			= static_cast<float>( Lua.GetFieldToNumber( -1, "zPos" ) );
		Lua.Pop( 1 );
	}
	Lua.Pop( 0 );

	m_dwWaitTime				= static_cast<DWORD>( Lua.GetGlobalNumber( "WaitTime" ) );
	m_dwQuestionTime			= static_cast<DWORD>( Lua.GetGlobalNumber( "QuestionTime" ) );
	m_dwAnswerTime				= static_cast<DWORD>( Lua.GetGlobalNumber( "AnswerTime" ) );
	m_dwWatchingZoneOpenTime	= static_cast<DWORD>( Lua.GetGlobalNumber( "WatchingZoneOpenTime" ) );
	m_dwCloseWaitTime			= static_cast<DWORD>( Lua.GetGlobalNumber( "CloseWaitTime" ) );
	m_dwPrizeItemId				= static_cast<DWORD>( CScript::GetDefineNum( Lua.GetGlobalString( "PrizeItemId" ) ) );
	m_nPrizeItemNum				= static_cast<int>( Lua.GetGlobalNumber( "PrizeItemNum" ) );
#endif // __WORLDSERVER
}
#endif // defined(__DBSERVER) || defined(__WORLDSERVER)

#ifdef __WORLDSERVER
void CQuiz::OpenQuizEvent()
{
	if( m_nState != QE_CLOSE )
		return;

	if( IsAutoMode() )
	{
		if( m_nChannel == g_uIdofMulti )
			m_bRun = TRUE;
	}
	else
		m_bRun = TRUE;

	if( m_bRun )
	{
		InitQuizData( GetType() );
		SetNPC();
		SetNextTime( GetTickCount() + m_dwEntranceTime );
		SetState( QE_OPEN );
		g_dpDBClient.SendQuizEventState( QE_OPEN, g_uIdofMulti );
	}
}

void CQuiz::MakeQuizList( QUIZLIST & QL )
{
	if( IsRun() )
		m_vecQuizList.push_back( QL );
}

void CQuiz::SetNPC()
{
	CMover* pMover	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, m_sNPC.dwNPCId );
	if( !pMover )
	{
		Error( "CQuiz::SetNPC() - pMover is NULL - %d, %s", m_sNPC.dwNPCId, m_sNPC.strCharKey.c_str() );
		return;
	}
	lstrcpy( pMover->m_szCharacterKey, m_sNPC.strCharKey.c_str() );
	pMover->InitNPCProperty();
	pMover->InitCharacter( pMover->GetCharacter() );
	pMover->SetPos( m_sNPC.vPos );
	pMover->InitMotion( MTI_STAND );
	pMover->UpdateLocalMatrix();
	pMover->AddItToGlobalId();
	m_sNPC.idNpc = pMover->GetId();
	CWorld* pWorld	= g_WorldMng.GetWorld( WI_WORLD_MADRIGAL );
	if( pWorld )
		pWorld->ADDOBJ( pMover, FALSE, nDefaultLayer );
}

void CQuiz::RemoveNPC()
{
	CMover* pMover = prj.GetMover( m_sNPC.idNpc );
	if( IsValidObj( pMover ) )
	{
		g_UserMng.AddCreateSfxObj( pMover, XI_SKILL_MER_ONE_SPECIALHIT02 );
		pMover->Delete();
	}
}

void CQuiz::NoticeMessage()
{
	if( GetState() == QE_OPEN )
	{
		DWORD dwRemainTime = GetNextTime() - GetTickCount();
		if( static_cast<int>( dwRemainTime ) <= 0 )
			return;
		g_DPCoreClient.SendQuizSystemMessage( TID_GAME_QUIZ_PRE_NOTICE1, FALSE, g_uIdofMulti, ( dwRemainTime / MIN( 1 ) + 1 ) );
		g_DPCoreClient.SendQuizSystemMessage( TID_GAME_QUIZ_PRE_NOTICE2 );
	}

	else if( GetState() == QE_WATCHINGZONE_OPEN )
	{
		DWORD dwRemainTime = GetNextTime() - GetTickCount();
		if( static_cast<int>( dwRemainTime ) <= 0 )
			return;
		g_DPCoreClient.SendQuizSystemMessage( TID_GAME_QUIZ_CUR_NOTICE1, FALSE, g_uIdofMulti, ( dwRemainTime / MIN( 1 ) + 1 ) );
		g_DPCoreClient.SendQuizSystemMessage( TID_GAME_QUIZ_CUR_NOTICE2 );
	}
}

int CQuiz::EntranceQuizEvent( CUser* pUser )
{
	if( !IsValidObj( pUser ) )
		return 0;

	if( pUser->IsFly() )
		return 0;
	
	if( m_nState == QE_CLOSE )
		return TID_GAME_QUIZ_NOT_OPEN;
	
	pUser->AddQuizEventState( GetType(), GetState() );
	if( pUser->HasActivatedSystemPet() )
		pUser->PetRelease();
	if( pUser->HasActivatedEatPet() )
		pUser->InactivateEatPet();
	if( pUser->HasPet() )
		pUser->RemovePet();
	
	else if( m_nState == QE_OPEN || m_nState == QE_WATCHINGZONE_OPEN )
	{
		// 퀴즈 존 입장
		if( !TeleportToQuizEvent( pUser, ZONE_QUIZ ) )
			g_dpDBClient.SendQuizEventEntrance( pUser->m_idPlayer, g_uIdofMulti );
		return 0;
	}
	else
	{
		// 대기 존 입장
		if( !TeleportToQuizEvent( pUser, ZONE_WATCHING ) )
			g_dpDBClient.SendQuizEventEntrance( pUser->m_idPlayer, g_uIdofMulti );
		return 0;
	}
	return TID_GAME_QUIZ_DO_NOT_TELEPORT;
}

int CQuiz::TeleportToQuizEvent( CUser* pUser, int nZone )
{
	if( !IsValidObj( pUser ) )
		return TID_GAME_QUIZ_DO_NOT_TELEPORT;

	if( ZONE_1 <= nZone && nZone <= ZONE_4 )
	{
		if( GetState() == QE_DROP_OUT || GetZoneType( (CMover*)pUser ) == ZONE_WATCHING )
			return TID_GAME_QUIZ_DO_NOT_TELEPORT;
	}

	if( nZone == ZONE_QUIZ )
	{
		if( GetState() != QE_OPEN && GetState() != QE_WATCHINGZONE_OPEN )
			return TID_GAME_QUIZ_DO_NOT_TELEPORT;
	}
	
	if( nZone == ZONE_EXIT )
	{
		GoOut( pUser );
		return 0;
	}

	D3DXVECTOR3 vPos = GetTeleportPos( nZone );
	if( vPos == D3DXVECTOR3( 0, 0, 0 ) )
		return TID_GAME_QUIZ_DO_NOT_TELEPORT;
	
	int nRandx = xRandom(4) - 2;
	int nRandz = xRandom(4) - 2;
	vPos += D3DXVECTOR3( (float)( nRandx ), (float)( 0 ), (float)( nRandz ) );

	pUser->REPLACE( g_uIdofMulti, WI_WORLD_QUIZ, vPos, REPLACE_NORMAL, nDefaultLayer );
	return 0;
}

D3DXVECTOR3 CQuiz::GetTeleportPos( int nZone )
{
	switch( nZone )
	{
	case ZONE_QUIZ:
		{
			int nRand = 0;
			if( GetType() == TYPE_OX )
				nRand = xRandom(2);
			else
				nRand = xRandom(4);
			return m_sQuizData.aExamplePos[nRand];
		}
		
	case ZONE_1:
		return m_sQuizData.aExamplePos[0];
		
	case ZONE_2:
		return m_sQuizData.aExamplePos[1];
		
	case ZONE_3:
		return m_sQuizData.aExamplePos[2];
		
	case ZONE_4:
		return m_sQuizData.aExamplePos[3];
		
	case ZONE_WATCHING:
		return m_sQuizData.vWatchingZonePos;
		
	default:
		return D3DXVECTOR3( 0, 0, 0 );
	}
}

void CQuiz::Process()
{
	DWORD dwTick = GetTickCount();
	vector<QUIZLIST>::iterator it = m_vecQuizList.begin();
	
	switch( m_nState )
	{
	case QE_CLOSE:
		// 퀴즈 이벤트 종료. 모든 유저 밖으로 이동
		if( dwTick > GetNextTime() )
			CloseQuizEvent();

		break;

	case QE_OPEN:
		{
			if( dwTick > GetNextTime() )
			{
				if( !m_vecQuizList.size() )
				{
					Error( "Quiz List Load Failed!" );
					CloseQuizEvent();
					return;
				}
				if( (int)( m_vecQuizList.size() ) < m_nQuizSize )
					break;
				BOOL bAble = FALSE;
				int nUser = 0;
				CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_QUIZ );
				if( pWorld )
				{
					for( DWORD i = 0; i < pWorld->m_dwObjNum; ++i )
					{
						if( IsInQuizEventPlayer( pWorld, nDefaultLayer, i ) )
						{
							static_cast<CUser*>( pWorld->m_apObject[i] )->AddQuizEventMessage( prj.GetText( TID_GAME_QUIZ_WAIT_QUESTION ) );
							bAble = TRUE;
							++nUser;
						}
					}
				}
				if( !bAble )
				{
					CloseQuizEvent();
					return;
				}

				if( nUser <= LIMIT_USER_COUNT )
					m_bSelectLog = TRUE;

				SetNextTime( dwTick + m_dwWaitTime );
				SetState( QE_QUESTION );
				g_dpDBClient.SendQuizEventState( QE_QUESTION, g_uIdofMulti );
			}
		}
		
		break;

	case QE_QUESTION:
		// 문제 출제
		if( dwTick > GetNextTime() )
		{
			// UI
			CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_QUIZ );
			if( pWorld )
			{
				for( DWORD i = 0; i < pWorld->m_dwObjNum; ++i )
				{
					if( IsInQuizEventPlayer( pWorld, nDefaultLayer, i ) )
					{
						static_cast<CUser*>( pWorld->m_apObject[i] )->AddQuizEventMessage( prj.GetText( TID_GAME_QUIZ_QUESTION ), QE_QUESTION );
						static_cast<CUser*>( pWorld->m_apObject[i] )->AddQuizQuestion( GetType(), it->strQuestion, m_dwQuestionTime / SEC(1) );
					}
				}
			}

			SetNextTime( dwTick + m_dwQuestionTime );
			SetState( QE_CORRECT_ANSWER );
			g_dpDBClient.SendQuizEventState( QE_CORRECT_ANSWER, g_uIdofMulti );
			++m_nQuizCount;
		}
		
		break;
		
	case QE_CORRECT_ANSWER:
		// 정답 공개
		if( dwTick > GetNextTime() )
		{
/*			// 유저 이동 불가 - object
			DWORD dwId	= NULL;
			
			if( GetType() == TYPE_OX )
				dwId = 1116;
			else
				dwId = 1115;
			
			m_pVObj = CreateObj( D3DDEVICE, OT_OBJ, dwId );
			if( !m_pVObj )
				return;
			
			m_pVObj->SetPos( m_sQuizData.vQuizZonePos );
			m_pVObj->UpdateLocalMatrix();
			CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_QUIZ );
			if( pWorld )
				pWorld->ADDOBJ( m_pVObj, TRUE, nDefaultLayer );
			
			if( GetType() == TYPE_4C )
			{
				m_pHObj = CreateObj( D3DDEVICE, OT_OBJ, dwId );
				if( !m_pHObj )
					return;
				
				m_pHObj->SetPos( m_sQuizData.vQuizZonePos );
				m_pHObj->SetAngle( 90 );
				m_pHObj->UpdateLocalMatrix();
				CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_QUIZ );
				if( pWorld )
					pWorld->ADDOBJ( m_pHObj, TRUE, nDefaultLayer );
			}*/

			// 문제 풀이 종료 메세지
			CString str_Message;
			str_Message.Format( prj.GetText( TID_GAME_QUIZ_END_SOLVE_QUESTION ), m_nQuizCount );

			// 정답 메세지
			CString strMessage;
			if( GetType() == TYPE_OX )
			{
				if( it->nCorrect == 1 )
					strMessage.Format( prj.GetText( TID_GAME_QUIZ_CORRECT_ANSWER_OX ), 'O' );
				else
					strMessage.Format( prj.GetText( TID_GAME_QUIZ_CORRECT_ANSWER_OX ), 'X' );
			}
			else if( GetType() == TYPE_4C )
				strMessage.Format( prj.GetText( TID_GAME_QUIZ_CORRECT_ANSWER_4C ), it->nCorrect );

			D3DXVECTOR3 vPos = m_sQuizData.aExamplePos[(*it).nCorrect - 1];
			g_UserMng.AddWorldCreateSfxObj( XI_QUIZCORRECTANSWER, vPos.x, vPos.y, vPos.z, FALSE, WI_WORLD_QUIZ );
			
			CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_QUIZ );
			if( pWorld )
			{
				for( DWORD i = 0; i < pWorld->m_dwObjNum; ++i )
				{
					if( IsInQuizEventPlayer( pWorld, nDefaultLayer, i ) )
					{
						static_cast<CUser*>( pWorld->m_apObject[i] )->AddQuizEventMessage( str_Message, QE_CORRECT_ANSWER );
						static_cast<CUser*>( pWorld->m_apObject[i] )->AddQuizEventMessage( strMessage );
					}
				}
			}

			SetNextTime( dwTick + m_dwWaitTime );
			SetState( QE_DROP_OUT );
			g_dpDBClient.SendQuizEventState( QE_DROP_OUT, g_uIdofMulti );
		}
		
		break;
		
	case QE_DROP_OUT:
		if( dwTick > GetNextTime() )
		{
			// 탈락자 대기존으로 이동
			DropOutWrongUser( (*it).nQuizId, (*it).nCorrect );
			m_vecQuizList.erase( it );
		}
		
		break;
		
	case QE_CLOSE_WAIT:
		{
			// 문제가 요구치 보다 많이 남아있을 시 대기존 개방
			if( m_vecQuizList.size() > REMAIN_QUIZ )
			{
				CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_QUIZ );
				if( pWorld )
				{
					for( DWORD i = 0; i < pWorld->m_dwObjNum; ++i )
					{
						if( IsInQuizEventPlayer( pWorld, nDefaultLayer, i ) )
						{
							static_cast<CUser*>( pWorld->m_apObject[i] )->AddQuizEventMessage( prj.GetText( TID_GAME_QUIZ_WATCHINGZONE_OPEN ), QE_WATCHINGZONE_OPEN );
							static_cast<CUser*>( pWorld->m_apObject[i] )->AddQuizEventMessage( prj.GetText( TID_GAME_QUIZ_TELEPORT_QUIZZONE ) );
						}
					}
				}
				
				SetNextTime( dwTick + m_dwWatchingZoneOpenTime );
				SetState( QE_WATCHINGZONE_OPEN );
				m_bSelectLog = FALSE;
				m_nQuizCount = 0;
				g_dpDBClient.SendQuizEventState( QE_WATCHINGZONE_OPEN, g_uIdofMulti );
			}
			// 문제가 요구치 보다 적을 시 퀴즈 이벤트 종료
			else
			{
				g_DPCoreClient.SendQuizSystemMessage( TID_GAME_QUIZ_CLOSING, TRUE, g_uIdofMulti );
				SetNextTime( dwTick + m_dwCloseWaitTime );
				SetState( QE_CLOSE );
				g_dpDBClient.SendQuizEventState( QE_CLOSE_WAIT, g_uIdofMulti );
			}
		}
		
		break;

	case QE_WATCHINGZONE_OPEN:
		// 대기존 개방
		if( dwTick > GetNextTime() )
		{
			BOOL bAble = FALSE;
			int nUser = 0;
			CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_QUIZ );
			if( pWorld )
			{
				for( DWORD i = 0; i < pWorld->m_dwObjNum; ++i )
				{
					if( IsInQuizEventPlayer( pWorld, nDefaultLayer, i ) )
					{
						static_cast<CUser*>( pWorld->m_apObject[i] )->AddQuizEventMessage( prj.GetText( TID_GAME_QUIZ_WAIT_QUESTION ) );
						//if( static_cast<CUser*>( pWorld->m_apObject[i] )->IsAuthHigher( AUTH_GAMEMASTER ) == FALSE )
						if( GetZoneType( static_cast<CMover*>( pWorld->m_apObject[i] ) ) == ZONE_QUIZ )
						{
							bAble = TRUE;
							++nUser;
						}
					}
				}
			}
			if( bAble )
			{
				SetNextTime( dwTick + m_dwWaitTime );
				SetState( QE_QUESTION );
				if( nUser <= LIMIT_USER_COUNT )
					m_bSelectLog = TRUE;
			}
			else
			{
				CloseQuizEvent();
				return;
			}
		}

		break;
	}
}

void CQuiz::DropOutWrongUser( int nQuizId, int nCorrect, int nItemId, int nItemNum )
{
	int nCorrectUser = 0;
	CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_QUIZ );
	if( pWorld )
	{
		for( DWORD i = 0; i < pWorld->m_dwObjNum; ++i )
		{
			if( IsInQuizEventPlayer( pWorld, nDefaultLayer, i ) )
			{
				if( m_bSelectLog )
				{
					int nSelect = GetUserSelectExample( static_cast<CUser*>( pWorld->m_apObject[i] ) );
					if( nSelect )
						g_dpDBClient.SendQuizEventSelect( static_cast<CUser*>( pWorld->m_apObject[i] )->m_idPlayer, g_uIdofMulti, nQuizId, nSelect, nCorrect );
				}
				static_cast<CUser*>( pWorld->m_apObject[i] )->AddQuizEventMessage( prj.GetText( TID_GAME_QUIZ_DROP_OUT ), QE_DROP_OUT );
				if( PtInCorrectZoneRect( static_cast<CUser*>( pWorld->m_apObject[i] ), nCorrect ) )
				{
					++nCorrectUser;
					// 091014 - mirchang : 시스템 부하 발생 가능성이 있어 보류
					//if( nItemId > 0 )	// Item Id가 있을시 정답자에게 해당 아이템 지급
					//	g_dpDBClient.SendPostPrizeItem( static_cast<CUser*>( pWorld->m_apObject[i] )->m_idPlayer, nItemId, nItemNum );
				}
			}
		}
		if( nCorrectUser <= LIMIT_USER_COUNT )
			m_bSelectLog = TRUE;
	}
	// 정답자 수가 2명 이상이고 문제가 남아있으면 문제 풀이 단계로 이동
	if( nCorrectUser > 1 && m_vecQuizList.size() > 1 )
	{
		SetNextTime( GetTickCount() + m_dwWaitTime );
		SetState( QE_QUESTION );
		g_dpDBClient.SendQuizEventState( QE_QUESTION, g_uIdofMulti );
	}
	// 우승자 상금 지급 후 진행 여부 판단
	else
	{
		CString strMessage;
		strMessage.Format( prj.GetText( TID_GAME_QUIZ_WINNERCOUNT ), nCorrectUser );
		if( pWorld )
		{
			for( DWORD i = 0; i < pWorld->m_dwObjNum; ++i )
			{
				if( IsInQuizEventPlayer( pWorld, nDefaultLayer, i ) )
				{
					static_cast<CUser*>( pWorld->m_apObject[i] )->AddQuizEventMessage( strMessage, QE_CLOSE_WAIT );
					// 스크립트에 정해진 아이템 지급
					if( PtInCorrectZoneRect( static_cast<CUser*>( pWorld->m_apObject[i] ), nCorrect ) )
						g_dpDBClient.SendPostPrizeItem( static_cast<CUser*>( pWorld->m_apObject[i] )->m_idPlayer, m_dwPrizeItemId, m_nPrizeItemNum );
				}
			}
			g_dpDBClient.SendQuizEventState( QE_WINNER, g_uIdofMulti, nCorrectUser, m_nQuizCount );
		}
		SetState( QE_CLOSE_WAIT );
	}
/*	// 유저 이동 가능 - object
	pWorld->DeleteObj( m_pVObj );
	m_pVObj = NULL;
	if( GetType() == TYPE_4C )
	{
		pWorld->DeleteObj( m_pHObj );
		m_pHObj = NULL;
	}*/
}

BOOL CQuiz::PtInCorrectZoneRect( CUser* pUser, int nCorrect )
{
//	if( pUser->IsAuthHigher( AUTH_GAMEMASTER ) )
//		return FALSE;

	D3DXVECTOR3 vPos = pUser->GetPos();
	POINT pt = { (int)vPos.x, (int)vPos.z };
	if( m_sQuizData.aExampleRect[nCorrect-1].PtInRect( pt ) )
		return TRUE;
	else
	{
		// 퀴즈존에 있는 오답자는 대기존으로 이동
		if( GetZoneType( pUser ) == ZONE_QUIZ )
			TeleportToQuizEvent( pUser, ZONE_WATCHING );
	}

	return FALSE;
}

int CQuiz::GetUserSelectExample( CUser* pUser )
{
	D3DXVECTOR3 vPos = pUser->GetPos();
	POINT pt = { (int)vPos.x, (int)vPos.z };
	if( m_sQuizData.aExampleRect[0].PtInRect( pt ) )
		return 1;
	else if( m_sQuizData.aExampleRect[1].PtInRect( pt ) )
		return 2;
	else if( m_sQuizData.aExampleRect[2].PtInRect( pt ) )
		return 3;
	else if( m_sQuizData.aExampleRect[3].PtInRect( pt ) )
		return 4;

	return 0;
}

BOOL CQuiz::IsInQuizEventPlayer( CWorld* pWorld, int nLayer, int nIndex )
{
	if( pWorld->m_apObject[nIndex] && pWorld->m_apObject[nIndex]->GetType() == OT_MOVER && 
		static_cast<CMover*>( pWorld->m_apObject[nIndex] )->IsPlayer()  && pWorld->m_apObject[nIndex]->GetLayer() == nLayer)
		return TRUE;
	
	return FALSE;
}

void CQuiz::GoOut( CUser* pUser )
{
//	if( !pUser->IsAuthHigher( AUTH_GAMEMASTER ) )
		pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6971.984f, 99.8f, 3336.884f ), REPLACE_NORMAL, nDefaultLayer );
}

void CQuiz::CloseQuizEvent()
{
	SetRun( FALSE );
	SetNextTime( 0 );
	SetState( QE_CLOSE );
	m_bSelectLog = FALSE;
	m_nQuizCount = 0;
	Clear();
	RemoveNPC();
	g_dpDBClient.SendQuizEventState( QE_CLOSE, g_uIdofMulti, 0, m_nQuizCount );
	CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_QUIZ );
	if( pWorld )
	{
		for( DWORD i = 0; i < pWorld->m_dwObjNum; ++i )
		{
			if( IsInQuizEventPlayer( pWorld, nDefaultLayer, i ) )
				GoOut( static_cast<CUser*>( pWorld->m_apObject[i] ) );
		}
	}
}
#endif // __WORLDSERVER

#if defined(__CLIENT) || defined(__WORLDSERVER)
void CQuiz::InitQuizData( int nType )
{
	memset( &m_sQuizData, 0, sizeof( QUIZDATA ) );
	
	if( nType == TYPE_OX )
	{
		m_sQuizData.vQuizZonePos = D3DXVECTOR3( 800.0f, 100.0f, 752.0f );				// 퀴즈존 좌표
		m_sQuizData.vWatchingZonePos = D3DXVECTOR3( 798.747f, 100.0f, 705.266f );		// 대기존 좌표
		m_sQuizData.aExamplePos[0] = D3DXVECTOR3( 783.500f, 100.0f, 752.0f );			// O 좌표
		m_sQuizData.aExamplePos[1] = D3DXVECTOR3( 816.500f, 100.0f, 752.0f );			// X 좌표
		m_sQuizData.aExampleRect[0] = CRect( 768, 736, 799, 768 );						// O 구역
		m_sQuizData.aExampleRect[1] = CRect( 801, 736, 832, 768 );						// X 구역
		m_sQuizData.QuizEventRect = CRect( 768, 736, 832, 768 );						// 퀴즈존 구역
	}
	
	else if( nType == TYPE_4C )
	{
		m_sQuizData.vQuizZonePos = D3DXVECTOR3( 1760.026f, 100.0f, 1856.028f );
		m_sQuizData.vWatchingZonePos = D3DXVECTOR3( 1759.507f, 100.0f, 1790.158f );
		m_sQuizData.aExamplePos[0] = D3DXVECTOR3( 1744.0f, 100.0f, 1872.0f );
		m_sQuizData.aExamplePos[1] = D3DXVECTOR3( 1766.0f, 100.0f, 1872.0f );
		m_sQuizData.aExamplePos[2] = D3DXVECTOR3( 1744.0f, 100.0f, 1840.0f );
		m_sQuizData.aExamplePos[3] = D3DXVECTOR3( 1766.0f, 100.0f, 1840.0f );
		m_sQuizData.aExampleRect[0] = CRect( 1728, 1857, 1759, 1888 );
		m_sQuizData.aExampleRect[1] = CRect( 1761, 1857, 1792, 1888 );
		m_sQuizData.aExampleRect[2] = CRect( 1728, 1824, 1759, 1855 );
		m_sQuizData.aExampleRect[3] = CRect( 1761, 1824, 1792, 1855 );
		m_sQuizData.QuizEventRect = CRect( 1728, 1792, 1824, 1888 );
	}
}

int CQuiz::GetZoneType( CMover* pMover )
{
	D3DXVECTOR3 vPos = pMover->GetPos();
	POINT pt = { (int)vPos.x, (int)vPos.z };
	if( m_sQuizData.QuizEventRect.PtInRect( pt ) )
		return ZONE_QUIZ;

	return ZONE_WATCHING;
}
#endif // defined(__CLIENT) || defined(__WORLDSERVER)
#endif // __QUIZ