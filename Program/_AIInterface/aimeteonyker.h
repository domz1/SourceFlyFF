#ifndef __AIMETEONYKER_H__
#define	__AIMETEONYKER_H__

class CAIMeteonyker : public CAIInterface
{
//	0. 기본 공격
	OBJID	m_dwIdTarget;
	u_long	m_uParty;
//	1. 30초 동안 자신에게 가장 큰 피해를 준 사용자 색적
//	Mvr_Meteonyker_dmg3.ani로 공격
//	피해량 5000-20000
	int		m_cbRetaliation;					// 30초 카운트
	map<OBJID, int>	m_mapDamage;	// 누적 피해
	void	AddDamage( OBJID idAttacker, int nDamage );	// 피해 누적
	void	ResetDamage( void );	// 초기화 GetDealer 후
	OBJID	GetDealer( void );	// 살아있는 않은 딜러 중
	BOOL	ProcessRetaliation( void );
// 1. 매 공격 시 5%의 확률로 발동.
// 30m 범위 내의 모든 캐릭터를 소환하여 공격한다.
	BOOL	ProcessSummon( BOOL bUnconditional = FALSE );
//	3. 메테오니커R 자신의 HP가 20%가 이하가 되었을 시 부터 매 공격 시 2% 확률로 발동된다.								
//	WdVolcaneRed 맵에 존재하는 모든 캐릭터에게 데미지를 주며, '화상'에 걸리게 한다.
//	화상(디버프)	대상은 틱당 500의 데미지를 120초 동안 입게 된다.
	BOOL	ProcessBurn( BOOL bUnconditional = FALSE );
//	4. 자신의 HP가 30%이하가 되었을 시 1회에 한해 HP를 50% 회복 한다.
//	BOOL	m_bHeal;

//	5. 변경	// 20번째 공격 마다 '광격 공격' or '스페셜 공격'이 발동됨
	int		m_cbAttack;
	int		m_nSPAttackType;
	BOOL	ProcessSPAttack( void );

	BOOL	ProcessHeal( void );
	time_t	m_tmRecovery;
	void	ProcessRecovery( void );
//	5. 자신의 HP가 2%가 되었을 시 모든 공격력 2배 증가
	DWORD	m_dwHPOld;
	BOOL	m_bCritical;
	time_t	m_tmAttackDelay;
	time_t	m_tmTrace;
	DWORD	m_dwAtk;
	BOOL	SelectTarget( void );
	OBJID	m_idLastAttacker;
	D3DXVECTOR3	m_vPosBegin;
	BOOL	m_bReturnToBegin;
	time_t	m_tmReturnToBegin;
	time_t	m_tmAttack;
	D3DXVECTOR3	m_vOldPos;
	time_t	m_tmMove;
	D3DXVECTOR3	m_vPosDamage;

	void Init( void );
	void Destroy( void );
	BOOL	MoveProcessIdle( void );
	BOOL	MoveProcessRage( void );
	BOOL	MoveProcessRunaway( void );
	BOOL	StateInit( const AIMSG & msg );
	BOOL	StateIdle( const AIMSG & msg );
	BOOL	StateRage( const AIMSG & msg );

	void	SetStop( DWORD dwTime );
	BOOL	IsEndStop( void );
	BOOL	IsMove() {	return GetMover()->m_pActMover->IsMove();	}
	void	MoveToDst( const D3DXVECTOR3& vDst );
	void	MoveToDst( OBJID dwIdTarget );
	void	MoveToRandom( UINT nState );
	BOOL	IsInRange( const D3DXVECTOR3& vDistant, FLOAT fRange );
	void	DoReturnToBegin( BOOL bReturn = TRUE );
private:
	CMover*		GetTarget( OBJID objid );
public:
	CAIMeteonyker();
	CAIMeteonyker( CObj* pObj );
	virtual	~CAIMeteonyker();
	virtual	void	InitAI( void );
	virtual	BOOL	IsReturnToBegin( void )		{	return m_bReturnToBegin;	}

	void	SetTarget( OBJID dwIdTarget, u_long uParty );

	DECLARE_AISTATE();
};

#endif	// __AIMETEONYKER_H__