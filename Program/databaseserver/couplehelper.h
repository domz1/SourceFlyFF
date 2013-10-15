#ifndef __COUPLEHELPER_H
#define	__COUPLEHELPER_H

#include "dbcontroller.h"
#include "couple.h"
#include "ar.h"

class CCoupleHelper;
class CCoupleController: public CDbController
{
public:
	CCoupleController( CCoupleHelper* pHelper );
	virtual	~CCoupleController();

	BOOL	Restore();
	BOOL	Propose( u_long idProposer, time_t t );
	BOOL	DeletePropose( time_t t );
	BOOL	Couple( u_long idProposer, u_long idTarget );
	BOOL	Decouple( u_long idPlayer );
#if __VER >= 13 // __COUPLE_1202
	BOOL	AddExperience( u_long idPlayer, int nExperience );
#endif	// __COUPLE_1202
	virtual	void	Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );
	virtual	void	OnTimer(); 

private:
	CCoupleHelper*	m_pHelper;
};

typedef map<u_long, time_t>	MUT;
class CCoupleHelper
{
public:
	enum	{	eTransfer, ePropose, eCouple, eDecouple, eClearPropose, eAddExperience	};
private:
	CCoupleHelper();
public:
	virtual	~CCoupleHelper();
	static	CCoupleHelper*	Instance();
	void	Clear();
	void	Serialize( CAr & ar );
	BOOL	Initialize();
	void	Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );
	void	OnTimer();
	BOOL	PostRequest( int nQuery, BYTE* lpBuf = NULL, int nBufSize = 0, DWORD dwCompletionKey = 0 )	{	return m_pController->PostRequest( nQuery, lpBuf, nBufSize, dwCompletionKey );	}
	void	OnTransfer( DPID dpid );
	void	OnPropose( CAr & ar, DPID dpid );
	void	OnCouple( CAr & ar );
	void	OnDecouple( CAr & ar );
	void	OnClearPropose();
#if __VER >= 13 // __COUPLE_1202
	void	OnAddExperience( CAr & ar );
	void	PostItem( CCouple* pCouple );
	void	PostItem( u_long idPlayer, const COUPLE_ITEM& ci, int nLevel );
#endif	// __COUPLE_1202
	BOOL	SetPropose( u_long idPlayer, time_t t );
	time_t	GetPropose( u_long idPlayer );
	void	ProcessPropose();
	bool	AddPropose( u_long idPlayer, time_t t )	{	return m_mapProposes.insert( MUT::value_type( idPlayer, t ) ).second;	}
	void	Couple( CCouple* pCouple )	{	ASSERT( m_pMgr );	m_pMgr->Couple( pCouple );	}
private:
	void	GetGender( CCouple* pCouple, int & nGenderFirst, int & nGenderSecond );
private:
	CCoupleMgr*	m_pMgr;
	CCoupleController* m_pController;
	MUT		m_mapProposes;
};

#endif	// __COUPLEHELPER_H