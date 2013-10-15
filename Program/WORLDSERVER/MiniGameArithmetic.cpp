#include "stdafx.h"

#if __VER >= 13 // __RAINBOW_RACE
#include ".\minigamearithmetic.h"

CMiniGameArithmetic::CMiniGameArithmetic(void)
{
	InitPrimeNumber();
}

CMiniGameArithmetic::CMiniGameArithmetic( CMiniGameBase* pMiniGame )
: m_nResult( 0 ), m_nCorrectCount( 0 )
{
	CMiniGameArithmetic* pMiniGameArithmetic = static_cast<CMiniGameArithmetic*>( pMiniGame );

	m_vecnPrimeNumber.assign( pMiniGameArithmetic->m_vecnPrimeNumber.begin(), pMiniGameArithmetic->m_vecnPrimeNumber.end() );
}

CMiniGameArithmetic::~CMiniGameArithmetic(void)
{
}

BOOL CMiniGameArithmetic::Excute( CUser* pUser, __MINIGAME_PACKET* pMiniGamePacket )
{
	BOOL bReturn = FALSE;
	
	__MINIGAME_EXT_PACKET MP( pMiniGamePacket->wNowGame );	
	if( pMiniGamePacket->nState == MP_OPENWND || pMiniGamePacket->nState == MP_FAIL )
	{
		MP.nState = pMiniGamePacket->nState;
		if( pMiniGamePacket->nState == MP_OPENWND )
			m_nCorrectCount = 0;		// 창을 새로 열면 정답수 초기화..
		MP.nParam1 = m_nCorrectCount;
		MP.vecszData.push_back( MakeQuestion() );
		SendExtPacket( pUser, MP );
		return bReturn;
	}

	if( pMiniGamePacket->nParam1 == m_nResult )
	{
		m_nCorrectCount++;
		if( m_nCorrectCount == 5 )		// 5문제 정답을 완료!!
		{
			MP.nState = MP_FINISH;
			bReturn = TRUE;
		}
		else
		{
			MP.nState = MP_TRUE;
			MP.vecszData.push_back( MakeQuestion() );
		}
	}
	else
	{
		MP.nState = MP_FALSE;
		MP.vecszData.push_back( MakeQuestion() );
		m_nCorrectCount = 0;
	}

	MP.nParam1 = m_nCorrectCount;

	SendExtPacket( pUser, MP );
	return bReturn;
}

void CMiniGameArithmetic::InitPrimeNumber()
{
	for( int i=0; i<1000; i++ )
	{
		int nCount	= 0;
		for( int j = 2; j <= i; j++ )
		{
			if( (float)i / (float)j == i / j )
				nCount++;
			if( nCount > 1 )
				break;
		}
		if( nCount == 1 )
			m_vecnPrimeNumber.push_back( i );
	}
}

int	CMiniGameArithmetic::GetDivisor( int nDivided )
{
	vector<int> vDivisors;
	vDivisors.push_back( 1 );
	int nFound	= 0;
	BOOL bLoop = TRUE;
	while( bLoop )
	{
		for( int i = 0; i < (int)( m_vecnPrimeNumber.size() ); ++i )
		{
			if( m_vecnPrimeNumber[i] > nDivided )
			{
				bLoop = FALSE;
				break;
			}

			if( (float)nDivided / (float)m_vecnPrimeNumber[i] == nDivided / m_vecnPrimeNumber[i] )
			{
				vDivisors.push_back( m_vecnPrimeNumber[i] );
				nDivided	/= m_vecnPrimeNumber[i];
				break;
			}
		}
//		if( nFound == 0 )
//			bLoop = FALSE;
	}

	int n	= xRandom( vDivisors.size() ) + 1;
	int nDivisor	=1;
	while( n-- > 0 )
	{
		int iIndex	= xRandom( vDivisors.size() );
		nDivisor	*= vDivisors[iIndex];
		vector<int>::iterator it = vDivisors.begin();
		it += iIndex;
		vDivisors.erase( it );
	}
	return nDivisor;
}

string CMiniGameArithmetic::MakeQuestion()
{
	int nNum[3] = {0,};
	int nOper[3] = {0,};
	char szQuestion[20] = {0,};

	for( int i=0; i<3; i++ )
	{
		nNum[i] = xRandom( 1, 1000 );
		nOper[i] = xRandom( 4 );
	}

	int nResult = 0;
	if( nOper[2] > 1 )		// 숫자가 3개인 경우다. 
	{
		if( (nOper[0] < 2) && (nOper[1] > 1) ) // 뒤쪽 연산자가 우선인 경우( *, / )
		{
			nResult = Calculate( nNum[1], nNum[2], nOper[1] );
			nResult = Calculate( nNum[0], nResult, nOper[0] );
		}
		else
		{
			nResult = Calculate( nNum[0], nNum[1], nOper[0] );
			nResult = Calculate( nResult, nNum[2], nOper[1] );
		}
		sprintf( szQuestion, "%d %c %d %c %d", nNum[0], GetOperatorCh( nOper[0] ), nNum[1],
														GetOperatorCh( nOper[1] ), nNum[2] );
	}
	else
	{
		nResult = Calculate( nNum[0], nNum[1], nOper[0] );
		sprintf( szQuestion, "%d %c %d", nNum[0], GetOperatorCh( nOper[0] ), nNum[1] );
	}

	m_nResult = nResult;
	return szQuestion;
}

int CMiniGameArithmetic::Calculate( int n1, int& n2, int nOper )
{
	switch( nOper )
	{
		case 0:	return n1 + n2;
		case 1:	return n1 - n2;
		case 2:	return n1 * n2;
		case 3:	n2 = GetDivisor( n1 );
				return n1 / n2;
	}

	return -1;
}

char CMiniGameArithmetic::GetOperatorCh( int nOper )
{
	switch( nOper )
	{
		case 0: return '+';
		case 1: return '-';
		case 2: return '*';
		case 3: return '/';
	}

	return '+';
}
#endif // __RAINBOW_RACE