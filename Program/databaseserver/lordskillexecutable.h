#ifndef __LORD_SKILL_EXECUTABLE_H__
#define	__LORD_SKILL_EXECUTABLE_H__

#include "lordskill.h"

// 군주 스킬
// 트랜스 서버에서 사용
// 효과 적용 처리 아님
// 재사용 대기 시간에 따라 성공 여부 판단
// 선 쿼리 후 적용
class CLordSkillComponentODBC
	: public CLordSkillComponentExecutable
{
public:
	CLordSkillComponentODBC();
	virtual ~CLordSkillComponentODBC();
public:
	virtual	void	Execute( u_long idPlayer, u_long idTarget, VOID* pParam );
};

#endif	// __LORD_SKILL_EXECUTABLE_H__