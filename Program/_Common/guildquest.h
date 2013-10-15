#ifndef __GUILDQUEST_H__
#define	__GUILDQUEST_H__

#pragma once

#include "projectcmn.h"

#define	MAX_GUILD_QUEST_TITLE_LEN		32
#define	MAX_GUILD_QUEST_STATE		16
#define	MAX_GUILD_QUEST		256
#define	MAX_GUILD_QUEST_DESC_LEN	260

struct	GUILDQUESTPROP
{
	char	szTitle[MAX_GUILD_QUEST_TITLE_LEN];
	int		nLevel;
	char	szDesc[MAX_GUILD_QUEST_STATE][MAX_GUILD_QUEST_DESC_LEN];
	DWORD	dwWormon;
	DWORD	dwWorldId;
	D3DXVECTOR3		vPos;
	int	x1;
	int y1;
	int x2;
	int y2;
//	DWORD	dwTime;
	
	GUILDQUESTPROP()
		{
			ZeroMemory( szTitle, sizeof(szTitle) );
			nLevel	= 0;
//			dwTime	= 0;
			ZeroMemory( szDesc, sizeof(szDesc) );
			dwWormon	= 0;
			dwWorldId	= 0;
			vPos	= D3DXVECTOR3( 0, 0, 0 );
			x1	= y1	= x2	= y2	= 0;
		};
};

typedef	struct	_GUILDQUEST
{
	int		nId;
	int		nState;
	u_long	idGuild;

	_GUILDQUEST()
		{
			nId		= -1;
			nState	= 0;
			idGuild		= 0;
		};
}
GUILDQUEST,	*PGUILDQUEST;

enum
{
	GQP_READY,	GQP_WORMON,	GQP_GETITEM,
};

typedef struct	_GUILDQUESTELEM : public _GUILDQUEST
{
	DWORD	dwEndTime;
	BYTE	nProcess;
	BYTE	ns;
	BYTE	nf;
	OBJID	objidWormon;
	short	nCount;

	_GUILDQUESTELEM() : _GUILDQUEST()
	{
		dwEndTime	= 0;
		nProcess	= GQP_READY;
		ns	= nf	= 0;
		objidWormon		= NULL_ID;
		nCount	= 0;
	};
}
GUILDQUESTELEM, *PGUILDQUESTELEM;

typedef struct	__GUILDQUESTRECT
{
	int nId;
	CRect rect;
}
GUILDQUESTRECT, *PGUILDQUESTRECT;

class CGuild;
class CGuildQuestProcessor
{
private:
	GUILDQUESTELEM	m_pElem[MAX_GUILD_QUEST];
	GUILDQUESTRECT	m_pRect[MAX_GUILD_QUEST];
	int		m_nRect;
public:
//	Constructions
	CGuildQuestProcessor();
	~CGuildQuestProcessor();

//	Operations
	void	Process();

	void	SetGuildQuest( int nQuestId, int nState, int ns, int nf, u_long idGuild, OBJID objidWormon );
	PGUILDQUESTELEM GetGuildQuest( int nQuestId );
	void	RemoveGuildQuest( int nQuestId );
	BOOL	IsQuesting( int nQuestId );
	static	CGuildQuestProcessor* GetInstance( void );

	void	AddQuestRect( int nId, int x1, int y1, int x2, int y2 );
	int		PtInQuestRect( const D3DXVECTOR3 & vPos );
	CRect* GetQuestRect( int nId );
};
#endif	// __GUILDQUEST_H__