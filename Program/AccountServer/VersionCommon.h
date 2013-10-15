#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__
#define __MAINSERVER
#define		__VER 19//15//16//17//18	// 15차

#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#define __DEBUG_SALT
#define __16_FACKEL
#define		__SERVER				// 클라이언트 전용코드를 빌드하지 않기 위한 define
//#define	__S0114_RELOADPRO		// 캐릭터 DB에서 읽어오기 : Neuz, Trans, login, World, Certifier, Account( ini : SKIP_TRACKING 추가 )
#define		__STL_0402		// stl

#if (_MSC_VER > 1200)
#define		__VS2003		// 컴파일러변경.net
#endif

// 08-01-22
#define	__GPAUTH_01		// 독일/프랑스 통합 인증
#define	__GPAUTH_02		// 독일/프랑스 통합 인증 - 구매
#define	__EUROPE_0514

// 11th
#define __LOG_PLAYERCOUNT_CHANNEL // 채널별 동접 로그 

#define	__REMOVE_PLAYER_0221		// 캐릭터 삭제 패킷 조작

#define __RT_1025

// 14th
//	#define __PCBANG			// PC방 혜택



#if	  defined(__INTERNALSERVER)	// 내부 사무실 테스트서버 
	#define	__SECURITY_0628	// 리소스 버전 인증

#elif defined(__TESTSERVER)		// 외부 유저 테스트서버 

#elif defined(__MAINSERVER)		// 외부 본섭


#endif	// end - 서버종류별 define 

#endif	// __VERSION_COMMON_H__