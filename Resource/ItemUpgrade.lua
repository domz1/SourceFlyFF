----------------------------------------------------------
-- Piercing (Probability n/10000 ) -----------------------
----------------------------------------------------------
tSuitProb = { 5000, 3500, 2500, 500 }
tWeaponProb = {  10000, 9000, 80000, 7000, 6000, 5000, 4000, 3000, 2000, 1000, 900, 850, 800, 750, 700, 650, 500, 450, 300, 200 }

----------------------------------------------------------
-- Global Upgrade Rates ----------------------------------
----------------------------------------------------------
tGeneral = { 10000, 10000, 10000, 9000, 8000, 7000, 6000, 5000, 4000, 3000, 2000, 1000, 900, 800, 700, 600, 500, 400, 300, 200 };

----------------------------------------------------------
-- Element Upgrading -------------------------------------
----------------------------------------------------------
tAttribute = {}
function AddAttribute( nNum, nProb, nDamageRate, nDefenseRate, nAddAtkDmgRate )
	tAttribute[nNum] = {}
	tAttribute[nNum].nProb = nProb
	tAttribute[nNum].nDamageRate = nDamageRate
	tAttribute[nNum].nDefenseRate = nDefenseRate
	tAttribute[nNum].nAddAtkDmgRate = nAddAtkDmgRate
end
--		Upgrade,	Probability,	Damage,			Defense,		Property compensation rate (excluding the value of refining all the values n/10000)
AddAttribute(	1,		10000,		200,			200,			500		)
AddAttribute(	2,		10000,		221,			221,			522		)
AddAttribute(	3,		10000,		256,			256,			560		)
AddAttribute(	4,		9000,		305,			305,			612		)
AddAttribute(	5,		8000,		368,			368,			680		)
AddAttribute(	6,		7000,		446,			446,			763		)
AddAttribute(	7,		6000,		537,			537,			860		)
AddAttribute(	8,		5000,		642,			642,			973		)
AddAttribute(	9,		4000,		761,			761,			1101		)
AddAttribute(	10,		3000,		895,			895,			1244		)
AddAttribute(	11,		2000,		1042,			1042,			1402		)
AddAttribute(	12,		1000,		1203,			1203,			1575		)
AddAttribute(	13,		900,		1379,			1379,			1763		)
AddAttribute(	14,		800,		1568,			1568,			1966		)
AddAttribute(	15,		700,		1772,			1772,			2184		)
AddAttribute(	16,		600,		1989,			1989,			2417		)
AddAttribute(	17,		500,		2221,			2221,			2665		)
AddAttribute(	18,		400,		2467,			2467,			2998		)
AddAttribute(	19,		300,		2726,			2726,			3207		)
AddAttribute(	20,		200,		3000,			3000,			3500		)

----------------------------------------------------------
-- Transy Probability (Do not touch!) --------------------
----------------------------------------------------------
nItemTransyLowLevel	= 500000
nItemTransyHighLevel	= 2000000