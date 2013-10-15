#ifndef __HWOPTION_H__
#define	__HWOPTION_H__

class COption
{
public:
	TCHAR	m_szFileName[MAX_PATH];
//	int		m_nSoundEffect;
	int		m_nResWidth;
	int		m_nResHeight;
	BOOL	m_bStartFullScreen;
//	float	m_fMusicVolume;
	int		m_nTextureQuality;		// 텍스쳐 품질 높음0,중간1,낮음2
	int		m_nViewArea;			// 시야범위 멀리0,중간1,가깝게2
	int		m_nObjectDetail;		// 오브젝트 표현수준 높음0,중간1,낮음2
	int     m_nObjectDistant;       // 오브젝트 거리  높음0,중간1,낮음2
	int     m_nShadow; // 그림자 
	int		m_nToggleScreen;		// 알트엔터 가능/못가능.  이건 유저들은 모른다.
	BOOL    m_bOperator;
	DWORD   m_dwChatFilter[5];
	DWORD   m_dwChatFilterEtc;
	int		m_nBloom;				// 뽀샤시 효과.	
	int		m_nMonName;		// 몬스터 이름 On / Off
	int		m_nPlayerName;		    // 자신 이름 On / Off
	int		m_nOtherPlayerName;		// 다른 플레이어 이름 On / Off
	int		m_nToolTipTexture;		// 툴팁 인터페이스 바꾸기
	int		m_nToolTipText;
#ifdef __SFX_OPT
	int		m_nSfxLevel;	
#endif
	BOOL	m_nWeatherEffect;
//#ifdef __YADDSOUND
	BOOL    m_bVoice;
	float	m_fEffectVolume;
	float	m_fBGMVolume;
//#endif //__YADDSOUND

	// 윈도 옵션
	int     m_nWindowAlpha; // 0 ~ 255
	int     m_nWindowEffect; // 0(일반), 1(반투명), 2(사각)

	// 속어 단어 필터
	int     m_nSlangWord; // 0(교체), 1(숨김)

	// 채팅 명령어
	int     m_nChatCommand; // 0(한글 풀문장),1(한글축약어),2(영문풀문장),3(영문축약어)
	
	// 초보자 도움말(채팅창에 나오는 것)
	int     m_nInstantHelp; // 
	
	// 인터페이스 방식
	int		m_nInterface;	// 0 : 구버전 1 : 신버전(우클릭으로 카메라 회전하는..)
	BOOL	m_bRollEffect;	// 비행시 롤링 효과.
	
	BOOL	m_bSay;			// 귀속말 승인 / 거부
	BOOL	m_bTrade;		// 거래 승인 / 거부
	BOOL	m_bParty;		// 파티 승인 / 거부
	BOOL	m_bShout;		// 외치기 표시 ON / OFF
	BOOL	m_bMessengerJoin; // Messenger 알림 표시 On / Off
	BOOL	m_bMessenger;	// Messenger 승인 / 거부
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	BOOL	m_bBattleBGM;	// 전투 음악 ON / OFF
#endif // __IMPROVE_SYSTEM_VER15
#ifdef __GAME_GRADE_SYSTEM
	BOOL	m_bGameGradeRendering;
#endif // __GAME_GRADE_SYSTEM

	int		m_nSer;		// 이전에 접속했던 서버
	int		m_nMSer;	// 이전에 접속했던 멀티 서버
	
	BOOL    m_bSaveAccount;
	TCHAR   m_szAccount[ 64 ];
	BOOL    m_bNotice;
	BOOL    m_bTip;
	time_t  m_tNoticeTime;
	
	int		m_nGuide1;	
	int		m_nGuide2;	
	int		m_nGuide3;	
#if __VER >= 12 // __MOD_TUTORIAL
	int		m_nTutorialLv;
#endif
#if __VER >= 12 // __UPDATE_OPT
	BOOL	m_bViewMask;
#endif
	BOOL    m_bIsShowReSkillMessage;

	TCHAR	m_IPAddress[32];

#ifdef __YENV
	BOOL	m_bExitChange;
	BOOL	m_bSpecBump;
#endif //__YENV

	BOOL	m_bDamageRender;

	BOOL	m_bVisibleBuffTimeRender;
	
	int		m_MouseSpeed;
	BOOL	m_bSFXRenderOff;	
	BOOL	m_bCameraLock;
	BOOL	m_bAutoAttack;
	int*	m_pGuide;
	
#if __VER >= 8 //__Y_GAMMA_CONTROL_8
	int     m_nOverBright;
	float	m_fGamma;
	float   m_fContrast;
#endif //__Y_GAMMA_CONTROL_8

#if __VER >= 8 //__CSC_VER8_3
	int m_BuffStatusMode;
#endif //__CSC_VER8_3

#if __VER >= 11 // __ADD_ZOOMOPT
	BOOL	m_bZoomLimit;		// 줌 제한 여부
#endif

	COption();
	~COption();
	
	void Init();
	int Load( LPCTSTR szFileName );
	int Save( LPCTSTR szFileName );

#ifdef __OPTION_EX
	int Defalt( LPCTSTR szFileName );
#endif//__OPTION_EX

#ifdef _DEBUG
	int LoadToolTip( LPCTSTR szFileName );
#endif // _DEBUG
	
#if __VER >= 9 // __CSC_VER9_RESOLUTION
	void CheckResolution();
#endif //__CSC_VER9_RESOLUTION
};

extern COption		g_Option;

#endif
