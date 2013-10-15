// CampusHelper.cpp: implementation of the CCampusHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CampusHelper.h"

#if __VER >= 15 // __CAMPUS

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCampusHelper::CCampusHelper()
{
	m_pCampus = new CCampus;
}

CCampusHelper::~CCampusHelper()
{
	Clear();
}

CCampusHelper* CCampusHelper::GetInstance()
{
	static CCampusHelper sCH;
	return & sCH;
}

void CCampusHelper::Clear()
{
	SAFE_DELETE( m_pCampus );
}

void CCampusHelper::OnUpdateCampus( CAr & ar )
{
	Clear();
	m_pCampus = new CCampus;
	m_pCampus->Serialize( ar );
	g_pPlayer->SetCampusId( m_pCampus->GetCampusId() );
}

void CCampusHelper::OnRemoveCampus( CAr & ar )
{
	u_long idCampus;
	ar >> idCampus;
	if( m_pCampus->GetCampusId() == idCampus )
	{
		Clear();
		g_pPlayer->SetCampusId( 0 );
	}
}

void CCampusHelper::OnUpdateCampusPoint( CAr & ar )
{
	int nCampusPoint;
	ar >> nCampusPoint;
	g_pPlayer->SetCampusPoint( nCampusPoint );
}
#endif // __CAMPUS