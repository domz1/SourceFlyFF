#include "stdafx.h"
#include "randomoption.h"

CRandomOptionProperty::CRandomOptionProperty()
{
	memset( m_anRandomOptionProb, 0, sizeof(m_anRandomOptionProb) );
}

CRandomOptionProperty::~CRandomOptionProperty()
{
}

CRandomOptionProperty* CRandomOptionProperty::GetInstance()
{
	static	CRandomOptionProperty	sRandomOptionProperty;

	return	&sRandomOptionProperty;
}

void CRandomOptionProperty::LoadScriptBlock( CScript & s, int nRandomOptionKind )
{
	int nTotalProb1		= 0;
	for( int i = 0; i < 3; i++ )	// 3	// 랜덤 옵션이 몇개 부여될 것인가에 대한 확률
	{
		m_anRandomOptionProb[nRandomOptionKind][i]	= s.GetNumber() + nTotalProb1;
		nTotalProb1	= m_anRandomOptionProb[nRandomOptionKind][i];
	}
	s.GetToken();	// {
	int nTotalProb2	= 0;
	RANDOM_OPTION	ro;
	ro.nDst		= s.GetNumber();
	while( *s.token != '}' )
	{
		ASSERT( ro.nDst <= 0xFF );	// 255	// 8
		ro.nProb	= s.GetNumber() + nTotalProb2;
		nTotalProb2		= ro.nProb;
		s.GetToken();	// {
		short	nAdj	= s.GetNumber();
		DWORD dwTotalProb3	= 0;
		while( *s.token != '}' )
		{
			DWORD dwProb	= s.GetNumber() + dwTotalProb3;
			dwTotalProb3	= dwProb;
			ASSERT( abs( nAdj ) <= 0x1FF );		// 511	// 1 | 9
			ro.aAdjData.push_back( AdjData( nAdj, dwProb ) );
			nAdj	= s.GetNumber();
		}
		if( dwTotalProb3 != 1000000000 )
			Error( "Awakening: nDst = %d, dwTotalProb = %d", ro.nDst, dwTotalProb3 );
		m_aRandomOption[nRandomOptionKind].push_back( ro );
		ro.aAdjData.clear();
		ro.nDst		= s.GetNumber();
	}
}

BOOL CRandomOptionProperty::LoadScript( LPCTSTR szFile )
{
	CScript s;
	if( s.Load( szFile ) == FALSE )
		return FALSE;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "Awakening" ) )
			LoadScriptBlock( s, eAwakening );
		else if( s.Token == _T( "Blessing" ) )
			LoadScriptBlock( s, eBlessing );
#if __VER >= 12 // __PET_0519
		// 시스템 펫 각성과 먹펫 각성 추가
		else if( s.Token == _T( "SystemPet" ) )
			LoadScriptBlock( s, eSystemPet );
		else if( s.Token == _T( "EatPet" ) )
			LoadScriptBlock( s, eEatPet );
#endif	// __PET_0519
		s.GetToken();
	}
#if __VER >= 12 // __J12_0
	AwakeningExtension();	// 각성, 축복 가능 파츠 추가를 위한 테이블 확장
#endif	// __J12_0
	return TRUE;
}

void	CRandomOptionProperty::SetParam( __int64* pnRandomOptItemId, int nDst, int nAdj )
{
	int nSize	= GetRandomOptionSize( *pnRandomOptItemId );
	ASSERT( nSize < MAX_RANDOM_OPTION );

	__int64 nRandomOptionItemId		= nDst;
	nRandomOptionItemId		= nRandomOptionItemId << 10;		// 2 | 8 | 10 | 8 | 10 | 8 | 10 | 8
	//	2 : 

	//	1 :
	//	7 :
	//	10 :

	//	8 :

	if( nAdj < 0 )
	{
		nRandomOptionItemId	|= 0x0200;	// 음수인 경우 최상위 비트 1
		nRandomOptionItemId	= nRandomOptionItemId | ( -nAdj );
	}
	else
		nRandomOptionItemId	= nRandomOptionItemId | nAdj;

	nRandomOptionItemId	= nRandomOptionItemId << ( 8 + 18 * nSize );	// 해당 위치로 시프트
	*pnRandomOptItemId	|= nRandomOptionItemId;		// 결과 값 저장
}

int CRandomOptionProperty::GetRandomOptionKind( CItemElem* pItemElem )
{
	ItemProp* pProp		= pItemElem->GetProp();
	switch( pProp->dwParts )
	{
		case PARTS_UPPER_BODY:
		case PARTS_RWEAPON:
		case PARTS_SHIELD:
#if __VER >= 12 // __J12_0
		// 각성 가능 파츠 추가
		case PARTS_HAND:	// 손
		case PARTS_FOOT:	// 발
		case PARTS_CAP:		// 머리
#endif	// __J12_0
			{
					return static_cast<int>( eAwakening );
			}
		case PARTS_CLOTH:
		case PARTS_CLOAK:
#if __VER >= 12 // __J12_0
		case PARTS_HAT:		// 겉옷 머리
		case PARTS_GLOVE:	// 겉옷 손
		case PARTS_BOOTS:	// 겉옷 발
#endif	// __J12_0
			return static_cast<int>( eBlessing );
#if __VER >= 12 // __PET_0519
		default:
			{
				// C급 이상의 시스템 펫인가?
				if( pProp->dwItemKind3 == IK3_EGG && pItemElem->m_pPet && pItemElem->m_pPet->GetLevel() >= PL_C )
					return static_cast<int>( eSystemPet );
				// 먹펫인가?
				else if( pProp->dwItemKind3 == IK3_PET )
					return static_cast<int>( eEatPet );

				break;
			}
#endif	// __PET_0519
	}
	return -1;
}

BOOL	CRandomOptionProperty::GetParam( __int64 nRandomOptItemId, int i, int* pnDst, int* pnAdj )
{
//	ASSERT( i < MAX_RANDOM_OPTION );
	if( i >= MAX_RANDOM_OPTION )
		return FALSE;

	int nRandomOption	= static_cast<int>( nRandomOptItemId >> ( 8 + i * 18 ) );	// 값을 가져오기 위하여 해당 위치로 시프트
	*pnAdj	= nRandomOption & 0x000001FF;	// 하위 9비트 가져옴.
	if( nRandomOption & 0x00000200 )	// 최상위 10 비트가 1이면 음수
		*pnAdj	= -*pnAdj;
	nRandomOption	= nRandomOption >> 10;	// 10비트 밀고, 

	//	mulcom	BEGIN100405	각성 보호의 두루마리
	//*pnDst	= nRandomOption & 0x000000FF;	// 8비트 가져옴
	*pnDst	= nRandomOption & 0x0000007F;	// 8비트 가져옴
	//	mulcom	END100405	각성 보호의 두루마리

	return ( *pnDst > 0 );
}

int CRandomOptionProperty::GetRandomOptionSize( __int64 nRandomOptItemId )
{
	int nSize	= 0;

	//	mulcom	BEGIN100405	각성 보호의 두루마리
	bool	bCheckedSafeFlag = false;
	bCheckedSafeFlag	= IsCheckedSafeFlag( nRandomOptItemId );

	if( bCheckedSafeFlag == true )
	{
		return	nSize;
	}
	//	mulcom	END100405	각성 보호의 두루마리

	__int64 i	= 0x3FFFF << 8;
	for( int j = 0; j < MAX_RANDOM_OPTION; j++ )
	{
		if( nRandomOptItemId & i )
			nSize++;
		else
			return nSize;

		i	= i << 18;
	}

	return nSize;
}


//	mulcom	BEGIN100405		각성 보호의 두루마리
int		CRandomOptionProperty::GetViewRandomOptionSize( __int64 n64RandomOptItemId )
{
	int nSize	= 0;

	__int64 i	= 0x3FFFF << 8;
	for( int j = 0; j < MAX_RANDOM_OPTION; j++ )
	{
		if( n64RandomOptItemId & i )
			nSize++;
		else
			return nSize;

		i	= i << 18;
	}

	return nSize;
}
//	mulcom	END100405	각성 보호의 두루마리


//	mulcom	BEGIN100405	각성 보호의 두루마리
//BOOL	CRandomOptionProperty::GenRandomOption( __int64* pnRandomOptItemId, int nRandomOptionKind, int nParts )
BOOL	CRandomOptionProperty::GenRandomOption( __int64* pnRandomOptItemId, int nRandomOptionKind, int nParts, bool bDecreaseFlag /* = false */ )
//	mulcom	END100405	각성 보호의 두루마리
{ 
	if( GetRandomOptionSize( *pnRandomOptItemId )  > 0 )
	{
		return FALSE;
	}

	int nRandomOptionSize	= DetermineRandomOptionSize( nRandomOptionKind );

	//	mulcom	BEGIN100405	각성 보호의 두루마리
	int	nDecreaseNumber	= -1;

	if( bDecreaseFlag == true )
	{
		nDecreaseNumber = xRandom( nRandomOptionSize );
	}
	//	mulcom	END100405	각성 보호의 두루마리

	
	while( nRandomOptionSize-- > 0 )
	{
		RANDOM_OPTION* pRandomOption	= DetermineRandomOptionDst( nRandomOptionKind, nParts );
		ASSERT( pRandomOption );


		//	mulcom	BEGIN100405	각성 보호의 두루마리
		//short nAdj	= DetermineRandomOptionAdj( pRandomOption );
		bool	bDecreaseAdj	= false;

		if( nRandomOptionSize == nDecreaseNumber )
		{
			bDecreaseAdj = true;
		}
		else
		{
			bDecreaseAdj = false;
		}

		short nAdj	= DetermineRandomOptionAdj( pRandomOption, bDecreaseAdj );
		//	mulcom	END100405	각성 보호의 두루마리

		SetParam( pnRandomOptItemId, pRandomOption->nDst, nAdj );
	}
	return TRUE;
}

int	CRandomOptionProperty::DetermineRandomOptionSize( int nRandomOptionKind )
{
	int nUpper	= m_anRandomOptionProb[nRandomOptionKind][MAX_RANDOM_OPTION - 1];
	DWORD dwRand	= xRandom( nUpper );
	for( int i = 0; i < MAX_RANDOM_OPTION; i++ )
	{
		if( (int)( dwRand ) < m_anRandomOptionProb[nRandomOptionKind][i] )
			return i + 1;
	}
	return 0;
}

RANDOM_OPTION*	CRandomOptionProperty::DetermineRandomOptionDst( int nRandomOptionKind, int nParts )
{
	int iRandomOptionKindIndex	= GetRandomOptionKindIndex( nRandomOptionKind, nParts );
	DWORD dwRand	= xRandom( GetUpperProbability( iRandomOptionKindIndex ) );
	for( DWORD i = 0; i < m_aRandomOption[iRandomOptionKindIndex].size(); i++ )
	{
		RANDOM_OPTION* pRandomOption	= &m_aRandomOption[iRandomOptionKindIndex][i];
		if( (int)( dwRand ) < pRandomOption->nProb )
		{
			return pRandomOption;
		}
	}
	return NULL;
}

//	mulcom	BEGIN100405	각성 보호의 두루마리
//short CRandomOptionProperty::DetermineRandomOptionAdj( RANDOM_OPTION* pRandomOption )
short CRandomOptionProperty::DetermineRandomOptionAdj( RANDOM_OPTION* pRandomOption, bool bDecreaseAdj /* = false */ )
//	mulcom	END100405	각성 보호의 두루마리
{
	ASSERT( pRandomOption );
	DWORD dwRand	= xRandom( 1000000000 );	// 0 - 9999999
	for( DWORD i = 0; i < pRandomOption->aAdjData.size(); i++ )
	{
		AdjData	ad	= pRandomOption->aAdjData[i];
		if( dwRand < ad.dwProb )
		{
			//	mulcom	BEGIN100405
			//return ad.nAdj;
			if( bDecreaseAdj == true && i > 0 )
			{
				return	( pRandomOption->aAdjData[i-1].nAdj );
			}
			else
			{
				return ad.nAdj;
			}
			//	mulcom	END100405
		}
	}
	Error( "CRandomOptionProperty.DetermineRandomOptionAdj: 0" );
	return 0;
}


int	CRandomOptionProperty::GetRandomOptionKindIndex( int nRandomOptionKind, int nParts )
{	// 각성 축복 시 참조할 테이블의 인덱스를 반환한다
#if __VER >= 12 // __J12_0
	//PARTS_HAND: 4, PARTS_FOOT: 5, PARTS_CAP: 6
	// 기본 테이블의 마지막 인덱스부터 확장 인덱스 시작한다
	if( nRandomOptionKind == eAwakening && ( nParts == PARTS_HAND || nParts == PARTS_FOOT || nParts == PARTS_CAP ) )
		return eMaxRandomOptionKind + nParts - PARTS_HAND;
#endif	// __J12_0
	return nRandomOptionKind;
}

void CRandomOptionProperty::InitializeRandomOption( __int64* pnRandomOptItemId )
{
	*pnRandomOptItemId	&= 0xC0000000000000FF;
}




//	mulcom	BEGIN100405	각성 보호의 두루마리
void	CRandomOptionProperty::SetSafeFlag( __int64* pn64RandomOption )
{
	if( pn64RandomOption == NULL )
	{
		WriteLog( "pn64RandomOption is NULL in SetSafeFlag function" );
	}
	else
	{
		bool	bCheckedSafeFlag = false;
		bCheckedSafeFlag	= IsCheckedSafeFlag( *pn64RandomOption );

		if( bCheckedSafeFlag == true )
		{
			WriteLog( "bCheckedSafeFlag is already true in SetSafeFlag function." );
		}
		else
		{
			*pn64RandomOption = *pn64RandomOption | _AWAKE_SAFE_FLAG;
		}
	}

	return;
}

void	CRandomOptionProperty::ResetSafeFlag( __int64* pn64RandomOption )
{
	if( pn64RandomOption == NULL )
	{
		WriteLog( "pn64RandomOption is NULL in ResetSafeFlag function" );
	}
	else
	{
		bool	bCheckedSafeFalg = false;
		bCheckedSafeFalg	= IsCheckedSafeFlag( *pn64RandomOption );

		if( bCheckedSafeFalg == true )
		{
			*pn64RandomOption = *pn64RandomOption & ~(_AWAKE_SAFE_FLAG);
		}
		else
		{
			WriteLog( "bCheckedSafeFlag is already false in ResetSafeFlag function." );
		}
	}

	return;
}

bool	CRandomOptionProperty::IsCheckedSafeFlag( __int64 n64RandomeOption )
{
	if( (n64RandomeOption & _AWAKE_SAFE_FLAG) == _AWAKE_SAFE_FLAG )
	{
#ifdef __PROTECT_AWAKE
		return	true;
#endif	
	}

	return	false;
}
//	mulcom	END100405	각성 보호의 두루마리

#ifdef __FASHION_COMBINE
int CRandomOptionProperty::MergeRandomOption( CItemElem* pItemElemMain, CItemElem* pItemElemDestroy, int nMaxCombinedSize )
{
	int nMainSize = g_xRandomOptionProperty->GetRandomOptionSize( pItemElemMain->GetRandomOptItemId() );
	int nDestroySize = g_xRandomOptionProperty->GetRandomOptionSize( pItemElemDestroy->GetRandomOptItemId() );

	if( nMainSize + nDestroySize > nMaxCombinedSize )
		return 1;

	for( int i = 0; i < nDestroySize; i++ )
	{
		int nDst, nAdj;
		if( g_xRandomOptionProperty->GetParam( pItemElemDestroy->GetRandomOptItemId(), i, &nDst, &nAdj ) )
			g_xRandomOptionProperty->SetParam( pItemElemMain->GetRandomOptItemIdPtr(), nDst, nAdj );
	}
	return 0;
}
#endif // __FASHION_COMBINE

#if __VER >= 12 // __J12_0
void CRandomOptionProperty::AwakeningExtension( void )
{
	int	anDst[eAwakeningExtension][6]	= {		// 각 부분 별 각성 가능 속성
		DST_CHR_CHANCECRITICAL, DST_ATKPOWER, DST_SPELL_RATE, DST_ATTACKSPEED, DST_MP_MAX, DST_FP_MAX,
		DST_CRITICAL_BONUS, DST_SPEED, DST_ADJDEF, DST_HP_MAX, DST_MP_MAX, DST_FP_MAX,
		DST_STR, DST_DEX, DST_INT, DST_STA, DST_MP_MAX, DST_FP_MAX	};
	int anTotal[eAwakeningExtension];
	memset( anTotal, 0, sizeof(int) * eAwakeningExtension );

	// 각성 테이블에서 해당 부분에 각성 가능한 속성만을 포함하도록
	// 각성 테이블 확장 후 구성한다
	for( DWORD i = 0; i < m_aRandomOption[eAwakening].size(); i++ )
	{
		RANDOM_OPTION* pRandomOption	= &m_aRandomOption[eAwakening][i];
		int nProb	= ( i == 0?
			m_aRandomOption[eAwakening][i].nProb:
			m_aRandomOption[eAwakening][i].nProb - m_aRandomOption[eAwakening][i-1].nProb );

		for( int j = 0; j < eAwakeningExtension; j++ )
		{
			int iRandomOptionKindIndex	= GetRandomOptionKindIndex( eAwakening, PARTS_HAND + j );
			for( int k = 0; k < 6; k++ )
				if( pRandomOption->nDst == anDst[j][k] )
				{
					RANDOM_OPTION ro( pRandomOption );
					ro.nProb	= anTotal[j] + nProb;
					anTotal[j]	= ro.nProb;
					m_aRandomOption[iRandomOptionKindIndex].push_back( ro );
				}
		}
	}
}
#endif	// __J12_0

int	CRandomOptionProperty::GetUpperProbability( int iRandomOptionKindIndex )
{
	int iLast	= m_aRandomOption[iRandomOptionKindIndex].size() - 1;
	return m_aRandomOption[iRandomOptionKindIndex][iLast].nProb;
}