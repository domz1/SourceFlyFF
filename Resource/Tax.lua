----------------------------------------------------------------------------
dofile( ".\\LuaFunc\\TaxFunc.lua" )
----------------------------------------------------------------------------

MinTaxRate = 5				-- The minimum tax rate
MaxTaxRate = 6				-- The maximum tax rate

TaxSecretRoomRate	= 5	-- Occupation of the continent Guild Yield (%)
TaxLordRate			= 5	-- Sovereign Yield (%)

AdmissionSecretRoomRate	= 5	-- Admission rate (%)
AdmissionLordRate		= 5	-- Admission is sovereign yield (%)

PayTime			= "00:00"		-- Taxes paid hours
ChangeTaxTime	= "Mon 00:00"	-- Rate change time

DBSaveCount		= 10			-- DB storage rate (number)

----------------------------------------------------------------------------
-- Guild occupation set only purchased items -------------------------------
----------------------------------------------------------------------------
AddOccupationShopItem( "II_CHP_RED" )