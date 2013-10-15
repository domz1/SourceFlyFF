// ImeView.cpp : implementation file
//

#include "stdafx.h"
#include "AppDefine.h"
#include "..\_UnhandledException\ExceptionHandler.h"
#include "clipboard.h"
#include <malloc.h>

///////////////////////////////////////////////////////////////////
// class CIMEMgr
// Remark:
//	IME 의 상태를 관리한다.
//   
///////////////////////////////////////////////////////////////////
CIMEMgr	g_imeMgr;

#define CHT_IMEFILENAME1    "TINTLGNT.IME" // New Phonetic
#define CHT_IMEFILENAME2    "CINTLGNT.IME" // New Chang Jie
#define CHT_IMEFILENAME3    "MSTCIPHA.IME" // Phonetic 5.1
#define CHS_IMEFILENAME1    "PINTLGNT.IME" // MSPY1.5/2/3
#define CHS_IMEFILENAME2    "MSSCIPYA.IME" // MSPY3 for OfficeXP

#define LANG_CHT            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL)
#define LANG_CHS            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)
#define _CHT_HKL            ( (HKL)(INT_PTR)0xE0080404 ) // New Phonetic
#define _CHT_HKL2           ( (HKL)(INT_PTR)0xE0090404 ) // New Chang Jie
#define _CHT_HKL3			( (HKL)(INT_PTR)0xE0010404 ) // Phonetic
#define _CHT_HKL4			( (HKL)(INT_PTR)0x04040404 ) // US

#define _CHS_HKL            ( (HKL)(INT_PTR)0xE00E0804 ) // MSPY
#define MAKEIMEVERSION( major, minor ) \
    ( (DWORD)( ( (BYTE)( major ) << 24 ) | ( (BYTE)( minor ) << 16 ) ) )

#define IMEID_CHT_VER42 ( LANG_CHT | MAKEIMEVERSION( 4, 2 ) )	// New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
#define IMEID_CHT_VER43 ( LANG_CHT | MAKEIMEVERSION( 4, 3 ) )	// New(Phonetic/ChanJie)IME98a : 4.3.x.x // Win2k
#define IMEID_CHT_VER44 ( LANG_CHT | MAKEIMEVERSION( 4, 4 ) )	// New ChanJie IME98b          : 4.4.x.x // WinXP
#define IMEID_CHT_VER50 ( LANG_CHT | MAKEIMEVERSION( 5, 0 ) )	// New(Phonetic/ChanJie)IME5.0 : 5.0.x.x // WinME
#define IMEID_CHT_VER51 ( LANG_CHT | MAKEIMEVERSION( 5, 1 ) )	// New(Phonetic/ChanJie)IME5.1 : 5.1.x.x // IME2002(w/OfficeXP)
#define IMEID_CHT_VER52 ( LANG_CHT | MAKEIMEVERSION( 5, 2 ) )	// New(Phonetic/ChanJie)IME5.2 : 5.2.x.x // IME2002a(w/Whistler)
#define IMEID_CHT_VER60 ( LANG_CHT | MAKEIMEVERSION( 6, 0 ) )	// New(Phonetic/ChanJie)IME6.0 : 6.0.x.x // IME XP(w/WinXP SP1)
#define IMEID_CHS_VER41	( LANG_CHS | MAKEIMEVERSION( 4, 1 ) )	// MSPY1.5	// SCIME97 or MSPY1.5 (w/Win98, Office97)
#define IMEID_CHS_VER42	( LANG_CHS | MAKEIMEVERSION( 4, 2 ) )	// MSPY2	// Win2k/WinME
#define IMEID_CHS_VER53	( LANG_CHS | MAKEIMEVERSION( 5, 3 ) )	// MSPY3	// WinXP

enum { INDICATOR_NON_IME, INDICATOR_CHS, INDICATOR_CHT, INDICATOR_KOREAN, INDICATOR_JAPANESE };
enum { IMEUI_STATE_OFF, IMEUI_STATE_ON, IMEUI_STATE_ENGLISH };

#define LCID_INVARIANT MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)

wchar_t s_aszIndicator[5][3] =  
{
	L"En",
	L"\x7B80",
	L"\x7E41",
	L"\xAC00",
	L"\x3042",
};

int	GetCharsetFromLang( LANGID langid )
{
	switch( PRIMARYLANGID(langid) )
	{
	case LANG_JAPANESE:
		return SHIFTJIS_CHARSET;
	case LANG_KOREAN:
		return HANGEUL_CHARSET;
	case LANG_CHINESE:
		switch( SUBLANGID(langid) )
		{
		case SUBLANG_CHINESE_SIMPLIFIED:
			return GB2312_CHARSET;
		case SUBLANG_CHINESE_TRADITIONAL:
			return CHINESEBIG5_CHARSET;
		default:
			return ANSI_CHARSET;
		}
	case LANG_GREEK:
		return GREEK_CHARSET;
	case LANG_TURKISH:
		return TURKISH_CHARSET;
	case LANG_HEBREW:
		return HEBREW_CHARSET;
	case LANG_ARABIC:
		return ARABIC_CHARSET;
	case LANG_ESTONIAN:
	case LANG_LATVIAN:
	case LANG_LITHUANIAN:
		return BALTIC_CHARSET;
	case LANG_THAI:
		return THAI_CHARSET;
	case LANG_CZECH:
	case LANG_HUNGARIAN:
	case LANG_POLISH:
	case LANG_CROATIAN:
	case LANG_MACEDONIAN:
	case LANG_ROMANIAN:
	case LANG_SLOVAK:
	case LANG_SLOVENIAN:
		return EASTEUROPE_CHARSET;
	case LANG_RUSSIAN:
	case LANG_BELARUSIAN:
	case LANG_BULGARIAN:
	case LANG_UKRAINIAN:
		return RUSSIAN_CHARSET;
	default:
		return ANSI_CHARSET;
	}
}

int	GetCodePageFromCharset( int charset )
{
	switch( charset )
	{
	case SHIFTJIS_CHARSET:
		return 932;
	case HANGUL_CHARSET:
		return 949;
	case GB2312_CHARSET:
		return 936;
	case CHINESEBIG5_CHARSET:
		return 950;
	case GREEK_CHARSET:
		return 1253;
	case TURKISH_CHARSET:
		return 1254;
	case HEBREW_CHARSET:
		return 1255;
	case ARABIC_CHARSET:
		return 1256;
	case BALTIC_CHARSET:
		return 1257;
	case THAI_CHARSET:
		return 874;
	case EASTEUROPE_CHARSET:
		return 1250;
	case RUSSIAN_CHARSET:
		return 1251;
	default:
		return 1252;
	}
}

int	GetCodePageFromLang( LANGID langid )
{
	return GetCodePageFromCharset(GetCharsetFromLang(langid));
}


///////////////////////////////////////////////////////////////////
// class CIMEMgr
CIMEMgr::CIMEMgr()
{
	/* Class Member Variables */ 
	m_dwId[0] = m_dwId[1] = 0;
	m_ImeState = IMEUI_STATE_OFF;

	/* IME.DLL */ 
	m_hDllIme = NULL;

	/* IMM32.DLL */ 
    CHAR szPath[MAX_PATH+1];
    
	if( GetSystemDirectoryA( szPath, MAX_PATH+1 ) ) {

		strcat( szPath, "\\imm32.dll" );
		m_hDllImm32 = LoadLibraryA( szPath );
		if( m_hDllImm32 )
		{
			_ImmLockIMC		= (INPUTCONTEXT*(WINAPI *)( HIMC ))
				GetProcAddress( m_hDllImm32, "ImmLockIMC" );
			_ImmUnlockIMC	= (BOOL(WINAPI *)( HIMC ))
				GetProcAddress( m_hDllImm32, "ImmUnlockIMC" );
			_ImmLockIMCC	= (LPVOID(WINAPI *)( HIMCC ))
				GetProcAddress( m_hDllImm32, "ImmLockIMCC" );
			_ImmUnlockIMCC	= (BOOL(WINAPI *)( HIMCC ))
				GetProcAddress( m_hDllImm32, "ImmUnlockIMCC" );
		}
	}
}
/*----------------------------------------------------------------------------*/ 
CIMEMgr::~CIMEMgr()
{
	if( m_hDllIme ) FreeLibrary( m_hDllIme );

	if(m_hDllImm32) FreeLibrary(m_hDllImm32);
}
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::InputLangChange(HWND hWnd, HKL hkl)
{
	if(m_hkl != hkl) {

		m_hkl = (HKL)hkl;
		m_langId = LOWORD(m_hkl);
		m_codePage = GetCodePageFromLang(m_langId);

		/* Check Property */ 
		m_property = ImmGetProperty(m_hkl, IGP_PROPERTY);

		m_bUnicodeIME = (m_property & IME_PROP_UNICODE) ? true : false;

		/* Update Indicator */ 
		CheckToggleState(hWnd);

		/* Update m_dwId[] */ 
		GetImeId();

		/* Bind Proc */ 
		SetupImeApi(hWnd);

		/* Setup IME */ 
		HIMC hIMC = ImmGetContext(hWnd);
		if(hIMC) {
			ImmSetOpenStatus(hIMC,TRUE);	
			ImmReleaseContext(hWnd, hIMC);
		}

	}
}
/*----------------------------------------------------------------------------*/ 
void CIMEMgr::Composition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC hIMC = ImmGetContext(hWnd);
	if(hIMC) {
		if(lParam&GCS_COMPSTR) {

			int compSize = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, NULL, 0);
			
			m_composition.resize( compSize/sizeof(wchar_t) );

			ImmGetCompositionStringW(hIMC, GCS_COMPSTR, &*m_composition.begin(), compSize);
		}
		if(lParam&GCS_RESULTSTR) {

			int resultSize = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0);

			m_result.resize( resultSize/sizeof(wchar_t) );

			ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, &*m_result.begin(), resultSize);
		}
		if(lParam&GCS_COMPATTR) {

			int attrSize = ImmGetCompositionStringW(hIMC, GCS_COMPATTR, NULL, 0);

			m_attr.resize( attrSize );

			ImmGetCompositionStringW(hIMC, GCS_COMPATTR, &*m_attr.begin(), attrSize);
		}
		if(lParam&GCS_CURSORPOS) {

			m_cursorPos = LOWORD(ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, NULL, 0));

		}
		ImmReleaseContext(hWnd, hIMC);
	}
}
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::Candidate(HWND hWnd)
{
	HIMC hIMC = ImmGetContext(hWnd);

	if(hIMC) {

		int candidateSize = ImmGetCandidateListW(hIMC, 0, NULL, 0);

		if(candidateSize > 0) { 

			m_candidate.resize(candidateSize);

			ImmGetCandidateListW(hIMC, 0, (CANDIDATELIST*)&m_candidate[0], candidateSize);
		}

		ImmReleaseContext(hWnd, hIMC);
	}
}
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::Reading(HWND hWnd)
{
    m_reading.clear();

    if( !m_dwId[0] ) return;

	HIMC hIMC = ImmGetContext(hWnd);
	if( hIMC ) {

		DWORD dwErr = 0;

		if( _GetReadingString ) {

			UINT uMaxUiLen;
			BOOL bVertical;
			// Obtain the reading string size

			m_reading.resize( _GetReadingString( hIMC, 0, NULL, (PINT)&dwErr, &bVertical, &uMaxUiLen ) );

			if( m_reading.size() ) {

				_GetReadingString( hIMC, m_reading.size(), &*m_reading.begin(), (PINT)&dwErr, &bVertical, &uMaxUiLen );
			}

			m_bVerticalReading = bVertical ? true : false;

		} else {

			// IMEs that doesn't implement Reading String API
			wchar_t* temp;
			DWORD tempLen;
			bool bUnicodeIme = false;
			INPUTCONTEXT *lpIC = _ImmLockIMC(hIMC);		

			if(lpIC == NULL) {
				temp = NULL;
				tempLen = 0;
			} else {
				LPBYTE p = 0;
				switch( m_dwId[0] )
				{
					case IMEID_CHT_VER42: // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
					case IMEID_CHT_VER43: // New(Phonetic/ChanJie)IME98a : 4.3.x.x // WinMe, Win2k
					case IMEID_CHT_VER44: // New ChanJie IME98b          : 4.4.x.x // WinXP
						p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 24 );
						if( !p ) break;
						tempLen = *(DWORD *)( p + 7 * 4 + 32 * 4 );
						dwErr = *(DWORD *)( p + 8 * 4 + 32 * 4 );
						temp = (wchar_t *)( p + 56 );
						bUnicodeIme = true;
						break;

					case IMEID_CHT_VER50: // 5.0.x.x // WinME
						p = *(LPBYTE *)( (LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 3 * 4 );
						if( !p ) break;
						p = *(LPBYTE *)( (LPBYTE)p + 1*4 + 5*4 + 4*2 );
						if( !p ) break;
						tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16);
						dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 + 1*4);
						temp = (wchar_t *)(p + 1*4 + (16*2+2*4) + 5*4);
						bUnicodeIme = false;
						break;

					case IMEID_CHT_VER51: // 5.1.x.x // IME2002(w/OfficeXP)
					case IMEID_CHT_VER52: // 5.2.x.x // (w/whistler)
					case IMEID_CHS_VER53: // 5.3.x.x // SCIME2k or MSPY3 (w/OfficeXP and Whistler)
						p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 4);
						if( !p ) break;
						p = *(LPBYTE *)((LPBYTE)p + 1*4 + 5*4);
						if( !p ) break;
						tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2);
						dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2 + 1*4);
						temp  = (wchar_t *) (p + 1*4 + (16*2+2*4) + 5*4);
						bUnicodeIme = true;
						break;

					// the code tested only with Win 98 SE (MSPY 1.5/ ver 4.1.0.21)
					case IMEID_CHS_VER41:
						{
							int nOffset;
							nOffset = ( m_dwId[1] >= 0x00000002 ) ? 8 : 7;

							p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + nOffset * 4);
							if( !p ) break;
							tempLen = *(DWORD *)(p + 7*4 + 16*2*4);
							dwErr = *(DWORD *)(p + 8*4 + 16*2*4);
							dwErr = min( dwErr, tempLen );
							temp = (wchar_t *)(p + 6*4 + 16*2*1);
							bUnicodeIme = true;
						}
						break;

					case IMEID_CHS_VER42: // 4.2.x.x // SCIME98 or MSPY2 (w/Office2k, Win2k, WinME, etc)
						{
							OSVERSIONINFOA osi;
							osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
							GetVersionExA( &osi );

							int nTcharSize = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? sizeof(wchar_t) : sizeof(char);
							p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC( lpIC->hPrivate ) + 1*4 + 1*4 + 6*4);
							if( !p ) break;
							tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize);
							dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize + 1*4);
							temp  = (wchar_t *) (p + 1*4 + (16*2+2*4) + 5*4);
							bUnicodeIme = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? true : false;
						}
						break;

					default:
						temp = NULL;
						tempLen = 0;
						break;
				}
			}

			

			if(tempLen == 0) {
				m_reading.clear();
			} else {
				if( bUnicodeIme ) {
					m_reading.assign(temp, temp + tempLen);
				} else {
					m_reading.resize( MultiByteToWideCharEx(m_codePage, 0, (char*)temp, tempLen, NULL, 0) );

					MultiByteToWideCharEx(m_codePage, 0, (char*)temp, tempLen, &*m_reading.begin(), m_reading.size()); 
				}
			}

			_ImmUnlockIMCC(lpIC->hPrivate);
			_ImmUnlockIMC(hIMC);

			m_bVerticalReading = !GetReadingWindowOrientation();
		}
		ImmReleaseContext(hWnd, hIMC);
	}

	return;
}
/*----------------------------------------------------------------------------*/ 
bool CIMEMgr::Private(HWND hWnd, LPARAM dwData)
{
	// Trap some messages to hide reading window
	switch( m_dwId[0] )
	{
	case IMEID_CHT_VER42:
	case IMEID_CHT_VER43:
	case IMEID_CHT_VER44:
	case IMEID_CHS_VER41:
	case IMEID_CHS_VER42:
		if((dwData==1)||(dwData==2)) 
			return true;
		else
			return false;

	case IMEID_CHT_VER50:
	case IMEID_CHT_VER51:
	case IMEID_CHT_VER52:
	case IMEID_CHT_VER60:
	case IMEID_CHS_VER53:
		if((dwData==16)||(dwData==17)||(dwData==26)||(dwData==27)||(dwData==28))
			return true;
		else
			return false;

	default:
		return false;
	}
}
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::CompleteComposition(HWND hWnd)
{
	HIMC hIMC = ImmGetContext(hWnd);
	if(hIMC) {
		ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_COMPLETE, 0 );
		ImmReleaseContext(hWnd, hIMC);
	}
}
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::CancelComposition(HWND hWnd)
{
	HIMC hIMC = ImmGetContext(hWnd);
	if(hIMC) {
		ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
		ImmReleaseContext(hWnd, hIMC);
	}
}

/*----------------------------------------------------------------------------*/ 
HIMC	CIMEMgr::AssociateContext(HWND hWnd, HIMC hIMC)
{
	return ImmAssociateContext( hWnd, hIMC );
}
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::SetOpenStatus(HWND hWnd, BOOL openState)
{
	HIMC hIMC = ImmGetContext(hWnd);
	if(hIMC) {
		ImmSetOpenStatus(hIMC,openState);
		ImmReleaseContext(hWnd, hIMC);
	}
}
/*----------------------------------------------------------------------------*/ 
BOOL	CIMEMgr::GetOpenStatus(HWND hWnd)
{
	BOOL result = FALSE;

	HIMC hIMC = ImmGetContext(hWnd);
	if(hIMC) {
		result = ImmGetOpenStatus(hIMC);
		ImmReleaseContext(hWnd, hIMC);
	}

	return result;
}
/*----------------------------------------------------------------------------*/ 
void	CIMEMgr::NotifyIME(HWND hWnd, DWORD dwAction, DWORD dwIndex, DWORD dwValue)
{
	HIMC hIMC = ImmGetContext(hWnd);
	if(hIMC) {
		ImmNotifyIME(hIMC, dwAction, dwIndex, dwValue);
		ImmReleaseContext(hWnd, hIMC);
	}
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::GetResult(char* text, int len)
{
	return ConvertString(m_codePage, &*m_result.begin(), m_result.size(), text, len);
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::GetCompositon(char* text, int len)
{
	int n = m_composition.size();
	if( n > 0 )
		return ConvertString(m_codePage, &*m_composition.begin(), n, text, len);
	else
		return 0;
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::GetAttribute(BYTE* attr, int len)
{
	int attrSize = WideCharToMultiByteEx(m_codePage, 0, &*m_composition.begin(), m_composition.size(), NULL, 0, NULL, NULL);

	if(attr == NULL) {
		return attrSize;
	} else {
		if(len < attrSize) {
			return 0;
		} else {

			for(int i=0; i<(int)( m_composition.size() ); ++i)
			{
				int count = WideCharToMultiByteEx(m_codePage, 0, &m_composition[i], 1, NULL, 0, NULL, NULL);

				for(int j=0; j<count; ++j) *attr++ = m_attr[i];
			}

			return attrSize;
		}			
	}
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::GetCursorPos()
{
	return	ConvertString(m_codePage, &*m_composition.begin(), min(m_cursorPos, (int)( m_composition.size() )), NULL, 0);
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::GetCandidate(DWORD index, char* text, int len)
{
	if(m_candidate.empty()) {
		return 0;
	} else {
		CANDIDATELIST* candidateList = (CANDIDATELIST*)&m_candidate[0];

		if(index >= candidateList->dwCount) {
			return 0;
		} else {

			if(m_bUnicodeIME) {
				
				wchar_t* wText = (wchar_t*)(&m_candidate[0] + candidateList->dwOffset[index]);

				return ConvertString(m_codePage, wText, wcslen(wText), text, len);

			} else {

				char* temp = (char*)(&m_candidate[0] + candidateList->dwOffset[index]);

				if(text == 0) {
					return strlen(temp);
				} else {
					int tempLen = strlen(temp);

					if(len < tempLen) {
						return 0;
					} else {
						memcpy(text, temp, tempLen);
						return tempLen;
					}
				}
			}
		}
	}
}
/*----------------------------------------------------------------------------*/ 
bool	CIMEMgr::GetCandidateList(CANDIDATELIST* candidateList)
{
	if(m_candidate.empty()) {
		return false;
	} else {
		memcpy(candidateList, &m_candidate[0], sizeof(CANDIDATELIST));
		return true;
	}
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::GetReading(char* text, int len)
{
	return ConvertString(m_codePage, &*m_reading.begin(), m_reading.size(), text, len);
}
/*----------------------------------------------------------------------------*/ 
int		CIMEMgr::ConvertString(UINT codePage, const wchar_t* wText, int wLen, char* text, int len)
{
	if(text == 0) {
		return WideCharToMultiByteEx(codePage, 0, wText, wLen, NULL, 0, NULL, NULL);
	} else {
		int tLen = WideCharToMultiByteEx(codePage, 0, wText, wLen, NULL, 0, NULL, NULL);

		if(tLen > len) {
			return 0;
		} else {
			return WideCharToMultiByteEx(codePage, 0, wText, wLen, text, tLen, NULL, NULL);
		}
	}
}
/*----------------------------------------------------------------------------*/ 
void CIMEMgr::CheckToggleState(HWND hWnd)
{
	/* Update Indicator */ 
    switch (PRIMARYLANGID(m_langId)) 
	{
    case LANG_KOREAN:
        m_bVerticalCandidate = false;
        m_wszCurrIndicator = s_aszIndicator[INDICATOR_KOREAN];
        break;

    case LANG_JAPANESE:
        m_bVerticalCandidate = true;
        m_wszCurrIndicator = s_aszIndicator[INDICATOR_JAPANESE];
        break;

    case LANG_CHINESE:

	    m_bVerticalCandidate = true;
		switch(SUBLANGID(m_langId)) 
		{
        case SUBLANG_CHINESE_SIMPLIFIED:
            m_bVerticalCandidate = m_dwId[0] == 0;
            m_wszCurrIndicator = s_aszIndicator[INDICATOR_CHS];
            break;

        case SUBLANG_CHINESE_TRADITIONAL:
            m_wszCurrIndicator = s_aszIndicator[INDICATOR_CHT];
            break;

        default:
            m_wszCurrIndicator = s_aszIndicator[INDICATOR_NON_IME];
            break;
        }
        break;

    default:
        m_wszCurrIndicator = s_aszIndicator[INDICATOR_NON_IME];
		break;
    }

    if( m_wszCurrIndicator == s_aszIndicator[INDICATOR_NON_IME] )
    {
        char szLang[10];
        GetLocaleInfoA( MAKELCID( m_langId, SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, szLang, sizeof(szLang) );
        m_wszCurrIndicator[0] = szLang[0];
        m_wszCurrIndicator[1] = towlower( szLang[1] );
    }
	
	/* Check Toggle State */ 
	HIMC hIMC = ImmGetContext(hWnd);

    if( hIMC ) {

		bool bIme = ImmIsIME( m_hkl ) != 0;

        if( ( PRIMARYLANGID(m_langId) == LANG_CHINESE ) && bIme ) 
		{
            DWORD dwConvMode, dwSentMode;
			ImmGetConversionStatus(hIMC, &dwConvMode, &dwSentMode);
            m_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;

        } else
		if( ( PRIMARYLANGID(m_langId) == LANG_JAPANESE ) && bIme ) 
		{			
			DWORD dwConvMode, dwSentMode;
			ImmGetConversionStatus(hIMC, &dwConvMode, &dwSentMode);
			m_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;
				
		} else 
		{
            m_ImeState = ( bIme && ImmGetOpenStatus(hIMC) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
        }

		ImmReleaseContext(hWnd, hIMC);
        
	} else {

        m_ImeState = IMEUI_STATE_OFF;
	}
}
/*----------------------------------------------------------------------------*/ 
void CIMEMgr::GetImeId()
{
    char    szTmp[1024];

	m_dwId[0] = m_dwId[1] = 0;

    if(!((m_hkl==_CHT_HKL) || (m_hkl==_CHT_HKL2) || (m_hkl==_CHS_HKL)))
		return;
        
	if ( !ImmGetIMEFileNameA( m_hkl, szTmp, ( sizeof(szTmp) / sizeof(szTmp[0]) ) - 1 ) )
        return;

    if ( !_GetReadingString ) {

        if( ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME2, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME3, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME2, -1 ) != CSTR_EQUAL ) ) {

	        return;
        }
    }

    DWORD   dwVerHandle;
    DWORD   dwVerSize = GetFileVersionInfoSize( szTmp, &dwVerHandle );

    if( dwVerSize ) {

        LPVOID  lpVerBuffer = alloca( dwVerSize );

        if( GetFileVersionInfo( szTmp, dwVerHandle, dwVerSize, lpVerBuffer ) ) {

			LPVOID  lpVerData;
			UINT    cbVerData;

            if( VerQueryValue( lpVerBuffer, "\\", &lpVerData, &cbVerData ) ) {

                DWORD dwVer = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionMS;
                dwVer = ( dwVer & 0x00ff0000 ) << 8 | ( dwVer & 0x000000ff ) << 16;
                if( _GetReadingString
                    ||
                    ( m_langId == LANG_CHT &&
                        ( dwVer == MAKEIMEVERSION(4, 2) || 
                        dwVer == MAKEIMEVERSION(4, 3) || 
                        dwVer == MAKEIMEVERSION(4, 4) || 
                        dwVer == MAKEIMEVERSION(5, 0) ||
                        dwVer == MAKEIMEVERSION(5, 1) ||
                        dwVer == MAKEIMEVERSION(5, 2) ||
                        dwVer == MAKEIMEVERSION(6, 0) ) )
                    ||
                    ( m_langId == LANG_CHS &&
                        ( dwVer == MAKEIMEVERSION(4, 1) ||
                        dwVer == MAKEIMEVERSION(4, 2) ||
                        dwVer == MAKEIMEVERSION(5, 3) ) ) ) {

                    m_dwId[0] = dwVer | m_langId;
                    m_dwId[1] = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionLS;
                }
            }
        }
    }
}
/*----------------------------------------------------------------------------*/ 
void CIMEMgr::SetupImeApi(HWND hWnd)
{
    char szImeFile[MAX_PATH + 1];

    _GetReadingString = NULL;
    _ShowReadingWindow = NULL;

	if( ImmGetIMEFileNameA( m_hkl, szImeFile, sizeof(szImeFile)/sizeof(szImeFile[0]) - 1 ) != 0 ) {

		if( m_hDllIme ) FreeLibrary( m_hDllIme );
	    
		m_hDllIme = LoadLibraryA( szImeFile );

		if ( m_hDllIme ) {
			_GetReadingString = (UINT (WINAPI*)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT))
				( GetProcAddress( m_hDllIme, "GetReadingString" ) );
			_ShowReadingWindow =(BOOL (WINAPI*)(HIMC, BOOL))
				( GetProcAddress( m_hDllIme, "ShowReadingWindow" ) );

			if( _ShowReadingWindow ) {
				HIMC hIMC = ImmGetContext(hWnd);
				if(hIMC) {
					_ShowReadingWindow( hIMC, false );
					ImmReleaseContext(hWnd, hIMC);
				}
			}
		}
	}
}
/*----------------------------------------------------------------------------*/ 
bool CIMEMgr::GetReadingWindowOrientation()
{
    bool bHorizontalReading = ( m_hkl == _CHS_HKL ) || ( m_hkl == _CHT_HKL2 ) || ( m_dwId[0] == 0 );
    if( !bHorizontalReading && ( m_dwId[0] & 0x0000FFFF ) == LANG_CHT )
    {
        char szRegPath[MAX_PATH];
        HKEY hKey;
        DWORD dwVer = m_dwId[0] & 0xFFFF0000;
        strcpy( szRegPath, "software\\microsoft\\windows\\currentversion\\" );
        strcat( szRegPath, ( dwVer >= MAKEIMEVERSION( 5, 1 ) ) ? "MSTCIPH" : "TINTLGNT" );
        LONG lRc = RegOpenKeyExA( HKEY_CURRENT_USER, szRegPath, 0, KEY_READ, &hKey );
        if (lRc == ERROR_SUCCESS)
        {
            DWORD dwSize = sizeof(DWORD), dwMapping, dwType;
            lRc = RegQueryValueExA( hKey, "Keyboard Mapping", NULL, &dwType, (PBYTE)&dwMapping, &dwSize );
            if (lRc == ERROR_SUCCESS)
            {
                if ( ( dwVer <= MAKEIMEVERSION( 5, 0 ) && 
                       ( (BYTE)dwMapping == 0x22 || (BYTE)dwMapping == 0x23 ) )
                     ||
                     ( ( dwVer == MAKEIMEVERSION( 5, 1 ) || dwVer == MAKEIMEVERSION( 5, 2 ) ) &&
                       (BYTE)dwMapping >= 0x22 && (BYTE)dwMapping <= 0x24 )
                   )
                {
                    bHorizontalReading = true;
                }
            }
            RegCloseKey( hKey );
        }
    }

	return bHorizontalReading;
}

bool CIMEMgr::IsNewPhonetic()
{
	if( PRIMARYLANGID(g_imeMgr.m_hkl) == LANG_CHINESE )
	{
		if( m_hkl == _CHT_HKL )
			return TRUE;
	}

	return FALSE;
}

bool CIMEMgr::IsPhonetic()
{
	if( PRIMARYLANGID(g_imeMgr.m_hkl) == LANG_CHINESE )
	{
		if( m_hkl == _CHT_HKL3 )
			return TRUE;
	}
	
	return FALSE;
}

bool CIMEMgr::IsUs()
{
	if( PRIMARYLANGID(g_imeMgr.m_hkl) == LANG_CHINESE )
	{
		if( m_hkl == _CHT_HKL4 )
			return TRUE;
	}
	
	return FALSE;
}



///////////////////////////////////////////////////////////////////
// class CWndCandList
// Remark:
//   일본어나 중국어는 입력 이후 유사어 리스트를 통해 단어를 고른다.
//   CWndCandList는 그 유사어를 선택할 수 있게하는 팝업 윈도다.
//
///////////////////////////////////////////////////////////////////
CWndCandList::CWndCandList() 
{ 
	m_nPageStart = 0;
	m_nScrollPos = 0;
	m_nLineSpace = 0;
	m_ptWindowPos = 0;
} 
CWndCandList::~CWndCandList() 
{ 
} 
void CWndCandList::UpdateCandList( CPoint windowPos )
{ 
	//if( m_ptWindowPos != CPoint( 0, 0 ) && m_ptWindowPos != windowPos )
	//{
		//g_imeMgr.NotifyIME( m_hWnd, NI_CLOSECANDIDATE,0,0);
	//	return;
	//}
	CANDIDATELIST candidateList;

	g_imeMgr.GetCandidateList(&candidateList);

	TRACE( "Candlist ----------------------\n" );
	TRACE( "Count %d \n", candidateList.dwCount );
	TRACE( "Selection %d \n", candidateList.dwSelection );
	TRACE( "PageStart %d \n", candidateList.dwPageStart );
	TRACE( "PageSize %d \n", candidateList.dwPageSize );

	int nPageStart = candidateList.dwSelection - (candidateList.dwSelection%9);

	if( m_nPageStart != nPageStart )
	{
		m_nPageStart = nPageStart; 
		g_imeMgr.NotifyIME( m_hWnd, NI_SETCANDIDATE_PAGESTART, 0, nPageStart );
		// ImmNotifeIME를 하게 되면 다시 이 함수를 부른다. 그렇게 되면 위에 ResetContent를 실행하게 되므로
		// 스크롤 포지션이 다시 리셋된다. 이 부분이 맨 마지막에 실행되므로 여기서 스크롤 포지션을 세팅해야한다.
		m_wndScrollBar.SetScrollPos( nPageStart );
		m_nScrollPos = nPageStart;
		return;
	}
	// 리셋을 하면 스크롤 포지션이 초기화 된다. 
	// 스크롤 바를 눌러서 스크롤 위치가 조절된 상태라면 초기화가 되면
	// 안되기 때문에 현재 스크롤 포지션을 저장해 둔다.
	int nScrollPos = m_wndScrollBar.GetScrollPos();
	ResetContent();

	// get the longest string length
	DWORD dwPreferNumPerPage = max(candidateList.dwPageSize, DEFAULT_CAND_NUM_PER_PAGE);
	
	int max_width = 0;

	for( int i = 0; i < (int)( candidateList.dwCount ); i++ ) 
	{
		char buf[255];
		int len = g_imeMgr.GetCandidate(i, buf+2, sizeof(buf)-3);
		buf[0] = ' ';
		buf[1] = ' ';
		buf[len+2] = 0;

		AddString( buf );

		SIZE size;
		m_pFont->GetTextExtent( buf, &size, g_imeMgr.m_codePage );

		max_width = max(max_width, size.cx);
    }
	m_ptWindowPos = windowPos;
	CRect rect( 0, 0, max_width + 24, dwPreferNumPerPage * ( GetFontHeight() + m_nLineSpace ) + 10  );
	rect.OffsetRect( windowPos.x, windowPos.y - rect.Height() );
	SetWndRect( rect );
	AdjustWndBase();
	RestoreDeviceObjects();
	
	// 9개의 배수로 갯수를 채운다.
	int nDiv2 = 9 - ( candidateList.dwCount % 9 );
	if( nDiv2 < 9 )
	{
		for( int i = 0; i < nDiv2; i++ )
			AddString( "" );
	}
	// 스크롤 포지션을 복원한다.
	m_wndScrollBar.SetScrollPos( nScrollPos );
	SetCurSel( candidateList.dwSelection );
}
void CWndCandList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_ESCAPE )
		Destroy();
}
void CWndCandList::OnDraw( C2DRender* p2DRender ) 
{ 
	p2DRender->SetFont(m_pFont);

	WORD wOldCodePage = m_pFont->m_wCodePage;
	m_pFont->m_wCodePage = g_imeMgr.m_codePage;
	
	CWndListBox::OnDraw( p2DRender );
	int nMaxHeight = m_pFont->GetMaxHeight() + m_nLineSpace;

    for ( int y = 0 ; y < 9; y++ ) 
	{
		char buf[255];
		wsprintf(buf, "%d:", y + 1);
		p2DRender->TextOut( 2, 2 + y * nMaxHeight, buf, 0xffffffff );//, lstrlen( buf ) );//ADEILSON 0xff000000
    }
	if( m_nScrollPos != m_wndScrollBar.GetScrollPos() )
	{
		m_nScrollPos = m_wndScrollBar.GetScrollPos();
		g_imeMgr.NotifyIME( m_hWnd, NI_SETCANDIDATE_PAGESTART, 0, m_nScrollPos );
	}
	m_pFont->m_wCodePage = wOldCodePage;
} 
void CWndCandList::OnInitialUpdate() 
{ 
	CWndListBox::OnInitialUpdate(); 

	int nCount = 0, nNum = 0;

	m_wndScrollBar.SetScrollPage( 9 );

	if( nNum > 20 ) nNum = 20;
	if( nNum < 5 ) nNum = 5;
	SetWndRect( CRect( 0, 0, 150, nNum * ( GetFontHeight() + 3 ) + 8 ) );
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCandList::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	SetTitle( _T( "World" ) );
	CRect rect( 0, 0, 150, 200 );
	CWndBase::Create( WBS_TOPMOST, rect, pWndParent, APP_COMMAND );
	AdjustWndBase();
	RestoreDeviceObjects();
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return TRUE;
} 
void CWndCandList::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	CWndListBox::OnLButtonUp( nFlags, point );
	// 클릭했을 경우 리스트박스의 현재 셀렉트된 것을 Ime에게 선택했다고 알린다.
	g_imeMgr.NotifyIME( m_hWnd, NI_SELECTCANDIDATESTR,0,GetCurSel());
	// 선택 이후 유사어 리스트를 닫으라고 Ime알린다.
	m_pParentWnd->PostMessage( WM_CLOSECANDIDATE );
} 
void CWndCandList::OnKillFocus(CWndBase* pNewWnd)
{
	// 마우스 커서로 다른 창을 클릭하거나 포커스를 변경했을 경우 단어 조합을 완료한다.
	// 완료 조건은 pNewWnd가 NULL이 아니어야한다. (NULL일 때는 창이 파괴될 때 외에는 없다.)
	// 즉, 포커스가 다른 창으로 변경될 때만 조합을 완결하는 것이다.
	// 그리고 현재창의 프레임윈도와 새윈도의 프레임윈도가 다르면 포커스가 이전된 것으로 간주한다.
	if( pNewWnd && pNewWnd != m_pParentWnd->GetParentWnd() )
	{
		// 단어 조합을 완료하라는 매시지를 Ime에게 보낸다.
		// 보통 완료는 유사어 창이 열려 있으면 닫으면서 완료하고 열리 있지 않으면 그냥 완료한다.
		g_imeMgr.CompleteComposition(m_hWnd);
	}
}

///////////////////////////////////////////////////////////////////////////////
// class CWndHCandList
// 
///////////////////////////////////////////////////////////////////////////////
void CWndHCandList::UpdateCandList(CPoint windowPos)
{
	CANDIDATELIST candidateList;

	g_imeMgr.GetCandidateList(&candidateList);

	int nPageStart = candidateList.dwSelection - (candidateList.dwSelection%9);

	DWORD dwPreferNumPerPage = max(candidateList.dwPageSize, DEFAULT_CAND_NUM_PER_PAGE);
	
	m_maxWidth = 0;

	DWORD dwPageEnd = min(candidateList.dwPageStart+candidateList.dwPageSize, candidateList.dwCount);

	m_candidate.clear();

	for( int i = candidateList.dwPageStart; i < (int)( dwPageEnd ); i++ ) 
	{
		int len = 0;
		char buf[255];
		len += wsprintf(buf, "%d:", i-candidateList.dwPageStart+1);
		len += g_imeMgr.GetCandidate(i, buf+len, sizeof(buf)-len);
		buf[len] = 0;

		m_candidate.push_back( string(buf, buf+len) );

		SIZE size;
		
		m_pFont->GetTextExtent( buf, &size, g_imeMgr.m_codePage );

		m_maxWidth = max(m_maxWidth, size.cx + 2);
    }

	CRect rect( 0, 0, m_maxWidth * (dwPageEnd-candidateList.dwPageStart) + 4, 16 );
	rect.OffsetRect( windowPos.x, windowPos.y - rect.Height() );
	SetWndRect( rect );
}
BOOL CWndHCandList::Initialize( CWndBase* pWndParent, DWORD nType )
{
	CRect rect( 0, 0, 100, 100 );
	CWndBase::Create( WBS_TOPMOST | WBS_NOFRAME, rect, pWndParent, APP_COMMAND );
	return TRUE;
}
void CWndHCandList::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	
	m_pTheme->RenderWndTextFrame( p2DRender, &rect );
}
void CWndHCandList::OnDraw( C2DRender* p2DRender )
{
	DWORD dwColor1 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 0,0,0 )/*adeilson 255,255,255*/;//D3DCOLOR_ARGB( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 0,0,0 )/*adeilson 226,198,181 */;//D3DCOLOR_ARGB( 255,  80,  80, 120 );//

	CRect rtWindow = this->GetWindowRect();
	p2DRender->RenderFillRect(rtWindow, dwColor2 );
	p2DRender->RenderFillRect(CRect(rtWindow.left+1, rtWindow.top+1, rtWindow.right-1, rtWindow.bottom-1), 0xFF000000);//adeilson 0XFFFFFFFF

	p2DRender->SetFont(m_pFont);

	WORD wOldCodePage = m_pFont->m_wCodePage;
	m_pFont->m_wCodePage = g_imeMgr.m_codePage;

	for(int i=0; i<(int)( m_candidate.size() ); ++i)
	{
		const char* text = m_candidate[i].c_str();

		p2DRender->TextOut(2+m_maxWidth*i, 2, text, 0xFFFFFFFF );//ADEILSON 0xFF000000
	}

	m_pFont->m_wCodePage = wOldCodePage;
}
///////////////////////////////////////////////////////////////////////////////
// class CReadingList
// 
// CandidateList 와 흡사한 ReadingList 이다.
///////////////////////////////////////////////////////////////////////////////
void CReadingList::UpdateReading(CPoint windowPos)
{
	int len = g_imeMgr.GetReading(NULL, 0);
	
	m_reading.resize(len + 1);

	g_imeMgr.GetReading(&*m_reading.begin(), m_reading.size());

	m_reading[len] = 0;

	CRect rect;
	if(g_imeMgr.IsVerticalReading())
		rect = CRect(0,0,18,66);
	else
	{
		SIZE size;
		m_pFont->GetTextExtent(&*m_reading.begin(), &size, g_imeMgr.m_codePage);
		rect = CRect(0,0,size.cx+4,18);
	}
	rect.OffsetRect( windowPos.x, windowPos.y - rect.Height() );
	SetWndRect( rect );
}
BOOL CReadingList::Initialize( CWndBase* pWndParent, DWORD nType)
{
	CRect rect( 0, 0, 100, 100 );
	CWndBase::Create( WBS_TOPMOST | WBS_NOFRAME, rect, pWndParent, APP_COMMAND );
	return TRUE;
}
void CReadingList::PaintFrame(C2DRender* p2DRender)
{
}
void CReadingList::OnDraw( C2DRender* p2DRender )
{
	DWORD dwColor1 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 0,0,0 );//D3DCOLOR_ARGB( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 0,0,0 );//D3DCOLOR_ARGB( 255,  80,  80, 120 );//

	CRect rtWindow = this->GetWindowRect();
	p2DRender->RenderFillRect(rtWindow, dwColor2 );
	p2DRender->RenderFillRect(CRect(rtWindow.left+1, rtWindow.top+1, rtWindow.right-1, rtWindow.bottom-1), 0xFF000000);//adeilson 0XFFFFFFFF


	LPCTSTR begin = &*m_reading.begin();
	LPCTSTR end = begin + m_reading.size();

	p2DRender->SetFont(m_pFont);

	WORD wOldCodePage = m_pFont->m_wCodePage;
	m_pFont->m_wCodePage = g_imeMgr.m_codePage;
	
	if(g_imeMgr.IsVerticalReading())
	{
		int x = 1;
		int y = 1;

		while( *begin && begin < end )
		{
			char text[8];
			const char* next = CharNextEx( begin, g_imeMgr.m_codePage );
			memcpy(text, begin, next-begin);
			text[next-begin] = 0;

			SIZE size;
			
			m_pFont->GetTextExtent(text, &size, g_imeMgr.m_codePage);

			p2DRender->TextOut(x + (16-size.cx)/2, y + (16-size.cy)/2, text, 0xFFFFFFFF );//ADEILSON 0xff000000

			y += 16;
			begin = next;
		}
	} 
	else 
	{
		p2DRender->TextOut( 2, 1, &*m_reading.begin(), 0xFFFFFFFF );//ADEILSON 0xff000000
	}

	m_pFont->m_wCodePage = wOldCodePage;
}


#define EDIT_COLOR 0xffffffff//ADEILSON 0xff000000


///////////////////////////////////////////////////////////////////////////////
// CWndEdit

HWND		CWndEdit::m_hWnd = NULL;
HIMC		CWndEdit::m_hIMCBackup = NULL;
HCURSOR		CWndEdit::m_hEditCursor = NULL;
CWndEdit*	CWndEdit::m_pActWndEditCtrl = NULL;

CWndEdit::CWndEdit()
{
	m_pFont = NULL;

	m_bEnableModeChange = FALSE; // 한영 전환 가능한가?

	// override
	m_bCaretVisible = TRUE;
	m_byWndType = WTYPE_EDITCTRL;
	
	m_bEnableClipboard = TRUE;//FALSE;
	m_pWndCandList = NULL;
	m_pWndHCandList = NULL;
	m_pWndReadingList = NULL;
	m_bImeNativeMode = FALSE;
	m_bKeepComposition = FALSE;


	m_string.m_bWordAlign = FALSE;
	m_stringBack.m_bWordAlign = FALSE;
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	m_dwMaxStringNumber = 0xffffffff;
#endif // __2ND_PASSWORD_SYSTEM
}

CWndEdit::~CWndEdit()
{
	if(m_pActWndEditCtrl == this)
		m_pActWndEditCtrl = NULL;

	SAFE_DELETE( m_pWndCandList );
	SAFE_DELETE( m_pWndHCandList );
	SAFE_DELETE( m_pWndReadingList );
}
// 창을 생성한다.
BOOL CWndEdit::Create(HWND hwnd,DWORD dwStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID)
{
	if(hwnd) m_hWnd = hwnd;

	g_imeMgr.InputLangChange(m_hWnd, GetKeyboardLayout(0));
	ClearAll();

	SetAlphaNumericMode();

	if( dwStyle & EBS_PASSWORD )
		EnableModeChange( FALSE );
	else
		EnableModeChange( TRUE );

	m_strTexture = DEF_CTRL_TEXT;
	return CWndBase::Create(dwStyle|WBS_CHILD,rect,pParentWnd,nID);
}
// CWndBase overide
void CWndEdit::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( m_pWndCandList == pWndChild )
	{
		SAFE_DELETE( m_pWndCandList );
		SetFocus();
	}
	if( m_pWndHCandList == pWndChild )
	{
		SAFE_DELETE( m_pWndHCandList );
		SetFocus();
	}
	if( m_pWndReadingList == pWndChild )
	{
		SAFE_DELETE( m_pWndReadingList );
		SetFocus();
	}
}
void CWndEdit::OnSetFocus(CWndBase* pOldWnd)
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( IsWndStyle( EBS_READONLY ) == TRUE )
		return;
#endif // __IMPROVE_QUEST_INTERFACE
	// Edit창에 포커스가 올 경우 눌린 이동키에 대한 해제를 하자.
	g_bKeyTable[g_Neuz.Key.chUp] = FALSE;
	g_bKeyTable[g_Neuz.Key.chLeft] = FALSE;
	g_bKeyTable['S'] = FALSE;
	g_bKeyTable['D'] = FALSE;
	g_bKeyTable[VK_SPACE] = FALSE;

	m_pActWndEditCtrl = this;

	if( m_bEnableModeChange )
		EnableModeChange( TRUE );
	else
		EnableModeChange( FALSE ); 

	if( m_bImeNativeMode )
		SetNativeMode();
	else
		SetAlphaNumericMode();

	m_bCaretVisible = TRUE;

	HKL hkl = GetKeyboardLayout(0);

	if(g_imeMgr.m_hkl != hkl) {
		g_imeMgr.InputLangChange(m_hWnd, hkl);
		Empty();
	}
	// 입력 완료할 때 m_stringBack을 제대로 지우면 여기서는 안해도 된다.
	// 여기서하게 되면 입력중인 스트링이 사라질 수도 있다.
	//m_stringBack.Empty();

	CWndText::OnSetFocus( pOldWnd );
}
void CWndEdit::OnKillFocus(CWndBase* pNewWnd)
{
	if( !m_bKeepComposition )
		g_imeMgr.CompleteComposition(m_hWnd);

	if( (m_pWndCandList == NULL || (m_pWndCandList && m_pWndCandList->IsDestroy())) &&
		(m_pWndHCandList == NULL || (m_pWndHCandList && m_pWndHCandList->IsDestroy())) &&
		(m_pWndReadingList == NULL || (m_pWndReadingList && m_pWndReadingList->IsDestroy())) )
	{
		m_bCaretVisible = FALSE;
		m_pActWndEditCtrl = NULL;
	}
	CRect rect = GetClientRect();
	m_string.Init( m_pFont, &rect );
	CWndText::OnKillFocus( pNewWnd );
}
void CWndEdit::SetWndRect(CRect rectWnd, BOOL bOnSize )
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
 	m_rectClient.DeflateRect( 3, 3, 3, 0 );
	if( m_wndScrollBar.IsVisible() )
		m_rectClient.right -= 16; 
	if( bOnSize )
		OnSize( 0, m_rectClient.Width(), m_rectClient.Height() );
}
void CWndEdit::PaintFrame(C2DRender* p2DRender)
{
	return;
	CRect rect = GetWindowRect();
	m_pTheme->RenderWndEditFrame( p2DRender, &rect );
}
BOOL CWndEdit::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( IsWndStyle( EBS_READONLY ) == FALSE )
		m_pApp->SetDeviceCursor( m_hEditCursor );
#else // __IMPROVE_QUEST_INTERFACE
	m_pApp->SetDeviceCursor( m_hEditCursor );
#endif // __IMPROVE_QUEST_INTERFACE
	return TRUE;
}
// 마우스 왼쪽 버튼을 에디트 창에 누르면 조합이 완료된다.
// 이는 조합시 다른 오동작을 방지하기 위해 클릭하면 무조건 조합을 완료하게 한 것이다.
void CWndEdit::OnLButtonDown( UINT nFlags, CPoint point )
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if( IsWndStyle( EBS_READONLY ) == TRUE )
		return;
#endif // __IMPROVE_QUEST_INTERFACE
	// 조합 완료 매지시를 ime에게 보낸다.
	g_imeMgr.CompleteComposition(m_hWnd);
	CWndText::OnLButtonDown( nFlags, point );
}
// CWndText overide
// 캐럿을 그린다. 국가마다 캐럿모양이 다를 수 있다. 한글은 박스 형태의 캐럿모양이다.
void CWndEdit::DrawCaret(C2DRender* p2DRender)
{
	if( m_bCaretVisible )
	{
		CPoint pt1;
		int nFontBottom = m_pFont->GetMaxHeight();// - 1;
		pt1 = CPoint( m_ptCaret.x, m_ptCaret.y - 1 );
		int nPos = IsWndStyle( WBS_VSCROLL ) ? m_wndScrollBar.GetScrollPos() * ( p2DRender->m_pFont->GetMaxHeight() + m_nLineSpace ) : 0;
		pt1.y -= nPos;
		if( m_timerCaret.GetLeftTime() < 500 )
		{
			if( PRIMARYLANGID( g_imeMgr.m_hkl ) == LANG_KOREAN && g_imeMgr.GetCompositon(NULL, 0) ) 
			{

				char comp[64];
				int len = g_imeMgr.GetCompositon(comp, sizeof(comp));
				comp[len] = 0;

				SIZE size;
				m_pFont->GetTextExtent( comp, &size, g_imeMgr.m_codePage );

				p2DRender->RenderFillRect( CRect( pt1.x, pt1.y, pt1.x + size.cx, pt1.y + size.cy ), D3DCOLOR_ARGB( 255, 255, 255, 255 ) );//adeilson D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/ )
				p2DRender->TextOut( m_ptCaret.x, m_ptCaret.y, comp, D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/ );//adeilson D3DCOLOR_ARGB( 255, 255, 255, 255 ) )
			}
			else
			{
				p2DRender->RenderFillRect( CRect( pt1.x, pt1.y, pt1.x + 2, pt1.y + nFontBottom ), D3DCOLOR_ARGB( 255, 255, 255, 255 ) );//adeilson D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/ )
			}
		}
		if( m_timerCaret.IsTimeOut() )
			m_timerCaret.Reset();
	}
}
// public CWndEdit functions
void CWndEdit::Empty()
{
	ClearAll();
	m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
}

void CWndEdit::OnKeyEnd()
{
	if( (int)( m_dwOffset ) < m_string.GetLength())
	{
		BYTE chr, chr2;

		DWORD dwLine = m_string.OffsetToLine( m_dwOffset );
		m_dwOffset = m_string.GetLineOffset( dwLine + 1 );

		if( m_dwOffset >= 1 )
			chr  = m_string.GetAt( m_dwOffset - 1 );
		if( m_dwOffset >= 2)
			chr2 = m_string.GetAt( m_dwOffset - 2 );

		if( (int)( m_dwOffset ) < m_string.GetLength() || chr == '\n' )
		{
			if( IsDBCSLeadByte( chr ) && IsDBCSLeadByte( chr2 ) ) // KS 한글?
			{
				if( m_dwOffset > 1 ) m_dwOffset -= 2;
			}
			else
			{
				if( m_dwOffset > 0 ) m_dwOffset--;
			}
		}
	}
}
LRESULT CWndEdit::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	CWndBase::WindowProc( message, wParam, lParam );
	switch( message )
	{
	case WM_CLOSECANDIDATE: 
		g_imeMgr.NotifyIME( m_hWnd, NI_CLOSECANDIDATE,0,0);
		break;
	}
	return 1;
}

BOOL CWndEdit::IsYouMessage(UINT msg,WPARAM wparam, LPARAM lparam)
{
	if( !m_hWnd )//|| !m_bAct ) 
		return FALSE;
	if( m_pWndCandList == NULL && m_pWndHCandList == NULL && m_pWndReadingList == NULL )
	{		
		if(GetFocusWnd() != GetFrameWnd())
			return FALSE;
		if(GetParentWnd()->GetFocusChild() != this)
			return FALSE;
	}
#if __VER >= 14 // __WND_EDIT_NUMBER_MODE
	if(IsWndStyle(EBS_NUMBER) == TRUE)
	{
		switch(msg)
		{
		case WM_CHAR:
			{
				switch(wparam)
				{
				case 8:case 27:case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
					{
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
						if( static_cast< DWORD >( strlen( m_string ) ) < m_dwMaxStringNumber )
							OnChar_(wparam);
#endif // __2ND_PASSWORD_SYSTEM
						break;
					}
				}
				break;
			}
		case WM_KEYDOWN:
			{
				switch(wparam)
				{
				case VK_BACK:
					{
						if(!IsEmptyBlock())
						{
							DeleteBlock();
						} 
						else
						{
							m_dwBlockBegin = m_dwOffset;
							m_dwBlockEnd = m_dwOffset;
							if(KeyLeft()) 
							{
								m_dwBlockBegin = m_dwOffset;
								DeleteBlock();
							}
						}
						break;
					}
				case VK_LEFT:
					{
						KeyLeft();
						break;
					}
				case VK_RIGHT:
					{
						KeyRight();
						break;
					}
				case VK_DELETE:
					{
						if(!IsEmptyBlock())
						{
							DeleteBlock();
						} 
						else
						{
							m_dwBlockBegin = m_dwOffset;
							m_dwBlockEnd = m_dwOffset;
							if(KeyRight()) 
							{
								m_dwBlockEnd = m_dwOffset;
								DeleteBlock();
							}						
						}
						break;
					}
				case VK_HOME:
					{
						DWORD dwLine = m_string.OffsetToLine( m_dwOffset );
						m_dwOffset = m_string.GetLineOffset( dwLine );
						break;
					}
				case VK_END:
					{
						OnKeyEnd();
						break;
					}
				case VK_RETURN:
					{
						if( IsWndStyle( EBS_WANTRETURN ) )
						{
							m_string.Insert( m_dwOffset, '\n', EDIT_COLOR, 0, g_imeMgr.m_codePage );
							m_dwOffset++;
							CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
							SetCaretPos( ptCaret );
						}
						CloseCandidate();
						g_imeMgr.CancelComposition( m_hWnd );
						m_pParentWnd->OnChildNotify( EN_RETURN, m_nIdWnd, (LRESULT*)this ); 
						return FALSE;
					}
				case VK_ESCAPE:
					break;
				}
				CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
				SetCaretPos( ptCaret );
				OnKeyDown(wparam, LOWORD(lparam), HIWORD(lparam));
			}
		case WM_KEYUP:
			{
				TRACE( "WM_KEYUP %x\n", wparam );
				if( wparam == 0x0d )
				{
					int a = 0;
					a++;
				}
				OnKeyUp(wparam, LOWORD(lparam), HIWORD(lparam));
				return TRUE;
				break;
			}
		}
		return FALSE;
	}
#endif //__WND_EDIT_NUMBER_MODE
	switch( msg )
	{ 
/*
	case WM_INPUTLANGCHANGE:       // 키보드 레이아웃이 바뀌는 경우 IME를 초기화      
		OnInputLangChange(wparam, lparam);
		return TRUE;
*/
    case WM_INPUTLANGCHANGE:       // 키보드 레이아웃이 바뀌는 경우 IME를 초기화      
		{
			if( PRIMARYLANGID((WORD)lparam) != LANG_ENGLISH) 
				OnInputLangChange(wparam,lparam);
			else
				::DefWindowProc(m_hWnd, WM_INPUTLANGCHANGE, wparam, lparam);
			
			return TRUE;
		}
	case WM_IME_STARTCOMPOSITION:     // 글자 조합의 시작. WM_IME_COMPOSITION 메시지를           // 받을 준비를 한다. 
		OnIMEStartComposition(wparam,lparam);
		return TRUE;    
	case WM_IME_COMPOSITION:          // 현재 조합중인 문자열이나 완성된 문자열을 얻어서 화면 출력 
		OnIMEComposition(wparam,lparam);
		return TRUE;    
	case WM_IME_ENDCOMPOSITION:          // 글자 조합의 끝. 조합된 문자열 처리를 끝낸다          break; 
		HideReading();
		OnIMEEndComposition(wparam,lparam);
		TRACE( "WM_IME_ENDCOMPOSITION\n" );
		return TRUE;    
	case WM_IME_SETCONTEXT:						// 글자 조합 윈도우와 한자 변환 윈도우를 표시하지 않게 바꿈
		OnIMESetContext(wparam,lparam);
		return TRUE;     
	case WM_IME_NOTIFY:        
		OnIMENotify(wparam,lparam);
		return TRUE;             
	case WM_IME_CONTROL:
		OnIMEControl(wparam,lparam);
		return TRUE;    
	case WM_IME_COMPOSITIONFULL:
		OnIMECompositionFull(wparam,lparam);
		return TRUE;    
	case WM_CHAR:
		if( wparam != VK_RETURN )
			OnChar_(wparam);
		break;
	case WM_KEYDOWN:
		{
//			int	i;
			TRACE( "WM_KEYDOWN %x\n", wparam );
			switch( wparam )
			{
				case 3: // Ctrl-C : Copy
					break;

				case 22: // Ctrl-V : Paste
					break;

				case 24: // Ctrl-X : Paste
					break;

				case VK_BACK:
					if(!IsEmptyBlock())
					{
						DeleteBlock();
					} 
					else
					{
						m_dwBlockBegin = m_dwOffset;
						m_dwBlockEnd = m_dwOffset;
						if(KeyLeft()) 
						{
							m_dwBlockBegin = m_dwOffset;
							DeleteBlock();
						}
					}
					break;

				case VK_LEFT:
					KeyLeft();
					break;

				case VK_RIGHT:
					KeyRight();
					break;

				case VK_DELETE:
					if(!IsEmptyBlock())
					{
						DeleteBlock();
					} 
					else
					{
						m_dwBlockBegin = m_dwOffset;
						m_dwBlockEnd = m_dwOffset;
						if(KeyRight()) 
						{
							m_dwBlockEnd = m_dwOffset;
							DeleteBlock();
						}						
					}
					break;

				case VK_DOWN:
					break;
				case VK_UP:
					{
						if( m_pParentWnd->OnChildNotify( EN_UP, m_nIdWnd, (LRESULT*)this ))
						{
							OnKeyEnd();
							CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
							SetCaretPos( ptCaret );
							return FALSE;
						}
					}
					break;

				case VK_HOME:
					{
						DWORD dwLine = m_string.OffsetToLine( m_dwOffset );
						m_dwOffset = m_string.GetLineOffset( dwLine );
					}
					break;

				case VK_END:
					OnKeyEnd();
					break;

				case VK_RETURN:
					if( IsWndStyle( EBS_WANTRETURN ) )
					{
						m_string.Insert( m_dwOffset, '\n', EDIT_COLOR, 0, g_imeMgr.m_codePage );
						m_dwOffset++;
						CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
						SetCaretPos( ptCaret );
					}
					CloseCandidate();
					g_imeMgr.CancelComposition( m_hWnd );
					m_pParentWnd->OnChildNotify( EN_RETURN, m_nIdWnd, (LRESULT*)this ); 
					return FALSE;
				case VK_ESCAPE:
					break;
			}
			CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
			SetCaretPos( ptCaret );
			OnKeyDown(wparam, LOWORD(lparam), HIWORD(lparam));
		}
		case WM_KEYUP:
			TRACE( "WM_KEYUP %x\n", wparam );
			if( wparam == 0x0d )
			{
				int a = 0;
				a++;
			}
			OnKeyUp(wparam, LOWORD(lparam), HIWORD(lparam));
			return TRUE;
			break;
	}
	return FALSE;
}
// 알파벳 모드로 변경한다. 모드 플렉이 있으므로 상태를 알수 있다.
// 모드를 알고 있어야 ime이 리셋후에 다시 모드를 복원할 수 있다.
void CWndEdit::SetAlphaNumericMode()
{
	m_bImeNativeMode = FALSE;
	SetAlphaNumeric();
}
// 국가 모드로 변경한다. 모드 플렉이 있으므로 상태를 알수 있다.
// 모드를 알고 있어야 ime이 리셋후에 다시 모드를 복원할 수 있다.
void CWndEdit::SetNativeMode()
{
	m_bImeNativeMode = TRUE;
	SetNative();
}
// ime 언어 변경 여부를 세팅한다.
// 특정 언어로 유지하게 하는 기능을 한다.
// 영문에서 한글로 변경하지 못하게 한다던가..
void CWndEdit::EnableModeChange( BOOL bEnable )
{
	if( bEnable )
	{
		if( m_hIMCBackup )
		{
			g_imeMgr.AssociateContext(m_hWnd, m_hIMCBackup);
			m_hIMCBackup = NULL;
		}
	}
	else
	{
		if( m_hIMCBackup == NULL )
		{
			m_hIMCBackup = g_imeMgr.AssociateContext(m_hWnd, NULL);
		}
	}
	m_bEnableModeChange = bEnable;
}
///////////////////////////////////////////////////////////////
// public static
// 알파벳 상태로 변경한다. 
void CWndEdit::SetAlphaNumeric()
{
	g_imeMgr.SetOpenStatus(m_hWnd, FALSE);
}

// 현재 기본으로 세팅된 국가 언어 상태로 변경한다.
void CWndEdit::SetNative()
{
	g_imeMgr.SetOpenStatus(m_hWnd, TRUE);
}
///////////////////////////////////////////////////////////////
// private 
void CWndEdit::DeleteBlock( )
{
	if( !IsEmptyBlock() )
	{
		CRect rect = GetClientRect();
		m_string.Init( m_pFont, &rect );
		
		DWORD dwBlockBegin = min(m_dwBlockBegin, m_dwBlockEnd);
		DWORD dwBlockEnd = max(m_dwBlockBegin, m_dwBlockEnd);

		m_string.Delete( dwBlockBegin, dwBlockEnd - dwBlockBegin );

		m_dwOffset = dwBlockBegin;
		m_dwBlockBegin = m_dwBlockEnd = 0;

		m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
	}
}
///////////////////////////////////////////////////////////////
// for IME Control Message
LONG CWndEdit::OnInputLangChange(WPARAM dwCommand, LPARAM dwData)
{
	g_imeMgr.InputLangChange(m_hWnd, (HKL)dwData);
//	Empty();

	g_imeMgr.ClearComposition();
	
	m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 

	return ::DefWindowProc(m_hWnd, WM_INPUTLANGCHANGE, dwCommand, dwData);
}
// WM_IME_STARTCOMPOSITION
LONG CWndEdit::OnIMEStartComposition(WPARAM dwCommand, LPARAM dwData)
{
	m_stringBack = m_string;
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );

	return TRUE;
}
// WM_IME_COMPOSITION
LONG CWndEdit::OnIMEComposition(WPARAM dwCommand, LPARAM dwData)
{
	g_imeMgr.Composition(m_hWnd, dwCommand, dwData);

	if (dwData & GCS_RESULTSTR)
		GetResultString();
	
	if (dwData & GCS_COMPSTR)
		GetCompString(dwData);

	return TRUE;
}
// WM_IME_ENDCOMPOSITION
LONG CWndEdit::OnIMEEndComposition(WPARAM dwCommand, LPARAM dwData)
{
	g_imeMgr.Composition(m_hWnd, dwCommand, GCS_RESULTSTR);

	if( g_imeMgr.GetResultSize() == 0 )
		m_string = m_stringBack;
	g_imeMgr.ClearComposition();

	return TRUE;
}
// WM_IME_SETCONTEXT
LONG CWndEdit::OnIMESetContext(WPARAM dwCommand, LPARAM dwData)
{
	if (g_imeMgr.m_property & IME_PROP_AT_CARET) {
		dwData &= ~(ISC_SHOWUICOMPOSITIONWINDOW | ISC_SHOWUIALLCANDIDATEWINDOW);
	}
	return ::DefWindowProc(m_hWnd, WM_IME_SETCONTEXT, dwCommand, dwData);
}
// WM_IME_COMPOSITIONFULL
LONG CWndEdit::OnIMECompositionFull(WPARAM dwCommand, LPARAM dwData)
{
	return 0;
}
// WM_IME_NOTIFY
LONG CWndEdit::OnIMENotify(WPARAM dwCommand, LPARAM dwData)
{
    switch (dwCommand) {
        case IMN_OPENCANDIDATE:
 			return OpenCandidate();

        case IMN_CLOSECANDIDATE:
			return CloseCandidate();

        case IMN_CHANGECANDIDATE:
			return ChangeCandidate();

		case IMN_SETCONVERSIONMODE:
        case IMN_SETOPENSTATUS:
			{
/*
				if( PRIMARYLANGID((WORD)dwData) != LANG_ENGLISH) 
					OnInputLangChange(dwCommand,dwData);
				else
					::DefWindowProc(m_hWnd, WM_INPUTLANGCHANGE, dwCommand, dwData);
*/				
				g_imeMgr.CheckToggleState(m_hWnd);

				return SetOpenStatus();
			}

		case IMN_PRIVATE:
			g_imeMgr.Reading(m_hWnd);
			if(g_imeMgr.GetReading(NULL, 0) > 0)
				ShowReading();
			else
				HideReading();
			if(g_imeMgr.Private(m_hWnd, dwData)) {
				return TRUE;
			}
			break;

	}
    return ::DefWindowProc(m_hWnd,WM_IME_NOTIFY, dwCommand, dwData);
}
// WM_IME_CONTROL
LONG CWndEdit::OnIMEControl(WPARAM dwCommand, LPARAM dwData)
{
	return 0;
}
// WM_CHAR
void CWndEdit::OnChar_(UINT nChar)
{
// 영문자를 입력할 때, 채팅 창의 가로 크기를 넘기지 못하는 문제 수정 - 2009/11/05
// 	if( !IsWndStyle( EBS_AUTOHSCROLL ) && !IsWndStyle( EBS_AUTOVSCROLL ) )
// 	{
// 		CString strTemp = m_string + (CHAR)nChar; // 길이를 확인하기 위해서 일단 뒤에 str을 추가한다.
// 		if( m_pFont->GetTextExtent( strTemp ).cx >= GetClientRect().Width() )
// 			return;
// 	}
	TRACE( "CWndEdit::OnChar_(UINT nChar = %x)\n",nChar );

	if( ( nChar == 8 ) || ( nChar == 27 )) // backspace
	{
		if(PRIMARYLANGID(g_imeMgr.m_hkl) == LANG_CHINESE && SUBLANGID(g_imeMgr.m_langId) == SUBLANG_CHINESE_TRADITIONAL) //홍콩 Quick IME관련 m_stringBack갱신
			m_stringBack = m_string;
		
		return;
	}
	
	if( nChar == 3 ) // CTRL-C : copy
	{
		if( m_dwBlockBegin != m_dwBlockEnd  && m_bEnableClipboard)
		{
			DWORD dwBlockBegin = min(m_dwBlockBegin, m_dwBlockEnd);
			DWORD dwBlockEnd = max(m_dwBlockBegin, m_dwBlockEnd);

			CString strClipboard = m_string.Mid( dwBlockBegin, dwBlockEnd - dwBlockBegin );

			CClipboard::SetText( strClipboard );
		}
		return;
	}
	if( nChar == 22 && m_bEnableClipboard ) // Ctrl-V : Paste
	{
		DeleteBlock();

		CHAR* buf;
		DWORD dwSize;
		CClipboard::GetTextLength( &dwSize );
		if( dwSize )
		{
			buf = new CHAR[ dwSize + 1 ];
			CClipboard::GetText( buf, dwSize + 1 );
			buf[ dwSize ] = 0; 
			CRect rect = GetClientRect();
			m_string.Init( m_pFont, &rect );//, IsWndStyle( EBS_AUTOHSCROLL ), IsWndStyle( EBS_AUTOVSCROLL ) );
			m_string.Insert( m_dwOffset, buf, EDIT_COLOR, 0, g_imeMgr.m_codePage );
			m_dwOffset += dwSize - 1;
			m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
			safe_delete( buf );
		}
		return;
	}
	if( nChar == 24 ) // Ctrl-X : Paste
	{
	}
	else
	if( nChar != 9 ) // !tab
	{
		DeleteBlock();

		// 콘트롤 문자면 리턴 
		if( iscntrl( nChar ) )
			return;

		if( g_imeMgr.m_codePage == 874 )
		{
			UINT prevChar;

			if(m_dwOffset)
				prevChar = (BYTE)m_string.GetAt(m_dwOffset-1);
			else 
				prevChar = 0;

			if(!IsComposibleTh(prevChar, nChar, 1) )
				return;
		}

		CRect rect = GetClientRect();
		m_string.Init( m_pFont, &rect );//, IsWndStyle( EBS_AUTOHSCROLL ), IsWndStyle( EBS_AUTOVSCROLL ) );

		m_string.Insert( m_dwOffset, nChar, EDIT_COLOR, 0, g_imeMgr.m_codePage );
		m_dwOffset++;

		if(PRIMARYLANGID(g_imeMgr.m_hkl) == LANG_CHINESE && SUBLANGID(g_imeMgr.m_langId) == SUBLANG_CHINESE_TRADITIONAL && isgraph(nChar)) //홍콩 Quick IME관련 m_stringBack갱신
			m_stringBack = m_string;
		
		m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
	}
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
}
// GCS_RESULTSTR
BOOL CWndEdit::GetResultString()
{
    DWORD	len = g_imeMgr.GetResult(NULL, 0);
    LPSTR	str = (LPSTR)_alloca(len + 1);

	g_imeMgr.GetResult(str, len);
	str[len] = 0;

	/* Update m_string */
	m_string = m_stringBack;
	CRect rect = GetClientRect();
	m_string.Init( m_pFont, &rect );
	
	for( int i = 0 ; i < (int)( len ); i++)
	{
		m_string.Insert( i + m_dwOffset, str[ i ], EDIT_COLOR, 0, g_imeMgr.m_codePage );
	}
	m_dwOffset+=len;

	m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
	CPoint ptCaret = OffsetToPoint( m_dwOffset, m_szCaret );
	SetCaretPos( ptCaret );
	m_stringBack = m_string;

	return TRUE;
}
// GCS_COMPSTR
BOOL CWndEdit::GetCompString(LONG flag)
{
    int len = g_imeMgr.GetCompositon(NULL, 0);
    LPSTR str = (LPSTR)_alloca(len);
    BYTE* attr = (BYTE*)_alloca(len);

    g_imeMgr.GetCompositon(str, len);

	if (flag & GCS_COMPATTR) {
		g_imeMgr.GetAttribute(attr, len);
	} else {
		memset(attr, 0, len);
	}

	int curPos = (flag & GCS_CURSORPOS) ? g_imeMgr.GetCursorPos() : 0;

	/* Update m_string */ 
	m_string = m_stringBack;

	CRect rect = GetClientRect();
	m_string.Init( m_pFont, &rect );

	int inputPos = len;

	DWORD dwInputColor = (PRIMARYLANGID(g_imeMgr.m_hkl)==LANG_KOREAN) ? 0xffffffff : 0xff00ffff;//ADEILSON 0xff000000

	for(int i=0; i<len; i++)
	{
		DWORD dwColor = 0xffffffff;//ADEILSON 0xff000000
		DWORD dwStyle = 0;
		switch ( attr[ i ] )
		{
			case ATTR_INPUT:
				dwColor = dwInputColor;
				break;
			case ATTR_TARGET_CONVERTED:
				inputPos = min(inputPos, i);
				dwColor = 0xff00ffff;////ADEILSON 0xff0000ff
				dwStyle = ESSTY_BOLD;
				break;
			case ATTR_TARGET_NOTCONVERTED:
				inputPos = min(inputPos, i);
				dwColor = 0xff00ffff;//ADEILSON 0xff0000ff
				dwStyle = ESSTY_UNDERLINE | ESSTY_BOLD;
				break;
			case ATTR_CONVERTED:
				dwColor = 0xff00ffff;//ADEILSON 0xff0000ff
				break;
		}
		m_string.Insert( m_dwOffset + i, str[ i ], dwColor, dwStyle, g_imeMgr.m_codePage );
	}

	m_ptInputPos = OffsetToPoint( m_dwOffset + inputPos, m_szCaret );
	
	m_pParentWnd->OnChildNotify( EN_CHANGE, m_nIdWnd, (LRESULT*)this ); 
	CPoint ptCaret = OffsetToPoint( m_dwOffset + curPos, m_szCaret );

	SetCaretPos( ptCaret );

	return TRUE;
}
// IMN_OPENCANDIDATE:
BOOL CWndEdit::OpenCandidate()
{
  	if(g_imeMgr.IsVerticalCandidate())
	{
		if(m_pWndCandList == NULL) 
		{
 			m_bKeepComposition = TRUE;
			m_pWndCandList = new CWndCandList;
			m_pWndCandList->Initialize( this );
			m_bKeepComposition = FALSE;
		}
	}
	else
	{
		if(m_pWndHCandList == NULL) 
		{
			m_bKeepComposition = TRUE;
			m_pWndHCandList = new CWndHCandList;
			m_pWndHCandList->Initialize( this );
			m_bKeepComposition = FALSE;
		}
	}

	ChangeCandidate();

	return TRUE;
}
// IMN_CHANGECANDIDATE:
BOOL CWndEdit::ChangeCandidate()
{
	HideReading();

	g_imeMgr.Candidate(m_hWnd);

	CANDIDATELIST candidateList;

   	if(g_imeMgr.GetCandidateList(&candidateList)) {

		CRect rectScreen = GetScreenRect();
		CPoint candidatePos( rectScreen.left + m_ptInputPos.x, rectScreen.top + m_ptInputPos.y );

		if(g_imeMgr.IsVerticalCandidate())
		{
			if( m_pWndCandList )
				m_pWndCandList->UpdateCandList(candidatePos);
		}
		else
		{
			if( m_pWndCandList )
				m_pWndHCandList->UpdateCandList(candidatePos);
		}
	}
	return TRUE;
}

// IMN_CLOSECANDIDATE:
BOOL CWndEdit::CloseCandidate()
{
	//SAFE_DELETE( m_pWndCandList );
	//SAFE_DELETE( m_pWndHCandList );
	if(m_pWndCandList)
		m_pWndCandList->Destroy();

	if(m_pWndHCandList)
		m_pWndHCandList->Destroy();

	g_imeMgr.ClearCandidate();
	
	return TRUE;
}

// IMN_SETOPENSTATUS:
BOOL CWndEdit::SetOpenStatus()
{
	m_bImeNativeMode = g_imeMgr.GetOpenStatus(m_hWnd);

	return TRUE;
}
void CWndEdit::ShowReading()
{
	char reading[16];
	int readingLen = g_imeMgr.GetReading(reading, sizeof(reading));
	reading[readingLen] = 0;

	if( m_pWndReadingList == NULL )
	{
		m_bKeepComposition = TRUE;
		m_pWndReadingList = new CReadingList;
		m_pWndReadingList ->Initialize( this );//GetParentWnd() );
		m_bKeepComposition = FALSE;
	}

	CRect rectScreen = GetScreenRect();
	CPoint candidatePos( rectScreen.left + m_ptInputPos.x, rectScreen.top + m_ptInputPos.y );

	m_pWndReadingList->UpdateReading(candidatePos);
}
void CWndEdit::HideReading()
{
	if( m_pWndReadingList )
		m_pWndReadingList->Destroy();
	g_imeMgr.ClearReading();
}
bool CWndEdit::KeyLeft()
{
	WORD codePage = g_codePage;

	const char* begin = m_string;
	const char* end = begin + m_string.GetLength();
	const char* iter = begin;

	while(*iter && iter < end)
	{
		WORD wCodePage = m_string.m_awCodePage[iter-begin];

		const char* next = CharNextEx(iter, wCodePage);
		if( (next - begin) >= (int)( m_dwOffset ) )
		{
			m_dwOffset = iter - begin;
			return true;
		}
		iter = next;
	}

	m_dwOffset = iter - begin;
	return false;		
}
bool CWndEdit::KeyRight()
{
	const char* begin = m_string;
	const char* end = begin + m_string.GetLength();
	const char* iter = begin;

	while(*iter && iter < end)
	{
		WORD wCodePage = m_string.m_awCodePage[iter-begin];

		iter = CharNextEx(iter, wCodePage);
		if( (iter - begin) > (int)( m_dwOffset ) )
		{
			m_dwOffset = iter - begin;
			return true;
		}
	}

	m_dwOffset = iter - begin;
	return false;		
}
void CWndEdit::ClearAll()
{
	g_imeMgr.ClearComposition();

	m_string.Empty();
	m_stringBack.Empty();
	m_dwBlockBegin = m_dwBlockEnd = m_dwOffset = 0;

	SetCaretPos( CPoint(0,0) );

	m_wndScrollBar.SetMinScrollPos();
}

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
void CWndEdit::SetMaxStringNumber( DWORD dwMaxStringNumber )
{
	m_dwMaxStringNumber = dwMaxStringNumber;
}
#endif // __2ND_PASSWORD_SYSTEM
/*----------------------------------------------------------------------------*/ 
