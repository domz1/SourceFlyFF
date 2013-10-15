// WndArcane.h: interface for the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __RT_1025
#include "rtmessenger.h"
#else	// __RT_1025
#include "messenger.h"
#endif	// __RT_1025
#include "WndWorld.h"
#include "WndBank.h"
#include "WndRepairItem.h"	 // 수리창
#include "WndGuildWarPeaceConfirm.h"
#include "WndGuildWarRequest.h"
#include "WndUpgradebase.h"
#include "WndCommItem.h"
#include "WndPvp.h"

#include "WndPiercing.h"

#ifdef __FASTJOBCHANGE
#include "WndChangeJobEx.h"
#endif//__FASTJOBCHANGE

#ifdef __TARGET_INF_V19
#include "WndMonsterInformation.h"
#endif//__TARGET_INF_V19

#include "WndInvenRemoveItem.h"
#include "WndCollecting.h"

#if __VER >= 11 // __SYS_IDENTIFY
#include "WndAwakening.h"
#include "WndLvReqDown.h"
#include "WndBlessingCancel.h"
#endif

#if __VER >= 12 // __LORD
#include "WndLord.h"
#endif

#include "WndPetRes.h"

#if __VER >= 8 //__CSC_VER8_5
#include "WndSummonAngel.h"
#endif //__CSC_VER8_5

#ifdef __EVE_MINIGAME
#include "WndMiniGame.h"
#endif //__EVE_MINIGAME

#if __VER >= 9 // __CSC_VER9_1
#include "WndPetSys.h"
#endif //__CSC_VER9_1

#if __VER >= 11 // __GUILD_COMBAT_1TO1
#include "WndGuildCombat1to1.h"
#endif //__GUILD_COMBAT_1TO1

#if __VER >= 12 // __SECRET_ROOM
#include "WndSecretRoom.h"
#endif //__SECRET_ROOM
#ifdef __AZRIA_1023
#include "WndSelectCh.h"
#endif
#ifdef __HELP_BUG_FIX
#include "WndHelp.h"
#endif //__HELP_BUG_FIX

#if __VER >= 13 // __RAINBOW_RACE
#include "WndRainbowRace.h"
#endif //__RAINBOW_RACE
#if __VER >= 13 // __HOUSING
#include "WndRoomList.h"
#include "WndQuitRoom.h"
#endif // __HOUSING
#ifdef __QUIZ
#include "WndQuizEvent.h"
#endif // __QUIZ
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
#include "WndQuestQuickInfo.h"
#include "WndQuest.h"
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
#include "WndChattingBlockingList.h"
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#ifdef __GUILD_HOUSE_MIDDLE
#include "WndHousing.h"
#endif //__GUILD_HOUSE_MIDDLE

#ifdef	__COLOSSEUM
#include "WndColosseum.h"
#endif	// __COLOSSEUM

typedef CMap<DWORD, DWORD, void *, void *> CMapDWordToPtr;


#define DECLAREAPPLET( AddMain_Func, AllocClass) CWndBase* AddMain_Func() { return AllocClass; }

class CWndMessage;
class CWndInstantMsg;
class CWndCommand;
class CWndMap;
class CWndPartyChangeName;
class CWndPartyConfirm;
class CWndPartyLeaveConfirm;
class CWndFriendConFirm;
class CWndDuelConfirm;

#if __VER >= 8     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
class CWndDuelResult;
#endif	// __VER >= 8  

class CWndAddFriend;
class CWndCloseExistingConnection;
class CWndMessageNote;
class CWndMessengerNote;
class CWndTextBook;
class CWndTextScroll;
class CWndTextLetter;
class CWndSelectVillage;
 
#define REG_VERSION 1

#include "WndGuild.h"
#include "WndRankGuild.h"
#include "WndRankInfo.h"
#include "WndRankWar.h"
#include "WndGuildBank.h"

#include "WndChangeName.h"

#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
#include "WndSealChar.h"
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#define MAX_SKILL		  16

// chat style
#define CHATSTY_GENERAL   0x0001
#define CHATSTY_WHISPER   0x0002
#define CHATSTY_SHOUT     0x0004
#define CHATSTY_PARTY     0x0008
#define CHATSTY_GUILD     0x0010

#define CHATSTY_HELP          0x0060
#define CHATSTY_HELP_CLIENT   0x0020
#define CHATSTY_HELP_CHAT     0x0040
#define CHATSTY_SYSTEM        0x0180
#define CHATSTY_SYSTEM_CLIENT 0x0080
#define CHATSTY_SYSTEM_CHAT   0x0100
#define CHATSTY_GAME          0x0600
#define CHATSTY_GAME_CLIENT   0x0200
#define CHATSTY_GAME_CHAT     0x0400

#define PS_USE_MACRO     0x00000001
#define PS_NOT_MACRO     0x00000002


typedef struct tagWNDREGINFO
{
	DWORD dwWndId;
	CRect rect;
	BOOL  bOpen;
	DWORD dwVersion;
	DWORD dwWndSize;
	DWORD dwSize;
	BYTE* lpArchive;
} WNDREGINFO,* LPWNDREGINFO;

struct AppletFunc
{
	CWndBase* (*m_pFunc)();
	DWORD m_dwIdApplet;
	LPCTSTR m_pAppletName;
	LPCTSTR m_pAppletDesc;
	LPCTSTR m_pszIconName;
	CHAR m_cHotkey;
};

struct ToolTipItemTextColor
{
	DWORD dwName0;				// 이름 0단계
	DWORD dwName1;				// 이름 1단계
	DWORD dwName2;				// 이름 2단계
	DWORD dwName3;				// 이름 3단계
	DWORD dwName4;				// 이름 4단계
	DWORD dwName5;				// 이름 5단계

/**************************************
              ADEILSON V19
/**************************************/
    DWORD dwBarunaName;
    DWORD dwUltimateName;
    DWORD dwUniqueName;
    DWORD dwArmorSetName;
	DWORD dwWeaGeneralName;
	DWORD dwDestParamName;	
/**************************************/
	DWORD dwGeneral;			// 일반
	DWORD dwPiercing;			// 피어싱
	DWORD dwPlusOption;			// 플러스 옵션
	DWORD dwResist;				// 속성 일반
	DWORD dwResistFire;			// 속성 불
	DWORD dwResistWater;		// 속성 물
	DWORD dwResistEarth;		// 속성 땅
	DWORD dwResistElectricity;	// 속성 전기
	DWORD dwResistWind;			// 속성 바람
	DWORD dwResistSM;			// 속성 상용화 무기
	DWORD dwResistSM1;			// 속성 상용화 방어구
	DWORD dwTime;				// 시간 및 효과
	DWORD dwEffective0;			// 아이템 효능
	DWORD dwEffective1;			// 아이템 효능1
	DWORD dwEffective2;			// 아이템 효능2
	DWORD dwEffective3;			// 아이템 효능3
	DWORD dwRandomOption;		// 랜덤 옵션
	DWORD dwEnchantOption;		// 인첸트 옵션
	DWORD dwSetName;			// 세트 이름
	DWORD dwSetItem0;			// 세트 목록(비착용시)
	DWORD dwSetItem1;			// 세트 목록(착용시)
	DWORD dwSetEffect;			// 세트 효과
	DWORD dwGold;				// 가격
	DWORD dwCommand;			// 설명
	DWORD dwNotUse;				// 사용못함
#if __VER >= 9 // __CSC_VER9_1
	DWORD dwAddedOpt1;		//무기추가옵션1
	DWORD dwAddedOpt2;		//무기추가옵션2
	DWORD dwAddedOpt3;		//무기추가옵션3
	DWORD dwAddedOpt4;		//무기추가옵션4
	DWORD dwAddedOpt5;		//무기추가옵션5
	DWORD dwAddedOpt6;		//무기추가옵션6
	DWORD dwAddedOpt7;		//무기추가옵션7
	DWORD dwAddedOpt8;		//무기추가옵션8
	DWORD dwAddedOpt9;		//무기추가옵션9
#endif //__CSC_VER9_1
#if __VER >= 11 // __SYS_IDENTIFY
	DWORD dwAwakening;	// 아이템 각성
	DWORD dwBlessing;	// 여신의 축복
	DWORD dwBlessingWarning;	// 여신의 축복 경고
#endif	// __SYS_IDENTIFY
};

enum 
{
	FIRST_TC,
	SECOND_TC,
	THIRD_TC,
	MAX_TC
};

#if __VER >= 15 // __GUILD_HOUSE
class CWndGHMainMenu;
class CWndGHUpkeep;
#endif

class CWndMgr : public CWndBase
{ 
	CString m_strChatBackup;
	CTimer m_timerDobe;
#ifdef __BAN_CHATTING_SYSTEM
	int m_nWarningCounter;
	int m_nWarning2Counter;
	CTimer m_timerWarning;
	CTimer m_timerWarning2;
	CTimer m_timerShortcutWarning;
	CTimer m_timerBanning;
	BOOL m_bShortcutCommand;

public:
	enum { BANNING_POINT = 5, BANNING_2_POINT = 2 };
	enum { WARNING_MILLISECOND = 700, WARNING_2_MILLISECOND = 1000, SHORTCUT_WARNING_MILLISECOND = 3000, BANNING_MILLISECOND = 180000 };

public:
	void SetWarningCounter( int nWarningCounter );
	void SetWarning2Counter( int nWarning2Counter );
	int GetWarningCounter( void ) const;
	int GetWarning2Counter( void ) const;
	CTimer& GetWarningTimer( void );
	CTimer& GetWarning2Timer( void );
	CTimer& GetShortcutWarningTimer( void );
	CTimer& GetBanningTimer( void );
	BOOL IsShortcutCommand( void ) const;

private:
#endif // __BAN_CHATTING_SYSTEM

public:
	CStringArray m_aChatString;
	CUIntArray   m_aChatColor ;
	CUIntArray   m_aChatStyle ;
#ifdef __RT_1025
	CRTMessenger	m_RTMessenger;
#else	//__RT_1025
	CMessenger m_Messenger;
#endif	// __RT_1025
	CMapStringToPtr m_mapMap;
	CMapStringToPtr m_mapInstantMsg;
	CMapStringToPtr m_mapMessage;
	CEditString m_ChatString;

	BOOL m_bAutoRun;
	
	DWORD m_dwSavePlayerTime;
	
	TCHAR m_szTimerChat[ 128 ];
	CTimer m_timerChat;
	
	BOOL   m_bConnect;
	CTimer m_timerMessenger;
	ToolTipItemTextColor	dwItemColor[MAX_TC];

	CPtrArray	m_awndShortCut;
	BOOL	m_bTitle        ;
	CMapDWordToPtr	m_mapAppletFunc ;
#if __VER >= 8 // __CSC_VER8_1
	list<int> m_tempWndId;
	BOOL m_clearFlag;
#endif //__CSC_VER8_1

	AppletFunc*		GetAppletFunc( DWORD dwIdApplet );
	void	AddAllApplet();
	BOOL ScreenCapture();
	BOOL SaveBitmap( LPCTSTR lpszName );
#if __VER >= 11 // __CAPTURE_JPG
	BOOL SaveJPG( LPCTSTR lpszName );
#endif
	void InitSetItemTextColor();
	void Free();
	void SetMessengerAutoState();

public:
	DWORD		   m_dwSkillTime[MAX_SKILL];
	
	CWndBeautyShop* m_pWndBeautyShop;
#if __VER >= 8 //__CSC_VER8_4
	CWndFaceShop* m_pWndFaceShop;
#ifdef __NEWYEARDAY_EVENT_COUPON
	CWndUseCouponConfirm* m_pWndUseCouponConfirm;
#endif //__NEWYEARDAY_EVENT_COUPON

#endif //__CSC_VER8_4
#if __VER >= 8 //__CSC_VER8_5
	CWndSummonAngel* m_pWndSummonAngel;
#endif //__CSC_VER8_5
	CWndDialog*    m_pWndDialog;
	CWndShop*      m_pWndShop;
	CWndBase*      m_pWndActiveDesktop;
	CWndTrade*     m_pWndTrade;
	CWndTradeGold* m_pWndTradeGold;
	CWndDropItem*  m_pWndDropItem;
	CWndConfirmTrade * m_pWndConfirmTrade;
	CWndTradeConfirm * m_pWndTradeConfirm;
	CWndQuickList* m_pWndQuickList;
	CWndCommand*   m_pWndCommand;
	CWndBank*	   m_pWndBank;
	CWndWorld*     m_pWndWorld;
#ifdef __JEFF_11
	CWndPetRes*    m_pPetRes;
#endif
#ifdef __EVE_MINIGAME
	CWndKawiBawiBoGame* m_pWndKawiBawiBoGame;
	CWndKawiBawiBoGameConfirm* m_pWndKawiBawiBoGameConfirm;
	CWndFindWordGame* m_pWndFindWordGame;
	CWndDiceGame* m_pWndDiceGame;
	CWndPuzzleGame* m_pWndPuzzleGame;
#endif //__EVE_MINIGAME
	
#if __VER >= 9 // __CSC_VER9_1
	CWndMixJewel* m_pWndSmeltMixJewel;
	CWndSmeltJewel* m_pWndSmeltJewel;
	CWndExtraction* m_pWndExtraction;
	CWndChangeWeapon* m_pWndChangeWeapon;
	CWndPetStatus* m_pWndPetStatus;

#ifdef __PETFILTER
	CWndPetfilter* m_pWndPetfilter;
#endif //__PETFILTER

	CWndFoodConfirm* m_pWndFoodConfirm;
	CWndPetMiracle* m_pWndPetMiracle;
	CWndPetFoodMill* m_pWndPetFoodMill;
	CWndPetLifeConfirm* m_pWndPetLifeConfirm;
#if __VER >= 10 // __CSC_VER9_1 -> __LEGEND
	CWndHeroSkillUp* m_pWndHeroSkillUp;
#endif //__CSC_VER9_1 -> __LEGEND
#endif //__CSC_VER9_1
#if __VER >= 11 // __PIERCING_REMOVE
	CWndRemovePiercing* m_pWndRemovePiercing;
#endif //__PIERCING_REMOVE
#if __VER >= 12 // __CSC_VER12_4
	CWndRemoveJewel* m_pWndRemoveJewel;
#endif //__CSC_VER12_4
#if __VER >= 12 // __CSC_VER12_5
	CWndPetTransEggs* m_pWndPetTransEggs;
#endif //__CSC_VER12_5
#ifdef __TRADESYS
	CWndDialogEvent* m_pWndDialogEvent;
#endif //__TRADESYS
#if __VER >= 12 // __HEAVEN_TOWER
	CWndHeavenTower* m_pWndHeavenTower;
#endif //__HEAVEN_TOWER
#if __VER >= 8 //__Y_CHAT_SYSTEM_8
	CWndChatLog* m_pWndChatLog;
#endif //__Y_CHAT_SYSTEM_8

#if __VER >= 10 // __REMOVE_ATTRIBUTE
	CWndRemoveAttribute* m_pWndRemoveAttribute;
#endif //__REMOVE_ATTRIBUTE
	CWndPost* m_pWndPost;
	CWndQueryEquip* m_pWndQueryEquip;
	CWndSelectVillage*		m_pWndSelectVillage;
	CWndQuestItemWarning* m_pWndQuestItemWarning;
	CWndChangeName*		m_pWndChangeName;
	CWndDropConfirm*  m_pWndDropConfirm;
	CWndUpgradeBase*  m_pWndUpgradeBase;
	CWndPiercing*	  m_pWndPiercing;

#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	CWndSealChar*		m_pWndSealChar;
	CWndSealCharSelect*		m_pWndSealCharSelect;
	CWndSealCharSend*		m_pWndSealCharSend;
	CWndSealCharSet*		m_pWndSealCharSet;
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

	CWndInvenRemoveItem* m_pWndInvenRemoveItem;

	CWndCommercialElem* m_pWndCommerialElem;
	CWndRemoveElem* m_pRemoveElem;
	CWndRepairItem* m_pWndRepairItem;
	CWndConfirmBank* m_pWndConfirmBank;
	CWndBankPassword* m_pWndBankPassword;
	CWndPenaltyPK* m_pWndPenaltyPK;
	CWndRankGuild* m_pWndRankGuild;
	CWndRankInfo* m_pWndRankInfo;
	CWndRankWar* m_pWndRankWar;
	CWndGuildBank*	m_pWndGuildBank;
#ifdef __FUNNY_COIN
	CWndFunnyCoinConfirm* m_pFunnyCoinConfirm;
#endif //__FUNNY_COIN

#ifdef __MAIL_REQUESTING_BOX
	BOOL	m_bWaitRequestMail;
#endif

#if __VER >= 11 // __GUILD_BANK_LOG
	CWndGuildBankLog* m_pWndGuildBankLog;
#endif //__GUILD_BANK_LOG
	CWndGuildConfirm* m_pWndGuildConfirm;
	CWndGuildWarPeaceConfirm* m_pWndGuildWarPeaceConfirm;
	CWndGuildWarRequest* m_pWndGuildWarRequest;
	CWndGuildNickName* m_pWndGuildNickName;
	CWndGuildCombatRank_Person*   m_pWndGuildCombatRanking;
	CWndGuildCombatState*		  m_pWndGuildWarState;	
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	CWndGuildCombat1to1Offer*		  m_pWndGuildCombatOffer;
#else //__GUILD_COMBAT_1TO1
	CWndGuildCombatOffer*		  m_pWndGuildCombatOffer;
#endif //__GUILD_COMBAT_1TO1
	CWndGuildCombatSelection*	  m_pWndGuildCombatSelection;
	CWndGuildCombatJoinSelection* m_pWndGuildCombatJoinSelection;
	CWndGuildCombatBoard*		  m_pWndGuildCombatBoard;
	CGuildCombatInfoMessageBox2*  m_pWndGuildCombatInfoMessageBox2;
	CGuildCombatInfoMessageBox*   m_pWndGuildCombatInfoMessageBox;
	CWndGuildCombatResult*		  n_pWndGuildCombatResult;

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	CWndGuildCombat1to1Selection*	m_pWndGuildCombat1to1Selection;
	CWndGuildCombat1to1Offer*		m_pWndGuildCombat1to1Offer;
#endif //__GUILD_COMBAT_1TO1
	CCollectingWnd*				  m_pWndCollecting;
#if __VER >= 11 // __SYS_IDENTIFY
	CWndLvReqDown*				  m_pWndLvReqDown;
	CWndBlessingCancel*			  m_pWndBlessingCancel;
	CWndAwakening*				  m_pWndAwakening;
#endif
#if __VER >= 12 // __LORD
	CWndLordTender*				m_pWndLordTender;
	CWndLordVote*				m_pWndLordVote;
	CWndLordState*				m_pWndLordState;
	CWndLordEvent*				m_pWndLordEvent;
	CWndLordSkillConfirm*		m_pWndLordSkillConfirm;
	CWndLordInfo*				m_pWndLordInfo;
	CWndLordRPInfo*				m_pWndLordRPInfo;
	bool						m_bIsOpenLordSkill;
#endif
#if __VER >= 12 // __SECRET_ROOM
	CWndSecretRoomInfoMsgBox*		m_pWndSecretRoomMsg;
	CWndSecretRoomSelection*		m_pWndSecretRoomSelection;
	CWndSecretRoomOffer*			m_pWndSecretRoomOffer;
	CWndSecretRoomOfferState*		m_pWndSecretRoomOfferState;
	CWndSecretRoomChangeTaxRate*	m_pWndSecretRoomChangeTaxRate;
	CWndSecretRoomCheckTaxRate*		m_pWndSecretRoomCheckTaxRate;
	CWndSecretRoomBoard*			m_pWndSecretRoomBoard;
	CWndSecretRoomQuick*			m_pWndSecretRoomQuick;
#endif //__SECRET_ROOM
#if __VER >= 12 // __PET_0519
	CWndPetAwakCancel*				m_pWndPetAwakCancel;
#endif
#ifdef __AZRIA_1023
	CWndSelectCh*					m_pWndSelectCh;
#endif
#if __VER >= 13 // __RAINBOW_RACE
	CWndRainbowRaceOffer*			m_pWndRainbowRaceOffer;
	CWndRainbowRaceInfo*			m_pWndRainbowRaceInfo;
	CWndRainbowRaceRule*			m_pWndRainbowRaceRule;
	CWndRainbowRaceRanking*			m_pWndRainbowRaceRanking;
	CWndRainbowRacePrize*			m_pWndRainbowRacePrize;
	CWndRainbowRaceMiniGameButton*	m_pWndRainbowRaceMiniGameButton;
	CWndRainbowRaceMiniGame*		m_pWndRainbowRaceMiniGame;
	CWndRainbowRaceMiniGameEnd*		m_pWndRainbowRaceMiniGameEnd;
	CWndRRMiniGameKawiBawiBo*		m_pWndRRMiniGameKawiBawiBo;
	CWndRRMiniGameDice*				m_pWndRRMiniGameDice;
	CWndRRMiniGameArithmetic*		m_pWndRRMiniGameArithmetic;
	CWndRRMiniGameStopWatch*		m_pWndRRMiniGameStopWatch;
	CWndRRMiniGameTyping*			m_pWndRRMiniGameTyping;
	CWndRRMiniGameCard*				m_pWndRRMiniGameCard;
	CWndRRMiniGameLadder*			m_pWndRRMiniGameLadder;
#endif //__RAINBOW_RACE
#if __VER >= 13 // __CSC_VER13_2
	CWndCoupleMessage*				m_pWndCoupleMessage;
#endif //__CSC_VER13_2
#if __VER >= 13 // __EXT_ENCHANT
	CWndChangeAttribute*			m_pWndChangeAttribute;
#endif //__EXT_ENCHANT

	CWndReSkillWarning*			  m_pWndReSkillWarning;	

#ifdef __FASTJOBCHANGE
	CWndJobChangeEx*			m_pJobChangeEx;
#endif//__FASTJOBCHANGE

	CWndFontEdit*				  m_pWndFontEdit;

	CWndPartyChangeName* m_pWndPartyChangeName;
	CWndPartyConfirm* m_pWndPartyConfirm;
	
	CWndMessageNote* m_pWndMessageNote;
	CWndMessengerNote* m_pWndMessengerNote;

	CWndPartyLeaveConfirm * m_pWndPartyLeaveConfirm;
	CWndFriendConFirm* m_pWndFriendConfirm;
	CWndDuelConfirm* m_pWndDuelConfirm;

#if __VER >= 8     // 8차 듀얼존에 관계없이 PVP가능하게함   Neuz, World
	CWndDuelResult* m_pWndDuelResult;
#endif	// __VER >= 8  

	CWndAddFriend* m_pWndAddFriend;
	CWndStateConfirm* m_pWndStateConfirm;
	CWndCloseExistingConnection*	m_pWndCloseExistingConnection;
	
	CWndRandomScrollConfirm*		m_pWndRandomScrollConfirm;

#ifdef __HELP_BUG_FIX
	CWndHelpInstant* m_pWndHelpInstant;
#endif //__HELP_BUG_FIX
#if __VER >= 13 // __HOUSING
	CWndRoomList*				m_pWndRoomList;
	CWndQuitRoom*				m_pWndQuitRoom;
#endif // __HOUSING
#ifdef __PET_1024
	CWndChangePetName*				m_pWndChangePetName;
#endif
#ifdef __S_SERVER_UNIFY
	BOOL							m_bAllAction;
#endif // __S_SERVER_UNIFY

#if __VER >= 14 // __SMELT_SAFETY
	CWndSmeltSafety* m_pWndSmeltSafety;
	CWndSmeltSafetyConfirm* m_pWndSmeltSafetyConfirm;
#endif //__SMELT_SAFETY

#if __VER >= 14 // __EQUIP_BIND
	CWndEquipBindConfirm* m_pWndEquipBindConfirm;
#endif // __EQUIP_BIND

#if __VER >= 14 // __RESTATE_CONFIRM
	CWndRestateConfirm* m_pWndRestateConfirm;
#endif // __RESTATE_CONFIRM

#ifdef __QUIZ
	CWndQuizEventConfirm* m_pWndQuizEventConfirm;
	CWndQuizEventQuestionOX* m_pWndQuizEventQuestionOX;
	CWndQuizEventQuestion4C* m_pWndQuizEventQuestion4C;
	CWndQuizEventButton* m_pWndQuizEventButton;
#endif // __QUIZ

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CWndQuestQuickInfo* m_pWndQuestQuickInfo;
	CWndQuestDetail* m_pWndQuestDetail;
#endif // __IMPROVE_QUEST_INTERFACE

#if __VER >= 15 // __CAMPUS
	CWndCampusInvitationConfirm* m_pWndCampusInvitationConfirm;
	CWndCampusSeveranceConfirm* m_pWndCampusSeveranceConfirm;
#endif // __CAMPUS

#if __VER >= 15 // __PETVIS
	CWndBuffPetStatus* m_pWndBuffPetStatus;
	CWndConfirmVis* m_pWndConfirmVis;
#endif

#if __VER >= 15 // __GUILD_HOUSE
	CWndGHMainMenu* m_pWndGHMain;
	CWndGHUpkeep* m_pWndUpkeep;
#endif

#ifdef __PROTECT_AWAKE
	CWndSelectAwakeCase* m_pWndSelectAwakeCase;
#endif 

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
	CWndChattingBlockingList* m_pWndChattingBlockingList;
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#ifdef __GUILD_HOUSE_MIDDLE
	CWndGuildHouseBid* m_pWndGHBid;
#endif //__GUILD_HOUSE_MIDDLE

#ifdef __NEW_ITEM_VARUNA
	CWndBarunaNPC* m_pWndBarunaNPC;
	CWndBarunaEnchant* m_pWndBarunaEnchant;
	CWndBarunaExtractOffer* m_pWndBarunaExtractOffer;
	CWndBarunaOffer* m_pWndBarunaOffer;
	CWndBarunaSeed* m_pWndBarunaSeed;
	CWndBarunaOfferSeed* m_pWndBarunaOfferSeed;
#ifdef __BARUNA_PIERCING
	CWndBarunaPiercing* m_pWndBarunaPiercing;
#endif // __BARUNA_PIERCING

#endif // __NEW_ITEM_VARUNA
#ifdef __FASHION_COMBINE
	CWndFashionCombine* m_pWndFashionCombine;
#endif // __FASHION_COMBINE

	// 인터페이스 텍스춰
	CTexturePack  m_texture;
	CTexturePack  m_texCommand;
	CTexturePack  m_texIcon;
	CTexturePack  m_texWnd;

	// 메지시 윈도
	CWndMessageBox*		m_pWndMessageBox;
	CWndMessageBoxUpper*		m_pWndMessageBoxUpper;

	//	퀘스트 아이템 정보
	CWndQuestItemInfo*	m_pQuestItemInfo;

#ifdef	__COLOSSEUM
	CWndColosseumComplete*	m_pColosseumComplete;
	CWndColosseumEnter*		m_pColosseumEnter;
	CWndColosseumMassage*	m_pColosseumMassage;
	CWndColosseumRankGuild*	m_pColosseumRankGuild;
	CWndColosseumRankNomal*	m_pColosseumRankNomal;
	CWndColosseumRetry*		m_pColosseumRetry;
	CWndColosseumStart*		m_pColosseumStart;
	CWndColosseumStartReady1*	m_pColosseumStartReady1;
	CWndColosseumStartReady2*	m_pColosseumStartReady2;
#endif // __COLOSSEUM

#ifdef __TARGET_INF_V19
	CWndMonsterInformation* m_pWndMonInfo;
#endif//__TARGET_INF_V19

#ifdef __TREASUREBOX
	CWndTreasureBox* m_pWndTreasureBox;
	CWndTreasureRecieve* m_pWndTreasureRecieve;
#endif

	void OpenQuestItemInfo(CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL);
	void ChangeQuestItemInfo( CItemBase* pItemBase = NULL );

	CWndTextBook* m_pWndTextBook;
	CWndTextScroll* m_pWndTextScroll;
	CWndTextLetter* m_pWndTextLetter;
	void OpenTextBook     ( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL);
	void OpenTextScroll   ( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL);
	void OpenTextLetter   ( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL);
	void ChangeTextBook   ( CItemBase* pItemBase = NULL );
	void ChangeTextScroll ( CItemBase* pItemBase = NULL );
	void ChangeTextLetter ( CItemBase* pItemBase = NULL );
	// Field
	CWndTaskBar*	m_pWndTaskBar;
#ifdef __NEW_TASKBAR_V19
	CWndTaskBar_HUD*	     m_pWndTaskBar_HUD;
	CWndTaskBar_HUD_EX1*	 m_pWndTaskBar_HUD_EX1;
	CWndTaskBar_HUD_EX2*	 m_pWndTaskBar_HUD_EX2;
	CWndTaskBar_HUD_EX3*	 m_pWndTaskBar_HUD_EX3;
	CWndTaskBar_HUD_SLIDE*   m_pWndTaskBar_HUD_SLIDE;
	CWndNewSkillSlot*        m_pWndNewSkillSlot;
#endif//__NEW_TASKBAR_V19

	CWndTaskMenu*	m_pWndMenu;
	CWndRevival*    m_pWndRevival;
	CWndResurrectionConfirm* m_pWndResurrectionConfirm;
//	CWndRecoverChao*    m_pWndRecoverChao;
	CWndReWanted*		m_pReWanted;
	CWndWanted*			m_pWanted;
	CWndLogOutWaitting* m_pLogOutWaitting;
	CWndCommItemDlg*	m_pWndCommItemDlg;
	CWndChangeClass1*	m_pWndChangeClass1;
	CWndChangeClass2*	m_pWndChangeClass2;

	CMapDWordToPtr	m_mapWndApplet ;
	CMapDWordToPtr  m_mapWndRegInfo;

	void PutDestParam( DWORD dwDst1, DWORD dwDst2, DWORD dwAdj1, DWORD dwAdj2, CEditString &str );	
	void PutDefinedString( DWORD dwText, ... );

	void PutString( LPCTSTR lpszString, CObj* pObj = NULL, DWORD dwColor = 0xffffffff, DWORD dwChatStyle = CHATSTY_GAME, DWORD dwPStyle = 0x00000001 );
	void ParsingChat( CString string );

	void WordChange( CString& rString );

	BOOL	IsTitle()	{	return m_bTitle;	}

	void	OpenTitle( BOOL bFirstTime = FALSE );
	void	OpenField();

	BOOL	OpenTaskBar();
	BOOL	OpenMenu();

	void	ObjectExecutor( DWORD dwShortcut, DWORD dwId, DWORD dwType = 0 );
	void	ObjectExecutor( LPSHORTCUT lpShortcut );
	void	UseSkillShortCut( DWORD dwType, DWORD dwSkillIdx );
		
	// map
	CWndMap* OpenMap( LPCTSTR lpszMapFileName );
	CCollectingWnd* OpenCollecting();
	BOOL CloseCollecting();

	// massenger
	CWndMessage* GetMessage( LPCTSTR lpszFrom );
	CWndMessage* OpenMessage( LPCTSTR lpszFrom );
	BOOL	UpdateMessage( LPCTSTR pszOld, LPCTSTR pszNew );
	CWndInstantMsg* GetInstantMsg( LPCTSTR lpszFrom );
	CWndInstantMsg* OpenInstantMsg( LPCTSTR lpszFrom );

	// MessageBox
	BOOL	OpenCustomBox( LPCTSTR strMessage, CWndMessageBox* pWndMessageBox, CWndBase* pWndParent = NULL );
	BOOL	OpenMessageBox( LPCTSTR strMessage, UINT nType = MB_OK, CWndBase* pWndParent = NULL );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	BOOL	OpenMessageBoxWithTitle( LPCTSTR lpszMessage, const CString& strTitle, UINT nType = MB_OK, CWndBase* pWndParent = NULL );
#endif // __IMPROVE_QUEST_INTERFACE
	BOOL    OpenMessageBoxUpper( LPCTSTR lpszMessage, UINT nType = MB_OK, BOOL bPostLogoutMsg = FALSE );
		
	void	CloseMessageBox();

	// tooltip
	void PutToolTip_Troupe( DWORD dwSkill, CPoint point, CRect* pRect );
	void PutToolTip_Skill( DWORD dwSkill, DWORD dwLevel, CPoint point, CRect* pRect, BOOL bActive = TRUE );
	void PutToolTip_Item( DWORD dwType, DWORD dwId, CPoint point, CRect* pRect, int flag = 0 );
	void PutToolTip_Item( CItemBase* pItemBase, CPoint point, CRect* pRect, int flag = 0 );
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	enum { TOOL_TIP_SWITCH_MAIN = 0, TOOL_TIP_SWITCH_SUB1 = 1, TOOL_TIP_SWITCH_SUB2 = 2 };
	void PutToolTipItemAndSubToolTip( CItemBase* pItemBase, CPoint point, CRect* pRect, int nFlag = 0 );
	void PutToolTipParts( CItemBase* pPartsItemBase, CPoint point, CRect* pRect, const int nFlag, const int nSubToolTipFlag );
	void MakeToolTipText( CItemBase* pItemBase, CEditString& strEdit, int flag = 0 );
#endif // __IMPROVE_SYSTEM_VER15
	void PutToolTip_Character( int SelectCharacter, CPoint point, CRect* pRect );

	DWORD PutItemName( CItemElem* pItemElem, CEditString* pEdit );
	void PutItemAbilityPiercing( CItemElem* pItemElem, CEditString* pEdit, DWORD dwColorBuf );
	void PutItemResist( CItemElem* pItemElem, CEditString* pEdit );
	void PutItemSpeed( CItemElem* pItemElem, CEditString* pEdit );
	void PutItemMinMax( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutRandomOpt( CItemElem* pItemElem, CEditString* pEdit );
#if __VER >= 11 // __SYS_IDENTIFY
	void	PutAwakeningBlessing( CItemElem* pItemElem, CEditString* pEdit );
#endif	// __SYS_IDENTIFY
	void PutPiercingOpt( CItemElem* pItemElem, CEditString* pEdit );
	void PutEnchantOpt( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit, int flag );
	void PutSetItemOpt( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutBaseItemOpt( CItemElem* pItemElem, CEditString* pEdit );
	void PutMedicine( CItemElem* pItemElem, DWORD dwParam, LONG nParamVal, CEditString* pEdit );
	void PutBaseResist( CItemElem* pItemElem, CEditString* pEdit );
	void PutItemGold( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit, int flag );
	void PutCoolTime( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutKeepTime( CItemElem* pItemElem, CEditString* pEdit );
	void PutCommand( CItemElem* pItemElem, CEditString* pEdit );
	void PutEndurance( CItemElem* pItemElem, CEditString* pEdit, int flag );
	void PutSex( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutJob( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutLevel( CMover* pMover, CItemElem* pItemElem, CEditString* pEdit );
	void PutWeapon( CItemElem* pItemElem, CEditString* pEdit );
#if __VER >= 9 // __CSC_VER9_1
	void PutAddedOpt( CItemElem* pItemElem, CEditString* pEdit );
	void PutPetInfo( CItemElem* pItemElem, CEditString* pEdit );
	void PutPetFeedPocket( CItemElem* pItemElem, CEditString* pEdit );
#endif //__CSC_VER9_1

#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	void	PutSealChar( CItemElem* pItemElem, CEditString* pEdit );
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

#if __VER >= 15 // __PETVIS
	void PutNeededVis( CItemElem* pItemElem, CEditString* pEdit );
	void PutVisPetInfo( CItemElem* pItemElem, CEditString* pEdit );
	void PutPetKind( CItemElem* pItemElem, CEditString* pEdit );
	BOOL CheckConfirm( CItemBase* pItem );
#endif // __PETVIS

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	void PutEquipItemText( CEditString* pEdit );
#endif // __IMPROVE_SYSTEM_VER15

	CWndMgr();
	virtual	~CWndMgr();

	void	DestroyApplet();
	void	AlighWindow( CRect rcOld, CRect rcNew );

	//virtual CItem* GetFocusItem();
	virtual	BOOL    Process();
	virtual	BOOL	Initialize(CWndBase* pWndParent = NULL);
	virtual	void	OnInitialUpdate();
	virtual	void	OnDestroyChildWnd(CWndBase* pWndChild);
	virtual	void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void	OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	BOOL	OnCommand(UINT nID,DWORD dwMessage);
	virtual	void	OnSize(UINT nType, int cx, int cy);
	virtual BOOL    OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual	BOOL	OnEraseBkgnd(C2DRender* p2DRender);
	virtual	BOOL	OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);

	DWORD	GetAppletId( TCHAR* lpszAppletName );
	CWndBase* GetApplet( DWORD dwIdApplet );
	CWndBase* CreateApplet( DWORD dwIdApplet );
	void	AddAppletFunc( LPVOID pAppletFunc, DWORD dwIdApplet, LPCTSTR lpszKeyword, LPCTSTR pszIconName, LPCTSTR lpszAppletDesc = NULL, CHAR cHotkey = 0 );

	HRESULT	RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	
//	Initialize player
	void SetPlayer( CMover* pMover );

	BOOL LoadRegInfo( LPCTSTR lpszFileName );
	BOOL SaveRegInfo( LPCTSTR lpszFileName );
	LPWNDREGINFO GetRegInfo( DWORD dwWndId );
	BOOL PutRegInfo( DWORD dwWndId, CRect rect, BOOL bOpen );
	BOOL PutRegInfo( LPWNDREGINFO lpRegInfo );
	BOOL PutRegInfo( CWndNeuz* pWndNeuz, BOOL bOpen );


	CWndBase* GetWndVendorBase( void );

#if __VER >= 8 //__CSC_VER8_1
	void ClearAllWnd();
	void RestoreWnd();
#endif //__CSC_VER8_1

#if __VER >= 12 // __CSC_VER12_4
	DWORD GetDSTStringId(int nDstParam);
	BOOL IsDstRate(int nDstParam);
#endif //__CSC_VER12_4

#ifdef __WINDOW_INTERFACE_BUG
	void CloseBoundWindow(void); // 아이템이 걸려 있거나, 아이템을 조작할 가능성이 있는 모든 창을 강제로 닫는다.
#endif // __WINDOW_INTERFACE_BUG

	friend CWndApplet;
protected:
	void __HotKeyChange( DWORD dwId, char* pch );
	void	FormalizeChatString( CString & str );

};

#ifdef __VS2003
extern CPtrArray      m_wndOrder;
#endif

extern CWndMgr          g_WndMng; // 윈도 매니저 클래스 

extern void RenderEnchant( C2DRender* p2DRender, CPoint pt );
extern void RenderRadar( C2DRender* p2DRender, CPoint pt, DWORD dwValue, DWORD dwDivisor );
#define WID_MESSAGEBOX        10

#endif // !defined(AFX_WNDMANAGER_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
