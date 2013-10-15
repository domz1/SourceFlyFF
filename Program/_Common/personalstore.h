#ifndef __PERSONALSTORE_H__
#define	__PERSONALSTORE_H__

#pragma	once

typedef	struct	_ITEMFORSALE
{
	DWORD dwId;
	int		nRemnant;
	int		nCost;
	_ITEMFORSALE()
	{
		dwId	= NULL_ID;
		nCost	= 0;
	}
}
ITEMFORSALE, *PITEMFORSALE;

#define	MAX_STORE_ITEM	12

class CMover;
class CItemElem;
class CPersonalStore
{
	ITEMFORSALE	m_aItem[MAX_STORE_ITEM];
	CMover*		m_pMover;
public:
//	Constructions
	CPersonalStore( CMover* pMover )	{	m_pMover	= pMover;	}
	~CPersonalStore()	{}
//	Operations
	BOOL	Add( CItemElem* pItemElem, int nIndex, int nNum, int nCost );
	BOOL	Remove( int nIndex );
	BOOL	Sell( CMover* pBuyer, int nIndex, int nNum );
	void	Serialize( CAr & ar );
};
#endif	// __PERSONALSTORE_H__