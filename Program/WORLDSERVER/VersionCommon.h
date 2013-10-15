#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__ 
#define __MAINSERVER
#define		__VER	15	// 15차

#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#if (_MSC_VER > 1200)
#define		__VS2003		// 컴파일러변경.net
#endif


#undef		__VER
#define		__VER 19//15//16//17//18

#define __DEBUG_SALT
#define __PAID_SET
#define __AI_AGGRO
#define __16_SHOP
#define __TREASUREBOX
//#define __16_FACKEL

#if __VER >= 19
#define __NEW_TASKBAR_V19
#define __Y_INTERFACE_VER3//TaskBar Part.
#endif//__VER >= 19

//#define __TABBED_INV

#if __VER >= 16
#define __NEW_PROP_PARAMETER 
#define __3RD_LEGEND16 
#define __CROSSBOW	
#define __NO_SUB_LANG 
#define __IMPROVE_MAP_SYSTEM
#define __INSTANCE_AGGRO_SYSTEM

#ifdef __NEW_PROP_PARAMETER
#define __NEW_ITEM_VARUNA
#define __NEW_STACKABLE_AMPS
#define __UPGRADE_SUCCESS_SCROLL
#endif // __NEW_PROP_PARAMETER	
#endif // __VER >= 16

#if __VER >= 17
#define	__FASHION_COMBINE
#define __NEW_WEAPON_GLOW
#define __BARUNA_PIERCING
#define __SHOPPING_CART
#define __COLOSSEUM
#define __FASTJOBCHANGE 
#define __BARUNA_WAKEUP
#define __ENVIRONMENT_EFFECT
#endif // __VER >= 17


#define	__SECURITY_FIXES


#define		D3DDEVICE	NULL
#define		__SERVER						// 클라이언트 전용코드를 빌드하지 않기 위한 define
#define		__X15
#define		__VERIFY_LOOP041010
#define		__S1108_BACK_END_SYSTEM			// 백앤드시스템 Trans, World, Neuz
//#define		__PROFILE_RUN
#define		__RES0807						// 리스폰 개선
#define		__CPU_UTILDOWN_060502			// CPU 사용률 감소작업 
#define		__SLIDE_060502					// 땅으로 꺼지는 문제 수정 
#define		__S8_SERVER_PORT				// 2006월 11월 7일 업데이트 - World, Cache, CoreServer
#define		__EVENT_1101					// 출첵 이벤트
#define		__NEWYEARDAY_EVENT_COUPON		// 설 이벤트 - 헤어샾, 성형 무료이용권 관련.
// 해외 6차 수정 적용
#define		__LANG_1013						// 해외 버전 추가 시 스크립트로 적용 Neuz. World
#define		__HACK_1130						// 미국 해킹 막기 - 강제 듀얼, 스크립트 실행
#define		__QUEST_1208					// 스크립트 패킷 처리 제한 V
#define		__TRAFIC_1222					// 중복 패킷 전송 막기
#define		__S_SERVER_UNIFY				// 서버 통합 Neuz, World I

// 해외 7차 수정 적용
#define		__BUGFIX_0326					// 비행 몬스터 아이템 드롭 R

// 8.5차
#define		__INFINITE_0227

#define		__LANG_IME_0327					// 국가별 ime표시 창 출력 구분 T
#define		__STL_0402						// stl U
// 9차
#define		__TRADESYS						// 교환 아이템 스크립트(이벤트, 퀘스트, ...) Neuz, World O
#define		__EVE_BALLOON					//풍선 이벤트 관련 S

#define		__ITEMDROPRATE					// 이벤트 아이템 드롭률 조정정

#define		__PK_PVP_SKILL_REGION			// PK금지구역에서 악행 스킬은 PVP상대에게만 가능하도록..
#define		__PVPDEBUFSKILL					// PVP 종료 후 디버프로 인해 카오가 되는 문제 수정 - World
#define		__EVENT_1101_2					// 출첵 이벤트 64비트 변경
#define		__S_RECOMMEND_EVE				// 추천 이벤트 Neuz, Trans, World
#define		__EVE_MINIGAME					// 이벤트 미니게임 4종, Neuz, World

#define		__ANGEL_LOG						// 엔젤 관련 로그
#define		__EXP_ANGELEXP_LOG				// 경험치, 엔젤 경험치 로그 관련 CHARACTER_TBL 컬럼 추가
#define		__S_ADD_EXP						// EXP_S 아이템 생성

#define		__RULE_0615						// 명명 규칙 정리
#define		__S_BUG_GC						// 길드대전 신청시 같은 금액을 신청하면 길드아이디로 쇼트 되서 순위가 바뀌는 문제 수정( map -> vector ) 
#define		__S_ADD_RESTATE					// 상용화 아이템 생성( 리스테트 힘, 민첩, 체력, 지능 )

//	#define		__REMOVE_ATTRIBUTE			// 속성제련 제거(10차로 변경)
//	#define		__CHAO_DMGDEC				// 카오시 PKValue에 따라 PVP Damage 감소(9차)

#define		__PROP_0827						// 아이템 프로퍼티 적용 파라미터 확장
#define		__RIGHTHAND_SKILL				// 스마이트 엑스, 엑스 마스터리, 블레이징소드, 소드 마스터리 오른손의 무기만을 기준으로 스킬사용
#define		__LOG_MATCHLESS					// 일반 유저가 무적상태가 되는 경우가 가끔 발생하여 로그 남김.

#define		__PKSERVER_USE_ANGEL			// PK서버에서만 엔젤관련 아이템,버프 사용

//	#define		__EVENTLUA_ATKDEF			// 루아 이벤트 - 공격력, 방어력 증가(9차로 변경됨)


// 11차
//	#define		__MA_VER11_02				// 수표 개념 화페 '페린' 추가
//	#define		__MA_VER11_04				// 길드 창고 로그 기능 world,database
//	#define		__MA_VER11_05				// 케릭터 봉인 거래 기능 world,database,neuz
//	#define		__MA_VER11_06				// 확율스킬 효과수정 world,neuz

//	#define		__CSC_VER11_3				// 칩으로 상점 이용 하기 (길드 대전 관련)
//	#define		__CSC_VER11_5				// 태스크바 확장
//	#define		__GUILDCOMBATCHIP			// 길드대전 칩보상 및 칩을 통한 상점 이용
//	#define		__GUILD_COMBAT_1TO1			// 일대일 길드 대전
//	#define		__EXPITEM_TOOLTIP_CHANGE	// 경험치 아이템 표기 방법 변경

//	#define		__REMOVE_ENDURANCE			// 아이템 내구력 개념 제거
//	#define		__PIERCING_REMOVE			// 피어싱 옵션 제거
#define		__EVENTLUA_COUPON			// 쿠폰 이벤트
#define		__NOLIMIT_RIDE_ITEM			// 레벨 제한 없는 비행체 아이템		// 국내만 적용
#define		__NPC_BUFF					// NPC를 통해 버프받기
//	#define		__REFLECTDMG_AFTER			// 반사데미지를 나중에 적용
//	#define		__CHIPI_071210				// 기간제 아이템 기간 만료 검사를 SavePlayer()에서 하도록 변경(15분 이내에 접속 종료시 무한대 사용 막음) - WORLDSERVER

//	#define		__SYS_POCKET				// 주머니
//	#define		__SYS_COLLECTING			// 채집
//	#define		__SYS_IDENTIFY				// 각성, 축복
//	#define		__FIX_COLLISION
//	#define		__MOD_VENDOR

#define		__JEFF_11					// 먹펫 언덕에 있는 아이템을 줏어먹지 못하는 문제등의 AI 개선이 필요
#define		__SYS_TICKET				// 입장권
//	#define		__SYS_PLAYER_DATA			// 캐릭터 통합 정보
#define		__HACK_1023					// 리소스 변조 // 날것 속도 // 무기 공격 속도
#define		__RT_1025					// 메신저
#define		__VENDOR_1106				// 개인상점 허용 문자

#define		__INVALID_LOGIN_0320		// 저장이 완료 되지 않은 사용자의 접속 막기(복사 방지)

#define		__JEFF_9_20					// 비행 시간을 침묵 시간으로 전용

#define		__EVENTLUA_GIFT				// 특정 레벨로 레벨업시 아이템 지급

#define		__JEFF_11_4					// 아레나
#define		__JEFF_11_5					// 메모리 누수
#define		__JEFF_11_6					// 바보 몬스터

#define		__DST_GIFTBOX				// 착용 아이템에 기프트 박스 기능 추가
#define		__EVENT_MONSTER				// 이벤트 몬스터(WorldServer)

#define		__CHIPI_DYO					// NPC를 특정국가에서만 출력할 수 있게 character.inc에 설정 
#define		__STL_GIFTBOX_VECTOR		// GiftBox Vector로 변경(박스 갯수 제한 문제)
#define		__CHIPI_ITEMUPDATE_080804	// 국내의 경우 g_uKey에 101이 없다. 그래서 수정...

#define		__VM_0820
//#define		__VM_0819	// 가상 메모리 누수 찾기

// 12차	
//	#define 	__ANGEL_NODIE				// 캐릭터가 사망해도 엔젤은 사망하지 않는다.
//	#define		__SECRET_ROOM				// 비밀의 방
//	#define		__TAX						// 세금
//	#define		__HEAVEN_TOWER				// 심연의 탑
//	#define		__EXT_PIERCING				// 무기 피어싱
//	#define		__MONSTER_SKILL				// 몬스터가 캐릭터 스킬 가능하도록 수정
//	#define		__NEW_SUMMON_RULE			// 몬스터 소환 규칙 변경
//	#define		__LORD		// 군주 시스템
//	#define		__PET_0519	// 펫 각성
//	#define		__J12_0		// 각성, 축복 가능 파츠 추가
//	#define		__RANGDA_0521	// 랜덤 이벤트 몬스터
//	#define		__MOD_TUTORIAL
//	#define		__JHMA_VER12_1	//12차 극단유료아이템  world,core
//	#define		__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
//	#define		__ITEMCREATEMON_S0602		// 몬스터 생성 Neuz, World
//	#define		__NEW_ITEMCREATEMON_SERVER	// 몬스터 생성 아이템(IK3_CREATE_MONSTER) 규칙 변경(서버)
//	#define		__EVENTLUA_0826
//	#define		__LEAK_0827
//	#define		__UPDATE_OPT

// 13차
//	#define		__EXT_ENCHANT				// 제련 확장(속성, 일반)
//	#define		__RAINBOW_RACE				// 레인보우 레이스
//	#define		__HOUSING					// 하우징 시스템
//	#define		__QUEST_HELPER				// 퀘스트 NPC위치 알림.
//	#define		__CHIPI_QUESTITEM_FLAG		// 퀘스트 보상 아이템 귀속 설정
//	#define		__HONORABLE_TITLE			// 달인
//	#define		__COUPLE_1117				// 커플 시스템
//	#define		__COUPLE_1202				// 커플 보상

#define		__LAYER_1015		// 동적 객체 층
#define		__LAYER_1020		// __LAYER_1015 테스트
#define		__LAYER_1021		// __LAYER_1015 리스폰
#define		__AZRIA_1023	// 입장권 층 적용
#define		__PET_1024		// 펫 작명
#define		__BUFF_1107

#define		__OCCUPATION_SHOPITEM	// 점령길드 전용 구매 가능 아이템

#define		__SYNC_1217		// 비동기 수정(필리핀 보고)
#define		__SPEED_SYNC_0108		// ResetDestParam speed 수정 느려지게 보이는것 수정 타유저의 초기스피드비동기도 수정 예정 
#define		__SYS_ITEMTRANSY			// 아이템 트랜지를 시스템에서 지원

#define		__EVENTLUA_CHEEREXP		// 루아 이벤트 - 응원 경험치 설정

#define		__FUNNY_COIN			// 퍼니 코인

#define		__MAP_SECURITY				// 맵툴로 변조한 맵 검사

// 14차
//	#define		__NEW_CONTINENT				// 14차 신규대륙 하르모닌 추가
//	#define		__SMELT_SAFETY				// 14차 안전제련
//	#define		__INSTANCE_DUNGEON			// 14차 인스턴스 던전 기반
//	#define		__PARTY_DUNGEON				// 14차 극단 전용 인스턴스 전던
//	#define		__ANGEL_EXPERIENCE			// 14차 엔젤 변경 사항(경험치 습득)
//	#define		__EQUIP_BIND				// 장착아이템 착용시 귀속
//	#define		__EXT_ATTRIBUTE				// 속성제련 공식 확장 및 변경
//	#define		__NEW_ITEM_LIMIT_LEVEL		// 아이템 착용레벨 규칙 변경(MASTER, HERO도 레벨 적용을 받고 하위 클래스 무기는 무조건 장착 가능)
//	#define		__BALLOON_CODE_IMPROVEMENT	// 풍선 관련 코드 개선
//	#define		__PCBANG					// PC방 혜택
	#define		__QUIZ						// 퀴즈 이벤트 시스템

#define		__BUFF_TOGIFT				// 버프 시간 만료되면 아이템 지급하는 시스템(IK2_BUFF_TOGIFT)
#define		__EVENTLUA_SPAWN			// 아이템 및 몬스터 스폰 이벤트
#define		__EVENTLUA_KEEPCONNECT		// 누적 접속 아이템 지급 이벤트

#define		__PERIN_BUY_BUG				// 페냐 반복구매 버그 확인용 코드

#define		__ERROR_LOG_TO_DB		// 에러 로그 시스템

#define		__EVENTLUA_RAIN				// 장마 이벤트 -> 루아로 변경

#define		__EVENTLUA_SNOW				// 강설 이벤트

#define		__ADD_RESTATE_LOW			// 상용화 아이템 생성( 리스테트 하급 힘, 민첩, 체력, 지능 )


// 15차
//	#define		__PETVIS					// 15차 비스펫
//	#define		__GUILD_HOUSE				// 15차 길드하우스
//	#define		__TELEPORTER				// 15차 텔레포터 
//	#define		__IMPROVE_QUEST_INTERFACE	// 15차 향상된 퀘스트 인터페이스 시스템
//	#define		__CAMPUS					// 15차 사제 시스템
//	#define		__HERO129_VER15				// 15차 히어로 레벨확장
//	#define		__IMPROVE_SYSTEM_VER15		// 15차 시스템 개선사항
//	#define		__DYNAMIC_MPU				// 변경가능한 MPU
//	#define		__USING_CONTINENT_DATA		// 대륙 경계정보 외부데이터에서 긁어옴
//	#define		__REACTIVATE_EATPET			// 유저와 먹펫이 일정거리 이상 멀어지면 재소환
//	#define		__15_5TH_ELEMENTAL_SMELT_SAFETY	// 15.5차 속성 안전 제련 추가
//	end15th


	#define		__FORCE_KILL_SERVER

	#define		__SHOP_COST_RATE			// 상점 가격 조정

//	#define		__ITEMTRANSY_PENYA			//	페냐로 아이템 트랜지 가능하게 설정.
	#define		__PROTECT_AWAKE				//	각성 보호의 두루마리.

//	#define		__ENVIRONMENT_EFFECT

#if	  defined(__INTERNALSERVER)	// 내부 사무실 테스트서버 
//	#define	__RULE_0615

//	#define		__VERIFY_MEMPOOL
	#define		__GUILDVOTE					// 길드 투표 
	#define		__IAOBJ0622					// 상대 목표 좌표 전송	// 월드, 뉴즈
	#define		__SKILL0517
//	#define		__S_NEW_SKILL_2				// 스킬 개선 패킷 전송 Neuz, World, Trans
	#define		__Y_CASTING_SKIP			// 케스팅 없이 스킬 발동 - Neuz, World
	#define		__YAIMONSTER_EX				// AI기능 추가 - Neuz, World
	#define		__Y_PATROL					// 배회하기 - Neuz, World
	#define		__V060721_TEXTDRAG			// 글자에 효과주기
	#define		__Y_BEAUTY_SHOP_CHARGE
//	#define		__LUASCRIPT060908			// lua 스크립트엔진( 아이템 ) 
	#define		__Y_FLAG_SKILL_BUFF			// 팻, 변신 아이템 토글버프 적용... Neuz, World
//	#define		__EVENT_FALL				// 단풍 이벤트 - 모델교체 Neuz, World
//	#define		__LANG_1013					// 해외 버전 추가 스크립트
//	#define		__JEFF_VER_8				// 8차 작업
//	#define		__JHMA_VER_8_1				// 8차 게임내돈드롭금지	Neuz, World 
//	#define		__JHMA_VER_8_2				// 8차 게임내아이템판매가격제한풀기	Neuz, World
//	#define		__JHMA_VER_8_5				// 8차 스킬경험치다운변경	Neuz, World
//	#define		__JHMA_VER_8_6				// 8차 지상몬스터가 저공비행유저를 공격가능하게함   World
//	#define		__JHMA_VER_8_7				// 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
//	#define		__JHMA_VER_8_5_1			// 8.5차 경비병 범위스킬 공격효과 불가로 수정 World
//	#define		__JHMA_VER_8_5_2			// 8.5차 듀얼중인 두구룹이 연관되지않게 수정  World
//	#define		__CSC_VER8_3				// 8차 Buff창 관련. Neuz, World
//	#define		__CSC_VER8_4				// 8차 헤어샾, 성형수술 관련 Neuz, World
//	#define		__CSC_VER8_5				// 8차 엔젤 소환 Neuz, World, Trans
//	#define		__CSC_VER8_6				// 운영자 명령어 statall 추가 World
	#define		__Y_MAX_GENERAL_LEVEL_8				// 만랩 120으로 조정... Neuz, World, Trans
	#define		__Y_HAIR_BUG_FIX
	#define		__EVENT_0117				// 구정 이벤트
//	#define		__NEWYEARDAY_EVENT_COUPON	// 설 이벤트 - 헤어샾, 성형 무료이용권 관련.
	#define		__TRAFIC_1215
//	#define     __Y_NEW_ENCHANT				// 제련 아이템 변경, Neuz, World

//	#define		__INVALID_LOGIN_0320		// 저장이 완료 되지 않은 사용자의 접속 막기(복사 방지)
	#define		__BUGFIX_0326				// 비행 몬스터 이벤트 아이템 드롭 수정
// 10차
//	#define		__LEGEND					// 10차 전승시스템	Neuz, World, Trans
// 10차
//	#define		__ULTIMATE					// 9, 10차 얼터멋 웨폰 제련 시스템
	
//	#define		__PET_0410					// 9, 10차 펫
//	#define		__JEFF_9					// 9, 10차 추가 작업
//	#define		__AI_0509					// 몬스터 인공지능
	#define		__HACK_0516					// 미국 해킹 2차
//	#define		__LUASCRIPT					// 루아 스크립트 사용 (World, Trans, Neuz)
//	#define		__EVENTLUA					// 이벤트 (루아 스크립트 적용) - World, Trans, Neuz

//	#define		__FLYBYATTACK0608			// 9th FlyByAttack edit
//	#define		__PVPDEMAGE0608				// 9th PVP DEMAGE edit
//	#define		__BLADELWEAPON0608			// 9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정
//	#define		__METEONYKER_0608
//	#define		__Y_DRAGON_FIRE
//	#define		__CSC_VER9_5				// 9차 제련관련 (추가 검광 기능)

	#define		__GLOBAL_COUNT_0705			// CTime::GetTimer

//	#define		__SKILL_0706				// 대인용 AddSkillProp 컬럼 추가 및 적용
//	#define		__AI_0711					// 메테오니커 AI 수정
//	#define		__HACK_0720					// 리소스 해킹
//11
//	#define		__JEFF_11					// 먹펫 언덕에 있는 아이템을 줏어먹지 못하는 문제등의 AI 개선이 필요
	
//	#define		__SYS_TICKET				// 입장권
//	#define		__SYS_PLAYER_DATA			// 캐릭터 통합 정보
//	#define		__HACK_1023					// 리소스 변조 // 날것 속도 // 무기 공격 속도
//	#define		__RT_1025					// 메신저

//	#define		__VENDOR_1106				// 개인상점 허용 문자

	#define		__JEFF_11_1
	#define		__JEFF_11_3					// 서버/클라이언트 리소스 분리
//	#define		__JEFF_11_4					// 아레나

//	#define		__JEFF_11_5					// 메모리 누수

//	#define		__DST_GIFTBOX				// 착용 아이템에 기프트 박스 기능 추가

	#define		__PERF_0226

	#define		__OPT_MEM_0811
//	#define		__MEM_TRACE
//	#define		__NEW_PROFILE		// 새로운 프로파일러 적용

	#define		__VTN_TIMELIMIT				// 베트남 플레이 시간 제한

	#define		__PROTECT_AWAKE				//	각성 보호의 두루마리.


	#undef		__VER
	#define		__VER 16

	#define		__GUILD_HOUSE_MIDDLE		// 길드하우스 중형

	#define		__MOVER_STATE_EFFECT		// 무버 상태값 변화에 따른 이펙트 적용

	#define		__NEW_ITEM_VARUNA			// 신 제련 시스템(바루나)


#elif defined(__TESTSERVER)		// 외부 유저 테스트서버 
	
	#define		__GUILDVOTE					// 길드 투표 
//	#define		__SKILL0517					// 스킬 레벨 파라미터
	#define		__Y_CASTING_SKIP			// 케스팅 없이 스킬 발동 - Neuz, World
	#define		__YAIMONSTER_EX				// AI기능 추가 - Neuz, World
	#define		__Y_PATROL					// 배회하기 - Neuz, World
	#define		__V060721_TEXTDRAG			// 글자에 효과주기
	#define		__Y_BEAUTY_SHOP_CHARGE
	#define		__Y_FLAG_SKILL_BUFF			// 팻, 변신 아이템 토글버프 적용... Neuz, World
//	#define		__EVENT_FALL				// 단풍 이벤트 - 모델교체 Neuz, World
	#define		__Y_MAX_GENERAL_LEVEL_8				// 만랩 120으로 조정... Neuz, World, Trans
	#define		__TRAFIC_1215

	// 10차
//	#define		__LEGEND					// 10차 전승시스템	Neuz, World, Trans

//	#define		__CSC_VER9_2				// 9차 상태창 변경 관련 Neuz, World

	#define		__HACK_0516					// 미국 해킹 2차

//	#define		__FLYBYATTACK0608			// 9th FlyByAttack edit
//	#define		__PVPDEMAGE0608				// 9th PVP DEMAGE edit
//	#define		__BLADELWEAPON0608			// 9th 블레이드 양손에 무기를 착용 시 추가 옵션은 오른손에 들고 있는 무기의 것만 적용이 되도록 수정

//	#define		__Y_DRAGON_FIRE
//	#define		__LUASCRIPT					// 루아 스크립트 사용 (World, Trans, Neuz)
//	#define		__EVENTLUA					// 이벤트 (루아 스크립트 적용) - World, Trans, Neuz
//	#define		__CSC_VER9_5				// 9차 제련관련 (추가 검광 기능)

//	#define		__PET_0410					// 9, 10차 펫
//	#define		__METEONYKER_0608
//	#define		__SKILL_0706				// 대인용 AddSkillProp 컬럼 추가 및 적용
//	#define		__AI_0711					// 메테오니커 AI 수정
//	#define		__JEFF_9					// 9, 10차 추가 작업
//	#define		__AI_0509					// 몬스터 인공지능
	#define		__GLOBAL_COUNT_0705			// CTime::GetTimer

//	#define		__JEFF_11					// 먹펫 언덕에 있는 아이템을 줏어먹지 못하는 문제등의 AI 개선이 필요
//	#define		__SYS_TICKET				// 입장권
//	#define		__SYS_PLAYER_DATA			// 캐릭터 통합 정보
//	#define		__HACK_1023					// 리소스 변조 // 날것 속도 // 무기 공격 속도
//	#define		__RT_1025					// 메신저
//	#define		__VENDOR_1106				// 개인상점 허용 문자

//	#undef	__VER
//	#define	__VER	11						// 11차

	#define		__JEFF_11_1
	#define		__JEFF_11_3					// 서버/클라이언트 리소스 분리
//	#define		__JEFF_11_4					// 아레나
//	#define		__JEFF_11_5					// 메모리 누수

//	#define		__JAPAN_SAKURA				// 벗꽃 이벤트

	#define		__OPT_MEM_0811
//	#define		__MEM_TRACE
	
//	#define		__NEW_PROFILE		// 새로운 프로파일러 적용

	#define		__GUILD_HOUSE_MIDDLE		// 길드하우스 중형

#elif defined(__MAINSERVER)	// 외부 본섭
 
	#define		__ON_ERROR
	#define		__IDC
	#define		__Y_BEAUTY_SHOP_CHARGE		// 뷰티샵, 개인상점 유료아이템화..Neuz, World
	#define		__TRAFIC_1215
//	#define		__EVENT_FALL		// 단풍

//	#define     __Y_NEW_ENCHANT				// 제련 아이템 변경, Neuz, World

	#define		__JEFF_11_1
	#define		__JEFF_11_3		// 서버/클라이언트 리소스 분리

	#define		__EVENT_0117				// propEvent.inc spawn
//	#define		__JAPAN_SAKURA				// 벗꽃 이벤트
//	#define		__RAIN_EVENT		// 장마 이벤트(비오는 동안 경험치 2배)

	#define		__OPT_MEM_0811
//	#define		__MEM_TRACE
//	#define		__NEW_PROFILE		// 새로운 프로파일러 적용
	#define		__GLOBAL_COUNT_0705			// CTime::GetTimer

#endif	// end - 서버종류별 define 

// 임시 - 여기다 넣지 말것 
#if __VER >= 7
	#define		__REMOVE_SCIRPT_060712		 
#endif

#endif