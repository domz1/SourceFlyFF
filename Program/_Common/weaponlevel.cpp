#include "stdafx.h"
#include "weaponlevel.h"

#ifdef __LEVELABLE_WEAPON

CWeaponLevelProperty::CWeaponLevelProperty()
{
}

CWeaponLevelProperty::~CWeaponLevelProperty()
{
}

CWeaponLevelProperty* CWeaponLevelProperty::GetInstance()
{
	static	CWeaponLevelProperty	sWeaponLevelProperty;

	return	&sWeaponLevelProperty;
}
BOOL CWeaponLevelProperty::LoadScript( LPCTSTR szFile )
{
	CScript s;
	if( s.Load( szFile ) == FALSE )
		return FALSE;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "Bonusoption" ) )
			LoadScriptBonus( s );
		else if( s.Token == _T( "Leveloption" ) )
			LoadScriptLevel( s );

		s.GetToken();
	}
	return TRUE;
}

void CWeaponLevelProperty::LoadScriptBonus( CScript & s )
{
	Error( "Start LoadScriptBonus()" );
	s.GetToken(); // {
	int nItemKind = s.GetNumber();
	Error( "itemkind %i", nItemKind );
	while( *s.token != '}' )
	{
		if( nItemKind >= MAX_IK3_KIND )
			Error( "To High IK3 %i", nItemKind );
		else
		{
			s.GetToken(); // {
			int nLevel = s.GetNumber();
			while( *s.token != '}' )
			{
				int nDst   = s.GetNumber(); //nDst
				int nAdj   = s.GetNumber(); //nAdj
				if( nLevel > MAX_BONUS_LEVEL )
					Error( "LoadScriptBonus() Max Bonuslevel overwritet %i", nLevel );
				else
				{
					WEAPON_BONUS_OPTION wbo;
					wbo.nDst = nDst;
					wbo.nAdj = nAdj;
					m_aBonusOption[nItemKind][nLevel - 1].push_back( wbo );
				}
				nLevel = s.GetNumber();
			}
		}
		nItemKind = s.GetNumber();
	}
	Error( "Start listening ");
	for( int i = 0; i < MAX_IK3_KIND; i++ )
	{
		for( int j = 0; j < MAX_BONUS_LEVEL; j++ )
		{
			for( int k = 0; k < m_aBonusOption[i][j].size(); j++ )
				Error( "nItemkind %i nLevel %i nDst %i nAdj %i", i, j, m_aBonusOption[i][j].at(k).nDst, m_aBonusOption[i][j].at(k).nAdj );
		}
	}
	Error( "End listening ");
}
void CWeaponLevelProperty::LoadScriptLevel( CScript & s )
{
	Error( "Start loading LoadScriptLevel()" );
	s.GetToken();	// {
	int nLevel = s.GetNumber();
	while( *s.token != '}' )
	{
		DWORD dwExperience = s.GetNumber();
		if( nLevel > MAX_BONUS_LEVEL )
			Error( "LoadScriptLevel() Max Level overwritet %i", nLevel );
		else
			dwMaxExperience[nLevel - 1] = dwExperience;

		nLevel = s.GetNumber();
	}
	Error( "Start watching LoadScriptLevel()" );
	for( int i = 0; i < MAX_BONUS_LEVEL; i++ )
	{
		Error( "Level %i, max experience %d", i, dwMaxExperience[i] );
	}
}

CWeaponAdjParam::CWeaponAdjParam()
{
}
CWeaponAdjParam::~CWeaponAdjParam()
{
}
CWeaponAdjParam* CWeaponLevelProperty::GetBonusWeaponOpt( DWORD dwItemKind3, int nLevel )
{
	CWeaponAdjParam* pWeaponAdjParam = new CWeaponAdjParam;
	for( int i = 0; i < m_aBonusOption[dwItemKind3][nLevel - 1].size(); i++ )
		pWeaponAdjParam->Add( m_aBonusOption[dwItemKind3][nLevel - 1].at( i ).nDst, m_aBonusOption[dwItemKind3][nLevel - 1].at( i ).nAdj );

	return pWeaponAdjParam;
}
void CWeaponAdjParam::Add( int nDst, int nAdj )
{
	_WEAPONADJPARAM wpadj;
	wpadj.nDst = nDst;
	wpadj.nAdj = nAdj;
	m_aWeaponAdj.push_back( wpadj );
}

#endif //__LEVELABLE_WEAPON