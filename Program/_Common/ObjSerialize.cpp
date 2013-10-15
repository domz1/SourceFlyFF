#include "stdafx.h"

#if defined(__DBSERVER) || defined(__CORESERVER) || defined(__ACTIVEXPLAYER) || defined(__VPW)
#include "..\_Network\Objects\Obj.h"
#else
#include "Mover.h"
#endif


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void CCtrl::Serialize( CAr & ar )	// 33
{
	CObj::Serialize( ar );

	if( ar.IsStoring() )
	{
		ar << m_objid;
	}
	else
	{
		ar >> m_objid;
	}
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void CItemBase::Serialize( CAr & ar )	// 11	// 20
{
	if( ar.IsStoring() )
	{
		ar << m_dwObjId << m_dwItemId;
		ar << m_liSerialNumber;
		ar.WriteString( m_szItemText );
	}
	else
	{
		ar >> m_dwObjId >> m_dwItemId;
		ar >> m_liSerialNumber;
		ar.ReadString( m_szItemText, 32 );
	}
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void CItemElem::Serialize( CAr & ar )	// 18
{
	CItemBase::Serialize( ar );
	if( ar.IsStoring() )
	{
		ar << m_nItemNum;
		ar << m_nRepairNumber << m_nHitPoint;
		ar << m_nRepair;
		ar << m_byFlag;
		ar << m_nAbilityOption;
		ar << m_idGuild;				// 길드번호도 저장.
		ar << m_bItemResist;			// 어느 속성 인가?
		ar << m_nResistAbilityOption ;	// 속성 추가 능력치 가변 옵션
		ar << m_nResistSMItemId;
		m_piercing.Serialize( ar );
		ar << m_bCharged;
#if __VER >= 11 // __SYS_IDENTIFY
		ar << m_iRandomOptItemId;
#else	// __SYS_IDENTIFY
		ar << m_nRandomOptItemId;
#endif	// __SYS_IDENTIFY
		ar << m_dwKeepTime;
		if( m_dwKeepTime )
		{
			time_t t	= m_dwKeepTime - time_null();
			ar << t;
		}
#if __VER >= 9	// __PET_0410
		if( m_pPet )
		{
			ar << (BYTE)0x01;
			m_pPet->Serialize( ar );
		}
		else
		{
			ar << (BYTE)0x00;
		}
#endif	// __PET_0410
#if __VER >= 15 // __PETVIS
		ar << m_bTranformVisPet;
#endif // __PETVIS
	}
	else
	{
		ar >> m_nItemNum;
		ar >> m_nRepairNumber ;
		ar >> m_nHitPoint;
		ar >> m_nRepair;
		ar >> m_byFlag;
		ar >> m_nAbilityOption;
		ar >> m_idGuild;		
		ar >> m_bItemResist;	
		ar >> m_nResistAbilityOption ;	
		ar >> m_nResistSMItemId;
//		memset( &m_piercingInfo, 0, sizeof(m_piercingInfo) );
		m_piercing.Serialize( ar );
		ar >> m_bCharged;
#if __VER >= 11 // __SYS_IDENTIFY
		ar >> m_iRandomOptItemId;
#else	// __SYS_IDENTIFY
		ar >> m_nRandomOptItemId;
#endif	// __SYS_IDENTIFY
		ar >> m_dwKeepTime;

		if( m_dwKeepTime ) 
		{
			time_t t;
			ar >> t;
		#ifdef __CLIENT
			m_dwKeepTime	= time_null() + t;
		#endif	// __CLIENT
		}
#if __VER >= 9	// __PET_0410
		SAFE_DELETE( m_pPet );
		BYTE bPet;
		ar >> bPet;
		if( bPet )
		{
			m_pPet	= new CPet;
			m_pPet->Serialize( ar );
		}
#endif	// __PET_0410
#if __VER >= 15 // __PETVIS
		ar >> m_bTranformVisPet;
#endif // __PETVIS

#ifdef __CLIENT
		SetTexture();
#endif
	}
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*
#ifdef __XPET2
void CPetElem::Serialize( CAr & ar )	// 18
{
	CItemBase::Serialize( ar );
	if( ar.IsStoring() )
	{
		ar << m_nMaxLevel << m_nLevel << m_dwHungry << m_dwFeeling;
	} else
	// Loading
	{
		ar >> m_nMaxLevel >> m_nLevel >> m_dwHungry >> m_dwFeeling;
	}
}
#endif // xPet2
*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void CItem::Serialize( CAr & ar )	// 45
{
	CCtrl::Serialize( ar );		// 33
	
	if( ar.IsStoring() )
	{
	}
	else
	{
		SAFE_DELETE( m_pItemBase );
		m_pItemBase	= new CItemElem;
	}

	m_pItemBase->Serialize( ar );	// 11
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

// 지웠음. 따로 보관함...xuzhu
/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

