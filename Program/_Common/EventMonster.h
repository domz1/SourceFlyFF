// EventMonster.h: interface for the CEventMonster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTMONSTER_H__812603BA_A499_4C90_9975_D3C74CF95390__INCLUDED_)
#define AFX_EVENTMONSTER_H__812603BA_A499_4C90_9975_D3C74CF95390__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __EVENT_MONSTER

class CEventMonster  
{
public:	
	struct __EVENTMONSTER
	{
		int nLevel;
		DWORD dwLootTime;
		float fItemDropRange;
		BOOL bPet;
		BOOL bGiftBox;
	};

	CEventMonster();
	virtual ~CEventMonster();
	
	static CEventMonster*	GetInstance( void );

	void	LoadScript();

	BOOL	SetEventMonster( DWORD dwId );
	BOOL	IsEventMonster( DWORD dwId );
	BOOL	IsPetAble();
	BOOL	IsGiftBoxAble();
	DWORD	GetLootTime();
	int		GetLevelGap();
	float	GetItemDropRange();

	
	map< DWORD, __EVENTMONSTER > m_mapEventMonster;
	map< DWORD, __EVENTMONSTER >::iterator m_it;
};
#endif // __EVENT_MONSTER

#endif // !defined(AFX_EVENTMONSTER_H__812603BA_A499_4C90_9975_D3C74CF95390__INCLUDED_)
