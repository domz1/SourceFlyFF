#include "stdafx.h"
#include "2DRender.h"
#include "ScreenEffect.h"

CScreenEffect::CScreenEffect()
{
	m_nFrame = 0;
	m_fFrame = .0f;
	nCount = 64;
}
CScreenEffect::~CScreenEffect()
{
}

void CScreenEffect::Process()
{
	if( m_fFrame < 3.13000f )
		m_fFrame += 0.03f;
	else
		m_fFrame = -1.0f;
}
void CScreenEffect::Render( C2DRender* p2DRender )
{
	TRACE( "%f %f\n",sin( m_fFrame ), m_fFrame );
	if( m_fFrame >= 0.0f )
		p2DRender->RenderFillRect( CRect( 0, 0, 1024, 768 ), D3DCOLOR_ARGB( (int)(sin( m_fFrame ) * 256), 255, 255, 255 ) );

	return;
}
