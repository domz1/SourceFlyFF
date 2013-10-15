#include "stdafx.h"
#include "PathObj.h"
#include "Respawn.h"

#ifdef __WORLDSERVER

CPatrolPath::CPatrolPath()
{
	Clear();
}

CPatrolPath::~CPatrolPath()
{
}

void CPatrolPath::Clear()
{
	m_mapPatrolPath.clear();
	m_bReturn.clear();
}

CPatrolPath*	CPatrolPath::GetInstance( void )
{
	static CPatrolPath	sMailBox;
	return &sMailBox;
}

void CPatrolPath::SetReturn( DWORD dwIndex, BOOL bReturn )
{
}

// 좌표를 추가한다.
void CPatrolPath::AddPatrolPath( DWORD dwIndex, _VECINFO vecInfo )
{
	map< DWORD, vector<_VECINFO> >::iterator it = m_mapPatrolPath.find( dwIndex );

	vector<_VECINFO> pTemp;

	// 기존에 들어가있다
	if( it != m_mapPatrolPath.end() )
	{
		// 처음엔 시작좌표 넣는다...
		it->second.push_back( vecInfo );
	}
	else
	// 새롭게 넣는다
	{
		pTemp.push_back( vecInfo );
		m_mapPatrolPath.insert( map< DWORD, vector<_VECINFO> >::value_type(dwIndex, pTemp) );
	}
}

// 다음 
void CPatrolPath::GetNextPosInfo( CObj* pObj, const D3DXVECTOR3 vPos, D3DXVECTOR3& vDest, _VECINFO& _vecInfo )
//void CPatrolPath::GetNextPosInfo( DWORD dwIndex, DWORD& dwCount, const D3DXVECTOR3 vPos, D3DXVECTOR3& vDest, _VECINFO& _vecInfo )
{
	map< DWORD, vector<_VECINFO> >::iterator it1 = m_mapPatrolPath.find( pObj->m_dwPatrolIndex );
	
	if( it1 == m_mapPatrolPath.end() )
	{
		_vecInfo.m_vDir    = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		_vecInfo.m_fLength = 0.0f;
		_vecInfo.m_dwDelay = 0;
		
		vDest = vPos;

		return;
	}

	vector<_VECINFO> _vecinfoTemp = it1->second;

	D3DXVECTOR3	v3Dir = _vecinfoTemp[pObj->m_nPatrolIndexCount].m_vDir;
	FLOAT fLength = _vecinfoTemp[pObj->m_nPatrolIndexCount].m_fLength;
				
	_vecInfo = _vecinfoTemp[pObj->m_nPatrolIndexCount];

	if( pObj->m_bPatrolReverse == 0 )
		v3Dir = +(v3Dir);
	else
		v3Dir = -(v3Dir);	

	vDest    = vPos + (v3Dir * fLength );

	// 전체순환이냐
	if( pObj->m_bPatrolCycle == 1 )
		pObj->m_nPatrolIndexCount++;
	else
	{
		if( pObj->m_bPatrolReverse == 0 )
			pObj->m_nPatrolIndexCount++;
		else
			pObj->m_nPatrolIndexCount--;		
	}
	
	// 전체순환이냐
	if( pObj->m_bPatrolCycle == 1 )
	{
		if( pObj->m_nPatrolIndexCount >= (int)( _vecinfoTemp.size() ) )
		{
			pObj->m_nPatrolIndexCount = 0;
		}
	}
	else
	{
		if( pObj->m_nPatrolIndexCount >= (int)_vecinfoTemp.size()-1 )
		{
			pObj->m_nPatrolIndexCount = pObj->m_nPatrolIndexCount - 1;
			pObj->m_bPatrolReverse = 1; // 거꾸로..
		}
		else
		if( pObj->m_nPatrolIndexCount < 0 )
		{
			pObj->m_nPatrolIndexCount = 0;
			pObj->m_bPatrolReverse = 0; // 거꾸로..
		}
	}
}

void CPatrolPath::AddPatrolIndex(DWORD dwIndex)
{
	map< DWORD, vector<_VECINFO> >::iterator it = m_mapPatrolPath.find( dwIndex );
	
	// 기존에 들어가있다
	if( it != m_mapPatrolPath.end() )
	{
		return;
	}
	else
	// 새롭게 넣는다
	{
		vector<_VECINFO> pTemp;
		pTemp.clear();
		m_mapPatrolPath.insert( map< DWORD, vector<_VECINFO> >::value_type(dwIndex, pTemp) );
	}	
}

BOOL CPatrolPath::IsFirstPath( DWORD dwIndex )
{
	map< DWORD, vector<_VECINFO> >::iterator it = m_mapPatrolPath.find( dwIndex );
	
	// 기존에 들어가있다
	if( it != m_mapPatrolPath.end() )
	{
		if( it->second.size() == 0 )
			return TRUE;
	}

	return FALSE;
}

BOOL CPatrolPath::LoadPatrol( LPCTSTR szFileName )
{
	CScript s;
	if( s.Load( szFileName, FALSE ) )
	{
		Clear();
		
		DWORD nIndex = 0;
		D3DXVECTOR3 v3Pos;
		vector<D3DXVECTOR3> v3PosList;
		_VECINFO vecInfo;
		_VECINFO vecInfoBackup;
		
		s.GetToken();
		
		while( s.tok != FINISHED )
		{
			if( s.Token == _T( "INDEX" ) )
			{
				v3PosList.clear();
				nIndex = s.GetNumber();
				AddPatrolIndex( nIndex );
			}
			
			s.GetToken();
			
			int nCount = s.GetNumber();
			
			while( s.Token != _T( "}" ) )
			{
				v3Pos.x = s.GetFloat();
				v3Pos.y = s.GetFloat();
				v3Pos.z = s.GetFloat();

				v3PosList.push_back( v3Pos );
				
				if( nCount != 0 )
				{
					vecInfo.m_vDir.x = s.GetFloat();
					vecInfo.m_vDir.y = s.GetFloat();
					vecInfo.m_vDir.z = s.GetFloat();
					vecInfo.m_dwState = s.GetNumber();
					vecInfo.m_dwDelay = s.GetNumber();
					vecInfo.m_fLength = s.GetFloat();	
					
					AddPatrolPath( nIndex, vecInfo );
				}	
				
				if( nCount == 1 )
				{
					vecInfoBackup = vecInfo;
				}
				
				nCount = s.GetNumber();
			}

			D3DXVECTOR3	v3Pos1 = v3PosList[0];
			D3DXVECTOR3	v3Pos2 = v3PosList[v3PosList.size()-1];
			D3DXVECTOR3	v3Dir  = v3Pos1 - v3Pos2;

			D3DXVec3Subtract( &v3Dir, &v3Pos1, &v3Pos2 );
			D3DXVec3Normalize( &vecInfoBackup.m_vDir, &v3Dir );		
			vecInfoBackup.m_fLength = D3DXVec3Length( &v3Dir );			

			AddPatrolPath( nIndex, vecInfoBackup );
			
			
			s.GetToken();			
		}
	}	
	else
	{
		return FALSE;
	}
	
	return TRUE;	
}


#endif //__WORLDSERVER

