#ifndef __COUPLEHELPER_H
#define	__COUPLEHELPER_H

#include "ar.h"
#include "couple.h"

class CCoupleHelper
{
private:
	CCoupleHelper();
public:
	virtual	~CCoupleHelper();
	static	CCoupleHelper*	Instance();
	void	Clear();
	BOOL	Initialize();
	void	OnCouple( CAr & ar );
	void	OnProposeResult( CAr & ar );
	void	OnCoupleResult( CAr & ar );
	void	OnDecoupleResult();
#if __VER >= 13 // __COUPLE_1202
	void	OnAddCoupleExperience( CAr & ar );
#endif	// __COUPLE_1202
	CCouple*	GetCouple()		{	return m_pCouple;	}
private:
	CCouple*	m_pCouple;
};
#endif	// __COUPLEHELPER_H