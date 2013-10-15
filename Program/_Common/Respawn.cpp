#include "stdafx.h"
#include "Respawn.h"

#include "CommonCtrl.h"
#include "defineObj.h"

///////////////////////////////////////////////////////////////////////////////////////////
// CRespawnInfo
///////////////////////////////////////////////////////////////////////////////////////////

CRespawnInfo::CRespawnInfo()
{
	m_dwIndex	= 0; 
	m_dwType	= 0;
	m_cb		= 0; // 10
	m_uTime	= m_cbTime = 0; // 60
	memset( &m_rect, 0, sizeof(m_rect) );
	m_vPos	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_nActiveAttackNum = 0;
	m_nDayMin = 1;
	m_nDayMax = 30;
	m_nHourMin = 1;
	m_nHourMax = 24;
	m_nItemMin = 1;
	m_nItemMax = 1;
	m_fY = 0;
	ZeroMemory( &m_CtrlElem, sizeof( m_CtrlElem ) );
	m_CtrlElem.m_dwMinItemNum = 1;
	m_CtrlElem.m_dwMaxiItemNum = 1;

#ifdef __S1108_BACK_END_SYSTEM
	m_nMaxcb = 0;
	m_nMaxAttackNum = 0;
	m_nGMIndex = 0;
#endif // __S1108_BACK_END_SYSTEM
	m_bRemove = FALSE;
	
	m_dwAiState = 2;
	m_fAngle    = 0.0f;
	
	m_dwPatrolIndex = NULL_ID;
	m_bPatrolCycle  = 0;

#ifdef __RES0807
	memset( m_aResPoint, 0, sizeof(POINT) * MAX_RESPOINT_PER_REGION );
#endif	// __RES0807

	m_bHalf	= FALSE;
	m_cbRespawn	= 0;
}

CRespawnInfo::CRespawnInfo( const CRespawnInfo & ri )
{
	m_dwIndex	= ri.m_dwIndex;	
	m_dwType	= ri.m_dwType;	
	m_uTime	    = ri.m_uTime;		
	m_cb		= ri.m_cb; 	
	m_cbTime	= ri.m_cbTime;	
	m_rect	    = ri.m_rect;
	m_vPos	    = ri.m_vPos;
	m_fY  	   = ri.m_fY;
	m_nDayMin  = ri.m_nDayMin ;
	m_nDayMax  = ri.m_nDayMax ;
	m_nHourMin = ri.m_nHourMin;
	m_nHourMax = ri.m_nHourMax;
	m_nItemMin = ri.m_nItemMin;
	m_nItemMax = ri.m_nItemMax;
	m_nActiveAttackNum = ri.m_nActiveAttackNum;
	memcpy( &m_CtrlElem, &ri.m_CtrlElem, sizeof( m_CtrlElem ) );
	m_cbRespawn	= (u_short)( m_cb );
//	m_uTime		/= 2;	
	m_bHalf		= FALSE;
#ifdef __S1108_BACK_END_SYSTEM
	m_nMaxcb = ri.m_nMaxcb;
	m_nMaxAttackNum = ri.m_nMaxAttackNum;
	m_nGMIndex = ri.m_nGMIndex;
#endif // __S1108_BACK_END_SYSTEM
	m_bRemove = ri.m_bRemove;
	m_dwAiState = ri.m_dwAiState;
	m_fAngle    = ri.m_fAngle;	
	m_dwPatrolIndex = ri.m_dwPatrolIndex;
	m_bPatrolCycle  = ri.m_bPatrolCycle;
#ifdef __RES0807
	memcpy( m_aResPoint, ri.m_aResPoint, sizeof(POINT) * MAX_RESPOINT_PER_REGION );
#endif	// __RES0807
}

CRespawnInfo::~CRespawnInfo()
{
}

#ifdef __RES0807
#define	MAX_FAIL_TO_RESPAWN	10000
BOOL CRespawnInfo::GenResPoint( CWorld* pWorld )
{
	int nCheckAttr	= HATTR_NOWALK;
	if( m_dwType == OT_MOVER && prj.GetMoverProp( m_dwIndex )->dwFlying )
		nCheckAttr	= HATTR_NOFLY;
	D3DXVECTOR3 v;
	int nSize	= 0;
	int nFail	= 0;
	while( 1 )
	{
		GetPos( v, FALSE );
		int nAttr	= pWorld->GetHeightAttribute( v.x, v.z );
		if( nAttr == -1 || nAttr == HATTR_NOMOVE || nAttr == nCheckAttr ) 
		{
			if( ++nFail > MAX_FAIL_TO_RESPAWN )
				return FALSE;
			continue;
		}
		m_aResPoint[nSize].x	= (LONG)( v.x );
		m_aResPoint[nSize++].y	= (LONG)( v.z );

		if( nSize >= MAX_RESPOINT_PER_REGION )
			break;
	}
//	TRACE( "nFail=%d\n", nFail );
	return TRUE;
}
#endif	// __RES0807

void CRespawnInfo::GetPos( D3DXVECTOR3 & v, BOOL bRespawn )
{
	if( m_nMaxcb == 1 )
	{
		if( m_dwAiState == 8 || m_dwAiState == 9 )
		{
			v = m_vPos;
			return;
		}
	}

#ifdef __RES0807
	if( bRespawn )
	{
		int r	= xRandom( 0, MAX_RESPOINT_PER_REGION );
		v.x		= (float)( m_aResPoint[r].x );
		v.z		= (float)( m_aResPoint[r].y );
		v.y		= m_vPos.y;
	}
	else
	{
		int nWidth = m_rect.right - m_rect.left;
		if( nWidth <= 0 ) nWidth = 1;
		int nHeight = m_rect.bottom - m_rect.top;
		if( nHeight <= 0 ) nHeight = 1;
		v.x		= (float)( m_rect.left + xRandom( nWidth ) );
		v.z		= (float)( m_rect.top + xRandom( nHeight ) );
		v.y		= m_vPos.y;
	}
#else	// __RES0807
	int nWidth = m_rect.right - m_rect.left;
	if( nWidth <= 0 ) nWidth = 1;
	int nHeight = m_rect.bottom - m_rect.top;
	if( nHeight <= 0 ) nHeight = 1;
	v.x		= (float)( m_rect.left + xRandom( nWidth ) );
	v.z		= (float)( m_rect.top + xRandom( nHeight ) );
	v.y		= m_vPos.y;
#endif	// __RES0807
}

void CRespawnInfo::Increment( BOOL bActiveAttack )	
{ 
#ifdef __S1108_BACK_END_SYSTEM
	InterlockedDecrement( &m_cb ); 
	if( bActiveAttack ) 
		InterlockedDecrement( &m_nActiveAttackNum );	
#else // __S1108_BACK_END_SYSTEM
	InterlockedIncrement( &m_cb ); 
	if( bActiveAttack ) 
		InterlockedIncrement( &m_nActiveAttackNum );	
#endif // __S1108_BACK_END_SYSTEM
}

CRespawnInfo& CRespawnInfo::operator=( const CRespawnInfo & ri )
{
	m_dwIndex	= ri.m_dwIndex;	
	m_dwType	= ri.m_dwType;	
	m_uTime	    = ri.m_uTime;		
	m_cb		= ri.m_cb; 	
	m_cbTime	= ri.m_cbTime;	
	m_rect	    = ri.m_rect;
	m_vPos	    = ri.m_vPos;
	m_fY  	   = ri.m_fY;
	m_nDayMin  = ri.m_nDayMin ;
	m_nDayMax  = ri.m_nDayMax ;
	m_nHourMin = ri.m_nHourMin;
	m_nHourMax = ri.m_nHourMax;
	m_nItemMin = ri.m_nItemMin;
	m_nItemMax = ri.m_nItemMax;
	m_nActiveAttackNum = ri.m_nActiveAttackNum;
	memcpy( &m_CtrlElem, &ri.m_CtrlElem, sizeof( m_CtrlElem ) );

	m_cbRespawn	= (u_short)( m_cb );
//	m_uTime		/= 2;	
	m_bHalf		= FALSE;

#ifdef __S1108_BACK_END_SYSTEM
	m_nMaxcb = ri.m_nMaxcb;
	m_nMaxAttackNum = ri.m_nMaxAttackNum;
	m_nGMIndex = ri.m_nGMIndex;
#endif // __S1108_BACK_END_SYSTEM
	m_bRemove = ri.m_bRemove;

	m_dwAiState = ri.m_dwAiState;
	m_fAngle    = ri.m_fAngle;	

	m_dwPatrolIndex = ri.m_dwPatrolIndex;
	m_bPatrolCycle  = ri.m_bPatrolCycle;

#ifdef __RES0807
	memcpy( m_aResPoint, ri.m_aResPoint, sizeof(POINT) * MAX_RESPOINT_PER_REGION );
#endif	// __RES0807
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////
// CRespawner
///////////////////////////////////////////////////////////////////////////////////////////
CRespawner::CRespawner()
{
}

#ifdef __LAYER_1021
CRespawner::CRespawner( const CRespawner & respawner )
{
	for( int i = 0; i < 3; ++i )
		m_vRespawnInfo[i].assign( respawner.m_vRespawnInfo[i].begin(), respawner.m_vRespawnInfo[i].end() );
}
#endif	// __LAYER_1021

CRespawner::~CRespawner()
{
}

int CRespawner::Add( CRespawnInfo & ri, int nType )
{	
	if( nType == RESPAWNTYPE_REGION )
	{
		ri.m_uTime	/= 2;
		ri.m_nGMIndex	= m_vRespawnInfo[nType].size();
		m_vRespawnInfo[nType].push_back( ri );
	}
	else
	if( nType == RESPAWNTYPE_SCRIPT )
	{
		for( VRI::iterator i = m_vRespawnInfo[nType].begin(); i != m_vRespawnInfo[nType].end(); ++i )
		{
			 if( ( * i ).m_nGMIndex == ri.m_nGMIndex )
			 {
				Error( "CRespawner::Add 같은 ID 발견 : %d, %d, %f, %f, %d\n", ri.m_dwIndex, ri.m_dwType, ri.m_vPos.x, ri.m_vPos.z, nType );
				return -1;
			 }
		}
		m_vRespawnInfo[nType].push_back( ri );
	}
	return m_vRespawnInfo[nType].size() - 1;
}

// 실제 삭제 
BOOL CRespawner::DoRemove( int nRespawnNo, int nType )
{
	if( nRespawnNo < 0 )	
		return NULL;
	BOOL bResult = FALSE;
	if( nType == RESPAWNTYPE_REGION )
	{
		Error( "CRespawner::Remove에서 RESPAWN_REGION 타입은 삭제할 수 없음 : %d\n", nRespawnNo );
		return FALSE;
	}
	else
	if( nType == RESPAWNTYPE_SCRIPT )
	{
		for( VRI::iterator i = m_vRespawnInfo[nType].begin(); i != m_vRespawnInfo[nType].end(); ++i )
		{
			if( ( *i ).m_nGMIndex == nRespawnNo )
			{
				m_vRespawnInfo[nType].erase( i );
				bResult = TRUE;
				break;
			}
		}
	}
	else
	if( nType == RESPAWNTYPE_BACKEND )
	{
		return TRUE;
	}
	return bResult;
}
// 여기서는 Remove 상태만 세팅 
BOOL CRespawner::Remove( int nRespawnNo, int nType )
{
	if( nRespawnNo < 0 )	
		return NULL;
	if( nType == RESPAWNTYPE_REGION )
	{
		Error( "CRespawner::Remove에서 RESPAWN_REGION 타입은 삭제할 수 없음 : %d\n", nRespawnNo );
		return FALSE;
	}
	else
	if( nType == RESPAWNTYPE_SCRIPT )
	{
		for( VRI::iterator i = m_vRespawnInfo[nType].begin(); i != m_vRespawnInfo[nType].end(); ++i )
		{
			if( ( *i ).m_nGMIndex == nRespawnNo )
			{
				( *i ).m_bRemove	= TRUE;
				return TRUE;
			}
		}
	}
	else
	if( nType == RESPAWNTYPE_BACKEND )
	{
		return TRUE;
	}
	return FALSE;
}
CRespawnInfo* CRespawner::GetRespawnInfo( int nRespawnNo, int nType )
{
	if( nRespawnNo < 0 )	
		return NULL;
	if( nType == RESPAWNTYPE_REGION )
	{
		if( nRespawnNo < (int)( m_vRespawnInfo[nType].size() ) )
			return &m_vRespawnInfo[nType][nRespawnNo];
	}
	else
	if( nType == RESPAWNTYPE_SCRIPT )
	{
		for( VRI::iterator i = m_vRespawnInfo[nType].begin(); i != m_vRespawnInfo[nType].end(); ++i )
		{
			if( ( *i ).m_nGMIndex == nRespawnNo )
				return &( *i );
		}
	}
	else
	if( nType == RESPAWNTYPE_BACKEND )
	{
	}
	return NULL;
}
void CRespawner::Increment( int nRespawnNo, int nType, BOOL bActiveAttack )	
{
	if( nRespawnNo < 0 )	
		return;
	if( nType == RESPAWNTYPE_REGION )
	{
		if( nRespawnNo < (int)( m_vRespawnInfo[nType].size() ) )
			m_vRespawnInfo[nType][nRespawnNo].Increment( bActiveAttack );
	}
	else
	if( nType == RESPAWNTYPE_SCRIPT )
	{
		for( VRI::iterator i = m_vRespawnInfo[nType].begin(); i != m_vRespawnInfo[nType].end(); ++i )
		{
			if( ( *i ).m_nGMIndex == nRespawnNo )
			{
				( *i ).Increment( bActiveAttack );
				break;
			}
		}
	}
	else
	if( nType == RESPAWNTYPE_BACKEND )
	{
	}
}

#ifdef __LAYER_1021
u_long CRespawner::Spawn( CWorld* pWorld, int nLayer )
#else	// __LAYER_1021
u_long CRespawner::Spawn( CWorld* pWorld )
#endif	// __LAYER_1021
{
#if defined(__CLIENT)
	return 0;
#else
	if( g_eLocal.GetState( EVE_RESPAWN ) == 0 )
		return 0;

	ASSERT( pWorld );

	CRespawnInfo*	pi;
	short			cb;
	CObj*			pObj;
	D3DXVECTOR3		v;
	u_long			uRespawned	= 0;
#ifndef __RES0807
	int				nAttr, nCheckAttr;
	int				nRetry;
#endif	// __RES0807
	MoverProp*		pMoverProp;
	DWORD			dwFlying;

	for( int nType = 0; nType < 3; nType++ )
	{
		int nSize	= m_vRespawnInfo[nType].size();
		for( int i = 0; i < nSize; i++ )			// 04.10.11 - 480개 정도 이다.
		{
			pi	= &m_vRespawnInfo[nType][i];

			if( pi->m_dwIndex == 0 )
				continue;

			// 삭제 명령이 내려졌고, 리스폰된 오브젝트가 없다면?
			if( pi->m_bRemove )
			{
				if( pi->m_cb == 0 )
				{
					if( DoRemove( pi->m_nGMIndex, nType ) )
					{
						i--; 
						nSize--;
						continue;
					}
				}
			}

			int nDay  = g_GameTimer.m_nDay ;
			int nHour = g_GameTimer.m_nHour;

			if( pi->m_nDayMin < pi->m_nDayMax )
			{
				if( nDay < pi->m_nDayMin || nDay > pi->m_nDayMax )
					continue;
			}
			else
			{
				if( nDay < pi->m_nDayMin && nDay > pi->m_nDayMax )
					continue;
			}
			if( pi->m_nHourMin < pi->m_nHourMax )
			{
				if( nHour < pi->m_nHourMin || nHour > pi->m_nHourMax )
					continue;
			}
			else
			{
				if( nHour < pi->m_nHourMin && nHour > pi->m_nHourMax )
					continue;
			}

			pi->m_cbTime--;									// 리스폰 타이머 시간 감소 
			if( pi->m_cbTime < 0 )							// 0 이면 리스폰 시작
			{
				short nTime = (short)( ( pi->m_uTime * xRandom( 50, 150 ) ) / 100 );
			#ifdef __S1108_BACK_END_SYSTEM
				cb = 0;
				if( pi->m_nGMIndex != 0 )
				{
					pi->m_cbTime = nTime;					// 타이머 reset	
					if( pi->m_nMaxcb - pi->m_cb > 0 )
						cb	= (short)( pi->m_nMaxcb - pi->m_cb );		// cb = 죽은 갯수
				}
				else
				{
					if( pWorld->GetID() == WI_WORLD_MADRIGAL )
					{
						pi->m_cbTime = (short)(nTime * prj.m_fMonsterRebirthRate);			// 타이머 reset
						if( (pi->m_nMaxcb * prj.m_fMonsterRespawnRate) - pi->m_cb > 0 )
							cb	= (short)( (pi->m_nMaxcb * prj.m_fMonsterRespawnRate) - pi->m_cb );	// cb = 죽은 갯수
					}
					else
					{
						pi->m_cbTime = nTime;							// 타이머 reset
						if( pi->m_nMaxcb - pi->m_cb > 0 )
							cb	= (short)( pi->m_nMaxcb - pi->m_cb );				// cb = 죽은 갯수
					}
				}
			#else // __S1108_BACK_END_SYSTEM
				pi->m_cbTime = nTime;				// 타이머 reset
				cb = pi->m_cb;						// cb = 죽은 갯수
			#endif // __S1108_BACK_END_SYSTEM
				
				if( pi->m_bHalf )
				{
				#ifdef __S1108_BACK_END_SYSTEM
					cb	= (short)( ( pi->m_nMaxcb / 2 ) - ( pi->m_nMaxcb - cb ) );
				#else	// __S1108_BACK_END_SYSTEM
					cb	= (short)( ( pi->m_cbRespawn / 2 ) - ( pi->m_cbRespawn - cb ) );
				#endif	// __S1108_BACK_END_SYSTEM
				}
				pi->m_bHalf	= !pi->m_bHalf;

				dwFlying = 0;
				if( pi->m_dwType == OT_MOVER )
				{
					pMoverProp = prj.GetMoverProp( pi->m_dwIndex );
					if( pMoverProp )
						dwFlying	= pMoverProp->dwFlying;
				}

#ifndef __RES0807
				if( dwFlying )
					nCheckAttr = HATTR_NOFLY;
				else
					nCheckAttr = HATTR_NOWALK;
#endif	// __RES0807

				while( cb-- > 0 )
				{
					if( pi->m_dwIndex == 0 )
					{
						Error( "CRespawner::Spawn()\t// 0 index\n" );
						continue;
					}
					pObj	= CreateObj( D3DDEVICE, pi->m_dwType, pi->m_dwIndex );
					
					if( pi->m_dwType == OT_ITEM )
					{
						CItemElem* pItemElem	= new CItemElem;
						pItemElem->m_dwItemId	= pi->m_dwIndex;
						pItemElem->m_nItemNum	= (short)( pi->m_nItemMin + xRandom( pi->m_nItemMax - pi->m_nItemMin + 1 ) );
						ItemProp* pItemProp	= pItemElem->GetProp();
						if( !pItemProp )
						{
							char lpOutputString[100]	= { 0,};
							sprintf( lpOutputString, "%d item property not found", pi->m_dwIndex );
							OutputDebugString( lpOutputString );
						}
						pItemElem->m_nHitPoint	= pItemProp->dwEndurance;
						pItemElem->SetSerialNumber();
						( (CItem*)pObj )->m_pItemBase	= pItemElem;
						if( pItemElem->m_nItemNum > (int)( pItemProp->dwPackMax ) )
						{
							pItemElem->m_nItemNum	= (int)( pItemProp->dwPackMax );
						}
					}
					else
					if( pi->m_dwType == OT_CTRL )
					{
						memcpy( &(((CCommonCtrl*)pObj)->m_CtrlElem), &(pi->m_CtrlElem), sizeof( CCtrlElem ) );
						((CCommonCtrl*)pObj)->SetAngle( (float)( xRandom(360) ) );
					}
					else
					if( pi->m_dwType == OT_MOVER )
					{
						((CMover*)pObj)->SetGold(((CMover*)pObj)->GetLevel()*15);  // 몬스터 생성시 기본 페냐를 설정
					}

					if( !pObj )
						WriteLog( "SPAWN: %d, %d", pi->m_dwType, pi->m_dwIndex );

					pObj->m_dwAIInterfaceState = pi->m_dwAiState;
					pObj->m_dwPatrolIndex      = pi->m_dwPatrolIndex;
					pObj->m_bPatrolCycle       = pi->m_bPatrolCycle;
						
					if( ( pObj->m_dwAIInterfaceState == 9 || pObj->m_dwAIInterfaceState == 8 ) && pi->m_nMaxcb == 1 )
						pObj->SetAngle( pi->m_fAngle );					

#ifdef __RES0807
					pi->GetPos( v );
#else	// __RES0807
					nRetry	= 0;
lbl_retry:
					pi->GetPos( v ); // v에 랜덤 좌표를 얻는다.
					nAttr	= pWorld->GetHeightAttribute( v.x, v.z );

					if( nAttr == -1 || nAttr == HATTR_NOMOVE || nAttr == nCheckAttr ) 
					{
						v = pi->m_vPos;
						if( v == D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
						{
							if( ++nRetry < 100 )
								goto lbl_retry;
						#ifdef __S1108_BACK_END_SYSTEM
							if( pi->m_nGMIndex != 0 )
								Remove( pi->m_nGMIndex, TRUE );
							WriteLog( "GM RESPAWN(%d): %d, %d, %d, %d, %d, %d",
								pi->m_nGMIndex, pi->m_dwType, pi->m_dwIndex, pi->m_rect.left, pi->m_rect.top, pi->m_rect.right, pi->m_rect.bottom );
						#else // __S1108_BACK_END_SYSTEM
							WriteLog( "SPAWN: %d, %d, %d, %d, %d, %d",
								pi->m_dwType, pi->m_dwIndex, pi->m_rect.left, pi->m_rect.top, pi->m_rect.right, pi->m_rect.bottom );
						#endif // __S1108_BACK_END_SYSTEM

							SAFE_DELETE( pObj );
							pi->m_dwIndex		= 0;	// REMOVE
							break;
						}
					}
					else
					{
						pi->m_vPos	= v;
					}
#endif	// __RES0807

					if( dwFlying )
					{
						v.y	+= xRandom( 21 );	
						float y	= pWorld->GetLandHeight( v.x, v.z );
						if( v.y < y )	
							v.y	= y	+ 10.0f;
					}
					else
					{
						v.y = pi->m_fY + 1.0f;	// 약간 더해주자. 안그러면 뚫고 들어갈 수 있음 
						v.y	= pWorld->GetUnderHeight( v );
					}


					( (CCtrl*)pObj )->SetRespawn( pi->m_nGMIndex, nType );
					#ifdef __S1108_BACK_END_SYSTEM
						int nMaxAttckNum = 0;
						if( nType == RESPAWNTYPE_BACKEND )	// 운영자가 설정을 해놓은것은 적용을 안시킴(선공갯수를 적용시킬수 없음)
						{
							nMaxAttckNum = int( float( pi->m_nMaxcb * prj.m_fMonsterRespawnRate ) * float((float)pi->m_nMaxAttackNum / 100.0f) );
						}
						else
						{
							if( pi->m_nMaxAttackNum == 1 && pi->m_nMaxcb == 1 ) // 거대 몬스터? 몬스터 마리수가 1명이고 선공 몬스터시 무조건 선공 몬스터 임
							{
								nMaxAttckNum = pi->m_nActiveAttackNum + 1;
							}
							else
							{
								if( pWorld->GetID() == WI_WORLD_MADRIGAL )
								{
									nMaxAttckNum = int( ( pi->m_nMaxcb * prj.m_fMonsterRespawnRate ) * prj.m_fMonsterAggressiveRate );
								}
								else
								{
									nMaxAttckNum = pi->m_nMaxAttackNum;
								}
							}
						}		
						
						if( pObj->GetType() == OT_MOVER && ( pi->m_nActiveAttackNum < nMaxAttckNum ) )
						{
							InterlockedIncrement( &pi->m_nActiveAttackNum );
							if( pMoverProp->dwLevel >= 7 )	// 레벨이 7 이상인것만 선공으로 리스폰됨.
								((CMover*)pObj)->m_bActiveAttack	= TRUE;
						}
					#else // __S1108_BACK_END_SYSTEM
						if( pObj->GetType() == OT_MOVER && pi->m_nActiveAttackNum > 0 )
						{
							InterlockedDecrement( &pi->m_nActiveAttackNum );
							if( pMoverProp->dwLevel >= 7 )	// 레벨이 7 이상인것만 선공으로 리스폰됨.
								((CMover*)pObj)->m_bActiveAttack	= TRUE;
						}
					#endif // __S1108_BACK_END_SYSTEM

					pObj->SetPos( v );
					pWorld->ADDOBJ( pObj, TRUE, nLayer );
					uRespawned++;
//					InterlockedDecrement( &pi->m_cb );
				#ifdef __S1108_BACK_END_SYSTEM
					InterlockedIncrement( &pi->m_cb );
				#else // __S1108_BACK_END_SYSTEM
					InterlockedDecrement( &pi->m_cb );
				#endif // __S1108_BACK_END_SYSTEM
				}	// while( cb-- > 0 )					
			} // if( pi->m_cbTime < 0 )
		} // for( int i = 0; i < nSize; i++ )
	} // for( int nType = 0; nType < 3; nType++ )

	return uRespawned;
#endif // __!CLIENT
}
////////////////////////////////////////////////////////////////////////////////
#ifdef __LAYER_1021
CLayerdRespawner::CLayerdRespawner()
{
}

CLayerdRespawner::~CLayerdRespawner()
{
	Clear();
}

void CLayerdRespawner::Clear()
{
	for( MRP::iterator i = m_mapRespawners.begin(); i != m_mapRespawners.end(); ++i )
	{
		CRespawner* pRespawner	= i->second;
		SAFE_DELETE( pRespawner );
	}
	m_mapRespawners.clear();
}

int CLayerdRespawner::Add( CRespawnInfo & ri, int nType )
{
	return m_proto.Add( ri, nType );
}

BOOL CLayerdRespawner::Remove( int nRespawn, int nType )
{
	for( MRP::iterator i = m_mapRespawners.begin(); i != m_mapRespawners.end(); ++i )
	{
		CRespawner* pRespawner	= i->second;
		pRespawner->Remove( nRespawn, nType );
	}
	return m_proto.Remove( nRespawn, nType );
}

CRespawnInfo* CLayerdRespawner::GetRespawnInfo( int nRespawn, int nType, int nLayer )
{
	MRP::iterator i = m_mapRespawners.find( nLayer );
	if( i != m_mapRespawners.end() )
		return i->second->GetRespawnInfo( nRespawn, nType );
	return NULL;
}

u_long CLayerdRespawner::Spawn( CWorld* pWorld )
{
	u_long uRespawn	= 0;
	for( MRP::iterator i = m_mapRespawners.begin(); i != m_mapRespawners.end(); ++i )
		uRespawn	+= i->second->Spawn( pWorld, i->first );
	return uRespawn;
}

void CLayerdRespawner::Increment( int nRespawn, int nType, BOOL bActiveAttack, int nLayer )
{
	MRP::iterator i = m_mapRespawners.find( nLayer );
	if( i != m_mapRespawners.end() )
		i->second->Increment( nRespawn, nType, bActiveAttack );
}

void CLayerdRespawner::Expand( int nLayer )
{
	MRP::iterator i = m_mapRespawners.find( nLayer );
	if( i == m_mapRespawners.end() )
	{
		CRespawner* pRespawner	= new CRespawner( m_proto );
		bool bResult	= m_mapRespawners.insert( MRP::value_type( nLayer, pRespawner ) ).second;
	}
}

void CLayerdRespawner::Release( int nLayer )
{
	MRP::iterator i = m_mapRespawners.find( nLayer );
	if( i != m_mapRespawners.end() )
	{
		CRespawner* pRespawner	= i->second;
		SAFE_DELETE( pRespawner );
		m_mapRespawners.erase( i );
	}
}
#endif	// __LAYER_1021