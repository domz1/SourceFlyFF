#include "stdafx.h"

#if __VER >= 13 // __RAINBOW_RACE
#include ".\minigamestopwatch.h"

CMiniGameStopwatch::CMiniGameStopwatch(void)
{
}

CMiniGameStopwatch::CMiniGameStopwatch( CMiniGameBase* pMiniGame )
: m_nTargetTime( 0 )
{
	CMiniGameStopwatch* pMiniGameStopwatch = static_cast<CMiniGameStopwatch*>( pMiniGame );
}

CMiniGameStopwatch::~CMiniGameStopwatch(void)
{
}

int	CMiniGameStopwatch::SetTargetTime()
{
	m_nTargetTime = xRandom( 300, 1000 ) * 10;
	return m_nTargetTime;
}

BOOL CMiniGameStopwatch::Excute( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket )
{
	BOOL bReturn = FALSE;

	__MINIGAME_PACKET MP( pMiniGamePacket->wNowGame );	
	if( pMiniGamePacket->nState == MP_OPENWND )
	{
		MP.nState = MP_OPENWND;
		MP.nParam1 = SetTargetTime();
		SendPacket( pUser, MP );
		return bReturn;
	}

	// 0.05초 오차범위 인정..
	if( GetTargetTime() <= (pMiniGamePacket->nParam1 + 30)
		&& GetTargetTime() >= (pMiniGamePacket->nParam1 - 30) )
	{
		MP.nState = MP_FINISH;
		bReturn = TRUE;
	}
	else
	{
		MP.nState = MP_FAIL;
		MP.nParam1 = SetTargetTime();
	}

	SendPacket( pUser, MP );
	return bReturn;
}

#endif // __RAINBOW_RACE