#ifndef __VERSION_COMMON__
#define __VERSION_COMMON__
#define		__VER	15	// 15차
#define __MAINSERVER
#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#undef		__VER
#define		__VER 19//15//16//17//18

#define __DEBUG_SALT

#define __NEW_TASKBAR_V19
#define __Y_INTERFACE_VER3

#define __NEW_PROP_PARAMETER // new function added called LoadPropItemEx, since both skill and item share the same struct.
#define __3RD_LEGEND16
#define __NEW_ITEM_VARUNA
#define __ENVIRONMENT_EFFECT
#define __FASTJOBCHANGE
#define	__SECURITY_FIXES

#define		__SERVER					// 클라이언트 전용코드를 빌드하지 않기 위한 define
#define		__ITEM_REMOVE_LIST			// 아이템 제거 
#define		__S1108_BACK_END_SYSTEM		// 백앤드시스템 Trans, World, Neuz

#define		__EVENT_1101				// 출첵 이벤트

// 해외 6차 적용
//	#define		__VERIFY_0201

#define		__LANG_1013
//	mulcom	BEGIN100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
//#define		__TRANS_0413				// 트랜스 서버 메로리 최적화
//	mulcom	END100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.

#define		__SKILL_0205				// 스킬 변경 부분만 저장
#define		__STL_0402					// stl

#define		__ITEMDROPRATE				// 아이템 드롭률 조정(이벤트)
#define		__EVENT_1101_2				// 출첵 이벤트 64비트 변경
#define		__GETMAILREALTIME			// 실시간 메일 가지고 오기 Trans
#define		__S_RECOMMEND_EVE			// 추천 이벤트 Neuz, Trans, World
#define		__EXP_ANGELEXP_LOG			// 경험치, 엔젤 경험치 로그 관련 CHARACTER_TBL 컬럼 추가

#define		__RULE_0615					// 명명 규칙 정리
#define		__S_BUG_GC					// 길드대전 신청시 같은 금액을 신청하면 길드아이디로 쇼트 되서 순위가 바뀌는 문제 수정( map -> vector ) 

#define		__PROP_0827					// 아이템 프로퍼티 적용 파라미터 확장

//11th
//#define		__MA_VER11_04				// 길드 창고 로그 기능 world,database
//#define		__MA_VER11_05				// 케릭터 봉인 거래 기능 world,database,neuz
//#define		__CSC_VER11_5				// 태스크바 확장
//#define		__GUILD_COMBAT_1TO1			// 일대일 길드대전

#define		__EVENTLUA_COUPON			// 쿠폰 이벤트
#define		__LOG_PLAYERCOUNT_CHANNEL	// 채널별 동접 로그

//	#define		__SYS_POCKET				// 주머니
//	#define		__SYS_COLLECTING			//
//	#define		__SYS_IDENTIFY				// 각성, 축복
//	#define		__SYS_PLAYER_DATA			// 캐릭터 통합 정보
#define		__RT_1025					// 메신저
#define		__CONV_0906
#define		__INVALID_LOGIN_0320		// 저장이 완료 되지 않은 사용자의 접속 막기(복사 방지)
#define		__INVALID_LOGIN_0612		// 보완
#define		__VENDOR_1106				// 개인상점 제목 허용 문자

#define		__JEFF_9_20		// 비행 시간을 침묵 시간으로 전용
#define		__JEFF_FIX_0

#define	__REMOVE_PLAYER_0221		// 캐릭터 삭제 패킷 조작

#define		__SEND_ITEM_ULTIMATE	// 얼터멋 웨폰도 ITEM_SEND_TBL에서 지급가능하게...

#if (_MSC_VER > 1200)
#define		__VS2003		// 컴파일러변경.net
#endif

#define		__AUTO_NOTICE		// 자동 공지

// 12차
//	#define		__SECRET_ROOM
//	#define		__TAX
//	#define		__LORD
//	#define		__EXT_PIERCING				// 무기 피어싱
//	#define		__MOD_TUTORIAL	// 튜토리얼 개선

// 13차
//	#define		__RAINBOW_RACE		// 레인보우 레이스
//	#define		__HOUSING			// 하우징 시스템

#define		__PET_1024
#define		__BUFF_1107	
//	#define		__HONORABLE_TITLE			// 달인
//	#define		__COUPLE_1117
//	#define		__COUPLE_1202		// 커플 보상
//	#define		__POST_1204		// 우편 삭제 최적화

#define		__OCCUPATION_SHOPITEM	// 점령길드 전용 구매 가능 아이템

#define		__POST_DUP_1209		// 우편 로드 시 무결성 검사

#define		__LAYER_1015	// 레이어 저장	

#define		__FUNNY_COIN			// 퍼니 코인

#define		__REMOVEITEM_POCKET		// 삭제 예약 테이블에 휴대가방 포함..

#define		__PERIN_BUY_BUG				// 페냐 반복구매 버그 확인용 코드

// 14차
//	#define		__INSTANCE_DUNGEON			// 14차 인스턴스 던전 기반
//	#define		__PCBANG				// PC방 혜택

#define		__QUIZ					// 퀴즈 이벤트 시스템

#define		__ERROR_LOG_TO_DB		// 에러 로그 시스템


// 15차
//	#define		__USING_CONTINENT_DATA		// 대륙 경계데이터 외부에서 긁어옴! 

//	#define		__PETVIS					// 15차 비스펫
//	#define		__GUILD_HOUSE				// 15차 길드하우스

//	#define		__HERO129_VER15				// 15차 히어로 레벨확장

//	#define		__IMPROVE_QUEST_INTERFACE	// 15차 퀘스트 시스템
//	#define		__CAMPUS					// 15차 사제 시스템
//	#define		__2ND_PASSWORD_SYSTEM		// 로그인 시 2차 비밀번호 입력

#if	  defined(__INTERNALSERVER)	// 내부 사무실 테스트서버 
//	#define	__LANG_1013
//	#define	__RULE_0615

	#define		__SKILL_0205
	#define		__GUILDVOTE				// 길드 투표 
	#define		__VERIFY_MEMPOOL

//	#define		__S_NEW_SKILL_2			// 스킬 개선 패킷 전송 Neuz, World, Trans
	#define		__Y_MAX_GENERAL_LEVEL_8			// 만랩 120으로 조정... Neuz, World, Trans
//	#define		__INVALID_LOGIN_0320	// 저장이 완료 되지 않은 사용자의 접속 막기(복사 방지)
//	#define		__INVALID_LOGIN_0612	// 보완

//	#define		__LEGEND				//	9차 전승시스템	Neuz, World, Trans
//	#define		__ULTIMATE				// 얼터멋 웨폰

//	#define		__PET_0410				// 9, 10차 펫
//	#define		__AI_0509				// 몬스터 인공지능

//	#define		__LUASCRIPT				// 루아 스크립트 사용 (World, Trans, Neuz)
//	#define		__EVENTLUA				// 이벤트 (루아 스크립트 적용) - World, Trans, Neuz
//	#define		__SKILL_0706			// 대인용 AddSkillProp 컬럼 추가 및 적용

//	#define		__CONV_0906

//	#define		__SYS_POCKET			// 주머니
//	#define		__SYS_COLLECTING	//

//	#define		__SYS_IDENTIFY			// 각성, 축복
//	#define		__SYS_PLAYER_DATA		// 캐릭터 통합 정보
//	#define		__RT_1025				// 메신저

//	#define		__VENDOR_1106			// 개인상점 허용 문자

	#define		__JEFF_11_2		// 수신자 없는 메일 로딩 시 반송

	#define		__VTN_TIMELIMIT

	#define		 __GUILD_HOUSE_MIDDLE	// 길드하우스 중형



#elif defined(__TESTSERVER)		// 외부 유저 테스트서버 

//	#define		__NOTIFYSTART0706		// 런쳐에게 시작 알림 방식을 이벤트로  
	#define		__GUILDVOTE				// 길드 투표 
	#define		__Y_MAX_GENERAL_LEVEL_8			// 만랩 120으로 조정... Neuz, World, Trans
//	#define		__PET_0410				// 9, 10차 펫
//	#define		__AI_0509				// 몬스터 인공지능
//	#define		__LEGEND				//	9차 전승시스템	Neuz, World, Trans
//	#define		__ULTIMATE				// 얼터멋 웨폰
	
//	#define		__LUASCRIPT				// 루아 스크립트 사용 (World, Trans, Neuz)
//	#undef	__VER
//	#define	__VER	11					// 11차

//	#define		__SYS_POCKET			// 주머니
//	#define		__SYS_COLLECTING	//
//	#define		__SYS_IDENTIFY			// 각성, 축복
//	#define		__SYS_PLAYER_DATA		// 캐릭터 통합 정보
//	#define		__RT_1025				// 메신저
//	#define		__CONV_0906
//	#define		__INVALID_LOGIN_0320	// 저장이 완료 되지 않은 사용자의 접속 막기(복사 방지)
//	#define		__INVALID_LOGIN_0612	// 보완
//	#define		__VENDOR_1106			// 개인상점 허용 문자

	#define	__JEFF_11_2		// 수신자 없는 메일 로딩 시 반송

	#define __GUILD_HOUSE_MIDDLE	// 길드하우스 중형


#elif defined(__MAINSERVER)		// 외부 본섭 

	#define		__IDC
//	#define		__TOOMANY_PENDINGLOG1115

#endif	// end - 서버종류별 define 

#endif