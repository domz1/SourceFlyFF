
#include "stdafx.h"
#include "..\_Common\honor.h"

//#include "DPSrvr.h"
//extern CDPSrvr g_DPSrvr;

#if __VER >= 13 // __HONORABLE_TITLE

CTitleManager::CTitleManager()
{
	m_mapMonster.clear();
	m_mapItem.clear();
	m_mapEtc.clear();
	m_mapAll.clear();
	m_nCurrentTitleCount = 0;
#ifdef __CLIENT
	m_vecEarned.clear();
#endif	// __CLIENT
}


CTitleManager::~CTitleManager()
{

}

CTitleManager* CTitleManager::Instance( void )
{
	static CTitleManager tManager;
	return &tManager;
}

BOOL	CTitleManager::LoadTitle(LPCTSTR lpszFileName)
{
	HonorData	TempData;
	CScript		script;


	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;

	script.tok = 0;

	TempData.Init();
	TempData.nID		= script.GetNumber(); // ID

	while(script.tok != FINISHED)
	{
		
		TempData.nLGrouping = script.GetNumber();	// 대분류
		TempData.nSGrouping = script.GetNumber();	// 소분류
		TempData.nNeed		= script.GetNumber();	// 획득 조건
		script.GetToken();
		TempData.strTitle	= script.Token;	// 타이틀 이름
		switch(TempData.nLGrouping)
		{
			case HI_HUNT_MONSTER:
				if(m_mapMonster.size() < MONSTER_TITLE_MAX)
				{
					m_mapMonster.insert(map<int, HonorData>::value_type(TempData.nSGrouping, TempData));
					m_mapAll.insert(map<int, HonorData>::value_type(TempData.nID, TempData));
				}
				else
				{
					Error("Too many title about monster!");
					return FALSE;
				}
				break;

			case HI_USE_ITEM:
				if(m_mapItem.size() < MONSTER_TITLE_MAX)
				{
					m_mapItem.insert(map<int, HonorData>::value_type(TempData.nSGrouping, TempData));
					m_mapAll.insert(map<int, HonorData>::value_type(TempData.nID, TempData));
				}
				else
				{
					Error("Too many title about item!");
					return FALSE;
				}
				break;

			default:
				m_mapEtc.insert(map<int, HonorData>::value_type(TempData.nSGrouping, TempData));
				m_mapAll.insert(map<int, HonorData>::value_type(TempData.nID, TempData));
				break;
		};
		m_nCurrentTitleCount++;
		TempData.Init();
		TempData.nID		= script.GetNumber(); // ID
	}
	
	return TRUE;
}
/*
BOOL	CTitleManager::AddValue( int nId, int nGroup, int nAmount)	// 수치 카운팅이나 시간 더하기 할때 쓴다.
{

	map<int, HonorData>::iterator iter;

	switch(nGroup)
	{
		case HI_HUNT_MONSTER:
			iter = m_mapMonster.find(nId); // 몬스터 아이디로 검색
			if(iter != m_mapMonster.end()) 
			{
				iter->second.nValue += nAmount;
				if(iter->second.nNeed <= iter->second.nValue)
					return HM_SATISFIED;
				else
					return iter->second.nValue;
			}
			else
				return HM_NONE;
			break;

		case HI_USE_ITEM:
			iter = m_mapItem.find(nId);	// 아이템 아이디로 검색
			if(iter != m_mapItem.end()) 
			{
				iter->second.nValue += nAmount;
				if(iter->second.nNeed <= iter->second.nValue)
					return HM_SATISFIED;
				else
					return iter->second.nValue;
			}
			else
				return HM_NONE;
			break;
		
		default:
			for(iter = m_mapEtc.begin(); iter != m_mapEtc.end(); ++iter) // 그외는 1대1 탐색..갯수가 얼마안되서 무리 없을듯..
			{
				if(iter != m_mapEtc.end()) 
				{
					iter->second.nValue += nAmount;
					if(iter->second.nNeed <= iter->second.nValue)
						return HM_SATISFIED;
					else
						return iter->second.nValue;
				}
				else
					return HM_NONE;
			}
			break;
	};

	return HM_NONE;

}*/

/*
BOOL	CTitleManager::IsQualified(int nId, int nGroup)		// 자격 획득 여부만 확인할 때 쓴다. 
{

	map<int, HonorData>::iterator iter;

	switch(nId)
	{
		case HI_HUNT_MONSTER:
			iter = m_mapMonster.find(nId); // 몬스터 아이디로 검색
			if(iter != m_mapMonster.end()) 
			{
				if(iter->second.nNeed <= iter->second.nValue)
					return TRUE;
				else
					return iter->second.nValue;
			}
			else
				return FALSE;
			break;

		case HI_USE_ITEM:
			iter = m_mapItem.find(nId);	// 아이템 아이디로 검색
			if(iter != m_mapItem.end()) 
			{
				if(iter->second.nNeed <= iter->second.nValue)
					return TRUE;
				else
					return iter->second.nValue;
			}
			else
				return FALSE;
			break;

		default:
			for(iter = m_mapEtc.begin(); iter != m_mapEtc.end(); ++iter) // 그외는 1대1 탐색..갯수가 얼마안되서 무리 없을듯..
			{
				if(iter != m_mapEtc.end()) 
				{
					if(iter->second.nNeed <= iter->second.nValue)
						return TRUE;
					else
						return iter->second.nValue;
				}
				else
					return FALSE;
			}
			break;
	};
	return FALSE;

}
*/
/*
BOOL	CTitleManager::Reset(int nId, int nGroup, int nNew)	// 말그대로 현재값을 초기화할때나, 자격획득 같은걸 세팅할때 쓴다.
{

	map<int, HonorData>::iterator iter;

	switch(nGroup)
	{
		case HI_HUNT_MONSTER:
			iter = m_mapMonster.find(nId); // 몬스터 아이디로 검색
			if(iter != m_mapMonster.end()) 
			{
				iter->second.nValue = nNew;
				if(iter->second.nNeed <= iter->second.nValue)
					return HM_SATISFIED;
				else
					return iter->second.nValue;
			}
			else
				return HM_NONE;
			break;

		case HI_USE_ITEM:
			iter = m_mapItem.find(nId);	// 아이템 아이디로 검색
			if(iter != m_mapItem.end()) 
			{
				iter->second.nValue = nNew;
				if(iter->second.nNeed <= iter->second.nValue)
					return HM_SATISFIED;
				else
					return iter->second.nValue;
			}
			else
				return HM_NONE;
			break;
		
		default:
			for(iter = m_mapEtc.begin(); iter != m_mapEtc.end(); ++iter) // 그외는 1대1 탐색..갯수가 얼마안되서 무리 없을듯..
			{
				if(iter != m_mapEtc.end()) 
				{
					iter->second.nValue = nNew;
					if(iter->second.nNeed <= iter->second.nValue)
						return HM_SATISFIED;
					else
						return iter->second.nValue;
				}
				else
					return HM_NONE;
			}
			break;
	};

	return HM_NONE;

}
*/

char	*CTitleManager::GetTitle(int nId)		// 타이틀을 반환
{
	map<int, HonorData>::iterator iter;
	if(nId == -1) return NULL; 
	//int nGroup = GetIdxType(nId);
	iter = m_mapAll.find(nId);	// 몬스터 아이디로 검색
	if(iter != m_mapAll.end()) 
	{
		return (LPSTR)(LPCSTR)iter->second.strTitle;
	}/*
	switch(nGroup)
	{
		
		case HI_HUNT_MONSTER:
			iter = m_mapMonster.find(nId); // 몬스터 아이디로 검색
			if(iter != m_mapMonster.end()) 
			{
				return (LPSTR)(LPCSTR)iter->second.strTitle;
			}
			else
				return NULL;
			break;

		case HI_USE_ITEM:
			iter = m_mapItem.find(nId); // 몬스터 아이디로 검색
			if(iter != m_mapItem.end()) 
			{
				return (LPSTR)(LPCSTR)iter->second.strTitle;
			}
			else
				return NULL;
			break;
		
		default:
			for(iter = m_mapEtc.begin(); iter != m_mapEtc.end(); ++iter) // 그외는 1대1 탐색..갯수가 얼마안되서 무리 없을듯..
			{
				if(iter != m_mapEtc.end()) 
				{
					return (LPSTR)(LPCSTR)iter->second.strTitle;
				}
				else
					return NULL;
			}
			break;
	};*/

	return NULL;

}

int	CTitleManager::GetNeedCount(int nId, int nGroup)		// 획득가능 카운트 리턴
{

	map<int, HonorData>::iterator iter;

	switch(nGroup)
	{
		case HI_HUNT_MONSTER:
			iter = m_mapMonster.find(nId); // 몬스터 아이디로 검색
			if(iter != m_mapMonster.end()) 
			{
				return iter->second.nNeed;
			}
			else
				return -1;
			break;

		case HI_USE_ITEM:
			iter = m_mapItem.find(nId);	// 아이템 아이디로 검색
			if(iter != m_mapItem.end()) 
			{
				return iter->second.nNeed;
			}
			else
				return -1;
			break;

		case -1:		// 전부다 검색
			iter = m_mapAll.find(nId);	// 몬스터 아이디로 검색
			if(iter != m_mapAll.end()) 
			{
				return iter->second.nNeed;
			}
			else
				return -1;
			break;

		default:
			iter = m_mapEtc.find(nId);	// 아이템 아이디로 검색
			if(iter != m_mapEtc.end()) 
			{
				return iter->second.nNeed;
			}
			else
				return -1;
			break;
	};
	return -1;
}

HonorData*		CTitleManager::GetHonorDataByID(int nId,int nGroup)
{
	map<int, HonorData>::iterator iter;

	switch(nGroup)
	{
		case HI_HUNT_MONSTER:
			iter = m_mapMonster.find(nId); // 몬스터 아이디로 검색
			if(iter != m_mapMonster.end()) 
			{
				return &iter->second;
			}
			else
				return NULL;
			break;

		case HI_USE_ITEM:
			iter = m_mapItem.find(nId);	// 아이템 아이디로 검색
			if(iter != m_mapItem.end()) 
			{
				return &iter->second;
			}
			else
				return NULL;
			break;

		default:
			iter = m_mapEtc.find(nId);	// 아이템 아이디로 검색
			if(iter != m_mapEtc.end()) 
			{
				return &iter->second;
			}
			else
				return NULL;
			break;
	};
	return NULL;
}
int	CTitleManager::GetIdx(int nId, int nGroup)		// 
{

	map<int, HonorData>::iterator iter;

	switch(nGroup)
	{
		case HI_HUNT_MONSTER:
			iter = m_mapMonster.find(nId); // 몬스터 아이디로 검색
			if(iter != m_mapMonster.end()) 
			{
				return iter->second.nID;
			}
			else
				return -1;
			break;

		case HI_USE_ITEM:
			iter = m_mapItem.find(nId);	// 아이템 아이디로 검색
			if(iter != m_mapItem.end()) 
			{
				return iter->second.nID;
			}
			else
				return -1;
			break;

		default:
			iter = m_mapEtc.find(nId);	// 아이템 아이디로 검색
			if(iter != m_mapEtc.end()) 
			{
				return iter->second.nID;
			}
			else
				return -1;
			break;
	};
	return -1;
}


int		CTitleManager::GetIdxType(int nId)
{
	map<int, HonorData>::iterator iter;

	iter = m_mapAll.find(nId); // 몬스터 아이디로 검색

	if(iter != m_mapAll.end()) 
	{
		return iter->second.nLGrouping;
	}
	
	return -1;
}


int CTitleManager::GetIdxByName(LPCTSTR chName)		// 
{
	map<int, HonorData>::iterator iter;

	for(iter = m_mapAll.begin(); iter != m_mapAll.end(); ++iter) // 그외는 1대1 탐색..갯수가 얼마안되서 무리 없을듯..
	{
		HonorData	TempData = iter->second;
		if(lstrcmp(TempData.strTitle,chName) == 0 )
           return TempData.nID;
	};
	return -1;
	
}
int     CTitleManager::GetIdxByGroup(int nLarge,int nSmall)
{
	map<int, HonorData>::iterator iter;

	for( iter = m_mapEtc.begin(); iter != m_mapEtc.end(); ++iter )
	{
		HonorData	TempData = iter->second;
		if( TempData.nLGrouping == nLarge && TempData.nSGrouping == nSmall  )
			return TempData.nID;
	}
	return -1;
}	
#ifdef __CLIENT
BOOL	CTitleManager::IsEarned(int nId)
{
	vector<EarnedTitle>::iterator	iter;
	for(iter = m_vecEarned.begin(); iter != m_vecEarned.end(); ++iter)
	{
		if(iter->nId == nId) return TRUE;
	}
	return FALSE;
}

void	CTitleManager::AddEarned(int nId)
{
	EarnedTitle Temp;
	Temp.nId		= nId;
	Temp.strTitle	= GetTitle(nId); 
	m_vecEarned.push_back(Temp);
}

BOOL	CTitleManager::RemoveEarned(int nId)
{
	vector<EarnedTitle>::iterator	iter;
	for(iter = m_vecEarned.begin(); iter != m_vecEarned.end(); ++iter)
	{
		if(iter->nId == nId)
		{
			m_vecEarned.erase(iter);
			return TRUE;
		}
	}
	return FALSE;
}

void	CTitleManager::InitEarned()
{
	m_vecEarned.clear();
}

#endif	// __CLIENT

#endif	// __HONORABLE_TITLE