#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__
#define		__VER 19//15//16//17//18	// 15차
#define __MAINSERVER
#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif
#define __DEBUG_SALT
#define		__SERVER					// 클라이언트 전용코드를 빌드하지 않기 위한 define
#define		__CRC
#define		__SO1014					// 소켓 예외 처리( 캐쉬, 인증, 로그인 )
#define		__PROTOCOL0910
#define		__PROTOCOL1021
#define		__VERIFYNETLIB
#define		__MAP_SIZE					// CServerdesc사용 때문에 
#define		__S8_SERVER_PORT			// 2006월 11월 7일 업데이트 - World, Cache, CoreServer
#define		__STL_0402					// stl

// 11
#define		__MA_VER11_07				// cache 서버에서 loginserver로 유저접속끊낌을 알려주는 부분
//#define		__SYS_PLAYER_DATA
#define		__RT_1025					// 메신저	//

#if (_MSC_VER > 1200)
#define		__VS2003		// 컴파일러변경.net
#endif

//#define		__US_LOGIN_0223


#if	  defined(__INTERNALSERVER)	// 내부 사무실 테스트서버 
	#define		__GUILDVOTE				// 길드 투표 
	#define		__VERIFY_ARCHIVE1129
	#define		__CRASH_0404			// 캐시 서버 크래시 상태 출력
#elif defined(__TESTSERVER)		// 외부 유저 테스트서버 
	#define		__GUILDVOTE				// 길드 투표 
#elif defined(__MAINSERVER)		// 외부 본섭
 
#endif	// end - 서버종류별 define 


#endif
