#pragma once

#include <map>
#include <vector>
#include <string>


#if __VER >= 13 // __HONORABLE_TITLE

#include "defineHonor.h"

typedef struct HonorData
{
	int		nID;			// 인덱스
	int		nLGrouping;		// 대분류
	int		nSGrouping;		// 소분류
	int		nNeed;			// 타이틀 획득 조건
	int		nValue;			// 현재 수치
	CString	strTitle;		// 타이틀명
	HonorData() {Init();};
	~HonorData() {};
	void Init() {nID = -1;nLGrouping =  nSGrouping = nNeed = nValue = 0; strTitle.Empty(); };
}PHonorData, *PPHonorData;

#ifdef __CLIENT
struct EarnedTitle
{
	int	nId;
	CString	strTitle;
};
#endif	// __CLIENT

class CTitleManager
{

private:
	map<int, HonorData>		m_mapMonster;
	map<int, HonorData>		m_mapItem;
	map<int, HonorData>		m_mapEtc;
	map<int, HonorData>		m_mapAll;

public:
	CTitleManager();
	~CTitleManager();
	static	CTitleManager*	Instance();

	int		m_nCurrentTitleCount;
	BOOL	LoadTitle(LPCTSTR lpszFileName);
//	BOOL	IsQualified(int nId, int nGroup);
//	BOOL	Reset(int nId, int nGroup, int nNew);
	char*	GetTitle(int nId);
	int		GetNeedCount(int nId,int nGroup);
	int		GetIdxType(int nId);
	int		GetIdx(int nId,int nGroup);
	HonorData*		GetHonorDataByID(int nId,int nGroup);
	int     GetIdxByName(LPCTSTR chName);	
	int     GetIdxByGroup(int nLarge,int nSmall);	

#ifdef __CLIENT
	// 획득한 타이틀 인덱스
	vector<EarnedTitle>	m_vecEarned;

	BOOL	IsEarned(int nId);
	void	AddEarned(int nId);
	BOOL	RemoveEarned(int nId);
	void	InitEarned();
#endif	// __CLIENT

};





#endif	// __HONORABLE_TITLE