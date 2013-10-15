// Cloth.cpp: implementation of the CCloth class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Cloth.h"

#ifdef __YCLOTH

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCloth::CCloth()
{
	m_nGridSize = 0;
	
	m_nNumBalls = 0;
	m_pBalls1   = NULL;
	m_pBalls2   = NULL;
	
	m_pCurrentBalls = NULL;
	m_pNextBalls    = NULL;

	m_v3Gravity = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	
	m_fSpringConstant = 0;
	m_fNaturalLength  = 0;
	m_fMass = 0;
	m_fDampFactor = 0;
	
	m_nNumSprings = 0;
	m_pSprings    = NULL;
	
	m_nPatchTesselation = 0;

	m_pTexture = NULL;

	m_dStartTime = 0;
}

CCloth::~CCloth()
{
	DeleteDeviceObjects();
}

void CCloth::ResetCloth(D3DXMATRIX *pWorldM)
{
	//Initialise the balls in an evenly spaced grid in the x-z plane
	for(int i=0; i<m_nGridSize; ++i)
	{
		for(int j=0; j<m_nGridSize; ++j)
		{
			//m_pBalls1[i*m_nGridSize+j].m_v3Pos  = D3DXVECTOR3( float(j)-float(m_nGridSize-1)/2, 8.5f, float(i)-float(m_nGridSize-1)/2);
			//D3DXVec3TransformCoord( &m_pBalls1[i*m_nGridSize+j].m_v3Pos, &D3DXVECTOR3( float(j)-float(m_nGridSize-1)/2, 0.0f, float(i)-float(m_nGridSize-1)/2), pWorldM );
			D3DXVec3TransformCoord( &m_pBalls1[i*m_nGridSize+j].m_v3Pos, 
									&D3DXVECTOR3( float(j*0.005f)-float((m_nGridSize-1)*0.005f)/2, 0.0f, float(i*0.005f)-float((m_nGridSize-1)*0.005f)/2), pWorldM );
			m_pBalls1[i*m_nGridSize+j].m_v3Velo = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_pBalls1[i*m_nGridSize+j].m_fMass  = m_fMass;
			m_pBalls1[i*m_nGridSize+j].m_bFixed = false;
		}
	}

	//Fix the top left & top right balls in place
/*	balls1[0].fixed=true;
	balls1[gridSize-1].fixed=true;
	
	//Fix the bottom left & bottom right balls
	balls1[gridSize*(gridSize-1)].fixed=true;
	balls1[gridSize*gridSize-1].fixed=true;
*/
	
	//Copy the balls into the other array
	for(i=0; i<m_nNumBalls; ++i)
		m_pBalls2[i] = m_pBalls1[i];

	//Set the currentBalls and nextBalls pointers
	m_pCurrentBalls = m_pBalls1;
	m_pNextBalls    = m_pBalls2;

	//Initialise the springs
	SPRING * currentSpring=&m_pSprings[0];

	//The first (gridSize-1)*gridSize springs go from one ball to the next,
	//excluding those on the right hand edge
	for(i=0; i<m_nGridSize; ++i)
	{
		for(int j=0; j<m_nGridSize-1; ++j)
		{
			currentSpring->m_nBall1 = i*m_nGridSize+j;
			currentSpring->m_nBall2 = i*m_nGridSize+j+1;

			currentSpring->m_fSpringConstant = m_fSpringConstant;
			currentSpring->m_fNaturalLength  = m_fNaturalLength;
			
			++currentSpring;
		}
	}

	//The next (gridSize-1)*gridSize springs go from one ball to the one below,
	//excluding those on the bottom edge
	for(i=0; i<m_nGridSize-1; ++i)
	{
		for(int j=0; j<m_nGridSize; ++j)
		{
			currentSpring->m_nBall1 = i*m_nGridSize+j;
			currentSpring->m_nBall2 = (i+1)*m_nGridSize+j;

			currentSpring->m_fSpringConstant = m_fSpringConstant;
			currentSpring->m_fNaturalLength  = m_fNaturalLength;
			
			++currentSpring;
		}
	}

	//The next (gridSize-1)*(gridSize-1) go from a ball to the one below and right
	//excluding those on the bottom or right
	for(i=0; i<m_nGridSize-1; ++i)
	{
		for(int j=0; j<m_nGridSize-1; ++j)
		{
			currentSpring->m_nBall1 = i*m_nGridSize+j;
			currentSpring->m_nBall2 = (i+1)*m_nGridSize+j+1;

			currentSpring->m_fSpringConstant = m_fSpringConstant;
			currentSpring->m_fNaturalLength  = m_fNaturalLength*float(sqrt(2));

			++currentSpring;
		}
	}

	//The next (gridSize-1)*(gridSize-1) go from a ball to the one below and left
	//excluding those on the bottom or right
	for(i=0; i<m_nGridSize-1; ++i)
	{
		for(int j=1; j<m_nGridSize; ++j)
		{
			currentSpring->m_nBall1=i*m_nGridSize+j;
			currentSpring->m_nBall2=(i+1)*m_nGridSize+j-1;

			currentSpring->m_fSpringConstant = m_fSpringConstant;
			currentSpring->m_fNaturalLength  = m_fNaturalLength*float(sqrt(2));
			
			++currentSpring;
		}
	}

	//The first (gridSize-2)*gridSize springs go from one ball to the next but one,
	//excluding those on or next to the right hand edge
	for(i=0; i<m_nGridSize; ++i)
	{
		for(int j=0; j<m_nGridSize-2; ++j)
		{
			currentSpring->m_nBall1=i*m_nGridSize+j;
			currentSpring->m_nBall2=i*m_nGridSize+j+2;

			currentSpring->m_fSpringConstant = m_fSpringConstant;
			currentSpring->m_fNaturalLength  = m_fNaturalLength*2;
			
			++currentSpring;
		}
	}

	//The next (gridSize-2)*gridSize springs go from one ball to the next but one below,
	//excluding those on or next to the bottom edge
	for(i=0; i<m_nGridSize-2; ++i)
	{
		for(int j=0; j<m_nGridSize; ++j)
		{
			currentSpring->m_nBall1=i*m_nGridSize+j;
			currentSpring->m_nBall2=(i+2)*m_nGridSize+j;

			currentSpring->m_fSpringConstant = m_fSpringConstant;
			currentSpring->m_fNaturalLength  = m_fNaturalLength*2;
			
			++currentSpring;
		}
	}
}

void CCloth::Process(float f, D3DXVECTOR3 v3Pos1, D3DXVECTOR3 v3Pos2 )
{

	static double lastTime = ((double)timeGetTime())-m_dStartTime;
	double currentTime     = ((double)timeGetTime())-m_dStartTime;
	double timePassed      = currentTime-lastTime;
	lastTime = currentTime;

	m_pCurrentBalls[m_nGridSize*(m_nGridSize-1)].m_bFixed = true;
	m_pCurrentBalls[m_nGridSize*(m_nGridSize-1)].m_v3Pos = v3Pos1;
	m_pCurrentBalls[m_nGridSize*m_nGridSize-1].m_bFixed = true;
	m_pCurrentBalls[m_nGridSize*m_nGridSize-1].m_v3Pos = v3Pos2;
	
/*
	if ( GetAsyncKeyState('1') )
		m_pCurrentBalls[0].m_bFixed=false;

	if ( GetAsyncKeyState('2') )
		m_pCurrentBalls[m_nGridSize-1].m_bFixed=false;
	
	if ( GetAsyncKeyState('3') )
		m_pCurrentBalls[m_nGridSize*(m_nGridSize-1)].m_bFixed=false;

	if ( GetAsyncKeyState('4') )
		m_pCurrentBalls[m_nGridSize*m_nGridSize-1].m_bFixed=false;

	static float fmovez = 0.0f;
	static float fmovey = 8.0f;
	static float fmovex1 = m_pCurrentBalls[0].m_v3Pos.x;
	static float fmovex2 = m_pCurrentBalls[m_nGridSize-1].m_v3Pos.x;
	
	m_pCurrentBalls[m_nGridSize*(m_nGridSize-1)].m_bFixed = true;
	m_pCurrentBalls[m_nGridSize*(m_nGridSize-1)].m_v3Pos.x = fmovex1;
	m_pCurrentBalls[m_nGridSize*(m_nGridSize-1)].m_v3Pos.y = fmovey;
	m_pCurrentBalls[m_nGridSize*m_nGridSize-1].m_bFixed = true;
	m_pCurrentBalls[m_nGridSize*m_nGridSize-1].m_v3Pos.x = fmovex2;
	m_pCurrentBalls[m_nGridSize*m_nGridSize-1].m_v3Pos.y = fmovey;


	m_pCurrentBalls[m_nGridSize*(m_nGridSize-1)].m_v3Pos.z = fmovez;
	m_pCurrentBalls[m_nGridSize*m_nGridSize-1].m_v3Pos.z = fmovez;
	
	if( GetAsyncKeyState('W') )
	{
		fmovey += 0.05f;
	}
	if( GetAsyncKeyState('S') )
	{
		fmovey -= 0.05f;
	}
	if( GetAsyncKeyState('A') )
	{
		fmovex1 -= 0.05f;
		fmovex2 -= 0.05f;
	}
	if( GetAsyncKeyState('D') )
	{
		fmovex1 += 0.05f;
		fmovex2 += 0.05f;
	}
	if( GetAsyncKeyState('Q') )
	{
		fmovez += 0.05f;
	}
	if( GetAsyncKeyState('E') )
	{
		fmovez -= 0.05f;
	}
/**/
	BOOL bUpdateMade = false;
	static double timeSinceLastUpdate=0.0;
	timeSinceLastUpdate+=timePassed;
	
	while(timeSinceLastUpdate>10.0)
	{
		timeSinceLastUpdate-=10.0;
		float timePassedInSeconds=0.01f;

		bUpdateMade = true;

		//Calculate the tensions in the springs
		for(int i=0; i<m_nNumSprings; ++i)
		{
			D3DXVECTOR3 v3temp = m_pCurrentBalls[m_pSprings[i].m_nBall1].m_v3Pos - m_pCurrentBalls[m_pSprings[i].m_nBall2].m_v3Pos;

			float springLength;
			springLength = D3DXVec3Length( &v3temp );

			float extension=springLength-m_pSprings[i].m_fNaturalLength;

			m_pSprings[i].m_fTension=m_pSprings[i].m_fSpringConstant*extension/m_pSprings[i].m_fNaturalLength;
		}

		//Calculate the nextBalls from the currentBalls
		for(i=0; i<m_nNumBalls; ++i)
		{
			//Transfer properties which do not change
			m_pNextBalls[i].m_bFixed=m_pCurrentBalls[i].m_bFixed;
			m_pNextBalls[i].m_fMass=m_pCurrentBalls[i].m_fMass;

			//If the ball is fixed, transfer the position and zero the velocity, otherwise calculate
			//the new values
			if(m_pCurrentBalls[i].m_bFixed)
			{
				m_pNextBalls[i].m_v3Pos  = m_pCurrentBalls[i].m_v3Pos;
				m_pNextBalls[i].m_v3Velo = D3DXVECTOR3(0.0f,0.0f,0.0f);
			}
			else
			{
				//Calculate the force on this ball
				D3DXVECTOR3 force=m_v3Gravity;

				//Loop through springs
				for(int j=0; j<m_nNumSprings; ++j)
				{
					//If this ball is "ball1" for this spring, add the tension to the force
					if(m_pSprings[j].m_nBall1==i)
					{
						D3DXVECTOR3 tensionDirection = m_pCurrentBalls[m_pSprings[j].m_nBall2].m_v3Pos - m_pCurrentBalls[i].m_v3Pos;
						
						D3DXVec3Normalize( &tensionDirection, &tensionDirection );

						force+=m_pSprings[j].m_fTension*tensionDirection;
					}

					//Similarly if the ball is "ball2"
					if(m_pSprings[j].m_nBall2==i)
					{
						D3DXVECTOR3 tensionDirection = m_pCurrentBalls[m_pSprings[j].m_nBall1].m_v3Pos - m_pCurrentBalls[i].m_v3Pos;

						D3DXVec3Normalize( &tensionDirection, &tensionDirection );
						
						force+=m_pSprings[j].m_fTension*tensionDirection;
					}
				}

				//Calculate the acceleration
				D3DXVECTOR3 acceleration=force/m_pCurrentBalls[i].m_fMass;
			
				//Update velocity
				m_pNextBalls[i].m_v3Velo=m_pCurrentBalls[i].m_v3Velo+acceleration*
																	(float)timePassedInSeconds;

				//Damp the velocity
				m_pNextBalls[i].m_v3Velo*=m_fDampFactor;
			
				//Calculate new position
				m_pNextBalls[i].m_v3Pos=m_pCurrentBalls[i].m_v3Pos+
					(m_pNextBalls[i].m_v3Velo+m_pCurrentBalls[i].m_v3Velo)*(float)timePassedInSeconds/2;

				//Check against sphere (at origin)
				float flength;
				D3DXVECTOR3 v3Temp, v3Normal;
				flength = D3DXVec3LengthSq( &m_pNextBalls[i].m_v3Pos );
				if(flength<4.0f*1.08f*4.0f*1.08f)
				{
					//D3DXVec3Normalize( &v3Normal, &m_pNextBalls[i].m_v3Pos );
					//m_pNextBalls[i].m_v3Pos= v3Normal*4.0f*1.08f;
				}

				//Check against floor
				//if(nextBalls[i].position.y<-8.5f)
				//	nextBalls[i].position.y=-8.5f;
			}
		}

		//Swap the currentBalls and newBalls pointers
		BALL * temp=m_pCurrentBalls;
		m_pCurrentBalls=m_pNextBalls;
		m_pNextBalls=m_pCurrentBalls;
	}

	if(bUpdateMade)
	{
		//Zero the normals on each ball
		for(int i=0; i<m_nNumBalls; ++i)
			m_pCurrentBalls[i].m_v3Normal = D3DXVECTOR3(0.0f,0.0f,0.0f);

		//Calculate the normals on the current balls
		for(i=0; i<m_nGridSize-1; ++i)
		{
			for(int j=0; j<m_nGridSize-1; ++j)
			{
				D3DXVECTOR3 & p0=m_pCurrentBalls[i*m_nGridSize+j].m_v3Pos;
				D3DXVECTOR3 & p1=m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Pos;
				D3DXVECTOR3 & p2=m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Pos;
				D3DXVECTOR3 & p3=m_pCurrentBalls[(i+1)*m_nGridSize+j+1].m_v3Pos;

				D3DXVECTOR3 & n0=m_pCurrentBalls[i*m_nGridSize+j].m_v3Normal;
				D3DXVECTOR3 & n1=m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Normal;
				D3DXVECTOR3 & n2=m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Normal;
				D3DXVECTOR3 & n3=m_pCurrentBalls[(i+1)*m_nGridSize+j+1].m_v3Normal;

				//Calculate the normals for the 2 triangles and add on
				D3DXVECTOR3 normal=(p1-p0);
				D3DXVECTOR3 normal2=(p2-p0);
				D3DXVec3Cross( &normal, &normal, &normal2 );

				n0+=normal;
				n1+=normal;
				n2+=normal;

				normal=(p1-p2);
				normal2=(p3-p2);
				D3DXVec3Cross( &normal, &normal, &normal2 );
				
				n1+=normal;
				n2+=normal;
				n3+=normal;
			}
		}

		//Normalize normals
		for(i=0; i<m_nNumBalls; ++i)
			D3DXVec3Normalize( &m_pCurrentBalls[i].m_v3Normal, &m_pCurrentBalls[i].m_v3Normal );
	}
}

void CCloth::Render(LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* pWorldM )
{
	if( m_pTexture )
		pd3dDevice->SetTexture(0, m_pTexture );
	else
		pd3dDevice->SetTexture(0, NULL );

	pd3dDevice->SetFVF( CUSTOM_VERTEX::FVF );
	
	int square = m_nGridSize;

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	
	for(int i=0; i<m_nGridSize-1; ++i)
	{
		for(int j=0; j<m_nGridSize-1; ++j)
		{
			CUSTOM_VERTEX v3Pos[6];
			
			v3Pos[0].m_v3Pos = (m_pCurrentBalls[i*m_nGridSize+j].m_v3Pos);
			v3Pos[1].m_v3Pos = (m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Pos);
			v3Pos[2].m_v3Pos = (m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Pos);
			v3Pos[3].m_v3Pos = (m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Pos);
			v3Pos[4].m_v3Pos  = ( m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Pos );
			v3Pos[5].m_v3Pos  = ( m_pCurrentBalls[(i+1)*m_nGridSize+j+1].m_v3Pos );
			
			v3Pos[0].m_v3Nor = (m_pCurrentBalls[i*m_nGridSize+j].m_v3Normal);
			v3Pos[1].m_v3Nor = (m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Normal);
			v3Pos[2].m_v3Nor = (m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Normal);
			v3Pos[3].m_v3Nor = (m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Normal);
			v3Pos[4].m_v3Nor  = ( m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Normal );
			v3Pos[5].m_v3Nor  = ( m_pCurrentBalls[(i+1)*m_nGridSize+j+1].m_v3Normal );

/*
			D3DXVec3TransformCoord( &v3Pos[0].m_v3Pos, &v3Pos[0].m_v3Pos, pWorldM );
			D3DXVec3TransformCoord( &v3Pos[1].m_v3Pos, &v3Pos[1].m_v3Pos, pWorldM );
			D3DXVec3TransformCoord( &v3Pos[2].m_v3Pos, &v3Pos[2].m_v3Pos, pWorldM );
			D3DXVec3TransformCoord( &v3Pos[3].m_v3Pos, &v3Pos[3].m_v3Pos, pWorldM );
			D3DXVec3TransformCoord( &v3Pos[4].m_v3Pos, &v3Pos[4].m_v3Pos, pWorldM );
			D3DXVec3TransformCoord( &v3Pos[5].m_v3Pos, &v3Pos[5].m_v3Pos, pWorldM );

			D3DXVec3TransformCoord( &v3Pos[0].m_v3Nor, &v3Pos[0].m_v3Nor, pWorldM );
			D3DXVec3TransformCoord( &v3Pos[1].m_v3Nor, &v3Pos[1].m_v3Nor, pWorldM );
			D3DXVec3TransformCoord( &v3Pos[2].m_v3Nor, &v3Pos[2].m_v3Nor, pWorldM );
			D3DXVec3TransformCoord( &v3Pos[3].m_v3Nor, &v3Pos[3].m_v3Nor, pWorldM );
			D3DXVec3TransformCoord( &v3Pos[4].m_v3Nor, &v3Pos[4].m_v3Nor, pWorldM );
			D3DXVec3TransformCoord( &v3Pos[5].m_v3Nor, &v3Pos[5].m_v3Nor, pWorldM );
/**/
			v3Pos[0].m_dwColor = D3DCOLOR_XRGB( 0, 255, 255 );
			v3Pos[1].m_dwColor = D3DCOLOR_XRGB( 0, 255, 255 );
			v3Pos[2].m_dwColor = D3DCOLOR_XRGB( 0, 255, 255 );
			v3Pos[3].m_dwColor = D3DCOLOR_XRGB( 0, 255, 255 );
			v3Pos[4].m_dwColor = D3DCOLOR_XRGB( 0, 255, 255 );
			v3Pos[5].m_dwColor = D3DCOLOR_XRGB( 0, 255, 255 );
			
			
			v3Pos[0].m_v2uv  = D3DXVECTOR2( float(j)/(square-1),  float(i)/(square-1) );
			v3Pos[1].m_v2uv  = D3DXVECTOR2( float(j+1)/(square-1),float(i)/(square-1) );
			v3Pos[2].m_v2uv  = D3DXVECTOR2( float(j)/(square-1),  float(i+1)/(square-1) );
			v3Pos[3].m_v2uv  = D3DXVECTOR2( float(j)/(square-1),  float(i+1)/(square-1) );
			v3Pos[4].m_v2uv  = D3DXVECTOR2( float(j+1)/(square-1),  float(i)/(square-1) );
			v3Pos[5].m_v2uv  = D3DXVECTOR2( float(j+1)/(square-1),  float(i+1)/(square-1) );
			
			pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, v3Pos, sizeof(CUSTOM_VERTEX) );
		}
	}

	for(i=0; i<m_nNumSprings; ++i)
	{
		//Check the spring has been initialised and the ball numbers are in bounds
		if(	m_pSprings[i].m_nBall1!=-1 && m_pSprings[i].m_nBall2!=-1	&&
			m_pSprings[i].m_nBall1<m_nNumSprings && m_pSprings[i].m_nBall2<m_nNumSprings)
		{
			CUSTOM_VERTEX v3Pos[2];
			
			v3Pos[0].m_v3Pos = (m_pCurrentBalls[m_pSprings[i].m_nBall1].m_v3Pos);
			v3Pos[1].m_v3Pos = (m_pCurrentBalls[m_pSprings[i].m_nBall2].m_v3Pos);
			pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, v3Pos, sizeof(CUSTOM_VERTEX) );
		}
	}
}

void CCloth::DeleteDeviceObjects()
{
	SAFE_DELETE_ARRAY( m_pBalls1 );
	SAFE_DELETE_ARRAY( m_pBalls2 );
	SAFE_DELETE_ARRAY( m_pSprings );
}

void CCloth::Init( int nGridSize, D3DXVECTOR3 v3Gravity, FLOAT fSpringConstant, 
				   FLOAT fNaturalLength, FLOAT fMass, FLOAT fDampFactor, int nPatchTesselation,
				   LPDIRECT3DTEXTURE9 pTexture, D3DXMATRIX* pWorldM )
{
	m_nGridSize = nGridSize;
	
	m_v3Gravity = v3Gravity;
	
	m_fSpringConstant = fSpringConstant;
	m_fNaturalLength  = fNaturalLength;
	
	m_fMass = fMass;
	
	m_fDampFactor = fDampFactor;
	
	m_nPatchTesselation = nPatchTesselation;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Calculate number of balls
	m_nNumBalls = m_nGridSize * m_nGridSize;
	
	//Calculate number of springs
	//There is a spring pointing right for each ball which is not on the right edge, 
	//and one pointing down for each ball not on the bottom edge
	m_nNumSprings=(m_nGridSize-1)*m_nGridSize*2;
	
	//There is a spring pointing down & right for each ball not on bottom or right,
	//and one pointing down & left for each ball not on bottom or left
	m_nNumSprings+=(m_nGridSize-1)*(m_nGridSize-1)*2;
	
	//There is a spring pointing right (to the next but one ball)
	//for each ball which is not on or next to the right edge, 
	//and one pointing down for each ball not on or next to the bottom edge
	m_nNumSprings+=(m_nGridSize-2)*m_nGridSize*2;
	
	//Create space for balls & springs
	m_pBalls1=new BALL[m_nNumBalls];
	m_pBalls2=new BALL[m_nNumBalls];
	m_pSprings=new SPRING[m_nNumSprings];

	if(!m_pBalls1 || !m_pBalls2 || !m_pSprings)
	{
		//TRACE( "CCloth::Init" );
		return;
	}
	
	m_dStartTime = (double)timeGetTime();
	m_pTexture = pTexture;

	//Reset cloth
	ResetCloth(pWorldM);
/*
	for(int i=0; i<m_nGridSize-1; ++i)
	{
		for(int j=0; j<m_nGridSize-1; ++j)
		{
			D3DXVec3TransformCoord( &m_pCurrentBalls[i*m_nGridSize+j].m_v3Pos, &m_pCurrentBalls[i*m_nGridSize+j].m_v3Pos, pWorldM );
			D3DXVec3TransformCoord( &m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Pos, &m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Pos, pWorldM );
			D3DXVec3TransformCoord( &m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Pos, &m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Pos, pWorldM );
			D3DXVec3TransformCoord( &m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Pos, &m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Pos, pWorldM );
			D3DXVec3TransformCoord( &m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Pos, &m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Pos, pWorldM );
			D3DXVec3TransformCoord( &m_pCurrentBalls[(i+1)*m_nGridSize+j+1].m_v3Pos, &m_pCurrentBalls[(i+1)*m_nGridSize+j+1].m_v3Pos, pWorldM );
			
			D3DXVec3TransformCoord( &m_pCurrentBalls[i*m_nGridSize+j].m_v3Normal, &m_pCurrentBalls[i*m_nGridSize+j].m_v3Normal, pWorldM );
			D3DXVec3TransformCoord( &m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Normal, &m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Normal, pWorldM );
			D3DXVec3TransformCoord( &m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Normal, &m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Normal, pWorldM );
			D3DXVec3TransformCoord( &m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Normal, &m_pCurrentBalls[(i+1)*m_nGridSize+j].m_v3Normal, pWorldM );
			D3DXVec3TransformCoord( &m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Normal, &m_pCurrentBalls[i*m_nGridSize+j+1].m_v3Normal, pWorldM );
			D3DXVec3TransformCoord( &m_pCurrentBalls[(i+1)*m_nGridSize+j+1].m_v3Normal, &m_pCurrentBalls[(i+1)*m_nGridSize+j+1].m_v3Normal, pWorldM );
		}
	}
		
/**/
}


#endif