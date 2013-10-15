#ifndef __RAINBOW_NPC_INFORMATION_H__
#define __RAINBOW_NPC_INFORMATION_H__

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT

class CRainbowNPCInformation
{
public:
	CRainbowNPCInformation( void );
	~CRainbowNPCInformation( void );

public:
	void SetMapID( int nMapID );
	int GetMapID( void ) const;
	void SetWorldPositionRect( const CRect& rectWorldPosition );
	const CRect& GetWorldPositionRect( void ) const;
	void SetRegionPositionRect( const CRect& rectRegionPosition );
	const CRect& GetRegionPositionRect( void ) const;

private:
	int m_nMapID;
	CRect m_rectWorldPosition;
	CRect m_rectRegionPosition;
};
typedef vector< CRainbowNPCInformation* > RainbowNPCInformationVector;

#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#endif // __RAINBOW_NPC_INFORMATION_H__