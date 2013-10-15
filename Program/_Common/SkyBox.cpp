#include "stdafx.h"
#include "defineObj.h"
#include "sfx.h"

#ifdef __JEFF_11
#include "environment.h"

#ifndef __ENVIRONMENT_EFFECT
extern	CEnvironment	g_Environment;
#endif // __ENVIRONMENT_EFFECT
#endif	// __JEFF_11


CSkyBox::CSkyBox()
{
	m_fCloudx=m_fCloudy=0;
	m_fSunAngle=10.0f;
	m_pWorld=NULL;
	if( g_Option.m_nTextureQuality == 0 )
	{
		m_nFall=400;
		//m_nFall=1000;
		//m_nStar=1000;
	}
	else
	if( g_Option.m_nTextureQuality == 1 )
	{
		m_nFall=300;
		//m_nFall=500;
		//m_nStar=500;
	}
	else
	if( g_Option.m_nTextureQuality == 2 )
	{
		m_nFall=300;
		//m_nFall=300;
		//m_nStar=300;
	}
	for( int i = 0; i < (int)( m_nFall ); i++ )  
	{
		m_vFall[ i ] = D3DXVECTOR3( 0, 0, 0 );
		m_vVelocity[ i ] = D3DXVECTOR3( 0, 0.3f, 0 );
	}
	//for(i=0;i<m_nFall;i++) m_vFall[i]=D3DXVECTOR3(0,0,0);
	//m_bOldSkyBox=TRUE;
	m_bOldSkyBox=FALSE;
	m_nWeather = WEATHER_NONE; 
//	for(i=0;i<m_nStar;i++) m_vStar[i]=D3DXVECTOR3(0,0,0);

#if __VER >= 15 // __BS_CHANGING_ENVIR
	m_bLockWeather = FALSE;
#endif
}
CSkyBox::~CSkyBox()
{
	Free();
}
void CSkyBox::Free()
{
}

#ifdef __ENVIRONMENT_EFFECT
CSkyBox* CSkyBox::GetInstance()
{
	static CSkyBox	sSkyBox;
	return & sSkyBox;
}
#endif // __ENVIRONMENT_EFFECT

void CSkyBox::SetWeather( int nWeather, BOOL bOnOff )
{
#if __VER >= 15 // __BS_CHANGING_ENVIR
	if( m_bLockWeather )		//gmpbigsun(100112) : 현재 대륙날씨쪽에서 락을 시키고 풀어줌 
		return;
#endif

	if( bOnOff == FALSE )
	{
		if( nWeather == m_nWeather )
			m_timerWeather.Set( 0 ); // 내리는 것을 그치게 한다.
	}
	else
		m_timerWeather.Set( MIN( 1 ) ); // 5분간 비나 눈이 내린다.

	if( bOnOff == FALSE )
	{
		if( nWeather == m_nWeather )
			m_nWeather = WEATHER_NONE;
	}
	else
	if( m_nWeather != nWeather )
	{
		m_nWeather = nWeather;
		InitFall();
	}
}

void CSkyBox::InitFall()
{
	for( int i = 0; i < (int)( m_nFall ); i++ ) 
	{
		m_vFall[i].x = g_pPlayer->GetPos().x + ( float( rand() - RAND_MAX / 2 ) / RAND_MAX * 25 );
		m_vFall[i].z = g_pPlayer->GetPos().z + ( float( rand() - RAND_MAX / 2 ) / RAND_MAX * 25 );
		m_vFall[i].y = g_pPlayer->GetPos().y + 10 + ( float( rand() ) / RAND_MAX * 10 );

		if( m_nWeather == WEATHER_RAIN )
		{
			int x = 0;//xRandom( 20 ) - 10;
			int y = (-1) * (int)( xRandom( 10 ) ) + (-7);
			int z = 0;//xRandom( 20 ) - 10;
			m_vVelocity[ i ] = D3DXVECTOR3( float( x ) / 800, float( y ) / 200 - 0.05f, float( z ) / 800  );
		}
		else
		{
			int x = xRandom( 50 ) - 25;
			int y = (-1) * (int)( xRandom( 10 ) );
			int z = xRandom( 50 ) - 25;
			m_vVelocity[ i ] = D3DXVECTOR3( float( x ) / 800, float( y ) / 200 - 0.05f, float( z ) / 800  );
		}
		//m_vVelocity[ i ] = D3DXVECTOR3( 0, float( y ) / 50.0f, 0  );
	}
}
void CSkyBox::Process()
{
	if(!g_Option.m_nWeatherEffect) return;

	if( m_pWorld ) 
	{
#ifdef __JEFF_11
#ifdef __CLIENT
		if( m_pWorld->GetID() == WI_WORLD_KEBARAS )
		{
#ifdef __ENVIRONMENT_EFFECT

			if( CEnvironment::GetInstance()->GetSeason() != SEASON_WINTER )
			{
				CEnvironment::GetInstance()->SetSeason( SEASON_WINTER );

				CWorld::m_skyBox.SetWeather( WEATHER_SNOW, TRUE );
			}

#else // __ENVIRONMENT_EFFECT

			if( !g_Environment.m_bSnow )
			{
				g_Environment.m_bSnow	= TRUE;
				g_Environment.m_bRain	= FALSE;

				CWorld::m_skyBox.SetWeather( WEATHER_SNOW, TRUE );
			}

#endif // __ENVIRONMENT_EFFECT
		}
#endif	// __CLIENT
#endif	// __JEFF_11
		BOOL bFall = FALSE;

		if( m_nWeather == WEATHER_RAIN )
		{
			for( int i = 0; i < (int)( m_nFall ); i++ ) 
			{
				FLOAT fheight = m_pWorld->GetLandHeight_Fast( m_vFall[ i ].x, m_vFall[ i ].z ) - 1.0f;
				if( fheight == 0 ) fheight = 100000.0f;
				if( 1 ) //m_timerWeather.IsTimeOut() == FALSE )
				{
					if( fheight > m_vFall[ i ].y ) 
					{
						if( xRandom(10) >= 7 )
						{
							D3DXVECTOR3 v3Pos;
							D3DXVECTOR3 vec3Tri[3];
							m_pWorld->GetLandTri( m_vFall[ i ].x, m_vFall[ i ].z, vec3Tri );
							FLOAT ffHeight = m_pWorld->GetLandHeight( m_vFall[ i ].x, m_vFall[ i ].z );
									
							D3DXVECTOR3 vVector1 = vec3Tri[2] - vec3Tri[0];
							D3DXVECTOR3 vVector2 = vec3Tri[1] - vec3Tri[0];
							D3DXVECTOR3 vNormal;
							D3DXVec3Cross( &vNormal, &vVector1, &vVector2);	
							D3DXVec3Normalize( &vNormal, &vNormal );
								
							D3DXVECTOR3 v3Up = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
							D3DXVECTOR3 v3Cross;
							FLOAT fDot;
							FLOAT fTheta;
							D3DXVec3Cross( &v3Cross, &v3Up, &vNormal );
							fDot = D3DXVec3Dot( &v3Up, &vNormal );
							fTheta = acos( fDot );
							
							D3DXQUATERNION qDirMap;
							D3DXQuaternionRotationAxis( &qDirMap, &v3Cross, fTheta );

							v3Pos = m_vFall[ i ];
							v3Pos.y = ffHeight+0.1f;

							D3DXVECTOR3 vYPW;
							QuaternionRotationToYPW( qDirMap, vYPW );
							
							// 물이 있으면 물효과
							LPWATERHEIGHT pWaterHeight = m_pWorld->GetWaterHeight( m_vFall[ i ] );

							if( pWaterHeight && 
								( pWaterHeight->byWaterTexture & (byte)(~MASK_WATERFRAME) )	== WTYPE_WATER )
							{
								FLOAT fHeight = (FLOAT)pWaterHeight->byWaterHeight;
								v3Pos.y = fHeight;
								CSfx *pObj = CreateSfx( g_Neuz.m_pd3dDevice, XI_GEN_RAINCIRCLE01, v3Pos );
								FLOAT fscal = (FLOAT)( xRandom(2)+1 );

								fscal *= 0.1f;

								pObj->SetScale( D3DXVECTOR3( fscal, fscal, fscal ) ); 
							}
							else
							{
								FLOAT fscal = xRandom(2)+0.05f;
								CSfx *pObj = CreateSfx( g_Neuz.m_pd3dDevice, XI_GEN_RAINCIRCLE01, v3Pos );
								fscal *= 0.1f;
								pObj->SetScale( D3DXVECTOR3( fscal, fscal, fscal ) ); 

								pObj->m_pSfxObj->m_vRotate.x = D3DXToDegree(vYPW.x);
								pObj->m_pSfxObj->m_vRotate.y = D3DXToDegree(vYPW.y);
								pObj->m_pSfxObj->m_vRotate.z = D3DXToDegree(vYPW.z);
								
							}
						}
						m_vFall[i].x = g_pPlayer->GetPos().x+ (float(rand()-RAND_MAX/2)/RAND_MAX*25);
						m_vFall[i].z = g_pPlayer->GetPos().z+ (float(rand()-RAND_MAX/2)/RAND_MAX*25);
						m_vFall[i].y = g_pPlayer->GetPos().y+10+(float(rand())/RAND_MAX*10);
					}
					else 
					{
						m_vFall[ i ] += m_vVelocity[ i ] * 1.5;
					}
					bFall = TRUE;
				}
				else
				// 타임 아웃 되었음. 끝.
				if( fheight < m_vFall[ i ].y ) 
				{
					bFall = TRUE;
					m_vFall[ i ] += m_vVelocity[ i ] * 1.5; // 눈보다 빠르게 
				}
			}
		}
		else
		if( m_nWeather == WEATHER_SNOW )
		{
			for( int i = 0; i < (int)( m_nFall ); i++) 
			{
				FLOAT fheight = m_pWorld->GetLandHeight_Fast( m_vFall[ i ].x, m_vFall[ i ].z ) - 1.0f;
				if( fheight == 0 ) 
					fheight = 100000.0f;
				if( 1 ) //m_timerWeather.IsTimeOut() == FALSE )
				{
					if( fheight > m_vFall[ i ].y ) 
					{
						m_vFall[i].x = g_pPlayer->GetPos().x+ (float(rand()-RAND_MAX/2)/RAND_MAX*70);
						m_vFall[i].z = g_pPlayer->GetPos().z+ (float(rand()-RAND_MAX/2)/RAND_MAX*70);
						m_vFall[i].y = g_pPlayer->GetPos().y+10+(float(rand())/RAND_MAX*20);
					}
					else 
						m_vFall[ i ] += m_vVelocity[ i ];
					bFall = TRUE;
				}
				else
				// 타임 아웃 되었음. 끝.
				if( fheight < m_vFall[ i ].y ) 
				{
					bFall = TRUE;
					m_vFall[ i ] += m_vVelocity[ i ];
				}
			}			
		}
		else
		if( m_nWeather == WEATHER_FALL )
		{
			for( int i = 0; i < (int)( m_nFall ); i++) 
			{
				FLOAT fheight = m_pWorld->GetLandHeight_Fast( m_vFall[ i ].x, m_vFall[ i ].z ) - 1.0f;
				if( fheight == 0 ) 
					fheight = 100000.0f;
				if( 1 ) //m_timerWeather.IsTimeOut() == FALSE )
				{
					if( fheight > m_vFall[ i ].y ) 
					{
						m_vFall[i].x = g_pPlayer->GetPos().x+ (float(rand()-RAND_MAX/2)/RAND_MAX*70);
						m_vFall[i].z = g_pPlayer->GetPos().z+ (float(rand()-RAND_MAX/2)/RAND_MAX*70);
						m_vFall[i].y = g_pPlayer->GetPos().y+10+(float(rand())/RAND_MAX*20);
					}
					else 
						m_vFall[ i ] += m_vVelocity[ i ];
					bFall = TRUE;
				}
				else
				// 타임 아웃 되었음. 끝.
				if( fheight < m_vFall[ i ].y ) 
				{
					bFall = TRUE;
					m_vFall[ i ] += m_vVelocity[ i ];
				}
			}			
		}
	//	if( bFall == FALSE )
	//		m_nWeather = WEATHER_NONE;
	}
}
void CSkyBox::Render( CWorld* pWorld, LPDIRECT3DDEVICE9 pd3dDevice )
{
	_PROFILE("CSkyBox::Render()");

	m_pWorld=pWorld;

	D3DXVECTOR3 vecPos( 0, 0, 0 );
	D3DXVECTOR3 vecLookAt = pWorld->m_pCamera->m_vLookAt;
	D3DXVECTOR3 vecTempLookAt;
	vecLookAt -= pWorld->m_pCamera->GetPos();//m_vPos2;
	float angle1=(float)(atan(vecLookAt.x/vecLookAt.z)*180/3.1415926f);
	if(vecLookAt.z<0) angle1+=180;
	
	// 하늘 출력 
	D3DXMATRIX mat, matWorld;
	float fTemp=-angle1/360.0f;

	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	D3DXMATRIX  matView;
	
	vecLookAt.z=(float)(sqrt((vecLookAt.x*vecLookAt.x)+(vecLookAt.z*vecLookAt.z)));
	vecLookAt.x=0;
	D3DXMatrixRotationY( &mat,angle1*3.1415926f/180.0f);
	D3DXVec3TransformCoord(  &vecLookAt,  &vecLookAt,  &mat );
	vecLookAt.y+=pWorld->m_pCamera->GetPos().y;//m_vPos2.y;
	vecPos.y+=pWorld->m_pCamera->GetPos().y;//m_vPos2.y;
	vecTempLookAt=vecLookAt;
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	matView._41=0;matView._42=0;matView._43=0;
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_ALWAYS );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
//	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
//	pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0,128,128,128) ); //m_dwAmbient );//D3DCOLOR_ARGB( 255,128,128,128) );//D3DCOLOR_ARGB( 255,50,50,70) );
	pd3dDevice->SetFVF( D3DFVF_D3DSKYBOXVERTEX );
	pd3dDevice->SetVertexShader( NULL );

	D3DXMATRIX matProj, matProj2;
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj2 );
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, 1.0f, 0.1f, 300.0f );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	mat=matView;

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,   D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	pd3dDevice->SetTransform( D3DTS_VIEW, &mat);
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj2 );

	// 병풍 하늘 출력 
/*
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
	DWORD dwColor = CWorld::GetDiffuseColor();
	pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  dwColor );//CWorld::m_dwBgColor ) ;//CWorld::m_dwBgColor );
	pd3dDevice->SetRenderState( D3DRS_FOGSTART,   FtoDW(29.0f) );
	pd3dDevice->SetRenderState( D3DRS_FOGEND,     FtoDW(100.0f) );
*/	
	pd3dDevice->SetStreamSource( 0, m_pSideSkyBoxVB, 0, sizeof( D3DSKYBOXVERTEX ) );
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/ );
	// 시간에 따라 하늘 텍스쳐를 자연스럽게 바꿔치기한다.

	float fAlphaEX = 1.0f;			//added 
	int rstA = 255;				//added
#if __VER >= 15 // __BS_CHANGING_ENVIR
	//gmpbigsun: Hook은 리턴값으로 기존 루틴을 타거나 타지 않는다 
	// FALSE인 경우( 루틴을 타야 하는경우 ) 곱해질 알파가 계산되어져 나오게 댐.
	if( !m_pWorld->HookRenderSky( SKY_SIDE, 348/3, fAlphaEX ) )
#endif 
	{
		IDirect3DTexture9* pSky1 = m_pSkyBoxTexture;
		IDirect3DTexture9* pSky2 = m_pSkyBoxTexture2;
		IDirect3DTexture9* pSky3 = m_pSkyBoxTexture3;

#if __VER >= 15 // __BS_CHANGING_ENVIR
		WORLD_ENVIR_INFO& kWorldInfo = m_pWorld->m_kWorldEnvir;
		if( kWorldInfo._kSky[ 0 ]._bUse )
		{
			// use sky texture of the world ( not default )
			pSky1 = kWorldInfo._kSky[ 0 ]._pTexture;
			pSky2 = kWorldInfo._kSky[ 1 ]._pTexture;
			pSky3 = kWorldInfo._kSky[ 2 ]._pTexture;
		}

		if( !pSky1 || !pSky2 || !pSky3 )
		{
			assert( 0 );
			return;
		}

#endif

		if(g_GameTimer.m_nHour<6) 
		{
			rstA = (int)( fAlphaEX * 255 );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pSky3);
			//pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 85 );//56);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 348 / 3 );
		}
		else
		if(g_GameTimer.m_nHour == 6) 
		{
			rstA = (int)( (255-(g_GameTimer.m_nMin*255/59))*fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pSky3);
			//pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 85 );// 56);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 348 / 3 );

			rstA = (int)( (g_GameTimer.m_nMin*255/59)*fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pSky1);
			//pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 85 );//56);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 348 / 3 );
		}
		else 
		if(g_GameTimer.m_nHour < 17) 
		{
			rstA = (int)( fAlphaEX * 255 );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pSky1);
			//pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 85 );//56);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 348 / 3 );
		}
		else 
		if(g_GameTimer.m_nHour == 17 || g_GameTimer.m_nHour == 18 ) 
		{
			int nMin = g_GameTimer.m_nHour == 17 ? g_GameTimer.m_nMin : g_GameTimer.m_nMin + 60;
			rstA	= (int)( (255-(nMin*255/119))*fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pSky1);
			//pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 85 );//56);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 348 / 3 );

			rstA	= (int)( (nMin*255/119)*fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pSky2);
			//pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 85 );//56);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 348 / 3 );
		}
		else 
		if(g_GameTimer.m_nHour == 19) 
		{
			rstA	= (int)( (255-(g_GameTimer.m_nMin*255/59))*fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pSky2);
			//pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 85 );//56);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 348 / 3 );

			rstA	= (int)( (g_GameTimer.m_nMin*255/59)*fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pSky3);
			//pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 85 );//56);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 348 / 3 );
		}
		else 
		{
			rstA	= (int)( fAlphaEX * 255 );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pSky3);
			//pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 85 );//56);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 348 / 3 );
		}
	}

	// 자붕 하늘 출력 
//	pd3dDevice->SetTexture( 0, m_pSkyBoxTexture);
//	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 56);
///	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 58, 28);
//	pWorld->SetFogEnable(pd3dDevice,FALSE); 
//	pd3dDevice->SetRenderState( D3DRS_FOGSTART,   FtoDW(50.0f) );
//	pd3dDevice->SetRenderState( D3DRS_FOGEND,     FtoDW(100.0f) );
	// 높은 구름 UV 스크롤 
	D3DXMatrixIdentity( &mat );
	static float m_fCloud_u1 = 0;
	static float m_fCloud_v1 = 0;
	mat._31 = m_fCloud_u1;
	mat._32 = m_fCloud_v1;
	m_fCloud_u1 += 0.0002f;
	m_fCloud_v1 += 0.0002f;
	pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );

	pd3dDevice->SetStreamSource( 0, m_pTopSkyBoxVB, 0, sizeof( D3DSKYBOXVERTEX ) );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 )/*adeilson*/ );

#if __VER >= 15 // __BS_CHANGING_ENVIR
	if( !m_pWorld->HookRenderSky( SKY_CLOUD, 28, fAlphaEX ) )
#endif 
	{
		IDirect3DTexture9* pCloud1 = m_pCloudTexture;
		IDirect3DTexture9* pCloud2 = m_pCloudTexture2;
		IDirect3DTexture9* pCloud3 = m_pCloudTexture3;
	
#if __VER >= 15 // __BS_CHANGING_ENVIR
		WORLD_ENVIR_INFO& kWorldInfo = m_pWorld->m_kWorldEnvir;
		if( kWorldInfo._kCloud[ 0 ]._bUse )
		{
			// use cloud texture of the world( not default )
			pCloud1 = kWorldInfo._kCloud[ 0 ]._pTexture;
			pCloud2 = kWorldInfo._kCloud[ 1 ]._pTexture;
			pCloud3 = kWorldInfo._kCloud[ 2 ]._pTexture;
		}

		if( !pCloud1 || !pCloud2 || !pCloud3 )
		{
			assert( 0 );
			return;
		}
#endif
		if( g_GameTimer.m_nHour < 6 ) 
		{
			rstA	= (int)( fAlphaEX * 255 );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pCloud3);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 28);
		}
		else 
		if( g_GameTimer.m_nHour == 6 ) 
		{
			rstA	= (int)( ( 255-(g_GameTimer.m_nMin*255/59) ) * fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pCloud3);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 28);

			rstA	= (int)( ( g_GameTimer.m_nMin*255/59 ) * fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pCloud1);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 28);
		//	TRACE("%d\n",g_GameTimer.m_nMin*255/59 );
		}
		else 
		if( g_GameTimer.m_nHour < 17 ) 
		{
			rstA	= (int)( 255 * fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pCloud1);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 28);
		}
		else 
		if( g_GameTimer.m_nHour == 17 || g_GameTimer.m_nHour == 18 ) 
		{
			int nMin = g_GameTimer.m_nHour == 17 ? g_GameTimer.m_nMin : g_GameTimer.m_nMin + 60;
			rstA	= (int)( ( 255-(nMin*255/119) ) * fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pCloud1);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 28);

			rstA	= (int)( (nMin*255/119) * fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pCloud2);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 28);
		}
		else 
		if( g_GameTimer.m_nHour == 19 ) 
		{
	/*		else if(g_GameTimer.m_nHour==17) {
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255-(g_GameTimer.m_nMin*255/59), 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, m_pCloudTexture);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 28);
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( g_GameTimer.m_nMin*255/59, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, m_pCloudTexture2);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 28);
		}
		else if(g_GameTimer.m_nHour==18) {
		*/

			rstA	= (int)( ( 255-(g_GameTimer.m_nMin*255/59) ) * fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pCloud2 );
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 28 );

			rstA	= (int)( ( g_GameTimer.m_nMin*255/59 ) * fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pCloud3 );
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 28 );
		}
		else 
		{
			rstA	= (int)( 255 * fAlphaEX );
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( rstA, 0, 0, 0 ) );
			pd3dDevice->SetTexture( 0, pCloud3 );
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 28);
		}
	}
	
	//태양 & 달 
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
//	pd3dDevice->SetTexture( 0, m_pCloudTexture);
//	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 28);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

//	if( g_GameTimer.GetSunPercent() )
	m_fSunAngle = g_GameTimer.m_nHour*60+g_GameTimer.m_nMin-720.0f+((float)g_GameTimer.m_nSec/60.0f);
	m_fSunAngle = -(float)(m_fSunAngle*m_fSunAngle)/2500.0f+45.0f;

	IDirect3DTexture9* pSun1 = m_pSunTexture;
	IDirect3DTexture9* pMoon = m_pMoonTexture;

#if __VER >= 15 // __BS_CHANGING_ENVIR
	WORLD_ENVIR_INFO& kWorldInfo = m_pWorld->m_kWorldEnvir;

	if( kWorldInfo._kSun._bUse )
		pSun1 = kWorldInfo._kSun._pTexture;

	if( kWorldInfo._kMoon._bUse )
		pMoon = kWorldInfo._kMoon._pTexture;

	if( !pSun1 || !pMoon )
	{
		assert ( 0 );
		return;
	}

#endif

//	if(m_fSunAngle>-20.0f)
	{
		if( (g_GameTimer.m_nHour >= 20 ) || ( g_GameTimer.m_nHour >= 1 && g_GameTimer.m_nHour <=6 ) )
		{
			D3DSUNVERTEX* pVB;
			m_pSunVB->Lock( 0, 0, (void**)&pVB, 0 );
			
			pVB[0].p=D3DXVECTOR3( -2.0f, 2.0f,30.0f); pVB[0].tu1=0.0f; pVB[0].tv1=0.0f;
			pVB[1].p=D3DXVECTOR3(  2.0f, 2.0f,30.0f); pVB[1].tu1=1.0f; pVB[1].tv1=0.0f;
			pVB[2].p=D3DXVECTOR3(  2.0f,-2.0f,30.0f); pVB[2].tu1=1.0f; pVB[2].tv1=1.0f;
			pVB[3].p=D3DXVECTOR3( -2.0f,-2.0f,30.0f); pVB[3].tu1=0.0f; pVB[3].tv1=1.0f;

			int nS = g_GameTimer.m_nHour;
			if( ( g_GameTimer.m_nHour >= 1 && g_GameTimer.m_nHour <=6 ) )
				nS = 24 + g_GameTimer.m_nHour;

			CTimeSpan cp( 0, nS, g_GameTimer.m_nMin, g_GameTimer.m_nSec );

			LONG nSec = (LONG)( cp.GetTotalSeconds() );

			m_fSunAngle = (nSec*0.00015f)+-90;
			D3DXMatrixRotationX( &mat,m_fSunAngle);
			
			//D3DXMatrixRotationX( &mat,timeGetTime()/9000.0f);
	//		D3DXMatrixRotationX( &mat,-m_fSunAngle*3.1415926f/180.0f);
			D3DXVec3TransformCoord(&pVB[0].p,&pVB[0].p,&mat);
			D3DXVec3TransformCoord(&pVB[1].p,&pVB[1].p,&mat);
			D3DXVec3TransformCoord(&pVB[2].p,&pVB[2].p,&mat);
			D3DXVec3TransformCoord(&pVB[3].p,&pVB[3].p,&mat);
			//	pVB[0].p.y+=pWorld->m_pCamera->m_vPos2.y;///1.5f;
			//	pVB[1].p.y+=pWorld->m_pCamera->m_vPos2.y;///1.5f;
			//	pVB[2].p.y+=pWorld->m_pCamera->m_vPos2.y;///1.5f;
			//	pVB[3].p.y+=pWorld->m_pCamera->m_vPos2.y;///1.5f;
			pVB[0].c=pVB[1].c=pVB[2].c=pVB[3].c=D3DCOLOR_ARGB(255,255,255,255);
			
			m_pSunVB->Unlock();
			
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
			
			pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			pd3dDevice->SetFVF( D3DFVF_D3DSUNVERTEX );
			
			pd3dDevice->SetStreamSource( 0, m_pSunVB, 0, sizeof( D3DSUNVERTEX ) );
			pd3dDevice->SetTexture( 0, pMoon);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);		
		}
		else
		if( (g_GameTimer.m_nHour >= 7 ) && ( g_GameTimer.m_nHour <= 17 ) )
		{
			D3DSUNVERTEX* pVB;
			m_pSunVB->Lock( 0, 0, (void**)&pVB, 0 );
			
			pVB[0].p=D3DXVECTOR3( -1.0f, 1.0f,30.0f); pVB[0].tu1=0.0f; pVB[0].tv1=0.0f;
			pVB[1].p=D3DXVECTOR3(  1.0f, 1.0f,30.0f); pVB[1].tu1=1.0f; pVB[1].tv1=0.0f;
			pVB[2].p=D3DXVECTOR3(  1.0f,-1.0f,30.0f); pVB[2].tu1=1.0f; pVB[2].tv1=1.0f;
			pVB[3].p=D3DXVECTOR3( -1.0f,-1.0f,30.0f); pVB[3].tu1=0.0f; pVB[3].tv1=1.0f;

			int nS = g_GameTimer.m_nHour;

			CTimeSpan cp( 0, nS, g_GameTimer.m_nMin, g_GameTimer.m_nSec );

			LONG nSec = (LONG)( cp.GetTotalSeconds() );

			m_fSunAngle = (nSec*0.00015f)+-90;
			D3DXMatrixRotationX( &mat,m_fSunAngle);
			/*
			D3DXMatrixRotationX( &mat, m_fSunAngle*0.9f );
			D3DXMatrixRotationX( &mat,-m_fSunAngle*3.1415926f/180.0f);
			D3DXMatrixRotationX( &mat,(nSec*0.00015f)+-270);
			*/
			//D3DXMatrixRotationX( &mat,timeGetTime()/9000.0f);
	//		D3DXMatrixRotationX( &mat,-m_fSunAngle*3.1415926f/180.0f);
			D3DXVec3TransformCoord(&pVB[0].p,&pVB[0].p,&mat);
			D3DXVec3TransformCoord(&pVB[1].p,&pVB[1].p,&mat);
			D3DXVec3TransformCoord(&pVB[2].p,&pVB[2].p,&mat);
			D3DXVec3TransformCoord(&pVB[3].p,&pVB[3].p,&mat);
			//	pVB[0].p.y+=pWorld->m_pCamera->m_vPos2.y;///1.5f;
			//	pVB[1].p.y+=pWorld->m_pCamera->m_vPos2.y;///1.5f;
			//	pVB[2].p.y+=pWorld->m_pCamera->m_vPos2.y;///1.5f;
			//	pVB[3].p.y+=pWorld->m_pCamera->m_vPos2.y;///1.5f;
			pVB[0].c=pVB[1].c=pVB[2].c=pVB[3].c=D3DCOLOR_ARGB(255,255,255,255);
			
			m_pSunVB->Unlock();
			
			
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_DIFFUSE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			pd3dDevice->SetFVF( D3DFVF_D3DSUNVERTEX );
		
			/*
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_DIFFUSE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,   D3DTA_DIFFUSE);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
			*/
			pd3dDevice->SetStreamSource( 0, m_pSunVB, 0, sizeof( D3DSUNVERTEX ) );
			pd3dDevice->SetTexture( 0, pSun1);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);	
			
			//////////////////////////////////////////////////////////////////////////
			// 태양 출력 2
			//////////////////////////////////////////////////////////////////////////
			
			pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
			matView._41=0.0f;
			matView._42=0.0f;
			matView._43=0.0f;
			pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
			
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_DIFFUSE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
			//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			pd3dDevice->SetFVF( D3DFVF_D3DSUNVERTEX );
			
			pd3dDevice->SetStreamSource( 0, m_pStarVB, 0, sizeof( D3DSUNVERTEX ) );
			//	pd3dDevice->SetTexture( 0, NULL);
			
			m_pLensFlareVB[0]->Lock( 0, 0, (void**)&pVB, 0 );
			pVB[0].p=D3DXVECTOR3( -5.0f, 5.0f,0.0f); pVB[0].tu1=0.0f; pVB[0].tv1=0.0f;
			pVB[1].p=D3DXVECTOR3(  5.0f, 5.0f,0.0f); pVB[1].tu1=1.0f; pVB[1].tv1=0.0f;
			pVB[2].p=D3DXVECTOR3(  5.0f,-5.0f,0.0f); pVB[2].tu1=1.0f; pVB[2].tv1=1.0f;
			pVB[3].p=D3DXVECTOR3( -5.0f,-5.0f,0.0f); pVB[3].tu1=0.0f; pVB[3].tv1=1.0f;
			pVB[0].c=pVB[1].c=pVB[2].c=pVB[3].c=D3DCOLOR_ARGB(255,255,255,255);
			m_pLensFlareVB[0]->Unlock();
			
			D3DXMatrixRotationX( &mat,m_fSunAngle);
			matWorld._43=30;
			D3DXMatrixMultiply(&matWorld,&matWorld,&mat);
			
			D3DXMATRIX  matView2,matView3,matWorld2;
			matView3=matWorld;
			
			pd3dDevice->GetTransform( D3DTS_VIEW, &matView2 );
			pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld2 );
			D3DXMatrixInverse(&matView2,&fTemp,&matView2);
			matWorld=matView2;
			matWorld._41=matView3._41;matWorld._42=matView3._42;matWorld._43=matView3._43;
			
			pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
			
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_DIFFUSE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,   D3DTA_DIFFUSE);
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			pd3dDevice->SetFVF( D3DFVF_D3DSUNVERTEX );
			pd3dDevice->SetStreamSource( 0, m_pLensFlareVB[0], 0, sizeof( D3DSUNVERTEX ) );
			pd3dDevice->SetTexture( 0, m_pSunTexture2);
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
			pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld2 );
		}


	}
	//////////////////////////////////////////////////////////////////////////
	// 상태 초기화 
	//////////////////////////////////////////////////////////////////////////
		
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
/*
	if(!m_bOldSkyBox) {
		D3DXMATRIX matTemp;
		pd3dDevice->GetTransform( D3DTS_WORLD, &matTemp );
		D3DXMatrixIdentity(&matWorld);
		for(int i=0;i<m_nStar;i++) {
			pd3dDevice->SetTransform( D3DTS_WORLD, &(m_matStar[i]) );
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
		}
	}
*/
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	//pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}


BOOL CSkyBox::CheckSun(LPDIRECT3DDEVICE9 pd3dDevice)
{
	D3DXMATRIX mat,matView,matProj;
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
	D3DXVECTOR3 m_vIntersect;
	D3DXVECTOR3 vSun( 0.0f,0.0f,300.0f);
	D3DXVECTOR3 vPos(m_pWorld->m_pCamera->m_vPos);

	D3DXMatrixRotationX( &mat,m_fSunAngle);
	D3DXVec3TransformCoord(&vSun,&vSun,&mat);
	vSun+=vPos;
	D3DXMatrixLookAtLH( &matView, &vPos, &vSun, &D3DXVECTOR3(0.0f,1.0f,0.0f) );

	CRect rect(0,0,800,600);

	CPoint ptCursor((rect.right-rect.left)/2,(rect.bottom-rect.top)/2);
//return TRUE;
	if( m_pWorld->IsPickTerrain( rect, ptCursor, &matProj, &matView ) )
		return FALSE;
	if( m_pWorld->PickObject_Fast( rect, ptCursor, &matProj, &matView, 0xffffffff, NULL, TRUE ) )
		return FALSE;

	return TRUE;
}

void CSkyBox::DrawLensFlare(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if( !((g_GameTimer.m_nHour >= 7 ) && ( g_GameTimer.m_nHour <= 17 )) )
		return;

	if( g_GameTimer.m_nHour < 12 )
	{
		if(m_fSunAngle<-84.6f) 
			return;
	}
	else
	{
		if(m_fSunAngle>-81.66f) 
			return;
	}

	
	
	if(!CheckSun(pd3dDevice)) return;
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	D3DSUNVERTEX* pVB;
	D3DXVECTOR3 vecPos( 0, 0, 0 );
	D3DXVECTOR3 vecLookAt = m_pWorld->m_pCamera->m_vLookAt;
	D3DXVECTOR3 vecTempLookAt;
	vecLookAt -= m_pWorld->m_pCamera->GetPos();//m_vPos2;
	float angle1=(float)(atan(vecLookAt.x/vecLookAt.z)*180/3.1415926f);
	if(vecLookAt.z<0) angle1+=180;
	// 하늘 출력 
	D3DXMATRIX mat, matWorld;
	float fTemp=-angle1/360.0f;

	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXMATRIX  matView;
	vecLookAt.z=(float)(sqrt((vecLookAt.x*vecLookAt.x)+(vecLookAt.z*vecLookAt.z)));
	vecLookAt.x=0;
	D3DXMatrixRotationY( &mat,angle1*3.1415926f/180.0f);
	D3DXVec3TransformCoord(  &vecLookAt,  &vecLookAt,  &mat );
	vecLookAt.y+=m_pWorld->m_pCamera->GetPos().y;//m_vPos2.y;
	vecPos.y+=m_pWorld->m_pCamera->GetPos().y;//m_vPos2.y;
	vecTempLookAt=vecLookAt;
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	matView._41=0;matView._42=0;matView._43=0;
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );


	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
/*
	m_pLensFlareVB[0]->Lock( 0, 0, (void**)&pVB, 0 );
	pVB[0].p=D3DXVECTOR3( -5.0f, 5.0f,0.0f); pVB[0].tu1=0.0f; pVB[0].tv1=0.0f;
	pVB[1].p=D3DXVECTOR3(  5.0f, 5.0f,0.0f); pVB[1].tu1=1.0f; pVB[1].tv1=0.0f;
	pVB[2].p=D3DXVECTOR3(  5.0f,-5.0f,0.0f); pVB[2].tu1=1.0f; pVB[2].tv1=1.0f;
	pVB[3].p=D3DXVECTOR3( -5.0f,-5.0f,0.0f); pVB[3].tu1=0.0f; pVB[3].tv1=1.0f;
	pVB[0].c=pVB[1].c=pVB[2].c=pVB[3].c=D3DCOLOR_ARGB(255,255,255,255);
	m_pLensFlareVB[0]->Unlock();
*/
	D3DXMatrixRotationX( &mat,m_fSunAngle);
	matWorld._43=30;
	D3DXMatrixMultiply(&matWorld,&matWorld,&mat);

	D3DXMATRIX  matView2,matView3;
	matView3=matWorld;

	pd3dDevice->GetTransform( D3DTS_VIEW, &matView2 );
	D3DXMatrixInverse(&matView2,&fTemp,&matView2);
	matWorld=matView2;
	matWorld._41=matView3._41;matWorld._42=matView3._42;matWorld._43=matView3._43;

	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,   D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetFVF( D3DFVF_D3DSUNVERTEX );
/*
	pd3dDevice->SetStreamSource(0,m_pLensFlareVB[0],sizeof(D3DSUNVERTEX));
	pd3dDevice->SetTexture( 0, m_pSunTexture2);
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
*/
	vecTempLookAt.y-=m_pWorld->m_pCamera->GetPos().y;//m_vPos2.y;
	D3DXVec3Normalize(&vecTempLookAt,&vecTempLookAt);
	vecTempLookAt*=30.0f;
	D3DXVECTOR3 vecSunPos=D3DXVECTOR3(matWorld._41,matWorld._42,matWorld._43);
	D3DXVECTOR3 vecDelta=vecSunPos-vecTempLookAt;
	vecTempLookAt=vecSunPos;
	float fLength=D3DXVec3Length(&vecDelta);
	int nAlpha=0;
	if(fLength<20) nAlpha=255-(int)(fLength*255/20.0f);

	float radius;
	float afRadius[ 8 ] = { 25.0f, 2.0f, 2.0f, 1.0f, 1.0f, 1.0f, 3.0f, 4.0f };
	DWORD adwColor[ 8 ] = 
	{
		D3DCOLOR_ARGB(nAlpha,255,240,220),
		D3DCOLOR_ARGB(nAlpha,255,235,200),
		D3DCOLOR_ARGB(nAlpha,255,235,200),
		D3DCOLOR_ARGB(nAlpha,255,235,200),
		D3DCOLOR_ARGB(nAlpha,50,50,100),
		D3DCOLOR_ARGB(nAlpha,100,0,0),
		D3DCOLOR_ARGB(nAlpha,200,255,200),
		D3DCOLOR_ARGB(nAlpha,255,255,255)
	};
	for( int i = 0; i < 8; i++ )
	{
		m_pLensFlareVB[ i ]->Lock( 0, 0, (void**)&pVB, 0 );
		radius = afRadius[ i];
		pVB[ 0 ].p=D3DXVECTOR3( -radius, radius,0.0f); pVB[0].tu1=0.0f; pVB[0].tv1=0.0f;
		pVB[ 1 ].p=D3DXVECTOR3(  radius, radius,0.0f); pVB[1].tu1=1.0f; pVB[1].tv1=0.0f;
		pVB[ 2 ].p=D3DXVECTOR3(  radius,-radius,0.0f); pVB[2].tu1=1.0f; pVB[2].tv1=1.0f;
		pVB[ 3 ].p=D3DXVECTOR3( -radius,-radius,0.0f); pVB[3].tu1=0.0f; pVB[3].tv1=1.0f;
		pVB[ 0 ].c = pVB[ 1 ].c = pVB[ 2 ].c = pVB[ 3 ].c = adwColor[ i ];//D3DCOLOR_ARGB( nAlpha, 255, 240, 220 );
		m_pLensFlareVB[ i ]->Unlock();
	}
	for( i = 0; i < 7; i++ )
	{
		pd3dDevice->SetStreamSource(0,m_pLensFlareVB[ i + 1 ],0,sizeof(D3DSUNVERTEX));
		pd3dDevice->SetTexture( 0, m_pLensTexture[ 6 - i ]);
		vecTempLookAt-=vecSunPos/10;	vecTempLookAt-=vecDelta/10;
		matWorld._41=vecTempLookAt.x;matWorld._42=vecTempLookAt.y;matWorld._43=vecTempLookAt.z;
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
	}
/*
	pd3dDevice->SetStreamSource(0,m_pLensFlareVB[0],sizeof(D3DSUNVERTEX));
	pd3dDevice->SetTexture( 0, m_pLensTexture[7]);
	vecTempLookAt+=vecSunPos*7/10;	vecTempLookAt+=vecDelta*7/10;
	matWorld._41=vecTempLookAt.x;matWorld._42=vecTempLookAt.y;matWorld._43=vecTempLookAt.z;
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
*/
	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}
void CSkyBox::RenderFall( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if(!g_Option.m_nWeatherEffect) return;

	if( m_nWeather == WEATHER_RAIN )
		DrawRain( pd3dDevice );
	else
	if( m_nWeather == WEATHER_SNOW )
		DrawSnow( pd3dDevice );
	else//meu
	if( m_nWeather == WEATHER_FALL )
		DrawFall( pd3dDevice );
}
void CSkyBox::DrawRain(LPDIRECT3DDEVICE9 pd3dDevice)
{
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	pd3dDevice->SetFVF( D3DFVF_D3DSUNVERTEX );
	pd3dDevice->SetStreamSource(0,m_pRainVB,0,sizeof(D3DSUNVERTEX));
	pd3dDevice->SetTexture( 0, NULL);

	D3DXMATRIX matTemp,matWorld,matView;
	pd3dDevice->GetTransform( D3DTS_WORLD, &matTemp );
	D3DXMatrixIdentity(&matWorld);
	
	if( m_pWorld ) 
	{
		D3DXVECTOR3 vPos    = m_pWorld->m_pCamera->m_vPos;
		D3DXVECTOR3 vLookAt = m_pWorld->m_pCamera->m_vLookAt;
		vPos.y = 0;
		vLookAt.y = 0;
		D3DXMatrixLookAtLH( &matWorld, &vPos, &vLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
		float fTemp;
		D3DXMatrixInverse(&matWorld,&fTemp,&matWorld);
		for(int i=0;i<(int)( m_nFall );i++) 
		{
			matWorld._41 = m_vFall[i].x; 
			matWorld._42 = m_vFall[i].y; 
			matWorld._43 = m_vFall[i].z;
			pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
		}

	}
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}

void CSkyBox::DrawSnow(LPDIRECT3DDEVICE9 pd3dDevice)
{
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	pd3dDevice->SetTexture( 0, m_pSunTexture);
	pd3dDevice->SetFVF( D3DFVF_D3DSUNVERTEX );
	pd3dDevice->SetStreamSource(0,m_pSnowVB,0,sizeof(D3DSUNVERTEX));

	D3DXMATRIX  matTemp,matWorld,matView;
	float fTemp;
	pd3dDevice->GetTransform( D3DTS_WORLD, &matTemp );
	pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	D3DXMatrixInverse(&matView,&fTemp,&matView);
	matWorld=matView;

	if( ::GetLanguage() == LANG_JAP )
	{
		D3DXMATRIX matScal;
		D3DXMatrixScaling( &matScal, 2.5f, 2.5f, 2.5f );
		matWorld *= matScal;
	}

	for(int i=0;i<(int)( m_nFall );i++) 
	{
		matWorld._41=m_vFall[i].x; matWorld._42=m_vFall[i].y; matWorld._43=m_vFall[i].z;
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
	}
	pd3dDevice->SetTransform( D3DTS_WORLD, &matTemp );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}
//MEU
void CSkyBox::DrawFall(LPDIRECT3DDEVICE9 pd3dDevice)
{
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	pd3dDevice->SetTexture( 0, m_pSunTexture);
	pd3dDevice->SetFVF( D3DFVF_D3DSUNVERTEX );
	pd3dDevice->SetStreamSource(0,m_pFallVB,0,sizeof(D3DSUNVERTEX));

	D3DXMATRIX  matTemp,matWorld,matView;
	float fTemp;
	pd3dDevice->GetTransform( D3DTS_WORLD, &matTemp );
	pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	D3DXMatrixInverse(&matView,&fTemp,&matView);
	matWorld=matView;

	if( ::GetLanguage() == LANG_JAP )
	{
		D3DXMATRIX matScal;
		D3DXMatrixScaling( &matScal, 2.5f, 2.5f, 2.5f );
		matWorld *= matScal;
	}

	for(int i=0;i<(int)( m_nFall );i++) 
	{
		matWorld._41=m_vFall[i].x; matWorld._42=m_vFall[i].y; matWorld._43=m_vFall[i].z;
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
	}
	pd3dDevice->SetTransform( D3DTS_WORLD, &matTemp );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}

void CSkyBox::SetVertices()
{
	float tiled=1.0f;
	float radius=110.0f;
	float x,z;
	if( m_pTopSkyBoxVB ) 
	{
		D3DSKYBOXVERTEX* pVB;
		m_pTopSkyBoxVB->Lock( 0, 0, (void**)&pVB, 0 );
//		if(m_pWorld)
//			pVB[0].p=D3DXVECTOR3(0,radius/6+m_pWorld->m_pCamera->m_vPos2.y,0);
//		else
//			pVB[0].p=D3DXVECTOR3(0.0f,radius/6,0.0f);
		pVB[0].p=D3DXVECTOR3(0.0f,100.0f,0.0f);
		pVB[0].tu1=tiled+m_fCloudx;
		pVB[0].tv1=tiled;
		pVB[0].c=D3DCOLOR_ARGB(255,255,255,255);
		for(int i = 1; i < 30; i++ ) 
		{
			x=(float)sin( 2 * D3DX_PI / 28 * i ) * radius;
			z=(float)cos( 2 * D3DX_PI / 28 * i ) * radius;
			pVB[ i ].p   = D3DXVECTOR3( x, 40.0f, z );
			pVB[ i ].tu1 = ( x + radius ) * tiled / radius + m_fCloudx;
			pVB[ i ].tv1 = ( z + radius ) * tiled / radius;
			pVB[ i ].c   = D3DCOLOR_ARGB( 0, 255, 255, 255 );
		}
		m_pTopSkyBoxVB->Unlock();
	}
	if( m_pSideSkyBoxVB ) 
	{
		D3DSKYBOXVERTEX* pVB;
		m_pSideSkyBoxVB->Lock( 0, 0, (void**)&pVB, 0 );
		for( int i = 0; i < 28; i++ ) 
		{
			FLOAT x1 = (float)sin( 2 * D3DX_PI / 28 * i ) * radius;
			FLOAT z1 = (float)cos( 2 * D3DX_PI / 28 * i ) * radius;
			FLOAT x2 = (float)sin( 2 * D3DX_PI / 28 * ( i + 1 ) ) * radius;
			FLOAT z2 = (float)cos( 2 * D3DX_PI / 28 * ( i + 1 ) ) * radius;
			FLOAT fu1 = 1.0f / 28.0f * i;
			FLOAT fu2 = 1.0f / 28.0f * ( i + 1 );

			pVB->p   = D3DXVECTOR3( x1, 150.0f, z1 );
			pVB->tu1 = fu1;
			pVB->tv1 = 0.0f;
			pVB->c   = D3DCOLOR_ARGB( 0, 255, 255, 255 );
			pVB++;

			pVB->p   = D3DXVECTOR3( x1, 0.0f, z1 );
			pVB->tu1 = fu1;
			pVB->tv1 = 0.9f;
			pVB->c   = D3DCOLOR_ARGB( 255, 255, 255, 255 );
			pVB++;
			
			pVB->p   = D3DXVECTOR3( x2, 150.0f, z2 );
			pVB->tu1 = fu2;
			pVB->tv1 = 0.0f;
			pVB->c   = D3DCOLOR_ARGB( 0, 255, 255, 255 );
			pVB++;

			pVB->p   = D3DXVECTOR3( x2, 150.0f, z2 );
			pVB->tu1 = fu2;
			pVB->tv1 = 0.0f;
			pVB->c   = D3DCOLOR_ARGB( 0, 255, 255, 255 );
			pVB++;
			
			pVB->p   = D3DXVECTOR3( x1, 0.0f, z1 );
			pVB->tu1 = fu1;
			pVB->tv1 = 0.9f;
			pVB->c   = D3DCOLOR_ARGB( 255, 255, 255, 255 );
			pVB++;
			
			pVB->p   = D3DXVECTOR3( x2, 0.0f, z2 );
			pVB->tu1 = fu2;
			pVB->tv1 = 0.9f;
			pVB->c   = D3DCOLOR_ARGB( 255, 255, 255, 255 );
			pVB++;
						
			//////////////////

			pVB->p   = D3DXVECTOR3( x1, 0.0f, z1 );
			pVB->tu1 = fu1;
			pVB->tv1 = 0.9f;
			pVB->c   = D3DCOLOR_ARGB( 255, 255, 255, 255 );
			pVB++;
			
			pVB->p   = D3DXVECTOR3( x1, -20.0f, z1 );
			pVB->tu1 = fu1;
			pVB->tv1 = 1.0f;
			pVB->c   = D3DCOLOR_ARGB( 0, 255, 255, 255 );
			pVB++;
			
			pVB->p   = D3DXVECTOR3( x2, 0.0f, z2 );
			pVB->tu1 = fu2;
			pVB->tv1 = 0.9f;
			pVB->c   = D3DCOLOR_ARGB( 255, 255, 255, 255 );
			pVB++;
			
			pVB->p   = D3DXVECTOR3( x2, 0.0f, z2 );
			pVB->tu1 = fu2;
			pVB->tv1 = 0.9f;
			pVB->c   = D3DCOLOR_ARGB( 255, 255, 255, 255 );
			pVB++;
			
			pVB->p   = D3DXVECTOR3( x1, -20.0f, z1 );
			pVB->tu1 = fu1;
			pVB->tv1 = 1.0f;
			pVB->c   = D3DCOLOR_ARGB( 0, 255, 255, 255 );
			pVB++;
			
			pVB->p   = D3DXVECTOR3( x2, -20.0f, z2 );
			pVB->tu1 = fu2;
			pVB->tv1 = 1.0f;
			pVB->c   = D3DCOLOR_ARGB( 0, 255, 255, 255 );
			pVB++;

			
/*
			pVB->p   = D3DXVECTOR3( x, 150.0f, z );
			pVB->p   = D3DXVECTOR3( x,  50.0f, z );
			pVB->p   = D3DXVECTOR3( x, -20.0f, z );
			pVB->tu1 = 1.0f / 28.0f * i;
			pVB->tu1 = pVB[ i * 3 ].tu1;
			pVB->tu1 = pVB[ i * 3 ].tu1;
			pVB->tv1 = 0.0f;
			pVB->tv1 = 0.4f;
			pVB->tv1 = 1.0f;
			pVB->c   = D3DCOLOR_ARGB(0,255,255,255);
			pVB->c   = D3DCOLOR_ARGB(255,255,255,255);
			pVB->c   = D3DCOLOR_ARGB(0,255,255,255);
			

			pVB[ i * 3     ].p   = D3DXVECTOR3( x, 150.0f, z );
			pVB[ i * 3 + 1 ].p   = D3DXVECTOR3( x,  50.0f, z );
			pVB[ i * 3 + 2 ].p   = D3DXVECTOR3( x, -20.0f, z );
			pVB[ i * 3     ].tu1 = 1.0f / 28.0f * i;
			pVB[ i * 3 + 1 ].tu1 = pVB[ i * 3 ].tu1;
			pVB[ i * 3 + 2 ].tu1 = pVB[ i * 3 ].tu1;
			pVB[ i * 3     ].tv1 = 0.0f;
			pVB[ i * 3 + 1 ].tv1 = 0.4f;
			pVB[ i * 3 + 2 ].tv1 = 1.0f;
			pVB[ i * 3     ].c   = D3DCOLOR_ARGB(0,255,255,255);
			pVB[ i * 3 + 1 ].c   = D3DCOLOR_ARGB(255,255,255,255);
			pVB[ i * 3 + 2 ].c   = D3DCOLOR_ARGB(0,255,255,255);
			*/
			/*
			pVB[ i * 3     ].p   = D3DXVECTOR3( x, 150.0f, z );
			pVB[ i * 3 + 1 ].p   = D3DXVECTOR3( x,  50.0f, z );
			pVB[ i * 3 + 2 ].p   = D3DXVECTOR3( x, -20.0f, z );
			pVB[ i * 3     ].tu1 = 1.0f / 28.0f * i;
			pVB[ i * 3 + 1 ].tu1 = pVB[ i * 3 ].tu1;
			pVB[ i * 3 + 2 ].tu1 = pVB[ i * 3 ].tu1;
			pVB[ i * 3     ].tv1 = 0.0f;
			pVB[ i * 3 + 1 ].tv1 = 0.4f;
			pVB[ i * 3 + 2 ].tv1 = 1.0f;
			pVB[ i * 3     ].c   = D3DCOLOR_ARGB(0,255,255,255);
			pVB[ i * 3 + 1 ].c   = D3DCOLOR_ARGB(255,255,255,255);
			pVB[ i * 3 + 2 ].c   = D3DCOLOR_ARGB(0,255,255,255);
			*/
		}
/*
		pVB[58].p=D3DXVECTOR3(0.0f,130.0f,0.0f);
		pVB[58].tu1=.0f;
		pVB[58].tv1=.0f;
		pVB[58].c=D3DCOLOR_ARGB(255,255,255,255);
		for(i=1;i<30;i++) {
			x=(float)sin(2*D3DX_PI/28*i)*radius;
			z=(float)cos(2*D3DX_PI/28*i)*radius;
			pVB[i+58].p=D3DXVECTOR3(x,130.0f,z);
			pVB[i+58].tu1=.0f;
			pVB[i+58].tv1=.01f;
			pVB[i+58].c=D3DCOLOR_ARGB(255,255,255,255);
		}
*/
		m_pSideSkyBoxVB->Unlock();
	}
}
void CSkyBox::SetStarVertices()
{
	//D3DXMATRIX matTemp;
	//for(int i=0;i<m_nStar;i++) 
	//{
	//	D3DXMatrixRotationX( &(m_matStar[i]),rand());
	//	D3DXMatrixRotationY( &matTemp,rand());
	//	D3DXMatrixMultiply(&(m_matStar[i]),&(m_matStar[i]),&matTemp);
	//}
}

HRESULT CSkyBox::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "Moon.dds"),
										&m_pMoonTexture ) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}

#ifdef __ENVIRONMENT_EFFECT

	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "sundisk.bmp"), &m_pSunTexture ) ) )
	{
		return	D3DAPPERR_MEDIANOTFOUND;
	}

	
	if( CEnvironment::GetInstance()->GetSeason() == SEASON_SPRING )
	{
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "sakura.bmp"), &m_pSunTexture ) ) )
		{
			return	D3DAPPERR_MEDIANOTFOUND;
		}
	}

	else if( CEnvironment::GetInstance()->GetSeason() == SEASON_FALL )
	{
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "leaf.bmp"), &m_pSunTexture ) ) )
		{
			return	D3DAPPERR_MEDIANOTFOUND;
		}
	}

	else
	{
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "sundisk.bmp"), &m_pSunTexture ) ) )
		{
			return	D3DAPPERR_MEDIANOTFOUND;
		}
	}

#else // __ENVIRONMENT_EFFECT

#if defined( __EVENT_FALL )
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "leaf.bmp"), &m_pSunTexture ) ) )
	{
		return	D3DAPPERR_MEDIANOTFOUND;
	}
#elif defined( __JAPAN_SAKURA )
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "sakura.bmp"), &m_pSunTexture ) ) )
	{
		return	D3DAPPERR_MEDIANOTFOUND;
	}
#else
    if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "sundisk.bmp"), &m_pSunTexture ) ) )
	{
		return	D3DAPPERR_MEDIANOTFOUND;
	}
#endif

#endif // __ENVIRONMENT_EFFECT

	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "flare_3.tga"),
									   &m_pSunTexture2 ) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "lenzflare_001.tga"),
									   &(m_pLensTexture[0])) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "lenzflare_002.tga"),
									   &(m_pLensTexture[1])) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "lenzflare_003.tga"),
									   &(m_pLensTexture[2])) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "lenzflare_004.tga"),
									   &(m_pLensTexture[3])) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "lenzflare_005.tga"),
									   &(m_pLensTexture[4])) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "lenzflare_006.tga"),
									   &(m_pLensTexture[5])) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "lenzflare_007.tga"),
									   &(m_pLensTexture[6])) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "lenzflare_008.tga"),
									   &(m_pLensTexture[7])) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}
	/*
	if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "snow04.bmp"),
									   &m_pSnowTexture ) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}
	*/
	if( g_Option.m_nTextureQuality == 2 )
	{
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "skybox01_low.dds" ), &m_pSkyBoxTexture ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "skybox02_low.dds"), &m_pSkyBoxTexture2 ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "skybox03_low.dds"),  &m_pSkyBoxTexture3 ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "cloud01_low.dds"), &m_pCloudTexture ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "cloud02_low.dds"), &m_pCloudTexture2 ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "cloud03_low.dds"), &m_pCloudTexture3 ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
	}
	else
	if( g_Option.m_nTextureQuality == 1 )
	{
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "skybox01_low.dds" ), &m_pSkyBoxTexture ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "skybox02_low.dds"), &m_pSkyBoxTexture2 ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "skybox03.dds"),  &m_pSkyBoxTexture3 ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "cloud01_low.dds"), &m_pCloudTexture ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "cloud02_low.dds"), &m_pCloudTexture2 ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "cloud03.dds"), &m_pCloudTexture3 ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
	}
	else
	{
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "skybox01.dds" ), &m_pSkyBoxTexture ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "skybox02.dds"), &m_pSkyBoxTexture2 ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "skybox03.dds"),  &m_pSkyBoxTexture3 ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "cloud01.dds"), &m_pCloudTexture ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "cloud02.dds"), &m_pCloudTexture2 ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
		if( FAILED( LoadTextureFromRes( pd3dDevice, MakePath( DIR_WEATHER, "cloud03.dds"), &m_pCloudTexture3 ) ) )
		{ return D3DAPPERR_MEDIANOTFOUND; }
	}
	return S_OK;
}
HRESULT CSkyBox::RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if( m_pTopSkyBoxVB ) return S_OK;

	pd3dDevice->CreateVertexBuffer( 30 *sizeof(D3DSKYBOXVERTEX),
									D3DUSAGE_WRITEONLY, 0, D3DPOOL_SYSTEMMEM ,
									&m_pTopSkyBoxVB, NULL);
	pd3dDevice->CreateVertexBuffer( 348/*58*/ *sizeof(D3DSKYBOXVERTEX),
									D3DUSAGE_WRITEONLY, 0, D3DPOOL_SYSTEMMEM ,
									&m_pSideSkyBoxVB, NULL);
	pd3dDevice->CreateVertexBuffer( 4 *sizeof(D3DSUNVERTEX),
									D3DUSAGE_WRITEONLY, 0, D3DPOOL_SYSTEMMEM ,
									&m_pSunVB, NULL);

	for(int i=0;i<8;i++)
		pd3dDevice->CreateVertexBuffer( 4 *sizeof(D3DSUNVERTEX),
										D3DUSAGE_WRITEONLY, 0, D3DPOOL_SYSTEMMEM ,
										&m_pLensFlareVB[i],NULL);
	pd3dDevice->CreateVertexBuffer( 4*sizeof(D3DSUNVERTEX),
									D3DUSAGE_WRITEONLY, 0, D3DPOOL_SYSTEMMEM ,
									&m_pRainVB, NULL);
	pd3dDevice->CreateVertexBuffer( 4 *sizeof(D3DSUNVERTEX),
									D3DUSAGE_WRITEONLY, 0, D3DPOOL_SYSTEMMEM ,
									&m_pSnowVB, NULL);

	pd3dDevice->CreateVertexBuffer( 4 *sizeof(D3DSUNVERTEX),
									D3DUSAGE_WRITEONLY, 0, D3DPOOL_SYSTEMMEM ,
									&m_pFallVB, NULL);

	pd3dDevice->CreateVertexBuffer( 4 *sizeof(D3DSUNVERTEX),
									D3DUSAGE_WRITEONLY, 0, D3DPOOL_SYSTEMMEM ,
									&m_pStarVB, NULL);
	D3DSUNVERTEX* pRainVB;
	m_pRainVB->Lock( 0, 0, (void**)&pRainVB, 0 );
	pRainVB[0].p=D3DXVECTOR3( 0.0f, 2.0f, 0.0f); pRainVB[0].tu1=0.0f; pRainVB[0].tv1=0.0f;
	pRainVB[1].p=D3DXVECTOR3( 0.01f,2.0f, 0.0f); pRainVB[1].tu1=1.0f; pRainVB[1].tv1=0.0f;
	pRainVB[2].p=D3DXVECTOR3( 0.01f,0.0f, 0.0f); pRainVB[2].tu1=0.0f; pRainVB[2].tv1=1.0f;
	pRainVB[3].p=D3DXVECTOR3( 0.0f, 0.0f, 0.0f); pRainVB[3].tu1=1.0f; pRainVB[3].tv1=1.0f;
	pRainVB[0].c=pRainVB[1].c=D3DCOLOR_ARGB(0,128,128,128);
	pRainVB[2].c=pRainVB[3].c=D3DCOLOR_ARGB(100,128,128,128);
	m_pRainVB->Unlock();

	D3DSUNVERTEX* pSnowVB;
	m_pSnowVB->Lock( 0, 0, (void**)&pSnowVB, 0 );
	pSnowVB[0].p=D3DXVECTOR3( -0.05f,  0.05f, 0.0f); pSnowVB[0].tu1=0.0f; pSnowVB[0].tv1=0.0f;
	pSnowVB[1].p=D3DXVECTOR3(  0.05f,  0.05f, 0.0f); pSnowVB[1].tu1=1.0f; pSnowVB[1].tv1=0.0f;
	pSnowVB[2].p=D3DXVECTOR3(  0.05f, -0.05f, 0.0f); pSnowVB[2].tu1=1.0f; pSnowVB[2].tv1=1.0f;
	pSnowVB[3].p=D3DXVECTOR3( -0.05f, -0.05f, 0.0f); pSnowVB[3].tu1=0.0f; pSnowVB[3].tv1=1.0f;
	pSnowVB[0].c=pSnowVB[1].c=D3DCOLOR_ARGB(200,200,200,200);
	pSnowVB[2].c=pSnowVB[3].c=D3DCOLOR_ARGB(200,200,200,200);
	m_pSnowVB->Unlock();

	D3DSUNVERTEX* pFallVB;
	m_pFallVB->Lock( 0, 0, (void**)&pFallVB, 0 );
	pFallVB[0].p=D3DXVECTOR3( -0.05f,  0.05f, 0.0f); pFallVB[0].tu1=0.0f; pFallVB[0].tv1=0.0f;
	pFallVB[1].p=D3DXVECTOR3(  0.05f,  0.05f, 0.0f); pFallVB[1].tu1=1.0f; pFallVB[1].tv1=0.0f;
	pFallVB[2].p=D3DXVECTOR3(  0.05f, -0.05f, 0.0f); pFallVB[2].tu1=1.0f; pFallVB[2].tv1=1.0f;
	pFallVB[3].p=D3DXVECTOR3( -0.05f, -0.05f, 0.0f); pFallVB[3].tu1=0.0f; pFallVB[3].tv1=1.0f;
	pFallVB[0].c=pFallVB[1].c=D3DCOLOR_ARGB(200,200,200,200);
	pFallVB[2].c=pFallVB[3].c=D3DCOLOR_ARGB(200,200,200,200);
	m_pFallVB->Unlock();

	D3DSUNVERTEX* pStarVB;
	m_pStarVB->Lock( 0, 0, (void**)&pStarVB, 0 );
	pStarVB[0].p=D3DXVECTOR3(-0.1f, 50.0f, -0.1f); pStarVB[0].tu1=0.0f; pStarVB[0].tv1=0.0f;
	pStarVB[1].p=D3DXVECTOR3( 0.1f, 50.0f, -0.1f); pStarVB[1].tu1=1.0f; pStarVB[1].tv1=0.0f;
	pStarVB[2].p=D3DXVECTOR3( 0.1f, 50.0f,  0.1f); pStarVB[2].tu1=1.0f; pStarVB[2].tv1=1.0f;
	pStarVB[3].p=D3DXVECTOR3(-0.1f, 50.0f,  0.1f); pStarVB[3].tu1=0.0f; pStarVB[3].tv1=1.0f;
	pStarVB[0].c=pStarVB[1].c=D3DCOLOR_ARGB(255,255,200,100);
	pStarVB[2].c=pStarVB[3].c=D3DCOLOR_ARGB(255,255,200,100);
	m_pStarVB->Unlock();

	SetVertices();
	SetStarVertices();
	HRESULT hr=S_OK;
	return hr;
}
HRESULT CSkyBox::InvalidateDeviceObjects()
{
	SAFE_RELEASE(m_pTopSkyBoxVB);
	SAFE_RELEASE(m_pSideSkyBoxVB);
	SAFE_RELEASE(m_pSunVB);
	for(int i=0;i<8;i++) SAFE_RELEASE(m_pLensFlareVB[i]);
	SAFE_RELEASE(m_pRainVB);
	SAFE_RELEASE(m_pSnowVB);
	SAFE_RELEASE(m_pFallVB);
	SAFE_RELEASE(m_pStarVB);
	return S_OK;
}
HRESULT CSkyBox::DeleteDeviceObjects()
{
	SAFE_RELEASE(m_pMoonTexture);
	SAFE_RELEASE(m_pSkyBoxTexture);
	SAFE_RELEASE(m_pSkyBoxTexture2);
	SAFE_RELEASE(m_pSkyBoxTexture3);
	SAFE_RELEASE(m_pCloudTexture);
	SAFE_RELEASE(m_pCloudTexture2);
	SAFE_RELEASE(m_pCloudTexture3);
	SAFE_RELEASE(m_pSunTexture);
	SAFE_RELEASE(m_pSunTexture2);
	SAFE_RELEASE(m_pSnowTexture);
	for(int i=0;i<8;i++) SAFE_RELEASE(m_pLensTexture[i]);

	SAFE_RELEASE(m_pTopSkyBoxVB);
	SAFE_RELEASE(m_pSideSkyBoxVB);
	SAFE_RELEASE(m_pSunVB);
	for(i=0;i<8;i++) SAFE_RELEASE(m_pLensFlareVB[i]);
	SAFE_RELEASE(m_pRainVB);
	SAFE_RELEASE(m_pSnowVB);
	SAFE_RELEASE(m_pFallVB);
	SAFE_RELEASE(m_pStarVB);
	return S_OK;
}

IDirect3DTexture9* CSkyBox::GetCurrSkyTexture( SKY_TYPE eType )
{
	if(g_GameTimer.m_nHour<=6 || g_GameTimer.m_nHour > 19 ) 
	{
		if( SKY_SIDE == eType )
			return m_pSkyBoxTexture3;
		else return m_pCloudTexture3;
	}
	else
	if(g_GameTimer.m_nHour < 17) 
	{
		if( SKY_SIDE == eType )
			return m_pSkyBoxTexture;
		else return m_pCloudTexture;
	}
	else 
	if(g_GameTimer.m_nHour >= 17 && g_GameTimer.m_nHour <=  19 ) 
	{
		if( SKY_SIDE == eType )
			return m_pSkyBoxTexture2;
		else return m_pCloudTexture2;
	}

	return m_pSkyBoxTexture3;
}