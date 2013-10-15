----------------------------------------------------------------------
---- Event-related loading Lua functions -----------------------------
---------------------------------------------------------------------- 
dofile(".\\LuaFunc\\EventFunc.lua")

----------------------------------------------------------------------
-- Auto-Bulletin -----------------------------------------------------
----------------------------------------------------------------------
bNotice = True -- Notice whether to run automatically(true or false)
Notice( "Tue 05:00", 50, 1 ) -- Automatic notification time interval Notice (MIN), notice the number of
--{
AddMessage( "Hey everyone!" )
AddMessage( "Server is still under fixes so expect reboot from time to time" )
AddMessage( "Report all bugs on forum please!" )
--}

----------------------------------------------------------------------
---- Initialization --------------------------------------------------
----------------------------------------------------------------------

-- Hourly drop-weight items
tHour = { 505, 409, 324, 280, 220, 203, 202, 212,
	  227, 261, 302, 349, 571, 701, 764, 803,
	  790, 789, 754, 849, 936, 940, 919, 720 }
----------------------------------------------------------------------

----------------------------------------------------------------------------------------------------------------
---- 1. AddEvent( strDesc ) -- Add an event registration and description
---- 2. SetTime( strStartTime, strEndTime ) -- Of the event start time, end time registration (multiple configurable)
---- ( ?? ?? -- "2007-05-03 17:53" )
---- 3. SetItem( ItemId, nMax, nNum, nLevel ) -- Events and drop items, most of one day, drop the number of (random),
---- The minimum level of monsters drop items - several possible
---- 4. SetExpFactor( fFactor ) -- Increased experience
---- 5. SetItemDropRate( fFactor ) -- Drops increased the item drop rate
---- 6. SetPieceItemDropRate( fFactor ) -- The piece has a monster multiple of items increase
---- 7. SetGoldDropFactor( fFactor ) -- Pena drop
---- 8. SetAttackPower( nAttackPower ) -- Increase Attack
---- 9. SetDefensePower( nDefensePower ) -- Increased defense
---- 10. SetCouponEvent( SEC(n) ) -- Coupon Event (access time - SEC (n) or MIN (n))
---- 11. SetLevelUpGift( nLevel, "all", ItemId, nNum, byFlag ) -- Level-Up Gifts (nLevel achieve items paid for, "all" part specifies the specific account (eg. "__bu" - Buddy, "__an" - Angel)
---- 12. SetCheerExpFactor( fFactor ) -- Set cheer experience
---- 13. SetSpawn( TYPE, strId, nNum ) -- Sponsored events: Type - ITEM or MONSTER, ID, 1 Seuponil seuponryang
---- 14. SetKeepConnectEvent( nTime, strItemId, nItemNum ) -- Cumulative access items paid event (connection time, the item ID, item number)
---- 15. SetRainEvent( fExpFactor, strTitle ) -- Rainy events (Exp Value, Event Title)
---- 16. SetSnowEvent( fExpFactor, strTitle ) -- Snowfall events (Exp Value, Event Title)
---- *** Add an event to register from 1 to repeat 3-16 times as needed may be omitted. 
----------------------------------------------------------------------------------------------------------------
--[[ SAMPLE
AddEvent( "EVENT TEST 01" )
--{
	SetTime( "2007-06-08 14:23", "2007-06-08 16:11" )
	SetTime( "2007-06-09 14:23", "2007-06-10 00:00" )
	
	SetItem( "II_SYS_SYS_EVE_HAPPYMONEY01", 30000, 5, 15 )
	SetItem( "II_SYS_SYS_EVE_PUMPKIN01", 2000, 3, 15 )
	SetExpFactor( 2 )
	SetItemDropRate( 2 )
	SetPieceItemDropRate( 2 )
	SetGoldDropFactor( 2 )
	SetAttackPower( 150 )
	SetDefensePower( 100 )
	SetCouponEvent( MIN(120) )
	SetLevelUpGift( 15, "__bu", "II_SYS_SYS_SCR_BX_PET_LAWOLF7", 1, 2 )
	SetCheerExpFactor( 1.3 )
	SetSpawn( ITEM, "II_SYS_SYS_EVE_HAPPYMONEY01", 10000 )
	SetSpawn( MONSTER, "MI_AIBATT1", 2000 )
	SetSpawn( MONSTER, "MI_AIBATT4", 2000 )
	SetKeepConnectEvent( MIN(60), "II_SYS_SYS_SCR_BXTREASURE01", 1 )
	SetRainEvent( 2, "Experience the two times when it rains!" )
	SetSnowEvent( 2, "If snow falls experience 2 times!" ) 
--}
--]]

-------------------------------------------------------------------------
---- Begin Script -------------------------------------------------------
-------------------------------------------------------------------------

AddEvent( "RedChip Level Up Reward" )
--{
	SetTime( "2010-02-23 00:00", "2011-12-25 23:59" )
	
	SetRainEvent( 2, "Rain event (x2 Exp)" )
	SetSnowEvent( 1.1, "Snow event (x2 Drop)" )

--}

AddEvent( "InsanityFlyff Level-UP Rewards!!" )
--{
	SetTime( "2010-02-23 00:00", "2099-12-25 23:59" )

	SetLevelUpGift( 60, "all", "II_CHP_RED", 100 )
	SetLevelUpGift( 70, "all", "II_CHP_RED", 200 )
	SetLevelUpGift( 80, "all", "II_CHP_RED", 300 )
	SetLevelUpGift( 90, "all", "II_CHP_RED", 400 )
	SetLevelUpGift( 100, "all", "II_CHP_RED", 500 )
	SetLevelUpGift( 110, "all", "II_CHP_RED", 600 )
	SetLevelUpGift( 120, "all", "II_CHP_RED", 700 )
	SetLevelUpGift( 130, "all", "II_CHP_RED", 800 )
	SetLevelUpGift( 140, "all", "II_CHP_RED", 900 )
	SetLevelUpGift( 150, "all", "II_CHP_RED", 1000 )
	SetLevelUpGift( 160, "all", "II_CHP_RED", 1100 )
	SetLevelUpGift( 170, "all", "II_CHP_RED", 1200 )
	SetLevelUpGift( 180, "all", "II_CHP_RED", 1300 )
	SetLevelUpGift( 190, "all", "II_CHP_RED", 1400 )
	SetLevelUpGift( 200, "all", "II_CHP_RED", 1500 )
	SetLevelUpGift( 120, "all", "II_SYS_SYS_SCR_PERIN", 1, 2 )
	SetLevelUpGift( 65, "all", "II_SYS_SYS_GM_SKLVLALL", 10 )
	SetLevelUpGift( 110, "all", "II_SYS_SYS_GM_SKLVLALL", 5 )

--}