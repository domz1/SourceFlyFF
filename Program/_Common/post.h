#ifndef __POST_H__
#define	__POST_H__

#include <map>
#include <list>
using	namespace	std;

//	mulcom	BEGIN100420	메일 관련 사이즈 변경.
//#define	MAX_MAILTITLE	128
#define	MAX_MAILTITLE	32
//	mulcom	END100420	메일 관련 사이즈 변경.
#define	MAX_MAILTEXT	256

#ifdef __INTERNALSERVER
// 내부
#define MAX_KEEP_MAX_DAY		3   // 최대 보관일수 5일
#define MAX_KEEP_BASIC_DAY		2   // 기본 보관일수 1일
#else //__INTERNALSERVER
// 정섭
#define MAX_KEEP_MAX_DAY		3  // 최대 보관일수 15일
#define MAX_KEEP_BASIC_DAY		7	// 기본 보관일수 7일
#endif //__INTERNALSERVER

class CItemElem;
#ifdef __DBSERVER
class CMailBox;
#endif	// __DBSERVER
class CMail
{
public:
	CMail();
	CMail( u_long idSender, CItemElem* pItemElem, int nGold, char* szTitle, char* szText );
	virtual	~CMail();

	enum	{	mail,	item,	gold,	read	};

	void	Clear( void );
	DWORD	GetCost( void );
#ifdef __DBSERVER
	void	SetMailBox( CMailBox* pMailBox )	{	m_pMailBox	= pMailBox;	}
	CMailBox*	GetMailBox( void )	{	return m_pMailBox;	}
#endif	// __DBSERVER
	void	Serialize( CAr & ar, BOOL bData = TRUE );
public:
	u_long	m_nMail;
	u_long	m_idSender;
	CItemElem*	m_pItemElem;
	DWORD	m_nGold;
	time_t	m_tmCreate;
	BYTE	m_byRead;
	char	m_szTitle[MAX_MAILTITLE];
	char	m_szText[MAX_MAILTEXT];

	void	GetMailInfo( int* nKeepingDay, DWORD* dwKeepingTime = NULL );

	static	u_long	s_nMail;
#ifdef __DBSERVER
private:
	CMailBox*	m_pMailBox;
#endif	// __DBSERVER
};

#ifdef __DBSERVER
class CPost;
#endif	// __DBSERVER


typedef	std::vector<CMail*>			MailVector;
typedef MailVector::iterator		MailVectorItr;


class CMailBox : public MailVector
{
public:
	CMailBox();
	CMailBox( u_long idReceiver );
	virtual	~CMailBox();
	u_long	AddMail( CMail* pMail );
	void	Serialize( CAr & ar, BOOL bData = TRUE );
#ifdef __DBSERVER
	void	Write( CAr & ar );
#endif	// __DBSERVER
#ifdef __WORLDSERVER
	void	Read( CAr & ar );




	//////////////////////////////////////////////////////////////////////////
	void	ReadReq( CAr & ar );
	//////////////////////////////////////////////////////////////////////////



#endif	// __WORLDSERVER
	CMail*	GetMail( u_long nMail );
	BOOL	RemoveMail( u_long nMail );
	BOOL	RemoveMailItem( u_long nMail );
	BOOL	RemoveMailGold( u_long nMail );
	BOOL	ReadMail( u_long nMail );
	BOOL	IsStampedMailExists( void );
	void	Clear( void );
#ifdef __DBSERVER
	void	SetPost( CPost* pPost )	{	m_pPost	= pPost;	}
	CPost*	GetPost( void )	{	return m_pPost;	}
#endif	// __DBSERVER
	static	CMailBox*	GetInstance( void );
private:
	MailVectorItr Find( u_long nMail );
#ifdef __DBSERVER
	CPost*	m_pPost;
#endif	// __DBSERVER

public:
	u_long	m_idReceiver;
#ifdef __WORLDSERVER
	enum	{	nodata	= 0,	read	= 1,	data	= 2	};
	int		m_nStatus;
#endif	// __WORLDSERVER
};

//	mulcom	BEGIN100420	메일 관련 사이즈 변경.
//#define	MAX_MAIL	100
#define	MAX_MAIL	50
//	mulcom	END100420	메일 관련 사이즈 변경.
class CPost
{
public:
//	Constructions
	CPost();
	virtual	~CPost();
	void	Clear( void );
//	Operations
	u_long	AddMail( u_long idReceiver, CMail* pMail );
	CMailBox*	GetMailBox( u_long idReceiver );
	BOOL	AddMailBox( CMailBox* pMailBox );
	void	Serialize( CAr & ar, BOOL bData = TRUE );
#ifdef __DBSERVER
	map< u_long, CMail* >	m_mapMail4Proc;
	CMclCritSec	m_csPost;
	void	Process( void );
#endif	// __DBSERVER
	static	CPost*	GetInstance( void );
private:
	map< u_long, CMailBox* >	m_mapMailBox;
};
#endif	// __POST_H__