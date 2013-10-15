
// Implement of CWorld
// date : 2009_09_09
// author : gmpbigsun

#include "stdafx.h"

#ifdef __CLIENT

#if __VER >= 15 // __GUILD_HOUSE
#include "WndGuildHouse.h"
#include "ResData.h"
#endif

// 오래전부터 써오던 게임내 시간단위 Light color values, 이 녀석을 default로 쓴다.
static const LIGHTCOLOR gs_kLight[24] = // diffuse, ambient
{
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 0 12
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 1
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 2
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 3
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 4
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f,  // 5

	0.5f,  0.5f,  0.6f,    0.4f, 0.4f, 0.4f, // 6
	0.7f,  0.7f,  0.7f,    0.5f, 0.5f, 0.5f, // 7
	0.8f,  0.8f,  0.8f,    0.5f, 0.5f, 0.5f, // 8
	0.9f,  0.9f,  0.9f,    0.5f, 0.5f, 0.5f, // 9
	1.0f,  1.0f,  1.0f,    0.5f, 0.5f, 0.5f, // 10
	1.0f,  1.0f,  1.0f,    0.6f, 0.6f, 0.6f, // 11
	1.0f,  1.0f,  1.0f,    0.6f, 0.6f, 0.6f, // 12
	1.0f,  1.0f,  1.0f,    0.6f, 0.6f, 0.6f, // 13 1
	1.0f,  1.0f,  1.0f,    0.6f, 0.6f, 0.6f, // 14 2
	1.0f,  1.0f,  1.0f,    0.5f, 0.5f, 0.5f, // 15 3
	0.9f,  0.9f,  0.9f,    0.5f, 0.5f, 0.5f, // 16 5
	0.9f,  0.6f,  0.2f,    0.5f, 0.5f, 0.4f, // 17 6
	0.6f,  0.6f,  0.4f,    0.4f, 0.4f, 0.4f, // 18 7
	0.5f,  0.5f,  0.4f,    0.4f, 0.4f, 0.4f, // 19 8
	0.45f, 0.45f, 0.4f,    0.35f, 0.35f, 0.35f, // 20 8
	0.43f, 0.43f, 0.5f,    0.33f, 0.33f, 0.3f, // 21 9
	0.41f, 0.41f, 0.5f,    0.31f, 0.31f, 0.3f, // 22 10
	0.4f,  0.4f,  0.5f,    0.3f, 0.3f, 0.4f  // 23 11
};

BOOL CWorld::InitWorldEnvir( )
{
	memcpy( m_k24Light, gs_kLight, sizeof( LIGHTCOLOR ) * 24 );
	return TRUE;
}

#if __VER >= 15 // __GUILD_HOUSE
void CWorld::InProcessing( )
{
	//진입시 한번 호출 
	if( IsWorldGuildHouse() )		//길드 하우스 입장이면 
	{
		//메인메뉴창 띄우고;;등등 
		if( !g_WndMng.m_pWndGHMain )
		{
			g_WndMng.m_pWndGHMain = new CWndGHMainMenu;
			g_WndMng.m_pWndGHMain->Initialize( );
		}

		GuildHouse->ApplyEFTexture( );

	}else
	{
		if( g_WndMng.m_pWndGHMain )
			SAFE_DELETE( g_WndMng.m_pWndGHMain );
	}
}

void CWorld::OutProcessing( )
{
	//퇴장시 한번 호출 
	if( IsWorldGuildHouse() )		
	{
		//메인메뉴창 닫기 등등 
		if( g_WndMng.m_pWndGHMain )
			SAFE_DELETE( g_WndMng.m_pWndGHMain );

		if( GuildDeploy()->IsReady( ) )
			GuildDeploy()->EndDeploy( );

//		GuildDeploy()->Init();
		ForceTexture( NULL );
	
	}

#if __VER >= 15 // __PETVIS
	SAFE_DELETE( g_WndMng.m_pWndBuffPetStatus );
#endif // __PETVIS


	//gmpbigsun(20100419): 개인하우징에서 빠져나오면 물가 텍스쳐가 하우징 바닥이다?
	if( GetID() == WI_WORLD_MINIROOM )
		ForceTexture( NULL );

}

#endif // __GUILD_HOUSE

#endif	//__CLIENT

#if __VER >= 15 // __BS_CHANGING_ENVIR
#include "Continent.h"

enum ChangingTo
{
	NO_CHANGING = 0,
	CHANGING_TO_WORLD = 1,			
	CHANGING_TO_CONTINENT = 2,
};

BOOL LoadTextureSet( TextureSet* pSet )
{
	assert( pSet );
	if( string::npos != pSet->_strFileName.find( '.' ) ) //".xxx"
	{
		pSet->_pTexture = TexturePool::Get()->GetTexture( DIR_WEATHER, pSet->_strFileName );

		if( !pSet->_pTexture )
		{
			CString tmp;
			tmp.Format( "LoadTextureSet: pSet->_pTexture is NULL, filename : %s", pSet->_strFileName );
			Error( tmp.GetBuffer(0) );
			return FALSE;
		}
	}

	return TRUE;
}

void CWorld::InitWorldEnvir( const char* filename )
{
	// Initialize world sky 

	//todo:
	TextureSet* pSet = NULL;
	for( int i = 0; i < 3; ++i )
	{
		//sky
		pSet = &m_kWorldEnvir._kSky[ i ];
		if( pSet->_bUse )
			LoadTextureSet( pSet );

		//cloud
		pSet = &m_kWorldEnvir._kCloud[ i ];
		if( pSet->_bUse )
			LoadTextureSet( pSet );
	}

	pSet = &m_kWorldEnvir._kSun;
	if( pSet->_bUse )
		LoadTextureSet( pSet );

	//moon
	pSet = &m_kWorldEnvir._kMoon;
	if( pSet->_bUse )
		LoadTextureSet( pSet );

	//24 hours light
	if( m_bUsing24Light )
	{
		//ok 24시간 Light정보를 외부에서 가져오겠다!, 만약 읽기에 실패했다면 default값을 ! 
		m_bUsing24Light = ReadFile24Light( filename );
		assert( m_bUsing24Light );
	}

	if( !m_bUsing24Light )
	{
		//hm.. 미리 정의되어 있는 값을 쓰자!
		memcpy( m_k24Light, gs_kLight, sizeof( LIGHTCOLOR ) * 24 );
	}

#ifdef __BS_CHANGEABLE_WORLD_SEACLOUD
	if( m_kSeaCloud._bUse )
		LoadTextureSet( &m_kSeaCloud );
	else m_kSeaCloud._pTexture = NULL;
#endif //__BS_CHANGEABLE_WORLD_SEACLOUD
}

BOOL CWorld::ReadFile24Light( const char* filename )
{
	char fullname[ 256 ];
	sprintf( fullname, "%s%s", filename, ".24l" );

	CScript script;
	if( script.Load( fullname ) == FALSE )
		return FALSE;

	for( int i = 0; i < 24; ++i )
	{
		m_k24Light[ i ].r1 = script.GetFloat( );
		m_k24Light[ i ].g1 = script.GetFloat( );
		m_k24Light[ i ].b1 = script.GetFloat( );
		m_k24Light[ i ].r2 = script.GetFloat( );
		m_k24Light[ i ].g2 = script.GetFloat( );
		m_k24Light[ i ].b2 = script.GetFloat( );
	}

	return TRUE;
}

void CWorld::InitContinent( const char* filename )
{
	// 월드 생성시 초기화 
	m_strCurContName = "";
	m_cContinents.clear( );
	ReadFileContinent( filename );

#ifdef _DEBUG
	UpdateContinentLines( );
#endif
}

void CWorld::InitAfterCreatedPlayer( )
{
	// g_pPlayer가 생성된 직후 딱한번 초기화 
	// 최초 player가 속한 곳에 따라 적절한 값으로 초기화 된다.
	DWORD dwAmbient = m_dwAmbient;
	DWORD dwDiffuse = GetDiffuseColor( );

	// player가 생성되면 world 이거나 continent 인데 continent인경우는 MoveIn이 호출된다 이경우 월드정보를 미리 입력해놔야만 제대로된 변화를 볼수있다.
	m_kCurContinent._fAmbient[ 0 ] = ( (dwAmbient>>16) & 0xff ) / 255.0f;
	m_kCurContinent._fAmbient[ 1 ] = ( (dwAmbient>>8) & 0xff ) / 255.0f;
	m_kCurContinent._fAmbient[ 2 ] = ( (dwAmbient) & 0xff ) / 255.0f;
	
	m_kCurContinent._fDiffuse[ 0 ] = ( (dwDiffuse>>16) & 0xff ) / 255.0f;
	m_kCurContinent._fDiffuse[ 1 ] = ( (dwDiffuse>>8) & 0xff ) / 255.0f;
	m_kCurContinent._fDiffuse[ 2 ] = ( (dwDiffuse) & 0xff ) / 255.0f;

	ENVIR_INFO* pInfo = GetInContinent( g_pPlayer->GetPos() );
	if( !pInfo )
	{
		// 처음 시작지점이 월드 
		m_strCurContName = m_szFileName;
		
	}else
	{
		// 처음 시작지점이 대륙이다, 대륙정보로 환경을 재구성해야 하기때문에 MoveIn....함수를 호출한다.
		MoveInContinent( *pInfo );
	}
}

BOOL CWorld::CheckInOutContinent( )
{
	//called every frame 

	if( m_strCurContName.empty() )		//처음 월드가 생성되고 g_pPlayer가 생성된 시기 
		InitAfterCreatedPlayer( );		// g_pPlayer위치를 기준으로 초기화 

	ENVIR_INFO* pInfo = GetInContinent( g_pPlayer->GetPos( ) );
	if( !pInfo )	// 월드에 있음
	{
		if( string::npos == m_strCurContName.find( ".wld" ) )// 찾을수 없다! 이전에 대륙 
			MoveOutContinent( m_szFileName );	// 대륙 탈출!
	
		return TRUE;
	}

	// 이후는 대륙에 있음이 보장됨 
	if( pInfo->_strName != m_strCurContName )	// 이전과 틀리다면 
	{
	//	if( string::npos != m_strCurContName.find( ".wld" ) ) //  이전에 월드였다.
			MoveInContinent( *pInfo );	// 대륙 입장!
	}

	return TRUE;
}


void CWorld::MoveInContinent( const ENVIR_INFO& kInfo )
{	
	// 월드 -> 대륙진입 
	m_strCurContName = kInfo._strName;		

	//use
	m_kCurContinent._bUseEnvir = kInfo._bUseEnvir;

	//fog
	m_fOldFogStart[0] = m_fFogStartValue;
	m_fOldFogEnd[0] = m_fFogEndValue;

	m_fOldFogStart[1] = kInfo._fFogStart;
	m_fOldFogEnd[1] = kInfo._fFogEnd;

	m_iOldWorldWeather = m_skyBox.GetWeather();

	//weather
	if( kInfo._iWeather != WEATHER_NONE )
	{
#ifdef __BS_ADD_CONTINENT_WEATHER
		if( 5 == kInfo._iWeather )			//기본날씨로!( 대륙이기때문에 바뀌는 날씨에 대해서도 무시한다. )
			m_skyBox.SetWeather( 0, TRUE );
#else
		m_skyBox.SetWeather( kInfo._iWeather, TRUE );
#endif
		m_skyBox.m_bLockWeather = TRUE;
	}

	//skybox
	TextureSet* pSet = const_cast< TextureSet*> ( &kInfo._kSky );
	if( pSet->_bUse && !pSet->_pTexture )
		LoadTextureSet( pSet );

	//cloud
	pSet = const_cast< TextureSet* > ( &kInfo._kCloud );
	if( pSet->_bUse && !pSet->_pTexture )
		LoadTextureSet( pSet );
	
	m_bProcessingEnvir = CHANGING_TO_CONTINENT;
	StartEV( m_kCurContinent  );
}

void CWorld::MoveOutContinent( const string& worldname )
{
	// 대륙 -> 월드 
	ENVIR_INFO* pOldContInfo = GetContinentInfo( m_strCurContName );		//미리 받아놓고.
	if( !pOldContInfo )
	{
		assert( pOldContInfo );
		return;
	}
	
	m_strCurContName = worldname;	
	
	// use
	m_kCurContinent._bUseEnvir = pOldContInfo->_bUseEnvir;

	// fog
	m_fFogStartValue = m_fOldFogStart[ 0 ];
	m_fFogEndValue = m_fOldFogEnd[ 0 ];

	// 시작과 끝을 뒤집는다.
	m_fOldFogStart[ 0 ] = m_fOldFogStart[ 1 ];
	m_fOldFogStart[ 1 ] = m_fFogStartValue;

	m_fOldFogEnd[ 0 ] = m_fOldFogEnd[ 1 ];
	m_fOldFogEnd[ 1 ] = m_fFogEndValue;

	//skybox

	//Weather
	m_skyBox.m_bLockWeather = FALSE;
	m_skyBox.SetWeather( m_iOldWorldWeather, TRUE );

	// 대륙 sky정보 남김 ( sky변화에 이용 )
	m_kCurContinent._kCloud = pOldContInfo->_kCloud;			
	m_kCurContinent._kSky = pOldContInfo->_kSky;

	m_bProcessingEnvir = CHANGING_TO_WORLD;
	
	StartEV( m_kCurContinent );
}

inline float LerpValueToValue( const float oldVal_, const float newVal_, const float dt_ )
{
	return oldVal_ -  (oldVal_ - newVal_) * dt_;			// oldVal * ( 1.0f - dt_ ) + newVal * dt_ 
}

const float fCON_DUR = 2.0f;	//변화할 총 시간.
const float fINV_CON_DUR = 1.0f / fCON_DUR;

void CWorld::HookUpdateLight( CLight* pLight )
{
	// time 0.0 ~ 1.0
	// ambient
	// diffuse
	// fog light

	if( !m_kCurContinent._bUseEnvir )		
		return;

	if( !m_bProcessingEnvir )
		return;

	DWORD dwCurTime = timeGetTime( );
	
	DWORD dwDeltaTime = dwCurTime - m_dwOldTime;
	m_dwOldTime = dwCurTime;
	m_dwAddedTime += dwDeltaTime;
	
	if( m_dwAddedTime > ( 1000 * fCON_DUR ) )
		m_dwAddedTime = (DWORD)( 1000 * fCON_DUR );
	
	float fTime = m_dwAddedTime * 0.001f * ( fINV_CON_DUR );

	// 목표 값은 현재 라이트값!
	//set light here!!

	m_kCurContinent._fAmbient[ 0 ] = pLight->Ambient.r = LerpValueToValue( m_kOldContinent._fAmbient[ 0 ], pLight->Ambient.r, fTime );
	m_kCurContinent._fAmbient[ 1 ] = pLight->Ambient.g = LerpValueToValue( m_kOldContinent._fAmbient[ 1 ], pLight->Ambient.g, fTime );
	m_kCurContinent._fAmbient[ 2 ] = pLight->Ambient.b = LerpValueToValue( m_kOldContinent._fAmbient[ 2 ], pLight->Ambient.b, fTime );

	m_kCurContinent._fDiffuse[ 0 ] = pLight->Diffuse.r = LerpValueToValue( m_kOldContinent._fDiffuse[ 0 ], pLight->Diffuse.r, fTime );
	m_kCurContinent._fDiffuse[ 1 ] = pLight->Diffuse.g = LerpValueToValue( m_kOldContinent._fDiffuse[ 1 ], pLight->Diffuse.g, fTime );
	m_kCurContinent._fDiffuse[ 2 ] = pLight->Diffuse.b = LerpValueToValue( m_kOldContinent._fDiffuse[ 2 ], pLight->Diffuse.b, fTime );

	m_fFogStartValue = LerpValueToValue( m_fOldFogStart[ 0 ], m_fOldFogStart[ 1 ], fTime );
	m_fFogEndValue = LerpValueToValue( m_fOldFogEnd[ 0 ], m_fOldFogEnd[ 1 ], fTime );
	
	if( m_dwAddedTime >= 1000 * fCON_DUR )
	{
		//ok finished the working
		m_bProcessingEnvir = FALSE;
		m_dwAddedTime = 0;
		m_dwOldTime = 0;
	}
}


ENVIR_INFO* CWorld::GetInContinent( const D3DXVECTOR3& test_point )
{
	int counter = 0;
	int i;
	double xinters;
	D3DXVECTOR3 p1,p2;

	for( ENVIR_INFO_Iter iter = m_cContinents.begin(); iter != m_cContinents.end(); ++iter )
	{
		ENVIR_INFO& kInfo = *iter;

		Vec3D_Container& vecMap = kInfo._cVertices;

		p1 =  vecMap[0];
		for (i=1;i<=(int)( vecMap.size() );i++) {
			p2 = vecMap[i % vecMap.size()];
			if (test_point.z > min_cont(p1.z,p2.z)) {
				if (test_point.z <= max_cont(p1.z,p2.z)) {
					if (test_point.x <= max_cont(p1.x,p2.x)) {
						if (p1.z != p2.z) {
							xinters = (test_point.z-p1.z)*(p2.x-p1.x)/(p2.z-p1.z)+p1.x;
							if (p1.x == p2.x || test_point.x <= xinters)
								counter++;
						}
					}
				}
			}
			p1 = p2;
		}
	
		if (counter % 2 == 0)
			continue;
		else
			return &kInfo;
	}

	return NULL;
}

BOOL CWorld::IsInContinent( )
{
	if( !g_pPlayer )
		return FALSE;

	ENVIR_INFO* pInfo = GetInContinent( g_pPlayer->GetPos() );
	return ( pInfo ? TRUE : FALSE );
}

void CWorld::StartEV( const ENVIR_INFO& pOrg )
{
	m_kOldContinent = pOrg;

	m_dwOldTime		= timeGetTime( );
	m_dwAddedTime	= (DWORD)( 0.0f );
}

int	CWorld::GetDiffuseAvg( )
{
	ENVIR_INFO* pInfo = GetInContinent( g_pPlayer->GetPos() );
	if( pInfo )
	{
		return	( (int)( ( pInfo->_fDiffuse[ 0 ] + pInfo->_fDiffuse[ 0 ] + pInfo->_fDiffuse[ 0 ]) * 255 / 3 ) );
	}

	return 0;
}

BOOL CWorld::HookRenderSky( CSkyBox::SKY_TYPE eType, int numRender, float& fAlpha )
{
	//working: 대륙 in out과정이라면 과정을 그려준다.
	//working: 과정이 끝나고 in이라면 대륙 스카이로 그려준다.

	if( !m_kCurContinent._bUseEnvir )
		return FALSE;

	ENVIR_INFO* pInfo = GetInContinent( g_pPlayer->GetPos( ) );
	if( !m_bProcessingEnvir && !pInfo )		// 변화 과정도 아니고 대륙안도 아니면 할일없음.
		return FALSE;

	// 여기서부턴 대륙안이거나 변화과정중임.
	switch( eType )
	{
	case CSkyBox::SKY_SIDE : return HookRenderSky_Side( numRender, pInfo, fAlpha );
	case CSkyBox::SKY_CLOUD : return HookRenderSky_Cloud( numRender, pInfo, fAlpha );
	}

	return TRUE;		// 뭔가 hooking해서 처리했다.
}

BOOL CWorld::HookRenderSky_Side( int numRender, ENVIR_INFO* pInfo, float& fAlpha )
{
	if( m_bProcessingEnvir )
	{
		//변화 과정중 : lerp alpha org to target 
		float fTime = m_dwAddedTime * 0.001f * ( fINV_CON_DUR );
		int grow = (int)( LerpValueToValue( 1, 255, fTime ) );
		int degrow = 255 - grow;

		if( CHANGING_TO_WORLD == m_bProcessingEnvir )
		{
			TextureSet* pSet = &m_kOldContinent._kSky;
			if( pSet->_bUse && pSet->_pTexture )
			{
				m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( degrow, 0, 0, 0 ) );
				m_pd3dDevice->SetTexture( 0, pSet->_pTexture );
				m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, numRender );
			}

			// 월드 스카이 alpha값을 넘겨준다. 
			fAlpha = (float)grow / 255.0f;		//점점 나타나는 
			return FALSE;

		}else 
		if( CHANGING_TO_CONTINENT == m_bProcessingEnvir )
		{
			fAlpha = (float)degrow / 255.0f;	//점점 사라지는 

			TextureSet* pSet = &pInfo->_kSky;
			if( pSet->_bUse && pSet->_pTexture )
			{
				m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( grow, 0, 0, 0 ) );
				m_pd3dDevice->SetTexture( 0, pSet->_pTexture );
				m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, numRender );
			}
			
			return FALSE;
		}

	}
	else //변화가 끝나고 대륙안 
	{
		TextureSet* pSet = &pInfo->_kSky;

		if( !pSet->_bUse )		//사용하지 않을경우 월드로 넘김 
			return FALSE;

		if( pSet->_bUse && !pSet->_pTexture )		//사용하겠다고 해놓고 텍스쳐 없을경우 아무것도 그리지 않음.
			return TRUE;

		m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/ );
		m_pd3dDevice->SetTexture( 0, pSet->_pTexture );
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, numRender );
	}

	return TRUE;
}

BOOL CWorld::HookRenderSky_Cloud( int numRender, ENVIR_INFO* pInfo, float& fAlpha )
{
	if( m_bProcessingEnvir )
	{
		//변화 과정중 : lerp alpha org to target 
		float fTime = m_dwAddedTime * 0.001f * ( fINV_CON_DUR );
		int grow = (int)( LerpValueToValue( 1, 255, fTime ) );
		int degrow = 255 - grow;

		if( CHANGING_TO_WORLD == m_bProcessingEnvir )
		{
			// 대륙 스카이는 점점 사라지고 
			TextureSet* pSet = &m_kOldContinent._kCloud;
			if( pSet->_bUse && pSet->_pTexture )
			{
				m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( degrow, 0, 0, 0 ) );
				m_pd3dDevice->SetTexture( 0, pSet->_pTexture );
				m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, numRender );
			}

			// 월드 스카이는 점점 나타나고
			fAlpha = (float)grow / 255.0f;
			return FALSE;

		}else 
		if( CHANGING_TO_CONTINENT == m_bProcessingEnvir )
		{
			// 대륙 스카이는 점점 나타나고
			TextureSet* pSet = &pInfo->_kCloud;
			if( pSet->_bUse &&  pSet->_pTexture )
			{
				m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( grow, 0, 0, 0 ) );
				m_pd3dDevice->SetTexture( 0, pSet->_pTexture );
				m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, numRender );
			}

			// 월드 스카이는 점점 사라지고
			fAlpha = (float)degrow / 255.0f;	
			return FALSE;
		}

	}
	else //변화가 끝나고 대륙안 
	{
		TextureSet* pSet = &pInfo->_kCloud;

		if( !pSet->_bUse )		// 사용하지 않을경우 그냥 월드껄 그려라~
			return FALSE;

		if( pSet->_bUse && !pSet->_pTexture )		//사용하겠다고 해놓고 텍스쳐 없을경우 아무것도 그리지 않음.
			return TRUE;
	
		m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/ );
		m_pd3dDevice->SetTexture( 0, pSet->_pTexture );
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, numRender );
	}

	return TRUE;
}

ENVIR_INFO* CWorld::GetContinentInfo( const string& name )
{
	for( ENVIR_INFO_Iter iter = m_cContinents.begin(); iter != m_cContinents.end(); ++iter )
	{
		ENVIR_INFO* pInfo = &(*iter);
		if( pInfo->_strName == name )
			return pInfo;
	}

	return NULL;
}


void CWorld::ReadFileContinent( const char* filename )
{
	char fullname[ 256 ];
	sprintf( fullname, "%s%s", filename, ".cnt" );

	CScript script;
	if( script.Load( fullname ) == FALSE )
		return;

	ENVIR_INFO kInfo;

	do{
		script.GetToken();

		if( script.Token == _T( "WORLD" ) )
		{
			script.GetToken( );
			if( script.Token != _T( filename ) )
			{
			}
		}
		else
		if( script.Token == _T( "Continent" ) )
		{
			script.GetToken();
			if( script.Token == _T( "BEGIN" ) )
			{
				//ok
				kInfo._cVertices.clear();
			}
			else if( script.Token == _T( "END" ) )
			{
				if( !kInfo._cVertices.empty() )
					kInfo._cVertices.push_back( kInfo._cVertices[ 0 ] );

				if( kInfo._bUseEnvir )		// 환경을 이용할 경우만 저장 
					m_cContinents.push_back( kInfo );
			}
		}
		else
		if( script.Token == _T( "C_useEnvir" ) )
		{
			kInfo._bUseEnvir = script.GetNumber( );
		}
		else
		if( script.Token == _T( "C_useRealData" ) )
		{
			kInfo._bUseRealData = script.GetNumber( );
		}
		else 
		if( script.Token == _T( "C_name" ) )
		{
			//대륙 이름
			script.GetToken();
			kInfo._strName = script.token;
		}
		else
		if( script.Token == _T( "C_id" ) )
		{
			kInfo._id = script.GetNumber( );
		}
		else
		if( script.Token == _T( "C_ambient" ) )
		{
			kInfo._fAmbient[ 0 ] = script.GetFloat();
			kInfo._fAmbient[ 1 ] = script.GetFloat();
			kInfo._fAmbient[ 2 ] = script.GetFloat();
		}
		else
		if( script.Token == _T( "C_diffuse" ) )
		{
			kInfo._fDiffuse[ 0 ] = script.GetFloat( );
			kInfo._fDiffuse[ 1 ] = script.GetFloat( );
			kInfo._fDiffuse[ 2 ] = script.GetFloat( );
		}
		else
		if( script.Token == _T( "C_fogstartend" ) )
		{
			kInfo._fFogStart = script.GetFloat( );
			kInfo._fFogEnd = script.GetFloat( );
		}
		else
		if( script.Token == _T( "C_weather" ) )
		{
			kInfo._iWeather = script.GetNumber( );
		}
		else 
		if( script.Token == _T( "C_sky" ) )
		{
			kInfo._kSky._bUse = script.GetNumber( );

			script.GetToken( );
			kInfo._kSky._strFileName = script.Token;
		}
		else 
		if( script.Token == _T( "C_cloud" ) )	
		{
			kInfo._kCloud._bUse = script.GetNumber( );

			script.GetToken( );
			kInfo._kCloud._strFileName = script.Token;
		}
		else
		if( script.Token == _T( "VERTEX" ) )
		{
			float x = script.GetFloat();
			float y = script.GetFloat();
			float z = script.GetFloat();
			kInfo._cVertices.push_back( D3DXVECTOR3( x, y, z ) );
		}	

	}while( script.tok != FINISHED );
}


#ifdef _DEBUG
struct ColorVertex
{
	enum { FVF = ( D3DFVF_XYZ | D3DFVF_DIFFUSE ) };
	ColorVertex( float x, float y, float z, D3DCOLOR color ) : _x( x ), _y( y ), _z( z ), _color( color ) { }

	float _x, _y, _z;
	D3DCOLOR _color;
};


void CWorld::UpdateContinentLines( )
{
	for( ENVIR_INFO_Iter iter = m_cContinents.begin(); iter != m_cContinents.end(); ++iter )
	{
		ENVIR_INFO& kInfo = *iter;

		size_t size = kInfo._cVertices.size();
		if( size < 2 )
			continue;

		if( kInfo._VBforLine )
			SAFE_RELEASE( kInfo._VBforLine );

		m_pd3dDevice->CreateVertexBuffer( (size) * sizeof( ColorVertex ), D3DUSAGE_WRITEONLY, ColorVertex::FVF, D3DPOOL_MANAGED, &kInfo._VBforLine, 0 );
		
		ColorVertex* pOtVertices = 0;
		kInfo._VBforLine->Lock( 0, 0, (void**)&pOtVertices, 0 );

		D3DXVECTOR3 kPos;
		for( int idx = 0; idx < (int)( size ); ++idx )
		{
			kPos = kInfo._cVertices[ idx ];
			pOtVertices[ idx ] = ColorVertex( kPos.x, kPos.y, kPos.z, D3DCOLOR_XRGB( 255, 0, 0 ) );
		}

		kInfo._VBforLine->Unlock( );
	}
}

void CWorld::RenderContinentLines( )
{
	for( ENVIR_INFO_Iter iter = m_cContinents.begin(); iter != m_cContinents.end(); ++iter )
	{
		ENVIR_INFO& kInfo = *iter;
	
		size_t size = kInfo._cVertices.size();
		m_pd3dDevice->SetStreamSource(0, kInfo._VBforLine, 0, sizeof(ColorVertex));
		m_pd3dDevice->SetFVF(ColorVertex::FVF);
		m_pd3dDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, size-1 );
	}
}
#endif


#endif //__BS_CHANGING_ENVIR