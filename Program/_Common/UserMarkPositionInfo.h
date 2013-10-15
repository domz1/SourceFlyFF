#ifndef __USER_MARK_POSITION_INFO_H__
#define __USER_MARK_POSITION_INFO_H__

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
class CUserMarkPositionInfo
{
public:
	CUserMarkPositionInfo( void );
	~CUserMarkPositionInfo( void );

public:
	void SetID( DWORD dwID );
	DWORD GetID( void ) const;
	void SetName( const CString& strName );
	const CString& GetName( void ) const;
	void SetPositionX( FLOAT fPositionX );
	FLOAT GetPositionX( void ) const;
	void SetPositionY( FLOAT fPositionY );
	FLOAT GetPositionY( void ) const;

private:
	DWORD m_dwID;
	CString m_strName;
	FLOAT m_fPositionX;
	FLOAT m_fPositionY;
};
typedef list< CUserMarkPositionInfo* > UserMarkPositionInfoList;
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#endif // __MAP_POSITION_INFO_H__