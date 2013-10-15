//*************************************************************
//													        
//  정수만이 가능한 Recursive descent parser		        
//  변수의 사용이 가능하고 함수를 호출할 수 있으며,			 
//  goto 명령 등 Little C에서 제공하지 않는 기능을 제공한다. 
//                            								
//  by Travis nam			                       			
//												               
//*************************************************************

#include "stdafx.h"
#include <comdef.h>
#include "vutil.h"
#include "xutil.h"


#include "lang.h"


inline int iswhite( wchar_t c )
{
	if(c > 0 && c <= 0x20)
		return 1;
	return 0;
}

inline int isdelim( wchar_t c )
{
	if(strchr(" !:;,+-<>'/*%^=()&|\"{}",c) || c == 9 || c == '\r' || c == 0 || c == '\n' ) 
		return 1;
	return 0;
}

//
// 토큰 스트림
//
WORD g_codePage = 0;	// 949

namespace THAI
{
	enum CLASS {	
		CTRL= 0,
			NON= 1,
			CONS= 2,
			LV= 3,
			FV1= 4,
			FV2= 5,
			FV3= 6,
			BV1= 7,
			BV2= 8,
			BD= 9,
			TONE=10,
			AD1=11,
			AD2=12,
			AD3=13,
			AV1=14,
			AV2=15,
			AV3=16,
	};
	
	CLASS	CLASS_TABLE[256] = {
		CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
			CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, CTRL, 
			CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
			CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
			NON, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, 
			CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, 
			CONS, CONS, CONS, CONS,  FV3, CONS,  FV3, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS,  NON, 
			FV1,  AV2,  FV1,  FV1,  AV1,  AV3,  AV2,  AV3,  BV1,  BV2,   BD,  NON,  NON,  NON,  NON,  NON, 
			LV,   LV,   LV,   LV,   LV,  FV2,  NON,  AD2, TONE, TONE, TONE, TONE,  AD1,  AD1,  AD3,  NON, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, CTRL, 
	};
	
	namespace INPUT
	{
		enum TYPE { A, C, S, R, X };
		
		bool COMPOSIBLE[3][5] = {
			{ true, true, true, true, true },
			{ true, true, true,false, true },
			{ true, true,false,false, true },
		};

		TYPE STATE_CHECK[17][17] = {
		//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 
			{ X,A,A,A,A,A,A,R,R,R,R,R,R,R,R,R,R }, // 0 
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R }, // 1 
			{ X,A,A,A,A,S,A,C,C,C,C,C,C,C,C,C,C }, // 2 
			{ X,S,A,S,S,S,S,R,R,R,R,R,R,R,R,R,R }, // 3 
			{ X,S,A,S,A,S,A,R,R,R,R,R,R,R,R,R,R }, // 4 
			{ X,A,A,A,A,S,A,R,R,R,R,R,R,R,R,R,R }, // 5 
			{ X,A,A,A,S,A,S,R,R,R,R,R,R,R,R,R,R }, // 6 
			{ X,A,A,A,A,S,A,R,R,R,C,C,R,R,R,R,R }, // 7 
			{ X,A,A,A,S,S,A,R,R,R,C,R,R,R,R,R,R }, // 8 
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R }, // 9 
			{ X,A,A,A,A,A,A,R,R,R,R,R,R,R,R,R,R }, // 0 
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R }, // 1 
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R }, // 2 
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R }, // 3 
			{ X,A,A,A,S,S,A,R,R,R,C,C,R,R,R,R,R }, // 4 
			{ X,A,A,A,S,S,A,R,R,R,C,R,R,R,R,R,R }, // 5 
			{ X,A,A,A,S,S,A,R,R,R,C,R,C,R,R,R,R }, // 6 
		};
	}
	
	namespace OUTPUT
	{
		enum TYPE { N, C, X };
		//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 
		TYPE STATE_CHECK[17][17] = {
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 0 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 1 
			{ X,N,N,N,N,N,N,C,C,C,C,C,C,C,C,C,C }, // 2 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 3 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 4 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 5 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 6 
			{ X,N,N,N,N,N,N,N,N,N,C,C,N,N,N,N,N }, // 7 
			{ X,N,N,N,N,N,N,N,N,N,C,N,N,N,N,N,N }, // 8 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 9 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 0 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 1 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 2 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 3 
			{ X,N,N,N,N,N,N,N,N,N,C,C,N,N,N,N,N }, // 4 
			{ X,N,N,N,N,N,N,N,N,N,C,N,N,N,N,N,N }, // 5 
			{ X,N,N,N,N,N,N,N,N,N,C,N,C,N,N,N,N }, // 6 
		};
	}
	
}

bool	IsComposibleTh(BYTE prev, BYTE curr, int mode)
{
	using namespace THAI;
	using namespace INPUT;

	if(mode > 2) {
		return false;
	} else {
		return COMPOSIBLE[mode][STATE_CHECK[CLASS_TABLE[prev]][CLASS_TABLE[curr]]];
	}
}

// Unicode of thai : 0x0E00~0x0E7F (Direct Mapping)
// Thai Character -> Unicode    (char+0x0E00)-0xA0 =    char+0x0D60
// Unicode -> Thai Character (wchar_t-0x0E00)+0xA0 = wchar_t-0x0D60

const char* CharNextTh(const char* lpsz)
{
	using namespace THAI;
	using namespace OUTPUT;

	const BYTE* stream = (const BYTE*)(lpsz);

	while(STATE_CHECK
		[ 
			CLASS_TABLE
			[ 
				stream[ 0 ] 
			] 
		]
		[ 
			CLASS_TABLE
			[ 
				stream[ 1 ] 
			] 
		] == C) ++stream;

	return (const char*)(stream+1);
}

const char* CharNextVt(const char* lpsz)
{
	const BYTE* stream = (const BYTE*)(lpsz);

	// 두번째 값이 성조일 경우 체크
	switch(stream[1])
	{
		case 0xcc:
		case 0xec:
		case 0xd2:
		case 0xde:
		case 0xf2:
		{
			++stream;
		}
	    break;
	}

	return (const char*)(stream+1);
}

const char*	CharNextEx(const char* strText, WORD codePage)
{
	if(::GetLanguage() == LANG_THA)
	{
		return CharNextTh(strText);
	}
	else if(::GetLanguage() == LANG_VTN)
	{
		return CharNextVt(strText);
	}
	else
	{
		return CharNextExA(g_codePage, strText, 0);
	}
}

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

int IsTone(wchar_t input)
{
	using namespace VTN;
	//U0300 U0301 U0309 U0303 U0323 일때 true

	for(int i=0; i<5; i++)
	{
		if(input == wVTNCombiningDia[i])
			return i;
	}

	return -1;
}

wchar_t ComposeTone(wchar_t letter, int nTone)
{
	using namespace VTN;

	for(int i=0; i<24; i++)
	{
		if(letter == wVTNToneChar[i][5])
			return wVTNToneChar[i][nTone];
	}

	return NULL;
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

// 조합형 유니코드 -> 완성형 유니코드
void ComposeString(wchar_t* pwSrc, wchar_t* pwDest, int nSize)
{
	using namespace VTN;

	if(pwSrc == NULL || pwDest == NULL || nSize <= 0)
		return;

	for(int i=0; i<nSize; i++)
	{
		*pwDest = *pwSrc;
		pwSrc++;

		if(*pwDest == NULL)
			break;

		int nTone = IsTone(*pwSrc);
		if(nTone > -1)
		{
			*pwDest = ComposeTone(*pwDest, nTone);
			pwSrc++;
		}
		pwDest++;
	}
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

// 러시아어 키릴문자
BOOL IsCyrillic( const char chSrc )
{
	if( ::GetLanguage() != LANG_RUS )
		return FALSE;

	BYTE c = chSrc & 0x000000ff;
	static BOOL bCyrill[256] = 
	{//	0x00	0x01	0x02	0x03	0x04	0x05	0x06	0x07	0x08	0x09	0x0a	0x0b	0x0c	0x0d	0x0e	0x0f
		FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE,	// 0x00
		FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE,	// 0x10
		FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE,	// 0x20
		FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE,	// 0x30
		FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE,	// 0x40
		FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE,	// 0x50
		FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE,	// 0x60
		FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE,	// 0x70
		FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE,	// 0x80
		FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE,	// 0x90
		FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	TRUE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE,	// 0xa0
		FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	TRUE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE, 	FALSE,	// 0xb0
		TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE,	// 0xc0
		TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE,	// 0xd0
		TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE,	// 0xe0
		TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE, 	TRUE	// 0xf0
	};

	return bCyrill[c];
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
	if(::GetLanguage() == LANG_VTN && lpMultiByteStr != NULL && cchMultiByte > 0)
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

int MultiByteToWideCharEx(
    UINT     CodePage,
    DWORD    dwFlags,
    LPCSTR   lpMultiByteStr,
    int      cchMultiByte,
    LPWSTR   lpWideCharStr,
    int      cchWideChar)
{
	int nLength;
	if(::GetLanguage() == LANG_VTN && lpWideCharStr != NULL && cchWideChar > 0)
	{
		int nSize = lstrlen(lpMultiByteStr);
		wchar_t* pwSrc = new wchar_t[nSize+1];
		nLength = MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cchMultiByte, pwSrc, nSize+1);
		ComposeString(pwSrc, lpWideCharStr, nLength);
		SAFE_DELETE_ARRAY(pwSrc);
	}
	else
		nLength = MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cchMultiByte, lpWideCharStr, cchWideChar);

	return nLength;
}

CScanner::CScanner( BOOL bComma )
{
	m_bComma = bComma;
	m_pProg = m_pBuf = NULL;
	m_bMemFlag = 1;
	dwValue	= 0;
	m_lpMark = NULL;
	m_dwDef = 0;
	token = m_mszToken;
	m_bErrorCheck = TRUE;
}

CScanner::CScanner( LPVOID p, BOOL bComma )
{
	m_bComma = bComma;
	m_pProg = m_pBuf = (CHAR*)p;
	m_bMemFlag = 1;
	dwValue	= 0;
	m_dwDef = 0;
	token = m_mszToken;
	m_bErrorCheck = TRUE;
}

CScanner::~CScanner()
{
	Free();
}

void CScanner::Free()
{
	if(!m_bMemFlag && m_pBuf) 
		safe_delete_array( m_pBuf );
	m_pProg = m_pBuf = 0;
}

BOOL CScanner::Read( CFileIO* pFile, BOOL )
{
	m_bMemFlag = 0;
	m_nProgSize = pFile->GetLength();

	int nSize = m_nProgSize + 2;
	char* pProg = new char[ nSize ];
	if( !pProg )
		return 0;

	m_pProg = m_pBuf = pProg;
	pFile->Read( m_pBuf, m_nProgSize );
	// 0xfffe의 2바이트를 빼고 널(0x0000)을 2바이트를 추가해서 결국 m_nProgSize는 변화 없음.
	*(pProg + m_nProgSize ) = '\0';
	*(pProg + m_nProgSize + 1 ) = '\0';

	if( (BYTE)*(pProg + 0 ) == 0xff && (BYTE)*(pProg + 1 ) == 0xfe ) // is unicode ?
	{
		// 텍스트가 유니코드일 경우 멀티바이트로 변경 
		char* lpMultiByte = new char[ nSize ];
		int nResult = WideCharToMultiByteEx( g_codePage, 0, 
			                               (LPCWSTR)(pProg + 2), -1, 
			                               lpMultiByte, nSize, 
										   NULL, NULL );
		if( nResult > 0 )
		{
			lpMultiByte[nResult-1] = 0;
			memcpy( pProg, lpMultiByte, nResult );
		}
		safe_delete_array( lpMultiByte );
	}
	return 1;
}

BOOL CScanner::Load( LPCTSTR lpszFileName, BOOL bMultiByte )
{
	CResFile file; 
	CString fileName = lpszFileName;
	fileName.MakeLower();
	if( !file.Open(lpszFileName, "rb" ) ) 
		return 0;
	m_strFileName = lpszFileName;
	return Read( &file, bMultiByte );
}


BOOL CScanner::Load_FileIO( LPCTSTR lpszFileName, BOOL bMultiByte )
{
	CFileIO file;
	CString fileName = lpszFileName;
	fileName.MakeLower();
	if( !file.Open(lpszFileName, "rb" ) ) 
		return 0;
	m_strFileName = lpszFileName;
	return Read( &file, bMultiByte );
}

void CScanner::SetProg( LPVOID pProg )
{
	m_bMemFlag = 1; m_pProg = m_pBuf = (CHAR*)pProg;
}
int CScanner::GetLineNum( LPVOID lpProg )
{
	int nLine = 0;
	if( lpProg == NULL )
		lpProg = m_pProg;
	
	CHAR* pCur = m_pBuf;
	while( lpProg != pCur )
	{
		if( *pCur == '\r' )
			nLine++;
		pCur++;
	}
	return nLine;
}
void CScanner::GetLastFull() // 현재부터 끝까지 한번에 읽는다.
{
	// 앞쪽 white space를 스킵
	while( iswhite( *m_pProg ) && *m_pProg && *m_pProg != '\r' ) 
		++m_pProg;
	
	char* pCur = token; 
	while( *m_pProg && *m_pProg!='\r' )
	{
		int count = CopyChar( m_pProg, pCur );
		m_pProg += count;
		pCur += count;
	}
	// 뒤쪽 white space를 스킵
	if( pCur != token )
	{
		--pCur;
		while( iswhite( *pCur ) && *pCur ) 
			--pCur;
		++pCur;
	}
	*pCur = '\0';
	Token = token;
}

int CScanner::GetToken( BOOL bComma )
{
	char* pszCur = m_mszToken; 
	*pszCur = '\0';
	tokenType = TEMP; 
	tok = NONE_;
	m_nDoubleOps = 0;

	BOOL bLoop;
	do
	{
		bLoop = FALSE;
		
		while( iswhite( *m_pProg ) && *m_pProg ) // white space를 스킵
			++m_pProg;

		while(*m_pProg == '/') // 주석문 처리
		{
			++m_pProg;
			if(*m_pProg == '/') 
			{ 
				++m_pProg;
				while(*m_pProg != '\r' && *m_pProg != '\0') 
					++m_pProg;
				if(*m_pProg == '\r') 
					m_pProg+=2;
			}
			else if(*m_pProg == '*') 
			{
				++m_pProg;
				do 
				{
					while(*m_pProg != '*' && *m_pProg != '\0') 
						++m_pProg;
					
					if(*m_pProg == '\0') 
					{
						tok = FINISHED;
						tokenType = DELIMITER; 
						return tokenType;
					}
					++m_pProg;
				} 
				while(*m_pProg != '/');
				
				++m_pProg;
				while(iswhite(*m_pProg) && *m_pProg) 
					++m_pProg;
			}
			else 
			{
				--m_pProg;
				break;
			}
			bLoop = TRUE;	// 주석 끝 다시 loop시작 
		}
	}
	while( bLoop ) ;

	// 화일의 끝
	if(*m_pProg=='\0') 
	{
		tok = FINISHED;
		tokenType = DELIMITER; 
		goto EXIT;
	}

	// Comma 형식의 구분자인가?
	if( bComma || m_bComma )
	{
		// 콤마나 데이타의 끝, 또는 엔터 코드가 나올 때까지 읽는다.
		// 콤마 구분일 경우는 그것은 pszCur와 NUMBER만을 구분한다.
		if(*m_pProg=='"') 
		{
			++m_pProg;
			while(*m_pProg != '"' && *m_pProg != '\r' && *m_pProg != '\0') 
				*pszCur++ = *m_pProg++;
			++m_pProg; 
			tokenType = STRING; 
		}
		else
		{
			while(*m_pProg!=',' && *m_pProg != '\r' && *m_pProg != '\0') 
				*pszCur++ = *m_pProg++;
		}
		if( *m_pProg == ',' || *m_pProg == '\r')
			++m_pProg; // skim comma
		// white space를 스킵
		if( pszCur != token )
		{
			--pszCur;
			while( iswhite( *pszCur ) && *pszCur ) 
				--pszCur;
			++pszCur;
			if( isdigit2( token[0] ) )
				tokenType = NUMBER;
			if(token[0]=='0' && token[1]=='x') 
				tokenType = HEX;
		}
		goto EXIT;
	}

	//relation operator?
	switch(*m_pProg) 
	{
		case '=':
			if(*(m_pProg+1)=='=') 
			{
				*pszCur++ = *m_pProg++;
				*pszCur++ = *m_pProg++;
				m_nDoubleOps = EQ;
			}

			if(*token) { tokenType = DELIMITER; goto EXIT; 	}
			break;
		case '!':
			if(*(m_pProg+1)=='=') 
			{
				*pszCur++ = *m_pProg++;
				*pszCur++ = *m_pProg++;
				m_nDoubleOps = NE;
			}
			else 
			{
				*pszCur++ = *m_pProg++;
				m_nDoubleOps = NT;
			}
			
			if(*token) { tokenType = DELIMITER; goto EXIT; }
			break;
		case '<':
			if(*(m_pProg+1)=='=') 
			{
				*pszCur++ = *m_pProg++;
				*pszCur++ = *m_pProg++;
				m_nDoubleOps = LE;
			}
			else 
			{
				*pszCur++ = *m_pProg++;
				m_nDoubleOps = LT;
			}
			
			if(*token) { tokenType = DELIMITER; goto EXIT; }
			break;
		case '>':
			if(*(m_pProg+1)=='=') 
			{
				*pszCur++ = *m_pProg++;
				*pszCur++ = *m_pProg++;
				m_nDoubleOps = GE;
			}
			else 
			{
				*pszCur++ = *m_pProg++;
				m_nDoubleOps = GT;
			}

			if(*token) { tokenType = DELIMITER; goto EXIT; }
			break;
	}
		
	// delimiter
	if(*m_pProg == '&' && *(m_pProg+1) == '&') 
	{
		*pszCur = *(pszCur+1) = '&';
		pszCur+=2; 
		m_pProg+=2; 
		tokenType = DELIMITER; 
		goto EXIT; 
	}
	if(*m_pProg == '|' && *(m_pProg+1) == '|') 
	{
		*pszCur = *(pszCur+1) = '|';
		pszCur+=2; 
		m_pProg+=2; 
		tokenType = DELIMITER; 
		goto EXIT; 
	}
	if( strchr( "+-*^/%=;(),':{}.", *m_pProg ) )  
	{
		*pszCur = *m_pProg; 
		++m_pProg;
		++pszCur;	
		tokenType = DELIMITER; 
		goto EXIT;
	}

	// 스트링 
	if(*m_pProg=='"') 
	{
		++m_pProg;
		while( *m_pProg != '"' && *m_pProg != '\r' && *m_pProg != '\0' && ( pszCur - token ) < MAX_TOKENSTR ) 
		{
			int count = CopyChar( m_pProg, pszCur );
			m_pProg += count;
			pszCur += count;
		}
		++m_pProg; 
		tokenType = STRING; 
		if( *(m_pProg-1) != '"' )
		{
			if( *(m_pProg-1) == '\0' )
				--m_pProg; 
			if( m_bErrorCheck )
			{
				CString string;
				if( ( pszCur - token ) >= MAX_TOKENSTR )
					string.Format( "line(%d) 파일 %s에서 \"%s\" 스트링 길이가 %d바이트를 초과했음.", GetLineNum(), m_strFileName, token, MAX_TOKENSTR );
				else
					string.Format( "line(%d) 파일 %s에서 \"%s\" 스트링이 따옴표로 끝나지 않음.", GetLineNum(), m_strFileName, token );
				Error( string );
			}
		}
		goto EXIT;
	}
	// hex
	if( *m_pProg=='0' && *(m_pProg + 1)=='x' ) 
	{
		m_pProg+=2;
		while(!isdelim(*m_pProg)) 
			*pszCur++ = *m_pProg++;
		tokenType = HEX; 
		goto EXIT;
	}
	// 숫자
	if( isdigit2( *m_pProg ) && !IsMultiByte( m_pProg ) ) //숫자 검사만으로 끝나는데 왜 한글이 아닐 경우도 체크하지?
	{
		while( !isdelim( *m_pProg ) )
		{
			int count	= CopyChar( m_pProg, pszCur );
			m_pProg	+= count;
			pszCur	+= count;
		}
//		while( !isdelim( *m_pProg ) ) 
//			*pszCur++ = *m_pProg++;
		tokenType = NUMBER; 
		goto EXIT;
	}

	// 변수와 명령
#ifdef __VS2003
	if( iswalpha( *m_pProg ) || IsMultiByte( m_pProg ) || IsCyrillic( *m_pProg ) 
		|| *m_pProg == '#' || *m_pProg == '_' || *m_pProg == '@' || *m_pProg=='$' || *m_pProg == '?' ) 
#else //__VS2003
	if( isalpha( *m_pProg ) || IsMultiByte( m_pProg ) || IsCyrillic( *m_pProg )
		|| *m_pProg == '#' || *m_pProg == '_' || *m_pProg == '@' || *m_pProg=='$' || *m_pProg == '?' ) 
#endif //__VS2003
	{
		while( !isdelim( *m_pProg ) ) 
		{
			int count = CopyChar( m_pProg, pszCur );
			m_pProg += count;
			pszCur += count;
		}
	}
	else
	{
		/*
		while( !isdelim( *m_pProg ) ) 
		{
			int count	= CopyChar( m_pProg, pszCur );
			m_pProg	+= count;
			pszCur	+= count;
		}
		*/
		*pszCur++ = *m_pProg++;
	}

	tokenType = TEMP;

EXIT:
	*pszCur = '\0';
	Token = token; 
	return tokenType;
}

int CScanner::GetNumber( BOOL bComma )
{
	m_dwDef = 1;
	if( GetToken( bComma ) == HEX )
	{
		Token.MakeLower();
		DWORDLONG dwlNumber = 0;
		DWORD dwMulCnt = 0;
		CHAR cVal;
		for( int i = Token.GetLength() - 1; i >= 0; i--)
		{
			cVal = Token[ i ];
			if( cVal >= 'a' )
				cVal = ( cVal - 'a' ) + 10;
			else cVal -= '0';
			dwlNumber |= (DWORDLONG) cVal << dwMulCnt;
			dwMulCnt += 4;
		}
		m_dwDef = 0;
		return (DWORD)dwlNumber;
	}
	else
	if(!Token.IsEmpty())
	{
		switch(Token[0])
		{
			case '=': 
				m_dwDef = 0;
				return NULL_ID;
			case '-': 
				if( bComma == FALSE )
				{
					GetToken();	
					m_dwDef = 0;
					return -atoi(Token);
				}
				else
				{
					m_dwDef = 0;
					return atoi(Token);
				}
			case '+':	
				if( bComma == FALSE )
					GetToken();	
		}
		m_dwDef = 0;
		return atoi(Token);
	}
	m_dwDef = 0;
	return 0;
}

__int64 CScanner::GetInt64( BOOL bComma )
{
	m_dwDef		= 1;
	if( GetToken( bComma ) == HEX )
	{
	}
	else if( !Token.IsEmpty() )
	{
		switch( Token[0] )
		{
			case '=': 
				m_dwDef = 0;
				return NULL_ID;
			case '-': 
				if( bComma == FALSE )
				{
					GetToken();	
					m_dwDef = 0;
					return -_atoi64( Token );
				}
				else
				{
					m_dwDef = 0;
					return _atoi64( Token );
				}
			case '+':	
				if( bComma == FALSE )
					GetToken();	
		}
		m_dwDef = 0;
		return _atoi64( Token );
	}
	m_dwDef = 0;
	return (__int64)0;
}

FLOAT CScanner::GetFloat( BOOL bComma )
{
	m_dwDef = 1;
	GetToken( bComma );
	if( !Token.IsEmpty() )
	{
		switch( Token[0] )
		{
			case '=': 
				m_dwDef = 0;
				return -1;
			case '-': 
				GetToken(bComma);	
				m_dwDef = 0;
				return (FLOAT)-atof(Token);
			case '+':	
				GetToken(bComma);	
		}
		m_dwDef = 0;
		return (FLOAT)atof(Token);
	}
	m_dwDef = 0;
	return 0.0f;
}
DWORD CScanner::GetHex( BOOL bComma )
{
	m_dwDef = 1;
	if( GetToken( bComma ) == HEX )
	{
		Token.MakeLower();
		DWORDLONG dwlNumber = 0;
		DWORD dwMulCnt = 0;
		CHAR cVal;
		for( int i = Token.GetLength() - 1; i >= 0; i--)
		{
			cVal = Token[ i ];
			if( cVal >= 'a' )
				cVal = ( cVal - 'a' ) + 10;
			else cVal -= '0';
			dwlNumber |= (DWORDLONG) cVal << dwMulCnt;
			dwMulCnt += 4;
		}
		m_dwDef = 0;
		return (DWORD)dwlNumber;
	}
	m_dwDef = 0;
	return 0;
}


void CScanner::PutBack()
{ 
	for( CHAR *t = m_mszToken; *t; t++ ) 
		m_pProg--;
	if( tokenType == STRING ) // 따옴표 처리 
		m_pProg-=2;
}



SERIALNUMBER CScanner::GetSerialNumber( BOOL bComma )
{
	return GetNumber( bComma );
}
