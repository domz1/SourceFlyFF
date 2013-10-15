#include "StdAfx.h"
#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "MapMonsterInformationPack.h"

//-----------------------------------------------------------------------------
CMapMonsterInformationPack::CMapMonsterInformationPack( void )
{
}
//-----------------------------------------------------------------------------
CMapMonsterInformationPack::~CMapMonsterInformationPack( void )
{
	DeleteAllMapMonsterInformation();
}
//-----------------------------------------------------------------------------
BOOL CMapMonsterInformationPack::LoadScript( const CString& strFileName )
{
	CScript scanner;
	if( scanner.Load( strFileName ) == FALSE )
	{
		Error( _T( "%s not found" ), strFileName );
		return FALSE;
	}

	DeleteAllMapMonsterInformation();

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

			D3DXIMAGE_INFO ImageInformation;

			if( bMultiLanguage )
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
					&ImageInformation, 
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
					&ImageInformation, 
					NULL, 
					&m_pTexture );
			}

			m_size.cx = ImageInformation.Width;
			m_size.cy = ImageInformation.Height;
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
				CMapMonsterInformation* pMapMonsterInformation = new CMapMonsterInformation;
				pMapMonsterInformation->SetMonsterIDNumber( scanner.GetNumber() );
				for( int j = 0; j < pMapMonsterInformation->GetMonsterIDNumber(); ++j )
				{
					DWORD dwMonsterID = scanner.GetNumber();
					pMapMonsterInformation->InsertMonsterID( dwMonsterID );
				}
				CRect rectIconPosition( 0, 0, 0, 0 );
				rectIconPosition.left = scanner.GetNumber();
				rectIconPosition.top = scanner.GetNumber();
				rectIconPosition.right = rectIconPosition.left + size.cx;
				rectIconPosition.bottom = rectIconPosition.top + size.cy;
				pMapMonsterInformation->SetIconPositionRect( rectIconPosition );
				pMapMonsterInformation->SetDropItemID( scanner.GetNumber() );

				m_MapMonsterInformationVector.push_back( pMapMonsterInformation );
			}
		}
	}
	while( scanner.tok != FINISHED );

	return TRUE;
}
//-----------------------------------------------------------------------------
const CMapMonsterInformation* CMapMonsterInformationPack::GetMapMonsterInformation( int nIndex ) const
{
	if( nIndex < 0 || nIndex >= static_cast< int >( m_MapMonsterInformationVector.size() ) )
	{
		return NULL;
	}
	return m_MapMonsterInformationVector[ nIndex ];
}
//-----------------------------------------------------------------------------
void CMapMonsterInformationPack::DeleteAllMapMonsterInformation( void )
{
	for( MapMonsterInformationVector::iterator Iterator = m_MapMonsterInformationVector.begin(); Iterator != m_MapMonsterInformationVector.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_MapMonsterInformationVector.clear();
}
//-----------------------------------------------------------------------------
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM