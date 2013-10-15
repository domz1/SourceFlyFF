#include "stdafx.h"



CTerrainMng::CTerrainMng()
{
	ZeroMemory( m_aTerrain, sizeof( m_aTerrain ) );//* MAX_TERRAIN );
	m_nWaterFrame = 0;
	m_pd3dDevice = NULL;
	m_nSize = 0;
#if __VER >= 14 // __WATER_EXT
	for(int i=0; i<MAX_WATER; i++)
		m_fWaterFrame[i] = 0.0f;
#endif //__WATER_EXT
}

CTerrainMng::~CTerrainMng()
{
	for ( int i = 0 ; i < m_nWaterFrame ; i++ )
	{
		SAFE_DELETE_ARRAY( m_pWaterIndexList[i].pList );
	}
	SAFE_DELETE_ARRAY( m_pWaterIndexList );
}
HRESULT CTerrainMng::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return S_OK;
}
HRESULT CTerrainMng::RestoreDeviceObjects()
{
	return S_OK;
}
HRESULT CTerrainMng::InvalidateDeviceObjects()
{
	return S_OK;
}
HRESULT CTerrainMng::DeleteDeviceObjects()
{
	for( int i = 0; i < MAX_TERRAIN; i++ )
	{
		if( m_aTerrain[ i ].m_pTexture )
			SAFE_RELEASE( m_aTerrain[ i ].m_pTexture );
	}
	return S_OK;
}
BOOL CTerrainMng::LoadTexture( DWORD dwId )
{
	if( (int)( dwId ) >= m_nSize )
		return FALSE;
	LPTERRAIN lpTerrain = GetTerrain( dwId );
	if( lpTerrain && lpTerrain->m_pTexture == NULL )
	{
		CString strPath;
		if( g_Option.m_nTextureQuality == 0 )
			strPath = MakePath( DIR_WORLDTEX, lpTerrain->m_szTextureFileName );
		else
		if( g_Option.m_nTextureQuality == 1 )
			strPath = MakePath( DIR_WORLDTEXMID, lpTerrain->m_szTextureFileName );
		else
			strPath = MakePath( DIR_WORLDTEXLOW, lpTerrain->m_szTextureFileName );
		D3DXIMAGE_INFO srcInfo;
		HRESULT hr = LoadTextureFromRes( m_pd3dDevice, strPath, 
			D3DX_DEFAULT, D3DX_DEFAULT, 4, 0, D3DFMT_UNKNOWN, 
			D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
			D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, &srcInfo, NULL, &lpTerrain->m_pTexture );
		
		if( FAILED( hr ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}
BOOL CTerrainMng::LoadScript( LPCTSTR lpszFileName )
{
	CScanner scanner;
	if(scanner.Load(lpszFileName, FALSE )==FALSE)
		return FALSE;

	LPTERRAIN lpTerrain;
	m_nSize = 0;
	int nBrace = 1;
	scanner.SetMark();
	int i = scanner.GetNumber(); // folder or id
	
	//	현재는 물만 이런 형태의 프레임을 사용할 것이라는 전제하에 
	//	하나만 되어 있지만 여러개를 사용할때는 이 부분을 구조체나
	//	클래스로 바꾸어서 다시 만들어야 할것이다.
	int FrameCnt = 0, ImageCnt = 0, IdCnt = 0;
	FrameCnt = scanner.GetNumber();
	if ( nBrace == 1 && FrameCnt )
	{
		m_nWaterFrame = FrameCnt;
		m_pWaterIndexList = new WaterTexList[ m_nWaterFrame ];
		ZeroMemory( m_pWaterIndexList, m_nWaterFrame * sizeof( WaterTexList ) );
	}

	while( nBrace )
	{
		if( *scanner.token == '}' || scanner.tok == FINISHED )
		{
			nBrace--;
			if( nBrace > 0 )
			{
				scanner.SetMark();
				i = scanner.GetNumber();  // folder or id
				if( i > m_nSize ) m_nSize = i;
				
				FrameCnt = scanner.GetNumber();
				IdCnt = 0;
				if ( nBrace == 1 && FrameCnt )
				{
					m_nWaterFrame = FrameCnt;
					m_pWaterIndexList = new WaterTexList[ m_nWaterFrame ];
					ZeroMemory( m_pWaterIndexList, m_nWaterFrame * sizeof( WaterTexList ) );
					ImageCnt = 0;
				}
				else if ( nBrace == 2 && FrameCnt )
				{
					m_pWaterIndexList[ImageCnt].ListCnt = FrameCnt;
#if __VER >= 14 // __WATER_EXT
					m_pWaterIndexList[ImageCnt].fWaterFrame = 0.15f;
#endif //__WATER_EXT
					m_pWaterIndexList[ImageCnt].pList = new int[ FrameCnt ];
					ZeroMemory( m_pWaterIndexList[ImageCnt].pList, FrameCnt * sizeof(int) );
					ImageCnt++;
				}				
				continue;
			}
			if( nBrace == 0 )
				continue;
		}
		//i = _ttoi( scanner.token );
		scanner.GetToken(); // { or filename

		if( *scanner.token == '{' )
		{
			nBrace++;
			scanner.SetMark();
			i = scanner.GetNumber(); // id
			//	여기서 추가하는 방법이 괄호후에 바로 문자열이 오지 못하게 하는 것 뿐인듯하다.
			//	마침 디폴트는 괄호 다음에 오는 것이 아닌 상태이기 때문에 가능하다.
			FrameCnt = scanner.GetNumber();	//	Count
			if ( i == 0 )
			{
				if ( nBrace == 2 && FrameCnt )
				{
					m_pWaterIndexList[ImageCnt].ListCnt = FrameCnt;
#if __VER >= 14 // __WATER_EXT
					m_pWaterIndexList[ImageCnt].fWaterFrame = 0.15f;
#endif //__WATER_EXT
					m_pWaterIndexList[ImageCnt].pList = new int[ FrameCnt ];
					ImageCnt++;
				}
			}

			if( i > m_nSize ) m_nSize = i;
			continue;
		}
		else
		{
			scanner.GoMark();
			i = scanner.GetNumber(); // id
			if( i > m_nSize ) m_nSize = i;
			FrameCnt = scanner.GetNumber();	//	Count
			if ( nBrace == 3 )
			{
				m_pWaterIndexList[ImageCnt - 1].pList[IdCnt] = i;
				IdCnt++;
			}
		}
		lpTerrain = &m_aTerrain[ i ];
		lpTerrain->m_dwId = i;
		scanner.GetToken();  // texture fileName
		strcpy( lpTerrain->m_szTextureFileName, scanner.token );
		lpTerrain->m_bBlock = scanner.GetNumber(); // block
		scanner.GetToken(); // sound
		strcpy( lpTerrain->m_szSoundFileName, scanner.token );
		scanner.SetMark();
		i = scanner.GetNumber();  // texture fileName
		if( i > m_nSize ) m_nSize = i;
	}
/*
	
	int i = scanner.GetNumber( TRUE );  // id
	while( scanner.tok != FINISHED )
	{
		lpTerrain = &m_aTerrain[ i ];
		lpTerrain->m_dwId = i;
		scanner.GetToken( TRUE );  // texture fileName
		strcpy( lpTerrain->m_szTextureFileName, scanner.token );
		lpTerrain->m_bBlock = scanner.GetNumber( TRUE ); // block
		scanner.GetToken( TRUE ); // sound
		strcpy( lpTerrain->m_szSoundFileName, scanner.token );
		i = scanner.GetNumber( TRUE );  // texture fileName
		if( i > m_nSize ) m_nSize = i;
	}
*/	
	m_nSize++;
	return TRUE;
}
