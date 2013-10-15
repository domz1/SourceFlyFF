#include "stdafx.h"
#include "linkmap.h"
#include "layeredlinkmap.h"

#ifdef __LAYER_1015

CLayeredLinkMap::CLayeredLinkMap()
:
m_nLandWidth( 0 ),
m_nLandHeight( 0 ),
m_nView( 0 )
{
}

CLayeredLinkMap::~CLayeredLinkMap()
{
	Release();
}

void CLayeredLinkMap::Init( int nLandWidth, int nLandHeight, int nView, BOOL bBaseLayer, int nMPU )
{
	m_nLandWidth	= nLandWidth;
	m_nLandHeight	= nLandHeight;
	m_nView		= nView;
	m_iMPU = nMPU;

	if( bBaseLayer )
		CreateLinkMap( nDefaultLayer );
	
}

void CLayeredLinkMap::Release()
{
	for( MLM::iterator i = m_mapLinkMap.begin(); i != m_mapLinkMap.end(); ++i )
	{
		CLinkMap* pLinkMap	= i->second;
		pLinkMap->Release();
		SAFE_DELETE( pLinkMap );
	}
	m_mapLinkMap.clear();
}

int CLayeredLinkMap::GetLinkWidth( DWORD dwLinkType, int nLinkLevel, int nLayer )
{
	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
	return pLinkMap->GetLinkWidth( dwLinkType, nLinkLevel );
}

CObj** CLayeredLinkMap::GetObj( DWORD dwLinkType, DWORD dwLinkLevel, int nLayer )
{
	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
	return pLinkMap->GetObj( dwLinkType, dwLinkLevel );
}

DWORD CLayeredLinkMap::CalcLinkLevel( CObj* pObj, float fObjWidth, int nLayer )
{
	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
	return pLinkMap->CalcLinkLevel( pObj, fObjWidth );
}

int	CLayeredLinkMap::GetMaxLinkLevel( DWORD dwLinkType, int nLayer )
{
	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
	return pLinkMap->GetMaxLinkLevel( dwLinkType );
}

void CLayeredLinkMap::SetMaxLinkLevel( DWORD dwLinkType, int nLevel, int nLayer )
{
	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
	pLinkMap->SetMaxLinkLevel( dwLinkType, nLevel );
}

BOOL CLayeredLinkMap::InsertObjLink( CObj* pObj )
{
	CLinkMap* pLinkMap	= GetLinkMap( pObj->GetLayer() );
	return pLinkMap->InsertObjLink( pObj );
}

BOOL CLayeredLinkMap::RemoveObjLink2( CObj* pObj )
{
	CLinkMap* pLinkMap	= GetLinkMap( pObj->GetLayer() );
	return pLinkMap->RemoveObjLink2( pObj );
}

BOOL CLayeredLinkMap::RemoveObjLink( CObj* pObj )
{
	CLinkMap* pLinkMap	= GetLinkMap( pObj->GetLayer() );
	return pLinkMap->RemoveObjLink( pObj );
}

CObj* CLayeredLinkMap::GetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, int nLayer )
{
	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
	return pLinkMap->GetObjInLinkMap( vPos, dwLinkType, nLinkLevel );
}

BOOL CLayeredLinkMap::SetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj, int nLayer )
{
	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
	return pLinkMap->SetObjInLinkMap( vPos, dwLinkType, nLinkLevel, pObj );
}

void CLayeredLinkMap::AddItToView( CCtrl* pCtrl )
{
	CLinkMap* pLinkMap	= GetLinkMap( pCtrl->GetLayer() );
	pLinkMap->AddItToView( pCtrl );
}

void CLayeredLinkMap::ModifyView( CCtrl* pCtrl )
{
	CLinkMap* pLinkMap	= GetLinkMap( pCtrl->GetLayer() );
	pLinkMap->ModifyView( pCtrl );
}

BOOL CLayeredLinkMap::CreateLinkMap( int nLayer )
{
	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
	if( pLinkMap )
		return FALSE;
	pLinkMap	= new CLinkMap;
	pLinkMap->Init( m_nLandWidth, m_nLandHeight, m_nView, m_iMPU );
	bool bResult	= m_mapLinkMap.insert( MLM::value_type( nLayer, pLinkMap ) ).second;
	return TRUE;
}

CLinkMap* CLayeredLinkMap::GetLinkMap( int nLayer )
{
	MLM::iterator i	= m_mapLinkMap.find( nLayer );
	if( i != m_mapLinkMap.end() )
		return i->second;
	return NULL;
}

void CLayeredLinkMap::DeleteLinkMap( int nLayer )
{
	MLM::iterator i	= m_mapLinkMap.find( nLayer );
	if( i != m_mapLinkMap.end() )
	{
		CLinkMap* pLinkMap	= i->second;
		pLinkMap->Release();
		SAFE_DELETE( pLinkMap );
		m_mapLinkMap.erase( i );
	}
}

void CLayeredLinkMap::Process( CWorld* pWorld )
{
	for( MLM::iterator i = m_mapLinkMap.begin(); i != m_mapLinkMap.end(); )
	{
		int nLayer	= i->first;
		CLinkMap* pLinkMap	= i->second;
		++i;
		if( nLayer <= 0 )
			continue;
		if( pLinkMap->IsInvalid() )
		{
			if( pWorld->HasNobody( nLayer ) )
			{
				pWorld->ReleaseLayer( nLayer );
				TRACE( "CWorld.ReleaseLayer( %d, %d )\n", pWorld->GetID(), nLayer );
			}
			else
				pWorld->DriveOut( nLayer );
		}
	}
}

#if __VER >= 15 // __GUILD_HOUSE
void CLayeredLinkMap::Invalidate( int nLayer, BOOL bInvalid )
#else // __GUILD_HOUSE
void CLayeredLinkMap::Invalidate( int nLayer )
#endif // __GUILD_HOUSE
{
	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
	if( pLinkMap )
		pLinkMap->Invalidate( bInvalid );
}

#endif	// __LAYER_1015