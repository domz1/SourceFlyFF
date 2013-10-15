// CampusHelper.h: interface for the CCampusHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMPUSHELPER_H__8C0DE44B_A33F_472A_8502_1966B9EBA8EB__INCLUDED_)
#define AFX_CAMPUSHELPER_H__8C0DE44B_A33F_472A_8502_1966B9EBA8EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 15 // __CAMPUS
#include "Campus.h"

class CCampusHelper  
{
public:
	CCampusHelper();
	~CCampusHelper();

	static CCampusHelper* GetInstance();

	void	Serialize( CAr & ar )	{	m_pCampus->Serialize( ar );	}
	CCampus*	GetCampus()	{	return m_pCampus;	}

	void	Clear();
	void	OnUpdateCampus( CAr & ar );
	void	OnRemoveCampus( CAr & ar );
	void	OnUpdateCampusPoint( CAr & ar );

private:
	CCampus* m_pCampus;
};

#endif // __CAMPUS
#endif // !defined(AFX_CAMPUSHELPER_H__8C0DE44B_A33F_472A_8502_1966B9EBA8EB__INCLUDED_)
