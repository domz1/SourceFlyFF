#include "stdafx.h"
#include "DialogMsg.h"
#include "timer.h"


CDialogMsg g_DialogMsg;

/////////////////

CDialogMsg::CDialogMsg()
{
//	m_textArray.SetSize(7);
	m_nLineSpace = 1;
	m_nBeginLine =  0;
	m_nAddLineNum = 0;
	m_pFont = NULL;
	m_pFontEffect = NULL;

	// 텍스쳐렌더 버그 관련 수정
	for(int i = 0; i < 3; i++)
		m_pTex[i] = NULL;
}
CDialogMsg::~CDialogMsg()
{
	ClearAllMessage();
}

HRESULT CDialogMsg::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_texPack_ex[0].LoadScript( pd3dDevice, MakePath( DIR_THEME, _T( "texDialogBox.inc" ) ) );
	m_texPack_ex[1].LoadScript( pd3dDevice, MakePath( DIR_THEME, _T( "texDialogBoxParty.inc" ) ) );
	m_texPack_ex[2].LoadScript( pd3dDevice, MakePath( DIR_THEME, _T( "texDialogBoxGuild.inc" ) ) );
	
	m_texEmoticon.LoadScript( pd3dDevice, MakePath( DIR_THEME, _T( "texDialogEmoticon.inc" ) ) );


	m_texEmoticonUser.LoadScript( pd3dDevice, MakePath( DIR_THEME, _T( "texChatEmoticon.inc" ) ) );
	LoadEmotion();

	m_pTex[0] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "texDialogBox_left.tga" ), 0xffff00ff );
	m_pTex[1] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "texDialogBox_center.TGA" ), 0xffff00ff );
	m_pTex[2] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "texDialogBox_right.tga" ), 0xffff00ff );
	
	RemoveDeleteObjMsg();
	
	return S_OK;
}
HRESULT CDialogMsg::DeleteDeviceObjects()
{
	m_texPack_ex[0].DeleteDeviceObjects();
	m_texPack_ex[1].DeleteDeviceObjects();
	m_texPack_ex[2].DeleteDeviceObjects();

	m_texEmoticon.DeleteDeviceObjects();
	m_texEmoticonUser.DeleteDeviceObjects();
	
	return S_OK;
}
HRESULT CDialogMsg::RestoreDeviceObjects()
{
	m_texEmoticon.RestoreDeviceObjects(g_Neuz.m_pd3dDevice);

	m_texPack_ex[0].RestoreDeviceObjects(g_Neuz.m_pd3dDevice);
	m_texPack_ex[1].RestoreDeviceObjects(g_Neuz.m_pd3dDevice);
	m_texPack_ex[2].RestoreDeviceObjects(g_Neuz.m_pd3dDevice);

	m_texEmoticonUser.RestoreDeviceObjects(g_Neuz.m_pd3dDevice);
	
	return S_OK;
}

HRESULT CDialogMsg::InvalidateDeviceObjects()
{
	m_texEmoticon.InvalidateDeviceObjects();

	m_texPack_ex[0].InvalidateDeviceObjects();
	m_texPack_ex[1].InvalidateDeviceObjects();
	m_texPack_ex[2].InvalidateDeviceObjects();

	m_texEmoticonUser.InvalidateDeviceObjects();
	
	return S_OK;
}
void CDialogMsg::ClearAllMessage()
{
	for(int i = 0; i < m_textArray.GetSize(); i++)
	{
		LPCUSTOMTEXT lpCustomText = (LPCUSTOMTEXT)m_textArray.GetAt(i);
		// 퀘스트 이모티콘을 다시 보이게 한다.
		if( lpCustomText->m_pObj->GetType() == OT_MOVER )
			((CMover*)lpCustomText->m_pObj)->m_bShowQuestEmoticon = TRUE;
	}
	m_textArray.RemoveAll();
	m_nAddLineNum = 0;

	for(i = 0; i < m_VendortextArray.GetSize(); i++)
		safe_delete( (LPCUSTOMTEXT)m_VendortextArray.GetAt( i ) );
	m_VendortextArray.RemoveAll();
}

void CDialogMsg::ClearMessage( CObj* pObj )
{
	for(int i = 0; i < m_textArray.GetSize(); i++)
	{
		LPCUSTOMTEXT pText	= (LPCUSTOMTEXT)m_textArray.GetAt( i );
		if( pText->m_pObj == pObj )
		{
			// 퀘스트 이모티콘을 다시 보이게 한다.
			if( pObj->GetType() == OT_MOVER )
				((CMover*)pObj)->m_bShowQuestEmoticon = TRUE;
			safe_delete( pText );
			m_textArray.RemoveAt( i );
			i--;
		}
	}

	for( i = 0; i < m_VendortextArray.GetSize(); i++ )
	{
		LPCUSTOMTEXT pText	= (LPCUSTOMTEXT)m_VendortextArray.GetAt( i );
		if( pText->m_pObj == pObj )
		{
			safe_delete( pText );
			m_VendortextArray.RemoveAt( i );
			i--;
		}
	}
}

void CDialogMsg::RemoveDeleteObjMsg()
{
	for( int i = 0; i < m_textArray.GetSize(); i++ )
	{
		LPCUSTOMTEXT lpCustomText 
			= (LPCUSTOMTEXT) m_textArray.GetAt( i );
		if( !IsValidObj( lpCustomText->m_pObj ) )
		{
			// 퀘스트 이모티콘을 다시 보이게 한다.
			if( lpCustomText->m_pObj->GetType() == OT_MOVER )
				((CMover*)lpCustomText->m_pObj)->m_bShowQuestEmoticon = TRUE;
			safe_delete( lpCustomText );
			m_textArray.RemoveAt( i );
			i --;
		}
	}

	for( i = 0; i < m_VendortextArray.GetSize(); i++ )
	{
		LPCUSTOMTEXT lpCustomText 
			= (LPCUSTOMTEXT) m_VendortextArray.GetAt( i );
		if( !IsValidObj( lpCustomText->m_pObj ) )
		{
			safe_delete( lpCustomText );
			m_VendortextArray.RemoveAt( i );
			i --;
		}
	}
	
}
//#define DLGMSG_ALPHA 180
#if 0
void CDialogMsg::Render( C2DRender* p2DRender )
{
	CSize size;	
	LPCUSTOMTEXT lpCustomText;
	for( int i = 0; i < m_textArray.GetSize(); i++ )
	{
		lpCustomText = (LPCUSTOMTEXT) m_textArray.GetAt( i );
		if( lpCustomText->m_timer.TimeOut() )
		{
			// 퀘스트 이모티콘을 다시 보이게 한다.
			if( lpCustomText->m_pObj->GetType() == OT_MOVER )
				((CMover*)lpCustomText->m_pObj)->m_bShowQuestEmoticon = TRUE;
			safe_delete( lpCustomText );
			m_textArray.RemoveAt( i );
			i --;
		}
		else
		{
			LPCTSTR lpStr = lpCustomText->m_string;
			lpCustomText->m_pFont->GetTextExtent( (TCHAR*)lpStr, &size );
			CObj* pObj = lpCustomText->m_pObj;

			if( pObj->IsCull() == FALSE )
			{
				int nAlpha = 200;
				if( lpCustomText->m_timer.GetLeftTime() > 4000 )
					nAlpha = 200 - ( ( lpCustomText->m_timer.GetLeftTime() - 4000 )* 200 / 1000 );
				LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

				// 월드 좌표를 스크린 좌표로 프로젝션 한다.
				D3DXVECTOR3 vOut, vPos = pObj->GetPos(), vPosHeight;
				D3DVIEWPORT9 vp;
				const BOUND_BOX* pBB = pObj->m_pModel->GetBBVector();

				pd3dDevice->GetViewport( &vp );

				D3DXMATRIX matTrans;
				D3DXMATRIX matWorld;
				D3DXMatrixIdentity(&matWorld);
				D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y , vPos.z);

				D3DXMatrixMultiply( &matWorld, &matWorld, &pObj->GetMatrixScale() );
				D3DXMatrixMultiply( &matWorld, &matWorld, &pObj->GetMatrixRotation() );
				D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );

				vPosHeight = pBB->m_vPos[0];
				vPosHeight.x = 0;
				vPosHeight.z = 0;
				
				D3DXVec3Project( &vOut, &vPosHeight, &vp, &pObj->GetWorld()->m_matProj,
					&pObj->GetWorld()->m_pCamera->m_matView, &matWorld);
			
				CRect rect = lpCustomText->m_rect;
				vOut.x -= rect.Width() / 2;
				vOut.y -= rect.Height();
				CRectClip rectClip = p2DRender->m_clipRect;
				DWORD dwLineCount = lpCustomText->m_string.GetLineCount();
				DWORD dwMaxHeight = lpCustomText->m_pFont->GetMaxHeight();
				CPoint ptOrigin = p2DRender->GetViewportOrg();
				p2DRender->SetViewportOrg( 0, 0 );
				int x = vOut.x;
				int y = vOut.y;
				int nHeight = rect.Height() / 8;
				int nWidth  = rect.Width()  / 8;

				for( int i = 0; i < nHeight; i++)
				{
					int nIndex;
					if( i == 0 ) 
						nIndex = 0; 
					else if( i != nHeight - 1 ) 
						nIndex = 3;
					else 
						nIndex = 6;
					for( int j = 0; j < nWidth; j++)
					{
						CPoint point = CPoint( x + j * 8, y + i * 8);
						if( j == 0 )
							m_texPack.Render( p2DRender, point, nIndex, nAlpha ); 
						else
						if( j != nWidth - 1 )
						{
							// 맨 밑쪽 
							/*
							if( i == nHeight - 1 ) 
							{
								if( nWidth >= 6 && ( j == 3 * nWidth / 5 || j == 3 * nWidth / 5 + 1) )
								{ m_texPack.Render( p2DRender, point, 4); continue; } // 5 : 3 = 10 : 6 
								else
								if( nWidth == 5 && ( j == 2 || j == 3 ) )
								{ m_texPack.Render( p2DRender, point, 4); continue; }
								else
								if( nWidth == 4 && j == 2 )
								{ m_texPack.Render( p2DRender, point, 4); continue; }
								else
								if( nWidth == 3 && j == 1 )
								{ m_texPack.Render( p2DRender, point, 4); continue; }
							}*/
							m_texPack.Render( p2DRender, point, nIndex + 1, nAlpha); 
						}
						else
							m_texPack.Render( p2DRender, point, nIndex + 2, nAlpha); 
					}
				}
				// 꼬랑지 출력 
				if( nWidth >= 6 )
					m_texPack.Render( p2DRender, CPoint( x + ( 3 * nWidth / 5 ) * 8, y + i * 8 - 1),  9, nAlpha ); 
				else
				if( nWidth == 5 ) 
					m_texPack.Render( p2DRender, CPoint( x + 2 * 8, y + i * 8 - 1), 9, nAlpha ); 
				else
				if( nWidth == 4 ) 
					m_texPack.Render( p2DRender, CPoint( x + 2 * 8, y + i * 8 - 1),  11, nAlpha ); 
				else
				if( nWidth == 3 ) 
					m_texPack.Render( p2DRender, CPoint( x + 1 * 8, y + i * 8 - 1),  11 , nAlpha); 
				p2DRender->SetViewportOrg( ptOrigin );

				x = vOut.x + 8;
				y = vOut.y + 8;
				p2DRender->TextOut_EditString( x, y, lpCustomText->m_string, 0, 0, 0 );
				/*
				for( i = 0; i < dwLineCount; i++)
				{
					CString string = lpCustomText->m_string.GetLine( i );
					DWORD dwOffset = lpCustomText->m_string.GetLineOffset( i );
					LPCTSTR lpszStr = string;
					int nLength = string.GetLength();
					DWORD dwCurOffset;
					TCHAR strHan[ 3 ];
					x = vOut.x + 8;;
					for( int j = 0; j < nLength; j++)
					{
						if( IsHangul( string[j] ) )
						{
							strHan[0] = string[j++];
							strHan[1] = string[j];
							strHan[2] = '\0';
							dwCurOffset = dwOffset + (j - 1);
						}
						else
						{
							strHan[0] = string[j];
							strHan[1] = '\0';
							dwCurOffset = dwOffset + j;
						}
						CSize size = lpCustomText->m_pFont->GetTextExtent(strHan);
						DWORD dwColor = lpCustomText->m_string.m_adwColor[dwCurOffset];
						DWORD dwStyle = lpCustomText->m_string.m_abyStyle[dwCurOffset];
						
						//p2DRender->TextOut( dwBegin, 0 + y * dwMaxHeight, strHan, dwColor);
						lpCustomText->m_pFont->DrawText( x, y, dwColor, (TCHAR*) strHan );
						if( dwStyle & ESSTY_BOLD )
							lpCustomText->m_pFont->DrawText( x + 1, y, dwColor, (TCHAR*) strHan );
						if( dwStyle & ESSTY_UNDERLINE )
							p2DRender->RenderLine( CPoint( x, y + size.cy ), CPoint( x + size.cx, y + size.cy ), dwColor );
							
						x+= size.cx;
					}	
					//lpCustomText->m_pFont->DrawText( x, y, D3DCOLOR_ARGB( nAlpha, 0, 0, 0 ), (TCHAR*) lpszStr );
					y += dwMaxHeight;
				}
				*/
			}
		}
	}
}
#else
void CDialogMsg::Render( C2DRender* p2DRender )
{
	CSize size;	
	LPCUSTOMTEXT lpCustomText;

//	CD3DFont* pOldFont = p2DRender->GetFont();
//	p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
	
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
	
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );
	
	TEXTUREVERTEX vertex[ 4 * 18 ];
	CPoint point;
	int nIndex;
		
	for( int i = 0; i < m_textArray.GetSize(); i++ )
	{
		lpCustomText = (LPCUSTOMTEXT) m_textArray.GetAt( i );
		TEXTUREVERTEX* pVertices = vertex; 
		if( !lpCustomText->m_bInfinite && lpCustomText->m_timer.TimeOut() )
		{
			// 퀘스트 이모티콘을 다시 보이게 한다.
			if( lpCustomText->m_pObj->GetType() == OT_MOVER )
				((CMover*)lpCustomText->m_pObj)->m_bShowQuestEmoticon = TRUE;
			safe_delete( lpCustomText );
			m_textArray.RemoveAt( i );
			i --;
		}
		else
		{
			CObj* pObj = lpCustomText->m_pObj;
			if( lpCustomText->m_pTexture )
			{
				if( pObj->IsCull() == FALSE )
				{
					// 월드 좌표를 스크린 좌표로 프로젝션 한다.
					D3DXVECTOR3 vOut, vPos = pObj->GetPos(), vPosHeight;
					D3DVIEWPORT9 vp;
					const BOUND_BOX* pBB;
					
					if( pObj->m_pModel )
						pBB	= pObj->m_pModel->GetBBVector();
					else
						return;

					pd3dDevice->GetViewport( &vp );

					D3DXMATRIX matTrans;
					D3DXMATRIX matWorld;
					D3DXMatrixIdentity(&matWorld);
					D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y , vPos.z);

					D3DXMatrixMultiply( &matWorld, &matWorld, &pObj->GetMatrixScale() );
					D3DXMatrixMultiply( &matWorld, &matWorld, &pObj->GetMatrixRotation() );
					D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );

					vPosHeight = pBB->m_vPos[0];
					vPosHeight.x = 0;
					vPosHeight.z = 0;
					
					D3DXVec3Project( &vOut, &vPosHeight, &vp, &pObj->GetWorld()->m_matProj,
						&pObj->GetWorld()->m_pCamera->m_matView, &matWorld);

					CPoint point;
					point.x = (LONG)( vOut.x - 32 / 2 );
					point.y = (LONG)( vOut.y - 32 );
					
					MakeEven( point.x );			
	
					p2DRender->RenderTexture( point, lpCustomText->m_pTexture );
				}
			}
			else
			{
			LPCTSTR lpStr = lpCustomText->m_string;
			lpCustomText->m_pFont->GetTextExtent( (TCHAR*)lpStr, &size );
			if( pObj->IsCull() == FALSE )
			{
				int nAlpha = 200;
				if( !lpCustomText->m_bInfinite && lpCustomText->m_timer.GetLeftTime() > 4000 )
					nAlpha = (int)( 200 - ( ( lpCustomText->m_timer.GetLeftTime() - 4000 )* 200 / 1000 ) );
	
				// 월드 좌표를 스크린 좌표로 프로젝션 한다.
				D3DXVECTOR3 vOut, vPos = pObj->GetPos(), vPosHeight;
				D3DVIEWPORT9 vp;
				const BOUND_BOX* pBB;
				
				if( pObj->m_pModel )
					pBB	= pObj->m_pModel->GetBBVector();
				else
					return;

				pd3dDevice->GetViewport( &vp );

				D3DXMATRIX matTrans;
				D3DXMATRIX matWorld;
				D3DXMatrixIdentity(&matWorld);
				D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y , vPos.z);

				D3DXMatrixMultiply( &matWorld, &matWorld, &pObj->GetMatrixScale() );
				D3DXMatrixMultiply( &matWorld, &matWorld, &pObj->GetMatrixRotation() );
				D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );

				vPosHeight = pBB->m_vPos[0];
				vPosHeight.x = 0;
				vPosHeight.z = 0;
				
				D3DXVec3Project( &vOut, &vPosHeight, &vp, &pObj->GetWorld()->m_matProj,
					&pObj->GetWorld()->m_pCamera->m_matView, &matWorld);
			
				CRect rect = lpCustomText->m_rect;
				vOut.x -= rect.Width() / 2;
				vOut.y -= rect.Height();
				CPoint ptOrigin = p2DRender->GetViewportOrg();

				vOut.y *= 0.9f;

				FLOAT x = vOut.x;
				FLOAT y = vOut.y;

				int nHeight = rect.Height() / 8;
				int nWidth  = rect.Width()  / 8;
				CTexture* pTexture;

				FLOAT fEdge  = 8.0f;
				FLOAT fWidth = ( nWidth <= 5 ) ? rect.Width() * 0.7f : rect.Width() * 0.9f;
				FLOAT fHeight = rect.Height() * 0.75f;

				FLOAT fOffsetX = 0;
				FLOAT fOffsetY = 0;
				
				nIndex = 0;

				int i;
				

				if( lpCustomText->m_nKind == CHAT_SHOUT )
				{
					char buffer[256] = { 0 };
					
					// 텍스쳐렌더 버그 관련 수정
					CTexture* pShoutTex = NULL;

					int nlen = _tcsclen(lpStr);

					if( nlen >= 1 && nlen <= 1 )
						strcpy( buffer, "texDialogBoxShout00.bmp" );
					else
						if( nlen >= 2 && nlen <= 3 )
							strcpy( buffer, "texDialogBoxShout01.bmp" );
						else
							if( nlen >= 4 && nlen <= 5 )
								strcpy( buffer, "texDialogBoxShout02.bmp" );
							else
								if( nlen >= 6 && nlen <= 9 )
									strcpy( buffer, "texDialogBoxShout03.bmp" );
								else
									if( nlen >= 10 && nlen <= 20 )
										strcpy( buffer, "texDialogBoxShout04.bmp" );
									else
										if( nlen >= 21 && nlen <= 40 )
											strcpy( buffer, "texDialogBoxShout05.bmp" );
										else
											if( nlen >= 41 && nlen <= 60 )
												strcpy( buffer, "texDialogBoxShout06.bmp" );
											else
												if( nlen >= 61 )
													strcpy( buffer, "texDialogBoxShout07.bmp" );

					pShoutTex = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, buffer ), 0xffff00ff );
					if( pShoutTex )
					{
						p2DRender->RenderTextureEx( CPoint( (int)( x-20 ), (int)( y-20 ) ), CPoint( (int)( fWidth+50.0f ), (int)( fHeight+40.0f ) ), pShoutTex, nAlpha, 1.0f, 1.0f, FALSE );
						goto g_ShoutChat;
					}
				}


				for( i=0; i<3; i++ )
				{
					if( i==0 || i==2 )
						fOffsetY=fEdge;
					else
						fOffsetY=fHeight;
					
					fOffsetX = 0;

					for( int j=0; j<3; j++ )
					{
						if( j==0 || j==2 )
							fOffsetX = fEdge;
						else
							fOffsetX = fWidth;

						pTexture = m_texPack_ex[lpCustomText->m_nKind].GetAt( nIndex );

						SetTextureVertex( pVertices, x, y, pTexture->m_fuLT, pTexture->m_fvLT );
						pVertices++;
						SetTextureVertex( pVertices, x+fOffsetX, y, pTexture->m_fuRT, pTexture->m_fvRT );
						pVertices++;
						SetTextureVertex( pVertices, x, y+fOffsetY, pTexture->m_fuLB, pTexture->m_fvLB );
						pVertices++;
						SetTextureVertex( pVertices, x, y+fOffsetY, pTexture->m_fuLB, pTexture->m_fvLB );
						pVertices++;
						SetTextureVertex( pVertices, x+fOffsetX, y, pTexture->m_fuRT, pTexture->m_fvRT );
						pVertices++;
						SetTextureVertex( pVertices, x+fOffsetX, y+fOffsetY, pTexture->m_fuRB, pTexture->m_fvRB );
						pVertices++;

						x+=fOffsetX;			

						nIndex++;
					}

					x = vOut.x;
					
					y+=fOffsetY;

				}

				// 꼬랑지 출력 
				if( nWidth >= 6 )
				{
					point =  CPoint( (int)( x + ( 3 * nWidth / 5 ) * 8 ), (int)( y ) );
					nIndex = 9;
				}
				else
				if( nWidth == 4 || nWidth == 5 ) 
				{
					point =  CPoint( (int)( x + 3 * 8 ), (int)( y ) );
					nIndex = 9;
				}
				else
				if( nWidth == 3 ) 
				{
					point =  CPoint( (int)( x + 1 * 8 ), (int)( y ) );
					nIndex = 9;
				}
				pTexture = m_texPack_ex[lpCustomText->m_nKind].GetAt( nIndex );

				SetTextureVertex( pVertices, (FLOAT)( point.x ), (FLOAT)( point.y ), pTexture->m_fuLT, pTexture->m_fvLT );
				pVertices++;
				SetTextureVertex( pVertices, (FLOAT)( point.x + 8.0f ), (FLOAT)( point.y ), pTexture->m_fuRT, pTexture->m_fvRT);
				pVertices++;
				SetTextureVertex( pVertices, (FLOAT)( point.x ), (FLOAT)( point.y + 8.0f ), pTexture->m_fuLB, pTexture->m_fvLB);
				pVertices++;
				
				SetTextureVertex( pVertices, (FLOAT)( point.x + 8.0f ), (FLOAT)( point.y ), pTexture->m_fuRT, pTexture->m_fvRT);
				pVertices++;
				SetTextureVertex( pVertices, (FLOAT)( point.x ), (FLOAT)( point.y + 8.0f ), pTexture->m_fuLB, pTexture->m_fvLB);
				pVertices++;
				SetTextureVertex( pVertices, (FLOAT)( point.x + 8.0f ), (FLOAT)( point.y + 8.0f ), pTexture->m_fuRB, pTexture->m_fvRB);
				pVertices++;

				pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( nAlpha, 0, 0, 0 ) );
				pd3dDevice->SetVertexShader( NULL );

				pd3dDevice->SetTexture( 0, m_texPack_ex[lpCustomText->m_nKind].GetAt( 0 )->m_pTexture );
				
				pd3dDevice->SetFVF( D3DFVF_TEXTUREVERTEX );
				pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 20, vertex, sizeof( TEXTUREVERTEX ) );

g_ShoutChat:

				/*
				CPoint point;
				point.x = vOut.x + 8;;		
				point.y = vOut.y + 8;;		
				*/

				x = vOut.x + 8;
				y = vOut.y + 8;
				x -= ptOrigin.x;
				y -= ptOrigin.y;
				/*
				point.x -= ptOrigin.x;
				MakeEven( point.x );
				point.y -= ptOrigin.y;
				*/
				
				lpCustomText->m_string.SetAlpha( nAlpha );
//				p2DRender->TextOut_EditString( point.x, point.y, lpCustomText->m_string, 0, 0, 0 );
				p2DRender->TextOut_EditString( (int)( x ), (int)( y ), lpCustomText->m_string, 0, 0, 0 );
			}
			}
		}
	}
	
	for( i = 0; i < m_VendortextArray.GetSize(); i++ )
	{
		lpCustomText = (LPCUSTOMTEXT) m_VendortextArray.GetAt( i );
		TEXTUREVERTEX* pVertices = vertex; 

		LPCTSTR lpStr = lpCustomText->m_string;
		lpCustomText->m_pFont->GetTextExtent( (TCHAR*)lpStr, &size );
		CObj* pObj = lpCustomText->m_pObj;
		if( pObj->IsCull() == FALSE )
		{
			int nAlpha = 200;

			// 월드 좌표를 스크린 좌표로 프로젝션 한다.
			D3DXVECTOR3 vOut, vPos = pObj->GetPos(), vPosHeight;
			D3DVIEWPORT9 vp;
			const BOUND_BOX* pBB;
			
			if( pObj->m_pModel )
				pBB	= pObj->m_pModel->GetBBVector();
			else
				return;

			pd3dDevice->GetViewport( &vp );

			D3DXMATRIX matTrans;
			D3DXMATRIX matWorld;
			D3DXMatrixIdentity(&matWorld);
			D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y , vPos.z);

			D3DXMatrixMultiply( &matWorld, &matWorld, &pObj->GetMatrixScale() );
			D3DXMatrixMultiply( &matWorld, &matWorld, &pObj->GetMatrixRotation() );
			D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );

			vPosHeight = pBB->m_vPos[0];
			vPosHeight.x = 0;
			vPosHeight.z = 0;
			
			D3DXVec3Project( &vOut, &vPosHeight, &vp, &pObj->GetWorld()->m_matProj,
				&pObj->GetWorld()->m_pCamera->m_matView, &matWorld);
		
			CRect rect = lpCustomText->m_rect;
			vOut.x -= rect.Width() / 2;
			vOut.y -= rect.Height();
			DWORD dwMaxHeight = lpCustomText->m_pFont->GetMaxHeight();
			CPoint ptOrigin = p2DRender->GetViewportOrg();

			int x = (int)( vOut.x );
			int y = (int)( vOut.y );
			int nHeight = rect.Height() / 8;
			int nWidth  = rect.Width()  / 8;

			FLOAT fGap    = 40;
			FLOAT fWidth  = (FLOAT)( rect.Width() );
			FLOAT fHeight = rect.Height()*0.8f;

			fGap += 2.0f;

			CPoint ptTex1;
			CPoint ptTex2;
			
			ptTex1.x = (LONG)( vOut.x-24 );
			ptTex1.y = (LONG)( vOut.y-fGap );
			
			p2DRender->RenderTexture(ptTex1, m_pTex[0], nAlpha, 1.0f, 1.0f );

			ptTex1.x = (LONG)( vOut.x+8 );
			ptTex1.y = (LONG)( vOut.y-fGap );
				
			ptTex2.x = (int)fWidth;
			ptTex2.y = 32;
			p2DRender->RenderTextureEx( ptTex1, ptTex2, m_pTex[1], nAlpha, 1.0f, 1.0f );

			ptTex1.x = (LONG)( (int)(vOut.x+8)+ptTex2.x );
			ptTex1.y = (LONG)( vOut.y-fGap );
				
			p2DRender->RenderTexture(ptTex1, m_pTex[2], nAlpha, 1.0f, 1.0f );

			fGap -= 2.0f;
			x = (int)( vOut.x + 8 );
			y = (int)( vOut.y + 8 );
			x -= ptOrigin.x;
			y -= ptOrigin.y;
			lpCustomText->m_string.SetAlpha( 250 );
			p2DRender->TextOut_EditString( x, (int)( y-fGap ), lpCustomText->m_string, 0, 0, 0 );
		}
	}
}
#endif
void CDialogMsg::SetLinePos(C2DRender* p2DRender,int nLinePos)
{
//	MakeImageString(p2DRender);
}
void CDialogMsg::SetRectPage(C2DRender* p2DRender,CRect rect)
{
	ClearAllMessage();
	m_rectPage = rect;
	//SetSize(rect.Height() / ( p2DRender->GetFontManager()->GetHanFontSize().cy + m_nLineSpace));
//	MakeImageString(p2DRender);
}
void CDialogMsg::SetLineNum(C2DRender* p2DRender,int nLineNum)
{
	ClearAllMessage();
	//rectPage = rect;
	SetSize(nLineNum);
//	MakeImageString(p2DRender);
}
void CDialogMsg::Scroll()
{
	if(m_nAddLineNum == 0)
		return;
	LPCUSTOMTEXT lpCustomText = (LPCUSTOMTEXT)m_textArray.GetAt(0);
	if(lpCustomText)
	{
		safe_delete( lpCustomText );
		m_textArray.SetAt(0,NULL);
	}
	m_nAddLineNum--;
	for(int i = 0; i < m_nAddLineNum; i++)
		m_textArray.SetAt(i,m_textArray.GetAt(i+1));

	/*
	Image* pImage = (Image*)GetAt(0);
	if(pImage)
	{
		delete pImage->m_pDibBitmap;
		delete pImage;
		SetAt(0,NULL);
	}
	m_nAddLineNum--;
	for(int i = 0; i < m_nAddLineNum; i++)
		SetAt(i,GetAt(i+1));
		*/
}

#define		MAX_CLIENTMSG_LEN		100

void CDialogMsg::RemoveMessage( CObj* pObj )
{
	for( int i = 0; i < m_textArray.GetSize(); i++ )
	{
		LPCUSTOMTEXT lpCustomText = (LPCUSTOMTEXT) m_textArray.GetAt( i );
		if( lpCustomText->m_pObj == pObj )
		{
			safe_delete( lpCustomText );
			m_textArray.RemoveAt( i );
			return;
		}
	}
}

void CDialogMsg::AddEmoticonUser( CObj* pObj, int nEmoticonIdx )
{
	if( nEmoticonIdx < (int)( m_texEmoticonUser.GetNumber() ) )
		AddTexture( pObj, m_texEmoticonUser.GetAt( nEmoticonIdx ) );
}

void CDialogMsg::AddEmoticon( CObj* pObj, int nEmoticonIdx )
{
	if( nEmoticonIdx < (int)( m_texEmoticon.GetNumber() ) )
		AddTexture( pObj, m_texEmoticon.GetAt( nEmoticonIdx ) );
}
void CDialogMsg::AddTexture( CObj* pObj, CTexture* pTexture )
{
	for( int i = 0; i < m_textArray.GetSize(); i++ )
	{
		LPCUSTOMTEXT lpCustomText = (LPCUSTOMTEXT) m_textArray.GetAt( i );
		if( lpCustomText->m_pObj == pObj )
		{
			safe_delete( lpCustomText );
			m_textArray.RemoveAt( i );
			break;
		}
	}
	LPCUSTOMTEXT lpCustomText = new CUSTOMTEXT;
	lpCustomText->m_dwRGB = 0;
	lpCustomText->m_pFont = CWndBase::m_Theme.m_pFontText; //ect ? m_pFontEffect : m_pFont;
	lpCustomText->m_pObj = pObj;
	lpCustomText->m_timer.Set( 5000 );
	lpCustomText->m_bInfinite	= FALSE;	//( pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->m_vtInfo.IsVendorOpen() );
	lpCustomText->m_pTexture = pTexture;
	m_textArray.Add( lpCustomText );
}

void CDialogMsg::AddMessage( CObj* pObj, LPCTSTR lpszMessage, DWORD RGB, int nKind, DWORD dwPStyle )
{
	if( pObj->GetType() == OT_MOVER )
	{
		CScanner scanner;
		scanner.SetProg( (LPVOID)lpszMessage );
		scanner.GetToken();

		// 이모티콘 명령
/*		
		if( scanner.Token == "/" )
		{			
			scanner.GetToken();

			CString strstr = scanner.token;
			for( int j=0; j < MAX_EMOTICON_NUM; j++ )
			{
				CString strstr2 = m_EmiticonCmd[ j ].m_szCommand;

				if( strstr == strstr2 )
				{
					AddEmoticonUser( pObj, m_EmiticonCmd[ j ].m_dwIndex );
					return;
				}
			}
		}
*/
		if( scanner.Token == "/" )
		{			
			CString strstr = lpszMessage;
			scanner.GetToken();
			for( int j=0; j < MAX_EMOTICON_NUM; j++ )
			{
				CString strstr2 = "/";
				strstr2+=m_EmiticonCmd[ j ].m_szCommand;
				if( strstr.GetLength() > 0 && strstr == strstr2 )
				{
					AddEmoticonUser( pObj, m_EmiticonCmd[ j ].m_dwIndex );
					return;
				}
			}
		}		
		else
		if( scanner.Token == "!" )
		{
			int nEmoticonIdx = scanner.GetNumber();
			AddEmoticon( pObj, nEmoticonIdx );
			return;
		}
		
		for( int i = 0; i < m_textArray.GetSize(); i++ )
		{
			LPCUSTOMTEXT lpCustomText = (LPCUSTOMTEXT) m_textArray.GetAt( i );
			if( lpCustomText->m_pObj == pObj )
			{
				safe_delete( lpCustomText );
				m_textArray.RemoveAt( i );
				break;
			}
		}
	}
	LPCUSTOMTEXT lpCustomText = new CUSTOMTEXT;
	lpCustomText->m_dwRGB = RGB;
	lpCustomText->m_pFont = CWndBase::m_Theme.m_pFontText; //ect ? m_pFontEffect : m_pFont;
	lpCustomText->m_pObj = pObj;
	lpCustomText->m_timer.Set( 5000 );
	lpCustomText->m_bInfinite	= FALSE;//( pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->m_vtInfo.IsVendorOpen() );
	lpCustomText->m_pTexture = NULL;
	lpCustomText->m_nKind = nKind;

	// Mover라면 대사 출력 시간 동안 퀘스트 이모티콘을 감춘다.
	if( pObj->GetType() == OT_MOVER )
		((CMover*)pObj)->m_bShowQuestEmoticon = FALSE;

	if( lpCustomText->m_bInfinite )
	{
		DWORD dwColor = 0;
		dwColor = 0xff008000;
		lpCustomText->m_string.SetParsingString( lpszMessage, dwColor, 0x00000000, 0, dwPStyle );
	}
	else
		lpCustomText->m_string.SetParsingString( lpszMessage, 0xff000000, 0x00000000, 0, dwPStyle );

	int nMaxHeight = lpCustomText->m_pFont->GetMaxHeight();
	CSize size = lpCustomText->m_pFont->GetTextExtent( lpszMessage );
	int cx, cy;
	// 기본 사이즈 계산 
	if( size.cx >= 160 )
	{
		cx = 160;
		cy = 160; // 이 수치는 의미가 없음. Reset 이후에 줄수로 재 계산 됨
		
		lpCustomText->m_string.Init( lpCustomText->m_pFont, &CRect( 0, 0, cx - 16, cy - 16) );
	}
	else
	{
		cx = size.cx + 16; 
		cx = ( ( cx / 16 ) * 16 ) + ( ( cx % 16 ) ? 16 : 0 );
		cy = size.cy;// 이 수치는 의미가 없음. Reset 이후에 줄수로 재 계산 됨 
		lpCustomText->m_string.Init( lpCustomText->m_pFont, &CRect( 0, 0, cx - 16, cy - 16) );
	}


	cy = lpCustomText->m_string.GetLineCount() * nMaxHeight + 16; // 라인 줄수로 세로 길이를 구함 
	cy = ( ( cy / 16 ) * 16 ) + ( ( cy % 16 ) ? 16 : 0 );

	lpCustomText->m_rect = CRect( 0, 0, cx, cy );
	m_textArray.Add( lpCustomText );
}

void CDialogMsg::ClearVendorObjMsg()
{
	for(int i = 0; i < m_VendortextArray.GetSize(); i++)
		safe_delete( (LPCUSTOMTEXT)m_VendortextArray.GetAt(i) );
	m_VendortextArray.RemoveAll();
}

void CDialogMsg::AddVendorMessage(CObj *pObj, LPCTSTR lpszMessage, DWORD RGB)
{
	// 줄바꿈 스페이스 표기로 바꿔주기
	CString tempstr;
	tempstr = lpszMessage;
	for(int j=0; j<tempstr.GetLength(); j++)
	{
		if(tempstr.GetAt(j) == '\n')
		{
			tempstr.SetAt(j, ' ');
		}
	}
	lpszMessage = tempstr.GetBuffer(0);

	for( int i = 0; i < m_VendortextArray.GetSize(); i++ )
	{
		LPCUSTOMTEXT lpCustomText = (LPCUSTOMTEXT) m_VendortextArray.GetAt( i );
		if( lpCustomText->m_pObj == pObj )
		{
			safe_delete( lpCustomText );
			m_VendortextArray.RemoveAt( i );
			break;
		}
	}
	
	LPCUSTOMTEXT lpCustomText = new CUSTOMTEXT;
	lpCustomText->m_dwRGB = RGB;
	lpCustomText->m_pFont = CWndBase::m_Theme.m_pFontText; //ect ? m_pFontEffect : m_pFont;
	lpCustomText->m_pObj = pObj;
	lpCustomText->m_timer.Set( 5000 );
	lpCustomText->m_bInfinite	= TRUE;	//( pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->m_vtInfo.IsVendorOpen() );
	lpCustomText->m_pTexture = NULL;
	
	int nMaxHeight = lpCustomText->m_pFont->GetMaxHeight();
	CSize size = lpCustomText->m_pFont->GetTextExtent( lpszMessage );
	int cx, cy;
	// 기본 사이즈 계산 
	/*
	if( size.cx >= 160 )
	{
		cx = 160;
		cy = 160; // 이 수치는 의미가 없음. Reset 이후에 줄수로 재 계산 됨 
		lpCustomText->m_string.Init( lpCustomText->m_pFont, &CRect( 0, 0, cx - 16, cy - 16) );
	}
	else
	*/
	{
		cx = size.cx + 16; 
		cx = ( ( cx / 16 ) * 16 ) + ( ( cx % 16 ) ? 16 : 0 );
		cy = size.cy;// 이 수치는 의미가 없음. Reset 이후에 줄수로 재 계산 됨 
		lpCustomText->m_string.Init( lpCustomText->m_pFont, &CRect( 0, 0, cx - 16, cy - 16) );
	}
	if( lpCustomText->m_bInfinite )
	{
		DWORD dwColor = 0;

		dwColor = ( rand()%2 ) ? 0xffff0000: 0xFFFFDD66;//0xff00ffff;

		lpCustomText->m_string.SetParsingString( lpszMessage, dwColor );
	}
	else
		lpCustomText->m_string.SetParsingString( lpszMessage );
	
	cy = lpCustomText->m_string.GetLineCount() * nMaxHeight + 16; // 라인 줄수로 세로 길이를 구함 
	cy = ( ( cy / 16 ) * 16 ) + ( ( cy % 16 ) ? 16 : 0 );
	
	lpCustomText->m_rect = CRect( 0, 0, cx, cy );
	m_VendortextArray.Add( lpCustomText );
}

void CDialogMsg::RemoveVendorMessage(CObj *pObj)
{
	for( int i = 0; i < m_VendortextArray.GetSize(); i++ )
	{
		LPCUSTOMTEXT lpCustomText = (LPCUSTOMTEXT) m_VendortextArray.GetAt( i );
		if( lpCustomText->m_pObj == pObj )
		{
			safe_delete( lpCustomText );
			m_VendortextArray.RemoveAt( i );
			return;
		}
	}	
}

bool CDialogMsg::LoadEmotion()
{
	CScript script;
	if( script.Load( "textEmotion.txt" ) == FALSE )
		return FALSE;

	memset( m_EmiticonCmd, 0, sizeof(EmticonCmd)*MAX_EMOTICON_NUM );
	
	int i = 0;
	script.GetToken();

	while( script.tok != FINISHED )
	{
		if( i>=MAX_EMOTICON_NUM )
		{
			Error( "textEmotion.txt 갯수가 맞지 않음" );
			return FALSE;
		}

		m_EmiticonCmd[ i ].m_dwIndex = atoi( script.token );
		script.GetToken();
		_tcscpy( m_EmiticonCmd[ i ].m_szCommand, script.token );

		script.GetToken();
		i++;	
	}

	return TRUE;
}
