// ProfileInfo.cpp: implementation of the CProfileInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProfileInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef __NEW_PROFILE
#define PROFILE_TAB_STR		"  |"

CProfileInfo::CProfileInfo()
:m_bToggleProfiling( TRUE )
{
}

CProfileInfo::~CProfileInfo()
{
}

CProfileInfo* CProfileInfo::GetInstance()
{
	static CProfileInfo sProfileInfo;
	return & sProfileInfo;
}


void CProfileInfo::SetToggleProfiling()
{
	m_bToggleProfiling = !m_bToggleProfiling;
	Clear();
	Reset();
}

void CProfileInfo::SetProfileInfo()
{
	CProfileIterator* it = CProfileManager::Get_Iterator();
	
	float fTotal = 0.0f;
	while( !it->Is_Done() )
	{
		fTotal += it->Get_Current_Total_Time();
		it->Next();
	}
	it->First();

	Clear();
	m_vecstrProfileInfo.push_back( "Root" );
	char szTab[64], szTemp[4096];
	while( !it->Is_Done() )
	{
		memset( szTab, 0, sizeof(szTab) );
		for( DWORD i=0; i<it->GetPreOrderDepth(); i++ )
			strcat( szTab, PROFILE_TAB_STR );
		sprintf( szTemp, "%s---> %.2f%%  %s,  <%.3f ms>,  <%d calls>,  <%f ms/call>", szTab, (it->Get_Current_Total_Time() / fTotal) * 100, 
						it->Get_Current_Name(), it->Get_Current_Total_Time() * 1000, it->Get_Current_Total_Calls(),
						(it->Get_Current_Total_Time() * 1000) / (it->Get_Current_Total_Calls() ? it->Get_Current_Total_Calls() : 1));
		m_vecstrProfileInfo.push_back( szTemp );
		it->NextPreOrder();
	}

	CProfileManager::Release_Iterator( it );
	Reset();

	if( fTotal * 1000 > SEC( 1 ) )	// 1초이상 지연되면 로그 남김
		FileOut( "..\\profile.txt" );
}
void CProfileInfo::Clear()
{
	m_vecstrProfileInfo.clear();
	m_vecstrProfileInfo.reserve( 20 );
}

void CProfileInfo::Reset()
{
	CProfileManager::Reset();
}

void CProfileInfo::FileOut( const char* szFile )
{
	FILE* f	= fopen( MakeFileNameDate( szFile ), "a" );
	if( f == NULL )
		return;

	SYSTEMTIME	time;
	GetLocalTime( &time );
	fprintf( f, "%d/%02d/%02d\t%02d:%02d:%02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond );
	
	for( DWORD i=0; i<m_vecstrProfileInfo.size(); i++ )
		fprintf( f, "%s\n", m_vecstrProfileInfo[i].c_str() );
	fprintf( f, "\n\n" );
	fclose( f );
}

#endif // __NEW_PROFILE