#ifndef __WND_MAIL_REQUESTING_BOX_H__
#define __WND_MAIL_REQUESTING_BOX_H__

#ifdef __MAIL_REQUESTING_BOX
#ifdef __CLIENT
class CWndMailRequestingBox : public CWndNeuz
{
public:
	enum { REQUEST_BUTTON_WAITING_TIME = 5 };
	enum { DESTRUCTION_TIME = 3 };

public:
	CWndMailRequestingBox( void );
	~CWndMailRequestingBox( void );

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	BOOL Process( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

private:
	DWORD m_timeWaitingRequest;
	int m_nDestroyCounter;
};
#endif // __CLIENT
#endif // __MAIL_REQUESTING_BOX

#endif // __WND_MAIL_REQUESTING_BOX_H__