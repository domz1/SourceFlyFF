#ifndef __AI_PET_H
#define __AI_PET_H

#include "..\_AIInterface\AIInterface.h"
#include "mempooler.h"
#include "Mover.h"

class CAIPet : public CAIInterface
{
private:
	OBJID m_idOwner;		// 주인님
	BOOL	m_bLootMove;				// 루팅하러 가는중.
	DWORD	m_idLootItem;				// 루팅할 아이템.
	int		m_nState;					// 현재 상태.
	
	void MoveToDst(	OBJID idTarget );
	void MoveToDst(	D3DXVECTOR3 vDst );

	BOOL  MoveProcessIdle( const AIMSG & msg );
	BOOL  MoveProcessRage( const AIMSG & msg );

	void Init( void );
	void Destroy( void );
	
public:
	CAIPet();
	CAIPet( CObj* pObj );
	virtual ~CAIPet();

	virtual void InitAI();

	void	SetOwner( OBJID idOwner ) { m_idOwner = idOwner; }
	BOOL	GetLootMove( void ) { return m_bLootMove; }
	DWORD	GetLootItem( void ) { return m_idLootItem; }
	int		GetState( void ) { return m_nState; }
	BOOL	StateInit( const AIMSG & msg );
	BOOL	StateIdle( const AIMSG & msg );
	BOOL	StateRage( const AIMSG & msg );

	BOOL SubItemLoot( void );

#if __VER >= 12 // __PET_0519
	// 주인과 먹펫 아이템 설정, 설정 시 각성 효과 적용
	void	SetItem( CMover* pPlayer, CItemElem* pItem );
	// 주인으로부터 먹펫 각성 효과 제거
	void	ResetItem( void );
	// 해당 먹펫 아이템 식별자 반환
	OBJID	GetPetItemId( void )	{	return m_idPetItem;		}
private:
	void	SetSkillId( DWORD dwSkill )		{ m_dwSkillId = dwSkill; }	// 사용 예 없음
	DWORD	GetSkillId()	{	return m_dwSkillId;	}	// 사용 예 없음
	void	SetSkill( CMover* pPlayer, ItemProp* pProp );	// 사용 예 없음
	void	ResetSkill( CMover* pPlayer );	// 사용 예 없음
	DWORD	m_dwSkillId;	// 사용 예 없음
	OBJID	m_idPetItem;	// 먹펫 아이템 식별자
#endif	// __PET_0519
private:
	BOOL	NotOwnedPetInactivated();

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CAIPet>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CAIPet::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CAIPet::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CAIPet::m_pPool->Free( (CAIPet*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CAIPet::m_pPool->Free( (CAIPet*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
	
	DECLARE_AISTATE()
};

#endif

