#include "stdafx.h"
#include "FSMstate.h"


//////////////////////////////////////////////////////////////////////
// CFSMstate
//////////////////////////////////////////////////////////////////////

CFSMstate::CFSMstate( int nStateID )
{
	m_nStateID = nStateID;
	m_nPolling = 0;
}

CFSMstate::~CFSMstate()
{
	int nSize = m_array.size();
	for( int i=0; i<nSize; ++i )
		safe_delete( m_array[i] );
}

//////////////////////////////////////////////////////////////////////
// member
//////////////////////////////////////////////////////////////////////
void CFSMstate::SetPolling( int nPolling )
{
	m_nPolling = nPolling;
}

int	CFSMstate::GetPolling()
{
	return m_nPolling;
}

void CFSMstate::AddTransition( int nInput, int nOutput, DWORD dwExtra )
{
	FSM_STATE* p = new FSM_STATE;
	p->nInput = nInput;
	p->nOutput = nOutput;
	p->dwExtra = dwExtra;

	m_array.push_back( p );
}

BOOL CFSMstate::GetOutput( int nInput, int& nOutputID )
{
	// for each possible transistion
	int nSize = m_array.size();
	for( int i=0; i<nSize; ++i )
	{
		FSM_STATE* p = m_array[i];
		// state transition function: look for a match with the input value
		if( nInput == p->nInput )
		{
			nOutputID = p->nOutput;	// output state id
			return TRUE;
		}
	}
	return FALSE;
}


