#ifndef __PATHOBJ_H__
#define __PATHOBJ_H__

typedef struct _VECINFO
{
	D3DXVECTOR3		m_vDir;			// 방향벡터
	FLOAT			m_fLength;		// 거리
	DWORD			m_dwState;		// 모션 종류
	DWORD			m_dwDelay;		// 정지시간
	CTimer			m_ctStop;       // 정지시간
} _VECINFO;

class CPatrolPath
{
	map< DWORD, vector<_VECINFO> > m_mapPatrolPath;
	map< DWORD, BOOL	 > m_bReturn;
	
public:
	CPatrolPath();
	~CPatrolPath();
	
	static	CPatrolPath*				GetInstance( void );
	
	BOOL			LoadPatrol			( LPCTSTR szFileName );
	void			AddPatrolIndex		( DWORD dwIndex );
	BOOL			IsFirstPath			( DWORD dwIndex );
	void			Clear				();
	void			SetReturn			( DWORD dwIndex, BOOL bReturn );
	void			AddPatrolPath		( DWORD dwIndex, _VECINFO vecInfo );
	void			GetNextPosInfo		( CObj* pObj, const D3DXVECTOR3 vPos, D3DXVECTOR3& vDest, _VECINFO& _vecInfo );
};




#endif //__PATHOBJ_H__