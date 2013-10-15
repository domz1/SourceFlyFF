#ifndef __CTRLOBJS_H__
#define __CTRLOBJS_H__

#ifdef __WORLDSERVER
#include "Ctrl.h"

/// 일정지역에 지속적 데미지를 주는 가상오브젝트
class CContDamageCtrl : public CCtrl
{
private:
	DWORD	m_tmStart, m_tmUpdate;
	
	void Init( void );
	void Destroy( void );
	
public:
	ItemProp *m_pSkillProp;
	AddSkillProp *m_pAddSkillProp;
	OBJID m_idSrc;
	OBJID m_idTarget;
#if __VER >= 8 // __S8_PK
	BOOL m_bControl;
#endif // __VER >= 8 // __S8_PK
	// chipi - 범위 ctrl 생성 스킬로 인해 듀얼 종료 후 카오되는 현상 수정
	BOOL m_bDuelTarget;

	void	ApplyDamage( void );
	void	Process();
	
	CContDamageCtrl() 
	{ 
	#ifdef __WORLDSERVER
		SetVirtual( TRUE ); 
	#endif
		Init(); 
	}
	virtual ~CContDamageCtrl() { Destroy(); }
	
};

#endif // Worldserver

#endif // CtrlObjs_h