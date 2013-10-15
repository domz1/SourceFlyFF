#ifndef __BUFF_H
#define	__BUFF_H

#ifdef __BUFF_1107

#include "ar.h"

#include <map>
using	namespace	std;

#define	RBF_COMMON	(DWORD)0x00000001
#define	RBF_IK3	(DWORD)0x00000002
#define	RBF_GOODSKILL	(DWORD)0x00000004
#define	RBF_CHRSTATE	(DWORD)0x00000008
#define	RBF_DSTPARAM	(DWORD)0x00000010
#define	RBF_DEBUF		(DWORD)0x00000020
#define	RBF_ATTACKER	(DWORD)0x00000040
#define	RBF_UNCONDITIONAL	(DWORD)0x01000000
#define	RBF_ONCE	(DWORD)0x10000000

typedef	DWORD	OBJID;

class CBuffMgr;
class IBuff
{
public:
	IBuff();
	virtual	~IBuff()	{}
	////////////////////////////////////////
	WORD	GetType()	{	return m_wType;		}
	WORD	GetId()		{	return m_wId;	}
	DWORD	GetLevel()	{	return m_dwLevel;	}
	DWORD	GetTotal()	{	return m_tmTotal;	}
	void	SetType( WORD wType )	{	m_wType	= wType;	}
	void	SetId( WORD wId )	{	m_wId	= wId;	}
	void	SetTotal( DWORD tmTotal );
	void	SetLevel( DWORD dwLevel )	{	m_dwLevel	= dwLevel;	}
	void	SetAttacker( OBJID oiAttacker )		{	m_oiAttacker	= oiAttacker;	}
	DWORD	GetInst()		{	return m_tmInst;	}
	void	SetInst( DWORD tmInst )	{	m_tmInst	= tmInst;	}
	virtual	void	Serialize( CAr & ar, CMover* pMover );
	virtual	void	SerializeLevel( CAr & ar );
	virtual	IBuff& operator=( const IBuff & buff );
	DWORD	GetRemain();
	void	SetRemove()	{	m_bRemove	= TRUE;		}
	BOOL	GetRemove()		{	return m_bRemove;	}
	////////////////////////////////////////
#ifndef __DBSERVER
public:
	virtual BOOL	Process( CMover* pMover, DWORD tmCurrent );
	virtual	BOOL	IsIk1( DWORD dwIk1 )	{	return FALSE;	}
	virtual	BOOL	IsIk3( DWORD dwIk3 )	{	return FALSE;	}
	virtual	BOOL	IsAddable( CMover* pMover );
	virtual	void	AddTotal( DWORD tmTotal );
	virtual	ItemProp*	GetProp()	{	return NULL;	}
	virtual	void	Release( CBuffMgr* pBuffMgr )	{}
	virtual	BOOL	IsRemovable( CMover* pMover )	{	return TRUE;	}
	virtual	DWORD	GetDisguise()	{	return NULL_ID;		}
	OBJID	GetAttacker()	{	return m_oiAttacker;	}
	virtual	BOOL	IsCommon()	{	return TRUE;	}
	virtual	BOOL	IsGood( CMover* pMover )	{	return FALSE;	}
	virtual	BOOL	HasDstParam( CMover*, DWORD dwDstParam )	{	return FALSE;	}
	virtual	BOOL	HasChrState( CMover*, DWORD dwChrState )	{	return FALSE;	}
	virtual	BOOL	IsDebuf( CMover* pMover )	{	return FALSE;	}
	virtual	BOOL	IsAttacker( CMover*, OBJID oiAttacker )	{	return FALSE;	}
protected:
	virtual	BOOL	IsInvalid( CMover* )	{	return FALSE;	}
	virtual	void	Apply( CMover* pMover )		{}
	virtual	BOOL	Timeover( CMover* pMover, DWORD tmCurrent );	
private:
	BOOL	Expire( CMover* pMover, DWORD tmCurrent );
#endif	// __DBSERVER
	////////////////////////////////////////
#ifdef __WORLDSERVER
public:
	virtual	void	OnExpire( CMover* pMover )	{}
#endif	// __WORLDSERVER
	////////////////////////////////////////
#ifdef __CLIENT
public:
	BOOL	HasSFX()	{	return m_bSFX;	}
	void	SetSFX()	{	m_bSFX	= TRUE;	}
	virtual	BOOL	IsCommercial()	{	return FALSE;	}
private:
	void	CreateSFX( CMover* pMover );
#endif	// __CLIENT
	////////////////////////////////////////
private:
	WORD	m_wType;
	WORD	m_wId;
	DWORD	m_dwLevel;
	DWORD	m_tmTotal;
	DWORD	m_tmInst;
#ifdef __CLIENT
	BOOL	m_bSFX;
#endif	// __CLIENT
	OBJID	m_oiAttacker;
	BOOL	m_bRemove;
};

#ifndef __DBSERVER
class IBuffItemBase : public IBuff
{
public:
	IBuffItemBase()		{}
	virtual	~IBuffItemBase()	{}
	virtual	ItemProp*	GetProp();
	virtual void Release( CBuffMgr * pBuffMgr );
	virtual	BOOL	IsCommon();
	virtual	BOOL	HasChrState( CMover*, DWORD dwChrState );
	virtual	BOOL	IsDebuf( CMover* pMover );
	////////////////////////////////////////
#ifdef __CLIENT
	BOOL	IsCommercial();
#endif	// __CLIENT
};

class CBuffItem : public IBuffItemBase
{
public:
	CBuffItem()		{}
	virtual	~CBuffItem()	{}
	virtual	void	AddTotal( DWORD tmTotal );
	virtual	BOOL	IsRemovable( CMover* pMover );
	virtual	DWORD	GetDisguise();
	virtual	BOOL	IsIk1( DWORD dwIk1 );
	virtual	BOOL	IsIk3( DWORD dwIk3 );
protected:
	virtual	void	Apply( CMover* pMover );
	////////////////////////////////////////
#ifdef __WORLDSERVER
public:
	virtual	void	OnExpire( CMover* pMover );
#endif	// __WORLDSERVER
};

class CBuffSkill : public IBuff
{
public:
	CBuffSkill()	{}
	virtual	~CBuffSkill()	{}
	virtual	ItemProp*	GetProp();
	virtual	BOOL	IsAddable( CMover* pMover );
	virtual void Release( CBuffMgr * pBuffMgr );
	virtual	BOOL	IsRemovable( CMover* pMover );
	virtual	BOOL	IsGood( CMover* pMover );
	virtual	BOOL	HasDstParam( CMover* pMover, DWORD dwDstParam );
	virtual	BOOL	HasChrState( CMover* pMover, DWORD dwChrState );
	virtual	BOOL	IsDebuf( CMover* pMover );
	virtual	BOOL	IsAttacker( CMover* pMover, OBJID oiAttacker );
protected:
	virtual	BOOL	IsInvalid( CMover* pMover );
	virtual	void	Apply( CMover* pMover );
	////////////////////////////////////////
#ifdef __WORLDSERVER
public:
	virtual	void	OnExpire( CMover* pMover );
#endif	// __WORLDSERVER
};

class CBuffPet : public IBuff
{
public:
	CBuffPet()	{}
	virtual	~CBuffPet()	{}
	virtual	BOOL	IsRemovable( CMover* pMover )	{	return FALSE;	}
	virtual	BOOL	IsCommon()	{	return FALSE;	}
protected:
	virtual	void	Apply( CMover* pMover );
	virtual void Release( CBuffMgr * pBuffMgr );
};

class CBuffItem2 : public IBuffItemBase
{
public:
	CBuffItem2()	{}
	virtual	~CBuffItem2()	{}
	virtual	void	SerializeLevel( CAr & ar );
	virtual	BOOL	Timeover( CMover* pMover, DWORD tmCurrent );	
};

class CBuffEquip : public IBuff
{
public:
	CBuffEquip()	{}
	virtual	~CBuffEquip()	{}
	virtual	BOOL	IsRemovable( CMover* pMover )	{	return FALSE;	}
	virtual	BOOL	IsCommon()	{	return FALSE;	}
};
#endif	// __DBSERVER

typedef	map<DWORD, IBuff*>	MAPBUFF;

#ifdef __CLIENT
class	CWndWorld;
#endif	// __CLIENT
class CMover;
class CBuffMgr
{
public:
	CBuffMgr( CMover* pMover );
	virtual	~CBuffMgr();
	void	Clear();
	CMover*	GetMover()	{	return const_cast<CMover*>( m_pMover );	}
	void	Serialize( CAr & ar );
	CBuffMgr&	operator=( CBuffMgr & bm );
	virtual	IBuff*	CreateBuff( WORD wType );
protected:
	virtual	DWORD	GetCurrentTime()	{	return timeGetTime();	}
#ifdef __DBSERVER
public:
	bool	Add( IBuff* pBuff )	{	return m_mapBuffs.insert( MAPBUFF::value_type( MAKELONG( pBuff->GetId(), pBuff->GetType() ), pBuff ) ).second;	}
#else	// __DBSERVER
private:
	bool	Add( IBuff* pBuff )	{	return m_mapBuffs.insert( MAPBUFF::value_type( MAKELONG( pBuff->GetId(), pBuff->GetType() ), pBuff ) ).second;	}
#endif	// __DBSERVER
	////////////////////////////////////////
#ifndef __DBSERVER
public:
	void	Process();
	BOOL	HasBuff( WORD wType, WORD wId );
	void	RemoveBuff( IBuff* pBuff, BOOL bFake );
	void	RemoveBuff( WORD wType, WORD wId, BOOL bFake = TRUE );
	BOOL	HasBuffByIk3( DWORD dwIk3 );
	BOOL	HasPet()	{	return GetBuffPet() != NULL;	}
	void	RemoveBuffPet();
	IBuff*	GetBuffPet();
	
	IBuff*	GetBuff( WORD wType, WORD wId );
	IBuff*	GetBuffByIk3( DWORD dwIk3 );
	void	RemoveBuffs( DWORD dwFlags, DWORD dwParam = 0 );
	void	ClearInst();
	DWORD	GetDisguise();
	BOOL	AddBuff( WORD wType, WORD wId, DWORD dwLevel, DWORD tmTotal, OBJID oiAttacker = 0xFFFFFFFF );
protected:
	BOOL	AddBuff( IBuff* pBuff );
private:
	BOOL	Overwrite( IBuff* pBuff );
	void	PrepareBS( IBuff* pBuff );
	BOOL	IsBSFull();
	IBuff*	GetFirstBS();
#endif	// __DBSERVER
	////////////////////////////////////////
#ifdef __WORLDSERVER
public:
	void	RemoveBuffPet( IBuff* pBuff );
#endif	// __WORLDSERVER
	////////////////////////////////////////
#ifdef __CLIENT
public:
	int		GetCommercialCount();
#endif	// __CLIENT
	////////////////////////////////////////
#ifdef __DBSERVER
public:
	void	ToString( char* szString );
#endif	// __DBSERVER
	////////////////////////////////////////
private:
	MAPBUFF	m_mapBuffs;
	const CMover* m_pMover;
#ifdef __CLIENT
	friend	class	CWndCommItemCtrl;
	friend	class	CWndBuffStatus;
	friend	class	CWndMgr;
	friend	class	CWndWorld;
	friend	class	CMover;
#endif	// __CLIENT
#ifdef __NEW_STACKABLE_AMPS
#ifdef __WORLDSERVER
	friend	class	CMover;
#endif // __WORLDSERVER
#endif // __NEW_STACKABLE_AMPS
// chipi_090217
	size_t	GetRemoveBuffSize();
};

#endif	// __BUFF_1107

#endif	// __BUFF_H
