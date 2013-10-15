#ifndef __AUCTION_H__
#define	__AUCTION_H__

class CUser;
class CAuction
{
public:
	CAuction();
	virtual	~CAuction();

	static	CAuction*	GetInstance( void );

	void	ActionOff( CUser* CUser, BYTE byId, short nItemNum, DWORD dw1, DWORD dw2 );
	void	Bid( int nAuctionId, DWORD dwBid, BOOL bKnockdown );
	void	KnockDown( u_long idPlayer, int nAuctionId );
};

#endif	// __AUCTION_H__
