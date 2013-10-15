#pragma once

#if __VER >= 13 // __RAINBOW_RACE
#include "minigamebase.h"

class CMiniGameDiceplay : public CMiniGameBase
{
public:
	CMiniGameDiceplay(void);
	CMiniGameDiceplay( CMiniGameBase* pMiniGame );
	virtual ~CMiniGameDiceplay(void);

	virtual BOOL Excute( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket );

private:
	int m_nTargetNum;
};

#endif // __RAINBOW_RACE