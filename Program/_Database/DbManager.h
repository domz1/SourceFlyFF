/********************************************************************
	created:	2003/02/26
	created:	26:2:2003   16:49
	filename: 	c:\neurospace\program\_database\dbmanager.h
	file path:	c:\neurospace\program\_database
	file base:	dbmanager
	file ext:	h
	author:		Kim, pan-young
	
	purpose:	데이타 베이스 처리 클래스
*********************************************************************/

#ifndef _DB_MANAGER_H_
#define _DB_MANAGER_H_

#include "Query.h"
#include "DPlay.h"
#include "Ar.h"
#include "DPMng.h"
#include "mempooler.h"
#include "misc.h"
#include "..\_Network\Objects\Obj.h"
#include "AccountCacheMgr.h"

#ifdef __TRANS_0413
	const int	MAX_GETTHREAD_SIZE		= 8;
	const int	MAX_PUTTHREAD_SIZE		= 16;
#else	// __TRANS_0413
	//	mulcom	BEGIN100415
// 	const int	MAX_GETTHREAD_SIZE		= 4;
// 	const int	MAX_PUTTHREAD_SIZE		= 4;
	const int	MAX_GETTHREAD_SIZE		= 8;
	const int	MAX_PUTTHREAD_SIZE		= 16;
	//	mulcom	END100415
#endif	// __TRANS_0413
	const int	MAX_UPDATETHREAD_SIZE	= 8;

const int	MAX_JOIN_SIZE			= 16;
const int	MAX_TASKBAR        = MAX_SHORTCUT_STRING * 2 + 44; 
const int	MAX_APPLETTASKBAR	 = MAX_TASKBAR * MAX_SLOT_APPLET; 
const int	MAX_ITEMTASKBAR    = MAX_TASKBAR * MAX_SLOT_ITEM_COUNT * MAX_SLOT_ITEM;   
const char	NullStr[2] = "$";

#define	QUERY_SIZE	40960

enum QUERYMODE
{ 
	DB_CONNECT, DB_THREAD_END, CREATEACCOUNT, CREATEPLAYER, LOGIN, 
	REMOVEPLAYER, SEL_ECT, SENDPLAYERLIST, JOIN, SAVEPLAYER, SAVECONCURRENTUSERNUMBER,
	SAVE_PLAY_TIME, LOG_PLAY_CONNECT, LOG_ITEM, LOG_PLAY_DEATH, LOG_LEVELUP, 
	LOG_SERVER_DEATH, LOG_UNIQUEITEM, LOG_QUEST,
	CREATE_GUILD, DESTROY_GUILD, ADD_GUILD_MEMBER, REMOVE_GUILD_MEMBER,
	UPDATE_GUILD_LOGO, UPDATE_GUILD_CONTRIBUTION, UPDATE_GUILD_NOTICE,
	UPDATE_GUILD_MEMBER_LEVEL, UPDATE_GUILD_AUTHORITY, UPDATE_GUILD_PENYA, GUILD_SETNAME,
	UPDATE_GUILD_REALPAY, GUILD_BANK_UPDAATE, GUILD_BANK_QUERY, GUILD_RANKING_UPDATE, GUILD_RANKING_UPDATE_DB,
	ADD_GUILD_VOTE, REMOVE_GUILD_VOTE, CLOSE_GUILD_VOTE, CAST_GUILD_VOTE, 
	LOG_GUILD_CREATE, LOG_GUILD_REMOVE,
	ACPT_GUILD_WAR, WAR_END, SURRENDER, WAR_DEAD, WAR_MASTER_ABSENT,
	REMOVE_FRIEND, ADD_PARTYNAME, CHANGE_BANKPASS, GAMEMA_CHAT,
	INSERT_FREQUENCY,
	LOG_PK_PVP,
	INSERT_TAG,
	SCHOOL_REPORT,
	UPDATE_GUILD_CLASS, UPDATE_GUILD_NICKNAME,
	LOG_SCHOOL,
	QM_QUERYGUILDQUEST,
	QM_INSERTGUILDQUEST,
	QM_UPDATEGUILDQUEST,
	BS_TEST,
	QM_DELETEGUILDQUEST,
	WANTED_OP,
	QM_SET_PLAYER_NAME,
	QM_SNOOPGUILD,
	START_GUILDCOMBAT,
	IN_GUILDCOMBAT,
	OUT_GUILDCOMBAT,
	RESULT_GUILDCOMBAT,
	GETPENYAGUILD_GUILDCOMBAT,
	GETPENYAPLAYER_GUILDCOMBAT,
	CONTINUE_GUILDCOMBAT,
	ADD_MAIL,
	REMOVE_MAIL,
	REMOVE_MAIL_ITEM,
	REMOVE_MAIL_GOLD,
	READ_MAIL,
	QM_UPDATEGUILDMASTER,
	QM_DELETE_REMOVE_GUILD_BANK_TBL,
	QM_LOG_EXPBOX,
	QM_CALL_USPLOGGINGQUEST,
	QM_CALL_XXX_MULTI_SERVER,
	LOG_SKILLPOINT,
#ifdef __S_NEW_SKILL_2
	SAVE_SKILL,
#endif // __S_NEW_SKILL_2
	QM_CALL_USP_PET_LOG,
#ifdef __S_RECOMMEND_EVE
	RECOMMEND_EVE,
#endif // __S_RECOMMEND_EVE
#ifdef __GETMAILREALTIME
	QM_GETMAIL_REALTIME,
#endif // __GETMAILREALTIME
#ifdef __RT_1025
	QM_ADD_MESSENGER,
	QM_DELETE_MESSENGER,
	QM_UPDATE_MESSENGER,
#endif	// __RT_1025
#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
	QM_GUILDBANK_LOG_VIEW,
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	LOG_GETHONORTIME,
#endif	// __HONORABLE_TITLE			// 달인
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	QM_SEALCHAR,
	QM_SEALCHARCONM,
	QM_SEALCHARGET,
	QM_SEALCHARSET,
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
#if __VER >= 11 // __GUILD_COMBAT_1TO1
	GC1TO1_STATE,
	GC1TO1_LOAD,
	GC1TO1_TENDER,
	GC1TO1_LINEUP,
	GC1TO1_WARPERSON,
	GC1TO1_WARGUILD,
#endif // __GUILD_COMBAT_1TO1
#ifdef __JEFF_FIX_0
	QM_QUERY_MAIL_BOX,
	QM_ALL_PLAYER_DATA,
#endif	// __JEFF_FIX_0
#if __VER >= 14 // __INSTANCE_DUNGEON
	LOG_INSTANCEDUNGEON,
#endif // __INSTANCE_DUNGEON
#ifdef __ERROR_LOG_TO_DB
	LOG_ERROR,
#endif // __ERROR_LOG_TO_DB
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	QM_LOGIN_PROTECT, 
#endif // __2ND_PASSWORD_SYSTEM
#if __VER >= 15 // __GUILD_HOUSE
	LOG_GUILDFURNITURE
#endif // __GUILD_HOUSE
};

typedef struct tagACCOUNT_INFO
{
	char	szAccount[MAX_ACCOUNT];
	char	szPassword[MAX_PASSWORD];
	char	szPlayer[MAX_PLAYER];
	int		nPlayerSize;
	int		nPlayerSlot;
}	ACCOUNT_INFO, *LPACCOUNT_INFO;

typedef struct tagDB_OVERLAPPED_PLUS
{
	OVERLAPPED	Overlapped;
	int		nQueryMode;
	//	mulcom	BEGIN100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
	BYTE*	lpBuf;
	//BYTE	lpBuf[CAr::nGrowSize * 2];
	//	mulcom	END100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
	u_long	uBufSize;
	ACCOUNT_INFO	AccountInfo;
	DPID	dpid;
	DPID	dpidCache;
	DPID	dpidUser;
	DWORD	dwAuthKey;
	tagDB_OVERLAPPED_PLUS()
	{
		//	mulcom	BEGIN100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
		lpBuf	= NULL;
		//::memset( lpBuf, 0, sizeof(lpBuf) );
		//	mulcom	END100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
		
		uBufSize = 0;
	}
}	DB_OVERLAPPED_PLUS,	*LPDB_OVERLAPPED_PLUS;

struct	MAIL_QUERYINFO
{
	LPCTSTR		pszType;
	int		nMail;
	u_long	idReceiver;
	u_long	idSender;
	int		nGold;
	time_t	tmCreate;
	BYTE	byRead;
	LPCTSTR		szTitle;
	LPCTSTR	szText;
	DWORD	dwItemId;
	int		nItemNum;
	int		nRepairNumber;
	int		nHitPoint;
	int		nMaxHitPoint;
	int		nMaterial;
	BYTE	byFlag;
	SERIALNUMBER	iSerialNumber;
	int		nOption;
	int		bItemResist;
	int		nResistAbilityOption;
	u_long	idGuild;
	int		nResistSMItemId;
	BYTE	bCharged;
	DWORD	dwKeepTime;
#if __VER >= 11 // __SYS_IDENTIFY
	__int64	iRandomOptItemId;
#else	// __SYS_IDENTIFY
	int		nRandomOptItemId;
#endif	// __SYS_IDENTIFY
	int		nPiercedSize;
	DWORD	dwItemId1;
	DWORD	dwItemId2;
	DWORD	dwItemId3;
	DWORD	dwItemId4;
#if __VER >= 9 // __ULTIMATE
	DWORD	dwItemId5;
#endif // __ULTIMATE
	BOOL	bPet;
	BYTE	nKind;
	BYTE	nLevel;
	DWORD	dwExp;
	WORD	wEnergy;
	WORD	wLife;
	BYTE	anAvailLevel[PL_MAX];
#ifdef __PET_1024
	char	szPetName[MAX_PET_NAME];
#endif	// __PET_1024

	MAIL_QUERYINFO( LPCTSTR pszQueryType )
		{
			pszType	= pszQueryType;
			nMail	= 0;
			idReceiver	= 0;
			idSender	= 0;
			nGold	= 0;
			tmCreate	= 0;
			byRead	= 0;
			szTitle	= "";
			szText	= "";
			dwItemId	= 0;
			nItemNum	= 0;
			nRepairNumber	= 0;
			nHitPoint	= 0;
			nMaxHitPoint	= 0;
			nMaterial	= 0;
			byFlag	= 0;
			iSerialNumber	= 0;
			nOption	= 0;
			bItemResist	= 0;
			nResistAbilityOption	= 0;
			idGuild	= 0;
			nResistSMItemId	= 0;
			bCharged	= 0;
			dwKeepTime	= 0;
#if __VER >= 11 // __SYS_IDENTIFY
			iRandomOptItemId	= 0;
#else	// __SYS_IDENTIFY
			nRandomOptItemId	= 0;
#endif	// __SYS_IDENTIFY
			nPiercedSize	= 0;
			dwItemId1	= 0;
			dwItemId2	= 0;
			dwItemId3	= 0;
			dwItemId4	= 0;
#if __VER >= 9 // __ULTIMATE
			dwItemId5	= 0;
#endif // __ULTIMATE
			bPet	= FALSE;
			nKind	= nLevel	= 0;
			dwExp	= 0;
			wEnergy	= wLife	= 0;
			memset( anAvailLevel, 0, sizeof(BYTE) * PL_MAX );
#ifdef __PET_1024
			szPetName[0]	= '\0';
#endif // __PET_1024
		}
};

// 길드쿼리문장을 만든다.
struct GUILD_QUERYINFO
{
	LPCTSTR	pszType;
	u_long	idPlayer;
	u_long	idGuild;
	LPCTSTR	pszGuild;
	DWORD   dwLv1;
	DWORD   dwLv2;
	DWORD   dwLv3;
	DWORD   dwLv4;
	WORD	nLevel;
	int     nGuildGold;
	int     nGuildPxp;
	int     nWin;
	int		nLose;
	int     nSurrender;
	DWORD   dwLogo;
	int		nClass;
	LPCTSTR pszNotice;

	GUILD_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idPlayer = 1;
		idGuild = 1;
		pszGuild = "";
		dwLv1 = dwLv2 = dwLv3 = dwLv4 = 0;
		nLevel = 0;
		nGuildGold = 0;
		nGuildPxp = 0;
		nWin = 0;
		nLose = 0;
		nSurrender = 0;
		dwLogo = 0;
		nClass = 0;
		pszNotice = "";
	};
};

// 길드 투표 쿼리문장을 만든다.
struct VOTE_QUERYINFO
{
	LPCTSTR		pszType;
	u_long		idVote;
	u_long		idGuild;
	LPCTSTR		pszTitle;
	LPCTSTR		pszQuestion;
	LPCTSTR		pszSelections[4];
	BYTE		cbSelection;

	VOTE_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idVote = 0;
		idGuild = 0;
		pszTitle = "";
		pszQuestion = "";
		for( int i=0; i<4; ++i )
			pszSelections[i] = "";

		cbSelection = 0;
	};
}; 

// 길드 Log 쿼리문장을 만든다.
struct GUILDLOG_QUERYINFO
{
	LPCTSTR		pszType;			// 로그 타입
	u_long		idGuild;			// 길드 아이디
	u_long		idPlayer;			// 당한자
	u_long		idDoPlayer;			// 시킨자
	int			nGuildGold;			// 길드 골드
	int			nGuildPxp;			// 길드 Pxp
	int			nGuildLv;			// 길드 레벨
	int			nLevel;				// 당한자 레벨
	int 		nItem;				// 아이템 아이디 / 거래 페냐량
	int			nAbilityOption;		// 아이템 옵션
	int			nItem_count;		// 거래 갯수
	SERIALNUMBER	iUniqueNo;			// 유니크 넘버
	LPCTSTR		pszTime;			// 시각
	LPCTSTR		pszGuildBank;		// 길드 뱅크
	LPCTSTR		pszState;
	
	GUILDLOG_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idGuild = 0;	
		idPlayer = 0;	
		idDoPlayer = 0;	
		nGuildGold = 0;	
		nGuildPxp = 0;	
		nGuildLv = 0;	
		nLevel = 0;		
		nItem = 0;
		nAbilityOption = 0;
		nItem_count = 0 ;
		iUniqueNo = 0;
		pszTime = "";
		pszGuildBank = "";
		pszState = "";
	};
}; 

struct ADDBANK_QUERYINFO
{
	LPCTSTR		pszType;
	u_long		idPlayer;
	LPCTSTR		pszBank;
	LPCTSTR		pszBankIndex;
	LPCTSTR		pszObjIndexBank;
	LPCTSTR		pszExtBank;
	LPCTSTR		pszPirecingBank;
	DWORD		dwGoldBank;
	LPCTSTR		pszBankPet;

	ADDBANK_QUERYINFO( LPCTSTR pszQueryType )
	{
		pszType = pszQueryType;
		idPlayer = 0;
		pszBank = "";
		pszBankIndex = "";
		pszObjIndexBank = "";
		pszExtBank = "";
		pszPirecingBank = "";
		dwGoldBank = 0;
		pszBankPet	= "";
	};
};

#if __VER >= 11 // __SYS_POCKET
typedef	struct	_PocketParam
{
	u_long		idPlayer;
	int		nPocket;
	const char*	pszItem;
	const char* pszIndex;
	const char* pszObjIndex;
	const char* pszExt;
	const char* pszPiercing;
	const char* pszPet;
	BOOL	bExpired;
	time_t	tExpirationDate;

	_PocketParam()
	{
		idPlayer	= 0;
		nPocket		= 0;
		pszItem		= "";
		pszIndex	= "";
		pszObjIndex		= "";
		pszExt	= "";
		pszPiercing		= "";
		pszPet	= "";
		bExpired	= TRUE;
		tExpirationDate		= 0;
	}
}	PocketParam;
#endif	// __SYS_POCKET

struct WAR_QUERYINFO
{
	LPCTSTR	pszType;
	u_long	idWar;
	u_long	idGuild;
	u_long	idPlayer;
	u_long	f_idGuild;
	int		nWinPoint;
	int		f_nWinPoint;
	char	iState;

	WAR_QUERYINFO( LPCTSTR pszQueryType )
	{
		pszType = pszQueryType;
		idWar	= 0;
		idGuild		= 0;
		iState	= '0';
		f_idGuild	= 0;
		idPlayer	= 0;
		nWinPoint	= 0;
		f_nWinPoint	= 0;
	};
};

///////////////////////////////////////////////////////////////////////
// WANTED_QUERYINFO
///////////////////////////////////////////////////////////////////////

// 길드 투표 쿼리문장을 만든다.
struct WANTED_QUERYINFO
{
	LPCTSTR		pszType;
	u_long		idPlayer;
	int			nGold;
	long		nEnd;
	char		szMsg[WANTED_MSG_MAX + 1];

	WANTED_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idPlayer = 0;
		nGold = 0;
		nEnd = 0;
		szMsg[0] = '\0';
	};
}; 




///////////////////////////////////////////////////////////////////////
// ACCOUNT_CACHE
///////////////////////////////////////////////////////////////////////
struct ACCOUNT_CACHE
{
	time_t							m_tmPrevent;
	CMover*							pMover[3];
	u_long							m_idPlayerBank[3];
	CItemContainer< CItemElem  >	m_Bank[3];
	DWORD							m_dwGoldBank[3];
	time_t							m_tmLastAccess;
	string							m_strAccount;

	void	Init();
	void	Clear();
	void	ClearMover( BYTE nSlot, u_long idPlayer );
	BOOL	IsReleaseable();
#ifdef __INVALID_LOGIN_0320
private:
	u_long	m_uMultiServer;
public:
	u_long	GetMultiServer( void )	{	return m_uMultiServer;	}
	void	ChangeMultiServer( u_long uMultiServer )	{ m_uMultiServer	= uMultiServer;	}
	BOOL	IsPlaying( void )	{ return m_uMultiServer != 0;		}
#endif	// __INVALID_LOGIN_0320
};

inline void ACCOUNT_CACHE::Init()
{
	m_tmPrevent = 0;
	m_tmLastAccess = ::time( NULL );

	for( int i = 0 ; i < 3 ; ++i )
	{
		pMover[i] = NULL;
		m_Bank[i].SetItemContainer( ITYPE_ITEM, MAX_BANK ) ;
		m_dwGoldBank[i] = 0;
		m_idPlayerBank[i] = 0;
	}
#ifdef __INVALID_LOGIN_0320
	m_uMultiServer	= 0;
#endif	// __INVALID_LOGIN_0320
}


inline void ACCOUNT_CACHE::Clear()
{
	for( int i = 0 ; i < 3 ; ++i )
	{
		SAFE_DELETE( pMover[i] );
	}
}

inline void ACCOUNT_CACHE::ClearMover( BYTE nSlot, u_long idPlayer )
{
	m_Bank[nSlot].Clear();
	m_dwGoldBank[nSlot]	= 0;
	m_idPlayerBank[nSlot] = idPlayer;
	SAFE_DELETE( pMover[nSlot] );
}


inline BOOL ACCOUNT_CACHE::IsReleaseable()
{
	time_t t	= m_tmLastAccess + TIMEWAIT_RELEASE;
	if( ::time( NULL ) > t )
		return TRUE;
	return FALSE;
}


#ifdef __TOOMANY_PENDINGLOG1115
#define	MAX_PENDING		1024
#endif	// __TOOMANY_PENDINGLOG1115

#define	MAX_QUERY_RESERVED	2

//typedef CMyMap2<CMover*>	C2Mover;
//typedef CMyBucket2<CMover*> CMoverBucket;
typedef	set<string>			SET_STRING;
typedef map<u_long, string>	ULONG2STRING;
typedef map<int, string>	INT2STRING;

typedef	struct	_ItemStruct
{
	char	szItem[512];
	char	szExt[64];
#if __VER >= 12 // __EXT_PIERCING
	char	szPiercing[256];
#else // __EXT_PIERCING
	char	szPiercing[64];
#endif // __EXT_PIERCING

#ifdef __PET_1024
	char	szPet[100];
#else	// __PET_1024
	char	szPet[64];
#endif	// __PET_1024

	_ItemStruct()
		{
			szItem[0]	= '\0';
			szExt[0]	= '\0';
			szPiercing[0]	= '\0';
			szPet[0]	= '\0';
		}
}	ItemStruct, *PItemStruct;

typedef	struct	_ItemContainerStruct
{
	char	szItem[6144];
	char	szIndex[512];
	char	szObjIndex[512];
	char	szExt[2048];
#if __VER >= 15 // __PETVIS
	char	szPiercing[8000];
#else // __PETVIS
	char	szPiercing[4096];
#endif // __PETVIS
#ifdef __PET_1024
	char	szPet[4200];	// 42 * 100
#else	// __PET_1024
	char	szPet[2688];	// 42 * 64
#endif	// __PET_1024
	_ItemContainerStruct()
		{
			*szItem	= '\0';
			*szIndex	= '\0';
			*szObjIndex	= '\0';
			*szExt	= '\0';
			*szPiercing	= '\0';
			*szPet	= '\0';
		}
}	ItemContainerStruct, *PItemContainerStruct;

#if __VER >= 11 // __SYS_POCKET
typedef	struct	_PocketStruct : public ItemContainerStruct
{
	BOOL	bExpired;
	time_t	tExpirationDate;
	_PocketStruct() : ItemContainerStruct()
	{
		bExpired	= TRUE;
		tExpirationDate		= 0;
	}
}	PocketStruct,	*PPocketStruct;
#endif	// __SYS_POCKET


#if __VER >= 11 // __SYS_COLLECTING
typedef	struct	_CONV_RESULT_ITEM
{
	DWORD	dwItemId;
	int		nAbilityOption;
	_CONV_RESULT_ITEM()
	{
		dwItemId	= 0;
		nAbilityOption	= 0;
	}
}	CONV_RESULT_ITEM;
#endif	// __SYS_COLLECTING

class CGuildMng;
class CMail;

#if __VER >= 12
typedef	map<DWORD, CONV_RESULT_ITEM>	MDC;
#endif	// 12

class CDbManager
{
	struct __GUILDCOMBATJOIN
	{
		DWORD dwPenya;
		BOOL  bRequest;
#ifdef __S_BUG_GC
		u_long uGuildId;
#endif // __S_BUG_GC
	};
	struct __SendItemContents
	{
		char OneItem[MAX_PATH];
		int nNo, Item_Count;
		__int64 m_dwKeepTime;
#if __VER >= 12 // __EXT_PIERCING
		CItemElem itemElem;
#else // __EXT_PIERCING
		int m_nAbilityOption, nItemResist, nResistAbilityOption, nCharged;
		int nPiercedSize;
		DWORD adwItemId0, adwItemId1, adwItemId2, adwItemId3;
#ifdef __SEND_ITEM_ULTIMATE
		DWORD adwItemId4;
#endif // __SEND_ITEM_ULTIMATE
#if __VER >= 11 // __SYS_IDENTIFY
		__int64	iRandomOptItemId;
#else	// __SYS_IDENTIFY
		int	nRandomOptItemId;
#endif	// __SYS_IDENTIFY
#endif // __EXT_PIERCING
	};
	struct __GCRESULTVALUEGUILD
	{
		int	nCombatID;			// 길드대전 아이디
		u_long uidGuild;		// 길드 아이디
		__int64 nReturnCombatFee;	// 돌려받을 참여금
		__int64 nReward;				// 보상금
	};
	struct __GCRESULTVALUEPLAYER
	{
		int nCombatID;			// 길드대전 아이디
		u_long uidGuild;		// 길드 아이디
		u_long uidPlayer;		// 플레이어 아이디
		__int64 nReward;			// 보상금
	};
	struct __GCPLAYERPOINT
	{
		u_long	uidPlayer;
		int		nJob;
		int		nPoint;
		
		__GCPLAYERPOINT()
		{
			uidPlayer = nJob = nPoint = 0;
		}
	};
	
	struct __MAIL_REALTIME
	{
		int nMail_Before;
		int nMail_After;
		SERIALNUMBER m_liSerialNumber;
		int m_nHitPoint;
	};

public:
#ifdef __ITEM_REMOVE_LIST
	SET_STRING	m_RemoveItem_List;
#endif // __ITEM_REMOVE_LIST
	INT2STRING		m_int2StrItemUpdate;

	CAccountCacheMgr m_AccountCacheMgr;
	CMclCritSec		m_AddRemoveLock;
#if __VER < 11 // __SYS_PLAYER_DATA
	ULONG2STRING	m_ulong2str;
#endif	// __SYS_PLAYER_DATA
	ULONG2STRING	m_2PartyNamePtr;
	CMclCritSec		m_joinLock;
	char			m_aszJoin[MAX_JOIN_SIZE][MAX_PLAYER];
	int				m_nJoin;
	CMclCritSec		m_csMapSavePlayer;
	map<u_long, int>	m_mapSavePlayer;		// m_idPlayer, nSlot
	HANDLE			m_hIOCPGet;
	HANDLE			m_hIOCPPut;
	HANDLE			m_hIOCPUpdate;
	HANDLE			m_hIOCPGuild;			// 길드 업뎃용 
#ifdef __S1108_BACK_END_SYSTEM
	HANDLE			m_hWorker;
	HANDLE			m_hCloseWorker;
#endif // __S1108_BACK_END_SYSTEM
	HANDLE			m_hItemUpdateWorker;
	HANDLE			m_hItemUpdateCloseWorker;
	int				m_nItemUpdate;

	DB_OVERLAPPED_PLUS*		AllocRequest( void );
	void	FreeRequest( DB_OVERLAPPED_PLUS* pOverlappedPlus );
	void	MakeRequest( DB_OVERLAPPED_PLUS* pOverlappedPlus,  LPBYTE lpBuf, u_long uBufSize );

#ifndef __TRANS_0413
	MemPooler<DB_OVERLAPPED_PLUS>*	m_pDbIOData;
#endif	// __TRANS_0413

#ifdef __J0826
	CMclCritSec		m_csCreatePlayer;
#endif	// __J0826

	vector<__GCRESULTVALUEGUILD>			m_GCResultValueGuild;		// 길드대전 결과값
	vector<__GCRESULTVALUEPLAYER>			m_GCResultValuePlayer;		// 길드대전 결과값
	vector<__GCPLAYERPOINT> m_vecGCPlayerPoint;

#ifdef __S_BUG_GC
	vector<__GUILDCOMBATJOIN>	m_vecGuildCombat;	
#else // __S_BUG_GC
	map<u_long, __GUILDCOMBATJOIN>	m_GuildCombat;	
#endif // __S_BUG_GC
	int		m_nGuildCombatIndex;
	u_long	m_uWinGuildId;
	u_long	m_uBestPlayer;
	int		m_nWinGuildCount;
//	CMclCritSec		m_csGuildCombat;

	char DB_ADMIN_PASS_LOG[256];
	char DB_ADMIN_PASS_CHARACTER01[256];
	char DB_ADMIN_PASS_BACKSYSTEM[256];
	char DB_ADMIN_PASS_ITEMUPDATE[256]; 

public:
	static CDbManager & GetInstance();
	~CDbManager();

	BOOL	RemoveQuest( void );

	void	PostSavePlayer( u_long idPlayer, BYTE nSlot );
	void	SavePlayer( CQuery *qry, CQuery* pQueryLog, CMover* pMover, char* szQuery );
	void	SavePlayTime( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );	
#ifdef __S_NEW_SKILL_2
	void	AllSaveSkill( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif // __S_NEW_SKILL_2	

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	void	SaveHonor( CQuery *qry, u_long uidPlayer, int * aHonor, char* szQuery );
#endif	// __HONORABLE_TITLE			// 달인

#ifdef __SKILL_0205
	void	SaveSkill( CQuery *qry, u_long uidPlayer, LPSKILL aJobSkill, LPBYTE abUpdateSkill, char* szQuery );
#else	// __SKILL_0205
	void	SaveSkill( CQuery *qry, u_long uidPlayer, LPSKILL aJobSkill, char* szQuery );
#endif	// __SKILL_0205
	void	SaveJobLv( CMover* pMover, char* szJobLv );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	void	SaveQuest( CMover* pMover, char* szQuestCnt, char* szm_aCompleteQuest, char* szCheckedQuest );
#else // __IMPROVE_QUEST_INTERFACE
	void	SaveQuest( CMover* pMover, char* szQuestCnt, char* szm_aCompleteQuest );
#endif // __IMPROVE_QUEST_INTERFACE

	void	SaveInventory( CMover* pMover, PItemContainerStruct pItemContainerStruct );
	void	SaveBank( CMover* pMover, CItemContainer<CItemElem>* pPlayerBank, PItemContainerStruct pItemContainerStruct );
	void	SaveGuildBank( CItemContainer<CItemElem>*  pGuildBank, PItemContainerStruct pItemContainerStruct );

	void	SaveEquipment( CMover* pMover, char* szEquipmen );
	void	SaveCardCube( CMover* pMover, char* szCard, char* szsCardIndex, char* szsCardObjIndex, char* szCube, char* szsCubeIndex, char* szsCubeObjIndex );
	void	SaveTaskBar( CMover* pMover, char* szAppletTaskBar, char* szItemTaskBar, char* szSkillTaskBar );
	void	SaveSMCode( CMover* pMover, char* szszSMTime );
	void	SaveSkillInfluence( CMover* pMover, char* szszSkillInfluence );
#if __VER >= 11 // __SYS_POCKET
	void	MakeQueryPocket( char* szQuery, const PocketParam & p );
#endif	// __SYS_POCKET
	void	DBQryAddBankSave( char* szSql, const ADDBANK_QUERYINFO & info );
	void	SaveOneItem( CItemElem* pItemElem, PItemStruct pItemStruct );
	void	SendPlayerList( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SendJoin( CMover* pMover, LPDB_OVERLAPPED_PLUS lpDBOP, DWORD dwAuthKey, 
						  DWORD dwBillingClass, LPCTSTR szPartyName, LPCTSTR szBankPass, 
						  DWORD dwPlayTime, DWORD dwFlyTime, int nMaximumLevel, int nTags, TAG_ENTRY* tags,
						  LPCTSTR szTimeGuild );
	void	Join( CQuery* qry, CQuery* qry1, CQuery* qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	CreatePlayer( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	RemovePlayer( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ChangeBankPass( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	Gamema_Chat( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

#if __VER < 11 // __SYS_PLAYER_DATA
	void	SerializePlayerID( CAr & ar );
#endif	// __SYS_PLAYER_DATA

//	u_long	GetPlayerID( LPCSTR lpszPlayer );
#if __VER >= 11 // __SYS_PLAYER_DATA
	BOOL	LoadPlayerData( void );
#else	// __SYS_PLAYER_DATA
	BOOL	OpenPlayerID( void );
#endif	// __SYS_PLAYER_DATA
	BOOL	GetPartyName( void );
	void	SerializeGuildCombat( CAr & ar );
	void	SerializeResultValue( CAr & ar );
	void	SerializePlayerPoint( CAr & ar );
	BOOL	OpenGuildCombat( void );
	BOOL	LoadPost( void );
	void	GetItemFromMail( CQuery* pQuery, CItemElem* pItemElem );
#ifdef __POST_1204
	void	RemoveMail( list<CMail*> & lspMail, time_t t );
#else	// __POST_1204
	void	RemoveMail( list<CMail*> & lspMail );
#endif	// __POST_1204
	CQuery*	m_apQuery[MAX_QUERY_RESERVED];
	BOOL	QueryRemoveGuildBankTbl( void );
	void	DeleteRemoveGuildBankTbl( CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	call_uspLoggingQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
#ifdef __GETMAILREALTIME
	BOOL	QueryGetMailRealTime( CQuery* pQuery );
#endif // __GETMAILREALTIME

#if __VER >= 11 // __GUILD_COMBAT_1TO1
	void	LoadGC1to1TenderGuild( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, DPID dpid );
	void	GC1to1State( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GC1to1Tender( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GC1to1LineUp( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GC1to1WarPerson( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GC1to1WarGuild( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif // __GUILD_COMBAT_1TO1

#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
	void	GuildBankLogView( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
	void	SealChar( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SealCharConm( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SealCharGet( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SealCharSet( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz

	void	CalluspPetLog( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );

	BOOL	GetMyPartyName( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, u_long uidPlayer, char * szPartyName );
	BOOL	BankToItemSendTbl( LPCSTR lpFileName );
	BOOL	InventoryToItemSendTbl( LPCSTR lpFileName );
	BOOL	ItemRemove0203( LPCSTR lpFileName );
	BOOL	RemoveInvalidItem();

	BOOL	Conv( LPCSTR lpFileName );
#if __VER >= 11 // __SYS_COLLECTING
	BOOL	ConvInventory( map<DWORD, CONV_RESULT_ITEM> & mConv );
	BOOL	ConvBank( map<DWORD, CONV_RESULT_ITEM> & mConv );
	BOOL	ConvGuildBank( map<DWORD, CONV_RESULT_ITEM> & mConv );
#else	// __SYS_COLLECTING
	BOOL	ConvInventory( map<DWORD, DWORD> & mConv );
	BOOL	ConvBank( map<DWORD, DWORD> & mConv );
	BOOL	ConvGuildBank( map<DWORD, DWORD> & mConv );
#endif	// __SYS_COLLECTING

#if __VER >= 12
	CQuery*		CreateCharQuery( void );
	BOOL	ConvertPocket( MDC & mConv );
#endif	// 12

	BOOL	RestorePet( LPCTSTR lpFileName );
	BOOL	RestorePetInventory( map<DWORD, int> & mRestore );
	BOOL	RestorePetBank( map<DWORD, int> & mRestore );
	BOOL	RestorePetGuildBank( map<DWORD, int> & mRestore );

	void	CreateGuild( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	DestroyGuild( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	AddGuildMember( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	RemoveGuildMember( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	OpenGuild( void );
	void	UpdateGuildMemberLv( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildClass( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildNickName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildMaster( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

#ifdef __TOOMANY_PENDINGLOG1115
	static	LONG	m_lPending;
	static	BOOL	IsTooManyPending( void );
	static	void	AddPending( void )	{	InterlockedIncrement( &m_lPending );	}
	static	void	ReleasePending( void )	{	InterlockedDecrement( &m_lPending );	}
#endif	// __TOOMANY_PENDINGLOG1115

	void	UpdateGuildLogo( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildContribution( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildNotice( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildAuthority( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildPenya( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildRealPay( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildSetName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildBankUpdate( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	OpenQueryGuildBank( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	OpenQueryGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	
	void	AddGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	RemoveGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	CloseGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	CastGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	AcptWar( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	WarEnd( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	Surrender( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	WarDead( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	WarMasterAbsent( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );


	void	UpdateGuildRanking( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	UpdateGuildRankingDB( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	AddPartyName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

#if __VER >= 11 // __SYS_IDENTIFY
	BOOL	call_uspLoggingTrade( CQuery* pQuery, int nFlag, int nTradeID,  DWORD dwWorldID = 0, u_long idPlayer = 0, DWORD dwTradeGold = 0, const char* lpAddr = "", int nLevel = 0, int nJob = 0, int nItemIndex = 0, SERIALNUMBER iItemSerialNum = 0, int nItemCnt = 0, int nAbilityOpt = 0, int nItemResist = 0, int nResistAbilityOpt = 0, __int64 iRandomOpt=0 );
#else	// __SYS_IDENTIFY
	BOOL	call_uspLoggingTrade( CQuery* pQuery, int nFlag, int nTradeID,  DWORD dwWorldID = 0, u_long idPlayer = 0, DWORD dwTradeGold = 0, const char* lpAddr = "", int nLevel = 0, int nJob = 0, int nItemIndex = 0, SERIALNUMBER iItemSerialNum = 0, int nItemCnt = 0, int nAbilityOpt = 0, int nItemResist = 0, int nResistAbilityOpt = 0, int nRandomOpt=0 );
#endif	// __SYS_IDENTIFY
	LONG	m_cbTrade;
	int		GetTradeNo( void );
	void	call_uspXXXMultiServer( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );

	BOOL	CreateDbWorkers( void );
	void	CloseDbWorkers( void );
	void	GetThread( void );
	void	PutThread( void );
	void	SPThread( void );
	void	UpdateThread( void );
	void	GuildThread( void );
#ifdef __S1108_BACK_END_SYSTEM
	void	BackSystem( void );
#endif // __S1108_BACK_END_SYSTEM
	void	ItemUpdateThread( void );
	void	ChangeItemUpdate( CQuery* pQuery );
	void	ChangeSkillUpdate( CQuery* pQuery );
	void	ChangeMonsterUpdate( CQuery* pQuery );
	void	ChangeQuestUpdate( CQuery* pQuery );
	void	ChangeJobNameUpdate( CQuery* pQuery );
	void	ChangeSexUpdate( CQuery* pQuery );
	//void	GetBank( BOOL bCache, ACCOUNT_CACHE* AccountCache, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, int nMySlot, char* pszBankPW );
	BOOL	GetBank( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, int nSlot );
	BOOL	GetInventory( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#if __VER >= 12 // __EXT_PIERCING
	void	LoadPiercingInfo( CItemElem & itemElem, char* szPirecingInven, int* pLocation );
	void	GetPiercingInfoFromMail( CQuery* pQuery, CItemElem* pItemElem );
#endif // __EXT_PIERCING
#if __VER >= 11 // __SYS_POCKET
	BOOL	GetPocket( CMover* pMover, CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	SavePocket( CMover* pMover, PPocketStruct pPocketStruct );
#endif	// __SYS_POCKET
	BOOL	GetEquipment( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	GetTaskBar( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetCardCube( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetBaseCharacter( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	GetSkill( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetJobLv( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	GetQuest( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	GetSMMode( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	GetSKillInfluence( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	void	GetHonor( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif	// __HONORABLE_TITLE			// 달인

	static	UINT	_GetThread( LPVOID pParam );
	static	UINT	_PutThread( LPVOID pParam );
	static	UINT	_SPThread( LPVOID pParam );
	static	UINT	_UpdateThread( LPVOID pParam );
	static	UINT	_GuildThread( LPVOID pParam );
#ifdef __S1108_BACK_END_SYSTEM
	static  UINT	_BackSystem( LPVOID pParam );
#endif // __S1108_BACK_END_SYSTEM
	static	UINT	_ItemUpdateThread( LPVOID pParam );

	void	Clear( void );

	void	DBQryCharacter( char* qryCharacter, char* Gu, u_long idPlaeyr, int nserverindex, char* szAccount = '\0', char* szName = '\0',
		int nPlayerslot = 0, DWORD dwWorldID = 0, DWORD dwIndex = 0, float vPos_x = 0.0f, float vPos_y = 0.0f,
		float vPos_z = 0.0f, char* szCharacterKey = '\0', DWORD dwSkinSet = 0, DWORD dwHairMesh = 0, DWORD dwHairColor = 0,
		DWORD dwHeadMesh = 0, DWORD dwSex = 0, float vScale_x = 0.0f, DWORD dwMotion = 0, float fAngle = 0.0f,
		int nHitPoint = 0, int nManaPoint = 0, int nFatiguePoint = 0, DWORD dwRideItemIdx = 0, DWORD dwGold = 0,
		int nJob = 0, char* szActMover = '\0', int nStr = 0, int nSta = 0, int nDex = 0,
		int nInt = 0, int nLevel = 0, EXPINTEGER nExp1 = 0, EXPINTEGER nExp2 = 0, char* szJobSkill = '\0',
		char* szLicenseSkill = '\0', char* aJobLv = '\0', DWORD dwExpertLv = 0, int nidMarkingWorld = 0, float vMarkingPos_x = 0.0f,
		float vMarkingPos_y = 0.0f, float vMarkingPos_z = 0.0f, int nRemainGP = 0, int nRemainLP = 0, int nFlightLv = 0,
		int nFxp = 0, int nTxp = 0, char* szQuestCntArray = '\0', char szAuthority = 'F', DWORD dwMode = 0,
#if __VER >= 8 // __S8_PK
		int nidparty = 0, int nidMuerderer = 0, int nFame = 0,
#else // __VER >= 8 // __S8_PK
		int nidparty = 0, int nNumKill = 0, int nidMuerderer = 0, int nSlaughter = 0, int nFame = 0,
#endif // __VER >= 8 // __S8_PK
		EXPINTEGER nDeathExp = 0, int nDeathLevel = 0, DWORD dwFlyTime = 0, DWORD dwMessengerState = 0,
		int nTotalPlayTime = 0, char* szCard = '\0', char* szIndex_Card = '\0', char* szObjIndexCard = '\0', char* szCube = '\0',
		char* szIndex_Cube = '\0', char* szObjIndex_Cube = '\0', char* szInventory = '\0', char* dzapIndex = '\0', char* szadwEquipment = '\0',
		char* szdwObjIndex = '\0', char* szSlotApplet = '\0', char* szSlotItem = '\0', char* szSlotQueue = '\0', int nActionPoint = 0,
		char* szBank = '\0', char* m_apIndex_Bank = '\0', char* sadwObjIndex_Bank = '\0', DWORD dwGoldBank = 0,
		int nFuel = 0, int tmAccFuel = 0, char* szSMTime = '\0', char* szSkillInfluence = '\0', DWORD m_dwSkillPoint = 0
		, char* m_aCompleteQuest = '\0'
		, char* ExtInven = '\0', char* PirecingInven = '\0', char* ExtBank = '\0', char* PirecingBank = '\0'
		, DWORD dwReturnWorldID = 0, float fReturnPosX  = 0.0f, float fReturnPosY = 0.0f, float fReturnPosZ = 0.0f
#if __VER >= 8 // __S8_PK
		, int nPKValue = 0, DWORD dwPKPropensity = 0, DWORD dwPKExp	= 0
#endif // __VER >= 8 // __S8_PK
#if __VER >= 8 //__CSC_VER8_5
		, EXPINTEGER nAngelExp = 0, LONG nAngelLevel = 0
#endif // __CSC_VER8_5
		, char* szInventoryPet	= '\0',	char* szBankPet	= '\0', DWORD dwPetId = 0
#ifdef __EXP_ANGELEXP_LOG
		, int nExpLog = 0, int nAngelExpLog = 0
#endif // __EXP_ANGELEXP_LOG
#ifdef __EVENTLUA_COUPON
		, int nCoupon = 0
#endif // __EVENTLUA_COUPON
#if __VER >= 13 // __HONORABLE_TITLE			// 달인
		, int nHonor = -1					// 달인선택 
#endif	// __HONORABLE_TITLE			// 달인
#ifdef __LAYER_1015
		, int nLayer	= 0
#endif	// __LAYER_1015
#if __VER >= 15 // __CAMPUS
		, int nCampusPoint = 0, u_long idCampus = 0
#endif // __CAMPUS
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		, char* m_aCheckedQuest = '\0'
#endif // __IMPROVE_QUEST_INTERFACE
		);

	void	DBQryLog( char* qryLog, char* Gu, u_long idPlayer, int nserverindex, EXPINTEGER nExp1 = 0, int nLevel = 0,
						  int nJob = 0, int JobLv = 0, int FlightLv = 0, int nStr = 0, int nDex = 0,
						  int nInt = 0, int nSta = 0, int nRemainGP = 0, int nRemainLP = 0, char szState = 'A',
						  int WorldID = 0, char* szkilled_szName = '\0', float vPos_x = 0.0f, float vPos_y = 0.0f, float vPos_z = 0.0f,
						  int nAttackPower = 0, int nMax_HP = 0, char *uGetidPlayer = '\0', DWORD dwGold = 0, int nRemainGold = 0,
						  int nItem_UniqueNo = 0, char *szItem_Name = '\0', int Item_durability = 0, int Item_count = 0, int ItemAddLv = 0,
						  char* szStart_Time = '\0', int TotalPlayerTime = 0, char *szRemoteIP = '\0', int nQuest_Index = 0, u_long uKill_idPlayer = 0 );	

	void	DBQryGuild( char* szSql, const char* szType, u_long idPlayer = 0, u_long idGuild = 0, const char* szGuild = "" );
	void	DBQryGuild( char* szSql, const GUILD_QUERYINFO & info);
	void	DBQryVote( char* szSql, const VOTE_QUERYINFO& info );
	void	DBQryGuildLog( char* szSql, const GUILDLOG_QUERYINFO& info );
	void	DBQryWar( char* szSql, const WAR_QUERYINFO & info );
	void	DBQryNewItemLog( char* qryLog, const LogItemInfo& info );

	int		GetOneItem( CItemElem* pItemElem, char* pstrItem, int *pLocation );
	void	GetOneSkill( LPSKILL pSkill, char* pstrSkill, int *pLocation );
	void	GetOneQuest( LPQUEST pQuest, char* pstrQuest, int *pLocation );

#ifdef __CONV_SKILL
	void	ConvSkill( void );
#endif	// __CONV_SKILL

#ifdef __CONV_SKILL_11_MONTH_JOB1
	void	ConvSkill_11_MONTH_POINT( void );
#endif // __CONV_SKILL_11_MONTH_JOB1
#ifdef __CONV_SKILL_11_MONTH_JOB2
	void	ConvSkill_11_MONTH( void );
#endif // __CONV_SKILL_11_MONTH_JOB2

#ifdef __CONV_SKILL_STOP
	void	ConvSkillStop( void );
#endif // __CONV_SKILL_STOP

#ifdef __VERIFY_PLAYER
	BOOL	VerifyPlayer( void );
#endif	// __VERIFY_PLAYER
	
#ifdef __S0707_ITEM_CONV
	BOOL	ConvItem( void );
#endif // __S0707_ITEM_CONV
	BOOL	ConvItem050615( void );
	void	ConvChangeItem050615( CMover* pMover );
	void	ConvChangeGuild050615( CMover* pMover );
	void	ConvChangeGuild050615( CItemContainer< CItemElem  >*  GuildBank );
	BOOL	GetBankMover( CMover* pMover, CQuery *qry, int nSlot );
	void	GetGuildBank( CItemContainer< CItemElem  >*  GuildBank, CQuery *qry );
	BOOL	RemoveItemInvenBank( CMover* pMover, int* nCountItem0, int* nCountItem1 );
	BOOL	RemoveItemGuildBank( int nGuildId, CItemContainer< CItemElem  >*  GuildBank, int* nCountItem0, int* nCountItem1 );
	DWORD	GetRemoveItemPanya( int nItem0, int nItem1 );

#ifdef __ITEM_REMOVE_LIST
	void	InitConvItemDialog( void );
	void	UpdateConvItemDialog( char* pTitle, char* pString, char* pPosStirng, int nPos );
	BOOL	ConvItemConnectDB( CQuery* pQueryChar, CQuery* pQuerySave );
	DWORD	GetMaxInventory( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	DWORD	GetMaxGuildBank( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	CreateInvenBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	CreateGuildBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	DeleteInvenBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	DeleteGuildBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	GetInventoryBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	GetGuildBankView( char* pszSQL, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	GetUserInventoryBank( CMover* pMover, CQuery* pQueryChar );	
	BOOL	InventoryBankConv( char* pszSQL, CMover* pMover, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	GuildBankConv( char* pszSQL, int nGuildId, CItemContainer< CItemElem >* GuildBank, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	RemoveGuildBankList( char* pszSQL, int nGuildId, CItemContainer< CItemElem  >*  GuildBank, int* nCountItem0, int* nCountItem1 );
	BOOL 	RemoveInventoryBankList( char* pszSQL, CMover* pMover, int* nCountItem0, int* nCountItem1 );
	BOOL	RemoveGuildBankListPanya( char* pszSQL, int nGuildId, CQuery* pQueryChar, CQuery* pQuerySave, int nCountItem0, int nCountItem1 );
	BOOL	RemoveInventoryBankListPanya( char* pszSQL, CMover* pMover, CQuery* pQueryChar, CQuery* pQuerySave, int nCountItem0, int nCountItem1 );
	BOOL	SaveUserInventoryBank( char* pszSQL, CMover* pMover, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	SaveConvGuildBank( char* pszSQL, int nGuildId, CItemContainer< CItemElem >* GuildBank, CQuery* pQueryChar, CQuery* pQuerySave );
	BOOL	PiercingConfirmInventoryBank( CMover* pMover );
	BOOL	PiercingConfirmGuildBank( int nGuildId, CItemContainer< CItemElem >* GuildBank );
	BOOL	ConvItemStart( void );
#endif // __ITEM_REMOVE_LIST

#ifdef __RECOVER0816
	BOOL	RecoverCharacter( void );
#endif	// __RECOVER0816

#ifdef __INITITEMHITPOINT
	BOOL	InitItemHitPoint( void );
#endif	// __INITITEMHITPOINT

	BOOL	OpenWanted( CAr& ar );

	static	void	MakeQueryAddMail( char* szSql, CMail* pMail, u_long idReceiver );

private:
			CDbManager();
#ifdef __RT_1025
	void	LoadMessenger( CMover* pMover, CQuery* pQuery );
	void	AddMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	DeleteMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	UpdateMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
#else	// __RT_1025
	void	SaveMessengerFriend( CQuery *qry, CMover* pMover, char* szQuery );
public:
	BOOL	SetMessenger( void );
private:
	void	GetMessengerFriend( CMover* pMover, CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	RemoveFriend( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif	// __RT_1025
	BOOL	IsAbnormalPlayerData( CMover* pMover );
	BOOL	NormalizePlayerData( CMover* pMover );
	int		SelectTag( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, u_long uidPlayer, TAG_ENTRY* tags );
	void	WriteTag( CAr &ar, int count, const TAG_ENTRY* tags );
	void	InsertTag( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetStrTime( CTime *time, const char *strbuf );
	BOOL	VerifyString( const char* lpString, const char* lpFileName, int nLine, const char* lpName, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = NULL );
	BOOL	SN( void );
	void	LogPlayConnect(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogPlayDeath(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogLevelUp(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogServerDeath(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogUniqueItem(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogQuest(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogPkPvp( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	LogSchool( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	LogSkillPoint( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	LogItem(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogConcurrentUserNumber( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#if __VER >= 14 // __INSTANCE_DUNGEON
	void	LogInstanceDungeon( CQuery *pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif // __INSTANCE_DUNGEON
#ifdef __ERROR_LOG_TO_DB
	void	LogError( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif // __ERROR_LOG_TO_DB
#if __VER >= 15 // __GUILD_HOUSE
	void	LogGuildFurniture( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif // __GUILD_HOUSE

	void	SetPlayerName( CQuery* pQueryChar, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	SnoopGuild( CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	PutExpLog( CQuery* pQueryLog, CMover* pMover, char ch );
	void	QueryGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	InsertGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus ); 
	void	UpdateGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	BOOL	SendItemtoCharacter( int nSlot, CMover* pMover, CQuery *qry, CQuery *qry1, CQuery * qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	RemoveItemtoCharacter( CMover* pMover, CQuery *qry, CQuery *qry1, CQuery *qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	RemovePenya( CQuery *pQry, char* szSql, CMover* pMover, int nItem_Count, int nNo, char chState );
	SERIALNUMBER	RemoveItem( CQuery *pQry, char* szSql, int nNo, CMover* pMover, char* szItemName, int nItemCount, int nAbilityOption, int nItemResist, int nResistAbilityOption, char chState );
	SERIALNUMBER	RemoveItemBank( ItemProp* pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount );
#ifdef __REMOVEITEM_POCKET
	SERIALNUMBER	RemoveItemPocket( ItemProp* pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount );
#endif // __REMOVEITEM_POCKET

	void	SchoolReport( CQuery* pQuery, LPDB_OVERLAPPED_PLUS po );

	BOOL	GetSendItem( CQuery *pQry, __SendItemContents * pSendItemContents );
#if __VER >= 12 // __EXT_PIERCING
	SERIALNUMBER	SendItem( CQuery *pQry, char* szSql, CMover* pMover, __SendItemContents& SendItemContents );
#else // __EXT_PIERCING
	SERIALNUMBER	SendItem( CQuery *pQry, char* szSql, CMover* pMover, __SendItemContents SendItemContents );
#endif // __EXT_PIERCING
	SERIALNUMBER	RemoveItemInventory( ItemProp* pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount );
	BOOL	GetRemoveItem( CQuery *pQry, int &nNo, char* pOneItem, int &nItem_Count, int &nAbilityOption, int &nItemResist, int &nResistAbilityOption, char &chState );	

	BOOL	SendPenya( CQuery *pQry, char* szSql, CMover* pMover, int nPenya, int nNo );
	BOOL	SendItemDeleteQuery( CQuery *pQry, char* szSql, int nNo );
	BOOL	ItemLogQuery( CQuery *pQryLog, char *szQueryState, const LogItemInfo& info, u_long uIdPlayer, int nNo, char *szItemName );
	BOOL	RemoveItemDeleteQuery( CQuery *pQry, char* szSql, int nNo );

#ifdef __S_RECOMMEND_EVE
	void	RecommendEve( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif // __S_RECOMMEND_EVE

#if __VER >= 13 // __HONORABLE_TITLE			// 달인
	void	LogGetHonorTime(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
#endif	// __HONORABLE_TITLE			// 달인
	CQuery	m_qryPostProc;
	void	AddMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	RemoveMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	RemoveMailItem( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	RemoveMailGold( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	ReadMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov );
	void	DbQryMail( char* szSql, const MAIL_QUERYINFO & info );
	void	LogExpBox( CQuery *pQuery, LPDB_OVERLAPPED_PLUS pov );

#ifdef __S1108_BACK_END_SYSTEM
	void	GetGemeSettingtime( CQuery* pQuery, int nChat = 0 );
	BOOL	GetBaseGameSetting( CQuery* pQuery, char* lpstrTime = '\0' );
	void	GetMonsterRespawnSetting( CQuery* pQuery, int& nRemoveMaxRespawn, int aRemoveRespawn[] = NULL, char* lpstrTime = '\0' );
	void	GetMonsterPropSetting( CQuery* pQuery, char* lpstrTime = '\0' );
	BOOL	AddMonsterRespawn( MONSTER_RESPAWN BufMonsterRespawn );
	BOOL	RemoveMonsterRespawn( int nIndex, int &nMaxRemove, int aRemoveRespawn[] );
	BOOL	AddMonsterProp( MONSTER_PROP BufMonsterProp );
	BOOL	RemoveMonsterProp( MONSTER_PROP BufMonsterProp );
	BOOL	GetGMChat( CQuery* pQuery, char * lpstrTime, int& nChatCount );
#endif // __S1108_BACK_END_SYSTEM

	BOOL	OnWantedQuery( CQuery* pQuery, WANTED_QUERYINFO& info );
	void	SerializeWanted( CQuery* pQuery, CAr& ar );
	void	WantedOperation( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	BOOL	CanJoin( LPCTSTR szAccount, ACCOUNT_CACHE* pAccountCache );

	void	StartGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	InGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	OutGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ResultGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetPenyaGuildGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	GetPenyaPlayerGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
	void	ContinueGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

private:
	void	GetStrFromDBFormat( char* szDst, const char* szSrc, int& n );
	void	SetStrDBFormat( char* szDst, const char* szSrc );
#ifdef __PET_1024
	void	GetDBFormatStr( char* szDst, int nMaxLen, const char* szSrc );
	void	SetDBFormatStr( char* szDst, int nMaxLen, const char* szSrc );
#endif	// __PET_1024

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	void	LoginProtectCert( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );
#endif // __2ND_PASSWORD_SYSTEM
	
	u_long			m_idPlayer;
	HANDLE			m_hThreadGet[MAX_GETTHREAD_SIZE];
	HANDLE			m_hThreadPut[MAX_PUTTHREAD_SIZE];
	HANDLE			m_hSPThread;
	HANDLE			m_hCloseSPThread;
	HANDLE			m_hThreadUpdate[MAX_UPDATETHREAD_SIZE];
	HANDLE			m_hThreadGuild;
	HANDLE			m_hWait;
};

inline BOOL CDbManager::VerifyString( const char* lpString, const char* lpFileName, int nLine, const char* lpName, LPDB_OVERLAPPED_PLUS lpOverlapped )
{
	int len		= strlen( lpString );
	if( len > 0 && lpString[len-1] == '$' )
		return TRUE;
	WriteLog( "%s, %d\t%s", lpFileName, nLine, lpName );
	if( NULL != lpOverlapped )
		FreeRequest( lpOverlapped );
	return FALSE;
}

extern CDbManager & g_DbManager;

#endif	// _DB_MANAGER_H_