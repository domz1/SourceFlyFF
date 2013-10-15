#ifndef	__COOLTIMEMGR_H__
#define __COOLTIMEMGR_H__

const int MAX_COOLTIME_TYPE = 3;

struct ItemProp;

// 쿨타임 정보를 관리하는 클래스 
class CCooltimeMgr 
{
public:
	CCooltimeMgr();	
	virtual ~CCooltimeMgr();	

	static DWORD		GetGroup( ItemProp* pItemProp );

	BOOL				CanUse( DWORD dwGroup );
	void				SetTime( DWORD dwGroup, DWORD dwCoolTime );	
	DWORD				GetTime( DWORD dwGroup ) 
	{ 
		ASSERT( dwGroup > 0 );
		return m_times[ dwGroup - 1];
	}

	DWORD				GetBase( DWORD dwGroup ) 
	{ 
		ASSERT( dwGroup > 0 );
		return m_bases[ dwGroup - 1];
	}

protected:
	DWORD				m_times[MAX_COOLTIME_TYPE];		// 최종 사용가능 시각 	
	DWORD				m_bases[MAX_COOLTIME_TYPE];		// 이벤트 발생 시각 ( 아이템을 먹은 시각 ) 
};

#endif	__COOLTIMEMGR_H__