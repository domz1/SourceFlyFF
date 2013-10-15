#ifndef __SKILLINFLUENCE_H__
#define	__SKILLINFLUENCE_H__

#define	BUFF_ITEM	0
#define	BUFF_SKILL	1
#define	BUFF_PET	2
#define	BUFF_ITEM2	3
#define	BUFF_EQUIP	4	// 장착류 아이템에 특정 DST(DST_GIFTBOX)에 대해 버프 아이콘만 출력
#define	BUFF_NULL_ID	(WORD)0xFFFF
#define	MAX_SKILLINFLUENCE	64
#define	MAX_SKILLBUFF_COUNT	99

#ifndef __BUFF_1107

#include "ar.h"

typedef	struct	tagSKILLINFLUENCE
{
	WORD	wType;			// 0:아이템 1:스킬 2:기타?
	WORD	wID;			// 아이템이나 스킬의 프로퍼티 ID
	DWORD	dwLevel;		// 스킬 레벨 - 저장
	DWORD	tmCount;		// 남은 시간(카운트).  - 저장
	DWORD	tmTime;			// 시작 타이머.
	BOOL	bEffect;		// 지속효과이펙트를 가지고 있을때 그것이 로딩됐는지.. FALSE면 로딩해야한다.
#ifdef __PVPDEBUFSKILL
	DWORD	dwAttackerID;	// 스킬 시전자 ID
#endif // __PVPDEBUFSKILL
}

SKILLINFLUENCE, * LPSKILLINFLUENCE;

class CMover;
class CSkillInfluence
{
private:
#if __VER < 8 //__CSC_VER8_3
	SKILLINFLUENCE *m_pEmptyNode;	// 비어있는 공간의 인덱스.
#endif //__CSC_VER8_3
	CMover* m_pMover;
//	CRIT_SEC	m_AddRemoveLock;

	void	RemoveSkillInfluence( SKILLINFLUENCE* pSkillInfluence );
	
public:
//	BOOL	IsEmpty();
#if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 	
#ifdef __PVPDEBUFSKILL
	BOOL	RemoveSkillInfluenceFromID( OBJID dwAttackerID );
#endif // __PVPDEBUFSKILL
	BOOL	RemoveSkillInfluence( WORD wType, WORD wID );
	BOOL	RemoveSkillInfluenceState( DWORD dwChrState );
#if __VER >= 11 // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
	BOOL	RemoveSkillInfluenceDestParam( DWORD dwDestParam );
#endif // __MA_VER11_06				// 확율스킬 효과수정 world,neuz

	BOOL    RemoveAllSkillInfluence();
	BOOL	RemoveAllSkillDebuff( void );
	BOOL	RemoveAllSkillBuff( void );
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	BOOL	RemoveAllBuff( void );
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	BOOL	RemoveOneSkillBuff( void );
	
		
#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 
//	Constructions
	CSkillInfluence();
	virtual	~CSkillInfluence();

	SKILLINFLUENCE		m_aSkillInfluence[ MAX_SKILLINFLUENCE ];
	
	LPSKILLINFLUENCE	FindPtr( WORD wType, WORD wID );
	void	Init( void );
	void	Destroy( void ) {}
	
	void	Serialize( CAr & ar );

//	Operations
	void	SetMover( CMover* pMover );
#ifdef __PVPDEBUFSKILL
	BOOL	Set( WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime, DWORD dwAttackerID = NULL_ID );
	BOOL	Set( SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime, DWORD dwAttackerID );
#else // __PVPDEBUFSKILL
	BOOL	Set( WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime );
	BOOL	Set( SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime );
#endif // __PVPDEBUFSKILL
	
	LPSKILLINFLUENCE	Get( int nIdx );
#if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 
	void	Process( void );
#endif // #if !( defined( __CORESERVER ) || defined( __DBSERVER ) ) 
	void	Reset( void );		// 클라로 버프정보를 다시 갱신하도록 타이머를 클리어 시킴.
	BOOL	HasSkill( WORD wType, WORD wID );

#if __VER >= 9	// __PET_0410
	BOOL	HasPet( void )	{	return FindPet() != NULL;	}
	SKILLINFLUENCE *	FindPet( void );
	BOOL	RemovePet( void );
#endif	// __PET_0410

#ifdef __CLIENT
	DWORD	GetDisguise( void );
#endif // __CLIENT
	BOOL	HasLikeItemBuf( DWORD dwItemKind3 );
	void	RemoveLikeItemBuf( DWORD dwItemKind3 );
	SKILLINFLUENCE *Find( WORD wType, WORD wID );
	SKILLINFLUENCE *GetItemBuf( DWORD dwItemKind3 );
	
private:
//	Attributes
	void	Remove( SKILLINFLUENCE *pNode );
	BOOL	LikeItemBuf( DWORD dwItemKind3 );
#if __VER >= 8 //__CSC_VER8_3
	SKILLINFLUENCE* SortSkillArray();
#ifdef __PVPDEBUFSKILL
	BOOL InsertBuff(SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime, DWORD dwAttackerID);
#else // __PVPDEBUFSKILL
	BOOL InsertBuff(SKILLINFLUENCE *pNode, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime );
#endif // __PVPDEBUFSKILL
#endif //__CSC_VER8_3
};

//
inline void CSkillInfluence::SetMover( CMover* pMover )
	{	m_pMover	= pMover;	}

//
inline SKILLINFLUENCE* CSkillInfluence::Find( WORD wType, WORD wID )
{
	int		i = MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	
	while( i-- )
	{
		pNode = pList++;
		if( pNode->wType == wType && pNode->wID == wID )	// 같은걸 찾음.
			return pNode;
	}

	return NULL;
}

#if __VER >= 9	// __PET_0410
inline SKILLINFLUENCE* CSkillInfluence::FindPet( void )
{
	int	i	= MAX_SKILLINFLUENCE;
	SKILLINFLUENCE *pList = m_aSkillInfluence, *pNode;
	while( i-- )
	{
		pNode	= pList++;
		if( pNode->wType == BUFF_PET )
			return pNode;
	}
	return NULL;
}
#endif	// __PET_0410

//
inline BOOL CSkillInfluence::HasSkill( WORD wType, WORD wID )
{
	return Find( wType, wID ) != NULL;
}

//
inline LPSKILLINFLUENCE CSkillInfluence::FindPtr( WORD wType, WORD wID )
{
	return( Find( wType, wID ) );
}

//
inline LPSKILLINFLUENCE CSkillInfluence::Get( int nIdx )
{
	if( nIdx < 0 || nIdx >= MAX_SKILLINFLUENCE )
	{
		Error( "SKILLINFLUENCE::Get() : 범위를 넘어섬 %d", nIdx );
		return( NULL );
	}

	return( m_aSkillInfluence + nIdx );
}

//
inline void CSkillInfluence::Remove( SKILLINFLUENCE *pNode )
{
	pNode->wType = 0;
	pNode->wID = 0;
	pNode->bEffect = 0;
	pNode->tmTime = 0;
	pNode->tmCount = 0;
#if __VER < 8 //__CSC_VER8_3
	m_pEmptyNode = pNode;		// 지운 노드는 비어있으므로 그것을 받아둠.
#endif //__CSC_VER8_3
}

#endif	// __BUFF_1107

#endif	// __SKILLINFLUENCE_H__