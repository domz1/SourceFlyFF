#ifndef __NPC_PROPERTY__
#define __NPC_PROPERTY__

#include "Project.h"
#include "ScriptDialog.h"

class CNpcProperty
{
public:
	CNpcProperty();
	virtual ~CNpcProperty();

public:
	BOOL			LoadDialog( LPCHARACTER lpCharacter );
	BOOL			IsDialogLoaded();
	BOOL			IsTimeOut();
	void			ResetTimer();
	BOOL			RunDialog( LPCTSTR pKey, int* nResult, int nValue, int nDstId, int nSrcId, int nQuestId );

protected:

#if defined(__REMOVE_SCIRPT_060712)
	char			m_szName[64];
	int				m_nGlobal;
#else
	CScriptDialog	m_Dialog;				// 스크립트 대사 
#endif // __REMOVE_SCIRPT_060712
	DWORD			m_dwTick;
};

#endif
