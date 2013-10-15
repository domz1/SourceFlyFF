#include "stdafx.h"

#if __VER >= 13 // __RAINBOW_RACE

#ifdef __WORLDSERVER
#include ".\minigamebase.h"
#include "User.h"

CMiniGameBase::CMiniGameBase(void)
{
}

CMiniGameBase::~CMiniGameBase(void)
{
}

void CMiniGameBase::SendPacket( CUser* pUser, __MINIGAME_PACKET MiniGamePacket )
{
	pUser->AddMiniGameState( MiniGamePacket );
}

void CMiniGameBase::SendExtPacket( CUser* pUser, __MINIGAME_EXT_PACKET MiniGameExtPacket )
{
	pUser->AddMiniGameExtState( MiniGameExtPacket );
}
#endif // __WORLDSERVER

#endif // __RAINBOW_RACE