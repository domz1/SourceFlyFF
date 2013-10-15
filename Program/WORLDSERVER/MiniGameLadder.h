#pragma once

#if __VER >= 13 //	__RAINBOW_RACE
#include "minigamebase.h"

class CMiniGameLadder :	public CMiniGameBase
{
public:
	CMiniGameLadder(void);
	CMiniGameLadder( CMiniGameBase* pMiniGame );
	virtual ~CMiniGameLadder(void);

	virtual BOOL Excute( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket );
};

#endif // __RAINBOW_RACE
