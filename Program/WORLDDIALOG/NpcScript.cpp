#include "stdafx.h"
#include "NpcScript.h"

/////////////////////////////////////////////////////////////////////////////
// extern functions
/////////////////////////////////////////////////////////////////////////////
extern		void InitFunctions();
extern		PFN GetFunction( const char* szFunction );

/////////////////////////////////////////////////////////////////////////////
// global variable
/////////////////////////////////////////////////////////////////////////////
static map< string, int >	g_mapKey;
static vector<string>		g_strings;
static int g_nLanguage;

/////////////////////////////////////////////////////////////////////////////
// global functions
/////////////////////////////////////////////////////////////////////////////
LPCTSTR		GetDialogText( DWORD dwText );
BOOL		LoadDialogText( LPCTSTR szFile );
BOOL		InitDLL( LPCTSTR szFile );
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[](p); (p)=NULL; } }

// 베트남어 처리
// 유니코드에서 1258 : [완성형 유니코드] -> [조합형 유니코드] -> [CodePage 1258 ANSI 호환코드] 
// 1258에서 유니코드 : [CodePage 1258 ANSI 호환코드] -> [조합형 유니코드] -> [완성형 유니코드] 

namespace VTN
{
	wchar_t wVTNToneChar[24][6] = {

		/*--------Precomposed characters------------Various*/
		{0x00c1, 0x00c0, 0x1ea2, 0x00c3, 0x1ea0,    0x0041},
		{0x00e1, 0x00e0, 0x1ea3, 0x00e3, 0x1ea1,    0x0061},
		{0x1eae, 0x1eb0, 0x1eb2, 0x1eb4, 0x1eb6,    0x0102},
		{0x1eaf, 0x1eb1, 0x1eb3, 0x1eb5, 0x1eb7,    0x0103},
		{0x1ea4, 0x1ea6, 0x1ea8, 0x1eaa, 0x1eac,    0x00c2},
		{0x1ea5, 0x1ea7, 0x1ea9, 0x1eab, 0x1ead,    0x00e2},
		{0x00c9, 0x00c8, 0x1eba, 0x1ebc, 0x1eb8,    0x0045},
		{0x00e9, 0x00e8, 0x1ebb, 0x1ebd, 0x1eb9,    0x0065},
		{0x1ebe, 0x1ec0, 0x1ec2, 0x1ec4, 0x1ec6,    0x00ca},
		{0x1ebf, 0x1ec1, 0x1ec3, 0x1ec5, 0x1ec7,    0x00ea},
		{0x00cd, 0x00cc, 0x1ec8, 0x0128, 0x1eca,    0x0049},
		{0x00ed, 0x00ec, 0x1ec9, 0x0129, 0x1ecb,    0x0069},
		{0x00d3, 0x00d2, 0x1ece, 0x00d5, 0x1ecc,    0x004f},
		{0x00f3, 0x00f2, 0x1ecf, 0x00f5, 0x1ecd,    0x006f},
		{0x1ed0, 0x1ed2, 0x1ed4, 0x1ed6, 0x1ed8,    0x00d4},
		{0x1ed1, 0x1ed3, 0x1ed5, 0x1ed7, 0x1ed9,    0x00f4},
		{0x1eda, 0x1edc, 0x1ede, 0x1ee0, 0x1ee2,    0x01a0},
		{0x1edb, 0x1edd, 0x1edf, 0x1ee1, 0x1ee3,    0x01a1},
		{0x00da, 0x00d9, 0x1ee6, 0x0168, 0x1ee4,    0x0055},
		{0x00fa, 0x00f9, 0x1ee7, 0x0169, 0x1ee5,    0x0075},
		{0x1ee8, 0x1eea, 0x1eec, 0x1eee, 0x1ef0,    0x01af},
		{0x1ee9, 0x1eeb, 0x1eed, 0x1eef, 0x1ef1,    0x01b0},
		{0x1ef2, 0x00dd, 0x1ef6, 0x1ef8, 0x1ef4,    0x0059},
		{0x1ef3, 0x00fd, 0x1ef7, 0x1ef9, 0x1ef5,    0x0079}
	};

	wchar_t wVTNCombiningDia[5] = {0x0301, 0x0300, 0x0309, 0x0303, 0x0323};
}

bool GetTone(wchar_t input, int* pnLetterLine, int* pnTone)
{
	using namespace VTN;

	for(int i=0; i<24; i++)
	{
		for(int j=0; j<5; j++)
		{
			if(wVTNToneChar[i][j] == input)
			{
				if(pnLetterLine != NULL && pnTone != NULL)
				{
					*pnLetterLine = i;
					*pnTone = j;
					return true;
				}
			}
		}
	}

	return false;
}

// 완성형 유니코드 -> 조합형 유니코드
void DecomposeString(wchar_t* pwSrc, wchar_t* pwDest, int nSize)
{
	using namespace VTN;

	if(pwSrc == NULL || pwDest == NULL || nSize <= 0)
		return;

	int nLetterLine;
	int nTone;

	for(int i=0; i<nSize; i++)
	{
		if(GetTone(*pwSrc, &nLetterLine, &nTone))
		{
			*pwDest = wVTNToneChar[nLetterLine][5];
			++pwDest;
			*pwDest = wVTNCombiningDia[nTone];
		}
		else
		{
			*pwDest = *pwSrc;
		}
		++pwSrc;
		++pwDest;
	}
}

int WideCharToMultiByteEx(
    UINT     CodePage,
    DWORD    dwFlags,
    LPCWSTR  lpWideCharStr,
    int      cchWideChar,
    LPSTR    lpMultiByteStr,
    int      cchMultiByte,
    LPCSTR   lpDefaultChar,
    LPBOOL   lpUsedDefaultChar)
{
	int nLength;
	if(g_nLanguage == LANG_VTN && lpMultiByteStr != NULL && cchMultiByte > 0)
	{
		int nSize = wcslen((LPCWSTR)lpWideCharStr);
		wchar_t* pwSrc = new wchar_t[nSize+1];
		wchar_t* pwDest = new wchar_t[(nSize*2)+1];
		memcpy(pwSrc, lpWideCharStr, sizeof(wchar_t)*(nSize+1));
		DecomposeString(pwSrc, pwDest, nSize+1);
		nLength = WideCharToMultiByte(CodePage, dwFlags, pwDest, cchWideChar, lpMultiByteStr, cchMultiByte, lpDefaultChar, lpUsedDefaultChar);
		SAFE_DELETE_ARRAY(pwSrc);
		SAFE_DELETE_ARRAY(pwDest);
	}
	else
		nLength = WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cchMultiByte, lpDefaultChar, lpUsedDefaultChar);

	return nLength;
}

void CNpcScript::SetLang(int nLanguage)
{
	g_nLanguage = nLanguage;
}

BOOL LoadDialogText( LPCTSTR szFile )
{
	FILE* fp = fopen( szFile, "rt" );
	if( fp == NULL )
		return FALSE;

	BOOL bOK;
	char szBuffer[1024];

	if(g_nLanguage == LANG_VTN)
	{
		int nFileLength = _filelength(fileno(fp));
		int nSize = nFileLength + 2;
		char* pProg = new char[ nSize ];
		if( !pProg )
			return FALSE;

		fseek( fp, 0, SEEK_SET );
		if( fread( pProg, nFileLength, 1, fp ) ) 
		{
			*(pProg + nFileLength ) = '\0';
			*(pProg + nFileLength + 1 ) = '\0';

			if( (BYTE)*(pProg + 0 ) == 0xff && (BYTE)*(pProg + 1 ) == 0xfe ) // is unicode ?
			{
				char* lpMultiByte = new char[ nFileLength + 2 ];
				int nResult = WideCharToMultiByteEx( 1258, 0, (LPWSTR)(pProg+2), -1, lpMultiByte, nFileLength, NULL, NULL );
				if( nResult > 0 )
				{
					lpMultiByte[nResult-1] = 0;
					char szBuffer[1024];

					int nIndex = 0;
					CHAR* pCur = lpMultiByte;
					while(*pCur != '\0')
					{
						szBuffer[nIndex] = *pCur;
						if(*pCur == '\r')
						{
							szBuffer[nIndex] = 0;
							g_strings.push_back( szBuffer );
							nIndex = 0;
							pCur += 2;
						}
						else
						{
							nIndex++;
							pCur++;
						}
					}
				}
			}

			if( ferror( fp ) )
				bOK = FALSE;
			else
				bOK = TRUE;
		}
	}
	else
	{
		for( ;; )
		{
			char* result = fgets( szBuffer, sizeof(szBuffer), fp );
			if( result )
			{
				if( szBuffer[0] != '\0' )
					szBuffer[ strlen( szBuffer ) - 1 ] = 0;
				g_strings.push_back( szBuffer );
			}
			else
			{
				if( ferror( fp ) )
					bOK = FALSE;
				else
					bOK = TRUE;
				break;
			}
		}
	}
	
	fclose( fp );
	return bOK;
}

LPCTSTR GetDialogText( DWORD dwText )
{
	LPCTSTR lpText = "";
	
	if( dwText < g_strings.size() )
		lpText = g_strings[ dwText ].c_str();
	return lpText;
}

BOOL InitDLL( LPCTSTR szFile )
{
	if( g_strings.size() > 0 )		// for single initilize
		return TRUE;

	if( LoadDialogText( szFile ) == FALSE )
		return FALSE;

	if( MakeKeyIndex() == FALSE )
		return FALSE;

	InitFunctions();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNpcScript
/////////////////////////////////////////////////////////////////////////////

void CNpcScript::DoExec( const char* szName, const char* szKey )
{
	int nIndex = GetKeyIndex( szKey );
	if( nIndex < 0 )
		return;

	char szFunction[64];
	sprintf( szFunction, "%s_%d", szName, nIndex );
	PFN pmf = GetFunction( szFunction );
	if( pmf )
	{
		(this->*pmf)();
	}
	else
	{
		char szBuffer[1024];
		sprintf( szBuffer, "%s key:%s\n", szFunction, szKey );
		OutputDebugString( szBuffer );
	}
}

BOOL CNpcScript::Find( const char* szName, const char* szKey )
{
	int nIndex = GetKeyIndex( szKey );
	if( nIndex < 0 )
		return FALSE;

	char szFunction[64];
	sprintf( szFunction, "%s_%d", szName, nIndex );
	PFN pmf = GetFunction( szFunction );
	if( pmf )
		return TRUE;
	else
		return FALSE;
}

int g_nKeys[] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,
	30,
	31,
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	41,
	42,
	43,
	865,
	866,
};
//Script Begin
void CNpcScript::dudk_drian_0()
{
	Speak( NpcId(), 44 );
	SetScriptTimer( 15 );
}
void CNpcScript::dudk_drian_1()
{
	if(GetQuestState(QUEST_DUDK_VOL1) == QS_END)
	{ 
	LaunchQuest();
	}
	else
	{
	AddKey( 9 );
	AddKey( 10 );
	}
}
void CNpcScript::dudk_drian_2()
{
	AddKey( 9 );
	if(GetItemNum(II_SYS_SYS_QUE_DRIANCARGO) == 0)
	{
	AddCondKey( 45,11 );
	}
	AddKey( 10 );
}
void CNpcScript::dudk_drian_3()
{
	Say ( 46 );
}
void CNpcScript::dudk_drian_4()
{
	if(IsSetQuest(QUEST_DUDK_VOL3) == TRUE)
	{
		if(IsSetQuest(QUEST_DUDK_VOL4) == TRUE)
		{
		Say ( 47);
		}
		else
		{
		Say ( 48);	
		}	
	}
	else 
	{
	Say ( 49 );
	}
}
void CNpcScript::dudk_drian_5()
{
}
void CNpcScript::dudk_drian_6()
{
	if(IsSetQuest(QUEST_DUDK_VOL3) == TRUE)
	{ 
		if( GetEmptyInventoryNum() < 1 )
		{
			PrintSystemMessage(TID_GAME_LACKSPACE);
			Exit();	
		}
		
		else
		{
			CreateItem(II_SYS_SYS_KEY_NEVILPRISON, 1);
		}
	}
}
void CNpcScript::dudk_drian_7()
{
}
void CNpcScript::dudk_drian_8()
{
}
void CNpcScript::dudk_drian_9()
{
	Say( 50 );
}
void CNpcScript::dudk_drian_10()
{
	Say( 51 );
	Exit();
}
void CNpcScript::dudk_drian_11()
{
	Say( 52 );
	if( GetEmptyInventoryNum() < 1 )
	{
		PrintSystemMessage(TID_GAME_LACKSPACE);
		Exit();	
	}
		
	else
	{
		CreateItem( II_SYS_SYS_QUE_DRIANCARGO, 1 );
	}
	RemoveKey(11);
}

//Script End
//Script Begin
void CNpcScript::dudk_kazen_0()
{
	Speak( NpcId(), 53 );
	SetScriptTimer( 15 );
}
void CNpcScript::dudk_kazen_1()
{

	LaunchQuest();

}
void CNpcScript::dudk_kazen_2()
{
	AddKey( 9 );
	AddKey( 10 );
}
void CNpcScript::dudk_kazen_3()
{
	Say ( 54 );
}
void CNpcScript::dudk_kazen_4()
{
	Say ( 55 );

}
void CNpcScript::dudk_kazen_5()
{
}
void CNpcScript::dudk_kazen_6()
{
}
void CNpcScript::dudk_kazen_7()
{
}
void CNpcScript::dudk_kazen_8()
{
}
void CNpcScript::dudk_kazen_9()
{
	Say( 56 );
}
void CNpcScript::dudk_kazen_10()
{
	Say( 57 );
	Exit();
}

//Script End
//Script Begin
void CNpcScript::dudk_nevil_0()
{
	Speak( NpcId(), 58 );
	SetScriptTimer( 15 );
}
void CNpcScript::dudk_nevil_1()
{
	LaunchQuest();
}
void CNpcScript::dudk_nevil_2()
{

}
void CNpcScript::dudk_nevil_3()
{
	  AddKey( 12 );
	  AddKey( 10 );	
}
void CNpcScript::dudk_nevil_4()
{
	Say ( 59 );
}
void CNpcScript::dudk_nevil_5()
{
}
void CNpcScript::dudk_nevil_6()
{
	

}
void CNpcScript::dudk_nevil_7()
{
}
void CNpcScript::dudk_nevil_8()
{
}
void CNpcScript::dudk_nevil_12()
{
	Say( 60 );
}
void CNpcScript::dudk_nevil_10()
{
	Say( 61 );
	Exit();
}
//Script End
void CNpcScript::kepe_ciel_0()
	{
		Speak( NpcId(), 62 );
		SetScriptTimer( 15 );
	}
	
void CNpcScript::kepe_ciel_1()
	{
		AddKey( 9 );
		AddKey( 10 );

		Say( 63 );
		
	}
	
void CNpcScript::kepe_ciel_9()
	{
		Say( 64 );
	}
	
void CNpcScript::kepe_ciel_10()
	{
		Speak( NpcId(), 65 );
		Exit();
	}

//Script Begin
void CNpcScript::kepe_heron_0()
{
	Speak( NpcId(), 66 );
	SetScriptTimer( 15 );
}
void CNpcScript::kepe_heron_1()
{
	LaunchQuest();
}
void CNpcScript::kepe_heron_2()
{
	//AddKey( "죄 사함" );
	AddKey( 9 );
	AddKey( 10 );
	AddKey( 13 );

}
void CNpcScript::kepe_heron_3()
{
	Say ( 67 );
}
void CNpcScript::kepe_heron_4()
{
	Say ( 68 );
}
void CNpcScript::kepe_heron_5()
{
}
void CNpcScript::kepe_heron_6()
{
}
void CNpcScript::kepe_heron_7()
{
}
void CNpcScript::kepe_heron_8()
{
}
void CNpcScript::kepe_heron_9()
{
	Say( 69 );

}
void CNpcScript::kepe_heron_13()
{
	Say( 70 );

}
void CNpcScript::kepe_heron_10()
{
	Say( 71 );
	Exit();
}

//Script End
void CNpcScript::kepe_rocbin_0()
	{
		Speak( NpcId(), 72 );
		SetScriptTimer( 15 );
	}
	
void CNpcScript::kepe_rocbin_1()
	{
		AddKey( 9 );
		AddKey( 10 );

		Say( 73 );
		
	}
	
void CNpcScript::kepe_rocbin_9()
	{
		Say( 74 );
	}
	
void CNpcScript::kepe_rocbin_10()
	{
		Speak( NpcId(), 75 );
		Exit();
	}

void CNpcScript::kepe_shun_0()
	{
		Speak( NpcId(), 76 );
		SetScriptTimer( 15 );
	}
	
void CNpcScript::kepe_shun_1()
	{
		AddKey( 9 );
		AddKey( 10 );

		Say( 77 );
		
	}
	
void CNpcScript::kepe_shun_9()
	{
		Say( 78 );
	}
	
void CNpcScript::kepe_shun_10()
	{
		Speak( NpcId(), 79 );
		Exit();
	}

void CNpcScript::kepe_yuna_0()
	{
		Speak( NpcId(), 80 );
		SetScriptTimer( 15 );
	}
	
void CNpcScript::kepe_yuna_1()
	{
		AddKey( 9 );
		AddKey( 10 );

		Say( 81 );
		
	}
	
void CNpcScript::kepe_yuna_9()
	{
		Say( 82 );
	}
	
void CNpcScript::kepe_yuna_10()
	{
		Speak( NpcId(), 65 );
		Exit();
	}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Darken General Shop Owner
// File			: MaDa_Achaben.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_achaben_0()
{
	Speak( NpcId(), 83 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_achaben_1()
{
	LaunchQuest();
}

void CNpcScript::mada_achaben_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_achaben_3()
{
	Say( 84 );
}

void CNpcScript::mada_achaben_4()
{
	Say( 85 );
}

void CNpcScript::mada_achaben_5()
{
	
}

void CNpcScript::mada_achaben_6()
{
	
}

void CNpcScript::mada_achaben_8()
{
	
}

void CNpcScript::mada_achaben_9()
{
	Say( 86 );	
}

void CNpcScript::mada_achaben_10()
{
	Speak( NpcId(), 87 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Darken General Shop Owner
// File			: MaDa_Achaben.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_almani_0()
{
	Speak( NpcId(), 88 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_almani_1()
{
	LaunchQuest();
}

void CNpcScript::mada_almani_2()
{
	AddKey( 9 );
	AddKey( 10 );
	AddKey( 14 );
        AddKey( 15 );		
}

void CNpcScript::mada_almani_3()
{
	Say( 89 );
}

void CNpcScript::mada_almani_4()
{
	Say( 90 );
}

void CNpcScript::mada_almani_5()
{
	
}

void CNpcScript::mada_almani_6()
{
	
}

void CNpcScript::mada_almani_8()
{
	
}

void CNpcScript::mada_almani_9()
{
	Say( 91 );	
}

void CNpcScript::mada_almani_14()
{
	Say( 92 );
        Say( 93 ); 
	Say( 94 );
	Say( 95 );
        Say( 96 );
        Say( 97 );
}

void CNpcScript::mada_almani_15()
{
	Say(98);
	Say(99);
        Say(100);
        Say(101);
        Say(102);
	Say(103);
        Say(104); 	
}

void CNpcScript::mada_almani_10()
{
	Speak( NpcId(), 105 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Erons Factory Guard Manager
// File			: MaDa_Amadolka.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_amadolka_0()
{
	Speak( NpcId(), 106 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_amadolka_1()
{
	LaunchQuest();
}

void CNpcScript::mada_amadolka_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_amadolka_3()
{
	Say( 107 );
}

void CNpcScript::mada_amadolka_4()
{
	Say( 108 );
}

void CNpcScript::mada_amadolka_5()
{
	
}

void CNpcScript::mada_amadolka_6()
{
	
}

void CNpcScript::mada_amadolka_8()
{
	
}

void CNpcScript::mada_amadolka_9()
{
	Say( 109 );	
}

void CNpcScript::mada_amadolka_10()
{
	Speak( NpcId(), 110 );
	Exit();
}

//Script End


///Script Begin
void CNpcScript::mada_andre_0()
{
	Speak( NpcId(), 111 );
	SetScriptTimer( 15 );
}
void CNpcScript::mada_andre_1()
{
	LaunchQuest();
}
void CNpcScript::mada_andre_2()
{
	AddKey( 12 );
	AddKey( 10 );
}
void CNpcScript::mada_andre_3()
{

}
void CNpcScript::mada_andre_4()
{
	Say ( 112 );
}
void CNpcScript::mada_andre_5()
{
}
void CNpcScript::mada_andre_6()
{
	

}
void CNpcScript::mada_andre_7()
{
}
void CNpcScript::mada_andre_8()
{

}
void CNpcScript::mada_andre_12()
{
	Say( 113 );

}
void CNpcScript::mada_andre_10()
{
	Say( 114 );
	Exit();
}


//Script End
//Script Begin
void CNpcScript::mada_bernard_0()
{
	Speak( NpcId(), 58 );
	SetScriptTimer( 15 );
}
void CNpcScript::mada_bernard_1()
{
	LaunchQuest();
}
void CNpcScript::mada_bernard_2()
{
	  AddKey( 12 );
	  AddCondKey( 115,16 );
	  AddKey( 10 );	
}
void CNpcScript::mada_bernard_3()
{
	Say ( 116 );
}
void CNpcScript::mada_bernard_4()
{
	Say ( 117 );
}
void CNpcScript::mada_bernard_5()
{
}
void CNpcScript::mada_bernard_6()
{
	if(IsSetQuest(QUEST_DUDK_VOL6) == TRUE)
	{ 
		if( GetEmptyInventoryNum() < 1)
		{
			PrintSystemMessage(TID_GAME_LACKSPACE);
			Exit();
		}
	
		else
		{
			CreateItem(II_SYS_SYS_QUE_WAKEROACHWATER, 1);
		}
	}	

}
void CNpcScript::mada_bernard_7()
{
}
void CNpcScript::mada_bernard_8()
{
}
void CNpcScript::mada_bernard_12()
{
	Say( 118 );
}
void CNpcScript::mada_bernard_10()
{
	Say( 119 );
	Exit();
}
void CNpcScript::mada_bernard_16()
{
	Say( 120 );
	//if( GetEmptyInventoryNum() < 1)
	//{
	//	PrintSystemMessage(TID_GAME_LACKSPACE);
	//	Exit();
	//}
	//
	//else
	//{
	//	CreateItem( II_SYS_SYS_QUE_DRIANCARGO, 1 );
	//}
	//Exit();
}
//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Darken Food Shop Owner
// File			: MaDa_Bolpor.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_bolpor_0()
{
	Speak( NpcId(), 121 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_bolpor_1()
{
	LaunchQuest();
}

void CNpcScript::mada_bolpor_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_bolpor_3()
{
	Say( 122 );
}

void CNpcScript::mada_bolpor_4()
{
	Say( 123 );
}

void CNpcScript::mada_bolpor_5()
{
	
}

void CNpcScript::mada_bolpor_6()
{
	
}

void CNpcScript::mada_bolpor_8()
{
	
}

void CNpcScript::mada_bolpor_9()
{
	Say( 124 );	
}

void CNpcScript::mada_bolpor_10()
{
	Speak( NpcId(), 125 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Knight Guardian
// File			: MaDa_Boneper.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_boneper_0()
{
	Speak( NpcId(), 126 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_boneper_1()
{
	LaunchQuest();
}

void CNpcScript::mada_boneper_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_boneper_3()
{
	Say( 107 );
}

void CNpcScript::mada_boneper_4()
{
	Say( 127 );
}

void CNpcScript::mada_boneper_5()
{
	
}

void CNpcScript::mada_boneper_6()
{
	
}

void CNpcScript::mada_boneper_8()
{
	if( GetQuestState(QUEST_HEROKNI_TRN4) == QS_END && GetPlayerJob() == 1 && GetPlayerLvl() == 60 )
	{
		ChangeJob( 6 );
	}
	else
	{
		Exit();
	}
}

void CNpcScript::mada_boneper_9()
{
	Say( 128 );	
}

void CNpcScript::mada_boneper_10()
{
	Speak( NpcId(), 129 );
	Exit();
}

//Script End



//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Billposter Guardian
// File			: MaDa_Capafe.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_capafe_0()
{
	Speak( NpcId(), 130 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_capafe_1()
{
	LaunchQuest();
}

void CNpcScript::mada_capafe_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_capafe_3()
{
	Say( 107 );
}

void CNpcScript::mada_capafe_4()
{
	Say( 131 );
}

void CNpcScript::mada_capafe_5()
{
	
}

void CNpcScript::mada_capafe_6()
{
	
}

void CNpcScript::mada_capafe_8()
{
	if( GetQuestState(QUEST_HEROBIL_TRN4) == QS_END && GetPlayerJob() == 3 && GetPlayerLvl() == 60 )
	{
		ChangeJob( 11 );
	}
	else
	{
		Exit();
	}
}

void CNpcScript::mada_capafe_9()
{
	Say( 132 );	
}

void CNpcScript::mada_capafe_10()
{
	Speak( NpcId(), 133 );
	Exit();
}

//Script End

///Script Begin
void CNpcScript::mada_cell_0()
{
	Speak( NpcId(), 134 );
	SetScriptTimer( 15 );
}
void CNpcScript::mada_cell_1()
{
	LaunchQuest();
}
void CNpcScript::mada_cell_2()
{
	AddKey( 12 );
	AddKey( 10 );
}
void CNpcScript::mada_cell_3()
{

}
void CNpcScript::mada_cell_4()
{
	Say ( 135 );
}
void CNpcScript::mada_cell_5()
{
}
void CNpcScript::mada_cell_6()
{
	

}
void CNpcScript::mada_cell_7()
{
}
void CNpcScript::mada_cell_8()
{
	RemoveAllItem(II_SYS_SYS_QUE_LETTERRANKASHU);
}
void CNpcScript::mada_cell_12()
{
	Say( 136 );
	AddKey( 17 );
}
void CNpcScript::mada_cell_10()
{
	Say( 137 );
	Exit();
}
void CNpcScript::mada_cell_17()
{
	Say( 138 );
	Say( 139 );
	Say( 140 );
	RemoveKey( 17 );
	//LaunchQuest();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Nomads Armor Merchant
// File			: MaDa_Chenbing.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_chenbing_0()
{
	Speak( NpcId(), 141 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_chenbing_1()
{
	LaunchQuest();
}

void CNpcScript::mada_chenbing_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_chenbing_3()
{
	Say( 142 );
}

void CNpcScript::mada_chenbing_4()
{
	Say( 143 );
}

void CNpcScript::mada_chenbing_5()
{
	
}

void CNpcScript::mada_chenbing_6()
{
	
}

void CNpcScript::mada_chenbing_8()
{
	
}

void CNpcScript::mada_chenbing_9()
{
	Say( 144 );	
}

void CNpcScript::mada_chenbing_10()
{
	Speak( NpcId(), 145 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: ClockWork's Cage Guardian
// File			: MaDa_CloneEsylop.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_cloneeshylop_0()
{
	
}

void CNpcScript::mada_cloneeshylop_1()
{
	if( GetPlayerLvl() >= 70  && IsWormonServer() == TRUE && GetGuildQuestState( QUEST_WARMON_LV1 ) == QS_END )
	{
	 	Say( 146 );		
	 	AddKey( 18 );	
	}

	else
	{		
		LaunchQuest();
	}
}

void CNpcScript::mada_cloneeshylop_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_cloneeshylop_3()
{
	Say( 147 );
}

void CNpcScript::mada_cloneeshylop_4()
{
	Say( 148 );
}

void CNpcScript::mada_cloneeshylop_5()
{
	
}

void CNpcScript::mada_cloneeshylop_6()
{
	
}

void CNpcScript::mada_cloneeshylop_8()
{
	
}

void CNpcScript::mada_cloneeshylop_9()
{
	Say( 149 );	
}

void CNpcScript::mada_cloneeshylop_18()
{
	Replace( WI_WORLD_MADRIGAL, 3780.0, 58.0, 4466.0 );		
}

void CNpcScript::mada_cloneeshylop_10()
{
	Speak( NpcId(), 150 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Outcast Troupe Member of Pepoview
// File			: MaDa_Colar.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_colar_0()
{
	Speak( NpcId(), 151 );
	SetScriptTimer( 15 );
}
	
void CNpcScript::mada_colar_1()
{
	LaunchQuest();
}

void CNpcScript::mada_colar_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_colar_3()
{
	Say ( 107 );
}

void CNpcScript::mada_colar_4()
{
	Say( 152 );
}

void CNpcScript::mada_colar_5()
{

}

void CNpcScript::mada_colar_6()
{
	
}

void CNpcScript::mada_colar_7()
{
	
}

void CNpcScript::mada_colar_8()
{
	
}

void CNpcScript::mada_colar_9()
{
	Say( 153 );
}

void CNpcScript::mada_colar_10()
{
	Speak( NpcId(), 154 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Elementor Priestess
// File			: MaDa_Condram.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_condram_0()
{
	Speak( NpcId(), 155 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_condram_1()
{
	LaunchQuest();
}

void CNpcScript::mada_condram_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_condram_3()
{
	Say( 107 );
}

void CNpcScript::mada_condram_4()
{
	Say( 156 );
}

void CNpcScript::mada_condram_5()
{
	
}

void CNpcScript::mada_condram_6()
{
	
}

void CNpcScript::mada_condram_8()
{
	
}

void CNpcScript::mada_condram_9()
{
	Say( 157 );	
}

void CNpcScript::mada_condram_10()
{
	Speak( NpcId(), 158 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Blade Master
// File			: MaDa_Corel.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_corel_0()
{
	Speak( NpcId(), 159 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_corel_1()
{
	if( GetQuestState( QUEST_HEROKNI_TRN1 ) >= 0 )
	{
		SayQuest( QUEST_HEROBLA_TRN1, QSAY_EXTRA01 );
		AddKey( 10 );
	}
		
	else
	{ 
		LaunchQuest();
	}
}

void CNpcScript::mada_corel_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_corel_3()
{
	Say( 107 );
}

void CNpcScript::mada_corel_4()
{
	Say( 160 );
}

void CNpcScript::mada_corel_5()
{
	
}

void CNpcScript::mada_corel_6()
{
	
}

void CNpcScript::mada_corel_8()
{
	
}

void CNpcScript::mada_corel_9()
{
	Say( 161 );	
}

void CNpcScript::mada_corel_10()
{
	Speak( NpcId(), 162 );
	Exit();
}

//Script End




//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: PsyChiper Master
// File			: MaDa_Cylor.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_cylor_0()
{
	Speak( NpcId(), 163 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_cylor_1()
{
	if( GetQuestState( QUEST_HEROELE_TRN1 ) >= 0 )
	{
		SayQuest( QUEST_HEROPSY_TRN1, QSAY_EXTRA01 );
		AddKey( 10 );
	}
		
	else
	{ 
		LaunchQuest();
	}
}

void CNpcScript::mada_cylor_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_cylor_3()
{
	Say( 107 );
}

void CNpcScript::mada_cylor_4()
{
	Say( 164 );
}

void CNpcScript::mada_cylor_5()
{
	
}

void CNpcScript::mada_cylor_6()
{
	
}

void CNpcScript::mada_cylor_8()
{
	
}

void CNpcScript::mada_cylor_9()
{
	Say( 165 );	
}

void CNpcScript::mada_cylor_10()
{
	Speak( NpcId(), 166 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Madrigal Bionomics Investigator 
// File			: MaDa_DrEst.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_drest_0()
{
	Speak( NpcId(), 167 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_drest_1()
{
	LaunchQuest();
}

void CNpcScript::mada_drest_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_drest_3()
{
	Say( 107 );
}

void CNpcScript::mada_drest_4()
{
	Say( 168 );
}

void CNpcScript::mada_drest_5()
{
	
}

void CNpcScript::mada_drest_6()
{
	
}

void CNpcScript::mada_drest_8()
{
	
}

void CNpcScript::mada_drest_9()
{
	Say( 169 );	
}

void CNpcScript::mada_drest_10()
{
	Speak( NpcId(), 170 );
	Exit();
}

//Script End

//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040418_004_v1
// Title    : Vocation Master( 2nd ) 
// Link NPC : Madrigal Darkon Eliff
// File Name: MaDa_Eliff.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mada_eliff_0()
{
	Speak( NpcId(), 171 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_eliff_1()
{
	if( GetQuestState( QUEST_HEROJES_TRN1 ) >= 0 || GetQuestState( QUEST2_HEROSTORY ) >= 0 )
	{
		SayQuest( QUEST_HERORAN_TRN1, QSAY_EXTRA01 );
		AddKey( 10 );
	}
		
	else
	{ 
		LaunchQuest();
	}
}

void CNpcScript::mada_eliff_2()
{

	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_eliff_3()
{
	Say ( 172 );
}

void CNpcScript::mada_eliff_4()
{
	Say ( 173 );
}

void CNpcScript::mada_eliff_5()
{

}

void CNpcScript::mada_eliff_6()
{

}

void CNpcScript::mada_eliff_7()
{

}

void CNpcScript::mada_eliff_8()
{

}

void CNpcScript::mada_eliff_9()
{
	Say ( 174 );
}

void CNpcScript::mada_eliff_10()
{
	Speak( NpcId(), 175 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Ringmaster Master
// File			: MaDa_Ellend.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_ellend_0()
{
	Speak( NpcId(), 176 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_ellend_1()
{
	if( GetQuestState( QUEST_HEROBIL_TRN1 ) >= 0 )
	{
		SayQuest( QUEST_HERORIN_TRN1, QSAY_EXTRA01 );
		AddKey( 10 );
	}
		
	else
	{ 
		LaunchQuest();
	}
}

void CNpcScript::mada_ellend_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_ellend_3()
{
	Say( 107 );
}

void CNpcScript::mada_ellend_4()
{
	Say( 177 );
}

void CNpcScript::mada_ellend_5()
{
	
}

void CNpcScript::mada_ellend_6()
{
	
}

void CNpcScript::mada_ellend_8()
{
	
}

void CNpcScript::mada_ellend_9()
{
	Say( 178 );	
}

void CNpcScript::mada_ellend_10()
{
	Speak( NpcId(), 179 );
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: ClockWork's Cage Guardian
// File			: MaDa_Eshylop.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_eshylop_0()
{
	Speak( NpcId(), 180 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_eshylop_1()
{
	if( GetPlayerLvl() >= 70 && IsWormonServer() == TRUE && IsGuild() == 1 && IsGuildMaster() == 1 )
	{			
		if( IsGuildQuest( QUEST_WARMON_LV1 ) == FALSE && GetQuestState( QUEST_ABILITYYE ) == QS_END )
		{
			Say( 181 ); 
		}
			
		else if( IsGuildQuest( QUEST_WARMON_LV1 ) == TRUE && (GetGuildQuestState( QUEST_WARMON_LV1 ) == QS_BEGIN || GetGuildQuestState( QUEST_WARMON_LV1 ) == 1 ) )
		{
			Say( 182 );
		}
               
		else if( GetGuildQuestState( QUEST_WARMON_LV1 ) == QS_END )
		{
			Say( 183 );
		}

		else
		{	
			LaunchQuest();
	        }
	}

	else
	{		
		LaunchQuest();
	}
}

void CNpcScript::mada_eshylop_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_eshylop_3()
{
	Say( 147 );
}

void CNpcScript::mada_eshylop_4()
{
	Say( 148 );
}

void CNpcScript::mada_eshylop_5()
{
	
}

void CNpcScript::mada_eshylop_6()
{
	
}

void CNpcScript::mada_eshylop_8()
{
	
}

void CNpcScript::mada_eshylop_9()
{
	Say( 149 );	
}

void CNpcScript::mada_eshylop_19()
{
	Say( 184 );
}

void CNpcScript::mada_eshylop_20()
{
	Say( 185 );
}

void CNpcScript::mada_eshylop_21()
{
	Say( 186 );	
}

void CNpcScript::mada_eshylop_22()
{
	if( IsGuildQuest( QUEST_WARMON_LV1 ) == FALSE || GetGuildQuestState( QUEST_WARMON_LV1 ) == QS_BEGIN || GetGuildQuestState( QUEST_WARMON_LV1 ) == 1 )
	{
		if( MonHuntStart( QUEST_WARMON_LV1, QS_BEGIN, QS_END, 1 ) == FALSE )
		{
			Say( 187 );
			
		}

		else if( MonHuntStart( QUEST_WARMON_LV1, QS_BEGIN, QS_END, 1 ) == TRUE )
		{
			
		}
			
		else
		{
			Say( 188 );
		}
	}	
}

void CNpcScript::mada_eshylop_23()
{
	Say( 189 );
}

void CNpcScript::mada_eshylop_10()
{
	Speak( NpcId(), 150 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Billposter Priestess
// File			: MaDa_Fera.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_fera_0()
{
	Speak( NpcId(), 190 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_fera_1()
{
	LaunchQuest();
}

void CNpcScript::mada_fera_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_fera_3()
{
	Say( 107 );
}

void CNpcScript::mada_fera_4()
{
	Say( 191 );
}

void CNpcScript::mada_fera_5()
{
	
}

void CNpcScript::mada_fera_6()
{
	
}

void CNpcScript::mada_fera_8()
{
	
}

void CNpcScript::mada_fera_9()
{
	Say( 192 );	
}

void CNpcScript::mada_fera_10()
{
	Speak( NpcId(), 193 );
	Exit();
}

//Script End

void CNpcScript::mada_furan_0()
{
	Speak( NpcId(), 194 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_furan_1()
{
	LaunchQuest();
}

void CNpcScript::mada_furan_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_furan_3()
{
	Say( 107 );
}

void CNpcScript::mada_furan_4()
{
	Say( 195 );
}

void CNpcScript::mada_furan_5()
{

}

void CNpcScript::mada_furan_6()
{
	
}

void CNpcScript::mada_furan_7()
{

}

void CNpcScript::mada_furan_8()
{

}

void CNpcScript::mada_furan_9()
{
	Say( 196 );
}

void CNpcScript::mada_furan_10()
{
	Speak( NpcId(), 197 );
	Exit();
}
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: kyk1236
// Title		: Guild Combat Shop
// File			: MaSa_Guildcombatshop.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_guildcombatshop_0()
{
	Speak( NpcId(), 198 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_guildcombatshop_1()
{
	LaunchQuest();
}

void CNpcScript::mada_guildcombatshop_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_guildcombatshop_3()
{
	
}

void CNpcScript::mada_guildcombatshop_4()
{
	Say( 199 );
}

void CNpcScript::mada_guildcombatshop_5()
{

}

void CNpcScript::mada_guildcombatshop_6()
{

}

void CNpcScript::mada_guildcombatshop_7()
{

}

void CNpcScript::mada_guildcombatshop_8()
{

}

void CNpcScript::mada_guildcombatshop_9()
{
	Say( 200 );
}

void CNpcScript::mada_guildcombatshop_10()
{
	Speak( NpcId(), 201 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Madrigal History Scholar
// File			: MaDa_Hachal.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_hachal_0()
{
	Speak( NpcId(), 202 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_hachal_1()
{
	LaunchQuest();
}

void CNpcScript::mada_hachal_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_hachal_3()
{
	Say( 107 );
}

void CNpcScript::mada_hachal_4()
{
	Say( 203 );
}

void CNpcScript::mada_hachal_5()
{
	
}

void CNpcScript::mada_hachal_6()
{
	
}

void CNpcScript::mada_hachal_8()
{
	
}

void CNpcScript::mada_hachal_9()
{
	Say( 204 );	
}

void CNpcScript::mada_hachal_10()
{
	Speak( NpcId(), 205 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Darken City Magic Shop Owner
// File			: MaDa_Haven.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_haven_0()
{
	Speak(NpcId(), 848 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_haven_1()
{
	LaunchQuest();
}

void CNpcScript::mada_haven_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_haven_3()
{
	Say( 849 );
}

void CNpcScript::mada_haven_4()
{
	Say( 850 );
}

void CNpcScript::mada_haven_5()
{
	
}

void CNpcScript::mada_haven_6()
{
	
}

void CNpcScript::mada_haven_8()
{
	
}

void CNpcScript::mada_haven_9()
{
	Say( 851 );
}

void CNpcScript::mada_haven_10()
{
	Speak( NpcId(), 852 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Ringmaster Priestess
// File			: MaDa_Heedeou.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_heedan_0()
{
	Speak( NpcId(), 239 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_heedan_1()
{
	LaunchQuest();
}

void CNpcScript::mada_heedan_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_heedan_3()
{
	Say( 107 );
}

void CNpcScript::mada_heedan_4()
{
	Say( 240 );
}

void CNpcScript::mada_heedan_5()
{
	
}

void CNpcScript::mada_heedan_6()
{
	
}

void CNpcScript::mada_heedan_8()
{
	
}

void CNpcScript::mada_heedan_9()
{
	Say( 241 );	
}

void CNpcScript::mada_heedan_10()
{
	Speak( NpcId(), 242 );
	Exit();
}

//Script End

//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040418_009_v1
// Title    : Hero Guardian 
// Link NPC : Madrigal Darkon Heingard
// File Name: MaDa_Heingard.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mada_heingard_0()
{
	Speak( NpcId(), 243 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_heingard_1()
{
	LaunchQuest();
}

void CNpcScript::mada_heingard_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_heingard_3()
{
	Say ( 244 );
}

void CNpcScript::mada_heingard_4()
{
	if( GetQuestState( QUEST_HEROJES_TRN4 ) == QS_END && GetPlayerJob() == 2 && GetPlayerLvl() == 60  )
	{
		
		ChangeJob( 8 );
	}
	
	else
	{
		Say ( 245 );
	}
}

void CNpcScript::mada_heingard_5()
{

}

void CNpcScript::mada_heingard_6()
{
	
}

void CNpcScript::mada_heingard_7()
{

}

void CNpcScript::mada_heingard_8()
{
	if( GetQuestState( QUEST_HEROJES_TRN4 ) == QS_END && GetPlayerJob() == 2 && GetPlayerLvl() == 60  )
	{
		
		ChangeJob( 8 );
	}
	
	else
	{
		Exit();
	}
}

void CNpcScript::mada_heingard_9()
{
	Say ( 246 );
	AddAnswer( 10 );
}

void CNpcScript::mada_heingard_10()
{
	Speak( NpcId(), 247 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( 20051018 Modyfied )
// Author		: Jimmie
// Title		: Acrobat Traning Assist
// File			: MaDa_Hent.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_hent_0()
{
	Speak( NpcId(), 248 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_hent_1()
{
	//추가(20051018)
	if( GetQuestState( QUEST_VOCMER_BFTRN ) >= 0 || GetQuestState( QUEST_VOCASS_BFTRN ) >= 0 || GetQuestState( QUEST_VOCMAG_BFTRN ) >= 0)
	{
		if( GetQuestState( QUEST_SCE_MDRIGALTEACHER4 ) >= 0 )
			LaunchQuest();
		else
			SayQuest( QUEST_VOCACR_BFTRN, QSAY_EXTRA01 );
		AddKey( 9 );
		AddKey( 10 );
	}

	else
	{
		LaunchQuest();
	}
}

void CNpcScript::mada_hent_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_hent_3()
{
	Say( 249 );
}

void CNpcScript::mada_hent_4()
{
	Say( 250 );
}

void CNpcScript::mada_hent_5()
{

}

void CNpcScript::mada_hent_6()
{
	//새로번역(20060104)
	//Say( "아쉽게도, 이제 내가 가지고 있는 블링크윙은 모두 소진되었다 제군. 대신 시간을 더 늘렸지. 블링크윙은 구입하여 쓰도록, 이상!" );
	if( GetQuestState(QUEST_VOCACR_BFTRN) == QS_BEGIN )
	{
		Replace( WI_WORLD_MADRIGAL, 7161.0, 100.0, 3264.0 );
	}
}

void CNpcScript::mada_hent_7()
{

}

void CNpcScript::mada_hent_8()
{

}

void CNpcScript::mada_hent_9()
{
	Say( 251 );
}

void CNpcScript::mada_hent_10()
{
	Speak( NpcId(), 252 );
	Exit();
}

//Script End
//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040418_005_v1
// Title    : Jester Priestess 
// Link NPC : Madrigal Darkon Homeit
// File Name: MaDa_Homeit.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mada_homeit_0()
{
	Speak( NpcId(), 253 );
	SetScriptTimer( 12 );
}

void CNpcScript::mada_homeit_1()
{
	LaunchQuest();
}

void CNpcScript::mada_homeit_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_homeit_3()
{
	Say ( 254 );
}

void CNpcScript::mada_homeit_4()
{
	Say ( 255 );
}

void CNpcScript::mada_homeit_5()
{

}

void CNpcScript::mada_homeit_6()
{

}

void CNpcScript::mada_homeit_7()
{

}

void CNpcScript::mada_homeit_8()
{
	
}

void CNpcScript::mada_homeit_9()
{
	Say ( 256 );
}

void CNpcScript::mada_homeit_10()
{
	Speak( NpcId(), 257 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Elementor Master
// File			: MaDa_Horison.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_horison_0()
{
	Speak( NpcId(), 258 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_horison_1()
{
	if( GetQuestState( QUEST_HEROPSY_TRN1 ) >= 0 )
	{
		SayQuest( QUEST_HEROELE_TRN1, QSAY_EXTRA01 );
		AddKey( 10 );
	}
		
	else
	{ 
		LaunchQuest();
	}
}

void CNpcScript::mada_horison_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_horison_3()
{
	Say( 107 );
}

void CNpcScript::mada_horison_4()
{
	Say( 259 );
}

void CNpcScript::mada_horison_5()
{
	
}

void CNpcScript::mada_horison_6()
{
	
}

void CNpcScript::mada_horison_8()
{
	
}

void CNpcScript::mada_horison_9()
{
	Say( 260 );	
}

void CNpcScript::mada_horison_10()
{
	Speak( NpcId(), 261 );
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Blade Priestess
// File			: MaDa_Jeperdy.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_jeperdy_0()
{
	Speak( NpcId(), 262 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_jeperdy_1()
{
	LaunchQuest();
}

void CNpcScript::mada_jeperdy_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_jeperdy_3()
{
	Say( 107 );
}

void CNpcScript::mada_jeperdy_4()
{
	Say( 263 );
}

void CNpcScript::mada_jeperdy_5()
{
	
}

void CNpcScript::mada_jeperdy_6()
{
	
}

void CNpcScript::mada_jeperdy_8()
{
	
}

void CNpcScript::mada_jeperdy_9()
{
	Say( 264 );	
}

void CNpcScript::mada_jeperdy_10()
{
	Speak( NpcId(), 265 );
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Nomads Food Merchant
// File			: MaDa_Jinitte.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_jinitte_0()
{
	Speak( NpcId(), 266 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_jinitte_1()
{
	LaunchQuest();
}

void CNpcScript::mada_jinitte_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_jinitte_3()
{
	Say( 142 );
}

void CNpcScript::mada_jinitte_4()
{
	Say( 267 );
}

void CNpcScript::mada_jinitte_5()
{
	
}

void CNpcScript::mada_jinitte_6()
{
	
}

void CNpcScript::mada_jinitte_8()
{
	
}

void CNpcScript::mada_jinitte_9()
{
	Say( 268 );	
}

void CNpcScript::mada_jinitte_10()
{
	Speak( NpcId(), 145 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Nomads Weapon Merchant
// File			: MaDa_Kablloc.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_kablloc_0()
{
	Speak( NpcId(), 269 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_kablloc_1()
{
	LaunchQuest();
}

void CNpcScript::mada_kablloc_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_kablloc_3()
{
	Say( 142 );
}

void CNpcScript::mada_kablloc_4()
{
	Say( 143 );
}

void CNpcScript::mada_kablloc_5()
{
	
}

void CNpcScript::mada_kablloc_6()
{
	
}

void CNpcScript::mada_kablloc_8()
{
	
}

void CNpcScript::mada_kablloc_9()
{
	Say( 270 );	
}

void CNpcScript::mada_kablloc_10()
{
	Speak( NpcId(), 145 );
	Exit();
}

//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040418_010_v1
// Title    : Hero Guardian 
// Link NPC : Madrigal Darkon Kailreard
// File Name: MaDa_Kailreard.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mada_kailreard_0()
{
	Speak( NpcId(), 271 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_kailreard_1()
{
	LaunchQuest();
}

void CNpcScript::mada_kailreard_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_kailreard_3()
{
	Say ( 244 );
}

void CNpcScript::mada_kailreard_4()
{
	if( GetQuestState( QUEST_HERORAN_TRN4 ) == QS_END && GetPlayerJob() == 2 && GetPlayerLvl() == 60  )
	{
		ChangeJob( 9 );
	}

	else
	{
		Say ( 272 );
	}
}

void CNpcScript::mada_kailreard_5()
{

}

void CNpcScript::mada_kailreard_6()
{
	
}

void CNpcScript::mada_kailreard_7()
{

}

void CNpcScript::mada_kailreard_8()
{
	if( GetQuestState( QUEST_HERORAN_TRN4 ) == QS_END && GetPlayerJob() == 2 && GetPlayerLvl() == 60  )
	{
		ChangeJob( 9 );
	}

	else
	{
		Exit();
	}
}

void CNpcScript::mada_kailreard_9()
{
	Say ( 273 );
}

void CNpcScript::mada_kailreard_10()
{
	Speak( NpcId(), 274 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Knight Master
// File			: MaDa_Karanduru.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_karanduru_0()
{
	Speak( NpcId(), 275 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_karanduru_1()
{
	if( GetQuestState( QUEST_HEROBLA_TRN1 ) >= 0 )
	{
		SayQuest( QUEST_HEROKNI_TRN1, QSAY_EXTRA01 );
		AddKey( 10 );
	}
		
	else
	{ 
		LaunchQuest();
	}
}

void CNpcScript::mada_karanduru_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_karanduru_3()
{
	Say( 107 );
}

void CNpcScript::mada_karanduru_4()
{
	Say( 276 );
}

void CNpcScript::mada_karanduru_5()
{
	
}

void CNpcScript::mada_karanduru_6()
{
	
}

void CNpcScript::mada_karanduru_8()
{
	
}

void CNpcScript::mada_karanduru_9()
{
	Say( 277 );	
}

void CNpcScript::mada_karanduru_10()
{
	Speak( NpcId(), 278 );
	Exit();
}

//Script End


//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040422_003_v1
// Title    : Darkon Shield Merchant 
// Link NPC : Madrigal Darkon Krinton
// File Name: MaDa_Krinton.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mada_krinton_0()
{
	Speak( NpcId(), 279 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_krinton_1()
{
	LaunchQuest();
}

void CNpcScript::mada_krinton_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_krinton_3()
{
	
}

void CNpcScript::mada_krinton_4()
{
	Say( 280 );
}

void CNpcScript::mada_krinton_5()
{

}

void CNpcScript::mada_krinton_6()
{

}

void CNpcScript::mada_krinton_7()
{

}

void CNpcScript::mada_krinton_8()
{

}

void CNpcScript::mada_krinton_9()
{
	Say( 281 );
	AddAnswer( 10 );
}

void CNpcScript::mada_krinton_10()
{
	Speak( NpcId(), 282 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: ArcMagicion
// File			: MaDa_Laloa.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_laloa_0()
{
	Speak( NpcId(), 283 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_laloa_1()
{
	LaunchQuest();
}

void CNpcScript::mada_laloa_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_laloa_3()
{
	Say( 107 );
}

void CNpcScript::mada_laloa_4()
{
	Say( 284 );
}

void CNpcScript::mada_laloa_5()
{
	
}

void CNpcScript::mada_laloa_6()
{
	
}

void CNpcScript::mada_laloa_8()
{
	
}

void CNpcScript::mada_laloa_9()
{
	Say( 285 );	
}

void CNpcScript::mada_laloa_10()
{
	Speak( NpcId(), 286 );
	Exit();
}

//Script End

//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040418_008_v1
// Title    : Hi-Dwarpet
// Link NPC : Madrigal Darkon Liekyen
// File Name: MaDa_Liekyen.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//
//Script Begin
void CNpcScript::mada_liekyen_0()
{
	Speak( NpcId(), 287 );
	SetScriptTimer( 15 );
}
	
void CNpcScript::mada_liekyen_1()
{
	LaunchQuest();
}

void CNpcScript::mada_liekyen_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_liekyen_3()
{
	Say ( 288 );
}

void CNpcScript::mada_liekyen_4()
{
	Say ( 289 );
}

void CNpcScript::mada_liekyen_5()
{

}

void CNpcScript::mada_liekyen_6()
{
	
}

void CNpcScript::mada_liekyen_7()
{

}

void CNpcScript::mada_liekyen_8()
{
	
}

void CNpcScript::mada_liekyen_9()
{
	Say( 290 );
}

void CNpcScript::mada_liekyen_10()
{
	Speak( NpcId(), 291 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Knight Priestess
// File			: MaDa_Lobiet.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_lobiet_0()
{
	Speak( NpcId(), 292 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_lobiet_1()
{
	LaunchQuest();
}

void CNpcScript::mada_lobiet_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_lobiet_3()
{
	Say( 107 );
}

void CNpcScript::mada_lobiet_4()
{
	Say( 293 );
}

void CNpcScript::mada_lobiet_5()
{
	
}

void CNpcScript::mada_lobiet_6()
{
	
}

void CNpcScript::mada_lobiet_8()
{
	
}

void CNpcScript::mada_lobiet_9()
{
	Say( 294 );	
}

void CNpcScript::mada_lobiet_10()
{
	Speak( NpcId(), 295 );
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Darkon Lodelight Manager
// File			: MaDa_LodeLight.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_lodelight_0()
{
	Speak( NpcId(), 296 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_lodelight_1()
{
	LaunchQuest();
}

void CNpcScript::mada_lodelight_2()
{
	AddKey( 9 );
	AddKey( 10 );
	AddKey( 36 );		
}

void CNpcScript::mada_lodelight_3()
{
	Say( 297 );
}

void CNpcScript::mada_lodelight_4()
{
	Say( 298 );
}

void CNpcScript::mada_lodelight_5()
{
	
}

void CNpcScript::mada_lodelight_6()
{
	
}

void CNpcScript::mada_lodelight_8()
{
	
}

void CNpcScript::mada_lodelight_9()
{
	Say( 299 );	
}

void CNpcScript::mada_lodelight_36()
{
	Say( 300 );
}

void CNpcScript::mada_lodelight_10()
{
	Speak( NpcId(), 301 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Darkon Lodelight Manager
// File			: MaDa_LodeLight2.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_lodelight2_0()
{
	Speak( NpcId(), 296 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_lodelight2_1()
{
	LaunchQuest();
}

void CNpcScript::mada_lodelight2_2()
{
	AddKey( 9 );
	AddKey( 10 );
	AddKey( 36 );		
}

void CNpcScript::mada_lodelight2_3()
{
	Say( 297 );
}

void CNpcScript::mada_lodelight2_4()
{
	Say( 298 );
}

void CNpcScript::mada_lodelight2_5()
{
	
}

void CNpcScript::mada_lodelight2_6()
{
	
}

void CNpcScript::mada_lodelight2_8()
{
	
}

void CNpcScript::mada_lodelight2_9()
{
	Say( 302 );	
}

void CNpcScript::mada_lodelight2_36()
{
	Say( 300 );
}

void CNpcScript::mada_lodelight2_10()
{
	Speak( NpcId(), 301 );
	Exit();
}

//Script End
//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040418_001_v1
// Title    : Vocation Master( 2nd ) 
// Link NPC : Madrigal Darkon Lorein
// File Name: MaDa_Lorein.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mada_lorein_0()
{
	Speak( NpcId(), 303 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_lorein_1()
{
	if( GetQuestState( QUEST_HERORAN_TRN1 ) >= 0 || GetQuestState( QUEST2_HEROSTORY ) >= 0 )
	{
		SayQuest( QUEST_HEROJES_TRN1, QSAY_EXTRA01 );
		AddKey( 10 );
	}
		
	else
	{ 
		LaunchQuest();
	}
}

void CNpcScript::mada_lorein_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_lorein_3()
{
	Say ( 172 );
}

void CNpcScript::mada_lorein_4()
{
	Say ( 304 );
}

void CNpcScript::mada_lorein_5()
{

}

void CNpcScript::mada_lorein_6()
{

}

void CNpcScript::mada_lorein_7()
{

}

void CNpcScript::mada_lorein_8()
{

}

void CNpcScript::mada_lorein_9()
{
	Say ( 305 );
}

void CNpcScript::mada_lorein_10()
{
	Speak( NpcId(), 306 );
	Exit();
}

//Script End
//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040406_003_v1
// Title    : Request Box Quest Agent
// Link NPC : Madrigal Darkon Lurif
// File Name: MaDa_Lurif.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mada_lurif_0()
{
	Speak( NpcId(), 307 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_lurif_1()
{
	LaunchQuest();
}

void CNpcScript::mada_lurif_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_lurif_3()
{
	Say ( 308 );
}

void CNpcScript::mada_lurif_4()
{
	Say ( 309 );
}

void CNpcScript::mada_lurif_5()
{

}

void CNpcScript::mada_lurif_6()
{

}

void CNpcScript::mada_lurif_7()
{

}

void CNpcScript::mada_lurif_8()
{

}

void CNpcScript::mada_lurif_9()
{
	Say ( 310 );
}

void CNpcScript::mada_lurif_10()
{
	Speak( NpcId(), 65 );
	Exit();
}

//Script End
//Script Begin
void CNpcScript::mada_nein_0()
{
	Speak( NpcId(), 311 );
	SetScriptTimer( 15 );
}
void CNpcScript::mada_nein_1()
{

	LaunchQuest();

}
void CNpcScript::mada_nein_2()
{
	AddKey( 9 );
	AddKey( 10 );
}
void CNpcScript::mada_nein_3()
{
	Say ( 312 );
}
void CNpcScript::mada_nein_4()
{
	Say ( 313 );

}
void CNpcScript::mada_nein_5()
{
}
void CNpcScript::mada_nein_6()
{
}
void CNpcScript::mada_nein_7()
{
}
void CNpcScript::mada_nein_8()
{
}
void CNpcScript::mada_nein_9()
{
	Say( 314 );
}
void CNpcScript::mada_nein_10()
{
	Say( 315 );
	Exit();
}


//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Darken Office Manager
// File			: MaDa_Ollien.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_ollien_0()
{
	Speak( NpcId(), 316 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_ollien_1()
{
	LaunchQuest();
}

void CNpcScript::mada_ollien_2()
{
	AddKey( 9 );
	AddKey( 10 );
	//AddKey( "스탯초기화" );		
}

void CNpcScript::mada_ollien_3()
{
	Say( 317 );
}

void CNpcScript::mada_ollien_4()
{
	Say( 318 );
}

void CNpcScript::mada_ollien_5()
{
	
}

void CNpcScript::mada_ollien_6()
{
	
}

void CNpcScript::mada_ollien_8()
{
	
}

void CNpcScript::mada_ollien_9()
{
	Say( 319);	
}

void CNpcScript::mada_ollien_37()
{
	InitStat();
}

void CNpcScript::mada_ollien_10()
{
	Speak( NpcId(), 320 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Psychiper Priestess
// File			: MaDa_Pereb.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_pereb_0()
{
	Speak( NpcId(), 321 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_pereb_1()
{
	LaunchQuest();
}

void CNpcScript::mada_pereb_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_pereb_3()
{
	Say( 107 );
}

void CNpcScript::mada_pereb_4()
{
	Say( 322 );
}

void CNpcScript::mada_pereb_5()
{
	
}

void CNpcScript::mada_pereb_6()
{
	
}

void CNpcScript::mada_pereb_8()
{
	
}

void CNpcScript::mada_pereb_9()
{
	Say( 323 );	
}

void CNpcScript::mada_pereb_10()
{
	Speak( NpcId(), 324 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Erons Factory Armor Shop Owner
// File			: MaDa_Phacham.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_phacham_0()
{
	Speak( NpcId(), 325 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_phacham_1()
{
	LaunchQuest();
}

void CNpcScript::mada_phacham_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_phacham_3()
{
	Say( 326 );
}

void CNpcScript::mada_phacham_4()
{
	Say( 327 );
}

void CNpcScript::mada_phacham_5()
{
	
}

void CNpcScript::mada_phacham_6()
{
	
}

void CNpcScript::mada_phacham_8()
{
	
}

void CNpcScript::mada_phacham_9()
{
	Say( 328 );	
}

void CNpcScript::mada_phacham_10()
{
	Speak( NpcId(), 329 );
	Exit();
}

//Script End
//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON050411_016_v1
// Title    : Vocation Master
// Link NPC : Madrigal Darkon Pyre
// File Name: MaDa_Pyre.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mada_pyre_0()
{
	Speak( NpcId(), 330 );
	SetScriptTimer( 15 );
}
void CNpcScript::mada_pyre_1()
{
	LaunchQuest(); 
}

void CNpcScript::mada_pyre_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_pyre_3()
{
	Say ( 331 );
}

void CNpcScript::mada_pyre_4()
{
	Say ( 332 );
}

void CNpcScript::mada_pyre_5()
{

}

void CNpcScript::mada_pyre_6()
{
	
}

void CNpcScript::mada_pyre_7()
{

}

void CNpcScript::mada_pyre_8()
{
	
}

void CNpcScript::mada_pyre_9()
{
	Say ( 333 );
}

void CNpcScript::mada_pyre_10()
{
	Speak( NpcId(), 334 );
	Exit();
}

//Script End
//Script Begin
void CNpcScript::mada_rankashu_0()
{
	Speak( NpcId(), 335 );
	SetScriptTimer( 15 );
}
void CNpcScript::mada_rankashu_1()
{
	LaunchQuest();
}
void CNpcScript::mada_rankashu_2()
{
	AddKey( 12 );
	if(GetItemNum(II_SYS_SYS_QUE_LETTERRANKASHU) == 0)
	{
	AddCondKey( 336,39 );
	}
	AddKey( 10 );
	AddKey( 38 );
}
void CNpcScript::mada_rankashu_3()
{
	Say ( 337 );
}
void CNpcScript::mada_rankashu_4()
{
	if(GetQuestState(QUEST_DUDK_VOL1) == QS_END)
	{
	Say ( 338 );
	}
	else
	{
	Say ( 339 );
	}	
}
void CNpcScript::mada_rankashu_5()
{
}
void CNpcScript::mada_rankashu_6()
{	
}
void CNpcScript::mada_rankashu_7()
{
}
void CNpcScript::mada_rankashu_8()
{
}
void CNpcScript::mada_rankashu_12()
{
	Say( 340 );
}
void CNpcScript::mada_rankashu_38()
{
	Say( 341 );
}
void CNpcScript::mada_rankashu_10()
{
	Say( 342 );
	Exit();
}
void CNpcScript::mada_rankashu_39()
{
	Say( 343 );
	if( GetEmptyInventoryNum() < 1 )
	{
		PrintSystemMessage(TID_GAME_LACKSPACE);
		Exit();
	}

	else
	{
		if( GetQuestState(QUEST_DUDK_VOL1) == QS_BEGIN && GetPlayerLvl() >= 70 )
		{
			CreateItem( II_SYS_SYS_QUE_LETTERRANKASHU, 1 );
			RemoveKey(39);
		}

		else
		{
			Exit();
		}
	}
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Darken City Weapon Shop Owner
// File			: MaDa_Remine.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_remine_0()
{
	Speak( NpcId(), 344 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_remine_1()
{
	LaunchQuest();
}

void CNpcScript::mada_remine_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_remine_3()
{
	Say( 345 );
}

void CNpcScript::mada_remine_4()
{
	Say( 346 );
}

void CNpcScript::mada_remine_5()
{
	
}

void CNpcScript::mada_remine_6()
{
	
}

void CNpcScript::mada_remine_8()
{
	
}

void CNpcScript::mada_remine_9()
{
	Say( 347 );	
}

void CNpcScript::mada_remine_10()
{
	Speak( NpcId(), 348 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Billposter Master
// File			: MaDa_Ride.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_ride_0()
{
	Speak( NpcId(), 349 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_ride_1()
{
	if( GetQuestState( QUEST_HERORIN_TRN1 ) >= 0 )
	{
		SayQuest( QUEST_HEROBIL_TRN1, QSAY_EXTRA01 );
		AddKey( 10 );
	}
		
	else
	{ 
		LaunchQuest();
	}
}

void CNpcScript::mada_ride_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_ride_3()
{
	Say( 107 );
}

void CNpcScript::mada_ride_4()
{
	Say( 350 );
}

void CNpcScript::mada_ride_5()
{
	
}

void CNpcScript::mada_ride_6()
{
	
}

void CNpcScript::mada_ride_8()
{
	
}

void CNpcScript::mada_ride_9()
{
	Say( 351 );	
}

void CNpcScript::mada_ride_10()
{
	Speak( NpcId(), 352 );
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Ringmaster Guardian
// File			: MaDa_Romvoette.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_romvoette_0()
{
	Speak( NpcId(), 353 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_romvoette_1()
{
	LaunchQuest();
}

void CNpcScript::mada_romvoette_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_romvoette_3()
{
	Say( 107 );
}

void CNpcScript::mada_romvoette_4()
{
	Say( 354 );
}

void CNpcScript::mada_romvoette_5()
{
	
}

void CNpcScript::mada_romvoette_6()
{
	
}

void CNpcScript::mada_romvoette_8()
{
	if( GetQuestState(QUEST_HERORIN_TRN4) == QS_END && GetPlayerJob() == 3 && GetPlayerLvl() == 60 )
	{
		ChangeJob( 10 );
	}

	else
	{
		Exit();
	}
}

void CNpcScript::mada_romvoette_9()
{
	Say( 355 );	
}

void CNpcScript::mada_romvoette_10()
{
	Speak( NpcId(), 356 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Darken City Armor Shop Owner
// File			: MaDa_Roocky.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_roocky_0()
{
	Speak( NpcId(), 357 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_roocky_1()
{
	LaunchQuest();
}

void CNpcScript::mada_roocky_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_roocky_3()
{
	Say( 358 );
}

void CNpcScript::mada_roocky_4()
{
	Say( 359 );
}

void CNpcScript::mada_roocky_5()
{
	
}

void CNpcScript::mada_roocky_6()
{
	
}

void CNpcScript::mada_roocky_8()
{
	
}

void CNpcScript::mada_roocky_9()
{
	Say( 360 );	
}

void CNpcScript::mada_roocky_10()
{
	Speak( NpcId(), 361 );
	Exit();
}

//Script End
//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040418_006_v1
// Title    : Ranger Priestess 
// Link NPC : Madrigal Darkon Homeit
// File Name: MaDa_Rupim.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mada_rupim_0()
{
	Speak( NpcId(), 362 );
	SetScriptTimer( 12 );
}

void CNpcScript::mada_rupim_1()
{
	LaunchQuest();
}

void CNpcScript::mada_rupim_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_rupim_3()
{
	Say ( 254 );
}

void CNpcScript::mada_rupim_4()
{
	Say ( 363 );
}

void CNpcScript::mada_rupim_5()
{

}

void CNpcScript::mada_rupim_6()
{
	
}

void CNpcScript::mada_rupim_7()
{

}

void CNpcScript::mada_rupim_8()
{
	
}

void CNpcScript::mada_rupim_9()
{
	Say ( 364 );
}

void CNpcScript::mada_rupim_10()
{
	Speak( NpcId(), 365 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Blade Guardian
// File			: MaDa_Sencyrit.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_sencyrit_0()
{
	Speak( NpcId(), 366 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_sencyrit_1()
{
	LaunchQuest();
}

void CNpcScript::mada_sencyrit_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_sencyrit_3()
{
	Say( 107 );
}

void CNpcScript::mada_sencyrit_4()
{
	Say( 367 );
}

void CNpcScript::mada_sencyrit_5()
{
	
}

void CNpcScript::mada_sencyrit_6()
{
	
}

void CNpcScript::mada_sencyrit_8()
{
	if( GetQuestState(QUEST_HEROBLA_TRN4) == QS_END && GetPlayerJob() == 1 && GetPlayerLvl() == 60 )
	{
		ChangeJob( 7 );
	}

	else
	{
		Exit();
	}
}

void CNpcScript::mada_sencyrit_9()
{
	Say( 368 );	
}

void CNpcScript::mada_sencyrit_10()
{
	Speak( NpcId(), 369 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Psychiper Guardian
// File			: MaDa_Shyniff.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_shyniff_0()
{
	Speak( NpcId(), 370 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_shyniff_1()
{
	LaunchQuest();
}

void CNpcScript::mada_shyniff_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_shyniff_3()
{
	Say( 107 );
}

void CNpcScript::mada_shyniff_4()
{
	Say( 371 );
}

void CNpcScript::mada_shyniff_5()
{
	
}

void CNpcScript::mada_shyniff_6()
{
	
}

void CNpcScript::mada_shyniff_8()
{
	if( GetQuestState(QUEST_HEROPSY_TRN4) == QS_END && GetPlayerJob() == 4 && GetPlayerLvl() == 60 )
	{
		ChangeJob( 12 );
	}

	else
	{
		Exit();
	}
}

void CNpcScript::mada_shyniff_9()
{
	Say( 372 );	
}

void CNpcScript::mada_shyniff_10()
{
	Speak( NpcId(), 373 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Erons Factory Weapon Shop Owner
// File			: MaDa_Stima.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_stima_0()
{
	Speak( NpcId(), 374 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_stima_1()
{
	LaunchQuest();
}

void CNpcScript::mada_stima_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_stima_3()
{
	Say( 375 );
}

void CNpcScript::mada_stima_4()
{
	Say( 376 );
}

void CNpcScript::mada_stima_5()
{
	
}

void CNpcScript::mada_stima_6()
{
	
}

void CNpcScript::mada_stima_8()
{
	
}

void CNpcScript::mada_stima_9()
{
	Say( 377 );	
}

void CNpcScript::mada_stima_10()
{
	Speak( NpcId(), 378 );
	Exit();
}

//Script End
//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040411_008_v1
// Title    : Vocation Traning Assist
// Link NPC : Madrigal Flaris Tailer
// File Name: MaSa_Tailer.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mada_tailer_0()
{
	Speak( NpcId(), 379 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_tailer_1()
{
	LaunchQuest();
}

void CNpcScript::mada_tailer_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_tailer_3()
{
	Say ( 380 );
}

void CNpcScript::mada_tailer_4()
{
	Say ( 381 );
}

void CNpcScript::mada_tailer_5()
{

}

void CNpcScript::mada_tailer_6()
{

}

void CNpcScript::mada_tailer_7()
{

}

void CNpcScript::mada_tailer_8()
{

	if( GetQuestState(QUEST_VOCACR_TRN3) == QS_END && GetPlayerJob() == 0 && GetPlayerLvl() == 15 )
	{
		ChangeJob( 2 );
		InitStat();
	}

	else
	{
		Exit();
	}
}

void CNpcScript::mada_tailer_9()
{
	Say ( 382 );
	AddAnswer( 10 );
}

void CNpcScript::mada_tailer_10()
{
	Speak( NpcId(), 383 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Nomads General Merchant
// File			: MaDa_Tandy.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_tandy_0()
{
	Speak( NpcId(), 853 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_tandy_1()
{
	LaunchQuest();
}

void CNpcScript::mada_tandy_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_tandy_3()
{
	Say( 854 );
}

void CNpcScript::mada_tandy_4()
{
	Say( 855 );
}

void CNpcScript::mada_tandy_5()
{
	
}

void CNpcScript::mada_tandy_6()
{
	
}

void CNpcScript::mada_tandy_8()
{
	
}

void CNpcScript::mada_tandy_9()
{
	Say( 856 );	
}

void CNpcScript::mada_tandy_10()
{
	Speak( NpcId(), 857 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Elementor Guardian
// File			: MaDa_Wendien.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_wendien_0()
{
	Speak( NpcId(), 389 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_wendien_1()
{
	LaunchQuest();
}

void CNpcScript::mada_wendien_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_wendien_3()
{
	Say( 107 );
}

void CNpcScript::mada_wendien_4()
{
	Say( 390 );
}

void CNpcScript::mada_wendien_5()
{
	
}

void CNpcScript::mada_wendien_6()
{
	
}

void CNpcScript::mada_wendien_8()
{
	if( GetQuestState(QUEST_HEROELE_TRN4) == QS_END && GetPlayerJob() == 4 && GetPlayerLvl() == 60 )
	{
		ChangeJob( 13 );
	}

	else
	{
		Exit();
	}
}

void CNpcScript::mada_wendien_9()
{
	Say( 391 );	
}

void CNpcScript::mada_wendien_10()
{
	Speak( NpcId(), 392 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Mercenery Master
// File			: MaFl_Andy.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_andy_0()
{
	Speak( NpcId(), 393 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_andy_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_andy_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_andy_3()
{
	Say( 107 );
}

void CNpcScript::mafl_andy_4()
{
	Say( 394 );
}

void CNpcScript::mafl_andy_5()
{
	
}

void CNpcScript::mafl_andy_6()
{
	
}

void CNpcScript::mafl_andy_8()
{
	
}

void CNpcScript::mafl_andy_9()
{
	Say( 395 );	
}

void CNpcScript::mafl_andy_10()
{
	Speak( NpcId(), 396 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Madrigal Santa
// File			: MaFl_Aramy.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_aramy_0()
{
	Speak( NpcId(), 397 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_aramy_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_aramy_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_aramy_3()
{
	Say( 107 );
}

void CNpcScript::mafl_aramy_4()
{
	Say( 398 );
}

void CNpcScript::mafl_aramy_5()
{
	
}

void CNpcScript::mafl_aramy_6()
{
	
}

void CNpcScript::mafl_aramy_8()
{
	
}

void CNpcScript::mafl_aramy_9()
{
	Say( 399 );	
}

void CNpcScript::mafl_aramy_10()
{
	Speak( NpcId(), 400 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Astronomy Scholar 
// File			: MaFl_Ata.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_ata_0()
{
	Speak( NpcId(), 401 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_ata_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_ata_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_ata_3()
{
	Say( 107 );
}

void CNpcScript::mafl_ata_4()
{
	Say( 402 );
}

void CNpcScript::mafl_ata_5()
{
	
}

void CNpcScript::mafl_ata_6()
{
	
}

void CNpcScript::mafl_ata_8()
{
	
}

void CNpcScript::mafl_ata_9()
{
	Say( 403 );	
}

void CNpcScript::mafl_ata_10()
{
	Speak( NpcId(), 404 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Madrigal Santa
// File			: MaFl_Atto.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_atto_0()
{
	Speak( NpcId(), 405 );	
	SetScriptTimer( 10 );
}

void CNpcScript::mafl_atto_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_atto_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_atto_3()
{
	Say( 107 );
}

void CNpcScript::mafl_atto_4()
{
	Say( 406 );
}

void CNpcScript::mafl_atto_5()
{
	
}

void CNpcScript::mafl_atto_6()
{
	
}

void CNpcScript::mafl_atto_8()
{
	
}

void CNpcScript::mafl_atto_9()
{
	Say( 407 );	
}

void CNpcScript::mafl_atto_10()
{
	Speak( NpcId(), 408 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Flarine City Armor Shop Owner
// File			: MaFl_Boboko.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_boboko_0()
{
	Speak( NpcId(), 409 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_boboko_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_boboko_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_boboko_3()
{
	Say( 410 );
}

void CNpcScript::mafl_boboko_4()
{
	Say( 411 );
}

void CNpcScript::mafl_boboko_5()
{
	
}

void CNpcScript::mafl_boboko_6()
{
	
}

void CNpcScript::mafl_boboko_8()
{
	
}

void CNpcScript::mafl_boboko_9()
{
	Say( 412 );	
}

void CNpcScript::mafl_boboko_10()
{
	Speak( NpcId(), 413 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Flarine City Weapon Shop Owner
// File			: MaFl_Boboku.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_boboku_0()
{
	Speak( NpcId(), 414 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_boboku_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_boboku_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_boboku_3()
{
	Say( 415 );
}

void CNpcScript::mafl_boboku_4()
{
	Say( 416 );
}

void CNpcScript::mafl_boboku_5()
{
	
}

void CNpcScript::mafl_boboku_6()
{
	
}

void CNpcScript::mafl_boboku_8()
{
	
}

void CNpcScript::mafl_boboku_9()
{
	Say( 417 );	
}

void CNpcScript::mafl_boboku_10()
{
	Speak( NpcId(), 418 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Madrigal Santa
// File			: MaFl_Buruto.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_buruto_0()
{
	Speak( NpcId(), 419 );	
	SetScriptTimer( 13 );
}

void CNpcScript::mafl_buruto_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_buruto_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_buruto_3()
{
	Say( 107 );
}

void CNpcScript::mafl_buruto_4()
{
	Say( 420 );
}

void CNpcScript::mafl_buruto_5()
{
	
}

void CNpcScript::mafl_buruto_6()
{
	
}

void CNpcScript::mafl_buruto_8()
{
	
}

void CNpcScript::mafl_buruto_9()
{
	Say( 421 );	
}

void CNpcScript::mafl_buruto_10()
{
	Speak( NpcId(), 422 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Paul's doughter
// File			: MaFl_ChiChi.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_chichi_0()
{
	Speak( NpcId(), 423 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_chichi_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_chichi_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_chichi_3()
{
	Say( 107 );
}

void CNpcScript::mafl_chichi_4()
{
	Say( 424 );
}

void CNpcScript::mafl_chichi_5()
{
	
}

void CNpcScript::mafl_chichi_6()
{
	
}

void CNpcScript::mafl_chichi_8()
{
	
}

void CNpcScript::mafl_chichi_9()
{
	Say( 425 );	
}

void CNpcScript::mafl_chichi_10()
{
	Speak( NpcId(), 426 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Madrigal Chorus
// File			: MaFl_Chorus1.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_chorus1_0()
{
	Speak( NpcId(), 427 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_chorus1_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_chorus1_2()
{
	AddKey( 10 );		
}

void CNpcScript::mafl_chorus1_3()
{
	Say( 107 );
}

void CNpcScript::mafl_chorus1_4()
{
	Say( 428 );
}

void CNpcScript::mafl_chorus1_5()
{
	
}

void CNpcScript::mafl_chorus1_6()
{
	
}

void CNpcScript::mafl_chorus1_8()
{
	
}

void CNpcScript::mafl_chorus1_10()
{
	Speak( NpcId(), 429 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Madrigal Chorus
// File			: MaFl_Chorus2.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_chorus2_0()
{
	Speak( NpcId(), 430 );	
	SetScriptTimer( 17 );
}

void CNpcScript::mafl_chorus2_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_chorus2_2()
{
	AddKey( 10 );		
}

void CNpcScript::mafl_chorus2_3()
{
	Say( 107 );
}

void CNpcScript::mafl_chorus2_4()
{
	Say( 431 );
}

void CNpcScript::mafl_chorus2_5()
{
	
}

void CNpcScript::mafl_chorus2_6()
{
	
}

void CNpcScript::mafl_chorus2_8()
{
	
}

void CNpcScript::mafl_chorus2_10()
{
	Speak( NpcId(), 429 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Madrigal Chorus
// File			: MaFl_Chorus3.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_chorus3_0()
{
	Speak( NpcId(), 432 );	
	SetScriptTimer( 19 );
}

void CNpcScript::mafl_chorus3_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_chorus3_2()
{
	AddKey( 10 );		
}

void CNpcScript::mafl_chorus3_3()
{
	Say( 107 );
}

void CNpcScript::mafl_chorus3_4()
{
	Say( 433 );
}

void CNpcScript::mafl_chorus3_5()
{
	
}

void CNpcScript::mafl_chorus3_6()
{
	
}

void CNpcScript::mafl_chorus3_8()
{
	
}

void CNpcScript::mafl_chorus3_10()
{
	Speak( NpcId(), 429 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Madrigal Chorus
// File			: MaFl_Chorus3.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_chorus4_0()
{
	Speak( NpcId(), 434 );	
	SetScriptTimer( 21 );
}

void CNpcScript::mafl_chorus4_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_chorus4_2()
{
	AddKey( 10 );		
}

void CNpcScript::mafl_chorus4_3()
{
	Say( 107 );
}

void CNpcScript::mafl_chorus4_4()
{
	Say( 435 );
}

void CNpcScript::mafl_chorus4_5()
{
	
}

void CNpcScript::mafl_chorus4_6()
{
	
}

void CNpcScript::mafl_chorus4_8()
{
	
}

void CNpcScript::mafl_chorus4_10()
{
	Speak( NpcId(), 429 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Wise Man of Ringmaster
// File			: MaFl_Clamb.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_clamb_0()
{
	Speak( NpcId(), 436 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_clamb_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_clamb_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_clamb_3()
{
	Say( 107 );
}

void CNpcScript::mafl_clamb_4()
{
	Say( 437 );
}

void CNpcScript::mafl_clamb_5()
{
	
}

void CNpcScript::mafl_clamb_6()
{
	
}

void CNpcScript::mafl_clamb_8()
{
	
}

void CNpcScript::mafl_clamb_9()
{
	Say( 438 );	
}

void CNpcScript::mafl_clamb_10()
{
	Speak( NpcId(), 439 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Wise Man of Psychiper
// File			: MaFl_Cuarine.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_cuarine_0()
{
	Speak( NpcId(), 440 );	
	SetScriptTimer( 12 );
}

void CNpcScript::mafl_cuarine_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_cuarine_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_cuarine_3()
{
	Say( 107 );
}

void CNpcScript::mafl_cuarine_4()
{
	Say( 441 );
}

void CNpcScript::mafl_cuarine_5()
{
	
}

void CNpcScript::mafl_cuarine_6()
{
	
}

void CNpcScript::mafl_cuarine_8()
{
	
}

void CNpcScript::mafl_cuarine_9()
{
	Say( 442 );	
}

void CNpcScript::mafl_cuarine_10()
{
	Speak( NpcId(), 443 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Wise Man of Elementor
// File			: MaFl_Cuzrill.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_cuzrill_0()
{
	Speak( NpcId(), 444 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_cuzrill_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_cuzrill_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_cuzrill_3()
{
	Say( 107 );
}

void CNpcScript::mafl_cuzrill_4()
{
	Say( 445 );
}

void CNpcScript::mafl_cuzrill_5()
{
	
}

void CNpcScript::mafl_cuzrill_6()
{
	
}

void CNpcScript::mafl_cuzrill_8()
{
	
}

void CNpcScript::mafl_cuzrill_9()
{
	Say( 446 );	
}

void CNpcScript::mafl_cuzrill_10()
{
	Speak( NpcId(), 447 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060802 )
// Author		: Jimmie
// Title		: Flarine City Station Manager
// File			: MaFl_Dior.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_dior_0()
{
	Speak( NpcId(), 448 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_dior_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_dior_2()
{
	AddKey( 9 );
	AddKey( 40 );
	AddKey( 41 );
	AddKey( 10 );		
}

void CNpcScript::mafl_dior_3()
{
	Say( 449 );
}

void CNpcScript::mafl_dior_4()
{
	Say( 450 );
}

void CNpcScript::mafl_dior_5()
{
	
}

void CNpcScript::mafl_dior_6()
{
	
}

void CNpcScript::mafl_dior_8()
{
	
}

void CNpcScript::mafl_dior_9()
{
	Say( 451 );	
}

void CNpcScript::mafl_dior_40()
{
 	Say(452);
        Say(453); 
	Say(454);
	Say(455);
        Say(456);
        Say(457);
}

void CNpcScript::mafl_dior_41()
{
	Say(458);
	Say(459);
        Say(460);
        Say(461);
        Say(462);
	Say(463);
        Say(464); 
}

void CNpcScript::mafl_dior_10()
{
	Speak( NpcId(), 465 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Wise Man of Billposter
// File			: MaFl_Domek.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_domek_0()
{
	Speak( NpcId(), 466 );	
	SetScriptTimer( 12 );
}

void CNpcScript::mafl_domek_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_domek_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_domek_3()
{
	Say( 107 );
}

void CNpcScript::mafl_domek_4()
{
	Say( 467 );
}

void CNpcScript::mafl_domek_5()
{
	
}

void CNpcScript::mafl_domek_6()
{
	
}

void CNpcScript::mafl_domek_8()
{
	
}

void CNpcScript::mafl_domek_9()
{
	Say( 468 );	
}

void CNpcScript::mafl_domek_10()
{
	Speak( NpcId(), 469 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Guild Combat Manager
// File			: MaSa_Donaris.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_donaris_0()
{
	Speak( NpcId(), 470 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_donaris_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_donaris_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_donaris_3()
{
	
}

void CNpcScript::mafl_donaris_4()
{
	Say( 471 );
}

void CNpcScript::mafl_donaris_5()
{

}

void CNpcScript::mafl_donaris_6()
{

}

void CNpcScript::mafl_donaris_7()
{

}

void CNpcScript::mafl_donaris_8()
{

}

void CNpcScript::mafl_donaris_9()
{
	Say( 472 );
}

void CNpcScript::mafl_donaris_10()
{
	Speak( NpcId(), 473 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Madrigal Bionomics Investigator 
// File			: MaFl_DrEstly.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_drestly_0()
{
	Speak( NpcId(), 474 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_drestly_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_drestly_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_drestly_3()
{
	Say( 107 );
}

void CNpcScript::mafl_drestly_4()
{
	Say( 475 );
}

void CNpcScript::mafl_drestly_5()
{
	
}

void CNpcScript::mafl_drestly_6()
{
	
}

void CNpcScript::mafl_drestly_8()
{
	
}

void CNpcScript::mafl_drestly_9()
{
	Say( 476 );	
}

void CNpcScript::mafl_drestly_10()
{
	Speak( NpcId(), 477 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Assist Traning Assist
// File			: MaFl_Elic.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_elic_0()
{
	Speak( NpcId(), 478 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_elic_1()
{
	if( GetQuestState( QUEST_VOCMER_BFTRN ) >= 0 || GetQuestState( QUEST_VOCMAG_BFTRN ) >= 0 || GetQuestState( QUEST_VOCACR_BFTRN ) >= 0)
	{
		if( GetQuestState( QUEST_SCE_MDRIGALTEACHER1 ) >= 0 )
			LaunchQuest();
		else
			SayQuest( QUEST_VOCACR_BFTRN, QSAY_EXTRA01 );
		AddKey( 9 );
		AddKey( 10 );
	}
	else
	{
		LaunchQuest();
	}
}

void CNpcScript::mafl_elic_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_elic_3()
{
	Say( 107 );
}

void CNpcScript::mafl_elic_4()
{
	Say( 479 );
}

void CNpcScript::mafl_elic_5()
{
	
}

void CNpcScript::mafl_elic_6()
{
	
}

void CNpcScript::mafl_elic_8()
{
	
}

void CNpcScript::mafl_elic_9()
{
	Say( 480 );	
}

void CNpcScript::mafl_elic_10()
{
	Speak( NpcId(), 481 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Kyon Warrior
// File			: MaFl_Eoners.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_eoners_0()
{
	Speak( NpcId(), 482 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_eoners_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_eoners_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_eoners_3()
{
	Say( 107 );
}

void CNpcScript::mafl_eoners_4()
{
	Say( 483 );
}

void CNpcScript::mafl_eoners_5()
{
	
}

void CNpcScript::mafl_eoners_6()
{
	
}

void CNpcScript::mafl_eoners_8()
{
	
}

void CNpcScript::mafl_eoners_9()
{
	Say( 484 );	
}

void CNpcScript::mafl_eoners_10()
{
	Speak( NpcId(), 485 );
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Flarine Mayor
// File			: MaFl_FlaMayor.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_flamayor_0()
{
	Speak( NpcId(), 486 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_flamayor_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_flamayor_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_flamayor_3()
{
	Say( 107 );
}

void CNpcScript::mafl_flamayor_4()
{
	Say( 487 );
}

void CNpcScript::mafl_flamayor_5()
{
	
}

void CNpcScript::mafl_flamayor_6()
{
	
}

void CNpcScript::mafl_flamayor_8()
{
	
}

void CNpcScript::mafl_flamayor_9()
{
	Say( 488 );	
}

void CNpcScript::mafl_flamayor_10()
{
	Speak( NpcId(), 489 );
	Exit();
}

//Script End



//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040422_001_v1
// Title    : Flaris Vagrant Merchant 
// Link NPC : Madrigal Flaris Juria
// File Name: MaFl_Flraness.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mafl_flraness_0()
{
	Speak( NpcId(), 490 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_flraness_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_flraness_2()
{
	AddKey( 10 );
}

void CNpcScript::mafl_flraness_3()
{
	
}

void CNpcScript::mafl_flraness_4()
{
	Say( 491 );
}

void CNpcScript::mafl_flraness_5()
{

}

void CNpcScript::mafl_flraness_6()
{

}

void CNpcScript::mafl_flraness_7()
{

}

void CNpcScript::mafl_flraness_8()
{

}

void CNpcScript::mafl_flraness_10()
{
	Speak( NpcId(), 492 );
	Exit();
}

//Script End
void CNpcScript::mafl_furan_0()
{
	Speak( NpcId(), 194 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_furan_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_furan_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_furan_3()
{
	Say( 107 );
}

void CNpcScript::mafl_furan_4()
{
	Say( 195 );
}

void CNpcScript::mafl_furan_5()
{

}

void CNpcScript::mafl_furan_6()
{
	
}

void CNpcScript::mafl_furan_7()
{

}

void CNpcScript::mafl_furan_8()
{

}

void CNpcScript::mafl_furan_9()
{
	Say( 196 );
}

void CNpcScript::mafl_furan_10()
{
	Speak( NpcId(), 197 );
	Exit();
}
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Novelist
// File			: MaFl_Gergantes.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_gergantes_0()
{
	Speak( NpcId(), 493 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_gergantes_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_gergantes_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_gergantes_3()
{
	Say( 107 );
}

void CNpcScript::mafl_gergantes_4()
{
	Say( 494 );
}

void CNpcScript::mafl_gergantes_5()
{
	
}

void CNpcScript::mafl_gergantes_6()
{
	
}

void CNpcScript::mafl_gergantes_8()
{
	
}

void CNpcScript::mafl_gergantes_9()
{
	Say( 495 );	
}

void CNpcScript::mafl_gergantes_10()
{
	Speak( NpcId(), 496 );
	Exit();
}

//Script End




//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Kyon Warrior
// File			: MaFl_Ghalade.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_ghalade_0()
{
	Speak( NpcId(), 497 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_ghalade_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_ghalade_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_ghalade_3()
{
	Say( 107 );
}

void CNpcScript::mafl_ghalade_4()
{
	Say( 498 );
}

void CNpcScript::mafl_ghalade_5()
{
	
}

void CNpcScript::mafl_ghalade_6()
{
	
}

void CNpcScript::mafl_ghalade_8()
{
	
}

void CNpcScript::mafl_ghalade_9()
{
	Say( 499 );	
}

void CNpcScript::mafl_ghalade_10()
{
	Speak( NpcId(), 500 );
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Assist Hi-Dwarpet
// File			: MaFl_Goripeg.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_goripeg_0()
{
	Speak( NpcId(), 501 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_goripeg_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_goripeg_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_goripeg_3()
{
	Say( 107 );
}

void CNpcScript::mafl_goripeg_4()
{
	Say( 502 );
}

void CNpcScript::mafl_goripeg_5()
{
	
}

void CNpcScript::mafl_goripeg_6()
{
	
}

void CNpcScript::mafl_goripeg_8()
{
	
}

void CNpcScript::mafl_goripeg_9()
{
	Say( 503 );	
}

void CNpcScript::mafl_goripeg_10()
{
	Speak( NpcId(), 504 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Dradpet Manager
// File			: MaFl_Gornus.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_gornus_0()
{
	Speak( NpcId(), 505 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_gornus_1()
{
	if( GetPlayerLvl() >= 30 )
	{
		if( IsWormonServer() == TRUE )
		{	
			if( IsSetQuest( QUEST_DREADKREN ) == TRUE && GetQuestState( QUEST_DREADKREN ) == QS_END )
			{
				if( IsParty() == TRUE )
				{
					if( MonHuntStartParty( QUEST_BOSS_LV2, QS_BEGIN, QS_END, 1 ) == FALSE )
					{
						Say( 506 );
						AddKey( 10 );
					}
			

					else if( MonHuntStartParty( QUEST_BOSS_LV2, QS_BEGIN, QS_END, 1 ) == TRUE )
					{
											
					}

					else
					{
						Say( 507 );
						AddKey( 10 );	
					}
				}	
					
				else
				{
					Say( 508 );	
					AddKey( 10 );
				}
	
			}	
			
			else
			{
				Say( 509);
				Say( 510 );	
				AddKey( 10 );
			}
		}

		else
		{
			PrintSystemMessage( TID_GAME_NOTPLAYSERVER );
			Exit();	
		}
	}

	else
	{
		Say( 511 );	
		AddKey( 10 );
	}
}

void CNpcScript::mafl_gornus_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_gornus_3()
{
	
}

void CNpcScript::mafl_gornus_4()
{
	
}

void CNpcScript::mafl_gornus_5()
{
	
}

void CNpcScript::mafl_gornus_6()
{
	
}

void CNpcScript::mafl_gornus_8()
{
	
}

void CNpcScript::mafl_gornus_9()
{
	Say( 512 );	
}

void CNpcScript::mafl_gornus_10()
{
	Speak( NpcId(), 513 );
	Exit();
}

//Script End




//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Wise Man of Blade
// File			: MaFl_Guabrill.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_guabrill_0()
{
	Speak( NpcId(), 514 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_guabrill_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_guabrill_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_guabrill_3()
{
	Say( 107 );
}

void CNpcScript::mafl_guabrill_4()
{
	Say( 515 );
}

void CNpcScript::mafl_guabrill_5()
{
	
}

void CNpcScript::mafl_guabrill_6()
{
	
}

void CNpcScript::mafl_guabrill_8()
{
	
}

void CNpcScript::mafl_guabrill_9()
{
	Say( 516 );	
}

void CNpcScript::mafl_guabrill_10()
{
	Speak( NpcId(), 517 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modified )
// Author		: Jimmie
// Title		: Guild Combat Manager
// File			: MaSa_GuildWarser.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_guildwarser_0()
{
	Speak( NpcId(), 518 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_guildwarser_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_guildwarser_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_guildwarser_3()
{
	
}

void CNpcScript::mafl_guildwarser_4()
{
	Say( 519 );
}

void CNpcScript::mafl_guildwarser_5()
{

}

void CNpcScript::mafl_guildwarser_6()
{

}

void CNpcScript::mafl_guildwarser_7()
{

}

void CNpcScript::mafl_guildwarser_8()
{

}

void CNpcScript::mafl_guildwarser_9()
{
	Say( 520 );
}

void CNpcScript::mafl_guildwarser_10()
{
	Speak( NpcId(), 521 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Magicion Hi-Dwarpet
// File			: MaFl_Hastan.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_hastan_0()
{
	Speak( NpcId(), 522 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_hastan_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_hastan_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_hastan_3()
{
	Say( 107 );
}

void CNpcScript::mafl_hastan_4()
{
	Say( 523 );
}

void CNpcScript::mafl_hastan_5()
{
	
}

void CNpcScript::mafl_hastan_6()
{
	
}

void CNpcScript::mafl_hastan_8()
{
	
}

void CNpcScript::mafl_hastan_9()
{
	Say( 524 );	
}

void CNpcScript::mafl_hastan_10()
{
	Speak( NpcId(), 525 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Dradpet Manager
// File			: MaFl_Himadel.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_himadel_0()
{
	Speak( NpcId(), 505 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_himadel_1()
{
	if( GetPlayerLvl() >= 20 )
	{
		if( IsWormonServer() == TRUE )
		{	
			if( IsSetQuest( QUEST_DREADBIG ) == TRUE && GetQuestState( QUEST_DREADBIG ) == QS_END )
			{
				if( IsParty() == TRUE )
				{
					if( MonHuntStartParty( QUEST_BOSS_LV1, QS_BEGIN, QS_END, 1 ) == FALSE )
					{
						Say( 506 );
						AddKey( 10 );
					}
			

					else if( MonHuntStartParty( QUEST_BOSS_LV1, QS_BEGIN, QS_END, 1 ) == TRUE )
					{
						
					}

					else
					{
						Say( 507 );
						AddKey( 10 );	
					}
				}

				else
				{
					Say( 508 );	
					AddKey( 10 );
				}
			}

			else
			{
				Say( 509);
				Say( 510 );
				AddKey( 10 );	
			}
		}

		else
		{
			PrintSystemMessage( TID_GAME_NOTPLAYSERVER );
			Exit();	
		}
	}

	else
	{
		Say( 511 );	
		AddKey( 10 );
	}
}

void CNpcScript::mafl_himadel_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_himadel_3()
{
	
}

void CNpcScript::mafl_himadel_4()
{
	
}

void CNpcScript::mafl_himadel_5()
{
	
}

void CNpcScript::mafl_himadel_6()
{
	
}

void CNpcScript::mafl_himadel_8()
{
	
}

void CNpcScript::mafl_himadel_9()
{
	Say( 526 );	
}

void CNpcScript::mafl_himadel_10()
{
	Speak( NpcId(), 513 );
	Exit();
}

//Script End




//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Wise Man of Knight
// File			: MaFl_Hormes.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_hormes_0()
{
	Speak( NpcId(), 527 );	
	SetScriptTimer( 12 );
}

void CNpcScript::mafl_hormes_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_hormes_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_hormes_3()
{
	Say( 107 );
}

void CNpcScript::mafl_hormes_4()
{
	Say( 528 );
}

void CNpcScript::mafl_hormes_5()
{
	
}

void CNpcScript::mafl_hormes_6()
{
	
}

void CNpcScript::mafl_hormes_8()
{
	
}

void CNpcScript::mafl_hormes_9()
{
	Say( 529 );	
}

void CNpcScript::mafl_hormes_10()
{
	Speak( NpcId(), 530 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Mercenery Traning Instructor
// File			: MaFl_Hyuit
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_hyuit_0()
{
	Speak( NpcId(), 531 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_hyuit_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_hyuit_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_hyuit_3()
{
	Say( 107 );
}

void CNpcScript::mafl_hyuit_4()
{
	Say( 532 );
}

void CNpcScript::mafl_hyuit_5()
{
	
}

void CNpcScript::mafl_hyuit_6()
{
	
}

void CNpcScript::mafl_hyuit_8()
{
	
	if( GetQuestState(QUEST_VOCMER_TRN3) == QS_END && GetPlayerJob() == 0 && GetPlayerLvl() == 15 )
	{
		ChangeJob(1);
		InitStat();
	}

	else
	{
		Exit();
	}
}

void CNpcScript::mafl_hyuit_9()
{
	Say( 533 );	
}

void CNpcScript::mafl_hyuit_10()
{
	Speak( NpcId(), 534 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Flarine City Helper
// File			: MaFl_Is.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_is_0()
{
	Speak( NpcId(), 535 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_is_1()
{
	switch( Random( 3 ) )
	{
		case 0:	PlaySound( 536 ); break;
		case 1:	PlaySound( 537 ); break;
		case 2:	PlaySound( 538 ); break;
	}

	LaunchQuest();
}

void CNpcScript::mafl_is_2()
{
	AddKey( 9 );
	AddKey( 26 );
	AddKey( 42 );
	AddKey( 43 );
	AddKey( 10 );
}

void CNpcScript::mafl_is_3()
{
	Say( 539 );	
}

void CNpcScript::mafl_is_4()
{
	Say( 540 );
}

void CNpcScript::mafl_is_5()
{

}

void CNpcScript::mafl_is_6()
{

}

void CNpcScript::mafl_is_7()
{

}

void CNpcScript::mafl_is_8()
{

}

void CNpcScript::mafl_is_9()
{
	Say( 541);
	Say( 542 );
	Say( 543 );
}

void CNpcScript::mafl_is_26()
{
	Say( 544 );
	Say( 545 );
}

void CNpcScript::mafl_is_42()
{
	Say( 546 );
	Say( 547 );
	Say( 548 );
}

void CNpcScript::mafl_is_43()
{
	Say( 549 ); 
	Say( 550 ); 
	Say( 551);
}

void CNpcScript::mafl_is_10()
{
	switch( Random( 2 ) )
	{
		case 0:	PlaySound( 552 ); break;
		case 1:	PlaySound( 553 ); break;
	}
	Speak( NpcId(), 554 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Flarine City Helper
// File			: MaFl_Is.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_hairshop_0()
{
	Speak( NpcId(), 535 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_hairshop_1()
{
	switch( Random( 3 ) )
	{
		case 0:	PlaySound( 555 ); break;
		case 1:	PlaySound( 556 ); break;
		case 2:	PlaySound( 557 ); break;
	}

	LaunchQuest();
}

void CNpcScript::mafl_hairshop_2()
{
	AddKey( 9 );
	AddKey( 26 );
	AddKey( 42 );
	AddKey( 43 );
	AddKey( 10 );
}

void CNpcScript::mafl_hairshop_3()
{	
	Say( 539 );	
}

void CNpcScript::mafl_hairshop_4()
{
	Say( 540 );
}

void CNpcScript::mafl_hairshop_5()
{

}

void CNpcScript::mafl_hairshop_6()
{

}

void CNpcScript::mafl_hairshop_7()
{

}

void CNpcScript::mafl_hairshop_8()
{

}

void CNpcScript::mafl_hairshop_9()
{
	Say( 558);
	Say( 542 );
	Say( 543 );
}

void CNpcScript::mafl_hairshop_10()
{
	switch( Random( 2 ) )
	{
		case 0:	PlaySound( 559 ); break;
		case 1:	PlaySound( 560 ); break;
	}
	Speak( NpcId(), 554 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Flarine City Helper
// File			: MaFl_Ismeralda.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_ismeralda_0()
{
	Speak( NpcId(), 535 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_ismeralda_1()
{
	switch( Random( 3 ) )
	{
		case 0:	PlaySound( 561 ); break;
		case 1:	PlaySound( 562 ); break;
		case 2:	PlaySound( 563 ); break;
	}

	LaunchQuest();
}

void CNpcScript::mafl_ismeralda_2()
{
	AddKey( 9 );
	AddKey( 26 );
	AddKey( 42 );
	AddKey( 43 );
	AddKey( 10 );
}

void CNpcScript::mafl_ismeralda_3()
{	
	Say( 539 );	
}

void CNpcScript::mafl_ismeralda_4()
{
	Say( 540 );
}

void CNpcScript::mafl_ismeralda_5()
{

}

void CNpcScript::mafl_ismeralda_6()
{

}

void CNpcScript::mafl_ismeralda_7()
{

}

void CNpcScript::mafl_ismeralda_8()
{

}

void CNpcScript::mafl_ismeralda_9()
{
	Say( 564);
	Say( 542 );
	Say( 543 );
}

void CNpcScript::mafl_ismeralda_26()
{
	Say( 544 );
	Say( 545 );
}

void CNpcScript::mafl_ismeralda_42()
{
	Say( 546 );
	Say( 547 );
	Say( 548 );
}

void CNpcScript::mafl_ismeralda_43()
{
	Say( 549 ); 
	Say( 550 ); 
	Say( 551);
}

void CNpcScript::mafl_ismeralda_10()
{
	switch( Random( 2 ) )
	{
		case 0:	PlaySound( 565 ); break;
		case 1:	PlaySound( 566 ); break;
	}
	Speak( NpcId(), 554 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Flarine City Helper
// File			: MaFl_Ispim.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_ispim_0()
{
	Speak( NpcId(), 535 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_ispim_1()
{
	switch( Random( 3 ) )
	{
		case 0:	PlaySound( 567 ); break;
		case 1:	PlaySound( 568 ); break;
		case 2:	PlaySound( 569 ); break;
	}

	LaunchQuest();
}

void CNpcScript::mafl_ispim_2()
{
	AddKey( 9 );
	AddKey( 26 );
	AddKey( 42 );
	AddKey( 43 );
	AddKey( 10 );
}

void CNpcScript::mafl_ispim_3()
{	
	Say( 539 );	
}

void CNpcScript::mafl_ispim_4()
{
	Say( 540 );
}

void CNpcScript::mafl_ispim_5()
{

}

void CNpcScript::mafl_ispim_6()
{

}

void CNpcScript::mafl_ispim_7()
{

}

void CNpcScript::mafl_ispim_8()
{

}

void CNpcScript::mafl_ispim_9()
{
	Say( 570);
	Say( 542 );
	Say( 543 );
}

void CNpcScript::mafl_ispim_26()
{
	Say( 544 );
	Say( 545 );
}

void CNpcScript::mafl_ispim_42()
{
	Say( 546 );
	Say( 547 );
	Say( 548 );
}

void CNpcScript::mafl_ispim_43()
{
	Say( 549 ); 
	Say( 550 ); 
	Say( 551);
}

void CNpcScript::mafl_ispim_10()
{
	switch( Random( 2 ) )
	{
		case 0:	PlaySound( 571 ); break;
		case 1:	PlaySound( 572 ); break;
	}
	Speak( NpcId(), 554 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Flarine City Helper
// File			: MaFl_Isruel.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_isruel_0()
{
	Speak( NpcId(), 535 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_isruel_1()
{
	switch( Random( 3 ) )
	{
		case 0:	PlaySound( 573 ); break;
		case 1:	PlaySound( 574 ); break;
		case 2:	PlaySound( 575 ); break;
	}

	LaunchQuest();
}

void CNpcScript::mafl_isruel_2()
{
	AddKey( 9 );
	AddKey( 26 );
	AddKey( 42 );
	AddKey( 43 );
	AddKey( 10 );
}

void CNpcScript::mafl_isruel_3()
{	
	Say( 539 );	
}

void CNpcScript::mafl_isruel_4()
{
	Say( 540 );
}

void CNpcScript::mafl_isruel_5()
{

}

void CNpcScript::mafl_isruel_6()
{

}

void CNpcScript::mafl_isruel_7()
{

}

void CNpcScript::mafl_isruel_8()
{

}

void CNpcScript::mafl_isruel_9()
{
	Say( 576);
	Say( 542 );
	Say( 543 );
}

void CNpcScript::mafl_isruel_26()
{
	Say( 544 );
	Say( 545 );
}

void CNpcScript::mafl_isruel_42()
{
	Say( 546 );
	Say( 547 );
	Say( 548 );
}

void CNpcScript::mafl_isruel_43()
{
	Say( 549 ); 
	Say( 550 ); 
	Say( 551);
}

void CNpcScript::mafl_isruel_10()
{
	switch( Random( 2 ) )
	{
		case 0:	PlaySound( 577 ); break;
		case 1:	PlaySound( 578 ); break;
	}
	Speak( NpcId(), 554 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Flarine City Mechanic Engineer
// File			: MaFl_Jeff.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_jeff_0()
{
	Speak( NpcId(), 579 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_jeff_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_jeff_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_jeff_3()
{
	Say( 580 );
}

void CNpcScript::mafl_jeff_4()
{
	Say( 581 );
}

void CNpcScript::mafl_jeff_5()
{
	
}

void CNpcScript::mafl_jeff_6()
{
	
}

void CNpcScript::mafl_jeff_8()
{
	
}

void CNpcScript::mafl_jeff_9()
{
	Say( 582 );	
}

void CNpcScript::mafl_jeff_10()
{
	Speak( NpcId(), 583 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Flarine Apprenticeship Cook
// File			: MaFl_Juglin.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_juglin_0()
{
	Speak( NpcId(), 584 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_juglin_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_juglin_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_juglin_3()
{
	Say( 107 );
}

void CNpcScript::mafl_juglin_4()
{
	Say( 585 );
}

void CNpcScript::mafl_juglin_5()
{
	
}

void CNpcScript::mafl_juglin_6()
{
	
}

void CNpcScript::mafl_juglin_8()
{
	
}

void CNpcScript::mafl_juglin_9()
{
	Say( 586 );	
}

void CNpcScript::mafl_juglin_10()
{
	Speak( NpcId(), 587 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.2( Modify060803 )
// Author		: Jimmie
// Title		: Flarine City Office Manager
// File			: MaFl_Juria.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_juria_0()
{
	Speak( NpcId(), 588 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_juria_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_juria_2()
{
	AddKey( 9 );
	//AddKey( 37 );
	AddKey( 10 );		
}

void CNpcScript::mafl_juria_3()
{
	Say( 589 );
}

void CNpcScript::mafl_juria_4()
{
	Say( 590 );
}

void CNpcScript::mafl_juria_5()
{
	
}

void CNpcScript::mafl_juria_6()
{
	
}

void CNpcScript::mafl_juria_8()
{
	
}

void CNpcScript::mafl_juria_9()
{
	Say( 591 );	
}

void CNpcScript::mafl_juria_37()
{
	InitStat();	
}

void CNpcScript::mafl_juria_10()
{
	Speak( NpcId(), 65 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Dradpet Manager
// File			: MaFl_Kanane.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_kanane_0()
{
	Speak( NpcId(), 505 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_kanane_1()
{
	if( GetPlayerLvl() >= 40 )
	{
		if( IsWormonServer() == TRUE )
		{
		
			if( IsSetQuest( QUEST_DREADMUTE ) == TRUE && GetQuestState( QUEST_DREADMUTE ) == QS_END )
			{
				if( IsParty() == TRUE )
				{
					if( MonHuntStartParty( QUEST_BOSS_LV3, QS_BEGIN, QS_END, 1 ) == FALSE )
					{
						Say( 506 );
						AddKey( 10 );
					}
			

					else if( MonHuntStartParty( QUEST_BOSS_LV3, QS_BEGIN, QS_END, 1 ) == TRUE )
					{
								
					}

					else
					{
						Say( 507 );
						AddKey( 10 );	
					}
				}

				else
				{
					Say( 508 );	
					AddKey( 10 );
				}

			}
				
			else
			{
				Say( 509);
				Say( 510 );
				AddKey( 10 );	
			}
		}

		else
		{
			PrintSystemMessage( TID_GAME_NOTPLAYSERVER );
			Exit();	
		}
	}

	else
	{
		Say( 511 );
		AddKey( 10 );	
	}
}

void CNpcScript::mafl_kanane_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_kanane_3()
{
	Say( 107 );
}

void CNpcScript::mafl_kanane_4()
{
	//Say( "" );
}

void CNpcScript::mafl_kanane_5()
{
	
}

void CNpcScript::mafl_kanane_6()
{
	
}

void CNpcScript::mafl_kanane_8()
{
	
}

void CNpcScript::mafl_kanane_9()
{
	Say( 512 );	
}

void CNpcScript::mafl_kanane_10()
{
	Speak( NpcId(), 513 );
	Exit();
}

//Script End




//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Assist Traning Instructor
// File			: MaFl_Kidmen.txt.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_kidmen_0()
{
	Speak( NpcId(), 592 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_kidmen_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_kidmen_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_kidmen_3()
{
	Say( 107 );
}

void CNpcScript::mafl_kidmen_4()
{
	Say( 593 );
}

void CNpcScript::mafl_kidmen_5()
{
	
}

void CNpcScript::mafl_kidmen_6()
{
	
}

void CNpcScript::mafl_kidmen_8()
{
	if( GetQuestState(QUEST_VOCASS_TRN3) == QS_END && GetPlayerJob() == 0 && GetPlayerLvl() == 15 )
	{
		ChangeJob(3);
		InitStat();
	}

	else
	{
		Exit();
	}
}

void CNpcScript::mafl_kidmen_9()
{
	Say( 594 );	
}

void CNpcScript::mafl_kidmen_10()
{
	Speak( NpcId(), 595 );
	Exit();
}

//Script End

//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040418_003_v1
// Title    : One of 8 Sages 
// Link NPC : Madrigal Flaris Kimel
// File Name: MaFl_Kimel.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mafl_kimel_0()
{
	Speak( NpcId(), 596 );
	SetScriptTimer( 12 );
}

void CNpcScript::mafl_kimel_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_kimel_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_kimel_3()
{
	Say ( 597 );
}

void CNpcScript::mafl_kimel_4()
{
	Say ( 598 );
}

void CNpcScript::mafl_kimel_5()
{

}

void CNpcScript::mafl_kimel_6()
{
	
}

void CNpcScript::mafl_kimel_7()
{

}

void CNpcScript::mafl_kimel_8()
{

}

void CNpcScript::mafl_kimel_9()
{
	Say ( 599 );
}

void CNpcScript::mafl_kimel_10()
{
	Speak( NpcId(), 600 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Manager of Plane of Bubble
// File			: MaFl_Kurumin.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_kurumin_0()
{
	Speak( NpcId(), 601 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_kurumin_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_kurumin_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_kurumin_3()
{
	Say( 107 );
}

void CNpcScript::mafl_kurumin_4()
{
	Say( 602 );
}

void CNpcScript::mafl_kurumin_5()
{
	
}

void CNpcScript::mafl_kurumin_6()
{
	
}

void CNpcScript::mafl_kurumin_8()
{
	
}

void CNpcScript::mafl_kurumin_9()
{
	Say( 603 );	
}

void CNpcScript::mafl_kurumin_10()
{
	Speak( NpcId(), 65 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Mercenery Hi-Dwarpet
// File			: MaFl_Langdrong.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_langdrong_0()
{
	Speak( NpcId(), 604 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_langdrong_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_langdrong_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_langdrong_3()
{
	Say( 107 );
}

void CNpcScript::mafl_langdrong_4()
{
	Say( 605 );
}

void CNpcScript::mafl_langdrong_5()
{
	
}

void CNpcScript::mafl_langdrong_6()
{
	
}

void CNpcScript::mafl_langdrong_8()
{
	
}

void CNpcScript::mafl_langdrong_9()
{
	Say( 606 );	
}

void CNpcScript::mafl_langdrong_10()
{
	Speak( NpcId(), 607 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Flarine Lodelight Manager
// File			: MaFl_LodeLight.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_lodelight_0()
{
	Speak( NpcId(), 296 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_lodelight_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_lodelight_2()
{
	AddKey( 9 );
	AddKey( 10 );
	AddKey( 36 );		
}

void CNpcScript::mafl_lodelight_3()
{
	Say( 297 );
}

void CNpcScript::mafl_lodelight_4()
{
	Say( 298 );
}

void CNpcScript::mafl_lodelight_5()
{
	
}

void CNpcScript::mafl_lodelight_6()
{
	
}

void CNpcScript::mafl_lodelight_8()
{
	
}

void CNpcScript::mafl_lodelight_9()
{
	Say( 608 );	
}

void CNpcScript::mafl_lodelight_36()
{
	Say( 300 );
}

void CNpcScript::mafl_lodelight_10()
{
	Speak( NpcId(), 301 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Flarine City Food Shop Manager
// File			: MaFl_Losha.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_losha_0()
{
	Speak( NpcId(), 609 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_losha_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_losha_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_losha_3()
{
	Say( 610 );
}

void CNpcScript::mafl_losha_4()
{
	Say( 611 );
}

void CNpcScript::mafl_losha_5()
{
	
}

void CNpcScript::mafl_losha_6()
{
	
}

void CNpcScript::mafl_losha_8()
{
	
}

void CNpcScript::mafl_losha_9()
{
	Say( 612 );	
}

void CNpcScript::mafl_losha_10()
{
	Speak( NpcId(), 613 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Flarine Food Shop Cooker
// File			: MaFl_Loyah.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_loyah_0()
{
	Speak( NpcId(), 614 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_loyah_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_loyah_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_loyah_3()
{
	Say( 107 );
}

void CNpcScript::mafl_loyah_4()
{
	Say( 615 );
}

void CNpcScript::mafl_loyah_5()
{
	
}

void CNpcScript::mafl_loyah_6()
{
	
}

void CNpcScript::mafl_loyah_8()
{
	
}

void CNpcScript::mafl_loyah_9()
{
	Say( 616 );	
}

void CNpcScript::mafl_loyah_10()
{
	Speak( NpcId(), 617 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Flarine Shield Shop Owner
// File			: MaFl_Luda.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_luda_0()
{
	Speak( NpcId(), 618 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_luda_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_luda_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_luda_3()
{
	
}

void CNpcScript::mafl_luda_4()
{
	Say( 619 );
}

void CNpcScript::mafl_luda_5()
{

}

void CNpcScript::mafl_luda_6()
{

}

void CNpcScript::mafl_luda_7()
{

}

void CNpcScript::mafl_luda_8()
{

}

void CNpcScript::mafl_luda_9()
{
	Say( 620 );
}

void CNpcScript::mafl_luda_10()
{
	Speak( NpcId(), 621 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Flarine City General Shop Owner
// File			: MaFl_Lui.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_lui_0()
{
	Speak( NpcId(), 622 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_lui_1()
{
		switch( Random( 3 ) )
	{
		case 0:	PlaySound( 1036 ); break;
		case 1:	PlaySound( 1037 ); break;
		case 2:	PlaySound( 1038 ); break;
	}
	LaunchQuest();
}

void CNpcScript::mafl_lui_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_lui_3()
{
	Say( 623 );
}

void CNpcScript::mafl_lui_4()
{
	Say( 624 );
}

void CNpcScript::mafl_lui_5()
{
	
}

void CNpcScript::mafl_lui_6()
{
	
}

void CNpcScript::mafl_lui_8()
{
	
}

void CNpcScript::mafl_lui_9()
{
	Say( 625 );	
}

void CNpcScript::mafl_lui_10()
{
		switch( Random( 2 ) )
	{
		case 0:	PlaySound( 1039 ); break;
		case 1:	PlaySound( 1040 ); break;
	}
	Speak( NpcId(), 626 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Assist Master
// File			: MaFl_Maki.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_maki_0()
{
	Speak( NpcId(), 627 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_maki_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_maki_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_maki_3()
{
	Say( 107 );
}

void CNpcScript::mafl_maki_4()
{
	Say( 628 );
}

void CNpcScript::mafl_maki_5()
{
	
}

void CNpcScript::mafl_maki_6()
{
	
}

void CNpcScript::mafl_maki_8()
{
	
}

void CNpcScript::mafl_maki_9()
{
	Say( 629 );	
}

void CNpcScript::mafl_maki_10()
{
	Speak( NpcId(), 630 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Flarine City Magic Shop Owner
// File			: MaFl_Marche.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_marche_0()
{
	Speak( NpcId(), 631 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_marche_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_marche_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_marche_3()
{
	Say( 632 );
}

void CNpcScript::mafl_marche_4()
{
	Say( 633 );
}

void CNpcScript::mafl_marche_5()
{
	
}

void CNpcScript::mafl_marche_6()
{
	
}

void CNpcScript::mafl_marche_8()
{
	
}

void CNpcScript::mafl_marche_9()
{
	Say( 634 );	
}

void CNpcScript::mafl_marche_10()
{
	Speak( NpcId(), 635 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Madrigal History Scholar
// File			: MaFl_Martinyc.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_martinyc_0()
{
	Speak( NpcId(), 636 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_martinyc_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_martinyc_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_martinyc_3()
{
	Say( 107 );
}

void CNpcScript::mafl_martinyc_4()
{
	Say( 637 );
}

void CNpcScript::mafl_martinyc_5()
{
	
}

void CNpcScript::mafl_martinyc_6()
{
	
}

void CNpcScript::mafl_martinyc_8()
{
	
}

void CNpcScript::mafl_martinyc_9()
{
	Say( 638 );	
}

void CNpcScript::mafl_martinyc_10()
{
	Speak( NpcId(), 639 );
	Exit();
}

//Script End


//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040406_001_v1
// Title    : Request Box Quest Agent
// Link NPC : Madrigal Flaris Mikyel
// File Name: MaFl_Mikyel.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mafl_mikyel_0()
{
	Speak( NpcId(), 640 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_mikyel_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_mikyel_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_mikyel_3()
{
	Say ( 308 );
}

void CNpcScript::mafl_mikyel_4()
{
	Say ( 309 );
}

void CNpcScript::mafl_mikyel_5()
{

}

void CNpcScript::mafl_mikyel_6()
{

}

void CNpcScript::mafl_mikyel_7()
{

}

void CNpcScript::mafl_mikyel_8()
{

}

void CNpcScript::mafl_mikyel_9()
{
	Say ( 641 );
}

void CNpcScript::mafl_mikyel_10()
{
	Speak( NpcId(), 65 );
	Exit();
}
//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Mercenery Traning Assist
// File			: MaFl_Mustang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_mustang_0()
{
	Speak( NpcId(), 642 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_mustang_1()
{
	if( GetQuestState( QUEST_VOCASS_BFTRN ) >= 0 || GetQuestState( QUEST_VOCMAG_BFTRN ) >= 0 || GetQuestState( QUEST_VOCACR_BFTRN ) >= 0)
	{
		if( GetQuestState( QUEST_SCE_MDRIGALTEACHER2 ) >= 0 )
			LaunchQuest();
		else
			SayQuest( QUEST_VOCACR_BFTRN, QSAY_EXTRA01 );
		AddKey( 9 );
		AddKey( 10 );
	}
		
	else
	{ 
		LaunchQuest();
	}
}

void CNpcScript::mafl_mustang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_mustang_3()
{
	Say( 107 );
}

void CNpcScript::mafl_mustang_4()
{
	Say( 643 );
}

void CNpcScript::mafl_mustang_5()
{
	
}

void CNpcScript::mafl_mustang_6()
{
	
}

void CNpcScript::mafl_mustang_8()
{
	
}

void CNpcScript::mafl_mustang_9()
{
	Say( 644);	
}

void CNpcScript::mafl_mustang_10()
{
	Speak( NpcId(), 645 );
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Paul's Wife
// File			: MaFl_Nina.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_nina_0()
{
	Speak( NpcId(), 646 );	
	SetScriptTimer( 12 );
}

void CNpcScript::mafl_nina_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_nina_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_nina_3()
{
	Say( 107 );
}

void CNpcScript::mafl_nina_4()
{
	Say( 647 );
}

void CNpcScript::mafl_nina_5()
{
	
}

void CNpcScript::mafl_nina_6()
{
	
}

void CNpcScript::mafl_nina_8()
{
	
}

void CNpcScript::mafl_nina_9()
{
	Say( 648 );	
}

void CNpcScript::mafl_nina_10()
{
	Speak( NpcId(), 649 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Flaris Observertory Scholar
// File			: MaFl_Noier.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_noier_0()
{
	Speak( NpcId(), 650 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_noier_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_noier_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_noier_3()
{
	Say( 651 );
}

void CNpcScript::mafl_noier_4()
{
	Say( 652 );
}

void CNpcScript::mafl_noier_5()
{
	
}

void CNpcScript::mafl_noier_6()
{
	
}

void CNpcScript::mafl_noier_8()
{
	
}

void CNpcScript::mafl_noier_9()
{
	Say( 653 );	
}

void CNpcScript::mafl_noier_10()
{
	Speak( NpcId(), 654 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Madrigal Flarine Official 
// File			: MaFl_Official.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_official_0()
{
	Speak( NpcId(), 655 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_official_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_official_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_official_3()
{
	Say( 107 );
}

void CNpcScript::mafl_official_4()
{
	Say( 656 );
}

void CNpcScript::mafl_official_5()
{
	
}

void CNpcScript::mafl_official_6()
{
	
}

void CNpcScript::mafl_official_8()
{
	
}

void CNpcScript::mafl_official_9()
{
	Say( 657 );	
}

void CNpcScript::mafl_official_10()
{
	Speak( NpcId(), 658 );
	Exit();
}

//Script End




//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Kyon Warrior
// File			: MaFl_Phoho.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_phoho_0()
{
	Speak( NpcId(), 659 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_phoho_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_phoho_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_phoho_3()
{
	Say( 107 );
}

void CNpcScript::mafl_phoho_4()
{
	Say( 660 );
}

void CNpcScript::mafl_phoho_5()
{
	
}

void CNpcScript::mafl_phoho_6()
{
	
}

void CNpcScript::mafl_phoho_8()
{
	
}

void CNpcScript::mafl_phoho_9()
{
	Say( 661 );	
}

void CNpcScript::mafl_phoho_10()
{
	Speak( NpcId(), 662 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Paul's son
// File			: MaFl_PiPi.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_pipi_0()
{
	Speak( NpcId(), 663 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_pipi_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_pipi_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_pipi_3()
{
	Say( 107 );
}

void CNpcScript::mafl_pipi_4()
{
	Say( 664 );
}

void CNpcScript::mafl_pipi_5()
{
	
}

void CNpcScript::mafl_pipi_6()
{
	
}

void CNpcScript::mafl_pipi_8()
{
	
}

void CNpcScript::mafl_pipi_9()
{
	Say( 665 );	
}

void CNpcScript::mafl_pipi_10()
{
	Speak( NpcId(), 65 );
	Exit();
}

//Script End

//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040418_002_v1
// Title    : One of 8 Sages 
// Link NPC : Madrigal Flaris Radyon
// File Name: MaFl_Radyon.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::mafl_radyon_0()
{
	Speak( NpcId(), 666 );
	SetScriptTimer( 10 );
}

void CNpcScript::mafl_radyon_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_radyon_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_radyon_3()
{
	Say ( 597 );
}

void CNpcScript::mafl_radyon_4()
{
	Say ( 667 );
}

void CNpcScript::mafl_radyon_5()
{

}

void CNpcScript::mafl_radyon_6()
{
	
}

void CNpcScript::mafl_radyon_7()
{

}

void CNpcScript::mafl_radyon_8()
{

}

void CNpcScript::mafl_radyon_9()
{
	Say ( 668 );
}

void CNpcScript::mafl_radyon_10()
{
	Speak( NpcId(), 669 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Flarine Billionaire
// File			: MaFl_Rudvihil.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_rudvihil_0()
{
	Speak( NpcId(), 670 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_rudvihil_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_rudvihil_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_rudvihil_3()
{
	Say( 107 );	
}

void CNpcScript::mafl_rudvihil_4()
{
	Say( 671 );
}

void CNpcScript::mafl_rudvihil_5()
{

}

void CNpcScript::mafl_rudvihil_6()
{

}

void CNpcScript::mafl_rudvihil_7()
{

}

void CNpcScript::mafl_rudvihil_8()
{

}

void CNpcScript::mafl_rudvihil_9()
{
	Say( 672 );
}

void CNpcScript::mafl_rudvihil_10()
{
	Speak( NpcId(), 673 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Head of Chivalrous robber Troop
// File			: MaFl_Ryupang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_ryupang_0()
{
	Speak( NpcId(), 674 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_ryupang_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_ryupang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_ryupang_3()
{
	Say( 107 );
}

void CNpcScript::mafl_ryupang_4()
{
	Say( 675 );
}

void CNpcScript::mafl_ryupang_5()
{
	
}

void CNpcScript::mafl_ryupang_6()
{
	
}

void CNpcScript::mafl_ryupang_8()
{
	
}

void CNpcScript::mafl_ryupang_9()
{
	Say( 676 );	
}

void CNpcScript::mafl_ryupang_10()
{
	Speak( NpcId(), 677 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Astronomy Scholar 
// File			: MaFl_Segho.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_segho_0()
{
	Speak( NpcId(), 678 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_segho_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_segho_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_segho_3()
{
	Say( 107 );
}

void CNpcScript::mafl_segho_4()
{
	Say( 679 );
}

void CNpcScript::mafl_segho_5()
{
	
}

void CNpcScript::mafl_segho_6()
{
	
}

void CNpcScript::mafl_segho_8()
{
	
}

void CNpcScript::mafl_segho_9()
{
	Say( 680 );	
}

void CNpcScript::mafl_segho_10()
{
	Speak( NpcId(), 681 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Great Wise Man
// File			: MaFl_SgRadion.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_sgradion_0()
{
	Speak( NpcId(), 682 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_sgradion_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_sgradion_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_sgradion_3()
{
	Say( 683 );
}

void CNpcScript::mafl_sgradion_4()
{
	Say( 684 );
}

void CNpcScript::mafl_sgradion_5()
{
	
}

void CNpcScript::mafl_sgradion_6()
{
	
}

void CNpcScript::mafl_sgradion_8()
{
	
}

void CNpcScript::mafl_sgradion_9()
{
	Say( 685 );	
}

void CNpcScript::mafl_sgradion_10()
{
	Speak( NpcId(), 686 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Flarine Viniger Rice Cooker(Beginner)
// File			: MaFl_SsoTta.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_ssotta_0()
{
	Speak( NpcId(), 687 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_ssotta_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_ssotta_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_ssotta_3()
{
	Say( 107 );
}

void CNpcScript::mafl_ssotta_4()
{
	Say( 688 );
}

void CNpcScript::mafl_ssotta_5()
{
	
}

void CNpcScript::mafl_ssotta_6()
{
	
}

void CNpcScript::mafl_ssotta_8()
{
	
}

void CNpcScript::mafl_ssotta_9()
{
	Say( 689 );	
}

void CNpcScript::mafl_ssotta_10()
{
	Speak( NpcId(), 690 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Commander of Kion
// File			: MaFl_Teshar.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_teshar_0()
{
	Speak( NpcId(), 691 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_teshar_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_teshar_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_teshar_3()
{
	Say( 107 );
}

void CNpcScript::mafl_teshar_4()
{
	Say( 692 );
}

void CNpcScript::mafl_teshar_5()
{
	
}

void CNpcScript::mafl_teshar_6()
{
	
}

void CNpcScript::mafl_teshar_8()
{
	
}

void CNpcScript::mafl_teshar_9()
{
	Say( 693 );	
}

void CNpcScript::mafl_teshar_10()
{
	Speak( NpcId(), 694 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Kyon Warrior
// File			: MaFl_Tomba.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_tomba_0()
{
	Speak( NpcId(), 695 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_tomba_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_tomba_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_tomba_3()
{
	Say( 107 );
}

void CNpcScript::mafl_tomba_4()
{
	Say( 696 );
}

void CNpcScript::mafl_tomba_5()
{
	
}

void CNpcScript::mafl_tomba_6()
{
	
}

void CNpcScript::mafl_tomba_8()
{
	
}

void CNpcScript::mafl_tomba_9()
{
	Say( 697 );	
}

void CNpcScript::mafl_tomba_10()
{
	Speak( NpcId(), 698 );
	Exit();
}

//Script End


///Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Acrobat Hi-Dwarpet
// File			: MaFl_Tucani.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_tucani_0()
{
	Speak( NpcId(), 699 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_tucani_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_tucani_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_tucani_3()
{
	Say ( 700 );
}

void CNpcScript::mafl_tucani_4()
{
	Say ( 701 );
}

void CNpcScript::mafl_tucani_5()
{

}

void CNpcScript::mafl_tucani_6()
{

}

void CNpcScript::mafl_tucani_7()
{

}

void CNpcScript::mafl_tucani_8()
{

}

void CNpcScript::mafl_tucani_9()
{
	Say ( 702 );
}

void CNpcScript::mafl_tucani_10()
{
	Speak( NpcId(), 703 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060906 )
// Author		: Jimmie
// Title		: Madrigal Ancient Magic Box
// File			: MaFl_Ancimys.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_ancimys_0()
{
	Speak( NpcId(), 879 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_ancimys_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_ancimys_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_ancimys_3()
{
	Say( 880 );
}

void CNpcScript::mafl_ancimys_4()
{
	Say( 881 );
}

void CNpcScript::mafl_ancimys_5()
{
	
}

void CNpcScript::mafl_ancimys_6()
{
	
}

void CNpcScript::mafl_ancimys_8()
{
	
}

void CNpcScript::mafl_ancimys_9()
{
	Say( 882 );	
}

void CNpcScript::mafl_ancimys_10()
{
	Speak( NpcId(), 883 );
	Exit();
}


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060803 )
// Author		: Jimmie
// Title		: Madrigal Ancient Magic Box
// File			: MaMa_Ancimys.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mama_ancimys_0()
{
	Speak( NpcId(), 704 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mama_ancimys_1()
{
	LaunchQuest();
}

void CNpcScript::mama_ancimys_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mama_ancimys_3()
{
	Say( 705 );
}

void CNpcScript::mama_ancimys_4()
{
	Say( 706 );
}

void CNpcScript::mama_ancimys_5()
{
	
}

void CNpcScript::mama_ancimys_6()
{
	
}

void CNpcScript::mama_ancimys_8()
{
	
}

void CNpcScript::mama_ancimys_9()
{
	Say( 707 );	
}

void CNpcScript::mama_ancimys_10()
{
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Sain City Helper
// File			: MaSa_Ann.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_ann_0()
{
	Speak( NpcId(), 535 );
	SetScriptTimer( 15 );
}

void CNpcScript::masa_ann_1()
{
	LaunchQuest();
}

void CNpcScript::masa_ann_2()
{
	AddKey( 9 );
	AddKey( 26 );
	AddKey( 42 );
	AddKey( 43 );
	AddKey( 10 );		
}

void CNpcScript::masa_ann_3()
{
	Say( 589 );
}

void CNpcScript::masa_ann_4()
{
	Say( 540 );
}

void CNpcScript::masa_ann_5()
{
	
}

void CNpcScript::masa_ann_6()
{
	
}

void CNpcScript::masa_ann_8()
{
	
}

void CNpcScript::masa_ann_9()
{
	Say( 708);
}

void CNpcScript::masa_ann_26()
{
	Say(709);
	Say(710);
}

void CNpcScript::masa_ann_42()
{
	Say( 546 );
	Say( 547 );
	Say( 548 );
}

void CNpcScript::masa_ann_43()
{
	Say( 549 ); 
	Say( 550 ); 
	Say( 551);
}

void CNpcScript::masa_ann_10()
{
	Speak( NpcId(), 65 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Sain City General Shop Owner
// File			: MaSa_Bill.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_bill_0()
{
	Speak( NpcId(), 711 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_bill_1()
{
	LaunchQuest();
}

void CNpcScript::masa_bill_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_bill_3()
{
	Say( 712 );
}

void CNpcScript::masa_bill_4()
{
	Say( 713 );
}

void CNpcScript::masa_bill_5()
{
	
}

void CNpcScript::masa_bill_6()
{
	
}

void CNpcScript::masa_bill_8()
{
	
}

void CNpcScript::masa_bill_9()
{
	Say( 714 );	
}

void CNpcScript::masa_bill_10()
{
	Speak( NpcId(), 715 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Bowl Shop Owner
// File			: MaSa_Bowler.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_bowler_0()
{
	Speak( NpcId(), 716 );
	SetScriptTimer( 15 );
}

void CNpcScript::masa_bowler_1()
{
	LaunchQuest();
}

void CNpcScript::masa_bowler_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::masa_bowler_3()
{
	Say ( 107 );
}

void CNpcScript::masa_bowler_4()
{
	Say ( 717 );
}

void CNpcScript::masa_bowler_5()
{

}

void CNpcScript::masa_bowler_6()
{

}

void CNpcScript::masa_bowler_7()
{

}

void CNpcScript::masa_bowler_8()
{

}

void CNpcScript::masa_bowler_9()
{
	Say ( 718 );
}

void CNpcScript::masa_bowler_10()
{
	Speak( NpcId(), 719 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Sain City Armor Shop Owner
// File			: MaSa_Bozman.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_bozman_0()
{
	Speak( NpcId(), 720 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_bozman_1()
{
	LaunchQuest();
}

void CNpcScript::masa_bozman_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_bozman_3()
{
	Say( 721 );
}

void CNpcScript::masa_bozman_4()
{
	Say( 722 );
}

void CNpcScript::masa_bozman_5()
{
	
}

void CNpcScript::masa_bozman_6()
{
	
}

void CNpcScript::masa_bozman_8()
{
	
}

void CNpcScript::masa_bozman_9()
{
	Say( 723 );	
}

void CNpcScript::masa_bozman_10()
{
	Speak( NpcId(), 724 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modified 20060622 )
// Author		: Jimmie
// Title		: Troupe Master
// File			: MaSa_Brodeay.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_brodeay_0()
{
	Speak( NpcId(), 725 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_brodeay_1()
{
	LaunchQuest();
}

void CNpcScript::masa_brodeay_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_brodeay_3()
{
	Say( 107 );
}

void CNpcScript::masa_brodeay_4()
{
	Say( 726 );
}

void CNpcScript::masa_brodeay_5()
{
	
}

void CNpcScript::masa_brodeay_6()
{
	
}

void CNpcScript::masa_brodeay_8()
{
	
}

void CNpcScript::masa_brodeay_9()
{
	Say( 727 );	
}

void CNpcScript::masa_brodeay_10()
{
	Speak( NpcId(), 728 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Sain City Weapon Shop Owner
// File			: MaSa_Bulrox.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_bulrox_0()
{
	Speak( NpcId(), 729 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_bulrox_1()
{
		switch( Random( 3 ) )
	{
		case 0:	PlaySound( 1041 ); break;
		case 1:	PlaySound( 1042 ); break;
		case 2:	PlaySound( 1043 ); break;
	}
	LaunchQuest();
}

void CNpcScript::masa_bulrox_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_bulrox_3()
{
	Say( 730 );
}

void CNpcScript::masa_bulrox_4()
{
	Say( 731 );
}

void CNpcScript::masa_bulrox_5()
{
	
}

void CNpcScript::masa_bulrox_6()
{
	
}

void CNpcScript::masa_bulrox_8()
{
	
}

void CNpcScript::masa_bulrox_9()
{
	Say( 732 );	
}

void CNpcScript::masa_bulrox_10()
{
		switch( Random( 2 ) )
	{
		case 0:	PlaySound( 1044 ); break;
		case 1:	PlaySound( 1045 ); break;
	}
	Speak( NpcId(), 733 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Saincity Shield Shop Owner
// File			: MaSa_Colack.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_colack_0()
{
	Speak( NpcId(), 734 );
	SetScriptTimer( 15 );
}

void CNpcScript::masa_colack_1()
{
	LaunchQuest();
}

void CNpcScript::masa_colack_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::masa_colack_3()
{
	
}

void CNpcScript::masa_colack_4()
{
	Say( 735 );
}

void CNpcScript::masa_colack_5()
{

}

void CNpcScript::masa_colack_6()
{

}

void CNpcScript::masa_colack_7()
{

}

void CNpcScript::masa_colack_8()
{

}

void CNpcScript::masa_colack_9()
{
	Say( 736 );
}

void CNpcScript::masa_colack_10()
{
	Speak( NpcId(), 737 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Madrigal Bionomics Investigator 
// File			: MaSa_DrEstern.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_drestern_0()
{
	Speak( NpcId(), 738 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_drestern_1()
{
	LaunchQuest();
}

void CNpcScript::masa_drestern_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_drestern_3()
{
	Say( 739 );
}

void CNpcScript::masa_drestern_4()
{
	Say( 740 );
}

void CNpcScript::masa_drestern_5()
{
	
}

void CNpcScript::masa_drestern_6()
{
	
}

void CNpcScript::masa_drestern_8()
{
	
}

void CNpcScript::masa_drestern_9()
{
	Say( 741 );	
}

void CNpcScript::masa_drestern_10()
{
	Speak( NpcId(), 742 );
	Exit();
}

//Script End

void CNpcScript::masa_furan_0()
{
	Speak( NpcId(), 194 );
	SetScriptTimer( 15 );
}

void CNpcScript::masa_furan_1()
{
	LaunchQuest();
}

void CNpcScript::masa_furan_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::masa_furan_3()
{
	Say( 107 );
}

void CNpcScript::masa_furan_4()
{
	Say( 195 );
}

void CNpcScript::masa_furan_5()
{

}

void CNpcScript::masa_furan_6()
{
	
}

void CNpcScript::masa_furan_7()
{

}

void CNpcScript::masa_furan_8()
{

}

void CNpcScript::masa_furan_9()
{
	Say( 196 );
}

void CNpcScript::masa_furan_10()
{
	Speak( NpcId(), 197 );
	Exit();
}
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Madrigal History Association Master
// File			: MaSa_Gothante.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_gothante_0()
{
	Speak( NpcId(), 743 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_gothante_1()
{
	LaunchQuest();
}

void CNpcScript::masa_gothante_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_gothante_3()
{
	Say( 107 );
}

void CNpcScript::masa_gothante_4()
{
	Say( 744 );
}

void CNpcScript::masa_gothante_5()
{
	
}

void CNpcScript::masa_gothante_6()
{
	
}

void CNpcScript::masa_gothante_8()
{
	
}

void CNpcScript::masa_gothante_9()
{
	Say( 745 );	
}

void CNpcScript::masa_gothante_10()
{
	Speak( NpcId(), 746 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Magicion Master
// File			: MaSa_Hee.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_hee_0()
{
	Speak( NpcId(), 747 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_hee_1()
{
	LaunchQuest();
}

void CNpcScript::masa_hee_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_hee_3()
{
	Say( 107 );
}

void CNpcScript::masa_hee_4()
{
	Say( 748 );
}

void CNpcScript::masa_hee_5()
{
	
}

void CNpcScript::masa_hee_6()
{
	
}

void CNpcScript::masa_hee_8()
{
	
}

void CNpcScript::masa_hee_9()
{
	Say( 749 );	
}

void CNpcScript::masa_hee_10()
{
	Speak( NpcId(), 750 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Saincity Assistance Manager
// File			: MaSa_Helena.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_helena_0()
{
	Speak( NpcId(), 751 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_helena_1()
{
	if(  IsGuildMaster() == 1 )
	{
		LaunchQuest();
	}
		
	else if( IsSetQuest( QUEST_CREGUILD ) == TRUE && GetQuestState( QUEST_CREGUILD ) == QS_END && GetPlayerGold() >= 3000000 )	
	{
		if( IsPartyMaster() == 1 && GetPartyNum() >= 3  && IsPartyGuild() == 0 )
		{

			Say(864);
			AddKey(866);
			AddKey(865);
		}
		
		else
		{
			Say( 863 );
			AddKey( 10 );
		} 
	}

	else
	{	
		LaunchQuest();
	}
}

void CNpcScript::masa_helena_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_helena_3()
{
	Say( 107 );
}

void CNpcScript::masa_helena_4()
{
	Say( 754 );
}

void CNpcScript::masa_helena_5()
{
	
}

void CNpcScript::masa_helena_6()
{
	
}

void CNpcScript::masa_helena_8()
{
	if( IsSetQuest( QUEST_CREGUILD ) == TRUE && GetQuestState( QUEST_CREGUILD ) == QS_END && GetPlayerGold() >= 3000000 )	
	{
		if( IsPartyMaster() == 1 && GetPartyNum() >= 3 && IsPartyGuild() == 0   )
		{
			RemoveGold( 3000000 );
			CreateGuild();
			Say(752);
		}
		
		else
		{
			Say( 753 );
		} 
	}

	else
	{	
		Say( 755 );
	}
}

void CNpcScript::masa_helena_9()
{
	Say( 756 );	
}

void CNpcScript::masa_helena_10()
{
	Speak( NpcId(), 757 );
	Exit();
}

void CNpcScript::masa_helena_865()
{
	if( IsSetQuest( QUEST_CREGUILD ) == TRUE && GetQuestState( QUEST_CREGUILD ) == QS_END && GetPlayerGold() >= 3000000 )
	{

		if( IsPartyMaster() == 1 && GetPartyNum() >= 3 && IsPartyGuild() == 0  )
		{
			RemoveGold( 3000000 );
			CreateGuild();
			Say(752);
			AddKey( 10 );
			RemoveKey( 865 );
		}

		else
		{
			Say( 863 );
			AddKey( 10 );
		} 
	}

	else
	{
		Say( 755 );
	}
}

void CNpcScript::masa_helena_866()
{
	LaunchQuest();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Saincity Appraiser
// File			: MaSa_Helgar.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_helgar_0()
{
	Speak( NpcId(), 758 );
	SetScriptTimer( 15 );
}

void CNpcScript::masa_helgar_1()
{
	LaunchQuest();
}

void CNpcScript::masa_helgar_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::masa_helgar_3()
{
	Say( 107 );	
}

void CNpcScript::masa_helgar_4()
{
	Say( 759 );
}

void CNpcScript::masa_helgar_5()
{

}

void CNpcScript::masa_helgar_6()
{

}

void CNpcScript::masa_helgar_7()
{

}

void CNpcScript::masa_helgar_8()
{

}

void CNpcScript::masa_helgar_9()
{
	Say( 760 );
}

void CNpcScript::masa_helgar_10()
{
	Speak( NpcId(), 761 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Reseacher of Madrigal Lab
// File			: MaSa_Heltung.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_heltung_0()
{
	Speak( NpcId(), 762 );
	SetScriptTimer( 15 );
}

void CNpcScript::masa_heltung_1()
{
	LaunchQuest();
}

void CNpcScript::masa_heltung_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::masa_heltung_3()
{
	Say ( 107 );
}

void CNpcScript::masa_heltung_4()
{
	Say ( 763 );
}

void CNpcScript::masa_heltung_5()
{

}

void CNpcScript::masa_heltung_6()
{

}

void CNpcScript::masa_heltung_7()
{

}

void CNpcScript::masa_heltung_8()
{

}

void CNpcScript::masa_heltung_9()
{
	Say ( 764 );
}

void CNpcScript::masa_heltung_10()
{
	Speak( NpcId(), 765 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Sain City Helper
// File			: MaSa_Herth.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_herth_0()
{
	Speak( NpcId(), 535 );
	SetScriptTimer( 15 );
}

void CNpcScript::masa_herth_1()
{
	LaunchQuest();
}

void CNpcScript::masa_herth_2()
{
	AddKey( 9 );
	AddKey( 26 );
	AddKey( 42 );
	AddKey( 43 );
	AddKey( 10 );		
}

void CNpcScript::masa_herth_3()
{
	Say( 589 );
}

void CNpcScript::masa_herth_4()
{
	Say( 540 );
}

void CNpcScript::masa_herth_5()
{
	
}

void CNpcScript::masa_herth_6()
{
	
}

void CNpcScript::masa_herth_8()
{
	
}

void CNpcScript::masa_herth_9()
{
	Say( 766);
}

void CNpcScript::masa_herth_26()
{
	Say(709);
	Say(710);
}

void CNpcScript::masa_herth_42()
{
	Say( 546 );
	Say( 547 );
	Say( 548 );
}

void CNpcScript::masa_herth_43()
{
	Say( 549 ); 
	Say( 550 ); 
	Say( 551);
}

void CNpcScript::masa_herth_10()
{
	Speak( NpcId(), 65 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Sain City Helper
// File			: MaSa_Herth.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_janne_0()
{
	Speak( NpcId(), 535 );
	SetScriptTimer( 15 );
}

void CNpcScript::masa_janne_1()
{
	LaunchQuest();
}

void CNpcScript::masa_janne_2()
{
	AddKey( 9 );
	AddKey( 26 );
	AddKey( 42 );
	AddKey( 43 );
	AddKey( 10 );		
}

void CNpcScript::masa_janne_3()
{
	Say( 589 );
}

void CNpcScript::masa_janne_4()
{
	Say( 540 );
}

void CNpcScript::masa_janne_5()
{
	
}

void CNpcScript::masa_janne_6()
{
	
}

void CNpcScript::masa_janne_8()
{
	
}

void CNpcScript::masa_janne_9()
{
	Say( 767);
}

void CNpcScript::masa_janne_26()
{
	Say(709);
	Say(710);
}

void CNpcScript::masa_janne_42()
{
	Say( 546 );
	Say( 547 );
	Say( 548 );
}

void CNpcScript::masa_janne_43()
{
	Say( 549 ); 
	Say( 550 ); 
	Say( 551);
}

void CNpcScript::masa_janne_10()
{
	Speak( NpcId(), 65 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Saincity Citizen
// File			: MaSa_JeongHwa.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_jeonghwa_0()
{
	Speak( NpcId(), 768 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_jeonghwa_1()
{
	LaunchQuest();
}

void CNpcScript::masa_jeonghwa_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_jeonghwa_3()
{
	Say( 107 );
}

void CNpcScript::masa_jeonghwa_4()
{
	Say( 769 );
}

void CNpcScript::masa_jeonghwa_5()
{
	
}

void CNpcScript::masa_jeonghwa_6()
{
	
}

void CNpcScript::masa_jeonghwa_8()
{
	
}

void CNpcScript::masa_jeonghwa_9()
{
	Say( 770 );	
}

void CNpcScript::masa_jeonghwa_10()
{
	Speak( NpcId(), 771 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Sain City Food Shop Owner
// File			: MaSa_Karin.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_karin_0()
{
	Speak( NpcId(), 772 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_karin_1()
{
	LaunchQuest();
}

void CNpcScript::masa_karin_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_karin_3()
{
	Say( 773 );
}

void CNpcScript::masa_karin_4()
{
	Say( 774 );
}

void CNpcScript::masa_karin_5()
{
	
}

void CNpcScript::masa_karin_6()
{
	
}

void CNpcScript::masa_karin_8()
{
	
}

void CNpcScript::masa_karin_9()
{
	Say( 775 );	
}

void CNpcScript::masa_karin_10()
{
	Speak( NpcId(), 776 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Sain City Helper
// File			: MaSa_Kimberley.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_kimberley_0()
{
	Speak( NpcId(), 535 );
	SetScriptTimer( 15 );
}

void CNpcScript::masa_kimberley_1()
{
	LaunchQuest();
}

void CNpcScript::masa_kimberley_2()
{
	AddKey( 9 );
	AddKey( 26 );
	AddKey( 42 );
	AddKey( 43 );
	AddKey( 10 );		
}

void CNpcScript::masa_kimberley_3()
{
	Say( 589 );
}

void CNpcScript::masa_kimberley_4()
{
	Say( 540 );
}

void CNpcScript::masa_kimberley_5()
{
	
}

void CNpcScript::masa_kimberley_6()
{
	
}

void CNpcScript::masa_kimberley_8()
{
	
}

void CNpcScript::masa_kimberley_9()
{
	Say( 777);
}

void CNpcScript::masa_kimberley_26()
{
	Say(709);
	Say(710);
}

void CNpcScript::masa_kimberley_42()
{
	Say( 546 );
	Say( 547 );
	Say( 548 );
}

void CNpcScript::masa_kimberley_43()
{
	Say( 549 ); 
	Say( 550 ); 
	Say( 551);
}

void CNpcScript::masa_kimberley_10()
{
	Speak( NpcId(), 65 );
	Exit();
}

//Script End
//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040406_002_v1
// Title    : Request Box Quest Agent
// Link NPC : Madrigal Saintmorning Lancomi
// File Name: MaSa_Lancomi.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//

//Script Begin
void CNpcScript::masa_lancomi_0()
{
	Speak( NpcId(), 778 );
	SetScriptTimer( 15 );
}

void CNpcScript::masa_lancomi_1()
{
	LaunchQuest();
}

void CNpcScript::masa_lancomi_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::masa_lancomi_3()
{
	Say ( 308 );
}

void CNpcScript::masa_lancomi_4()
{
	Say ( 309 );
}

void CNpcScript::masa_lancomi_5()
{

}

void CNpcScript::masa_lancomi_6()
{

}

void CNpcScript::masa_lancomi_7()
{

}

void CNpcScript::masa_lancomi_8()
{

}

void CNpcScript::masa_lancomi_9()
{
	Say ( 779 );
}

void CNpcScript::masa_lancomi_10()
{
	Speak( NpcId(), 65 );
	Exit();
}
	
//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Sain City Helper
// File			: MaSa_Leann.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_leann_0()
{
	Speak( NpcId(), 535 );
	SetScriptTimer( 15 );
}

void CNpcScript::masa_leann_1()
{
	LaunchQuest();
}

void CNpcScript::masa_leann_2()
{
	AddKey( 9 );
	AddKey( 26 );
	AddKey( 42 );
	AddKey( 43 );
	AddKey( 10 );		
}

void CNpcScript::masa_leann_3()
{
	Say( 589 );
}

void CNpcScript::masa_leann_4()
{
	Say( 540 );
}

void CNpcScript::masa_leann_5()
{
	
}

void CNpcScript::masa_leann_6()
{
	
}

void CNpcScript::masa_leann_8()
{
	
}

void CNpcScript::masa_leann_9()
{
	Say( 780);
}

void CNpcScript::masa_leann_26()
{
	Say(709);
	Say(710);
}

void CNpcScript::masa_leann_42()
{
	Say( 546 );
	Say( 547 );
	Say( 548 );
}

void CNpcScript::masa_leann_43()
{
	Say( 549 ); 
	Say( 550 ); 
	Say( 551);
}

void CNpcScript::masa_leann_10()
{
	Speak( NpcId(), 65 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Saincity Lodelight Manager
// File			: MaFl_LodeLight.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_lodelight_0()
{
	Speak( NpcId(), 296 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_lodelight_1()
{
	LaunchQuest();
}

void CNpcScript::masa_lodelight_2()
{
	AddKey( 9 );
	AddKey( 10 );
	AddKey( 36 );		
}

void CNpcScript::masa_lodelight_3()
{
	Say( 297 );
}

void CNpcScript::masa_lodelight_4()
{
	Say( 298 );
}

void CNpcScript::masa_lodelight_5()
{
	
}

void CNpcScript::masa_lodelight_6()
{
	
}

void CNpcScript::masa_lodelight_8()
{
	
}

void CNpcScript::masa_lodelight_9()
{
	Say( 781 );	
}

void CNpcScript::masa_lodelight_36()
{
	Say( 300 );
}

void CNpcScript::masa_lodelight_10()
{
	Speak( NpcId(), 301 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Magicion Traning Instructor
// File			: MaSa_Lopaze.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_lopaze_0()
{
	Speak( NpcId(), 782 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_lopaze_1()
{
	LaunchQuest();
}

void CNpcScript::masa_lopaze_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_lopaze_3()
{
	Say( 107 );
}

void CNpcScript::masa_lopaze_4()
{
	Say( 783 );
}

void CNpcScript::masa_lopaze_5()
{
	
}

void CNpcScript::masa_lopaze_6()
{
	
}

void CNpcScript::masa_lopaze_8()
{
	if( GetQuestState(QUEST_VOCMAG_TRN3) == QS_END && GetPlayerJob() == 0 && GetPlayerLvl() == 15 )
	{
		ChangeJob(4);
		InitStat();
	}

	else
	{
		Exit();
	}
}

void CNpcScript::masa_lopaze_9()
{
	Say( 784 );	
}

void CNpcScript::masa_lopaze_10()
{
	Speak( NpcId(), 785 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Saincity Magic Shop Owner
// File			: MaSa_Martin.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_martin_0()
{
	Speak( NpcId(), 786 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_martin_1()
{
	LaunchQuest();
}

void CNpcScript::masa_martin_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_martin_3()
{
	Say( 787 );
}

void CNpcScript::masa_martin_4()
{
	Say( 788 );
}

void CNpcScript::masa_martin_5()
{
	
}

void CNpcScript::masa_martin_6()
{
	
}

void CNpcScript::masa_martin_8()
{
	
}

void CNpcScript::masa_martin_9()
{
	Say( 789 );	
}

void CNpcScript::masa_martin_10()
{
	Speak( NpcId(), 790 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Jeong-hwa's Son
// File			: MaSa_MaYun.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_mayun_0()
{
	Speak( NpcId(), 791 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_mayun_1()
{
	LaunchQuest();
}

void CNpcScript::masa_mayun_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_mayun_3()
{
	Say( 107 );
}

void CNpcScript::masa_mayun_4()
{
	Say( 792 );
}

void CNpcScript::masa_mayun_5()
{
	
}

void CNpcScript::masa_mayun_6()
{
	
}

void CNpcScript::masa_mayun_8()
{
	
}

void CNpcScript::masa_mayun_9()
{
	Say( 793 );	
}

void CNpcScript::masa_mayun_10()
{
	Speak( NpcId(), 794 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Sub Manager of Saincity Station
// File			: MaSa_Parine.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_parine_0()
{
	Speak( NpcId(), 795 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_parine_1()
{
	LaunchQuest();
}

void CNpcScript::masa_parine_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_parine_3()
{
	Say( 107 );
}

void CNpcScript::masa_parine_4()
{
	Say( 796 );
}

void CNpcScript::masa_parine_5()
{
	
}

void CNpcScript::masa_parine_6()
{
	
}

void CNpcScript::masa_parine_8()
{
	
}

void CNpcScript::masa_parine_9()
{
	Say( 797 );	
}

void CNpcScript::masa_parine_10()
{
	Speak( NpcId(), 798 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Lost Child Association Master
// File			: MaSa_Porgo.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_porgo_0()
{
	Speak( NpcId(), 799 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_porgo_1()
{
	LaunchQuest();
}

void CNpcScript::masa_porgo_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_porgo_3()
{
	Say( 107 );
}

void CNpcScript::masa_porgo_4()
{
	Say( 800 );
}

void CNpcScript::masa_porgo_5()
{
	
}

void CNpcScript::masa_porgo_6()
{
	
}

void CNpcScript::masa_porgo_8()
{
	
}

void CNpcScript::masa_porgo_9()
{
	Say( 801 );	
}

void CNpcScript::masa_porgo_10()
{
	Speak( NpcId(), 802 );
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Queer Collector
// File			: MaSa_QueerCollector.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_queercollector_0()
{
	Speak( NpcId(), 803 );
	SetScriptTimer( 15 );
}

void CNpcScript::masa_queercollector_1()
{
	LaunchQuest();
}

void CNpcScript::masa_queercollector_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::masa_queercollector_3()
{
	Say ( 107 );
}

void CNpcScript::masa_queercollector_4()
{
	Say ( 804 );
}

void CNpcScript::masa_queercollector_5()
{

}

void CNpcScript::masa_queercollector_6()
{

}

void CNpcScript::masa_queercollector_7()
{

}

void CNpcScript::masa_queercollector_8()
{

}

void CNpcScript::masa_queercollector_9()
{
	Say ( 805 );
}

void CNpcScript::masa_queercollector_10()
{
	Speak( NpcId(), 806 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Saincity National Library Ancient Record Manager
// File			: MaSa_Rovanett.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_rovanett_0()
{
	Speak( NpcId(), 807 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_rovanett_1()
{
	LaunchQuest();
}

void CNpcScript::masa_rovanett_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_rovanett_3()
{
	Say( 107 );
}

void CNpcScript::masa_rovanett_4()
{
	Say( 808 );
}

void CNpcScript::masa_rovanett_5()
{
	
}

void CNpcScript::masa_rovanett_6()
{
	
}

void CNpcScript::masa_rovanett_8()
{
	
}

void CNpcScript::masa_rovanett_9()
{
	Say( 809 );	
}

void CNpcScript::masa_rovanett_10()
{
	Speak( NpcId(), 810 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Saincity Mayor
// File			: MaSa_SainMayor.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_sainmayor_0()
{
	Speak( NpcId(), 811 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_sainmayor_1()
{
	LaunchQuest();
}

void CNpcScript::masa_sainmayor_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_sainmayor_3()
{
	Say( 107 );
}

void CNpcScript::masa_sainmayor_4()
{
	Say( 812 );
}

void CNpcScript::masa_sainmayor_5()
{
	
}

void CNpcScript::masa_sainmayor_6()
{
	
}

void CNpcScript::masa_sainmayor_8()
{
	
}

void CNpcScript::masa_sainmayor_9()
{
	Say( 813 );	
}

void CNpcScript::masa_sainmayor_10()
{
	Speak( NpcId(), 814 );
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Saincity Station Manager
// File			: MaSa_Tina.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_tina_0()
{
	Speak( NpcId(), 815 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_tina_1()
{
	LaunchQuest();
}

void CNpcScript::masa_tina_2()
{
	AddKey( 9 );
	AddKey( 40 );
	AddKey( 41 );
	AddKey( 10 );		
}

void CNpcScript::masa_tina_3()
{
	Say( 816 );
}

void CNpcScript::masa_tina_4()
{
	Say( 817 );
}

void CNpcScript::masa_tina_5()
{
	
}

void CNpcScript::masa_tina_6()
{
	
}

void CNpcScript::masa_tina_8()
{
	
}

void CNpcScript::masa_tina_9()
{
	Say( 818 );	
}

void CNpcScript::masa_tina_40()
{
 	Say(452);
        Say(453); 
	Say(454);
	Say(455);
        Say(456);
        Say(457);
}

void CNpcScript::masa_tina_41()
{
	Say(458);
	Say(459);
        Say(460);
        Say(461);
        Say(462);
	Say(463);
        Say(464); 
}

void CNpcScript::masa_tina_10()
{
	Speak( NpcId(), 819 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modified 20051125 )
// Author		: Jimmie
// Title		: Pepoview Troupe Member
// File			: MaSa_Troupemember1.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_troupemember1_0()
{
	switch( Random ( 3 ) )
	{
		case 0:
		{
			Speak( NpcId(), 820 );
			SetScriptTimer( 15 );
		}
		break;

		case 1:
		{
			Speak( NpcId(), 821 );
			SetScriptTimer( 15 );
		}
		break;

		case 2:
		{
			Speak( NpcId(), 822 );
			SetScriptTimer( 15 );
		}
		break;
	}
}
	
void CNpcScript::masa_troupemember1_1()
{
	LaunchQuest();
}

void CNpcScript::masa_troupemember1_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::masa_troupemember1_3()
{
	Say ( 107 );
}

void CNpcScript::masa_troupemember1_4()
{
	Say( 823 );
}

void CNpcScript::masa_troupemember1_5()
{

}

void CNpcScript::masa_troupemember1_6()
{
	
}

void CNpcScript::masa_troupemember1_7()
{
	
}

void CNpcScript::masa_troupemember1_8()
{
	
}

void CNpcScript::masa_troupemember1_9()
{
	Say( 824 );
}

void CNpcScript::masa_troupemember1_10()
{
	Speak( NpcId(), 825 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Pepoview Troupe Member
// File			: MaSa_Troupemember2.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_troupemember2_0()
{
	switch( Random ( 3 ) )
	{
		case 0:
			{
				Speak( NpcId(), 826 );
				SetScriptTimer( 10 );
			}
			break;

		case 1:
			{
				Speak( NpcId(), 827 );
				SetScriptTimer( 10 );
			}
			break;

		case 2:
			{
				Speak( NpcId(), 828 );
				SetScriptTimer( 10 );
			}
			break;
	}
}
	
void CNpcScript::masa_troupemember2_1()
{
	LaunchQuest();
}

void CNpcScript::masa_troupemember2_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::masa_troupemember2_3()
{
	Say ( 829 );
}

void CNpcScript::masa_troupemember2_4()
{
	Say( 830 );
}

void CNpcScript::masa_troupemember2_5()
{

}

void CNpcScript::masa_troupemember2_6()
{
	
}

void CNpcScript::masa_troupemember2_7()
{
	
}

void CNpcScript::masa_troupemember2_8()
{
	
}

void CNpcScript::masa_troupemember2_9()
{
	Say( 824 );
}

void CNpcScript::masa_troupemember2_10()
{
	Speak( NpcId(), 825 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Pepoview Troupe Member
// File			: MaSa_Troupemember3.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_troupemember3_0()
{
	switch( Random ( 3 ) )
	{
		case 0:
			{
				Speak( NpcId(), 831 );
				SetScriptTimer( 12 );
			}
			break;

		case 1:
			{
				Speak( NpcId(), 832 );
				SetScriptTimer( 12 );
			}
			break;

		case 2:
			{
				Speak( NpcId(), 833 );
				SetScriptTimer( 12 );
			}
			break;
	}
}
	
void CNpcScript::masa_troupemember3_1()
{
	LaunchQuest();
}

void CNpcScript::masa_troupemember3_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::masa_troupemember3_3()
{
	Say ( 834 );
}

void CNpcScript::masa_troupemember3_4()
{
	Say( 830 );
}

void CNpcScript::masa_troupemember3_5()
{

}

void CNpcScript::masa_troupemember3_6()
{
	
}

void CNpcScript::masa_troupemember3_7()
{
	
}

void CNpcScript::masa_troupemember3_8()
{
	
}

void CNpcScript::masa_troupemember3_9()
{
	Say( 824 );
}

void CNpcScript::masa_troupemember3_10()
{
	Speak( NpcId(), 825 );
	Exit();
}

//Script End
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.1( Modify060804 )
// Author		: Jimmie
// Title		: Pepoview Troupe Member
// File			: MaSa_Troupemember3.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_troupemember4_0()
{
	switch( Random ( 3 ) )
	{
		case 0:
			{
				Speak( NpcId(), 835 );
				SetScriptTimer( 15 );
			}
			break;

		case 1:
			{
				Speak( NpcId(), 836 );
				SetScriptTimer( 15 );
			}
			break;

		case 2:
			{
				Speak( NpcId(), 837 );
				SetScriptTimer( 15 );
			}
			break;
	}
}
	
void CNpcScript::masa_troupemember4_1()
{
	LaunchQuest();
}

void CNpcScript::masa_troupemember4_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::masa_troupemember4_3()
{
	Say ( 838 );
}

void CNpcScript::masa_troupemember4_4()
{
	Say( 830 );
}

void CNpcScript::masa_troupemember4_5()
{

}

void CNpcScript::masa_troupemember4_6()
{
	
}

void CNpcScript::masa_troupemember4_7()
{
	
}

void CNpcScript::masa_troupemember4_8()
{
	
}

void CNpcScript::masa_troupemember4_9()
{
	Say( 824 );
}

void CNpcScript::masa_troupemember4_10()
{
	Speak( NpcId(), 825 );
	Exit();
}

//Script End
//Script Informaiton--------------------------//
// Version  : 1.0
// Author   : Jimui
// Serial   : FLYFF_AEON040418_007_v1
// Title    : Troupe <Pepoview> Member2
// Link NPC : Madrigal Saintmorning Clak cable
// File Name: MaSa_Troupemember5.txt
// Code     : ANSI
// Country  : KR 
//--------------------------------------------//
//Script Begin
void CNpcScript::masa_troupemember5_0()
{
	switch( Random ( 3 ) )
	{
		case 0:
		{
			Speak( NpcId(), 839 );
			SetScriptTimer( 5 );
		}
		break;

		case 1:
		{
			Speak( NpcId(), 840 );
			SetScriptTimer( 5 );
		}
		break;

		case 2:
		{
			Speak( NpcId(), 841 );
			SetScriptTimer( 5 );
		}
		break;
	}
}
	
void CNpcScript::masa_troupemember5_1()
{
	LaunchQuest();
}

void CNpcScript::masa_troupemember5_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::masa_troupemember5_3()
{
	Say ( 842 );
}

void CNpcScript::masa_troupemember5_4()
{
	Say ( 843 );
}

void CNpcScript::masa_troupemember5_5()
{

}

void CNpcScript::masa_troupemember5_6()
{
	
}

void CNpcScript::masa_troupemember5_7()
{
	
}

void CNpcScript::masa_troupemember5_8()
{
	
}

void CNpcScript::masa_troupemember5_9()
{
	Say( 824 );
	AddAnswer( 10 );
}

void CNpcScript::masa_troupemember5_10()
{
	Speak( NpcId(), 825 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Magicion Traning Assist
// File			: MaSa_Wingyei.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_wingyei_0()
{
	Speak( NpcId(), 844 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_wingyei_1()
{
	if( GetQuestState( QUEST_VOCMER_BFTRN ) >= 0 || GetQuestState( QUEST_VOCASS_BFTRN ) >= 0 || GetQuestState( QUEST_VOCACR_BFTRN ) >= 0)
	{
		if( GetQuestState( QUEST_SCE_MDRIGALTEACHER3 ) >= 0 )
			LaunchQuest();
		else
			SayQuest( QUEST_VOCACR_BFTRN, QSAY_EXTRA01 );
		AddKey( 9 );
		AddKey( 10 );
	}

	else
	{
		LaunchQuest();
	}
}

void CNpcScript::masa_wingyei_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_wingyei_3()
{
	Say( 107 );
}

void CNpcScript::masa_wingyei_4()
{
	Say( 845 );
}

void CNpcScript::masa_wingyei_5()
{
	
}

void CNpcScript::masa_wingyei_6()
{
	//Say( "아쉽게도, 이제 내가 가지고 있는 블링크윙은 모두 소진되었다 제군. 대신 시간이 늘어났으니 블링크윙은 구입하여 쓰도록." );
	if( GetQuestState(QUEST_VOCMAG_BFTRN) == QS_BEGIN )
	{
		Replace( WI_WORLD_MADRIGAL, 7161.0, 100.0, 3264.0 );
	}
}

void CNpcScript::masa_wingyei_8()
{
	
}

void CNpcScript::masa_wingyei_9()
{
	Say( 846 );	
}

void CNpcScript::masa_wingyei_10()
{
	Speak( NpcId(), 847 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Darken Mayor
// File			: MaDa_DarMayor.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_darmayor_0()
{
	Speak( NpcId(), 858 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_darmayor_1()
{
	LaunchQuest();
}

void CNpcScript::mada_darmayor_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_darmayor_3()
{
	Say( 859 );
}

void CNpcScript::mada_darmayor_4()
{
	Say( 860 );
}

void CNpcScript::mada_darmayor_5()
{
	
}

void CNpcScript::mada_darmayor_6()
{

}

void CNpcScript::mada_darmayor_8()
{
	
}

void CNpcScript::mada_darmayor_9()
{
	Say( 861 );	
}

void CNpcScript::mada_darmayor_10()
{
	Speak( NpcId(), 862 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Temple of Iblis Manager
// File			: MaSa_Daz.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_daz_0()
{
	Speak( NpcId(), 877 );	
	SetScriptTimer( 15 );
}

void CNpcScript::masa_daz_1()
{
	if( GetPlayerLvl() >= 40 )
	{
		if( IsSetQuest( QUEST_INTHEDARK ) == TRUE && GetQuestState( QUEST_INTHEDARK ) == QS_END )
		{
				if( IsParty() == TRUE )
				{
					if( GetItemNum( II_SYS_SYS_QUE_RUBY01 ) >= 1 && GetItemNum( II_SYS_SYS_QUE_RUBY02 ) >= 1 && GetItemNum( II_SYS_SYS_QUE_RUBY03 ) >=1 )
					{
						if( MonHuntStartParty( QUEST_IBLBOSS, QS_BEGIN, QS_END, 1 ) == FALSE )
						{
							Say( 871 );
							AddKey( 10 );
						}
						else
						{
							RemoveItem( II_SYS_SYS_QUE_RUBY01, 1 );
							RemoveItem( II_SYS_SYS_QUE_RUBY02, 1 );
							RemoveItem( II_SYS_SYS_QUE_RUBY03, 1 );
						}
											
						/*
						if( MonHuntStartParty( QUEST_IBLBOSS, QS_BEGIN, QS_END, 1 ) == FALSE )
						{
							Say( 871 );
							AddKey( 10 );
						}
											
						else if( MonHuntStartParty( QUEST_IBLBOSS, QS_BEGIN, QS_END, 1 ) == TRUE )
						{
							
						}
						
						else
						{
							Say( 872 );
							AddKey( 10 );	
						}
						*/
					}

					else
					{
				    	Say( 873 );
						AddKey( 10 );
					}
				}	
				
				else
				{
					Say( 870 );	
					AddKey( 10 );
				}
				
		}	
			
		else
		{
			Say( 869);
			AddKey( 10 );
		}
	}
	
	else
	{
		Say( 868 );	
		AddKey( 10 );
	}
}

void CNpcScript::masa_daz_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::masa_daz_3()
{
	Say( 874 );
}

void CNpcScript::masa_daz_4()
{
	Say( 875 );
}

void CNpcScript::masa_daz_5()
{
	
}

void CNpcScript::masa_daz_6()
{
	
}

void CNpcScript::masa_daz_8()
{
	
}

void CNpcScript::masa_daz_9()
{
	Say( 876 );	
}

void CNpcScript::masa_daz_10()
{
	Speak( NpcId(), 877 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Hangawi Event NPC
// File			: MaFl_Iblis01.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_iblis01_0()
{
	Speak( NpcId(), 884 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_iblis01_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_iblis01_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_iblis01_3()
{
}

void CNpcScript::mafl_iblis01_4()
{
	Say( 885 );
}

void CNpcScript::mafl_iblis01_5()
{
	
}

void CNpcScript::mafl_iblis01_6()
{

}

void CNpcScript::mafl_iblis01_8()
{
	
}

void CNpcScript::mafl_iblis01_9()
{
	Say( 886 );	
}

void CNpcScript::mafl_iblis01_10()
{
	Speak( NpcId(), 887 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Hangawi Event NPC
// File			: mafl_iblis02.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_iblis02_0()
{
	Speak( NpcId(), 888 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_iblis02_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_iblis02_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_iblis02_3()
{
}

void CNpcScript::mafl_iblis02_4()
{
	Say( 889 );
}

void CNpcScript::mafl_iblis02_5()
{
	
}

void CNpcScript::mafl_iblis02_6()
{

}

void CNpcScript::mafl_iblis02_8()
{
	
}

void CNpcScript::mafl_iblis02_9()
{
	Say( 890 );	
}

void CNpcScript::mafl_iblis02_10()
{
	Speak( NpcId(), 891 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Hangawi Event NPC
// File			: mafl_iblis03.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_iblis03_0()
{
	Speak( NpcId(), 892 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_iblis03_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_iblis03_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_iblis03_3()
{
}

void CNpcScript::mafl_iblis03_4()
{
	Say( 893 );
}

void CNpcScript::mafl_iblis03_5()
{
	
}

void CNpcScript::mafl_iblis03_6()
{

}

void CNpcScript::mafl_iblis03_8()
{
	
}

void CNpcScript::mafl_iblis03_9()
{
	Say( 894 );	
}

void CNpcScript::mafl_iblis03_10()
{
	Speak( NpcId(), 895 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Hangawi Event NPC
// File			: mafl_iblis04.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_iblis04_0()
{
	Speak( NpcId(), 896  );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_iblis04_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_iblis04_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_iblis04_3()
{
}

void CNpcScript::mafl_iblis04_4()
{
	Say( 897 );
}

void CNpcScript::mafl_iblis04_5()
{
	
}

void CNpcScript::mafl_iblis04_6()
{

}

void CNpcScript::mafl_iblis04_8()
{
	
}

void CNpcScript::mafl_iblis04_9()
{
	Say( 898 );	
}

void CNpcScript::mafl_iblis04_10()
{
	Speak( NpcId(), 899 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: Jimmie
// Title		: Hangawi Event NPC
// File			: mafl_iblis05.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_iblis05_0()
{
	Speak( NpcId(), 900 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_iblis05_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_iblis05_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_iblis05_3()
{
}

void CNpcScript::mafl_iblis05_4()
{
	Say( 901 );
}

void CNpcScript::mafl_iblis05_5()
{
	
}

void CNpcScript::mafl_iblis05_6()
{

}

void CNpcScript::mafl_iblis05_8()
{
	
}

void CNpcScript::mafl_iblis05_9()
{
	Say( 902 );	
}

void CNpcScript::mafl_iblis05_10()
{
	Speak( NpcId(), 903 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: kyk1236
// Title		: autumnevent NPC
// File			: MaFl_autumnevent01.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_autumnevent01_0()
{
	Speak( NpcId(), 904 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_autumnevent01_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_autumnevent01_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_autumnevent01_3()
{
}

void CNpcScript::mafl_autumnevent01_4()
{
	Say( 905 );
}

void CNpcScript::mafl_autumnevent01_5()
{
	
}

void CNpcScript::mafl_autumnevent01_6()
{

}

void CNpcScript::mafl_autumnevent01_8()
{
	
}

void CNpcScript::mafl_autumnevent01_9()
{
	Say( 906 );	
}

void CNpcScript::mafl_autumnevent01_10()
{
	Speak( NpcId(), 907 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: kyk1236
// Title		: autumnevent NPC Event NPC
// File			: mafl_autumnevent02.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_autumnevent02_0()
{
	Speak( NpcId(), 908 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_autumnevent02_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_autumnevent02_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_autumnevent02_3()
{
}

void CNpcScript::mafl_autumnevent02_4()
{
	Say( 909 );
}

void CNpcScript::mafl_autumnevent02_5()
{
	
}

void CNpcScript::mafl_autumnevent02_6()
{

}

void CNpcScript::mafl_autumnevent02_8()
{
	
}

void CNpcScript::mafl_autumnevent02_9()
{
	Say( 910 );	
}

void CNpcScript::mafl_autumnevent02_10()
{
	Speak( NpcId(), 911 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: kyk1236
// Title		: autumnevent NPC Event NPC
// File			: mafl_autumnevent03.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_autumnevent03_0()
{
	Speak( NpcId(), 912 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_autumnevent03_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_autumnevent03_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_autumnevent03_3()
{
}

void CNpcScript::mafl_autumnevent03_4()
{
	Say( 913 );
}

void CNpcScript::mafl_autumnevent03_5()
{
	
}

void CNpcScript::mafl_autumnevent03_6()
{

}

void CNpcScript::mafl_autumnevent03_8()
{
	
}

void CNpcScript::mafl_autumnevent03_9()
{
	Say( 914 );	
}

void CNpcScript::mafl_autumnevent03_10()
{
	Speak( NpcId(), 915 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: kyk1236
// Title		: autumnevent NPC Event NPC
// File			: mafl_autumnevent04.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_autumnevent04_0()
{
	Speak( NpcId(), 916  );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_autumnevent04_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_autumnevent04_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_autumnevent04_3()
{
}

void CNpcScript::mafl_autumnevent04_4()
{
	Say( 917 );
}

void CNpcScript::mafl_autumnevent04_5()
{
	
}

void CNpcScript::mafl_autumnevent04_6()
{

}

void CNpcScript::mafl_autumnevent04_8()
{
	
}

void CNpcScript::mafl_autumnevent04_9()
{
	Say( 918 );	
}

void CNpcScript::mafl_autumnevent04_10()
{
	Speak( NpcId(), 919 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: kyk1236
// Title		: PKNPC01
// File			: MaMa_PKNPC01.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mama_pknpc01_0()
{
	Speak( NpcId(), 920  );	
	SetScriptTimer( 15 );
}

void CNpcScript::mama_pknpc01_1()
{
	LaunchQuest();
}

void CNpcScript::mama_pknpc01_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mama_pknpc01_3()
{
}

void CNpcScript::mama_pknpc01_4()
{
	Say( 921 );
}

void CNpcScript::mama_pknpc01_5()
{
	
}

void CNpcScript::mama_pknpc01_6()
{

}

void CNpcScript::mama_pknpc01_8()
{
	
}

void CNpcScript::mama_pknpc01_9()
{
	Say( 922 );	
}

void CNpcScript::mama_pknpc01_10()
{
	Speak( NpcId(), 923 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: kyk1236
// Title		: Teacher01
// File			: mafl_teacher01.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_teacher01_0()
{
	Speak( NpcId(), 924 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_teacher01_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_teacher01_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_teacher01_3()
{
}

void CNpcScript::mafl_teacher01_4()
{
	Say( 925 );
}

void CNpcScript::mafl_teacher01_5()
{
	
}

void CNpcScript::mafl_teacher01_6()
{

}

void CNpcScript::mafl_teacher01_8()
{
	
}

void CNpcScript::mafl_teacher01_9()
{
	Say( 926 );	
}

void CNpcScript::mafl_teacher01_10()
{
	Speak( NpcId(), 927 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: kyk1236
// Title		: CryTeacher
// File			: mafl_teacher02.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_teacher02_0()
{
	Speak( NpcId(), 928 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_teacher02_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_teacher02_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_teacher02_3()
{
}

void CNpcScript::mafl_teacher02_4()
{
	Say( 929 );
}

void CNpcScript::mafl_teacher02_5()
{
	
}

void CNpcScript::mafl_teacher02_6()
{

}

void CNpcScript::mafl_teacher02_8()
{
	
}

void CNpcScript::mafl_teacher02_9()
{
	Say( 930 );	
}

void CNpcScript::mafl_teacher02_10()
{
	Speak( NpcId(), 931 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: CryTeacher
// File			: mafl_teacher03.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_teacher03_0()
{
	Speak( NpcId(), 932 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_teacher03_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_teacher03_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_teacher03_3()
{
}

void CNpcScript::mafl_teacher03_4()
{
	Say( 933 );
}

void CNpcScript::mafl_teacher03_5()
{
	
}

void CNpcScript::mafl_teacher03_6()
{

}

void CNpcScript::mafl_teacher03_8()
{
	
}

void CNpcScript::mafl_teacher03_9()
{
	Say( 934 );	
}

void CNpcScript::mafl_teacher03_10()
{
	Speak( NpcId(), 935 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: White day
// File			: mafl_cooker01.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_cooker01_0()
{
	Speak( NpcId(), 936 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_cooker01_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_cooker01_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_cooker01_3()
{
}

void CNpcScript::mafl_cooker01_4()
{
	Say( 937 );
}

void CNpcScript::mafl_cooker01_5()
{
	
}

void CNpcScript::mafl_cooker01_6()
{

}

void CNpcScript::mafl_cooker01_8()
{
	
}

void CNpcScript::mafl_cooker01_9()
{
	Say( 938 );	
}

void CNpcScript::mafl_cooker01_10()
{
	Speak( NpcId(), 939 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: White day
// File			: mafl_cooker02.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_cooker02_0()
{
	Speak( NpcId(), 940 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_cooker02_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_cooker02_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_cooker02_3()
{
}

void CNpcScript::mafl_cooker02_4()
{
	Say( 941 );
}

void CNpcScript::mafl_cooker02_5()
{
	
}

void CNpcScript::mafl_cooker02_6()
{

}

void CNpcScript::mafl_cooker02_8()
{
	
}

void CNpcScript::mafl_cooker02_9()
{
	Say( 942 );	
}

void CNpcScript::mafl_cooker02_10()
{
	Speak( NpcId(), 943 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Arbor Day
// File			: mafl_arborday01.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_arborday01_0()
{
	Speak( NpcId(), 944);	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_arborday01_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_arborday01_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_arborday01_3()
{
}

void CNpcScript::mafl_arborday01_4()
{
	Say( 945);
}

void CNpcScript::mafl_arborday01_5()
{
	
}

void CNpcScript::mafl_arborday01_6()
{

}

void CNpcScript::mafl_arborday01_8()
{
	
}

void CNpcScript::mafl_arborday01_9()
{
	Say( 946);	
}

void CNpcScript::mafl_arborday01_10()
{
	Speak( NpcId(), 947);
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Arbor Day
// File			: mafl_arborday02.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_arborday02_0()
{
	Speak( NpcId(), 948);	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_arborday02_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_arborday02_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_arborday02_3()
{
}

void CNpcScript::mafl_arborday02_4()
{
	Say( 949);
}

void CNpcScript::mafl_arborday02_5()
{
	
}

void CNpcScript::mafl_arborday02_6()
{

}

void CNpcScript::mafl_arborday02_8()
{
	
}

void CNpcScript::mafl_arborday02_9()
{
	Say( 950 );	
}

void CNpcScript::mafl_arborday02_10()
{
	Speak( NpcId(), 951 );
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Arbor Day
// File			: mafl_arborday03.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_arborday03_0()
{
	Speak( NpcId(), 952 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_arborday03_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_arborday03_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_arborday03_3()
{
}

void CNpcScript::mafl_arborday03_4()
{
	Say( 953 );
}

void CNpcScript::mafl_arborday03_5()
{
	
}

void CNpcScript::mafl_arborday03_6()
{

}

void CNpcScript::mafl_arborday03_8()
{
	
}

void CNpcScript::mafl_arborday03_9()
{
	Say( 954 );
}

void CNpcScript::mafl_arborday03_10()
{
	Speak( NpcId(), 955 );
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Arbor Day
// File			: mafl_arborday04.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_arborday04_0()
{
	Speak( NpcId(), 956 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_arborday04_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_arborday04_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_arborday04_3()
{
}

void CNpcScript::mafl_arborday04_4()
{
	Say( 957 );
}

void CNpcScript::mafl_arborday04_5()
{
	
}

void CNpcScript::mafl_arborday04_6()
{

}

void CNpcScript::mafl_arborday04_8()
{
	
}

void CNpcScript::mafl_arborday04_9()
{
	Say( 958 );	
}

void CNpcScript::mafl_arborday04_10()
{
	Speak( NpcId(), 959 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Nescafe 3in1 Potion Event
// File			: mafl_Pang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_pang_0()
{
	Speak( NpcId(), 960 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_pang_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_pang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_pang_3()
{
}

void CNpcScript::mafl_pang_4()
{

}

void CNpcScript::mafl_pang_5()
{
	
}

void CNpcScript::mafl_pang_6()
{

}

void CNpcScript::mafl_pang_8()
{
	
}

void CNpcScript::mafl_pang_9()
{
	Say( 961 );	
}

void CNpcScript::mafl_pang_10()
{
	Speak( NpcId(), 962 );
	Exit();
}


//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 2007년 6월 로또 보다 즐거운 가위 바위 보 게임(대만)
// File			: mafl_Pang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_kawibawibo01_0()
{
	Speak( NpcId(), 963 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_kawibawibo01_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_kawibawibo01_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_kawibawibo01_3()
{
}

void CNpcScript::mafl_kawibawibo01_4()
{

}

void CNpcScript::mafl_kawibawibo01_5()
{
	
}

void CNpcScript::mafl_kawibawibo01_6()
{

}

void CNpcScript::mafl_kawibawibo01_8()
{
	
}

void CNpcScript::mafl_kawibawibo01_9()
{
	Say( 964 );	
}

void CNpcScript::mafl_kawibawibo01_10()
{
	Speak( NpcId(), 965 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: kyk1236
// Title		: 의문의 로브 사내
// File			: mada_redrobeman.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_redrobeman_0()
{
	Speak( NpcId(), 966 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_redrobeman_1()
{
	LaunchQuest();
}

void CNpcScript::mada_redrobeman_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_redrobeman_3()
{
	Say( 967 );
}

void CNpcScript::mada_redrobeman_4()
{
	Say( 967 );
}

void CNpcScript::mada_redrobeman_5()
{
	
}

void CNpcScript::mada_redrobeman_6()
{

}

void CNpcScript::mada_redrobeman_8()
{
	
}

void CNpcScript::mada_redrobeman_9()
{
	Say( 968 );	
}

void CNpcScript::mada_redrobeman_10()
{
	Speak( NpcId(), 969 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: kyk1236
// Title		: 의문의 로브 소녀
// File			: mada_redrobegirl.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_redrobegirl_0()
{
	Speak( NpcId(), 970 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_redrobegirl_1()
{
	LaunchQuest();
}

void CNpcScript::mada_redrobegirl_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_redrobegirl_3()
{
	Say( 971 );
}

void CNpcScript::mada_redrobegirl_4()
{
	Say( 971 );
}

void CNpcScript::mada_redrobegirl_5()
{
	
}

void CNpcScript::mada_redrobegirl_6()
{

}

void CNpcScript::mada_redrobegirl_8()
{
	if( GetQuestState(QUEST_MASTER) == QS_END && GetPlayerJob() == 6 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 16 );
		InitStat();
		SetLevel(60);
		AddGPPoint( -120 );
	}
	else if( GetQuestState(QUEST_MASTER) == QS_END && GetPlayerJob() == 7 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 17 );
		InitStat();
		SetLevel(60);
		AddGPPoint( -120 );
	}
	else if( GetQuestState(QUEST_MASTER) == QS_END && GetPlayerJob() == 8 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 18 );
		InitStat();
		SetLevel(60);
		AddGPPoint( -120 );
	}
	else if( GetQuestState(QUEST_MASTER) == QS_END && GetPlayerJob() == 9 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 19 );
		InitStat();
		SetLevel(60);
		AddGPPoint( -120 );
	}
	else if( GetQuestState(QUEST_MASTER) == QS_END && GetPlayerJob() == 10 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 20 );
		InitStat();
		SetLevel(60);
		AddGPPoint( -120 );
	}
	else if( GetQuestState(QUEST_MASTER) == QS_END && GetPlayerJob() == 11 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 21 );
		InitStat();
		SetLevel(60);
		AddGPPoint( -120 );
	}
	else if( GetQuestState(QUEST_MASTER) == QS_END && GetPlayerJob() == 12 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 22 );
		InitStat();
		SetLevel(60);
		AddGPPoint( -120 );
	}
	else if( GetQuestState(QUEST_MASTER) == QS_END && GetPlayerJob() == 13 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 23 );
		InitStat();
		SetLevel(60);
		AddGPPoint( -120 );
	}
	else if( GetQuestState(QUEST_HERO) == QS_END && GetPlayerJob() == 16 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 24 );
		SetLevel(121);
		AddGPPoint( 15 );
	}
	else if( GetQuestState(QUEST_HERO) == QS_END && GetPlayerJob() == 17 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 25 );
		SetLevel(121);
		AddGPPoint( 15 );
	}
	else if( GetQuestState(QUEST_HERO) == QS_END && GetPlayerJob() == 18 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 26 );
		SetLevel(121);
		AddGPPoint( 15 );
	}
	else if( GetQuestState(QUEST_HERO) == QS_END && GetPlayerJob() == 19 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 27 );
		SetLevel(121);
		AddGPPoint( 15 );
	}
	else if( GetQuestState(QUEST_HERO) == QS_END && GetPlayerJob() == 20 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 28 );
		SetLevel(121);
		AddGPPoint( 15 );
	}
	else if( GetQuestState(QUEST_HERO) == QS_END && GetPlayerJob() == 21 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 29 );
		SetLevel(121);
		AddGPPoint( 15 );
	}
	else if( GetQuestState(QUEST_HERO) == QS_END && GetPlayerJob() == 22 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 30 );
		SetLevel(121);
		AddGPPoint( 15 );
	}
	else if( GetQuestState(QUEST_HERO) == QS_END && GetPlayerJob() == 23 && GetPlayerLvl() == 120 &&  GetPlayerExpPercent() == 9999 )
	{
		ChangeJob( 31 );
		SetLevel(121);
		AddGPPoint( 15 );
	}
	else
	{
		Exit();
	}
}

void CNpcScript::mada_redrobegirl_9()
{
	Say( 972 );	
}

void CNpcScript::mada_redrobegirl_10()
{
	Speak( NpcId(), 973 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 펫 테이머
// File			: mafl_pettamer.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_pettamer_0()
{
	Speak( NpcId(), 974 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_pettamer_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_pettamer_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_pettamer_3()
{
}

void CNpcScript::mafl_pettamer_4()
{
	Say( 975 );
}

void CNpcScript::mafl_pettamer_5()
{
	
}

void CNpcScript::mafl_pettamer_6()
{

}

void CNpcScript::mafl_pettamer_8()
{
	
}

void CNpcScript::mafl_pettamer_9()
{
	Say( 976 );	
}

void CNpcScript::mafl_pettamer_10()
{
	Speak( NpcId(), 977 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: kyk1236
// Title		: 게이트 키퍼
// File			: mada_gatekeeper.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_gatekeeper_0()
{
	Speak( NpcId(), 978 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mada_gatekeeper_1()
{
	LaunchQuest();
}

void CNpcScript::mada_gatekeeper_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mada_gatekeeper_3()
{
}

void CNpcScript::mada_gatekeeper_4()
{
	Say( 979 );
}

void CNpcScript::mada_gatekeeper_5()
{
	
}

void CNpcScript::mada_gatekeeper_6()
{

}

void CNpcScript::mada_gatekeeper_8()
{
	
}

void CNpcScript::mada_gatekeeper_9()
{
	Say( 980 );	
}

void CNpcScript::mada_gatekeeper_10()
{
	Speak( NpcId(), 981 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 칠석이벤트01
// File			: MaFl_July01.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_july01_0()
{
	Speak( NpcId(), 982 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_july01_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_july01_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_july01_3()
{
}

void CNpcScript::mafl_july01_4()
{
	Say( 983 );
}

void CNpcScript::mafl_july01_5()
{
	
}

void CNpcScript::mafl_july01_6()
{

}

void CNpcScript::mafl_july01_8()
{
	
}

void CNpcScript::mafl_july01_9()
{
	Say( 984 );	
}

void CNpcScript::mafl_july01_10()
{
	Speak( NpcId(), 985 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 칠석이벤트02
// File			: MaFl_July02.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_july02_0()
{
	Speak( NpcId(), 986 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_july02_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_july02_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_july02_3()
{
}

void CNpcScript::mafl_july02_4()
{
	Say( 987 );
}

void CNpcScript::mafl_july02_5()
{
	
}

void CNpcScript::mafl_july02_6()
{

}

void CNpcScript::mafl_july02_8()
{
	
}

void CNpcScript::mafl_july02_9()
{
	Say( 988 );	
}

void CNpcScript::mafl_july02_10()
{
	Speak( NpcId(), 989 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 보보찬
// File			: MaFl_Bobochan.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_bobochan_0()
{
	Speak( NpcId(), 990 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_bobochan_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_bobochan_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_bobochan_3()
{
}

void CNpcScript::mafl_bobochan_4()
{
	Say( 991 );
}

void CNpcScript::mafl_bobochan_5()
{
	
}

void CNpcScript::mafl_bobochan_6()
{

}

void CNpcScript::mafl_bobochan_8()
{
	
}

void CNpcScript::mafl_bobochan_9()
{
	Say( 992 );	
}

void CNpcScript::mafl_bobochan_10()
{
	Speak( NpcId(), 993 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Nescafe 3in1 Hoverboard Quest!
// File			: Mafl_Nestle02.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_nestle02_0()
{
	Speak( NpcId(), 994 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_nestle02_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_nestle02_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_nestle02_3()
{
}

void CNpcScript::mafl_nestle02_4()
{

}

void CNpcScript::mafl_nestle02_5()
{
	
}

void CNpcScript::mafl_nestle02_6()
{

}

void CNpcScript::mafl_nestle02_8()
{
	
}

void CNpcScript::mafl_nestle02_9()
{
	Say( 995 );	
}

void CNpcScript::mafl_nestle02_10()
{
	Speak( NpcId(), 996 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Nescafe Ultimate Hoverbike Quest!
// File			: Mafl_Nestle03.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_nestle03_0()
{
	Speak( NpcId(), 997 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_nestle03_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_nestle03_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_nestle03_3()
{
}

void CNpcScript::mafl_nestle03_4()
{

}

void CNpcScript::mafl_nestle03_5()
{
	
}

void CNpcScript::mafl_nestle03_6()
{

}

void CNpcScript::mafl_nestle03_8()
{
	
}

void CNpcScript::mafl_nestle03_9()
{
	Say( 998 );	
}

void CNpcScript::mafl_nestle03_10()
{
	Speak( NpcId(), 999 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 모기 퇴치 대 작전
// File			: Mafl_KAWIBAWIBO03.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_kawibawibo03_0()
{
	Speak( NpcId(), 1000 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_kawibawibo03_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_kawibawibo03_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_kawibawibo03_3()
{
}

void CNpcScript::mafl_kawibawibo03_4()
{
	Say( 1001 );	
}

void CNpcScript::mafl_kawibawibo03_5()
{
	
}

void CNpcScript::mafl_kawibawibo03_6()
{

}

void CNpcScript::mafl_kawibawibo03_8()
{
	
}

void CNpcScript::mafl_kawibawibo03_9()
{
	Say( 1002 );	
}

void CNpcScript::mafl_kawibawibo03_10()
{
	Speak( NpcId(), 1003 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 비공정 재료를 사수하라!
// File			: Mafl_StrangeCollector.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_strangecollector_0()
{
	Speak( NpcId(), 1004 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_strangecollector_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_strangecollector_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_strangecollector_3()
{
}

void CNpcScript::mafl_strangecollector_4()
{
	Say( 1005 );	
}

void CNpcScript::mafl_strangecollector_5()
{
	
}

void CNpcScript::mafl_strangecollector_6()
{

}

void CNpcScript::mafl_strangecollector_8()
{
	
}

void CNpcScript::mafl_strangecollector_9()
{
	Say( 1006 );	
}

void CNpcScript::mafl_strangecollector_10()
{
	Speak( NpcId(), 1007 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 1To1 Guild Combat Manager
// File			: MaFl_Amos.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_amos_0()
{
	Speak( NpcId(), 1008 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_amos_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_amos_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_amos_3()
{
	
}

void CNpcScript::mafl_amos_4()
{
	Say( 1009 );
}

void CNpcScript::mafl_amos_5()
{

}

void CNpcScript::mafl_amos_6()
{

}

void CNpcScript::mafl_amos_7()
{

}

void CNpcScript::mafl_amos_8()
{

}

void CNpcScript::mafl_amos_9()
{
	Say( 1010 );
}

void CNpcScript::mafl_amos_10()
{
	Speak( NpcId(), 1011 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 1To1 Guild Combat Manager
// File			: MaFl_Annie.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_annie_0()
{
	Speak( NpcId(), 1012 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_annie_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_annie_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_annie_3()
{
	
}

void CNpcScript::mafl_annie_4()
{
	Say( 1013 );
}

void CNpcScript::mafl_annie_5()
{

}

void CNpcScript::mafl_annie_6()
{

}

void CNpcScript::mafl_annie_7()
{

}

void CNpcScript::mafl_annie_8()
{

}

void CNpcScript::mafl_annie_9()
{
	Say( 1014 );
}

void CNpcScript::mafl_annie_10()
{
	Speak( NpcId(), 1015 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Red Chip Trader
// File			: MaFl_Waforu.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_waforu_0()
{
	Speak( NpcId(), 1016 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_waforu_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_waforu_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_waforu_3()
{
	
}

void CNpcScript::mafl_waforu_4()
{
	Say( 1017 );
}

void CNpcScript::mafl_waforu_5()
{

}

void CNpcScript::mafl_waforu_6()
{

}

void CNpcScript::mafl_waforu_7()
{

}

void CNpcScript::mafl_waforu_8()
{

}

void CNpcScript::mafl_waforu_9()
{
	Say( 1018 );
}

void CNpcScript::mafl_waforu_10()
{
	Speak( NpcId(), 1019 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Santa
// File			: MaFl_Santa01.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_santa01_0()
{
	Speak( NpcId(), 1020 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_santa01_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_santa01_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_santa01_3()
{
	
}

void CNpcScript::mafl_santa01_4()
{
	Say( 1021 );
}

void CNpcScript::mafl_santa01_5()
{

}

void CNpcScript::mafl_santa01_6()
{

}

void CNpcScript::mafl_santa01_7()
{

}

void CNpcScript::mafl_santa01_8()
{

}

void CNpcScript::mafl_santa01_9()
{
	Say( 1022 );
}

void CNpcScript::mafl_santa01_10()
{
	Speak( NpcId(), 1023 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Harold
// File			: MaFl_Harold.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_harold_0()
{
	Speak( NpcId(), 1024 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_harold_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_harold_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_harold_3()
{
	
}

void CNpcScript::mafl_harold_4()
{
	Say( 1025 );
}

void CNpcScript::mafl_harold_5()
{

}

void CNpcScript::mafl_harold_6()
{

}

void CNpcScript::mafl_harold_7()
{

}

void CNpcScript::mafl_harold_8()
{

}

void CNpcScript::mafl_harold_9()
{
	Say( 1026 );
}

void CNpcScript::mafl_harold_10()
{
	Speak( NpcId(), 1027 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Dick
// File			: MaFl_Dick.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_dick_0()
{
	Speak( NpcId(), 1028 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_dick_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_dick_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_dick_3()
{
	
}

void CNpcScript::mafl_dick_4()
{
	Say( 1029 );
}

void CNpcScript::mafl_dick_5()
{

}

void CNpcScript::mafl_dick_6()
{

}

void CNpcScript::mafl_dick_7()
{

}

void CNpcScript::mafl_dick_8()
{

}

void CNpcScript::mafl_dick_9()
{
	Say( 1030 );
}

void CNpcScript::mafl_dick_10()
{
	Speak( NpcId(), 1031 );
	Exit();
}
//Script End

//Script Informaion----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Helper
// File			: MaFl_Helper.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_helper_0()
{
	Speak( NpcId(), 1032 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_helper_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_helper_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_helper_3()
{
	
}

void CNpcScript::mafl_helper_4()
{
	Say( 1033 );
}

void CNpcScript::mafl_helper_5()
{

}

void CNpcScript::mafl_helper_6()
{

}

void CNpcScript::mafl_helper_7()
{

}

void CNpcScript::mafl_helper_8()
{

}

void CNpcScript::mafl_helper_9()
{
	Say( 1034 );
}

void CNpcScript::mafl_helper_10()
{
	Speak( NpcId(), 1035 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Ray
// File			: MaFl_Ray.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_ray_0()
{
	Speak( NpcId(), 1046 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_ray_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_ray_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_ray_3()
{
	
}

void CNpcScript::mafl_ray_4()
{
	Say( 1047 );
}

void CNpcScript::mafl_ray_5()
{

}

void CNpcScript::mafl_ray_6()
{

}

void CNpcScript::mafl_ray_7()
{

}

void CNpcScript::mafl_ray_8()
{

}

void CNpcScript::mafl_ray_9()
{
	Say( 1048 );
}

void CNpcScript::mafl_ray_10()
{
	Speak( NpcId(), 1049 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: ForestManager
// File			: MaFl_ForestManager.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_forestmanager_0()
{
	Speak( NpcId(), 1050 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_forestmanager_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_forestmanager_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_forestmanager_3()
{
	
}

void CNpcScript::mafl_forestmanager_4()
{
	Say( 1051 );
}

void CNpcScript::mafl_forestmanager_5()
{

}

void CNpcScript::mafl_forestmanager_6()
{

}

void CNpcScript::mafl_forestmanager_7()
{

}

void CNpcScript::mafl_forestmanager_8()
{

}

void CNpcScript::mafl_forestmanager_9()
{
	Say( 1052 );
}

void CNpcScript::mafl_forestmanager_10()
{
	Speak( NpcId(), 1053 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: CollectorManager
// File			: MaFl_CollectorManager.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_collectormanager_0()
{
	Speak( NpcId(), 1054 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_collectormanager_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_collectormanager_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_collectormanager_3()
{
	
}

void CNpcScript::mafl_collectormanager_4()
{
	Say( 1055 );
}

void CNpcScript::mafl_collectormanager_5()
{

}

void CNpcScript::mafl_collectormanager_6()
{

}

void CNpcScript::mafl_collectormanager_7()
{

}

void CNpcScript::mafl_collectormanager_8()
{

}

void CNpcScript::mafl_collectormanager_9()
{
	Say( 1056 );
}

void CNpcScript::mafl_collectormanager_10()
{
	Speak( NpcId(), 1057 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Karin_EVENT
// File			: MaSa_Karin_EVENT.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::masa_karin_event_0()
{
	Speak( NpcId(), 1058 );
	SetScriptTimer( 15 );
}

void CNpcScript::masa_karin_event_1()
{
	LaunchQuest();
}

void CNpcScript::masa_karin_event_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::masa_karin_event_3()
{
	
}

void CNpcScript::masa_karin_event_4()
{
	Say( 1059 );
}

void CNpcScript::masa_karin_event_5()
{

}

void CNpcScript::masa_karin_event_6()
{

}

void CNpcScript::masa_karin_event_7()
{

}

void CNpcScript::masa_karin_event_8()
{

}

void CNpcScript::masa_karin_event_9()
{
	Say( 1060 );
}

void CNpcScript::masa_karin_event_10()
{
	Speak( NpcId(), 1061 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Priest
// File			: Mafl_Priest.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_priest_0()
{
	Speak( NpcId(), 1062 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_priest_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_priest_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_priest_3()
{
	
}

void CNpcScript::mafl_priest_4()
{
	Say( 1063 );
}

void CNpcScript::mafl_priest_5()
{

}

void CNpcScript::mafl_priest_6()
{

}

void CNpcScript::mafl_priest_7()
{

}

void CNpcScript::mafl_priest_8()
{

}

void CNpcScript::mafl_priest_9()
{
	Say( 1064 );
}

void CNpcScript::mafl_priest_10()
{
	Speak( NpcId(), 1065 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: PrepaidCard
// File			: MaFl_PrepaidCard.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_prepaidcard_0()
{
	Speak( NpcId(), 1066 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_prepaidcard_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_prepaidcard_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_prepaidcard_3()
{
	
}

void CNpcScript::mafl_prepaidcard_4()
{
	Say( 1067 );
}

void CNpcScript::mafl_prepaidcard_5()
{

}

void CNpcScript::mafl_prepaidcard_6()
{

}

void CNpcScript::mafl_prepaidcard_7()
{

}

void CNpcScript::mafl_prepaidcard_8()
{

}

void CNpcScript::mafl_prepaidcard_9()
{
	Say( 1068 );
}

void CNpcScript::mafl_prepaidcard_10()
{
	Speak( NpcId(), 1069 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: July03
// File			: MaFl_July03.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_july03_0()
{
	Speak( NpcId(), 1070 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_july03_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_july03_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_july03_3()
{
	
}

void CNpcScript::mafl_july03_4()
{
	Say( 1071 );
}

void CNpcScript::mafl_july03_5()
{

}

void CNpcScript::mafl_july03_6()
{

}

void CNpcScript::mafl_july03_7()
{

}

void CNpcScript::mafl_july03_8()
{

}

void CNpcScript::mafl_july03_9()
{
	Say( 1072 );
}

void CNpcScript::mafl_july03_10()
{
	Speak( NpcId(), 1073 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: July03
// File			: MaFl_July03.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_july04_0()
{
	Speak( NpcId(), 1074 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_july04_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_july04_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_july04_3()
{
	
}

void CNpcScript::mafl_july04_4()
{

}

void CNpcScript::mafl_july04_5()
{

}

void CNpcScript::mafl_july04_6()
{

}

void CNpcScript::mafl_july04_7()
{

}

void CNpcScript::mafl_july04_8()
{

}

void CNpcScript::mafl_july04_9()
{
	Say( 1076 );
}

void CNpcScript::mafl_july04_10()
{
	Speak( NpcId(), 1077 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: SecretRoom_EAST
// File			: MaFl_SecretRoom_EAST.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_secretroom_east_0()
{
	Speak( NpcId(), 1078 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_secretroom_east_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_secretroom_east_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_secretroom_east_3()
{
	
}

void CNpcScript::mafl_secretroom_east_4()
{
	Say( 1079 );
}

void CNpcScript::mafl_secretroom_east_5()
{

}

void CNpcScript::mafl_secretroom_east_6()
{

}

void CNpcScript::mafl_secretroom_east_7()
{

}

void CNpcScript::mafl_secretroom_east_8()
{

}

void CNpcScript::mafl_secretroom_east_9()
{
	Say( 1080 );
}

void CNpcScript::mafl_secretroom_east_10()
{
	Speak( NpcId(), 1081 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: SecretRoom_WEST
// File			: MaFl_SecretRoom_WEST.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_secretroom_west_0()
{
	Speak( NpcId(), 1082 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_secretroom_west_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_secretroom_west_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_secretroom_west_3()
{
	
}

void CNpcScript::mafl_secretroom_west_4()
{
	Say( 1083 );
}

void CNpcScript::mafl_secretroom_west_5()
{

}

void CNpcScript::mafl_secretroom_west_6()
{

}

void CNpcScript::mafl_secretroom_west_7()
{

}

void CNpcScript::mafl_secretroom_west_8()
{

}

void CNpcScript::mafl_secretroom_west_9()
{
	Say( 1084 );
}

void CNpcScript::mafl_secretroom_west_10()
{
	Speak( NpcId(), 1085 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: CardMaster
// File			: MaFl_CardMaster.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_cardmaster_0()
{
	Speak( NpcId(), 1086 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_cardmaster_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_cardmaster_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_cardmaster_3()
{
	
}

void CNpcScript::mafl_cardmaster_4()
{
	Say( 1087 );
}

void CNpcScript::mafl_cardmaster_5()
{

}

void CNpcScript::mafl_cardmaster_6()
{

}

void CNpcScript::mafl_cardmaster_7()
{

}

void CNpcScript::mafl_cardmaster_8()
{

}

void CNpcScript::mafl_cardmaster_9()
{
	Say( 1088 );
}

void CNpcScript::mafl_cardmaster_10()
{
	Speak( NpcId(), 1089 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: Peach
// File			: MaFl_Peach.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_peach_0()
{
	Speak( NpcId(), 1090 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_peach_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_peach_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_peach_3()
{
	
}

void CNpcScript::mafl_peach_4()
{
	Say( 1091 );
}

void CNpcScript::mafl_peach_5()
{

}

void CNpcScript::mafl_peach_6()
{

}

void CNpcScript::mafl_peach_7()
{

}

void CNpcScript::mafl_peach_8()
{

}

void CNpcScript::mafl_peach_9()
{
	Say( 1092 );
}

void CNpcScript::mafl_peach_10()
{
	Speak( NpcId(), 1093 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: MINIWHEEL
// File			: MaFl_MiniWheel.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_miniwheel_0()
{
	Speak( NpcId(), 1094 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_miniwheel_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_miniwheel_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_miniwheel_3()
{
	
}

void CNpcScript::mafl_miniwheel_4()
{
	Say( 1095 );
}

void CNpcScript::mafl_miniwheel_5()
{

}

void CNpcScript::mafl_miniwheel_6()
{

}

void CNpcScript::mafl_miniwheel_7()
{

}

void CNpcScript::mafl_miniwheel_8()
{

}

void CNpcScript::mafl_miniwheel_9()
{
	Say( 1096 );
}

void CNpcScript::mafl_miniwheel_10()
{
	Speak( NpcId(), 1097 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: EVENTBALL
// File			: MaFl_EventBall.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_eventball_0()
{
	Speak( NpcId(), 1098 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_eventball_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_eventball_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_eventball_3()
{
	
}

void CNpcScript::mafl_eventball_4()
{
	Say( 1099 );
}

void CNpcScript::mafl_eventball_5()
{

}

void CNpcScript::mafl_eventball_6()
{

}

void CNpcScript::mafl_eventball_7()
{

}

void CNpcScript::mafl_eventball_8()
{

}

void CNpcScript::mafl_eventball_9()
{
	Say( 1100 );
}

void CNpcScript::mafl_eventball_10()
{
	Speak( NpcId(), 1101 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: HALLOWEEN
// File			: MaFl_RedRobeGirl.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_redrobegirl_0()
{
	Speak( NpcId(), 1102 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_redrobegirl_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_redrobegirl_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_redrobegirl_3()
{
	
}

void CNpcScript::mafl_redrobegirl_4()
{
	Say( 1103 );
}

void CNpcScript::mafl_redrobegirl_5()
{

}

void CNpcScript::mafl_redrobegirl_6()
{

}

void CNpcScript::mafl_redrobegirl_7()
{

}

void CNpcScript::mafl_redrobegirl_8()
{

}

void CNpcScript::mafl_redrobegirl_9()
{
	Say( 1104 );
}

void CNpcScript::mafl_redrobegirl_10()
{
	Speak( NpcId(), 1105 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: HOUSING
// File			: Mafl_Charlie.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_charlie_0()
{
	Speak( NpcId(), 1106 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_charlie_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_charlie_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_charlie_3()
{
	
}

void CNpcScript::mafl_charlie_4()
{
	Say( 1107 );
}

void CNpcScript::mafl_charlie_5()
{

}

void CNpcScript::mafl_charlie_6()
{

}

void CNpcScript::mafl_charlie_7()
{

}

void CNpcScript::mafl_charlie_8()
{

}

void CNpcScript::mafl_charlie_9()
{
	Say( 1108 );
}

void CNpcScript::mafl_charlie_10()
{
	Speak( NpcId(), 1109 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 신년 이벤트
// File			: Mafl_Snowman01.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_snowman01_0()
{
	Speak( NpcId(), 1110 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_snowman01_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_snowman01_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_snowman01_3()
{
	
}

void CNpcScript::mafl_snowman01_4()
{
	Say( 1111 );
}

void CNpcScript::mafl_snowman01_5()
{

}

void CNpcScript::mafl_snowman01_6()
{

}

void CNpcScript::mafl_snowman01_7()
{

}

void CNpcScript::mafl_snowman01_8()
{

}

void CNpcScript::mafl_snowman01_9()
{
	Say( 1112 );
}

void CNpcScript::mafl_snowman01_10()
{
	Speak( NpcId(), 1113 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 신년 이벤트
// File			: Mafl_Snowman02.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_snowman02_0()
{
	Speak( NpcId(), 1114 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_snowman02_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_snowman02_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_snowman02_3()
{
	
}

void CNpcScript::mafl_snowman02_4()
{
	Say( 1115 );
}

void CNpcScript::mafl_snowman02_5()
{

}

void CNpcScript::mafl_snowman02_6()
{

}

void CNpcScript::mafl_snowman02_7()
{

}

void CNpcScript::mafl_snowman02_8()
{

}

void CNpcScript::mafl_snowman02_9()
{
	Say( 1116 );
}

void CNpcScript::mafl_snowman02_10()
{
	Speak( NpcId(), 1117 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 09년 발렌타인 이벤트
// File			: Mafl_Grahoo.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_grahoo_0()
{
	Speak( NpcId(), 1118 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_grahoo_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_grahoo_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_grahoo_3()
{
	
}

void CNpcScript::mafl_grahoo_4()
{
	Say( 1119 );
}

void CNpcScript::mafl_grahoo_5()
{

}

void CNpcScript::mafl_grahoo_6()
{

}

void CNpcScript::mafl_grahoo_7()
{

}

void CNpcScript::mafl_grahoo_8()
{

}

void CNpcScript::mafl_grahoo_9()
{
	Say( 1120 );
}

void CNpcScript::mafl_grahoo_10()
{
	Speak( NpcId(), 1121 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 09년 화이트 데이 이벤트
// File			: Mafl_Aberoo.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_aberoo_0()
{
	Speak( NpcId(), 1122 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_aberoo_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_aberoo_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_aberoo_3()
{
	Say ( 1126 );	
}

void CNpcScript::mafl_aberoo_4()
{
	Say( 1123 );
}

void CNpcScript::mafl_aberoo_5()
{

}

void CNpcScript::mafl_aberoo_6()
{

}

void CNpcScript::mafl_aberoo_7()
{

}

void CNpcScript::mafl_aberoo_8()
{

}

void CNpcScript::mafl_aberoo_9()
{
	Say( 1124 );
}

void CNpcScript::mafl_aberoo_10()
{
	Speak( NpcId(), 1125 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 09년 궁중 요리사를 도와라 이벤트
// File			: Mafl_Cook01.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_cook01_0()
{
	Speak( NpcId(), 1126 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_cook01_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_cook01_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_cook01_3()
{
	
}

void CNpcScript::mafl_cook01_4()
{
	Say( 1127 );
}

void CNpcScript::mafl_cook01_5()
{

}

void CNpcScript::mafl_cook01_6()
{

}

void CNpcScript::mafl_cook01_7()
{

}

void CNpcScript::mafl_cook01_8()
{

}

void CNpcScript::mafl_cook01_9()
{
	Say( 1128 );
}

void CNpcScript::mafl_cook01_10()
{
	Speak( NpcId(), 1129 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 국내 활성화 이벤트
// File			: Mafl_Alice.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_alice_0()
{
	Speak( NpcId(), 1131 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_alice_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_alice_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_alice_3()
{
	
}

void CNpcScript::mafl_alice_4()
{
	Say( 1132 );
}

void CNpcScript::mafl_alice_5()
{

}

void CNpcScript::mafl_alice_6()
{

}

void CNpcScript::mafl_alice_7()
{

}

void CNpcScript::mafl_alice_8()
{

}

void CNpcScript::mafl_alice_9()
{
	Say( 1133 );
}

void CNpcScript::mafl_alice_10()
{
	Speak( NpcId(), 1134 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 달려라 운동회
// File			: Mafl_Oasis.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_oasis_0()
{
	Speak( NpcId(), 1135 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_oasis_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_oasis_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_oasis_3()
{
	
}

void CNpcScript::mafl_oasis_4()
{
	Say( 1136 );
}

void CNpcScript::mafl_oasis_5()
{

}

void CNpcScript::mafl_oasis_6()
{

}

void CNpcScript::mafl_oasis_7()
{

}

void CNpcScript::mafl_oasis_8()
{

}

void CNpcScript::mafl_oasis_9()
{
	Say( 1137 );
}

void CNpcScript::mafl_oasis_10()
{
	Speak( NpcId(), 1138 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 하르모닌 NPC
// File			: MaHa_Jano.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::maha_jano_0()
{
	Speak( NpcId(), 1139 );
	SetScriptTimer( 15 );
}

void CNpcScript::maha_jano_1()
{
	LaunchQuest();
}

void CNpcScript::maha_jano_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::maha_jano_3()
{
		Say( 1140 );
}

void CNpcScript::maha_jano_4()
{
	Say( 1141 );
}

void CNpcScript::maha_jano_5()
{
			
}

void CNpcScript::maha_jano_6()
{

}

void CNpcScript::maha_jano_7()
{

}

void CNpcScript::maha_jano_8()
{

}

void CNpcScript::maha_jano_9()
{
	Say( 1142 );
}

void CNpcScript::maha_jano_10()
{
	Speak( NpcId(), 1143 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 하르모닌 NPC
// File			: MaHa_luca.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::maha_luca_0()
{
	Speak( NpcId(), 1144 );
	SetScriptTimer( 15 );
}

void CNpcScript::maha_luca_1()
{
	LaunchQuest();
}

void CNpcScript::maha_luca_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::maha_luca_3()
{
		Say( 1145 );
}

void CNpcScript::maha_luca_4()
{
	Say( 1146 );
}

void CNpcScript::maha_luca_5()
{
			
}

void CNpcScript::maha_luca_6()
{

}

void CNpcScript::maha_luca_7()
{

}

void CNpcScript::maha_luca_8()
{

}

void CNpcScript::maha_luca_9()
{
	Say( 1147 );
}

void CNpcScript::maha_luca_10()
{
	Speak( NpcId(), 1148 );
	Exit();
}
//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 하르모닌 NPC
// File			: MaHa_vespu.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::maha_vespu_0()
{
	Speak( NpcId(), 1149 );
	SetScriptTimer( 15 );
}

void CNpcScript::maha_vespu_1()
{
	LaunchQuest();
}

void CNpcScript::maha_vespu_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::maha_vespu_3()
{
		Say( 1150 );
}

void CNpcScript::maha_vespu_4()
{
	Say( 1151 );
}

void CNpcScript::maha_vespu_5()
{
			
}

void CNpcScript::maha_vespu_6()
{

}

void CNpcScript::maha_vespu_7()
{

}

void CNpcScript::maha_vespu_8()
{

}

void CNpcScript::maha_vespu_9()
{
	Say( 1152 );
}

void CNpcScript::maha_vespu_10()
{
	Speak( NpcId(), 1153 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 하르모닌 NPC
// File			: MaDa_esof.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_esof_0()
{
	Speak( NpcId(), 1155 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_esof_1()
{
	LaunchQuest();
}

void CNpcScript::mada_esof_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_esof_3()
{
		Say( 1156 );
}

void CNpcScript::mada_esof_4()
{
	Say( 1157 );
}

void CNpcScript::mada_esof_5()
{
			
}

void CNpcScript::mada_esof_6()
{

}

void CNpcScript::mada_esof_7()
{

}

void CNpcScript::mada_esof_8()
{

}

void CNpcScript::mada_esof_9()
{
	Say( 1158 );
}

void CNpcScript::mada_esof_10()
{
	Speak( NpcId(), 1159 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 하르모닌 NPC
// File			: MaHa_ryan.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::maha_ryan_0()
{
	Speak( NpcId(), 1160 );
	SetScriptTimer( 15 );
}

void CNpcScript::maha_ryan_1()
{
	LaunchQuest();
}

void CNpcScript::maha_ryan_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::maha_ryan_3()
{
		Say( 1161 );
}

void CNpcScript::maha_ryan_4()
{
	Say( 1162 );
}

void CNpcScript::maha_ryan_5()
{
			
}

void CNpcScript::maha_ryan_6()
{

}

void CNpcScript::maha_ryan_7()
{

}

void CNpcScript::maha_ryan_8()
{

}

void CNpcScript::maha_ryan_9()
{
	Say( 1163 );
}

void CNpcScript::maha_ryan_10()
{
	Speak( NpcId(), 1164 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 하르모닌 NPC
// File			: MaHa_lasa.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::maha_lasa_0()
{
	Speak( NpcId(), 1165 );
	SetScriptTimer( 15 );
}

void CNpcScript::maha_lasa_1()
{
	LaunchQuest();
}

void CNpcScript::maha_lasa_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::maha_lasa_3()
{
		Say( 1166 );
}

void CNpcScript::maha_lasa_4()
{
	Say( 1167 );
}

void CNpcScript::maha_lasa_5()
{
			
}

void CNpcScript::maha_lasa_6()
{

}

void CNpcScript::maha_lasa_7()
{

}

void CNpcScript::maha_lasa_8()
{

}

void CNpcScript::maha_lasa_9()
{
	Say( 1168 );
}

void CNpcScript::maha_lasa_10()
{
	Speak( NpcId(), 1169 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: ysiwar
// Title		: 플라리스 NPC
// File			: MaFl_Meron.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_meron_0()
{
	Speak( NpcId(), 1170 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_meron_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_meron_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_meron_3()
{
		Say( 1171 );
}

void CNpcScript::mafl_meron_4()
{
	Say( 1172 );
}

void CNpcScript::mafl_meron_5()
{
			
}

void CNpcScript::mafl_meron_6()
{

}

void CNpcScript::mafl_meron_7()
{

}

void CNpcScript::mafl_meron_8()
{

}

void CNpcScript::mafl_meron_9()
{
	Say( 1173 );
}

void CNpcScript::mafl_meron_10()
{
	Speak( NpcId(), 1174 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: ysiwar
// Title		: 플라리스 NPC
// File			: MaFl_August.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_august_0()
{
	Speak( NpcId(), 1175 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_august_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_august_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_august_3()
{

}

void CNpcScript::mafl_august_4()
{
	Say( 1176 );
}

void CNpcScript::mafl_august_5()
{
			
}

void CNpcScript::mafl_august_6()
{

}

void CNpcScript::mafl_august_7()
{

}

void CNpcScript::mafl_august_8()
{

}

void CNpcScript::mafl_august_9()
{
	Say( 1177 );
}

void CNpcScript::mafl_august_10()
{
	Speak( NpcId(), 1178 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: ysiwar
// Title		: 플라리스 NPC
// File			: MaFl_picksirose.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_picksirose_0()
{
	Speak( NpcId(), 1179 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_picksirose_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_picksirose_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_picksirose_3()
{

}

void CNpcScript::mafl_picksirose_4()
{
	Say( 1180 );
}

void CNpcScript::mafl_picksirose_5()
{
			
}

void CNpcScript::mafl_picksirose_6()
{

}

void CNpcScript::mafl_picksirose_7()
{

}

void CNpcScript::mafl_picksirose_8()
{

}

void CNpcScript::mafl_picksirose_9()
{
	Say( 1181 );
}

void CNpcScript::mafl_picksirose_10()
{
	Speak( NpcId(), 1182 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: ysiwar
// Title		: 플라리스 NPC
// File			: MaFl_heroevent01.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_heroevent01_0()
{
	Speak( NpcId(), 1183 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_heroevent01_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_heroevent01_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_heroevent01_3()
{

}

void CNpcScript::mafl_heroevent01_4()
{
	Say( 1184 );
}

void CNpcScript::mafl_heroevent01_5()
{
			
}

void CNpcScript::mafl_heroevent01_6()
{

}

void CNpcScript::mafl_heroevent01_7()
{

}

void CNpcScript::mafl_heroevent01_8()
{

}

void CNpcScript::mafl_heroevent01_9()
{
	Say( 1185 );
}

void CNpcScript::mafl_heroevent01_10()
{
	Speak( NpcId(), 1186 );
	Exit();
}
//Script End
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: 안성국
// Title		: 플라리스 NPC
// File			: MaFl_Vanharlen.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_vanharlen_0()
{
	Speak( NpcId(), 1187 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_vanharlen_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_vanharlen_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_vanharlen_3()
{

}

void CNpcScript::mafl_vanharlen_4()
{
	Say( 1188 );
}

void CNpcScript::mafl_vanharlen_5()
{
			
}

void CNpcScript::mafl_vanharlen_6()
{

}

void CNpcScript::mafl_vanharlen_7()
{

}

void CNpcScript::mafl_vanharlen_8()
{

}

void CNpcScript::mafl_vanharlen_9()
{
	Say( 1189 );
}

void CNpcScript::mafl_vanharlen_10()
{
	Speak( NpcId(), 1190 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: 안성국
// Title		: 플라리스 NPC
// File			: MaFl_ete.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_ete_0()
{
	Speak( NpcId(), 1191 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_ete_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_ete_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_ete_3()
{

}

void CNpcScript::mafl_ete_4()
{
	Say( 1192 );
}

void CNpcScript::mafl_ete_5()
{
			
}

void CNpcScript::mafl_ete_6()
{

}

void CNpcScript::mafl_ete_7()
{

}

void CNpcScript::mafl_ete_8()
{

}

void CNpcScript::mafl_ete_9()
{
	Say( 1193 );
}

void CNpcScript::mafl_ete_10()
{
	Speak( NpcId(), 1194 );
	Exit();
}
//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: 안성국
// Title		: 플라리스 NPC
// File			: MaFl_tonge.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_tonge_0()
{
	Speak( NpcId(), 1195 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_tonge_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_tonge_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_tonge_3()
{

}

void CNpcScript::mafl_tonge_4()
{
	Say( 1196 );
}

void CNpcScript::mafl_tonge_5()
{
			
}

void CNpcScript::mafl_tonge_6()
{

}

void CNpcScript::mafl_tonge_7()
{

}

void CNpcScript::mafl_tonge_8()
{

}

void CNpcScript::mafl_tonge_9()
{
	Say( 1197 );
}

void CNpcScript::mafl_tonge_10()
{
	Speak( NpcId(), 1198 );
	Exit();
}
//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: 안성국
// Title		: 플라리스 NPC
// File			: MaFl_missvtn.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_missvtn_0()
{
	Speak( NpcId(), 1199 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_missvtn_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_missvtn_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_missvtn_3()
{

}

void CNpcScript::mafl_missvtn_4()
{
	Say( 1200 );
}

void CNpcScript::mafl_missvtn_5()
{
			
}

void CNpcScript::mafl_missvtn_6()
{

}

void CNpcScript::mafl_missvtn_7()
{

}

void CNpcScript::mafl_missvtn_8()
{

}

void CNpcScript::mafl_missvtn_9()
{
	Say( 1201 );
}

void CNpcScript::mafl_missvtn_10()
{
	Speak( NpcId(), 1202 );
	Exit();
}
//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: 안성국
// Title		: 플라리스 NPC
// File			: MaFl_teleporter_03nomal.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_teleporter_03nomal_0()
{
	Speak( NpcId(), 1203 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_teleporter_03nomal_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_teleporter_03nomal_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_teleporter_03nomal_3()
{

}

void CNpcScript::mafl_teleporter_03nomal_4()
{
	Say( 1204 );
}

void CNpcScript::mafl_teleporter_03nomal_5()
{
			
}

void CNpcScript::mafl_teleporter_03nomal_6()
{

}

void CNpcScript::mafl_teleporter_03nomal_7()
{

}

void CNpcScript::mafl_teleporter_03nomal_8()
{

}

void CNpcScript::mafl_teleporter_03nomal_9()
{
	Say( 1205 );
}

void CNpcScript::mafl_teleporter_03nomal_10()
{
	Speak( NpcId(), 1206 );
	Exit();
}
//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: 안성국
// Title		: 플라리스 NPC
// File			: MaFl_guildhousesale.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_guildhousesale_0()
{
	Speak( NpcId(), 1207 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_guildhousesale_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_guildhousesale_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_guildhousesale_3()
{

}

void CNpcScript::mafl_guildhousesale_4()
{
	Say( 1208 );
}

void CNpcScript::mafl_guildhousesale_5()
{
			
}

void CNpcScript::mafl_guildhousesale_6()
{

}

void CNpcScript::mafl_guildhousesale_7()
{

}

void CNpcScript::mafl_guildhousesale_8()
{

}

void CNpcScript::mafl_guildhousesale_9()
{
	Say( 1209 );
}

void CNpcScript::mafl_guildhousesale_10()
{
	Speak( NpcId(), 1210 );
	Exit();
}
//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: 안성국
// Title		: 플라리스 NPC
// File			: MaFl_Christmas_Romeo.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_christmas_romeo_0()
{
	Speak( NpcId(), 1211 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_christmas_romeo_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_christmas_romeo_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_christmas_romeo_3()
{

}

void CNpcScript::mafl_christmas_romeo_4()
{
	Say( 1212 );
}

void CNpcScript::mafl_christmas_romeo_5()
{
			
}

void CNpcScript::mafl_christmas_romeo_6()
{

}

void CNpcScript::mafl_christmas_romeo_7()
{

}

void CNpcScript::mafl_christmas_romeo_8()
{

}

void CNpcScript::mafl_christmas_romeo_9()
{
	Say( 1213 );
}

void CNpcScript::mafl_christmas_romeo_10()
{
	Speak( NpcId(), 1214 );
	Exit();
}
//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: 안성국
// Title		: 플라리스 NPC
// File			: MaFl_Christmas_Juliet.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_christmas_juliet_0()
{
	Speak( NpcId(), 1215 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_christmas_juliet_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_christmas_juliet_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_christmas_juliet_3()
{

}

void CNpcScript::mafl_christmas_juliet_4()
{
	Say( 1216 );
}

void CNpcScript::mafl_christmas_juliet_5()
{
			
}

void CNpcScript::mafl_christmas_juliet_6()
{

}

void CNpcScript::mafl_christmas_juliet_7()
{

}

void CNpcScript::mafl_christmas_juliet_8()
{

}

void CNpcScript::mafl_christmas_juliet_9()
{
	Say( 1217 );
}

void CNpcScript::mafl_christmas_juliet_10()
{
	Speak( NpcId(), 1218 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: ysiwar
// Title		: 다콘 스승 NPC
// File			: MaDa_Icinis.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_icinis_0()
{
	Speak( NpcId(), 1219 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_icinis_1()
{
	LaunchQuest();
}

void CNpcScript::mada_icinis_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_icinis_3()
{
	Say( 1220 );
}

void CNpcScript::mada_icinis_4()
{
	Say( 1221 );
}

void CNpcScript::mada_icinis_5()
{
			
}

void CNpcScript::mada_icinis_6()
{

}

void CNpcScript::mada_icinis_7()
{

}

void CNpcScript::mada_icinis_8()
{

}

void CNpcScript::mada_icinis_9()
{
	Say( 1222 );
}

void CNpcScript::mada_icinis_10()
{
	Speak( NpcId(), 1223 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: 안성국
// Title		: 플라리스 NPC
// File			: MaFl_commoner03.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_commoner03_0()
{
	Speak( NpcId(), 1224 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_commoner03_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_commoner03_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_commoner03_3()
{

}

void CNpcScript::mafl_commoner03_4()
{
	Say( 1225 );
}

void CNpcScript::mafl_commoner03_5()
{
			
}

void CNpcScript::mafl_commoner03_6()
{

}

void CNpcScript::mafl_commoner03_7()
{

}

void CNpcScript::mafl_commoner03_8()
{

}

void CNpcScript::mafl_commoner03_9()
{
	Say( 1226 );
}

void CNpcScript::mafl_commoner03_10()
{
	Speak( NpcId(), 1227 );
	Exit();
}


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: 안성국
// Title		: 플라리스 NPC
// File			: MaFl_teleporter_03veteran.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_teleporter_03veteran_0()
{
	Speak( NpcId(), 1228 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_teleporter_03veteran_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_teleporter_03veteran_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_teleporter_03veteran_3()
{

}

void CNpcScript::mafl_teleporter_03veteran_4()
{
	Say( 1229 );
}

void CNpcScript::mafl_teleporter_03veteran_5()
{
			
}

void CNpcScript::mafl_teleporter_03veteran_6()
{

}

void CNpcScript::mafl_teleporter_03veteran_7()
{

}

void CNpcScript::mafl_teleporter_03veteran_8()
{

}

void CNpcScript::mafl_teleporter_03veteran_9()
{
	Say( 1230 );
}

void CNpcScript::mafl_teleporter_03veteran_10()
{
	Speak( NpcId(), 1231 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: ysiwar
// Title		: 다콘 제련 전문가 npc
// File			: MaDa_Laydian.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mada_laydian_0()
{
	Speak( NpcId(), 1232 );
	SetScriptTimer( 15 );
}

void CNpcScript::mada_laydian_1()
{
	LaunchQuest();
}

void CNpcScript::mada_laydian_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mada_laydian_3()
{

}

void CNpcScript::mada_laydian_4()
{
	Say( 1233 );
}

void CNpcScript::mada_laydian_5()
{
			
}

void CNpcScript::mada_laydian_6()
{

}

void CNpcScript::mada_laydian_7()
{

}

void CNpcScript::mada_laydian_8()
{

}

void CNpcScript::mada_laydian_9()
{
	Say( 1234 );
}

void CNpcScript::mada_laydian_10()
{
	Speak( NpcId(), 1235 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 큐피드 신 npc
// File			: MaFl_Cupid.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_cupid_0()
{
	Speak( NpcId(), 1236 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_cupid_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_cupid_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_cupid_3()
{

}

void CNpcScript::mafl_cupid_4()
{
	Say( 1237 );
}

void CNpcScript::mafl_cupid_5()
{
			
}

void CNpcScript::mafl_cupid_6()
{

}

void CNpcScript::mafl_cupid_7()
{

}

void CNpcScript::mafl_cupid_8()
{

}

void CNpcScript::mafl_cupid_9()
{
	Say( 1238 );
}

void CNpcScript::mafl_cupid_10()
{
	Speak( NpcId(), 1239 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 신년 특파원 npc
// File			: MaFl_Correspondent.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_correspondent_0()
{
	Speak( NpcId(), 1240 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_correspondent_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_correspondent_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_correspondent_3()
{

}

void CNpcScript::mafl_correspondent_4()
{

}

void CNpcScript::mafl_correspondent_5()
{
			
}

void CNpcScript::mafl_correspondent_6()
{

}

void CNpcScript::mafl_correspondent_7()
{

}

void CNpcScript::mafl_correspondent_8()
{

}

void CNpcScript::mafl_correspondent_9()
{

}

void CNpcScript::mafl_correspondent_10()
{
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 완소뉴비 지원 담당관
// File			: MaFl_Newbie.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_newbie_0()
{
	Speak( NpcId(), 1241 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_newbie_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_newbie_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_newbie_3()
{
	Say( 1242 );
}

void CNpcScript::mafl_newbie_4()
{
	Say( 1243 );
}

void CNpcScript::mafl_newbie_5()
{
			
}

void CNpcScript::mafl_newbie_6()
{

}

void CNpcScript::mafl_newbie_7()
{

}

void CNpcScript::mafl_newbie_8()
{

}

void CNpcScript::mafl_newbie_9()
{
	Say( 1244 );
}

void CNpcScript::mafl_newbie_10()
{
	Speak( NpcId(), 1245 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 버블의 사제 npc
// File			: MaFl_Bubble.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_bubble_0()
{
	Speak( NpcId(), 1246 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_bubble_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_bubble_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_bubble_3()
{

}

void CNpcScript::mafl_bubble_4()
{
	Say( 1247 );
}

void CNpcScript::mafl_bubble_5()
{
			
}

void CNpcScript::mafl_bubble_6()
{

}

void CNpcScript::mafl_bubble_7()
{

}

void CNpcScript::mafl_bubble_8()
{

}

void CNpcScript::mafl_bubble_9()
{
	Say( 1248 );
}

void CNpcScript::mafl_bubble_10()
{
	Speak( NpcId(), 1249 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 레프러콘 npc
// File			: MaFl_Leprechaun.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_leprechaun_0()
{
	Speak( NpcId(), 1250 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_leprechaun_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_leprechaun_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_leprechaun_3()
{
	Say( 1251 );
}

void CNpcScript::mafl_leprechaun_4()
{
	Say( 1252 );
}

void CNpcScript::mafl_leprechaun_5()
{
			
}

void CNpcScript::mafl_leprechaun_6()
{

}

void CNpcScript::mafl_leprechaun_7()
{

}

void CNpcScript::mafl_leprechaun_8()
{

}

void CNpcScript::mafl_leprechaun_9()
{
	Say( 1253 );
}

void CNpcScript::mafl_leprechaun_10()
{
	Speak( NpcId(), 1254 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: uri110
// Title		: 큐피드
// File			: MaFl_Cupid02.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_cupid02_0()
{
	Speak( NpcId(), 1255 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_cupid02_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_cupid02_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_cupid02_3()
{

}

void CNpcScript::mafl_cupid02_4()
{
	Say( 1256 );
}

void CNpcScript::mafl_cupid02_5()
{
			
}

void CNpcScript::mafl_cupid02_6()
{

}

void CNpcScript::mafl_cupid02_7()
{

}

void CNpcScript::mafl_cupid02_8()
{

}

void CNpcScript::mafl_cupid02_9()
{
	Say( 1257 );
}

void CNpcScript::mafl_cupid02_10()
{
	Speak( NpcId(), 1258 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: piyopiyo20
// Title		: 큐피드
// File			: MaFl_Babario.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_Babario_0()
{
	Speak( NpcId(), 1259 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_Babario_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_Babario_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_Babario_3()
{

}

void CNpcScript::mafl_Babario_4()
{
	Say( 1260 );
}

void CNpcScript::mafl_Babario_5()
{
			
}

void CNpcScript::mafl_Babario_6()
{

}

void CNpcScript::mafl_Babario_7()
{

}

void CNpcScript::mafl_Babario_8()
{

}

void CNpcScript::mafl_Babario_9()
{
	Say( 1261 );
}

void CNpcScript::mafl_Babario_10()
{
	Speak( NpcId(), 1262 );
	Exit();
}

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )
// Author		: piyopiyo20
// Title		: 큐피드
// File			: MaFl_Door.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::mafl_Door_0()
{
	Speak( NpcId(), 1263 );
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_Door_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_Door_2()
{
	AddKey( 9 );
	AddKey( 10 );
}

void CNpcScript::mafl_Door_3()
{

}

void CNpcScript::mafl_Door_4()
{
	Say( 1264 );
}

void CNpcScript::mafl_Door_5()
{
			
}

void CNpcScript::mafl_Door_6()
{

}

void CNpcScript::mafl_Door_7()
{

}

void CNpcScript::mafl_Door_8()
{

}

void CNpcScript::mafl_Door_9()
{
	Say( 1265 );
}

void CNpcScript::mafl_Door_10()
{
	Speak( NpcId(), 1266 );
	Exit();
}
//Script End

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/////////////////////v16 NPCs start////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: Eillun Public Office
// File			: MaEw_Ruidan.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::maew_ruidan_0()
{
	Speak( NpcId(), 1263 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_ruidan_1()
{
	LaunchQuest();
}

void CNpcScript::maew_ruidan_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_ruidan_3()
{
	Say( 1264 );
}

void CNpcScript::maew_ruidan_4()
{
	Say( 1265 );
}

void CNpcScript::maew_ruidan_5()
{	
}

void CNpcScript::maew_ruidan_6()
{	
}

void CNpcScript::maew_ruidan_7()
{	
}

void CNpcScript::maew_ruidan_8()
{	
}

void CNpcScript::maew_ruidan_9()
{
	Say( 1266 );	
}

void CNpcScript::maew_ruidan_10()
{
	Speak( NpcId(), 1267 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: Eillun Mayor
// File			: MaEw_Huntrang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::maew_huntrang_0()
{
	Speak( NpcId(), 1268 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_huntrang_1()
{
	LaunchQuest();
}

void CNpcScript::maew_huntrang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_huntrang_3()
{
	Say( 1269 );
}

void CNpcScript::maew_huntrang_4()
{
	Say( 1270 );
}

void CNpcScript::maew_huntrang_5()
{	
}

void CNpcScript::maew_huntrang_6()
{	
}

void CNpcScript::maew_huntrang_7()
{	
}

void CNpcScript::maew_huntrang_8()
{	
}

void CNpcScript::maew_huntrang_9()
{
	Say( 1271 );	
}

void CNpcScript::maew_huntrang_10()
{
	Speak( NpcId(), 1272 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: Quest Office
// File			: MaEw_Miorang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::maew_miorang_0()
{
	Speak( NpcId(), 1273 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_miorang_1()
{
	LaunchQuest();
}

void CNpcScript::maew_miorang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_miorang_3()
{
	Say( 1274 );
}

void CNpcScript::maew_miorang_4()
{
	Say( 1275 );
}

void CNpcScript::maew_miorang_5()
{	
}

void CNpcScript::maew_miorang_6()
{	
}

void CNpcScript::maew_miorang_7()
{	
}

void CNpcScript::maew_miorang_8()
{	
}

void CNpcScript::maew_miorang_9()
{
	Say( 1276 );	
}

void CNpcScript::maew_miorang_10()
{
	Speak( NpcId(), 1277 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: Magic Vendor
// File			: MaEw_Mawrang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin
void CNpcScript::maew_mawrang_0()
{
	Speak( NpcId(), 1278 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_mawrang_1()
{
	LaunchQuest();
}

void CNpcScript::maew_mawrang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_mawrang_3()
{
	Say( 1279 );
}

void CNpcScript::maew_mawrang_4()
{
	Say( 1280 );
}

void CNpcScript::maew_mawrang_5()
{	
}

void CNpcScript::maew_mawrang_6()
{	
}

void CNpcScript::maew_mawrang_7()
{	
}

void CNpcScript::maew_mawrang_8()
{	
}

void CNpcScript::maew_mawrang_9()
{
	Say( 1281 );	
}

void CNpcScript::maew_mawrang_10()
{
	Speak( NpcId(), 1282 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: Weapon Vendor
// File			: MaEw_Kanrang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_kanrang_0()
{
	Speak( NpcId(), 1283 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_kanrang_1()
{
	LaunchQuest();
}

void CNpcScript::maew_kanrang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_kanrang_3()
{
	Say( 1284 );
}

void CNpcScript::maew_kanrang_4()
{
	Say( 1285 );
}

void CNpcScript::maew_kanrang_5()
{	
}

void CNpcScript::maew_kanrang_6()
{	
}

void CNpcScript::maew_kanrang_7()
{	
}

void CNpcScript::maew_kanrang_8()
{	
}

void CNpcScript::maew_kanrang_9()
{
	Say( 1286 );	
}

void CNpcScript::maew_kanrang_10()
{
	Speak( NpcId(), 1287 );
	Exit();
}

//Script End


//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: Arena Manager Rarbadan
// File			: MaEw_Ralbadan.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_ralbadan_0()
{
	Speak( NpcId(), 1288 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_ralbadan_1()
{
	LaunchQuest();
}

void CNpcScript::maew_ralbadan_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_ralbadan_3()
{
	Say( 1289 );
}

void CNpcScript::maew_ralbadan_4()
{
	Say( 1290 );
}

void CNpcScript::maew_ralbadan_5()
{	
}

void CNpcScript::maew_ralbadan_6()
{	
}

void CNpcScript::maew_ralbadan_7()
{	
}

void CNpcScript::maew_ralbadan_8()
{	
}

void CNpcScript::maew_ralbadan_9()
{
	Say( 1291 );	
}

void CNpcScript::maew_ralbadan_10()
{
	Speak( NpcId(), 1292 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: Chief Guard Rergantus
// File			: MaEw_Relgantus.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_relgantus_0()
{
	Speak( NpcId(), 1293 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_relgantus_1()
{
	LaunchQuest();
}

void CNpcScript::maew_relgantus_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_relgantus_3()
{
	Say( 1294 );
}

void CNpcScript::maew_relgantus_4()
{
	Say( 1295 );
}

void CNpcScript::maew_relgantus_5()
{	
}

void CNpcScript::maew_relgantus_6()
{	
}

void CNpcScript::maew_relgantus_7()
{	
}

void CNpcScript::maew_relgantus_8()
{	
}

void CNpcScript::maew_relgantus_9()
{
	Say( 1296 );	
}

void CNpcScript::maew_relgantus_10()
{
	Speak( NpcId(), 1297 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: Apothecary Miriun
// File			: MaEw_Miriun.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_miriun_0()
{
	Speak( NpcId(), 1298 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_miriun_1()
{
	LaunchQuest();
}

void CNpcScript::maew_miriun_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_miriun_3()
{
	Say( 1299 );
}

void CNpcScript::maew_miriun_4()
{
	Say( 1300 );
}

void CNpcScript::maew_miriun_5()
{	
}

void CNpcScript::maew_miriun_6()
{	
}

void CNpcScript::maew_miriun_7()
{	
}

void CNpcScript::maew_miriun_8()
{	
}

void CNpcScript::maew_miriun_9()
{
	Say( 1301 );	
}

void CNpcScript::maew_miriun_10()
{
	Speak( NpcId(), 1302 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Shield) Karang
// File			: MaEw_Karang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_karang_0()
{
	Speak( NpcId(), 1303 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_karang_1()
{
	LaunchQuest();
}

void CNpcScript::maew_karang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_karang_3()
{
	Say( 1304 );
}

void CNpcScript::maew_karang_4()
{
	Say( 1305 );
}

void CNpcScript::maew_karang_5()
{	
}

void CNpcScript::maew_karang_6()
{	
}

void CNpcScript::maew_karang_7()
{	
}

void CNpcScript::maew_karang_8()
{	
}

void CNpcScript::maew_karang_9()
{
	Say( 1306 );	
}

void CNpcScript::maew_karang_10()
{
	Speak( NpcId(), 1307 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Shield2) Kurang
// File			: MaEw_Kurang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_kurang_0()
{
	Speak( NpcId(), 1308 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_kurang_1()
{
	LaunchQuest();
}

void CNpcScript::maew_kurang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_kurang_3()
{
	Say( 1309 );
}

void CNpcScript::maew_kurang_4()
{
	Say( 1310 );
}

void CNpcScript::maew_kurang_5()
{	
}

void CNpcScript::maew_kurang_6()
{	
}

void CNpcScript::maew_kurang_7()
{	
}

void CNpcScript::maew_kurang_8()
{	
}

void CNpcScript::maew_kurang_9()
{
	Say( 1311 );	
}

void CNpcScript::maew_kurang_10()
{
	Speak( NpcId(), 1312 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Station) Heirang
// File			: MaEw_Heirang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_heirang_0()
{
	Speak( NpcId(), 1313 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_heirang_1()
{
	LaunchQuest();
}

void CNpcScript::maew_heirang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_heirang_3()
{
	Say( 1314 );
}

void CNpcScript::maew_heirang_4()
{
	Say( 1315 );
}

void CNpcScript::maew_heirang_5()
{	
}

void CNpcScript::maew_heirang_6()
{	
}

void CNpcScript::maew_heirang_7()
{	
}

void CNpcScript::maew_heirang_8()
{	
}

void CNpcScript::maew_heirang_9()
{
	Say( 1316 );	
}

void CNpcScript::maew_heirang_10()
{
	Speak( NpcId(), 1317 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Food Vendor) Arang
// File			: MaEw_Mawrang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_arang_0()
{
	Speak( NpcId(), 1318 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_arang_1()
{
	LaunchQuest();
}

void CNpcScript::maew_arang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_arang_3()
{
	Say( 1319 );
}

void CNpcScript::maew_arang_4()
{
	Say( 1320 );
}

void CNpcScript::maew_arang_5()
{	
}

void CNpcScript::maew_arang_6()
{	
}

void CNpcScript::maew_arang_7()
{	
}

void CNpcScript::maew_arang_8()
{	
}

void CNpcScript::maew_arang_9()
{
	Say( 1321 );	
}

void CNpcScript::maew_arang_10()
{
	Speak( NpcId(), 1322 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (General) Maurang
// File			: MaEw_Maurang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_maurang_0()
{
	Speak( NpcId(), 1323 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_maurang_1()
{
	LaunchQuest();
}

void CNpcScript::maew_maurang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_maurang_3()
{
	Say( 1324 );
}

void CNpcScript::maew_maurang_4()
{
	Say( 1325 );
}

void CNpcScript::maew_maurang_5()
{	
}

void CNpcScript::maew_maurang_6()
{	
}

void CNpcScript::maew_maurang_7()
{	
}

void CNpcScript::maew_maurang_8()
{	
}

void CNpcScript::maew_maurang_9()
{
	Say( 1326 );	
}

void CNpcScript::maew_maurang_10()
{
	Speak( NpcId(), 1327 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: Force Master Jobchange
// File			: MaEw_Bato.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_bato_0()
{
	Speak( NpcId(), 1328 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_bato_1()
{
	LaunchQuest();
}

void CNpcScript::maew_bato_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_bato_3()
{
	Say( 1329 );
}

void CNpcScript::maew_bato_4()
{
	Say( 1330 );
}

void CNpcScript::maew_bato_5()
{	
}

void CNpcScript::maew_bato_6()
{	
}

void CNpcScript::maew_bato_7()
{	
}

void CNpcScript::maew_bato_8()
{	
	if( GetQuestState(228) == QS_END && GetPlayerJob() == 29 && GetPlayerLvl() == 130 )
	{
		ChangeJob(37);
	}

	else
	{
		Exit();
	}
}

void CNpcScript::maew_bato_9()
{
	Say( 1331 );	
}

void CNpcScript::maew_bato_10()
{
	Speak( NpcId(), 1332 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: Crack Shooter Jobchange
// File			: MaEw_Meialune.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_meialune_0()
{
	Speak( NpcId(), 1333 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_meialune_1()
{
	LaunchQuest();
}

void CNpcScript::maew_meialune_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_meialune_3()
{
	Say( 1334 );
}

void CNpcScript::maew_meialune_4()
{
	Say( 1335 );
}

void CNpcScript::maew_meialune_5()
{	
}

void CNpcScript::maew_meialune_6()
{	
}

void CNpcScript::maew_meialune_7()
{	
}

void CNpcScript::maew_meialune_8()
{	
	if( GetQuestState(216) == QS_END && GetPlayerJob() == 27 && GetPlayerLvl() == 130 )
	{
		ChangeJob(35);
	}

	else
	{
		Exit();
	}
}

void CNpcScript::maew_meialune_9()
{
	Say( 1336 );	
}

void CNpcScript::maew_meialune_10()
{
	Speak( NpcId(), 1337 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: Elementor Lord Jobchange
// File			: MaEw_Hawrang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_hawrang_0()
{
	Speak( NpcId(), 1338 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_hawrang_1()
{
	LaunchQuest();
}

void CNpcScript::maew_hawrang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_hawrang_3()
{
	Say( 1339 );
}

void CNpcScript::maew_hawrang_4()
{
	Say( 1340 );
}

void CNpcScript::maew_hawrang_5()
{	
}

void CNpcScript::maew_hawrang_6()
{	
}

void CNpcScript::maew_hawrang_7()
{	
}

void CNpcScript::maew_hawrang_8()
{	
	if( GetQuestState(233) == QS_END && GetPlayerJob() == 31 && GetPlayerLvl() == 130 )
	{
		ChangeJob(39);
	}

	else
	{
		Exit();
	}
}

void CNpcScript::maew_hawrang_9()
{
	Say( 1341 );	
}

void CNpcScript::maew_hawrang_10()
{
	Speak( NpcId(), 1342 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Upgrade Specialist) Rutam
// File			: MaEw_Rutam.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_rutam_0()
{
	Speak( NpcId(), 1343 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_rutam_1()
{
	LaunchQuest();
}

void CNpcScript::maew_rutam_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_rutam_3()
{
	Say( 1344 );
}

void CNpcScript::maew_rutam_4()
{
	Say( 1345 );
}

void CNpcScript::maew_rutam_5()
{	
}

void CNpcScript::maew_rutam_6()
{	
}

void CNpcScript::maew_rutam_7()
{	
}

void CNpcScript::maew_rutam_8()
{	
}

void CNpcScript::maew_rutam_9()
{
	Say( 1346 );	
}

void CNpcScript::maew_rutam_10()
{
	Speak( NpcId(), 1347 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Jewel Manager) Raia
// File			: MaEw_Raya.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_raya_0()
{
	Speak( NpcId(), 1348 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_raya_1()
{
	LaunchQuest();
}

void CNpcScript::maew_raya_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_raya_3()
{
	Say( 1349 );
}

void CNpcScript::maew_raya_4()
{
	Say( 1350 );
}

void CNpcScript::maew_raya_5()
{	
}

void CNpcScript::maew_raya_6()
{	
}

void CNpcScript::maew_raya_7()
{	
}

void CNpcScript::maew_raya_8()
{	
}

void CNpcScript::maew_raya_9()
{
	Say( 1351 );	
}

void CNpcScript::maew_raya_10()
{
	Speak( NpcId(), 1352 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Housing Specialist) Harlie
// File			: MaEw_Harlie.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_harlie_0()
{
	Speak( NpcId(), 1353 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_harlie_1()
{
	LaunchQuest();
}

void CNpcScript::maew_harlie_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_harlie_3()
{
	Say( 1354 );
}

void CNpcScript::maew_harlie_4()
{
	Say( 1355 );
}

void CNpcScript::maew_harlie_5()
{	
}

void CNpcScript::maew_harlie_6()
{	
}

void CNpcScript::maew_harlie_7()
{	
}

void CNpcScript::maew_harlie_8()
{	
}

void CNpcScript::maew_harlie_9()
{
	Say( 1356 );	
}

void CNpcScript::maew_harlie_10()
{
	Speak( NpcId(), 1357 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Guild House Manager) Rodella
// File			: MaEw_Rodella.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_rodella_0()
{
	Speak( NpcId(), 1358 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_rodella_1()
{
	LaunchQuest();
}

void CNpcScript::maew_rodella_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_rodella_3()
{
	Say( 1359 );
}

void CNpcScript::maew_rodella_4()
{
	Say( 1360 );
}

void CNpcScript::maew_rodella_5()
{	
}

void CNpcScript::maew_rodella_6()
{	
}

void CNpcScript::maew_rodella_7()
{	
}

void CNpcScript::maew_rodella_8()
{	
}

void CNpcScript::maew_rodella_9()
{
	Say( 1361 );	
}

void CNpcScript::maew_rodella_10()
{
	Speak( NpcId(), 1362 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Card Master) Epie
// File			: MaEw_Epie.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_epie_0()
{
	Speak( NpcId(), 1363 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_epie_1()
{
	LaunchQuest();
}

void CNpcScript::maew_epie_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_epie_3()
{
	Say( 1364 );
}

void CNpcScript::maew_epie_4()
{
	Say( 1365 );
}

void CNpcScript::maew_epie_5()
{	
}

void CNpcScript::maew_epie_6()
{	
}

void CNpcScript::maew_epie_7()
{	
}

void CNpcScript::maew_epie_8()
{	
}

void CNpcScript::maew_epie_9()
{
	Say( 1366 );	
}

void CNpcScript::maew_epie_10()
{
	Speak( NpcId(), 1367 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Makeup Artist) Huri
// File			: MaEw_Huri.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_huri_0()
{
	Speak( NpcId(), 1368 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_huri_1()
{
	LaunchQuest();
}

void CNpcScript::maew_huri_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_huri_3()
{
	Say( 1369 );
}

void CNpcScript::maew_huri_4()
{
	Say( 1370 );
}

void CNpcScript::maew_huri_5()
{	
}

void CNpcScript::maew_huri_6()
{	
}

void CNpcScript::maew_huri_7()
{	
}

void CNpcScript::maew_huri_8()
{	
}

void CNpcScript::maew_huri_9()
{
	Say( 1371 );	
}

void CNpcScript::maew_huri_10()
{
	Speak( NpcId(), 1372 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Hair Designer) Gelga
// File			: MaEw_Gelga.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_gelga_0()
{
	Speak( NpcId(), 1373 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_gelga_1()
{
	LaunchQuest();
}

void CNpcScript::maew_gelga_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_gelga_3()
{
	Say( 1374 );
}

void CNpcScript::maew_gelga_4()
{
	Say( 1375 );
}

void CNpcScript::maew_gelga_5()
{	
}

void CNpcScript::maew_gelga_6()
{	
}

void CNpcScript::maew_gelga_7()
{	
}

void CNpcScript::maew_gelga_8()
{	
}

void CNpcScript::maew_gelga_9()
{
	Say( 1376 );	
}

void CNpcScript::maew_gelga_10()
{
	Speak( NpcId(), 1377 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Red Chip Merchant) Ruoboru
// File			: MaEw_Ruoboru.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_ruoboru_0()
{
	Speak( NpcId(), 1378 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_ruoboru_1()
{
	LaunchQuest();
}

void CNpcScript::maew_ruoboru_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_ruoboru_3()
{
	Say( 1379 );
}

void CNpcScript::maew_ruoboru_4()
{
	Say( 1380 );
}

void CNpcScript::maew_ruoboru_5()
{	
}

void CNpcScript::maew_ruoboru_6()
{	
}

void CNpcScript::maew_ruoboru_7()
{	
}

void CNpcScript::maew_ruoboru_8()
{	
}

void CNpcScript::maew_ruoboru_9()
{
	Say( 1381 );	
}

void CNpcScript::maew_ruoboru_10()
{
	Speak( NpcId(), 1382 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Pet Tamer) Cheirang
// File			: MaEw_Cheirang.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_cheirang_0()
{
	Speak( NpcId(), 1383 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_cheirang_1()
{
	LaunchQuest();
}

void CNpcScript::maew_cheirang_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_cheirang_3()
{
	Say( 1384 );
}

void CNpcScript::maew_cheirang_4()
{
	Say( 1385 );
}

void CNpcScript::maew_cheirang_5()
{	
}

void CNpcScript::maew_cheirang_6()
{	
}

void CNpcScript::maew_cheirang_7()
{	
}

void CNpcScript::maew_cheirang_8()
{	
}

void CNpcScript::maew_cheirang_9()
{
	Say( 1386 );	
}

void CNpcScript::maew_cheirang_10()
{
	Speak( NpcId(), 1387 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Storm Blade Master) Karl
// File			: MaEw_Karl.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_karl_0()
{
	Speak( NpcId(), 1388 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_karl_1()
{
	LaunchQuest();
}

void CNpcScript::maew_karl_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_karl_3()
{
	Say( 1389 );
}

void CNpcScript::maew_karl_4()
{
	Say( 1390 );
}

void CNpcScript::maew_karl_5()
{	
}

void CNpcScript::maew_karl_6()
{	
}

void CNpcScript::maew_karl_7()
{	
}

void CNpcScript::maew_karl_8()
{
	if( GetQuestState(204) == QS_END && GetPlayerJob() == 25 && GetPlayerLvl() == 130 )
	{
		ChangeJob(33);
	}

	else
	{
		Exit();
	}
}

void CNpcScript::maew_karl_9()
{
	Say( 1391 );	
}

void CNpcScript::maew_karl_10()
{
	Speak( NpcId(), 1392 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Florist Master) Isillis
// File			: MaEw_Isilis.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_isilis_0()
{
	Speak( NpcId(), 1393 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_isilis_1()
{
	LaunchQuest();
}

void CNpcScript::maew_isilis_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_isilis_3()
{
	Say( 1394 );
}

void CNpcScript::maew_isilis_4()
{
	Say( 1395 );
}

void CNpcScript::maew_isilis_5()
{	
}

void CNpcScript::maew_isilis_6()
{	
}

void CNpcScript::maew_isilis_7()
{	
}

void CNpcScript::maew_isilis_8()
{	
	if( GetQuestState(222) == QS_END && GetPlayerJob() == 28 && GetPlayerLvl() == 130 )
	{
		ChangeJob(36);
	}

	else
	{
		Exit();
	}
}

void CNpcScript::maew_isilis_9()
{
	Say( 1396 );	
}

void CNpcScript::maew_isilis_10()
{
	Speak( NpcId(), 1397 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Mentalist Master) Romain
// File			: MaEw_Romain.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_romain_0()
{
	Speak( NpcId(), 1398 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_romain_1()
{
	LaunchQuest();
}

void CNpcScript::maew_romain_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_romain_3()
{
	Say( 1399 );
}

void CNpcScript::maew_romain_4()
{
	Say( 1400 );
}

void CNpcScript::maew_romain_5()
{	
}

void CNpcScript::maew_romain_6()
{	
}

void CNpcScript::maew_romain_7()
{	
}

void CNpcScript::maew_romain_8()
{	
	if( GetQuestState(238) == QS_END && GetPlayerJob() == 30 && GetPlayerLvl() == 130 )
	{
		ChangeJob(38);
	}

	else
	{
		Exit();
	}
}

void CNpcScript::maew_romain_9()
{
	Say( 1401 );	
}

void CNpcScript::maew_romain_10()
{
	Speak( NpcId(), 1402 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Windlurker Master) Branka
// File			: MaEw_Branka.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_branka_0()
{
	Speak( NpcId(), 1403 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_branka_1()
{
	LaunchQuest();
}

void CNpcScript::maew_branka_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_branka_3()
{
	Say( 1404 );
}

void CNpcScript::maew_branka_4()
{
	Say( 1405 );
}

void CNpcScript::maew_branka_5()
{	
}

void CNpcScript::maew_branka_6()
{	
}

void CNpcScript::maew_branka_7()
{	
}

void CNpcScript::maew_branka_8()
{	
	if( GetQuestState(210) == QS_END && GetPlayerJob() == 26 && GetPlayerLvl() == 130 )
	{
		ChangeJob(34);
	}

	else
	{
		Exit();
	}
}

void CNpcScript::maew_branka_9()
{
	Say( 1406 );	
}

void CNpcScript::maew_branka_10()
{
	Speak( NpcId(), 1407 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: (Templar Master) Emblum
// File			: MaEw_Emblum.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_emblum_0()
{
	Speak( NpcId(), 1408 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_emblum_1()
{
	LaunchQuest();
}

void CNpcScript::maew_emblum_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_emblum_3()
{
	Say( 1409 );
}

void CNpcScript::maew_emblum_4()
{
	Say( 1410 );
}

void CNpcScript::maew_emblum_5()
{	
}

void CNpcScript::maew_emblum_6()
{	
}

void CNpcScript::maew_emblum_7()
{	
}

void CNpcScript::maew_emblum_8()
{	
	if( GetQuestState(198) == QS_END && GetPlayerJob() == 24 && GetPlayerLvl() == 130 )
	{
		ChangeJob(32);
	}

	else
	{
		Exit();
	}
}

void CNpcScript::maew_emblum_9()
{
	Say( 1411 );	
}

void CNpcScript::maew_emblum_10()
{
	Speak( NpcId(), 1412 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: MaEw_Rukas.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_rukas_0()
{
	Speak( NpcId(), 1413 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_rukas_1()
{
	LaunchQuest();
}

void CNpcScript::maew_rukas_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_rukas_3()
{
	Say( 1414 );
}

void CNpcScript::maew_rukas_4()
{
	Say( 1415 );
}

void CNpcScript::maew_rukas_5()
{	
}

void CNpcScript::maew_rukas_6()
{	
}

void CNpcScript::maew_rukas_7()
{	
}

void CNpcScript::maew_rukas_8()
{	
}

void CNpcScript::maew_rukas_9()
{
	Say( 1416 );	
}

void CNpcScript::maew_rukas_10()
{
	Speak( NpcId(), 1417 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: MaEw_Pinanoco.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_pinanoco_0()
{
	Speak( NpcId(), 1418 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_pinanoco_1()
{
	LaunchQuest();
}

void CNpcScript::maew_pinanoco_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_pinanoco_3()
{
	Say( 1419 );
}

void CNpcScript::maew_pinanoco_4()
{
	Say( 1420 );
}

void CNpcScript::maew_pinanoco_5()
{	
}

void CNpcScript::maew_pinanoco_6()
{	
}

void CNpcScript::maew_pinanoco_7()
{	
}

void CNpcScript::maew_pinanoco_8()
{	
}

void CNpcScript::maew_pinanoco_9()
{
	Say( 1421 );	
}

void CNpcScript::maew_pinanoco_10()
{
	Speak( NpcId(), 1422 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: MaEw_Raundas.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_raundas_0()
{
	Speak( NpcId(), 1423 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_raundas_1()
{
	LaunchQuest();
}

void CNpcScript::maew_raundas_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_raundas_3()
{
	Say( 1424 );
}

void CNpcScript::maew_raundas_4()
{
	Say( 1425 );
}

void CNpcScript::maew_raundas_5()
{	
}

void CNpcScript::maew_raundas_6()
{	
}

void CNpcScript::maew_raundas_7()
{	
}

void CNpcScript::maew_raundas_8()
{	
}

void CNpcScript::maew_raundas_9()
{
	Say( 1426 );	
}

void CNpcScript::maew_raundas_10()
{
	Speak( NpcId(), 1427 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: MaEw_Idelruna.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_idelruna_0()
{
	Speak( NpcId(), 1428 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_idelruna_1()
{
	LaunchQuest();
}

void CNpcScript::maew_idelruna_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_idelruna_3()
{
	Say( 1429 );
}

void CNpcScript::maew_idelruna_4()
{
	Say( 1430 );
}

void CNpcScript::maew_idelruna_5()
{	
}

void CNpcScript::maew_idelruna_6()
{	
}

void CNpcScript::maew_idelruna_7()
{	
}

void CNpcScript::maew_idelruna_8()
{	
}

void CNpcScript::maew_idelruna_9()
{
	Say( 1431 );	
}

void CNpcScript::maew_idelruna_10()
{
	Speak( NpcId(), 1432 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: MaEw_Gigaka.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::maew_gigaka_0()
{
	Speak( NpcId(), 1433 );	
	SetScriptTimer( 15 );
}

void CNpcScript::maew_gigaka_1()
{
	LaunchQuest();
}

void CNpcScript::maew_gigaka_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::maew_gigaka_3()
{
	Say( 1434 );
}

void CNpcScript::maew_gigaka_4()
{
	Say( 1435 );
}

void CNpcScript::maew_gigaka_5()
{	
}

void CNpcScript::maew_gigaka_6()
{	
}

void CNpcScript::maew_gigaka_7()
{	
}

void CNpcScript::maew_gigaka_8()
{	
}

void CNpcScript::maew_gigaka_9()
{
	Say( 1436 );	
}

void CNpcScript::maew_gigaka_10()
{
	Speak( NpcId(), 1437 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: mafl_zandark.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::mafl_zandark_0()
{
	Speak( NpcId(), 1438 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_zandark_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_zandark_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_zandark_3()
{
	Say( 1439 );
}

void CNpcScript::mafl_zandark_4()
{
	Say( 1440 );
}

void CNpcScript::mafl_zandark_5()
{	
}

void CNpcScript::mafl_zandark_6()
{	
}

void CNpcScript::mafl_zandark_7()
{	
}

void CNpcScript::mafl_zandark_8()
{
	if( GetQuestState(QUEST_VOCMER_TRN2) == QS_END && GetPlayerJob() == 0 && GetPlayerLvl() == 15 )
	{
		ChangeJob( 1 );
	}
	else
	{
		Exit();
	}	
}

void CNpcScript::mafl_zandark_9()
{
	Say( 1441 );	
}

void CNpcScript::mafl_zandark_10()
{
	Speak( NpcId(), 1442 );
	Exit();
}
//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: mafl_asis.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::mafl_asis_0()
{
	Speak( NpcId(), 1443 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_asis_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_asis_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_asis_3()
{
	Say( 1444 );
}

void CNpcScript::mafl_asis_4()
{
	Say( 1445 );
}

void CNpcScript::mafl_asis_5()
{	
}

void CNpcScript::mafl_asis_6()
{	
}

void CNpcScript::mafl_asis_7()
{	
}

void CNpcScript::mafl_asis_8()
{
	if( GetQuestState(QUEST_VOCASS_TRN2) == QS_END && GetPlayerJob() == 0 && GetPlayerLvl() == 15 )
	{
		ChangeJob( 3 );
	}
	else
	{
		Exit();
	}	
}

void CNpcScript::mafl_asis_9()
{
	Say( 1446 );	
}

void CNpcScript::mafl_asis_10()
{
	Speak( NpcId(), 1447 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: mafl_ssunder.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::mafl_ssunder_0()
{
	Speak( NpcId(), 1448 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_ssunder_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_ssunder_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_ssunder_3()
{
	Say( 1449 );
}

void CNpcScript::mafl_ssunder_4()
{
	Say( 1450 );
}

void CNpcScript::mafl_ssunder_5()
{	
}

void CNpcScript::mafl_ssunder_6()
{	
}

void CNpcScript::mafl_ssunder_7()
{	
}

void CNpcScript::mafl_ssunder_8()
{
	if( GetQuestState(QUEST_VOCACR_TRN2) == QS_END && GetPlayerJob() == 0 && GetPlayerLvl() == 15 )
	{
		ChangeJob( 2 );
	}
	else
	{
		Exit();
	}	
}

void CNpcScript::mafl_ssunder_9()
{
	Say( 1451 );	
}

void CNpcScript::mafl_ssunder_10()
{
	Speak( NpcId(), 1452 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: mafl_masics.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::mafl_masics_0()
{
	Speak( NpcId(), 1453 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_masics_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_masics_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_masics_3()
{
	Say( 1454 );
}

void CNpcScript::mafl_masics_4()
{
	Say( 1455 );
}

void CNpcScript::mafl_masics_5()
{	
}

void CNpcScript::mafl_masics_6()
{	
}

void CNpcScript::mafl_masics_7()
{	
}

void CNpcScript::mafl_masics_8()
{
	if( GetQuestState(QUEST_VOCMAG_TRN2) == QS_END && GetPlayerJob() == 0 && GetPlayerLvl() == 15 )
	{
		ChangeJob( 4 );
	}
	else
	{
		Exit();
	}	
}

void CNpcScript::mafl_masics_9()
{
	Say( 1456 );	
}

void CNpcScript::mafl_masics_10()
{
	Speak( NpcId(), 1457 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: mafl_ringda.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::mafl_ringda_0()
{
	Speak( NpcId(), 1458 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_ringda_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_ringda_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_ringda_3()
{
	Say( 1459 );
}

void CNpcScript::mafl_ringda_4()
{
	Say( 1460 );
}

void CNpcScript::mafl_ringda_5()
{	
}

void CNpcScript::mafl_ringda_6()
{	
}

void CNpcScript::mafl_ringda_7()
{	
}

void CNpcScript::mafl_ringda_8()
{	
}

void CNpcScript::mafl_ringda_9()
{
	Say( 1461 );	
}

void CNpcScript::mafl_ringda_10()
{
	Speak( NpcId(), 1462 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: mafl_ria.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::mafl_ria_0()
{
	Speak( NpcId(), 1463 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_ria_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_ria_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_ria_3()
{
	Say( 1464 );
}

void CNpcScript::mafl_ria_4()
{
	Say( 1465 );
}

void CNpcScript::mafl_ria_5()
{	
}

void CNpcScript::mafl_ria_6()
{	
}

void CNpcScript::mafl_ria_7()
{	
}

void CNpcScript::mafl_ria_8()
{	
}

void CNpcScript::mafl_ria_9()
{
	Say( 1466 );	
}

void CNpcScript::mafl_ria_10()
{
	Speak( NpcId(), 1467 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: mafl_ruks.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::mafl_ruks_0()
{
	Speak( NpcId(), 1468 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_ruks_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_ruks_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_ruks_3()
{
	Say( 1469 );
}

void CNpcScript::mafl_ruks_4()
{
	Say( 1470 );
}

void CNpcScript::mafl_ruks_5()
{	
}

void CNpcScript::mafl_ruks_6()
{	
}

void CNpcScript::mafl_ruks_7()
{	
}

void CNpcScript::mafl_ruks_8()
{	
}

void CNpcScript::mafl_ruks_9()
{
	Say( 1471 );	
}

void CNpcScript::mafl_ruks_10()
{
	Speak( NpcId(), 1472 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: mafl_pire.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::mafl_pire_0()
{
	Speak( NpcId(), 1473 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_pire_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_pire_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_pire_3()
{
	Say( 1474 );
}

void CNpcScript::mafl_pire_4()
{
	Say( 1475 );
}

void CNpcScript::mafl_pire_5()
{	
}

void CNpcScript::mafl_pire_6()
{	
}

void CNpcScript::mafl_pire_7()
{	
}

void CNpcScript::mafl_pire_8()
{	
}

void CNpcScript::mafl_pire_9()
{
	Say( 1476 );	
}

void CNpcScript::mafl_pire_10()
{
	Speak( NpcId(), 1477 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: mafl_colonpc.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::mafl_colonpc_0()
{
	Speak( NpcId(), 1478 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_colonpc_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_colonpc_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_colonpc_3()
{
	Say( 1479 );
}

void CNpcScript::mafl_colonpc_4()
{
	Say( 1480 );
}

void CNpcScript::mafl_colonpc_5()
{	
}

void CNpcScript::mafl_colonpc_6()
{	
}

void CNpcScript::mafl_colonpc_7()
{	
}

void CNpcScript::mafl_colonpc_8()
{	
}

void CNpcScript::mafl_colonpc_9()
{
	Say( 1481 );	
}

void CNpcScript::mafl_colonpc_10()
{
	Speak( NpcId(), 1482 );
	Exit();
}

//Script End

//Script Information----------------------------------------------------------------//
// Script Modify Version: 1.0( Create )

// Title		: ???
// File			: mafl_costumenerupa.txt
// Save Type		: ANSI
// Country Code		: KR
//----------------------------------------------------------------------------------//

//Script Begin 

void CNpcScript::mafl_costumenerupa_0()
{
	Speak( NpcId(), 1483 );	
	SetScriptTimer( 15 );
}

void CNpcScript::mafl_costumenerupa_1()
{
	LaunchQuest();
}

void CNpcScript::mafl_costumenerupa_2()
{
	AddKey( 9 );
	AddKey( 10 );		
}

void CNpcScript::mafl_costumenerupa_3()
{
	Say( 1484 );
}

void CNpcScript::mafl_costumenerupa_4()
{
	Say( 1485 );
}

void CNpcScript::mafl_costumenerupa_5()
{	
}

void CNpcScript::mafl_costumenerupa_6()
{	
}

void CNpcScript::mafl_costumenerupa_7()
{	
}

void CNpcScript::mafl_costumenerupa_8()
{	
}

void CNpcScript::mafl_costumenerupa_9()
{
	Say( 1486 );	
}

void CNpcScript::mafl_costumenerupa_10()
{
	Speak( NpcId(), 1487 );
	Exit();
}

BOOL MakeKeyIndex()
{
	int n = sizeof(g_nKeys) / sizeof(g_nKeys[0]);
	for( int i=0; i<n; ++i )
	{
		int nIndex = g_nKeys[i];
		const char* szText = GetDialogText( nIndex );

		if( szText[0] != '\0' )
			g_mapKey.insert( make_pair( szText, nIndex ) );
		else
			return FALSE;
	}
	return TRUE;
}

int GetKeyIndex( LPCTSTR szKey )
{
	map< string, int >::iterator it = g_mapKey.find( szKey );
	if( it != g_mapKey.end() )
		return it->second;
	else
		return -1;
}
