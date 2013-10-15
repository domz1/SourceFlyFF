#include "stdafx.h"
#include "ModelMng.h"


static TCHAR g_szRoot[ MAX_OBJTYPE ][32] = { 
	"obj", 
	"ani", 
	"ctrl", 
	"sfx", 
	"item", 
	"mvr", 
	"region",
	"obj",		// ship
	"path"
};

CModelMng::CModelMng()
{
}
CModelMng::~CModelMng()
{
	for( int i = 0; i < MAX_OBJTYPE ; i++ )
	{
		CFixedArray< tagMODELELEM >* apModelElem = &m_aaModelElem[ i ];
		for( int j = 0; j < apModelElem->GetSize(); j++ )
		{
			LPMODELELEM pModelElem = (LPMODELELEM) apModelElem->GetAt( j );
			if( pModelElem )
				SAFE_DELETE_ARRAY( pModelElem->m_apszMotion );
		}
	}

	Free();
}
void CModelMng::Free()
{
}
LPMODELELEM CModelMng::GetModelElem( DWORD dwType, DWORD dwIndex )
{
	LPMODELELEM pElem = m_aaModelElem[ dwType ].GetAt( dwIndex );
	if( pElem == NULL )
	{
		if( dwType == OT_ITEM )	// 아이템인 경우, 돈 모델을 사용   
//			return m_aaModelElem[ OT_ITEM ].GetAt( II_GOLD_SEED1 );
			return m_aaModelElem[ OT_ITEM ].GetAt( II_ARM_M_VAG_HELMET02 );	// 일반 방어구 모양으로 교체
		else
		{
			LPCTSTR szErr = Error( "GetModelElem - out of range: type=%d, size=%d, index=%d", (int)dwType, m_aaModelElem[ dwType ].GetSize(), (int)dwIndex );
			ADDERRORMSG( szErr );
		}
	}
	
	return pElem;
}
void CModelMng::MakeBoneName( TCHAR* pszModelName, DWORD dwType, DWORD dwIndex )
{
	LPMODELELEM lpModelElem = GetModelElem( dwType, dwIndex );
	_tcscpy( pszModelName, g_szRoot[dwType]);
	_tcscat( pszModelName, "_" );
	_tcscat( pszModelName, lpModelElem->m_szName );
	_tcscat( pszModelName, _T(".chr") );
}
void CModelMng::MakeModelName( TCHAR* pszModelName, DWORD dwType, DWORD dwIndex )
{
	LPMODELELEM lpModelElem = GetModelElem( dwType, dwIndex );
	if( lpModelElem == NULL )
	{
		Error( "MakeModelName : dwType=%d dwIndex=%d", dwType, dwIndex );
	}
	if( lpModelElem->m_dwModelType == MODELTYPE_BILLBOARD )
	{
		_tcscpy( pszModelName, lpModelElem->m_szName );
		return;
	}
	if( dwType == OT_SFX )
	{
		if( _tcschr( lpModelElem->m_szName, _T( '_' ) ) == NULL )
		{
			_tcscpy( pszModelName, g_szRoot[ dwType ] );
			_tcscat( pszModelName, "_" );
			_tcscat( pszModelName, lpModelElem->m_szName );
		}
		else
			_tcscpy( pszModelName, lpModelElem->m_szName );
	}
	else
	{
		_tcscpy( pszModelName, g_szRoot[ dwType ] );
		_tcscat( pszModelName, "_" );
		_tcscat( pszModelName, lpModelElem->m_szName );
	}
	if( lpModelElem->m_dwModelType != MODELTYPE_SFX )
		_tcscat( pszModelName, _T( ".o3d" ) );
}
void CModelMng::MakePartsName( TCHAR* pszPartsName, LPCTSTR lpszRootName, DWORD dwIndex, int nSex )
{
	LPMODELELEM lpModelElem = GetModelElem( OT_ITEM, dwIndex );
	_tcscpy( pszPartsName, lpszRootName );
	_tcscat( pszPartsName, _T( "_" ) );
	if( nSex == SEX_SEXLESS || nSex == -1 )
		_tcscat( pszPartsName, lpModelElem->m_szPart );
	else
	{
		TCHAR* pszPart2 = strchr( lpModelElem->m_szPart, '/' );
		if( pszPart2 == NULL )
			_tcscat( pszPartsName, lpModelElem->m_szPart );
		else
		{
			TCHAR szPartName[ 128 ];
			if( nSex == SEX_MALE )
			{
				_tcscpy( szPartName, lpModelElem->m_szPart );
				szPartName[ (int)pszPart2 - (int)lpModelElem->m_szPart ] = 0;
				_tcscat( pszPartsName, szPartName );
			}
			else
			{
				_tcscat( pszPartsName, pszPart2 + 1 );
			}
		}
	}
	_tcscat( pszPartsName, _T( ".o3d" ) );
}
void CModelMng::MakeMotionName( TCHAR* pszMotionName, DWORD dwType, DWORD dwIndex, DWORD dwMotion )
{
	LPMODELELEM lpModelElem = GetModelElem( dwType, dwIndex );
	if( lpModelElem == NULL )
		Error( "MakeMotionName GetModelElem dwType:%d dwIndex:%d, dwMotion:%d", dwType, dwIndex, dwMotion );

	// 초과됐을 경우 MTI_STAND(정지상태)로 강제 세팅 
	if( (int)( dwMotion ) >= lpModelElem->m_nMax || dwMotion == NULL_ID )
	{
		dwMotion = MTI_STAND;
	}

	_tcscpy( pszMotionName, g_szRoot[ dwType ] );
	_tcscat( pszMotionName, "_" );

	LPCTSTR lpszMotion = lpModelElem->GetMotion( dwMotion ); 
	if( _tcschr( lpszMotion, '_' ) == NULL )
	{
		_tcscat( pszMotionName, lpModelElem->m_szName );
		_tcscat( pszMotionName, "_" );
	}

	// 공란일 경우 MTI_STAND(정지상태)로 강제 세팅 
	if( lpszMotion[0] == 0 )
	{
		dwMotion = MTI_STAND;
		lpszMotion = lpModelElem->GetMotion( dwMotion ); 
	}

	_tcscat( pszMotionName, lpszMotion );
	_tcscat( pszMotionName, ".ani" );
}
BOOL CModelMng::LoadMotion( CModel* pModel, DWORD dwType, DWORD dwIndex, DWORD dwMotion )
{
	if( dwType != OT_MOVER )
		return FALSE;
	TCHAR szMotionName[ MAX_PATH ];
	MakeMotionName( szMotionName, dwType, dwIndex, dwMotion );

	((CModelObject*)pModel)->LoadMotion( szMotionName );		// bone animation 읽음
	return TRUE;
}
CModel* CModelMng::LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, int nType, int nIndex, BOOL bParts )
{
	LPMODELELEM lpModelElem = GetModelElem( nType, nIndex );
	if( lpModelElem == NULL ) 
	{
		Error( "CModelMng::loadModel mdlObj/mdlDyna 에 objtype=%d index=%d bpart=%d 의 정보가 없군여.", nType, nIndex, bParts );
		return NULL;
	}

	TCHAR szFileName[ MAX_PATH ];
	MakeModelName( szFileName, nType, nIndex );
	return LoadModel( pd3dDevice, szFileName, lpModelElem, nType, bParts ); 
}
CModel* CModelMng::LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* lpszFileName, LPMODELELEM lpModelElem, int nType, BOOL bParts ) 
{
	HRESULT hr;
	CModel* pModel = NULL;
	CModel* pModelBill = NULL;
	int nModelType = lpModelElem->m_dwModelType;
	TCHAR szFileName[MAX_PATH];
	MapStrToPtrItor mapItor;

	switch( nModelType )
	{
		case MODELTYPE_SFX: 
		#ifndef __WORLDSERVER		// 새버전에선 월드에서 sfx를 new하지 않는다.
			pModel = new CSfxModel;		
			pModel->SetModelType( nModelType );
			((CSfxModel*)pModel)->SetSfx( lpszFileName );
			pModel->m_pModelElem = lpModelElem;
			pModel->m_pModelElem->m_bUsed = TRUE;
		#endif // not World
			break;

		case MODELTYPE_MESH: 
			mapItor = m_mapFileToMesh.find( lpszFileName );
			if( mapItor != m_mapFileToMesh.end() )
			{
				((CModel*)(*mapItor).second)->m_pModelElem->m_bUsed = TRUE;

#ifdef __CSC_EXTEXTURE
				pModel = (CModel*) (*mapItor).second;
				pModel->SetModelType( nModelType );
				pModel->m_pModelElem = lpModelElem;

				return pModel;
#else //__CSC_EXTEXTURE
				return (CModel*) (*mapItor).second;
#endif //__CSC_EXTEXTURE
			}
			pModel = new CModelObject;
			pModel->SetModelType( nModelType );
			pModel->m_pModelElem = lpModelElem;
			hr = pModel->InitDeviceObjects( pd3dDevice );
			hr = pModel->LoadModel( lpszFileName );
			if( hr == SUCCESS )
			{
				hr = pModel->RestoreDeviceObjects();
			#ifdef _DEBUG
				if( ((CModelObject*)pModel)->GetObject3D()->m_nHavePhysique )
					Error( "CModelMng::LoadModel : %s가 동적오브젝트인데 정적오브젝트로 설정되어 있다.", lpszFileName );
			#endif			
				m_mapFileToMesh.insert( MapStrToPtrType( lpszFileName, pModel ) );
				pModel->m_pModelElem->m_bUsed = TRUE;
			}
			else
				SAFE_DELETE( pModel )
			break;
		case MODELTYPE_ANIMATED_MESH:
			pModel = new CModelObject;
			pModel->SetModelType( nModelType );
			pModel->InitDeviceObjects( pd3dDevice );
			pModel->m_pModelElem = lpModelElem;
			pModel->m_pModelElem->m_bUsed = TRUE;
#if __VER >= 14 // __WING_ITEM
			memset( szFileName, 0, sizeof(szFileName) );
			_tcsncpy( szFileName, lpszFileName, _tcslen( lpszFileName ) - 4 );	// .o3d를 떼고 파일명부분만 카피
			_tcscat( szFileName, _T(".chr") );
			switch( nType )
			{
			case OT_ITEM:	// 아이템일 경우, 외장본(.chr)이 있다면 로딩한다 (예: 날개)
				{
					CResFile resFp;
					BOOL bResult = resFp.Open( MakePath( DIR_MODEL, szFileName ), "rb" );
					if( bResult == TRUE )
						((CModelObject*)pModel)->LoadBone( szFileName );
					break;
				}
			case OT_MOVER:	// 무버일 경우 외장본(.chr)을 로딩한다
				{
					((CModelObject*)pModel)->LoadBone( szFileName );
					break;
				}
			}
#else // __WING_ITEM
			if( nType == OT_MOVER )	// 무버는 외장본(.chr)을 먼저로딩해야한다.  여기다 넣어도 되는건지 몰겠군 -_-;
			{
				memset( szFileName, 0, sizeof(szFileName) );
				_tcsncpy( szFileName, lpszFileName, _tcslen( lpszFileName ) - 4 );	// .o3d를 떼고 파일명부분만 카피
				_tcscat( szFileName, _T(".chr") );
				((CModelObject*)pModel)->LoadBone( szFileName );
			}
#endif // __WING_ITEM
			if( bParts == FALSE )
			{
				if( ((CModelObject*)pModel)->LoadModel( lpszFileName ) == SUCCESS )  // skin 읽음
				{
					((CModelObject*)pModel)->RestoreDeviceObjects();
				}
			}
			break;
	}
	return pModel;
}
HRESULT CModelMng::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr = S_OK;
	return hr;
}
HRESULT CModelMng::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr = S_OK;
	MapStrToPtrItor itor;
	for( itor = m_mapFileToMesh.begin(); itor != m_mapFileToMesh.end(); itor++ )
		((CModel*)(*itor).second)->RestoreDeviceObjects();
	return hr;
}
HRESULT CModelMng::InvalidateDeviceObjects()
{
	HRESULT hr = S_OK;
	MapStrToPtrItor itor;
	for( itor = m_mapFileToMesh.begin(); itor != m_mapFileToMesh.end(); itor++ )
		((CModel*)(*itor).second)->InvalidateDeviceObjects();
	return hr;
}
HRESULT CModelMng::DeleteDeviceObjects()
{
	HRESULT hr = S_OK;
	MapStrToPtrItor itor;
	for( itor = m_mapFileToMesh.begin(); itor != m_mapFileToMesh.end(); itor++ )
	{
		((CModel*)(*itor).second)->DeleteDeviceObjects();
		CModel* pModel = ( CModel* )(*itor).second;
		SAFE_DELETE( pModel );
	}
	m_mapFileToMesh.clear();
	return hr;
}
BOOL CModelMng::LoadScript( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;

	CFixedArray< tagMODELELEM >* apModelElem; 
	TCHAR szObject[48];
	TCHAR szMotion[48];
	UINT iType, iObject, iMotion;
	MODELELEM modelElem;
	script.GetToken(); // subject or FINISHED
	while( script.tok != FINISHED )
	{
		iType = script.GetNumber();

		apModelElem = &m_aaModelElem[ iType ];
		script.GetToken(); // {
		script.GetToken(); // object name or }
		int nBrace = 1;
		// 여기부터 오브젝트 단위 obj, ctrl, item, sfx, mover
		while( nBrace )
		{
			ZeroMemory( &modelElem, sizeof( modelElem ) );
			if( *script.token == '}' ) 
			{
				nBrace--;
				if( nBrace > 0 )
				{
					script.GetToken(); // object name or }
					continue;
				}
				if( nBrace == 0 )
					continue;
			}
		#ifdef _DEBUG
			if( sizeof(szObject) <= strlen(script.token) + 1 )
				Error( "%s 스트링의 길이가 너무길다. %d", lpszFileName, strlen(script.token) );
		#endif

			_tcscpy( szObject, script.token ); // folder 또는 object name

			script.SetMark();
			// 스크립트 실수 검출 루틴 필요. - xuzhu -
			script.GetToken(); // {
			if( *script.token == '{' )
			{
				nBrace++;
				script.GetToken(); // object name or }
			#ifdef _DEBUG
				if( sizeof(szObject) <= strlen(script.token) + 1 )
					Error( "%s 스트링의 길이가 너무길다. %d", lpszFileName, strlen(script.token) );
			#endif

				_tcscpy( szObject, script.token ); // folder 또는 object name
				continue;
			}
			else
				script.GoMark();
			iObject = script.GetNumber();
			if( iObject == 0 )
			{
				CString str;
				str.Format( "CModelMng::LoadScript(%d) 0으로 지정된 모션 아이디 : %s, %s", script.GetLineNum(), szObject, script.token );
				AfxMessageBox( str );
			}
			modelElem.m_dwType = iType;
			modelElem.m_dwIndex = iObject;
		#ifdef _DEBUG
			if( sizeof(modelElem.m_szName) <= strlen(szObject) + 1 )
				Error( "%s 스트링의 길이가 너무길다. %d", lpszFileName, strlen(szObject) );
		#endif
			_tcscpy( modelElem.m_szName, szObject );
			modelElem.m_dwModelType = script.GetNumber();
			script.GetToken();
		#ifdef _DEBUG
			if( sizeof(modelElem.m_szPart) <= strlen(script.token) + 1 )
				Error( "%s 스트링의 길이가 너무길다. %d", lpszFileName, strlen(script.token) );
		#endif
			_tcscpy( modelElem.m_szPart, script.Token );
			modelElem.m_bFly = script.GetNumber();
			modelElem.m_dwDistant = script.GetNumber();
			modelElem.m_bPick = script.GetNumber();
			modelElem.m_fScale = script.GetFloat();
			modelElem.m_bTrans = script.GetNumber();
			modelElem.m_bShadow = script.GetNumber();			
			modelElem.m_bReserved = 0;
			modelElem.m_nTextureEx = script.GetNumber();
			modelElem.m_bRenderFlag = script.GetNumber();

			script.GetToken(); // object name or { or }
			// 여기부터 애니메이션 
			if( *script.token == '{' )
			{
				script.SetMark();
				script.GetToken(); // motion name or }
				int nMax = 0;
				// 모션 리스트 카운트 
				while( *script.token != '}' )
				{
				#ifdef _DEBUG
					if( sizeof(szMotion) <= strlen(script.token) + 1 )
						Error( "%s 스트링의 길이가 너무길다. %d", lpszFileName, strlen(script.token) );
				#endif
					_tcscpy( szMotion, script.token );
					iMotion = script.GetNumber();
					if( (int)( iMotion ) > nMax )
						nMax = iMotion;
					script.GetToken(); // motion name or }
				}
				nMax++;
				script.GoMark();
				// 실제 모션 리스트 세팅 
				script.GetToken(); // motion name or }
				modelElem.m_apszMotion = new TCHAR[ nMax * 32 ];
				modelElem.m_nMax = nMax;
				ZeroMemory( modelElem.m_apszMotion, sizeof( TCHAR ) * nMax * 32 );
				//TRACE( " %s %p\n", modelElem.m_szName, modelElem.m_apszMotion);
				while( *script.token != '}' )
				{
				#ifdef _DEBUG
					if( sizeof(szMotion) <= strlen(script.token) + 1 )
						Error( "%s 스트링의 길이가 너무길다. %d", lpszFileName, strlen(script.token) );
				#endif
					_tcscpy( szMotion, script.token );
					iMotion = script.GetNumber();
					TCHAR* lpszMotion = modelElem.GetMotion( iMotion );
					if( lpszMotion[0] )
					{
						CString str;
						str.Format( "CModelMng::LoadScript(%d) %s모션 중복 아이디 : %s", script.GetLineNum(), lpszFileName, lpszMotion );
						AfxMessageBox( str );
					}
					_tcscpy( lpszMotion, szMotion );
					script.GetToken(); // motion name or }
				}
				script.GetToken(); // object name or }
			}
			if( apModelElem->GetAt( iObject ) )
			{
				CString str;
				str.Format( "CModelMng::LoadScript(%d) %s중복 아이디 : type = %d, idx = %d, name = %s", script.GetLineNum(), lpszFileName, iType, iObject, modelElem.m_szName );
				AfxMessageBox( str );
			}

//////////////////////////////////////////////////////////

#ifndef __ENVIRONMENT_EFFECT

#ifdef __EVENT_FALL
			BOOL bEventFlag = FALSE;
			
			if( stricmp(modelElem.m_szName, "MaCoPrTr01") == 0 || stricmp(modelElem.m_szName, "MaCoPrTr03") == 0 ||
				stricmp(modelElem.m_szName, "MaCoPrTr10") == 0 || stricmp(modelElem.m_szName, "MaCoPrTr20") == 0 ||
				stricmp(modelElem.m_szName, "MaCoPrTr14") == 0 || stricmp(modelElem.m_szName, "MaCoPrTr11") == 0 )
			{
				bEventFlag = TRUE;
				_tcscpy( modelElem.m_szName, "MapleTree01" );
			}
			
			if( stricmp(modelElem.m_szName, "MaCoPrTr04") == 0 || stricmp(modelElem.m_szName, "MaCoPrTr05") == 0 ||
				stricmp(modelElem.m_szName, "MaCoPrTr12") == 0 || stricmp(modelElem.m_szName, "MaCoPrTr15") == 0 ||
				stricmp(modelElem.m_szName, "MaCoPrTr13") == 0 || stricmp(modelElem.m_szName, "MaCoPrTr02") == 0 )
			{
				bEventFlag = TRUE;
				_tcscpy( modelElem.m_szName, "MapleTree02" );
			}			

			if(	bEventFlag == TRUE )
				modelElem.m_fScale = 1.0f;	
#endif //__EVENT_FALL			
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
#ifdef __JAPAN_SAKURA
			BOOL bEventFlag = FALSE;
			
			if( stricmp(modelElem.m_szName, "MaCoPrTr01") == 0 || stricmp(modelElem.m_szName, "MaCoPrTr03") == 0 ||
				stricmp(modelElem.m_szName, "MaCoPrTr10") == 0 || stricmp(modelElem.m_szName, "MaCoPrTr20") == 0 ||
				stricmp(modelElem.m_szName, "MaCoPrTr14") == 0 || stricmp(modelElem.m_szName, "MaCoPrTr11") == 0 )
			{
				bEventFlag = TRUE;
				_tcscpy( modelElem.m_szName, "MaCoPrTr16" );
			}
			
			if( stricmp(modelElem.m_szName, "MaCoPrTr04") == 0 || stricmp(modelElem.m_szName, "MaCoPrTr05") == 0 ||
				stricmp(modelElem.m_szName, "MaCoPrTr12") == 0 || stricmp(modelElem.m_szName, "MaCoPrTr15") == 0 ||
				stricmp(modelElem.m_szName, "MaCoPrTr13") == 0 || stricmp(modelElem.m_szName, "MaCoPrTr02") == 0 )
			{
				bEventFlag = TRUE;
				_tcscpy( modelElem.m_szName, "MaCoPrTr17" );
			}			

			if(	bEventFlag == TRUE )
			{
				modelElem.m_fScale = 1.0f;
			}
#endif //__JAPAN_SAKURA			

#endif // __ENVIRONMENT_EFFECT
//////////////////////////////////////////////////////////
			
		#ifdef __WORLDSERVER
			if( iType != OT_SFX )	// sfx는 서버에서 skip
				apModelElem->SetAtGrow( iObject, &modelElem );
		#else
			apModelElem->SetAtGrow( iObject, &modelElem );
		#endif

		} // while( nBrace )

		script.GetToken(); // type name or }
		apModelElem->Optimize();
	}	// while( script.tok != FINISHED )

	return TRUE;
}

