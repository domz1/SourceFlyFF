QUEST_BOSS_LV1
{
	Title = "등장 빅머슬"		
	Level = 25				
	World = 210  // Region보다 위에 선언되어야한다!!! 안그러면... 문제 많아짐...리플레이스 관련해서....ata3k	
	WorldKey = boss1
	Region = 0, 500, 500, 0         //전투 영역 Left, Top, Right, Bottom	
	
	Wormon
	{
		// ID, Pos
		MI_BIGMUSCLE, 267.8, 100, 338,		//몬스터 생성 위치 x, y, z
		MI_ANTTURTLE, 256, 100, 216,
		MI_ANTTURTLE, 251, 100, 219,
		MI_ANTTURTLE, 267, 100, 221,
		MI_ANTTURTLE, 263, 100, 241,
		MI_ANTTURTLE, 263, 100, 257,
		MI_ANTTURTLE, 257, 100, 259,
		MI_ANTTURTLE, 273, 100, 259
	}

	State 0
	{
		desc = "퀘스트 시작"
		//차후추가용////////////////////////////////////////////////////////////////
		//SetDesc
		//(
		//	LANG_KOR, "",
		//	LANG_ENG, "",
		//	LANG_JAP, "",
		//	LANG_CHI, "",
		//	LANG_THA, ""
		//);
 	}

	State 1
	{
		desc = "퀘스트 진행(몬스터 사냥 실패)"
		//차후추가용////////////////////////////////////////////////////////////////
		//SetDesc
		//(
		//	LANG_KOR, "",
		//	LANG_ENG, "",
		//	LANG_JAP, "",
		//	LANG_CHI, "",
		//	LANG_THA, ""
		//);
 	}

	State 14
	{
		desc = "퀘스트 완료(몬스터 사냥 성공)"
		//차후추가용////////////////////////////////////////////////////////////////
		//SetDesc
		//(
		//	LANG_KOR, "",
		//	LANG_ENG, "",
		//	LANG_JAP, "",
		//	LANG_CHI, "",
		//	LANG_THA, ""
		//);
 	}
}

QUEST_BOSS_LV2
{
	Title = "등장 크르릉"		
	Level = 35				
	World = 220	
	WorldKey = boss2
	Region = 0, 500, 500, 0         //전투 영역 Left, Top, Right, Bottom	

	Wormon
	{
		// ID, Pos
		MI_KRRR, 270.2, 100, 311,		//몬스터 생성 위치 x, y, z
		MI_HAMMERKICK, 242, 100, 207,		//몬스터 생성 위치 x, y, z
		MI_HAMMERKICK, 245, 100, 211,		//몬스터 생성 위치 x, y, z
		MI_HAMMERKICK, 242, 100, 212,		//몬스터 생성 위치 x, y, z
		MI_HAMMERKICK, 302, 100, 214,		//몬스터 생성 위치 x, y, z
		MI_HAMMERKICK, 298, 100, 210,		//몬스터 생성 위치 x, y, z
		MI_HAMMERKICK, 302, 100, 208,		//몬스터 생성 위치 x, y, z
		MI_HAMMERKICK, 270, 100, 231,		//몬스터 생성 위치 x, y, z
		MI_HAMMERKICK, 277, 100, 230,		//몬스터 생성 위치 x, y, z
		MI_HAMMERKICK, 263, 100, 230		//몬스터 생성 위치 x, y, z
	}
	
	State 0
	{
		desc = "퀘스트 시작"
		//차후추가용////////////////////////////////////////////////////////////////
		//SetDesc
		//(
		//	LANG_KOR, "",
		//	LANG_ENG, "",
		//	LANG_JAP, "",
		//	LANG_CHI, "",
		//	LANG_THA, ""
		//);
 	}

	State 1
	{
		desc = "퀘스트 진행(몬스터 사냥 실패)"
		//차후추가용////////////////////////////////////////////////////////////////
		//SetDesc
		//(
		//	LANG_KOR, "",
		//	LANG_ENG, "",
		//	LANG_JAP, "",
		//	LANG_CHI, "",
		//	LANG_THA, ""
		//);
 	}

	State 14
	{
		desc = "퀘스트 완료(몬스터 사냥 성공)"
		//차후추가용////////////////////////////////////////////////////////////////
		//SetDesc
		//(
		//	LANG_KOR, "",
		//	LANG_ENG, "",
		//	LANG_JAP, "",
		//	LANG_CHI, "",
		//	LANG_THA, ""
		//);
 	}
}

QUEST_BOSS_LV3
{
	Title = "등장 머쉬무트"		
	Level = 45				
	World = 230	
	WorldKey = boss3
	Region = 0, 500, 500, 0         //전투 영역 Left, Top, Right, Bottom	

	Wormon
	{
		// ID, Pos
		MI_MUSHMOOT, 333.7, 100, 312.6,		//몬스터 생성 위치 x, y, z
		
		MI_LEYENA4, 131, 131, 222,
		MI_LEYENA3, 131, 131, 222,

		MI_LEYENA4, 131, 129, 280,
		MI_LEYENA3, 131, 129, 280,

		MI_LEYENA3, 198, 143, 151,
		MI_DRILLER4, 198, 143, 151

	}
	
	
	State 0
	{
		desc = "퀘스트 시작"
		//차후추가용////////////////////////////////////////////////////////////////
		//SetDesc
		//(
		//	LANG_KOR, "",
		//	LANG_ENG, "",
		//	LANG_JAP, "",
		//	LANG_CHI, "",
		//	LANG_THA, ""
		//);
 	}

	State 1
	{
		desc = "퀘스트 진행(몬스터 사냥 실패)"
		//차후추가용////////////////////////////////////////////////////////////////
		//SetDesc
		//(
		//	LANG_KOR, "",
		//	LANG_ENG, "",
		//	LANG_JAP, "",
		//	LANG_CHI, "",
		//	LANG_THA, ""
		//);
 	}

	State 14
	{
		desc = "퀘스트 완료(몬스터 사냥 성공)"
		//차후추가용////////////////////////////////////////////////////////////////
		//SetDesc
		//(
		//	LANG_KOR, "",
		//	LANG_ENG, "",
		//	LANG_JAP, "",
		//	LANG_CHI, "",
		//	LANG_THA, ""
		//);
 	}
}

QUEST_IBLBOSS
{
	Title = "등장 이블리네스"		
	Level = 70				
	World = 205  // Region보다 위에 선언되어야한다!!! 안그러면... 문제 많아짐...리플레이스 관련해서....ata3k	
	WorldKey = IBLBOSS
	Region = 399, 499, 511, 413         //전투 영역 Left, Top, Right, Bottom	
	
	Wormon
	{
		// ID, Pos
		MI_IBLLINESS, 466, 2, 464		//몬스터 생성 위치 x, y, z
	}

	State 0
	{
		desc = "퀘스트 시작"
		//차후추가용////////////////////////////////////////////////////////////////
		//SetDesc
		//(
		//	LANG_KOR, "",
		//	LANG_ENG, "",
		//	LANG_JAP, "",
		//	LANG_CHI, "",
		//	LANG_THA, ""
		//);
 	}

	State 1
	{
		desc = "퀘스트 진행(몬스터 사냥 실패)"
		//차후추가용////////////////////////////////////////////////////////////////
		//SetDesc
		//(
		//	LANG_KOR, "",
		//	LANG_ENG, "",
		//	LANG_JAP, "",
		//	LANG_CHI, "",
		//	LANG_THA, ""
		//);
 	}

	State 14
	{
		desc = "퀘스트 완료(몬스터 사냥 성공)"
		//차후추가용////////////////////////////////////////////////////////////////
		//SetDesc
		//(
		//	LANG_KOR, "",
		//	LANG_ENG, "",
		//	LANG_JAP, "",
		//	LANG_CHI, "",
		//	LANG_THA, ""
		//);
 	}
}