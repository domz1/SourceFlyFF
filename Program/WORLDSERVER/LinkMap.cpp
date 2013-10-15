#include "stdafx.h"
#include "LinkMap.h"
#include "World.h"
#include "user.h"


// 랜드스케이프의 길이 = 512미터
// 512미터 * 30개의 랜드스케이프 = 15360미터 ( 하나의 랜드스케이프 = 512미터 )
// 맵: 15360미터 * 15360미터 

// FOR_LINKMAP은 충돌 검출을 위해서 범위를 0으로 주어도 루프를 2 * 2 를 돈다.
// 지형과의 충돌 검출은 octree로, 그외의 객체는 기존 링크맵을 이용하고 싶다. ( 2*2를 돌지 않게끔 수정 후)

static float g_table[MAX_LINKLEVEL] = { 32.0f, 64.0f, 128.0f, 256.0f, 512.0f, 512.0f, 512.0f };    
static float fDynamicGrid[MAX_LINKLEVEL] = { 512.0f, 512.0f, 512.0f, 512.0f, 512.0f, 512.0f, 512.0f };	// 강제로 0레벨에 넣기 위한 테이블 
static float fStaticGrid[MAX_LINKLEVEL]  = { 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f, 256.0f };


CLinkMap::CLinkMap()
:
m_nLandWidth( 0 ), m_nLandHeight( 0 )
#ifdef __LAYER_1015
,m_bInvalid( FALSE )
#endif	// __LAYER_1015
{
	memset( m_infos, 0x00, sizeof(m_infos) );
}

void CLinkMap::Init( int nLandWidth, int nLandHeight, int nView, int nMPU )
{
	m_nLandWidth = nLandWidth;
	m_nLandHeight = nLandHeight;

	//gmpbigsun:  now static grid is min 1 to max 4
	m_iMPU = nMPU;
	float fMulGrid = 0.25f * (float)m_iMPU;
	assert( m_iMPU > 0 && m_iMPU < 5 );
	
	int nLevel, nType;
	int nLandscape = MAP_SIZE * m_iMPU;	

	int nCX = nLandscape * nLandWidth;  // 맵의 한변의 길이 (미터 단위) = 랜드스케이프 길이 * 맵의 한변 랜드스케이프 갯수 
	float* fDivisor;

	for( nType = 0; nType < MAX_LINKTYPE; nType++  )
	{
		LINKMAP_INFO* pInfo = &m_infos[nType];
		if( nType == CObj::linkStatic )
			fDivisor = fStaticGrid;
		else
			fDivisor = g_table;

		for( nLevel = 0; nLevel < MAX_LINKLEVEL; nLevel++ )
		{
			int reviseGrid = (int)(fDivisor[ nLevel ] * fMulGrid);
			assert( reviseGrid > 0 );
			//pInfo->nWidth[nLevel]  = nLandscape / (int)fDivisor[nLevel];  // 랜드스케이프의 크기를 그리드의 크기로 나누면 
			pInfo->nWidth[nLevel]  = nLandscape / reviseGrid;  // 랜드스케이프의 크기를 그리드의 크기로 나누면 

			int nCount = ( nCX / reviseGrid  ) * ( nCX / reviseGrid );
			pInfo->apObj[nLevel] = new CObj*[ nCount ];
			memset( pInfo->apObj[nLevel], 0x00, sizeof(CObj*) * nCount );
		}
	}
	
	for( int i=0; i < MAX_LINKLEVEL; i++ )
	{
		// m_nVisibilityRange[i] = { 2, 1, 0, 0, 0, 0 };
		m_nVisibilityRange[i]	= (int)( 2 * nView / pow( 2, i ) );		
		int nSizeOfMask	= ( 2 * m_nVisibilityRange[i] + 1 ) * ( 2 * m_nVisibilityRange[i] + 1 );
		m_apfMask[i]	= new BOOL[nSizeOfMask];
		memset( m_apfMask[i], 0, sizeof(BOOL) * nSizeOfMask );
	}
}

void CLinkMap::Release()
{
	int nLevel, nType;
	for( nLevel = 0; nLevel < MAX_LINKLEVEL; nLevel++ )
	{
		SAFE_DELETE_ARRAY( m_apfMask[nLevel] );
	}

	for( nType = 0; nType < MAX_LINKTYPE; nType++ )
		for( nLevel = 0; nLevel < MAX_LINKLEVEL; nLevel++ )
			SAFE_DELETE_ARRAY( m_infos[nType].apObj[nLevel] );
}

// vPos위치, dwLinkType, nLinkLevel에 위치한 obj포인터의 포인터를 리턴 
CObj** CLinkMap::GetObjPtr( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel )
{
	CObj** aObjLinkMap	= GetObj( dwLinkType, nLinkLevel );
	int nMaxWidth = GetLinkWidth( dwLinkType, nLinkLevel ) * m_nLandWidth;
	
	int nUnit = (MAP_SIZE * m_nLandWidth) / nMaxWidth;
	int nPos = int( vPos.z / m_iMPU / nUnit ) * nMaxWidth + int( vPos.x / m_iMPU / nUnit );
	if( nPos >= 0 && nPos < nMaxWidth * nMaxWidth )
		return (aObjLinkMap + nPos) ;
	return NULL;
}

BOOL CLinkMap::InsertObjLink( CObj* pObj )
{
	if( pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->IsPlayer() && pObj->GetLinkLevel() != 0 )
		WriteError( "ML//%s//%d//%d", ( (CMover*)pObj )->GetName(), ( (CMover*)pObj )->m_idPlayer, pObj->GetLinkLevel() );
	
	CObj** ppObj = GetObjPtr( pObj->GetLinkPos(), pObj->GetLinkType(), pObj->GetLinkLevel() );
	if( ppObj )
	{
		CObj* pBegObj = *ppObj;
		if( pBegObj )
		{
			if( pObj->m_pNext || pObj->m_pPrev )
			{
				Error("헉 InsertObjWorld 새로 추가된 오브젝트가 Next와 Prev에 값이 있네?? Type = %d \n", pObj->GetType());
			}
			pBegObj->InsNextNode(pObj);
		}
		else 
		{
			*ppObj = pObj;
			if( pObj->m_pNext || pObj->m_pPrev )
			{
				Error("헉 InsertObjWorld 새로 추가된 오브젝트가 Next와 Prev에 값이 있네?? Type = %d \n", pObj->GetType());
			}
		}
	}
	else
	{
		Error("헉 InsObjInFld에서 범위를 벗어난게 추가되려고 하네?\n");
		return FALSE;
	}
	return TRUE;
}

BOOL CLinkMap::RemoveObjLink2( CObj* pObj )
{
	WriteError( "ROL//CALLED" );

	int nLinkLevel	= pObj->GetLinkLevel();
	int nLinkType	= pObj->GetLinkType();
	CObj** aObjLinkMap	= GetObj( nLinkType, nLinkLevel );
	int nMaxWidth	= GetLinkWidth( nLinkType, nLinkLevel ) * m_nLandWidth;
	int nMaxPos		= nMaxWidth * nMaxWidth;

	for( int nPos = 0; nPos < nMaxPos; nPos++ )
	{
		if( aObjLinkMap[nPos] == pObj )
		{
			aObjLinkMap[nPos]	= pObj->GetNextNode();
			pObj->DelNode();
			WriteError( "ROL//FIXED//%d, %d", pObj->GetType(), pObj->GetIndex() );
			return TRUE;
		}
	}
	WriteError( "ROL//NOT FIXED//%d, %d", pObj->GetType(), pObj->GetIndex() );
	return FALSE;
}

BOOL CLinkMap::RemoveObjLink( CObj* pObj )
{
	if( pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->IsPlayer() && pObj->GetLinkLevel() != 0 )
		WriteError( "ROL//%s//%d//%d", ( (CMover*)pObj )->GetName(), ( (CMover*)pObj )->m_idPlayer, pObj->GetLinkLevel() );

	CObj** ppObj = GetObjPtr( pObj->GetLinkPos(), pObj->GetLinkType(), pObj->GetLinkLevel() );
	if( ppObj )
	{
		if( !pObj->m_pPrev && *ppObj != pObj )
			return RemoveObjLink2( pObj );

		if( *ppObj == pObj )
			*ppObj = pObj->GetNextNode();
		pObj->DelNode();
		return TRUE;
	}

	D3DXVECTOR3 vPos = pObj->GetLinkPos();
	WriteError( "ROL//POS OVERFLOW//%d, %d, %f, %f, %f, %f, %f, %f", 
			pObj->GetType(), pObj->GetIndex(), pObj->GetPos().x, pObj->GetPos().y, pObj->GetPos().z, vPos.x, vPos.y, vPos.z );
	if( RemoveObjLink2( pObj ) == FALSE )
		pObj->DelNode();
	return FALSE;
}

CObj* CLinkMap::GetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel )
{
	CObj** ppObj = GetObjPtr( vPos, dwLinkType, nLinkLevel );
	if( ppObj )
		return *ppObj;
	else
		return NULL;
}

BOOL CLinkMap::SetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj )
{
	CObj** ppObj = GetObjPtr( vPos, dwLinkType, nLinkLevel );
	if( ppObj )
	{
		*ppObj = pObj;
		return TRUE;
	}
	else
		return FALSE;
}

void CLinkMap::AddItToView( CCtrl* pCtrl )
{
	D3DXVECTOR3 vPos	= pCtrl->GetLinkPos();


	int nVisibilityRange;
	int nMaxWidth, nMaxHeight, nUnit, nUBoundX, nUBoundZ;
	int i, j, k;
	int nLinkX, nLinkZ;
	CObj** apObj;
//	CObj* pObj;

	CUser *pUser, *pUsertmp;
	CCtrl* pCtrltmp;
	
	// is player
	if( pCtrl->GetType() == OT_MOVER && ( (CMover*)pCtrl )->IsPlayer() )
	{
		pUser	= (CUser*)pCtrl;
		for( i = 0; i < MAX_LINKLEVEL; i++ )
		{
			nMaxWidth	= GetLinkWidth( CObj::linkDynamic, i) * m_nLandWidth;
			nMaxHeight	= GetLinkWidth( CObj::linkDynamic, i) * m_nLandHeight;

			nUnit	= (MAP_SIZE * m_nLandWidth) / nMaxWidth;
			nLinkX	= (int)( ( vPos.x / m_iMPU ) / nUnit );
			nLinkZ	= (int)( ( vPos.z / m_iMPU ) / nUnit );
			nVisibilityRange	= m_nVisibilityRange[i];
			nUBoundX	= nLinkX + nVisibilityRange;
			nUBoundZ	= nLinkZ + nVisibilityRange;
			pUser->m_nOldCenter[i]	= nLinkZ * nMaxWidth + nLinkX;

//			player link
			apObj	= GetObj( CObj::linkPlayer, i );	
			for( j = nLinkZ - nVisibilityRange; j <= nUBoundZ; j++ )
			{
				if( j < 0 || j >= nMaxHeight )	continue;
				for( k = nLinkX - nVisibilityRange; k <= nUBoundX; k++ )
				{
					if( k < 0 || k >= nMaxWidth )	continue;
					pUsertmp	= (CUser*)apObj[j * nMaxWidth + k];
					while( pUsertmp )
					{
						if( !pUsertmp->IsDelete() && pUsertmp != pUser )
						{
							pUsertmp->PCSetAt( pUser->GetId(), (CCtrl*)pUser );
							pUsertmp->AddAddObj( (CCtrl*)pUser );
							pUser->PCSetAt( pUsertmp->GetId(), (CCtrl*)pUsertmp );
							pUser->AddAddObj( (CCtrl*)pUsertmp );
						}
						pUsertmp	= (CUser*)pUsertmp->GetNextNode();
					}
				}
			}

//			dynamic link
			apObj	= GetObj( CObj::linkDynamic, i );
			for( j = nLinkZ - nVisibilityRange; j <= nUBoundZ; j++ )
			{
				if( j < 0 || j >= nMaxHeight )	continue;
				for( k = nLinkX - nVisibilityRange; k <= nUBoundX; k++ )
				{
					if( k < 0 || k >= nMaxWidth )	continue;
					pCtrltmp	= (CCtrl*)apObj[j * nMaxWidth + k];
					while( pCtrltmp )
					{
						if( !pCtrltmp->IsDelete() )
						{
							pCtrltmp->PCSetAt( pUser->GetId(), (CCtrl*)pUser );
							pUser->NPCSetAt( pCtrltmp->GetId(), pCtrltmp );
							pUser->AddAddObj( pCtrltmp );
						}
						pCtrltmp	= (CCtrl*)pCtrltmp->GetNextNode();
					}
				}
			}

//			airship link
			apObj	= GetObj( CObj::linkAirShip, i );
			for( j = nLinkZ - nVisibilityRange; j <= nUBoundZ; j++ )
			{
				if( j < 0 || j >= nMaxHeight )	continue;
				for( k = nLinkX - nVisibilityRange; k <= nUBoundX; k++ )
				{
					if( k < 0 || k >= nMaxWidth )	continue;
					pCtrltmp	= (CCtrl*)apObj[j * nMaxWidth + k];
					while( pCtrltmp )
					{
						if( !pCtrltmp->IsDelete() )
						{
							pCtrltmp->PCSetAt( pUser->GetId(), (CCtrl*)pUser );
							pUser->NPCSetAt( pCtrltmp->GetId(), pCtrltmp );
							pUser->AddAddObj( pCtrltmp );
						}
						pCtrltmp	= (CCtrl*)pCtrltmp->GetNextNode();
					}
				}
			}
		}
	}
	else	// is not player
	{
//		for( i = 0; i < MAX_LINKLEVEL; i++ )
		i	= 0;		//
//		{
		
			nMaxWidth	= GetLinkWidth( CObj::linkDynamic, i ) * m_nLandWidth;  // CObj::linkDynamic -> CObj::linkPlayer
			nMaxHeight	= GetLinkWidth( CObj::linkDynamic, i ) * m_nLandHeight;

			nUnit	= (MAP_SIZE * m_nLandWidth) / nMaxWidth;
			nLinkX	= (int)( ( vPos.x / m_iMPU ) / nUnit );
			nLinkZ	= (int) ( ( vPos.z / m_iMPU ) / nUnit );
			nVisibilityRange	= m_nVisibilityRange[i];
			nUBoundX	= nLinkX + nVisibilityRange;
			nUBoundZ	= nLinkZ + nVisibilityRange;
			if( pCtrl->GetType() == OT_SHIP || pCtrl->GetType() == OT_MOVER )
				pCtrl->m_nOldCenter[i]	= nLinkZ * nMaxWidth + nLinkX;

			apObj	= GetObj( CObj::linkPlayer, i );

			for( j = nLinkZ - nVisibilityRange; j <= nUBoundZ; j++ )
			{
				if( j < 0 || j >= nMaxHeight )	continue;
				for( k = nLinkX - nVisibilityRange; k <= nUBoundX; k++ )
				{
					if( k < 0 || k >= nMaxWidth )	continue;
					pUsertmp	= (CUser*)apObj[j * nMaxWidth + k];
					while( pUsertmp )
					{
						if( !pUsertmp->IsDelete() )
						{
							pUsertmp->NPCSetAt( pCtrl->GetId(), pCtrl );
							pUsertmp->AddAddObj( pCtrl );
							pCtrl->PCSetAt( pUsertmp->GetId(), (CCtrl*)pUsertmp );
						}
						pUsertmp	= (CUser*)pUsertmp->GetNextNode();
					}
				}
			}
//		}
	}
}

void CLinkMap::ModifyView( CCtrl* pCtrl )
{
	int nCenter, nOldCenter;
	int nMaxWidth, nUnit, nPos, nMaxPos, nVisibilityRange;
	int i, j;
	
	BOOL* pfMask;
	int nMaskPos;

	CObj** apObj;
//	CObj* pObj;
	CUser *pUser, *pUsertmp;
	CCtrl* pCtrltmp;

	D3DXVECTOR3 vPos	= pCtrl->GetLinkPos();

	// is player
	if( pCtrl->GetType() == OT_MOVER && ( (CMover*)pCtrl )->IsPlayer() )
	{
		pUser	= (CUser*)pCtrl;
		
		for( int nLinkLevel = 0; nLinkLevel < MAX_LINKLEVEL; nLinkLevel++ )
		{
//			player link
			apObj	    = GetObj( CObj::linkPlayer, nLinkLevel );

			nMaxWidth	= GetLinkWidth( CObj::linkPlayer, nLinkLevel) * m_nLandWidth;
			nMaxPos		= nMaxWidth * nMaxWidth;

			nUnit	    = (MAP_SIZE * m_nLandWidth) / nMaxWidth;
			nCenter		= (int)( vPos.z / m_iMPU / nUnit ) * nMaxWidth + (int)( vPos.x / m_iMPU / nUnit );
			nOldCenter	= pUser->m_nOldCenter[nLinkLevel];

			if( nCenter == nOldCenter ) 
			{
				if( nLinkLevel == 0 )	
					return;
				continue;
			}
	
			nVisibilityRange	= m_nVisibilityRange[nLinkLevel];
			pfMask	= m_apfMask[nLinkLevel];
			
			for( i = -nVisibilityRange; i <= nVisibilityRange; i++ )
			{
				for( j = -nVisibilityRange; j <= nVisibilityRange; j++ )
				{
					nPos	= nCenter + i * nMaxWidth + j;
					if( nPos < 0 || nPos >= nMaxPos )	continue;
					if( ( nMaskPos = IsOverlapped( nOldCenter, nPos, nVisibilityRange, nMaxWidth ) ) < 0 )
					{
						pUsertmp	= (CUser*)apObj[nPos];
						while( pUsertmp )
						{
							if( !pUsertmp->IsDelete() && pUsertmp != pUser )
							{
								pUser->PCSetAt( pUsertmp->GetId(), (CCtrl*)pUsertmp );
								pUser->AddAddObj( (CCtrl*)pUsertmp );
								pUsertmp->PCSetAt( pUser->GetId(), (CCtrl*)pUser );
								pUsertmp->AddAddObj( (CCtrl*)pUser );
							}
							pUsertmp	= (CUser*)pUsertmp->GetNextNode();
						}
					}
					else	// old pos
					{
						pfMask[nMaskPos]	= TRUE;
					}
				}
			}

			for( i = -nVisibilityRange; i <= nVisibilityRange; i ++ )
			{
				for( j = -nVisibilityRange; j <= nVisibilityRange; j++ )
				{
					nPos	= nOldCenter + i * nMaxWidth + j;
					if( nPos < 0 || nPos >= nMaxPos )	continue;
					nMaskPos		= ( i + nVisibilityRange ) * ( 2 * nVisibilityRange + 1 ) + ( j + nVisibilityRange ) ;
					if( pfMask[nMaskPos] == FALSE )
					{
						pUsertmp	= (CUser*)apObj[nPos];
						while( pUsertmp )
						{
							if( !pUsertmp->IsDelete() )
							{
								pUser->PCRemoveKey( pUsertmp->GetId() );
								pUser->AddRemoveObj( pUsertmp->GetId() );
								pUsertmp->PCRemoveKey( pUser->GetId() );
								pUsertmp->AddRemoveObj( pUser->GetId() );
							}
							pUsertmp	= (CUser*)pUsertmp->GetNextNode();
						}
					}
					else
					{
						pfMask[nMaskPos]	= FALSE;
					}
				}
			}

//			dynamic link
			apObj	= GetObj( CObj::linkDynamic, nLinkLevel );
			
			for( i = -nVisibilityRange; i <= nVisibilityRange; i++ )
			{
				for( j = -nVisibilityRange; j <= nVisibilityRange; j++ )
				{
					nPos	= nCenter + i * nMaxWidth + j;
					if( nPos < 0 || nPos >= nMaxPos )	continue;
					if( ( nMaskPos = IsOverlapped( nOldCenter, nPos, nVisibilityRange, nMaxWidth ) ) < 0 )
					{
						pCtrltmp	= (CCtrl*)apObj[nPos];
						while( pCtrltmp )
						{
							if( !pCtrltmp->IsDelete() )
							{
								pUser->NPCSetAt( pCtrltmp->GetId(), pCtrltmp );
								pUser->AddAddObj( pCtrltmp );
								pCtrltmp->PCSetAt( pUser->GetId(), (CCtrl*)pUser );
							}
							pCtrltmp	= (CCtrl*)pCtrltmp->GetNextNode();
						}
					}
					else	// old pos
					{
						pfMask[nMaskPos]	= TRUE;
					}
				}
			}

			for( i = -nVisibilityRange; i <= nVisibilityRange; i ++ )
			{
				for( j = -nVisibilityRange; j <= nVisibilityRange; j++ )
				{
					nPos	= nOldCenter + i * nMaxWidth + j;
					if( nPos < 0 || nPos >= nMaxPos )	continue;
					nMaskPos		= ( i + nVisibilityRange ) * ( 2 * nVisibilityRange + 1 ) + ( j + nVisibilityRange ) ;
					if( pfMask[nMaskPos] == FALSE )
					{
						pCtrltmp	= (CCtrl*)apObj[nPos];
						while( pCtrltmp )
						{
							if( !pCtrltmp->IsDelete() )
							{
								pUser->NPCRemoveKey( pCtrltmp->GetId() );
								pUser->AddRemoveObj( pCtrltmp->GetId() );
								pCtrltmp->PCRemoveKey( pUser->GetId() );
							}
							pCtrltmp	= (CCtrl*)pCtrltmp->GetNextNode();
						}
					}
					else
					{
						pfMask[nMaskPos]	= FALSE;
					}
				}
			}
//			airship link
			apObj	= GetObj( CObj::linkAirShip, nLinkLevel );
			
			for( i = -nVisibilityRange; i <= nVisibilityRange; i++ )
			{
				for( j = -nVisibilityRange; j <= nVisibilityRange; j++ )
				{
					nPos	= nCenter + i * nMaxWidth + j;
					if( nPos < 0 || nPos >= nMaxPos )	continue;
					if( ( nMaskPos = IsOverlapped( nOldCenter, nPos, nVisibilityRange, nMaxWidth ) ) < 0 )
					{
						pCtrltmp	= (CCtrl*)apObj[nPos];
						while( pCtrltmp )
						{
							if( !pCtrltmp->IsDelete() )
							{
								pUser->NPCSetAt( pCtrltmp->GetId(), pCtrltmp );
								pUser->AddAddObj( pCtrltmp );
								pCtrltmp->PCSetAt( pUser->GetId(), (CCtrl*)pUser );
							}
							pCtrltmp	= (CCtrl*)pCtrltmp->GetNextNode();
						}
					}
					else	// old pos
					{
						pfMask[nMaskPos]	= TRUE;
					}
				}
			}

			for( i = -nVisibilityRange; i <= nVisibilityRange; i ++ )
			{
				for( j = -nVisibilityRange; j <= nVisibilityRange; j++ )
				{
					nPos	= nOldCenter + i * nMaxWidth + j;
					if( nPos < 0 || nPos >= nMaxPos )	continue;
					nMaskPos		= ( i + nVisibilityRange ) * ( 2 * nVisibilityRange + 1 ) + ( j + nVisibilityRange ) ;
					if( pfMask[nMaskPos] == FALSE )
					{
						pCtrltmp	= (CCtrl*)apObj[nPos];
						while( pCtrltmp )
						{
							if( !pCtrltmp->IsDelete() )
							{
								pUser->NPCRemoveKey( pCtrltmp->GetId() );
								pUser->AddRemoveObj( pCtrltmp->GetId() );
								pCtrltmp->PCRemoveKey( pUser->GetId() );
							}
							pCtrltmp	= (CCtrl*)pCtrltmp->GetNextNode();
						}
					}
					else
					{
						pfMask[nMaskPos]	= FALSE;
					}
				}
			}
			pUser->m_nOldCenter[nLinkLevel]	= nCenter;
		}
	}
	// is not player
	else
	{
		const int nMaxLevel	= 1;
		for( int nLinkLevel = 0; nLinkLevel < nMaxLevel; nLinkLevel++ )
		{
			apObj	    = GetObj( CObj::linkPlayer, nLinkLevel );
			nMaxWidth	= GetLinkWidth( CObj::linkPlayer, nLinkLevel) * m_nLandWidth;
			nMaxPos		= nMaxWidth * nMaxWidth;

			nUnit	    = (MAP_SIZE * m_nLandWidth) / nMaxWidth;
			nCenter		= (int)( vPos.z / m_iMPU / nUnit ) * nMaxWidth + (int)( vPos.x / m_iMPU / nUnit );
			nOldCenter	= pCtrl->m_nOldCenter[nLinkLevel];

			if( nCenter == nOldCenter ) 
			{
				if( nLinkLevel == 0 )	
					return;
				continue;
			}
	
			nVisibilityRange	= m_nVisibilityRange[nLinkLevel];
			pfMask	= m_apfMask[nLinkLevel];

			for( i = -nVisibilityRange; i <= nVisibilityRange; i++ )
			{
				for( j = -nVisibilityRange; j <= nVisibilityRange; j++ )
				{
					nPos	= nCenter + i * nMaxWidth + j;
					if( nPos < 0 || nPos >= nMaxPos )	continue;
					if( ( nMaskPos = IsOverlapped( nOldCenter, nPos, nVisibilityRange, nMaxWidth ) ) < 0 )
					{
						pUsertmp	= (CUser*)apObj[nPos];
						while( pUsertmp )
						{
							if( !pUsertmp->IsDelete() )
							{
								pCtrl->PCSetAt( pUsertmp->GetId(), (CCtrl*)pUsertmp );
								pUsertmp->NPCSetAt( pCtrl->GetId(), pCtrl );
								pUsertmp->AddAddObj( pCtrl );
							}
							pUsertmp	= (CUser*)pUsertmp->GetNextNode();
						}
					}
					else	// old pos
					{
						pfMask[nMaskPos]	= TRUE;
					}
				}
			}

			for( i = -nVisibilityRange; i <= nVisibilityRange; i ++ )
			{
				for( j = -nVisibilityRange; j <= nVisibilityRange; j++ )
				{
					nPos	= nOldCenter + i * nMaxWidth + j;
					if( nPos < 0 || nPos >= nMaxPos )	continue;
					nMaskPos		= ( i + nVisibilityRange ) * ( 2 * nVisibilityRange + 1 ) + ( j + nVisibilityRange ) ;
					if( pfMask[nMaskPos] == FALSE )
					{
						pUsertmp	= (CUser*)apObj[nPos];
						while( pUsertmp )
						{
							if( !pUsertmp->IsDelete() )
							{
								pCtrl->PCRemoveKey( pUsertmp->GetId() );
								pUsertmp->NPCRemoveKey( pCtrl->GetId() );
								pUsertmp->AddRemoveObj( pCtrl->GetId() );
							}
							pUsertmp	= (CUser*)pUsertmp->GetNextNode();
						}
					}
					else
					{
						pfMask[nMaskPos]	= FALSE;
					}
				}
			}
			pCtrl->m_nOldCenter[nLinkLevel]	= nCenter;
		}
	}
}

int CLinkMap::IsOverlapped( int c, int p, int r, int w )
{	/*	c	- old center,	p	- index,	r	- range,	w	- width		*/
	int nZ;
	float fZ	= (float)( p - c + r ) / w;
	
	// find row
	if( fZ >= 0 )
	{
		nZ	= (int)fZ;
	}
	else
	{
		if( (int)fZ / fZ == 1.0f )
			nZ	= (int)fZ;
		else
			nZ	= (int)( fZ - 1 );
	}

	int k	= c + nZ * w;
	if( nZ >= -r && nZ <= r &&  k - r <= p && p <= k + r )
	{
		return ( ( nZ + r ) * ( 2 * r + 1 ) + ( p - k/*nX*/ + r ) );	// mask index
	}

	return -1;
}

void CLinkMap::SetMaxLinkLevel( DWORD dwLinkType, int nLevel )
{
	m_infos[dwLinkType].nMaxLevel = nLevel;
}

// 링크레벨을 계산한다.
DWORD CLinkMap::CalcLinkLevel( CObj* pObj, float fObjWidth )
{
	float* fGrid = NULL;
	DWORD dwLinkType = pObj->GetLinkType();

	if( dwLinkType == CObj::linkStatic )
		fGrid = fStaticGrid;
	else
		fGrid = fDynamicGrid;

	float fMulGrid = 0.25f * (float)m_iMPU;			//gmpbigsun!!
	
	for( int nLevel = 0; nLevel < MAX_LINKLEVEL; ++nLevel )
	{
		if( fObjWidth < ( fGrid[nLevel] * fMulGrid ) )
			break;
	}

	if( nLevel == MAX_LINKLEVEL )
	{
		//ASSERT( 0 );
		TRACE("pObj->m_dwIndex:%d fObjWidth: %f linkType:%d\n", pObj->m_dwIndex, fObjWidth, dwLinkType ); 
	}
	
	nLevel = min( nLevel, (MAX_LINKLEVEL - 1));

	if( (nLevel+1) > GetMaxLinkLevel(dwLinkType) )
		SetMaxLinkLevel( dwLinkType, nLevel+1 );
	return nLevel;
}


