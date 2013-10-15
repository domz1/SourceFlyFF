--------------------------------------------------------------------
-- Initialization --------------------------------------------------
--------------------------------------------------------------------
dofile( ".\\LuaFunc\\CampusFunc.lua" )
--------------------------------------------------------------------

RecoveryTime		=	MIN(5)		-- Priest Point recovery cycle
RecoveryPoint		=	1		-- Recovery Point


-- Recovery points that must be completed quests Priest
SetQuest( "QUEST_SCE_MDRIGALTEACHER5" )


-- Buff the increased accuracy between connected partner
SetBuffLevel( 1, "II_TS_BUFF_POWER_LOVE01" )
SetBuffLevel( 2, "II_TS_BUFF_POWER_LOVE02" )
SetBuffLevel( 3, "II_TS_BUFF_POWER_LOVE03" )


-- Payment Disciples of the priest during the leveling point
SetCampusReward( 15, 1, 1 )
SetCampusReward( 40, 1, 2 )
SetCampusReward( 60, 1, 2 )
SetCampusReward( 75, 2, 5 ) -- Graduation


