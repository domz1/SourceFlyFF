//-----------------------------------------------------------------------------
// File: DSUtil.cpp
//
// Desc: DirectSound framework classes for reading and writing wav files and
//       playing them in DirectSound buffers. Feel free to use this class 
//       as a starting point for adding extra functionality.
//
// Copyright (c) 1999-2001 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include "stdafx.h"

#include <windows.h>
#include <mmsystem.h>
#include <dxerr9.h>
#include <dsound.h> 
#include "DSUtil.h"
#include "DXUtil.h"

CSoundManager  g_SoundMng;

BOOL CSoundManager::Play( int nId, D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity, BOOL bIsAutoplay, BOOL bIsVoice ) 
{ 
	if( !m_bSuccess )
		return FALSE;
	
	if( nId <= 0 )	return FALSE;
	LPTSTR lpszFileName = GetFileName( nId );
	if( lpszFileName )
	{
		TCHAR szFileName[ MAX_PATH ]; 
		MakePath( szFileName, DIR_SOUND, GetFileName( nId ) ); 
//		Error( "%s", szFileName );
		return Play( szFileName, pvPosition, pvVelocity, bIsAutoplay, bIsVoice );  
	}
	return FALSE;
}
// 사운드 프로퍼티를 위한 것들 
BOOL CSoundManager::LoadScript( LPCTSTR lpszFileName )
{
	if( !m_bSuccess )
		return FALSE;
	
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	CString string;
	MUSIC music;
	int id = script.GetNumber();
	do
	{
		script.GetToken(); // fileName
		_tcscpy( music.szMusicFileName, script.token );
		m_aMusic.SetAtGrow( id, &music );
		id = script.GetNumber();
	} while( script.tok != FINISHED );
	m_aMusic.Optimize();

	return TRUE;
}
LPTSTR CSoundManager::GetFileName( int nId )
{
	if( !m_bSuccess )
		return NULL;
	
	if( nId == -1 )
		return NULL;
	if( nId >= m_aMusic.GetSize() )	
		return NULL;
	MUSIC* pMusic = m_aMusic.GetAt( nId );
	if( pMusic && pMusic->szMusicFileName[0] )
		return (LPTSTR)pMusic->szMusicFileName;
	return NULL;
}

#define CLASSNAME       TEXT("BGMusicPlayer")
#define APPNAME         TEXT("BGMusic Player")
#define APPWIDTH        200
#define APPHEIGHT       100
#define MEDIA_TIMEOUT   (10 * 1000)  // 10 seconds, represented in milliseconds

//#define _DIRECTSHOW_MP3 


/*

		CMapStringToPtr         m_map;
		CSound*                 m_pSound;
		LPDIRECTSOUND3DBUFFER   m_pDS3DBuffer;      // 3D sound buffer
		LPDIRECTSOUND3DLISTENER m_pDSListener;      // 3D listener object
		DS3DBUFFER              m_dsBufferParams;   // 3D buffer properties
		DS3DLISTENER            m_dsListenerParams; // Listener properties

public:
		void Play( LPTSTR pszFileName );
		void Stop( LPTSTR pszFileName );
		void AllStop(  );
		void SetListener( D3DVECTOR vector );
*/
#define ORBIT_MAX_RADIUS        5.0f
#define IDT_MOVEMENT_TIMER      1

void CSoundManager::SetListener( D3DXVECTOR3 vecPos, FLOAT fAngle )
{
	if( !m_bSuccess )
		return;
	
	FLOAT fTheta = D3DXToRadian( fAngle );

	m_fTheta = fTheta;
	m_vOrientFront.x = sinf( fTheta );
	m_vOrientFront.y = 0.0f;
	m_vOrientFront.z = -cosf( fTheta );

	m_vOrientTop.x = 0.0f;
	m_vOrientTop.y = 1.0f;
	m_vOrientTop.z = 0.0f;

	m_vPosListener = vecPos;

	//m_pDSListener->SetOrientation( m_vOrientFront.x,  m_vOrientFront.y,  m_vOrientFront.z,  m_vOrientTop.x,  m_vOrientTop.y,  m_vOrientTop.z, DS3D_DEFERRED );//DS3D_IMMEDIATE );
	//m_pDSListener->SetPosition(	vecPos.x, vecPos.y, vecPos.z, DS3D_DEFERRED  );//DS3D_IMMEDIATE );
//	m_pDSListener->CommitDeferredSettings();
}
/*
HRESULT CSoundManager::PlayMusic( LPCTSTR pszFileName ) 
{
  HRESULT hr = S_OK;

#ifdef _DIRECTSHOW_MP3

  IPin *pPin = NULL;
  //TCHAR szFilename[MAX_PATH];
  WCHAR wFileName[MAX_PATH];

  // Determine the file to load based on DirectX Media path (from SDK)
  //_tcscpy( szFilename, DXUtil_GetDXSDKMediaPath() );
  //_tcscat( szFilename, pstrFiles[m_iNextFile % m_iNumFiles]);
  //_tcscpy( m_szCurrentFile, pstrFiles[m_iNextFile % m_iNumFiles]);
  //m_iNextFile++;c

  // Make sure that this file exists
  DWORD dwAttr = GetFileAttributes(pszFileName);
  if (dwAttr == (DWORD) -1)
      return ERROR_FILE_NOT_FOUND;

  #ifndef UNICODE
      MultiByteToWideChar(CP_ACP, 0, pszFileName, -1, wFileName, MAX_PATH);
  #else
      lstrcpy(wFileName, pszFileName);
  #endif

  // OPTIMIZATION OPPORTUNITY
// This will open the file, which is expensive. To optimize, this
  // should be done earlier, ideally as soon as we knew this was the
  // next file to ensure that the file load doesn't add to the
  // filter swapping time & cause a hiccup.
  //
  // Add the new source filter to the graph. (Graph can still be running)
  hr = m_pGraphBuilder->AddSourceFilter(wFileName, wFileName, &m_pSourceNext);

  // Get the first output pin of the new source filter. Audio sources 
  // typically have only one output pin, so for most audio cases finding 
  // any output pin is sufficient.
  if (SUCCEEDED(hr)) {
      hr = m_pSourceNext->FindPin(L"Output", &pPin);  
  }

  // Stop the graph
  if (SUCCEEDED(hr)) {
      hr = m_pMediaControl->Stop();
  }

  // Break all connections on the filters. You can do this by adding 
  // and removing each filter in the graph
  if (SUCCEEDED(hr)) {
      IEnumFilters *pFilterEnum = NULL;

      if (SUCCEEDED(hr = m_pGraphBuilder->EnumFilters(&pFilterEnum))) {
          int iFiltCount = 0;
          int iPos = 0;

          // Need to know how many filters. If we add/remove filters during the
          // enumeration we'll invalidate the enumerator
          while (S_OK == pFilterEnum->Skip(1)) {
              iFiltCount++;
          }

          // Allocate space, then pull out all of the 
          IBaseFilter **ppFilters = reinterpret_cast<IBaseFilter **>
                                    (_alloca(sizeof(IBaseFilter *) * iFiltCount));
          pFilterEnum->Reset();

          while (S_OK == pFilterEnum->Next(1, &(ppFilters[iPos++]), NULL));
          SAFE_RELEASE(pFilterEnum);

          for (iPos = 0; iPos < iFiltCount; iPos++) {
              m_pGraphBuilder->RemoveFilter(ppFilters[iPos]);
			// Put the filter back, unless it is the old source
			if (ppFilters[iPos] != m_pSourceCurrent) {
				m_pGraphBuilder->AddFilter(ppFilters[iPos], NULL);
              }
			SAFE_RELEASE(ppFilters[iPos]);
          }
      }
  }

  // We have the new ouput pin. Render it
  if (SUCCEEDED(hr)) {
      hr = m_pGraphBuilder->Render(pPin);
      m_pSourceCurrent = m_pSourceNext;
      m_pSourceNext = NULL;
  }

  SAFE_RELEASE(pPin);
  SAFE_RELEASE(m_pSourceNext); // In case of errors

  // Re-seek the graph to the beginning
  if (SUCCEEDED(hr)) {
      LONGLONG llPos = 0;
      hr = m_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,
                                         &llPos, AM_SEEKING_NoPositioning);
  } 

  // Start the graph
  if (SUCCEEDED(hr)) {
      hr = m_pMediaControl->Run();
  }

  // Release the old source filter.
  SAFE_RELEASE(m_pSourceCurrent)
#endif	
  return S_OK;   
}
*/


void CSoundManager::Stop()
{
	if( !m_bSuccess )
		return;
	
	//if( m_pSound )
	{
//		m_pSound->Stop();
	//	m_pSound->Reset();
	}
}

BOOL CSoundManager::Play( LPTSTR pszFileName, D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity, BOOL bIsAutoplay, BOOL bIsVoice )
{
	if( !m_bSuccess )
		return FALSE;
	
	if( m_nSoundVolume == -5000 )
		return FALSE;

	FLOAT fVolume = 0.0f;		// <- 이거 왜 FLOAT로 했는가?

	fVolume = (FLOAT)( m_nSoundVolume );

	// 음성이면 최대 출력
	if( bIsVoice )
	{
#ifdef __CLIENT
		if( g_Option.m_bVoice == FALSE )
			return FALSE;
#endif

		fVolume = 0.0f;
	}

	CSound* pSound = GetSound( pszFileName );
	if( pSound == NULL )
	{
//		Error( "file not found: %s", pszFileName );
		return FALSE;
	}
	DWORD dwIndex = pSound->GetFreeBufferIndex();;

	//pSound->m_pBufferParams[dwIndex]
	if( pvPosition )
	{
		/*
		for( int i = 0; i < m_world.m_arySelect.size(); i++)
		{
			CObj* pObj = (CObj*)m_world.m_arySelect.at( i );
			CPoint pt = point - m_ptOrg;
			D3DXVECTOR3 vPos = pObj->GetPos();
			// 오브젝트 회전 
			// 객체와 중심 사이의 길이. 반지름.
			FLOAT fRadian = sqrt( 
				fabs ( vPos.x - m_world.m_vCenter.x ) * fabs ( vPos.x - m_world.m_vCenter.x ) +
				fabs ( vPos.z - m_world.m_vCenter.z ) * fabs ( vPos.z - m_world.m_vCenter.z ) 
				);
			D3DXVECTOR3 Line1 = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
			D3DXVECTOR3 Line2 = vPos;
			Line2 = m_world.m_vCenter - Line2;
			// Nomralize Vectors
			FLOAT vlength = sqrt( ( Line2.x * Line2.x ) + ( Line2.z * Line2.z ) ); 
			Line2 = Line2 / vlength;
			// 라디안 구하기 
			FLOAT a = acos( D3DXVec3Dot( &Line1, &Line2 ) );
			//a = ( D3DX_PI * 2.0f ) - a;
			if( vPos.x < m_world.m_vCenter.x )
				a = ( D3DX_PI * 2.0f ) - a;
			// 마우스가 위아래로 움직였으면 돌기 시작함.(pt.y를 검사하면 알 수 있음)
			if( pt.y )
			{
				// 마우스 이동에 따른 가변폭 구하기.
				FLOAT b = (FLOAT) ( pt.y < 0 ? -pt.y : -pt.y ) / 50.f;
				// 가변폭을 라디안에 더하기.
				a += b;
				// 오브젝트가 이동이 없고 원점에서 돌기만 하는 경우라면 vPos를 변경할 필요가 없다.
				if( vlength )
				{
					TRACE( "a %f\n", a );
					vPos.x = m_world.m_vCenter.x + sin( a ) * fRadian;
					vPos.z = m_world.m_vCenter.z + cos( a ) * fRadian;
					pObj->SetPos( vPos );
				}
				pObj->AddAngle( -(360.0f * b / ( D3DX_PI * 2.0f ) ));
				//pObj->SetOnLand();								
			}
		}
		*/
		D3DXVECTOR3 vPos = *pvPosition - m_vPosListener;
		FLOAT fRadian = sqrt( 
			fabs ( vPos.x ) * fabs ( vPos.x ) +	fabs ( vPos.z ) * fabs ( vPos.z ) );

	//Radian *= 2.0f;

		if( fRadian > 100.0f )
			return FALSE;

		D3DXVECTOR3 Line1 = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
		D3DXVECTOR3 Line2 = -vPos;
		//Line2 = m_world.m_vCenter - Line2;
		// Nomralize Vectors
		FLOAT vlength = sqrt( ( Line2.x * Line2.x ) + ( Line2.z * Line2.z ) ); 
		Line2 = Line2 / vlength;
		// 라디안 구하기 
		FLOAT a = acos( D3DXVec3Dot( &Line1, &Line2 ) );
		//a = ( D3DX_PI * 2.0f ) - a;
		if( vPos.x < 0 )
			a = ( D3DX_PI * 2.0f ) - a;
		a += -m_fTheta;

		vPos.x = (float)( (int)(sin( a ) * fRadian ) );	// 여기 (int)캐스팅 바꾸지 말것. (int)안하면 컴터 리붓됨.
		vPos.z = (float)( (int)(cos( a ) * fRadian ) );
		vPos.y = (float)( (int) vPos.y );

		D3DVECTOR v;
		v.x = vPos.x;	// 혹시 몰라 이렇게 해봤다.
		v.y = vPos.y;
		v.z = vPos.z;
		pSound->m_pBufferParams[dwIndex].vPosition = v;	// D3XVECTOR3를 D3DVECTOR에 그대로 넣어도 되나?
	}
	else
	{
		D3DVECTOR vec;
		m_pDSListener->GetPosition(	&vec );
		pSound->m_pBufferParams[dwIndex].vPosition.x = vec.x;
		pSound->m_pBufferParams[dwIndex].vPosition.y = vec.y;
		pSound->m_pBufferParams[dwIndex].vPosition.z = vec.z;
	}
	if( pvVelocity )
	{
		pSound->m_pBufferParams[dwIndex].vVelocity.x = pvVelocity->x;
		pSound->m_pBufferParams[dwIndex].vVelocity.y = pvVelocity->y;
		pSound->m_pBufferParams[dwIndex].vVelocity.z = pvVelocity->z;
	}
	else
	{
		D3DVECTOR vec;
		m_pDSListener->GetPosition(	&vec );
		pSound->m_pBufferParams[dwIndex].vVelocity.x = vec.x;
		pSound->m_pBufferParams[dwIndex].vVelocity.y = vec.y;
		pSound->m_pBufferParams[dwIndex].vVelocity.z = vec.z;
	}

   pSound->m_pBufferParams[dwIndex].flMinDistance = 4.0f;
   pSound->m_pBufferParams[dwIndex].flMaxDistance = 70.0f;

    //if( m_pDSListener )
      //  m_pDSListener->SetAllParameters( &m_dsListenerParams, dwApplyFlag );
   pSound->m_apDS3DBuffer[ dwIndex ]->SetAllParameters( &pSound->m_pBufferParams[dwIndex], DS3D_IMMEDIATE );//DS3D_IMMEDIATE );//DS3D_IMMEDIATE );

   pSound->GetBuffer(pSound->GetFreeBufferIndex())->SetVolume( (LONG)( fVolume ) );
//   pSound->GetBuffer(pSound->GetFreeBufferIndex())->SetVolume( m_nSoundVolume );
   
   if( bIsAutoplay )
   {
	   if( FAILED( pSound->Play( 0, 0 , &dwIndex ) ) )
			return FALSE;//return DXTRACE_ERR_MSGBOX( TEXT("Play"), hr );
   }

	return TRUE;
}


CSound* CSoundManager::GetSound( LPTSTR pszFileName )
{
	if( !m_bSuccess )
		return NULL;
	
	GUID  guid3DAlgorithm = GUID_NULL;
	HRESULT hr;
	CSound* pSound;

	if( m_map.Lookup(  pszFileName, (void*&)pSound ) )
	{
		//pSound->Stop();
		//pSound->Reset();
		return pSound;
	}
//	if( m_pSound )
	//{
		//m_pSound->Stop();
		//m_pSound->Reset();
	//}

	// Stop the timer while dialogs are displayed
//	m_bAllowMovementTimer = FALSE;

	// Free any previous sound, and make a new one
	//SAFE_DELETE( m_pSound );

	CWaveFile waveFile;
	waveFile.Open( pszFileName, NULL, WAVEFILE_READ );
	WAVEFORMATEX* pwfx = waveFile.GetFormat();
	if( pwfx == NULL )
	{
		//SetDlgItemText( hDlg, IDC_STATUS, TEXT("Invalid wave file format.") );
		return NULL;
	}
	if( pwfx->nChannels > 1 )
	{
		// Too many channels in wave.  Sound must be mono when using DSBCAPS_CTRL3D
		//SetDlgItemText( hDlg, IDC_STATUS, TEXT("Wave file must be mono for 3D control.") );
		//SetDlgItemText( hDlg, IDC_FILENAME, TEXT("") );
		return NULL;
	}
	if( pwfx->wFormatTag != WAVE_FORMAT_PCM )
	{
		// Sound must be PCM when using DSBCAPS_CTRL3D
		//SetDlgItemText( hDlg, IDC_STATUS, TEXT("Wave file must be PCM for 3D control.") );
		//SetDlgItemText( hDlg, IDC_FILENAME, TEXT("") );
		return NULL;
	}
	// User selected DS3DALm_NO_VIRTUALIZATION  
	guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
	// User selected DS3DALm_HRTF_FULL  
	//guid3DAlgorithm = DS3DALm_HRTF_FULL;
	// User selected DS3DALm_HRTF_LIGHT
	//guid3DAlgorithm = DS3DALm_HRTF_LIGHT;

	// Load the wave file into a DirectSound buffer
	hr = Create( &pSound, pszFileName, DSBCAPS_CTRL3D | DSBCAPS_MUTE3DATMAXDISTANCE , guid3DAlgorithm, 10 );  
	if( FAILED( hr ) || hr == DS_NO_VIRTUALIZATION )
	{
		DXTRACE_ERR( TEXT("Create"), hr );
		if( DS_NO_VIRTUALIZATION == hr )
		{
				//MessageBox( hDlg, "The 3D virtualization algorithm requested is not supported under this "
					//					"operating system.  It is available only on Windows 2000, Windows ME, and Windows 98 with WDM "
						//				"drivers and beyond.  Creating buffer with no virtualization.", 
							//			"DirectSound Sample", MB_OK );
		}
		// Unknown error, but not a critical failure, so just update the status
		//SetDlgItemText( hDlg, IDC_FILENAME, TEXT("Could not create sound buffer.") );
		return NULL;
	}
	m_map.SetAt( pszFileName, pSound );

	// Get the 3D buffer from the secondary buffer
	//LPDIRECTSOUND3DBUFFER pDS3DBuffer;     
	//if( FAILED( hr = pSound->Get3DBufferInterface( 0, &pDS3DBuffer ) ) )
	if( FAILED( hr = pSound->Get3DBufferInterface( ) ) )
	{
		DXTRACE_ERR_MSGBOX( TEXT("Get3DBufferInterface"), hr );
		//SetDlgItemText( hDlg, IDC_STATUS, TEXT("Could not get 3D buffer.") );
		//SetDlgItemText( hDlg, IDC_FILENAME, TEXT("") );
		return NULL;
	}
	/*
	DWORD dwIndex = pSound->GetFreeBufferIndex();

	// Get the 3D buffer parameters
	pSound->m_pBufferParams->dwSize = sizeof(DS3DBUFFER);
	pSound->m_apDS3DBuffer[ dwIndex ]->GetAllParameters( &pSound->m_pBufferParams[0] );

	// Set new 3D buffer parameters
	pSound->m_pBufferParams->dwMode = DS3DMODE_NORMAL;//HEADRELATIVE;//DS3DMODE_HEADRELATIVE;
	pSound->m_apDS3DBuffer[ dwIndex ]->SetAllParameters( &pSound->m_pBufferParams[ dwIndex ], DS3D_IMMEDIATE );

	*/
	//DSBCAPS dsbcaps;
	//ZeroMemory( &dsbcaps, sizeof(DSBCAPS) );
	//dsbcaps.dwSize = sizeof(DSBCAPS);

	//LPDIRECTSOUNDBUFFER pDSB = pSound->GetBuffer( 0 );
	//pDSB->GetCaps( &dsbcaps );
	//if( ( dsbcaps.dwFlags & DSBCAPS_LOCHARDWARE ) != 0 )
		;//SetDlgItemText( hDlg, IDC_STATUS, TEXT("File loaded using hardware mixing.") );
	//else
		;//SetDlgItemText( hDlg, IDC_STATUS, TEXT("File loaded using software mixing.") );
	//m_bAllowMovementTimer = TRUE;
	// Play buffer always in looped mode just for this sample
	
	return pSound;

}

/*
VOID Set3DParameters( FLOAT fDopplerFactor, FLOAT fRolloffFactor,
                      FLOAT fMinDistance,   FLOAT fMaxDistance )
{
    // Every change to 3-D sound buffer and listener settings causes 
    // DirectSound to remix, at the expense of CPU cycles. 
    // To minimize the performance impact of changing 3-D settings, 
    // use the DS3D_DEFERRED flag in the dwApply parameter of any of 
    // the IDirectSound3DListener or IDirectSound3DBuffer methods that 
    // change 3-D settings. Then call the IDirectSound3DListener::CommitDeferredSettings 
    // method to execute all of the deferred commands at once.
    DWORD dwApplyFlag = ( m_bDeferSettings ) ? DS3D_DEFERRED : DS3D_IMMEDIATE;

    m_dsListenerParams.flDopplerFactor = fDopplerFactor;
    m_dsListenerParams.flRolloffFactor = fRolloffFactor;

    if( m_pDSListener )
        m_pDSListener->SetAllParameters( &m_dsListenerParams, dwApplyFlag );

    m_dsBufferParams.flMinDistance = fMinDistance;
    m_dsBufferParams.flMaxDistance = fMaxDistance;

    if( m_pDS3DBuffer )
        m_pDS3DBuffer->SetAllParameters( &m_dsBufferParams, dwApplyFlag );
}

//-----------------------------------------------------------------------------
// Name: SetObjectProperties()
// Desc: Sets the position and velocity on the 3D buffer
//-----------------------------------------------------------------------------
VOID SetObjectProperties( D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity )
{
    // Every change to 3-D sound buffer and listener settings causes 
    // DirectSound to remix, at the expense of CPU cycles. 
    // To minimize the performance impact of changing 3-D settings, 
    // use the DS3D_DEFERRED flag in the dwApply parameter of any of 
    // the IDirectSound3DListener or IDirectSound3DBuffer methods that 
    // change 3-D settings. Then call the IDirectSound3DListener::CommitDeferredSettings 
    // method to execute all of the deferred commands at once.
    memcpy( &m_dsBufferParams.vPosition, pvPosition, sizeof(D3DVECTOR) );
    memcpy( &m_dsBufferParams.vVelocity, pvVelocity, sizeof(D3DVECTOR) );

    if( m_pDS3DBuffer )
        m_pDS3DBuffer->SetAllParameters( &m_dsBufferParams, DS3D_IMMEDIATE );
}






*/

//-----------------------------------------------------------------------------
// Name: CSoundManager::CSoundManager()
// Desc: Constructs the class
//-----------------------------------------------------------------------------
CSoundManager::CSoundManager()
{
	m_pDS = NULL;
	m_pDSListener  = NULL;   // 3D listener object
	m_nOldSoundVolume = m_nSoundVolume = 0;
	m_bSuccess = TRUE;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::~CSoundManager()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CSoundManager::~CSoundManager()
{
	POSITION pos = m_map.GetStartPosition();
	CString  string;
	CSound* pSound;

	while( pos )
	{
	  m_map.GetNextAssoc( pos, string, (void *&)pSound );
	  SAFE_DELETE( pSound );
	}
	SAFE_RELEASE( m_pDS ); 
	// Stop playback
	if (m_pMediaControl)
	  m_pMediaControl->Stop();

	// Release all remaining pointers
	SAFE_RELEASE( m_pSourceNext);
	SAFE_RELEASE( m_pSourceCurrent);
	//SAFE_RELEASE( m_pMediaSeeking);
	//SAFE_RELEASE( m_pMediaControl);
	//SAFE_RELEASE( m_pGraphBuilder);

	// Clean up COM
	CoUninitialize();
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Initialize()
// Desc: Initializes the IDirectSound object and also sets the primary buffer
//       format.  This function must be called before any others.
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Initialize( HWND  hWnd, 
                                   DWORD dwCoopLevel, 
                                   DWORD dwPrimaryChannels, 
                                   DWORD dwPrimaryFreq, 
                                   DWORD dwPrimaryBitRate )
{
    HRESULT             hr;

    SAFE_RELEASE( m_pDS );

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate8( NULL, &m_pDS, NULL ) ) )
	{
		//return DXTRACE_ERR_MSGBOX( TEXT("DirectSoundCreate8"), hr );
		m_bSuccess = FALSE;
		MessageBox( hWnd, "DirectSound init failed", "DirectSound", MB_OK );
		return E_FAIL;
	}
        

    // Set DirectSound coop level 
    if( FAILED( hr = m_pDS->SetCooperativeLevel( hWnd, dwCoopLevel ) ) )
	{
        //return DXTRACE_ERR_MSGBOX( TEXT("SetCooperativeLevel"), hr );
		m_bSuccess = FALSE;
		MessageBox( hWnd, "DirectSound init failed", "DirectSound", MB_OK );
		return E_FAIL;
	}
    
    // Set primary buffer format
    if( FAILED( hr = SetPrimaryBufferFormat( dwPrimaryChannels, dwPrimaryFreq, dwPrimaryBitRate ) ) )
	{
        //return DXTRACE_ERR_MSGBOX( TEXT("SetPrimaryBufferFormat"), hr );
		m_bSuccess = FALSE;
		MessageBox( hWnd, "SetPrimaryBufferFormat", "DirectSound", MB_OK );
		return E_FAIL;
	}


	// Create a static IDirectSound in the CSound class.  
	// Set coop level to DSSCL_PRIORITY, and set primary buffer 
	// format to stereo, 22kHz and 16-bit output.
	hr = Get3DListenerInterface( &m_pDSListener );
	ZeroMemory( &m_dsListenerParams, sizeof( DS3DLISTENER ) );
	m_dsListenerParams.dwSize = sizeof(DS3DLISTENER);

    DWORD dwApplyFlag = DS3D_IMMEDIATE;//DS3D_DEFERRED;//( m_bDeferSettings ) ? DS3D_DEFERRED : DS3D_IMMEDIATE;


	m_pDSListener->GetAllParameters( &m_dsListenerParams );

	FLOAT fTheta = D3DXToRadian( 0.0f );

	D3DVECTOR vOrientFront; 
	D3DVECTOR vOrientTop;

	vOrientFront.x = sinf( fTheta );
	vOrientFront.y = 0.0f;
	vOrientFront.z = -cosf( fTheta );

	vOrientTop.x = 0.0f;
	vOrientTop.y = 1.0f;
	vOrientTop.z = 0.0f;
    m_dsListenerParams.flDopplerFactor = 1.0f;
    m_dsListenerParams.flRolloffFactor = 1.0f;
	m_dsListenerParams.flDistanceFactor = 1.0f;//001f;
	m_dsListenerParams.vOrientFront.x = vOrientFront.x;
	m_dsListenerParams.vOrientFront.y = vOrientFront.y;
	m_dsListenerParams.vOrientFront.z = vOrientFront.z;
	m_dsListenerParams.vOrientTop.x = vOrientTop.x;
	m_dsListenerParams.vOrientTop.y = vOrientTop.y;
	m_dsListenerParams.vOrientTop.z = vOrientTop.z;
	
    if( m_pDSListener )
        m_pDSListener->SetAllParameters( &m_dsListenerParams, dwApplyFlag );
	
	m_pDSListener->SetOrientation( vOrientFront.x,  vOrientFront.y,  vOrientFront.z,  vOrientTop.x,  vOrientTop.y,  vOrientTop.z, DS3D_IMMEDIATE );
	m_pDSListener->SetPosition(	0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE );

#ifdef _DIRECTSHOW_MP3
	////////////////////////////////////////////////////
	// mp3 player


  // Initialize COM
  if (FAILED (hr = CoInitialize(NULL)) )
      return hr;

  // Create DirectShow Graph
  if (FAILED (hr = CoCreateInstance(CLSID_FilterGraph, NULL,
                                    CLSCTX_INPROC, IID_IGraphBuilder,
                                    reinterpret_cast<void **>(&m_pGraphBuilder))) )
      return hr;

  // Get the IMediaControl Interface
  if (FAILED (m_pGraphBuilder->QueryInterface(IID_IMediaControl,
                               reinterpret_cast<void **>(&m_pMediaControl))))
      return hr;

  // Get the IMediaControl Interface
  if (FAILED (m_pGraphBuilder->QueryInterface(IID_IMediaSeeking,
                               reinterpret_cast<void **>(&m_pMediaSeeking))))
      return hr;
			

/*
	// Set the icon for this dialog.
	//PostMessage( hDlg, WM_SETICON, ICON_BIG,   (LPARAM) hIcon );  // Set big icon
	//PostMessage( hDlg, WM_SETICON, ICON_SMALL, (LPARAM) hIcon );  // Set small icon

	// Create a timer to periodically move the 3D object around
	//SetTimer( hDlg, IDT_MOVEMENT_TIMER, 0, NULL );

	// Set the UI controls
	//SetDlgItemText( hDlg, IDC_FILENAME, TEXT("") );
	//SetDlgItemText( hDlg, IDC_STATUS, TEXT("No file loaded.") );

	// Get handles to dialog items
	HWND hDopplerSlider  = GetDlgItem( hDlg, IDC_DOPPLER_SLIDER );
	HWND hRolloffSlider  = GetDlgItem( hDlg, IDC_ROLLOFF_SLIDER );
	HWND hMinDistSlider  = GetDlgItem( hDlg, IDC_MINDISTANCE_SLIDER );
	HWND hMaxDistSlider  = GetDlgItem( hDlg, IDC_MAXDISTANCE_SLIDER );
	HWND hVertSlider     = GetDlgItem( hDlg, IDC_VERTICAL_SLIDER );
	HWND hHorzSlider     = GetDlgItem( hDlg, IDC_HORIZONTAL_SLIDER );

	// Set the range and position of the sliders
	PostMessage( hDopplerSlider, TBM_SETRANGEMAX, TRUE, 40L );
	PostMessage( hDopplerSlider, TBM_SETRANGEMIN, TRUE, 0L );

	PostMessage( hRolloffSlider, TBM_SETRANGEMAX, TRUE, 40L );
	PostMessage( hRolloffSlider, TBM_SETRANGEMIN, TRUE, 0L );

	PostMessage( hMinDistSlider, TBM_SETRANGEMAX, TRUE, 40L );
	PostMessage( hMinDistSlider, TBM_SETRANGEMIN, TRUE, 1L );

	PostMessage( hMaxDistSlider, TBM_SETRANGEMAX, TRUE, 40L );
	PostMessage( hMaxDistSlider, TBM_SETRANGEMIN, TRUE, 1L );

	PostMessage( hVertSlider,    TBM_SETRANGEMAX, TRUE, 100L );
	PostMessage( hVertSlider,    TBM_SETRANGEMIN, TRUE, -100L );
	PostMessage( hVertSlider,    TBM_SETPOS,      TRUE, 100L );

	PostMessage( hHorzSlider,    TBM_SETRANGEMAX, TRUE, 100L );
	PostMessage( hHorzSlider,    TBM_SETRANGEMIN, TRUE, -100L );
	PostMessage( hHorzSlider,    TBM_SETPOS,      TRUE, 100L );

	// Set the position of the sliders
	SetSlidersPos( hDlg, 0.0f, 0.0f, ORBIT_MAX_RADIUS, ORBIT_MAX_RADIUS*2.0f );
	*/








#endif
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::SetPrimaryBufferFormat()
// Desc: Set primary buffer to a specified format 
//       For example, to set the primary buffer format to 22kHz stereo, 16-bit
//       then:   dwPrimaryChannels = 2
//               dwPrimaryFreq     = 22050, 
//               dwPrimaryBitRate  = 16
//-----------------------------------------------------------------------------
HRESULT CSoundManager::SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
                                               DWORD dwPrimaryFreq, 
                                               DWORD dwPrimaryBitRate )
{
	if( !m_bSuccess )
		return S_FALSE;

    HRESULT             hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER  | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("CreateSoundBuffer"), hr );

    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) dwPrimaryChannels; 
    wfx.nSamplesPerSec  = dwPrimaryFreq; 
    wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("SetFormat"), hr );

    SAFE_RELEASE( pDSBPrimary );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Get3DListenerInterface()
// Desc: Returns the 3D listener interface associated with primary buffer.
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener )
{
	if( !m_bSuccess )
		return S_FALSE;

    HRESULT             hr;
    DSBUFFERDESC        dsbdesc;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( ppDSListener == NULL )
        return E_INVALIDARG;
    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    *ppDSListener = NULL;

    // Obtain primary buffer, asking it for 3D control
    ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER  | DSBCAPS_CTRLVOLUME;
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbdesc, &pDSBPrimary, NULL ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("CreateSoundBuffer"), hr );

    if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, 
                                                  (VOID**)ppDSListener ) ) )
    {
        SAFE_RELEASE( pDSBPrimary );
        return DXTRACE_ERR_MSGBOX( TEXT("QueryInterface"), hr );
    }

    // Release the primary buffer, since it is not need anymore
    SAFE_RELEASE( pDSBPrimary );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSoundManager::Create()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Create( CSound** ppSound, 
                               LPTSTR strWaveFileName, 
                               DWORD dwCreationFlags, 
                               GUID guid3DAlgorithm,
                               DWORD dwNumBuffers )
{
	if( !m_bSuccess )
		return S_FALSE;
	
    HRESULT hr;
    HRESULT hrRet = S_OK;
    DWORD   i;
    LPDIRECTSOUNDBUFFER* apDSBuffer     = NULL;
    DWORD                dwDSBufferSize = NULL;
    CWaveFile*           pWaveFile      = NULL;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;
    if( strWaveFileName == NULL || ppSound == NULL || dwNumBuffers < 1 )
        return E_INVALIDARG;

    apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    if( apDSBuffer == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile = new CWaveFile();
    if( pWaveFile == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile->Open( strWaveFileName, NULL, WAVEFILE_READ );

    if( pWaveFile->GetSize() == 0 )
    {
        // Wave is blank, so don't create it.
        hr = E_FAIL;
        goto LFail;
    }

    // Make the DirectSound buffer the same size as the wav file
    dwDSBufferSize = pWaveFile->GetSize();

    // Create the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags  | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat     = pWaveFile->m_pwfx;

    // DirectSound is only guarenteed to play PCM data.  Other
    // formats may or may not work depending the sound card driver.
    hr = m_pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[0], NULL );

    // Be sure to return this error code if it occurs so the
    // callers knows this happened.
    if( hr == DS_NO_VIRTUALIZATION )
        hrRet = DS_NO_VIRTUALIZATION;
            
    if( FAILED(hr) )
    {
        // DSERR_BUFFERTOOSMALL will be returned if the buffer is
        // less than DSBSIZE_FX_MIN (100ms) and the buffer is created
        // with DSBCAPS_CTRLFX.
        if( hr != DSERR_BUFFERTOOSMALL )
            DXTRACE_ERR_MSGBOX( TEXT("CreateSoundBuffer"), hr );
            
        goto LFail;
    }

    for( i=1; i<dwNumBuffers; i++ )
    {
        if( FAILED( hr = m_pDS->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
        {
            DXTRACE_ERR_MSGBOX( TEXT("DuplicateSoundBuffer"), hr );
            goto LFail;
        }
    }
    // Create the sound
    *ppSound = new CSound( apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile );
    
    SAFE_DELETE( apDSBuffer );
    return hrRet;

LFail:
    // Cleanup
    SAFE_DELETE( pWaveFile );
    SAFE_DELETE( apDSBuffer );
    return hr;
}









//-----------------------------------------------------------------------------
// Name: CSoundManager::CreateFromMemory()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSoundManager::CreateFromMemory( CSound** ppSound, 
                                        BYTE* pbData,
                                        ULONG  ulDataSize,
                                        LPWAVEFORMATEX pwfx,
                                        DWORD dwCreationFlags, 
                                        GUID guid3DAlgorithm,
                                        DWORD dwNumBuffers )
{
	if( !m_bSuccess )
		return S_FALSE;
	
    HRESULT hr;
    DWORD   i;
    LPDIRECTSOUNDBUFFER* apDSBuffer     = NULL;
    DWORD                dwDSBufferSize = NULL;
    CWaveFile*           pWaveFile      = NULL;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;
    if( pbData == NULL || ppSound == NULL || dwNumBuffers < 1 )
        return E_INVALIDARG;

    apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    if( apDSBuffer == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile = new CWaveFile();
    if( pWaveFile == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile->OpenFromMemory( pbData,ulDataSize, pwfx, WAVEFILE_READ );


    // Make the DirectSound buffer the same size as the wav file
    dwDSBufferSize = ulDataSize;

    // Create the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags  | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat     = pwfx;

    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[0], NULL ) ) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("CreateSoundBuffer"), hr );
        goto LFail;
    }

    for( i=1; i<dwNumBuffers; i++ )
    {
        if( FAILED( hr = m_pDS->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
        {
            DXTRACE_ERR_MSGBOX( TEXT("DuplicateSoundBuffer"), hr );
            goto LFail;
        }
    }

    // Create the sound
    *ppSound = new CSound( apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile );

    SAFE_DELETE( apDSBuffer );
    return S_OK;

LFail:
    // Cleanup
   
    SAFE_DELETE( apDSBuffer );
    return hr;
}





//-----------------------------------------------------------------------------
// Name: CSoundManager::CreateStreaming()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSoundManager::CreateStreaming( CStreamingSound** ppStreamingSound, 
                                        LPTSTR strWaveFileName, 
                                        DWORD dwCreationFlags, 
                                        GUID guid3DAlgorithm,
                                        DWORD dwNotifyCount, 
                                        DWORD dwNotifySize, 
                                        HANDLE hNotifyEvent )
{
	if( !m_bSuccess )
		return S_FALSE;
	
    HRESULT hr;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;
    if( strWaveFileName == NULL || ppStreamingSound == NULL || hNotifyEvent == NULL )
        return E_INVALIDARG;

    LPDIRECTSOUNDBUFFER pDSBuffer      = NULL;
    DWORD               dwDSBufferSize = NULL;
    CWaveFile*          pWaveFile      = NULL;
    DSBPOSITIONNOTIFY*  aPosNotify     = NULL; 
    LPDIRECTSOUNDNOTIFY pDSNotify      = NULL;

    pWaveFile = new CWaveFile();
    pWaveFile->Open( strWaveFileName, NULL, WAVEFILE_READ );

    // Figure out how big the DSound buffer should be 
    dwDSBufferSize = dwNotifySize * dwNotifyCount;

    // Set up the direct sound buffer.  Request the NOTIFY flag, so
    // that we are notified as the sound buffer plays.  Note, that using this flag
    // may limit the amount of hardware acceleration that can occur. 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags | 
                           DSBCAPS_CTRLPOSITIONNOTIFY | 
                           DSBCAPS_GETCURRENTPOSITION2  | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat     = pWaveFile->m_pwfx;

    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBuffer, NULL ) ) )
    {
        // If wave format isn't then it will return 
        // either DSERR_BADFORMAT or E_INVALIDARG
        if( hr == DSERR_BADFORMAT || hr == E_INVALIDARG )
            return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

        return DXTRACE_ERR_MSGBOX( TEXT("CreateSoundBuffer"), hr );
    }

    // Create the notification events, so that we know when to fill
    // the buffer as the sound plays. 
    if( FAILED( hr = pDSBuffer->QueryInterface( IID_IDirectSoundNotify, 
                                                (VOID**)&pDSNotify ) ) )
    {
        SAFE_DELETE( aPosNotify );
        return DXTRACE_ERR_MSGBOX( TEXT("QueryInterface"), hr );
    }

    aPosNotify = new DSBPOSITIONNOTIFY[ dwNotifyCount ];
    if( aPosNotify == NULL )
        return E_OUTOFMEMORY;

    for( DWORD i = 0; i < dwNotifyCount; i++ )
    {
        aPosNotify[i].dwOffset     = (dwNotifySize * i) + dwNotifySize - 1;
        aPosNotify[i].hEventNotify = hNotifyEvent;             
    }
    
    // Tell DirectSound when to notify us. The notification will come in the from 
    // of signaled events that are handled in WinMain()
    if( FAILED( hr = pDSNotify->SetNotificationPositions( dwNotifyCount, 
                                                          aPosNotify ) ) )
    {
        SAFE_RELEASE( pDSNotify );
        SAFE_DELETE( aPosNotify );
        return DXTRACE_ERR_MSGBOX( TEXT("SetNotificationPositions"), hr );
    }

    SAFE_RELEASE( pDSNotify );
    SAFE_DELETE( aPosNotify );

    // Create the sound
    *ppStreamingSound = new CStreamingSound( pDSBuffer, dwDSBufferSize, pWaveFile, dwNotifySize );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSound::CSound()
// Desc: Constructs the class
//-----------------------------------------------------------------------------
CSound::CSound( LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize, 
                DWORD dwNumBuffers, CWaveFile* pWaveFile )
{
	DWORD i;

    m_apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
		m_pBufferParams = new DS3DBUFFER[dwNumBuffers];  
		m_apDS3DBuffer = new LPDIRECTSOUND3DBUFFER[dwNumBuffers];

    for( i=0; i<dwNumBuffers; i++ )
        m_apDSBuffer[i] = apDSBuffer[i];

    m_dwDSBufferSize = dwDSBufferSize;
    m_dwNumBuffers   = dwNumBuffers;
    m_pWaveFile      = pWaveFile;

    FillBufferWithSound( m_apDSBuffer[0], FALSE );

    // Make DirectSound do pre-processing on sound effects
    for( i=0; i<dwNumBuffers; i++ )
        m_apDSBuffer[i]->SetCurrentPosition(0);


}




//-----------------------------------------------------------------------------
// Name: CSound::~CSound()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CSound::~CSound()
{
	for( DWORD i=0; i<m_dwNumBuffers; i++ )
	{
		SAFE_RELEASE( m_apDSBuffer[i] ); 
		SAFE_RELEASE( m_apDS3DBuffer[i] );
	}
	SAFE_DELETE_ARRAY( m_apDSBuffer ); 
	SAFE_DELETE_ARRAY( m_apDS3DBuffer );
	SAFE_DELETE_ARRAY( m_pBufferParams );
	SAFE_DELETE( m_pWaveFile );
}




//-----------------------------------------------------------------------------
// Name: CSound::FillBufferWithSound()
// Desc: Fills a DirectSound buffer with a sound file 
//-----------------------------------------------------------------------------
HRESULT CSound::FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger )
{
    HRESULT hr; 
    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 

    if( pDSB == NULL )
        return CO_E_NOTINITIALIZED;

    // Make sure we have focus, and we didn't just switch in from
    // an app which had a DirectSound device
    if( FAILED( hr = RestoreBuffer( pDSB, NULL ) ) ) 
        return DXTRACE_ERR_MSGBOX( TEXT("RestoreBuffer"), hr );

    // Lock the buffer down
    if( FAILED( hr = pDSB->Lock( 0, m_dwDSBufferSize, 
                                 &pDSLockedBuffer, &dwDSLockedBufferSize, 
                                 NULL, NULL, 0L ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("Lock"), hr );

    // Reset the wave file to the beginning 
    m_pWaveFile->ResetFile();

    if( FAILED( hr = m_pWaveFile->Read( (BYTE*) pDSLockedBuffer,
                                        dwDSLockedBufferSize, 
                                        &dwWavDataRead ) ) )           
        return DXTRACE_ERR_MSGBOX( TEXT("Read"), hr );

    if( dwWavDataRead == 0 )
    {
        // Wav is blank, so just fill with silence
        FillMemory( (BYTE*) pDSLockedBuffer, 
                    dwDSLockedBufferSize, 
                    (BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
    }
    else if( dwWavDataRead < dwDSLockedBufferSize )
    {
        // If the wav file was smaller than the DirectSound buffer, 
        // we need to fill the remainder of the buffer with data 
        if( bRepeatWavIfBufferLarger )
        {       
            // Reset the file and fill the buffer with wav data
            DWORD dwReadSoFar = dwWavDataRead;    // From previous call above.
            while( dwReadSoFar < dwDSLockedBufferSize )
            {  
                // This will keep reading in until the buffer is full 
                // for very short files
                if( FAILED( hr = m_pWaveFile->ResetFile() ) )
                    return DXTRACE_ERR_MSGBOX( TEXT("ResetFile"), hr );

                hr = m_pWaveFile->Read( (BYTE*)pDSLockedBuffer + dwReadSoFar,
                                        dwDSLockedBufferSize - dwReadSoFar,
                                        &dwWavDataRead );
                if( FAILED(hr) )
                    return DXTRACE_ERR_MSGBOX( TEXT("Read"), hr );

                dwReadSoFar += dwWavDataRead;
            } 
        }
        else
        {
            // Don't repeat the wav file, just fill in silence 
            FillMemory( (BYTE*) pDSLockedBuffer + dwWavDataRead, 
                        dwDSLockedBufferSize - dwWavDataRead, 
                        (BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
        }
    }

    // Unlock the buffer, we don't need it anymore.
    pDSB->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSound::RestoreBuffer()
// Desc: Restores the lost buffer. *pbWasRestored returns TRUE if the buffer was 
//       restored.  It can also be NULL if the information is not needed.
//-----------------------------------------------------------------------------
HRESULT CSound::RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored )
{
    HRESULT hr;

    if( pDSB == NULL )
        return CO_E_NOTINITIALIZED;
    if( pbWasRestored )
        *pbWasRestored = FALSE;

    DWORD dwStatus;
    if( FAILED( hr = pDSB->GetStatus( &dwStatus ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("GetStatus"), hr );

    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        // Since the app could have just been activated, then DirectSound 
        // may not be giving us control yet, so restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
        do 
        {
            hr = pDSB->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
        }
        while( hr != DS_OK );

        if( pbWasRestored != NULL )
            *pbWasRestored = TRUE;

        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}




//-----------------------------------------------------------------------------
// Name: CSound::GetFreeBuffer()
// Desc: Checks to see if a buffer is playing and returns TRUE if it is.
//-----------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER CSound::GetFreeBuffer()
{
	return m_apDSBuffer[ GetFreeBufferIndex() ];
}
DWORD CSound::GetFreeBufferIndex()
{
    if( m_apDSBuffer == NULL )
        return FALSE; 

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
    {
        if( m_apDSBuffer[i] )
        {  
            DWORD dwStatus = 0;
            m_apDSBuffer[i]->GetStatus( &dwStatus );
            if ( ( dwStatus & DSBSTATUS_PLAYING ) == 0 )
                break;
        }
    }

    if( i != m_dwNumBuffers )
        return i;
    else
        return rand() % m_dwNumBuffers;
}



//-----------------------------------------------------------------------------
// Name: CSound::GetBuffer()
// Desc: 
//-----------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER CSound::GetBuffer( DWORD dwIndex )
{
    if( m_apDSBuffer == NULL )
        return NULL;
    if( dwIndex >= m_dwNumBuffers )
        return NULL;

    return m_apDSBuffer[dwIndex];
}




//-----------------------------------------------------------------------------
// Name: CSound::Get3DBufferInterface()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSound::Get3DBufferInterface( DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer )
{
    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;
    if( dwIndex >= m_dwNumBuffers )
        return E_INVALIDARG;

    *ppDS3DBuffer = NULL;

    return m_apDSBuffer[dwIndex]->QueryInterface( IID_IDirectSound3DBuffer, 
                                                  (VOID**)ppDS3DBuffer );
}
HRESULT CSound::Get3DBufferInterface( DWORD dwIndex )
{
	/*
    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;
    if( dwIndex >= m_dwNumBuffers )
        return E_INVALIDARG;

    //*ppDS3DBuffer = NULL;
		//m_pDS3DBuffer = NULL;

    return m_apDSBuffer[dwIndex]->QueryInterface( IID_IDirectSound3DBuffer, 
                                                  (VOID**)&m_pDS3DBuffer );
																									*/
	return 0;
}
HRESULT CSound::Get3DBufferInterface()
{
  if( m_apDSBuffer == NULL )
      return CO_E_NOTINITIALIZED;

	for( int i = 0; i < (int)m_dwNumBuffers ; i++ )
	{
		m_apDSBuffer[i]->QueryInterface( IID_IDirectSound3DBuffer, (VOID**)&m_apDS3DBuffer[i] );

		m_pBufferParams[i].dwSize = sizeof(DS3DBUFFER);
		m_apDS3DBuffer[i]->GetAllParameters( &m_pBufferParams[i] );
		m_pBufferParams[i].dwMode = DS3DMODE_NORMAL;//HEADRELATIVE;//DS3DMODE_HEADRELATIVE;
		m_apDS3DBuffer[i]->SetAllParameters( &m_pBufferParams[i], DS3D_IMMEDIATE );
	}

	return 0;
}
//-----------------------------------------------------------------------------
// Name: CSound::Play()
// Desc: Plays the sound using voice management flags.  Pass in DSBPLAY_LOOPING
//       in the dwFlags to loop the sound
//-----------------------------------------------------------------------------
HRESULT CSound::Play( DWORD dwPriority, DWORD dwFlags, DWORD* pdwIndex)
{
    HRESULT hr;
    BOOL    bRestored;

    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;

    //LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer();

		DWORD dwIndex;
		
		if( pdwIndex )
		  dwIndex = *pdwIndex;
		else
		  dwIndex = GetFreeBufferIndex();
		LPDIRECTSOUNDBUFFER pDSB = m_apDSBuffer[ dwIndex ];
		if( pdwIndex )
			*pdwIndex = dwIndex;

    if( pDSB == NULL )
        return DXTRACE_ERR_MSGBOX( TEXT("GetFreeBuffer"), E_FAIL );

    // Restore the buffer if it was lost
    if( FAILED( hr = RestoreBuffer( pDSB, &bRestored ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("RestoreBuffer"), hr );

    if( bRestored )
    {
        // The buffer was restored, so we need to fill it with new data
        if( FAILED( hr = FillBufferWithSound( pDSB, FALSE ) ) )
            return DXTRACE_ERR_MSGBOX( TEXT("FillBufferWithSound"), hr );

        // Make DirectSound do pre-processing on sound effects
        Reset();
    }

	return pDSB->Play( 0, dwPriority, dwFlags );
}




//-----------------------------------------------------------------------------
// Name: CSound::Stop()
// Desc: Stops the sound from playing
//-----------------------------------------------------------------------------
HRESULT CSound::Stop()
{
    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;

    HRESULT hr = 0;

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
        hr |= m_apDSBuffer[i]->Stop();

    return hr;
}




//-----------------------------------------------------------------------------
// Name: CSound::Reset()
// Desc: Reset all of the sound buffers
//-----------------------------------------------------------------------------
HRESULT CSound::Reset()
{
    if( m_apDSBuffer == NULL )
        return CO_E_NOTINITIALIZED;

    HRESULT hr = 0;

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
        hr |= m_apDSBuffer[i]->SetCurrentPosition( 0 );

    return hr;
}




//-----------------------------------------------------------------------------
// Name: CSound::IsSoundPlaying()
// Desc: Checks to see if a buffer is playing and returns TRUE if it is.
//-----------------------------------------------------------------------------
BOOL CSound::IsSoundPlaying()
{
    BOOL bIsPlaying = FALSE;

    if( m_apDSBuffer == NULL )
        return FALSE; 

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
    {
        if( m_apDSBuffer[i] )
        {  
            DWORD dwStatus = 0;
            m_apDSBuffer[i]->GetStatus( &dwStatus );
            bIsPlaying |= ( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
        }
    }

    return bIsPlaying;
}




//-----------------------------------------------------------------------------
// Name: CStreamingSound::CStreamingSound()
// Desc: Setups up a buffer so data can be streamed from the wave file into 
//       buffer.  This is very useful for large wav files that would take a 
//       while to load.  The buffer is initially filled with data, then 
//       as sound is played the notification events are signaled and more data
//       is written into the buffer by calling HandleWaveStreamNotification()
//-----------------------------------------------------------------------------
CStreamingSound::CStreamingSound( LPDIRECTSOUNDBUFFER pDSBuffer, DWORD dwDSBufferSize, 
                                  CWaveFile* pWaveFile, DWORD dwNotifySize ) 
                : CSound( &pDSBuffer, dwDSBufferSize, 1, pWaveFile )           
{
    m_dwLastPlayPos     = 0;
    m_dwPlayProgress    = 0;
    m_dwNotifySize      = dwNotifySize;
    m_dwNextWriteOffset = 0;
    m_bFillNextNotificationWithSilence = FALSE;
}




//-----------------------------------------------------------------------------
// Name: CStreamingSound::~CStreamingSound()
// Desc: Destroys the class
//-----------------------------------------------------------------------------
CStreamingSound::~CStreamingSound()
{
}



//-----------------------------------------------------------------------------
// Name: CStreamingSound::HandleWaveStreamNotification()
// Desc: Handle the notification that tell us to put more wav data in the 
//       circular buffer
//-----------------------------------------------------------------------------
HRESULT CStreamingSound::HandleWaveStreamNotification( BOOL bLoopedPlay )
{
    HRESULT hr;
    DWORD   dwCurrentPlayPos;
    DWORD   dwPlayDelta;
    DWORD   dwBytesWrittenToBuffer;
    VOID*   pDSLockedBuffer = NULL;
    VOID*   pDSLockedBuffer2 = NULL;
    DWORD   dwDSLockedBufferSize;
    DWORD   dwDSLockedBufferSize2;

    if( m_apDSBuffer == NULL || m_pWaveFile == NULL )
        return CO_E_NOTINITIALIZED;

    // Restore the buffer if it was lost
    BOOL bRestored;
    if( FAILED( hr = RestoreBuffer( m_apDSBuffer[0], &bRestored ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("RestoreBuffer"), hr );

    if( bRestored )
    {
        // The buffer was restored, so we need to fill it with new data
        if( FAILED( hr = FillBufferWithSound( m_apDSBuffer[0], FALSE ) ) )
            return DXTRACE_ERR_MSGBOX( TEXT("FillBufferWithSound"), hr );
        return S_OK;
    }

    // Lock the DirectSound buffer
    if( FAILED( hr = m_apDSBuffer[0]->Lock( m_dwNextWriteOffset, m_dwNotifySize, 
                                            &pDSLockedBuffer, &dwDSLockedBufferSize, 
                                            &pDSLockedBuffer2, &dwDSLockedBufferSize2, 0L ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("Lock"), hr );

    // m_dwDSBufferSize and m_dwNextWriteOffset are both multiples of m_dwNotifySize, 
    // it should the second buffer should never be valid
    if( pDSLockedBuffer2 != NULL )
        return E_UNEXPECTED; 

    if( !m_bFillNextNotificationWithSilence )
    {
        // Fill the DirectSound buffer with wav data
        if( FAILED( hr = m_pWaveFile->Read( (BYTE*) pDSLockedBuffer, 
                                                  dwDSLockedBufferSize, 
                                                  &dwBytesWrittenToBuffer ) ) )           
            return DXTRACE_ERR_MSGBOX( TEXT("Read"), hr );
    }
    else
    {
        // Fill the DirectSound buffer with silence
        FillMemory( pDSLockedBuffer, dwDSLockedBufferSize, 
                    (BYTE)( m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
        dwBytesWrittenToBuffer = dwDSLockedBufferSize;
    }

    // If the number of bytes written is less than the 
    // amount we requested, we have a short file.
    if( dwBytesWrittenToBuffer < dwDSLockedBufferSize )
    {
        if( !bLoopedPlay ) 
        {
            // Fill in silence for the rest of the buffer.
            FillMemory( (BYTE*) pDSLockedBuffer + dwBytesWrittenToBuffer, 
                        dwDSLockedBufferSize - dwBytesWrittenToBuffer, 
                        (BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );

            // Any future notifications should just fill the buffer with silence
            m_bFillNextNotificationWithSilence = TRUE;
        }
        else
        {
            // We are looping, so reset the file and fill the buffer with wav data
            DWORD dwReadSoFar = dwBytesWrittenToBuffer;    // From previous call above.
            while( dwReadSoFar < dwDSLockedBufferSize )
            {  
                // This will keep reading in until the buffer is full (for very short files).
                if( FAILED( hr = m_pWaveFile->ResetFile() ) )
                    return DXTRACE_ERR_MSGBOX( TEXT("ResetFile"), hr );

                if( FAILED( hr = m_pWaveFile->Read( (BYTE*)pDSLockedBuffer + dwReadSoFar,
                                                          dwDSLockedBufferSize - dwReadSoFar,
                                                          &dwBytesWrittenToBuffer ) ) )
                    return DXTRACE_ERR_MSGBOX( TEXT("Read"), hr );

                dwReadSoFar += dwBytesWrittenToBuffer;
            } 
        } 
    }

    // Unlock the DirectSound buffer
    m_apDSBuffer[0]->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

    // Figure out how much data has been played so far.  When we have played
    // passed the end of the file, we will either need to start filling the
    // buffer with silence or starting reading from the beginning of the file, 
    // depending if the user wants to loop the sound
    if( FAILED( hr = m_apDSBuffer[0]->GetCurrentPosition( &dwCurrentPlayPos, NULL ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("GetCurrentPosition"), hr );

    // Check to see if the position counter looped
    if( dwCurrentPlayPos < m_dwLastPlayPos )
        dwPlayDelta = ( m_dwDSBufferSize - m_dwLastPlayPos ) + dwCurrentPlayPos;
    else
        dwPlayDelta = dwCurrentPlayPos - m_dwLastPlayPos;

    m_dwPlayProgress += dwPlayDelta;
    m_dwLastPlayPos = dwCurrentPlayPos;

    // If we are now filling the buffer with silence, then we have found the end so 
    // check to see if the entire sound has played, if it has then stop the buffer.
    if( m_bFillNextNotificationWithSilence )
    {
        // We don't want to cut off the sound before it's done playing.
        if( m_dwPlayProgress >= m_pWaveFile->GetSize() )
        {
            m_apDSBuffer[0]->Stop();
        }
    }

    // Update where the buffer will lock (for next time)
    m_dwNextWriteOffset += dwDSLockedBufferSize; 
    m_dwNextWriteOffset %= m_dwDSBufferSize; // Circular buffer

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CStreamingSound::Reset()
// Desc: Resets the sound so it will begin playing at the beginning
//-----------------------------------------------------------------------------
HRESULT CStreamingSound::Reset()
{
    HRESULT hr;

    if( m_apDSBuffer[0] == NULL || m_pWaveFile == NULL )
        return CO_E_NOTINITIALIZED;

    m_dwLastPlayPos     = 0;
    m_dwPlayProgress    = 0;
    m_dwNextWriteOffset = 0;
    m_bFillNextNotificationWithSilence = FALSE;

    // Restore the buffer if it was lost
    BOOL bRestored;
    if( FAILED( hr = RestoreBuffer( m_apDSBuffer[0], &bRestored ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("RestoreBuffer"), hr );

    if( bRestored )
    {
        // The buffer was restored, so we need to fill it with new data
        if( FAILED( hr = FillBufferWithSound( m_apDSBuffer[0], FALSE ) ) )
            return DXTRACE_ERR_MSGBOX( TEXT("FillBufferWithSound"), hr );
    }

    m_pWaveFile->ResetFile();

    return m_apDSBuffer[0]->SetCurrentPosition( 0L );  
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::CWaveFile()
// Desc: Constructs the class.  Call Open() to open a wave file for reading.  
//       Then call Read() as needed.  Calling the destructor or Close() 
//       will close the file.  
//-----------------------------------------------------------------------------
CWaveFile::CWaveFile()
{
    m_pwfx    = NULL;
    m_hmmio   = NULL;
    m_pResourceBuffer = NULL;
    m_dwSize  = 0;
    m_bIsReadingFromMemory = FALSE;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::~CWaveFile()
// Desc: Destructs the class
//-----------------------------------------------------------------------------
CWaveFile::~CWaveFile()
{
    Close();

    if( !m_bIsReadingFromMemory )
        SAFE_DELETE_ARRAY( m_pwfx );
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::Open()
// Desc: Opens a wave file for reading
//-----------------------------------------------------------------------------
HRESULT CWaveFile::Open( LPTSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags )
{
    HRESULT hr;

    m_dwFlags = dwFlags;
    m_bIsReadingFromMemory = FALSE;

    if( m_dwFlags == WAVEFILE_READ )
    {
        if( strFileName == NULL )
            return E_INVALIDARG;
        SAFE_DELETE_ARRAY( m_pwfx );

        m_hmmio = mmioOpen( strFileName, NULL, MMIO_ALLOCBUF | MMIO_READ );

        if( NULL == m_hmmio )
        {
            HRSRC   hResInfo;
            HGLOBAL hResData;
            DWORD   dwSize;
            VOID*   pvRes;

            // Loading it as a file failed, so try it as a resource
            if( NULL == ( hResInfo = FindResource( NULL, strFileName, TEXT("WAVE") ) ) )
            {
                if( NULL == ( hResInfo = FindResource( NULL, strFileName, TEXT("WAV") ) ) )
                    return DXTRACE_ERR( TEXT("FindResource"), E_FAIL );
            }

            if( NULL == ( hResData = LoadResource( NULL, hResInfo ) ) )
                return DXTRACE_ERR_MSGBOX( TEXT("LoadResource"), E_FAIL );

            if( 0 == ( dwSize = SizeofResource( NULL, hResInfo ) ) ) 
                return DXTRACE_ERR_MSGBOX( TEXT("SizeofResource"), E_FAIL );

            if( NULL == ( pvRes = LockResource( hResData ) ) )
                return DXTRACE_ERR_MSGBOX( TEXT("LockResource"), E_FAIL );

            m_pResourceBuffer = new CHAR[ dwSize ];
            memcpy( m_pResourceBuffer, pvRes, dwSize );

            MMIOINFO mmioInfo;
            ZeroMemory( &mmioInfo, sizeof(mmioInfo) );
            mmioInfo.fccIOProc = FOURCC_MEM;
            mmioInfo.cchBuffer = dwSize;
            mmioInfo.pchBuffer = (CHAR*) m_pResourceBuffer;

            m_hmmio = mmioOpen( NULL, &mmioInfo, MMIO_ALLOCBUF | MMIO_READ );
        }

        if( FAILED( hr = ReadMMIO() ) )
        {
            // ReadMMIO will fail if its an not a wave file
            mmioClose( m_hmmio, 0 );
            return DXTRACE_ERR( TEXT("ReadMMIO"), hr );
        }

        if( FAILED( hr = ResetFile() ) )
            return DXTRACE_ERR_MSGBOX( TEXT("ResetFile"), hr );

        // After the reset, the size of the wav file is m_ck.cksize so store it now
        m_dwSize = m_ck.cksize;
    }
    else
    {
        m_hmmio = mmioOpen( strFileName, NULL, MMIO_ALLOCBUF  | 
                                                  MMIO_READWRITE | 
                                                  MMIO_CREATE );
        if( NULL == m_hmmio )
            return DXTRACE_ERR_MSGBOX( TEXT("mmioOpen"), E_FAIL );

        if( FAILED( hr = WriteMMIO( pwfx ) ) )
        {
            mmioClose( m_hmmio, 0 );
            return DXTRACE_ERR_MSGBOX( TEXT("WriteMMIO"), hr );
        }
                        
        if( FAILED( hr = ResetFile() ) )
            return DXTRACE_ERR_MSGBOX( TEXT("ResetFile"), hr );
    }

    return hr;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::OpenFromMemory()
// Desc: copy data to CWaveFile member variable from memory
//-----------------------------------------------------------------------------
HRESULT CWaveFile::OpenFromMemory( BYTE* pbData, ULONG ulDataSize, 
                                   WAVEFORMATEX* pwfx, DWORD dwFlags )
{
    m_pwfx       = pwfx;
    m_ulDataSize = ulDataSize;
    m_pbData     = pbData;
    m_pbDataCur  = m_pbData;
    m_bIsReadingFromMemory = TRUE;
    
    if( dwFlags != WAVEFILE_READ )
        return E_NOTIMPL;       
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::ReadMMIO()
// Desc: Support function for reading from a multimedia I/O stream.
//       m_hmmio must be valid before calling.  This function uses it to
//       update m_ckRiff, and m_pwfx. 
//-----------------------------------------------------------------------------
HRESULT CWaveFile::ReadMMIO()
{
    MMCKINFO        ckIn;           // chunk info. for general use.
    PCMWAVEFORMAT   pcmWaveFormat;  // Temp PCM structure to load in.       

    m_pwfx = NULL;

    if( ( 0 != mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("mmioDescend"), E_FAIL );

    // Check to make sure this is a valid wave file
    if( (m_ckRiff.ckid != FOURCC_RIFF) ||
        (m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
        return DXTRACE_ERR( TEXT("mmioFOURCC"), E_FAIL ); 

    // Search the input file for for the 'fmt ' chunk.
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if( 0 != mmioDescend( m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK ) )
        return DXTRACE_ERR_MSGBOX( TEXT("mmioDescend"), E_FAIL );

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them
       if( ckIn.cksize < (LONG) sizeof(PCMWAVEFORMAT) )
           return DXTRACE_ERR_MSGBOX( TEXT("sizeof(PCMWAVEFORMAT)"), E_FAIL );

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if( mmioRead( m_hmmio, (HPSTR) &pcmWaveFormat, 
                  sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat) )
        return DXTRACE_ERR_MSGBOX( TEXT("mmioRead"), E_FAIL );

    // Allocate the waveformatex, but if its not pcm format, read the next
    // word, and thats how many extra bytes to allocate.
    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {
        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
        if( NULL == m_pwfx )
            return DXTRACE_ERR_MSGBOX( TEXT("m_pwfx"), E_FAIL );

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = 0;
    }
    else
    {
        // Read in length of extra bytes.
        WORD cbExtraBytes = 0L;
        if( mmioRead( m_hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD) )
            return DXTRACE_ERR_MSGBOX( TEXT("mmioRead"), E_FAIL );

        m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) + cbExtraBytes ];
        if( NULL == m_pwfx )
            return DXTRACE_ERR_MSGBOX( TEXT("new"), E_FAIL );

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        m_pwfx->cbSize = cbExtraBytes;

        // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
        if( mmioRead( m_hmmio, (CHAR*)(((BYTE*)&(m_pwfx->cbSize))+sizeof(WORD)),
                      cbExtraBytes ) != cbExtraBytes )
        {
            SAFE_DELETE( m_pwfx );
            return DXTRACE_ERR_MSGBOX( TEXT("mmioRead"), E_FAIL );
        }
    }

    // Ascend the input file out of the 'fmt ' chunk.
    if( 0 != mmioAscend( m_hmmio, &ckIn, 0 ) )
    {
        SAFE_DELETE( m_pwfx );
        return DXTRACE_ERR_MSGBOX( TEXT("mmioAscend"), E_FAIL );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::GetSize()
// Desc: Retuns the size of the read access wave file 
//-----------------------------------------------------------------------------
DWORD CWaveFile::GetSize()
{
    return m_dwSize;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::ResetFile()
// Desc: Resets the internal m_ck pointer so reading starts from the 
//       beginning of the file again 
//-----------------------------------------------------------------------------
HRESULT CWaveFile::ResetFile()
{
    if( m_bIsReadingFromMemory )
    {
        m_pbDataCur = m_pbData;
    }
    else 
    {
        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;

        if( m_dwFlags == WAVEFILE_READ )
        {
            // Seek to the data
            if( -1 == mmioSeek( m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC),
                            SEEK_SET ) )
                return DXTRACE_ERR_MSGBOX( TEXT("mmioSeek"), E_FAIL );

            // Search the input file for the 'data' chunk.
            m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
            if( 0 != mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
              return DXTRACE_ERR_MSGBOX( TEXT("mmioDescend"), E_FAIL );
        }
        else
        {
            // Create the 'data' chunk that holds the waveform samples.  
            m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
            m_ck.cksize = 0;

            if( 0 != mmioCreateChunk( m_hmmio, &m_ck, 0 ) ) 
                return DXTRACE_ERR_MSGBOX( TEXT("mmioCreateChunk"), E_FAIL );

            if( 0 != mmioGetInfo( m_hmmio, &m_mmioinfoOut, 0 ) )
                return DXTRACE_ERR_MSGBOX( TEXT("mmioGetInfo"), E_FAIL );
        }
    }
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::Read()
// Desc: Reads section of data from a wave file into pBuffer and returns 
//       how much read in pdwSizeRead, reading not more than dwSizeToRead.
//       This uses m_ck to determine where to start reading from.  So 
//       subsequent calls will be continue where the last left off unless 
//       Reset() is called.
//-----------------------------------------------------------------------------
HRESULT CWaveFile::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{
    if( m_bIsReadingFromMemory )
    {
        if( m_pbDataCur == NULL )
            return CO_E_NOTINITIALIZED;
        if( pdwSizeRead != NULL )
            *pdwSizeRead = 0;

        if( (BYTE*)(m_pbDataCur + dwSizeToRead) > 
            (BYTE*)(m_pbData + m_ulDataSize) )
        {
            dwSizeToRead = m_ulDataSize - (DWORD)(m_pbDataCur - m_pbData);
        }
        
        CopyMemory( pBuffer, m_pbDataCur, dwSizeToRead );
        
        if( pdwSizeRead != NULL )
            *pdwSizeRead = dwSizeToRead;

        return S_OK;
    }
    else 
    {
        MMIOINFO mmioinfoIn; // current status of m_hmmio

        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;
        if( pBuffer == NULL || pdwSizeRead == NULL )
            return E_INVALIDARG;

        if( pdwSizeRead != NULL )
            *pdwSizeRead = 0;

        if( 0 != mmioGetInfo( m_hmmio, &mmioinfoIn, 0 ) )
            return DXTRACE_ERR_MSGBOX( TEXT("mmioGetInfo"), E_FAIL );
                
        UINT cbDataIn = dwSizeToRead;
        if( cbDataIn > m_ck.cksize ) 
            cbDataIn = m_ck.cksize;       

        m_ck.cksize -= cbDataIn;
    
        for( DWORD cT = 0; cT < cbDataIn; cT++ )
        {
            // Copy the bytes from the io to the buffer.
            if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
            {
                if( 0 != mmioAdvance( m_hmmio, &mmioinfoIn, MMIO_READ ) )
                    return DXTRACE_ERR_MSGBOX( TEXT("mmioAdvance"), E_FAIL );

                if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
                    return DXTRACE_ERR_MSGBOX( TEXT("mmioinfoIn.pchNext"), E_FAIL );
            }

            // Actual copy.
            *((BYTE*)pBuffer+cT) = *((BYTE*)mmioinfoIn.pchNext);
            mmioinfoIn.pchNext++;
        }

        if( 0 != mmioSetInfo( m_hmmio, &mmioinfoIn, 0 ) )
            return DXTRACE_ERR_MSGBOX( TEXT("mmioSetInfo"), E_FAIL );

        if( pdwSizeRead != NULL )
            *pdwSizeRead = cbDataIn;

        return S_OK;
    }
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::Close()
// Desc: Closes the wave file 
//-----------------------------------------------------------------------------
HRESULT CWaveFile::Close()
{
    if( m_dwFlags == WAVEFILE_READ )
    {
        mmioClose( m_hmmio, 0 );
        m_hmmio = NULL;
        SAFE_DELETE_ARRAY( m_pResourceBuffer );
    }
    else
    {
        m_mmioinfoOut.dwFlags |= MMIO_DIRTY;

        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;

        if( 0 != mmioSetInfo( m_hmmio, &m_mmioinfoOut, 0 ) )
            return DXTRACE_ERR_MSGBOX( TEXT("mmioSetInfo"), E_FAIL );
    
        // Ascend the output file out of the 'data' chunk -- this will cause
        // the chunk size of the 'data' chunk to be written.
        if( 0 != mmioAscend( m_hmmio, &m_ck, 0 ) )
            return DXTRACE_ERR_MSGBOX( TEXT("mmioAscend"), E_FAIL );
    
        // Do this here instead...
        if( 0 != mmioAscend( m_hmmio, &m_ckRiff, 0 ) )
            return DXTRACE_ERR_MSGBOX( TEXT("mmioAscend"), E_FAIL );
        
        mmioSeek( m_hmmio, 0, SEEK_SET );

        if( 0 != (INT)mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) )
            return DXTRACE_ERR_MSGBOX( TEXT("mmioDescend"), E_FAIL );
    
        m_ck.ckid = mmioFOURCC('f', 'a', 'c', 't');

        if( 0 == mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) ) 
        {
            DWORD dwSamples = 0;
            mmioWrite( m_hmmio, (HPSTR)&dwSamples, sizeof(DWORD) );
            mmioAscend( m_hmmio, &m_ck, 0 ); 
        }
    
        // Ascend the output file out of the 'RIFF' chunk -- this will cause
        // the chunk size of the 'RIFF' chunk to be written.
        if( 0 != mmioAscend( m_hmmio, &m_ckRiff, 0 ) )
            return DXTRACE_ERR_MSGBOX( TEXT("mmioAscend"), E_FAIL );
    
        mmioClose( m_hmmio, 0 );
        m_hmmio = NULL;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::WriteMMIO()
// Desc: Support function for reading from a multimedia I/O stream
//       pwfxDest is the WAVEFORMATEX for this new wave file.  
//       m_hmmio must be valid before calling.  This function uses it to
//       update m_ckRiff, and m_ck.  
//-----------------------------------------------------------------------------
HRESULT CWaveFile::WriteMMIO( WAVEFORMATEX *pwfxDest )
{
    DWORD    dwFactChunk; // Contains the actual fact chunk. Garbage until WaveCloseWriteFile.
    MMCKINFO ckOut1;
    
    dwFactChunk = (DWORD)-1;

    // Create the output file RIFF chunk of form type 'WAVE'.
    m_ckRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');       
    m_ckRiff.cksize = 0;

    if( 0 != mmioCreateChunk( m_hmmio, &m_ckRiff, MMIO_CREATERIFF ) )
        return DXTRACE_ERR_MSGBOX( TEXT("mmioCreateChunk"), E_FAIL );
    
    // We are now descended into the 'RIFF' chunk we just created.
    // Now create the 'fmt ' chunk. Since we know the size of this chunk,
    // specify it in the MMCKINFO structure so MMIO doesn't have to seek
    // back and set the chunk size after ascending from the chunk.
    m_ck.ckid = mmioFOURCC('f', 'm', 't', ' ');
    m_ck.cksize = sizeof(PCMWAVEFORMAT);   

    if( 0 != mmioCreateChunk( m_hmmio, &m_ck, 0 ) )
        return DXTRACE_ERR_MSGBOX( TEXT("mmioCreateChunk"), E_FAIL );
    
    // Write the PCMWAVEFORMAT structure to the 'fmt ' chunk if its that type. 
    if( pwfxDest->wFormatTag == WAVE_FORMAT_PCM )
    {
        if( mmioWrite( m_hmmio, (HPSTR) pwfxDest, 
                       sizeof(PCMWAVEFORMAT)) != sizeof(PCMWAVEFORMAT))
            return DXTRACE_ERR_MSGBOX( TEXT("mmioWrite"), E_FAIL );
    }   
    else 
    {
        // Write the variable length size.
        if( (UINT)mmioWrite( m_hmmio, (HPSTR) pwfxDest, 
                             sizeof(*pwfxDest) + pwfxDest->cbSize ) != 
                             ( sizeof(*pwfxDest) + pwfxDest->cbSize ) )
            return DXTRACE_ERR_MSGBOX( TEXT("mmioWrite"), E_FAIL );
    }  
    
    // Ascend out of the 'fmt ' chunk, back into the 'RIFF' chunk.
    if( 0 != mmioAscend( m_hmmio, &m_ck, 0 ) )
        return DXTRACE_ERR_MSGBOX( TEXT("mmioAscend"), E_FAIL );
    
    // Now create the fact chunk, not required for PCM but nice to have.  This is filled
    // in when the close routine is called.
    ckOut1.ckid = mmioFOURCC('f', 'a', 'c', 't');
    ckOut1.cksize = 0;

    if( 0 != mmioCreateChunk( m_hmmio, &ckOut1, 0 ) )
        return DXTRACE_ERR_MSGBOX( TEXT("mmioCreateChunk"), E_FAIL );
    
    if( mmioWrite( m_hmmio, (HPSTR)&dwFactChunk, sizeof(dwFactChunk)) != 
                    sizeof(dwFactChunk) )
         return DXTRACE_ERR_MSGBOX( TEXT("mmioWrite"), E_FAIL );
    
    // Now ascend out of the fact chunk...
    if( 0 != mmioAscend( m_hmmio, &ckOut1, 0 ) )
        return DXTRACE_ERR_MSGBOX( TEXT("mmioAscend"), E_FAIL );
       
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CWaveFile::Write()
// Desc: Writes data to the open wave file
//-----------------------------------------------------------------------------
HRESULT CWaveFile::Write( UINT nSizeToWrite, BYTE* pbSrcData, UINT* pnSizeWrote )
{
    UINT cT;

    if( m_bIsReadingFromMemory )
        return E_NOTIMPL;
    if( m_hmmio == NULL )
        return CO_E_NOTINITIALIZED;
    if( pnSizeWrote == NULL || pbSrcData == NULL )
        return E_INVALIDARG;

    *pnSizeWrote = 0;
    
    for( cT = 0; cT < nSizeToWrite; cT++ )
    {       
        if( m_mmioinfoOut.pchNext == m_mmioinfoOut.pchEndWrite )
        {
            m_mmioinfoOut.dwFlags |= MMIO_DIRTY;
            if( 0 != mmioAdvance( m_hmmio, &m_mmioinfoOut, MMIO_WRITE ) )
                return DXTRACE_ERR_MSGBOX( TEXT("mmioAdvance"), E_FAIL );
        }

        *((BYTE*)m_mmioinfoOut.pchNext) = *((BYTE*)pbSrcData+cT);
        (BYTE*)m_mmioinfoOut.pchNext++;

        (*pnSizeWrote)++;
    }

    return S_OK;
}

