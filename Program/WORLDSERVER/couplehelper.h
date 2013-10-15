#ifndef __COUPLEHELPER_H
#define	__COUPLEHELPER_H

#include "ar.h"
#include "couple.h"
#include "playerdata.h"

class CCoupleMgr;
class CDPDatabaseClient;
class CDPSrvr;
class CCoupleHelper
{
private:
	CCoupleHelper();
public:
	virtual	~CCoupleHelper();
	static	CCoupleHelper*	Instance();
	BOOL	Initialize( CDPDatabaseClient* pdpClient, CDPSrvr* pdpServer );
	void	Clear();
	void	OnPropose( CUser* pUser, const char* szPlayer );
	void	OnProposeResult( CAr & ar );
	void	OnRefuse( CUser* pUser );
	void	OnCouple( CUser* pUser );
	void	OnCoupleResult( CAr & ar );
	void	OnDecouple( CUser* pUser );
	void	OnDecoupleResult( CAr & ar );
	void	Serialize( CAr & ar )	{	ASSERT( m_pMgr );	m_pMgr->Serialize( ar );	}
	CCouple* GetCouple( u_long idPlayer )	{	ASSERT( m_pMgr );	return m_pMgr->GetCouple( idPlayer );	}
#if __VER >= 13 // __COUPLE_1202
	void	OnAddCoupleExperience( CAr & ar );
#endif	// __COUPLE_1202
	void	OnUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData );
private:
	void	PlayProposeAnimation( CUser* pProposer, CUser* pTarget );
private:
	CCoupleMgr*	m_pMgr;
	CDPDatabaseClient*	m_pdpClient;
	CDPSrvr*	m_pdpServer;
};

#endif	// __COUPLEHELPER_H
