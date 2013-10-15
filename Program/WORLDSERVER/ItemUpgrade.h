#pragma once
#if __VER >= 12 // __EXT_PIERCING

#if __VER >= 13 // __EXT_ENCHANT
struct __ATTRIBUTE_ENCHANT
{
	int nProb;
	int nAddDamageRate;
#if __VER >= 14 // __EXT_ATTRIBUTE
	int nDefenseRate;
	int nAddAtkDmgRate;
#endif // __EXT_ATTRIBUTE
	__ATTRIBUTE_ENCHANT()
	{
		nProb = 0;
		nAddDamageRate = 0;
#if __VER >= 14 // __EXT_ATTRIBUTE
		nDefenseRate = 0;
		nAddAtkDmgRate = 0;
#endif // __EXT_ATTRIBUTE
	}
};
#endif // __EXT_ENCHANT

#ifdef __NEW_ITEM_VARUNA
struct __CHAOS_GEM
{
	int nChaosGemAmount;
	int nChaosGemChance;
	int nChaosPieceAmount;
	int nChaosPieceChance;
	__CHAOS_GEM()
	{
		nChaosGemAmount = 0;
		nChaosGemChance = 0;
		nChaosPieceAmount = 0;
		nChaosPieceChance = 0;
	}
};
#endif // __NEW_ITEM_VARUNA

class CItemUpgrade
{
public:
	CItemUpgrade(void);
	~CItemUpgrade(void);
	static CItemUpgrade*	GetInstance( void );

	void	LoadScript();

#ifdef __BARUNA
	void	LoadScriptBaruna();
/*	int		GetChaosGemAmount( int nAbilityOption );
	int		GetChaosGemChance( int nAbilityOption );
	int		GetChaosGemPieceAmount( int nAbilityOption );
	int		GetChaosGemPieceChance( int nAbilityOption );*/
	int		GetBarunaUp( int idx );
	int		GetBarunaDown( int idx );
//	int		GetBarunaPetFactor( int idx );
	int		GetElementDown( int idx );
	int		GetElementUp( int idx );
#endif

	void	OnPiercingSize( CUser* pUser, DWORD dwId1, DWORD dwId2, DWORD dwId3 );
	int		GetSizeProb( CItemElem* pItemElem );
	void	OnPiercing( CUser* pUser, DWORD dwItemId, DWORD dwSocketCard );
	void	OnPiercingRemove( CUser* pUser, DWORD objId );

#if __VER >= 14 // __SMELT_SAFETY
	BYTE	OnSmeltSafety( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr );
#endif // __SMELT_SAFETY
#if __VER >= 13 // __EXT_ENCHANT
	void	OnEnchant( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	int		GetAttributeDamageFactor( int nAbilityOption );
#if __VER >= 14 // __EXT_ATTRIBUTE
	int		GetAttributeDefenseFactor( int nAbilityOption );
	int		GetAttributeAddAtkDmgFactor( int nAbilityOption );
#endif // __EXT_ATTRIBUTE
	int		GetMaxGeneralEnchantSize() { return m_mapGeneralEnchant.size(); }
	int		GetMaxAttributeEnchantSize() { return m_mapAttributeEnchant.size(); }
	void	ChangeAttribute( CUser* pUser, DWORD dwTargetItem, DWORD dwUseItem , SAI79::ePropType nAttribute );
#endif // __EXT_ENCHANT
#ifdef __SYS_ITEMTRANSY
	void	OnItemTransy( CUser* pUser, OBJID objidTarget, OBJID objidTransy, DWORD dwChangeId, BOOL bCash );
private:
	BOOL	RunItemTransy( CUser* pUser, CItemElem* pItemElemTarget, DWORD dwChangeId );
	int		m_nItemTransyLowLevel, m_nItemTransyHighLevel;
#endif // __SYS_ITEMTRANSY

private:
	map<int, int>	m_mapSuitProb;
	map<int, int>	m_mapWeaponProb;
	
#if __VER >= 14 // __SMELT_SAFETY
private:
	void	RefineAccessory( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	void	RefineCollector( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	BYTE	SmeltSafetyNormal( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr );
	BYTE	SmeltSafetyGeneral( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr );
	BYTE	SmeltSafetyAccessory( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr );
	BYTE	SmeltSafetyPiercingSize( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr );
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	BYTE	SmeltSafetyAttribute(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr );
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
public:
#endif // __SMELT_SAFETY

#if __VER >= 13 // __EXT_ENCHANT
	void	EnchantGeneral( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	int		GetGeneralEnchantProb( int nAbilityOption );
	void	EnchantAttribute( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	int		GetAttributeEnchantProb( int nAbilityOption );
	
	DWORD	WhatEleCard( DWORD dwItemType );	// °ð »èÁ¦

	map<int, int>	m_mapGeneralEnchant;
	map<int, __ATTRIBUTE_ENCHANT> m_mapAttributeEnchant;
#endif // __EXT_ENCHANT
#ifdef __NEW_ITEM_VARUNA
	void	LoadScriptBaruna();
	int		GetMaxBarunaEnchantSize() { return m_mapBarunaEnchantUpgrade.size(); }

	void	CreateBarunaCrystal( CUser* pUser, CItemElem* pCrystal, CItemElem* pPotion );
	void	BarunaChaosGemExtract( CUser* pUser, CItemElem* pEquip );
	void	CreateBarunaChaosGem( CUser* pUser, CItemElem* pChaosDreg, CItemElem* pPotion );
	void	CreateStone( CUser* pUser, CItemElem* pCrystal, CItemElem* pChaosGem, CItemElem* pPowder );
	void	EnchantBaruna( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pBarunaPet, CItemElem* pProtectScroll = NULL );

	void	IncreaseBarunaPetFailCounter( CItemElem* pBarunaPet, int nFail = 1 );
	void	IncreaseBarunaPetLevel( CItemElem* pBarunaPet, int nLevel = 1 );

	int GetBarunaEnchantProp( int nAbilitiyOption );
	int GetBarunaEnchantDowngradeProp( int nAbilitiyOption );
	float GetBarunaPetMultiplicator( int nLevel );
	int GetBarunaFailForNextLevel( int nLevel );

	map<int, __CHAOS_GEM> m_mapChaosGemTable;

	map<int, int>	m_mapPetMultiplicator;
	map<int, int>	m_mapPetFailForLevel;
	map<int, int>	m_mapBarunaEnchantUpgrade;
	map<int, int>	m_mapBarunaEnchantDowngrade;

#ifdef __BARUNA_PIERCING
	void	CreateBarunaPiercing( CUser* pUser, CItemElem* pItemMain, CItemElem* pRune, CItemElem* pScroll, CItemElem* pCrystal );
	int	GetMaxBarunaPiercingSize() { return m_mapBarunaEnchantPiercing.size(); }
	int	GetBarunaEnchantPiercingProp( int nSize );
	map<int, int>	m_mapBarunaEnchantPiercing;
#endif // __BARUNA_PIERCING

#ifdef __BARUNA_V18
	map<int, int> m_nBarunaUp;	//Uprate
	map<int, int> m_nBarunaDown;	//Downrate
	map<int, int> m_nBarunaElementUp;
	map<int, int> m_nBarunaElementDown;
#endif//__BARUNA_V18

#endif // __NEW_ITEM_VARUNA
#if __VER >= 15 // __PETVIS
public:
	void	PetVisSize( CUser* pUser, OBJID objIdMaterial );
	void	SetPetVisItem( CUser* pUser, OBJID objIdVis );
	void	RemovePetVisItem( CUser* pUser, int nPosition, BOOL bExpired = FALSE );
	void	PutPetVisItemLog( CUser* pUser, const char* szAction, const char* szContext, CItemElem* pItem, int nPosition );
	void	SwapVis( CUser* pUser, int nPos1, int nPos2 );
	void	ChangeVisPetSfx( CUser* pUser, CItemElem* pItemElemPet );
	void	TransFormVisPet( CUser* pUser, OBJID objIdMaterial );
#endif // __PETVIS



};

#endif // __EXT_PIERCING