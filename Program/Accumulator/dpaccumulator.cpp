#include "stdafx.h"
#include "dpaccumulator.h"
#include "accumulatordlg.h"

const int MAX_ACCU_LANG = 5;
static char* szLang[] = { "KOR","CHI","JAP","TWN", "THA" };     // exception handler와 연계해야 한다.


CDPAccumulator::CDPAccumulator( CAccumulatorDlg* pWnd )
{
	m_pWnd	= pWnd;
//	BEGIN_MSG;
//	ON_MSG( PACKETTYPE_ERROR_TEXT, OnErrorText );
}

CDPAccumulator::~CDPAccumulator()
{

}

void CDPAccumulator::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				break;
			}
	}
}

void CDPAccumulator::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	try
	{
		LPBYTE	lpBuffer	= (LPBYTE)lpMsg;
		u_long	uBufSize	= dwMsgSize;

		CAr ar( lpBuffer, uBufSize );
		GETTYPE( ar );

		DWORD dwLanguage = dw - PACKETTYPE_ERROR_TEXT;
		if( 0 <= dwLanguage && dwLanguage < MAX_ACCU_LANG )
			OnErrorText( ar, dwLanguage );		
	}
	catch( ... )
	{

	}
}

void CDPAccumulator::OnErrorText( CAr & ar, DWORD dwLanguage )
{
	static	char	szErrorText[64*1024];
	static	char	szAccount[MAX_PATH], szPlayer[MAX_PATH];
	*szErrorText	= '\0';
	*szAccount	= '\0';
	*szPlayer	= '\0';
	ar.ReadString( szAccount );
	ar.ReadString( szPlayer );
	ar.ReadString( szErrorText );

	CTime time	= CTime::GetCurrentTime();

	CString strErrorText;
	strErrorText.Format(
		"%s\r\n%s/%s\r\n%s\r\n\r\n", 
		(LPCSTR)time.Format( "%Y/%m/%d %H:%M:%S" ),
		szAccount, szPlayer, szErrorText
	);

//	CString str	= m_pWnd->m_rtError.GetText();
//	str	+= strErrorText;
//	m_pWnd->m_rtError.SetText( str );
//	m_pWnd->m_rtError.SetScrollPos( SB_VERT, m_pWnd->m_rtError.GetScrollLimit( SB_VERT ), TRUE );

	CString strFilename = szLang[dwLanguage];
    strFilename += time.Format( "%Y%m%d.txt" );

	FILE* fp = fopen( strFilename, "a" );
	if( fp )
	{
		fprintf( fp, "%s", strErrorText );
		fclose( fp );
	}
}