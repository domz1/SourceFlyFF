// Region.h: interface for the CCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGION_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_)
#define AFX_REGION_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000  

#define MAX_REGIONDESC 256
#define MAX_REGIONTITLE 256


struct REGIONELEM
{
	FLOAT         m_fTeleAngle;
	D3DXVECTOR3   m_vTeleWorld;  
	D3DXVECTOR3   m_vPos;  
	TCHAR         m_szName[ 32];
	TCHAR         m_szKey[ 32];	
	DWORD         m_dwIdTeleWorld; // 
	TCHAR         m_szSound[ 32];
	TCHAR         m_szScript[ 32];
	DWORD         m_dwAttribute;
	CRect         m_rect;
	bool          m_bInside;
	char          m_cDescSize;

	TCHAR         m_szDesc [ MAX_REGIONDESC  ];
	TCHAR         m_szTitle[ MAX_REGIONTITLE ];
	union
	{
		bool          m_bDirectMusic;
		bool          m_bUnit; // structure type에서 unit인가?
	};

	bool          m_bPlayDirectMusic;
	bool          m_bTargetKey;

	union
	{
		DWORD		  m_dwStructure; // structure type 
		DWORD         m_dwIdMusic;
	};
	DWORD	m_dwWorldId;
	DWORD	m_dwIndex;
	
	u_long	m_uItemId;
	UINT	m_uiItemCount;
	UINT	m_uiMinLevel;
	UINT	m_uiMaxLevel;
	int		m_iQuest;
	int		m_iQuestFlag;
	int		m_iJob;
	int		m_iGender;
	BOOL	m_bCheckParty;
	BOOL	m_bCheckGuild;
	BOOL	m_bChaoKey;
};
typedef REGIONELEM*  LPREGIONELEM;

/*----------------------------------------*/
#define	MAX_REGIONELEM	256
class CRegionElemArray
{
private:
	DWORD	m_cbRegionElem;
	REGIONELEM	m_aRegionElem[MAX_REGIONELEM];
public:
//	Constructions
	CRegionElemArray()	{	m_cbRegionElem	= 0;	}
	~CRegionElemArray()		{}
//	Operations
	int	GetSize( void )	{	return m_cbRegionElem;	}
	void	AddTail( const LPREGIONELEM lpRegionElem );
	LPREGIONELEM	GetAt( int nIndex );
};

inline void CRegionElemArray::AddTail( const LPREGIONELEM lpRegionElem )
{
	ASSERT( m_cbRegionElem < MAX_REGIONELEM );
	memcpy( &m_aRegionElem[m_cbRegionElem++], lpRegionElem, sizeof(REGIONELEM) );
}
inline LPREGIONELEM CRegionElemArray::GetAt( int nIndex )
{
	if( nIndex >= 0 && nIndex < MAX_REGIONELEM )
		return &m_aRegionElem[nIndex];
	return NULL;
}
/*----------------------------------------*/
#endif // !defined(AFX_REGION_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_)
