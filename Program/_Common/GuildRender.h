#ifndef __GUILDRENDER_H__
#define __GUILDRENDER_H__

#define MAX_GUILD_TEXTURE	256			// 가능한 총길드수(256은 가능한 최대길드수로 대체하자)

// 64k가 넘지 않도록 하자.
struct GUILD_TEXTURE
{
	DWORD	idGuild;						// 길드 아이디
	LPDIRECT3DTEXTURE9	lpCloakTexture;		// 망토 텍스쳐
	//LPDIRECT3DTEXTURE9	lpIconTexture;	// ICON 텍스쳐
	DWORD	tmLoad;							// 메모리에 로딩한 시간. 시간이 지나면 삭제됨.
};

class CGuildTexture
{
	int m_nMax;
	GUILD_TEXTURE	m_Texture[ MAX_GUILD_TEXTURE ];		// 텍스쳐 메모리 풀.  퍼포먼스 떨어지면 리스트 구조로 바꾸자.
public:
	CGuildTexture();
	~CGuildTexture();
	
	// 길드번호를 받아서 CT+길드번호.JPG파일을 로딩한다. 로딩한 텍스처포인터를 리턴.
	LPDIRECT3DTEXTURE9	LoadGuildTexture( DWORD idGuild );		
};

extern CGuildTexture	g_GuildTexture;


#endif // __GUILDRENDER_H__