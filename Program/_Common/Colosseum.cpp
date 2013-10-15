#include "stdafx.h"
#include "Colosseum.h"

#ifdef __COLOSSEUM

#include "defineText.h"
#include "User.h"
#include "worldmng.h"

extern CWorldMng	g_WorldMng;
extern CUserMng		g_UserMng;
extern	CPartyMng	g_PartyMng;


CColosseumMng::CColosseumMng()
{
}

CColosseumMng::~CColosseumMng()
{
}

CColosseumMng* CColosseumMng::GetInstance( void )
{
	static CColosseumMng sColosseum;
	return &sColosseum;
}

BOOL CColosseumMng::CreateNewMatch( CUser* pUser, BYTE byDifficult, BYTE byMenu )
{
		CParty* pParty = g_PartyMng.GetParty( pUser->GetPartyId() );
		if( pParty && IsValidObj( pUser ) )
		{
			if( GetMatchByParty( pParty ) != 0 ) return FALSE;
			if( !pUser->GetGuild() && byMenu == 2 ) return FALSE;
			if( byDifficult != 1 && byDifficult != 2 ) byDifficult = 1;
			if( byMenu != 1 && byMenu != 2 ) byMenu = 1;
			if( byDifficult == 1 && pUser->m_nLevel > 90 ) return FALSE;

			CColosseumMatch* pMatch = new CColosseumMatch;
			pMatch->m_uPartyID = pParty->m_uPartyId;
			pMatch->m_byDifficult = byDifficult;
			pMatch->m_byMenu = byMenu;
			if( byMenu == 2 )
				pMatch->m_uGuildID = pUser->m_idGuild;

			if( byMenu == 1 )
				pMatch->m_byBossStage = xRandom( 1, byMaxLowStage );
			else
				pMatch->m_byBossStage = xRandom( 1, byMaxHardStage );

			pMatch->CreateRoomLayer();

			for( int i = 1; i < pParty->m_nSizeofMember; i++ )
			{
				CUser* pPartyUser = g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
				if( pPartyUser )
					if( pMatch->m_byMenu == 2 )
					{
						if( pPartyUser->m_idGuild == pMatch->m_uGuildID )
							pPartyUser->SendColosseumEnter();
					}
					else
						pPartyUser->SendColosseumEnter();
			}

			m_mapMatch.insert( make_pair( pParty->m_uPartyId, pMatch ));
			return TRUE;
		}
		return FALSE;
}

BOOL CColosseumMng::LoadScript()
{
	if( m_Lua.RunScript( "Colosseum.lua" ) != 0 )
	{
		Error( "Colosseum.lua Load Failed!!!" );
		exit(0);
	}

	nStartWaitTime  = static_cast<int>( m_Lua.GetGlobalNumber( "StartWaitTime" ) );
	nWaitAfterStage = static_cast<int>( m_Lua.GetGlobalNumber( "WaitAfterStage" ) );
	byMaxLowStage	= static_cast<BYTE>( m_Lua.GetGlobalNumber( "LowStages" ) );
	byMaxHardStage	= static_cast<BYTE>( m_Lua.GetGlobalNumber( "HardStages" ) );

	m_Lua.GetGloabal( "tMonster" );
	m_Lua.PushNil();


	while( m_Lua.TableLoop( -2 ) )
	{
		__COLOSSEUM_MONSTER srMonster;
		srMonster.dwFakeId = static_cast<int>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strFakeMonsterId" ) ) );
		srMonster.dwTrueId = static_cast<int>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strTrueMonsterId" ) ) );
		srMonster.nTime = static_cast<int>( m_Lua.GetFieldToNumber( -1, "nTime" ) );
		srMonster.nStage = static_cast<int>( m_Lua.GetFieldToNumber( -1, "nStage" ) );
		srMonster.nDifficult = static_cast<int>( m_Lua.GetFieldToNumber( -1, "nType" ) );
		srMonster.nProb = static_cast<int>( m_Lua.GetFieldToNumber( -1, "nProb" ) );
		m_vecMonster.push_back( srMonster );
		m_Lua.Pop(1);
	}


	m_Lua.Pop(0);


	return TRUE;
}

void CColosseumMng::DestroyMatch( u_long m_uPartyID )
{
	COLOSSEUMMATCH::iterator it=m_mapMatch.find( m_uPartyID );
	if( it == m_mapMatch.end() )
	{
		return;
	}
		
	safe_delete ( it->second );
	m_mapMatch.erase( it );
}

void CColosseumMng::Process()
{
	for( COLOSSEUMMATCH::iterator it = m_mapMatch.begin(); it != m_mapMatch.end(); it++ )
	{
		u_long idParty = it->first;
		CColosseumMatch* pMatch = it->second;

		if( !pMatch ) return;

		CMover* pMover = prj.GetMover(pMatch->m_nMoverID);
		if( pMover )
		{
			if( pMatch->m_nNextStage < (int)g_tmCurrent )
				pMatch->EndMatch( FALSE );
		}
		else
		{
			//Kontrollieren ob letze Stage erreicht wurde
			if( pMatch->m_nNextStage < (int)g_tmCurrent )
			{
				if( pMatch->m_nStage == 0 )
				{
					for( int i = 0; i < pMatch->m_vecPlayerID.size(); i++ )
					{
						CUser* pUser = pMatch->GetUser( pMatch->m_vecPlayerID.at(i) );
						if( IsValidObj( pUser ) && pMatch->m_bWaitTime )
							pUser->SendColosseumStart(0);
					}
				}
				if( pMatch->m_nStage >= byMaxLowStage )
					pMatch->EndMatch( TRUE );
				else
				{
					pMatch->m_bWaitTime = FALSE;
					pMatch->m_nStage++;
					pMatch->CreateMover();
				}
			}
		}
	}
}

CColosseumMatch* CColosseumMng::GetMatchByParty( CParty* pParty)
{
	for( COLOSSEUMMATCH::iterator it = m_mapMatch.begin(); it != m_mapMatch.end(); it++ )
		if( pParty->m_uPartyId == it->first )
		{
			return it->second;
		}


	return NULL;
}

CColosseumMatch* CColosseumMng::GetMatchByMoverID( DWORD dwMoverID )
{
	for( COLOSSEUMMATCH::iterator it = m_mapMatch.begin(); it != m_mapMatch.end(); it++ )
	{
		CColosseumMatch* cMatch = it->second;
		if( cMatch && cMatch->m_nMoverID == dwMoverID )
			return cMatch;
	}

	return NULL;
}

CColosseumMatch::CColosseumMatch()
{
	m_bWaitTime	 = TRUE;
	m_nMoverID	 = NULL;
	m_nStartTime = g_tmCurrent;
	m_nStage	 = 0;
	m_nNextStage = (int)g_tmCurrent + CColosseumMng::GetInstance()->nStartWaitTime;
}

CColosseumMatch::~CColosseumMatch()
{
	CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_COLOSSEUM );
	if( pWorld && pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_uPartyID ) ) )
		pWorld->Invalidate( m_uPartyID );
}

void CColosseumMatch::Process()
{
}
void CColosseumMatch::CreateMover()
{
	int nMoverID = 0;
	int nTime = 0;

	vector<__COLOSSEUM_MONSTER>	m_vecMobInfo = CColosseumMng::GetInstance()->m_vecMonster;
	for( unsigned int i = 0; i < m_vecMobInfo.size(); i++ )
	{
		if( m_vecMobInfo.at(i).nDifficult == m_byDifficult && m_vecMobInfo.at(i).nStage == m_nStage )
		{
			int nRnd = xRandom(1, 100);

			if( nRnd > 100 - m_vecMobInfo.at(i).nProb )
				nMoverID = m_vecMobInfo.at(i).dwTrueId;
			else
				nMoverID = m_vecMobInfo.at(i).dwFakeId;

			nTime = m_vecMobInfo.at(i).nTime;
			m_nNextStage = g_tmCurrent + nTime;
		}
	}

	if( nMoverID != 0 )
	{
		MoverProp* pMoverProp = prj.GetMoverPropEx( nMoverID );
		CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_COLOSSEUM );

		if( pWorld && pMoverProp && pMoverProp->dwID != 0 )
		{
			D3DXVECTOR3 vPos( 1291.0F, 100.0F, 1231.0F );
			CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
			pObj->SetPos( vPos );
			pObj->InitMotion( MTI_STAND );
			pObj->UpdateLocalMatrix();
			((CMover*)pObj)->m_bActiveAttack = TRUE;
			((CMover*)pObj)->AddItToGlobalId();
			pWorld->ADDOBJ( pObj, FALSE, m_uPartyID );

			m_nMoverID = ((CMover*)pObj)->GetId();
		}
	    if( pWorld->GetID() == WI_WORLD_MADRIGAL )
	    {
		CColosseumMng::GetInstance()->DestroyMatch( m_uPartyID );
	    }
	}

	for( unsigned int i = 0; i < m_vecPlayerID.size(); i++ )
	{
		CUser* pUser = g_UserMng.GetUserByPlayerID( m_vecPlayerID.at(i) );
		if( IsValidObj( pUser ) )
			pUser->SendColosseumReady2( nTime );
	}
}

void CColosseumMatch::CreateRoomLayer()
{
	CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_COLOSSEUM );
	if( pWorld && !pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_uPartyID ) ) )
		pWorld->CreateLayer( static_cast<int>( m_uPartyID ) );
}

CUser* CColosseumMatch::GetUser( u_long uPlayerID )
{
	return g_UserMng.GetUserByPlayerID( uPlayerID );
}

void CColosseumMatch::EnterRoom( CUser* pUser )
{
	if( IsValidObj( pUser ) && m_bWaitTime && !IsTeleported( pUser->m_idPlayer ) )
	{
		m_vecPlayerID.push_back( pUser->m_idPlayer );
		CWorld* pWorld = g_WorldMng.GetWorld( WI_WORLD_COLOSSEUM );
		if( pWorld )
			if( pWorld->m_linkMap.GetLinkMap( static_cast<int>( m_uPartyID ) ) )
				{
					pUser->SendColosseumStart( (int)(m_nNextStage) );
					pUser->REPLACE( g_uIdofMulti, WI_WORLD_COLOSSEUM, D3DXVECTOR3( 1446.0F, 100.0F, 1219.0F ), REPLACE_NORMAL, static_cast<int>( m_uPartyID ) );
				}
	}
}

void CColosseumMatch::KickUser( u_long m_uPlayerID )
{
	BOOL bWin;
	for( int i = 0; i < m_vecPlayerID.size(); i++ )
	{
		CUser* pUser = GetUser( m_vecPlayerID.at(i) );
		if( IsValidObj( pUser ) )
		{
			pUser->SendColosseumEndMatch( bWin, 0 );
			pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6971.984f, 99.8f, 3336.884f ), REPLACE_NORMAL, nDefaultLayer );
		}
	}
	CColosseumMng::GetInstance()->DestroyMatch( m_uPartyID );
}
void CColosseumMatch::EndWaitTime()
{

	if( m_bWaitTime )
	{
		m_nNextStage = (int)g_tmCurrent + (int)CColosseumMng::GetInstance()->nWaitAfterStage;
		m_bWaitTime	 = FALSE;
	}

	for( int i = 0; i < m_vecPlayerID.size(); i++ )
	{

		CUser* pUser = GetUser( m_vecPlayerID.at(i) );
		if( IsValidObj( pUser ) )
		{
			pUser->SendColosseumStart(0);
			pUser->SendColosseumReady1( m_nNextStage, m_nStage + 1 );
		}
	}
}

void CColosseumMatch::DestroyMover()
{
//CMover* g_pPlayer;
CWorld* pWorld;/// = g_pPlayer->GetWorld();
if( pWorld->GetID() == WI_WORLD_COLOSSEUM )
{
	m_nMoverID = NULL;
	m_bWaitTime  = FALSE;

	m_nNextStage = (int)g_tmCurrent + CColosseumMng::GetInstance()->nWaitAfterStage;

	TCHAR strInt[2];
	itoa( m_nStage, strInt, 10 );
	for( int i = 0; i < m_vecPlayerID.size(); i++ )
	{
		CUser* pUser = GetUser( m_vecPlayerID.at(i) );
		if( IsValidObj( pUser ) )
		{
				CString strTemp;
				strTemp.Format(_T( prj.GetTextColor(TID_COLOSSEUM_COLOSUCCESS01) ), strInt );//.GetText
				pUser->AddText( strTemp );
				

			if( m_nStage < CColosseumMng::GetInstance()->byMaxHardStage ) 
			pUser->SendColosseumReady1( m_nNextStage, m_nStage + 1 );
		}

	}
}
else 
   {
   }
}

void CColosseumMatch::EndMatch( BOOL bWin )
{
	for( int i = 0; i < m_vecPlayerID.size(); i++ )
	{
		CUser* pUser = GetUser( m_vecPlayerID.at(i) );
		if( IsValidObj( pUser ) )
		{
			pUser->SendColosseumEndMatch( bWin, ( g_tmCurrent - m_nStartTime ) );
	
			pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6971.984f, 99.8f, 3336.884f ), REPLACE_NORMAL, nDefaultLayer );
		}
	}
	CColosseumMng::GetInstance()->DestroyMatch( m_uPartyID );
}

BOOL CColosseumMatch::IsTeleported( u_long m_uPlayerID )
{
		for( int i = 0; i < m_vecPlayerID.size(); i++ )
			if( m_vecPlayerID.at(i) == m_uPlayerID )
				return TRUE;
		return FALSE;
}
#endif //__COLOSSEUM