// NeuzEnemy.h: interface for the CNeuzEnemy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEUZENEMY_H__69032C31_F342_4913_A4D4_1B41A82FEB6D__INCLUDED_)
#define AFX_NEUZENEMY_H__69032C31_F342_4913_A4D4_1B41A82FEB6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef map< OBJID, DWORD > MAP_ENEMY;

class CNeuzEnemy  
{
private:
	MAP_ENEMY	m_Enemy;

public:
	DWORD Add( OBJID objid );
	void Remove( OBJID objid );
	DWORD Find( OBJID objid );
	BOOL IsPKing( OBJID objid );
	void Clear();
	void CheckInvalid();
	
	CNeuzEnemy();
	virtual ~CNeuzEnemy();

};

#endif // !defined(AFX_NEUZENEMY_H__69032C31_F342_4913_A4D4_1B41A82FEB6D__INCLUDED_)
