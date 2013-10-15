--------------------------------------------------------------------
-- 초기화 ----------------------------------------------------------
--------------------------------------------------------------------
dofile( ".\\LuaFunc\\ItemUpgradeBaseFunc.lua" )
--------------------------------------------------------------------


--------------------------------------------------------------------
-- 일반 제련 -------------------------------------------------------
--------------------------------------------------------------------

AddGeneralEnchant( "IK1_WEAPON", "IK2_NONE", "IK3_NONE" )
--{
	SetGeneralMaterial( "IK1_PASSIVE", "IK2_MATERIAL", "IK3_OPERCID" )

--					제련값,	성공 확률%,	실패/파괴확률%,	마이너스 1 제련값 확률%( 10000 - 성공 확률 - 실패 확률 ) (%값은 n/10000)
	SetGeneralEnchantProb( 		1, 	10000,		0	)	-- auto --
	SetGeneralEnchantProb( 		2, 	10000,		0	)	-- auto --
	SetGeneralEnchantProb( 		3, 	9000,		750	)	-- auto --
	SetGeneralEnchantProb( 		4, 	8500,		1125	)	-- auto --
	SetGeneralEnchantProb( 		5, 	8000,		1500	)	-- auto --
	SetGeneralEnchantProb( 		6, 	7000,		2250	)	-- auto --
	SetGeneralEnchantProb( 		7, 	6000,		3000	)	-- auto --
	SetGeneralEnchantProb( 		8, 	5000,		3750	)	-- auto --
	SetGeneralEnchantProb( 		9, 	4000,		4500	)	-- auto --
	SetGeneralEnchantProb( 		10, 	3000,		5250	)	-- auto --
	SetGeneralEnchantProb(		11, 	2000,		6000	)	-- auto --
	SetGeneralEnchantProb(		12,	1000,		6750	)	-- auto --
	SetGeneralEnchantProb( 		13,	750,		6937	)	-- auto --
	SetGeneralEnchantProb( 		14,	500,		7125	)	-- auto --
	SetGeneralEnchantProb( 		15,	350,		7237	)	-- auto --
	SetGeneralEnchantProb( 		16, 	200,		7350	)	-- auto --
	SetGeneralEnchantProb( 		17, 	150,		7387	)	-- auto --
	SetGeneralEnchantProb( 		18, 	100,		7425	)	-- auto --
	SetGeneralEnchantProb( 		19, 	75,		7444	)	-- auto --
	SetGeneralEnchantProb( 		20, 	50,		7462	)	-- auto --
--}

AddGeneralEnchant( "IK1_ARMOR", "IK2_ARMOR", "IK3_NONE" )
--{
	SetGeneralMaterial( "IK1_PASSIVE", "IK2_MATERIAL", "IK3_OPERCID"  )

--					제련값,	성공 확률%,	실패/파괴확률%,	마이너스1 제련값 확률%( 10000 - 성공 확률 - 실패 확률 )	(%값은 n/10000)
	SetGeneralEnchantProb( 		1, 	10000,		0	)	-- auto --
	SetGeneralEnchantProb( 		2, 	10000,		0	)	-- auto --
	SetGeneralEnchantProb( 		3, 	9500,		375	)	-- auto --
	SetGeneralEnchantProb( 		4, 	9000,		750	)	-- auto --
	SetGeneralEnchantProb( 		5, 	8500,		1125	)	-- auto --
	SetGeneralEnchantProb( 		6, 	7750,		1687	)	-- auto --
	SetGeneralEnchantProb( 		7, 	7000,		2250	)	-- auto --
	SetGeneralEnchantProb( 		8, 	6250,		2812	)	-- auto --
	SetGeneralEnchantProb( 		9, 	5500,		3375	)	-- auto --
	SetGeneralEnchantProb( 		10, 	4750,		3937	)	-- auto --
	SetGeneralEnchantProb( 		11, 	4000,		4500	)	-- auto --
	SetGeneralEnchantProb( 		12,	3250,		5062	)	-- auto --
	SetGeneralEnchantProb( 		13,	2250,		5812	)	-- auto --
	SetGeneralEnchantProb( 		14,	1250,		6562	)	-- auto --
	SetGeneralEnchantProb( 		15,	1000,		6750	)	-- auto --
	SetGeneralEnchantProb( 		16, 	750,		6937	)	-- auto --
	SetGeneralEnchantProb( 		17, 	500,		7125	)	-- auto --
	SetGeneralEnchantProb( 		18, 	300,		7275	)	-- auto --
	SetGeneralEnchantProb( 		19, 	150,		7387	)	-- auto --
	SetGeneralEnchantProb( 		20, 	100,		7425	)	-- auto --
--}

AddGeneralEnchant( "IK1_ARMOR", "IK2_ARMORETC", "IK3_NONE" )
--{
	SetGeneralMaterial( "IK1_PASSIVE", "IK2_MATERIAL", "IK3_OPERCID"  )

--					제련값,	성공 확률%,	실패/파괴확률%,	마이너스1 제련값 확률%( 10000 - 성공 확률 - 실패 확률 )	(%값은 n/10000)
	SetGeneralEnchantProb( 		1, 	10000,		0	)	-- auto --
	SetGeneralEnchantProb( 		2, 	10000,		0	)	-- auto --
	SetGeneralEnchantProb( 		3, 	9500,		375	)	-- auto --
	SetGeneralEnchantProb( 		4, 	9000,		750	)	-- auto --
	SetGeneralEnchantProb( 		5, 	8500,		1125	)	-- auto --
	SetGeneralEnchantProb( 		6, 	7750,		1687	)	-- auto --
	SetGeneralEnchantProb( 		7, 	7000,		2250	)	-- auto --
	SetGeneralEnchantProb( 		8, 	6250,		2812	)	-- auto --
	SetGeneralEnchantProb( 		9, 	5500,		3375	)	-- auto --
	SetGeneralEnchantProb( 		10, 	4750,		3937	)	-- auto --
	SetGeneralEnchantProb( 		11, 	4000,		4500	)	-- auto --
	SetGeneralEnchantProb( 		12,	3250,		5062	)	-- auto --
	SetGeneralEnchantProb( 		13,	2250,		5812	)	-- auto --
	SetGeneralEnchantProb( 		14,	1250,		6562	)	-- auto --
	SetGeneralEnchantProb( 		15,	1000,		6750	)	-- auto --
	SetGeneralEnchantProb( 		16, 	750,		6937	)	-- auto --
	SetGeneralEnchantProb( 		17, 	500,		7125	)	-- auto --
	SetGeneralEnchantProb( 		18, 	300,		7275	)	-- auto --
	SetGeneralEnchantProb( 		19, 	150,		7387	)	-- auto --
	SetGeneralEnchantProb( 		20, 	100,		7425	)	-- auto --
--}
--------------------------------------------------------------------
-- 속성 제련 -------------------------------------------------------
--------------------------------------------------------------------

AddAttributeEnchant( "IK1_WEAPON", "IK2_NONE", "IK3_NONE" )
--{
	SetAttributeMaterial( "IK1_PASSIVE", "IK2_BARUNA", "IK3_ELEORB" )

	SetAttributeRemovePenya( 100000 )

--					제련값,	성공 확률%,	실패 확률%,	데미지증가율,	방어력증가율,	속성보정증가율  (제련값을 제외한 모든값은 n/10000)
	SetAttributeEnchantProb( 	1, 	10000,		0,		200,		200,		500	)
	SetAttributeEnchantProb( 	2, 	10000,		0,		221,		221,		522	)
	SetAttributeEnchantProb( 	3, 	9000,		750,		256,		256,		560	)
	SetAttributeEnchantProb( 	4, 	8500,		1125,		305,		305,		612	)
	SetAttributeEnchantProb( 	5, 	8000,		1500,		368,		368,		680	)
	SetAttributeEnchantProb( 	6, 	7000,		2250,		446,		446,		763	)
	SetAttributeEnchantProb( 	7, 	6000,		3000,		537,		537,		860	)
	SetAttributeEnchantProb( 	8, 	5000,		3750,		642,		642,		973	)
	SetAttributeEnchantProb( 	9, 	4000,		4500,		761,		761,		1101	)
	SetAttributeEnchantProb( 	10, 	3000,		5250,		895,		895,		1244	)
	SetAttributeEnchantProb(	11, 	2000,		6000,		1042,		1042,		1402	)
	SetAttributeEnchantProb(	12,	1000,		6750,		1203,		1203,		1575	)
	SetAttributeEnchantProb( 	13,	750,		6937,		1379,		1379,		1763	)
	SetAttributeEnchantProb( 	14,	500,		7125,		1568,		1568,		1966	)
	SetAttributeEnchantProb( 	15,	350,		7237,		1772,		1772,		2184	)
	SetAttributeEnchantProb( 	16, 	200,		7350,		1989,		1989,		2417	)
	SetAttributeEnchantProb( 	17, 	150,		7387,		2221,		2221,		2665	)
	SetAttributeEnchantProb( 	18, 	100,		7425,		2467,		2467,		2998	)
	SetAttributeEnchantProb( 	19, 	75,		7444,		2726,		2726,		3207	)
	SetAttributeEnchantProb( 	20, 	50,		7462,		3000,		3000,		3500	)
--}

AddAttributeEnchant( "IK1_ARMOR", "IK2_ARMOR", "IK3_SUIT" )
--{
	SetAttributeMaterial( "IK1_PASSIVE", "IK2_BARUNA", "IK3_ELEORB" )

	SetAttributeRemovePenya( 100000 )

--					제련값,	성공 확률%,	실패 확률%,	데미지증가율,	방어력증가율,	속성보정증가율  (제련값을 제외한 모든값은 n/10000)
	SetAttributeEnchantProb( 	1, 	10000,		0,		200,		200,		500	)
	SetAttributeEnchantProb( 	2, 	10000,		0,		221,		221,		522	)
	SetAttributeEnchantProb( 	3, 	9500,		375,		256,		256,		560	)
	SetAttributeEnchantProb( 	4, 	9000,		750,		305,		305,		612	)
	SetAttributeEnchantProb( 	5, 	8500,		1125,		368,		368,		680	)
	SetAttributeEnchantProb( 	6, 	7750,		1687,		446,		446,		763	)
	SetAttributeEnchantProb( 	7, 	7000,		2250,		537,		537,		860	)
	SetAttributeEnchantProb( 	8, 	6250,		2812,		642,		642,		973	)
	SetAttributeEnchantProb( 	9, 	5500,		3375,		761,		761,		1101	)
	SetAttributeEnchantProb( 	10, 	4750,		3937,		895,		895,		1244	)
	SetAttributeEnchantProb( 	11, 	4000,		4500,		1042,		1042,		1402	)
	SetAttributeEnchantProb( 	12,	3250,		5062,		1203,		1203,		1575	)
	SetAttributeEnchantProb( 	13,	2250,		5812,		1379,		1379,		1763	)
	SetAttributeEnchantProb( 	14,	1250,		6562,		1568,		1568,		1966	)
	SetAttributeEnchantProb( 	15,	1000,		6750,		1772,		1772,		2184	)
	SetAttributeEnchantProb( 	16, 	750,		6937,		1989,		1989,		2417	)
	SetAttributeEnchantProb( 	17, 	500,		7125,		2221,		2221,		2665	)
	SetAttributeEnchantProb( 	18, 	300,		7275,		2467,		2467,		2998	)
	SetAttributeEnchantProb( 	19, 	150,		7387,		2726,		2726,		3207	)
	SetAttributeEnchantProb( 	20, 	100,		7425,		3000,		3000,		3500	)
--}


--------------------------------------------------------------------
-- 피어싱 ----------------------------------------------------------
--------------------------------------------------------------------
		
AddPiercing( "IK1_WEAPON", "IK2_NONE", "IK3_NONE" )		-- 피어싱 할 아이템의 MainCategory, SubCategory
--{
	SetPiercingMaterial( "IK1_NONE", "IK2_NONE", "IK3_NONE" )	-- 피어싱 사이즈를 늘릴때 사용할 아이템의 MainCategory, SubCategory

	SetPiercedItem( "IK1_ACTIVE", "IK2_ONCE", "IK3_BARUNAWEA_PIERCE_RUNE" )	-- 소켓에 박을 아이템의 MainCategory, SubCategory

	SetPiercingSizeIncreasePenya( 0 )			-- 피어싱 사이즈를 늘릴때 소비되는 페냐

	SetPiercedItemRemovePenya( 0 )			-- 피어싱된 아이템을 제거할때 소비되는 페냐

	SetPiercingSizeProb( 1, 0 )				-- 사이즈, 성공 확률( 1/10000 )
--}

AddPiercing( "IK1_ARMOR", "IK2_ARMOR", "IK3_SHIELD" )	-- 피어싱 할 아이템의 MainCategory, SubCategory
--{
	SetPiercingMaterial( "IK1_NONE", "IK2_NONE", "IK3_NONE" )	-- 피어싱 사이즈를 늘릴때 사용할 아이템의 MainCategory, SubCategory

	SetPiercedItem( "IK1_ACTIVE", "IK2_ONCE", "IK3_BARUNAWEA_PIERCE_RUNE" )	-- 소켓에 박을 아이템의 MainCategory, SubCategory

	SetPiercingSizeIncreasePenya( 0 )			-- 피어싱 사이즈를 늘릴때 소비되는 페냐

	SetPiercedItemRemovePenya( 0 )			-- 피어싱된 아이템을 제거할때 소비되는 페냐

	SetPiercingSizeProb( 1, 0 )				-- 사이즈, 성공 확률( 1/10000 )
--}
AddPiercing( "IK1_ARMOR", "IK2_ARMOR", "IK3_ZEMBARUNA" )	-- 피어싱 할 아이템의 MainCategory, SubCategory
--{
	SetPiercingMaterial( "IK1_NONE", "IK2_NONE", "IK3_NONE" )	-- 피어싱 사이즈를 늘릴때 사용할 아이템의 MainCategory, SubCategory

	SetPiercedItem( "IK1_ACTIVE", "IK2_ONCE", "IK3_BARUNAWEA_PIERCE_RUNE" )	-- 소켓에 박을 아이템의 MainCategory, SubCategory

	SetPiercingSizeIncreasePenya( 0 )			-- 피어싱 사이즈를 늘릴때 소비되는 페냐

	SetPiercedItemRemovePenya( 0 )			-- 피어싱된 아이템을 제거할때 소비되는 페냐

	SetPiercingSizeProb( 1, 0 )				-- 사이즈, 성공 확률( 1/10000 )
--}
AddPiercing( "IK1_ARMOR", "IK2_ARMOR", "IK3_MAGICBARUNA" )	-- 피어싱 할 아이템의 MainCategory, SubCategory
--{
	SetPiercingMaterial( "IK1_NONE", "IK2_NONE", "IK3_NONE" )	-- 피어싱 사이즈를 늘릴때 사용할 아이템의 MainCategory, SubCategory

	SetPiercedItem( "IK1_ACTIVE", "IK2_ONCE", "IK3_BARUNAWEA_PIERCE_RUNE" )	-- 소켓에 박을 아이템의 MainCategory, SubCategory

	SetPiercingSizeIncreasePenya( 0 )			-- 피어싱 사이즈를 늘릴때 소비되는 페냐

	SetPiercedItemRemovePenya( 0 )			-- 피어싱된 아이템을 제거할때 소비되는 페냐

	SetPiercingSizeProb( 1, 0 )				-- 사이즈, 성공 확률( 1/10000 )
--}

AddPiercing( "IK1_ARMOR", "IK2_ARMOR", "IK3_SUIT" )	-- 피어싱 할 아이템의 MainCategory, SubCategory
--{
	SetPiercingMaterial( "IK1_NONE", "IK2_NONE", "IK3_NONE" )	-- 피어싱 사이즈를 늘릴때 사용할 아이템의 MainCategory, SubCategory

	SetPiercedItem( "IK1_ACTIVE", "IK2_ONCE", "IK3_BARUNAWEA_PIERCE_RUNE" )	-- 소켓에 박을 아이템의 MainCategory, SubCategory

	SetPiercingSizeIncreasePenya( 0 )			-- 피어싱 사이즈를 늘릴때 소비되는 페냐

	SetPiercedItemRemovePenya( 0 )			-- 피어싱된 아이템을 제거할때 소비되는 페냐

	SetPiercingSizeProb( 1, 0 )				-- 사이즈, 성공 확률( 1/10000 )
--}


--------------------------------------------------------------------
-- 각성 ------------------------------------------------------------
--------------------------------------------------------------------

AddRandomOptionExtension(  "IK1_WEAPON", "IK2_NONE", "IK3_NONE"  )
--{
	SetRandomOptionMaterial( "IK1_NONE", "IK2_NONE", "IK3_NONE"  )

	SetRandomOptionGeneratePenya( 100000 )

	SetRandomOptionSizeProb( 1, 10000 )
	SetRandomOptionSizeProb( 2, 7000 )
	SetRandomOptionSizeProb( 3, 3500 )

	SetDummyRandomOptionSize( 20 )

	SetDstProb( "DST_STR", 700, 440, 700 )
	SetDstProb( "DST_DEX", 1400 , 880, 1400)
	SetDstProb( "DST_INT", 2100 , 1320, 2100)
	SetDstProb( "DST_STA", 2800 , 1760, 2800)
	SetDstProb( "DST_CHR_CHANCECRITICAL", 3600 , 2200, 3600)
	SetDstProb( "DST_CRITICAL_BONUS", 4400 ,2640 , 4400)
	SetDstProb( "DST_SPEED", 5100 , 4040 , 5100)
	SetDstProb( "DST_ATTACKSPEED", 5800 , 4480, 5800)
	SetDstProb( "DST_ADJDEF", 6500 , 5880, 6500)
	SetDstProb( "DST_ATKPOWER", 7200 , 6320, 7200)
	SetDstProb( "DST_HP_MAX", 7900 , 7720, 7900)
	SetDstProb( "DST_MP_MAX", 8600 , 8160, 8600)
	SetDstProb( "DST_FP_MAX", 9300 , 9560, 9300)
	SetDstProb( "DST_SPELL_RATE", 10000 , 10000 , 10000)
--}

AddRandomOptionExtension( "IK1_ARMOR", "IK2_ARMOR", "IK3_SHIELD" )
--{
	SetRandomOptionMaterial( "IK1_NONE", "IK2_NONE", "IK3_NONE" )

	SetRandomOptionGeneratePenya( 100000 )

	SetRandomOptionSizeProb( 1, 10000 )
	SetRandomOptionSizeProb( 2, 7000 )
	SetRandomOptionSizeProb( 3, 3500 )

	SetDummyRandomOptionSize( 20 )

	SetDstProb( "DST_STR", 700, 440, 700 )
	SetDstProb( "DST_DEX", 1400 , 880, 1400)
	SetDstProb( "DST_INT", 2100 , 1320, 2100)
	SetDstProb( "DST_STA", 2800 , 1760, 2800)
	SetDstProb( "DST_CHR_CHANCECRITICAL", 3600 , 2200, 3600)
	SetDstProb( "DST_CRITICAL_BONUS", 4400 ,2640 , 4400)
	SetDstProb( "DST_SPEED", 5100 , 4040 , 5100)
	SetDstProb( "DST_ATTACKSPEED", 5800 , 4480, 5800)
	SetDstProb( "DST_ADJDEF", 6500 , 5880, 6500)
	SetDstProb( "DST_ATKPOWER", 7200 , 6320, 7200)
	SetDstProb( "DST_HP_MAX", 7900 , 7720, 7900)
	SetDstProb( "DST_MP_MAX", 8600 , 8160, 8600)
	SetDstProb( "DST_FP_MAX", 9300 , 9560, 9300)
	SetDstProb( "DST_SPELL_RATE", 10000 , 10000 , 10000)
--}
AddRandomOptionExtension( "IK1_ARMOR", "IK2_ARMOR", "IK3_ZEMBARUNA" )
--{
	SetRandomOptionMaterial( "IK1_NONE", "IK2_NONE", "IK3_NONE" )

	SetRandomOptionGeneratePenya( 100000 )

	SetRandomOptionSizeProb( 1, 10000 )
	SetRandomOptionSizeProb( 2, 7000 )
	SetRandomOptionSizeProb( 3, 3500 )

	SetDummyRandomOptionSize( 20 )

	SetDstProb( "DST_STR", 700, 440, 700 )
	SetDstProb( "DST_DEX", 1400 , 880, 1400)
	SetDstProb( "DST_INT", 2100 , 1320, 2100)
	SetDstProb( "DST_STA", 2800 , 1760, 2800)
	SetDstProb( "DST_CHR_CHANCECRITICAL", 3600 , 2200, 3600)
	SetDstProb( "DST_CRITICAL_BONUS", 4400 ,2640 , 4400)
	SetDstProb( "DST_SPEED", 5100 , 4040 , 5100)
	SetDstProb( "DST_ATTACKSPEED", 5800 , 4480, 5800)
	SetDstProb( "DST_ADJDEF", 6500 , 5880, 6500)
	SetDstProb( "DST_ATKPOWER", 7200 , 6320, 7200)
	SetDstProb( "DST_HP_MAX", 7900 , 7720, 7900)
	SetDstProb( "DST_MP_MAX", 8600 , 8160, 8600)
	SetDstProb( "DST_FP_MAX", 9300 , 9560, 9300)
	SetDstProb( "DST_SPELL_RATE", 10000 , 10000 , 10000)
--}
AddRandomOptionExtension( "IK1_ARMOR", "IK2_ARMOR", "IK3_MAGICBARUNA" )
--{
	SetRandomOptionMaterial( "IK1_NONE", "IK2_NONE", "IK3_NONE" )

	SetRandomOptionGeneratePenya( 100000 )

	SetRandomOptionSizeProb( 1, 10000 )
	SetRandomOptionSizeProb( 2, 7000 )
	SetRandomOptionSizeProb( 3, 3500 )

	SetDummyRandomOptionSize( 20 )

	SetDstProb( "DST_STR", 700, 440, 700 )
	SetDstProb( "DST_DEX", 1400 , 880, 1400)
	SetDstProb( "DST_INT", 2100 , 1320, 2100)
	SetDstProb( "DST_STA", 2800 , 1760, 2800)
	SetDstProb( "DST_CHR_CHANCECRITICAL", 3600 , 2200, 3600)
	SetDstProb( "DST_CRITICAL_BONUS", 4400 ,2640 , 4400)
	SetDstProb( "DST_SPEED", 5100 , 4040 , 5100)
	SetDstProb( "DST_ATTACKSPEED", 5800 , 4480, 5800)
	SetDstProb( "DST_ADJDEF", 6500 , 5880, 6500)
	SetDstProb( "DST_ATKPOWER", 7200 , 6320, 7200)
	SetDstProb( "DST_HP_MAX", 7900 , 7720, 7900)
	SetDstProb( "DST_MP_MAX", 8600 , 8160, 8600)
	SetDstProb( "DST_FP_MAX", 9300 , 9560, 9300)
	SetDstProb( "DST_SPELL_RATE", 10000 , 10000 , 10000)
--}
AddRandomOptionExtension( "IK1_ARMOR", "IK2_ARMORETC", "IK3_HELMET" )
--{
	SetRandomOptionMaterial( "IK1_NONE", "IK2_NONE", "IK3_NONE"  )

	SetRandomOptionGeneratePenya( 100000 )

	SetRandomOptionSizeProb( 1, 10000 )
	SetRandomOptionSizeProb( 2, 7000 )
	SetRandomOptionSizeProb( 3, 3500 )

	SetDummyRandomOptionSize( 20 )

	SetDstProb( "DST_STR", 1666,1333,1666 )
	SetDstProb( "DST_DEX", 3332,2666,3332 )
	SetDstProb( "DST_INT", 4998,3999,4998)
	SetDstProb( "DST_STA", 6664,5332,6664 )
	SetDstProb( "DST_MP_MAX", 8330,6665,8330 )
	SetDstProb( "DST_FP_MAX", 10000,10000,10000 )
--}

AddRandomOptionExtension( "IK1_ARMOR", "IK2_ARMOR", "IK3_SUIT" )
--{
	SetRandomOptionMaterial( "IK1_NONE", "IK2_NONE", "IK3_NONE"  )

	SetRandomOptionGeneratePenya( 100000 )

	SetRandomOptionSizeProb( 1, 10000 )
	SetRandomOptionSizeProb( 2, 7000 )
	SetRandomOptionSizeProb( 3, 3500 )

	SetDummyRandomOptionSize( 20 )

	SetDstProb( "DST_STR", 700, 440, 700 )
	SetDstProb( "DST_DEX", 1400 , 880, 1400)
	SetDstProb( "DST_INT", 2100 , 1320, 2100)
	SetDstProb( "DST_STA", 2800 , 1760, 2800)
	SetDstProb( "DST_CHR_CHANCECRITICAL", 3600 , 2200, 3600)
	SetDstProb( "DST_CRITICAL_BONUS", 4400 ,2640 , 4400)
	SetDstProb( "DST_SPEED", 5100 , 4040 , 5100)
	SetDstProb( "DST_ATTACKSPEED", 5800 , 4480, 5800)
	SetDstProb( "DST_ADJDEF", 6500 , 5880, 6500)
	SetDstProb( "DST_ATKPOWER", 7200 , 6320, 7200)
	SetDstProb( "DST_HP_MAX", 7900 , 7720, 7900)
	SetDstProb( "DST_MP_MAX", 8600 , 8160, 8600)
	SetDstProb( "DST_FP_MAX", 9300 , 9560, 9300)
	SetDstProb( "DST_SPELL_RATE", 10000 , 10000 , 10000)
--}

AddRandomOptionExtension( "IK1_ARMOR", "IK2_ARMORETC", "IK3_GAUNTLET" )
--{
	SetRandomOptionMaterial( "IK1_NONE", "IK2_NONE", "IK3_NONE"  )

	SetRandomOptionGeneratePenya( 100000 )

	SetRandomOptionSizeProb( 1, 10000 )
	SetRandomOptionSizeProb( 2, 7000 )
	SetRandomOptionSizeProb( 3, 3500 )

	SetDummyRandomOptionSize( 20 )

	SetDstProb( "DST_CHR_CHANCECRITICAL", 1700, 1333, 1700 )
	SetDstProb( "DST_ATTACKSPEED", 3360, 2666, 3360 )
	SetDstProb( "DST_ATKPOWER", 5020 ,3999, 5020 )
	SetDstProb( "DST_MP_MAX", 6680 , 5332, 6680)
	SetDstProb( "DST_FP_MAX", 8340 , 8832, 8340)
	SetDstProb( "DST_SPELL_RATE", 10000 , 10000, 10000)
--}

AddRandomOptionExtension( "IK1_ARMOR", "IK2_ARMORETC", "IK3_BOOTS" )
--{
	SetRandomOptionMaterial( "IK1_NONE", "IK2_NONE", "IK3_NONE"  )

	SetRandomOptionGeneratePenya( 100000 )

	SetRandomOptionSizeProb( 1, 10000 )
	SetRandomOptionSizeProb( 2, 7000 )
	SetRandomOptionSizeProb( 3, 3500 )

	SetDummyRandomOptionSize( 20 )

	SetDstProb( "DST_CRITICAL_BONUS", 1700,833,1700 )
	SetDstProb( "DST_SPEED", 3360,1666,3360 )
	SetDstProb( "DST_ADJDEF", 5020,4166,5020 )
	SetDstProb( "DST_HP_MAX", 6680,6666,6680 )
	SetDstProb( "DST_MP_MAX", 8340,7499,8340 )
	SetDstProb( "DST_FP_MAX", 10000,10000,10000 )
--}


--------------------------------------------------------------------
-- 각 파라미터들의 적용 수치와 확률 --------------------------------
--------------------------------------------------------------------

-- 확률 1/10억
AddDstParameter( "DST_STR" )
--{
	SetAdjValueProb( -28, 100000	,	150000	,	50000	)
	SetAdjValueProb( -25, 3038270	,	4557405	,	1519135	)
	SetAdjValueProb( -22, 9524381	,	14286571	,	4762190	)
	SetAdjValueProb( -19, 20572000	,	30857999	,	10285999	)
	SetAdjValueProb( -16, 37701630	,	56552444	,	18850814	)
	SetAdjValueProb( -13, 63346074	,	95019110	,	31673036	)
	SetAdjValueProb( -10, 101762741	,	152644110	,	50881369	)
	SetAdjValueProb( -7, 161466445	,	242199666	,	80733221	)
	SetAdjValueProb( -4, 263744223	,	395616333	,	131872110	)
	SetAdjValueProb( -1, 499999999	,	749999997	,	249999998	)
	SetAdjValueProb( 1, 736255775	,	868127889	,	604383664	)
	SetAdjValueProb( 4, 838533553	,	919266778	,	757800331	)
	SetAdjValueProb( 7, 898237257	,	949118630	,	847355887	)
	SetAdjValueProb( 10, 936653924	,	968326963	,	904980887	)
	SetAdjValueProb( 13, 962298368	,	981149185	,	943447553	)
	SetAdjValueProb( 16, 979427998	,	989714000	,	969141998	)
	SetAdjValueProb( 19, 990475617	,	995237809	,	985713426	)
	SetAdjValueProb( 22, 996961728	,	998480864	,	995442592	)
	SetAdjValueProb( 25, 999900000	,	999950000	,	999850000	)
	SetAdjValueProb( 28, 1000000000	,	1000000000	,	1000000000	)

--}

AddDstParameter( "DST_DEX" )
--{
	SetAdjValueProb( -28, 100000 	,	150000	,	50000	)
	SetAdjValueProb( -25, 3038270 	,	4557405	,	1519135	)
	SetAdjValueProb( -22, 9524381 	,	14286571	,	4762190	)
	SetAdjValueProb( -19, 20572000 	,	30857999	,	10285999	)
	SetAdjValueProb( -16, 37701630 	,	56552444	,	18850814	)
	SetAdjValueProb( -13, 63346074 	,	95019110	,	31673036	)
	SetAdjValueProb( -10, 101762741 	,	152644110	,	50881369	)
	SetAdjValueProb( -7, 161466445 	,	242199666	,	80733221	)
	SetAdjValueProb( -4, 263744223 	,	395616333	,	131872110	)
	SetAdjValueProb( -1, 499999999 	,	749999997	,	249999998	)
	SetAdjValueProb( 1, 736255775 	,	868127889	,	604383664	)
	SetAdjValueProb( 4, 838533553 	,	919266778	,	757800331	)
	SetAdjValueProb( 7, 898237257 	,	949118630	,	847355887	)
	SetAdjValueProb( 10, 936653924 	,	968326963	,	904980887	)
	SetAdjValueProb( 13, 962298368 	,	981149185	,	943447553	)
	SetAdjValueProb( 16, 979427998 	,	989714000	,	969141998	)
	SetAdjValueProb( 19, 990475617 	,	995237809	,	985713426	)
	SetAdjValueProb( 22, 996961728 	,	998480864	,	995442592	)
	SetAdjValueProb( 25, 999900000 	,	999950000	,	999850000	)
	SetAdjValueProb( 28, 1000000000 	,	1000000000	,	1000000000	)

--}

AddDstParameter( "DST_INT" )
--{
	SetAdjValueProb( -28, 100000 	,	150000	,	50000	)
	SetAdjValueProb( -25, 3038270 	,	4557405	,	1519135	)
	SetAdjValueProb( -22, 9524381 	,	14286571	,	4762190	)
	SetAdjValueProb( -19, 20572000 	,	30857999	,	10285999	)
	SetAdjValueProb( -16, 37701630 	,	56552444	,	18850814	)
	SetAdjValueProb( -13, 63346074 	,	95019110	,	31673036	)
	SetAdjValueProb( -10, 101762741 	,	152644110	,	50881369	)
	SetAdjValueProb( -7, 161466445 	,	242199666	,	80733221	)
	SetAdjValueProb( -4, 263744223 	,	395616333	,	131872110	)
	SetAdjValueProb( -1, 499999999 	,	749999997	,	249999998	)
	SetAdjValueProb( 1, 736255775 	,	868127889	,	604383664	)
	SetAdjValueProb( 4, 838533553 	,	919266778	,	757800331	)
	SetAdjValueProb( 7, 898237257 	,	949118630	,	847355887	)
	SetAdjValueProb( 10, 936653924 	,	968326963	,	904980887	)
	SetAdjValueProb( 13, 962298368 	,	981149185	,	943447553	)
	SetAdjValueProb( 16, 979427998 	,	989714000	,	969141998	)
	SetAdjValueProb( 19, 990475617 	,	995237809	,	985713426	)
	SetAdjValueProb( 22, 996961728 	,	998480864	,	995442592	)
	SetAdjValueProb( 25, 999900000 	,	999950000	,	999850000	)
	SetAdjValueProb( 28, 1000000000 	,	1000000000	,	1000000000	)
--}

AddDstParameter( "DST_STA" )
--{
	SetAdjValueProb( -28, 100000 	,	150000	,	50000	)
	SetAdjValueProb( -25, 3038270 	,	4557405	,	1519135	)
	SetAdjValueProb( -22, 9524381 	,	14286571	,	4762190	)
	SetAdjValueProb( -19, 20572000 	,	30857999	,	10285999	)
	SetAdjValueProb( -16, 37701630 	,	56552444	,	18850814	)
	SetAdjValueProb( -13, 63346074 	,	95019110	,	31673036	)
	SetAdjValueProb( -10, 101762741 	,	152644110	,	50881369	)
	SetAdjValueProb( -7, 161466445 	,	242199666	,	80733221	)
	SetAdjValueProb( -4, 263744223 	,	395616333	,	131872110	)
	SetAdjValueProb( -1, 499999999 	,	749999997	,	249999998	)
	SetAdjValueProb( 1, 736255775 	,	868127889	,	604383664	)
	SetAdjValueProb( 4, 838533553 	,	919266778	,	757800331	)
	SetAdjValueProb( 7, 898237257 	,	949118630	,	847355887	)
	SetAdjValueProb( 10, 936653924 	,	968326963	,	904980887	)
	SetAdjValueProb( 13, 962298368 	,	981149185	,	943447553	)
	SetAdjValueProb( 16, 979427998 	,	989714000	,	969141998	)
	SetAdjValueProb( 19, 990475617 	,	995237809	,	985713426	)
	SetAdjValueProb( 22, 996961728 	,	998480864	,	995442592	)
	SetAdjValueProb( 25, 999900000 	,	999950000	,	999850000	)
	SetAdjValueProb( 28, 1000000000 	,	1000000000	,	1000000000	)
--}

AddDstParameter( "DST_CHR_CHANCECRITICAL" )
--{
	SetAdjValueProb( -10, 100000 	,	150000	,	50000	)
	SetAdjValueProb( -9, 3038270 	,	4557405	,	1519135	)
	SetAdjValueProb( -8, 9524381 	,	14286571	,	4762190	)
	SetAdjValueProb( -7, 20572000 	,	30857999	,	10285999	)
	SetAdjValueProb( -6, 37701630 	,	56552444	,	18850814	)
	SetAdjValueProb( -5, 63346074 	,	95019110	,	31673036	)
	SetAdjValueProb( -4, 101762741 	,	152644110	,	50881369	)
	SetAdjValueProb( -3, 161466445 	,	242199666	,	80733221	)
	SetAdjValueProb( -2, 263744223 	,	395616333	,	131872110	)
	SetAdjValueProb( -1, 499999999 	,	749999997	,	249999998	)
	SetAdjValueProb( 1, 736255775 	,	868127889	,	604383664	)
	SetAdjValueProb( 2, 838533553 	,	919266778	,	757800331	)
	SetAdjValueProb( 3, 898237257 	,	949118630	,	847355887	)
	SetAdjValueProb( 4, 936653924 	,	968326963	,	904980887	)
	SetAdjValueProb( 5, 962298368 	,	981149185	,	943447553	)
	SetAdjValueProb( 6, 979427998 	,	989714000	,	969141998	)
	SetAdjValueProb( 7, 990475617 	,	995237809	,	985713426	)
	SetAdjValueProb( 8, 996961728 	,	998480864	,	995442592	)
	SetAdjValueProb( 9, 999900000 	,	999950000	,	999850000	)
	SetAdjValueProb( 10, 1000000000 	,	1000000000	,	1000000000	)
--}

AddDstParameter( "DST_CRITICAL_BONUS" )
--{
	SetAdjValueProb( -19, 100000 	,	150000	,	50000	)
	SetAdjValueProb( -17, 3038270 	,	4557405	,	1519135	)
	SetAdjValueProb( -15, 9524381 	,	14286571	,	4762190	)
	SetAdjValueProb( -13, 20572000 	,	30857999	,	10285999	)
	SetAdjValueProb( -11, 37701630 	,	56552444	,	18850814	)
	SetAdjValueProb( -9, 63346074 	,	95019110	,	31673036	)
	SetAdjValueProb( -7, 101762741 	,	152644110	,	50881369	)
	SetAdjValueProb( -5, 161466445 	,	242199666	,	80733221	)
	SetAdjValueProb( -3, 263744223 	,	395616333	,	131872110	)
	SetAdjValueProb( -1, 499999999 	,	749999997	,	249999998	)
	SetAdjValueProb( 1, 736255775 	,	868127889	,	604383664	)
	SetAdjValueProb( 3, 838533553 	,	919266778	,	757800331	)
	SetAdjValueProb( 5, 898237257 	,	949118630	,	847355887	)
	SetAdjValueProb( 7, 936653924 	,	968326963	,	904980887	)
	SetAdjValueProb( 9, 962298368 	,	981149185	,	943447553	)
	SetAdjValueProb( 11, 979427998 	,	989714000	,	969141998	)
	SetAdjValueProb( 13, 990475617 	,	995237809	,	985713426	)
	SetAdjValueProb( 15, 996961728 	,	998480864	,	995442592	)
	SetAdjValueProb( 17, 999900000 	,	999950000	,	999850000	)
	SetAdjValueProb( 19, 1000000000 	,	1000000000	,	1000000000	)
--}

AddDstParameter( "DST_SPEED" )
--{
	SetAdjValueProb( -10, 100000 	,	150000	,	50000	)
	SetAdjValueProb( -9, 3038270 	,	4557405	,	1519135	)
	SetAdjValueProb( -8, 9524381 	,	14286571	,	4762190	)
	SetAdjValueProb( -7, 20572000 	,	30857999	,	10285999	)
	SetAdjValueProb( -6, 37701630 	,	56552444	,	18850814	)
	SetAdjValueProb( -5, 63346074 	,	95019110	,	31673036	)
	SetAdjValueProb( -4, 101762741 	,	152644110	,	50881369	)
	SetAdjValueProb( -3, 161466445 	,	242199666	,	80733221	)
	SetAdjValueProb( -2, 263744223 	,	395616333	,	131872110	)
	SetAdjValueProb( -1, 499999999 	,	749999997	,	249999998	)
	SetAdjValueProb( 1, 736255775 	,	868127889	,	604383664	)
	SetAdjValueProb( 2, 838533553 	,	919266778	,	757800331	)
	SetAdjValueProb( 3, 898237257 	,	949118630	,	847355887	)
	SetAdjValueProb( 4, 936653924 	,	968326963	,	904980887	)
	SetAdjValueProb( 5, 962298368 	,	981149185	,	943447553	)
	SetAdjValueProb( 6, 979427998 	,	989714000	,	969141998	)
	SetAdjValueProb( 7, 990475617 	,	995237809	,	985713426	)
	SetAdjValueProb( 8, 996961728 	,	998480864	,	995442592	)
	SetAdjValueProb( 9, 999900000 	,	999950000	,	999850000	)
	SetAdjValueProb( 10, 1000000000 	,	1000000000	,	1000000000	)
--}

AddDstParameter( "DST_ATTACKSPEED" )
--{
	SetAdjValueProb( -380, 100000 	,	150000	,	50000	)
	SetAdjValueProb( -340, 3038270 	,	4557405	,	1519135	)
	SetAdjValueProb( -300, 9524381 	,	14286571	,	4762190	)
	SetAdjValueProb( -260, 20572000 	,	30857999	,	10285999	)
	SetAdjValueProb( -220, 37701630 	,	56552444	,	18850814	)
	SetAdjValueProb( -180, 63346074 	,	95019110	,	31673036	)
	SetAdjValueProb( -140, 101762741 	,	152644110	,	50881369	)
	SetAdjValueProb( -100, 161466445 	,	242199666	,	80733221	)
	SetAdjValueProb( -60, 263744223 	,	395616333	,	131872110	)
	SetAdjValueProb( -20, 499999999 	,	749999997	,	249999998	)
	SetAdjValueProb( 20, 736255775 	,	868127889	,	604383664	)
	SetAdjValueProb( 60, 838533553 	,	919266778	,	757800331	)
	SetAdjValueProb( 100, 898237257 	,	949118630	,	847355887	)
	SetAdjValueProb( 140, 936653924 	,	968326963	,	904980887	)
	SetAdjValueProb( 180, 962298368 	,	981149185	,	943447553	)
	SetAdjValueProb( 220, 979427998 	,	989714000	,	969141998	)
	SetAdjValueProb( 260, 990475617 	,	995237809	,	985713426	)
	SetAdjValueProb( 300, 996961728 	,	998480864	,	995442592	)
	SetAdjValueProb( 340, 999900000 	,	999950000	,	999850000	)
	SetAdjValueProb( 380, 1000000000 	,	1000000000	,	1000000000	)
--}

AddDstParameter( "DST_ADJDEF" )
--{
	SetAdjValueProb( -112, 100000 	,	150000	,	50000	)
	SetAdjValueProb( -100, 3038270 	,	4557405	,	1519135	)
	SetAdjValueProb( -88, 9524381 	,	14286571	,	4762190	)
	SetAdjValueProb( -76, 20572000 	,	30857999	,	10285999	)
	SetAdjValueProb( -64, 37701630 	,	56552444	,	18850814	)
	SetAdjValueProb( -52, 63346074 	,	95019110	,	31673036	)
	SetAdjValueProb( -40, 101762741 	,	152644110	,	50881369	)
	SetAdjValueProb( -28, 161466445 	,	242199666	,	80733221	)
	SetAdjValueProb( -16, 263744223 	,	395616333	,	131872110	)
	SetAdjValueProb( -4, 499999999 	,	749999997	,	249999998	)
	SetAdjValueProb( 4, 736255775 	,	868127889	,	604383664	)
	SetAdjValueProb( 16, 838533553 	,	919266778	,	757800331	)
	SetAdjValueProb( 28, 898237257 	,	949118630	,	847355887	)
	SetAdjValueProb( 40, 936653924 	,	968326963	,	904980887	)
	SetAdjValueProb( 52, 962298368 	,	981149185	,	943447553	)
	SetAdjValueProb( 64, 979427998 	,	989714000	,	969141998	)
	SetAdjValueProb( 76, 990475617 	,	995237809	,	985713426	)
	SetAdjValueProb( 88, 996961728 	,	998480864	,	995442592	)
	SetAdjValueProb( 100, 999900000 	,	999950000	,	999850000	)
	SetAdjValueProb( 112, 1000000000 	,	1000000000	,	1000000000	)
--}

AddDstParameter( "DST_ATKPOWER" )
--{
	SetAdjValueProb( -140, 100000 	,	150000	,	50000	)
	SetAdjValueProb( -125, 3038270 	,	4557405	,	1519135	)
	SetAdjValueProb( -110, 9524381 	,	14286571	,	4762190	)
	SetAdjValueProb( -95, 20572000 	,	30857999	,	10285999	)
	SetAdjValueProb( -80, 37701630 	,	56552444	,	18850814	)
	SetAdjValueProb( -65, 63346074 	,	95019110	,	31673036	)
	SetAdjValueProb( -50, 101762741 	,	152644110	,	50881369	)
	SetAdjValueProb( -35, 161466445 	,	242199666	,	80733221	)
	SetAdjValueProb( -20, 263744223 	,	395616333	,	131872110	)
	SetAdjValueProb( -5, 499999999 	,	749999997	,	249999998	)
	SetAdjValueProb( 5, 736255775 	,	868127889	,	604383664	)
	SetAdjValueProb( 20, 838533553 	,	919266778	,	757800331	)
	SetAdjValueProb( 35, 898237257 	,	949118630	,	847355887	)
	SetAdjValueProb( 50, 936653924 	,	968326963	,	904980887	)
	SetAdjValueProb( 65, 962298368 	,	981149185	,	943447553	)
	SetAdjValueProb( 80, 979427998 	,	989714000	,	969141998	)
	SetAdjValueProb( 95, 990475617 	,	995237809	,	985713426	)
	SetAdjValueProb( 110, 996961728 	,	998480864	,	995442592	)
	SetAdjValueProb( 125, 999900000 	,	999950000	,	999850000	)
	SetAdjValueProb( 140, 1000000000 	,	1000000000	,	1000000000	)
--}

AddDstParameter( "DST_HP_MAX" )
--{
	SetAdjValueProb( -500, 100000 	,	150000	,	50000	)
	SetAdjValueProb( -450, 3038270 	,	4557405	,	1519135	)
	SetAdjValueProb( -400, 9524381 	,	14286571	,	4762190	)
	SetAdjValueProb( -350, 20572000 	,	30857999	,	10285999	)
	SetAdjValueProb( -300, 37701630 	,	56552444	,	18850814	)
	SetAdjValueProb( -250, 63346074 	,	95019110	,	31673036	)
	SetAdjValueProb( -200, 101762741 	,	152644110	,	50881369	)
	SetAdjValueProb( -150, 161466445 	,	242199666	,	80733221	)
	SetAdjValueProb( -100, 263744223 	,	395616333	,	131872110	)
	SetAdjValueProb( -50, 499999999 	,	749999997	,	249999998	)
	SetAdjValueProb( 50, 736255775 	,	868127889	,	604383664	)
	SetAdjValueProb( 100, 838533553 	,	919266778	,	757800331	)
	SetAdjValueProb( 150, 898237257 	,	949118630	,	847355887	)
	SetAdjValueProb( 200, 936653924 	,	968326963	,	904980887	)
	SetAdjValueProb( 250, 962298368 	,	981149185	,	943447553	)
	SetAdjValueProb( 300, 979427998 	,	989714000	,	969141998	)
	SetAdjValueProb( 350, 990475617 	,	995237809	,	985713426	)
	SetAdjValueProb( 400, 996961728 	,	998480864	,	995442592	)
	SetAdjValueProb( 450, 999900000 	,	999950000	,	999850000	)
	SetAdjValueProb( 500, 1000000000 	,	1000000000	,	1000000000	)
--}

AddDstParameter( "DST_MP_MAX" )
--{
	SetAdjValueProb( -500, 100000 	,	150000	,	50000	)
	SetAdjValueProb( -450, 3038270 	,	4557405	,	1519135	)
	SetAdjValueProb( -400, 9524381 	,	14286571	,	4762190	)
	SetAdjValueProb( -350, 20572000 	,	30857999	,	10285999	)
	SetAdjValueProb( -300, 37701630 	,	56552444	,	18850814	)
	SetAdjValueProb( -250, 63346074 	,	95019110	,	31673036	)
	SetAdjValueProb( -200, 101762741 	,	152644110	,	50881369	)
	SetAdjValueProb( -150, 161466445 	,	242199666	,	80733221	)
	SetAdjValueProb( -100, 263744223 	,	395616333	,	131872110	)
	SetAdjValueProb( -50, 499999999 	,	749999997	,	249999998	)
	SetAdjValueProb( 50, 736255775 	,	868127889	,	604383664	)
	SetAdjValueProb( 100, 838533553 	,	919266778	,	757800331	)
	SetAdjValueProb( 150, 898237257 	,	949118630	,	847355887	)
	SetAdjValueProb( 200, 936653924 	,	968326963	,	904980887	)
	SetAdjValueProb( 250, 962298368 	,	981149185	,	943447553	)
	SetAdjValueProb( 300, 979427998 	,	989714000	,	969141998	)
	SetAdjValueProb( 350, 990475617 	,	995237809	,	985713426	)
	SetAdjValueProb( 400, 996961728 	,	998480864	,	995442592	)
	SetAdjValueProb( 450, 999900000 	,	999950000	,	999850000	)
	SetAdjValueProb( 500, 1000000000 	,	1000000000	,	1000000000	)
--}

AddDstParameter( "DST_FP_MAX" )
--{
	SetAdjValueProb( -500, 100000 	,	150000	,	50000	)
	SetAdjValueProb( -450, 3038270 	,	4557405	,	1519135	)
	SetAdjValueProb( -400, 9524381 	,	14286571	,	4762190	)
	SetAdjValueProb( -350, 20572000 	,	30857999	,	10285999	)
	SetAdjValueProb( -300, 37701630 	,	56552444	,	18850814	)
	SetAdjValueProb( -250, 63346074 	,	95019110	,	31673036	)
	SetAdjValueProb( -200, 101762741 	,	152644110	,	50881369	)
	SetAdjValueProb( -150, 161466445 	,	242199666	,	80733221	)
	SetAdjValueProb( -100, 263744223 	,	395616333	,	131872110	)
	SetAdjValueProb( -50, 499999999 	,	749999997	,	249999998	)
	SetAdjValueProb( 50, 736255775 	,	868127889	,	604383664	)
	SetAdjValueProb( 100, 838533553 	,	919266778	,	757800331	)
	SetAdjValueProb( 150, 898237257 	,	949118630	,	847355887	)
	SetAdjValueProb( 200, 936653924 	,	968326963	,	904980887	)
	SetAdjValueProb( 250, 962298368 	,	981149185	,	943447553	)
	SetAdjValueProb( 300, 979427998 	,	989714000	,	969141998	)
	SetAdjValueProb( 350, 990475617 	,	995237809	,	985713426	)
	SetAdjValueProb( 400, 996961728 	,	998480864	,	995442592	)
	SetAdjValueProb( 450, 999900000 	,	999950000	,	999850000	)
	SetAdjValueProb( 500, 1000000000 	,	1000000000	,	1000000000	)
--}

AddDstParameter( "DST_SPELL_RATE" )
--{
	SetAdjValueProb( -10, 100000 	,	150000	,	50000	)
	SetAdjValueProb( -9, 3038270 	,	4557405	,	1519135	)
	SetAdjValueProb( -8, 9524381 	,	14286571	,	4762190	)
	SetAdjValueProb( -7, 20572000 	,	30857999	,	10285999	)
	SetAdjValueProb( -6, 37701630 	,	56552444	,	18850814	)
	SetAdjValueProb( -5, 63346074 	,	95019110	,	31673036	)
	SetAdjValueProb( -4, 101762741 	,	152644110	,	50881369	)
	SetAdjValueProb( -3, 161466445 	,	242199666	,	80733221	)
	SetAdjValueProb( -2, 263744223 	,	395616333	,	131872110	)
	SetAdjValueProb( -1, 499999999 	,	749999997	,	249999998	)
	SetAdjValueProb( 1, 736255775 	,	868127889	,	604383664	)
	SetAdjValueProb( 2, 838533553 	,	919266778	,	757800331	)
	SetAdjValueProb( 3, 898237257 	,	949118630	,	847355887	)
	SetAdjValueProb( 4, 936653924 	,	968326963	,	904980887	)
	SetAdjValueProb( 5, 962298368 	,	981149185	,	943447553	)
	SetAdjValueProb( 6, 979427998 	,	989714000	,	969141998	)
	SetAdjValueProb( 7, 990475617 	,	995237809	,	985713426	)
	SetAdjValueProb( 8, 996961728 	,	998480864	,	995442592	)
	SetAdjValueProb( 9, 999900000 	,	999950000	,	999850000	)
	SetAdjValueProb( 10, 1000000000 	,	1000000000	,	1000000000	)
--}

