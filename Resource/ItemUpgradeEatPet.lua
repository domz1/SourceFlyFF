--------------------------------------------------------------------
-- 초기화 ----------------------------------------------------------
--------------------------------------------------------------------
dofile( ".\\LuaFunc\\ItemUpgradeBaseFunc.lua" )
--------------------------------------------------------------------


--------------------------------------------------------------------
-- 각성 ------------------------------------------------------------
--------------------------------------------------------------------

AddRandomOptionExtension( "IK1_GENERAL", "IK2_GENERAL", "IK3_PET" )
--{
	SetRandomOptionMaterial( "IK1_ACTIVE", "IK2_ONCE", "IK3_EATPET_RANDOMOPTION_GEN" )

	SetRandomOptionSizeProb( 1, 10000 )

	SetDstProb( "DST_STR", 700 )
	SetDstProb( "DST_DEX", 1400 )
	SetDstProb( "DST_INT", 2100 )
	SetDstProb( "DST_STA", 2800 )
	SetDstProb( "DST_CHR_CHANCECRITICAL", 3600 )
	SetDstProb( "DST_CRITICAL_BONUS", 4400 )
	SetDstProb( "DST_SPEED", 5100 )
	SetDstProb( "DST_ATTACKSPEED", 5800 )
	SetDstProb( "DST_ADJDEF", 6500 )
	SetDstProb( "DST_ATKPOWER", 7200 )
	SetDstProb( "DST_HP_MAX", 7900 )
	SetDstProb( "DST_MP_MAX", 8600 )
	SetDstProb( "DST_FP_MAX", 9300 )
	SetDstProb( "DST_SPELL_RATE", 10000 )
--}

--------------------------------------------------------------------
-- 각 파라미터들의 값과 확률 ---------------------------------------
--------------------------------------------------------------------

-- 확률 1/10억
AddDstParameter( "DST_STR" )
--{
	SetAdjValueProb( 1, 600000000 )
	SetAdjValueProb( 4, 900000000 )
	SetAdjValueProb( 7, 1000000000 )
--}

AddDstParameter( "DST_DEX" )
--{
	SetAdjValueProb( 1, 600000000 )
	SetAdjValueProb( 4, 900000000 )
	SetAdjValueProb( 7, 1000000000 )
--}

AddDstParameter( "DST_INT" )
--{
	SetAdjValueProb( 1, 600000000 )
	SetAdjValueProb( 4, 900000000 )
	SetAdjValueProb( 7, 1000000000 )
--}

AddDstParameter( "DST_STA" )
--{
	SetAdjValueProb( 1, 600000000 )
	SetAdjValueProb( 4, 900000000 )
	SetAdjValueProb( 7, 1000000000 )
--}

AddDstParameter( "DST_CHR_CHANCECRITICAL" )
--{
	SetAdjValueProb( 1, 600000000 )
	SetAdjValueProb( 2, 900000000 )
	SetAdjValueProb( 3, 1000000000 )
--}

AddDstParameter( "DST_CRITICAL_BONUS" )
--{
	SetAdjValueProb( 1, 600000000 )
	SetAdjValueProb( 3, 900000000 )
	SetAdjValueProb( 5, 1000000000 )
--}

AddDstParameter( "DST_SPEED" )
--{
	SetAdjValueProb( 1, 600000000 )
	SetAdjValueProb( 2, 900000000 )
	SetAdjValueProb( 3, 1000000000 )
--}

AddDstParameter( "DST_ATTACKSPEED" )
--{
	SetAdjValueProb( 20, 600000000 )
	SetAdjValueProb( 60, 900000000 )
	SetAdjValueProb( 100, 1000000000 )
--}

AddDstParameter( "DST_ADJDEF" )
--{
	SetAdjValueProb( 4, 600000000 )
	SetAdjValueProb( 16, 900000000 )
	SetAdjValueProb( 28, 1000000000 )
--}

AddDstParameter( "DST_ATKPOWER" )
--{
	SetAdjValueProb( 5, 600000000 )
	SetAdjValueProb( 20, 900000000 )
	SetAdjValueProb( 35, 1000000000 )
--}

AddDstParameter( "DST_HP_MAX" )
--{
	SetAdjValueProb( 50, 600000000 )
	SetAdjValueProb( 100, 900000000 )
	SetAdjValueProb( 150, 1000000000 )
--}

AddDstParameter( "DST_MP_MAX" )
--{
	SetAdjValueProb( 50, 600000000 )
	SetAdjValueProb( 100, 900000000 )
	SetAdjValueProb( 150, 1000000000 )
--}

AddDstParameter( "DST_FP_MAX" )
--{
	SetAdjValueProb( 50, 600000000 )
	SetAdjValueProb( 100, 900000000 )
	SetAdjValueProb( 150, 1000000000 )
--}

AddDstParameter( "DST_SPELL_RATE" )
--{
	SetAdjValueProb( 1, 600000000 )
	SetAdjValueProb( 2, 900000000 )
	SetAdjValueProb( 3, 1000000000 )
--}

