#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__
#define	__MAINSERVER
#define	__VER	15

#define	__LANG_1013
//#define	__EVENT_FALL	// 단풍 이벤트 - 모델교체 Neuz, World
#define	__TRAFIC_1215
//#define	__TRAFIC_1218
#define	__FOR_PROLOGUE_UPDATE
#define	__SKILL_WITHOUT_EXP

#define	__NEWYEARDAY_EVENT_COUPON	// 설 이벤트 - 헤어샾, 성형 무료이용권 관련.	//
// 8-6
#define	__CSC_VER9_REMOVE_PKSETTING	// 옵션 설정 창에서 PK설정 제거 (무조건 Ctrl키 누른 상태에서만 공격)

#ifdef _DEBUG
#define	NO_GAMEGUARD
#endif	// _DEBUG

#define	__EVE_MINIGAME	// 미니 게임

#define	__RULE_0615

#define	__FILTER_0705	// 필터링 규칙 변경
#define	__PK_PVP_SKILL_REGION		// PK금지구역에서 악행 스킬은 PVP상대에게만 가능하도록..

#define	__VENDOR_1106	// 개인 상점 제목 허용 문자

#define	__JEFF_9_20		// 비행 시간을 침묵 시간으로 전용

#define	__GPAUTH
#define	__GPAUTH_01
#define	__GPAUTH_02		// 독일/프랑스 통합 인증 - 구매
#define	__EUROPE_0514

#define		__JEFF_11_5		// 메모리 누수
#define		__JEFF_11_6		// 바보 몬스터

#define		__SFX_OPT	// 이펙트(파티클) 최적화 관련


#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#define		__TRANSFER_ERROR_TEXT
#define		__MSVR
#define		__CRC
#define		__PROTOCOL0910
#define		__PROTOCOL1021
#define		__INFO_SOCKLIB0516				// 네트워크 에러얻기 
#define		__S1108_BACK_END_SYSTEM			// Trans, World, Neuz : 백앤드시스템 
#define		__VER6						// 버젼6차의 모든 기능  
#define		__THROUGHPORTAL0810		// 웹에 의한 실행
#define		__TWN_LOGIN0816		// 대만 로그인 프로토콜 변경	// 클라/인증
#define		__FIX_WND_1109		// 창 생성 시 부모창 지정 오류 수정
#define		__EVENT_1101		// 출첵 이벤트
#define		__EVE_NEWYEAR		// 신년 효과
#define		__S_SERVER_UNIFY			// 서버 통합 Neuz, World
#define		__LANG_IME_0327		// 국가별 ime출력 여부 설정
#define		__STL_0402

#define __HACK_0516 // 미국 해킹 2차

#define	__SECURITY_0628		// 리소스 버전 인증

#define		__ITEMDROPRATE	// 아이템 드롭률 조정(이벤트)

// 10
#define		__TRADESYS					// 교환 아이템 스크립트(이벤트, 퀘스트, ... ) Neuz, World
#define		__EVE_BALLOON				// 풍선 이벤트 관련
#define		__CSC_GAME_GRADE			// 게임물등급관련 채팅창 알림 및 이미지 변경.
#define		__EVENT_1101_2				// 출첵 이벤트 64비트 변경
#define		__S_RECOMMEND_EVE			// 추천 이벤트 Neuz, Trans, World
#define		__S_ADD_EXP					// EXP_S 아이템 생성
#define		__S_BUG_GC					// 길드대전 신청시 같은 금액을 신청하면 길드아이디로 쇼트 되서 순위가 바뀌는 문제 수정( map -> vector ) 
#define		__S_ADD_RESTATE				// 상용화 아이템 생성( 리스테트 힘, 민첩, 체력, 지능 )
#define		__PROP_0827		// 아이템 프로퍼티 적용 파라미터 확장
#define		__PKSERVER_USE_ANGEL		// PK서버에서만 엔젤관련 아이템,버프 사용
#define		__CSC_ENCHANT_EFFECT_2		// 속성 제련 이펙트 조정

#define		__HACK_1023		// 리소스 변조	// 탈것 속도	// 무기 공격 속도

// 11
#define		__MA_VER11_02				// 수표 기능 페린 관련
#define		__MA_VER11_03				// neuz극단창(B)에서 극단원이 멀리 떨어져 있어도 레벨이 표시 되도록
#define		__MA_VER11_04				// 길드 창고 로그 기능 world,database,neuz
#define		__MA_VER11_05				// 케릭터 봉인 거래 기능 world,database,neuz
#define		__MA_VER11_06				// 확율스킬 효과수정 world,neuz

#define		__CSC_VER11_1				// <레벨??> 삭제, 채팅창 수정, 페냐거래 취소버튼 삭제
#define		__CSC_VER11_2				// 타겟 표시 방법 변경
#define		__CSC_VER11_3				// 칩으로 상점 이용 하기 (길드 대전 관련)
#define		__CSC_VER11_4				// 메신저창 개선
#define		__CSC_VER11_5				// 태스크바 확장
#define		__GUILDCOMBATCHIP			// 길드대전 칩보상 및 칩을 통한 상점 이용
#define		__GUILD_COMBAT_1TO1			// 일대일 길드 대전
#define		__GUILD_BANK_LOG			// 길드뱅크 로그창 추가
#define		__EXPITEM_TOOLTIP_CHANGE	// 경험치 아이템 표기 방법 변경

#define		__REMOVE_ENDURANCE			// 아이템 내구력 개념 제거
#define		__PIERCING_REMOVE			// 피어싱 옵션 제거
#define		__EVENTLUA_COUPON			// 쿠폰 이벤트
#define		__NOLIMIT_RIDE_ITEM			// 레벨 제한 없는 비행체 아이템
#define		__NPC_BUFF					// NPC를 통해 버프받기

#define		__SYS_POCKET				// 주머니
#define		__SYS_COLLECTING			// 채집 시스템
#define		__SYS_IDENTIFY				// 각성, 축복
#define		__FIX_ROTATE				// 비스트에서 발견된 오류 수정
#define		__ADD_ZOOMOPT				// 줌옵션 추가 
#define		__MOD_VENDOR				// 개인상점 개선
#define		__FIX_COLLISION				// 충돌 문제 수정
#define		__FIX_PICKING				// 피킹 문제 수정

#define		__SYS_TICKET				// 입장권
#define		__SYS_PLAYER_DATA			// 캐릭터 정보 통합
#define		__HACK_1023					// 리소스 변조 // 날것 속도, 무기 공격 속도
#define		__RT_1025					// 메신저
#define		__VENDOR_1106				// 개인상점 허용 문자
#define		__JEFF_11
#define		__CHIPI_071210				// 기간제 아이템 지속시간 초단위 표기

//임시
//#define		__LINK_PORTAL				// 외부 포탈 연결 시 무인자 처리
#define		__JEFF_9_20		// 비행 시간을 침묵 시간으로 전용
#define		__JEFF_11_4		// 아레나
#define		__JEFF_11_6		// 바보 몬스터
#define		__DST_GIFTBOX	// 착용 아이템에 기프트 박스 기능 추가 
#define		__JEFF_11_1

#define		__CAPTURE_JPG
#define		__DISABLE_GAMMA_WND // 윈도우 모드에서 감마 조절 불가하게 수정

//#define		__TMP_POCKET				// 휴대 가방 일시적으로 막기

#define		__HELP_BUG_FIX				// 도움말관련 버그 수정 (도움말 한개만 띄우기 인스턴스 관리)

#define		__CHIPI_DYO					// NPC를 특정국가에서만 출력할 수 있게 character.inc에 설정 

// 12

#define		__SECRET_ROOM				// 비밀의 방
#define		__TAX						// 세금
#define		__CSC_VER12_1				// 개인상점 중 플레이어 살펴보기 메뉴 가능하도록 수정 & 살펴보기 창 유지하기
#define		__CSC_VER12_2				// 길드 창의 멤버 텝에 항목별 정렬이 가능하도록 추가
//#define		__CSC_VER12_3				// 퀘스트가 많을 경우 퀘스트 항목이 잘리는 현상 때문에 대화창 늘림
#define		__CSC_VER12_4				// 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
#define		__CSC_VER12_5				// 펫 알 변환 기능 추가

#define		__HEAVEN_TOWER				// 심연의 탑
#define		__EXT_PIERCING				// 무기 피어싱
#define		__NEW_SUMMON_RULE			// 몬스터 소환 규칙 변경

#define		__MOD_TUTORIAL				// 튜토리얼 개선
#define		__LORD						// 군주
#define		__PET_0519	// 펫 각성
#define		__J12_0		// 각성, 축복 가능 파츠 추가

#define		__UPDATE_OPT				// 옵션창 개선
#define		__CAM_FAST_RECOVER			// 카메라 복원속도 증가

#define		__JHMA_VER12_1	//12차 극단유료아이템
#define		__ITEMCREATEMON_S0602		// 몬스터 생성 Neuz, World

#define		__PARSKILL1001	//12차 파스킬 아이템 수정  world,core,neuz

#define		__SYNC_1217		// 비동기 수정(필리핀 보고)
#define		__SPEED_SYNC_0108               // ResetDestParam speed 수정 
#define		__PARTY_DEBUG_0129		// 극단장 튕기는 현상 디버깅 neuz

// 13차
#define		__EXT_ENCHANT				// 제련 확장(속성, 일반)
#define		__RAINBOW_RACE				// 레인보우 레이스
#define		__HOUSING					// 하우징 시스템
#define		__QUEST_HELPER				// 퀘스트 NPC위치 알림.
#define		__CHIPI_QUESTITEM_FLAG

#define		__ANIMATED_PARTS			// 기능성 확장 파트
#define		__RENEW_CHARINFO			// 캐릭터정보창 리뉴얼
#define		__HONORABLE_TITLE			// 달인
#define		__MAX_BUY_ITEM9999			// 화살포스터구입갯수9999개
#define		__REMOVE_JOINPARTYMEMBER_ANI			// 파티맴버조인시 애니 빼기

#define		__CSC_VER13_1				// WorldMap 몬스터 표시
#define		__CSC_VER13_2				// 커플 시스템

#define		__LAYER_1020				// __LAYER_1015 테스트
#define		__AZRIA_1023				// 입장권 계층 적용
#define		__PET_1024					// 펫 작명
#define		__BUFF_1107
#define		__COUPLE_1117				// 커플 시스템
#define		__COUPLE_1202				// 커플 보상

#define		__SYNC_1217					// 비동기 수정(필리핀 보고)
#define		__SYS_ITEMTRANSY			// NPC메뉴 아이템 트렌지 기능

#define		__VTN_TIMELIMIT				// 베트남 플레이 시간 제한
#define		__PCBANG			// PC방 혜택
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
#define		__WING_ITEM
//#define		__USE_SOUND_LIB_FMOD		// use FMod sound lib
	#define		__QUIZ						// 퀴즈 이벤트 시스템

	#define		__2ND_PASSWORD_SYSTEM		// 2차 비밀번호 시스템

	#define		__EVENTLUA_RAIN				// 장마 이벤트 -> 루아로 변경

	#define		__EVENTLUA_SNOW				// 강설 이벤트

	#define		__YS_CHATTING_BLOCKING_SYSTEM	// 캐릭터 채팅 차단 시스템
#if	  defined(__INTERNALSERVER)	// 내부 사무실 테스트서버 

#elif defined(__TESTSERVER)  // 외부 유저 테스트서버 

#elif defined(__MAINSERVER)  // 외부 본섭

//	#define		__RAIN_EVENT				// 장마 이벤트(비오는 동안 경험치 2배)
//	#define		__JAPAN_SAKURA		// 벚꽃 효과

	#define		__NEW_PROFILE		// 프로파일러
#endif	// end - 서버종류별 define 

#ifndef NO_GAMEGUARD
	#define	__NPROTECT_VER	4	
#endif	

#endif // VERSION_COMMON_H
