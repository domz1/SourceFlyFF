#ifndef __SCREENEFFECT_H
#define __SCREENEFFECT_H

class CScreenEffect
{
	int m_nFrame;
	FLOAT m_fFrame;
public:

	CScreenEffect();
	~CScreenEffect();
	void Render( C2DRender* p2DRender );
	void Process();

	int nCount;

};


#endif // __SCREENEFFECT_H