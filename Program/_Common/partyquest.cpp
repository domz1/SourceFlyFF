#include "stdafx.h"

#ifdef __WORLDSERVER
#include "partyquest.h"
#include "party.h"
#include "user.h"
#include "dpdatabaseclient.h"
#include "worldmng.h"

extern	CPartyMng	g_PartyMng;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CWorldMng	g_WorldMng;

CPartyQuestProcessor::CPartyQuestProcessor()
{
	m_nRect	= 0;
}

CPartyQuestProcessor::~CPartyQuestProcessor()
{

}

void CPartyQuestProcessor::Process()
{
	DWORD dwTickCount	= GetTickCount();
	for( int i = 0; i < MAX_GUILD_QUEST; i++ )
	{
		PPARTYQUESTELEM pElem = &m_pElem[i];
		if( pElem->nId == i )
		{
			CParty* pParty	= g_PartyMng.GetParty( pElem->idParty );
//			if( !pGuild )
//				continue;

			PARTYQUESTPROP* pProp	= prj.GetPartyQuestProp( pElem->nId );
			if( !pProp )
				continue;

			// 제한 시간 지났다...
			if( pElem->dwEndTime < dwTickCount )
			{
				switch( pElem->nProcess )
				{
					case PQP_WORMON:
						{
							TRACE( "PQP_WORMON - r\n" );

							CRect rect;
							rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );

//							if( pParty )
//							{
//								pParty->ReplaceLodestar( rect );
//							}
/*
							CMover* pWormon	= prj.GetMover( pElem->objidWormon );
							if( pWormon )
								pWormon->Delete();
*/
							CPoint		point;
							D3DXVECTOR3 vPos;
							point  = rect.CenterPoint();
							vPos.x = (float)( point.x );
							vPos.z = (float)( point.y );
							vPos.y = 100.0f;
							RemoveAllDynamicObj( pProp->dwWorldId, vPos, 500 );

							RemovePartyQuest( pElem->nId );
						}
						break;
					case PQP_GETITEM:
						{
							TRACE( "PQP_GETITEM - r\n" );

							CRect rect;
							rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );

//							if( pParty )
//							{
//								pParty->ReplaceLodestar( rect );
//							}
							CPoint		point;
							D3DXVECTOR3 vPos;
							point  = rect.CenterPoint();
							vPos.x = (float)( point.x );
							vPos.z = (float)( point.y );
							vPos.y = 100.0f;
							RemoveAllDynamicObj( pProp->dwWorldId, vPos, 500 );

							RemovePartyQuest( pElem->nId );
						}
						break;
					default:
						break;
				}
			}
			else
			// 아직 열시미 잡고있는중이다~
			{
				switch( pElem->nProcess )
				{
					case PQP_WORMON:
						{
							TRACE( "PQP_WORMON - p\n" );
							
							if( ++pElem->nCount < 10 )
								continue;

							CRect rect;
							rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );
							BOOL bsurvivor	= FALSE;	// is survivor exists?

							if( pParty )
							{
								CUser* pUser;
								
								for( int k = 0 ; k < pParty->GetSizeofMember() ; k++ )
								{
									u_long idPlayer = pParty->GetPlayerId( k );
									pUser = (CUser*)prj.GetUserByID( idPlayer );
									if( IsValidObj( pUser ) )
									{										
										POINT point = { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
										if( rect.PtInRect( point ) && pUser->IsLive() )
										{
											bsurvivor	= TRUE;
											break;
										}
										else
										{
											pUser->AddQuestTextTime( 0, 0, 0xffffffff );											
										}
									}
								}

								for( int k = 0 ; k < pParty->GetSizeofMember() ; k++ )
								{
									u_long idPlayer = pParty->GetPlayerId( k );
									pUser = (CUser*)prj.GetUserByID( idPlayer );
									if( IsValidObj( pUser ) )
									{										
										POINT point = { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
										if( !rect.PtInRect( point ) )
										{
											pUser->AddQuestTextTime( 0, 0, 0xffffffff );											
										}
									}
								}
								
							}

							if( !bsurvivor  )
							{
//								if( pParty )
//								{
//									pParty->ReplaceLodestar( rect );
//								}
/*
								CMover* pWormon	= prj.GetMover( pElem->objidWormon );
								if( pWormon )
									pWormon->Delete();
*/
								CPoint		point;
								D3DXVECTOR3 vPos;
								point  = rect.CenterPoint();
								vPos.x = (float)( point.x );
								vPos.z = (float)( point.y );
								vPos.y = 100.0f;
								RemoveAllDynamicObj( pProp->dwWorldId, vPos, 500 );
								
								RemovePartyQuest( pElem->nId );
							}
						}
						break;
					case PQP_GETITEM:
						{
							TRACE( "PQP_GETITEM - p\n" );
							CRect rect;
							rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );
							BOOL bexist	= FALSE;	// is member here?

							if( pParty )
							{
								CUser* pUser;
								
								for( int k = 0 ; k < pParty->GetSizeofMember() ; k++ )
								{
									u_long idPlayer = pParty->GetPlayerId( k );
									pUser = (CUser*)prj.GetUserByID( idPlayer );
									if( IsValidObj( pUser ) )
									{
										POINT point = { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
										if( rect.PtInRect( point ) )
										{
											bexist	= TRUE;
											break;
										}
									}
								}

								for( int k = 0 ; k < pParty->GetSizeofMember() ; k++ )
								{
									u_long idPlayer = pParty->GetPlayerId( k );
									pUser = (CUser*)prj.GetUserByID( idPlayer );
									if( IsValidObj( pUser ) )
									{
										POINT point = { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
										if( !rect.PtInRect( point ) )
										{
											pUser->AddQuestTextTime( 0, 0, 0xffffffff );											
										}
									}
								}
							}

							if( !bexist )
							{
//								if( pParty )
//								{
//									pParty->ReplaceLodestar( rect );
//								}
								CPoint		point;
								D3DXVECTOR3 vPos;
								point  = rect.CenterPoint();
								vPos.x = (float)( point.x );
								vPos.z = (float)( point.y );
								vPos.y = 100.0f;
								RemoveAllDynamicObj( pProp->dwWorldId, vPos, 500 );

								RemovePartyQuest( pElem->nId );
							}
						}
						break;
					default:
						break;
				}
			}
		}
	}
}

void CPartyQuestProcessor::RemoveAllDynamicObj( DWORD dwWorldID, D3DXVECTOR3 vPos, int nRange  )
{
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );

	if( !pWorld )
		return;

	CObj*		pObj;
	D3DXVECTOR3 vDist;

	FOR_LINKMAP( pWorld, vPos, pObj, nRange, CObj::linkDynamic, nTempLayer )
	{
		if( pObj )
		{
			if( pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPeaceful() == FALSE )
			{
				pObj->Delete();
			}
		}
	}
	END_LINKMAP

	D3DXVECTOR3 vPos2	= D3DXVECTOR3( 6968.0f, 0, 3328.8f );
	FOR_LINKMAP( pWorld, vPos, pObj, nRange, CObj::linkPlayer, nTempLayer )
	{
		if( pObj )
		{
			CUser* pUser = (CUser*)pObj;
			pUser->AddQuestTextTime( 0, 0, 0xffffffff );			
			( (CUser*)pObj )->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, vPos2, REPLACE_NORMAL, nDefaultLayer );
		}
	}
	END_LINKMAP
}


void CPartyQuestProcessor::SetPartyQuest( int nQuestId, int nState, int ns, int nf, u_long idParty, OBJID objidWormon )
{
	if( nQuestId >= MAX_PARTY_QUEST )
	{
		Error( "" );
		return;
	}
	PARTYQUESTPROP* pProp	= prj.GetPartyQuestProp( nQuestId );
	if( !pProp )
	{
		Error( "" );
		return;
	}

	TRACE( "SET_PARTY_QUEST, %d, %d, %d\n", nQuestId, idParty, objidWormon );
	PPARTYQUESTELEM	pElem	= &m_pElem[nQuestId];
	pElem->nId	= nQuestId;
	pElem->nState	= nState;
	pElem->idParty	= idParty;
	pElem->dwEndTime	= GetTickCount() + MIN( 60 );
	
	pElem->nProcess		= PQP_WORMON;
	pElem->ns	= ns;
	pElem->nf	= nf;
	pElem->objidWormon	= objidWormon;
	pElem->nCount	= 0;

	// 유저에게 공략시간을 넘겨준다...
	SendQuestLimitTime( PQP_WORMON, MIN( 60 ), idParty );

	// 파티퀘스트시작시 정보를 파일에 기록한다.
	CString strFileName = "StartPartyQuestInfo.txt";
	CString strQuestInfo;
	CString strMemberName;
	CString strDateTime;
	
	CTime time = CTime::GetCurrentTime();
	strDateTime.Format( "DATE : %d%.2d%.2d\nTIME : %d::%d::%d\n", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond() );
	strQuestInfo.Format( "QUEST ID = %d, PARTY ID = %d, WORMON ID = %d\n", nQuestId, idParty, objidWormon );
	
	CParty* pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		for( int i = 0 ; i < pParty->GetSizeofMember() ; ++i )
		{
			CUser* pUsertmp = (CUser *)prj.GetUserByID( pParty->GetPlayerId( i ) );
			if( IsValidObj( pUsertmp ) )
			{
				strMemberName += pUsertmp->GetName();
				strMemberName += "\n";
			}
		}
	}

	FILEOUT( strFileName, "%s%s%s\n", strDateTime, strQuestInfo, strMemberName );
	////////////////////////////////////////////////////////////////////////////////////////////
}

void CPartyQuestProcessor::SendQuestLimitTime( int nState, DWORD dwTime, u_long idParty )
{
	CParty* pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		for( int i = 0 ; i < pParty->GetSizeofMember() ; ++i )
		{
			CUser* pUsertmp = (CUser *)prj.GetUserByID( pParty->GetPlayerId( i ) );
			if( IsValidObj( pUsertmp ) )
			{
				pUsertmp->AddQuestTextTime( 1, nState, dwTime );
			}
		}
	}
}

PPARTYQUESTELEM CPartyQuestProcessor::GetPartyQuest( int nQuestId )
{
	if( nQuestId >= MAX_PARTY_QUEST )
	{
		Error( "" );
		return NULL;
	}
	if( m_pElem[nQuestId].nId == -1 )
		return NULL;

	return &m_pElem[nQuestId];
}


PPARTYQUESTRECT CPartyQuestProcessor::GetPartyQuestRect( int nQuestId )
{
	if( nQuestId >= MAX_PARTY_QUEST )
		return NULL;
	if( m_pElem[nQuestId].nId == -1 )
		return NULL;
	for( int i = 0; i < m_nRect; i++ )
	{
		if( m_pRect[i].nId == nQuestId )
			return &m_pRect[i];
	}
	return NULL;
//	return &m_pRect[nQuestId];
}


void CPartyQuestProcessor::RemovePartyQuest( int nQuestId )
{
	if( nQuestId >= MAX_PARTY_QUEST )
	{
		Error( "" );
		return;
	}

	TRACE( "REMOVE_PARTY_QUEST, %d\n", nQuestId );

	PPARTYQUESTELEM pElem	= &m_pElem[nQuestId];
	pElem->nId		= -1;
	pElem->nState	= 0;
	pElem->idParty		= 0;
	pElem->nProcess	= PQP_READY;
	pElem->dwEndTime	= 0;
	pElem->nCount	= 0;
}

BOOL CPartyQuestProcessor::IsQuesting( int nQuestId )
{
	if( nQuestId >= MAX_PARTY_QUEST )
	{
		Error( "" );
		return FALSE;
	}

	if( m_pElem[nQuestId].nId != -1 )
		return TRUE;
	return FALSE;
}

CPartyQuestProcessor* CPartyQuestProcessor::GetInstance( void )
{
	static CPartyQuestProcessor sPartyQuestProcessor;
	return &sPartyQuestProcessor;
}

void CPartyQuestProcessor::AddQuestRect( int nId, DWORD dwWorldId, int x1, int y1, int x2, int y2 )
{
	if( m_nRect >= MAX_PARTY_QUEST )
		return;

	m_pRect[m_nRect].nId	= nId;
	m_pRect[m_nRect].dwWorldId	= dwWorldId;
	m_pRect[m_nRect].rect.SetRect( x1, y2, x2, y1 );
	m_nRect++;
}

int CPartyQuestProcessor::PtInQuestRect( DWORD dwWorldId, const D3DXVECTOR3 & vPos )
{
	POINT point	= { (int)vPos.x, (int)vPos.z	};
	for( int i = 0; i < m_nRect; i++ )
	{
		
		if( m_pRect[i].dwWorldId == dwWorldId && m_pRect[i].rect.PtInRect( point ) )
			return m_pRect[i].nId;
	}
	return -1;
}


#endif	// __WORLDSERVER
