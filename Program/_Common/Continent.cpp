// Continent.cpp: implementation of the CContinent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Continent.h"
#include "defineText.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CContinent::CContinent()
{
	Init();
}

CContinent::~CContinent()
{

}

CContinent* CContinent::GetInstance( void )
{
	static CContinent sContinent;
	return &sContinent;	
}

void CContinent::Init()
{
	m_MapCont.clear();
	vector<CPoint> vecMap;

	// 세금이나 비밀의 방에 관여하는 대륙은 마드리갈에만(메인월드) 존재하며 앞으로도로 그럴꺼라고 가정한다.
	const char* szWorldName = "WdMadrigal";				
	CString strDir = MakePath( DIR_WORLD, szWorldName );
	CString strFullFileName = strDir + CString( "\\" ) + CString(szWorldName) + CString(".wld.cnt");

	CScript script;
	if( script.Load( strFullFileName ) == FALSE )
		return;

	int id = 0;
	BOOL bTown = FALSE;

	do{
		script.GetToken( );

		if( script.Token == _T( "Continent" ) )
		{
			script.GetToken();
			if( script.Token == _T( "BEGIN" ) )
			{
				vecMap.clear( );
			}
			else if( script.Token == _T( "END" ) )
			{
				if( !vecMap.empty() )
					vecMap.push_back( vecMap[ 0 ] );

				pair< map< BYTE, vector<CPoint> >::iterator, BOOL > rst;

				if( bTown )
					rst = m_MapContTown.insert( make_pair( id, vecMap ) );
				else					
					rst = m_MapCont.insert( make_pair( id, vecMap ) );
				
				bTown = FALSE;		//reset town

				assert( rst.second );
			}
		}
		else
		if( script.Token == _T( "C_id" ) )
		{
			id = script.GetNumber( );
		}
		else
		if( script.Token == _T( "VERTEX" ) )
		{
			float x = script.GetFloat();
			float y = script.GetFloat();
			float z = script.GetFloat();
			vecMap.push_back( CPoint( (int)( x ), (int)( z ) ) );
		}	
		else
		if( script.Token == _T( "TOWN" ) )
		{
			bTown = script.GetNumber();
		}
		else 
		if( script.Token == _T( "C_useRealData" ) )
		{
			BOOL bReal = script.GetNumber( );
			if( !bReal )
			{
				//클라전용 비쥬얼변화 데이터로만 쓰인다. 클라는 월드 로드시 해당 대륙만 자동로드됨.
				do{
					script.GetToken( );
				}while( script.Token != "END" );		//한대륙 pass 
			}
		} 
	}
	while( script.tok != FINISHED );
}

BOOL CContinent::Point_In_Poly( vector<CPoint> vecMap, CPoint test_point )
{
	int counter = 0;
	int i;
	double xinters;
	CPoint p1,p2;
	
	p1 = vecMap[0];
	for( i=1;i<=(int)( vecMap.size() );i++ )
	{
		p2 = vecMap[i % vecMap.size()];
		
		if (test_point.y > min_cont(p1.y,p2.y))
		{
			if (test_point.y <= max_cont(p1.y,p2.y))
			{
				if (test_point.x <= max_cont(p1.x,p2.x))
				{
					if (p1.y != p2.y)
					{
						xinters = (test_point.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
						
						if (p1.x == p2.x || test_point.x <= xinters)
							counter++;
					}
				}
			}
		}
		p1 = p2;
	}
	
	if (counter % 2 == 0)
		return FALSE;
	else
		return TRUE;
}

#ifndef __DBSERVER
BYTE CContinent::GetContinent( CMover* pMover )
{
	if( !CContinent::IsValidObj( pMover ) )
		return CONT_NODATA;

	switch( pMover->GetWorld()->m_dwWorldID )
	{
		case WI_WORLD_MADRIGAL:
			return GetContinent( pMover->GetPos() );
		case WI_WORLD_ARENA:
		case WI_WORLD_ARENA_1:
			return CONT_DARKON12;
		case WI_WORLD_GUILDWAR :
			return CONT_FLARIS;
	}

	return CONT_NODATA;
}

BYTE CContinent::GetArea( CMover* pMover )
{
	if( !CContinent::IsValidObj( pMover ) )
		return CONT_NODATA;


	return GetArea( GetContinent(pMover) );	
}

BOOL CContinent::IsValidObj( CMover* pMover )
{
	return ( ::IsValidObj( pMover ) && pMover->GetWorld() );
}

#endif // !__DBSERVER

BYTE CContinent::GetContinent( const D3DXVECTOR3& vPos )
{
	CPoint  cpoint;
	vector<CPoint>	vecPoint;

	cpoint.x    = (long)( vPos.x );
	cpoint.y    = (long)( vPos.z );

	for( map< BYTE, vector<CPoint> >::iterator iter = m_MapCont.begin(); iter != m_MapCont.end(); ++iter )
	{
		vecPoint  = iter->second;

		if( Point_In_Poly( vecPoint, cpoint ) )
			return iter->first;
	}

	return CONT_NODATA;
}

BYTE CContinent::GetTown( const D3DXVECTOR3& vPos )
{
	CPoint  cpoint;
	vector<CPoint>	vecPoint;

	cpoint.x    = (long)( vPos.x );
	cpoint.y    = (long)( vPos.z );

	for( map< BYTE, vector<CPoint> >::iterator iter = m_MapContTown.begin(); iter != m_MapContTown.end(); ++iter )
	{
		vecPoint  = iter->second;

		if( Point_In_Poly( vecPoint, cpoint ) )
			return iter->first;
	}

	return CONT_NODATA;
}

BYTE CContinent::GetArea( BYTE nCont )
{
	if( nCont == CONT_NODATA )
		return CONT_NODATA;
	else if( nCont <= CONT_EAST )
		return CONT_EAST;
	else if( nCont >= CONT_WEST )
		return CONT_WEST;

	return CONT_NODATA;
}

CString CContinent::GetContinentName( BYTE nCont )
{
	CString strTemp;
	
	switch( nCont )
	{
		case CONT_FLARIS :
			strTemp = prj.GetText( TID_GAME_CONT_FLARIS );
			break;
		case CONT_SAINTMORNING :
			strTemp = prj.GetText( TID_GAME_CONT_SAINTMORNING );
			break;
		case CONT_RICIS :
			strTemp = prj.GetText( TID_GAME_CONT_RICIS );
			break;
		case CONT_EAST :
			strTemp = prj.GetText( TID_GAME_CONT_EAST );
			break;

		case CONT_DARKON12 :
			strTemp = prj.GetText( TID_GAME_CONT_DARKON12 );
			break;
		case CONT_DARKON3 :
			strTemp = prj.GetText( TID_GAME_CONT_DARKON3 );
			break;
		case CONT_HARMONIN :
			strTemp = prj.GetText( TID_GAME_CONT_HARMONIN );
			break;
		case CONT_WEST :
			strTemp = prj.GetText( TID_GAME_CONT_WEST );
			break;

		case CONT_ALL :
			strTemp = prj.GetText( TID_GAME_CONT_ALL );
			break;
	}

	return strTemp;
}

#ifdef __WORLDSERVER
D3DXVECTOR3 CContinent::GetRevivalPos( BYTE nContinent )
{
	D3DXVECTOR3 vPos;
	
	switch( nContinent )
	{
		case CONT_NODATA :			vPos = D3DXVECTOR3( 6968, 0, 3328 );	break;
		case CONT_FLARIS :			vPos = D3DXVECTOR3( 6968, 0, 3328 );	break;
		case CONT_SAINTMORNING :	vPos = D3DXVECTOR3( 8470, 0, 3635 );	break;
		case CONT_DARKON12 :		vPos = D3DXVECTOR3( 3808, 0, 4455 );	break;
		case CONT_DARKON3 :			vPos = D3DXVECTOR3( 3808, 0, 4455 );	break;
		case CONT_RICIS :			vPos = D3DXVECTOR3( 8470, 0, 3635 );	break;
		case CONT_EAST :			vPos = D3DXVECTOR3( 6959, 0, 3217 );	break;
		case CONT_WEST :			vPos = D3DXVECTOR3( 3824, 0, 4456 );	break;
		
		default :					vPos = D3DXVECTOR3( 6968, 0, 3328 );	break;
	}
	
	return vPos;
}
#endif // __WORLDSERVER

#ifdef __CLIENT
int	CContinent::GetMapNo( CMover* pMover )
{
	if( IsValidObj( pMover ) && pMover->GetWorld()->GetID() == WI_WORLD_MADRIGAL )
		return GetMapNo( pMover->GetPos() );
	return 100;
}

int	CContinent::GetMapNo( const D3DXVECTOR3& vPos )
{
	BYTE nCont = GetContinent( vPos );

	switch( nCont )
	{
		case CONT_FLARIS :			return 5;
		case CONT_SAINTMORNING :	return 4;
		case CONT_DARKON12 :		return 1;
		case CONT_DARKON3 :			return 2;
		case CONT_RICIS :			return 3;
		case CONT_HARMONIN :		return 6;
		case CONT_ESTIA :			return 7;
		case CONT_KAILLUN :			return 8;
#if __VER >= 17
		case CONT_BAHARA :			return 9;
#endif // __VER >= 17
		default:					return 100;
	}
}
#endif // __CLIENT

int CContinent::GetContinentIndex( const char* continentName )
{
	if( strcmp( "Flaris", continentName ) == 0 )
	{
		return CONT_FLARIS;
	}
	else 
	if( strcmp( "Saint", continentName ) == 0 )
	{
		return CONT_SAINTMORNING;
	}
	else
	if( strcmp( "Ricis", continentName ) == 0 )
	{
		return CONT_RICIS;
	}
	else
	if( strcmp( "Darkon12", continentName ) == 0 )
	{
		return CONT_DARKON12;
	}
	else
	if( strcmp( "Darkon3", continentName ) == 0 )
	{
		return CONT_DARKON3;
	}
	else
	if( strcmp( "Harmonin", continentName ) == 0 )
	{
		return CONT_HARMONIN;
	}
	else
	if( strcmp( "Estia_envir", continentName ) == 0 )
	{
		return CONT_ESTIA;
	}
	else
	if( strcmp( "Estia", continentName ) == 0 )
	{
		return CONT_ESTIA;
	}
	else
	if( strcmp( "Darken", continentName ) == 0 )
	{
		return TOWN_DARKEN;
	}
	else
	if( strcmp( "Flarinenosple", continentName ) == 0 )
	{
		return TOWN_FLARINENOSPLE;
	}
	else
	if( strcmp( "Saincity", continentName ) == 0 )
	{
		return TOWN_SAINCITY;
	}
	else
	if( strcmp( "Kaillun", continentName ) == 0 )
	{
		return CONT_KAILLUN;
	}
	else
	if( strcmp( "Eliun", continentName ) == 0 )
	{
		return TOWN_ELIUN;
	}

#if __VER >= 17
	else
	if( strcmp( "Bahara", continentName ) == 0 )
	{
		return CONT_BAHARA;
	}
	else
	if( strcmp( "UpresiaEnt", continentName ) == 0 )
	{
		return CONT_BAHARA;
	}
#endif // __VER >= 17

	return CONT_NODATA;
}
