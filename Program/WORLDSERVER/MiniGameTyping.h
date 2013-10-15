#pragma once

#if __VER >= 13 // __RAINBOW_RACE
#include "minigamebase.h"

class CMiniGameTyping :	public CMiniGameBase
{
public:
	CMiniGameTyping(void);
	CMiniGameTyping( CMiniGameBase* pMiniGame );
	virtual ~CMiniGameTyping(void);

	virtual BOOL Excute( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket );

private:
	void	MakeQuestion();	// 문제를 만든다.

	vector<string>	m_vecTyping;	// 만들어진 문제를 가질 멤버
};

#endif // __RAINBOW_RACE