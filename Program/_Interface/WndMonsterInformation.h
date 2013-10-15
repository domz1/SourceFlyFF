#ifndef __WND_MONINFO
#define __WND_MONINFO

#if __VER >= 19

class CWndMonsterInformation : public CWndNeuz
{

public:
	CMover * m_pMover;
	CTexture* m_pTexPetStatusBg;
	CItemElem* m_pPetElem;

	int m_nType;
	int m_nHPWidth;
	int m_nMPWidth;

	BOOL bIsAMonster;
	CTexture* NPCTexture;
	CTexture* JobTexture;

	BOOL m_bVBHPGauge;
	BOOL m_bVBMPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBHPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBMPGauge;

	CTexture   m_texGauHp;
	CTexture   m_texGauMp;
	CTexture   m_texBack;
	CTexture   m_texBack2;
	CTexture   m_texFront;
	CTexture   m_texDifficult;
	int        m_nDigit1;
	CTexture   m_texDigit1;
	int        m_nDigit2;
	CTexture   m_texDigit2;
	int        m_nDigit3;
	CTexture   m_texDigit3;
	CTexture   m_texDanger[3];
	int        m_nDangerNum;


	vector< multimap<DWORD, BUFFSKILL> >	m_pBuffTexture;

	CWndMonsterInformation( CMover* pMover );
	virtual ~CWndMonsterInformation();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = MB_OK );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual	void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize  );
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	void MakeGaugeVertex();
	void SetTextures();
};
#endif
#endif