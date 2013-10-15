#ifndef __COLLECTING_H__
#define	__COLLECTING_H__

typedef	struct	_COLLECTING_ITEM
{
	DWORD	dwItemId;
	DWORD	dwProb;
	_COLLECTING_ITEM()
	{
		dwItemId	= 0;
		dwProb	= 0;
	}
}	COLLECTING_ITEM;

class CCollectingProperty
{
public:
	CCollectingProperty();
	virtual	~CCollectingProperty();
	static	CCollectingProperty*	GetInstance();
	BOOL	LoadScript( LPCTSTR szFile );

	int		GetCool( int nAbilityOption );
	int		GetMaxBattery( void )	{	return m_nMaxBattery;	}
	DWORD	GetItem( void );
	int		GetEnchantProbability( int nAbilityOption );
	int		GetMaxCollectorLevel( void )	{	return m_anEnchant.size();	}

#ifdef __WORLDSERVER
	vector<int>*	GetEnchantProbabilityPtr( void )	{	return &m_anEnchant;		}
	vector<COLLECTING_ITEM>*	GetItemPtr( void )	{	return &m_aItem;	}
#endif	// __WORLDSERVER

private:
	vector<int>	m_anCool;
	vector<int>	m_anEnchant;
	vector<COLLECTING_ITEM>	m_aItem;
	int		m_nMaxBattery;
};
#endif	// __COLLECTING_H__