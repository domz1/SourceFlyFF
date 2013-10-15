#pragma once

#if __VER >= 13 // __RAINBOW_RACE
#include "minigamebase.h"

class CMiniGamePairgame : public CMiniGameBase
{
public:
	CMiniGamePairgame(void);
	CMiniGamePairgame( CMiniGameBase* pMiniGame );
	virtual ~CMiniGamePairgame(void);

	virtual BOOL Excute( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket );

private:
	string	MakeGame();

	string	m_strCardStream;
	int		m_nCorrectCount;
};

#endif // __RAINBOW_RACE