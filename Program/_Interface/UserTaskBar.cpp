// UserTaskBar.cpp: implementation of the CUserTaskBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UserTaskBar.h"
#ifdef __WORLDSERVER
#include "User.h"
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUserTaskBar::CUserTaskBar()
{
	InitTaskBar();
}

CUserTaskBar::~CUserTaskBar()
{

}

void CUserTaskBar::InitTaskBar()
{
	memset( m_aSlotApplet, 0, sizeof( m_aSlotApplet ) );
	memset( m_aSlotItem  , 0, sizeof( m_aSlotItem ) );
	memset( m_aSlotQueue , 0, sizeof( m_aSlotQueue ) );
	m_nActionPoint = 0;
	m_nUsedSkillQueue = -1;
}

void CUserTaskBar::InitTaskBarShorcutKind( DWORD dwShortcutKind )
{
	for( int i = 0; i < MAX_SLOT_APPLET; i++ )
	{
		if( m_aSlotApplet[i].m_dwShortcut == dwShortcutKind )		// 값이 들어있는지 검사
			memset( &m_aSlotApplet[i], 0, sizeof( SHORTCUT ) );
	}

	for( i = 0; i < MAX_SLOT_ITEM_COUNT; i++ )
	{
		for( int j = 0; j < MAX_SLOT_ITEM; j++ )
		{
			if( m_aSlotItem[i][j].m_dwShortcut == dwShortcutKind )		// 값이 들어있는지 검사
			{
				memset( &m_aSlotItem[i][j], 0, sizeof( SHORTCUT ) );
			}
		}
	}
	for( i = 0; i < MAX_SLOT_QUEUE; i++ )
	{
		if( m_aSlotQueue[i].m_dwShortcut == dwShortcutKind )		// 값이 들어있는지 검사
		{
			memset( &m_aSlotQueue[i], 0, sizeof( SHORTCUT ) );
		}
	}
}

void CUserTaskBar::Serialize( CAr &ar )
{
	if( ar.IsStoring() )
	{
		int nCount	= 0;
		u_long uOffset	= ar.GetOffset();
		ar << nCount;
		for( int i = 0; i < MAX_SLOT_APPLET; i++ )
		{
			if( m_aSlotApplet[i].m_dwShortcut != SHORTCUT_NONE )		// 값이 들어있는지 검사
			{
				ar << i;
				ar << m_aSlotApplet[i].m_dwShortcut <<	m_aSlotApplet[i].m_dwId << m_aSlotApplet[i].m_dwType;
				ar << m_aSlotApplet[i].m_dwIndex <<	m_aSlotApplet[i].m_dwUserId << m_aSlotApplet[i].m_dwData;
				if( m_aSlotApplet[i].m_dwShortcut == SHORTCUT_CHAT)
					ar.WriteString( m_aSlotApplet[i].m_szString );
				nCount++;
			}
		}
		int nBufSize;
		LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
		*(UNALIGNED int*)( lpBuf + uOffset )	= nCount;

		nCount	= 0;
		uOffset	= ar.GetOffset();
		ar << nCount;
		for( i = 0; i < MAX_SLOT_ITEM_COUNT; i++ )
		{
			for( int j = 0; j < MAX_SLOT_ITEM; j++ )
			{
				if( m_aSlotItem[i][j].m_dwShortcut != SHORTCUT_NONE )		// 값이 들어있는지 검사
				{
					ar << i << j;
					ar << m_aSlotItem[i][j].m_dwShortcut <<	m_aSlotItem[i][j].m_dwId << m_aSlotItem[i][j].m_dwType;
					ar << m_aSlotItem[i][j].m_dwIndex << m_aSlotItem[i][j].m_dwUserId << m_aSlotItem[i][j].m_dwData;
					if( m_aSlotItem[i][j].m_dwShortcut == SHORTCUT_CHAT)
						ar.WriteString( m_aSlotItem[i][j].m_szString );
					nCount++;
					
				}

				// m_aSlotItem[i][j]이 유효한 숏컷이라면
//				ar << i << j << m_aSlotItem[i][j]의 필요한 정보 저장
//				if( m_aSlotItem[i][j].m_dwType == SHORTCUT_CHAT )
//					ar.WriteString(
			}
		}
		lpBuf	= ar.GetBuffer( &nBufSize );
		*(UNALIGNED int*)( lpBuf + uOffset )	= nCount;

		nCount	= 0;
		uOffset	= ar.GetOffset();
		ar << nCount;
		for( i = 0; i < MAX_SLOT_QUEUE; i++ )
		{
			if( m_aSlotQueue[i].m_dwShortcut != SHORTCUT_NONE )		// 값이 들어있는지 검사
			{
				ar << i;
				ar << m_aSlotQueue[i].m_dwShortcut << m_aSlotQueue[i].m_dwId << m_aSlotQueue[i].m_dwType;
				ar << m_aSlotQueue[i].m_dwIndex << m_aSlotQueue[i].m_dwUserId << m_aSlotQueue[i].m_dwData;
				nCount++;
			}
			
							// m_aSlotQueue[i]이 유효한 숏컷이라면
//				ar << i << m_aSlotQueue[i]의 필요한 정보 저장
		}
		ar << m_nActionPoint;
		lpBuf	= ar.GetBuffer( &nBufSize );
		*(UNALIGNED int*)( lpBuf + uOffset )	= nCount;
	}
	else
	{
		memset( m_aSlotApplet, 0, sizeof(m_aSlotApplet) );
		memset( m_aSlotItem, 0, sizeof(m_aSlotItem) );
		memset( m_aSlotQueue, 0, sizeof(m_aSlotQueue) );

		int nCount, nIndex;
		ar >> nCount;	// applet count
		for( int i = 0; i < nCount; i++ )
		{
			ar >> nIndex;
			ar >> m_aSlotApplet[nIndex].m_dwShortcut >>	m_aSlotApplet[nIndex].m_dwId >> m_aSlotApplet[nIndex].m_dwType;
			ar >> m_aSlotApplet[nIndex].m_dwIndex >> m_aSlotApplet[nIndex].m_dwUserId >> m_aSlotApplet[nIndex].m_dwData;
			if( m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_CHAT)
				ar.ReadString( m_aSlotApplet[nIndex].m_szString, MAX_SHORTCUT_STRING );
//			ar >>  m_aSlotApplet[nIndex]에 아까 넣은 필요한 정보를 넣는다.
		}
		ar >> nCount;	// slot item count
		int nIndex2;
		for( i = 0; i < nCount; i++ )
		{
			ar >> nIndex >> nIndex2;	// index
			ar >> m_aSlotItem[nIndex][nIndex2].m_dwShortcut >>	m_aSlotItem[nIndex][nIndex2].m_dwId >> m_aSlotItem[nIndex][nIndex2].m_dwType;
			ar >> m_aSlotItem[nIndex][nIndex2].m_dwIndex >> m_aSlotItem[nIndex][nIndex2].m_dwUserId >> m_aSlotItem[nIndex][nIndex2].m_dwData;
			if( m_aSlotItem[nIndex][nIndex2].m_dwShortcut == SHORTCUT_CHAT)
				ar.ReadString( m_aSlotItem[nIndex][nIndex2].m_szString, MAX_SHORTCUT_STRING );
		}

		ar >> nCount;
		for( i = 0; i < nCount; i++ )
		{
			ar >> nIndex;
			ar >> m_aSlotQueue[nIndex].m_dwShortcut >> m_aSlotQueue[nIndex].m_dwId >> m_aSlotQueue[nIndex].m_dwType;
			ar >> m_aSlotQueue[nIndex].m_dwIndex >> m_aSlotQueue[nIndex].m_dwUserId >> m_aSlotQueue[nIndex].m_dwData;
//			ar >> m_aSlotQueue[nIndex]에 정보를 넣는다.
		}
		ar >> m_nActionPoint;
	}
}

void CUserTaskBar::SetShortcut( int nIndex, DWORD dwShortcut, DWORD dwType, DWORD dwIndex, DWORD dwId, DWORD dwData, int nWhere )
{
	LPSHORTCUT pShortcut;
	if( nWhere == 0 )
		pShortcut = &m_aSlotApplet[ nIndex ];
	else if( nWhere == 1 )
		pShortcut = &m_aSlotItem[0][ nIndex ];
	else
	{
		Error( "CUserTaskBar::SetShortcut nWhere:%d", nWhere ); 
		return;
	}

//	LPSHORTCUT pShortcut	= &m_aSlotApplet[ nIndex ];
	pShortcut->m_dwShortcut		= dwShortcut   ;
	pShortcut->m_dwType     = dwType;
	pShortcut->m_dwIndex    = dwIndex;
	pShortcut->m_dwId       = dwId;
	pShortcut->m_dwUserId   = 0 ;
	pShortcut->m_dwData     = nWhere;
}

#ifdef __WORLDSERVER
// 스킬바 사용이 완전히 끝났을때
void CUserTaskBar::OnEndSkillQueue( CUser *pUser )
{
	m_nUsedSkillQueue = -1;
	pUser->AddHdr( GETID( pUser ), SNAPSHOTTYPE_ENDSKILLQUEUE );
	((CMover*)pUser)->ClearCmd();
	TRACE( "\nOnEndSkillQueue\n" );
}

int	CUserTaskBar::SetNextSkill( CUser *pUser )
{
	m_nUsedSkillQueue ++;		// 큐인덱스 다음으로...

	int nAP = m_nActionPoint;

	if( !( pUser->IsSMMode( SM_ACTPOINT ) ) )
	{
		switch( m_nUsedSkillQueue )
		{
		case 1:	nAP -= 6;	break;		// 액션 포인트 소모량. 다음슬롯을 사용할수있는지 미리 알아보기 위함.
		case 2:	nAP -= 8;	break;
		case 3:	nAP -= 11;	break;
		case 4:	nAP -= 30;	break;
		}
	}

	LPSHORTCUT pShortcut = &m_aSlotQueue[ m_nUsedSkillQueue ];
	BOOL bResult = nAP < 0;
	if( ( m_nUsedSkillQueue >= MAX_SLOT_QUEUE) || pShortcut->IsEmpty() || (nAP < 0) )	// 큐진행이 끝까지 갔거나 || 큐가 비었거나 || AP가 없거나.
	{
		OnEndSkillQueue( pUser );
		return 0;		// 큐실행 끝.
	} else
	{
		if( nAP < 0 )	nAP = 0;
		m_nActionPoint = nAP;
		pUser->AddSetActionPoint( nAP );		// 액숀포인트 클라에 갱신.
		OBJID idTarget = pUser->m_idSetTarget;
		TRACE( "다음스킬사용 시도%d, ", pShortcut->m_dwId );
		if( pUser->CMD_SetUseSkill( idTarget, pShortcut->m_dwId, SUT_QUEUEING ) == 0 )		// 실행할 명령을 셋팅. 이동 + 스킬사용이 합쳐진 명령.
		{
			TRACE( "다음스킬사용 실패 %d, ", pShortcut->m_dwId );
			SetNextSkill( pUser );	// 스킬사용에 실패했다면 다음 스킬 사용하도록 넘어감.
		}
		return 1;
	}

	return 1;
}
#endif // __WORLDSERVER