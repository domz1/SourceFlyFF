#include "StdAfx.h"

#include "dpsrvr.h"
extern	CDPSrvr	g_DPSrvr;

#include "DPAccountClient.h"
#include "user.h"
#include "definetext.h"

//#ifdef _DEBUG
//#define new new( __FILE__, __LINE__ )
//#endif
/*
#ifdef __GIFTBOX0213

CDPAccountClient::CDPAccountClient()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_QUERYGLOBALGIFTBOX, OnQueryGlobalGiftbox );
}

CDPAccountClient::~CDPAccountClient()
{
}

void CDPAccountClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
}

void CDPAccountClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar );

	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );
	
	if( pfn ) {
		( this->*( pfn ) )( ar );
	}
}

void CDPAccountClient::OnQueryGlobalGiftbox( CAr & ar )
{
	u_long idPlayer;
	DWORD dwGiftbox, dwItem, dwObjId;
	int nNum, nQueryGiftbox;
	BOOL bResult;

	ar >> idPlayer >> dwGiftbox >> dwItem >> nNum >> dwObjId >> nQueryGiftbox >> bResult;
	CUser* pUser	= (CUser*)prj.GetUserByID( idPlayer );
	if( IsValidObj( (CObj*)pUser ) )
	{
		CItemElem* pItemElem	= (CItemElem*)pUser->GetItemId( dwObjId );
		if( pItemElem && pItemElem->m_nQueryGiftbox == nQueryGiftbox )
		{
			pItemElem->m_nQueryGiftbox	= 0;
			if( bResult == FALSE )
			{
				if( !CGiftboxMan::GetInstance()->OpenLowest( dwGiftbox, &dwItem, &nNum ) )
				{
					Error( "CGiftboxMan::GetInstance()->OpenLowest: key not found, g: %d, i: %d", dwGiftbox, dwItem );
					return;
				}
			}
			pUser->UpdateItem( pItemElem->m_dwObjId, UI_NUM, pItemElem->m_nItemNum - 1 );
			CItemElem itemElem;
			itemElem.m_dwItemId	= dwItem;
			itemElem.m_nItemNum	= nNum;
			itemElem.m_bCharged		= itemElem.GetProp()->bCharged;
			if( pUser->CreateItem( &itemElem ) )
			{
				pUser->AddDefinedText( TID_GAME_REAPITEM, "\"%s\"", itemElem.GetProp()->szName );
				LogItemInfo aLogItem;
				aLogItem.Action = "E";
				aLogItem.SendName = pUser->GetName();
				aLogItem.RecvName = "GLOBALGIFTBOX";
				aLogItem.WorldId = pUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
				g_DPSrvr.OnLogItem( aLogItem, &itemElem, nNum );
			}
		}
		else
		{
			if( bResult )
				CGiftboxMan::GetInstance()->Restore( this, dwGiftbox, dwItem );
		}
	}
	else
	{
		if( bResult )
			CGiftboxMan::GetInstance()->Restore( this, dwGiftbox, dwItem );
	}
}

CDPAccountClient*	CDPAccountClient::GetInstance( void )
{
	static	CDPAccountClient	sDPAccountClient;
	return	&sDPAccountClient;
}

#endif	// __GIFTBOX0213
  */