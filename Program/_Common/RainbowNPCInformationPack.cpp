#include "StdAfx.h"
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "RainbowNPCInformationPack.h"

//-----------------------------------------------------------------------------
CRainbowNPCInformationPack::CRainbowNPCInformationPack( void )
{
}
//-----------------------------------------------------------------------------
CRainbowNPCInformationPack::~CRainbowNPCInformationPack( void )
{
	DeleteAllRainbowNPCInformation();
}
//-----------------------------------------------------------------------------
BOOL CRainbowNPCInformationPack::LoadScript( const CString& strFileName )
{
	CScript scanner;
	if( scanner.Load( strFileName ) == FALSE )
	{
		Error( _T( "%s not found" ), strFileName );
		return FALSE;
	}

	DeleteAllRainbowNPCInformation();

	DWORD dwCount = 0;
	CTexture* pTexture = NULL;
	BOOL bMultiLanguage = FALSE;

	do
	{
		scanner.GetToken();
		if( scanner.Token == _T( "number" ) )
		{
			m_dwNumber = scanner.GetNumber();
			m_ap2DTexture = new CTexture[ m_dwNumber ];
		}
		else if( scanner.Token == _T( "MULTI_LANGUAGE" ) )
		{
			bMultiLanguage = TRUE;
		}
		else if( scanner.Token == _T( "texture" ) )
		{	
			scanner.GetToken();

			CString strTexturePackFileName = scanner.token;
			D3DCOLOR d3dKeyColor = scanner.GetHex();

			D3DXIMAGE_INFO imageInfo;

			if( bMultiLanguage == TRUE )
			{
				LoadTextureFromRes( 
					D3DDEVICE, 
					MakePath( _T( "Theme\\" ), ::GetLanguage(), strTexturePackFileName ), 
					D3DX_DEFAULT, 
					D3DX_DEFAULT, 
					D3DX_DEFAULT, 
					0, 
					D3DFMT_UNKNOWN, 
					D3DPOOL_MANAGED, 
					D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, 
					D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, 
					d3dKeyColor, 
					&imageInfo, 
					NULL, 
					&m_pTexture );
			}
			else
			{
				LoadTextureFromRes( 
					D3DDEVICE, 
					MakePath( DIR_THEME, strTexturePackFileName ), 
					D3DX_DEFAULT, 
					D3DX_DEFAULT, 
					D3DX_DEFAULT, 
					0, 
					D3DFMT_UNKNOWN, 
					D3DPOOL_MANAGED, 
					D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, 
					D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, 
					d3dKeyColor, 
					&imageInfo, 
					NULL, 
					&m_pTexture );
			}

			m_size.cx = imageInfo.Width;
			m_size.cy = imageInfo.Height;
		}
		else if( scanner.Token == _T( "serialize" ) )
		{
			if( dwCount >= m_dwNumber )
			{
				Error( _T( "%s 에러, 할당 :%d, 실제갯수 : %d" ), strFileName, m_dwNumber, dwCount );
				return FALSE;
			}
			int nCount = 0;
			int nFrame = scanner.GetNumber();
			SIZE size;
			size.cx = scanner.GetNumber();
			size.cy = scanner.GetNumber();
			POINT start;
			start.x = scanner.GetNumber();
			start.y = scanner.GetNumber();
			POINT center;
			center.x = scanner.GetNumber();
			center.y = scanner.GetNumber();

			for( int i = start.y; i < m_size.cy; i += size.cy )
			{
				for( int j = start.x; j < m_size.cx; j += size.cx, ++nCount )
				{
					if( nCount < nFrame )
					{
						if( dwCount >= m_dwNumber )
						{
							Error( _T( "%s 에러, 할당 :%d, 실제갯수 : %d" ), strFileName, m_dwNumber, dwCount );
							return FALSE;
						}

						pTexture = &m_ap2DTexture[ dwCount ];
						++dwCount;

						pTexture->m_size = size;
						pTexture->m_ptCenter = center;
						FLOAT fPositionX = static_cast< FLOAT >( j );
						FLOAT fPositionY = static_cast< FLOAT >( i );
						FLOAT fTextureSizeX = static_cast< FLOAT >( pTexture->m_size.cx );
						FLOAT fTextureSizeY = static_cast< FLOAT >( pTexture->m_size.cy );
						FLOAT fTexturePackSizeX = static_cast< FLOAT >( m_size.cx );
						FLOAT fTexturePackSizeY = static_cast< FLOAT >( m_size.cy );
						pTexture->m_fuLT = fPositionX / fTexturePackSizeX;
						pTexture->m_fvLT = fPositionY / fTexturePackSizeY;
						pTexture->m_fuRT = ( fPositionX + fTextureSizeX ) / fTexturePackSizeX;
						pTexture->m_fvRT = fPositionY / fTexturePackSizeY;
						pTexture->m_fuLB = fPositionX / fTexturePackSizeX;
						pTexture->m_fvLB = ( fPositionY + fTextureSizeY ) / fTexturePackSizeY;
						pTexture->m_fuRB = ( fPositionX + fTextureSizeX ) / fTexturePackSizeX;
						pTexture->m_fvRB = ( fPositionY + fTextureSizeY ) / fTexturePackSizeY;
						pTexture->m_pTexture = m_pTexture;
					}
				}
			}

			for( int i = 0; i < nFrame; ++i )
			{
				CRainbowNPCInformation* pRainbowNPCInformation = new CRainbowNPCInformation;
				// 현재 사용하고 있는 맵 ID = 기존 스크립트의 맵 ID + 4
				pRainbowNPCInformation->SetMapID( scanner.GetNumber() + 4 );
				CRect rectWorldPosition( 0, 0, 0, 0 );
				rectWorldPosition.left = scanner.GetNumber();
				rectWorldPosition.top = scanner.GetNumber();
				rectWorldPosition.right = rectWorldPosition.left + size.cx;
				rectWorldPosition.bottom = rectWorldPosition.top + size.cy;
				pRainbowNPCInformation->SetWorldPositionRect( rectWorldPosition );
				CRect rectRegionPosition( 0, 0, 0, 0 );
				rectRegionPosition.left = scanner.GetNumber();
				rectRegionPosition.top = scanner.GetNumber();
				rectRegionPosition.right = rectRegionPosition.left + size.cx;
				rectRegionPosition.bottom = rectRegionPosition.top + size.cy;
				pRainbowNPCInformation->SetRegionPositionRect( rectRegionPosition );
				m_RainbowNPCInformationVector.push_back( pRainbowNPCInformation );
			}
		}
	}
	while( scanner.tok != FINISHED );

	return TRUE;
}
//-----------------------------------------------------------------------------
const CRainbowNPCInformation* CRainbowNPCInformationPack::GetRainbowNPCInformation( int nIndex ) const
{
	if( nIndex < 0 || nIndex >= static_cast< int >( m_RainbowNPCInformationVector.size() ) )
	{
		return NULL;
	}
	return m_RainbowNPCInformationVector[ nIndex ];
}
//-----------------------------------------------------------------------------
void CRainbowNPCInformationPack::DeleteAllRainbowNPCInformation( void )
{
	for( RainbowNPCInformationVector::iterator Iterator = m_RainbowNPCInformationVector.begin(); Iterator != m_RainbowNPCInformationVector.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_RainbowNPCInformationVector.clear();
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM