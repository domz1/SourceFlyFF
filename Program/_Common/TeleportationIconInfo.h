#ifndef __TELEPORTATION_ICON_INFO_H__
#define __TELEPORTATION_ICON_INFO_H__

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
class TeleportationIconInfo
{
public:
	TeleportationIconInfo( void );
	~TeleportationIconInfo( void );

public:
	void SetIndex( int nIndex );
	int GetIndex( void ) const;
	void SetIconPoint( const CPoint& pointIcon );
	const CPoint& GetIconPoint( void ) const;

private:
	int m_nIndex;
	CPoint m_pointIcon;
};
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#endif // __TELEPORTATION_ICON_INFO_H__