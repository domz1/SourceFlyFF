#include "stdafx.h"

#if defined(__REMOVE_SCIRPT_060712)

#include "WorldDialog.h"

extern void		InitDialogFunctions();

CWorldDialog& CWorldDialog::GetInstance()
{
	static CWorldDialog obj;
	return obj;
}

CWorldDialog::CWorldDialog()
{
	m_hDLL = NULL;
	m_pfnRunDialog = NULL;
	m_pfnFindScriptKey = NULL;
	m_pfnInitWorldDialog = NULL;
}

CWorldDialog::~CWorldDialog()
{
	Free();
}

BOOL CWorldDialog::Reload()
{
	Free();
	return Load();
}

BOOL CWorldDialog::Load()
{
	HINSTANCE hModule = LoadLibrary( "WorldDialog.dll" );
	if( hModule )
	{
		m_hDLL = hModule;
		m_pfnRunDialog = (PFRUNDIALOG)GetProcAddress( m_hDLL, "RunDialog" );		
		m_pfnFindScriptKey = (PFFINDSCIRPTKEY)GetProcAddress( m_hDLL, "FindScriptKey" );
		m_pfnInitWorldDialog = (PFINITWORLDDIALOG)GetProcAddress( m_hDLL, "InitWorldDialog" );
		PFSETLANGUAGE pfSetLanguage = (PFSETLANGUAGE)GetProcAddress( m_hDLL, "SetLanguage" );
		if( m_pfnRunDialog && m_pfnFindScriptKey && m_pfnInitWorldDialog && pfSetLanguage )
		{
			pfSetLanguage(::GetLanguage());
			if(m_pfnInitWorldDialog())
				return TRUE;
			else
				return FALSE;
		}
	}
	
	return FALSE;
}

void CWorldDialog::Free()
{
	if( m_hDLL )
	{
		::FreeLibrary( m_hDLL );
		m_hDLL = NULL;
	}
}

BOOL CWorldDialog::Init()
{
	InitDialogFunctions();
	return Load();
}

BOOL CWorldDialog::Find( LPCTSTR szName, LPCTSTR szKey )
{
	if( m_hDLL )
		return m_pfnFindScriptKey( szName, szKey );
	else
		return FALSE;
}

int CWorldDialog::Run( LPCTSTR szName, LPCTSTR szKey, NPCDIALOG_INFO* pInfo )
{
	if( m_hDLL )
		return m_pfnRunDialog( szName, szKey, pInfo );
	else
		return 0;
}

#endif // #if defined(__REMOVE_SCIRPT_060712)
