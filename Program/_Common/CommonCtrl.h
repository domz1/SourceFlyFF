#ifndef __COMMONCTRL_H__
#define __COMMONCTRL_H__

#include "Ctrl.h"
//#include "Respawn.h"
#include "ProjectCmn.h"


#define			MAX_CTRLDROPITEM		4
#define			MAX_CTRLDROPMOB			3
#define			MAX_TRAP                3

#define			MAX_KEY					64

#define         OPEN_COOLTIME			SEC(10)

typedef struct CCtrlElem
{
	DWORD		m_dwSet;				    //-열기조건 설정 UA_ITEM, UA_LEVEL...
	DWORD 		m_dwSetItem;				//-박스 오픈 조건(아이템)								- UA_ITEM
	DWORD		m_dwSetLevel;				//-박스 오픈 조건(레벨)									- UA_LEVEL
	DWORD		m_dwSetQuestNum;			//-박스 오픈 조건(퀘스트 넘버,퀘스트 플래그 넘버)       - UA_QUEST
	DWORD		m_dwSetFlagNum;
	DWORD		m_dwSetGender;				//-박스 오픈 조건(성별)									- UA_GENDER
	BOOL		m_bSetJob[32];			//-박스 오픈 조건(클래스)								- UA_CLASS
	
	DWORD		m_dwSetEndu;				//-박스 내구도 ? hp와 같이 적용	
	 
	DWORD		m_dwMinItemNum;				//-최소 발생 아이템 수 
	DWORD		m_dwMaxiItemNum;			//-최대 발생 아이템 수
	DWORD		m_dwInsideItemKind[MAX_CTRLDROPITEM];		//-들어가는 아이템 종류 값 !!!주의!!! 마지막 MAX_CTRLDROPITEM-1 은 레어 아템공간
	DWORD		m_dwInsideItemPer [MAX_CTRLDROPITEM];		//-들어가는 아이템 의 발생 확룰( 3000000000 )
	
	DWORD       m_dwMonResKind  [MAX_CTRLDROPMOB];          //-몬스터1 발생 종류(몬스터 아이디 입력)
	DWORD		m_dwMonResNum   [MAX_CTRLDROPMOB];			//-몬스터1 발생 수량
	DWORD		m_dwMonActAttack[MAX_CTRLDROPMOB];			//-몬스터 타입1에서 선공 몬스터 발생 수량(위에 필드 수량 초과해서는 않됨)

	// 트랩관련
	DWORD		m_dwTrapOperType;              // 트랩 발동 타입 TOT_RANDOM, TOT_NOENDU
	DWORD		m_dwTrapRandomPer;             // 트랩 발동 확율(TOT_RANDOM일경우)
	DWORD		m_dwTrapDelay;				   // 트랩 발동 딜레이 0이면 즉시 발생
	
	DWORD		m_dwTrapKind	[MAX_TRAP];    // 트랩 종류 - 가상 스킬로 사용함 모든 트랩은 스킬임 (스킬프로퍼티의 ID값)
 	DWORD		m_dwTrapLevel	[MAX_TRAP];
	TCHAR		m_strLinkCtrlKey[MAX_KEY];		// 연결 컨트롤의 키 이름
	TCHAR		m_strCtrlKey[MAX_KEY];			// 컨트롤의 키 이름
	DWORD		m_dwSetQuestNum1;			//-박스 열고난후 적용(퀘스트 넘버,퀘스트 플래그 넘버)       - UA_QUEST
	DWORD		m_dwSetFlagNum1;
	DWORD		m_dwSetQuestNum2;			//-박스 열고난후 적용(퀘스트 넘버,퀘스트 플래그 넘버)       - UA_QUEST
	DWORD		m_dwSetFlagNum2;
	DWORD 		m_dwSetItemCount;
	DWORD		m_dwTeleWorldId;
	DWORD		m_dwTeleX;
	DWORD		m_dwTeleY;
	DWORD		m_dwTeleZ;
	
} CCtrlElem, *LPCtrlElem;

/// 지형 위의 상자, 트랩  
class CCommonCtrl : public CCtrl
{
public:
	CCommonCtrl();
	virtual ~CCommonCtrl();

public:
	OBJID		m_nMoverID;
	CCtrlElem	m_CtrlElem;

	BOOL		 m_bTrap;
	BOOL		 m_bAniPlay;
	BOOL		 m_bAlpha;
	int			 m_nBlendFactorLocal;
	DWORD		 m_dwTrapProcessTime;
	
	float		 m_fFrameCount;
	BOOL		 m_bAniDelay;
	DWORD		 m_dwAniDelay;
	
public:
	void Init();
	ItemProp *m_pSkillProp;
	AddSkillProp *m_pAddSkillProp;
#ifdef __CLIENT
	CSfxModel *m_pSfxModel;
	CSfxModel *m_pSfxModel2;
#endif // __CLIENT
	OBJID	m_idAttacker;			// 사이킥 월 시전자.
	BOOL	m_bControl;
	int		m_nCount;
	int		m_nEvent;
	DWORD	m_tmStart;
	int		m_nLife;				// 몇번 부딪히면 없어지냐.

	BOOL	m_bAction;
	DWORD   m_dwCtrlReadyTime;

	DWORD		m_dwDelete;
	EXPINTEGER	m_nExpBox;
	u_long      m_idExpPlayer;		
	
	void DestroyWall( void );
	void _ProcessWall( void );
	void _CreateTrapSkill();
	void _ProcessTrap();
	void _ProcessAction();
	void DropNPC();
	
	void			DropItem();
	void			SetActionPlay();
//#ifdef __WORLDSERVER
//	void			SendActionError();
//#endif //__WORLDSERVER
	CtrlProp*		GetProp() { return prj.GetCtrlProp( GetIndex() ); }

	virtual void Process();
	virtual void Render( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual void RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor = 0xffffffff );//adeilson
	virtual BOOL Read( CFileIO* pFile );
};

#endif //__COMMONCTRL_H__
