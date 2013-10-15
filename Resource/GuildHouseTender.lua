----------------------------------------------------------------------
----------------------------------------------------------------------
dofile(".\\LuaFunc\\GuildHouseFunc.lua")
----------------------------------------------------------------------
----------------------------------------------------------------------

AddTender( "WI_GUILDHOUSE_MIDDLE" )
--{
	SetMaxHouse( 20 )
	SetMinPenya( 500000000 )
	SetChargeRate( 0.85 )

	IsBeginNoticeTime 	= "Mon 10:00"
	IsEndNoticeTime 	= "Mon 15:00"
	IsBeginTenderTime	= "Mon 16:00"
	IsEndTenderTime		= "Mon 17:00"

	SetNpc= "MaFl_GUILDHOUSESALE 1"
	SetNpc= "MaSa_GUILDHOUSESALEROA 2"
	SetNpc= "MaEw_GUILDHOUSESALETROY 3"
--}

-- Flaris Guildhouse Notices
SetGHInfo( "MaFl_GUILDHOUSNOTICE_01", "WI_GUILDHOUSE_MIDDLE", GH_AREA_FLARIS , "Testtest Gildenhaus Flaris 1" )
SetGHInfo( "MaFl_GUILDHOUSNOTICE_02", "WI_GUILDHOUSE_MIDDLE", GH_AREA_FLARIS , "Testtest Gildenhaus Flaris 2" )
SetGHInfo( "MaFl_GUILDHOUSNOTICE_03", "WI_GUILDHOUSE_MIDDLE", GH_AREA_FLARIS , "Testtest Gildenhaus Flaris 3" )
SetGHInfo( "MaFl_GUILDHOUSNOTICE_04", "WI_GUILDHOUSE_MIDDLE", GH_AREA_FLARIS , "Testtest Gildenhaus Flaris 4" )
SetGHInfo( "MaFl_GUILDHOUSNOTICE_05", "WI_GUILDHOUSE_MIDDLE", GH_AREA_FLARIS , "Testtest Gildenhaus Flaris 5" )
-- Saint Morning Guildhouse Notices
SetGHInfo( "MaSa_GUILDHOUSNOTICE_01", "WI_GUILDHOUSE_MIDDLE", GH_AREA_SAINTMORNING , "Testtest Gildenhaus Saint Morning 1" )
SetGHInfo( "MaSa_GUILDHOUSNOTICE_02", "WI_GUILDHOUSE_MIDDLE", GH_AREA_SAINTMORNING , "Testtest Gildenhaus Saint Morning 2" )
SetGHInfo( "MaSa_GUILDHOUSNOTICE_03", "WI_GUILDHOUSE_MIDDLE", GH_AREA_SAINTMORNING , "Testtest Gildenhaus Saint Morning 3" )
SetGHInfo( "MaSa_GUILDHOUSNOTICE_04", "WI_GUILDHOUSE_MIDDLE", GH_AREA_SAINTMORNING , "Testtest Gildenhaus Saint Morning 4" )
SetGHInfo( "MaSa_GUILDHOUSNOTICE_05", "WI_GUILDHOUSE_MIDDLE", GH_AREA_SAINTMORNING , "Testtest Gildenhaus Saint Morning 5" )
SetGHInfo( "MaSa_GUILDHOUSNOTICE_06", "WI_GUILDHOUSE_MIDDLE", GH_AREA_SAINTMORNING , "Testtest Gildenhaus Saint Morning 6" )
SetGHInfo( "MaSa_GUILDHOUSNOTICE_07", "WI_GUILDHOUSE_MIDDLE", GH_AREA_SAINTMORNING , "Testtest Gildenhaus Saint Morning 7" )
SetGHInfo( "MaSa_GUILDHOUSNOTICE_08", "WI_GUILDHOUSE_MIDDLE", GH_AREA_SAINTMORNING , "Testtest Gildenhaus Saint Morning 8" )
SetGHInfo( "MaSa_GUILDHOUSNOTICE_09", "WI_GUILDHOUSE_MIDDLE", GH_AREA_SAINTMORNING , "Testtest Gildenhaus Saint Morning 9" )
-- Elriun Guildhouse Notices
SetGHInfo( "MaEw_GUILDHOUSNOTICE_01", "WI_GUILDHOUSE_MIDDLE", GH_AREA_ELLIUN , "Testtest Gildenhaus Elliun 1" )
SetGHInfo( "MaEw_GUILDHOUSNOTICE_02", "WI_GUILDHOUSE_MIDDLE", GH_AREA_ELLIUN , "Testtest Gildenhaus Elliun 2" )
SetGHInfo( "MaEw_GUILDHOUSNOTICE_03", "WI_GUILDHOUSE_MIDDLE", GH_AREA_ELLIUN , "Testtest Gildenhaus Elliun 3" )

----------------------------------------------------------------------
----------------------------------------------------------------------
SetGHQuest( "QUEST_GUILDHOUSE_TRN3", "WI_GUILDHOUSE_MIDDLE", 130 )