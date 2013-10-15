#include "stdafx.h"

#if __VER >= 13 // __RAINBOW_RACE
#include ".\minigamediceplay.h"

CMiniGameDiceplay::CMiniGameDiceplay(void)
{
}

CMiniGameDiceplay::CMiniGameDiceplay( CMiniGameBase* pMiniGame )
: m_nTargetNum( xRandom( 2, 13 ) )
{
	CMiniGameDiceplay* pMiniGameDiceplay = static_cast<CMiniGameDiceplay*>( pMiniGame );

	
}

CMiniGameDiceplay::~CMiniGameDiceplay(void)
{
}

BOOL CMiniGameDiceplay::Excute( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket )
{
	BOOL bReturn = FALSE;		// 현재 미니 게임이 완료되는건지...
	__MINIGAME_PACKET MP( pMiniGamePacket->wNowGame );
	if( pMiniGamePacket->nState == MP_OPENWND )
	{
		MP.nState = MP_OPENWND;
		MP.nParam1 = m_nTargetNum;
		SendPacket( pUser, MP );
		return bReturn;
	}
	
	MP.nParam1 = xRandom( 6 ) + 1;
	MP.nParam2 = xRandom( 6 ) + 1;

	if( ( MP.nParam1 + MP.nParam2 ) == m_nTargetNum )
	{
		MP.nState = MP_FINISH;
		bReturn = TRUE;
	}
	else
	{
		MP.nState = MP_FAIL;
		SendPacket( pUser, MP );
		m_nTargetNum = xRandom( 2, 13 );
		MP.nParam1 = m_nTargetNum;
		MP.nParam2 = -1;	// 실패시 새로 바뀐 목표값이라는 걸 알려줌..(-1)
	}

	SendPacket( pUser, MP );
	return bReturn;
}
#endif // __RAINBOW_RACE