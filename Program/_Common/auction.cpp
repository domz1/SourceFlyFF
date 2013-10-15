#include "stdafx.h"
#include "auction.h"

CAuction::CAuction()
{
}

CAuction::~CAuction()
{
}

CAuction* CAuction::GetInstance( void )
{
	static CAuction sAuction;
	return &sAuction;
}

void CAuction::ActionOff( CUser* pUser, BYTE byId, short nItemNum, DWORD dw1, DWORD dw2 )
{
}

void CAuction::Bid( int nAuctionId, DWORD dwBid, BOOL bKnockdown )
{
}

void CAuction::KnockDown( u_long idPlayer, int nAuctionId )
{
}
