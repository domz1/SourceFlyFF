#include "stdafx.h"
#include "vmlog.h"
#include "ar.h"
#include "buffer.h"
#include "aibear.h"
#include "aibigmuscle.h"
#include "aiclockworks.h"
#include "aiguard.h"
#include "aikrrr.h"
#include "aimonster.h"
#include "aipet.h"
#include "chatting.h"
#include "guild.h"
#include "party.h"
#include "clientsock.h"
#include "clientsocke.h"
#include "user.h"

CVMLog* CVMLog::Instance()
{
	static CVMLog sVMLog;
	return &sVMLog;
}

void CVMLog::Log()
{
#ifndef __VM_0820
	SYSTEMTIME	time;
	GetLocalTime( &time );
	FILEOUT( "../vm.txt", "\n%d/%02d/%02d\t%02d:%02d:%02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond );

	if( CBuffer::m_pHeapMng )
		CBuffer::m_pHeapMng->Log();
	if( CAr::m_pHeapMng )
		CAr::m_pHeapMng->Log();
	if( CAIBear::m_pPool )
		CAIBear::m_pPool->Log( "" );
	if( CAIBigMuscle::m_pPool )
		CAIBigMuscle::m_pPool->Log( "" );
	if( CAIClockWorks::m_pPool )
		CAIClockWorks::m_pPool->Log( "" );
	if( CAIGuard::m_pPool )
		CAIGuard::m_pPool->Log( "" );
	if( CAIKrrr::m_pPool )
		CAIKrrr::m_pPool->Log( "" );
	if( CAIMonster::m_pPool )
		CAIMonster::m_pPool->Log( "" );
#ifdef __AI_AGGRO
	if( CAIAggro::m_pPool )
		CAIAggro::m_pPool->Log( "" );
#endif//__AI_AGGRO
	if( CAIPet::m_pPool )
		CAIPet::m_pPool->Log( "" );
	if( CChatting::m_pPool )
		CChatting::m_pPool->Log( "" );
	if( CGuildMember::sm_pPool )
		CGuildMember::sm_pPool->Log( "" );
	if( CGuild::sm_pPool )
		CGuild::sm_pPool->Log( "" );
	if( CParty::m_pPool )
		CParty::m_pPool->Log( "" );
	if( CBuffer2::m_pPool2 )
		CBuffer2::m_pPool2->Log( "" );
	if( CBuffer::m_pPool )
		CBuffer::m_pPool->Log( "" );
	if( CClientSock::m_pPool )
		CClientSock::m_pPool->Log( "" );
	if( CClientSockE::m_pPool )
		CClientSockE::m_pPool->Log( "" );
	if( CUser::m_pPool )
		CUser::m_pPool->Log( "" );
	if( CMover::m_pPool )
		CMover::m_pPool->Log( "" );
	if( CActionMover::m_pPool )
		CActionMover::m_pPool->Log( "" );
	if( CItem::m_pPool )
		CItem::m_pPool->Log( "" );
#endif	// __VM_0820
}