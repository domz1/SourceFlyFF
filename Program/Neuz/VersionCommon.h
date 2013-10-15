#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__
#define __MAINSERVER
#define		__VER	15		// 15차

#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#if (_MSC_VER > 1200)
#define		__VS2003		// 컴파일러변경.net
#endif

#undef		__VER
#define		__VER 19//15//16//17//18

#define __DEBUG_SALT

/*===============================================*/

#if __VER >= 19
#define __CHAT_V19
#define __BUTTONS_V19
#define __WINDOWS_V19
#define __TARGET_INF_V19
#define __STATUS_V19
#define __GAUGE_V19
#define __OPTION_EX
#define __NEW_TASKBAR_V19
#define __Y_INTERFACE_VER3
#define __19_INTERFACE_UPDATE
#endif // __VER >= 19

//#define __TABBED_INV
#define __CASH_AREA
#define __PAID_SET
#define __AI_AGGRO
#define __TREASUREBOX

#define	__ENVIRONMENT_EFFECT
#define __FASTJOBCHANGE//original by punbaa edited by adeilson
#define __WINDOW_STYLE 
/*===============================================*/

#if __VER >= 16
#define __GUILD_COLOR
#define __NEW_PROP_PARAMETER 
#define __3RD_LEGEND16 
#define __CROSSBOW	
//#define __NO_AFTER_SKIN 
#define __NO_SUB_LANG 
#define __IMPROVE_MAP_SYSTEM 
#define __NEW_SKILL_TREE 
#define	__BS_EFFECT_LUA	
#ifdef __NEW_PROP_PARAMETER
#define __NEW_ITEM_VARUNA
#define __NEW_STACKABLE_AMPS
#define __UPGRADE_SUCCESS_SCROLL
#endif // __NEW_PROP_PARAMETER	
#endif // __VER >= 16

#if __VER >= 17
#define __INSTANCE_AGGRO_SYSTEM 
#define __LOADING_SCREEN_UPDATE
#define	__FASHION_COMBINE
#define __NEW_WEAPON_GLOW
#define __BARUNA_PIERCING
#define __COLOSSEUM
#define __BARUNA_WAKEUP
#define __16_SHOP
#endif // __VER >= 17

#define	__SECURITY_FIXES
#define __INVISIBLE_FIX

#define		__TRANSFER_ERROR_TEXT
#define		__CRC
#define		__PROTOCOL0910
#define		__PROTOCOL1021
#define		__INFO_SOCKLIB0516			// 네트워크 에러얻기 
#define		__S1108_BACK_END_SYSTEM		// Trans, World, Neuz : 백앤드시스템 
#define		__THROUGHPORTAL0810			// 웹에 의한 실행
#define		__TWN_LOGIN0816				// 대만 로그인 프로토콜 변경	// 클라/인증
#define		__FIX_WND_1109				// 창 생성 시 부모창 지정 오류 수정
#define		__EVENT_1101				// 출첵 이벤트
#define		__EVE_NEWYEAR				// 신년 효과

#define		__S_SERVER_UNIFY			// 서버 통합 Neuz, World
#define		__LANG_IME_0327				// 국가별 ime표시 창 출력 구분
#define		__STL_0402					// stl

#define		__NEWYEARDAY_EVENT_COUPON	// 설 이벤트 - 헤어샾, 성형 무료이용권 관련.
// 해외 6차 적용
#define		__LANG_1013					// 해외 버전 신규 추가 스크립트

#define		__ITEMDROPRATE				// 이벤트 아이템 드롭률 조정


// 10
#define		__TRADESYS					// 교환 아이템 스크립트(이벤트, 퀘스트, ... ) Neuz, World
#define		__EVE_BALLOON				// 풍선 이벤트 관련
#define		__CSC_GAME_GRADE			// 게임물등급관련 채팅창 알림 및 이미지 변경.

#define		__PK_PVP_SKILL_REGION		// PK금지구역에서 악행 스킬은 PVP상대에게만 가능하도록..
#define		__EVENT_1101_2				// 출첵 이벤트 64비트 변경
#define		__S_RECOMMEND_EVE			// 추천 이벤트 Neuz, Trans, World
#define		__EVE_MINIGAME				// Mini Game 4종 이벤트 추가 관련.
#define		__S_ADD_EXP					// EXP_S 아이템 생성

#define		__RULE_0615					// 명명 규칙 정리
#define		__FILTER_0705				// 필터링 규칙 변경
#define		__S_BUG_GC					// 길드대전 신청시 같은 금액을 신청하면 길드아이디로 쇼트 되서 순위가 바뀌는 문제 수정( map -> vector ) 
#define		__S_ADD_RESTATE				// 상용화 아이템 생성( 리스테트 힘, 민첩, 체력, 지능 )

#define		__PROP_0827					// 아이템 프로퍼티 적용 파라미터 확장

#define		__PKSERVER_USE_ANGEL		// PK서버에서만 엔젤관련 아이템,버프 사용
#define		__CSC_ENCHANT_EFFECT_2		// 기존 이펙트 + 향상된 이펙트

//11차
//	#define		__MA_VER11_02				// 수표 기능 페린 관련
//	#define		__MA_VER11_03				// neuz극단창(B)에서 극단원이 멀리 떨어져 있어도 레벨이 표시 되도록
//	#define		__MA_VER11_04				// 길드 창고 로그 기능 world,database,neuz
//	#define		__MA_VER11_05				// 케릭터 봉인 거래 기능 world,database,neuz
//	#define		__MA_VER11_06				// 확율스킬 효과수정 world,neuz
//	#define		__CSC_VER11_1				// <레벨??> 삭제, 채팅창 수정, 페냐거래 취소버튼 삭제
//	#define		__CSC_VER11_2				// 타겟 표시 방법 변경
//	#define		__CSC_VER11_3				// 칩으로 상점 이용 하기 (길드 대전 관련)
//	#define		__CSC_VER11_4				// 메신저창 개선
//	#define		__CSC_VER11_5				// 태스크바 확장
//	#define		__GUILDCOMBATCHIP			// 길드대전 칩보상 및 칩을 통한 상점 이용
//	#define		__GUILD_COMBAT_1TO1			// 일대일 길드 대전
//	#define		__GUILD_BANK_LOG			// 길드뱅크 로그창 추가
//	#define		__EXPITEM_TOOLTIP_CHANGE	// 경험치 아이템 표기 방법 변경
//	#define		__REMOVE_ENDURANCE			// 아이템 내구력 개념 제거
//	#define		__PIERCING_REMOVE			// 피어싱 옵션 제거
//	#define		__CHIPI_071210				// 기간제 아이템 지속시간 초단위 표기

#define		__EVENTLUA_COUPON			// 쿠폰 이벤트
#define		__NOLIMIT_RIDE_ITEM			// 레벨 제한 없는 비행체 아이템
#define		__NPC_BUFF					// NPC를 통해 버프받기

//	#define		__SYS_POCKET				// 주머니
//	#define		__SYS_COLLECTING			// 채집 시스템
//	#define		__SYS_IDENTIFY				// 각성, 축복
//	#define		__FIX_ROTATE				// 비스트에서 발견된 오류 수정
//	#define		__ADD_ZOOMOPT				// 줌옵션 추가 
//	#define		__MOD_VENDOR				// 개인상점 개선
//	#define		__FIX_COLLISION				// 충돌 문제 수정
//	#define		__FIX_PICKING				// 피킹 문제 수정
//	#define		__CAPTURE_JPG				// JPG포맷으로 캡쳐

#define		__SYS_TICKET				// 입장권
//	#define		__SYS_PLAYER_DATA			// 캐릭터 정보 통합
#define		__HACK_1023					// 리소스 변조 // 날것 속도, 무기 공격 속도
#define		__RT_1025					// 메신저
#define		__VENDOR_1106				// 개인상점 허용 문자
#define		__JEFF_11

#define		__JEFF_9_20					// 비행 시간을 침묵 시간으로 전용
#define		__JEFF_11_4					// 아레나
#define		__JEFF_11_5					// 메모리 누수
#define		__JEFF_11_6					// 바보 몬스터
#define		__DST_GIFTBOX				// 착용 아이템에 기프트 박스 기능 추가 

#define		__SFX_OPT					// 이펙트(파티클) 최적화 관련
//#define		__PERF_0229

#define		__HELP_BUG_FIX				// 도움말관련 버그 수정 (도움말 한개만 띄우기 인스턴스 관리)

#define		__CHIPI_DYO					// NPC를 특정국가에서만 출력할 수 있게 character.inc에 설정 

// 12차

//	#define		__SECRET_ROOM				// 비밀의 방
//	#define		__TAX						// 세금
//	#define		__CSC_VER12_1				// 개인상점 중 플레이어 살펴보기 메뉴 가능하도록 수정 & 살펴보기 창 유지하기
//	#define		__CSC_VER12_2				// 길드 창의 멤버 텝에 항목별 정렬이 가능하도록 추가
//	#define		__CSC_VER12_3				// 퀘스트가 많을 경우 퀘스트 항목이 잘리는 현상 때문에 대화창 늘림
//	#define		__CSC_VER12_4				// 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
//	#define		__CSC_VER12_5				// 펫 알 변환 기능 추가
//	#define		__HEAVEN_TOWER				// 심연의 탑
//	#define		__EXT_PIERCING				// 무기 피어싱
//	#define		__NEW_SUMMON_RULE			// 몬스터 소환 규칙 변경
//	#define		__MOD_TUTORIAL				// 튜토리얼 개선
//	#define		__LORD						// 군주
//	#define		__PET_0519	// 펫 각성
//	#define		__J12_0		// 각성, 축복 가능 파츠 추가
//	#define		__UPDATE_OPT				// 옵션창 개선
//	#define		__CAM_FAST_RECOVER			// 카메라 복원속도 증가
//	#define		__JHMA_VER12_1	//12차 극단유료아이템
//	#define		__ITEMCREATEMON_S0602		// 몬스터 생성 Neuz, World
//	#define		__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz
//	#define		__LEAK_0827


// 13차
//	#define		__EXT_ENCHANT				// 제련 확장(속성, 일반)
//	#define		__RAINBOW_RACE				// 레인보우 레이스
	#define		__HOUSING					// 하우징 시스템
//	#define		__QUEST_HELPER				// 퀘스트 NPC위치 알림.
//	#define		__CHIPI_QUESTITEM_FLAG
//	#define		__RENEW_CHARINFO			// 캐릭터정보창 리뉴얼
//	#define		__HONORABLE_TITLE			// 달인
	#define		__MAX_BUY_ITEM9999			// 화살포스터구입갯수9999개
//	#define		__REMOVE_JOINPARTYMEMBER_ANI			// 파티맴버조인시 애니 빼기
//	#define		__CSC_VER13_1				// WorldMap 몬스터 표시
//	#define		__CSC_VER13_2				// 커플 시스템
//	#define		__COUPLE_1117				// 커플 시스템
//	#define		__COUPLE_1202				// 커플 보상

#define		__LAYER_1020				// __LAYER_1015 테스트
#define		__AZRIA_1023				// 입장권 계층 적용
#define		__PET_1024					// 펫 작명
#define		__BUFF_1107
#define		__SYNC_1217					// 비동기 수정(필리핀 보고)
#define		__SYS_ITEMTRANSY			// NPC메뉴 아이템 트렌지 기능

#define		__SPEED_SYNC_0108		// ResetDestParam speed 수정 느려지게 보이는것 수정 타유저의 초기스피드비동기도 수정 예정 
#define		__PARTY_DEBUG_0129		// 극단장 튕기는 현상 디버깅 neuz

#define		__FUNNY_COIN			// 퍼니 코인

#define		__GPAUTH
#define		__GPAUTH_01
#define		__GPAUTH_02
#define		__EUROPE_0514
#define		__WINDOW_INTERFACE_BUG		// 월드 이동 상황에서 아이템 걸린 윈도우 창 띄워져 있으면 클라이언트 크래쉬되는 문제 수정

#define		__MAP_SECURITY				// 맵툴로 변조한 맵 검사

// 14차
	#define		__NEW_CONTINENT				// 14차 신규대륙 하르모닌 추가
	#define		__SMELT_SAFETY				// 14차 안전제련
	#define		__INSTANCE_DUNGEON			// 14차 인스턴스 던전
	#define		__EQUIP_BIND				// 장착아이템 착용시 귀속
	#define		__EXT_ATTRIBUTE				// 속성제련 공식 확장 및 변경
	#define		__NEW_ITEM_LIMIT_LEVEL		// 아이템 착용레벨 규칙 변경(MASTER, HERO도 레벨 적용을 받고 하위 클래스 무기는 무조건 장착 가능)
	#define		__WATER_EXT					// 물 표현 확장 (물 텍스쳐의 종류 및 프레임속도 변경 추가)
	#define		__WND_EDIT_NUMBER_MODE		// 에디트 컨트롤 숫자만 입력받게 하는 모드
#define		__SHIFT_KEY_CORRECTION		// 쉬프트 키 입력 문제 수정
	#define		__STATIC_ALIGN				// 스태틱 컨트롤에 정교한 정렬 기능 추가
	#define		__ITEM_DROP_SOUND			// 아이템 드롭 시 효과음 스크립트로 처리
	#define		__JOB_TEXT					// 마스터, 히어로 관련 아이템에 '필요 직업' 텍스트 출력되도록 수정
	#define		__RESTATE_CONFIRM			// 리스테트 사용 시, 확인 창 출력
	#define		__PREVENTION_TOOLTIP_BUG	// 프리벤션 툴팁에 의미 없는 텍스트 출력되는 문제 수정
	#define		__DROP_CONFIRM_BUG			// 아이템 관련 확인 창이 열린 상태에서 일어나는 각종 오류 수정
	#define		__CLOUD_ANIMATION_BUG		// 구름 애니메이션이 특정 영역에서만 수행되는 문제 수정
	#define		__BUFF_CRASH				// 버프 렌더링 관련하여 클라이언트 크래쉬되는 문제 수정
	#define		__BS_FIX_SHADOW_ONOBJECT	// 오브젝트 그림자 리시버 조건검사 설정 ( 주인공 위주, 인던 천정 그림자 제거 )
	#define		__BS_FIX_HAIR_AMBIENT		// 캐릭터 헤어 ambient 조정 ( 인던에서 머리카락 색깔 까맣게 나오는 문제 수정 )
	#define		__BALLOON_CODE_IMPROVEMENT	// 풍선 관련 코드 개선
	#define		__WING_ITEM					// 날개 아이템
	#define		__USE_SOUND_LIB_FMOD		// use FMod sound lib
	#define		__PCBANG					// PC방 혜택
//	end 14th

// 15차 ZU TEST ZWECKEN AUSDEKLARIERUNG ENTFERNT!
	#define		__DYNAMIC_MPU				// 변경가능한 MPU !!
	#define		__BOUND_BOX_COLLISION		// 바운드 박스 충돌 루틴 개선
	#define		__BS_CHANGING_ENVIR			// 대륙별 환경 변경 ( light, fog, sky, weather, 24hours light ... ) <<< ONLY CLIENT!!!! >>>
	#define		__USING_CONTINENT_DATA		// 대륙 경계데이터 외부에서 긁어옴! 
	#define		__IMPROVE_QUEST_INTERFACE	// 15차 향상된 퀘스트 인터페이스 시스템
	#define		__IMPROVE_SYSTEM_VER15		// 15차 개선 사항
	#define		__15TH_INSTANCE_DUNGEON		// 15차 인스턴스 던전
	#define		__PETVIS					// 15차 비스펫
	#define		__GUILD_HOUSE				// 15차 길드하우스
	#define		__TELEPORTER				// 15차 텔레포터
	#define		__FIND_OBJ_INSIGHT			// 시야거리 안의 동적오브젝트 출력( 미니맵 오른쪽버튼 클릭 )
	#define		__HERO129_VER15				// 15차 히어로 레벨확장
	#define		__MUSIC2					// 마일즈 7.2g ver 업데이트 ( OGG 재생 가능 버젼  ) 차후 Lib업데이트시에 같이 풀어줘야함.
#define		__NEW_CONTINENT15			// 추가 대륙 ( 협곡 )
	#define		__BS_BBOX_ABS_EXTENT		// BBOX 길이 절대값으로 수정
	#define		__CAMPUS					// 15차 사제 시스템
	#define		__BS_FIXED_KNOCKBACK		// 넉백시 상태 꼬임현상 수정 
	#define		__BS_FIXED_EQUIPMOTION		// 아이템 장/탈 착중 기존모션이 유지되는현상 수정 
	#define		__15_5TH_ELEMENTAL_SMELT_SAFETY	// 15.5차 속성 안전 제련 추가
	#define		__2ND_PASSWORD_SYSTEM		// 2차 비밀번호 시스템
 // end 15th

	#define		__QUIZ						// 퀴즈 이벤트 시스템

	#define		__EVENTLUA_RAIN				// 장마 이벤트 -> 루아로 변경

	#define		__EVENTLUA_SNOW				// 강설 이벤트

	#define		__ADD_RESTATE_LOW			// 상용화 아이템 생성( 리스테트 하급 힘, 민첩, 체력, 지능 )

	#define		__YS_CHATTING_BLOCKING_SYSTEM	// 캐릭터 채팅 차단 시스템
	#define		__BAN_CHATTING_SYSTEM			// 연속 채팅 금지 시스템


	#define __ENCRYPT_PASSWORD	//	mulcom	BEGIN100218	패스워드 암호화

//	#define		__CERTIFIER_COLLECTING_SYSTEM	// 인증 서버 수집 시스템

	#define		__DELETE_CHAR_CHANGE_KEY_VALUE	// 버디버디 i_PIN 도입으로 인한 캐릭터삭제 키값 변경(국내, 주민번호 -> 2차비번)
	#define		__BS_ADJUST_SYNC			// mover간 동기화 개선 

	#define		__SHOP_COST_RATE			// 상점 가격 조정

//	#define		__NEW_WEB_BOX				// 새로운 웹 표현 창
	#define		__PROTECT_AWAKE				//각성보호
	#define		__MAIL_REQUESTING_BOX		// 메일 요청 박스
	#define     __BS_SAFE_WORLD_DELETE		// CWorld::Process에서 safe delete obj( sfx 류로 확인댐 )
//	#define		__GAME_GRADE_SYSTEM			// 게임물 등급 표시 시스템
	#define		__BS_FIX_ARRIVEPOS_ALGO		// 마우스 이동시 도착 검사루틴 개선 ( 정뱡향 축으로 한곳만 도착했을때 멈추는 문제가있었음 )
	#define		__BS_ITEM_UNLIMITEDTIME		// 아이템 유지시간 ( 999999999 >= 일경우 무제한 표기 )
	
//	#define		__ENVIRONMENT_EFFECT

// 8.5차
#if	  defined(__INTERNALSERVER)	// 내부 사무실 테스트서버 
//	#define	__RULE_0615

	#define		NO_GAMEGUARD
	#undef		__TRANSFER_ERROR_TEXT

	#define		__CPU_UTILDOWN_060502		// CPU 사용률 감소작업 
	#define		__SLIDE_060502				// 땅으로 꺼지는 문제 수정 
	#define		__GUILDVOTE					// 길드 투표 
	#define		__IAOBJ0622					// 상대 목표 좌표 전송	// 월드, 뉴즈
	#define		__YNOTICE_UNI1026			// 공지사항 유니코드 지원
//	#define		__YENV
//	#define		__YENV_WITHOUT_BUMP
	#define		__SKILL0517					// 스킬 레벨 파라미터
	#define		__YAIMONSTER_EX				// AI기능 추가 - Neuz, World
//	#define		__Y_INTERFACE_VER3			// 인터페이스 버전 3.0 - Neuz
	#define		__VERIFY_MEMPOOL
//	#define		__S_NEW_SKILL_2				// 스킬 개선 패킷 전송 Neuz, World, Trans
	#define		__Y_ROTXZ					// 회전 추가 Neuz, World
	#define		__Y_PATROL					// 정찰...Neuz. World
	#define		__Y_CHARACTER_TEXT			// 캐릭터 정보창 프로그램 텍스트 출력..Neuz
	#define		__Y_RENDER_NAME_OPTION		// 이름 출력 선택 UI.Neuz
	#define		__V060721_TEXTDRAG
	#define		__Y_MAPVIEW_EX
	#define		__Y_BEAUTY_SHOP_CHARGE		// 뷰티샵, 개인상점 유료아이템화..Neuz, World
//	#define		__EVENT_FALL				// 단풍 이벤트 - 모델교체 Neuz, World
//	#define		__LANG_1013					// 해외 버전 추가 스크립트
//	#define		__KWCSC_UPDATE				// 스킬 발동 관련 문제 수정 Neuz
//	#define		__CSC_VER8_1				// 8차 'T'키 사용, 상대방 레벨 안보이기, 화면 모든 창 On/Off Neuz
//	#define		__CSC_VER8_2				// 8차 파티창 관련 적용 Neuz
//	#define		__JEFF_VER_8				// 8차 작업
//	#define		__Y_FLAG_SKILL_BUFF			// 팻, 변신 아이템 토글버프 적용... Neuz, World
//	#define		__Y_GAMMA_CONTROL_8			// 밝기, 감마, 명암 조절 Neuz
//	#define		__Y_CHAT_SYSTEM_8			// 시스템 메세지 윈도우화... Neuz
//	#define		__Y_EYE_FLASH_8				// 눈깜박거리기... Neuz
//	#define		__Y_CAMERA_SLOW_8			// 8차 키/마우스 X축 회전시 보간으로 회전하기..Neuz
//	#define		__Y_MAX_GENERAL_LEVEL_8				// 만랩 120으로 조정... Neuz, World, Trans
//	#define		__CSC_VER8_3				// 8차 버프창 관련 적용, Key Down시 스킬발동 1.5초간 딜레이로 지속되게 변경 Neuz
//	#define		__CSC_VER8_4				// 8차 헤어샵관련 수정 및 성형수술 기능 추가.
//	#define		__JHMA_VER_8_1				// 8차 게임내돈드롭금지	Neuz, World
//	#define		__JHMA_VER_8_2				// 8차 게임내아이템판매가격제한풀기	Neuz, World
	#define		__FIX_WND_1109				// 자식 창 초기화 오류 수정
//	#define		__JHMA_VER_8_3				// 8차 ctrl 자동공격기능 삭제	Neuz
//	#define		__JHMA_VER_8_4				// 8차 타겟클릭자동공격 이모티콘	Neuz
//	#define		__JHMA_VER_8_5				// 8차 스킬경험치다운변경	Neuz, World
//	#define		__JHMA_VER_8_7				// 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World


//	#define		__CSC_VER8_5				// 8차 엔젤 소환 Neuz, World, Trans

	#define		__TRAFIC_1218				// 서버에서 보낸 패킷 정보
	#define		__Y_HAIR_BUG_FIX
	#define		__FOR_PROLOGUE_UPDATE		// 태국,대만,미국,독일 Prologue추가
//	#define		__NEWYEARDAY_EVENT_COUPON	// 설 이벤트 - 헤어샾, 성형 무료이용권 관련.
	#define		__TRAFIC_1215				// 전송량 감소

//	#define     __Y_NEW_ENCHANT				// 제련 아이템 변경, Neuz, World

//	#define		__ULTIMATE					// 9,10차 제련
//	#define		__LEGEND					// 10차 전승시스템	Neuz, World, Trans
//	#define		__CSC_VER9_1				// 9차 전승관련 Clienet - Neuz

//	#define		__PET_0410					// 9, 10차 펫
	#define		__HACK_0516					// 미국 해킹 2차
//	#define		__AI_0509					// 몬스터 인공지능

//	#define		__CSC_VER9_RESOLUTION		// Wide및 고해상도 추가
//	#define		__Y_ADV_ENCHANT_EFFECT		// 제련 이팩트 향상 버전
//	#define		__S_9_ADD					// 9차 추가분
//	#define		__RECOVERY10				// 9차 추가 (타격, 피격 10초 후 자동 회복)

//	#define		__CSC_VER9_2				// 9차 상태창 변경 관련 Neuz, World
//	#define		__CSC_VER9_3				// 9차 채팅상황에서 Function Key 사용 가능
//	#define		__CSC_VER9_4				// 클라이언트 구동 시 공백시간에 이미지 추가.
//	#define		__CSC_VER9_5				// 9차 제련관련 (추가 검광 기능)

//	#define		__Y_DRAGON_FIRE				// 메테오니커 파이어 발사!!!

//	#define		__LUASCRIPT					// 루아 스크립트 사용 (World, Trans, Neuz)
//	#define		__EVENTLUA					// 이벤트 (루아 스크립트 적용) - World, Trans, Neuz

//	#define		__METEONYKER_0608
//	#define		__CSC_UPDATE_WORLD3D		// World3D Object Culling부분 업데이트
//	#define		__CSC_VER9_REMOVE_PKSETTING	// 옵션 설정 창에서 PK설정 제거 (무조건 Ctrl키 누른 상태에서만 공격)

	#define		__SECURITY_0628				// 리소스 버전 인증 not contained

//	#define		__INSERT_MAP				// 스타트메뉴에 전체지도 메뉴 추가 

	#define		__GLOBAL_COUNT_0705			// CTime::GetTimer
	
//	#define		__SKILL_0706				// 대인용 AddSkillProp 컬럼 추가 및 적용

//	#define		__AI_0711					// 메테오니커 AI 수정

//	#define		__REMOVE_ATTRIBUTE			// 속성제련 제거(10차로 변경)
//	#define		__CSC_ENCHANT_EFFECT_2		// 기존 이펙트 + 향상된 이펙트
	
//	#define		__SYS_TICKET				// 입장권
//	#define		__SYS_PLAYER_DATA			// 캐릭터 정보 통합
//	#define		__HACK_1023					// 리소스 변조 // 날것 속도, 무기 공격 속도

//	#define		__RT_1025					// 메신저

//  #define		__LINK_PORTAL				// 외부 포탈 연결 시 무인자 처리

//	#define		__VENDOR_1106				// 개인상점 허용 문자
//	#define		__JEFF_11

	#define		__JEFF_11_1

//	#define		__JEFF_11_4					// 아레나
//	#define		__JEFF_11_5					// 메모리 누수

//	#define		__DST_GIFTBOX				// 착용 아이템에 기프트 박스 기능 추가 
	#define		__DISABLE_GAMMA_WND			// 윈도우 모드에서 감마 조절 불가하게 수정

	#define		__ATTACH_MODEL				// 모델에 다른 모델 붙이기 (날개...)
	#define		__NEW_PROFILE

	#define		__VTN_TIMELIMIT				// 베트남 플레이 시간 제한

//	#define		__FLYFF_INITPAGE_EXT

	#define		__BS_CHECKLEAK				// 메모리 누수 탐지및 라인 확인 2009/07/14
	#define		__BS_ADJUST_COLLISION		// 충돌 루틴 개선 ( 2009. 07. 28 )
//	#define		__BS_PUTNAME_QUESTARROW		// 퀘스트 클릭시 보상 NPC 화살표 표현중 이름 추가			( 09_1228 제거 )

	#define		__JAPAN_AUTH				// 일본 인증 변경(웹인증)
	#define		__IMPROVE_MAP_SYSTEM		// 향상된 지도 시스템

//	#define		__BS_NO_CREATION_POST		// 임시 : 우체통 생성 무시


	#undef		__VER
	#define		__VER 15

	#define		__GUILD_HOUSE_MIDDLE		// 길드하우스 중형
	#define		__BS_ADDOBJATTR_INVISIBLE	// CObj Invisible 에 관한 속성추가   --> 16차 예정 
	#define		__BS_DEATH_ACTION			// die 상태로 진입시 연출 효과 ( client only )
	#define		__BS_EFFECT_LUA				// 오브젝트 상태별 효과연출 ( Lua base )
	#define		__BS_ADD_CONTINENT_WEATHER	// 대륙 날씨 추가 ( 온난화로 인한 태양날씨, 다른 어떤이유로든 변하지 않음 )
	#define		__BS_CHANGEABLE_WORLD_SEACLOUD	// 변경가능한 월드 바다구름 

	
#elif defined(__TESTSERVER)  // 외부 유저 테스트서버 

	#define		NO_GAMEGUARD

	#define		__CPU_UTILDOWN_060502		// CPU 사용률 감소작업 
	#define		__SLIDE_060502				// 땅으로 꺼지는 문제 수정 
	#define     __GUILDVOTE					// 길드 투표
//	#define		__SKILL0517					// 스킬 레벨 파라미터
	#define		__Y_CHARACTER_TEXT			// 캐릭터 정보창 프로그램 텍스트 출력..Neuz
	#define		__V060721_TEXTDRAG			// 글자에 효과주기
	#define		__Y_ROTXZ					// 회전 추가 Neuz, World
	#define		__Y_PATROL					// 정찰...Neuz. World
	#define		__Y_BEAUTY_SHOP_CHARGE		// 뷰티샵, 개인상점 유료아이템화..Neuz, World
	#define		__TRAFIC_1215				// 전송량 감소

//	#define		__ULTIMATE					// 9,10차 제련
//	#define		__LEGEND					// 10차 전승시스템	Neuz, World, Trans
//	#define		__CSC_VER9_1				// 9차 전승관련 Clienet - Neuz

//	#define		__CSC_VER9_RESOLUTION		// Wide및 고해상도 추가
//	#define		__Y_ADV_ENCHANT_EFFECT		// 제련 이팩트 향상 버전
//	#define		__S_9_ADD					// 9차 추가분
//	#define		__RECOVERY10				// 9차 추가 (타격, 피격 10초 후 자동 회복)

//	#define		__CSC_VER9_2				// 9차 상태창 변경 관련 Neuz, World
//	#define		__CSC_VER9_3				// 9차 채팅상황에서 Function Key 사용 가능
//	#define		__CSC_VER9_4				// 클라이언트 구동 시 공백시간에 이미지 추가.
//	#define		__CSC_VER9_5				// 9차 제련관련 (추가 검광 기능)

//	#define		__Y_DRAGON_FIRE				// 메테오니커 파이어 발사!!!
	#define		__HACK_0516					// 미국 해킹 2차

//	#define		__LUASCRIPT					// 루아 스크립트 사용 (World, Trans, Neuz)
//	#define		__EVENTLUA					// 이벤트 (루아 스크립트 적용) - World, Trans, Neuz
//	#define		__CSC_VER9_REMOVE_PKSETTING	// 옵션 설정 창에서 PK설정 제거 (무조건 Ctrl키 누른 상태에서만 공격)

//	#define		__INSERT_MAP				// 스타트메뉴에 전체지도 메뉴 추가 

//	#define		__PET_0410					// 9, 10차 펫
//	#define		__AI_0509					// 몬스터 인공지능
//	#define		__METEONYKER_0608
//	#define		__SKILL_0706				// 대인용 AddSkillProp 컬럼 추가 및 적용
//	#define		__AI_0711					// 메테오니커 AI 수정
	#define		__GLOBAL_COUNT_0705			// CTime::GetTimer

//	#undef		__VER
//	#define		__VER	11					// 11차

	#define		__JEFF_11_1
//	#define		__JEFF_11_4					// 아레나
//	#define		__JEFF_11_5					// 메모리 누수

//	#define		__JAPAN_SAKURA				// 벗꽃 이벤트


//   VER 16 __TESTSERVER begin
	#define		__GUILD_HOUSE_MIDDLE		// 길드하우스 중형
	#define		__BS_ADDOBJATTR_INVISIBLE	// CObj Invisible 에 관한 속성추가   --> 16차 예정 
	#define		__BS_DEATH_ACTION			// die 상태로 진입시 연출 효과 ( client only )
//	#define		__BS_EFFECT_LUA				// 오브젝트 상태별 효과연출 ( Lua base )
	#define		__BS_ADD_CONTINENT_WEATHER	// 대륙 날씨 추가 ( 온난화로 인한 태양날씨, 다른 어떤이유로든 변하지 않음 )
	#define		__BS_CHANGEABLE_WORLD_SEACLOUD	// 변경가능한 월드 바다구름 
//   VER 16 __TESTSERVER end

	
#elif defined(__MAINSERVER)  // 외부 본섭

	#define		NO_GAMEGUARD
//	#define		__TMP_POCKET				// 휴대 가방 일시적으로 막기
	#define		__Y_BEAUTY_SHOP_CHARGE		// 뷰티샵, 개인상점 유료아이템화..Neuz, World
	#define		__TRAFIC_1215
//	#define     __Y_NEW_ENCHANT				// 제련 아이템 변경, Neuz, World
//	#define		__EVENT_FALL				// 단풍
//	#define		__JAPAN_SAKURA				// 벗꽃 이벤트

	#define		__JEFF_11_1
//	#define		__RAIN_EVENT				// 장마 이벤트(비오는 동안 경험치 2배)

#endif	// end - 서버종류별 define 

#ifndef NO_GAMEGUARD 
	#define	__NPROTECT_VER	4	
#endif	

#endif // VERSION_COMMON_H
