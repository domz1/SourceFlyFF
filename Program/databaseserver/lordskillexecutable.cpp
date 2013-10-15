#include "stdafx.h"

#if __VER >= 12 // __LORD

#include "tlord.h"
#include "lordskillexecutable.h"
#include "dptrans.h"
#include "definetext.h"

CLordSkillComponentODBC::CLordSkillComponentODBC()
{
}

CLordSkillComponentODBC::~CLordSkillComponentODBC()
{
}

void CLordSkillComponentODBC::Execute( u_long idPlayer, u_long idTarget, VOID* pParam )
{	// 트랜스 서버의 군주 스킬 실행
	int nRet	= 0;
	if( GetTick() > 0 )		// 재사용 대기 시간이 남아있으면 불가
		nRet	= TID_GAME_LORD_SKILL_USE_E005;
	if( !nRet )
	{	// 재사용 대기 시간 설정 쿼리 요청
		CLController* pController	= reinterpret_cast<CLController*>( pParam );
		if( !pController->UpdateLordSkillTick( this, GetCooltime() ) )
			nRet	= TID_GAME_LORD_SKILL_USE_E006;
	}
	if( !nRet )		// 쿼리 성공하면 재사용 대기 시간 설정
		Use();
	// 월드 서버에 결과 전송
	CDPTrans::GetInstance()->SendLordSkillUse( idPlayer, idTarget, GetId(), nRet );
}

#endif	// __LORD