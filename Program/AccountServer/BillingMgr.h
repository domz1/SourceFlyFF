#ifndef __BILLINGMGR_H__
#define	__BILLINGMGR_H__

const int	MAX_DPBILLING = 32;

enum BILLING_ENUM
{
	BID_FREEPASS,			// 무료 유저의 타입 번호 (CAccount의 m_fCheck와 비교한다.) 
	BID_IP,					// 빌링서버 아이피 
	BID_PORT,				// 빌링서버 포트 
};

class	CAccount;
class	CQuery;
struct	tagDB_OVERLAPPED_PLUS;

class CBillingMgr
{
public:
	virtual bool		Init( HWND hWnd ) = 0;
	virtual void		Release() = 0;
	virtual BYTE		CheckAccount( int nType, DWORD dwKey, const char* szAccount, const char* szAddr ) = 0;
	virtual BOOL		PreTranslateMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) = 0; 
	virtual BOOL		SetConfig( BILLING_ENUM id, DWORD data ) = 0;
	virtual void		OnTimer( CAccount* pAccount ) = 0;
	virtual void		OnDBQuery( CQuery& query, tagDB_OVERLAPPED_PLUS* pOV ) = 0;
	virtual void		NotifyLogout( LPCTSTR lpszAccount, DWORD dwSession ) = 0;
};

BOOL			CreateBillingMgr();
CBillingMgr*	GetBillingMgr();
char*			GetBillingPWD();
//BOOL			SetBillingPWD( unsigned char* pEncryptedPWD );

extern			char* StrCpyExcludeNull( char* dst, const char* src );
extern			void AppendSpace( char* pCur, char* pEnd );
#endif	// __BILLINGMGR_H__