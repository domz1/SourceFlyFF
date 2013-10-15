#ifndef __WND_USER_MARK_NAME_CHANGER_H__
#define __WND_USER_MARK_NAME_CHANGER_H__

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
class CWndUserMarkNameChanger : public CWndNeuz
{
public:
	CWndUserMarkNameChanger( void );
	~CWndUserMarkNameChanger( void );

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

public:
	void SetInfo( DWORD dwUserMarkPositionInfoID, const CString& strName );

private:
	enum { USER_MARK_NAME_MAX_LENGTH = 16 };

private:
	DWORD m_dwUserMarkPositionInfoID;
};
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#endif // __WND_USER_MARK_NAME_CHANGER_H__