#ifndef __LAYERED_LINK_MAP_H
#define	__LAYERED_LINK_MAP_H

#include <map>
using	namespace	std;

const	int	nDefaultLayer	= 0;	// 변경 가능성이 없는 기본 계층이다.
const	int	nRevivalLayer	= 0;	// 부활이 계층으로 구분되면 해당 상수값 대신 변수를 사용한다.
const	int	nTempLayer	= 0;	// 기획에 따라 변경될 가능성이 있는 임시 계층 값이다.

class CLinkMap;
typedef	map<int, CLinkMap*>	MLM;
class CLayeredLinkMap
{
public:
	CLayeredLinkMap();
	virtual	~CLayeredLinkMap();
public:
	void	Init( int nLandWidth, int nLandHeight, int nView, BOOL bBaseLayer, int nMPU );
	void	Release();
	int	GetLinkWidth( DWORD dwLinkType, int nLinkLevel, int nLayer );
	CObj**	GetObj( DWORD dwLinkType, DWORD dwLinkLevel, int nLayer );
	DWORD	CalcLinkLevel( CObj* pObj, float fObjWidth, int nLayer );
	int	GetMaxLinkLevel( DWORD dwLinkType, int nLayer );
	void	SetMaxLinkLevel( DWORD dwLinkType, int nLevel, int nLayer );
	BOOL	InsertObjLink( CObj* pObj );
	BOOL	RemoveObjLink2( CObj* pObj );
	BOOL	RemoveObjLink( CObj* pObj );
	CObj*	GetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, int nLayer );
	BOOL	SetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj, int nLayer );
	void	AddItToView( CCtrl* pCtrl );
	void	ModifyView( CCtrl* pCtrl );

	BOOL	CreateLinkMap( int nLayer );
	CLinkMap*	GetLinkMap( int nLayer );
	void	DeleteLinkMap( int nLayer );
	void	Process( CWorld* pWorld );
#if __VER >= 15 // __GUILD_HOUSE
	void	Invalidate( int nLayer, BOOL bInvalid );
#else // __GUILD_HOUSE
	void	Invalidate( int nLayer );
#endif // __GUILD_HOUSE
private:
	MLM		m_mapLinkMap;
	int	m_nLandWidth;
	int	m_nLandHeight;
	int	m_nView;

	int m_iMPU;		//gmpbigsun
};

#endif	// __LAYERED_LINK_MAP_H