#ifndef __weapon_level_EX_H__
#define	__weapon_level_EX_H__

#ifdef __LEVELABLE_WEAPON

#include <vector>

#define MAX_IK3_KIND	15
#define MAX_BONUS_LEVEL	10

typedef struct _WEAPON_BONUS_OPTION
{
	int nDst;
	int nAdj;
	_WEAPON_BONUS_OPTION()
	{
		nDst = 0;
		nAdj = 0;
	}
} WEAPON_BONUS_OPTION;

class CWeaponAdjParam
{
public:

	typedef struct _WEAPONADJPARAM
	{
		int nDst;
		int nAdj;
		_WEAPONADJPARAM()
		{
			nDst = 0;
			nAdj = 0;
		}
	} WEAPONADJPARAM;

	CWeaponAdjParam();
	virtual ~CWeaponAdjParam();
	void Add( int nDst, int nAdj );
	int GetSize() { return m_aWeaponAdj.size(); }
	vector<_WEAPONADJPARAM> m_aWeaponAdj;
};

class CWeaponLevelProperty
{
public:
	CWeaponLevelProperty();
	virtual	~CWeaponLevelProperty();
	static	CWeaponLevelProperty*	GetInstance();
	BOOL	LoadScript(LPCTSTR szFile );
	DWORD	GetMaxExp( int nLevel ) { return dwMaxExperience[nLevel]; }
	CWeaponAdjParam*	GetBonusWeaponOpt( DWORD dwItemKind3, int nLevel );

private:
	void	LoadScriptBonus( CScript& s );
	void	LoadScriptLevel( CScript& s );
	vector<WEAPON_BONUS_OPTION>	m_aBonusOption[MAX_IK3_KIND][MAX_BONUS_LEVEL];
	DWORD	dwMaxExperience[MAX_BONUS_LEVEL];

};

#define g_xWeaponLevelProperty		CWeaponLevelProperty::GetInstance()

#endif //__LEVELABLE_WEAPON

#endif	//__RANDOM_OPTION_EX_H__