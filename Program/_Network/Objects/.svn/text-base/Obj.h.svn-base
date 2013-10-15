#ifndef __OBJ_H__
#define __OBJ_H__

#pragma once

#include <d3dx9math.h>
#include "Define.h"
#include "Data.h"
#include "Ar.h"
#include <string>
#include <map>
using	namespace	std;

#include "SkillInfluence.h"
#include "UserTaskBar.h"

#if defined(__DBSERVER) || defined(__VPW)
  #include "..\DatabaseServer\Project.h"
  extern	CProject	prj;
#endif	//__DBSERVER || __VPW

#if defined(__DBSERVER)
#include "dbmanager.h"
#endif	//

#ifdef __RT_1025
#include "rtmessenger.h"
#else	// __RT_1025
#include "messenger.h"
#endif	// __RT_1025

#ifdef __BUFF_1107
#include "buff.h"
#endif	// __BUFF_1107

#if __VER >= 9	// __PET_0410
#include "pet.h"
#endif	// __PET_0410

#include "Piercing.h"

typedef	DWORD	OBJID;

#define ITYPE_ITEM      0
#define ITYPE_CARD      1
#define ITYPE_CUBE      2 
#define ITYPE_PET		3

#define	MAX_INVENTORY	42

enum 
{
	METHOD_NONE, METHOD_EXCLUDE_ITEM
};


#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
#define MAX_SKILL_JOB	( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL )
#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
#define MAX_SKILL_JOB	( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL )
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CObj
{
public:
	DWORD	m_dwType;
	DWORD	m_dwIndex;
	D3DXVECTOR3		m_vScale;
	DWORD	m_dwMotion;
	D3DXVECTOR3		m_vPos;
	FLOAT	m_fAngle;
public:
//	Constructions
	CObj();
	virtual	~CObj()		{}
//	Operations
	virtual	void	Serialize( CAr & ar );
public:
static int	m_nMethod;
static int	GetMethod( void )	{	return m_nMethod;	}

#ifdef __LAYER_1015
protected:
	int		m_nLayer;
public:
	void	SetLayer( int nLayer )	{	m_nLayer	= nLayer;	}
	int		GetLayer()	{	return m_nLayer;	}
#endif	// __LAYER_1015
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CCtrl : public CObj
{
public:
	OBJID	m_objid;

public:
//	Constructions
	CCtrl();
	virtual	~CCtrl()	{}
//	Operations
	void	SetId( OBJID objid )	{	m_objid		= objid;	}
	OBJID	GetId( void )	{	return m_objid;	}
	virtual	void	Serialize( CAr & ar );
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CItemBase
{
public:
	OBJID       m_dwObjId ;		// 아이템 컨테이너에서 몇번째 
	DWORD		m_dwItemId;		// IID_ 값 
	TCHAR       m_szItemText[ 32 ];

	DWORD		m_dwObjIndex;	// 인벤토리에서 몇번째 
private:
	SERIALNUMBER	m_liSerialNumber;
public:
//	Constructions
	CItemBase();
	virtual ~CItemBase()	{}

//	Operations
	BOOL	IsEmpty()	{	return m_dwItemId ? FALSE : TRUE;	}
	virtual	void	Empty()
	{
		m_szItemText[0] = '\0';
		m_dwItemId      = 0;
	}
	virtual	CItemBase&	operator = ( CItemBase & ib );
	virtual void	Serialize( CAr & ar );

	SERIALNUMBER	GetSerialNumber( void )	{	return m_liSerialNumber;	}
	void	SetSerialNumber( SERIALNUMBER liSerialNumber )	{	m_liSerialNumber	= liSerialNumber;	}
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
class CItemElem : public CItemBase
{
private:
	CPiercing	m_piercing;
public:
	void	SetPiercingSize( int nSize )	{	m_piercing.SetPiercingSize( nSize ); }
	int		GetPiercingSize()	{	return m_piercing.GetPiercingSize();		}
	void	SetPiercingItem( int nth, DWORD dwItem)	{	m_piercing.SetPiercingItem( nth, dwItem );	}
	DWORD	GetPiercingItem( int nth )	{	return m_piercing.GetPiercingItem( nth );	}
	void	CopyPiercing( const CPiercing & piercing )		{	m_piercing	= piercing;		}
	void	SerializePiercing( CAr & ar )	{	m_piercing.Serialize( ar );		}
	BOOL	IsPiercedItem()	{ return m_piercing.IsPiercedItem(); }
#ifdef __DBSERVER
#if __VER >= 12 // __EXT_PIERCING
	void	SetUltimatePiercingSize( int nSize )	{	m_piercing.SetUltimatePiercingSize( nSize );	}
	int		GetUltimatePiercingSize()	{	return m_piercing.GetUltimatePiercingSize();		}
	void	SetUltimatePiercingItem( int nth, DWORD dwItem )	{	m_piercing.SetUltimatePiercingItem( nth, dwItem );	}
	DWORD	GetUltimatePiercingItem( int nth )	{	return m_piercing.GetUltimatePiercingItem( nth );	}

	ItemProp*	GetProp()	{	return prj.GetItemProp( m_dwItemId );	}
	BOOL	IsPierceAble( DWORD dwTargetItemKind3 = NULL_ID, BOOL bSize = FALSE );
#endif // __EXT_PIERCING
#endif // __DBSERVER

private:
	int		m_nAbilityOption;

public:
	enum	{	expired	= 0x01,	binds	= 0x02,	};
	
	int		m_nRepair;
	BYTE	m_nRepairNumber;
	BYTE	m_byFlag;
	short	m_nItemNum; 
	int		m_nHitPoint;
	DWORD	m_idGuild;
	BYTE	m_bItemResist;				// 어느 속성 인가?
	int		m_nResistAbilityOption;		// 속성 추가 능력치 가변 옵션
	int		m_nResistSMItemId;

//	PIERCINGINFO	m_piercingInfo;
	BOOL	m_bCharged;					// 상용화 아이템인지 확인
	DWORD	m_dwKeepTime;				// 지속시간
#if __VER >= 11 // __SYS_IDENTIFY
	// 비트별 연산
	// 12|12|16|16|8	= 64
private:
	__int64		m_iRandomOptItemId;
public:
	__int64		GetRandomOptItemId( void )	{	return m_iRandomOptItemId;	}
	void	SetRandomOptItemId( __int64 iRandomOptItemId )	{	m_iRandomOptItemId	= iRandomOptItemId;	}
	int		GetRandomOpt( void )	{	return static_cast<int>( m_iRandomOptItemId & 0x00000000000000FF );	}
#else	// __SYS_IDENTIFY
private:
	int			m_nRandomOptItemId;
public:
	int		GetRandomOptItemId( void )	{	return m_nRandomOptItemId;	}
	void	SetRandomOptItemId( int nRandomOptItemId )	{	m_nRandomOptItemId	= nRandomOptItemId;	}
	int		GetRandomOpt( void )	{	return m_nRandomOptItemId;	}
#endif	// __SYS_IDENTIFY

#if __VER >= 9	// __PET_0410
	CPet*	m_pPet;
#endif	// __PET_0410

public:
//	Constructions
	CItemElem();
	virtual	~CItemElem();

//	Operations
	int		GetOption( void )			{ return m_nAbilityOption;	}
	void	SetOption( int nOption )	{ m_nAbilityOption = nOption; }
	BOOL	IsFlag( BYTE byFlag )		{ return ( m_byFlag & byFlag ) ? TRUE : FALSE; }
	BOOL	IsInvalid( void );

	virtual	void	Empty()
	{
		CItemBase::Empty();
#if __VER >= 9
		SAFE_DELETE( m_pPet );
#endif	// __PET_0410
	}

	virtual	CItemElem&	operator = ( CItemElem & ie );
	virtual	void	Serialize( CAr & ar );

#ifdef __DBSERVER
#if __VER >= 15 // __PETVIS
	BOOL	IsVisPet()	{ return ( GetProp() && GetProp()->IsVisPet() ) || IsTransformVisPet() ; }
	void	SetVisKeepTimeSize( int nSize )				{ m_piercing.SetVisKeepTimeSize( nSize ); }
	void	SetVisKeepTime( int nth, time_t tmKeep )	{ m_piercing.SetVisKeepTime( nth, tmKeep ); }
	time_t	GetVisKeepTime( int nth )					{ return m_piercing.GetVisKeepTime( nth ); }
	BOOL	IsTransformVisPet() { return GetProp() && GetProp()->dwItemKind3 == IK3_PET && m_bTranformVisPet; }

	BOOL	m_bTranformVisPet;
#endif // __PETVIS
#endif // __DBSERVER
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
typedef	struct	tagSKILL
{
	DWORD	dwSkill;
	DWORD	dwLevel;
	void Init()	{};
}
SKILL, *LPSKILL;


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

template <class T> class CItemContainer  
{
public:
	LPDWORD	m_apIndex;
	DWORD	m_dwIndexNum;
	T*	m_apItem;
	DWORD	m_dwItemMax;

public:
//	Constructions
	CItemContainer();
//	CItemContainer( DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra = 0xffffffff );
	virtual	~CItemContainer();
//	Operations
	void Clear();
	void	SetItemContainer( DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra = 0xffffffff );
//	T*	Add( DWORD dwItemId );

	BOOL	Add( T* pElem, BYTE* pnId = NULL, short* pnNum = NULL, BYTE* pnCount = NULL );
#if __VER >= 11 // __SYS_IDENTIFY
#ifdef __SEND_ITEM_ULTIMATE
	BOOL	Add( DWORD dwItemId, short nNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist = 0, int nResistAbilityOption = 0, BYTE* pnId = NULL, short* pnNum = NULL, BYTE* pnCount = NULL, BOOL bCharged = FALSE, int nPiercedSize = 0, DWORD dwItemId0 = 0, DWORD dwItemId1 = 0, DWORD dwItemId2 = 0, DWORD dwItemId3 = 0, DWORD dwItemId4 = 0, DWORD dwKeepTime = 0, __int64 iRandomOptItemId = 0 );
#else // __SEND_ITEM_ULTIMATE
	BOOL	Add( DWORD dwItemId, short nNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist = 0, int nResistAbilityOption = 0, BYTE* pnId = NULL, short* pnNum = NULL, BYTE* pnCount = NULL, BOOL bCharged = FALSE, int nPiercedSize = 0, DWORD dwItemId0 = 0, DWORD dwItemId1 = 0, DWORD dwItemId2 = 0, DWORD dwItemId3 = 0, DWORD dwKeepTime = 0, __int64 iRandomOptItemId = 0 );
#endif // __SEND_ITEM_ULTIMATE
#else	// __SYS_IDENTIFY
#ifdef __SEND_ITEM_ULTIMATE
	BOOL	Add( DWORD dwItemId, short nNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist = 0, int nResistAbilityOption = 0, BYTE* pnId = NULL, short* pnNum = NULL, BYTE* pnCount = NULL, BOOL bCharged = FALSE, int nPiercedSize = 0, DWORD dwItemId0 = 0, DWORD dwItemId1 = 0, DWORD dwItemId2 = 0, DWORD dwItemId3 = 0, DWORD dwItemId4 = 0, DWORD dwKeepTime = 0, int nRandomOptItemId = 0 );
#else // __SEND_ITEM_ULTIMATE
	BOOL	Add( DWORD dwItemId, short nNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist = 0, int nResistAbilityOption = 0, BYTE* pnId = NULL, short* pnNum = NULL, BYTE* pnCount = NULL, BOOL bCharged = FALSE, int nPiercedSize = 0, DWORD dwItemId0 = 0, DWORD dwItemId1 = 0, DWORD dwItemId2 = 0, DWORD dwItemId3 = 0, DWORD dwKeepTime = 0, int nRandomOptItemId = 0 );
#endif // __SEND_ITEM_ULTIMATE
#endif	// __SYS_IDENTIFY
	BOOL	IsFull( ItemProp* pItemProp, short nNum, BOOL bCharged );

	T*	GetAt( DWORD dwIndex );
	T*	GetAtId( OBJID dwObjId );
	T*	GetItem( ItemProp * pItemProp, int nAbilityOption );
	T*  GetItem( ItemProp * pItemProp, int nAbilityOption, int nItemResist, int nResistAbilityOption );
	BOOL IsEquip( DWORD dwObjId );
	void RemoveAt( DWORD dwIndex );
	void	RemoveAtId( OBJID dwObjId );

	T*	GetEquip( DWORD dwIndex );
	BOOL	UnEquip( DWORD dwIndex );
	void	DoEquip( DWORD dwSrcIndex, DWORD dwDstIndex );
	void	Copy( CItemContainer<T> & rItemContainer );
	void	Serialize( CAr & ar );
};

template <class T> CItemContainer<T>::CItemContainer()
{
	m_apItem = NULL;
	m_apIndex = NULL;
}

/*
template <class T> CItemContainer<T>::CItemContainer( DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra )
{
	m_apItem = NULL;
	m_apIndex = NULL;

	SetItemContainer( dwItemType, dwItemMax, dwExtra );
}
*/

template <class T> CItemContainer<T>::~CItemContainer()
{
	SAFE_DELETE_ARRAY( m_apItem );
	SAFE_DELETE_ARRAY( m_apIndex );
}

template <class T> void CItemContainer<T>::Clear()
{
	for( DWORD i = 0; i < m_dwItemMax; i++ )
	{
		m_apItem[ i ].Empty();
#if __VER >= 9
		SAFE_DELETE( m_apItem[i].m_pPet );
#endif	// __PET_0410
		m_apItem[ i ].m_dwObjId = i;
		if( i < m_dwIndexNum )
		{
			m_apItem[ i ].m_dwObjIndex = i;
			m_apIndex[ i ] = i;
		}
		else 
		{
//			m_apItem[i].m_dwObjIndex	= NULL_ID:
			m_apIndex[ i ] = NULL_ID;
		}
	}
}

template <class T> void CItemContainer<T>::SetItemContainer( DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra )
{
	m_dwIndexNum	= dwItemMax;
	m_dwItemMax		= dwItemMax;
	if( dwExtra != 0xffffffff )
		m_dwItemMax		+= dwExtra;

	SAFE_DELETE_ARRAY( m_apItem );
	SAFE_DELETE_ARRAY( m_apIndex );

	m_apItem	= new T[ m_dwItemMax ];
	m_apIndex	= new DWORD[ m_dwItemMax ];
	for( u_long i = 0; i < m_dwItemMax; i++ )
	{
		m_apItem[i].m_dwObjId = i;
		if( i < m_dwIndexNum )
		{
			m_apItem[i].m_dwObjIndex	= i;
			m_apIndex[i]	= i;
		}
		else 
			m_apIndex[i]	= NULL_ID;
	}
}

#if __VER >= 11 // __SYS_IDENTIFY
#ifdef __SEND_ITEM_ULTIMATE
template <class T> BOOL CItemContainer<T>::Add( DWORD dwItemId, short nNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist, int nResistAbilityOption, BYTE* pnId, short* pnNum, BYTE* pnCount, BOOL bCharged, int nPiercedSize, DWORD dwItemId0, DWORD dwItemId1, DWORD dwItemId2, DWORD dwItemId3, DWORD dwItemId4, DWORD dwKeepTime, __int64 iRandomOptItemId )
#else // __SEND_ITEM_ULTIMATE
template <class T> BOOL CItemContainer<T>::Add( DWORD dwItemId, short nNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist, int nResistAbilityOption, BYTE* pnId, short* pnNum, BYTE* pnCount, BOOL bCharged, int nPiercedSize, DWORD dwItemId0, DWORD dwItemId1, DWORD dwItemId2, DWORD dwItemId3, DWORD dwKeepTime, __int64 iRandomOptItemId )
#endif // __SEND_ITEM_ULTIMATE
#else	// __SYS_IDENTIFY
#ifdef __SEND_ITEM_ULTIMATE
template <class T> BOOL CItemContainer<T>::Add( DWORD dwItemId, short nNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist, int nResistAbilityOption, BYTE* pnId, short* pnNum, BYTE* pnCount, BOOL bCharged, int nPiercedSize, DWORD dwItemId0, DWORD dwItemId1, DWORD dwItemId2, DWORD dwItemId3, DWORD dwItemId4, DWORD dwKeepTime, int nRandomOptItemId )
#else // __SEND_ITEM_ULTIMATE
template <class T> BOOL CItemContainer<T>::Add( DWORD dwItemId, short nNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist, int nResistAbilityOption, BYTE* pnId, short* pnNum, BYTE* pnCount, BOOL bCharged, int nPiercedSize, DWORD dwItemId0, DWORD dwItemId1, DWORD dwItemId2, DWORD dwItemId3, DWORD dwKeepTime, int nRandomOptItemId )
#endif // __SEND_ITEM_ULTIMATE
#endif	// __SYS_IDENTIFY
{
#if defined(__DBSERVER)
	if( pnId )
		*pnCount	= 0;
	ItemProp* pItemProp		= prj.GetItemProp( dwItemId );
	ASSERT( pItemProp );
	BOOL bPackItem = TRUE;
	int nId;
	short nNumtmp	= nNum;

	if( IsFull( pItemProp, nNum, bCharged ) )
		return FALSE;

	if( pItemProp->dwPackMax == 1 )
		bPackItem = FALSE;

	CItemElem* pElemtmp;
	if( bPackItem )
	{
		for( DWORD i = 0; i < m_dwIndexNum; i++ )	// 합쳐질수 있으면 합치기
		{
			nId	= m_apIndex[i];
			pElemtmp	= (CItemElem*)&m_apItem[nId];
				
			if( pElemtmp->IsEmpty() == FALSE && pElemtmp->m_dwItemId == dwItemId && pElemtmp->m_nItemNum < (short)pItemProp->dwPackMax
				&& pElemtmp->m_bCharged == bCharged && pElemtmp->m_byFlag == 0 )	// 아이템 플래그 0 아이템 머지 가능
			{
				if( pElemtmp->m_nItemNum + nNumtmp > (short)pItemProp->dwPackMax )
				{
					nNumtmp	-= ( (short)pItemProp->dwPackMax - pElemtmp->m_nItemNum );
					pElemtmp->m_nItemNum	= (short)pItemProp->dwPackMax;

					if( pnId ) {
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]	= pElemtmp->m_nItemNum;
						( *pnCount )++;
					}
				}
				else {
					pElemtmp->m_nItemNum	+= nNumtmp;
					if( pnId ) {
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]		= pElemtmp->m_nItemNum;	
						( *pnCount )++;
					}
					nNumtmp = 0;
					break;
				}
			}
		}
	}

	if( nNumtmp > 0 )
	{
		for( DWORD i = 0; i < m_dwIndexNum; i++ )
		{
			nId	= m_apIndex[i];
			pElemtmp	= (CItemElem*)&m_apItem[nId];
			if( pElemtmp->IsEmpty() )
			{
				pElemtmp->m_dwItemId	= dwItemId;
				pElemtmp->m_dwObjId		= nId;
				pElemtmp->m_dwObjIndex	= i;
				ItemProp* pItemProp		= prj.GetItemProp( dwItemId );
				if( pItemProp )
					pElemtmp->m_nHitPoint	= pItemProp->dwEndurance;
				else
					pElemtmp->m_nHitPoint	= 0;

				pElemtmp->m_nRepair		= 0;
				pElemtmp->SetOption( nOption );
				pElemtmp->SetSerialNumber( iSerialNumber );
				pElemtmp->m_bItemResist = nItemResist;
				pElemtmp->m_nResistAbilityOption = nResistAbilityOption;
				pElemtmp->m_bCharged = bCharged;
				pElemtmp->m_dwKeepTime	= 0;

				pElemtmp->SetPiercingSize( nPiercedSize );
				pElemtmp->SetPiercingItem( 0, dwItemId0 );
				pElemtmp->SetPiercingItem( 1, dwItemId1 );
				pElemtmp->SetPiercingItem( 2, dwItemId2 );
				pElemtmp->SetPiercingItem( 3, dwItemId3 );
#ifdef __SEND_ITEM_ULTIMATE
				pElemtmp->SetPiercingItem( 4, dwItemId4 );
#endif // __SEND_ITEM_ULTIMATE
				if( dwKeepTime )
					pElemtmp->m_dwKeepTime	= dwKeepTime;
#if __VER >= 11 // __SYS_IDENTIFY
				pElemtmp->SetRandomOptItemId( iRandomOptItemId );
#else	// __SYS_IDENTIFY
				pElemtmp->SetRandomOptItemId( nRandomOptItemId );
#endif	// __SYS_IDENTIFY
				
				if( nNumtmp > (short)pItemProp->dwPackMax )
				{
					pElemtmp->m_nItemNum	= (short)pItemProp->dwPackMax;
					nNumtmp	-= (short)pItemProp->dwPackMax;

					if( pnId ) {
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]	= pElemtmp->m_nItemNum;
						( *pnCount )++;
					}
				}
				else
				{
					pElemtmp->m_nItemNum	= nNumtmp;
					nNumtmp	= 0;
					if( pnId ) {
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]	= pElemtmp->m_nItemNum;
						( *pnCount )++;
					}
					break;
				}
			}
		}
	}
	if( nNumtmp > 0 )
		Error( "template <class T> BOOL CItemContainer<T>::Add( DWORD dwItemId... : nNumtmp > 0" );
	return TRUE;
#endif	// __DBSERVER
	return FALSE;
}

template <class T> BOOL CItemContainer<T>::Add( T* pElem, BYTE* pnId, short* pnNum, BYTE* pnCount )
{
	if( pnId )
		*pnCount	= 0;
	ItemProp* pItemProp		= prj.GetItemProp( pElem->m_dwItemId );
	if( !pItemProp )
		return FALSE;
	BOOL bPackItem = TRUE;
	int nId;

	short nNumtmp	= ( (CItemElem*)pElem )->m_nItemNum;
	if( IsFull( pItemProp, nNumtmp, pElem->m_bCharged ) )	// 아이템을 넣을수 있는지 검사
		return FALSE;

	if( pItemProp->dwPackMax == 1 )
		bPackItem = FALSE;

	CItemElem* pElemtmp;
	if( bPackItem )
	{
		for( DWORD i = 0; i < m_dwIndexNum; i++ )
		{
			nId	= m_apIndex[i];
			if( nId < 0 || nId >= (int)( m_dwItemMax ) )
				continue;
			pElemtmp	= (CItemElem*)&m_apItem[nId];
			
			//if( pElemtmp->IsEmpty() == FALSE && pElemtmp->m_dwItemId == pElem->m_dwItemId && pElemtmp->m_nItemNum < (short)pItemProp->dwPackMax )
			if( pElemtmp->IsEmpty() == FALSE && pElemtmp->m_dwItemId == pElem->m_dwItemId && pElemtmp->m_nItemNum < (short)pItemProp->dwPackMax
				&& pElemtmp->m_bCharged == pElem->m_bCharged && pElemtmp->m_byFlag == 0 ) // mirchang 100114 아이템 겹침 문제 수정
			{
				if( pElemtmp->m_nItemNum + nNumtmp > (short)pItemProp->dwPackMax )
				{
					nNumtmp	-= ( (short)pItemProp->dwPackMax - pElemtmp->m_nItemNum );
					pElemtmp->m_nItemNum	= (short)pItemProp->dwPackMax;
					if( pnId )
					{
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]	= pElemtmp->m_nItemNum;
						( *pnCount )++;
					}
				}
				else {
					pElemtmp->m_nItemNum	+= nNumtmp;
					if( pnId ) {
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]		= pElemtmp->m_nItemNum;	
						( *pnCount )++;
					}
					nNumtmp = 0;
					break;
				}
			}
			
		}
	}

	if( nNumtmp > 0 )
	{
		for( DWORD i = 0 ; i < m_dwIndexNum ; i++ )
		{
			nId	= m_apIndex[i];
			if( nId < 0 || nId >= (int)( m_dwItemMax ) )
				continue;
			pElemtmp	= (CItemElem*)&m_apItem[nId];
			if( pElemtmp->IsEmpty() )	// 빈공간인지 검사
			{
				*pElemtmp	= *( (CItemElem*)pElem );
				pElemtmp->m_dwObjId	= nId;
				pElemtmp->m_dwObjIndex		= i;

				if( nNumtmp > (short)pItemProp->dwPackMax )
				{
					pElemtmp->m_nItemNum	= (short)pItemProp->dwPackMax;
					nNumtmp		-= (short)pItemProp->dwPackMax;
					if( pnId ) {
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]	= pElemtmp->m_nItemNum;
						( *pnCount )++;
					}
				}
				else
				{
					pElemtmp->m_nItemNum	= nNumtmp;
					nNumtmp		= 0;
					if( pnId )
					{
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]		= pElemtmp->m_nItemNum;
						( *pnCount )++;
					}
					break;
				}
			}
		}
	}

	return TRUE;
}


template <class T> BOOL CItemContainer<T>::IsFull( ItemProp* pItemProp, short nNum, BOOL bCharged )
{
	int nId;
	short nNumtmp	= nNum;

	CItemElem* pElemtmp;
	for( DWORD i = 0; i < m_dwIndexNum; i++ )
	{
		nId	= m_apIndex[i];
		if( nId < 0 || nId >= (int)( m_dwItemMax ) )
			continue;
		pElemtmp	= (CItemElem*)&m_apItem[nId];
		if( pElemtmp->IsEmpty() )
		{
			if( nNumtmp > (short)pItemProp->dwPackMax )
				nNumtmp	-= (short)pItemProp->dwPackMax;
			else
				return FALSE;
		}
		else if( pElemtmp->m_dwItemId == pItemProp->dwID && pElemtmp->m_bCharged == bCharged && pElemtmp->m_byFlag == 0 )
		{
			if( pElemtmp->m_nItemNum + nNumtmp > (short)pItemProp->dwPackMax )
				nNumtmp	-= ( (short)pItemProp->dwPackMax - pElemtmp->m_nItemNum );
			else
				return FALSE;
		}
	}

	return TRUE;
}

template <class T> T* CItemContainer<T>::GetAt( DWORD dwIndex )
{
	DWORD dwIdx		= m_apIndex[dwIndex];
	if( dwIdx == NULL_ID )
		return NULL;
	T* pItemElem	= &m_apItem[dwIdx];
	if( pItemElem->IsEmpty() ) 
		return NULL;
	return pItemElem;
}

template <class T> T* CItemContainer<T>::GetAtId( OBJID dwObjId )
{
	if( dwObjId >= m_dwItemMax )	return NULL;
	T* pItemElem	= &m_apItem[dwObjId];
	if( pItemElem->IsEmpty() )
		return NULL;
	return pItemElem;
}

template <class T> T* CItemContainer<T>::GetItem( ItemProp * pItemProp, int nAbilityOption )
{
	int nId;
	for( int i = 0 ; i < m_dwItemMax ; i++ )
	{
		nId		= m_apIndex[i];
		if( 0 <= nId && m_apItem[nId].IsEmpty() == FALSE )
		{
			T* pItemElem = &m_apItem[nId];
			if( pItemElem->m_dwItemId == pItemProp->dwID && pItemElem->GetOption() == nAbilityOption )
			{
				return pItemElem;
			}
		}
	}
	return NULL;
}

template <class T> T* CItemContainer<T>::GetItem( ItemProp * pItemProp, int nAbilityOption, int nItemResist, int nResistAbilityOption )
{
	int nId;
	for( DWORD i = 0 ; i < m_dwItemMax ; i++ )
	{
		nId		= m_apIndex[i];
		if( 0 <= nId && m_apItem[nId].IsEmpty() == FALSE )
		{
			T* pItemElem = &m_apItem[nId];
			if( pItemElem->m_dwItemId == pItemProp->dwID && pItemElem->GetOption() == nAbilityOption && pItemElem->m_bItemResist == nItemResist && pItemElem->m_nResistAbilityOption == nResistAbilityOption )
			{
				return pItemElem;
			}
		}
	}
	return NULL;
}

template <class T> BOOL CItemContainer<T>::IsEquip( DWORD dwObjId )
{
	if( m_apItem[ dwObjId ].m_dwObjIndex >= m_dwIndexNum )
		return TRUE;
	return FALSE;
}
template <class T> void CItemContainer<T>::RemoveAt( DWORD dwIndex )
{
	m_apItem[ m_apIndex[ dwIndex ] ].Empty();
	if( dwIndex >= m_dwIndexNum )
	{
		m_apItem[ m_apIndex[ dwIndex ] ].m_dwObjIndex = NULL_ID;
		m_apIndex[ dwIndex ] = NULL_ID;
	}
	//for( int i = dwIndex; i < m_dwIndexNum - 1; i++ )
	//	m_apIndex[ i ] = m_apIndex[ i + 1 ];
	//m_dwIndexNum--;
}

template <class T> void CItemContainer<T>::RemoveAtId( OBJID dwObjId )
{
	if( dwObjId >= m_dwItemMax )
		return;
	if( m_apItem[ dwObjId ].m_dwObjIndex >= m_dwItemMax )
		return;
	m_apItem[ dwObjId ].Empty();
	if( m_apItem[ dwObjId ].m_dwObjIndex >= m_dwIndexNum )
	{
		m_apIndex[ m_apItem[ dwObjId ].m_dwObjIndex ] = NULL_ID;
		m_apItem[ dwObjId ].m_dwObjIndex = NULL_ID;
	}
}

template <class T> T* CItemContainer<T>::GetEquip( DWORD dwIndex )
{
	return GetAt( m_dwIndexNum + dwIndex );
}

template <class T> BOOL CItemContainer<T>::UnEquip( DWORD dwIndex )
{ 
	dwIndex		+= m_dwIndexNum;
	if( dwIndex >= m_dwItemMax )
		return FALSE;

	DWORD dwId	= m_apIndex[dwIndex];

	if( dwId >= m_dwItemMax )
		return FALSE;

	for( DWORD i = 0; i < m_dwIndexNum; i++ )
	{
		if( m_apIndex[i] >= m_dwItemMax )
			continue;

		if( m_apItem[m_apIndex[i]].IsEmpty() )
		{
			m_apItem[m_apIndex[i]].m_dwObjIndex		= NULL_ID;
			m_apIndex[dwIndex]	= NULL_ID;
			m_apItem[dwId].m_dwObjIndex		= i;
			m_apIndex[i]	= dwId;
			return TRUE;
		}
	}
	return FALSE;
}

template <class T> void CItemContainer<T>::DoEquip( DWORD dwSrcIndex, DWORD dwDstIndex )
{
	dwDstIndex	+= m_dwIndexNum;
	for( int i = 0; i < m_dwItemMax; i++ )
	{
		if( m_apItem[i].IsEmpty() && m_apItem[i].m_dwObjIndex == NULL_ID )
		{
			m_apIndex[dwDstIndex]	= m_apIndex[dwSrcIndex];
			m_apIndex[dwSrcIndex]	= i;
			m_apItem[m_apIndex[dwSrcIndex]].m_dwObjIndex	= dwSrcIndex;
			m_apItem[m_apIndex[dwDstIndex]].m_dwObjIndex	= dwDstIndex;
			return;
		}
	}
}

template <class T> void CItemContainer<T>::Copy( CItemContainer<T> & rItemContainer )
{
	m_dwIndexNum	= rItemContainer.m_dwIndexNum;
	memcpy( (void*)m_apIndex, (void*)rItemContainer.m_apIndex, sizeof(DWORD) * m_dwItemMax );
	for( u_long i = 0; i < m_dwItemMax; i++ )
	{
		m_apItem[i].m_dwObjId	= rItemContainer.m_apItem[i].m_dwObjId;
		m_apItem[i].m_dwObjIndex	= rItemContainer.m_apItem[i].m_dwObjIndex;
		if( rItemContainer.m_apItem[i].IsEmpty() == FALSE )
			m_apItem[i]		= rItemContainer.m_apItem[i];
		else
			m_apItem[i].Empty();
	}
}

template <class T> void CItemContainer<T>::Serialize( CAr & ar )
{
//	for( int i = 0; i < m_dwItemMax; i++ )	// 462
//		m_apItem[i].Serialize( ar );
	DWORD	adwObjIndex[128];

	unsigned char chSize	= 0;
	
	if( ar.IsStoring() )
	{
		
		ar.Write( m_apIndex, sizeof(DWORD) * m_dwItemMax );
		u_long uOffset	= ar.GetOffset();
		ar << chSize;

		for( u_char ch = 0; ch < m_dwItemMax; ch++ )	// 0-504
		{	
			if( m_apItem[ch].IsEmpty() == FALSE )
			{
				ar << ch;
				m_apItem[ch].Serialize( ar );
				chSize++;
			}
			adwObjIndex[ch]		= m_apItem[ch].m_dwObjIndex;
		}

		ar.Write( adwObjIndex, sizeof(DWORD) * m_dwItemMax );

		int nBufSize;
		LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
		*( lpBuf + uOffset )	= chSize;
	}
	else
	{
		ar.Read( m_apIndex, sizeof(DWORD) * m_dwItemMax );
		// Clear
		for( u_int i = 0; i < m_dwItemMax; i++ )
			m_apItem[i].Empty();

		ar >> chSize;

		unsigned char ch;
		for( i = 0; i < chSize; i++ )
		{
			ar >> ch;
			m_apItem[ch].Serialize( ar );
		}

		ar.Read( adwObjIndex, sizeof(DWORD) * m_dwItemMax );
		for( i = 0; i < m_dwItemMax; i++ )
		{
			m_apItem[i].m_dwObjIndex	= adwObjIndex[i];
		}
	}
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CItem : public CCtrl
{
public:
	CItemBase* m_pItemBase;

//	Constructions
	CItem();
	virtual ~CItem();
//	Operations
	virtual void	Serialize( CAr & ar );
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CAction
{
public:
	DWORD	m_dwState;
	DWORD	m_dwStateFlag;

public:
	CAction()
		{
			m_dwState	= 0;
			m_dwStateFlag		= 0;
		}
	void	ClearStateFlag() { m_dwStateFlag = 0; }
	void	AddStateFlag( DWORD dwStateFlag ) { m_dwStateFlag |= dwStateFlag; }
	DWORD	GetStateFlag() { return m_dwStateFlag; }
	virtual	~CAction()	{}
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
class CMover;
class CActionMover : public CAction
{
public:
	CActionMover()	{}
	CActionMover( CMover* pMover )	{}
	virtual	~CActionMover()	{}
	DWORD	GetState( void )	{	return m_dwState;	}
	void	__ForceSetState( DWORD dwState )	{	m_dwState	= dwState;	}
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
typedef struct tagQuest
{
	BYTE		m_nState;
	WORD		m_wTime;
	WORD		m_wId;
	// 추가 
	//BYTE		m_nKillNPCNum[ 2 ];	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
	WORD		m_nKillNPCNum[ 2 ];	// chipi_091015 - NPC Kill Quest 갯수 확장( BYTE -> WORD )
	BYTE		m_bPatrol   : 1;
	BYTE		m_bReserve2 : 1;
	BYTE		m_bReserve3 : 1;
	BYTE		m_bReserve4 : 1;
	BYTE		m_bReserve5 : 1;
	BYTE		m_bReserve6 : 1;
	BYTE		m_bReserve7 : 1;
	BYTE		m_bReserve8 : 1;
} QUEST,* LPQUEST;

//#ifdef __DBSERVER
#if defined(__DBSERVER)
typedef		MemPooler<CMover>	CMoverPool;
#endif	// __DBSERVER

typedef	struct	_EQUIP_INFO
{
	DWORD	dwId;
	int		nOption;
	BYTE	byFlag;
}	EQUIP_INFO,	*PEQUIP_INFO;

#if __VER >= 11 // __SYS_POCKET
#include "pocket.h"
#endif	// __SYS_POCKET

class CMover: public CCtrl
{
private:
	DWORD			m_dwGold;

public:
/// 몬스터에 해당되는 것 
	TCHAR			m_szCharacterKey[ 32 ];
	BOOL			m_bActiveAttack;
	DWORD			m_dwBelligerence;

	int				m_nMovePattern;
	int				m_nMoveEvent;
	int				m_nMoveEventCnt;
/// 몬스터에 해당되는 것 

	TCHAR			m_szName[32];
	BOOL			m_bPlayer;
	u_long			m_idPlayer;
	LONG			m_nHitPoint;
	LONG			m_nManaPoint;
	LONG			m_nFatiguePoint;
	DWORD			m_dwSkinSet;
	DWORD			m_dwHairMesh;
	DWORD			m_dwHairColor;
	DWORD			m_dwHeadMesh;

#if __VER >= 12 // __MOD_TUTORIAL
private:
	int		m_nTutorialState;
#else	// __MOD_TUTORIAL
	LONG			m_nFlightLv;
#endif	// __MOD_TUTORIAL
public:
#if __VER >= 12 // __MOD_TUTORIAL
	LONG	GetFlightLv( void )	{	return ( m_nLevel >= 20? 1: 0 );		}
	void	SetFlightLv( LONG nFlightLv )	{}
	int		GetTutorialState( void )	{	return m_nTutorialState;	}
	void	SetTutorialState( int nTutorialState )	{	m_nTutorialState	= nTutorialState;	}
#else	// __MOD_TUTORIAL
	LONG	GetFlightLv( void )	{	return m_nFlightLv;		}
	void	SetFlightLv( LONG nFlightLv )	{	m_nFlightLv	= nFlightLv;	}
#endif	// __MOD_TUTORIAL

	BOOL	IsEquipableNPC()	{	return FALSE;	}

	LONG			m_nFxp;
	BYTE			m_bySex;
	DWORD			m_dwRideItemIdx;
	int				m_nFuel;
	LONG			m_nJob;
	BYTE			m_nSlot;
	char			m_szBankPass[5];
	int				m_idGuildCloak;

	LONG			m_nStr,	m_nSta,	m_nDex,	m_nInt;
	LONG			m_nLevel, m_nDeathLevel;
	EXPINTEGER		m_nExp1, m_nDeathExp;
	SKILL			m_aJobSkill[ MAX_SKILL_JOB ];

#ifdef __SKILL_0205
	BYTE	m_abUpdateSkill[MAX_SKILL_JOB];
#endif	// __SKILL_0205

	OBJID			m_idMarkingWorld;
	D3DXVECTOR3		m_vMarkingPos;
	long			m_nRemainGP;
	DWORD			m_dwAuthorization;
	DWORD			m_dwMode;

	CUserTaskBar	m_UserTaskBar;
#ifdef __RT_1025
	CRTMessenger	m_RTMessenger;
#else	// __RT_1025
	CMessenger		m_Messenger;
#endif	// __RT_1025

	u_long			m_idparty;
	u_long			m_idGuild;
	u_long			m_idWar;
#if __VER >= 8 // __S8_PK
	DWORD			m_dwPKTime;					/// 핑크 상태 시간
	int				m_nPKValue;					/// PK 수치
	DWORD			m_dwPKPropensity;			/// PK 성향
	DWORD			m_dwPKExp;					/// PK 성향 습득 경험치
#else // __VER >= 8 // __S8_PK
	int				m_nNumKill;
	int				m_nSlaughter;
#endif // __VER >= 8 // __S8_PK
#if __VER >= 8 // __CSC_VER8_5
	EXPINTEGER		m_nAngelExp;				/// 엔젤 경험치
	LONG			m_nAngelLevel;				/// 엔젤 Level
#endif // __CSC_VER8_5

	int				m_nFame;
	u_long			m_idMurderer;
	int				m_nDuel;			// 듀얼중인가.( 0:없음 1:개인듀얼중 2:파티듀얼중.
	u_long			m_idDuelParty;		// 파티듀얼중이면 상대방 파티의 id
	int				m_nGuildCombatState;
	int				m_nSkillLevel;						// 지금까지 올린 스킬레벨
	int				m_nSkillPoint;						// SP

	CActionMover*					m_pActMover; 
	CItemContainer< CItemElem  >	m_Inventory ;

#if __VER >= 11 // __SYS_POCKET
	CPocketController	m_Pocket;
#endif	// __SYS_POCKET

	u_long							m_idPlayerBank[3];
	CItemContainer< CItemElem  >	m_Bank[ 3 ] ;
	DWORD							m_dwGoldBank[ 3 ];

	D3DXVECTOR3		m_vReturnPos;						//'귀환 두루마리'관련 
	DWORD			m_dwReturnWorldID;
	DWORD			m_tmAccFuel;
#ifdef __DBSERVER
#ifdef __JEFF_9_20
	DWORD	m_dwMute;
#else	// __JEFF_9_20
	DWORD			m_dwFlyTime;
#endif	// __JEFF_9_20
	DWORD			m_dwWorldID;
	CMclCritSec		m_AccessLock;
#ifdef __EVENT_1101
#ifdef __EVENT_1101_2
	__int64 m_nEventFlag; // 64비트 경과 일 번째 비트
#else // __EVENT_1101_2
	DWORD	m_dwEventFlag;	//	경과 일 번째 비트
#endif // __EVENT_1101_2
	DWORD	m_dwEventTime;	// 분 단위 계수
	DWORD	m_dwEventElapsed;	// 경과 일
#endif	// __EVENT_1101
#endif // __DBSERVER

#if __VER >= 15 // __PETVIS
private:
	OBJID	m_objIdVisPet;	// 소환중인 비스펫의 인벤토리 위치
	DWORD	m_dwMoverSfxId; // 무버에 붙는 이펙트
#endif // __PETVIS
#if __VER >= 9	// __PET_0410
private:
	DWORD	m_dwPetId;	// 소환중이 펫의 인벤토리 위치(自), 소환중인 펫 인덱스(他)
public:
	DWORD	GetPetId( void )	{	return m_dwPetId;	}
	void	SetPetId( DWORD dwPetId )		{	m_dwPetId	= dwPetId;	}
	CItemElem*	GetPetItem( void );
#endif	// __PET_0410
#if __VER >= 9	//__AI_0509
	FLOAT	m_fSpeedFactor;
#endif	// __AI_0509

#ifdef __EXP_ANGELEXP_LOG
	int		m_nExpLog;
	int		m_nAngelExpLog;
#endif // __EXP_ANGELEXP_LOG

#ifdef __EVENTLUA_COUPON
	int		m_nCoupon;
#endif // __EVENTLUA_COUPON

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	int				m_nHonor;					// 달인선택 
	int				m_aHonorTitle[MAX_HONOR_TITLE];			// 달인수치
	void			SetHonorCount(int nIdx , int nCount )	{	m_aHonorTitle[nIdx] = nCount;	}
#endif	// __HONORABLE_TITLE			// 달인

	LONG			m_nPlusMaxHitPoint;
	DWORD			m_dwSMTime[SM_MAX];
	BYTE			m_nAttackResistLeft;
	BYTE			m_nAttackResistRight;
	BYTE			m_nDefenseResist;

#ifdef __BUFF_1107
	CBuffMgr	m_buffs;
#else	// __BUFF_1107
	CSkillInfluence m_SkillState;				// 지속성을 갖는 스킬의 상태 처리(예:프로텍션 ??초동안 방어력 얼마 증가)
#endif	// __BUFF_1107

	EQUIP_INFO		m_aEquipInfo[MAX_HUMAN_PARTS];
	void	ClearEquipInfo( void )
	{
		memset( m_aEquipInfo, 0, sizeof(EQUIP_INFO) * MAX_HUMAN_PARTS );
		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
			m_aEquipInfo[i].dwId	= NULL_ID;
	}
	
	DWORD			m_dwStateMode;
	OBJID			m_dwUseItemId;					// 비행 시전 시간
	LPQUEST			m_aQuest;
	LPWORD			m_aCompleteQuest; 
	BYTE			m_nQuestSize;
	BYTE			m_nCompleteQuestSize;
	DWORD			m_dwPeriodicTick;
	DWORD			m_dwTickCheer;
	int				m_nCheerPoint;
#if __VER >= 14 // __PCBANG
	DWORD			m_dwPCBangClass;
#endif // __PCBANG
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	LPWORD			m_aCheckedQuest;
	BYTE			m_nCheckedQuestSize;
#endif // __IMPROVE_QUEST_INTERFACE
#if __VER >= 15 // __CAMPUS
private:
	u_long	m_idCampus;
	int		m_nCampusPoint;
public:
	u_long	GetCampusId()	{	return m_idCampus;	}
	void	SetCampusId( u_long idCampus )	{	m_idCampus = idCampus;	}
	int		GetCampusPoint()	{	return m_nCampusPoint;	}
	void	SetCampusPoint( int nMPPoint )		{	m_nCampusPoint = nMPPoint;	}
#endif // __CAMPUS

#if __VER >= 15 // __GUILD_HOUSE
public:
	int		m_nRestPoint;
	time_t	m_tLogOut;
#endif // __GUILD_HOUSE

public:
//	Constructions
	CMover();
	virtual	~CMover();

//	Operations
	void	InitProp( void );
	int		InitSkillExp( void );
#if defined(__DBSERVER)
	MoverProp*	GetProp( void )		{	return( prj.GetMoverProp( m_dwIndex ) );	}
#endif	// __DBSERVER

	BOOL	RemoveQuest( int nQuestId );

#if __VER >= 11 // __SYS_IDENTIFY
#ifdef __SEND_ITEM_ULTIMATE
	BOOL	AddItem( DWORD dwItemType, DWORD dwId, DWORD dwNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist = 0, int nResistAbilityOption = 0, int nCharged = 0, int nPiercedSize = 0, DWORD dwItemId0 = 0, DWORD dwItemId1 = 0, DWORD dwItemId2 = 0, DWORD dwItemId3 = 0, DWORD dwItemId4 = 0, DWORD dwKeepTime = 0, __int64 iRandomOptItemId = 0 );
#else // __SEND_ITEM_ULTIMATE
	BOOL	AddItem( DWORD dwItemType, DWORD dwId, DWORD dwNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist = 0, int nResistAbilityOption = 0, int nCharged = 0, int nPiercedSize = 0, DWORD dwItemId0 = 0, DWORD dwItemId1 = 0, DWORD dwItemId2 = 0, DWORD dwItemId3 = 0, DWORD dwKeepTime = 0, __int64 iRandomOptItemId = 0 );
#endif // __SEND_ITEM_ULTIMATE
#else	// __SYS_IDENTIFY
#ifdef __SEND_ITEM_ULTIMATE
	BOOL	AddItem( DWORD dwItemType, DWORD dwId, DWORD dwNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist = 0, int nResistAbilityOption = 0, int nCharged = 0, int nPiercedSize = 0, DWORD dwItemId0 = 0, DWORD dwItemId1 = 0, DWORD dwItemId2 = 0, DWORD dwItemId3 = 0, DWORD dwItemId4 = 0, DWORD dwKeepTime = 0, int nRandomOptItemId = 0 );
#else // __SEND_ITEM_ULTIMATE
	BOOL	AddItem( DWORD dwItemType, DWORD dwId, DWORD dwNum, int nOption, SERIALNUMBER iSerialNumber, int nItemResist = 0, int nResistAbilityOption = 0, int nCharged = 0, int nPiercedSize = 0, DWORD dwItemId0 = 0, DWORD dwItemId1 = 0, DWORD dwItemId2 = 0, DWORD dwItemId3 = 0, DWORD dwKeepTime = 0, int nRandomOptItemId = 0 );
#endif // __SEND_ITEM_ULTIMATE
#endif	// __SYS_IDENTIFY
	BOOL	AddItemBank( int nSlot, DWORD dwId, DWORD dwNum, int nOption, SERIALNUMBER iSerialNumber );
	virtual void	Serialize( CAr & ar );
//	int		GetMaxHitPoint()	    { return( ( m_nLevel * 16 ) + ( m_nSta * 6 ) + ( m_nStr  * 3 ) );		}
//	int		GetMaxManaPoint()		{ return( ( m_nLevel + m_nInt ) * 2 + 20 );		}
//	int		GetMaxFatiguePoint()	{ return(int)( ( m_nLevel * 2 ) + ( m_nSta * m_nLevel / 40 ) + ( ( m_nStr + m_nSta + m_nDex ) * 2.5 ) ); }
	BOOL	IsAbnormalPlayerData();
	BOOL	NormalizePlayerData();
	BOOL	RemoveItemIK3();
	BYTE	GetSex() { return m_bySex; }
	void	SetSex( BYTE bySex ) { m_bySex = bySex; }
	int		GetGold();
	void	SetGold( int nGold );
	BOOL	AddGold( int nGold, BOOL bSend = TRUE );

	void	Copy( CMover* pMover, BOOL bCopyAll );
	void	PeriodTick();

#if defined(__DBSERVER)
	void	Lock( void )	{	m_AccessLock.Enter();	}
	void	Unlock( void )	{	m_AccessLock.Leave();	}
#ifdef __INVALID_LOGIN_0612
	BOOL	m_bLogout;
	char	m_szAccount[MAX_ACCOUNT];
#endif	// __INVALID_LOGIN_0612
#ifndef __VM_0820
#ifndef __MEM_TRACE
public:
	static CMoverPool*	m_pPool;
	void*	operator new( size_t nSize )	{	return CMover::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CMover::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CMover::m_pPool->Free( (CMover*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CMover::m_pPool->Free( (CMover*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
#endif	// __DBSERVER
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

inline int CMover::GetGold()
{
	ASSERT( m_dwGold <= INT_MAX );
	return m_dwGold;
}

inline void CMover::SetGold( int nGold )
{
	ASSERT( nGold >= 0 );
	m_dwGold = (DWORD)nGold;
}

inline BOOL CMover::AddGold( int nGold, BOOL bSend )
{
	if( nGold == 0 )
		return TRUE;

#ifdef __PERIN_BUY_BUG
	float fTotal = static_cast<float>( GetGold() ) + static_cast<float>( nGold );
	if( fTotal > static_cast<float>( INT_MAX ) )
		fTotal = static_cast<float>( INT_MAX );
	else if( fTotal < 0.0f )
		return FALSE;

	SetGold( static_cast<int>( fTotal ) );
#else // __PERIN_BUY_BUG
	int nTotal = GetGold() + nGold;

	if( nGold > 0 )
	{
		if( nTotal < 0 )		// overflow?
			nTotal = INT_MAX;
	}
	else
	{
		if( nTotal < 0 )		// underflow?
			return FALSE;
	}

	SetGold( nTotal );
#endif // __PERIN_BUY_BUG
	if( bSend )
	{
	#ifdef __WORLDSERVER
		g_UserMng.AddSetPointParam( this, DST_GOLD, nTotal );
	#endif	// __WORLDSERVER
	}

	return TRUE;
}

#endif	// __OBJ_H__