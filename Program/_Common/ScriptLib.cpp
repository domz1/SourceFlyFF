#include "stdafx.h"
#include "defineText.h"
#include "defineObj.h"
#include "party.h"
#include "User.h"
#include "guildquest.h"
#include "defineQuest.h"
#include "worldmng.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "dpSrvr.h"
#include "guild.h"
#include "ScriptHelper.h"
#if defined(__REMOVE_SCIRPT_060712)
	#include "WorldDialog.h"
#endif

extern	CPartyMng			g_PartyMng;
extern	CWorldMng			g_WorldMng;
extern	CDPCoreClient		g_DPCoreClient;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPSrvr				g_DPSrvr;
extern	CGuildMng			g_GuildMng;
extern	CUserMng			g_UserMng;


#if defined(__REMOVE_SCIRPT_060712)

Functions		g_functions;

// npc이름과 키를 이용해서 함수가 있는지 찾는다.
BOOL HasKey( NPCDIALOG_INFO* pInfo, LPCTSTR szKey )
{	
	return CWorldDialog::GetInstance().Find( pInfo->GetName(), szKey );
}

int APIENTRY SayName( NPCDIALOG_INFO* pInfo )
{
	return 1;
}

int APIENTRY Speak( NPCDIALOG_INFO* pInfo, int nId, LPCTSTR lpszStr )
{
	CMover* pMover = prj.GetMover(nId);
	if(pMover && pMover->GetType() == OT_MOVER)
	{
		g_UserMng.AddChat( (CCtrl*)pMover, lpszStr );
	}
	return 1;
}

int APIENTRY SpeakName( NPCDIALOG_INFO* pInfo )
{
	return 1;
}

int APIENTRY GetVal( NPCDIALOG_INFO* pInfo )
{
	return pInfo->GetReturn();
}

int APIENTRY GetSrcId( NPCDIALOG_INFO* pInfo )
{
	return pInfo->GetPcId();
}

int APIENTRY GetDstId( NPCDIALOG_INFO* pInfo )
{
	return pInfo->GetNpcId();
}

int APIENTRY NpcId( NPCDIALOG_INFO* pInfo )
{
	return GetDstId( pInfo );
}

int APIENTRY PcId( NPCDIALOG_INFO* pInfo )
{
	return GetSrcId( pInfo );
}

int APIENTRY GetGValue_( NPCDIALOG_INFO* pInfo )
{
	return 1;
}
int APIENTRY SetGValue( NPCDIALOG_INFO* pInfo )
{
	return 1;
}
int APIENTRY AddGValue( NPCDIALOG_INFO* pInfo )
{
	return 1;
}

int APIENTRY AddKey( NPCDIALOG_INFO* pInfo, LPCTSTR szWord, LPCTSTR szKey, DWORD dwParam )
{
	dwParam = 0;
	CString strWord, strKey;

	strWord = szWord;
	strKey = szKey;

	if( strKey.IsEmpty() )	
		strKey = strWord;

	if( HasKey( pInfo, strKey ) == FALSE ) 
	{
		Error( "key error AddKey %s %s", pInfo->GetName(), strKey );
		return 1;
	}
	__AddKey( pInfo->GetPcId(), strWord, strKey, dwParam );
	return 1;
}

int APIENTRY AddCondKey( NPCDIALOG_INFO* pInfo, LPCTSTR szWord, LPCTSTR szKey, DWORD dwParam )
{
	dwParam = 0;
	CString strWord, strKey;

	strWord = szWord;
	strKey = szKey;

	if( strKey.IsEmpty() )	
		strKey = strWord;

	if( HasKey( pInfo, strKey ) == FALSE ) 
	{
		Error( "key error %s %s", pInfo->GetName(), strKey );
		return 1;
	}
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	CMover* pMover = prj.GetMover( pInfo->GetNpcId() );

	for( int i = 0; i < pUser->m_nQuestSize; i++ )
	{
		QuestProp* pQuestProp = pUser->m_aQuest[ i ].GetProp();
		if( pQuestProp )
		{
			if( strcmp( pQuestProp->m_szEndCondDlgCharKey, pMover->m_szCharacterKey ) == 0 )
			{
				if( strcmp( pQuestProp->m_szEndCondDlgAddKey, strKey ) == 0 )
				{
					__AddKey( pInfo->GetPcId(), strWord, strKey, dwParam );
					return 1;
				}
			}
		}
	}
	return 0;
}

int APIENTRY AddAnswer( NPCDIALOG_INFO*  pInfo, LPCTSTR szWord, LPCTSTR szKey, DWORD dwParam )
{
	DWORD nParam1, nParam2;
	CString strWord, strKey;

	strWord = szWord;
	strKey = szKey;

	nParam1 = dwParam;
	nParam2 = 0;

	if( strKey.IsEmpty() )	
		strKey = strWord;

	if( HasKey( pInfo, strKey ) == FALSE ) 
	{
		Error( "key error AddAnswer %s %s", pInfo->GetName(), strKey );
		return 1;
	}
	return __AddAnswer( pInfo->GetPcId(), strWord, strKey, nParam1, nParam2 );
}

int APIENTRY RemoveQuest( NPCDIALOG_INFO* pInfo, int nQuest )      
{
	return __RemoveQuest( pInfo->GetPcId(), nQuest );
}

int APIENTRY RemoveAllKey(NPCDIALOG_INFO*  pInfo)
{
	__RemoveAllKey( pInfo->GetPcId() );
	return 1;
}

int APIENTRY SayQuest( NPCDIALOG_INFO* pInfo, int nQuest, int nDialog )
{
	__SayQuest( pInfo->GetPcId(), nQuest, nDialog );
	return 1;
}

int APIENTRY BeginQuest( NPCDIALOG_INFO* pInfo, int nQuest )
{
	return __RunQuest( pInfo->GetPcId(), pInfo->GetNpcId(), nQuest );
}

int APIENTRY EndQuest( NPCDIALOG_INFO* pInfo, int nQuest )
{
	return __EndQuest( pInfo->GetPcId(), nQuest, FALSE );
}

int APIENTRY AddQuestKey( NPCDIALOG_INFO* pInfo, int nQuest, LPCTSTR szKey )
{
	int nParam = 0;
	CString strKey = szKey;

	if( strKey.IsEmpty() )
	{
		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuest );
		if( pQuestProp )
			strKey = pQuestProp->m_szTitle;
	}

	if( HasKey( pInfo, strKey ) == FALSE ) 
	{
		Error( "key error AddQuestKey %s %s", pInfo->GetName(), strKey );
		return 1;
	}

	return __AddQuestKey( pInfo->GetPcId(), nQuest, strKey, nParam );
}


int APIENTRY CreateMover( NPCDIALOG_INFO* pInfo )
{
	return 1;
}     

int APIENTRY CreateChar( NPCDIALOG_INFO*  pInfo )
{
	return 1;
}    

int APIENTRY RemoveMover( NPCDIALOG_INFO*  pInfo )
{
	return 1;
}   

int APIENTRY GetDay( NPCDIALOG_INFO* pInfo )
{
	return g_GameTimer.m_nDay;
}        

int APIENTRY GetMin( NPCDIALOG_INFO* pInfo )
{
	return g_GameTimer.m_nMin;
}      

int APIENTRY GetHour( NPCDIALOG_INFO* pInfo )
{
	return g_GameTimer.m_nHour;
}       
 
int APIENTRY SetTimer( NPCDIALOG_INFO*  pInfo )  
{
	return 1;
}
int APIENTRY IsTimeOut( NPCDIALOG_INFO*  pInfo )
{
	return 1;
}

int APIENTRY LaunchQuest( NPCDIALOG_INFO* pInfo )
{
	int* p = pInfo->GetGlobal();
	*p = 0;
	__QuestEnd( pInfo->GetPcId(), pInfo->GetNpcId(), *p );
	return 1;
}
int APIENTRY GetEmptyInventoryNum( NPCDIALOG_INFO* pInfo )
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	return pUser->m_Inventory.GetEmptyCount();
}

int APIENTRY GetQuestState( NPCDIALOG_INFO* pInfo, int nQuest )
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	LPQUEST pQuest = pUser->GetQuest( nQuest );
	if( pQuest ) 
		return pQuest->m_nState;
	QUEST quest;
	if( pUser->MakeCompleteQuest( nQuest, &quest ) )
		return quest.m_nState;
	return -1;
}
int APIENTRY IsSetQuest( NPCDIALOG_INFO* pInfo, int nQuest )
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	LPQUEST pQuest = pUser->GetQuest( nQuest );
	if( pQuest ) return TRUE;
	if( pUser->IsCompleteQuest( nQuest ) )
		return TRUE;
	return FALSE;
}

int APIENTRY SetMark( NPCDIALOG_INFO* pInfo )
{
	return 1;
}
int APIENTRY GoMark( NPCDIALOG_INFO* pInfo )
{
	return 1;
}

int APIENTRY RandomIndex( NPCDIALOG_INFO* pInfo) 
{
	return 1;
}

int APIENTRY RemoveKey(NPCDIALOG_INFO* pInfo, LPCTSTR szKey )
{
	if( HasKey(pInfo, szKey) ) 
	{
		CUser* pUser = prj.GetUser( pInfo->GetPcId() );

		RunScriptFunc rsf;
		rsf.wFuncType = FUNCTYPE_REMOVEKEY;
		lstrcpy( rsf.lpszVal1, szKey );
		pUser->AddRunScriptFunc( rsf );
	}
	return 1;
}

int APIENTRY Say(NPCDIALOG_INFO* pInfo, LPCTSTR szMsg )
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	RunScriptFunc rsf;
	rsf.wFuncType = FUNCTYPE_SAY;
	lstrcpy( rsf.lpszVal1, szMsg );
	rsf.dwVal2 = 0;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}

int APIENTRY EndSay( NPCDIALOG_INFO*  pInfo )
{
	return 1;
}

int APIENTRY Random( NPCDIALOG_INFO*  pInfo, int n )
{
	return rand() % n;
}

int APIENTRY Exit( NPCDIALOG_INFO* pInfo )
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	RunScriptFunc rsf;
	rsf.wFuncType = FUNCTYPE_EXIT;
	pUser->AddRunScriptFunc( rsf );
	return TRUE;
}

int APIENTRY  SetScriptTimer( NPCDIALOG_INFO* pInfo, int nTimer )
{
	return 1;
}

// Replace( int nWorld, int x, int y, int z )
int APIENTRY Replace( NPCDIALOG_INFO*  pInfo, int nWorld, float x, float y, float z )
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );

	if( x > 0.0f && z > 0.0f )
	{
		RunScriptFunc rsf;
		rsf.wFuncType = FUNCTYPE_EXIT;
		pUser->AddRunScriptFunc( rsf );
		if( g_eLocal.GetState( EVE_SCHOOL ) )
			g_dpDBClient.SendLogSchool( pUser->m_idPlayer, pUser->GetName() );

		pUser->REPLACE( g_uIdofMulti, nWorld, D3DXVECTOR3( x, y, z ), REPLACE_NORMAL, nTempLayer );
	}
	return 1;
}

int APIENTRY ReplaceKey( NPCDIALOG_INFO* pInfo, int nWorld, LPCTSTR szKey )
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	RunScriptFunc rsf;	
	rsf.wFuncType = FUNCTYPE_EXIT;
	pUser->AddRunScriptFunc( rsf );

	PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( nWorld, szKey );
	if( pRgnElem )
		pUser->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
	return 1;
}

int APIENTRY GetPlayerLvl( NPCDIALOG_INFO* pInfo )  
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	return pUser->GetLevel();
}

int APIENTRY GetPlayerJob( NPCDIALOG_INFO* pInfo )  
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	return pUser->GetJob();
}

int APIENTRY IsGuildQuest( NPCDIALOG_INFO* pInfo, int nQuest )
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );

	CGuild* pGuild	= pUser->GetGuild();
	if( pGuild )
	{
		PGUILDQUEST pQuest = pGuild->GetQuest( nQuest );
		if( pQuest )
			return TRUE;
	}
	return FALSE;
}

int APIENTRY GetGuildQuestState( NPCDIALOG_INFO* pInfo, int nQuest )
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	CGuild* pGuild	= pUser->GetGuild();
	if( pGuild )
	{
		PGUILDQUEST pQuest	= pGuild->GetQuest( nQuest );
		if( pQuest )
		{
			int nState = pQuest->nState;
			return nState;
		}
	}
	return -1;
}

int APIENTRY IsWormonServer( NPCDIALOG_INFO* pInfo )
{
	return g_eLocal.GetState( EVE_WORMON );
}

int APIENTRY PrintSystemMessage( NPCDIALOG_INFO* pInfo, int nText )
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	pUser->AddDefinedText( (DWORD)nText );
	return 1;
}

int APIENTRY  MonHuntStart( NPCDIALOG_INFO* pInfo, int nQuest, int nState, int nState2, int n )
{
	CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
	if( pProcessor->IsQuesting( nQuest ) )
	{
		return 0;
	}
	else
	{
		CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
		CGuild* pGuild	= pUser->GetGuild();
		if( pGuild )
		{
			GUILDQUESTPROP* pProp	= prj.GetGuildQuestProp( nQuest );
			if( pProp )
			{
				CMover* pWormon	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, pProp->dwWormon );
				if( pWormon )
				{
					pWormon->SetPos( pProp->vPos );
					pWormon->InitMotion( MTI_STAND );
					pWormon->UpdateLocalMatrix();
					pWormon->m_nQuestKeeping	= nQuest;

					CWorld* pWorld = pUser->GetWorld();
					if( pWorld )
					{
						RunScriptFunc rsf;	// Exit()
						rsf.wFuncType = FUNCTYPE_EXIT;
						pUser->AddRunScriptFunc( rsf );
						pWormon->AddItToGlobalId();
						pWorld->ADDOBJ( pWormon, FALSE, pUser->GetLayer() );
						pGuild->SetQuest( nQuest, nState );
						if( nState == QS_BEGIN )
							g_dpDBClient.SendInsertGuildQuest( pGuild->m_idGuild, nQuest );
						else
							g_dpDBClient.SendUpdateGuildQuest( pGuild->m_idGuild, nQuest, nState );

						pProcessor->SetGuildQuest( nQuest, nState, nState2, n, pGuild->m_idGuild, pWormon->GetId() );
						D3DXVECTOR3 vPos( pProp->vPos.x, pProp->vPos.y, ( ( pProp->vPos.z * 2 ) + pProp->y2 ) / 3 );
						pGuild->Replace( pProp->dwWorldId, vPos, TRUE );
						return 1;
					}
					safe_delete( pWormon );
				}
			}
		}
	}
	return 0;
}

int APIENTRY  MonHuntStartParty( NPCDIALOG_INFO*  pInfo, int nQuest, int nState, int nState2, int n )
{
	CPartyQuestProcessor* pProcessor	= CPartyQuestProcessor::GetInstance();
	if( pProcessor->IsQuesting( nQuest ) )
	{
		return 0;
	}
	else
	{
		CUser* pUser	= prj.GetUser( pInfo->GetPcId() );

		CParty* pParty	= g_PartyMng.GetParty( pUser->GetPartyId() );
		if( pParty )
		{
			PARTYQUESTPROP* pProp	= prj.GetPartyQuestProp( nQuest );
			if( pProp )
			{				
				if( pParty->ReplaceChkLv( pProp->nLevel+5 ) == FALSE )
				{
					CString str;
					str.Format( prj.GetText(TID_GAME_PARTYQUEST_NOT_JOIN), pProp->nLevel+5 );
					pUser->AddDiagText( str );

					return 0;
				}

				// 기존에 들어있던...파티가 깨진 플레이어들... 마을로 소환시킨다...
//				g_UserMng.ReplaceWorldArea( pProp->dwWorldId, WI_WORLD_MADRIGAL, 6968.0f, 3328.8f, 0.5f, FALSE );				

				BOOL bSuccess = FALSE;

				for( DWORD i=0; i<pProp->vecWormon.size(); i++ )
				{
					WORMON* WorMon;
					WorMon = &(pProp->vecWormon[i]);
					CMover* pWormon	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, WorMon->dwWormon );

					if( pWormon )
					{
						pWormon->SetPos( WorMon->vPos );
						pWormon->InitMotion( MTI_STAND );
						pWormon->UpdateLocalMatrix();
						pWormon->m_nPartyQuestKeeping = nQuest;

						CWorld* pWorld	= g_WorldMng.GetWorld( pProp->dwWorldId );
						if( pWorld )
						{
							RunScriptFunc rsf;	// Exit()
							rsf.wFuncType		= FUNCTYPE_EXIT;
							pUser->AddRunScriptFunc( rsf );
							pWormon->AddItToGlobalId();
							pWormon->m_bActiveAttack = TRUE;
							pWorld->ADDOBJ( pWormon, FALSE, pUser->GetLayer() );

							bSuccess = TRUE;								
						}
						else
						{
							safe_delete( pWormon );
						}
					}
				}
				
				if( bSuccess )
				{
					pProcessor->SetPartyQuest( nQuest, nState, nState2, n, pUser->GetPartyId(), 0 );
					pParty->Replace( pProp->dwWorldId, pProp->szWorldKey );
									
					return 1;
				}
			}
		}
	}
	return 0;
}

int APIENTRY InitStat( NPCDIALOG_INFO*  pInfo )
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	pUser->SetStr( 15 );
	pUser->SetInt( 15 );
	pUser->SetDex( 15 );
	pUser->SetSta( 15 );
	pUser->m_nRemainGP	= ( pUser->GetLevel() - 1 ) * 2;
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITSTAT;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}

int	APIENTRY InitStr( NPCDIALOG_INFO*  pInfo )
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	int nStr = pUser->m_nStr;
	if( nStr - 15 > 0 )
	{
		pUser->SetStr( 15 );
		pUser->m_nRemainGP	+= nStr - 15;
	}
	
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITSTR;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}
int	APIENTRY InitSta( NPCDIALOG_INFO*  pInfo )
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	int nSta = pUser->m_nSta;
	if( nSta - 15 > 0 )
	{
		pUser->SetSta( 15 );
		pUser->m_nRemainGP	+= nSta - 15;
	}
	
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITSTA;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}
int	APIENTRY InitDex( NPCDIALOG_INFO*  pInfo )
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	int nDex = pUser->m_nDex;
	if( nDex - 15 > 0 )
	{
		pUser->SetDex( 15 );
		pUser->m_nRemainGP	+= nDex - 15;
	}
	
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITDEX;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}
int	APIENTRY InitInt( NPCDIALOG_INFO*  pInfo )
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	int nInt = pUser->m_nInt;
	if( nInt - 15 > 0 )
	{
		pUser->SetInt( 15 );
		pUser->m_nRemainGP	+= nInt - 15;
	}
	
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITINT;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}

int APIENTRY SetQuestState( NPCDIALOG_INFO* pInfo, int nQuest, int n )      
{
	return __SetQuestState( pInfo->GetPcId(), nQuest, n );
}
int APIENTRY SetQuest( NPCDIALOG_INFO* pInfo, int nQuest )      
{
	__SetQuest(  pInfo->GetPcId(), nQuest );
	return 1;
}

int APIENTRY CreateItem( NPCDIALOG_INFO* pInfo, DWORD dwID, int nCount )	
{
	__CreateItem( pInfo->GetPcId(), dwID, nCount );
	return 1;
}

int APIENTRY AddGold( NPCDIALOG_INFO* pInfo, int nGold )     
{
	__AddGold(  pInfo->GetPcId(), nGold );
	return 1;
}

int	APIENTRY RemoveGold( NPCDIALOG_INFO* pInfo, int nGold )
{
	__RemoveGold( pInfo->GetPcId(), nGold );
	return 1;
}

int APIENTRY GetPlayerGold( NPCDIALOG_INFO* pInfo )
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	return pUser->GetGold();
}

int APIENTRY GetLocalEventState( NPCDIALOG_INFO* pInfo, int nEvent )
{
	return g_eLocal.GetState( nEvent );
}

int APIENTRY AddExp( NPCDIALOG_INFO* pInfo, int nExp )      
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	if( pUser->AddExperience( nExp, TRUE, FALSE ) )
		pUser->LevelUpSetting();
	else
		pUser->ExpUpSetting();
	pUser->AddSetExperience( pUser->GetExp1(), (WORD)pUser->m_nLevel, pUser->m_nSkillPoint, pUser->m_nSkillLevel );
	pUser->AddDefinedText( TID_GAME_REAPEXP );
	return 1;
}

int APIENTRY IsParty( NPCDIALOG_INFO* pInfo )
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
		CParty* pParty = g_PartyMng.GetParty( pUser->m_idparty );
		int f = ( pParty != NULL && pParty->IsMember( pUser->m_idPlayer ) );
	return f;
}

int APIENTRY IsPartyMaster( NPCDIALOG_INFO* pInfo )
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
		CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
		int f = ( pParty != NULL && pParty->IsLeader( pUser->m_idPlayer ) );
	return f;
}

int APIENTRY GetPartyNum( NPCDIALOG_INFO* pInfo )
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	int nSize = 0;
	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
		nSize = pParty->GetSizeofMember();
	return nSize;
}

int	APIENTRY GetPartyLevel( NPCDIALOG_INFO* pInfo )
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	int nLevel = 0;
	CParty* pParty = g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
		nLevel = pParty->GetLevel();
	return nLevel;
}

int APIENTRY IsPartyGuild( NPCDIALOG_INFO* pInfo )
{
	int f	= 0;
	u_long	aidPlayer[MAX_PTMEMBER_SIZE];
	int	nSize	= 0;
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );

	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
	{
		nSize	= pParty->m_nSizeofMember;
		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
			aidPlayer[i]	= pParty->m_aMember[i].m_uPlayerId;
	}
	else
	{
		f	= 1;
	}

	for( int i = 0; i < nSize; i++ )
	{
		CUser* pUsertmp	= g_UserMng.GetUserByPlayerID( aidPlayer[i] );
		if( IsValidObj( pUsertmp ) )
		{
			if( pUsertmp->GetGuild() )
			{
				f	= 1;
				break;
			}
			CTime tCurrent = CTime::GetCurrentTime();
			if( tCurrent < pUsertmp->m_tGuildMember )
			{
				f	= 2;
				break;
			}
		}
		else
		{
			f	= 1;
			break;
		}
	}
	return f;
}

int APIENTRY IsGuild( NPCDIALOG_INFO* pInfo )
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	int f	= ( pGuild && pGuild->IsMember( pUser->m_idPlayer ) );
	return f;
}

int APIENTRY IsGuildMaster( NPCDIALOG_INFO* pInfo )
{
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	int f = ( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) );
	return f;
}

int APIENTRY CreateGuild( NPCDIALOG_INFO*  pInfo )
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	int nSize = 0;
	GUILD_MEMBER_INFO	info[MAX_PTMEMBER_SIZE];

	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty && pParty->IsLeader( pUser->m_idPlayer ) )
	{
		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			CUser* pUsertmp	= g_UserMng.GetUserByPlayerID( pParty->GetPlayerId( i ) );
			if( IsValidObj( pUsertmp ) )
			{
				info[nSize].idPlayer	= pUsertmp->m_idPlayer;
#if __VER < 11 // __SYS_PLAYER_DATA
				info[nSize].nLevel	= pUsertmp->GetLevel();
				info[nSize].nJob	= pUsertmp->GetJob();
				info[nSize].dwSex	= pUsertmp->GetSex();
#endif	// __SYS_PLAYER_DATA
				nSize++;
			}
			else
			{
				// error	- offline
				return 0;
			}
		}
	}
	else
	{
		// error	-
		return 0;
	}

	g_DPCoreClient.SendCreateGuild( info, nSize, "" );

	return 1;
}

int APIENTRY DestroyGuild( NPCDIALOG_INFO* pInfo )
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );

	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild && pGuild->IsMember( pUser->m_idPlayer ) )
	{
		if( pGuild->IsMaster( pUser->m_idPlayer ) )
		{
			RunScriptFunc rsf;
			rsf.wFuncType	= FUNCTYPE_DESTROYGUILD;
			pUser->AddRunScriptFunc( rsf );
		}
		else
			pUser->AddDefinedText( TID_GAME_COMDELNOTKINGPIN );
	}
	else
		pUser->AddDefinedText( TID_GAME_COMNOHAVECOM );

	return 1;
}

int APIENTRY IsPlayerSkillPoint( NPCDIALOG_INFO*  pInfo )     
{
	return 0;
}

int APIENTRY ChangeJob( NPCDIALOG_INFO* pInfo, int nJob )     
{
	int v1	=nJob;
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	if( pUser->IsBaseJob() && pUser->GetLevel() != MAX_JOB_LEVEL )
	{
		Error( "Error ChangeJob : 1" );
		return 0;
	}
	if( pUser->IsExpert() && pUser->GetLevel() < MAX_JOB_LEVEL + MAX_EXP_LEVEL )
	{
		Error( "Error ChangeJob : 2" );
		return 0;
	}
	if( pUser->IsPro() && pUser->GetLevel() < MAX_GENERAL_LEVEL && pUser->GetExpPercent() < 9999 )
	{
		Error( "Error ChangeJob : 3" );
		return 0;
	}
	if( pUser->AddChangeJob( v1 ) )
	{
		pUser->AddSetChangeJob( v1 );
		g_UserMng.AddNearSetChangeJob( (CMover*)pUser, v1, &pUser->m_aJobSkill[MAX_JOB_SKILL] );
		// #dlvr
		g_dpDBClient.SendLogLevelUp( (CUser*)pUser, 4 );
#ifdef __S_RECOMMEND_EVE
		if( g_eLocal.GetState( EVE_RECOMMEND ) && pUser->IsPlayer() )
			g_dpDBClient.SendRecommend( (CUser*)pUser, v1 );
#endif // __S_RECOMMEND_EVE
#if __VER >= 11 // __SYS_PLAYER_DATA
		g_dpDBClient.SendUpdatePlayerData( pUser );
#else	// __SYS_PLAYER_DATA
		g_DPCoreClient.SendPartyMemberJob( (CUser*)pUser );
		g_DPCoreClient.SendFriendChangeJob( (CUser*)pUser );
		if( pUser->m_idGuild != 0 )
			g_DPCoreClient.SendGuildChangeJobLevel( (CUser*)pUser );
#endif	// __SYS_PLAYER_DATA
	}
	else
	{
		Error( "Error ChangeJob : CMover::AddChangeJob" );
	}
	return 1;
}

int APIENTRY GetPlayerSex( NPCDIALOG_INFO* pInfo )  
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	return pUser->GetSex();
}
int APIENTRY GetItemNum( NPCDIALOG_INFO* pInfo, DWORD dwID )    
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	return pUser->GetItemNum( dwID );
}
int APIENTRY RemoveAllItem( NPCDIALOG_INFO* pInfo, DWORD dwID ) 
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	pUser->RemoveAllItem( dwID );
	return 1;
}
int APIENTRY EquipItem( NPCDIALOG_INFO* pInfo, DWORD dwID )     
{
	int v1	= dwID;
	CUser* pUser	= prj.GetUser( pInfo->GetPcId() );
	BYTE nId, nCount	= 1;
	
	CItemElem itemElem;
	itemElem.m_dwItemId		= v1;
	itemElem.m_nItemNum		= 1;
	itemElem.SetAbilityOption( 0 );
	if( pUser->CreateItem( &itemElem, &nId ) == TRUE )
	{
		CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( nId );
		if( pItemElem && TRUE == pUser->DoEquip( pItemElem, TRUE ) )
		{
			g_UserMng.AddDoEquip( (CMover*)pUser, -1, pItemElem, TRUE );
		}

		LogItemInfo aLogItem;
		aLogItem.Action = "Q";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "QUEST";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		g_DPSrvr.OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );
	}
	else
	{
		CItemElem* pItemElem	= new CItemElem;
		pItemElem->m_dwItemId	= v1;
		ItemProp* pItemProp		= pItemElem->GetProp();
		if( pItemProp )
		{
			pItemElem->m_nHitPoint	= pItemProp->dwEndurance;
			pItemElem->m_nItemNum	= 1;
			pItemElem->SetAbilityOption( 0 );
			CItem* pItem	= new CItem;
			pItem->m_pItemBase	= pItemElem;

			if( pItemElem->m_dwItemId == 0 ) Error("EquipItem SetIndex\n" );
			pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
			pItem->SetPos( pUser->GetPos() );
			pItem->SetAngle( (float)( xRandom( 360 ) ) );
			pItem->m_idHolder	= pUser->m_idPlayer;
			pUser->GetWorld()->ADDOBJ( pItem, TRUE, pUser->GetLayer() );
			pUser->AddDefinedText( TID_GAME_DROPITEM, "\"%s\"", prj.GetItemProp( v1 )->szName );
		}
	}
	return 1;
}

int APIENTRY PlaySound(NPCDIALOG_INFO* pInfo, LPCTSTR szMusic )
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	pUser->AddPlaySound2( szMusic );
	return 1;
}

int APIENTRY DropQuestItem( NPCDIALOG_INFO* pInfo, DWORD dwID, DWORD dwProb )
{
	int v1 = dwID, v2 = dwProb;
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );

	if( (int)( xRandom( 1000 ) ) < v2 )
	{
		ItemProp* pItemProp	= prj.GetItemProp( v1 );
		if( pItemProp )
		{
			CWorld* pWorld	= pUser->GetWorld();
			if( pWorld )
			{
				CItemElem* pItemElem	= new CItemElem;
				pItemElem->m_dwItemId	= v1;
				CItem* pItem	= new CItem;
				pItem->m_pItemBase	= pItemElem;
				if( pItemElem->m_dwItemId == 0 ) Error( "DropQuestItem SetIndex\n" );
				pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
				pItem->m_idHolder	= pUser->m_idPlayer;
				pItem->m_idOwn	= pUser->GetId();
				pItem->m_dwDropTime	= timeGetTime();
				CMover* pMover	= prj.GetMover( pInfo->GetNpcId() );
				D3DXVECTOR3 vPos	= pMover->GetPos();
				vPos.x	+= ( xRandomF( 2.0f ) - 1.0f );
				vPos.z	+= ( xRandomF( 2.0f ) - 1.0f );
				pItem->SetPos( vPos );
				pItem->SetAngle( (float)( xRandom( 360 ) ) );
				pWorld->ADDOBJ( pItem, TRUE, pUser->GetLayer() );
				return 1;
			}
		}
		else
		{
			Error( "DropQuestItem, %d null prop", v1 );
		}
	}
	return 0;
}

int APIENTRY ExpQuestEffect(NPCDIALOG_INFO* pInfo, DWORD dwID, BOOL bNPC )
{
	int v1 = dwID, v2 = bNPC;
	CCtrl* pCtrl = ( v2? prj.GetCtrl( pInfo->GetNpcId() ): prj.GetCtrl( pInfo->GetPcId() ) );
	g_UserMng.AddCreateSfxObj( pCtrl, v1 );
	return 1;
}

int APIENTRY RemoveItem( NPCDIALOG_INFO* pInfo, DWORD dwID, int nCount )
{
	int v1 = dwID, v2 = nCount;
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	return pUser->RemoveItemA( v1, v2 );
}

int APIENTRY Run( NPCDIALOG_INFO* pInfo, LPCTSTR szKey, int n )
{
	int v1 = n;
	CString string = szKey;

	CMover* pMover = prj.GetMover( pInfo->GetNpcId() );
	int nResult = 0;
	pMover->m_pNpcProperty->RunDialog( string, &nResult, v1, pInfo->GetNpcId(), pInfo->GetPcId(), 0 );

	return nResult;
}

int APIENTRY GetQuestId( NPCDIALOG_INFO* pInfo )
{
	return pInfo->GetQuest();
}
int APIENTRY GetLang( NPCDIALOG_INFO* pInfo )
{
	return ::GetLanguage();
}

// 캐릭터 명 변경
int	APIENTRY QuerySetPlayerName( NPCDIALOG_INFO* pInfo )
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId()  );
	if( pUser->IsAuthHigher( AUTH_GAMEMASTER ) || pUser->IsMode( QUERYSETPLAYERNAME_MODE ) )
	{
		RunScriptFunc rsf;
		rsf.wFuncType = FUNCTYPE_QUERYSETPLAYERNAME;
		pUser->AddRunScriptFunc( rsf );
		return 1;
	}
	return 0;
}

int APIENTRY Trace(NPCDIALOG_INFO* pInfo, LPCTSTR szMsg )
{
	TRACE( "%s\n", szMsg );
	return 1;
}


int APIENTRY GetPlayerExpPercent( NPCDIALOG_INFO* pInfo )  
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	return pUser->GetExpPercent();
}

int APIENTRY SetLevel( NPCDIALOG_INFO* pInfo , int nSetLevel )  
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	int rtn_val = pUser->SetLevel(nSetLevel);
#if __VER >= 11 // __SYS_PLAYER_DATA
	g_dpDBClient.SendUpdatePlayerData( pUser );
#endif	// __SYS_PLAYER_DATA
	return rtn_val;
}

int APIENTRY AddGPPoint( NPCDIALOG_INFO* pInfo , int nAddGPPoint )  
{
	CUser* pUser = prj.GetUser( pInfo->GetPcId() );
	return pUser->AddGPPoint(nAddGPPoint);
}



Functions* GetDialogFunctions()
{
	return &g_functions;
}

void InitDialogFunctions()
{
	InitPredefineKey();		// 미리 정의된 키들을 위한 table을 만든다.

	g_functions.SayName = SayName;
	g_functions.Speak = Speak;
	g_functions.SpeakName = SpeakName;
	g_functions.GetVal = GetVal;
	g_functions.GetSrcId = GetSrcId;
	g_functions.GetDstId = GetDstId;
	g_functions.NpcId = NpcId;
	g_functions.PcId = PcId;
	g_functions.GetGValue_ = GetGValue_;
	g_functions.SetGValue = SetGValue;
	g_functions.AddGValue = AddGValue;
	g_functions.AddKey = AddKey;
	g_functions.AddCondKey = AddCondKey;
	g_functions.AddAnswer = AddAnswer;
	g_functions.RemoveQuest = RemoveQuest;
	g_functions.RemoveAllKey = RemoveAllKey;
	g_functions.SayQuest = SayQuest;
	g_functions.BeginQuest = BeginQuest;
	g_functions.EndQuest = EndQuest;
	g_functions.AddQuestKey = AddQuestKey;
	g_functions.CreateMover = CreateMover;
	g_functions.CreateChar = CreateChar;
	g_functions.RemoveMover = RemoveMover;
	g_functions.GetDay = GetDay;
	g_functions.GetMin = GetMin;
	g_functions.GetHour = GetHour;
	g_functions.SetTimer = SetTimer;
	g_functions.IsTimeOut = IsTimeOut;
	g_functions.LaunchQuest = LaunchQuest;
	g_functions.GetEmptyInventoryNum = GetEmptyInventoryNum;
	g_functions.GetQuestState = GetQuestState;
	g_functions.IsSetQuest = IsSetQuest;
	g_functions.SetMark = SetMark;
	g_functions.GoMark = GoMark;
	g_functions.RandomIndex = RandomIndex;
	g_functions.RemoveKey = RemoveKey;
	g_functions.Say = Say;
	g_functions.EndSay = EndSay;
	g_functions.Random = Random;
	g_functions.Exit = Exit;
	g_functions.SetScriptTimer = SetScriptTimer;
	g_functions.Replace = Replace;
	g_functions.ReplaceKey = ReplaceKey;
	g_functions.GetPlayerLvl = GetPlayerLvl;
	g_functions.GetPlayerJob = GetPlayerJob;
	g_functions.IsGuildQuest = IsGuildQuest;
	g_functions.GetGuildQuestState = GetGuildQuestState;
	g_functions.IsWormonServer = IsWormonServer;
	g_functions.PrintSystemMessage = PrintSystemMessage;
	g_functions.MonHuntStart = MonHuntStart;
	g_functions.MonHuntStartParty = MonHuntStartParty;
	g_functions.InitStat = InitStat;
	g_functions.InitStr = InitStr;
	g_functions.InitSta = InitSta;
	g_functions.InitDex = InitDex;
	g_functions.InitInt = InitInt;
	g_functions.SetQuestState = SetQuestState;
	g_functions.SetQuest = SetQuest;
	g_functions.CreateItem = CreateItem;
	g_functions.AddGold = AddGold;
	g_functions.RemoveGold = RemoveGold;
	g_functions.GetPlayerGold = GetPlayerGold;
	g_functions.GetLocalEventState = GetLocalEventState;
	g_functions.AddExp = AddExp;
	g_functions.IsParty = IsParty;
	g_functions.IsPartyMaster = IsPartyMaster;
	g_functions.GetPartyNum = GetPartyNum;
	g_functions.GetPartyLevel = GetPartyLevel;
	g_functions.IsPartyGuild = IsPartyGuild;
	g_functions.IsGuild = IsGuild;
	g_functions.IsGuildMaster = IsGuildMaster;
	g_functions.CreateGuild = CreateGuild;
	g_functions.DestroyGuild = DestroyGuild;
	g_functions.IsPlayerSkillPoint = IsPlayerSkillPoint;
	g_functions.ChangeJob = ChangeJob;
	g_functions.GetPlayerSex = GetPlayerSex;
	g_functions.GetItemNum = GetItemNum;
	g_functions.RemoveAllItem = RemoveAllItem;
	g_functions.EquipItem = EquipItem;
	g_functions.PlaySound = PlaySound;
	g_functions.DropQuestItem = DropQuestItem;
	g_functions.ExpQuestEffect = ExpQuestEffect;
	g_functions.RemoveItem = RemoveItem;
	g_functions.Run = Run;
	g_functions.GetQuestId = GetQuestId;
	g_functions.GetLang = GetLang;
	g_functions.QuerySetPlayerName = QuerySetPlayerName;
	g_functions.Trace = Trace;
	g_functions.GetPlayerExpPercent = GetPlayerExpPercent;
	g_functions.SetLevel = SetLevel;
	g_functions.AddGPPoint = AddGPPoint;
}


#else

#define GetToken      pScript->GetToken
#define PutBack       pScript->PutBack
#define Compute       pScript->Compute
#define token         pScript->token
#define tokenType     pScript->tokenType

LPSCRIPT GetScript( CScript* pScript, LPCTSTR szKey )
{
	CScriptDialog* pScriptDlg = (CScriptDialog*)pScript->GetFunc();
	return pScriptDlg->Lookup( szKey );
}

//
// Func(1);를 가정했을 때 (와 1과 )을 읽는다.
//
int EvalExp( CScript *pScript )
{
	int v;
	GetToken(); // (
	Compute(&v);
	GetToken(); // )
	return v;
}
int EvalDefault(CScript *pScript,int nDefault)
{
	GetToken(); // , or ) or (
	int v2;
	if(*token == ',' || *token == '(')
		Compute(&v2);
	else
	{
		v2 = nDefault;
		PutBack();
	}
	return v2;
}
//
// Func(1);를 가정했을 때 (와 1을 읽는다.
//
void Eval1(CScript *pScript,int &v1)
{
	GetToken(); Compute(&v1);
}
//
// Func(1,2);를 가정했을 때 (와 1,2을 읽는다.
//
void Eval2(CScript *pScript,int &v1,int &v2)
{
	GetToken(); Compute(&v1);
	GetToken(); Compute(&v2);
}
void Eval3(CScript *pScript,int &v1,int &v2,int &v3)
{
	GetToken(); Compute(&v1);
	GetToken(); Compute(&v2);
	GetToken(); Compute(&v3);
}
void Eval4(CScript *pScript,int &v1,int &v2,int &v3,int &v4)
{
	GetToken(); Compute(&v1);
	GetToken(); Compute(&v2);
	GetToken(); Compute(&v3);
	GetToken(); Compute(&v4);
}
void Eval5(CScript *pScript,int &v1,int &v2,int &v3,int &v4,int &v5)
{
	GetToken(); Compute(&v1);
	GetToken(); Compute(&v2);
	GetToken(); Compute(&v3);
	GetToken(); Compute(&v4);
	GetToken(); Compute(&v5);
}

void Eval6(CScript *pScript,int &v1,int &v2,int &v3,int &v4,int &v5,int &v6)
{
	GetToken(); Compute(&v1);
	GetToken(); Compute(&v2);
	GetToken(); Compute(&v3);
	GetToken(); Compute(&v4);
	GetToken(); Compute(&v5);
	GetToken(); Compute(&v6);
}
void Eval7(CScript *pScript,int &v1,int &v2,int &v3,int &v4,int &v5,int &v6,int &v7)
{
	GetToken(); Compute(&v1);
	GetToken(); Compute(&v2);
	GetToken(); Compute(&v3);
	GetToken(); Compute(&v4);
	GetToken(); Compute(&v5);
	GetToken(); Compute(&v6);
	GetToken(); Compute(&v7);
}
void Get2Tkn( CScript *pScript )
{
	GetToken(); GetToken();    
}

////////////////////////////////
// 사용자 정의 함수들 
////////////////////////////////

// SayName(int nId,int nNameId,LPCTSTR lpszStr,...);
int SayName( CScript *pScript )
{
	int array[32], v1,v2, i = 0;
	CString strToken;
	CString string;
	Eval2(pScript,v1,v2);
	GetToken(); // ,
	GetToken(); // lpszStr
	strToken = token;
	GetToken(); 
	while(*token != ')')
	{
		Compute(&array[i++]);
		GetToken();
	}
	string.FormatV(strToken,(va_list)array);
	CMover* pMover = prj.GetMover(v1);
	CMover* pMoverName = prj.GetMover(v2);
	if(pMover && pMover->GetType() == OT_MOVER && pMoverName && pMoverName->GetType() == OT_MOVER)
	{
		CString stringName;
		stringName = pMoverName->GetName( TRUE );
		stringName += "! ";
		stringName += string;
	}
	return 1;
}
// 서버에서 클라이언트로 대화 메시지를 보낼 때 
// Speak(int nId,LPCTSTR lpszStr,...);
int Speak( CScript *pScript )
{
	int array[32], v1, i = 0;
	CString strToken;
	CString string;
	Eval1(pScript,v1);
	GetToken(); // ,
	GetToken(); // lpszStr
	strToken = token;
	GetToken(); 
	while(*token != ')')
	{
		Compute(&array[i++]);
		GetToken();
	}
	string.FormatV(strToken,(va_list)array);

	CMover* pMover = prj.GetMover(v1);
	if(pMover && pMover->GetType() == OT_MOVER)
	{
		g_UserMng.AddChat( (CCtrl*)pMover, (LPCSTR)string );
	}
	return 1;
}

// 서버에서 클라이언트로 대화 메시지를 보낼 때 
// SpeakName(int nId,int nNameId,LPCTSTR lpszStr,...);
int SpeakName( CScript *pScript )
{
	int array[32], v1,v2, i = 0;
	CString strToken;
	CString string;
	Eval2(pScript,v1,v2);
	GetToken(); // ,
	GetToken(); // lpszStr
	strToken = token;
	GetToken(); 
	while(*token != ')')
	{
		Compute(&array[i++]);
		GetToken();
	}
	string.FormatV(strToken,(va_list)array);
	CMover* pMover = prj.GetMover(v1);
	CMover* pMoverName = prj.GetMover(v2);
	if(pMover && pMover->GetType() == OT_MOVER && pMoverName && pMoverName->GetType() == OT_MOVER)
	{
		CString stringName;
		stringName = pMoverName->GetName( TRUE );
		stringName += "! ";
		stringName += string;
		g_UserMng.AddChat( (CCtrl*)pMover, (LPCSTR)stringName );
	}
	return 1;
}

int GetVal( CScript *pScript )
{
	Get2Tkn(pScript); 
	return pScript->GetReturn();
}

int GetSrcId( CScript *pScript )
{
	Get2Tkn(pScript); 
	return pScript->GetPcId();
}

int GetDstId( CScript *pScript )
{
	Get2Tkn(pScript); 
	return pScript->GetNpcId();
}

int NpcId( CScript *pScript )
{
	return GetDstId( pScript );
}

int PcId( CScript *pScript )
{
	return GetSrcId( pScript );
}

int GetGValue_( CScript *pScript )
{
	int v = EvalExp(pScript);
	if( v < 0 || v > 15 )
	{
		Error( "GetGValue_에서 %d(0~15) 값 이상", v );
		v = 0;
	}
	return pScript->m_panGlobalVal[ v ];
}
int SetGValue( CScript *pScript )
{
	int v1, v2;
	Eval2(pScript,v1,v2);
	GetToken();
	if( v1 < 0 || v1 > 15 )
	{
		Error( "SetGValue에서 %d(0~15) 값 이상", v1 );
		v1 = 0;
	}
	pScript->m_panGlobalVal[ v1 ] = v2;
	return TRUE;
}
int AddGValue( CScript *pScript )
{
	int v1, v2;
	Eval2(pScript,v1,v2);
	GetToken();
	if( v1 < 0 || v1 > 15 )
	{
		Error( "AddGValue에서 %d(0~15) 값 이상", v1 );
		v1 = 0;
	}
	pScript->m_panGlobalVal[ v1 ] += v2;
	return TRUE;
}
int AddKey( CScript* pScript )
{
	DWORD dwParam = 0;
	CString strWord, strKey;
	GetToken(); // (
	for( int i = 0; *token != ')' && i < 3; i++ )
	{
		GetToken(); // data
		if( i == 0 )
			strWord		= token;
		else
		if( i == 1 )
			strKey	= token;
		else
		if( i == 2 )
			dwParam		= atoi( token );
		GetToken(); // ,
	}
	if( strKey.IsEmpty() )	
		strKey	= strWord;
	LPSCRIPT lpScript = GetScript( pScript, strKey );
	if( lpScript == NULL ) 
	{
		Error( "%s(%d) : %s", pScript->m_strFileName, pScript->GetLineNum(), strKey );
		return 1;
	}
	__AddKey( pScript->GetPcId(), strWord, strKey, dwParam );

	return 1;
}
int AddCondKey( CScript* pScript )
{
	DWORD dwParam = 0;
	CString strWord, strKey;
	GetToken(); // (
	for( int i = 0; *token != ')' && i < 3; i++ )
	{
		GetToken(); // data
		if( i == 0 )
			strWord		= token;
		else
		if( i == 1 )
			strKey	= token;
		else
		if( i == 2 )
			dwParam		= atoi( token );
		GetToken(); // ,
	}
	if( strKey.IsEmpty() )	
		strKey	= strWord;
	
	LPSCRIPT lpScript = GetScript( pScript, strKey );
	if( lpScript == NULL ) 
	{
		Error( "%s(%d) : %s", pScript->m_strFileName, pScript->GetLineNum(), strKey );
		return 1;
	}
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	CMover* pMover = prj.GetMover( pScript->GetNpcId() );

	for( i = 0; i < pUser->m_nQuestSize; i++ )
	{
		LPQUEST lpQuest = &pUser->m_aQuest[ i ];
		QuestProp* pQuestProp = pUser->m_aQuest[ i ].GetProp();
		if( pQuestProp )
		{
			if( strcmp( pQuestProp->m_szEndCondDlgCharKey, pMover->m_szCharacterKey ) == 0 )
			{
				if( strcmp( pQuestProp->m_szEndCondDlgAddKey, strKey ) == 0 )
				{
					__AddKey( pScript->GetPcId(), strWord, strKey, dwParam );
					return 1;
				}
			}
		}
	}
	return 0;
}
int AddAnswer( CScript* pScript )
{
	int nParam1	= 0;
	int nParam2	= 0;
	CString strWord, strKey;
	GetToken(); // (
	for( int i = 0; *token != ')' && i < 3; i++ )
	{
		if( i == 0 )
		{
			GetToken();
			strWord	= token;
		}
		else
		if( i == 1 )
		{
			GetToken();
			if( tokenType == STRING )
				strKey = token;
			else
			{
				PutBack();
				i++;
			}
		}
		else
		if( i == 2 )
		{
			Compute( &nParam1 );
		}
		else
		if( i == 3 )
		{
			Compute( &nParam2 );
		}

		GetToken();	// ,
	}
	if( strKey.IsEmpty() )	
		strKey	= strWord;
	LPSCRIPT lpScript = GetScript( pScript, strKey );
	if( lpScript == NULL ) 
	{
		Error( "%s(%d) : %s", pScript->m_strFileName, pScript->GetLineNum(), strKey );
		return 1;
	}
	return __AddAnswer( pScript->GetPcId(), strWord, strKey, nParam1, nParam2 );
}
int GetParam( CScript* pScript )
{
	int v1, v2;
	Eval2( pScript, v1, v2 );

	if( v1 < 0 || v1 > 3 )
	{
		v1 = 0;
		Error( "GetParam에서 Idx 값 범위 초과(0~3)" ); 
	}
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( v2 );
	if( pQuestProp )
		return pQuestProp->m_nParam[ v1 ];
	return 0;
}
int RemoveQuest( CScript* pScript )      
{
	int v1	= EvalExp( pScript );
	return __RemoveQuest( pScript->GetPcId(), v1 );
}
int RemoveAllKey(CScript* pScript)
{
	Get2Tkn( pScript );
	__RemoveAllKey( pScript->GetPcId() );
	return 1;
}
// SayQuest( int nQuest, int nDialog )
int SayQuest( CScript* pScript )
{
	int v1, v2, i = 0;
	Eval2( pScript, v1, v2 );
	GetToken(); // )

	__SayQuest( pScript->GetPcId(), v1, v2 );
	return 1;
}

int BeginQuest( CScript* pScript )
{
	int v1	= EvalExp( pScript );
	return __RunQuest( pScript->GetPcId(), pScript->GetNpcId(), v1 );
}
/*
int IsEndQuestCondition( CScript* pScript )
{
	int v1 = EvalExp( pScript );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	return __IsEndQuestCondition( pUser, v1 );
}
*/
int IsBeginQuestCondition( CScript* pScript )
{
	int v1 = EvalExp( pScript );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	return __IsBeginQuestCondition( pUser, v1 );
}
int EndQuest( CScript* pScript )
{
	int v1 = EvalExp( pScript );
	return __EndQuest( pScript->GetPcId(), v1, FALSE );
}

// void AddQuestKey( int nQuest_szTitle, LPCTSTR lpKey, int nParam );
int AddQuestKey( CScript* pScript )
{
	int nParam = 0;
	int nQuest = -1;
	CString strKey;
	GetToken(); // (
	for( int i = 0; *token != ')' && i < 3; i++ )
	{
		if( i == 0 )
		{
			Compute( &nQuest );

		}
		if( i == 1 )
		{
			GetToken(); // data
			if( tokenType == STRING )
				strKey = token;
			else
			{
				PutBack();
				i++;
			}
		}
		if( i == 2 )
		{
			Compute( &nParam );
		}
		GetToken(); // ,
	}
	if( strKey.IsEmpty() )
	{
		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( nQuest );
		if( pQuestProp )
			strKey = pQuestProp->m_szTitle;
	}
	LPSCRIPT lpScript = GetScript( pScript, strKey );
	if( lpScript == NULL ) 
	{
		Error( "%s(%d) : %s", pScript->m_strFileName, pScript->GetLineNum(), strKey );
		return 1;
	}

	return __AddQuestKey( pScript->GetPcId(), nQuest, strKey, nParam );
}
// SysMessage( int nLeft, int nTop, int nTight, int nBottom, LPCTSTR lpszMessage );
int SysMessage( CScript* pScript )
{
	CRect rect;
	Eval4( pScript, (int&)rect.left, (int&)rect.top, (int&)rect.right, (int&)rect.bottom );
	GetToken(); // ,
	GetToken(); // string
	rect.NormalizeRect();
	g_UserMng.AddWorldMsg( &rect, token );
	GetToken(); // )
	return 1;
}
// CreateMover( int nMoverId, int nMoverIdx, int x, int y, int z, BOOL bActAttack )      
int CreateMover( CScript* pScript )
{
	int nMoverId, nMoverIdx;
	int x, y, z, nActAttack;

	Eval2( pScript, nMoverId, nMoverIdx );
	Eval3( pScript, x, y, z );
	Eval1( pScript, nActAttack );
	GetToken(); // )

	CWorld* pWorld = __GetWorld( pScript->GetPcId(), pScript->GetSrcId(), pScript->GetCaller() );
	if( pWorld == NULL )
		return 0;

	DWORD dwData;
	if( pWorld->m_mapCreateChar.Lookup( nMoverId, (void*&)dwData ) )
	{
		// 동일한 키가 들어있다. 제거되기 전에는 안됨.
		return FALSE;
	}
	MoverProp* pMoverProp = prj.GetMoverProp( nMoverIdx );

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		CMover* pMover = (CMover*)CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
		if( NULL == pMover )	
			return FALSE;
		D3DXVECTOR3 vPos( (FLOAT)x, (FLOAT)y, (FLOAT)z );
		pMover->SetPos( vPos );
		vPos.y -= 1.0f;
		pWorld->GetUnderHeight( vPos );
		pMover->InitMotion( MTI_STAND );
		pMover->UpdateLocalMatrix();
		if( nActAttack )
			pMover->m_bActiveAttack = nActAttack;
		pMover->AddItToGlobalId();
		pWorld->AddObj( pMover, FALSE );
		pWorld->m_mapCreateChar.SetAt( nMoverId, (void*)pMover->GetId() );
	}
	return 1;
}     
// CreateChar( int nMoverId, int nMoverIdx, LPCTSTR lpszCharKey, int x,int y,int z )
int CreateChar( CScript* pScript )
{
	int nMoverId, nMoverIdx;
	int x, y, z;
	CHAR szKey[ 64 ];

	Eval2( pScript, nMoverId, nMoverIdx );
	GetToken(); // ,
	GetToken();
	strcpy( szKey, token );
	Eval3( pScript, x, y, z );
	GetToken(); // )

	CWorld* pWorld = __GetWorld( pScript->GetPcId(), pScript->GetSrcId(), pScript->GetCaller() );
	if( pWorld == NULL )
		return 0;

	DWORD dwData;
	if( pWorld->m_mapCreateChar.Lookup( nMoverId, (void*&)dwData ) )
	{
		// 동일한 키가 들어있다. 제거되기 전에는 안됨.
		return FALSE;
	}
	//CWorld* pWorld = g_WorldMng.GetWorld( globalArray[1] );
	MoverProp* pMoverProp = prj.GetMoverProp( nMoverIdx );
	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		CMover* pMover = (CMover*)CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
		if( NULL == pMover ) return FALSE; // ASSERT( pObj );
		strcpy( pMover->m_szCharacterKey, szKey );
		pMover->InitNPCProperty();
		pMover->InitCharacter( pMover->GetCharacter() );
		D3DXVECTOR3 vPos( (FLOAT)x, (FLOAT)y, (FLOAT)z );
		pMover->SetPos( vPos );
		vPos.y -= 1.0f;
		pWorld->GetUnderHeight( vPos );
		pMover->InitMotion( MTI_STAND );
		pMover->UpdateLocalMatrix();
		pMover->AddItToGlobalId();
		pWorld->AddObj( pMover, FALSE );
		pWorld->m_mapCreateChar.SetAt( nMoverId, (void*)pMover->GetId() );
	}
	return TRUE;
}    
// RemoveMover( int nMoverId );
int RemoveMover( CScript* pScript )
{
	BOOL bResult = FALSE;
	int nMoverId = EvalExp( pScript );

	CWorld* pWorld = __GetWorld( pScript->GetPcId(), pScript->GetSrcId(), pScript->GetCaller() );
	if( pWorld == NULL )
		return 0;

	OBJID idMover;
	if( pWorld->m_mapCreateChar.Lookup( nMoverId, (void*&)idMover ) )
	{
		CMover* pMover	= prj.GetMover( idMover );
		if( IsValidObj( (CObj*)pMover ) )
		{
			if( pMover->IsNPC() )
			{
				pMover->Delete();
				bResult = TRUE;
			}
		}
		pWorld->m_mapCreateChar.RemoveKey( nMoverId );
	}

	return bResult;
}   
int GetDay( CScript* pScript )
{
	Get2Tkn( pScript );
	return g_GameTimer.m_nDay;
}        
int GetMin( CScript* pScript )
{
	Get2Tkn( pScript );
	return g_GameTimer.m_nMin;
}      
int GetHour( CScript* pScript )
{
	Get2Tkn( pScript );
	return g_GameTimer.m_nHour;
}       
 
int SetTimer( CScript* pScript )  
{
	int nTimerId, nSec;
	Eval2( pScript, nTimerId, nSec );
	GetToken();
	if( nTimerId >= MAX_SCRIPT_TIMER )
	{
		Error( "Script : SetTimer에서 타이머 ID Max 초과 ( %d / %d )", nTimerId, MAX_SCRIPT_TIMER  );
		nTimerId = 0 ;
	}
	pScript->m_paTimer[ nTimerId ].Set( SEC( nSec ) );
	return 1;
}
int IsTimeOut( CScript* pScript )
{
	int nTimerId = EvalExp( pScript );
	if( nTimerId >= MAX_SCRIPT_TIMER )
	{
		Error( "Script : IsTimeOut 타이머 ID Max 초과 ( %d / %d )", nTimerId, MAX_SCRIPT_TIMER  );
		nTimerId = 0 ;
	}
	if( pScript->m_paTimer[ nTimerId ].IsTimeOut() )
	{
		pScript->m_paTimer[ nTimerId ].Reset();
		return TRUE;
	}
	return FALSE;
}

int LaunchQuest( CScript* pScript )
{
	Get2Tkn( pScript );
	pScript->m_panGlobalVal[ 15 ] = 0;
	__QuestEnd( pScript->GetPcId(), pScript->GetNpcId(), pScript->m_panGlobalVal[ 15 ] );
	return 1;
}
int GetEmptyInventoryNum( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	return pUser->m_Inventory.GetEmptyCount();
}

int GetQuestState( CScript* pScript )
{
	int v1 = EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	LPQUEST pQuest = pUser->GetQuest( v1 );
	if( pQuest ) 
		return pQuest->m_nState;
	QUEST quest;
	if( pUser->MakeCompleteQuest( v1, &quest ) )
		return quest.m_nState;
	return -1;
}
int IsSetQuest( CScript* pScript )
{
	int v1 = EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	LPQUEST pQuest = pUser->GetQuest( v1 );
	if( pQuest ) return TRUE;
	if( pUser->IsCompleteQuest( v1 ) )
		return TRUE;
	return FALSE;
}

int SetMark( CScript* pScript )
{
	return 1;
}
int GoMark( CScript* pScript )
{
	return 1;
}

int RandomIndex( CScript* pScript) //100, 20, 12, 1, 20, 12, 1, 20, 13, 1 )
{
	int nRandom, nMax = 1;
	Eval1( pScript, nRandom ); // ( max
	GetToken(); // , or )
	int anValue[64], v1;
	anValue[ 0 ] = 0;
	while( *token != ')' )
	{
		Compute( &v1 );
		anValue[ nMax++ ] = v1;
		GetToken(); // , or )
	}
	int nResult = 0;
	for( int i = 0; i < nMax; i++ )
	{
		nResult += anValue[ i ];
		anValue[ i ] = nResult;
	}
	int nRand = random( nRandom );
	for( i = 0; i < nMax - 1;  i++ )
	{
		if( nRand >= anValue[ i ] && nRand < anValue[ i + 1 ]  )
			return i;
	}
	return 0;
}

int RemoveKey(CScript* pScript)
{
	GetToken();
	GetToken();
	CString strWord		= token;
	GetToken();

	LPSCRIPT lpScript = GetScript( pScript, strWord );
	if( lpScript )
	{
		CUser* pUser	= prj.GetUser( pScript->GetPcId() );

		RunScriptFunc rsf;
		rsf.wFuncType		= FUNCTYPE_REMOVEKEY;
		lstrcpy( rsf.lpszVal1, strWord );
		pUser->AddRunScriptFunc( rsf );
	}
	return 1;
}

int Say(CScript* pScript)
{
	int array[32], i	= 0;
	CString strToken;
	CString string;
	GetToken(); // (
	GetToken(); // lpszStr
	strToken	= token;
	GetToken(); 
	while( *token != ')' )
	{
		Compute( &array[i++] );
		GetToken();
	}
	string.FormatV( strToken, (va_list)array );

	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_SAY;
	lstrcpy( rsf.lpszVal1, string );
	rsf.dwVal2 = 0;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}

int EndSay( CScript* pScript )
{
	Get2Tkn( pScript );
	// 사용 안함 
	return 1;
}

int Random( CScript* pScript )
{
	int val = EvalExp( pScript );
	return rand() % val;
}

int Exit( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_EXIT;
	pUser->AddRunScriptFunc( rsf );
	return TRUE;
}

int SetScriptTimer( CScript* pScript )
{
	UINT nTimer = EvalExp( pScript );
	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	return 1;
}

// Replace( int nWorld, int x, int y, int z )
int Replace( CScript* pScript )
{
	int v1, v2, v3, v4;
	GetToken(); // (
	Compute( &v1 );
	GetToken(); // ,
	Compute( &v2 );
	GetToken(); // ,
	Compute( &v3 );
	GetToken(); // ,
	Compute( &v4 );
	GetToken(); // )

	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	if( v2 > 0 && v4 > 0 )
	{
		RunScriptFunc rsf;	// Exit()
		rsf.wFuncType		= FUNCTYPE_EXIT;
		pUser->AddRunScriptFunc( rsf );
		if( g_eLocal.GetState( EVE_SCHOOL ) )
		{
			g_dpDBClient.SendLogSchool( pUser->m_idPlayer, pUser->GetName() );
		}

		pUser->Replace( g_uIdofMulti, v1, D3DXVECTOR3( v2, v3, v4 ) );
	}
	return 1;
}

// Replace( int nWorld, LPCTSTR szKey )
int ReplaceKey( CScript* pScript )
{
	int v1;
	CString strKey;
	GetToken(); // (
	Compute( &v1 );
	GetToken(); // ,
	GetToken(); // key
	strKey = token;
	GetToken(); // )

	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	RunScriptFunc rsf;	// Exit()
	rsf.wFuncType		= FUNCTYPE_EXIT;
	pUser->AddRunScriptFunc( rsf );

	PRegionElem pRgnElem	= g_WorldMng.GetRevivalPos( v1, strKey );
	if( NULL != pRgnElem )
	{
		pUser->Replace( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos );
	}
	return 1;
}

int GetPlayerLvl( CScript* pScript )  
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	return pUser->GetLevel();
}



int GetPlayerJob( CScript* pScript )  
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	return pUser->GetJob();
}

int IsGuildQuest( CScript* pScript )
{
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	CGuild* pGuild	= pUser->GetGuild();
	if( pGuild )
	{
		PGUILDQUEST pQuest	= pGuild->GetQuest( v1 );
		if( pQuest )
			return TRUE;
	}
	return FALSE;
}

int GetGuildQuestState( CScript* pScript )
{
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	CGuild* pGuild	= pUser->GetGuild();
	if( pGuild )
	{
		PGUILDQUEST pQuest	= pGuild->GetQuest( v1 );
		if( pQuest )
			return pQuest->nState;
	}
	return -1;
}

int IsWormonServer( CScript* pScript )
{
	Get2Tkn( pScript );
	return g_eLocal.GetState( EVE_WORMON );
}


int PrintSystemMessage( CScript* pScript )
{
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	pUser->AddDefinedText( (DWORD)v1 );
	return 1;
}

int MonHuntStart( CScript* pScript )
{
	int v1, v2, v3, v4;
	Eval4( pScript, v1, v2, v3, v4 );
	GetToken();
	CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
	if( pProcessor->IsQuesting( v1 ) )
	{
		return 0;
	}
	else
	{
		CUser* pUser	= prj.GetUser( pScript->GetPcId() );
		CGuild* pGuild	= pUser->GetGuild();
		if( pGuild )
		{
			GUILDQUESTPROP* pProp	= prj.GetGuildQuestProp( v1 );
			if( pProp )
			{
				CMover* pWormon	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, pProp->dwWormon );
				if( pWormon )
				{
					pWormon->SetPos( pProp->vPos );
					pWormon->InitMotion( MTI_STAND );
					pWormon->UpdateLocalMatrix();
					pWormon->m_nQuestKeeping	= v1;

					CWorld* pWorld;
					if( ( pWorld = pUser->GetWorld() ) )
					{
						RunScriptFunc rsf;	// Exit()
						rsf.wFuncType		= FUNCTYPE_EXIT;
						pUser->AddRunScriptFunc( rsf );
						pWormon->AddItToGlobalId();
						pWorld->AddObj( pWormon, FALSE );
						pGuild->SetQuest( v1, v2 );
						if( v2 == QS_BEGIN )
							g_dpDBClient.SendInsertGuildQuest( pGuild->m_idGuild, v1 );
						else
							g_dpDBClient.SendUpdateGuildQuest( pGuild->m_idGuild, v1, v2 );

						pProcessor->SetGuildQuest( v1, v2, v3, v4, pGuild->m_idGuild, pWormon->GetId() );
						D3DXVECTOR3 vPos( pProp->vPos.x, pProp->vPos.y, ( ( pProp->vPos.z * 2 ) + pProp->y2 ) / 3 );
						pGuild->Replace( pProp->dwWorldId, vPos, TRUE );
						return 1;
					}
					safe_delete( pWormon );
				}
			}
		}
	}
	return 0;
}

int MonHuntStartParty( CScript* pScript )
{
	int v1, v2, v3, v4;
	Eval4( pScript, v1, v2, v3, v4 );
	GetToken();
	CPartyQuestProcessor* pProcessor	= CPartyQuestProcessor::GetInstance();
	if( pProcessor->IsQuesting( v1 ) )
	{
		return 0;
	}
	else
	{
		CUser* pUser	= prj.GetUser( pScript->GetPcId() );

		CParty* pParty	= g_PartyMng.GetParty( pUser->GetPartyId() );
		if( pParty )
		{
			PARTYQUESTPROP* pProp	= prj.GetPartyQuestProp( v1 );
			if( pProp )
			{				
				if( pParty->ReplaceChkLv( pProp->nLevel+5 ) == FALSE )
				{
					CString str;
					str.Format( prj.GetText(TID_GAME_PARTYQUEST_NOT_JOIN), pProp->nLevel+5 );
					pUser->AddDiagText( str );

					return 0;
				}

				// 기존에 들어있던...파티가 깨진 플레이어들... 마을로 소환시킨다...
//				g_UserMng.ReplaceWorldArea( pProp->dwWorldId, WI_WORLD_MADRIGAL, 6968.0f, 3328.8f, 0.5f, FALSE );				

				BOOL bSuccess = FALSE;

				for( int i=0; i<pProp->vecWormon.size(); i++ )
				{
					WORMON* WorMon;
					WorMon = &(pProp->vecWormon[i]);
					CMover* pWormon	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, WorMon->dwWormon );

					if( pWormon )
					{
						pWormon->SetPos( WorMon->vPos );
						pWormon->InitMotion( MTI_STAND );
						pWormon->UpdateLocalMatrix();
						pWormon->m_nPartyQuestKeeping = v1;

						CWorld* pWorld	= g_WorldMng.GetWorld( pProp->dwWorldId );
						if( pWorld )
						{
							RunScriptFunc rsf;	// Exit()
							rsf.wFuncType		= FUNCTYPE_EXIT;
							pUser->AddRunScriptFunc( rsf );
							pWormon->AddItToGlobalId();
							pWormon->m_bActiveAttack = TRUE;
							pWorld->AddObj( pWormon, FALSE );

							bSuccess = TRUE;								
						}
						else
						{
							safe_delete( pWormon );
						}
					}
				}
				
				if( bSuccess )
				{
					pProcessor->SetPartyQuest( v1, v2, v3, v4, pUser->GetPartyId(), 0 );
					pParty->Replace( pProp->dwWorldId, pProp->szWorldKey );
									
					return 1;
				}
			}
		}
	}
	return 0;
}

int	InitStat( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	pUser->SetStr( 15 );
	pUser->SetInt( 15 );
	pUser->SetDex( 15 );
	pUser->SetSta( 15 );
	pUser->m_nRemainGP	= ( pUser->GetLevel() - 1 ) * 2;
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITSTAT;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}

int	InitStr( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nStr = pUser->m_nStr;
	if( nStr - 15 > 0 )
	{
		pUser->SetStr( 15 );
		pUser->m_nRemainGP	+= nStr - 15;
	}
	
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITSTR;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}
int	InitSta( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nSta = pUser->m_nSta;
	if( nSta - 15 > 0 )
	{
		pUser->SetSta( 15 );
		pUser->m_nRemainGP	+= nSta - 15;
	}
	
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITSTA;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}
int	InitDex( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nDex = pUser->m_nDex;
	if( nDex - 15 > 0 )
	{
		pUser->SetDex( 15 );
		pUser->m_nRemainGP	+= nDex - 15;
	}
	
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITDEX;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}
int	InitInt( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nInt = pUser->m_nInt;
	if( nInt - 15 > 0 )
	{
		pUser->SetInt( 15 );
		pUser->m_nRemainGP	+= nInt - 15;
	}
	
	RunScriptFunc rsf;
	rsf.wFuncType		= FUNCTYPE_INITINT;
	rsf.dwVal1	= pUser->m_nRemainGP;
	pUser->AddRunScriptFunc( rsf );
	return 1;
}

int SetQuestState( CScript* pScript )      
{
	int v1, v2;
	Eval2( pScript, v1, v2 );
	GetToken();
	return __SetQuestState( pScript->GetPcId(), v1, v2 );
}
int SetQuest( CScript* pScript )      
{
	int v1	= EvalExp( pScript );
	__SetQuest(  pScript->GetPcId(), v1 );
	return 1;
}

int CreateItem( CScript* pScript )	
{
	int v1, v2;
	Eval2( pScript, v1, v2 );
	GetToken();
	__CreateItem( pScript->GetPcId(), v1, v2 );
	return 1;
}

int AddGold( CScript* pScript )     
{
	int v1	= EvalExp( pScript );
	__AddGold(  pScript->GetPcId(), v1 );
	return 1;
}

int	RemoveGold( CScript* pScript )
{
	int v1	= EvalExp( pScript );
	__RemoveGold( pScript->GetPcId(), v1 );
	return 1;
}
int GetPlayerGold( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	return pUser->GetGold();
}


int GetLocalEventState( CScript* pScript )
{
	int v1	= EvalExp( pScript );
	return g_eLocal.GetState( v1 );
}

int AddExp( CScript* pScript )      
{
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	if( pUser->AddExperience( v1, TRUE, FALSE ) )
		pUser->LevelUpSetting();
	else
		pUser->ExpUpSetting();

	pUser->AddSetExperience( pUser->GetExp1(), (WORD)pUser->m_nLevel, pUser->m_nSkillPoint, pUser->m_nSkillLevel );
	pUser->AddDefinedText( TID_GAME_REAPEXP );
	return 1;
}

int IsParty( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	int f	= ( pParty != NULL && pParty->IsMember( pUser->m_idPlayer ) );
	return f;
}

int IsPartyMaster( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	int f	= ( pParty != NULL && pParty->IsLeader( pUser->m_idPlayer ) );
	return f;
}

int GetPartyNum( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nSize	= 0;
	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
		nSize	= pParty->GetSizeofMember();
	return nSize;
}

int	GetPartyLevel( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nLevel	= 0;
	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
		nLevel	= pParty->GetLevel();
	return nLevel;
}

int IsPartyGuild( CScript* pScript )
{
	Get2Tkn( pScript );

	int f	= 0;
	u_long	aidPlayer[MAX_PTMEMBER_SIZE];
	int	nSize	= 0;
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );

	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty )
	{
		nSize	= pParty->m_nSizeofMember;
		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
			aidPlayer[i]	= pParty->m_aMember[i].m_uPlayerId;
	}
	else
	{
		f	= 1;
	}

	for( int i = 0; i < nSize; i++ )
	{
		CUser* pUsertmp	= g_UserMng.GetUserByPlayerID( aidPlayer[i] );
		if( IsValidObj( pUsertmp ) )
		{
			if( pUsertmp->GetGuild() )
			{
				f	= 1;
				break;
			}
			CTime tCurrent = CTime::GetCurrentTime();
			if( tCurrent < pUsertmp->m_tGuildMember )
			{
				f	= 2;
				break;
			}
		}
		else
		{
			f	= 1;
			break;
		}
	}
	return f;

}

int IsGuild( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	int f	= ( pGuild && pGuild->IsMember( pUser->m_idPlayer ) );
	return f;
}

int IsGuildMaster( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	int f	= ( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) );
	return f;
}

int CreateGuild( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	int nSize	= 0;
	GUILD_MEMBER_INFO	info[MAX_PTMEMBER_SIZE];

	CParty* pParty	= g_PartyMng.GetParty( pUser->m_idparty );
	if( pParty && pParty->IsLeader( pUser->m_idPlayer ) )
	{
		for( int i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			CUser* pUsertmp	= g_UserMng.GetUserByPlayerID( pParty->GetPlayerId( i ) );
			if( IsValidObj( pUsertmp ) )
			{
				info[nSize].idPlayer	= pUsertmp->m_idPlayer;
				info[nSize].nLevel	= pUsertmp->GetLevel();
				info[nSize].nJob	= pUsertmp->GetJob();
				info[nSize].dwSex	= pUsertmp->GetSex();
				nSize++;
			}
			else
			{
				// error	- offline
				return 0;
			}
		}
	}
	else
	{
		// error	-
		return 0;
	}

	g_DPCoreClient.SendCreateGuild( info, nSize, "" );

	return 1;
}

int DestroyGuild( CScript* pScript )
{
	Get2Tkn( pScript );

	CUser* pUser	= prj.GetUser( pScript->GetPcId() );

	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild && pGuild->IsMember( pUser->m_idPlayer ) )
	{
		if( pGuild->IsMaster( pUser->m_idPlayer ) )
		{
			RunScriptFunc rsf;
			rsf.wFuncType	= FUNCTYPE_DESTROYGUILD;
			pUser->AddRunScriptFunc( rsf );
		}
		else
			pUser->AddDefinedText( TID_GAME_COMDELNOTKINGPIN );
	}
	else
		pUser->AddDefinedText( TID_GAME_COMNOHAVECOM );


	return 1;
}

int IsPlayerSkillPoint( CScript* pScript )     
{
	return 0;
}

int ChangeJob( CScript* pScript )     
{
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	if( pUser->IsBaseJob() && pUser->GetLevel() != MAX_JOB_LEVEL )
	{
		Error( "Error ChangeJob : 1" );
		return 0;
	}
	if( pUser->IsExpert() && pUser->GetLevel() < MAX_JOB_LEVEL + MAX_EXP_LEVEL )
	{
		Error( "Error ChangeJob : 2" );
		return 0;
	}
	if( pUser->IsPro()  && pUser->GetLevel() < MAX_GENERAL_LEVEL && pUser->GetExpPercent() < 9999 )
	{
		Error( "Error ChangeJob : 3" );
		return 0;
	}
	if( pUser->AddChangeJob( v1 ) )
	{
		pUser->AddSetChangeJob( v1 );
		g_UserMng.AddNearSetChangeJob( (CMover*)pUser, v1, &pUser->m_aJobSkill[MAX_JOB_SKILL] );
		// #dlvr
		g_dpDBClient.SendLogLevelUp( (CUser*)pUser, 4 );
#ifdef __S_RECOMMEND_EVE
		if( g_eLocal.GetState( EVE_RECOMMEND ) && pUser->IsPlayer() )
			g_dpDBClient.SendRecommend( (CUser*)pUser, v1 );
#endif // __S_RECOMMEND_EVE
#if __VER >= 11 // __SYS_PLAYER_DATA
		g_DPDBClient.SendUpdatePlayerData( pUser );
#else	// __SYS_PLAYER_DATA
		g_DPCoreClient.SendPartyMemberJob( (CUser*)pUser );
		g_DPCoreClient.SendFriendChangeJob( (CUser*)pUser );
		if( pUser->m_idGuild != 0 )
			g_DPCoreClient.SendGuildChangeJobLevel( (CUser*)pUser );
#endif	// __SYS_PLAYER_DATA
	}
	else
	{
		Error( "Error ChangeJob : CMover::AddChangeJob" );
	}
	return 1;
}

int GetPlayerSex( CScript* pScript )  
{
	Get2Tkn(pScript);
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	return pUser->GetSex();
}
int GetItemNum( CScript* pScript )    
{
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	return pUser->GetItemNum( v1 );
}
int RemoveAllItem( CScript* pScript ) 
{
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	pUser->RemoveAllItem( v1 );
	return 1;
}
int EquipItem( CScript* pScript )     
{
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	BYTE nId, nCount	= 1;
	
	CItemElem itemElem;
	itemElem.m_dwItemId		= v1;
	itemElem.m_nItemNum		= 1;
	itemElem.SetAbilityOption( 0 );
	if( pUser->CreateItem( &itemElem, &nId ) == TRUE )
	{
		CItemElem* pItemElem	= pUser->m_Inventory.GetAtId( nId );
		if( pItemElem && TRUE == pUser->DoEquip( pItemElem, TRUE ) )
		{
			g_UserMng.AddDoEquip( (CMover*)pUser, -1, pItemElem, TRUE );
		}

		LogItemInfo aLogItem;
		aLogItem.Action = "Q";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "QUEST";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
		g_DPSrvr.OnLogItem( aLogItem, &itemElem, itemElem.m_nItemNum );
	}
	else
	{
		CItemElem* pItemElem	= new CItemElem;
		pItemElem->m_dwItemId	= v1;
		ItemProp* pItemProp		= pItemElem->GetProp();
		if( pItemProp )
		{
			pItemElem->m_nHitPoint	= pItemElem->m_nMaxHitPoint = pItemProp->dwEndurance;
			if( pItemElem->m_nMaxHitPoint < 0 )
				pItemElem->m_nMaxHitPoint = 0;

			pItemElem->m_nItemNum	= 1;
			pItemElem->SetAbilityOption( 0 );
			CItem* pItem	= new CItem;
			pItem->m_pItemBase	= pItemElem;

			if( pItemElem->m_dwItemId == 0 ) Error("EquipItem SetIndex\n" );
			pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
			pItem->SetPos( pUser->GetPos() );
			pItem->SetAngle( xRandom( 360 ) );
			pItem->m_idHolder	= pUser->m_idPlayer;
			pUser->GetWorld()->AddObj( pItem, TRUE );
			pUser->AddDefinedText( TID_GAME_DROPITEM, "\"%s\"", prj.GetItemProp( v1 )->szName );
		}
	}
	return 1;
}

// int PlaySound( LPCTSTR lpszSoundFileName )
int PlaySound(CScript* pScript)
{
	GetToken();
	GetToken();
	CUser* pUser = prj.GetUser( pScript->GetPcId() );
	if( pUser )
	{
		pUser->AddPlaySound2( token );
	}
	GetToken();
	return 1;
}

int DropQuestItem( CScript* pScript )
{
// ( dwItemId, dwProbability )
	int v1, v2;
	Eval2( pScript, v1, v2 );
	GetToken();
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );

	if( xRandom( 1000 ) <  v2 )
	{
		ItemProp* pItemProp	= prj.GetItemProp( v1 );
		if( pItemProp )
		{
			CWorld* pWorld	= pUser->GetWorld();
			if( pWorld )
			{
				CItemElem* pItemElem	= new CItemElem;
				pItemElem->m_dwItemId	= v1;
				CItem* pItem	= new CItem;
				pItem->m_pItemBase	= pItemElem;
				if( pItemElem->m_dwItemId == 0 ) Error( "DropQuestItem SetIndex\n" );
				pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
				pItem->m_idHolder	= pUser->m_idPlayer;
				pItem->m_idOwn	= pUser->GetId();
				pItem->m_dwDropTime	= timeGetTime();
				CMover* pMover	= prj.GetMover( pScript->GetNpcId() );
				D3DXVECTOR3 vPos	= pMover->GetPos();
				vPos.x	+= ( xRandomF( 2.0f ) - 1.0f );
				vPos.z	+= ( xRandomF( 2.0f ) - 1.0f );
				pItem->SetPos( vPos );
				pItem->SetAngle( xRandom( 360 ) );
				pWorld->AddObj( pItem, TRUE );
				return 1;
			}
		}
		else
		{
			Error( "DropQuestItem, %d null prop", v1 );
		}
	}
	return 0;
}

int ExpQuestEffect(CScript* pScript)
{
// ( dwSfx, bNPC )
	int v1, v2;
	Eval2( pScript, v1, v2 );
	GetToken();
	CCtrl* pCtrl	= ( v2? prj.GetCtrl( pScript->GetNpcId() ): prj.GetCtrl( pScript->GetPcId() ) );
	g_UserMng.AddCreateSfxObj( pCtrl, v1 );
	return 1;
}

int RemoveItem( CScript* pScript )
{
	int v1, v2;
	Eval2( pScript, v1, v2 );
	GetToken();
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	return pUser->RemoveItemA( v1, v2 );
}

int Run( CScript* pScript )
{
	int v1;
	CString string;
	GetToken(); // (
	GetToken(); // key
	string = token;
	Eval1( pScript, v1 );
	GetToken(); // )

	CScriptDialog::SetBeforeRun( (const char*)string );

	CMover* pMover = prj.GetMover( pScript->GetNpcId() );
	int nResult = 0;
	pMover->m_pNpcProperty->RunDialog( string, &nResult, v1, pScript->GetNpcId(), pScript->GetPcId(), 0 );

	CScriptDialog::SetAfterRun( (const char*)string );

	return nResult;
}

int GetQuestId( CScript* pScript )
{
	Get2Tkn( pScript );
	return pScript->GetQuest();
}
int GetLang( CScript* pScript )
{
	Get2Tkn( pScript );
	return ::GetLanguage();
}

// 캐릭터 명 변경
int	QuerySetPlayerName( CScript* pScript )
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId()  );
	if( pUser->IsAuthHigher( AUTH_GAMEMASTER ) || pUser->IsMode( QUERYSETPLAYERNAME_MODE ) )
	{
		RunScriptFunc rsf;
		rsf.wFuncType	= FUNCTYPE_QUERYSETPLAYERNAME;
		pUser->AddRunScriptFunc( rsf );
		return 1;
	}
	return 0;
}

int Trace(CScript* pScript)
{
	int array[32], i	= 0;
	CString strToken;
	CString string;
	GetToken(); // (
	GetToken(); // lpszStr
	strToken	= token;
	GetToken(); 
	while( *token != ')' )
	{
		Compute( &array[i++] );
		GetToken();
	}
	string.FormatV( strToken, (va_list)array );

	TRACE( "%s\n", string );
	return 1;
}


int GetPlayerExpPercent( CScript* pScript )  
{
	Get2Tkn( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	return pUser->GetExpPercent();
}

int SetLevel( CScript* pScript )  
{
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	return pUser->SetLevel(v1);
}

int AddGPPoint( CScript* pScript )  
{
	int v1	= EvalExp( pScript );
	CUser* pUser	= prj.GetUser( pScript->GetPcId() );
	return pUser->AddGPPoint(v1);
}


//
// 함수 테이블
//
InterFuncType interFunc[] = 
{
	"GetVal"            ,	GetVal            ,
	"GetSrcId"          ,	GetSrcId          ,
	"GetDstId"          ,	GetDstId          ,
	"NpcId"             ,	NpcId             ,
	"PcId"              ,	PcId              ,
	"Src"               ,	GetSrcId          ,
	"Dst"               ,	GetDstId          ,
	"PC"                ,	GetSrcId          ,
	"NPC"               ,	GetDstId          ,
	"Say"               ,	Say               ,
	"SayName"           ,	SayName           ,
	"AddSay"            ,	Say               ,
	"EndSay"            ,	EndSay            ,
	"Speak"             ,	Speak             ,
	"SpeakName"         ,	SpeakName         ,
	"Random"            ,	Random            ,
	"Replace"			,	Replace           ,
	"ReplaceKey"		,	ReplaceKey        ,
	"GetPlayerLvl"        , GetPlayerLvl      ,//( Id ) 캐릭터의 레벨을 얻어온다.
	"GetPlayerJob"        , GetPlayerJob      ,//( Id ) 캐릭터의 직업을 얻어온다. defineJob의 넘버
	"SetQuestState"       , SetQuestState     ,//( Id ) 해당 이벤트를 실행시켜준다.
	"GetQuestState"       , GetQuestState     ,//( Id ) 해당 이벤트가 실행중인지 확인한다.
	"IsSetQuest"          , IsSetQuest        ,//( Id ) 해당 이벤트가 실행중인지 확인한다.
	"SetQuest"            , SetQuest          ,//( Id ) 해당 이벤트가 실행중인지 확인한다.
	"RemoveQuest"         , RemoveQuest       ,//( Id ) 해당 이벤트가 실행중인지 확인한다.
	"AddExp"              , AddExp            ,// 경험치 업 
	"CreateItem"          , CreateItem        ,//( Id ) 해당 이벤트가 실행중인지 확인한다.
	"AddGold"         ,		AddGold           ,//( Id ) 해당 이벤트가 실행중인지 확인한다.
	"RemoveGold"	,		RemoveGold	      ,
	"GetPlayerGold"		,	GetPlayerGold	  ,
	"QuerySetPlayerName",	QuerySetPlayerName,// 플레이어 명 변경
	"IsGuildQuest"	,		IsGuildQuest	  ,
	"GetGuildQuestState",	GetGuildQuestState,// ( nQuestId )
	"IsWormonServer"	,	IsWormonServer    ,// ()
	"MonHuntStart"	,		MonHuntStart      ,// ( nQuestId, nState, ns, nf )
	"MonHuntStartParty"	,	MonHuntStartParty ,// ( nQuestId, nState, ns, nf )
	"DropQuestItem"	,		DropQuestItem	  ,// ( dwItemId, dwProbability )
	"ExpQuestEffect"	,	ExpQuestEffect	  ,// ( dwSfx, bNPC )
	"RemoveItem"	,		RemoveItem	      ,//( dwItemId, num )
	"GetLocalEventState",	GetLocalEventState,//길드대항전(독도이벤트)관련 
	"InitStat",				InitStat          ,// 스탯 초기화
	"InitStr",				InitStr           ,// 스탯 Str 초기화
	"InitSta",				InitSta           ,// 스탯 Sta 초기화
	"InitDex",				InitDex           ,// 스탯 Dex 초기화
	"InitInt",				InitInt           ,// 스탯 Int 초기화
	"RandomIndex"         , RandomIndex       ,//( Id ) 해당 이벤트가 실행중인지 확인한다.
	"GetItemNum"        ,	GetItemNum        ,//( Id, Item_ID ) 해당 유저의 해당 아이템을 인벤토리에 몇개 가지고 있는지 확인한다. 장비창 제외
	"RemoveAllItem"     ,	RemoveAllItem     ,//( Id, Item_ID) 해당 유저의 해당 아이템을 모두 제거 한다.
	"ChangeJob"         ,	ChangeJob         ,//( Id ) 해당 넘버로 직업을 변경해준다. defineJob의 넘버
	"GetPlayerSex"      ,	GetPlayerSex      ,// ( ID ) 해당 유저의 성이 무엇인지 확인 defineAttribute.h에 설정
	"EquipItem"         ,	EquipItem         ,//( ID, Item_ID) 해당 유저에게 해당 아이템을 장착해줌 능력치가 안되면 인벤토리에 넣어주며 인벤토리가 꽉차면 아무거나 땅에 떨어뜨림 장착하고 떨어지는 아이템도 동일하게 처리
	"IsPlayerSkillPoint",	IsPlayerSkillPoint,// 해당 유저가 m_dwSkillPoint값이 있으면 TRUE, 0이면 FALSE
	"SetScriptTimer"    ,	SetScriptTimer    ,
	"AddKey"           ,	AddKey            ,
	"AddAnswer"          ,	AddAnswer         ,
	"RemoveKey"        ,	RemoveKey         ,
	"RemoveAllKey"     ,	RemoveAllKey      ,
	"SetMark"          ,	SetMark           ,
	"GoMark"           ,	GoMark            ,
	"IsParty"        ,		IsParty           ,
	"IsPartyMaster"  ,		IsPartyMaster     ,
	"GetPartyNum"	,		GetPartyNum	      ,
	"GetPartyLevel"	,		GetPartyLevel     ,
	"IsPartyGuild"	,		IsPartyGuild	  ,	// 길드결성시 체크조항 
	"IsGuild"        ,		IsGuild           ,
	"IsGuildMaster"  ,		IsGuildMaster     ,
	"CreateGuild"    ,		CreateGuild       ,
	"DestroyGuild"   ,		DestroyGuild      ,
	"PlaySound"        ,	PlaySound         ,
	"GetLang"          ,	GetLang           ,
	"Run"	            ,	Run               ,
	"GetQuestId"        ,	GetQuestId        ,
	"BeginQuest"        ,	BeginQuest        ,
	"EndQuest"          ,	EndQuest          ,
	"AddQuestKey"       ,	AddQuestKey       ,
	"SayQuest"          ,	SayQuest          ,
	"PrintSystemMessage",	PrintSystemMessage,
	"GetDay"            ,	GetDay            , 
	"GetMin"            ,	GetMin            , 
	"GetHour"           ,	GetHour           , 
	"SetTimer"          ,	SetTimer          , 
	"CreateChar"        ,	CreateChar        , 
	"RemoveMover"       ,	RemoveMover       , 
	"AddCondKey"        ,	AddCondKey        , 
	"GetEmptyInventoryNum", GetEmptyInventoryNum,
	"GetGValue"			,	GetGValue_		  , //테스트용 
	"SetGValue"			,	SetGValue		  , //테스트용 
	"AddGValue"			,	AddGValue		  , //테스트용 
	"SysMessage"        ,	SysMessage        , //테스트용 
	"CreateMover"       ,	CreateMover       , //테스트용  
	"IsTimeOut"         ,	IsTimeOut         , //테스트용 
	"LaunchQuest"       ,	LaunchQuest       , //테스트용 
	"GetParam"          ,	GetParam          , //테스트용 
	"Trace"             ,	Trace             ,
	"Exit"              ,	Exit              ,
	"GetPlayerExpPercent"        , GetPlayerExpPercent      ,//( Id ) 캐릭터의 exp을 %로 얻어온다. 10000
	"SetLevel"        , SetLevel      ,						//케릭터의 레벨을 맞춘다
	"AddGPPoint"        , AddGPPoint      ,					//캐릭터에 GPPOINT를 더해준다.
	0                   ,   0                      
};
#endif // #if !defined(__REMOVE_SCIRPT_060712)