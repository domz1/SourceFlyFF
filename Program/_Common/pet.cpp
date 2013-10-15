#include "stdafx.h"
#include "pet.h"
#include "defineobj.h"

#ifdef __WORLDSERVER
#include "user.h"
#if __VER >= 12 // __PET_0519
#include "dpsrvr.h"
extern	CDPSrvr	g_DPSrvr;
#include "definetext.h"
#endif	// __PET_0519
#endif	// __WORLDSERVER

#include "xutil.h"

#if __VER >= 9	// __PET_0410

const	int		nDefaultLife	= 1;

CPetProperty::CPetProperty()
{
	memset( (void*)m_aPetAvailParam, 0, sizeof(PETAVAILPARAM) * PK_MAX );
	memset( (void*)m_anLevelupAvailLevelMax, 0, sizeof(BYTE) * PL_MAX );
	memset( (void*)m_adwLevelupAvailLevelProbability, 0, sizeof(DWORD) * PL_MAX * MAX_PET_AVAIL_LEVEL );
	memset( (void*)m_adwIncrementExp, 0, sizeof(DWORD) * PL_MAX );
	memset( (void*)m_awMaxEnergy, 0, sizeof(WORD) * PL_MAX );
	memset( (void*)m_aPenalty, 0, sizeof(PETPENALTY) * PL_MAX );
}

CPetProperty::~CPetProperty()
{
	m_aFeedEnergy[0].clear();
	m_aFeedEnergy[1].clear();
	m_awAddLifeProbability.clear();
}

PPETPENALTY CPetProperty::GetPenalty( BYTE nLevel )
{
	if( nLevel >= PL_MAX )
		return NULL;
	return &m_aPenalty[nLevel];
}

PPETAVAILPARAM	CPetProperty::GetAvailParam( BYTE nKind )
{
	if( nKind >= PK_TIGER && nKind < PK_MAX )
		return &m_aPetAvailParam[nKind];
	return NULL;
}

BYTE	CPetProperty::GetLevelupAvailLevel( BYTE wLevel )
{
	DWORD dwTotal	= 0;
	DWORD dwRandom	= xRandom( 1, 10001 );	// 1 ~ 10000
	for( int i = 0; i <= m_anLevelupAvailLevelMax[wLevel]; i++ )
	{
		dwTotal		+= m_adwLevelupAvailLevelProbability[wLevel][i];
		if(  dwTotal >= dwRandom )
			return i + 1;
	}
	return 0;
}

WORD	CPetProperty::GetFeedEnergy( DWORD dwCost, int nIndex )
{
	if( nIndex < 0 || nIndex > 1 )
		return 0;

	vector<FEEDENERGY>* pArr	= &m_aFeedEnergy[nIndex];
	for( vector<FEEDENERGY>::iterator i = pArr->begin(); i != pArr->end(); ++i )
	{
		if( dwCost >= i->dwCostMin && dwCost <= i->dwCostMax )
			return (WORD)xRandom( i->wEnergyMin, i->wEnergyMax + 1 );	// wEnergyMin ~ wEnergyMax
	}
	return 0;
}

BYTE	CPetProperty::Hatch( void )
{
	DWORD dwTotal	= 0;
	DWORD dwRandom	= xRandom( 1, 10001 );	// 1 - 10000
	for( int i = 0; i < PK_MAX; i++ )
	{
		dwTotal	+= m_aPetAvailParam[i].m_dwProbability;
		if( dwTotal >= dwRandom )
			return i;
	}
	return 0;	// error
}

DWORD	CPetProperty::GetIncrementExp( BYTE nLevel )
{
	if( nLevel >= PL_D && nLevel <= PL_A )
		return m_adwIncrementExp[nLevel-1];
	return 0;
}

WORD CPetProperty::GetMaxEnergy( BYTE nLevel )
{
	if( nLevel >= PL_D && nLevel <= PL_S )
		return m_awMaxEnergy[nLevel-1];
	return 1;	// 0 나누기 방지
}

WORD CPetProperty::GetAddLife( void )
{
	DWORD dwTotal	= 0;
	DWORD dwRandom	= xRandom( 1, 101 );	// 1 ~ 100
	for( DWORD i = 0; i < m_awAddLifeProbability.size(); i++ )
	{
		dwTotal	+= m_awAddLifeProbability[i];
		if( dwTotal >= dwRandom )
			return	( (WORD)( i + 1 ) );
	}
	return 1;
}

CPetProperty* CPetProperty::GetInstance( void )
{
	static	CPetProperty	sPetProperty;
	return &sPetProperty;
}

BOOL CPetProperty::LoadScript( LPCTSTR szFile )
{
	CScript s;
	if( s.Load( szFile ) == FALSE )
		return FALSE;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "AvailDestParam" ) )
		{
			// 	DST_STR
			int	nKind	= 0;
			s.GetToken();	// {
			DWORD dwDstParam	= s.GetNumber();
			while( *s.token != '}' )
			{
				ASSERT( nKind < PK_MAX );
				m_aPetAvailParam[nKind].dwDstParam	= dwDstParam;
				for( int i = 0; i < MAX_PET_AVAIL_LEVEL; i++ )
					m_aPetAvailParam[nKind].m_anParam[i]	= s.GetNumber();
				m_aPetAvailParam[nKind].m_dwItemId	= s.GetNumber();
				m_aPetAvailParam[nKind].m_adwIndex[0]	= s.GetNumber();
				m_aPetAvailParam[nKind].m_adwIndex[1]	= s.GetNumber();
				m_aPetAvailParam[nKind].m_adwIndex[2]	= s.GetNumber();
				m_aPetAvailParam[nKind++].m_dwProbability	= s.GetNumber();
				dwDstParam	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "LevelupAvail" ) )
		{
			// 	10000	0	0	0	0	0	0	0	0
			int	nLevel	= (int)PL_D;
			s.GetToken();	// {{
			DWORD	dwProbability	= s.GetNumber();
			while( *s.token != '}' )
			{
				m_adwLevelupAvailLevelProbability[nLevel][0]	= dwProbability;
				for( int i = 1; i < MAX_PET_AVAIL_LEVEL; i++ )
				{
					m_adwLevelupAvailLevelProbability[nLevel][i]	= s.GetNumber();
					if( m_adwLevelupAvailLevelProbability[nLevel][i] > 0 )
						m_anLevelupAvailLevelMax[nLevel]	= i;
				}
				nLevel++;
				dwProbability	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "FeedEnergy" ) )
		{
			int nIndex	= s.GetNumber();
			ASSERT( nIndex >= 0 && nIndex <= 1 );

			vector<FEEDENERGY>* pArr	= &m_aFeedEnergy[nIndex];
			// 	1	575	7	14
			FEEDENERGY	fe;
			s.GetToken();	// {
			fe.dwCostMin	= s.GetNumber();
			while( *s.token != '}' )
			{
				fe.dwCostMax	= s.GetNumber();
				fe.wEnergyMin	= s.GetNumber();
				fe.wEnergyMax	= s.GetNumber();
				pArr->push_back( fe );	// add
				fe.dwCostMin	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "IncrementExp" ) )
		{
			//	100	// 0.1%	// 100000분의 1 기준
			int	nLevel	= 0;
			s.GetToken();	// {
			DWORD	dwIncrementExp	= s.GetNumber();
			while( *s.token != '}' )
			{
				ASSERT( nLevel < PL_S );
				m_adwIncrementExp[nLevel++]	= dwIncrementExp;
				dwIncrementExp	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "MaxEnergy" ) )
		{
			//	250	// D	// 최대 기력
			int	nLevel	= 0;
			s.GetToken();	// {
			DWORD	dwMaxEnergy	= s.GetNumber();
			while( *s.token != '}' )
			{
				ASSERT( nLevel <= PL_S );
				m_awMaxEnergy[nLevel++]	= (WORD)( dwMaxEnergy );
				dwMaxEnergy		= s.GetNumber();
			}
		}
		else if( s.Token == _T( "AddLifeProbability" ) )
		{
			// 수명 회복액 확률	// 추가 될 경우 합이 100이 되도록 확장
			// 	50	// +1
			s.GetToken();	// {
			WORD wProbability	= s.GetNumber();
			while( *s.token != '}' )
			{
				m_awAddLifeProbability.push_back( wProbability );
				wProbability	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "Penalty" ) )
		{
			// Pet 소환 후 해제 시 D~A레벨에선 경험치를 잃게 되며,
			// 최소 0%에서는 더 이상 감소하지 않는다. 
			// S레벨에선 HP가 감소하며, 최소 5P 이하로는 더 이상 감소하지 않는다. 
			s.GetToken();	// {
			FLOAT fExp	= s.GetFloat();
			int nLevel	= PL_D;
			while( *s.token != '}' )
			{
				m_aPenalty[nLevel].fExp		= fExp;
				m_aPenalty[nLevel].wEnergy	= (WORD)s.GetNumber();
				nLevel++;
				fExp	= s.GetFloat();
			}
		}
		s.GetToken();
	}

/*
#ifdef _DEBUG
	TRACE( "GetAvailParam\n" );
	for( int i = 0; i < PK_MAX; i++ )
	{
		PPETAVAILPARAM pPetAvailParam	= GetAvailParam( i );
		TRACE( "dwDstParam=%d, nBase=%d, nParam=%d\n", pPetAvailParam->dwDstParam, pPetAvailParam->nBase, pPetAvailParam->nParam );
	}
	TRACE( "GetLevelupAvailLevel\n" );
	// 0은 모두 0이어야 한다.
	for( i = PL_EGG; i < PL_MAX; i++ )
	{
		for( int j = 0; j < 100; j++ )
		{
			BYTE nAvailLevel	= GetLevelupAvailLevel( i );
			TRACE( "PetLevel=%d, AvailLevel=%d\n", i, nAvailLevel );
		}
	}

	TRACE( "\nGetFeedEnergy\n" );
	for( i = 1; i <= 100; i++ )
	{
		DWORD dwCost	= i * 120;
		WORD wEnergy	= GetFeedEnergy( dwCost );
		TRACE( "dwCost=%d, wEnergy=%d\n", dwCost, wEnergy );
	}
	TRACE( "\nGetIncrementExp\n" );
	for( i = PL_D; i < PL_MAX; i++ )
		TRACE( "nLevel=%d, IncrementExp=%d\n", i, GetIncrementExp( (PETLEVEL)i ) );

	TRACE( "\nGetMaxEnergy\n" );
	for( i = PL_D; i < PL_MAX; i++ )
		TRACE( "nLevel=%d, MaxEnergy=%d\n", i, GetMaxEnergy( (PETLEVEL)i ) );

	TRACE( "\nGetAddLife\n" );
	for( i = 0; i < 100; i++ )
	{
		TRACE( "AddLife=%d\n", GetAddLife() );
	}
#endif	// _DEBUG
*/
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

CPet::CPet()
{
	m_nKind	= 0;
	m_nLevel	= PL_EGG;
	m_dwExp	= 0;
	m_wEnergy	= 0;
	m_wLife	= 0;
	memset( (void*)m_anAvailLevel, 0, sizeof(BYTE) * PL_MAX );
#ifdef __PET_1024
	m_szName[0]	= '\0';
#endif	// __PET_1024
}

CPet::~CPet()
{
}

void CPet::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_nKind;
		ar << (BYTE)m_nLevel;
		ar << m_dwExp;
		ar << m_wEnergy;
		ar << m_wLife;
		ar.Write( (const void*)m_anAvailLevel, sizeof(BYTE) * PL_MAX );
#ifdef __PET_1024
		ar.WriteString( m_szName );
#endif	// __PET_1024
	}
	else
	{
		m_nKind	= PK_TIGER;
		m_nLevel	= PL_EGG;
		ar >> m_nKind;
		ar >> (BYTE &)m_nLevel;
		ar >> m_dwExp;
		ar >> m_wEnergy;
		ar >> m_wLife;
		ar.Read( (void *)m_anAvailLevel, sizeof(BYTE) * PL_MAX );
#ifdef __PET_1024
		ar.ReadString( m_szName, MAX_PET_NAME );
#endif	// __PET_1024
	}
}

void CPet::SetEnergy( WORD wEnergy )
{
#ifdef __DBSERVER
	m_wEnergy	= wEnergy;		// trans서버에서는 pet.inc를 읽지 않는다.
#else	// __DBSERVER
	WORD wMaxEnergy		= GetMaxEnergy();
	m_wEnergy	= wEnergy > wMaxEnergy? wMaxEnergy: wEnergy;
#endif	// __DBSERVER
}

void CPet::SetExp( DWORD dwExp )
{
	m_dwExp		= dwExp > (DWORD)( GetMaxExp() ) ? GetMaxExp(): dwExp;
}

WORD CPet::GetLife( void )
{
	return ( m_nLevel == PL_EGG )? 0: m_wLife;
}

void CPet::SetAvailLevel( BYTE nLevel,  BYTE nAvailLevel )
{
	if( nLevel >= PL_D && nLevel <= PL_S )
		m_anAvailLevel[nLevel-1]	= nAvailLevel;
}

BYTE CPet::GetAvailLevel( BYTE nLevel )
{
	if( nLevel >= PL_D && nLevel <= PL_S )
		return m_anAvailLevel[nLevel-1];
	return 0;
}

#ifdef _DEBUG
#include "defineattribute.h"
#endif	// _DEBUG

void CPet::GetAvailDestParam( DWORD & dwDestParam, int & nParam )
{
	dwDestParam	= nParam	= 0;
	PPETAVAILPARAM pAvailParam	= CPetProperty::GetInstance()->GetAvailParam( m_nKind );
	if( pAvailParam )
	{
		dwDestParam	= pAvailParam->dwDstParam;
		for( int i = PL_D; i <= m_nLevel; i++ )
			nParam += pAvailParam->m_anParam[m_anAvailLevel[i-1] - 1];
	}
}

DWORD CPet::GetIndex( void )
{
	PPETAVAILPARAM pPetAvailParam	= CPetProperty::GetInstance()->GetAvailParam( m_nKind );
	if( m_nLevel == PL_EGG )
		return MI_PET_EGG;
	else if( m_nLevel > PL_EGG && m_nLevel < PL_B )
		return pPetAvailParam->m_adwIndex[0];
	else if( m_nLevel < PL_S )
		return pPetAvailParam->m_adwIndex[1];
	else
		return pPetAvailParam->m_adwIndex[2];
}

void CPet::InitEgg( void )
{
	SetLife( nDefaultLife );
	SetEnergy( GetMaxEnergy() );
}

CPet& CPet::operator=( CPet& pet )
{
	m_nKind		= pet.GetKind();
	m_nLevel	= pet.GetLevel();
	m_dwExp		= pet.GetExp();
	m_wEnergy	= pet.GetEnergy();
	m_wLife		= pet.GetLife();
	memcpy( m_anAvailLevel, pet.m_anAvailLevel, sizeof(BYTE) * PL_MAX );
#ifdef __PET_1024
	strcpy( m_szName, pet.GetName() );
#endif	// __PET_1024
	return *this;
}

#ifdef __CLIENT
enum
{
	STATE_INIT	= 1,
	STATE_IDLE,
	STATE_RAGE
};

BEGIN_AISTATE_MAP( CAIEgg, CAIInterface )

	ON_STATE( STATE_INIT, StateInit )
	ON_STATE( STATE_IDLE, StateIdle )
	ON_STATE( STATE_RAGE, StateRage )

END_AISTATE_MAP()

#define		PETSTATE_IDLE		0
#define		PETSTATE_TRACE		1

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAIEgg::CAIEgg( CObj* pObj, OBJID idOwner ) : CAIInterface( pObj )
{
	Init();
	m_idOwner	= idOwner;
}

CAIEgg::~CAIEgg()
{
	Destroy();
}

void CAIEgg::Init( void )
{
	m_idOwner = NULL_ID;
	m_nState = PETSTATE_IDLE;
}

void CAIEgg::Destroy( void )
{
	Init();
}

void CAIEgg::InitAI()
{
	PostAIMsg( AIMSG_SETSTATE, STATE_IDLE ); 
}

void CAIEgg::MoveToDst(	D3DXVECTOR3 vDst )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	pMover->SetDestPos( vDst, true, FALSE );
	pMover->m_nCorr		= -1;
}

void CAIEgg::MoveToDst(	OBJID idTarget )
{
	CMover* pMover = GetMover();
	if( pMover->GetDestId() == idTarget )
		return;
	pMover->SetDestObj( idTarget );
}

BOOL CAIEgg::StateInit( const AIMSG & msg )
{
	return TRUE;
}

BOOL CAIEgg::MoveProcessIdle( const AIMSG & msg )
{
//	static	CTimer	tmTrace( 333.33F );
	
	CMover *pMover = GetMover();
	CMover *pOwner = prj.GetMover( m_idOwner ); 
	CWorld *pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();

	// 데미지 상태거나 죽었을 경우 이동 처리 수행 않음 
	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;

	// 주인님과의 거리가 멀어지면 주인님쪽으로 달려간다.
	if( m_nState == PETSTATE_IDLE )
	{
		FLOAT fAngXZ	= pOwner->GetAngle();
		FLOAT fAngH		= pOwner->GetAngleX();

		D3DXVECTOR3 vPos;
//		AngleToVector( &vPos, fAngXZ, -fAngH, 1.0f );
//		vPos	+= pOwner->GetPos();
		vPos	= pOwner->GetPos();
		vPos.y	= pOwner->GetWorld()->GetLandHeight( pOwner->GetPos() );

		D3DXVECTOR3 vPos1, vPos2;
		fAngXZ	= pOwner->GetAngle();
		fAngH  = pOwner->GetAngleX();
		fAngXZ -= 90.0f;
		if( fAngXZ < 0 )
			fAngXZ += 360.0f;
		AngleToVector( &vPos1, fAngXZ, -fAngH, 1.0f );
		vPos1 += vPos;
		fAngXZ	= pOwner->GetAngle();
		fAngH  = pOwner->GetAngleX();
		fAngXZ += 90.0f;
		if( fAngXZ > 360.0f )
			fAngXZ -= 360.0f;
		AngleToVector( &vPos2, fAngXZ, -fAngH, 1.0f );
		vPos2	+= vPos;
		
		D3DXVECTOR3 v1	= vPos1 - pMover->GetPos();
		D3DXVECTOR3 v2	= vPos2 - pMover->GetPos();
		double d1	= (double)D3DXVec3LengthSq( &v1 );
		double d2	= (double)D3DXVec3LengthSq( &v2 );
		if( d1 < d2 )
			vPos	= vPos1;
		else
			vPos	= vPos2;
		D3DXVECTOR3 vDist = vPos - pMover->GetPos();
		vDist.y	= 0;
		FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
		if( fDistSq >1.0f )
		{
			MoveToDst( vPos );
//			m_nState = PETSTATE_TRACE;
		}
	}
	/*
	else if( m_nState == PETSTATE_TRACE )
	{
//		if( pOwner->IsRangeObj( pMover, 0 ) == TRUE )	// 주인님 곁으로 다가갔다.
//		if( pMover->IsEmptyDestPos() )
//		if( tmTrace.Over() )
//		{
//			tmTrace.Reset();
			m_nState = PETSTATE_IDLE;	// 대기상태로 전환
//			pMover->SendActMsg( OBJMSG_STOP );	// 모션 정지
//			pMover->ClearDest();	// 이동목표 클리어.
//		}
	}
	*/
	return TRUE;
}

BOOL CAIEgg::StateIdle( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )
			
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		MoveProcessIdle( msg );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
		SendAIMsg( AIMSG_EXIT );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	

	EndAIHandler( )

	return TRUE;
}

BOOL CAIEgg::MoveProcessRage( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	
	// 데미지 상태거나 죽었을 경우 이동 처리 수행 않음 
	if( pMover->IsDie() || ( pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL ) )
		return FALSE;
	return TRUE;
}

BOOL CAIEgg::StateRage( const AIMSG & msg )
{
	CMover* pMover	= GetMover();
	CWorld* pWorld	= GetWorld();

	if( IsInvalidObj( pMover ) )
		return FALSE;

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )		// 타겟을 클리어하고 대기모드로 돌아감.
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		MoveProcessRage( msg );
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDIE ) 
		SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_BEGINMOVE )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	

	EndAIHandler( )
	
	return TRUE;
}
#endif	// __CLIENT

#endif	// __PET_0410

#if __VER >= 12 // __PET_0519
// 알변환 일반적인 아이템 변환에 이용할 수 있도록 확장을 고려하여 만들었으나 아직 구체적이지 않다.
// 비슷한 변환이 추가될 때, 나머지 작업을 하자
CTransformStuff::CTransformStuff()
:
m_nTransform( 0 )
{
}

CTransformStuff::CTransformStuff( int nTransform )
:
m_nTransform( nTransform )
{
}

CTransformStuff::~CTransformStuff()
{
}

void CTransformStuff::AddComponent( int nItem, short nNum )
{
	TransformStuffComponent component( nItem, nNum );
	m_vComponents.push_back( component );
}

void CTransformStuff::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_nTransform;
		ar << m_vComponents.size();
		for( VTSC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); ++i )
			ar.Write( &( *i ), sizeof(TransformStuffComponent) );
	}
	else
	{
		ar >> m_nTransform;
		size_t nSize;
		ar >> nSize;
#ifdef __WORLDSERVER
		if( nSize != CTransformItemProperty::Instance()->GetStuffSize( m_nTransform ) )
			return;
#endif	// __WORLDSERVER
		for( size_t i = 0; i < nSize; i++ )
		{
			TransformStuffComponent component;
			ar.Read( &component, sizeof(TransformStuffComponent ) );
			AddComponent( component.nItem, component.nNum );
		}
	}
}

#ifdef __WORLDSERVER
// 변환 객체
ITransformer::~ITransformer()
{
}

ITransformer* ITransformer::Transformer( int nTransformer )
{
	if( nTransformer == 0 )
		return CTransformerEgg::Instance();
	assert( 0 );
	return NULL;
}

void ITransformer::Transform( CUser* pUser, CTransformStuff& stuff )
{
	if( !IsValidStuff( pUser, stuff ) )
		return;
	RemoveItem( pUser, stuff );
	CreateItem( pUser, stuff );
}

void ITransformer::RemoveItem( CUser* pUser, CTransformStuff & stuff )
{
	for( DWORD i = 0; i < stuff.GetSize(); i++ )
	{
		TransformStuffComponent* pComponent	= stuff.GetComponent( i );
		CItemElem* pItem	= static_cast<CItemElem*>( pUser->GetItemId( pComponent->nItem ) );
		if( pItem )
		{
			g_DPSrvr.PutItemLog( pUser, "X", "transform-removestuff", pItem, pComponent->nNum );
			pUser->RemoveItem( pComponent->nItem, pComponent->nNum );
		}
	}
}

void ITransformer::CreateItem( CUser* pUser, CTransformStuff &stuff )
{
	CItemElem* pItem	= CTransformItemProperty::Instance()->GetItem( stuff.GetTransform() );
	if( pItem && pUser->CreateItem( pItem ) )
	{
		pUser->AddDefinedText( TID_GAME_TRANSFORM_S00 );
		g_DPSrvr.PutCreateItemLog( pUser, pItem, "X", "transform" );
	}
	else
		pUser->AddDefinedText( TID_GAME_TRANSFORM_S01 );
}

BOOL ITransformer::IsValidStuff( CUser* pUser, CTransformStuff & stuff )
{
	// 사용자가 보내온 재료와 실제 필요한 재료 개수가 다르면 부적합 재료
	if( stuff.GetSize() != CTransformItemProperty::Instance()->GetStuffSize( stuff.GetTransform() ) )
		return FALSE;

	// 사용자가 보내온 재료를 사용자가 가지고 있고
	// 사용가능한 상태인지를 검사한다
	for( DWORD i = 0; i < stuff.GetSize(); i++ )
	{
		TransformStuffComponent* pComponent		= stuff.GetComponent( i );
		CItemElem* pItem	= static_cast<CItemElem*>( pUser->GetItemId( pComponent->nItem ) );
		if( !pItem || pUser->IsUsing( pItem ) )
			return FALSE;
	}
	return TRUE;
}

CTransformerEgg::CTransformerEgg()
{
}

CTransformerEgg::~CTransformerEgg()
{
}

CTransformerEgg*	CTransformerEgg::Instance( void )
{
	static CTransformerEgg sTransformerEgg;
	return &sTransformerEgg;
}

BOOL CTransformerEgg::IsValidStuff( CUser* pUser, CTransformStuff & stuff )
{
	// 기본 재료 조건을 만족하지 않으면 부적합 재료
	if( !ITransformer::IsValidStuff( pUser, stuff ) )
		return FALSE;

	set<int> setItems;
	for( DWORD i = 0; i < stuff.GetSize(); i++ )
	{
		TransformStuffComponent* pComponent		= stuff.GetComponent( i );
		// 중복된 재료를 가지고 있다면 조작된 것이다
		if( setItems.insert( pComponent->nItem ).second == false )
			return FALSE;
		CItemElem* pItem	= static_cast<CItemElem*>( pUser->GetItemId( pComponent->nItem ) );
		ASSERT( pItem );
		// 알이 아니면 부적합 재료
		if( !pItem->IsEgg() )
			return FALSE;
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
CTransformItemComponent::CTransformItemComponent( int nTransform )
:
m_nTransform( nTransform ),
m_nTotalProb( 0 )
{
}

CTransformItemComponent::~CTransformItemComponent()
{
	Clear();
}

void CTransformItemComponent::Clear( void )
{
	for( VTIE::iterator i  = m_vTransformItemElements.begin(); i != m_vTransformItemElements.end(); ++i )
		safe_delete( ( *i ).pItem );
	m_vTransformItemElements.clear();
}

void CTransformItemComponent::AddElement( TransformItemElement element )
{
	AdjustmentProbability( element );
	m_vTransformItemElements.push_back( element );
}

void CTransformItemComponent::AdjustmentProbability( TransformItemElement & element )
{
	m_nTotalProb	+= element.nProb;
	element.nProb	= m_nTotalProb;
}

CItemElem* CTransformItemComponent::GetItem( void )
{
	int nProb	= xRandom( 0, eMaxProb );
	for( VTIE::iterator i  = m_vTransformItemElements.begin(); i != m_vTransformItemElements.end(); ++i )
	{
		if( nProb < ( *i ).nProb )
		{
			( *i ).pItem->SetSerialNumber( xRand() );
			return ( *i ).pItem;
		}
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
CTransformItemProperty::CTransformItemProperty()
{
}

CTransformItemProperty::~CTransformItemProperty()
{
	for( MPTIC::iterator i = m_mapComponents.begin(); i != m_mapComponents.end(); ++i )
		safe_delete( i->second );
	m_mapComponents.clear();
}

CTransformItemProperty*	CTransformItemProperty::Instance( void )
{
	static	CTransformItemProperty	sTransformItemProperty;
	return &sTransformItemProperty;
}

BOOL CTransformItemProperty::LoadScript( const char* szFile )
{
	CScript s;
	if( s.Load( szFile ) == FALSE )
		return FALSE;

	int nTransform	= s.GetNumber();		// subject or FINISHED
	while( s.tok != FINISHED )
	{
		CTransformItemComponent* pComponent		= new CTransformItemComponent( nTransform );
		AddComponent( pComponent );
		s.GetToken();	// {
		s.GetToken();	// subject or '}'
		while( *s.token != '}' )
		{
			if( s.Token == _T( "stuff" ) )
			{
				pComponent->SetStuffSize( s.GetNumber() );
			}
			else if( s.Token == _T( "item" ) )
			{
				s.GetToken();	// {
				int nProb	= s.GetNumber();
				while( *s.token != '}' )
				{
					int nType	= s.GetNumber();
					CItemElem* pItem;
					if( nType == TI_GENERIC )
						pItem	= CreateItemGeneric( s );
					else if( nType == TI_PET )
						pItem	= CreateItemPet( s );
					assert( pItem );
					pComponent->AddElement( TransformItemElement( pItem, nProb ) );
					nProb	= s.GetNumber();
				}
				AddComponent( pComponent );
			}
			s.GetToken();
		}
		nTransform	= s.GetNumber();
	}
	return TRUE;
}

CItemElem* CTransformItemProperty::CreateItemGeneric( CScript & s )
{
	CItemElem* pItem	= new CItemElem;
	pItem->m_dwItemId	= s.GetNumber();
	if( !pItem->GetProp() )
	{
		safe_delete( pItem );
		return NULL;
	}
	pItem->m_nItemNum	= s.GetNumber();
	return pItem;
}

CItemElem* CTransformItemProperty::CreateItemPet( CScript & s )
{
	CItemElem* pItem	= new CItemElem;
	pItem->m_nItemNum	= 1;
	CPet* pPet	= pItem->m_pPet	= new CPet;
	pPet->SetKind( s.GetNumber() );
	s.GetToken();
	pPet->SetLevel( s.Token.GetLength() );
	pItem->m_dwItemId	= pPet->GetItemId();
	BYTE anAvail[PL_MAX - 1]	= { 0,};
	char sAvail[2]	= { 0,};
	for( int i = 0; i < pPet->GetLevel(); i++ )
	{
		sAvail[0]	= s.Token.GetAt( i );
		sAvail[1]	= '\0';
		anAvail[i]	= atoi( sAvail );
		if( anAvail[i] < 1 || anAvail[i] > 9 )
		{
			safe_delete( pItem );
			return NULL;
		}
	}
	for( i = PL_D; i <= pPet->GetLevel(); i++ )
		pPet->SetAvailLevel( i, anAvail[i-1] );
	for( i = pPet->GetLevel() + 1; i <= PL_S; i++ )
		pPet->SetAvailLevel( i, 0 );
	pPet->SetLife( s.GetNumber() );
	DWORD dwExp		= (DWORD)( pPet->GetMaxExp() * s.GetFloat() / 100 );
	pPet->SetExp( dwExp );
	pPet->SetEnergy( pPet->GetMaxEnergy() );
	return pItem;
}

void CTransformItemProperty::AddComponent( CTransformItemComponent* pComponent )
{
	bool bResult	= m_mapComponents.insert( MPTIC::value_type( pComponent->GetTransform(), pComponent ) ).second;
}

CTransformItemComponent* CTransformItemProperty::GetComponent( int nTransform )
{
	MPTIC::iterator i	= m_mapComponents.find( nTransform );
	if( i != m_mapComponents.end() )
		return i->second;
	return NULL;
}

u_int CTransformItemProperty::GetStuffSize( int nTransform )
{
	CTransformItemComponent* pComponent	= GetComponent( nTransform );
	if( pComponent )
		return pComponent->GetStuffSize();
	return 0;
}

CItemElem* CTransformItemProperty::GetItem( int nTransform )
{
	CTransformItemComponent* pComponent	= GetComponent( nTransform );
	if( pComponent )
		return pComponent->GetItem();
	return NULL;
}

#endif	// __WORLDSERVER

#endif	// __PET_0519

#ifdef __PET_1024
void CPet::SetName( const char* szName )
{
	strncpy( m_szName, szName, MAX_PET_NAME - 1 );
	m_szName[MAX_PET_NAME-1]	= '\0';
}
#endif	// __PET_1024