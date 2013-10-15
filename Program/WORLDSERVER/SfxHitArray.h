#ifndef __SFXHITARRAY_H__
#define __SFXHITARRAY_H__

typedef	struct	tagSfxHit
{
	int		id;
	OBJID	objid;	// target
	DWORD	dwAtkFlags;
	DWORD	dwSkill;
	int		nMaxDmgCnt;
} SfxHit, *PSfxHit;

class CSfxHitArray
{
enum	{	nMaxSizeOfSfxHit	= 32,	};
private:
	SfxHit	m_aSfxHit[nMaxSizeOfSfxHit];
	int		m_id;

public:
//	Constructions
	CSfxHitArray();
	~CSfxHitArray();
//	Operations
	int		Add( int id, OBJID objid, DWORD dwAtkFlags, DWORD dwSkill = 0xffffffff, int nMaxDmgCnt = 1 );	// return id, 스킬일경우 dwSkill에 값 넣을것.
	PSfxHit		GetSfxHit( int id );
	BOOL	RemoveSfxHit( int id, BOOL bForce = FALSE );

private:
	int		FindSfxHit( int id );	// return index
	PSfxHit		GetAt( int nIndex );
	BOOL	RemoveAt( int nIndex, BOOL bForce );
};

#endif	// __SFXHITARRAY_H__
