#ifndef PATCH_H
#define PATCH_H

class CLandscape;


// 패치 클래스
// 랜드스케이프를 구성하는 일종의 타일

class CPatch
{
protected:
	FLOAT *m_pHeightMap;			// 높이 맵 (이 패치에 사용될 주소)

	BOOL m_bDirty;					// 버텍스 버퍼를 수정할 필요가 있을 경우 TRUE로 세트

	BOOL m_bVisible;				// 컬링된 결과
	D3DXVECTOR3  m_avBounds[8];		// 컬링에 사용할 바운드 박스 벡터
	D3DXPLANE    m_aplaneBounds[6];	// 컬링에 사용할 바운드 박스 평면

	D3DXVECTOR3 m_vCenter;          // 컬링에 사용할 이 패치의 중심

public:
	int m_nWorldX,m_nWorldY;		// 이 패치의 좌하단 월드좌표

	int m_nLevel;           // 이 패치의 LOD 레벨
	int m_nTopLevel;        // 이 패치 상단부의 LOD 레벨
	int m_nLeftLevel;       // 이 패치 좌단부의 LOD 레벨
	int m_nRightLevel;      // 이 패치 우단부의 LOD 레벨
	int m_nBottomLevel;     // 이 패치 하단부의 LOD 레벨

	CPatch() { m_pHeightMap = NULL; m_nBottomLevel = m_nRightLevel = m_nLeftLevel = m_nTopLevel = m_nLevel = 0; }
	~CPatch();

//	void SetExPatch(BOOL bEnable); // 이거 이제 안쓴다. 전에 기능 추가했다가 삭제됐음.

	void Init( int heightX, int heightY, int worldX, int worldY, FLOAT *hMap ); // 패치 초기화. (전체 월드에서의 위치와 이 패치가 사용할 높이 맵의 주소를 지정한다.)
	BOOL isVisibile( ) { return m_bVisible; } // 컬링된 결과를 돌려준다.
	BOOL isDirty( ) { return m_bDirty; } // 하이트맵의 내용 등이 변경된 경우 버텍스 버퍼, 라이트맵을 수정한다.
	void SetDirty( BOOL bDirty ) { m_bDirty=bDirty; }
	void Render(LPDIRECT3DDEVICE9 pd3dDevice,int X,int Y);

	void CalculateBound();
	void Cull();

	void CalculateLevel();
};

#endif
