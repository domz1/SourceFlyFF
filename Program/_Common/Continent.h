// Continent.h: interface for the CContinent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTINENT_H__90911CD3_CA89_45BF_8632_B274DDDFE707__INCLUDED_)
#define AFX_CONTINENT_H__90911CD3_CA89_45BF_8632_B274DDDFE707__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define min_cont(x,y) (x < y ? x : y)
#define max_cont(x,y) (x > y ? x : y)

#include "ContinentDef.h"

#if __VER >= 12 // __SECRET_ROOM

class CContinent  
{
public:
	CContinent();
	virtual ~CContinent();

	static CContinent*	GetInstance( void );
#ifndef __DBSERVER	
	BYTE GetContinent( CMover* pMover );	// 현재 캐릭터가 위치하고 있는 대륙
	BYTE GetArea( CMover* pMover );			// 현재 캐릭터가 위치하고 있는 대륙의 지역(동부, 서부...)
	static BOOL	IsValidObj( CMover* pMover );
#endif // !__DBSERVER
	CString GetContinentName( BYTE nCont ); 
#ifdef __WORLDSERVER
	D3DXVECTOR3 GetRevivalPos( BYTE nContinent );
#endif // __WORLDSERVER

#if __VER >= 14 // __NEW_CONTINENT
#ifdef __CLIENT
	int	GetMapNo( CMover* pMover );
	int	GetMapNo( const D3DXVECTOR3& vPos );		//gmpbigsun : const 
#endif // __CLIENT
#endif // __NEW_CONTINENT

#if __VER >= 15 // __BS_CHANGING_ENVIR
	int GetContinentIndex( const char* continentName );
#endif

#if __VER >= 16 // __IMPROVE_MAP_SYSTEM
public:
	BYTE GetContinent( const D3DXVECTOR3& vPos );

private:
#else // __IMPROVE_MAP_SYSTEM
private:
	BYTE GetContinent( const D3DXVECTOR3& vPos );
#endif // __IMPROVE_MAP_SYSTEM
	BYTE GetArea( BYTE nCont );

private:
	void Init();
	BOOL Point_In_Poly( vector<CPoint> vecMap, CPoint test_point );
	map< BYTE, vector<CPoint> > m_MapCont;
	map< BYTE, vector<CPoint> > m_MapContTown;
public:
	BYTE GetTown( const D3DXVECTOR3& vPos );		//world pos로 get town id!
};

#endif // __SECRET_ROOM

#endif // !defined(AFX_CONTINENT_H__90911CD3_CA89_45BF_8632_B274DDDFE707__INCLUDED_)
