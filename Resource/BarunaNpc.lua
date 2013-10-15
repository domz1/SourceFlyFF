--------------------------------------------------------------------
-- 초기화 ----------------------------------------------------------
--------------------------------------------------------------------
dofile( ".\\LuaFunc\\BarunaNpcFunc.lua" )
--------------------------------------------------------------------

--		레벨,	누적 경험치,	추가 제련 성공률% (%값은 n/10000)
AddBarunaLevel(	1,	100,		50	)	
AddBarunaLevel(	2,	350,		100	)
AddBarunaLevel(	3,	900,		250	)
AddBarunaLevel(	4,	2050,		400	)
AddBarunaLevel(	5,	4400,		500	)


--					제련레벨,	성공경험치,	실패경험치,	감소경험치
AddBarunaNPCExpEnchantGeneral(		1,		1,		0,		0	)
AddBarunaNPCExpEnchantGeneral(		2,		1,		0,		0	)
AddBarunaNPCExpEnchantGeneral(		3,		4,		1,		1	)
AddBarunaNPCExpEnchantGeneral(		4,		6,		1,		1	)
AddBarunaNPCExpEnchantGeneral(		5,		8,		1,		1	)
AddBarunaNPCExpEnchantGeneral(		6,		12,		2,		2	)
AddBarunaNPCExpEnchantGeneral(		7,		16,		2,		2	)
AddBarunaNPCExpEnchantGeneral(		8,		20,		2,		2	)
AddBarunaNPCExpEnchantGeneral(		9,		24,		2,		2	)
AddBarunaNPCExpEnchantGeneral(		10,		28,		2,		2	)
AddBarunaNPCExpEnchantGeneral(		11,		32,		3,		3	)
AddBarunaNPCExpEnchantGeneral(		12,		36,		3,		3 	)
AddBarunaNPCExpEnchantGeneral(		13,		37,		3,		3 	)
AddBarunaNPCExpEnchantGeneral(		14,		38,		3,		3 	)
AddBarunaNPCExpEnchantGeneral(		15,		39,		3,		3 	)
AddBarunaNPCExpEnchantGeneral(		16,		40,		4,		4	)
AddBarunaNPCExpEnchantGeneral(		17,		41,		4,		4	)
AddBarunaNPCExpEnchantGeneral(		18,		42,		4,		4	)
AddBarunaNPCExpEnchantGeneral(		19,		43,		4,		4	)
AddBarunaNPCExpEnchantGeneral(		20,		44,		4,		4	)


--					제련레벨,	성공경험치,	실패경험치,	감소경험치
AddBarunaNPCExpEnchantAttribute(	1,		1,		0,		0	)
AddBarunaNPCExpEnchantAttribute(	2,		1,		0,		0	)
AddBarunaNPCExpEnchantAttribute(	3,		4,		1,		1	)
AddBarunaNPCExpEnchantAttribute(	4,		6,		1,		1	)
AddBarunaNPCExpEnchantAttribute(	5,		8,		1,		1	)
AddBarunaNPCExpEnchantAttribute(	6,		12,		2,		2	)
AddBarunaNPCExpEnchantAttribute(	7,		16,		2,		2	)
AddBarunaNPCExpEnchantAttribute(	8,		20,		2,		2	)
AddBarunaNPCExpEnchantAttribute(	9,		24,		2,		2	)
AddBarunaNPCExpEnchantAttribute(	10,		28,		2,		2	)
AddBarunaNPCExpEnchantAttribute(	11,		32,		3,		3	)
AddBarunaNPCExpEnchantAttribute(	12,		36,		3,		3 	)
AddBarunaNPCExpEnchantAttribute(	13,		37,		3,		3 	)
AddBarunaNPCExpEnchantAttribute(	14,		38,		3,		3 	)
AddBarunaNPCExpEnchantAttribute(	15,		39,		3,		3 	)
AddBarunaNPCExpEnchantAttribute(	16,		40,		4,		4	)
AddBarunaNPCExpEnchantAttribute(	17,		41,		4,		4	)
AddBarunaNPCExpEnchantAttribute(	18,		42,		4,		4	)
AddBarunaNPCExpEnchantAttribute(	19,		43,		4,		4	)
AddBarunaNPCExpEnchantAttribute(	20,		44,		4,		4	)


--				각성사이즈,	성공경험치,	취소경험치,	감소경험치
AddBarunaNPCExpRandomOption(	1,		1,		0,		0	)
AddBarunaNPCExpRandomOption(	2,		1,		0,		0	)
AddBarunaNPCExpRandomOption(	3,		2,		0,		0	)

