// FunnyCoin.h: interface for the CFunnyCoin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNNYCOIN_H__1CC36555_32D6_40AD_A74E_8B5DFA2F411F__INCLUDED_)
#define AFX_FUNNYCOIN_H__1CC36555_32D6_40AD_A74E_8B5DFA2F411F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __FUNNY_COIN

#ifdef __WORLDSERVER
//////////////////////////////////////////////////////////////////////
// CFunnyCoin ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class CFunnyCoin  
{
public:
	CFunnyCoin();
	~CFunnyCoin();
	static CFunnyCoin* GetInstance();

	BOOL	DoUseFunnyCoin( CUser* pUser, CItemElem* pItemElem );		// 퍼니코인을 사용한다.
	void	OnFunnyCoinAckUse( CAr & ar );	// 퍼니코인 사용에 대한 응답을 TranServer로 부터 받았다.
		
private:
	BOOL	IsUsable( CUser* pUser );		// 퍼니코인을 상용할 수 있는 Player인가?
	BOOL	IsAckWaitPlayer( DWORD dwPlayerId, BOOL bRemove = FALSE );	// 퍼니코인을 사용한 후 DB응답을 기다리고 있는 플레이어 인가?
																		// bRemove가 TRUE이면 해당 사용자를 리스트에서 제거한다.
	vector<DWORD> m_vecAckWaitPlayer;				// 퍼니코인 사용 후 DB응답을 기다리는 플레이어 목록
};
#endif // __WORLDSERVER


#ifdef __DBSERVER
//////////////////////////////////////////////////////////////////////
// CFunnyCoinDbCtrl //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#include "dbcontroller.h"
class CFunnyCoinDbCtrl : public CDbController
{
public:	
	enum { FC_USE = 0 };

	CFunnyCoinDbCtrl();
	virtual ~CFunnyCoinDbCtrl();

	virtual void Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );

private:
	int InsertFunnyCoin( DWORD dwPlayerId, DWORD dwItemId, SERIALNUMBER dwSerialNumber );	// 사용한 퍼니코인을 DB에 저장한다.
};


//////////////////////////////////////////////////////////////////////
// CFunnyCoinDbMng ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class CFunnyCoinDbMng
{
public:
	CFunnyCoinDbMng();
	~CFunnyCoinDbMng();
	static CFunnyCoinDbMng* GetInstance();

	BOOL PostRequest( int nQuery, BYTE* lpBuf = NULL, int nBufSize = 0, DWORD dwCompletionKey = 0 )
	{ return m_FCDbCtrl.PostRequest( nQuery, lpBuf, nBufSize, dwCompletionKey );	}

private:
	CFunnyCoinDbCtrl	m_FCDbCtrl;
};
#endif // __DBSERVER

#endif __FUNNY_COIN

#endif // !defined(AFX_FUNNYCOIN_H__1CC36555_32D6_40AD_A74E_8B5DFA2F411F__INCLUDED_)
