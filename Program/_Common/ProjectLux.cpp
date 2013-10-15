// Project.cpp: implementation of the CProject class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "defineItem.h"

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#include "lang.h"

#include "Ship.h"
#ifdef __WORLDSERVER
#include "worldmng.h"
extern	CWorldMng	g_WorldMng;
#endif	// __WORLDSERVER

#include "guild.h"


#ifdef __WORLDSERVER
#include "guildquest.h"
#endif	// __WORLDSERVER

#define SYNTAX_ERROR		Error( "%s(%d) : MoverID=%d syntax error %s", szFileName, script.GetLineNum(), nVal, script.token );
#define UNIDENTIFY_ERROR	Error( "%s(%d) : MoverID=%d 지정되지 않은 디파인 error %s", szFileName, script.GetLineNum(), nVal, script.token );
#define CUSTOM_ERROR(A)		Error( "%s(%d) : MoverID=%d %s %s", szFileName, script.GetLineNum(), nVal, A, script.token );		

enum AICMD {
	AICMD_NONE = 0,
	AICMD_SCAN, 
	AICMD_ATTACK, 		// 공격명령.
	AICMD_ATK_HPCOND,	// hp비교
	AICMD_ATK_LVCOND,	// lv비교
	AICMD_RECOVERY,		// 회복
	AICMD_RANGEATTACK,	// 원거리 공격.
	AICMD_KEEP_RANGEATTACK,	// 거리유지 원거리 공격.
	AICMD_SUMMON,		// 소환
	AICMD_EVADE,		// 도망
	AICMD_HELPER,		// 도움요청
	AICMD_BERSERK,		// 광분상태
	AICMD_RANDOMTARGET,	// 랜덤한상대에게 공격
	AICMD_LOOT,			// 루팅하는 몹.
};

// AI{}중 #SCAN 블럭.
BOOL CProject::LoadPropMoverEx_AI_SCAN( LPCTSTR szFileName, CScript &script, int nVal )
{
	AICMD nCommand = AICMD_NONE;

	script.GetToken();	// {
	if( script.token[0] != '{' )	// #SCAN 후에 {가 빠졌는가.
	{
		Error( "%s(%d) : MoverID=%d SCAN 섹터블럭 { 빠졌음.", szFileName, script.GetLineNum(), nVal );
		return FALSE;
	}

	MoverProp* pProp = GetMoverProp( nVal );
	
	while(1)
	{
		script.GetToken();		// 토큰 하나 읽음,.
		if( script.token[0] == '}' )	// SCAN블럭 끝.
			break;

		if( script.tokenType == IDENTIFIER )		// 토큰이 식별자.
		{
			if( nCommand == AICMD_SCAN )		// scan명령어가 나왔었다.
			{
				if( strcmpi( script.token, "job" ) == 0 )		// 스캔 직업 
					pProp->m_nScanJob = script.GetNumber();
				else 
				if( strcmpi( script.token, "range" ) == 0 )		// 스캔 범위 
					pProp->m_nAttackFirstRange = script.GetNumber();
				else 
				if( strcmpi( script.token, "quest" ) == 0 )	// 퀘스트를 갖은 플레이어 스켄 
					pProp->m_dwScanQuestId = script.GetNumber();
				else 
				if( strcmpi( script.token, "item" ) == 0 )	// 해당 아이템을 갖은 플레이어 스켄 
					pProp->m_dwScanItemIdx = script.GetNumber();
				else
				if( strcmpi( script.token, "chao" ) == 0 )	// 해당 아이템을 갖은 플레이어 스켄 
					pProp->m_nScanChao = script.GetNumber();
				else
				{
					// 스켄 뒤에 나온 스트링이 아무것도 아니라면 문제가 있음 
					SYNTAX_ERROR;
					return FALSE;
				}
			}
			if( strcmpi( script.Token, "scan" ) == 0 )	// 명령어.
			{
				if( nCommand )	// 이미 뭔가 명령이 세팅되었는데 또 나왔다.
				{
					SYNTAX_ERROR;
					return FALSE;
				}
				nCommand =	AICMD_SCAN;	// scan명령
				//pProp->m_nAttackFirstRange = 3;
			}
		}
	}

	return TRUE;
}

// AI{}중 #BATTLE 블럭.
BOOL CProject::LoadPropMoverEx_AI_BATTLE( LPCTSTR szFileName, CScript &script, int nVal )
{
	AICMD nCommand = AICMD_NONE;

	script.GetToken();	// {
	if( script.token[0] != '{' )	// #BATTLE 후에 {가 빠졌는가.
	{
		Error( "%s(%d) : MoverID=%d BATTLE 섹터블럭 { 빠졌음.", szFileName, script.GetLineNum(), nVal );
		return FALSE;
	}

	MoverProp* pProp = GetMoverProp( nVal );
	
	while(1)
	{
		script.GetToken();		// 토큰 하나 읽음,.
		if( script.token[0] == '}' )	// BATTLE블럭 끝.
			break;

		if( script.tokenType == IDENTIFIER )		// 토큰이 식별자.
		{
			//---- Attack
			if( strcmpi( script.token, "Attack" ) == 0 )	// 일반 공격명령.
			{
				nCommand =	AICMD_ATTACK;	//	명령 "공격" 시작.
				pProp->m_bMeleeAttack = 1;	// 근접공격 AI도 있다는것.
			} else
			//--- cunning
			if( strcmpi( script.token, "cunning" ) == 0 )	// 공격명령시 레벨 비교
			{
				if( nCommand == 0 )		// cunning명령앞에는 어떤명령이든 먼저 와야한다.
				{
					SYNTAX_ERROR;
					return FALSE;
				} else
				if( nCommand == AICMD_ATTACK )		// Attack <xxx> cunning 형태로 왔다.
				{
					script.GetToken();
					if( strcmpi( script.token, "low" ) == 0 )		// 타겟이 레벨이 낮은경우 공격
						pProp->m_nLvCond = 1;
					else if( strcmpi( script.token, "Sam" ) == 0 )	// 타겟이 레벨이 같은경우.
						pProp->m_nLvCond = 2;
					else if( strcmpi( script.token, "Hi" ) == 0 )	// 타겟이 레벨이 높은경우.
						pProp->m_nLvCond = 3;
					else
					{
						SYNTAX_ERROR;
						return FALSE;
					}
				}
			} else
			//--- Recovery
			if( strcmpi( script.token, "Recovery" ) == 0 )	// 회복 명령.
			{
				nCommand = AICMD_RECOVERY;		// 명령 "회복" 시작.
				pProp->m_bRecvCond = 1;			//일단은 디폴트로 1(전투중 치료)
				pProp->m_nRecvCondMe = 0;
				pProp->m_nRecvCondHow = 100;
				pProp->m_nRecvCondMP = 0;
			} else
			//--- 대상지정. u=다른이 m=나만, a=다른이/나
			if( strcmpi( script.token, "u" ) == 0 || 
				strcmpi( script.token, "m" ) == 0 || 
				strcmpi( script.token, "a" ) == 0 )	// 대상설정
			{
				if( nCommand == 0 )		// u,m,a명령앞에는 어떤명령이든 먼저 와야한다.
				{
					SYNTAX_ERROR;
					return FALSE;
				} else
				if( nCommand == AICMD_RECOVERY )		// recovery xx xx xx (u,m,a) 형태로 왔다.
				{
//					script.GetToken();
					if( strcmpi( script.token, "u" ) == 0 )		// 대상은 다른이.
						pProp->m_bRecvCondWho = 1;
					else if( strcmpi( script.token, "m" ) == 0 )	// 대상은 나
						pProp->m_bRecvCondWho = 2;
					else if( strcmpi( script.token, "a" ) == 0 )	// 대상은 다른이,나
						pProp->m_bRecvCondWho = 3;

					pProp->m_bRecvCond = 2;			// 일단 옵션이 붙었으므로 전투/비전투 모두 힐링.
				}
			} else
			if( strcmpi( script.token, "RangeAttack" ) == 0 )	// 원거리 공격
			{
				nCommand = AICMD_RANGEATTACK;
			} else
			if( strcmpi( script.token, "KeepRangeAttack" ) == 0 )	// 거리유지 원거리 공격
			{
				nCommand = AICMD_KEEP_RANGEATTACK;
			} else
			if( strcmpi( script.token, "Summon" ) == 0 )		// 소환 명령
			{
				nCommand = AICMD_SUMMON;
			} else
			if( strcmpi( script.token, "Evade" ) == 0 )			// 도망
			{
				nCommand = AICMD_EVADE;
			} else
			if( strcmpi( script.token, "Helper" ) == 0 )		// 도움요청.
			{
				nCommand = AICMD_HELPER;
				pProp->m_tmUnitHelp = 0;			// 몇초마다 한번씩 헬퍼를 부를꺼냐. 0이면 최초한번만.
				pProp->m_nHelpRangeMul = 2;			// 디폴트는 시야반경 2배내.
				pProp->m_bHelpWho = 1;
				pProp->m_nCallHelperMax = 5;
			} else
			// helper에서 쓰이는..
			if( strcmpi( script.token, "all" ) == 0 ||
				strcmpi( script.token, "sam" ) == 0 )
			{
				if( nCommand == 0 ) // all/sam 명령앞에는 어떤명령이든 먼저 와야 한다.
				{
					SYNTAX_ERROR;
					return FALSE;
				} else
				if( nCommand == AICMD_HELPER )
				{
					if( strcmpi( script.token, "all" ) == 0 )			// 아무나.
						pProp->m_bHelpWho = 1;	
					else if( strcmpi( script.token, "sam" ) == 0 )		// 같은 종만
						pProp->m_bHelpWho = 2;
				}
			} else
			if( strcmpi( script.token, "Berserk" ) == 0 )		// 광포화.
			{
				nCommand = AICMD_BERSERK;
			} else
			if( strcmpi( script.token, "Randomtarget" ) == 0 )	
			{
			} else
//			if( strcmpi( script.token, "Loot" ) == 0 )		// 루팅. 아이템 줏어먹기.
//			{
//				nCommand = AICMD_LOOT;
//				pProp->m_nLoot = 1;
//			} else
			{
				SYNTAX_ERROR;	// 이상한 명령이 들어왔음.
				return FALSE;
			}
		} else
		/////////// 토큰이 숫자.
		if( script.tokenType == NUMBER )
		{
			if( nCommand == 0 )		// 아무 명령없이 숫자가 나왔다.
			{
				SYNTAX_ERROR;
				return FALSE;
			} else
			if( nCommand == AICMD_ATTACK )		// 공격명령이 나왔었다.
			{
				int nNum = atoi( script.token );		// 파라메터값을 숫자로 변환.
				// nNum는 상대체력 %;
				pProp->m_nHPCond = nNum;				// 선공몹일때 타겟HP가 nNum%미만이면 공격한다.
			} else
			if( nCommand == AICMD_RECOVERY )	// 회복명령
			{
				int nNum = atoi( script.token );
				if( pProp->m_nRecvCondMe == 0 )		// 첫번째 파라메터(내HP가 몇%이하인가?)
				{
					pProp->m_nRecvCondMe = nNum;
				} else
				if( pProp->m_nRecvCondHow == 100 )	// 두번째 파라메터(몇%를 힐링할껀가)
				{
					pProp->m_nRecvCondHow = nNum;
				} else
				if( pProp->m_nRecvCondMP == 0 )		// 몇%의 마나를 소모할껀가.
				{
					pProp->m_nRecvCondMP = nNum;
				}
			} else
			if( nCommand == AICMD_RANGEATTACK || nCommand == AICMD_KEEP_RANGEATTACK )	// 원거리 공격명령
			{
				int nJob = MAX_JOB;
				int nRange	= atoi( script.token );			

				if( nCommand == AICMD_KEEP_RANGEATTACK )
				{
					nRange |= 0x80;		// KEEP_RANGE는 상위 1비트 온.
					if( nJob >= MAX_JOB )		// 모든 직업.
					{
						memset( pProp->m_bRangeAttack, nRange, sizeof(pProp->m_bRangeAttack) );		// 모든직업에 같은값으로 설정.
					} else
					{
						if( nJob >0 || nJob < MAX_JOB )
							pProp->m_bRangeAttack[ nJob ] = (BYTE)nRange;		// 해당직업의 원거리 공격길이를 저장.
						else
							SYNTAX_ERROR;
					}
				} else
				{
					if( nJob >= MAX_JOB )		// 모든 직업.
					{
						memset( pProp->m_bRangeAttack, nRange, sizeof(pProp->m_bRangeAttack) );		// 모든직업에 같은값으로 설정.
					} else
					{
						if( nJob >0 || nJob < MAX_JOB )
							pProp->m_bRangeAttack[ nJob ] = (BYTE)nRange;		// 해당직업의 원거리 공격길이를 저장.
						else
							SYNTAX_ERROR;
					}
				}
			} else
			if( nCommand == AICMD_SUMMON )		// 소환명령이 나왔었다.
			{
				pProp->m_nSummProb = atoi( script.token );	// 소환 할때 성공확률
				pProp->m_nSummNum  = script.GetNumber();		// 한번에 몇마리나 소환하냐.
				if( pProp->m_nSummNum > MAX_SUMMON )
				{
					CUSTOM_ERROR( "소환할수 있는 개수가 너무 많다" );
#if __VER >= 12 // __NEW_SUMMON_RULE				
					pProp->m_nSummNum = MAX_SUMMON;
#endif // __NEW_SUMMON_RULE
				}

				script.GetToken();					// 몬스터 아이디
				if( script.tokenType != NUMBER )
					UNIDENTIFY_ERROR;
				pProp->m_nSummID = atoi( script.token );
				if( GetMoverProp( pProp->m_nSummID ) == NULL )
					CUSTOM_ERROR( "지정된 몬스터 아이디가 프로퍼티에 없음" );
			} else
			if( nCommand == AICMD_EVADE )	// 도망 명령이 나왔었다.
			{
				pProp->m_nRunawayHP = atoi( script.token );		// 몇%남으면 도망가냐?
			} else
			if( nCommand == AICMD_HELPER )	// 헬퍼 명령이 나왔었다.
			{
				int nNum = atoi( script.token );
				if( pProp->m_tmUnitHelp == 0 )		// 파라메터가 아직 지정되지 않았다면..
					pProp->m_tmUnitHelp = (DWORD)nNum;		// 몇초마다 헬퍼를 부르는 시도를 할꺼냐.
				else if( pProp->m_nHelpRangeMul == 2 )		// 파라메터가 아직 지정되지 않았다면..
					pProp->m_nHelpRangeMul = nNum;			// 시야배수
			} else
			if( nCommand == AICMD_BERSERK )	// 버서커 명령이 나왔었다.
			{
				pProp->m_nBerserkHP = atoi( script.token );		// 버서커가 되기위한 %. 0이면 버서커없음.
				pProp->m_fBerserkDmgMul = script.GetFloat();	// 버서커거되었을때 데미지 배수.
				if( pProp->m_fBerserkDmgMul <= 0 || pProp->m_fBerserkDmgMul >= 20.0f )
				{
					CString str;
					str.Format( "Berserk명령의 데미지배수의 범위가 이상한거 같은디? %f", pProp->m_fBerserkDmgMul );
					CUSTOM_ERROR( (LPCTSTR)str );
				}
			}
		}
		
	}
	
	return TRUE;
}

// AI{}중 #MOVE 블럭.
BOOL CProject::LoadPropMoverEx_AI_MOVE( LPCTSTR szFileName, CScript &script, int nVal )
{
	AICMD nCommand = AICMD_NONE;
	
	script.GetToken();	// {
	if( script.token[0] != '{' )	// #MOVE 후에 {가 빠졌는가.
	{
		Error( "%s(%d) : MoverID=%d MOVE 섹터블럭 { 빠졌음.", szFileName, script.GetLineNum(), nVal );
		return FALSE;
	}
	
	MoverProp* pProp = GetMoverProp( nVal );
	
	while(1)
	{
		script.GetToken();		// 토큰 하나 읽음,.
		if( script.token[0] == '}' )	// MOVE블럭 끝.
			break;

		if( script.tokenType == IDENTIFIER )		// 토큰이 식별자.
		{
			if( strcmpi( script.token, "Loot" ) == 0 )		// 루팅. 아이템 줏어먹기.
			{
				nCommand = AICMD_LOOT;
//				pProp->m_nLoot = 1;		// 康: 임시로 막음
				pProp->m_nLoot	= 0;
			} else
			if( strcmpi( script.token, "d" ) == 0 )		// Loot명령 옵션
			{
//				pProp->m_nLoot = 2;		// 2는 d옵션.
				pProp->m_nLoot	= 0;	// 康: 임시로 막음
			} else
			{
				SYNTAX_ERROR;	// 이상한 명령이 들어왔음.
				return FALSE;
			}
		} else
		/////////// 토큰이 숫자.
		if( script.tokenType == NUMBER )
		{
			if( nCommand == 0 )		// 아무 명령없이 숫자가 나왔다.
			{
				SYNTAX_ERROR;
				return FALSE;
			}
			else if( nCommand == AICMD_LOOT )
			{
				if( script.tokenType != NUMBER )
					UNIDENTIFY_ERROR;
				pProp->m_nLootProb = atoi( script.token );
			}
		}
	}
	
	return TRUE;
}


// propMoverEx.inc중 AI {} 블럭 처리부.
BOOL CProject::LoadPropMoverEx_AI( LPCTSTR szFileName, CScript &script, int nVal )
{
	script.GetToken();		// {

	if( script.token[0] != '{' )
	{
		Error( "%s(%d) : MoverID=%d AI 다음에 { 없음.", szFileName, script.GetLineNum(), nVal );
		return FALSE;
	}

	while(1)
	{
		script.GetToken();

		if( script.token[0] == '}' )		// 블럭 끝날때까지 읽음.
			break;

		// 섹터 블럭 시작.
		if( script.token[0] == '#' )		// #???? 블럭 나와야 함.  #SCAN으로 한꺼번에 읽힌다.
		{
//			script.GetToken();	// SCAN/BATTLE/MOVING
			if( strcmpi( script.Token, "#SCAN" ) == 0 )	// 
			{
				if( LoadPropMoverEx_AI_SCAN( szFileName, script, nVal ) == FALSE )
					return FALSE;
			} else
			if( strcmpi( script.Token, "#BATTLE" ) == 0 )	
			{
				if( LoadPropMoverEx_AI_BATTLE( szFileName, script, nVal ) == FALSE )
					return FALSE;
			} else
			if( strcmpi( script.Token, "#MOVE" ) == 0 )	
			{
				if( LoadPropMoverEx_AI_MOVE( szFileName, script, nVal ) == FALSE )
					return FALSE;
			} else
			{
				Error( "%s(%d) : MoverID=%d 섹터블럭이름이 이상함(%s) ", szFileName, script.GetLineNum(), nVal, script.Token );
				return FALSE;
			}
		} else
		{
			// 섹터블럭이 없음.
			Error( "%s(%d) : MoverID=%d 섹터블럭(<)이 없음.", szFileName, script.GetLineNum(), nVal );
			return FALSE;
		}
	}

	return TRUE;
}

