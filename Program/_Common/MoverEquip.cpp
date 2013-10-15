#include "stdafx.h"
#include "defineText.h"
#include "defineSkill.h"
#include "defineSound.h"
#include "defineItem.h"

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#include "ModelObject.h"
#include "authorization.h"
#include "CreateObj.h"
#include "lang.h"
#include "party.h"
#include "guild.h"
#include "guildwar.h"

#ifdef __CLIENT
	#include "DPClient.h"
	#include "DialogMsg.h"
	extern	CDPClient	g_DPlay;
	#include "GuildRender.h"
#else	// __CLIENT
	#include "User.h"
	#include "WorldMng.h"
	#include "DPSrvr.h"
	#include "DPCoreClient.h"
	#include "dpdatabaseclient.h"
	#include "ItemScript.h"

	extern	CUserMng			g_UserMng;
	extern	CWorldMng			g_WorldMng;
	extern	CDPSrvr				g_DPSrvr;
	extern	CDPDatabaseClient	g_dpDBClient;
	extern	CDPCoreClient		g_DPCoreClient;
#endif	// __CLIENT

#if __VER >= 11 // __SYS_COLLECTING
#include "accessory.h"
#endif	// __SYS_COLLECTING



extern	CPartyMng		g_PartyMng;
extern	CGuildMng		g_GuildMng;
extern	CGuildWarMng	g_GuildWarMng;


BYTE  nMaleHairColor[10][3] = { 
	{ 144,  99,  101 },
	{ 100, 107, 125 },
	{ 158,119,109 },
	{ 199,156,100 },
	{ 100,123,117 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 } 
};

BYTE  nFeMaleHairColor[10][3] = { 
	{ 250,250,90 },
	{ 250,250,90 },
	{ 147,118,103 },
	{ 150, 100, 85 },
	{ 213,130,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 } 
};

#define TEX_PART_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair01.dds"  ) : _T( "Part_femaleHair01.dds"  ) )
#define TEX_PART_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper01.dds" ) : _T( "Part_femaleUpper01.dds" ) )
#define TEX_PART_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower01.dds" ) : _T( "Part_femaleLower01.dds" ) )

#define PARTSMESH_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair%02d.o3d" ) : _T( "Part_femaleHair%02d.o3d" ) )
#define PARTSMESH_HEAD( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHead%02d.o3d" ) : _T( "Part_femaleHead%02d.o3d" ) )
#define PARTSMESH_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper.o3d"    ) : _T( "Part_femaleUpper.o3d"    ) )
#define PARTSMESH_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower.o3d"    ) : _T( "Part_femaleLower.o3d"    ) )
#define PARTSMESH_HAND( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHand.o3d"     ) : _T( "Part_femaleHand.o3d"     ) )
#define PARTSMESH_FOOT( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleFoot.o3d"     ) : _T( "Part_femaleFoot.o3d"     ) )

#define PARTSTEX_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair%02d.dds"  ) : _T( "Part_femaleHair%02d.dds"  ) )
#define PARTSTEX_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper%02d.dds" ) : _T( "Part_femaleUpper%02d.dds" ) )
#define PARTSTEX_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower%02d.dds" ) : _T( "Part_femaleLower%02d.dds" ) )
#define PARTSTEX_HAND( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHand.dds"      ) : _T( "Part_femaleHand.dds"      ) )
#define PARTSTEX_FOOT( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleFoot.dds"      ) : _T( "Part_femaleFoot.dds"      ) )

int GetSelfDST( BYTE type )
{
	switch( type )
	{
		case SAI79::FIRE:
			return DST_RESIST_FIRE;
		case SAI79::WATER:
			return DST_RESIST_WATER;
		case SAI79::WIND:
			return DST_RESIST_WIND; 
		case SAI79::ELECTRICITY:
			return DST_RESIST_ELECTRICITY;
		case SAI79::EARTH:
			return DST_RESIST_EARTH;
	}
	return DST_RESIST_FIRE;
}

int GetStrongDST( BYTE type )
{
	switch( type )
	{
		case SAI79::FIRE:
			return DST_RESIST_WIND;
		case SAI79::WATER:
			return DST_RESIST_FIRE;
		case SAI79::WIND:
			return DST_RESIST_EARTH;
		case SAI79::ELECTRICITY:
			return DST_RESIST_WATER;
		case SAI79::EARTH:
			return DST_RESIST_ELECTRICITY;
	}
	return DST_RESIST_FIRE;
}

int GetWeakDST( BYTE type )
{
	switch( type )
	{
		case SAI79::FIRE:
			return DST_RESIST_WATER;
		case SAI79::WATER:
			return DST_RESIST_ELECTRICITY;
		case SAI79::WIND:
			return DST_RESIST_FIRE;
		case SAI79::ELECTRICITY:
			return DST_RESIST_EARTH;
		case SAI79::EARTH:
			return DST_RESIST_WIND;
	}
	return DST_RESIST_FIRE;
}

// 머리카락 모양과 색 바꾸기  
void CMover::SetHair( int nHair )
{
	if( nHair >= 0 && nHair < MAX_HAIR )
	{
		m_dwHairMesh = nHair;
		CModelObject* pModel = (CModelObject*)m_pModel;
		TCHAR lpszTemp[ 64 ];
		// Hair
		_stprintf( lpszTemp, PARTSMESH_HAIR( GetSex() ), m_dwHairMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HAIR );
		_stprintf( lpszTemp, PARTSTEX_HAIR( GetSex() ), 0 /*nHairColor*/ + 1 );
		pModel->ChangeTexture( PARTS_HAIR, TEX_PART_HAIR( GetSex() ), lpszTemp );
	}
}

void  CMover::SetHairColor( FLOAT r, FLOAT g, FLOAT b )
{ 
#ifndef __Y_HAIR_BUG_FIX	
	if( ( r == 1.0f && g == 1.0f && b == 1.0f ) || 
		( fabs( r - 1.0f ) < 0.001f && fabs( g - 1.0f ) < 0.001f && fabs( b - 1.0f ) < 0.001f ) )
	{
		if( GetSex() == SEX_MALE )
		{
			m_fHairColorR = (nMaleHairColor[m_dwHairMesh][0])/255.f;
			m_fHairColorG = (nMaleHairColor[m_dwHairMesh][1])/255.f;
			m_fHairColorB = (nMaleHairColor[m_dwHairMesh][2])/255.f;
		}
		else
		{
			m_fHairColorR = (nFeMaleHairColor[m_dwHairMesh][0])/255.f;
			m_fHairColorG = (nFeMaleHairColor[m_dwHairMesh][1])/255.f;
			m_fHairColorB = (nFeMaleHairColor[m_dwHairMesh][2])/255.f;
		}
	}
	else
#endif //__Y_HAIR_BUG_FIX
	{
#ifdef __Y_HAIR_BUG_FIX
		m_fHairColorR = ( r < 0.0f ) ? 0.0f : r;
		m_fHairColorG = ( g < 0.0f ) ? 0.0f : g;
		m_fHairColorB = ( b < 0.0f ) ? 0.0f : b;
#else //__Y_HAIR_BUG_FIX
		m_fHairColorR = ( r < 0.3f ) ? 0.3f : r;
		m_fHairColorG = ( g < 0.3f ) ? 0.3f : g;
		m_fHairColorB = ( b < 0.3f ) ? 0.3f : b;
#endif //__Y_HAIR_BUG_FIX
	}
	
	m_dwHairColor = D3DCOLOR_COLORVALUE( m_fHairColorR, m_fHairColorG, m_fHairColorB, 1.0f );
}


void CMover::SetHairColor( DWORD dwHairColor )
{
	m_dwHairColor = dwHairColor;
	m_fHairColorR = ((dwHairColor>>16) & 0xff) / 255.f;
	m_fHairColorG = ((dwHairColor>>8) & 0xff)  / 255.f;
	m_fHairColorB = ((dwHairColor) & 0xff)     / 255.f;

#ifndef __Y_HAIR_BUG_FIX
	if( ( m_fHairColorR == 1.0f && m_fHairColorG == 1.0f && m_fHairColorB == 1.0f ) || 
		( fabs( m_fHairColorR - 1.0f ) < 0.001f && fabs( m_fHairColorG - 1.0f ) < 0.001f && fabs( m_fHairColorB - 1.0f ) < 0.001f ) )
	{
		if( GetSex() == SEX_MALE )
		{
			m_fHairColorR = (nMaleHairColor[m_dwHairMesh][0])/255.f;
			m_fHairColorG = (nMaleHairColor[m_dwHairMesh][1])/255.f;
			m_fHairColorB = (nMaleHairColor[m_dwHairMesh][2])/255.f;
		}
		else
		{
			m_fHairColorR = (nFeMaleHairColor[m_dwHairMesh][0])/255.f;
			m_fHairColorG = (nFeMaleHairColor[m_dwHairMesh][1])/255.f;
			m_fHairColorB = (nFeMaleHairColor[m_dwHairMesh][2])/255.f;
		}

	}
#endif //__Y_HAIR_BUG_FIX

	m_dwHairColor = D3DCOLOR_COLORVALUE( m_fHairColorR, m_fHairColorG, m_fHairColorB, 1.0f );
}

// 표정 바꾸기 
void CMover::SetHead( int nHead )
{
	if( nHead < MAX_HEAD )
	{
		m_dwHeadMesh = nHead;
		CModelObject* pModel = (CModelObject*)m_pModel;
		TCHAR lpszTemp[ 64 ];
		// Hair
		_stprintf( lpszTemp, PARTSMESH_HEAD( GetSex() ), m_dwHeadMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HEAD );
	}
}
void CMover::SetSkinSet( int nSkinSet )
{
	if( nSkinSet < 2 )
	{
		m_dwSkinSet = nSkinSet;
#ifdef __CLIENT
		UpdateParts();
#endif
	}
}


// 이 함수는 전체 의상을 새로 완성하도록 한다.
// 보통 이 함수는 처음에 캐릭터를 생성할 때 한번 호출한다.
void CMover::UpdateParts( BOOL bFakeParts )
{
	UpdateParts( GetSex(), m_dwSkinSet, m_dwFace, m_dwHairMesh, m_dwHeadMesh, 
		m_aEquipInfo, (CModelObject*)m_pModel, bFakeParts ? NULL : &m_Inventory, GetProp()->bIfParts, this );
	((CModelObject*)m_pModel)->RestoreDeviceObjects();
	UpdateBoundBox();
	UpdateParam();
}

ItemProp* GetInventoryProp( PEQUIP_INFO pEquipInfo, CItemContainer< CItemElem  >* pInventory, int nParts )
{
	CItemElem* pItemElem = NULL;
	ItemProp* pItemProp = NULL;

	if( pInventory == NULL ) // fake only
	{
		if( pEquipInfo[nParts].dwId != NULL_ID ) 
			pItemProp = prj.GetItemProp( pEquipInfo[nParts].dwId );
	}
	else
	{
		pItemElem = pInventory->GetEquip( nParts );
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	}

	return pItemProp;
}

void CMover::UpdateParts( int nSex, int nSkinSet, int nFace, int nHairMesh, int nHeadMesh, PEQUIP_INFO pEquipInfo, CModelObject* pModel, CItemContainer< CItemElem  >* pInventory, BOOL bIfParts, CMover* pMover )
{
	if( pModel == NULL )
	{
		LPCTSTR szErr = Error( "CMover::UpdateParts : pModel is NULL" );
		ADDERRORMSG( szErr );
		return;
	}
	// 기본 부위(속살, 헤어, 등등)을 세팅한다.
	//LPSKINSET lpSkinset = prj.m_PartsMng.GetSkinSet( nSex, nSkinSet );
	//LPCTSTR lpHair = prj.m_PartsMng.GetHairMeshName( nSex, nHairMesh );
	TCHAR lpszTemp[ 64 ];
	if( bIfParts )
	{
		// Hair
		_stprintf( lpszTemp, PARTSMESH_HAIR( nSex ), nHairMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HAIR );
		// Head
		_stprintf( lpszTemp, PARTSMESH_HEAD( nSex ), nHeadMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HEAD );
	}
	// 먼저 익스크루시브 오브젝트를 TakeOff한다.
	BOOL abExclusiveParts[ MAX_HUMAN_PARTS ];
	ZeroMemory( abExclusiveParts, sizeof( abExclusiveParts ) );
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( i == PARTS_HAIR || i == PARTS_HEAD  )//|| i == PARTS_CAP || i == PARTS_HAND || i == PARTS_FOOT )
			continue;
		DWORD dwParts = -1;
		CItemElem* pItemElem = NULL;
		ItemProp* pItemProp = NULL;
		// 프로퍼티 꺼냄
		pItemProp = GetInventoryProp( pEquipInfo, pInventory, i );
		if( pItemProp )
		{
			if( pInventory )
			{
				CItemElem* pItemElem	= pInventory->GetAt( i + pInventory->GetSize() );
				if( pItemElem->IsFlag( CItemElem::expired ) )
					pItemProp	= NULL;
			}
			else
			{
				if( pEquipInfo[i].byFlag & CItemElem::expired )
					pItemProp	= NULL;
			}
		}

		// 익스클루시브가 있을때
		if( pItemProp && pItemProp->dwExclusive != NULL_ID )
		{
			pModel->TakeOffParts( pItemProp->dwExclusive );	// 익스클루시브를 제거
			abExclusiveParts[ pItemProp->dwExclusive ] = TRUE;		// 익스클루시브 된넘을 기억.
		}
	}
	// 장착부위를 세팅한다.
	for( i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( i == PARTS_HAIR || i == PARTS_HEAD  )//|| i == PARTS_CAP || i == PARTS_HAND || i == PARTS_FOOT )
			continue;
		DWORD dwParts = -1;
		CItemElem* pItemElem = NULL;
		ItemProp* pItemProp = NULL;
		if( pInventory == NULL ) // fake only
		{
			if( pEquipInfo[i].dwId != NULL_ID ) 
			{
				if( ( pEquipInfo[i].byFlag & CItemElem::expired ) != CItemElem::expired )
					pItemProp	= prj.GetItemProp( pEquipInfo[i].dwId );
			}
		}
		else
		{
			pItemElem = pInventory->GetEquip( i );//AtId( padwEquipment[ i ] );
			if( pItemElem )
			{
				if( !pItemElem->IsFlag( CItemElem::expired ) )
					pItemProp = pItemElem->GetProp();
			}
		}
		TCHAR szPartsName[ 32 ];

		// 각 파츠의 o3d를 로딩.
		if( pItemProp )
		{
			if( pItemProp->bPartsFile == TRUE )
				prj.m_modelMng.MakePartsName( szPartsName, _T( "part" ), pItemProp->dwID, nSex );
			else
				prj.m_modelMng.MakeModelName( szPartsName, OT_ITEM, pItemProp->dwID );
			//prj.m_modelMng.MakePartsName( szPartsName, _T( "item" ), pItemProp->dwID );
			if( bIfParts || ( i == PARTS_RWEAPON || i == PARTS_LWEAPON || i == PARTS_SHIELD ) )
			{
				pModel->LoadElement( szPartsName, i /*pItemProp->dwParts*/ );
			}
		}


		if( abExclusiveParts[ i ] == FALSE )
		{
			switch( i )
			{
			case PARTS_RWEAPON: 
				// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
				if( pItemProp && ( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) ) )
				{
					switch( pItemProp->dwItemKind3 )
					{
					case IK3_KNUCKLEHAMMER:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
						break;

					case IK3_BOW:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
						break;
					case IK3_YOYO:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
						pModel->LoadElement( szPartsName, PARTS_LWEAPON );
						((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
						break;

					default:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
					}
//					if( pItemProp->dwItemKind3 == IK3_KNUCKLEHAMMER )
//						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
//					else
//						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
				}
				else 
					pModel->TakeOffParts( i );
				break;
			case PARTS_LWEAPON: 
				// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
				if( pItemProp && ( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) ) )
				{
					((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
				}
				else 
					pModel->TakeOffParts( i );
				break;
			case PARTS_SHIELD: 
				// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
				if( pItemProp && ( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) ) )
					((CModelObject*)pModel)->SetParent( PARTS_SHIELD, ((CModelObject*)pModel)->GetLArmIdx() );
				else 
					pModel->TakeOffParts( i );
				break;
			case PARTS_UPPER_BODY:
				if( bIfParts )
					if( pItemProp == NULL )	
						pModel->LoadElement( PARTSMESH_UPPER( nSex ), PARTS_UPPER_BODY );
				break;
			case PARTS_LOWER_BODY:
				if( bIfParts )
					if( pItemProp == NULL ) 
						pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
				break;
			case PARTS_HAND:
				if( bIfParts )
					if( pItemProp == NULL ) 
						pModel->LoadElement( PARTSMESH_HAND( nSex ), PARTS_HAND );
				break;
			case PARTS_FOOT:
				if( bIfParts )
					if( pItemProp == NULL ) 
						pModel->LoadElement( PARTSMESH_FOOT( nSex ), PARTS_FOOT );
				break;
			default:
				if( bIfParts )
				{
					if( pItemProp == NULL ) 
						pModel->TakeOffParts( i );
				}

			}
		}
	}
} 
// 장비를 장착한다. 여기서 의상 교체 작업을 해준다.
// nOption은 Fake아이템 장착때만 쓰인다. - xuzhu -
BOOL CMover::DoEquip( int nSex, int nSkinSet, 
					  CItemElem* pItemElem, int nPart, const EQUIP_INFO & rEquipInfo, CItemContainer< CItemElem  >* pInventory, 
					  PEQUIP_INFO pEquipInfo, CModelObject* pModel, BOOL bEquip, CMover *pMover ) 
{
	ItemProp* pItemProp = pItemElem ? pItemElem->GetProp() : prj.GetItemProp( rEquipInfo.dwId );
	DWORD dwIndex = pItemElem ? pItemElem->m_dwObjIndex : 0;
	DWORD dwParts = pItemProp->dwParts;
	BOOL bIfParts = pMover ? pMover->GetProp()->bIfParts : 0;
	TCHAR lpszTemp[ 64 ];
	BOOL bFake = (pInventory == NULL) ? TRUE : FALSE;
#ifdef __SECURITY_FIXES
	if( dwParts == NULL_ID || ( nPart == -1 &&  dwParts != PARTS_RIDE && dwParts != PARTS_RING2 && dwParts != PARTS_EARRING2) )
		return FALSE;
#else // __SECURITY_FIXES
	if( dwParts == NULL_ID )
		return FALSE;
#endif// __SECURITY_FIXES

	// 반지/귀걸이 벗을때 처리.
	if( !bEquip )
	{
		if( dwParts == PARTS_RING1 || dwParts == PARTS_EARRING1 )
		{
			CItemElem *pItemElemTemp;
			
			if( pInventory )
			{
				pItemElemTemp = (CItemElem*)pInventory->GetEquip( dwParts );
				if( pItemElemTemp != pItemElem )
					dwParts = pItemProp->dwPartsub;
			}
		}
	}
	// 쌍칼(이도류) 처리.
	if( pItemProp && pItemProp->dwHanded == HD_ONE && pItemProp->dwParts == PARTS_RWEAPON )	// 원핸드 무기를 차려고 하는가?
	{
		if( bEquip )
		{
			// 디폴트는 장착위치 오른손
			// 왼손에 무기가 가야하는 상황을 검사
			ItemProp *pProp = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_RWEAPON );
#ifdef __3RD_LEGEND16
			if( pMover->GetJob() == JOB_BLADE || pMover->GetJob() == JOB_BLADE_MASTER || pMover->GetJob() == JOB_BLADE_HERO || pMover->GetJob() == JOB_STORMBLADE_HERO )
#else // __3RD_LEGEND16
#if __VER >= 10 // __LEGEND	//	9차 전승시스템	Neuz, World, Trans
			if( pMover->GetJob() == JOB_BLADE || pMover->GetJob() == JOB_BLADE_MASTER || pMover->GetJob() == JOB_BLADE_HERO )	// 쌍칼속성
#else //__LEGEND	//	9차 전승시스템	Neuz, World, Trans
			if( pMover->GetJob() == JOB_BLADE )	// 쌍칼속성
#endif	//__LEGEND	//	9차 전승시스템	Neuz, World, Trans
#endif // __3RD_LEGEND16
			{
				if( pItemProp->dwID != II_WEA_KNU_ISHOFIST )
				{
					if( pProp && pProp->dwHanded == HD_ONE )	// 오른손에 무기가 있냐? 그 무기가 원핸드냐
					{
						if( pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_SHIELD ) == NULL )	//왼손에 방패없냐?
							dwParts = PARTS_LWEAPON;		// 장착위치를 왼손으로 바꿈
					} 
				}
			}

			if( pProp && pProp->dwItemKind3 == IK3_YOYO )		// 오른손에 무기가 요요였으면
				pModel->TakeOffParts( PARTS_LWEAPON );			// 왼손에 요요모델도 삭제함. 오른손모델은 밑에서 자동으로 
			
		} 
		else
		{
			// 해제시도.
			if( nPart >= 0  )		// 파츠를 직접 지정했을때만
				dwParts = nPart;	// 해제하라고 한 부위를 해제시킨다.
		}
	} // 쌍칼 처리.
	
#ifndef __WORLDSERVER	// <<<< 월드에서 처리안하면 메모리 리크 나지 않나? -xuzhu-
	((CModelObject*)pModel)->TakeOffParts( dwParts );	
#endif

	if( pItemProp && pItemProp->dwItemKind3 == IK3_YOYO )	// 벗으려던 무기가 요요였으면
		pModel->TakeOffParts( PARTS_LWEAPON );		// 왼손에 있던 요요모델도 없애준다.

	if( bEquip )	// 장착하려 할때만...
	{
		ItemProp *pHandItemProp	= NULL;
		ItemProp *pLHandItemProp = NULL;

		// 들고있는 무기 프로퍼티 꺼냄.
		pHandItemProp = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_RWEAPON );		// 오른손 무기 프로퍼티.
		pLHandItemProp = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_LWEAPON );		// 왼손 무기 프로퍼티.
		
		// 조건검사.
		if( pItemProp->dwItemKind3 == IK3_SHIELD )	// 방패류를 착용하려 했을때
		{
			if( pHandItemProp && pHandItemProp->dwHanded == HD_TWO )		// 손에 쥐고 있는게 투핸드면.
			{
				if( pInventory )
				{
					CItemElem *pItemElemOld = pInventory->GetEquip( PARTS_RWEAPON );
					if( pInventory->UnEquip( PARTS_RWEAPON ) == FALSE )	
						return FALSE;
#ifdef __WORLDSERVER
					// 투핸드 무기벗김
					if( pItemElemOld )
					{
						ItemProp *pOldItemProp = pItemElemOld->GetProp();
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );
						g_UserMng.AddDoEquip( pMover, -1, pItemElemOld, FALSE );
					}
#if __VER >= 9 // __ULTIMATE
					prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElemOld, FALSE, PARTS_RWEAPON );
#endif // __ULTIMATE
#endif // worldserver
					if( pEquipInfo )
						pEquipInfo[PARTS_RWEAPON].dwId	= NULL_ID;
				} else
				{
					if( pEquipInfo )
						pEquipInfo[PARTS_RWEAPON].dwId	= NULL_ID;
				}
				pModel->TakeOffParts( PARTS_RWEAPON );
				if( pHandItemProp->dwItemKind3 == IK3_YOYO )	// 들고 있던 무기가 요요였으면
					pModel->TakeOffParts( PARTS_LWEAPON );		// 왼손에 있던 요요모델도 없애준다.
			}
			else
			if( pLHandItemProp && pLHandItemProp->dwParts == PARTS_RWEAPON )	// 왼손에 무기가 쥐어져있으면. 왼손에 드는 무기도 PARTS이름은 모두 RWEAPON이다.
			{
				if( pInventory )
				{
					CItemElem *pItemElemOld = pInventory->GetEquip( PARTS_LWEAPON );
					if( pInventory->UnEquip( PARTS_LWEAPON ) == FALSE )	
						return FALSE;
#ifdef __WORLDSERVER
					// 왼손 무기벗김
					if( pItemElemOld )
					{
						ItemProp *pOldItemProp = pItemElemOld->GetProp();
#ifndef 	__BLADELWEAPON0608	//	9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );
#endif	//__BLADELWEAPON0608	//	9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정
						g_UserMng.AddDoEquip( pMover, PARTS_LWEAPON, pItemElemOld, FALSE );
					}
#endif // worldserver
					if( pEquipInfo )
						pEquipInfo[PARTS_LWEAPON].dwId	= NULL_ID;
				}
				else
				{
					if( pEquipInfo )
						pEquipInfo[PARTS_LWEAPON].dwId	= NULL_ID;
				}
				pModel->TakeOffParts( PARTS_LWEAPON );
			}
		} // IK3_SHIELD
		if( pItemProp->dwHanded == HD_TWO )		// 장착하려는 무기가 투핸드 였을때
		{
			if( pHandItemProp && pHandItemProp->dwItemKind3 == IK3_YOYO )	// 들고 있던 무기가 요요였으면
				pModel->TakeOffParts( PARTS_LWEAPON );		// 왼손에 있던 요요모델도 없애준다.
			if( pInventory == NULL )
			{
				if( pEquipInfo[PARTS_SHIELD].dwId != NULL_ID )		// 방패가 있었는가 검사해서 있으면 벗김.
				{
					pModel->TakeOffParts( PARTS_SHIELD );
					if( pEquipInfo )
						pEquipInfo[PARTS_SHIELD].dwId	= NULL_ID;
				}
				if( pEquipInfo[PARTS_RWEAPON].dwId != NULL_ID )		// 무기가 있었는가 검사해서 벗김.
				{
					pModel->TakeOffParts( PARTS_RWEAPON );
					if( pEquipInfo )
						pEquipInfo[PARTS_RWEAPON].dwId	= NULL_ID;
				}
			}
			else
			{
				CItemElem *pItemElemOld = pInventory->GetEquip( PARTS_SHIELD );
				if( pItemElemOld )
				{
					if( pInventory->UnEquip( PARTS_SHIELD ) == FALSE )	// 방패 벗김
						return FALSE;	// 인벤이 꽉차면 걍 취소
#ifdef __WORLDSERVER
					if( pItemElemOld )	// 기존장비가 있었다면
					{
						ItemProp *pOldItemProp = pItemElemOld->GetProp();
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );
						g_UserMng.AddDoEquip( pMover, -1, pItemElemOld, FALSE );
					}
#endif
					pModel->TakeOffParts( PARTS_SHIELD );
					if( pEquipInfo )
						pEquipInfo[PARTS_SHIELD].dwId	= NULL_ID;
				}
				
				pItemElemOld = pInventory->GetEquip( PARTS_RWEAPON );
				if( pInventory->GetEquip( PARTS_RWEAPON ) )
				{
					if( pInventory->UnEquip( PARTS_RWEAPON ) == FALSE )	// 무기벗김
						return FALSE;	// 인벤이 꽉차면 걍 취소
#ifdef __WORLDSERVER
					if( pItemElemOld )	// 기존장비가 있었다면
					{
						ItemProp *pOldItemProp = pItemElemOld->GetProp();
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );
						g_UserMng.AddDoEquip( pMover, -1, pItemElemOld, FALSE );
					}
#if __VER >= 9 // __ULTIMATE
					prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElemOld, FALSE, PARTS_RWEAPON );
#endif // __ULTIMATE
#endif // __WORLDSERVER
					pModel->TakeOffParts( PARTS_RWEAPON );
					if( pEquipInfo )
						pEquipInfo[PARTS_RWEAPON].dwId	= NULL_ID;
				}
			}
		} // HD_TWO
	} // bEquip == TRUE

	if( bEquip )
	{
		if( pInventory == NULL )
		{
			// 익스크루시브 아이템 제거 
			if( pItemProp->dwExclusive != NULL_ID )
			{
				pModel->TakeOffParts( pItemProp->dwExclusive );
				if( pEquipInfo )
					pEquipInfo[pItemProp->dwExclusive].dwId		= NULL_ID;
			}
			// 페이크 장착 
			if( pEquipInfo )
				memcpy( &pEquipInfo[dwParts], &rEquipInfo, sizeof(EQUIP_INFO) );
		}
		else
		{
			// 익스크루시브 아이템 제거 
			if( pItemProp->dwExclusive != NULL_ID )
			{
				CItemElem* pExclusive	= pInventory->GetEquip( pItemProp->dwExclusive );
				if( pExclusive )
				{
					if( pInventory->UnEquip( pItemProp->dwExclusive ) == FALSE )
						return FALSE;
#ifdef __WORLDSERVER
					ItemProp *pProp	= pExclusive->GetProp();
					pMover->ResetDestParamEquip( pProp, pExclusive );
					g_UserMng.AddDoEquip( pMover, -1, pExclusive, FALSE );
#endif	// __WORLDSERVER

				}
				pModel->TakeOffParts( pItemProp->dwExclusive );

			}

			// 기존 장비 제거 
			if( dwParts == PARTS_RING1 || dwParts == PARTS_EARRING1 )
			{
				DWORD dwElem1 = NULL_ID, dwElem2 = NULL_ID;
				CItemElem* pItemElemTemp1 = (CItemElem*)pInventory->GetEquip( dwParts );
				CItemElem* pItemElemTemp2 = (CItemElem*)pInventory->GetEquip( dwParts + 1 );
				if( pItemElemTemp1 ) dwElem1 = pItemElemTemp1->m_dwItemId;
				if( pItemElemTemp2 ) dwElem2 = pItemElemTemp2->m_dwItemId;
				if( dwElem1 != NULL_ID && dwElem2 == NULL_ID )
					dwParts = pItemProp->dwPartsub;
			}
			CItemElem *pItemElemOld = pInventory->GetEquip( dwParts );
			if( pItemElemOld )
			{
				if( pInventory->UnEquip( dwParts ) == FALSE )
					return FALSE;
#ifdef __WORLDSERVER
				ItemProp *pOldItemProp = pItemElemOld->GetProp();
#ifdef 	__BLADELWEAPON0608	//	9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정
				if(dwParts != PARTS_LWEAPON )
#endif	//__BLADELWEAPON0608	//	9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정
					pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );					
				if( pItemElemOld->m_nResistSMItemId != 0 )	// 유료화 아이템이 셑팅되어 있다면
					((CUser* )pMover)->DoSMItemUnEquip( pItemElemOld, dwParts );
#if __VER >= 9 // __ULTIMATE
				prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElemOld, FALSE, dwParts );
#endif // __ULTIMATE
#endif // worldserver
				
			}
			// 새 장비 장착 
			if( TRUE == pInventory->DoEquip( dwIndex, /*MAX_INVENTORY +*/ dwParts ) )
			{
				if( pEquipInfo )
				{
					pEquipInfo[dwParts].dwId	= pItemElem->m_dwObjId;
					pEquipInfo[dwParts].nOption = pItemElem->GetAttrOption();
				}

#ifdef __WORLDSERVER
				if( pItemElem && pItemElem->m_nResistSMItemId != 0 ) // 유료화 아이템이 셑팅되어 있다면
				{
					((CUser* )pMover)->DoSMItemEquip( pItemElem, dwParts );
				}
#if __VER >= 9 // __ULTIMATE
				prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElem, TRUE, dwParts );
#endif // __ULTIMATE
#endif // __WORLDSERVER
			}
			else
			{
				WriteError( "%s %d", __FILE__, __LINE__ );
				return FALSE;
			}
		} // not Fake

#ifndef __WORLDSERVER
		TCHAR szPartsName[ 32 ];
		if( pItemProp->bPartsFile == TRUE )
			prj.m_modelMng.MakePartsName( szPartsName, _T( "part" ), pItemProp->dwID, nSex );
		else
			prj.m_modelMng.MakeModelName( szPartsName, OT_ITEM, pItemProp->dwID );
			//prj.m_modelMng.MakePartsName( szPartsName, _T( "item" ), pItemProp->dwID );
		switch( dwParts )
		{
		case PARTS_RWEAPON: 
			// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
			if( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				switch( pItemProp->dwItemKind3 )
				{
				case IK3_KNUCKLEHAMMER:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
					break;
				case IK3_BOW:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
					break;
				case IK3_YOYO:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
					((CModelObject*)pModel)->LoadElement( szPartsName, PARTS_LWEAPON );
					((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
					break;
				default:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
				}
	//			if( pItemProp->dwItemKind3 == IK3_KNUCKLEHAMMER )
	//				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
	//			else
	//				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
			}
			break;
		case PARTS_LWEAPON: 
			// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
			if( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
			}
			break;
		case PARTS_SHIELD: 
			// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
			if( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				((CModelObject*)pModel)->SetParent( PARTS_SHIELD, ((CModelObject*)pModel)->GetLArmIdx() );
			}
			break;
		case PARTS_UPPER_BODY:
			if( bIfParts )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				_stprintf( lpszTemp, PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
			}
			break;
		case PARTS_LOWER_BODY:
			if( bIfParts )
			{
				{
					((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
					_stprintf( lpszTemp, PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
					pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
				}
			}
			break;
		default:
			if( bIfParts )
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
			break;

		}

#ifdef __CLIENT
#if __VER >= 15 // __BS_FIXED_EQUIPMOTION
		if( PARTS_LWEAPON == dwParts || PARTS_RWEAPON == dwParts || PARTS_SHIELD == dwParts )
			pMover->SetMotion( MTI_STAND, ANILOOP_1PLAY , MOP_NO_TRANS );

#endif //__BS_FIXED_EQUIPMOTION
#endif //__CLIENT
		
#endif // not WorldServer
	} // 장착.
	else
	// 탈착.
	{
		if( bIfParts )
		{
			if( pItemProp->dwExclusive != NULL_ID )
			{
				switch( pItemProp->dwExclusive )
				{
				case PARTS_UPPER_BODY:
					pModel->LoadElement( PARTSMESH_UPPER( nSex ), PARTS_UPPER_BODY );
					_stprintf( lpszTemp, PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
					pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
					break;
				case PARTS_LOWER_BODY:
					{

						ItemProp* pItemPropEquip = NULL;
						if( pItemProp->dwParts == PARTS_CLOTH )
						{
							pItemPropEquip = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_UPPER_BODY );
						}
						else
						{
							pItemPropEquip = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_CLOTH );
						}

						if( pItemPropEquip == NULL )
						{
							pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
							_stprintf( lpszTemp, PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
							pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
						}					
					}					
					break;
				case PARTS_HAND:
					pModel->LoadElement( PARTSMESH_HAND( nSex ), PARTS_HAND );
					break;
				case PARTS_FOOT:
					pModel->LoadElement( PARTSMESH_FOOT( nSex ), PARTS_FOOT );
					break;
				}
			}
		}
		if( pInventory && pInventory->GetEquip( dwParts ) )
		{
			if( pInventory->UnEquip( dwParts ) == FALSE )
			{
#ifdef __WORLDSERVER
				if( TRUE == pMover->IsPlayer() )
					( (CUser*)pMover )->AddDefinedText(TID_GAME_LACKSPACE, "" );
#endif	// __WORLDSERVER				
				return FALSE;
			}

#ifdef __WORLDSERVER
#if __VER >= 9 // __ULTIMATE
			prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElem, FALSE, dwParts );
#endif // __ULTIMATE
			if( pItemElem && pItemElem->m_nResistSMItemId != 0 )	// 유료화 아이템이 셑팅되어 있다면
			{
				((CUser* )pMover)->DoSMItemUnEquip( pItemElem, dwParts );
			}
#endif // __WORLDSERVER
		}

		if( pEquipInfo )
		{
			pEquipInfo[dwParts].dwId	= NULL_ID;
			pEquipInfo[dwParts].nOption		= 0;
			pEquipInfo[dwParts].byFlag	= 0;
		}

#ifndef __WORLDSERVER
		if( bIfParts )
		{
			switch( dwParts )
			{
			case PARTS_UPPER_BODY:
				pModel->LoadElement( PARTSMESH_UPPER( nSex ), PARTS_UPPER_BODY );
				_stprintf( lpszTemp, PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
				break;
			case PARTS_LOWER_BODY:
				pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
				_stprintf( lpszTemp, PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
				break;
			case PARTS_HAND:
				pModel->LoadElement( PARTSMESH_HAND( nSex ), PARTS_HAND );
				break;
			case PARTS_FOOT:
				pModel->LoadElement( PARTSMESH_FOOT( nSex ), PARTS_FOOT );
				break;
			}
		}
#endif

#ifdef __CLIENT
#if __VER >= 15 // __BS_FIXED_EQUIPMOTION
		if( PARTS_LWEAPON == dwParts || PARTS_RWEAPON == dwParts || PARTS_SHIELD == dwParts )
			pMover->SetMotion( MTI_STAND, ANILOOP_1PLAY , MOP_FIXED );
#endif //__BS_FIXED_EQUIPMOTION
#endif //__CLIENT

	} // 탈착


	// 벗길때
	if( bEquip == FALSE )
	{
		// 위에서 오른손 무기를 해제 했었다. 
		if( nPart == PARTS_RWEAPON )
		{
			if( pInventory )
			{
				CItemElem* pItemLeft = pInventory->GetEquip( PARTS_LWEAPON );
				if( pItemLeft )	// 왼손에 무기가 남아있다.
				{
					//오른손으로 옮겨줌.
					#ifdef __WORLDSERVER
					if( pMover->IsSMMode( SM_RESIST_ATTACK_LEFT ) )
					{
						pMover->m_nAttackResistRight = pMover->m_nAttackResistLeft;
						pMover->m_nAttackResistLeft = 0xff;
						DWORD dwSMTime = pMover->m_dwSMTime[SM_RESIST_ATTACK_LEFT];
						pMover->SetSMMode( SM_RESIST_ATTACK_LEFT, 1 );
						pMover->SetSMMode( SM_RESIST_ATTACK_RIGHT, dwSMTime );
						((CUser*)pMover)->AddResistSMMode( pMover->m_nAttackResistLeft, pMover->m_nAttackResistRight, pMover->m_nDefenseResist );
					}
					#endif // __WORLDSERVER
					
					int nInvenSize = pInventory->GetSize();
					pInventory->Swap( nInvenSize + PARTS_LWEAPON, nInvenSize + PARTS_RWEAPON );	
#ifdef __WORLDSERVER			
#if __VER >= 9 // __ULTIMATE
					prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemLeft, TRUE, PARTS_RWEAPON );
#endif // __ULTIMATE
#endif // __WORLDSERVER

				#ifdef __CLIENT
					((CModelObject*)pModel)->MovePart( PARTS_RWEAPON, PARTS_LWEAPON );
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
				#endif //__CLIENT
				}
			}
			else
			{	// fake
				if( pEquipInfo )
				{
					if( pEquipInfo[PARTS_LWEAPON].dwId != NULL_ID )
					{
						memcpy( &pEquipInfo[PARTS_RWEAPON], &pEquipInfo[PARTS_LWEAPON], sizeof(EQUIP_INFO) );
						pEquipInfo[PARTS_LWEAPON].dwId	= NULL_ID;
						pEquipInfo[PARTS_LWEAPON].nOption	= 0;
						pEquipInfo[PARTS_LWEAPON].byFlag	= 0;
					#ifdef __CLIENT
						((CModelObject*)pModel)->MovePart( PARTS_RWEAPON, PARTS_LWEAPON );
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
					#endif	// __CLIENT
					}
				}
			}
		}
	}

#ifdef __CLIENT
	D3DXVECTOR3 vPos = pMover->GetPos();
				
	if( bEquip )
	{
		if( pItemProp )
		{
			if( pItemProp->dwItemKind1 == IK1_WEAPON )
			{
				PLAYSND( SND_INF_EQUIPWEAPON, &vPos );
			}
			else
			if( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY )
			{
				PLAYSND( SND_INF_EQUIPACCESSORY, &vPos );
			}
			else
			if( pItemProp->dwItemKind1 == IK1_ARMOR && pItemProp->dwItemKind2 == IK2_CLOTH )
			{
				PLAYSND( SND_INF_EQUIPCLOTH, &vPos );
			}
			else
			if( pItemProp->dwItemKind1 == IK1_ARMOR && pItemProp->dwItemKind2 == IK2_ARMOR 
				|| pItemProp->dwItemKind2 == IK2_ARMORETC )
			{
				PLAYSND( SND_INF_EQUIPARMOR, &vPos );
			}	
		}
	}
	else
	{
		PLAYSND( SND_INF_INVENTORYDROP, &vPos );		
	}
#endif //__CLIENT

	return TRUE;
}

#if 0
//{{AFX
// static DoEquip의 새버전.
BOOL CMover::DoEquip( CItemElem* pItemElem, DWORD dwItemIndex, int nOption, CItemContainer< CItemElem  >* pInventory, 
					  LPDWORD padwEquipment, CModelObject* pModel, BOOL bEquip, CMover *pMover ) 
{
	ItemProp* pItemProp = pItemElem ? pItemElem->GetProp() : prj.GetItemProp( dwItemIndex );
	DWORD dwIndex = pItemElem ? pItemElem->m_dwObjIndex : 0;
	DWORD dwParts = pItemProp->dwParts;
	BOOL bIfParts = pMover ? pMover->GetProp()->bIfParts : 0;
	TCHAR lpszTemp[ 64 ];
	BOOL bFake = (pInventory == NULL) ? TRUE : FALSE;
	if( dwParts == NULL_ID )
		return FALSE;

	// 반지/귀걸이 벗을때 처리.
	if( !bEquip )
	{
		if( dwParts == PARTS_RING1 || dwParts == PARTS_EARRING1 )
		{
			CItemElem *pItemElemTemp;
			
			if( pInventory )
			{
				pItemElemTemp = (CItemElem*)pInventory->GetEquip( dwParts );
				if( pItemElemTemp != pItemElem )
					dwParts = pItemProp->dwPartsub;
			}
		}
	}

	if( bEquip )
	{
		// 쌍칼(이도류) 처리.
		if( pItemProp && pItemProp->dwHanded == HD_ONE && pItemProp->dwParts == PARTS_RWEAPON )	// 원핸드 무기를 차려고 하는가?
		{
			// 디폴트는 장착위치 오른손
 #ifdef __XUZHU
			//if( 직업이 블레이드냐? )	// 쌍칼속성
 #else
			if( 0 /*직업이 블레이드냐?*/ )	// 쌍칼속성
 #endif
			{
				if( pMover->GetEquipItemProp( bFake, PARTS_RWEAPON ) )	// 오른손에 무기가 있냐?
				{
					if( pMover->GetEquipItemProp( bFake, PARTS_SHIELD ) == NULL )	//왼손에 방패없냐?
						dwParts = PARTS_LWEAPON;		// 장착위치를 왼손으로 바꿈
				} 
			}
		}
	} else
	{
		// 해제.
		CItemElem *pLHandElem = pInventory->Get&Equip( PARTS_LWEAPON );
		if( pItemElem == pLHandElem )	// 왼손에 장착한 것과 해제하려고 요청한 아이템이 같을때.
			dwParts = PARTS_LWEAPON;	// 파츠 이름을 이걸로 바꿔주자.
	}

	
#ifndef __WORLDSERVER
	((CModelObject*)pModel)->TakeOffParts( dwParts );
#endif

	if( bEquip )	// 장착하려 할때만...
	{
		ItemProp *pHandItemProp	= NULL;
		ItemProp *pLHandItemProp = NULL;

		// 들고있는 무기 프로퍼티 꺼냄.
		if( pInventory == NULL )
		{
			if( padwEquipment[PARTS_RWEAPON] != NULL_ID )
				pHandItemProp = prj.GetItemProp( padwEquipment[PARTS_RWEAPON] );
		} else
		{
			CItemElem *pItemElem = pInventory->GetEquip( PARTS_RWEAPON );		
			if( pItemElem )
				pHandItemProp = pItemElem->GetProp();
		}

		if( pInventory == NULL )
		{
			if( padwEquipment[PARTS_LWEAPON] != NULL_ID )
				pLHandItemProp = prj.GetItemProp( padwEquipment[PARTS_LWEAPON] );
		} else
		{
			CItemElem *pItemElem = pInventory->GetEquip( PARTS_LWEAPON );		
			if( pItemElem )
				pLHandItemProp = pItemElem->GetProp();
		}
		
		// 조건검사.
		if( pItemProp->dwItemKind3 == IK3_SHIELD )	// 방패류를 착용하려 했을때
		{
			if( pHandItemProp )
			{
				if( pHandItemProp->dwHanded == HD_TWO )		// 손에 쥐고 있는게 투핸드면.
				{
//					pModel->TakeOffParts( PARTS_RWEAPON );	// 실패할 수 있으므로 아래로 이동
					if( pInventory )
					{
						CItemElem *pItemElemOld = pInventory->GetEquip( PARTS_RWEAPON );
						if( pInventory->UnEquip( PARTS_RWEAPON ) == FALSE )	
							return FALSE;
//							ASSERT( 0 );
#ifdef __WORLDSERVER
						// 투핸드 무기벗김
						if( pItemElemOld )
						{
							ItemProp *pOldItemProp = pItemElemOld->GetProp();
							pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );
							g_UserMng.AddDoEquip( pMover, pItemElemOld->m_dwObjId, pItemElemOld->m_dwItemId, pItemElemOld->GetAttrOption(), (BYTE)FALSE );
						}
#endif // worldserver
						if( padwEquipment ) padwEquipment[ PARTS_RWEAPON ] = NULL_ID;
					} else
					{
						if( padwEquipment )
							padwEquipment[ PARTS_RWEAPON ] = NULL_ID;
					}
					pModel->TakeOffParts( PARTS_RWEAPON );
				}
				else
				if( pLHandItemProp && pLHandItemProp->dwParts == PARTS_RWEAPON )	// 왼손에 무기가 쥐어져있으면. 왼손에 드는 무기도 PARTS이름은 모두 RWEAPON이다.
				{
					if( pInventory )
					{
						CItemElem *pItemElemOld = pInventory->GetEquip( PARTS_LWEAPON );
						if( pInventory->UnEquip( PARTS_LWEAPON ) == FALSE )	
							return FALSE;
 #ifdef __WORLDSERVER
						// 왼손 무기벗김
						if( pItemElemOld )
						{
							ItemProp *pOldItemProp = pItemElemOld->GetProp();
							pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );
							g_UserMng.AddDoEquip( pMover, pItemElemOld->m_dwObjId, pItemElemOld->m_dwItemId, pItemElemOld->GetAttrOption(), (BYTE)FALSE );
						}
 #endif // worldserver
						
						if( padwEquipment ) padwEquipment[ PARTS_LWEAPON ] = NULL_ID;
					} else
					{
						if( padwEquipment )
							padwEquipment[ PARTS_LWEAPON ] = NULL_ID;
					}
					pModel->TakeOffParts( PARTS_LWEAPON );
				}
			}
		}
		if( pItemProp->dwHanded == HD_TWO )		// 장착하려는 무기가 투핸드 였을때
		{
			if( pInventory == NULL )
			{
				if( padwEquipment[ PARTS_SHIELD ] != NULL_ID )		// 방패가 있었는가 검사해서 있으면 벗김.
				{
					pModel->TakeOffParts( PARTS_SHIELD );
					if( padwEquipment )
						padwEquipment[ PARTS_SHIELD ] = NULL_ID;
				}
				if( padwEquipment[ PARTS_RWEAPON ] != NULL_ID )		// 무기가 있었는가 검사해서 벗김.
				{
					pModel->TakeOffParts( PARTS_RWEAPON );
					if( padwEquipment )
						padwEquipment[ PARTS_RWEAPON ] = NULL_ID;
				}
			}
			else
			{
				CItemElem *pItemElemOld = pInventory->GetEquip( PARTS_SHIELD );
				if( pItemElemOld )
				{
					if( pInventory->UnEquip( PARTS_SHIELD ) == FALSE )	// 방패 벗김
						return FALSE;	// 인벤이 꽉차면 걍 취소
#ifdef __WORLDSERVER
					if( pItemElemOld )	// 기존장비가 있었다면
					{
						ItemProp *pOldItemProp = pItemElemOld->GetProp();
						
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );

						g_UserMng.AddDoEquip( pMover, pItemElemOld->m_dwObjId, pItemElemOld->m_dwItemId, pItemElemOld->GetAttrOption(), (BYTE)FALSE );
					}
#endif
					pModel->TakeOffParts( PARTS_SHIELD );
					if( padwEquipment ) padwEquipment[ PARTS_SHIELD ] = NULL_ID;
				}
				
				pItemElemOld = pInventory->GetEquip( PARTS_RWEAPON );
				if( pInventory->GetEquip( PARTS_RWEAPON ) )
				{
					if( pInventory->UnEquip( PARTS_RWEAPON ) == FALSE )	// 무기벗김
						return FALSE;	// 인벤이 꽉차면 걍 취소
#ifdef __WORLDSERVER
					if( pItemElemOld )	// 기존장비가 있었다면
					{
						ItemProp *pOldItemProp = pItemElemOld->GetProp();
						
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );

						g_UserMng.AddDoEquip( pMover, pItemElemOld->m_dwObjId, pItemElemOld->m_dwItemId, pItemElemOld->GetAttrOption(), (BYTE)FALSE );
					}
#endif
					pModel->TakeOffParts( PARTS_RWEAPON );
					if( padwEquipment ) padwEquipment[ PARTS_RWEAPON ] = NULL_ID;
				}
			}
		}
	}

	if( bEquip )
	{
		if( pInventory == NULL )
		{
			// 익스크루시브 아이템 제거 
			if( pItemProp->dwExclusive != NULL_ID )
			{
				pModel->TakeOffParts( pItemProp->dwExclusive );
				if( padwEquipment )
					padwEquipment[ pItemProp->dwExclusive ] = NULL_ID;
			}
			// 페이크 장착 
			if( padwEquipment )
			{
				padwEquipment[ dwParts ] = dwItemIndex;
			}
		}
		else
		{
			// 익스크루시브 아이템 제거 
			if( pItemProp->dwExclusive != NULL_ID )
			{
				//if( pInventory->GetAt( MAX_INVENTORY + pItemProp->dwExclusive ) )
				if( pInventory->GetEquip( pItemProp->dwExclusive ) )
					if( pInventory->UnEquip( pItemProp->dwExclusive ) == FALSE )
						return FALSE;
					//if( pInventory->UnEquip( MAX_INVENTORY + pItemProp->dwExclusive ) == FALSE )
//						ASSERT( 0 );
				pModel->TakeOffParts( pItemProp->dwExclusive );
			}
			// 기존 장비 제거 
			//if( pInventory->GetAt( MAX_INVENTORY + dwParts ) )
			if( bEquip )
			{
				if( dwParts == PARTS_RING1 || dwParts == PARTS_EARRING1 )
				{
					DWORD dwElem1 = NULL_ID, dwElem2 = NULL_ID;
					CItemElem* pItemElemTemp1 = (CItemElem*)pInventory->GetEquip( dwParts );
					CItemElem* pItemElemTemp2 = (CItemElem*)pInventory->GetEquip( dwParts + 1 );
					if( pItemElemTemp1 ) dwElem1 = pItemElemTemp1->m_dwItemId;
					if( pItemElemTemp2 ) dwElem2 = pItemElemTemp2->m_dwItemId;
					if( dwElem1 != NULL_ID && dwElem2 == NULL_ID )
						dwParts = pItemProp->dwPartsub;
				}
			}

			CItemElem *pItemElemOld = pInventory->GetEquip( dwParts );
			
			if( pItemElemOld )
			{
				//if( pInventory->UnEquip(MAX_INVENTORY + dwParts ) == FALSE )
				if( pInventory->UnEquip( dwParts ) == FALSE )
					return FALSE;
#ifdef __WORLDSERVER
				if( pItemElemOld )	// 기존장비가 있었다면
				{
					ItemProp *pOldItemProp = pItemElemOld->GetProp();

					pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );					
				}
#endif // worldserver
				
			}
			// 새 장비 장착 
			if( TRUE == pInventory->DoEquip( dwIndex, /*MAX_INVENTORY +*/ dwParts ) )
			{
				if( padwEquipment )
					padwEquipment[ dwParts ] = pItemElem->m_dwObjId;
			}
			else
			{
				WriteError( "%s %d", __FILE__, __LINE__ );
				return FALSE;
			}
		}

#ifndef __WORLDSERVER
		TCHAR szPartsName[ 32 ];
		if( pItemProp->bPartsFile == TRUE )
			prj.m_modelMng.MakePartsName( szPartsName, _T( "part" ), pItemProp->dwID, nSex );
		else
			prj.m_modelMng.MakeModelName( szPartsName, OT_ITEM, pItemProp->dwID );
			//prj.m_modelMng.MakePartsName( szPartsName, _T( "item" ), pItemProp->dwID );
		switch( dwParts )
		{
		case PARTS_RWEAPON: 
			((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
			switch( pItemProp->dwItemKind3 )
			{
			case IK3_KNUCKLEHAMMER:
				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
				break;
			case IK3_BOW:
				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
				break;
			case IK3_YOYO:
				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
				((CModelObject*)pModel)->LoadElement( szPartsName, PARTS_LWEAPON );
				((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
				break;
			default:
				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
			}
//			if( pItemProp->dwItemKind3 == IK3_KNUCKLEHAMMER )
//				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
//			else
//				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
			break;
		case PARTS_LWEAPON: 
			((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
			((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
			break;
		case PARTS_SHIELD: 
			((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
			((CModelObject*)pModel)->SetParent( PARTS_SHIELD, ((CModelObject*)pModel)->GetLArmIdx() );
			break;
		case PARTS_UPPER_BODY:
			if( bIfParts )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				_stprintf( lpszTemp, PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
			}
			break;
		case PARTS_LOWER_BODY:
			if( bIfParts )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				_stprintf( lpszTemp, PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
			}
			break;
		default:
			if( bIfParts )
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
			break;
		}
#endif // not WorldServer
	} // 장착.
	else
	// 탈착.
	{
		if( bIfParts )
		{
			if( pItemProp->dwExclusive != NULL_ID )
			{
				switch( pItemProp->dwExclusive )
				{
				case PARTS_UPPER_BODY:
					pModel->LoadElement( PARTSMESH_UPPER( nSex ), PARTS_UPPER_BODY );
					_stprintf( lpszTemp, PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
					pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
					break;
				case PARTS_LOWER_BODY:
					pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
					_stprintf( lpszTemp, PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
					pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
					break;
				case PARTS_HAND:
					pModel->LoadElement( PARTSMESH_HAND( nSex ), PARTS_HAND );
					break;
				case PARTS_FOOT:
					pModel->LoadElement( PARTSMESH_FOOT( nSex ), PARTS_FOOT );
					break;
				}
			}
		}
		//if( pInventory && pInventory->GetAt( MAX_INVENTORY + dwParts ) )
		if( pInventory && pInventory->GetEquip( dwParts ) )
		{
			//if( pInventory->UnEquip( MAX_INVENTORY + dwParts ) == FALSE )
			if( pInventory->UnEquip( dwParts ) == FALSE )
			{
#ifdef __WORLDSERVER
				if( TRUE == pMover->IsPlayer() )
					( (CUser*)pMover )->AddDefinedText(TID_GAME_LACKSPACE, "" );
#endif	// __WORLDSERVER				
				return FALSE;
			}
		}
		
		if( padwEquipment ) 
		{
			padwEquipment[ dwParts ] = NULL_ID;
			paEquipOption[ dwParts ] = 0;
		}

#ifndef __WORLDSERVER
		if( bIfParts )
		{
			switch( dwParts )
			{
			case PARTS_UPPER_BODY:
				pModel->LoadElement( PARTSMESH_UPPER( nSex ), PARTS_UPPER_BODY );
				_stprintf( lpszTemp, PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
				break;
			case PARTS_LOWER_BODY:
				pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
				_stprintf( lpszTemp, PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
				break;
			case PARTS_HAND:
				pModel->LoadElement( PARTSMESH_HAND( nSex ), PARTS_HAND );
				break;
			case PARTS_FOOT:
				pModel->LoadElement( PARTSMESH_FOOT( nSex ), PARTS_FOOT );
				break;
			}
		}
#endif
	} // 탈착
	return TRUE;
}
//}}AFX
#endif // 0


// 장착 가능한가를 검사.
#if __VER >= 11 // __SYS_IDENTIFY
BOOL CMover::IsEquipAble( CItemElem* pItem,BOOL bIgnoreLevel )
#else	// __SYS_IDENTIFY
BOOL CMover::IsEquipAble( ItemProp *pItemProp, OBJID dwObjid ,BOOL bIgnoreLevel )
#endif	// __SYS_IDENTIFY
{
#if __VER >= 11 // __SYS_IDENTIFY
	if( !pItem )
		return FALSE;
	ItemProp* pItemProp	= pItem->GetProp();
	OBJID dwObjid	= pItem->m_dwObjId;
#endif	// __SYS_IDENTIFY
	// 빗자루류 검사.
	if( pItemProp->dwParts == PARTS_RIDE )
	{
		int nLimitLv = pItemProp->dwFlightLimit;
		if( nLimitLv == NULL_ID )
			nLimitLv = 1;
#ifdef __NOLIMIT_RIDE_ITEM
		if( pItemProp->dwID == II_RID_RID_STI_MAGIC01 )
			nLimitLv = 0;
#endif // __NOLIMIT_RIDE_ITEM
		
		CWorld* pWorld = GetWorld();
		if( pWorld )
		{
			int nAttr	= pWorld->GetHeightAttribute( GetPos().x, GetPos().z );
			
			if( GetFlightLv() < nLimitLv )		// 비행레벨이 안되면 못탄다.
			{
#ifdef __WORLDSERVER
				if( TRUE == IsPlayer() )
					( (CUser*)this )->AddDefinedText( TID_GAME_USEAIRCRAFT, "" );	// 잠시만 기다려주세요.
#else // __WORLDSERVER
				g_WndMng.PutString( prj.GetText( TID_GAME_USEAIRCRAFT ), NULL, prj.GetTextColor( TID_GAME_USEAIRCRAFT ) );
#endif // __WORLDSERVER
				return FALSE;
			}
			
			if( !pWorld->m_bFly )
			{
#ifdef __WORLDSERVER
				( (CUser*)this )->AddDefinedText( TID_ERROR_NOFLY, "" );	// 비행금지구역입니다.
#else // __WORLDSERVER
				g_WndMng.PutString( prj.GetText( TID_ERROR_NOFLY ), NULL, prj.GetTextColor( TID_ERROR_NOFLY ) );
#endif // __WORLDSERVER
				return FALSE;
			}

			if( HasBuffByIk3( IK3_TEXT_DISGUISE ) )
			{
#ifdef __WORLDSERVER
				( (CUser*)this )->AddDefinedText( TID_QUEST_DISQUISE_NOTFLY, "" );	// 변신중에는 비행을 할수 없습니다
#else // __WORLDSERVER
				g_WndMng.PutString( prj.GetText( TID_QUEST_DISQUISE_NOTFLY ), NULL, prj.GetTextColor( TID_QUEST_DISQUISE_NOTFLY ) );
#endif // __WORLDSERVER
				return FALSE;
			}
			
			// 장착 이전에 검사하여야 한다.
			if( nAttr == HATTR_NOFLY )		// 비행금지구역에선 못탄다.
				return FALSE;
			
		#ifdef __WORLDSERVER
			if( m_pActMover->IsState( OBJSTA_STAND ) == FALSE )
				return FALSE;
		#endif 
		}

#if __VER >= 8 // __S8_PK
#ifdef __WORLDSERVER
		if( IsChaotic() )
		{
			CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( GetPKPropensity() );
			if( !Propensity.nFly )
			{
				((CUser*)this)->AddDefinedText( TID_GAME_CHAOTIC_NOT_FLY );
				return FALSE;
			}
		}
#endif // __WORLDSERVER
#endif // __VER >= 8 // __S8_PK

#if __VER >= 9	// __PET_0410
#ifdef __WORLDSERVER
		if( HasActivatedEatPet() || HasActivatedSystemPet() )	// 펫이 소환된 상태라면 비행 불가
		{
			( (CUser*)this )->AddDefinedText( TID_GAME_CANNOT_FLY_WITH_PET );
			return FALSE;
		}
#endif	// __WORLDSERVER
#endif	// __PET_0410
	}

	// 무기류가 아닌것(방어구)는 성별을 확인한다.
	{
		// 성별 확인( 무기는 빠짐 )
		if( pItemProp->dwItemSex != NULL_ID && pItemProp->dwItemSex != GetSex() )
		{
#ifdef __WORLDSERVER
			if( TRUE == IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_WRONGSEX, "\"%s\"", pItemProp->szName );
#endif	// __WORLDSERVER
			return FALSE;
		} // 성별
	}// 무기
	
	// 방어구 입을때 직업확인.
	if( pItemProp->dwItemKind1 != IK1_WEAPON )
	{
		if( pItemProp->dwItemJob != NULL_ID && FALSE == IsInteriorityJob( pItemProp->dwItemJob ) ) {
#ifdef __WORLDSERVER
			if( TRUE == IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_WRONGJOB, "\"%s\"", pItemProp->szName );
#endif	// __WORLDSERVER
			return FALSE;
		}
	}

	// 아이템 필요직업검사.
	if( pItemProp->dwItemKind1 == IK1_WEAPON )		// 무기류를 장착하려할때
	{
		if( pItemProp->dwItemJob != NULL_ID && FALSE == IsInteriorityJob( pItemProp->dwItemJob ) )	// 아이템에 필요직업이 지정되어 있고
		{
#ifdef __WORLDSERVER
			if( TRUE == IsPlayer() )
				( (CUser*)this )->AddDefinedText( TID_GAME_WRONGJOB, "\"%s\"", pItemProp->szName );
#endif	// __WORLDSERVER
			return FALSE;
		}
	}

	if( !g_eLocal.GetState( EVE_SCHOOL ) && !bIgnoreLevel )
	{

		// 아이템 필요레벨 검사.
		if( pItemProp->dwLimitLevel1 != 0xffffffff )	// 필요레벨이 지정되어 있고
		{
#if __VER >= 14 // __NEW_ITEM_LIMIT_LEVEL
			if( pItem->IsLimitLevel( this ) )	// 레벨이 안되면 못참.
#else // __NEW_ITEM_LIMIT_LEVEL
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
#if __VER >= 11 // __SYS_IDENTIFY
			if( m_nLevel < pItem->GetLimitLevel() && !IsMaster() && !IsHero()  )	// 레벨이 안되면 못참.				
#else	// __SYS_IDENTIFY
			if( ( m_nLevel < pItemProp->dwLimitLevel1 ) && !IsMaster() && !IsHero()  )	// 레벨이 안되면 못참.				
#endif	// __SYS_IDENTIFY
#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
			if( m_nLevel < pItemProp->dwLimitLevel1 )	// 아이템을 찰 레벨이 안되었으면 못참.
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
#endif // __NEW_ITEM_LIMIT_LEVEL
			{
#	ifdef __WORLDSERVER
				if( TRUE == IsPlayer() )
				{
#if __VER >= 11 // __SYS_IDENTIFY
					( (CUser*)this )->AddDefinedText( TID_GAME_REQLEVEL, "\"%d\"", pItem->GetLimitLevel() );
#else	// __SYS_IDENTIFY
					( (CUser*)this )->AddDefinedText( TID_GAME_REQLEVEL, "\"%d\"", pItemProp->dwLimitLevel1 );
#endif	// __SYS_IDENTIFY
				}
#	endif	// __WORLDSERVER
				return FALSE;
			}
		}
		// 보석류를 장착
		if( pItemProp->dwItemKind2 == IK2_JEWELRY )
		{
			if( pItemProp->dwLimitLevel1 != 0xffffffff )	// 제한레벨이 걸려있고
			{
#if __VER >= 14 // __NEW_ITEM_LIMIT_LEVEL
				if( pItem->IsLimitLevel( this ) )	// 레벨이 안되면 못참.
#else // __NEW_ITEM_LIMIT_LEVEL
#if __VER >= 10 // __LEGEND	//	10차 전승시스템	Neuz, World, Trans
#if __VER >= 11 // __SYS_IDENTIFY
				if( m_nLevel < pItem->GetLimitLevel() && !IsMaster() && !IsHero()  )	// 레벨이 안되면 못참.				
#else	// __SYS_IDENTIFY
				if( ( m_nLevel < pItemProp->dwLimitLevel1 ) && !IsMaster() && !IsHero()  )	// 레벨이 안되면 못참.				
#endif	// __SYS_IDENTIFY
#else //__LEGEND	//	10차 전승시스템	Neuz, World, Trans
				if( m_nLevel < pItemProp->dwLimitLevel1 )	// 레벨이 안되면 못참.
#endif	//__LEGEND	//	10차 전승시스템	Neuz, World, Trans
#endif // __NEW_ITEM_LIMIT_LEVEL
				{
#	ifdef __WORLDSERVER
					if( TRUE == IsPlayer() )
					{
#if __VER >= 11 // __SYS_IDENTIFY
						( (CUser*)this )->AddDefinedText( TID_GAME_REQLEVEL, "\"%d\"", pItem->GetLimitLevel() );
#else	// __SYS_IDENTIFY
						( (CUser*)this )->AddDefinedText( TID_GAME_REQLEVEL, "\"%d\"", pItemProp->dwLimitLevel1 );
#endif	// __SYS_IDENTIFY
					}
#	endif	// __WORLDSERVER
					return FALSE;
				}
			}
		}

	}
#if __VER < 8 // __S8_PK
	//카오용 아이템을 선한자가 장착할 수 없다.
	if( (pItemProp->nEvildoing < 0) && IsChaotic() == FALSE )
	{
	#ifdef __WORLDSERVER
		if( IsPlayer() )
			( (CUser*)this )->AddDefinedText( TID_PK_ITEM_MESSAGE1 , "" );	// 카오 아이템은 카오상태에서만...
	#endif	// __WORLDSERVER
			
		return FALSE;
	}
#endif // __VER < 8 // __S8_PK

	ItemProp *pHandItemProp	= NULL;
	
	// 들고있는 무기 프로퍼티 꺼냄.
	CItemElem *pItemElem = m_Inventory.GetEquip( PARTS_RWEAPON );		
	if( pItemElem )
		pHandItemProp = pItemElem->GetProp();

	// 화살을 착용하려 했을때 보우일때만 착용돼야 함
#ifdef __CROSSBOW
	if( pItemProp->dwItemKind3 == IK3_ARROW && ( pHandItemProp == NULL || ( pHandItemProp->dwItemKind3 != IK3_BOW && pHandItemProp->dwItemKind3 != IK3_CROSSBOW ) ))
		return FALSE;		
	if( pItemProp->dwItemKind3 == IK3_CROSSARROW && ( pHandItemProp == NULL || pHandItemProp->dwItemKind3 != IK3_CROSSBOW ))
		return FALSE;
#else // __CROSSBOW
	if( pItemProp->dwItemKind3 == IK3_ARROW && ( pHandItemProp == NULL || pHandItemProp->dwItemKind3 != IK3_BOW ) )
		return FALSE;	
#endif // __CROSSBOW
	return TRUE;
}

//
//
// pItemElem이 벗는게 가능한가를 검사.
BOOL CMover::IsUnEquipAble( ItemProp *pItemProp )
{
	if( pItemProp->dwParts == PARTS_RIDE )
	{
		if( m_pActMover->IsSit() )	// 빗자루는 앉은 상태에서는 탈착 금지
			return FALSE;

		if( GetWorld() )	// RedoEquip에서 불렀을때 NULL인경우 있음. NULL이면 걍 벗김
		{
			int nAttr = GetWorld()->GetHeightAttribute( GetPos().x, GetPos().z );		// 이동할 위치의 속성 읽음.
			// 비행금지 or 걷기금지 or 이동금지 지역에서 는 못내림.
			if( (nAttr == HATTR_NOFLY || nAttr == HATTR_NOWALK || nAttr == HATTR_NOMOVE ) )		
				return FALSE;
		}
	}

	if( g_eLocal.GetState( EVE_SCHOOL ) && pItemProp->dwItemKind3 == IK3_CLOAK )
		return FALSE;
	
	return TRUE;
}

//
//
// 정상적인 아이템 장착, 탈착 
BOOL CMover::DoEquip( CItemElem* pItemElem, BOOL bEquip, int nPart )
{
	ItemProp* pItemProp = pItemElem->GetProp();
	
	if( IsDie() )	// 죽은 상태에서는 장/탈착 금지
	{
		return FALSE;
	}

	if( bEquip )
	{
		if( pItemProp->dwParts == PARTS_RIDE && m_Inventory.GetEquip( PARTS_RIDE ) )
			return FALSE;
		if( pItemProp->dwParts == PARTS_RIDE && pItemElem && pItemElem->IsFlag( CItemElem::expired ) )
			return FALSE;
#if __VER >= 11 // __SYS_IDENTIFY
		if( IsEquipAble( pItemElem ) == FALSE )
			return FALSE;
#else	// __SYS_IDENTIFY
		if( IsEquipAble( pItemProp, pItemElem->m_dwObjId ) == FALSE )
			return FALSE;
#endif	// __SYS_IDENTIFY
	}
	else
	{
		if( IsUnEquipAble( pItemProp ) == FALSE )
			return FALSE;
	}

	EQUIP_INFO equipInfo;
	equipInfo.dwId	= pItemElem->m_dwItemId;
	equipInfo.nOption	= pItemElem->GetAttrOption();
	equipInfo.byFlag	= pItemElem->m_byFlag;
	BOOL bResult = DoEquip( GetSex(), m_dwSkinSet, pItemElem, nPart, equipInfo, &m_Inventory, m_aEquipInfo, (CModelObject*)m_pModel, bEquip, this );
		
	if( !bResult )	// 2004/04/27
		return FALSE;

#ifndef __CLIENT
	if( bEquip )
	{
#ifdef 	__BLADELWEAPON0608	//	9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정
		if( (pItemElem->m_dwObjIndex - m_Inventory.GetSize() ) != PARTS_LWEAPON  )	// equiped item
#endif	//__BLADELWEAPON0608	//	9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정
			SetDestParamEquip( pItemProp, pItemElem );		// 장착한 장비의 DestParam설정
#if __VER >= 14 // __EQUIP_BIND
		if( ( pItemProp->dwFlag & IP_FLAG_EQUIP_BIND ) && !pItemElem->IsFlag( CItemElem::binds ) )
		{
			pItemElem->SetFlag( CItemElem::binds );
			UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_FLAG, MAKELONG( pItemElem->m_dwObjIndex, pItemElem->m_byFlag ) );
		}

#endif // __EQUIP_BIND
	}
	else
	if( bEquip == FALSE )
	{
#ifdef 	__BLADELWEAPON0608	//	9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정
		if( (pItemElem->m_dwObjIndex - m_Inventory.GetSize() ) != PARTS_LWEAPON  )	// equiped item
#endif	//__BLADELWEAPON0608	//	9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정
			ResetDestParamEquip( pItemProp, pItemElem );	// 해제하는 장비의 DestParam리셋
		
		if( pItemProp->dwItemKind1 == IK1_WEAPON ) 
		{
			if( m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) )
				SendActMsg( OBJMSG_MODE_PEACE );
		}

	}
#endif	// __CLIENT

	if( pItemProp->dwParts == PARTS_RIDE )
	{
		if( bEquip )
		{
#ifdef __CLIENT	
			if( IsPlayer() )
			{
				if( g_WndMng.m_pWndWorld->m_bFirstFlying == FALSE )
				{
					PlayMusic(BGM_TH_FLYING);
					g_WndMng.m_pWndWorld->m_bFirstFlying = TRUE;
				}
				else
				{
					PlayMusic(BGM_EV_LIGHTWING);
				}
				g_WndMng.m_bAutoRun = FALSE;
			}
#endif//__CLIENT				
			SendActMsg( OBJMSG_MODE_FLY, pItemProp->dwID, 0, 1 );
		}
		else
		{
			SendActMsg( OBJMSG_MODE_GROUND, 0, 0, 1 );
		}
		ClearDest();	// 2004/04/30
		ClearDestAngle();
		memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
	}

	// 망토 입을때 처리.
	if( bEquip )
	{
		if( pItemProp->dwID == II_ARM_S_CLO_CLO_BLANK )		// 길드전용 민짜 망토.
		{
			m_idGuildCloak = pItemElem->m_idGuild;			// 망토에 박혀있던 길드아이디를 받음.
#ifdef __XCLOAK
			m_pCloakTexture = g_GuildTexture.LoadGuildTexture( pItemElem->m_idGuild );
#endif
		} 
		else
		{
#ifdef __XCLOAK
			m_pCloakTexture = NULL;
			m_idGuildCloak = 0;			// 망토에 박혀있던 길드아이디를 받음.
#endif
		}
	}

	UpdateBoundBox();
	UpdateParam();

	return bResult;
}


#ifdef __CLIENT
// 장착시 dwItemIndex만을 사용하는 이 Method는 Fake 아이템을 위해 사용 
// pModel에 값이 있는 경우는 좀 특별한 경우다. 그 경우는 장착/탈착 후에도 내부 장착에 관한 세팅에 변경이 없게 된다.
BOOL CMover::DoFakeEquip( const EQUIP_INFO & rEquipInfo, BOOL bEquip, int nPart, CModelObject* pModel )
{
	BOOL bResult;
	if( pModel )
		bResult	= CMover::DoEquip( GetSex(), m_dwSkinSet, NULL, nPart, rEquipInfo, NULL, m_aEquipInfo, (CModelObject*)pModel, bEquip, this );
	else
		bResult = CMover::DoEquip( GetSex(), m_dwSkinSet, NULL, nPart, rEquipInfo, NULL, m_aEquipInfo, (CModelObject*)m_pModel, bEquip, this );

	if( !bResult )	// 2004/04/27
	{
		Error( "DoFakeEquip() return FALSE" );
		return FALSE;
	}

	UpdateBoundBox();
	UpdateParam();

	ItemProp* pProp = prj.GetItemProp( rEquipInfo.dwId );
#ifndef __CLIENT
	if( pProp && pProp->dwItemKind1 >= IK2_WEAPON_DIRECT && pProp->dwItemKind1 <= IK2_WEAPON_MAGIC )
	{
		if( bEquip ) {
			if( !m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) )
				SendActMsg( OBJMSG_MODE_COMBAT );
		}
		else {
			if( m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) )
				SendActMsg( OBJMSG_MODE_PEACE );
		}
	}
#endif	// __CLIENT
	
	
	if( pProp->dwParts == PARTS_RIDE )
	{
		if( bEquip )
		{
			SendActMsg( OBJMSG_MODE_FLY, pProp->dwID, 0, 1 );
			ClearDest();
		}
		else
		{
			SendActMsg( OBJMSG_MODE_GROUND, 0, 0, 1 );
			ClearDest();
		}
	}
	// 망토 입을때 처리.
	if( bEquip )
	{
		if( pProp->dwID == II_ARM_S_CLO_CLO_BLANK )		// 길드전용 민짜 망토.
		{
#ifdef __XCLOAK
			m_pCloakTexture = g_GuildTexture.LoadGuildTexture( m_idGuildCloak );
#endif
		} else
		{
#ifdef __XCLOAK
			m_pCloakTexture = NULL;
#endif
		}
	}

	return bResult;
}
#endif // CLIENT

void CMover::RedoEquip( BOOL fFakeParts, BOOL bDestParam )
{
	CItemElem* pItemElem = NULL;
	ItemProp* pItemProp	= NULL;
	BOOL bEquip;

	// 장착될 아이템의 선 처리 
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		bEquip	= fFakeParts? ( m_aEquipInfo[i].dwId != NULL_ID ) : ( m_Inventory.GetEquip( i ) != NULL );
		if( bEquip )
		{
			pItemElem = NULL;			pItemProp = NULL;
			
			// 장착된 아이템의 프로퍼티 꺼냄.
			if( !fFakeParts )	// 진짜 장비냐.
			{
				pItemElem	= m_Inventory.GetEquip( i );
				if( NULL != pItemElem )
					pItemProp	= pItemElem->GetProp();	

#ifdef __WORLDSERVER
#ifdef __TESTSERVER
				if( pItemProp )
				{
					if( pItemProp->dwParts == i || pItemProp->dwPartsub == i || 
						( pItemProp->dwParts == PARTS_RWEAPON && i == PARTS_LWEAPON ) )
					{
					}
					else
					{
						WriteError( "PARTS//%s//%d", GetName(), i );
						CItemElem itemElem;
						itemElem	= *pItemElem;
						RemoveItemId( pItemElem->m_dwObjId );
						AddItem( &itemElem );
						continue;
					}
				}
#endif	// __TESTSERVER
#endif	// __WORLDSERVER
			}
			else
			{
				pItemProp	= prj.GetItemProp( m_aEquipInfo[i].dwId );
			}

			
				
			if( ( pItemProp != NULL && pItemProp->dwID > 0 && 
				( pItemProp->dwParts == i || pItemProp->dwPartsub == i || 
				  (pItemProp->dwParts == PARTS_RWEAPON && i == PARTS_LWEAPON) )		// RWEAPON은 LWEAPON과 같은것으로 친다.
				) &&
				( fFakeParts || m_Inventory.IsEquip( pItemElem->m_dwObjId ) ) )
			{
				// 빗자루를 장착할껀데
				if( i == PARTS_RIDE )
				{
					if( FALSE == m_pActMover->IsFly() )	// 비행중이 아니면
					{
//						WriteLog( "RedoEquip(): OBJSTAF_FLY" );
						SendActMsg( OBJMSG_MODE_FLY, pItemProp->dwID, 0, 1 );	// 비행모드로 전환
					}
					if( m_dwRideItemIdx == 0 )	// 빗자루 ID가 없으면
					{
//						WriteLog( "RedoEquip(): m_dwRideItemIdx	= pItemProp->dwID" );
						m_dwRideItemIdx	= pItemProp->dwID;	// 값 보정.
					}
				} 
				if( pItemProp->dwID == II_ARM_S_CLO_CLO_BLANK )		// 길드전용 민짜 망토.
				{
					if( pItemElem )		// 서버 / 클라(액티브무버)
						m_idGuildCloak = pItemElem->m_idGuild;
 #ifdef __XCLOAK
					m_pCloakTexture = g_GuildTexture.LoadGuildTexture( m_idGuildCloak );
 #endif
				}
				if( pItemElem )
					m_aEquipInfo[i].nOption	= pItemElem->GetAttrOption();

				if( bDestParam )
				{
#ifdef 	__BLADELWEAPON0608	//	9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정
					if( i != PARTS_LWEAPON )	// equiped item
#endif	//__BLADELWEAPON0608	//	9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정
						SetDestParamEquip( pItemProp, pItemElem, TRUE );
#if __VER >= 9 // __ULTIMATE
					if( i == PARTS_RWEAPON )
						prj.m_UltimateWeapon.SetDestParamUltimate( this, pItemElem, TRUE, PARTS_RWEAPON );
#endif // __ULTIMATE
				}
			}
			else
			{
				WriteLog( "RedoEquip(): Illegal dwParts, %s, %d", m_szName, i );
				m_aEquipInfo[i].dwId	= NULL_ID;
			}
		}
		else 
		{	// 장착될게 없음.
			if( i == PARTS_RIDE )	// 빗자루가 장착되어 있지 않은데
			{
				if( m_pActMover->IsFly() )	// 비행중이다.
				{
//					WriteLog( "RedoEquip(): OBJSTAF_GROUND" );
					SendActMsg( OBJMSG_MODE_GROUND, 0, 0, 1 );	// 그러면 지상모드로 변환 시켜야지.
				}
				if( m_dwRideItemIdx != 0 )		// 빗자루 인덱스도 0이어야 하는데 뭔가 있다.
				{
//					WriteLog( "RedoEquip(): m_dwRideItemIdx	= 0" );
					m_dwRideItemIdx		= 0;	// 그러면 0으로 클리어.
				}
				if( m_Inventory.GetEquip( i ) )
				{
//					WriteLog( "RedoEquip(): pRide is not null" );
					m_Inventory.UnEquip( i );
				}
			}
		}
	}


	pItemElem	= m_Inventory.GetEquip( PARTS_RWEAPON );		// 위에 루프에서 모든파츠를 이렇게 일괄적으로 넣어도 될듯 싶다.
	if( pItemElem )
	{
		m_aEquipInfo[PARTS_RWEAPON].nOption	= pItemElem->GetAttrOption();
	}
	pItemElem	= m_Inventory.GetEquip( PARTS_LWEAPON );
	if( pItemElem )
	{
		m_aEquipInfo[PARTS_LWEAPON].nOption	= pItemElem->GetAttrOption();
	}

	// 아이템이 실제 장착.
	UpdateParts( fFakeParts );

#ifdef __WORLDSERVER
	InvalidEquipOff( fFakeParts );		// 장착되어설 안될 장비가 장착되어 있다면 벗김
#endif

	if( bDestParam )
	{
		int nAbilityOption	= GetSetItem();
		if( nAbilityOption > 0 )
			SetSetItemAvail( nAbilityOption );

		
		SetDestParamSetItem( NULL );
	}
}

// 장착되어설 안될 장비가 장착되어 있다면 벗김
int	CMover::InvalidEquipOff( BOOL bFakeParts )
{
	CItemElem* pItemElem = NULL;
	ItemProp* pItemProp	= NULL;
	int		i;

	for( i = 0; i < MAX_HUMAN_PARTS; i ++ )
	{
		pItemElem = NULL;		
		pItemProp = NULL;

		// 장착된 아이템의 프로퍼티 꺼냄.
		if( bFakeParts )	// Fake 장비
		{
			if( m_aEquipInfo[i].dwId	== NULL_ID )	continue;
			pItemProp	= prj.GetItemProp( m_aEquipInfo[i].dwId );
		}
		else
		{
			pItemElem = m_Inventory.GetEquip( i );
			if( pItemElem )
				pItemProp  = pItemElem->GetProp();	
		}

		if( pItemProp == NULL )	
			continue;	// 프로퍼티 없으면 실패.

#if __VER >= 8 // __S8_PK
#if __VER >= 11 // __SYS_IDENTIFY
		if( IsEquipAble( pItemElem,TRUE ) == FALSE )	// 장착할 수 없는 아이템이다.
#else	// __SYS_IDENTIFY
		if( IsEquipAble( pItemProp, pItemElem->m_dwObjId, TRUE ) == FALSE )	// 장착할 수 없는 아이템이다.
#endif	// __SYS_IDENTIFY
#else	// __S8_PK
		if( IsEquipAble( pItemProp, pItemElem->m_dwObjId) == FALSE )	// 장착할 수 없는 아이템이다.
#endif	// __S8_PK
		{
			if( pItemElem )
			{
				if( DoEquip( pItemElem, FALSE ) == FALSE )		// 벗김.
				{
				#ifdef __WORLDSERVER
					// 벗기는데 실패함. 인벤이 꽉찼다거나 기타등등 이유
					#ifndef _DEBUG
						Error( "아템 벗기는데 실패:%s", GetName() );
					#endif
				#endif
				}
			} 
		}
	} // for
	
	return FALSE;
}

void CMover::SetDestParamEquip( ItemProp* pItemProp, CItemElem* pItemElem, BOOL bIgnoreSetItem )
{
	if( pItemElem && pItemElem->IsFlag( CItemElem::expired ) )
		return;

#ifdef __WORLDSERVER
	RunItemScript( this, pItemProp->dwID, ITEM_OP_EQUIP, NULL );
#endif // __WORLDSERVER

	if( pItemProp->dwDestParam[0] != -1 )
		SetDestParam( 0, pItemProp, 1 );
	if( pItemProp->dwDestParam[1] != -1 )
		SetDestParam( 1, pItemProp );
#ifdef __PROP_0827
	if( pItemProp->dwDestParam[2] != -1 )
		SetDestParam( 2, pItemProp );
#endif	// __PROP_0827
#if __VER >= 19
	if( pItemProp->dwDestParam[3] != -1 )
		SetDestParam( 3, pItemProp, 1 );
	if( pItemProp->dwDestParam[4] != -1 )
		SetDestParam( 4, pItemProp );
	if( pItemProp->dwDestParam[5] != -1 )
		SetDestParam( 5, pItemProp );	
#endif// __VER >= 19

	// 원소별 속성이 붙은 아이템일경우 세팅함.
	
	// 방어구이고 속성옵션값이 있으면...
	if( pItemElem && pItemElem->m_nResistAbilityOption && pItemElem->GetProp()->dwItemKind1 == IK1_ARMOR )
	{
		if( pItemElem->m_bItemResist != SAI79::NO_PROP )
		{
			int nStrong = GetStrongDST( pItemElem->m_bItemResist );
			int nWeak   = GetWeakDST( pItemElem->m_bItemResist );

			SetDestParam( nWeak,  -pItemElem->m_nResistAbilityOption*2, NULL_CHGPARAM );
			SetDestParam( nStrong, pItemElem->m_nResistAbilityOption*2, NULL_CHGPARAM );
		}
	}
	else
	{
		if( pItemProp->nItemResistElecricity )
			SetDestParam( DST_RESIST_ELECTRICITY, pItemProp->nItemResistElecricity, NULL_CHGPARAM );
		if( pItemProp->nItemResistFire )
			SetDestParam( DST_RESIST_FIRE, pItemProp->nItemResistFire, NULL_CHGPARAM );
		if( pItemProp->nItemResistWind )
			SetDestParam( DST_RESIST_WIND, pItemProp->nItemResistWind, NULL_CHGPARAM );
		if( pItemProp->nItemResistWater )
			SetDestParam( DST_RESIST_WATER, pItemProp->nItemResistWater, NULL_CHGPARAM );
		if( pItemProp->nItemResistEarth )
			SetDestParam( DST_RESIST_EARTH, pItemProp->nItemResistEarth, NULL_CHGPARAM );
	}

#if __VER >= 11 // __SYS_COLLECTING
	CAccessoryProperty* pProperty	= CAccessoryProperty::GetInstance();
	if( pItemElem && pItemElem->IsAccessory() )		// 액세서리
	{
		vector<SINGLE_DST>* pDst	= pProperty->GetDst( pItemElem->m_dwItemId, pItemElem->GetAbilityOption() );
		for( DWORD i = 0; i < pDst->size(); i++ )
			SetDestParam( (*pDst)[i].nDst, (*pDst)[i].nAdj, NULL_CHGPARAM );
	}
#endif	// __SYS_COLLECTING

#ifdef __WORLDSERVER
#ifdef __DST_GIFTBOX
	if( pItemProp->dwDestParam[0] == DST_GIFTBOX || pItemProp->dwDestParam[1] == DST_GIFTBOX || pItemProp->dwDestParam[2] == DST_GIFTBOX
	 || pItemProp->dwDestParam[3] == DST_GIFTBOX || pItemProp->dwDestParam[4] == DST_GIFTBOX || pItemProp->dwDestParam[5] == DST_GIFTBOX )
		static_cast<CUser*>( this )->AddBuff( BUFF_EQUIP, (WORD)( pItemProp->dwID ), 1, 999999999 );
#endif // __DST_GIFTBOX
#endif // __WORLDSERVER

	if( !bIgnoreSetItem )
	{
		if( pItemElem )
		{
			int nAbilityOption	= GetSetItem( pItemElem );
			if( nAbilityOption > 0 )
				SetSetItemAvail( nAbilityOption );
			SetDestParamSetItem( pItemElem );
		}
	}

	if( pItemElem && !pItemElem->IsFlag( CItemElem::expired ) )
	{
		DestParamPiercingAvail( pItemElem, TRUE );
		SetDestParamRandomOpt( pItemElem );
	}
}

void CMover::ResetDestParamEquip( ItemProp* pItemProp, CItemElem* pItemElem )
{
	if( pItemElem && pItemElem->IsFlag( CItemElem::expired ) )
		return;

#ifdef __WORLDSERVER
	RunItemScript( this, pItemProp->dwID, ITEM_OP_UNEQUIP, NULL );
#endif // __WORLDSERVER

	if( pItemProp->dwDestParam1 != -1 )
		ResetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1 );
	if( pItemProp->dwDestParam2 != -1 )
		ResetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2 );
#ifdef __PROP_0827
	if( pItemProp->dwDestParam3 != -1 )
		ResetDestParam( pItemProp->dwDestParam3, pItemProp->nAdjParamVal3 );
#endif	// __PROP_0827
#if __VER >= 19
	if( pItemProp->dwDestParam4 != -1 )
		ResetDestParam( pItemProp->dwDestParam4, pItemProp->nAdjParamVal4 );
	if( pItemProp->dwDestParam5 != -1 )
		ResetDestParam( pItemProp->dwDestParam5, pItemProp->nAdjParamVal5 );
	if( pItemProp->dwDestParam6 != -1 )
		ResetDestParam( pItemProp->dwDestParam6, pItemProp->nAdjParamVal6 );
#endif//__VER >= 19



	if( pItemElem && pItemElem->m_nResistAbilityOption && pItemElem->GetProp()->dwItemKind1 == IK1_ARMOR )
	{
		if( pItemElem->m_bItemResist != SAI79::NO_PROP )
		{
			int nSelf   = GetSelfDST( pItemElem->m_bItemResist );
			int nStrong = GetStrongDST( pItemElem->m_bItemResist );
			int nWeak   = GetWeakDST( pItemElem->m_bItemResist );

			if( ::GetLanguage() == LANG_THA )			
			{
				float fResult = ((float)pItemElem->m_nResistAbilityOption+1.2f) * (0.7f+(float)(pItemElem->m_nResistAbilityOption*0.01f));
				int nResult = (int)( fResult * fResult );

				ResetDestParam( nSelf,   nResult );
				ResetDestParam( nWeak,  -nResult/2 );
				ResetDestParam( nStrong, nResult/2 );
			}
			else
			{
				ResetDestParam( nWeak,  -pItemElem->m_nResistAbilityOption*2 );
				ResetDestParam( nStrong, pItemElem->m_nResistAbilityOption*2 );
			}
		}
	} 
	else
	{	
		if( pItemProp->nItemResistElecricity )
			ResetDestParam( DST_RESIST_ELECTRICITY, pItemProp->nItemResistElecricity);
		if( pItemProp->nItemResistFire )
			ResetDestParam( DST_RESIST_FIRE, pItemProp->nItemResistFire );
		if( pItemProp->nItemResistWind )
			ResetDestParam( DST_RESIST_WIND, pItemProp->nItemResistWind );
		if( pItemProp->nItemResistWater )
			ResetDestParam( DST_RESIST_WATER, pItemProp->nItemResistWater );
		if( pItemProp->nItemResistEarth )
			ResetDestParam( DST_RESIST_EARTH, pItemProp->nItemResistEarth );
	}

	if( pItemElem )
	{
#if __VER >= 11 // __SYS_COLLECTING
		CAccessoryProperty* pProperty	= CAccessoryProperty::GetInstance();
		if( pItemElem->IsAccessory() )		// 액세서리
		{
			vector<SINGLE_DST>* pDst	= pProperty->GetDst( pItemElem->m_dwItemId, pItemElem->GetAbilityOption() );
			for( DWORD i = 0; i < pDst->size(); i++ )
				ResetDestParam( (*pDst)[i].nDst, (*pDst)[i].nAdj );
		}
#endif	// __SYS_COLLECTING

#ifdef __WORLDSERVER
#ifdef __DST_GIFTBOX
		if( pItemProp->dwDestParam[0] == DST_GIFTBOX || pItemProp->dwDestParam[1] == DST_GIFTBOX || pItemProp->dwDestParam[2] == DST_GIFTBOX
		 || pItemProp->dwDestParam[3] == DST_GIFTBOX || pItemProp->dwDestParam[4] == DST_GIFTBOX || pItemProp->dwDestParam[5] == DST_GIFTBOX)
		{
			((CUser*)this)->RemoveBuff( BUFF_EQUIP, (WORD)( pItemProp->dwID ) );
		}
#endif // __DST_GIFTBOX
#endif // __WORLDSERVER

		int nAbilityOption	= GetSetItem( pItemElem );
		if( nAbilityOption > 0 )
			ResetSetItemAvail( nAbilityOption );
		DestParamPiercingAvail( pItemElem, FALSE );
		ResetDestParamSetItem( pItemElem );
		ResetDestParamRandomOpt( pItemElem );
	}
}


// 돈획득 	
void CMover::PickupGold( int nGold, BOOL bDropMob )
{
	ASSERT( nGold > 0 );
	if( nGold <= 0 )
	{
		Error( "%s PickupGold - %d", GetName(), nGold );
		return;
	}

#ifdef __WORLDSERVER
	CParty *pParty	= g_PartyMng.GetParty( m_idparty );
	if( pParty && pParty->IsMember( m_idPlayer ) && bDropMob )	// 파티가 있고 몹이 떨군 돈만 분배된다.
	{
		// 파티가 있는 상태에서 돈을 집으려면 돈이 분배된다.
		// 몬스터가 드롭한돈만 분배되어야 한다.
		int nNumInsideMember = 1;		// 돈 줍는사람 중심으로 반경 xx미터 이내있는사람이 몇명이나 되는지 검사. 
		CUser *pListMember[MAX_PTMEMBER_SIZE];
		int	nMaxListMember = 0;
		CUser* pMember	= NULL;
		int i;
//		float fDist;
		D3DXVECTOR3	vDist;

		// 돈줍는사람 반경안에 드는 멤버들만 추려낸다.
		memset( pListMember, 0, sizeof(pListMember) );
		for( i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			pMember	= g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			/*
			if( IsValidObj( pMember ) )
			{
				vDist = pMember->GetPos() - GetPos();
				fDist = D3DXVec3LengthSq( &vDist );		// 돈 줍는사람과 멤버간의 거리.
				if( fDist < 32.0f * 32.0f )				// xx미터보다 작으면
				{
					pListMember[ nMaxListMember++ ] = pMember;
				}
			}
			*/ //2009.01.21 // 반경 검사에 레이어 추가
			if( IsValidArea( pMember, 32.0f ) )
				pListMember[ nMaxListMember++ ] = pMember;
		}

		if( nMaxListMember > 0 )
		{
			int nShare = nGold / nMaxListMember;		// 멤버 각각의 몫
			int nRest  = nGold % nMaxListMember;		// 나누고난 후 나머지.
			if( nShare )	// 몫이 있을때.
			{
				for( i = 0; i < nMaxListMember; i++ )
				{
					pMember = pListMember[i];
					pMember->PickupGoldCore( nShare );
				}
			}

			// 나머지는 랜덤으로 한사람이 가짐.
			if( nRest )
			{
				pMember = pListMember[ random(nMaxListMember) ];	
				pMember->PickupGoldCore( nRest );
			}
		}
		else
		{
			FILEOUT( "..\\PartyError.txt", "party pick money error:%s %d %d", GetName(), pParty->m_nSizeofMember, nMaxListMember );
			for( i = 0; i < pParty->m_nSizeofMember; i ++ )
			{
				pMember		= g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
				if( IsValidObj( pMember ) )
				{
					FILEOUT( "..\\PartyError.txt", "%d %s", i, pMember->GetName() );
				}
			}
		}
	} 
	else
#endif // WORLDSERVER	
	{
		// 파티 없을때
		PickupGoldCore( nGold );
	}
}


void CMover::PickupGoldCore( int nGold )
{
	if( AddGold( nGold ) == FALSE )
		return;

#ifdef __WORLDSERVER
	if( IsPlayer() )
	{
		((CUser*)this)->AddGoldText( nGold );
		if( nGold >= 500 )
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "R";
			aLogItem.SendName = "GROUND";
			aLogItem.RecvName = GetName();
			aLogItem.WorldId = GetWorld()->GetID();
			aLogItem.Gold = GetGold() - nGold;
			aLogItem.Gold2 = GetGold();
			//aLogItem.ItemName = "SEED";
			_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
			aLogItem.itemNumber = nGold;
			g_DPSrvr.OnLogItem( aLogItem );
		}										
	}
#endif	// __WORLDSERVER
}


#ifdef __WORLDSERVER
// 돈을 땅에 떨군다.  bPK - PK당한 경우 
CItem* CMover::DropGold( DWORD dwGold, const D3DXVECTOR3& vPos, BOOL bPK )
{
	if( IsDropableState( bPK ) == FALSE )
		return NULL;

	CWorld *pWorld = GetWorld();

	int nGold	= (int)dwGold;
	if( nGold > 0 && nGold <= (int)GetGold() )
	{
		if( nGold >= 500 )
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "D";
			aLogItem.SendName = GetName();
			aLogItem.RecvName = "GROUND";	// 땅에 버린것
			aLogItem.WorldId = pWorld->GetID();
			aLogItem.Gold = GetGold();
			aLogItem.Gold2 = GetGold() - nGold;
			//aLogItem.ItemName = "SEED";
			_stprintf( aLogItem.szItemName, "%d", II_GOLD_SEED1 );
			aLogItem.itemNumber = nGold;
			g_DPSrvr.OnLogItem( aLogItem );
		}

		AddGold( -nGold );

		CItemElem* pItemElem	= new CItemElem;
		
		if( nGold <= (int)prj.GetItemProp( II_GOLD_SEED1 )->dwAbilityMax )
			pItemElem->m_dwItemId	= II_GOLD_SEED1;
		else if( nGold <= (int)prj.GetItemProp( II_GOLD_SEED2 )->dwAbilityMax )
			pItemElem->m_dwItemId	= II_GOLD_SEED2;
		else if( nGold <= (int)prj.GetItemProp( II_GOLD_SEED3 )->dwAbilityMax )
			pItemElem->m_dwItemId	= II_GOLD_SEED3;
		else
			pItemElem->m_dwItemId	= II_GOLD_SEED4;

		pItemElem->m_nItemNum	= (short)nGold;				// __NPP_050308 이후에는 사용하지 않을 것
		pItemElem->m_nHitPoint  = nGold;					// int형으로 돈을 버리게 하기 위해서 이 변수를 사용 

		CItem* pItem	= new CItem;
		pItem->m_pItemBase	= pItemElem;
		pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
		pItem->SetPos( vPos );
		pItem->SetAngle( (float)xRandom( 360 ) );
		pItem->m_dwDropTime		= timeGetTime();	
		pWorld->ADDOBJ( pItem, TRUE, GetLayer() );
		return pItem;
	}
	
	return NULL;
}
#endif // __WORLDSERVER

/*
#ifdef __NEWITEM0624
BOOL CMover::RemoveEquipedItem( int nParts )
{
	CItemElem* pItemElem	= GetEquipItem( nParts );
	if( pItemElem )
	{
#ifdef __WORLDSERVER
		ItemProp* pItemProp		= pItemElem->GetProp();
		ResetDestParamEquip( pItemProp, pItemElem );
		( (CModelObject*)m_pModel )->TakeOffParts( nParts );
		UpdateBoundBox();
		UpdateParam();
		g_UserMng.AddRemoveItemAtId( this, pItemElem->m_dwObjId );
#endif	// __WORLDSERVER
		m_Inventory.RemoveAtId( pItemElem->m_dwObjId );
		return TRUE;
	}
	return FALSE;
}

#ifdef __WORLDSERVER
BOOL CMover::DropEquipedItem( int nParts )
{
	if( !GetWorld() )
		return FALSE;

	CItemElem* pItemElem	= GetEquipItem( nParts );
	if( pItemElem )
	{
		ItemProp* pItemProp		= pItemElem->GetProp();
		ResetDestParamEquip( pItemProp, pItemElem );
		CItem* pItem	= new CItem;
		pItem->m_pItemBase	= new CItemElem;
		*pItem->m_pItemBase		= *pItemElem;
		pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
		pItem->SetPos( GetPos() );
		pItem->SetAngle( xRandom( 360 ) );
		GetWorld()->AddObj( pItem, TRUE );
		g_UserMng.AddRemoveItemAtId( this, pItemElem->m_dwObjId );
		m_Inventory.RemoveAtId( pItemElem->m_dwObjId );
		return TRUE;
	}
	return FALSE;
}
#endif	// __WORLDSERVER
#endif	// __NEWITEM0624
*/


#ifdef __CLIENT
BOOL CMover::GetEquipFlag( int nParts, BYTE* pbyFlag )
{
	if( IsActiveMover() )
	{
		CItemElem* pItemElem	= GetEquipItem( nParts );
		if( pItemElem )
		{
			*pbyFlag	= pItemElem->m_byFlag;
			return TRUE;
		}
		return FALSE;
	}
	else
	{
		if( nParts < 0 || nParts >= MAX_HUMAN_PARTS )
			return FALSE;

		if( m_aEquipInfo[nParts].dwId != NULL_ID )
		{
			*pbyFlag	= m_aEquipInfo[nParts].byFlag;
			return TRUE;
		}
		return FALSE;
	}
}
#endif	// __CLIENT


// 장비를 장/탈착한다.
// pItemElem - 장착/탈착 할 아이템
// bEquip - TRUE 장착
// nPart - 파츠번호 
void CMover::EquipItem( CItemElem *pItemElem, BOOL bEquip, int nPart )
{
	if( m_pActMover && m_pActMover->IsActAttack() )
		return;

	if( DoEquip( pItemElem, bEquip, nPart ) )
	{
	#ifdef __WORLDSERVER
		g_UserMng.AddDoEquip( this, nPart, pItemElem, bEquip );
		if( pItemElem->GetProp()->dwParts == PARTS_RIDE && pItemElem->GetProp()->dwItemJob == JOB_VAGRANT && bEquip == FALSE )
//			( ( pItemElem->m_dwItemId == II_RID_RID_BOR_EVEINSHOVER || pItemElem->m_dwItemId == II_RID_RID_BOR_LADOLF ) && bEquip == FALSE )
			RemoveItem( (BYTE)( pItemElem->m_dwObjId ), pItemElem->m_nItemNum );
	#endif // __WORLDSERVER
	}
}

// 탈 것에서 내린다.
void CMover::UnequipRide()
{
	if( IsPlayer() )
	{
		CItemElem* pItemElem = GetEquipItem( PARTS_RIDE );
		if( pItemElem )
			EquipItem( pItemElem, FALSE, -1 );	// -1 파츠번호 
	}
}


// 장비아이템을 사용한다.( 장착 -> 탈착, 탈착 -> 장착 )
// 주의: pItemElem은 IsUsableItem으로 검사된 상태라고 가정한다.
BOOL CMover::DoUseEquipmentItem( CItemElem* pItemElem, DWORD dwId, int nPart )
{ 
#ifdef __WORLDSERVER
	BOOL bEquip	= !m_Inventory.IsEquip( dwId );	// equip
	if( bEquip && IsItemRedyTime( pItemElem->GetProp(), pItemElem->m_dwObjId, TRUE ) == FALSE )
		return FALSE;

	// chipi_090731 - 클라이언트 변조시 문제 발생
	ItemProp* pItemProp = pItemElem->GetProp();
	if( !pItemProp )
		return FALSE;
		
	if( pItemProp->dwParts != nPart && nPart != -1)
	{
		CItemElem* pTempElem = m_Inventory.GetAt( MAX_INVENTORY + nPart );
		if( !pTempElem || pTempElem->GetProp()->dwParts != pItemProp->dwParts )
			return FALSE;
	}
	// END chipi_090731 - 클라이언트 변조시 문제 발생

	EquipItem( pItemElem, bEquip, nPart );
#endif // __WORLDSERVER

	return TRUE;
}

void CMover::ClearEquipInfo()
{
	memset( m_aEquipInfo, 0, sizeof(EQUIP_INFO) * MAX_HUMAN_PARTS );
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		m_aEquipInfo[i].dwId = NULL_ID;
}
