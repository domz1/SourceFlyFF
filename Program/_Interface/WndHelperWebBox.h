#ifndef __WND_HELPER_WEB_BOX_H__
#define __WND_HELPER_WEB_BOX_H__

#ifdef __NEW_WEB_BOX
#ifdef __CLIENT

class CWndHelperWebBox : public CWndNeuz
{
public:
	enum { TASKBAR_HEIGHT = 48 };

public:
	CWndHelperWebBox( void );
	~CWndHelperWebBox( void );

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL Process( void );
	virtual HRESULT RestoreDeviceObjects( void );

public:
	void Release( void );
};

#endif // __CLIENT
#endif // __NEW_WEB_BOX

#endif // __WND_HELPER_WEB_BOX_H__