#include "stdafx.h"
#include "DefineObj.h"
#include "defineSkill.h"
#include "ActionMover.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif

#ifdef __WORLDSERVER
#include "User.h"
#include "DPCoreClient.h"
#include "DPDatabaseClient.h"
#include "DPSrvr.h"
#include "guild.h"

extern	CUserMng		g_UserMng;
extern	CDPCoreClient		g_DPCoreClient;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPSrvr				g_DPSrvr;
extern	CGuildMng			g_GuildMng;
#endif // __WORLDSERVER

#include "defineitem.h"

#if __VER >= 19
#include "defineitemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#include "party.h"
#ifdef __WORLDSERVER
#include "AttackArbiter.h"
#endif


extern	CPartyMng	g_PartyMng;


/////////////////////////////////////////////////////////////////////////////////////
// CActionMover
/////////////////////////////////////////////////////////////////////////////////////



DWORD CActionMover::_ProcessMsgDmg( DWORD dwMsg, CMover* pAttacker, DWORD dwAtkFlags, int nParam, BOOL bTarget, int nReflect )
{
#if		defined(__WORLDSERVER)
	CAttackArbiter arbiter( dwMsg, pAttacker, GetMover(), dwAtkFlags, nParam, bTarget, nReflect );
	return arbiter.OnDamageMsgW();
#elif	defined(__CLIENT)
	return OnDamageMsgC( dwMsg, pAttacker, dwAtkFlags, nParam );
#else
	return 0;
#endif
}

#ifdef	__CLIENT
DWORD CActionMover::OnDamageMsgC( DWORD dwMsg, CMover* pAttacker, DWORD dwAtkFlags, int nParam )
{
	CMover*	pMover = GetMover();
	BOOL bValid = IsValidObj( pMover ) && IsValidObj( pAttacker );

	if( !bValid || IsState( OBJSTA_DIE_ALL ) )
		return 0;

	if( IsSit() )										// 앉아있다가 맞으면 앉기해제 한다.
		ResetState( OBJSTA_MOVE_ALL );
	
	SendActMsg( OBJMSG_STAND );

	// 날때린놈에 대한 정보를 기록함.
	if( pMover->IsNPC() && pAttacker->IsPlayer() )		// 맞은놈은 NPC , 어태커가 플레이어 일때만 적용됨
	{
		pMover->m_idAttacker = pAttacker->GetId();		// 날 때린넘이 어떤놈인가를 기록함.
		pMover->m_idTargeter = pAttacker->GetId();	
	}
	pAttacker->m_idLastHitMover = pMover->GetId();		// 어태커가 마지막으로 때렸던넘이 나란걸 기록함.

	if( (dwAtkFlags & AF_GENERIC) )	
	{
		ItemProp* pAttackerProp = pAttacker->GetActiveHandItemProp();
		
		D3DXVECTOR3 vLocal;
		if( pAttackerProp && pAttackerProp->dwItemKind3 == IK3_YOYO )
		{
			vLocal    = pMover->GetPos();
			vLocal.y += 1.0f;
		}
		else
		{
			AngleToVectorXZ( &vLocal, pAttacker->GetAngle(), 1.0f );
			vLocal += pMover->GetPos();		//gmpbigsun : 피격자 일반 effect 09_12_17
			vLocal.y += 1.0f;			// 2006/6/20 xuzhu
		}

		if( pAttackerProp && pAttackerProp->dwSfxObj3 != NULL_ID )
			CreateSfx( g_Neuz.m_pd3dDevice, pAttackerProp->dwSfxObj3, vLocal );

		
		if( pAttackerProp && pAttackerProp->dwSfxObj5 != NULL_ID ) //gmpbigsun: 공격자 일반 effect 09_12_17
		{
			vLocal = pAttacker->GetPos( );		
			CreateSfx( g_Neuz.m_pd3dDevice, pAttackerProp->dwSfxObj5, vLocal );
		}
		
	}
	else if ( (dwAtkFlags & AF_MONSTER_SP_CLIENT) )
	{
		// hitter
		ItemProp* pAttackerProp = prj.GetItemProp( nParam >> 16 );
		assert( pAttackerProp );
		DWORD dwSfxObj = pAttackerProp->dwSfxObj3;		// gmpbigsun:특수공격에 이펙트가 있다면 3번사용.
		if( dwSfxObj != NULL_ID )
			CreateSfx( D3DDEVICE, dwSfxObj, pMover->GetPos() );

		// attacker
		dwSfxObj = pAttackerProp->dwSfxObj5;
		if( NULL_ID != dwSfxObj )
			CreateSfx( D3DDEVICE, dwSfxObj, pAttacker->GetPos() );
	}

	pMover->m_nDmgCnt = 10;								// 흔들리는 시간이다.

	pMover->SetDamagedMotion( pAttacker, dwAtkFlags );
	return 1;
}
#endif	// __CLIENT


