#include "stdafx.h"

#include "GuildRender.h"


#ifndef __WORLDSERVER
#ifdef __XCLOAK

//
//
//
CGuildTexture	g_GuildTexture;

//
//
//
CGuildTexture::CGuildTexture()
{
	m_nMax = 0;
	memset( m_Texture, 0, sizeof(m_Texture) );
//	m_pTextures = NULL;		// 시작 위치 지정.
//	m_pFree = NULL;
}

CGuildTexture::~CGuildTexture()
{
	// DeleteDevicesObject();		// 텍스쳐 디바이스 삭제
	m_nMax = 0;
	memset( m_Texture, 0, sizeof(m_Texture) );
//	m_pTextures = NULL;		// 시작 위치 지정.
//	m_pFree = NULL;
}

//
//
//
LPDIRECT3DTEXTURE9 CGuildTexture::LoadGuildTexture( DWORD idGuild )
{
#ifndef _DEBUG
	return NULL;		// 정섭에 올라갈땐 걍 널 리턴.
#endif
	int	i;
	GUILD_TEXTURE *pList = m_Texture, *pNode;
	MATERIAL	*pMtrl;
	D3DMATERIAL9	mMaterial;
	char	szName[128];

	memset( szName, 0, sizeof(szName) );

	for( i = 0; i < MAX_GUILD_TEXTURE; i ++ )
	{
		pNode = pList++;
		if( pNode->idGuild == 0 )	continue;
		if( pNode->idGuild == idGuild )		// 이미 로딩한걸 읽으려 하면 
			return pNode->lpCloakTexture;
	}

	pList = m_Texture;
	for( i = 0; i < MAX_GUILD_TEXTURE; i ++ )
	{
		pNode = pList++;
		if( pNode->idGuild )		continue;
		// idGuild 가 0인것만 비어있는 노드다.
		pNode->idGuild = idGuild;

		// CT + 길드번호 + .jpg 로 텍스쳐 이름 생성/로딩
		sprintf( szName, "CT%06d.jpg", idGuild );
		pMtrl = g_TextureMng.AddMaterial( D3DDEVICE, &mMaterial, szName, DIR_LOGO );

		pNode->lpCloakTexture = pMtrl->m_pTexture;
		pNode->tmLoad = timeGetTime();			// 로딩한 시간 기록.
		return pNode->lpCloakTexture;
	}

	LPCTSTR szErr = Error( "CGuildTexture::LoadGuildTexture : 꽉찬경우 생김" );
	ADDERRORMSG( szErr );

	return NULL;
}






/*
	. 유저가 망토텍스쳐 jpg를 홈피에 등록.
	. 텍스쳐를 CTxxx.jpg 형태로 변환 후 소스세이프에 지정된 폴더에 등록.  - 이파일들은 나중에 merge된다.
	. xxx는 길드아이디와 1:1대응된다 길드아이디의 재사용은 금지된다.
	. "CT" + 길드아이디 + ".jpg"의 형태로 파일이 로딩된다.
	. 정기적으로 DB에서 현재 존재하는 길드의 번호리스트를 텍스트 파일로 받아서 이를 토대로 없어진 길드는 파일도 삭제시켜준다.
	. 길드 망토는 길장이 제작하며 "제작"을 하면 돈이 지불되고 로고정보가 들은 민짜망토가 인벤에 생성.
	. 이 망토는 타인에게 줄수도 있다. 그러나 길드가 삭제되면 망토도 민짜망토로 돌아온다.

*/






#endif
#endif // not WORLDSERVER