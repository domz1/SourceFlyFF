#ifndef __MISC_H__
#define	__MISC_H__

#ifdef __TRAFIC_1218
#include "ar.h"
#endif	// __TRAFIC_1218 

typedef	struct	tagSERVER_DESC
{
	DWORD	dwParent;
	long	lCount;
	DWORD	dwID;
	char	lpName[36];
	char	lpAddr[16];
	BOOL	b18;
	long	lEnable;
	long	lMax;
	tagSERVER_DESC()
		{
			dwParent	= NULL_ID;
			lCount	= 0;
			dwID	= NULL_ID;
			*lpName		= '\0';
			*lpAddr		= '\0';
			b18	= 0x00;
			lEnable		= 0;
			lMax	= 0;
		}
}
SERVER_DESC, *LPSERVER_DESC;

// 쪽지 기능관련 스트럭쳐 
struct TAG_ENTRY
{
	u_long		idFrom;				// 누가 
	DWORD		dwDate;				// 언제 
	char		szString[256];		// 쪽지 내용 
};

const int MAX_TAGS = 20;			// 최대 쪽지 저장치 

// 길드 스탯 enum
enum GUILD_STAT
{
	GUILD_STAT_LOGO,
	GUILD_STAT_PXPCOUNT,
	GUILD_STAT_PENYA,
	GUILD_STAT_NOTICE,
};

const int WANTED_MSG_MAX = 20;

// 포상금 항목 
struct WANTED_ENTRY
{
	char		szPlayer[64];						// 현상범 이름 
	long		nEnd;								// 마감일 
	__int64		nGold;								// 현상금액 
	char		szMsg[WANTED_MSG_MAX + 1];			// 십자평 
};


#if __VER >= 11 // __MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
const int GUILDBANKLOGVIEW_MAX = 100;

struct __GUILDBANKLOG_ENTRY
{
	char		szPlayer[42];						// 케릭터 이름 
	long		nDate;								// 날짜 
	long		nItemID;								// 아이템아이디,돈
	long		nItemAbilityOption;								// 아이템아이디,돈
	long		nItemCount;								// 아이템숫자
	__GUILDBANKLOG_ENTRY()
	{
		*szPlayer		= '\0';
		nDate = 0;	
		nItemID = 0;	
		nItemAbilityOption = 0;
		nItemCount = 0;
	};
};
#endif //__MA_VER11_04	// 길드 창고 로그 기능 world,database,neuz
#if __VER >= 11 // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz
const int SEALCHAR_MAX = 3;

struct __SEALCHAR_ENTRY
{
	char		szPlayer[42];						// 케릭터 이름 
	u_long		idPlayer;							//  
	long		nPlayerSlot;								//
	__SEALCHAR_ENTRY()
	{
		*szPlayer		= '\0';
		idPlayer = 0;	
		nPlayerSlot = 0;	
	};
};
#endif // __MA_VER11_05	// 케릭터 봉인 거래 기능 world,database,neuz


#ifdef __TRAFIC_1218
class CTraficLog	: public map<BYTE, int>
{
private:
	DWORD	m_dwTotalPacket;
	DWORD	m_dwTotalBytes;
#ifndef __CLIENT
	static	list<CTraficLog*>	sm_lspTraficLog;
	static	CMclCritSec		sm_lockLspTraficLog;
#endif	// __CLIENT
public:
//	Constructions
	CTraficLog()	
		{
			m_dwTotalPacket	= m_dwTotalBytes	= 0;
		}
	virtual	~CTraficLog()
		{
			Clear();
		}
//	Operations
	void	AddTotal( DWORD dwBytes )
		{
			m_dwTotalPacket++;
			m_dwTotalBytes	+= dwBytes;
		}
	void	Clear( void )
		{
			clear();
			m_dwTotalPacket	= m_dwTotalBytes	= 0;
		}
	void	Add( BYTE byHdr )
		{
			map<BYTE, int>::iterator	i	= find( byHdr );
			if( i == end() )
				insert( map<BYTE, int>::value_type( byHdr, 1 ) );
			else
				i->second++;
		}
	void	Serialize( CAr & ar )
		{
			if( ar.IsStoring() )
			{
				ar << m_dwTotalPacket << m_dwTotalBytes;
				ar << (int)size();
				for( map<BYTE, int>::iterator i = begin(); i != end(); ++i )
					ar << i->first << i->second;
			}
			else	// load
			{
				Clear();
				ar >> m_dwTotalPacket >> m_dwTotalBytes;
				int nSize;
				ar >> nSize;
				BYTE byHdr;
				int nCount;
				for( int i = 0; i < nSize; i++ )
				{
					ar >> byHdr >> nCount;
					insert( map<BYTE, int>::value_type( byHdr, nCount ) );
				}
			}
	}
#ifndef __CLIENT
	static	void	AddLog( CTraficLog* pTraficLog )
		{
			CMclAutoLock	Lock( sm_lockLspTraficLog );
			sm_lspTraficLog.push_back( pTraficLog );
		}
	static	void	PrintLog( const char* szFileName )
		{
			Error( "----------------------------------------" );
			CMclAutoLock	Lock( sm_lockLspTraficLog );
			FILE* f	= fopen( szFileName,"a" );
			if( f )
			{
				for( list<CTraficLog*>::iterator i1 = sm_lspTraficLog.begin(); i1 != sm_lspTraficLog.end(); ++i1 )
				{
					CTraficLog* pTraficLog	= *i1;
					fprintf( f, "total bytes : %d\ttotal packets : %d\r\n", pTraficLog->m_dwTotalBytes, pTraficLog->m_dwTotalPacket );
					for( map<BYTE, int>::iterator i2 = pTraficLog->begin(); i2 != pTraficLog->end(); ++i2 )
						fprintf( f, "\t0x%02x : %d\t", i2->first, i2->second );
					fprintf( f, "\r\n" );
				}
				fclose( f );
			}
			ClearLog();
		}

	static	void	ClearLog( void )
		{
			for( list<CTraficLog*>::iterator i = sm_lspTraficLog.begin(); i != sm_lspTraficLog.end(); ++i )
				safe_delete( *i );
			sm_lspTraficLog.clear();
		}
#endif	// __CLIENT
};
#endif	// __TRAFIC_1218

#endif	// __MISC_H__