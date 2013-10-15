#include "stdafx.h"
#include "accessory.h"

CAccessoryProperty::CAccessoryProperty()
{
	memset( m_adwProbability, 0, sizeof(m_adwProbability) );
}

CAccessoryProperty::~CAccessoryProperty()
{
	for( map<DWORD, vector<SINGLE_DST>*>::iterator i = m_mapAccessory.begin(); i != m_mapAccessory.end(); ++i )
	{
		vector<SINGLE_DST>* pVector	= i->second;
		safe_delete_array( pVector );
	}
	m_mapAccessory.clear();
}

CAccessoryProperty*	CAccessoryProperty::GetInstance( void )
{
	static	CAccessoryProperty	sAccessoryProperty;
	return &sAccessoryProperty;
}

BOOL CAccessoryProperty::IsAccessory( DWORD dwItemId )
{
	map<DWORD, vector<SINGLE_DST>*>::iterator i	= m_mapAccessory.find( dwItemId );
	return( i != m_mapAccessory.end() );
}

vector<SINGLE_DST>* CAccessoryProperty::GetDst( DWORD dwItemId, int nAbilityOption )
{
	map<DWORD, vector<SINGLE_DST>*>::iterator i	= m_mapAccessory.find( dwItemId );
	if( i != m_mapAccessory.end() )
		return &( i->second[nAbilityOption] );
	return NULL;
}

BOOL CAccessoryProperty::LoadScript( LPCTSTR szFile )
{
	CScript s;
	if( s.Load( szFile ) == FALSE )
		return FALSE;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "Probability" ) )	// 아이템 각성
		{
			int i	= 0;
			s.GetToken();	// {
			DWORD	dwProbability	= s.GetNumber();
			while( *s.token != '}' )
			{
				m_adwProbability[i++]	= dwProbability;
				dwProbability	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "Accessory" ) )
		{
			s.GetToken();	// {
			DWORD dwItemId	= s.GetNumber();
			while( *s.token != '}' )
			{
				s.GetToken();	// {
				vector<SINGLE_DST>* pVector	= new vector<SINGLE_DST>[MAX_AAO+1];
				int nAbilityOption	= s.GetNumber();
				while( *s.token != '}' )
				{
					ASSERT( nAbilityOption >= 0 && nAbilityOption <= MAX_AAO );
					s.GetToken();	// {
					SINGLE_DST dst;
					dst.nDst	= s.GetNumber();
					while( *s.token != '}' )
					{
						dst.nAdj	= s.GetNumber();
						pVector[nAbilityOption].push_back( dst );
						dst.nDst	= s.GetNumber();
					}
					nAbilityOption	= s.GetNumber();
				}
				bool b	= m_mapAccessory.insert( map<DWORD, vector<SINGLE_DST>*>::value_type( dwItemId, pVector ) ).second;
				ASSERT( b );
				dwItemId	= s.GetNumber();
			}
		}
		s.GetToken();	//
	}
	return TRUE;
}