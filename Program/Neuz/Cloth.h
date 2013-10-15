// Cloth.h: interface for the CCloth class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __CLOTH_H
#define __CLOTH_H

#include <d3dx9.h>
#include <dxerr9.h>
#include "DXUtil.h"

#ifdef __YCLOTH

class CCloth  
{
public:
	class CUSTOM_VERTEX
	{
	public:
		D3DXVECTOR3				    m_v3Pos;
		D3DXVECTOR3					m_v3Nor; 
		D3DCOLOR				    m_dwColor;
		D3DXVECTOR2				    m_v2uv;
		
		enum { 
			FVF = (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2 )
		};
	};
	
	class BALL
	{
	public:
		D3DXVECTOR3 m_v3Pos;
		D3DXVECTOR3 m_v3Velo;
		D3DXVECTOR3 m_v3Normal;
		
		float m_fMass;

		bool m_bFixed;
	};
	
	class SPRING
	{
	public:
		//Indices of the balls at either end of the spring
		int m_nBall1;
		int m_nBall2;
		
		//Tension in the spring
		float m_fTension;
		
		float m_fSpringConstant;
		float m_fNaturalLength;
		
		SPRING()	:	m_nBall1(-1), m_nBall2(-1)
		{}
		~SPRING()
		{}
	};
	
private:
	double m_dStartTime;
	LPDIRECT3DTEXTURE9 m_pTexture;

	//Array of springs
	int m_nNumSprings;
	int m_nNumBalls;
	int m_nGridSize;

	SPRING* m_pSprings;

	BALL* m_pBalls1;
	BALL* m_pBalls2;
	
	BALL* m_pCurrentBalls;
	BALL* m_pNextBalls;

	D3DXVECTOR3 m_v3Gravity;
	
	float m_fSpringConstant;
	float m_fNaturalLength;
	
	//Values given to each ball
	float m_fMass;
	
	//Damping factor. Velocities are multiplied by this
	float m_fDampFactor;
	
	//How tesselated is the patch?
	int m_nPatchTesselation;
public:
	void Init( int nGridSize, D3DXVECTOR3 v3Gravity, FLOAT fSpringConstant, 
		       FLOAT fNaturalLength, FLOAT fMass, FLOAT fDampFactor, int nPatchTesselation,
			   LPDIRECT3DTEXTURE9 pTexture, D3DXMATRIX* pWorldM );

	void DeleteDeviceObjects();
	void Render(LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* pWorldM );
	void Process(float f, D3DXVECTOR3 v3Pos1, D3DXVECTOR3 v3Pos2 );
	void ResetCloth(D3DXMATRIX* pWorldM);
	CCloth();
	virtual ~CCloth();

};

#endif
#endif
