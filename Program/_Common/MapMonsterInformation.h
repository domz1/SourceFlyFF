#ifndef __MAP_MONSTER_INFORMATION_H__
#define __MAP_MONSTER_INFORMATION_H__

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
class CMapMonsterInformation
{
public:
	CMapMonsterInformation( void );
	~CMapMonsterInformation( void );

public:
	void SetMonsterIDNumber( int nMonsterIDNumber );
	int GetMonsterIDNumber( void ) const;
	void InsertMonsterID( DWORD dwMonsterID );
	DWORD GetMonsterID( int nIndex ) const;
	vector< DWORD >& GetMonsterIDVector( void );
	void SetDropItemID( DWORD dwDropItemID );
	DWORD GetDropItemID( void ) const;
	void SetIconPositionRect( const CRect& rectIconPosition );
	const CRect& GetIconPositionRect( void ) const;

private:
	int m_nMonsterIDNumber;
	vector< DWORD > m_vecMonsterID;
	DWORD m_dwDropItemID;
	CRect m_rectIconPosition;
};
typedef vector< CMapMonsterInformation* > MapMonsterInformationVector;
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#endif // __MAP_MONSTER_ICON_INFORMATION_H__
