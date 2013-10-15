#ifndef		__YUTIL_H__
#define		__YUTIL_H__

class CFLASH
{
	BOOL	m_bAction;
	BOOL	m_bFade;
	int		m_nAlphaCount;
	int		m_nAlphaAcc;
	int     m_nTick;
	DWORD	m_dwTime;
	DWORD	m_dwEndTime;	
	DWORD	m_dwStopTime;
	BOOL	m_bStopTime;
	BOOL	m_bRunTime;
	
	
public:
	CFLASH();
	~CFLASH();
	
	void	Init();
	void    RenderFlash( C2DRender* p2DRender );
	void	SetTime( DWORD dwStopTime, DWORD dwTime, BOOL bAction );
	void	SetAction( BOOL bAction );
};


BOOL		IsForceAttack();
int			Point_In_Poly( vector<CPoint> vecMap, CPoint test_point );


#endif //__YUTIL_H__