// CampusDBCtrl.h: interface for the CCampusDBCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMPUSDBCTRL_H__D25D11CF_A207_47BA_A35B_5AD4D63DAAB9__INCLUDED_)
#define AFX_CAMPUSDBCTRL_H__D25D11CF_A207_47BA_A35B_5AD4D63DAAB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 15 // __CAMPUS
#include "dbcontroller.h"
#include "Campus.h"

//////////////////////////////////////////////////////////////////////
// CCampusDBCtrl Construction/Destruction
//////////////////////////////////////////////////////////////////////

enum
{
	CAMPUS_LOAD,
	CAMPUS_SEND,
	CAMPUS_ADD_MEMBER,
	CAMPUS_REMOVE_MEMBER,
	CAMPUS_UPDATE_POINT,
};

class CCampusDBCtrl : public CDbController
{
public:
	
	CCampusDBCtrl();
	virtual ~CCampusDBCtrl();

	virtual void Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );

private:
	void	CreateLogQuery();

	void	AddCampusMember( CAr & ar );
	void	RemoveCampusMember( CAr & ar );
	void	UpdateCampusPoint( CAr & ar );

	void	LoadAllCampus();
	void	SendAllCampus( DPID dpId );
	void	InsertCampus( u_long idCampus );
	void	DeleteCampus( u_long idCampus );
	void	InsertCampusMember( u_long idCampus, u_long idPlayer, int nMemberLv );
	void	DeleteCampusMember( u_long idPlayer, int nMemberLv );
	int		UpdateCampusPoint( u_long idPlayer, int nCampusPoint );
	void	UpdateCampusId( u_long idPlayer, u_long idCampus );

	void	LogUpdateCampusMember( u_long idCampus, u_long idMaster, u_long idPupil, char chState );
	void	LogUpdateCampusPoint( u_long idPlayer, int nPrevPoint, int nCurrPoint, char chState );

	CQuery* m_pLogQuery;
};

//////////////////////////////////////////////////////////////////////
// CCampusHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

class CCampusHelper
{
public:
	CCampusHelper();
	~CCampusHelper();
	static CCampusHelper* GetInstance();

	BOOL PostRequest( int nQuery, BYTE* lpBuf = NULL, int nBufSize = 0, DWORD dwCompletionKey = 0 )
	{ return m_CampusDBCtrl.PostRequest( nQuery, lpBuf, nBufSize, dwCompletionKey );	}

	void	Serialize( CAr & ar )	{	m_pCampusMng->Serialize( ar );	}
	u_long	AddCampus( CCampus* pCampus )	{	return m_pCampusMng->AddCampus( pCampus );	}
	BOOL	RemoveCampus( u_long idCampus )	{	return m_pCampusMng->RemoveCampus( idCampus );	}
	BOOL	IsEmpty()	{	return m_pCampusMng->IsEmpty();	}
	
	CCampus*	GetCampus( u_long idCampus )	{	return m_pCampusMng->GetCampus( idCampus );	}

	void	Clear()	{	m_pCampusMng->Clear();	}
	bool	AddPlayerId2CampusId( u_long idPlayer, u_long idCampus )	{	return m_pCampusMng->AddPlayerId2CampusId( idPlayer, idCampus );	}
	void	RemovePlayerId2CampusId( u_long idPlayer )	{	m_pCampusMng->RemovePlayerId2CampusId( idPlayer );	}
	u_long	GetCampusIdByPlayerId( u_long idPlayer )	{	return m_pCampusMng->GetCampusIdByPlayerId( idPlayer );	}

	int		GetMaxPupilNum( int nCampusPoint );

private:
	CCampusDBCtrl	m_CampusDBCtrl;
	CCampusMng*		m_pCampusMng;
};

#endif // __CAMPUS
#endif // !defined(AFX_CAMPUSDBCTRL_H__D25D11CF_A207_47BA_A35B_5AD4D63DAAB9__INCLUDED_)
