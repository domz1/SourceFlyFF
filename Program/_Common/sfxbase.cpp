#include "stdafx.h"
#include "model.h" 
#include "sfxbase.h"
#include "..\_Common\ParticleMng.h"


LPDIRECT3DDEVICE9 CSfxMng::m_pd3dDevice;
LPDIRECT3DVERTEXBUFFER9 CSfxMng::m_pSfxVB;
CSfxTexture g_SfxTex;
CSfxMng g_SfxMng;
CSfxObjMng g_SfxObjMng;
CSfxMeshMng g_SfxMeshMng;

#define RANDF ((rand()%50000)/50000.0f) // 0.0~1.0 사이의 실수 랜덤값 생성용

// 텍스쳐 애니메이션용 파일명 추출
// strTex : 기본 텍스쳐 파일명
// nIndex : 기본 텍스쳐 파일명에서 몇번째 뒤의 파일명인가
// ex) GetTextureName("babo03.jpg",5) 하면 리턴값 "babo08.jpg"
// 주의 : 확장자는 무조건 3자리
CString GetTextureName(CString strTex,int nIndex)
{
	CString strRet;
	CString strTemp1,strName,strExt;
	strTemp1=strTex.Left(strTex.GetLength()-4);
	strTemp1=strTemp1.Right(2);
	int nTexnumStart=atoi(strTemp1);
	strTemp1.Format("%02d",nTexnumStart+nIndex);
	strName=strTex.Left(strTex.GetLength()-6);
	strExt=strTex.Right(4);
	strRet=strName+strTemp1+strExt;
	return strRet;
}

CSfxPart::CSfxPart()
{
}
CSfxPart::~CSfxPart()
{
	for(int i=0;i<m_apKeyFrames.GetSize();i++)
		safe_delete( (SfxKeyFrame*)m_apKeyFrames.GetAt(i) );
}

void CSfxPart::AddKeyFrame(WORD nFrame)
{
	if(KeyByFrame(nFrame)) return;
	SfxKeyFrame* pKey=new SfxKeyFrame;

	SfxKeyFrame* pPrevKey=GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey=GetNextKey(nFrame);
	if(pPrevKey==NULL || pNextKey==NULL) {
		pKey->vPos=D3DXVECTOR3(0,0,0);
		pKey->vPosRotate.x=pKey->vPosRotate.y=pKey->vPosRotate.z=.0f;
		if(this->m_nType==SFXPARTTYPE_CUSTOMMESH) pKey->vPosRotate.z=1.0f;
		pKey->vScale.x=pKey->vScale.y=pKey->vScale.z=1.0f;
		pKey->vRotate.x=pKey->vRotate.y=pKey->vRotate.z=.0f;
		pKey->nAlpha=255;
	}
	else {
		int dFrame=pNextKey->nFrame-pPrevKey->nFrame;
		*(pKey)=*(pPrevKey);
		if( dFrame!=0 )
		{
			pKey->vPos		+= (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)( (nFrame - pPrevKey->nFrame) ) / (FLOAT)( dFrame );
			pKey->vPosRotate = (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)( (nFrame - pPrevKey->nFrame) ) / (FLOAT)( dFrame );
			pKey->vRotate	+= (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)( (nFrame - pPrevKey->nFrame) ) / (FLOAT)( dFrame );
			pKey->vScale	+= (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)( (nFrame - pPrevKey->nFrame) ) / (FLOAT)( dFrame );
			pKey->nAlpha	+= (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame - pPrevKey->nFrame) / dFrame;
		}
	}
	if(m_apKeyFrames.GetSize()>0) {
		for(int i=0;i<m_apKeyFrames.GetSize();i++) {
			if(Key(i)->nFrame==nFrame) {
				safe_delete( pKey );
				return;
			}
			if(Key(i)->nFrame>nFrame) {
				pKey->nFrame=nFrame;
				m_apKeyFrames.InsertAt(i,pKey);
				return;
			}
//			*(pKey)=*(Key(i));
		}
	}
	pKey->nFrame=nFrame;
	m_apKeyFrames.Add(pKey);
}
void CSfxPart::DeleteKeyFrame(WORD nFrame)
{
	for(int i=0;i<m_apKeyFrames.GetSize();i++)
		if(Key(i)->nFrame==nFrame) {
			safe_delete( Key(i) );
			m_apKeyFrames.RemoveAt(i);
			return;
		}
}
void CSfxPart::DeleteAllKeyFrame()
{
	for(int i=0;i<m_apKeyFrames.GetSize();i++)
	{
		if( Key( i ) ) 
			safe_delete( Key(i) );
	}
	m_apKeyFrames.RemoveAll();
}

SfxKeyFrame* CSfxPart::GetPrevKey(WORD nFrame)
{
	SfxKeyFrame* pKey=NULL;
	SfxKeyFrame* pTempKey=NULL;
	for(int i=0;i<m_apKeyFrames.GetSize();i++) {
		pTempKey=Key(i);
		if(pTempKey->nFrame>nFrame) break;
		pKey=pTempKey;
	}
	return pKey;
}
SfxKeyFrame* CSfxPart::GetNextKey(WORD nFrame, BOOL bSkip)
{
	SfxKeyFrame* pKey=NULL;
	SfxKeyFrame* pTempKey;
	for( int i = 0; i < m_apKeyFrames.GetSize(); i++ ) 
	{
		pTempKey = Key( i );
		if( bSkip )
		{
			if( pTempKey->nFrame >= nFrame ) 
			{
				pKey=pTempKey;
				break;
			}
		}
		else
		{
			if( pTempKey->nFrame > nFrame ) 
			{
				pKey=pTempKey;
				break;
			}
		}
	}
	return pKey;
}

void CSfxPart::AdjustKeyFrame(WORD nFrame, SfxKeyFrame& key)
{
	key.nFrame=nFrame;
	for(int i=0;i<m_apKeyFrames.GetSize();i++)
		if(Key(i)->nFrame==nFrame) {
			*(Key(i))=key;
			return;
		}
}
#ifndef __WORLDSERVER
void CSfxPart::Render( D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale )
{
}
#endif
void CSfxPart::Render2( D3DXVECTOR3 vPos, WORD nFrame, D3DXVECTOR3 fAngle, D3DXVECTOR3 vScale )
{
}
void CSfxPart::Load(CResFile &file)
{
}
void CSfxPart::OldLoad(CResFile &file)
{
}
void CSfxPart::Load2(CResFile &file)
{
}

void CSfxPart::GetKey(WORD nFrame,SfxKeyFrame* pKey)
{
	SfxKeyFrame* pPrevKey=GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey=GetNextKey(nFrame);
	if(pPrevKey==NULL || pNextKey==NULL) return;
	int dFrame=pNextKey->nFrame-pPrevKey->nFrame;
	*pKey=*(pPrevKey);
	if(dFrame!=0) {
		pKey->vPos			+= (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)( (nFrame - pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		pKey->vPosRotate	+= (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)( (nFrame - pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		pKey->vRotate		+= (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)( (nFrame - pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		pKey->vScale		+= (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)( (nFrame - pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		pKey->nAlpha		+= (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame - pPrevKey->nFrame) / dFrame;
	}
}

CSfxPartBill::CSfxPartBill()
{
	m_nType=SFXPARTTYPE_BILL;
	m_nTexFrame=1;
	m_nTexLoop=1;
	m_bUseing = TRUE;
}

void CSfxPartBill::Render2( D3DXVECTOR3 vPos, WORD nFrame, D3DXVECTOR3 fAngle, D3DXVECTOR3 vScale )
{
#ifdef __CLIENT	
	if( m_nTexFrame > 1 ) 
	{ // 텍스쳐 애니메이션일 경우
		SfxKeyFrame* pFirstKey=GetNextKey(0);
		if(nFrame<pFirstKey->nFrame) return;
		int nTexFrame= (m_nTexFrame*(nFrame-pFirstKey->nFrame)/m_nTexLoop)%m_nTexFrame; // 이 프레임에서 출력할 텍스쳐 번호
		CSfxMng::m_pd3dDevice->SetTexture(0,g_SfxTex.Tex(GetTextureName(m_strTex,nTexFrame)));
	}
	else 
		CSfxMng::m_pd3dDevice->SetTexture( 0, g_SfxTex.Tex( m_strTex ) );
	SfxKeyFrame* pPrevKey = GetPrevKey( nFrame );
	SfxKeyFrame* pNextKey = GetNextKey( nFrame );
	if( pPrevKey == NULL || pNextKey == NULL ) return;
	SfxKeyFrame Key;
	int dFrame = pNextKey->nFrame-pPrevKey->nFrame;
	Key = *(pPrevKey);
	// 앞뒤 keyframe을 색출하여 현재 키값을 산출해 낸다.
	if( dFrame != 0 ) 
	{
		Key.vPos		+= (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)( (nFrame - pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vPosRotate	+= (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)( (nFrame - pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vRotate		+= (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)( (nFrame - pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vScale		+= (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)( (nFrame - pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.nAlpha		+= (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame - pPrevKey->nFrame)/dFrame;
	}

	D3DXMATRIX matTemp,matTemp1,matTemp2;
	D3DXMATRIX matTrans,matAngle,matScale,matInv;
	D3DXVECTOR3 vTemp;
	D3DXVECTOR3 vScaleTemp = vScale;
	vScaleTemp.x *= Key.vScale.x;
	vScaleTemp.y *= Key.vScale.y;
	vScaleTemp.z *= Key.vScale.z;
	// 텍스쳐 타입에 따라 world 매트릭스를 설정한다
	switch(m_nBillType) 
	{
	case SFXPARTBILLTYPE_BILL: // 빌보드이면
		{ // 무조건 화면을 정면으로 바라보게 한다.
			D3DXMatrixRotationZ( &matAngle, DEGREETORADIAN( Key.vRotate.z ) );
			D3DXMatrixScaling( &matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z );
			matTemp2=g_matView;	matTemp2._41=matTemp2._42=matTemp2._43=.0f;
//			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
			matTemp=matScale*matAngle*g_matInvView;
			break;
		}
	case SFXPARTBILLTYPE_BOTTOM: // 바닥에 깔리는 놈이면
		{ // x축으로 90도 돌린다.
			D3DXMatrixRotationZ( &matAngle, DEGREETORADIAN( Key.vRotate.z - fAngle.z ) );
			D3DXMatrixScaling( &matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z );
			D3DXMatrixRotationX( &matTemp2,DEGREETORADIAN( 90 ) );
			matTemp = matScale * matAngle * matTemp2;
			break;
		}
	case SFXPARTBILLTYPE_POLE:
		{ // 이건 기둥인데 아직 안했다.
			D3DXMatrixRotationZ( &matAngle, DEGREETORADIAN( Key.vRotate.z ) );
			D3DXMatrixScaling( &matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z );
			matTemp2 = g_matView; matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
//			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
			matTemp = matScale * matAngle * g_matInvView;
			break;
		}
	case SFXPARTBILLTYPE_NORMAL: // 보통이면.
		{ // 그냥 보통 오브젝트처럼...
			D3DXMATRIX mRot;
			D3DXVECTOR3 vRot  = DEGREETORADIAN(fAngle);
			D3DXVECTOR3 vTemp2=DEGREETORADIAN( Key.vRotate );
			D3DXMatrixRotationYawPitchRoll( &matAngle, vTemp2.y, vTemp2.x, vTemp2.z );
			D3DXMatrixRotationYawPitchRoll(&mRot,vRot.y,vRot.x,vRot.z);
			D3DXMatrixScaling( &matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z );
			matTemp = matScale * matAngle * mRot;
			break;
		}
	}
	D3DXMATRIX mRot;
	D3DXVECTOR3 vRot  = DEGREETORADIAN(fAngle);
	D3DXVECTOR3 vTemp2=DEGREETORADIAN(Key.vPosRotate); // 위치회전값에 의한 새로운 위치도 계산하고
	D3DXMatrixRotationYawPitchRoll(&matTemp2,vTemp2.y,vTemp2.x,vTemp2.z);
	D3DXMatrixRotationYawPitchRoll(&mRot,vRot.y,vRot.x,vRot.z);

	matTemp2 = mRot * matTemp2;
	
	D3DXVec3TransformCoord(&vTemp,&Key.vPos,&matTemp2);
	D3DXMatrixTranslation( &matTrans, 
		vPos.x + ( vTemp.x * vScale.x ),
		vPos.y + ( vTemp.y * vScale.y ),
		vPos.z + ( vTemp.z * vScale.z )
		);
	matTemp1=matTemp*matTrans;

	switch(m_nAlphaType) { // 알파의 용도에 따라 렌더 스테이트를 바꿔준다
	case SFXPARTALPHATYPE_BLEND: // 블렌드이면
		{ // 반투명 설정으로...
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	case SFXPARTALPHATYPE_GLOW: // 글로우면
		{ // 무조건 더하는걸로...
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		}
	}

	// 다른 part들도 기본적으로 이와 비슷하게 처리한다.

//	D3DXMATRIX matWorld;
//	CSfxMng::m_pd3dDevice->GetTransform(D3DTS_WORLD,&matWorld);
	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD,&matTemp1);
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, Key.nAlpha<<24 | 0x404040 );

	CSfxMng::m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2); // 이제 설정 다했으니 그려야지
//	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld);
#endif //__CLIENT
}

#ifndef __WORLDSERVER
void CSfxPartBill::Render( D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale )
{
	if( m_nTexFrame > 1 ) 
	{ // 텍스쳐 애니메이션일 경우
		SfxKeyFrame* pFirstKey=GetNextKey(0);
		if(nFrame<pFirstKey->nFrame) return;
		int nTexFrame= (m_nTexFrame*(nFrame-pFirstKey->nFrame)/m_nTexLoop)%m_nTexFrame; // 이 프레임에서 출력할 텍스쳐 번호
		CSfxMng::m_pd3dDevice->SetTexture(0,g_SfxTex.Tex(GetTextureName(m_strTex,nTexFrame)));
	}
	else 
		CSfxMng::m_pd3dDevice->SetTexture( 0, g_SfxTex.Tex( m_strTex ) );
	SfxKeyFrame* pPrevKey = GetPrevKey( nFrame );
	SfxKeyFrame* pNextKey = GetNextKey( nFrame );
	if( pPrevKey == NULL || pNextKey == NULL ) return;
	SfxKeyFrame Key;
	int dFrame = pNextKey->nFrame-pPrevKey->nFrame;
	Key = *(pPrevKey);
	// 앞뒤 keyframe을 색출하여 현재 키값을 산출해 낸다.
	if( dFrame != 0 ) 
	{
		Key.vPos		+= (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vPosRotate	+= (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vRotate		+= (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vScale		+= (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) /(FLOAT)( dFrame );
		Key.nAlpha		+= (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame-pPrevKey->nFrame) / dFrame;
	}

	D3DXMATRIX matTemp,matTemp1,matTemp2;
	D3DXMATRIX matTrans,matAngle,matScale,matInv;
	D3DXVECTOR3 vTemp;
	D3DXVECTOR3 vScaleTemp = vScale;
	vScaleTemp.x *= Key.vScale.x;
	vScaleTemp.y *= Key.vScale.y;
	vScaleTemp.z *= Key.vScale.z;
	// 텍스쳐 타입에 따라 world 매트릭스를 설정한다
	switch(m_nBillType) 
	{
	case SFXPARTBILLTYPE_BILL: // 빌보드이면
		{ // 무조건 화면을 정면으로 바라보게 한다.
			D3DXMatrixRotationZ( &matAngle, DEGREETORADIAN( Key.vRotate.z ) );
			D3DXMatrixScaling( &matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z );
			matTemp2=g_matView;	matTemp2._41=matTemp2._42=matTemp2._43=.0f;
//			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
			matTemp=matScale*matAngle*g_matInvView;
			break;
		}
	case SFXPARTBILLTYPE_BOTTOM: // 바닥에 깔리는 놈이면
		{ // x축으로 90도 돌린다.
			D3DXMatrixRotationZ( &matAngle, DEGREETORADIAN( Key.vRotate.z - fAngle ) );
			D3DXMatrixScaling( &matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z );
			D3DXMatrixRotationX( &matTemp2,DEGREETORADIAN( 90 ) );
			matTemp = matScale * matAngle * matTemp2;
			break;
		}
	case SFXPARTBILLTYPE_POLE:
		{ // 이건 기둥인데 아직 안했다.
			D3DXMatrixRotationZ( &matAngle, DEGREETORADIAN( Key.vRotate.z ) );
			D3DXMatrixScaling( &matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z );
			matTemp2 = g_matView; matTemp2._41 = matTemp2._42 = matTemp2._43 = .0f;
//			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
			matTemp = matScale * matAngle * g_matInvView;
			break;
		}
	case SFXPARTBILLTYPE_NORMAL: // 보통이면.
		{ // 그냥 보통 오브젝트처럼...
			D3DXVECTOR3 vTemp2=DEGREETORADIAN( Key.vRotate + D3DXVECTOR3( .0f, fAngle, .0f ) );
			D3DXMatrixRotationYawPitchRoll( &matAngle, vTemp2.y, vTemp2.x, vTemp2.z );
			D3DXMatrixScaling( &matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z );
			matTemp = matScale * matAngle;
			break;
		}
	}
	D3DXVECTOR3 vTemp2=DEGREETORADIAN(Key.vPosRotate+D3DXVECTOR3(.0f,fAngle,.0f)); // 위치회전값에 의한 새로운 위치도 계산하고
	D3DXMatrixRotationYawPitchRoll(&matTemp2,vTemp2.y,vTemp2.x,vTemp2.z);
	D3DXVec3TransformCoord(&vTemp,&Key.vPos,&matTemp2);
	D3DXMatrixTranslation( &matTrans, 
		vPos.x + ( vTemp.x * vScale.x ),
		vPos.y + ( vTemp.y * vScale.y ),
		vPos.z + ( vTemp.z * vScale.z )
		);
	matTemp1=matTemp*matTrans;

	switch(m_nAlphaType) { // 알파의 용도에 따라 렌더 스테이트를 바꿔준다
	case SFXPARTALPHATYPE_BLEND: // 블렌드이면
		{ // 반투명 설정으로...
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	case SFXPARTALPHATYPE_GLOW: // 글로우면
		{ // 무조건 더하는걸로...
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		}
	}

	// 다른 part들도 기본적으로 이와 비슷하게 처리한다.

//	D3DXMATRIX matWorld;
//	CSfxMng::m_pd3dDevice->GetTransform(D3DTS_WORLD,&matWorld);
	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD,&matTemp1);
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, Key.nAlpha<<24 | 0x404040 );

	CSfxMng::m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2); // 이제 설정 다했으니 그려야지
//	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld);
}
#endif

void CSfxPartBill::Load(CResFile &file)
{
	int i;
	char strTemp[255];
	int nStrLen;
	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strName = m_strTex=strTemp;
	file.Read(&m_nTexFrame,sizeof(WORD));
	file.Read(&m_nTexLoop,sizeof(WORD));
	m_bUseing = TRUE;
	
	if(m_nTexFrame>1) { // 텍스쳐 애니메이션일 경우
		for(i=0;i<m_nTexFrame;i++) { // 전부 로딩
			g_SfxTex.Tex(GetTextureName(strTemp,i));
		}
	}

	file.Read(&m_nBillType,sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType,sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey,sizeof(int));
	for(i=0;i<nKey;i++) {
		WORD nFrame;
		file.Read(&nFrame,sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha),sizeof(int));
		AdjustKeyFrame(nFrame,Key);
	}
}
void CSfxPartBill::Load2(CResFile &file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strName=strTemp;

	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strTex=strTemp;
	file.Read(&m_nTexFrame,sizeof(WORD));
	file.Read(&m_nTexLoop,sizeof(WORD));
	file.Read(&m_bUseing,sizeof(BOOL));
	
	if(m_nTexFrame>1) { // 텍스쳐 애니메이션일 경우
		for(i=0;i<m_nTexFrame;i++) { // 전부 로딩
			g_SfxTex.Tex(GetTextureName(strTemp,i));
		}
	}
	
	file.Read(&m_nBillType,sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType,sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey,sizeof(int));
	for(i=0;i<nKey;i++) {
		WORD nFrame;
		file.Read(&nFrame,sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha),sizeof(int));
		AdjustKeyFrame(nFrame,Key);
	}
}
void CSfxPartBill::OldLoad(CResFile &file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strName = m_strTex=strTemp;
	m_nTexFrame=1;
	m_nTexLoop=1;
	m_bUseing = TRUE;
	
	file.Read(&m_nBillType,sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType,sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey,sizeof(int));
	for(i=0;i<nKey;i++) {
		WORD nFrame;
		file.Read(&nFrame,sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha),sizeof(int));
		AdjustKeyFrame(nFrame,Key);
	}
}

CSfxPartParticle::CSfxPartParticle()
{
	m_nType=SFXPARTTYPE_PARTICLE;
	m_nTexFrame=1;
	m_nTexLoop=1;
	m_bUseing = TRUE;
	
	m_nParticleCreate=0;
	m_nParticleCreateNum=0;

	m_nParticleFrameAppear=0;
	m_nParticleFrameKeep=0;
	m_nParticleFrameDisappear=0;

	m_fParticleStartPosVar=0;
	m_fParticleStartPosVarY=0;

	m_fParticleYLow=0;
	m_fParticleYHigh=0;
	m_fParticleXZLow=0;
	m_fParticleXZHigh=0;

	m_vParticleAccel=D3DXVECTOR3(.0f,.0f,.0f);

	m_vScale=D3DXVECTOR3(.0f,.0f,.0f);
	m_vScaleSpeed=D3DXVECTOR3(.0f,.0f,.0f);
	m_vRotation=D3DXVECTOR3(.0f,.0f,.0f);

	m_vRotationLow=D3DXVECTOR3(.0f,.0f,.0f);
	m_vRotationHigh=D3DXVECTOR3(.0f,.0f,.0f);


//#ifdef __ATEST
	m_bRepeatScal = FALSE;
	m_bRepeat = FALSE;
	m_fScalSpeedXLow  = 0;
	m_fScalSpeedXHigh = 0;
	m_fScalSpeedYLow  = 0;
	m_fScalSpeedYHigh = 0;
	m_fScalSpeedZLow  = 0;
	m_fScalSpeedZHigh = 0;
	m_vScaleEnd = D3DXVECTOR3(.0f,.0f,.0f);
	m_vScaleSpeed2 = D3DXVECTOR3(.0f,.0f,.0f);
//#endif
	
}

CSfxPartParticle::~CSfxPartParticle()
{
}
#ifndef __WORLDSERVER
void CSfxPartParticle::Render( D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale )
{
	CSfxMng::m_pd3dDevice->SetTexture(0,g_SfxTex.Tex(m_strTex));
	SfxKeyFrame* pPrevKey=GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey=GetNextKey(nFrame);
	if(pPrevKey==NULL || pNextKey==NULL) return;
	SfxKeyFrame Key;
	int dFrame=pNextKey->nFrame-pPrevKey->nFrame;
	Key=*(pPrevKey);
	if(dFrame!=0) 
	{
		Key.vPos		+= (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vPosRotate	+= (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vRotate		+= (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vScale		+= (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.nAlpha		+= (pNextKey->nAlpha - pPrevKey->nAlpha)*(nFrame-pPrevKey->nFrame)/dFrame;
	}

	D3DXMATRIX matTemp,matTemp1,matTemp2;
	D3DXMATRIX matTrans,matAngle,matScale,matInv;
	D3DXVECTOR3 vTemp;
	switch(m_nBillType) {
	case SFXPARTBILLTYPE_BILL:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(fAngle+Key.vRotate.z));
			D3DXMatrixScaling(&matScale,Key.vScale.x,Key.vScale.y,1.0f);
			matTemp2=g_matView;	matTemp2._41=matTemp2._42=matTemp2._43=.0f;
//			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
			matTemp=matScale*matAngle*g_matInvView;
			break;
		}
	case SFXPARTBILLTYPE_BOTTOM:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(fAngle+Key.vRotate.z));
			D3DXMatrixScaling(&matScale,Key.vScale.x,Key.vScale.y,1.0f);
			D3DXMatrixRotationX(&matTemp2,DEGREETORADIAN(90));
			matTemp=matScale*matAngle*matTemp2;
			break;
		}
	case SFXPARTBILLTYPE_POLE:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(fAngle+Key.vRotate.z));
			D3DXMatrixScaling(&matScale,Key.vScale.x,Key.vScale.y,1.0f);
			matTemp2=g_matView;	matTemp2._41=matTemp2._42=matTemp2._43=.0f;
//			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
			matTemp=matScale*matAngle*g_matInvView;
			break;
		}
	case SFXPARTBILLTYPE_NORMAL:
		{
			D3DXVECTOR3 vTemp2=DEGREETORADIAN(Key.vRotate);
			D3DXMatrixRotationYawPitchRoll(&matAngle,vTemp2.y,vTemp2.x,vTemp2.z);
			D3DXMatrixScaling(&matScale,Key.vScale.x,Key.vScale.y,1.0f);
			matTemp=matScale*matAngle;
			break;
		}
	}
	D3DXVECTOR3 vTemp2=DEGREETORADIAN(Key.vPosRotate+D3DXVECTOR3(.0f,fAngle,.0f));
	D3DXMatrixRotationYawPitchRoll(&matTemp2,vTemp2.y,vTemp2.x,vTemp2.z);
	D3DXVec3TransformCoord(&vTemp,&Key.vPos,&matTemp2);
	//D3DXMatrixTranslation(&matTrans,vPos.x+vTemp.x,vPos.y+vTemp.y,vPos.z+vTemp.z);
	D3DXMatrixTranslation( &matTrans, 
		vPos.x + ( vTemp.x * vScale.x ), 
		vPos.y + ( vTemp.y * vScale.y ), 
		vPos.z + ( vTemp.z * vScale.z ) );
	
	matTemp1=matTemp*matTrans;

	switch(m_nAlphaType) 
	{
	case SFXPARTALPHATYPE_BLEND:
		{
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	case SFXPARTALPHATYPE_GLOW:
		{
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		}
	}
//	D3DXMATRIX matWorld;
//	CSfxMng::m_pd3dDevice->GetTransform(D3DTS_WORLD,&matWorld);
	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD,&matTemp1);
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, Key.nAlpha<<24 | 0x404040 );

	CSfxMng::m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
//	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld);
}
#endif // __WORLDSERVER


void CSfxPartParticle::Load(CResFile &file)
{
	int i;
	char strTemp[255];
	int nStrLen;
	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strName = m_strTex=strTemp;
	file.Read(&m_nTexFrame,sizeof(WORD));
	file.Read(&m_nTexLoop,sizeof(WORD));
	m_bUseing = TRUE;
	if(m_nTexFrame>1) { // 텍스쳐 애니메이션일 경우
		for(i=0;i<m_nTexFrame;i++) { // 전부 로딩
			g_SfxTex.Tex(GetTextureName(strTemp,i));
		}
	}

	file.Read(&m_nBillType,sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType,sizeof(SFXPARTALPHATYPE));

	file.Read(&m_nParticleCreate,sizeof(WORD));
	file.Read(&m_nParticleCreateNum,sizeof(WORD));

	file.Read(&m_nParticleFrameAppear,sizeof(WORD));
	file.Read(&m_nParticleFrameKeep,sizeof(WORD));
	file.Read(&m_nParticleFrameDisappear,sizeof(WORD));

	file.Read(&m_fParticleStartPosVar,sizeof(FLOAT));
	file.Read(&m_fParticleStartPosVarY,sizeof(FLOAT));

	file.Read(&m_fParticleYLow,sizeof(FLOAT));
	file.Read(&m_fParticleYHigh,sizeof(FLOAT));
	file.Read(&m_fParticleXZLow,sizeof(FLOAT));
	file.Read(&m_fParticleXZHigh,sizeof(FLOAT));

	file.Read(&m_vParticleAccel,sizeof(D3DXVECTOR3));

	file.Read(&m_vScale,sizeof(D3DXVECTOR3));
	file.Read(&m_vScaleSpeed,sizeof(D3DXVECTOR3));
	m_vRotation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vRotationLow = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vRotationHigh = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_bRepeatScal = FALSE;
	m_bRepeat = FALSE;
	m_fScalSpeedXLow   = 0;
	m_fScalSpeedXHigh  = 0;
	m_fScalSpeedYLow   = 0;
	m_fScalSpeedYHigh  = 0;
	m_fScalSpeedZLow   = 0;
	m_fScalSpeedZHigh  = 0;
	m_vScaleSpeed2 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vScaleEnd    = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	
	int nKey=m_apKeyFrames.GetSize();
	file.Read(&nKey,sizeof(int));
	for(i=0;i<nKey;i++) {
		int nFrame;
		file.Read(&nFrame,sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha),sizeof(int));
		AdjustKeyFrame(nFrame,Key);
	}
}
void CSfxPartParticle::Load2(CResFile &file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strName=strTemp;

	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strTex=strTemp;
	file.Read(&m_nTexFrame,sizeof(WORD));
	file.Read(&m_nTexLoop,sizeof(WORD));
	file.Read(&m_bUseing,sizeof(BOOL));
	if(m_nTexFrame>1) { // 텍스쳐 애니메이션일 경우
		for(i=0;i<m_nTexFrame;i++) { // 전부 로딩
			g_SfxTex.Tex(GetTextureName(strTemp,i));
		}
	}
	
	file.Read(&m_nBillType,sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType,sizeof(SFXPARTALPHATYPE));
	
	file.Read(&m_nParticleCreate,sizeof(WORD));
	file.Read(&m_nParticleCreateNum,sizeof(WORD));
	
	file.Read(&m_nParticleFrameAppear,sizeof(WORD));
	file.Read(&m_nParticleFrameKeep,sizeof(WORD));
	file.Read(&m_nParticleFrameDisappear,sizeof(WORD));
	
	file.Read(&m_fParticleStartPosVar,sizeof(FLOAT));
	file.Read(&m_fParticleStartPosVarY,sizeof(FLOAT));
	
	file.Read(&m_fParticleYLow,sizeof(FLOAT));
	file.Read(&m_fParticleYHigh,sizeof(FLOAT));
	file.Read(&m_fParticleXZLow,sizeof(FLOAT));
	file.Read(&m_fParticleXZHigh,sizeof(FLOAT));
	
	file.Read(&m_vParticleAccel,sizeof(D3DXVECTOR3));
	
	file.Read(&m_vScale,sizeof(D3DXVECTOR3));
	file.Read(&m_vScaleSpeed,sizeof(D3DXVECTOR3));
	file.Read(&m_vRotation,sizeof(D3DXVECTOR3));
	file.Read(&m_vRotationLow,sizeof(D3DXVECTOR3));
	file.Read(&m_vRotationHigh,sizeof(D3DXVECTOR3));
	
	file.Read(&m_bRepeatScal,sizeof(BOOL));
	m_bRepeat = FALSE;
	file.Read(&m_fScalSpeedXLow,sizeof(FLOAT));
	file.Read(&m_fScalSpeedXHigh,sizeof(FLOAT));
	file.Read(&m_fScalSpeedYLow,sizeof(FLOAT));
	file.Read(&m_fScalSpeedYHigh,sizeof(FLOAT));
	file.Read(&m_fScalSpeedZLow,sizeof(FLOAT));
	file.Read(&m_fScalSpeedZHigh,sizeof(FLOAT));
	file.Read(&m_vScaleSpeed2,sizeof(D3DXVECTOR3));
	file.Read(&m_vScaleEnd,sizeof(D3DXVECTOR3));

	int nKey=m_apKeyFrames.GetSize();
	file.Read(&nKey,sizeof(int));
	for(i=0;i<nKey;i++) {
		int nFrame;
		file.Read(&nFrame,sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha),sizeof(int));
		AdjustKeyFrame(nFrame,Key);
	}
}
void CSfxPartParticle::Load3(CResFile &file)
{
	int i;
	char strTemp[255];
	int nStrLen;
	
	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strName=strTemp;
	
	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strTex=strTemp;
	file.Read(&m_nTexFrame,sizeof(WORD));
	file.Read(&m_nTexLoop,sizeof(WORD));
	file.Read(&m_bUseing,sizeof(BOOL));
	if(m_nTexFrame>1) { // 텍스쳐 애니메이션일 경우
		for(i=0;i<m_nTexFrame;i++) { // 전부 로딩
			g_SfxTex.Tex(GetTextureName(strTemp,i));
		}
	}
	
	file.Read(&m_nBillType,sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType,sizeof(SFXPARTALPHATYPE));
	
	file.Read(&m_nParticleCreate,sizeof(WORD));
	file.Read(&m_nParticleCreateNum,sizeof(WORD));
	
	file.Read(&m_nParticleFrameAppear,sizeof(WORD));
	file.Read(&m_nParticleFrameKeep,sizeof(WORD));
	file.Read(&m_nParticleFrameDisappear,sizeof(WORD));
	
	file.Read(&m_fParticleStartPosVar,sizeof(FLOAT));
	file.Read(&m_fParticleStartPosVarY,sizeof(FLOAT));
	
	file.Read(&m_fParticleYLow,sizeof(FLOAT));
	file.Read(&m_fParticleYHigh,sizeof(FLOAT));
	file.Read(&m_fParticleXZLow,sizeof(FLOAT));
	file.Read(&m_fParticleXZHigh,sizeof(FLOAT));
	
	file.Read(&m_vParticleAccel,sizeof(D3DXVECTOR3));
	
	file.Read(&m_vScale,sizeof(D3DXVECTOR3));
	file.Read(&m_vScaleSpeed,sizeof(D3DXVECTOR3));
	file.Read(&m_vRotation,sizeof(D3DXVECTOR3));
	file.Read(&m_vRotationLow,sizeof(D3DXVECTOR3));
	file.Read(&m_vRotationHigh,sizeof(D3DXVECTOR3));
	
	file.Read(&m_bRepeatScal,sizeof(BOOL));
	file.Read(&m_bRepeat,sizeof(BOOL));
	file.Read(&m_fScalSpeedXLow,sizeof(FLOAT));
	file.Read(&m_fScalSpeedXHigh,sizeof(FLOAT));
	file.Read(&m_fScalSpeedYLow,sizeof(FLOAT));
	file.Read(&m_fScalSpeedYHigh,sizeof(FLOAT));
	file.Read(&m_fScalSpeedZLow,sizeof(FLOAT));
	file.Read(&m_fScalSpeedZHigh,sizeof(FLOAT));
	file.Read(&m_vScaleSpeed2,sizeof(D3DXVECTOR3));
	file.Read(&m_vScaleEnd,sizeof(D3DXVECTOR3));
	
	int nKey=m_apKeyFrames.GetSize();
	file.Read(&nKey,sizeof(int));
	for(i=0;i<nKey;i++) {
		int nFrame;
		file.Read(&nFrame,sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha),sizeof(int));
		AdjustKeyFrame(nFrame,Key);
	}
}
void CSfxPartParticle::OldLoad(CResFile &file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strName = m_strTex=strTemp;
	m_nTexFrame=1;
	m_nTexLoop=1;
	m_bUseing = TRUE;
	
	file.Read(&m_nBillType,sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType,sizeof(SFXPARTALPHATYPE));

	file.Read(&m_nParticleCreate,sizeof(WORD));
	file.Read(&m_nParticleCreateNum,sizeof(WORD));

	file.Read(&m_nParticleFrameAppear,sizeof(WORD));
	file.Read(&m_nParticleFrameKeep,sizeof(WORD));
	file.Read(&m_nParticleFrameDisappear,sizeof(WORD));

	file.Read(&m_fParticleStartPosVar,sizeof(FLOAT));
	file.Read(&m_fParticleStartPosVarY,sizeof(FLOAT));

	file.Read(&m_fParticleYLow,sizeof(FLOAT));
	file.Read(&m_fParticleYHigh,sizeof(FLOAT));
	file.Read(&m_fParticleXZLow,sizeof(FLOAT));
	file.Read(&m_fParticleXZHigh,sizeof(FLOAT));

	file.Read(&m_vParticleAccel,sizeof(D3DXVECTOR3));

	file.Read(&m_vScale,sizeof(D3DXVECTOR3));
	file.Read(&m_vScaleSpeed,sizeof(D3DXVECTOR3));
	m_vRotation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vRotationLow = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vRotationHigh = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_bRepeatScal = FALSE;
	m_bRepeat = FALSE;
	m_fScalSpeedXLow   = 0;
	m_fScalSpeedXHigh  = 0;
	m_fScalSpeedYLow   = 0;
	m_fScalSpeedYHigh  = 0;
	m_fScalSpeedZLow   = 0;
	m_fScalSpeedZHigh  = 0;
	m_vScaleSpeed2 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vScaleEnd    = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	
	int nKey=m_apKeyFrames.GetSize();
	file.Read(&nKey,sizeof(int));
	for(i=0;i<nKey;i++) {
		int nFrame;
		file.Read(&nFrame,sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha),sizeof(int));
		AdjustKeyFrame(nFrame,Key);
	}
}

CSfxPartMesh::CSfxPartMesh()
{
	m_nType=SFXPARTTYPE_MESH;
	m_nTexFrame=1;
	m_nTexLoop=1;
	m_bUseing = TRUE;
}
CSfxPartMesh::~CSfxPartMesh()
{
}
void CSfxPartMesh::Load(CResFile &file)
{
	int i;
	char strTemp[255];
	int nStrLen;
	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strName = m_strTex=strTemp;
	file.Read(&m_nTexFrame,sizeof(WORD));
	file.Read(&m_nTexLoop,sizeof(WORD));
	m_bUseing = TRUE;
/* 메쉬는 이런거 없다!
	if(m_nTexFrame>1) { // 텍스쳐 애니메이션일 경우
		for(i=0;i<m_nTexFrame;i++) { // 전부 로딩
			g_SfxTex.Tex(GetTextureName(strTemp,i));
		}
	}
*/
	file.Read(&m_nBillType,sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType,sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey,sizeof(int));
	for(i=0;i<nKey;i++) {
		WORD nFrame;
		file.Read(&nFrame,sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha),sizeof(int));
		AdjustKeyFrame(nFrame,Key);
	}
}
void CSfxPartMesh::Load2(CResFile &file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strName=strTemp;

	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strTex=strTemp;
	file.Read(&m_nTexFrame,sizeof(WORD));
	file.Read(&m_nTexLoop,sizeof(WORD));
	file.Read(&m_bUseing,sizeof(BOOL));
/* 메쉬는 이런거 없다!
	if(m_nTexFrame>1) { // 텍스쳐 애니메이션일 경우
		for(i=0;i<m_nTexFrame;i++) { // 전부 로딩
			g_SfxTex.Tex(GetTextureName(strTemp,i));
		}
	}
*/
	file.Read(&m_nBillType,sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType,sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey,sizeof(int));
	for(i=0;i<nKey;i++) {
		WORD nFrame;
		file.Read(&nFrame,sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha),sizeof(int));
		AdjustKeyFrame(nFrame,Key);
	}
}


void CSfxPartMesh::OldLoad(CResFile &file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strName = m_strTex=strTemp;
	m_nTexFrame=1;
	m_nTexLoop=1;
	m_bUseing = TRUE;
	
	file.Read(&m_nBillType,sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType,sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey,sizeof(int));
	for(i=0;i<nKey;i++) {
		WORD nFrame;
		file.Read(&nFrame,sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha),sizeof(int));
		AdjustKeyFrame(nFrame,Key);
	}
}

#ifndef __WORLDSERVER
void CSfxPartMesh::Render( D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale )
{
	SfxKeyFrame* pPrevKey=GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey=GetNextKey(nFrame);
	if(pPrevKey==NULL || pNextKey==NULL) 
	{
		return;
	}
	SfxKeyFrame Key;
	int dFrame=pNextKey->nFrame-pPrevKey->nFrame;
	Key=*(pPrevKey);
	if(dFrame!=0) {
		Key.vPos		+= (pNextKey->vPos-pPrevKey->vPos) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vPosRotate	+= (pNextKey->vPosRotate-pPrevKey->vPosRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vRotate		+= (pNextKey->vRotate-pPrevKey->vRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vScale		+= (pNextKey->vScale-pPrevKey->vScale) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.nAlpha		+= (pNextKey->nAlpha-pPrevKey->nAlpha) * (nFrame-pPrevKey->nFrame) / dFrame;
	}

	D3DXMATRIX matTemp,matTemp1,matTemp2;
	D3DXMATRIX matTrans,matAngle,matScale,matInv;
	D3DXVECTOR3 vTemp;
	D3DXVECTOR3 vScaleTemp = vScale;
	vScaleTemp.x *= Key.vScale.x;
	vScaleTemp.y *= Key.vScale.y;
	vScaleTemp.z *= Key.vScale.z;
	
	{
		D3DXVECTOR3 vTemp2=DEGREETORADIAN(Key.vRotate+D3DXVECTOR3(.0f,fAngle,.0f));
		D3DXMatrixRotationYawPitchRoll(&matAngle,vTemp2.y,vTemp2.x,vTemp2.z);
		D3DXMatrixScaling( &matScale, vScaleTemp.x, vScaleTemp.y, vScaleTemp.z );
		matTemp=matScale*matAngle;
		//matTemp=matScale*matAngle*g_matInvView;
	}

	D3DXVECTOR3 vTemp2 = DEGREETORADIAN( Key.vPosRotate + D3DXVECTOR3(.0f,fAngle,.0f) );
	D3DXMatrixRotationYawPitchRoll( &matTemp2, vTemp2.y, vTemp2.x, vTemp2.z );
	D3DXVec3TransformCoord( &vTemp, &Key.vPos, &matTemp2 );
	D3DXMatrixTranslation( &matTrans, 
		vPos.x + ( vTemp.x * vScale.x ), 
		vPos.y + ( vTemp.y * vScale.y ), 
		vPos.z + ( vTemp.z * vScale.z ) );
	matTemp1 = matTemp * matTrans;

	switch(m_nAlphaType) {
	case SFXPARTALPHATYPE_BLEND:
		{
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	case SFXPARTALPHATYPE_GLOW:
		{
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		}
	}
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, Key.nAlpha<<24 | 0x404040 );
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	CSfxMng::m_pd3dDevice->SetTransform(D3DTS_WORLD,&matTemp1);
	CModelObject* pMesh=g_SfxMeshMng.Mesh(m_strTex);
	
	if(pMesh) 
	{
//		pMesh->m_nNoEffect = 1;
/*		DWORD v;
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,   &v);
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &v);
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &v );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG1, &v );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,   &v );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG2, &v );
		pd3dDevice->GetRenderState( D3DRS_SRCBLEND, &v );
		pd3dDevice->GetRenderState( D3DRS_DESTBLEND, &v );
		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &v );
		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &v ); 
*/	
		pMesh->m_nNoEffect = 1;

		if(m_nAlphaType == SFXPARTALPHATYPE_BLEND)
		{
			pMesh->m_nNoEffect = 0;
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		}

		pMesh->Render(CSfxMng::m_pd3dDevice,&matTemp1);

		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, Key.nAlpha<<24 | 0x404040 );
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	}
	//CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZENABLE,FALSE);
	//CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
}
#endif // __WORLDSERVER
CSfxPartCustomMesh::CSfxPartCustomMesh()
{
	m_nType=SFXPARTTYPE_CUSTOMMESH;
	m_nTexFrame=1;
	m_nTexLoop=1;
	m_nPoints=20;
	m_bUseing = TRUE;
}
CSfxPartCustomMesh::~CSfxPartCustomMesh()
{
}

void CSfxPartCustomMesh::Load(CResFile &file)
{
	int i;
	char strTemp[255];
	int nStrLen;
	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strName = m_strTex=strTemp;
	file.Read(&m_nTexFrame,sizeof(WORD));
	file.Read(&m_nTexLoop,sizeof(WORD));
	m_bUseing = TRUE;

	if(m_nTexFrame>1) { // 텍스쳐 애니메이션일 경우
		for(i=0;i<m_nTexFrame;i++) { // 전부 로딩
			g_SfxTex.Tex(GetTextureName(strTemp,i));
		}
	}

	file.Read(&m_nBillType,sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType,sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey,sizeof(int));
	for(i=0;i<nKey;i++) {
		WORD nFrame;
		file.Read(&nFrame,sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha),sizeof(int));
		AdjustKeyFrame(nFrame,Key);
	}
	file.Read(&m_nPoints,sizeof(int));
}
void CSfxPartCustomMesh::Load2(CResFile &file)
{
	int i;
	char strTemp[255];
	int nStrLen;

	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strName=strTemp;

	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strTex=strTemp;
	file.Read(&m_nTexFrame,sizeof(WORD));
	file.Read(&m_nTexLoop,sizeof(WORD));
	file.Read(&m_bUseing,sizeof(BOOL));
	if(m_nTexFrame>1) { // 텍스쳐 애니메이션일 경우
		for(i=0;i<m_nTexFrame;i++) { // 전부 로딩
			g_SfxTex.Tex(GetTextureName(strTemp,i));
		}
	}
	
	file.Read(&m_nBillType,sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType,sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey,sizeof(int));
	for(i=0;i<nKey;i++) {
		WORD nFrame;
		file.Read(&nFrame,sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha),sizeof(int));
		AdjustKeyFrame(nFrame,Key);
	}
	file.Read(&m_nPoints,sizeof(int));
}
void CSfxPartCustomMesh::OldLoad(CResFile &file)
{
	int i;
	char strTemp[255];
	int nStrLen;
	
	file.Read(&nStrLen,sizeof(int));
	file.Read(strTemp,nStrLen); strTemp[nStrLen]=0;
	m_strName = m_strTex=strTemp;
	m_nTexFrame=1;
	m_nTexLoop=1;
	m_bUseing = TRUE;
	
	file.Read(&m_nBillType,sizeof(SFXPARTBILLTYPE));
	file.Read(&m_nAlphaType,sizeof(SFXPARTALPHATYPE));
	int nKey;
	file.Read(&nKey,sizeof(int));
	for(i=0;i<nKey;i++) {
		WORD nFrame;
		file.Read(&nFrame,sizeof(WORD));
		AddKeyFrame(nFrame);
		SfxKeyFrame Key;
		file.Read(&(Key.vPos),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vPosRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vScale),sizeof(D3DXVECTOR3));
		file.Read(&(Key.vRotate),sizeof(D3DXVECTOR3));
		file.Read(&(Key.nAlpha),sizeof(int));
		AdjustKeyFrame(nFrame,Key);
	}
	file.Read(&m_nPoints,sizeof(int));
}



#ifndef __WORLDSERVER
void CSfxPartCustomMesh::Render( D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale )
{
	if(m_nTexFrame>1) 
	{ // 텍스쳐 애니메이션일 경우
		SfxKeyFrame* pFirstKey=GetNextKey(0);
		if(nFrame<pFirstKey->nFrame) return;
		int nTexFrame= (m_nTexFrame*(nFrame-pFirstKey->nFrame)/m_nTexLoop)%m_nTexFrame; // 이 프레임에서 출력할 텍스쳐 번호
		CSfxMng::m_pd3dDevice->SetTexture(0,g_SfxTex.Tex(GetTextureName(m_strTex,nTexFrame)));
	}
	else CSfxMng::m_pd3dDevice->SetTexture(0,g_SfxTex.Tex(m_strTex));
	SfxKeyFrame* pPrevKey=GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey=GetNextKey(nFrame);
	if(pPrevKey==NULL || pNextKey==NULL) return;
	SfxKeyFrame Key;
	int dFrame=pNextKey->nFrame-pPrevKey->nFrame;
	Key=*(pPrevKey);
	if(dFrame!=0) 
	{
		Key.vPos		+= (pNextKey->vPos - pPrevKey->vPos) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vPosRotate	+= (pNextKey->vPosRotate - pPrevKey->vPosRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vRotate		+= (pNextKey->vRotate - pPrevKey->vRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vScale		+= (pNextKey->vScale - pPrevKey->vScale) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.nAlpha		+= (pNextKey->nAlpha - pPrevKey->nAlpha) * (nFrame-pPrevKey->nFrame) / dFrame;
	}

	D3DXMATRIX matTemp,matTemp1,matTemp2;
	D3DXMATRIX matTrans,matAngle,matScale,matInv;
	D3DXVECTOR3 vTemp;
	switch(m_nBillType) {
	case SFXPARTBILLTYPE_BILL:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(Key.vRotate.z-fAngle));
//			D3DXMatrixScaling(&matScale,Key.vScale.x,Key.vScale.y,1.0f);
			matTemp2=g_matView;	matTemp2._41=matTemp2._42=matTemp2._43=.0f;
			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
			matTemp=matAngle*matInv;
			break;
		}
	case SFXPARTBILLTYPE_BOTTOM:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(Key.vRotate.z));
//			D3DXMatrixScaling(&matScale,Key.vScale.x,Key.vScale.y,1.0f);
			D3DXMatrixRotationX(&matTemp2,DEGREETORADIAN(90));
			matTemp=matAngle*matTemp2;
			break;
		}
	case SFXPARTBILLTYPE_POLE:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(Key.vRotate.z));
//			D3DXMatrixScaling(&matScale,Key.vScale.x,Key.vScale.y,1.0f);
			matTemp2=g_matView;	matTemp2._41=matTemp2._42=matTemp2._43=.0f;
			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
			matTemp=matAngle*matInv;
			break;
		}
	case SFXPARTBILLTYPE_NORMAL:
		{
			D3DXVECTOR3 vTemp2=DEGREETORADIAN(Key.vRotate+D3DXVECTOR3(.0f,fAngle,.0f));
			D3DXMatrixRotationYawPitchRoll(&matAngle,vTemp2.y,vTemp2.x,vTemp2.z);
//			D3DXMatrixScaling(&matScale,Key.vScale.x,Key.vScale.y,1.0f);
			matTemp=matAngle;
			break;
		}
	}
/*
	D3DXVECTOR3 vTemp2=DEGREETORADIAN(Key.vPosRotate+D3DXVECTOR3(.0f,fAngle,.0f));
	D3DXMatrixRotationYawPitchRoll(&matTemp2,vTemp2.y,vTemp2.x,vTemp2.z);
	D3DXVec3TransformCoord(&vTemp,&Key.vPos,&matTemp2);
*/
	vTemp = Key.vPos;
//*
	D3DXMATRIX matAngle2;
	D3DXVECTOR3 v3Rot = DEGREETORADIAN(D3DXVECTOR3(.0f,fAngle,.0f));
	D3DXMatrixRotationYawPitchRoll(&matAngle2,v3Rot.y,v3Rot.x,v3Rot.z);
	D3DXVec3TransformCoord( &vTemp, &vTemp, &matAngle2 );
/**/
	//D3DXMatrixTranslation(&matTrans,vPos.x+vTemp.x,vPos.y+vTemp.y,vPos.z+vTemp.z);
	D3DXMatrixTranslation( &matTrans, 
		vPos.x + ( vTemp.x * vScale.x ), 
		vPos.y + ( vTemp.y * vScale.y ), 
		vPos.z + ( vTemp.z * vScale.z ) );
	
	matTemp1 = matTemp * matTrans;

	switch( m_nAlphaType ) 
	{
	case SFXPARTALPHATYPE_BLEND:
		{
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	case SFXPARTALPHATYPE_GLOW:
		{
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		}
	}
	// 파라미터에 따라서 버텍스 버퍼를 생성한다.
	// 커스텀 메쉬에서의 vScale의 용도는 다른 part와 다르다. vScale.x: 윗쪽 원의 반경 vScale.y: 원통의 높이 vScale.z: 아래쪽 원의 반경
	D3DSFXVERTEX *pVertices=new D3DSFXVERTEX[((m_nPoints*2)+1)*2];
	for(int i=0;i<(m_nPoints*2)+1;i++) 
	{
		pVertices[i*2  ].p = D3DXVECTOR3(sin( DEGREETORADIAN( 360/(m_nPoints*2)*i) )*Key.vScale.x, Key.vScale.y, cos( DEGREETORADIAN( 360/(m_nPoints*2)*i))*Key.vScale.x);
		pVertices[i*2+1].p = D3DXVECTOR3(sin( DEGREETORADIAN( 360/(m_nPoints*2)*i) )*Key.vScale.z, 0           , cos( DEGREETORADIAN( 360/(m_nPoints*2)*i))*Key.vScale.z);
		pVertices[i*2  ].p.x *= vScale.x;
		pVertices[i*2  ].p.y *= vScale.y;
		pVertices[i*2  ].p.z *= vScale.z;
		pVertices[i*2+1].p.x *= vScale.x;
		pVertices[i*2+1].p.y *= vScale.y;
		pVertices[i*2+1].p.z *= vScale.z;
		
		pVertices[i*2].tv1 = Key.vPosRotate.y+0.02f; pVertices[i*2+1].tv1 = Key.vPosRotate.z;
//		pVertices[i*2].tv1 = Key.vPosRotate.y; pVertices[i*2+1].tv1 = Key.vPosRotate.z;
		if(Key.vPosRotate.x==.0f) 
		{
			if(i/2*2==i) 
			{
				pVertices[i*2].tu1=.0f; pVertices[i*2+1].tu1=.0f;
			}
			else 
			{
				pVertices[i*2].tu1=1.0f; pVertices[i*2+1].tu1=1.0f;
			}
		}
		else 
		{
			pVertices[i*2].tu1=Key.vPosRotate.x*((FLOAT)i)/(m_nPoints*2); pVertices[i*2+1].tu1=Key.vPosRotate.x*((FLOAT)i)/(m_nPoints*2);
		}
	}

//	CSfxMng::m_pd3dDevice->SetStreamSource( 0, CSfxMng::m_pSfxVB, 0, sizeof( D3DSFXVERTEX ) );
	CSfxMng::m_pd3dDevice->SetTransform( D3DTS_WORLD, &matTemp1 );
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, Key.nAlpha << 24 | 0x404040 );
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	CSfxMng::m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, m_nPoints * 4, pVertices, sizeof( D3DSFXVERTEX ) );
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	CSfxMng::m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, m_nPoints * 4, pVertices, sizeof( D3DSFXVERTEX ) );
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	safe_delete_array( pVertices );
}
#endif

void CSfxPartCustomMesh::Render2( D3DXVECTOR3 vPos, WORD nFrame, D3DXVECTOR3 fAngle, D3DXVECTOR3 vScale )
{
#ifdef __CLIENT
	if(m_nTexFrame>1) 
	{ // 텍스쳐 애니메이션일 경우
		SfxKeyFrame* pFirstKey=GetNextKey(0);
		if(nFrame<pFirstKey->nFrame) return;
		int nTexFrame= (m_nTexFrame*(nFrame-pFirstKey->nFrame)/m_nTexLoop)%m_nTexFrame; // 이 프레임에서 출력할 텍스쳐 번호
		CSfxMng::m_pd3dDevice->SetTexture(0,g_SfxTex.Tex(GetTextureName(m_strTex,nTexFrame)));
	}
	else CSfxMng::m_pd3dDevice->SetTexture(0,g_SfxTex.Tex(m_strTex));
	SfxKeyFrame* pPrevKey=GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey=GetNextKey(nFrame);
	if(pPrevKey==NULL || pNextKey==NULL) return;
	SfxKeyFrame Key;
	int dFrame=pNextKey->nFrame-pPrevKey->nFrame;
	Key=*(pPrevKey);
	if(dFrame!=0) 
	{
		Key.vPos		+= (pNextKey->vPos-pPrevKey->vPos) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vPosRotate	+= (pNextKey->vPosRotate-pPrevKey->vPosRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vRotate		+= (pNextKey->vRotate-pPrevKey->vRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vScale		+= (pNextKey->vScale-pPrevKey->vScale) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.nAlpha		+= (pNextKey->nAlpha-pPrevKey->nAlpha) * (nFrame-pPrevKey->nFrame) / dFrame;
	}

	D3DXMATRIX matTemp,matTemp1,matTemp2;
	D3DXMATRIX matTrans,matAngle,matScale,matInv;
	D3DXVECTOR3 vTemp;
	switch(m_nBillType) {
	case SFXPARTBILLTYPE_BILL:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(Key.vRotate.z-fAngle.z));
			matTemp2=g_matView;	matTemp2._41=matTemp2._42=matTemp2._43=.0f;
			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
			matTemp=matAngle*matInv;
			break;
		}
	case SFXPARTBILLTYPE_BOTTOM:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(Key.vRotate.z));
			D3DXMatrixRotationX(&matTemp2,DEGREETORADIAN(90));
			matTemp=matAngle*matTemp2;
			break;
		}
	case SFXPARTBILLTYPE_POLE:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(Key.vRotate.z));
			matTemp2=g_matView;	matTemp2._41=matTemp2._42=matTemp2._43=.0f;
			D3DXMatrixInverse(&matInv,NULL,&matTemp2);
			matTemp=matAngle*matInv;
			break;
		}
	case SFXPARTBILLTYPE_NORMAL:
		{
			D3DXMATRIX mRot;
			D3DXVECTOR3 vRot  = DEGREETORADIAN(fAngle);
			D3DXVECTOR3 vTemp2=DEGREETORADIAN(Key.vRotate);
			D3DXMatrixRotationYawPitchRoll(&matAngle,vTemp2.y,vTemp2.x,vTemp2.z);
			D3DXMatrixRotationYawPitchRoll(&mRot,vRot.y,vRot.x,vRot.z);
			matTemp=matAngle * mRot;
			break;
		}
	}

	vTemp = Key.vPos;

	D3DXMATRIX matAngle2;
	D3DXVECTOR3 v3Rot = DEGREETORADIAN(fAngle);
	D3DXMatrixRotationYawPitchRoll(&matAngle2,v3Rot.y,v3Rot.x,v3Rot.z);
	D3DXVec3TransformCoord( &vTemp, &vTemp, &matAngle2 );

	D3DXMatrixTranslation( &matTrans, 
		vPos.x + ( vTemp.x * vScale.x ), 
		vPos.y + ( vTemp.y * vScale.y ), 
		vPos.z + ( vTemp.z * vScale.z ) );
	
	matTemp1 = matTemp * matTrans;

	switch( m_nAlphaType ) 
	{
	case SFXPARTALPHATYPE_BLEND:
		{
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	case SFXPARTALPHATYPE_GLOW:
		{
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		}
	}
	// 파라미터에 따라서 버텍스 버퍼를 생성한다.
	// 커스텀 메쉬에서의 vScale의 용도는 다른 part와 다르다. vScale.x: 윗쪽 원의 반경 vScale.y: 원통의 높이 vScale.z: 아래쪽 원의 반경
	D3DSFXVERTEX *pVertices=new D3DSFXVERTEX[((m_nPoints*2)+1)*2];
	for(int i=0;i<(m_nPoints*2)+1;i++) 
	{
		pVertices[i*2  ].p = D3DXVECTOR3(sin( DEGREETORADIAN( 360/(m_nPoints*2)*i) )*Key.vScale.x, Key.vScale.y, cos( DEGREETORADIAN( 360/(m_nPoints*2)*i))*Key.vScale.x);
		pVertices[i*2+1].p = D3DXVECTOR3(sin( DEGREETORADIAN( 360/(m_nPoints*2)*i) )*Key.vScale.z, 0           , cos( DEGREETORADIAN( 360/(m_nPoints*2)*i))*Key.vScale.z);
		pVertices[i*2  ].p.x *= vScale.x;
		pVertices[i*2  ].p.y *= vScale.y;
		pVertices[i*2  ].p.z *= vScale.z;
		pVertices[i*2+1].p.x *= vScale.x;
		pVertices[i*2+1].p.y *= vScale.y;
		pVertices[i*2+1].p.z *= vScale.z;
		
		pVertices[i*2].tv1 = Key.vPosRotate.y+0.1f; pVertices[i*2+1].tv1 = Key.vPosRotate.z;
		if(Key.vPosRotate.x==.0f) 
		{
			if(i/2*2==i) 
			{
				pVertices[i*2].tu1=.0f; pVertices[i*2+1].tu1=.0f;
			}
			else 
			{
				pVertices[i*2].tu1=1.0f; pVertices[i*2+1].tu1=1.0f;
			}
		}
		else 
		{
			pVertices[i*2].tu1=Key.vPosRotate.x*((FLOAT)i)/(m_nPoints*2); pVertices[i*2+1].tu1=Key.vPosRotate.x*((FLOAT)i)/(m_nPoints*2);
		}
	}

	CSfxMng::m_pd3dDevice->SetTransform( D3DTS_WORLD, &matTemp1 );
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, Key.nAlpha << 24 | 0x404040 );
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	CSfxMng::m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, m_nPoints * 4, pVertices, sizeof( D3DSFXVERTEX ) );
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	CSfxMng::m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, m_nPoints * 4, pVertices, sizeof( D3DSFXVERTEX ) );
	CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	safe_delete_array( pVertices );
#endif //__CLIENT
}

CSfxBase::CSfxBase()
{
}
CSfxBase::~CSfxBase()
{
	for(int i=0;i<m_apParts.GetSize();i++)
		safe_delete( (CSfxPart*)m_apParts.GetAt(i) );
	m_apParts.RemoveAll();
}

CSfxPart* CSfxBase::AddPart(SFXPARTTYPE nType)
{
	CSfxPart* pSfxPart;
	switch(nType) {
		case SFXPARTTYPE_BILL:
			{
				pSfxPart=new CSfxPartBill;
				break;
			}
		case SFXPARTTYPE_PARTICLE:
			{
				pSfxPart=new CSfxPartParticle;
				//CPtrArray* pParticles=new CPtrArray;
				//m_apPart.Add(pParticles);
				
				break;
			}
		case SFXPARTTYPE_MESH:
			{
				pSfxPart=new CSfxPartMesh;
				break;
			}
		case SFXPARTTYPE_CUSTOMMESH:
			{
				pSfxPart=new CSfxPartCustomMesh;
				break;
			}
	}
	pSfxPart->m_nType=nType;
	pSfxPart->m_strTex="";
	pSfxPart->m_nBillType=SFXPARTBILLTYPE_BILL;
	pSfxPart->m_nAlphaType=SFXPARTALPHATYPE_BLEND;
	m_apParts.Add(pSfxPart);
	return pSfxPart;
}

void CSfxBase::DeletePart(BYTE nIndex)
{
	CSfxPart* pSfxPart=Part(nIndex);
	if(pSfxPart) {
		safe_delete( pSfxPart );
		m_apParts.RemoveAt(nIndex);
	}
	CString strFilename;
}






BOOL CSfxBase::LoadMerge()
{
	CString strFilename= MakePath( DIR_SFX, LPCTSTR(m_strName+".sfx") );
	int i;
	CResFile file;
	if(file.Open(strFilename,"rb")) 
	{
		char strTemp[9];
		file.Read(strTemp,8);
		strTemp[8]=0;
		CString strTemp2(strTemp);
		if(strTemp2.Left(6)=="SFX0.1") { // 버젼체크. 현재는 구버젼인지 신버젼인지만 체크해서 버젼에 맞게 로딩.
			int nPart;
			file.Read(&nPart,sizeof(int));
			for(i=0;i<nPart;i++) 
			{
				CSfxPart* pPart;
				SFXPARTTYPE nType;
				file.Read(&nType,sizeof(SFXPARTTYPE));
				pPart=AddPart(nType);
				pPart->Load(file);
			}
		}
		else 
			if(strTemp2.Left(6)=="SFX0.2") { // 버젼체크. 현재는 구버젼인지 신버젼인지만 체크해서 버젼에 맞게 로딩.
				// 신버젼. 현재 SFX0.2
				int nPart;
				file.Read(&nPart,sizeof(int));
				for(i=0;i<nPart;i++) 
				{
					CSfxPart* pPart;
					SFXPARTTYPE nType;
					file.Read(&nType,sizeof(SFXPARTTYPE));
					pPart=AddPart(nType);
					pPart->Load2(file);
				}
			}
			else
			if(strTemp2.Left(6)=="SFX0.3") { // 버젼체크. 현재는 구버젼인지 신버젼인지만 체크해서 버젼에 맞게 로딩.
				// 신버젼. 현재 SFX0.3
				int nPart;
				file.Read(&nPart,sizeof(int));
				for(i=0;i<nPart;i++) 
				{
					CSfxPart* pPart;
					SFXPARTTYPE nType;
					file.Read(&nType,sizeof(SFXPARTTYPE));
					pPart=AddPart(nType);
					if( nType == SFXPARTTYPE_PARTICLE )
						pPart->Load3(file);
					else
						pPart->Load2(file);
				}
			}
			else
			{
				// 버젼 정보 없으면 구버젼
				file.Seek(0,SEEK_SET); // 위치 다시 돌려놓고 로딩
				int nPart;
				file.Read(&nPart,sizeof(int));
				for(i=0;i<nPart;i++) 
				{
					CSfxPart* pPart;
					SFXPARTTYPE nType;
					file.Read(&nType,sizeof(SFXPARTTYPE));
					pPart=AddPart(nType);
					pPart->OldLoad(file);
				}
			}
	}
	else 
	{
		return FALSE;
	}
	file.Close();
	return TRUE;
}

BOOL CSfxBase::Load(void)
{
	for(int i=0;i<m_apParts.GetSize();i++)
		safe_delete( (CSfxPart*)m_apParts.GetAt(i) );
	m_apParts.RemoveAll();
	CString strFilename=_T( MakePath( DIR_SFX, LPCTSTR(m_strName+".sfx") ) );
	CResFile file;
	if(file.Open(strFilename,"rb")) 
	{
		char strTemp[9];
		file.Read(strTemp,8);
		strTemp[8]=0;
		CString strTemp2(strTemp);
		if(strTemp2.Left(6)=="SFX0.1") { // 버젼체크. 현재는 구버젼인지 신버젼인지만 체크해서 버젼에 맞게 로딩.
			int nPart;
			file.Read(&nPart,sizeof(int));
			for(i=0;i<nPart;i++) 
			{
				CSfxPart* pPart;
				SFXPARTTYPE nType;
				file.Read(&nType,sizeof(SFXPARTTYPE));
				pPart=AddPart(nType);
				pPart->Load(file);
			}
		}
		else 
		if(strTemp2.Left(6)=="SFX0.2") { // 버젼체크. 현재는 구버젼인지 신버젼인지만 체크해서 버젼에 맞게 로딩.
			// 신버젼. 현재 SFX0.2
			int nPart;
			file.Read(&nPart,sizeof(int));
			for(i=0;i<nPart;i++) 
			{
				CSfxPart* pPart;
				SFXPARTTYPE nType;
				file.Read(&nType,sizeof(SFXPARTTYPE));
				pPart=AddPart(nType);
				pPart->Load2(file);
			}
		}
		else
		if(strTemp2.Left(6)=="SFX0.3") { // 버젼체크. 현재는 구버젼인지 신버젼인지만 체크해서 버젼에 맞게 로딩.
			// 신버젼. 현재 SFX0.3
			int nPart;
			file.Read(&nPart,sizeof(int));
			for(i=0;i<nPart;i++) 
			{
				CSfxPart* pPart;
				SFXPARTTYPE nType;
				file.Read(&nType,sizeof(SFXPARTTYPE));
				pPart=AddPart(nType);
				if( nType == SFXPARTTYPE_PARTICLE )
					pPart->Load3(file);
				else
					pPart->Load2(file);
			}
		}
		else
		{
			// 버젼 정보 없으면 구버젼
			file.Seek(0,SEEK_SET); // 위치 다시 돌려놓고 로딩
			int nPart;
			file.Read(&nPart,sizeof(int));
			for(i=0;i<nPart;i++) 
			{
				CSfxPart* pPart;
				SFXPARTTYPE nType;
				file.Read(&nType,sizeof(SFXPARTTYPE));
				pPart=AddPart(nType);
				pPart->OldLoad(file);
			}
		}
	}
	else 
	{
		LPCTSTR szErr = Error( "CSfxBase::Load %s 찾을 수 없음", strFilename );
		ADDERRORMSG( szErr );
		return FALSE;
	}
	file.Close();
	return TRUE;
}


CSfxMng::~CSfxMng()
{
	for(int i=0;i<m_apSfxBase.GetSize();i++)
		safe_delete( (CSfxBase*)m_apSfxBase.GetAt(i) );

#ifdef __BS_EFFECT_LUA
	close_lua_sfx( );
#endif	//__BS_EFFECT_LUA
}
void CSfxMng::AddSfxBase(CSfxBase* pSfxBase)
{
	m_apSfxBase.Add(pSfxBase);
}
CSfxBase* CSfxMng::GetSfxBase(BYTE nIndex)
{
	return (CSfxBase*)(m_apSfxBase[nIndex]);
}
CSfxBase* CSfxMng::GetSfxBase( CString strSfxName )
{
	for( int i = 0; i < m_apSfxBase.GetSize(); i++ ) 
	{
		if( GetSfxBase( i )->m_strName == strSfxName )
			return GetSfxBase( i );
	}
	// strSfxName 로드
	CSfxBase* pSfxBase = new CSfxBase;
	pSfxBase->m_strName = strSfxName;
	if( pSfxBase->Load() ) 
	{
		AddSfxBase( pSfxBase );
		return pSfxBase;
	}
	else 
	{
		safe_delete( pSfxBase );
		return NULL;
	}
}
HRESULT CSfxMng::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pd3dDevice=pd3dDevice;
	return S_OK;
}
HRESULT CSfxMng::RestoreDeviceObjects()
{
	m_pd3dDevice->CreateVertexBuffer( sizeof(D3DSFXVERTEX)*4, D3DUSAGE_WRITEONLY, D3DFVF_D3DSFXVERTEX, D3DPOOL_SYSTEMMEM, &m_pSfxVB, NULL );
	D3DSFXVERTEX* pVertices;
	m_pSfxVB->Lock( 0,sizeof(D3DSFXVERTEX)*4,(void**)&pVertices, 0 );
	pVertices->p=D3DXVECTOR3(-m_fScale,m_fScale,.0f);
	pVertices->tu1=.0f;
	pVertices->tv1=.0f;
	pVertices++;
	pVertices->p=D3DXVECTOR3(m_fScale,m_fScale,.0f);
	pVertices->tu1=1.0f;
	pVertices->tv1=.0f;
	pVertices++;
	pVertices->p=D3DXVECTOR3(m_fScale,-m_fScale,.0f);
	pVertices->tu1=1.0f;
	pVertices->tv1=1.0f;
	pVertices++;
	pVertices->p=D3DXVECTOR3(-m_fScale,-m_fScale,.0f);
	pVertices->tu1=.0f;
	pVertices->tv1=1.0f;
	pVertices++;

	m_pSfxVB->Unlock();
	return S_OK;
}
HRESULT CSfxMng::InvalidateDeviceObjects()
{
	SAFE_RELEASE(m_pSfxVB);
	return S_OK;
}
HRESULT CSfxMng::DeleteDeviceObjects()
{
	return S_OK;
}

#ifdef __CLIENT
#ifndef __VM_0820
MemPooler<CSfxModel>*	CSfxModel::m_pPool	= new MemPooler<CSfxModel>( 256 );
#endif	// __VM_0820
#endif	// __CLIENT
CSfxModel::CSfxModel()
{
	m_vPos=D3DXVECTOR3(0,0,0);
	m_vRotate=D3DXVECTOR3(0,0,0);
	m_vScale=D3DXVECTOR3(1,1,1);
	m_nCurFrame=0;
	m_pSfxBase=NULL;
}
CSfxModel::~CSfxModel()
{
	DeleteAll();
}

void CSfxModel::DeleteAll(void)
{
	int i,j;
	CPtrArray* pParticles;
	for(i=0;i<m_apParticles.GetSize();i++) 
	{
		pParticles=(CPtrArray*)m_apParticles[i];
		if(pParticles) 
		{
			for(j=0;j<pParticles->GetSize();j++)
			{
				safe_delete( (Particle*)pParticles->GetAt(j) );
			}
		}
		safe_delete( pParticles );
	}
	m_apParticles.RemoveAll();
}
void CSfxModel::RemovePart( int nIndex )
{
	CSfxPart* pPart = m_pSfxBase->Part( nIndex );
	pPart->DeleteAllKeyFrame();
	m_pSfxBase->DeletePart( nIndex );
	if( m_apParticles.GetSize() )
	{
		CPtrArray* pParticles = (CPtrArray*)m_apParticles.GetAt( nIndex );
		if( pParticles ) 
		{
			for( int i = 0; i < pParticles->GetSize(); i++ )
			{
				safe_delete( (Particle*)pParticles->GetAt( i ) );
			}
		}
		safe_delete( pParticles );
		m_apParticles.RemoveAt( nIndex );
	}
}
void CSfxModel::SetSfx( CSfxBase* pSfxBase )
{
	m_vPos=D3DXVECTOR3(0,0,0);
	m_vRotate=D3DXVECTOR3(0,0,0);
	m_vScale=D3DXVECTOR3(1,1,1);
	m_nCurFrame=0;
	m_pSfxBase=NULL;
	
	m_pSfxBase = pSfxBase;
	DeleteAll();
	for( int i = 0; i < pSfxBase->m_apParts.GetSize(); i++ )  
	{
		if(pSfxBase->Part(i)->m_nType==SFXPARTTYPE_PARTICLE) 
		{
			CPtrArray* pParticles=new CPtrArray;
			m_apParticles.Add(pParticles);
		}
		else 
		{
			m_apParticles.Add(NULL);
		}
	}
	//     - z
	//   3 | 2
	// - --+-- + x
	//   0 | 1
	//
	//   7 | 6
	// - --+-- + x
	//   4 | 5
	// 바운드 박스 사이즈 임의 지정. 이것이 있어야 컬링이 가능
	m_vMin = D3DXVECTOR3( -1.0f, 0.0f, -1.0f );
	m_vMax = D3DXVECTOR3(  1.0f, 1.0f,  1.0f );
	m_BB.m_vPos[0] = D3DXVECTOR3( -1.0f, 1.0f, -1.0f ); 
	m_BB.m_vPos[1] = D3DXVECTOR3(  1.0f, 1.0f, -1.0f ); 
	m_BB.m_vPos[2] = D3DXVECTOR3(  1.0f, 1.0f,  1.0f );
	m_BB.m_vPos[3] = D3DXVECTOR3( -1.0f, 1.0f,  1.0f );
	m_BB.m_vPos[4] = D3DXVECTOR3( -1.0f, 0.0f, -1.0f );
	m_BB.m_vPos[5] = D3DXVECTOR3(  1.0f, 0.0f, -1.0f );
	m_BB.m_vPos[6] = D3DXVECTOR3(  1.0f, 0.0f,  1.0f );
	m_BB.m_vPos[7] = D3DXVECTOR3( -1.0f, 0.0f,  1.0f );
}
void CSfxModel::SetSfx( LPCTSTR szSfxName )
{
	m_pSfxBase = g_SfxMng.GetSfxBase( szSfxName );
	if( m_pSfxBase ) 
	{
		DeleteAll();
		for( int i = 0; i < m_pSfxBase->m_apParts.GetSize(); i++ ) 
		{
			if( m_pSfxBase->Part( i )->m_nType == SFXPARTTYPE_PARTICLE ) 
			{
				CPtrArray* pParticles=new CPtrArray;
				m_apParticles.Add(pParticles);
			}
			else 
			{
				m_apParticles.Add(NULL);
			}
		}
	}
	//     - z
	//   3 | 2
	// - --+-- + x
	//   0 | 1
	//
	//   7 | 6
	// - --+-- + x
	//   4 | 5
	// 바운드 박스 사이즈 임의 지정. 이것이 있어야 컬링이 가능
	m_vMin = D3DXVECTOR3( -1.0f, 0.0f, -1.0f );
	m_vMax = D3DXVECTOR3(  1.0f, 1.0f,  1.0f );
	m_BB.m_vPos[0] = D3DXVECTOR3( -1.0f, 1.0f, -1.0f ); 
	m_BB.m_vPos[1] = D3DXVECTOR3(  1.0f, 1.0f, -1.0f ); 
	m_BB.m_vPos[2] = D3DXVECTOR3(  1.0f, 1.0f,  1.0f );
	m_BB.m_vPos[3] = D3DXVECTOR3( -1.0f, 1.0f,  1.0f );
	m_BB.m_vPos[4] = D3DXVECTOR3( -1.0f, 0.0f, -1.0f );
	m_BB.m_vPos[5] = D3DXVECTOR3(  1.0f, 0.0f, -1.0f );
	m_BB.m_vPos[6] = D3DXVECTOR3(  1.0f, 0.0f,  1.0f );
	m_BB.m_vPos[7] = D3DXVECTOR3( -1.0f, 0.0f,  1.0f );
}

void CSfxModel::SetSfx( DWORD dwIndex )
{
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( OT_SFX, dwIndex );

	if( lpModelElem )
	{
		m_pSfxBase = g_SfxMng.GetSfxBase( lpModelElem->m_szName );

		if( m_pSfxBase ) 
		{
			DeleteAll();
			for( int i = 0; i < m_pSfxBase->m_apParts.GetSize(); i++ ) 
			{
				if( m_pSfxBase->Part( i )->m_nType == SFXPARTTYPE_PARTICLE ) 
				{
					CPtrArray* pParticles=new CPtrArray;
					m_apParticles.Add(pParticles);
				}
				else 
				{
					m_apParticles.Add(NULL);
				}
			}
		}
	}
	//     - z
	//   3 | 2
	// - --+-- + x
	//   0 | 1
	//
	//   7 | 6
	// - --+-- + x
	//   4 | 5
	// 바운드 박스 사이즈 임의 지정. 이것이 있어야 컬링이 가능
	m_vMin = D3DXVECTOR3( -1.0f, 0.0f, -1.0f );
	m_vMax = D3DXVECTOR3(  1.0f, 1.0f,  1.0f );
	m_BB.m_vPos[0] = D3DXVECTOR3( -1.0f, 1.0f, -1.0f ); 
	m_BB.m_vPos[1] = D3DXVECTOR3(  1.0f, 1.0f, -1.0f ); 
	m_BB.m_vPos[2] = D3DXVECTOR3(  1.0f, 1.0f,  1.0f );
	m_BB.m_vPos[3] = D3DXVECTOR3( -1.0f, 1.0f,  1.0f );
	m_BB.m_vPos[4] = D3DXVECTOR3( -1.0f, 0.0f, -1.0f );
	m_BB.m_vPos[5] = D3DXVECTOR3(  1.0f, 0.0f, -1.0f );
	m_BB.m_vPos[6] = D3DXVECTOR3(  1.0f, 0.0f,  1.0f );
	m_BB.m_vPos[7] = D3DXVECTOR3( -1.0f, 0.0f,  1.0f );
}

BOOL CSfxModel::Render2( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld )
{
#ifdef __CLIENT
	if(m_pSfxBase) 
	{
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,		D3DTOP_DISABLE);
		
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
		
		for( int i = 0 ; i < m_pSfxBase->m_apParts.GetSize(); i++ ) 
		{
			if( m_pSfxBase->Part( i )->m_bUseing == FALSE )
				continue;
			
			switch( m_pSfxBase->Part( i )->m_nType ) 
			{
			case SFXPARTTYPE_BILL:
				{
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
					
					CSfxMng::m_pd3dDevice->SetVertexShader( NULL );
					CSfxMng::m_pd3dDevice->SetVertexDeclaration( NULL );
					CSfxMng::m_pd3dDevice->SetFVF( D3DFVF_D3DSFXVERTEX );
					CSfxMng::m_pd3dDevice->SetStreamSource( 0, CSfxMng::m_pSfxVB, 0, sizeof( D3DSFXVERTEX ) );

					m_pSfxBase->Part( i )->Render2( m_vPos, m_nCurFrame, m_vRotate, m_vScale );

					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
				}
				break;
			case SFXPARTTYPE_PARTICLE:
				{
					//  파티클이 프로세스 안되었으면 아직 생성되지 않았다.
					// m_apPart는 생성된 파티클들의 배열들.
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
					if( i < m_apParticles.GetSize() )
					{
						CSfxMng::m_pd3dDevice->SetVertexShader( NULL );
						CSfxMng::m_pd3dDevice->SetVertexDeclaration( NULL );
						CSfxMng::m_pd3dDevice->SetFVF( D3DFVF_D3DSFXVERTEX );
						CSfxMng::m_pd3dDevice->SetStreamSource( 0, CSfxMng::m_pSfxVB, 0, sizeof( D3DSFXVERTEX ) );
						RenderParticles2( m_vPos, m_nCurFrame, m_vRotate, (CSfxPartParticle*)(m_pSfxBase->Part( i ) ),(CPtrArray*)(m_apParticles[ i ] ), m_vScale );
					}
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
				}
				break;
			case SFXPARTTYPE_MESH:
				{
					m_pSfxBase->Part(i)->Render( m_vPos, m_nCurFrame, m_vRotate.y, m_vScale );
				}
				break;
			case SFXPARTTYPE_CUSTOMMESH:
				{
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
					CSfxMng::m_pd3dDevice->SetVertexShader( NULL );
					CSfxMng::m_pd3dDevice->SetVertexDeclaration( NULL );
					CSfxMng::m_pd3dDevice->SetFVF( D3DFVF_D3DSFXVERTEX );
					m_pSfxBase->Part(i)->Render2( m_vPos, m_nCurFrame, m_vRotate, m_vScale );
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
				}
				break;
			}
		}
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	}
#endif //__CLIENT
	return TRUE;
}
void CSfxModel::RenderParticles2( D3DXVECTOR3 vPos, WORD nFrame, D3DXVECTOR3 fAngle, CSfxPartParticle* pPartParticle, CPtrArray* pParticles, D3DXVECTOR3 vScale )
{
#ifdef __CLIENT
	// 파티클 part는 다른 part들과 다르게 모델 오브젝트에서 직접 렌더한다.
	// 똑같은 sfx가 여러개 나올수도 있는데 각 파티클들의 데이터를 공유할 수 없기 때문이다.

	// 기본 world 매트릭스의 계산 등은 빌보드part의 그것과 크게 다르지 않다.
	CSfxMng::m_pd3dDevice->SetTexture(0,g_SfxTex.Tex(pPartParticle->m_strTex));
	SfxKeyFrame* pPrevKey=pPartParticle->GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey=pPartParticle->GetNextKey(nFrame);
	if(pPrevKey==NULL || pNextKey==NULL) return;
	SfxKeyFrame Key;
	int dFrame=pNextKey->nFrame-pPrevKey->nFrame;
	Key=*(pPrevKey);
	if(dFrame!=0) 
	{
		Key.vPos		+= (pNextKey->vPos-pPrevKey->vPos) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vPosRotate	+= (pNextKey->vPosRotate-pPrevKey->vPosRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vRotate		+= (pNextKey->vRotate-pPrevKey->vRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		//Key.vScale	+=(pNextKey->vScale-pPrevKey->vScale)*(nFrame-pPrevKey->nFrame)/dFrame;

		if( !pPartParticle->m_bRepeatScal )
		{
			Key.vScale	+= (pNextKey->vScale-pPrevKey->vScale) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		}
		

		Key.nAlpha+=(pNextKey->nAlpha-pPrevKey->nAlpha)*(nFrame-pPrevKey->nFrame)/dFrame;
	}

	D3DXMATRIX matTemp,matTemp1,matTemp2;
	D3DXMATRIX matTrans,matAngle,matScale,matInv;
	D3DXVECTOR3 vTemp;
	switch(pPartParticle->m_nBillType) 
	{
	case SFXPARTBILLTYPE_BILL:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(Key.vRotate.z));
			matTemp2=g_matView;	matTemp2._41=matTemp2._42=matTemp2._43=.0f;
			matTemp=matAngle*g_matInvView;
			break;
		}
	case SFXPARTBILLTYPE_BOTTOM:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(Key.vRotate.z));
			D3DXMatrixRotationX(&matTemp2,DEGREETORADIAN(90));
			matTemp=matAngle*matTemp2;
			break;
		}
	case SFXPARTBILLTYPE_POLE:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(Key.vRotate.z));
			matTemp2=g_matView;	matTemp2._41=matTemp2._42=matTemp2._43=.0f;
			matTemp=matAngle*g_matInvView;
			break;
		}
	case SFXPARTBILLTYPE_NORMAL:
		{
			D3DXMATRIX mRot;
			D3DXVECTOR3 vRot  = DEGREETORADIAN(fAngle);
			D3DXVECTOR3 vTemp2=DEGREETORADIAN(Key.vRotate);
			D3DXMatrixRotationYawPitchRoll(&matAngle,vTemp2.y,vTemp2.x,vTemp2.z);
			D3DXMatrixRotationYawPitchRoll(&mRot,vRot.y,vRot.x,vRot.z);
			//			D3DXMatrixScaling(&matScale,Key.vScale.x,Key.vScale.y,1.0f);
			matTemp=matAngle * mRot;
			break;
		}
	}
	switch(pPartParticle->m_nAlphaType) 
	{
	case SFXPARTALPHATYPE_BLEND:
		{
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	case SFXPARTALPHATYPE_GLOW:
		{
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		}
	}

	D3DXMATRIX  mRot;
	D3DXVECTOR3 vAngle = DEGREETORADIAN( fAngle );
	D3DXVECTOR3 vTemp2 = DEGREETORADIAN( Key.vPosRotate );
	
	D3DXMatrixRotationYawPitchRoll( &mRot, vAngle.y, vAngle.x, vAngle.z );
	D3DXMatrixRotationYawPitchRoll( &matTemp2, vTemp2.y, vTemp2.x, vTemp2.z );
	matTemp2 = mRot * matTemp2;
	D3DXVec3TransformCoord( &vTemp, &(Key.vPos), &matTemp2 );

	// 위에서 산출된 기본 world 매트릭스를 로컬 원점으로 현재 생성되어 있는 모든 파티클들을 렌더한다.
	D3DXVECTOR3 vTemp3;
	Particle* pParticle = NULL;
	CString		TexName;

	for( int i = 0; i < pParticles->GetSize(); i++ ) 
	{
		pParticle = (Particle*)(pParticles->GetAt(i));
#ifdef __SFX_OPT
		if(pPartParticle->m_nTexFrame>1) 
		{ // 텍스쳐 애니메이션일 경우
			int nTexFrame= (pPartParticle->m_nTexFrame*(pParticle->nFrame)/pPartParticle->m_nTexLoop)%pPartParticle->m_nTexFrame; // 이 프레임에서 출력할 텍스쳐 번호
			CString tempTexName = GetTextureName(pPartParticle->m_strTex,nTexFrame);
			if(TexName != tempTexName)
			{
				CSfxMng::m_pd3dDevice->SetTexture(0,g_SfxTex.Tex(tempTexName));
				TexName = tempTexName;
			}
		}
		else 
		{
			if(TexName != pPartParticle->m_strTex)
			{
				CSfxMng::m_pd3dDevice->SetTexture( 0, g_SfxTex.Tex( pPartParticle->m_strTex ) );
				TexName = pPartParticle->m_strTex;
			}
		}
#else
		if(pPartParticle->m_nTexFrame>1) 
		{ // 텍스쳐 애니메이션일 경우
			int nTexFrame= (pPartParticle->m_nTexFrame*(pParticle->nFrame)/pPartParticle->m_nTexLoop)%pPartParticle->m_nTexFrame; // 이 프레임에서 출력할 텍스쳐 번호
			CSfxMng::m_pd3dDevice->SetTexture(0,g_SfxTex.Tex(GetTextureName(pPartParticle->m_strTex,nTexFrame)));
		}
		else 
			CSfxMng::m_pd3dDevice->SetTexture( 0, g_SfxTex.Tex( pPartParticle->m_strTex ) );
#endif	
		if( pPartParticle->m_bRepeatScal )
		{
			
			if( D3DXVec3Length( &pParticle->vScale ) >= D3DXVec3Length( &pParticle->vScaleEnd) )
				pParticle->bSwScal = TRUE;
			else
				if( D3DXVec3Length( &pParticle->vScale ) <= D3DXVec3Length( &pParticle->vScaleStart) )
					pParticle->bSwScal = FALSE;
				
				if( pParticle->bSwScal )
					pParticle->vScale -= pParticle->vScaleSpeed;
				else
					pParticle->vScale += pParticle->vScaleSpeed;
		}

		vTemp3 = DEGREETORADIAN( Key.vRotate + Key.vPosRotate );
		D3DXMatrixRotationYawPitchRoll( &matTemp2, vTemp3.y, vTemp3.x, vTemp3.z );
		matTemp2 *= mRot;
		D3DXVec3TransformCoord( &vTemp3, &(pParticle->vPos), &matTemp2 );
		/*
		D3DXMatrixTranslation( &matTrans, 
			vTemp.x + ( vTemp3.x ) + vPos.x, 
			vTemp.y + ( vTemp3.y ) + vPos.y, 
			vTemp.z + ( vTemp3.z ) + vPos.z );
		
*/
		D3DXMatrixTranslation( &matTrans, 
			vTemp.x + ( vTemp3.x * m_vScale.x) + vPos.x, 
			vTemp.y + ( vTemp3.y * m_vScale.y) + vPos.y, 
			vTemp.z + ( vTemp3.z * m_vScale.z) + vPos.z );

		D3DXMatrixScaling( &matScale, pParticle->vScale.x * m_vScale.x, pParticle->vScale.y * m_vScale.y, 1.0f * m_vScale.z );
		//D3DXMatrixScaling( &matScale, pParticle->vScale.x, pParticle->vScale.y, 1.0f );

		//D3DXMATRIX mScale;
		//D3DXMatrixScaling( &mScale, vScale.x, vScale.y, vScale.z );
		//D3DXMatrixMultiply( &matScale, &matScale, &mScale );

//#ifdef __ATEST
		D3DXMATRIX matRot;
		D3DXVECTOR3 v3Rot = D3DXVECTOR3(0.0f,0.0f,0.0f);
		
		if( dFrame != 0 )
		{
			FLOAT Lerp = (FLOAT)(nFrame-pPrevKey->nFrame)/dFrame;
			D3DXVec3Lerp( &v3Rot, &D3DXVECTOR3(0.0f,0.0f,0.0f), &pParticle->vRotation, Lerp );
		}
		
		D3DXMatrixRotationYawPitchRoll(&matRot, DEGREETORADIAN(v3Rot.y), DEGREETORADIAN(v3Rot.x), DEGREETORADIAN(v3Rot.z) );
		
		matTemp1 = matRot * matScale * matTemp * matTrans;
//#else
//		matTemp1 = matScale * matTemp * matTrans;
//#endif

		CSfxMng::m_pd3dDevice->SetTransform( D3DTS_WORLD, &matTemp1 );

		// 설정된 파라미터와 현재 frame에 따라 알파값 계산
		int nTempAlpha=0;
		if(pParticle->nFrame<pPartParticle->m_nParticleFrameAppear) 
		{ // 나타나는 중이다
			nTempAlpha = pParticle->nFrame*Key.nAlpha/pPartParticle->m_nParticleFrameAppear;
		}
		else 
		if(pParticle->nFrame>pPartParticle->m_nParticleFrameKeep) 
		{ // 사라지는 중이다
			nTempAlpha = Key.nAlpha - (Key.nAlpha * (pParticle->nFrame-pPartParticle->m_nParticleFrameKeep) / (pPartParticle->m_nParticleFrameDisappear-pPartParticle->m_nParticleFrameKeep));
		}
		else 
		{ // 둘 다 아니면 알파값 유지중이다
			nTempAlpha=Key.nAlpha;
		}
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, nTempAlpha<<24 | 0x404040 );

		CSfxMng::m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
	}
#endif //__CLIENT
}

#ifndef __WORLDSERVER
//void CSfxModel::Render(void)
BOOL CSfxModel::RenderZ( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld )
{
	if(m_pSfxBase) 
	{
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,		D3DTOP_DISABLE);

		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

		for( int i = 0 ; i < m_pSfxBase->m_apParts.GetSize(); i++ ) 
		{
			if( m_pSfxBase->Part( i )->m_bUseing == FALSE )
				continue;

			switch( m_pSfxBase->Part( i )->m_nType ) 
			{
			case SFXPARTTYPE_BILL:
				{
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
					CSfxMng::m_pd3dDevice->SetVertexShader( NULL );
					CSfxMng::m_pd3dDevice->SetVertexDeclaration( NULL );
					CSfxMng::m_pd3dDevice->SetFVF( D3DFVF_D3DSFXVERTEX );
					CSfxMng::m_pd3dDevice->SetStreamSource( 0, CSfxMng::m_pSfxVB, 0, sizeof( D3DSFXVERTEX ) );
					m_pSfxBase->Part( i )->Render( m_vPos, m_nCurFrame, m_vRotate.y, m_vScale );
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
				}
				break;
			case SFXPARTTYPE_PARTICLE:
				{
					//  파티클이 프로세스 안되었으면 아직 생성되지 않았다.
					// m_apPart는 생성된 파티클들의 배열들.
					
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
					
					if( i < m_apParticles.GetSize() )
					{
						CSfxMng::m_pd3dDevice->SetVertexShader( NULL );
						CSfxMng::m_pd3dDevice->SetVertexDeclaration( NULL );
						CSfxMng::m_pd3dDevice->SetFVF( D3DFVF_D3DSFXVERTEX );
						CSfxMng::m_pd3dDevice->SetStreamSource( 0, CSfxMng::m_pSfxVB, 0, sizeof( D3DSFXVERTEX ) );
						RenderParticles( m_vPos, m_nCurFrame, m_vRotate.y, (CSfxPartParticle*)(m_pSfxBase->Part( i ) ),(CPtrArray*)(m_apParticles[ i ] ), m_vScale );
					}

					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
					
				}
				break;
			case SFXPARTTYPE_MESH:
				{
					m_pSfxBase->Part(i)->Render( m_vPos, m_nCurFrame, m_vRotate.y, m_vScale );
				}
				break;
			case SFXPARTTYPE_CUSTOMMESH:
				{
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
					CSfxMng::m_pd3dDevice->SetFVF( D3DFVF_D3DSFXVERTEX );
					m_pSfxBase->Part(i)->Render( m_vPos, m_nCurFrame, m_vRotate.y, m_vScale );
					
				}
				break;
			}
		}
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	}
	return TRUE;
}

BOOL CSfxModel::Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld )
{
	if(m_pSfxBase) 
	{
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE);
		CSfxMng::m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,		D3DTOP_DISABLE);

		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

		for( int i = 0 ; i < m_pSfxBase->m_apParts.GetSize(); i++ ) 
		{
			if( m_pSfxBase->Part( i )->m_bUseing == FALSE )
				continue;

			switch( m_pSfxBase->Part( i )->m_nType ) 
			{
			case SFXPARTTYPE_BILL:
				{
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
					CSfxMng::m_pd3dDevice->SetVertexShader( NULL );
					CSfxMng::m_pd3dDevice->SetVertexDeclaration( NULL );
					CSfxMng::m_pd3dDevice->SetFVF( D3DFVF_D3DSFXVERTEX );
					CSfxMng::m_pd3dDevice->SetStreamSource( 0, CSfxMng::m_pSfxVB, 0, sizeof( D3DSFXVERTEX ) );
					m_pSfxBase->Part( i )->Render( m_vPos, m_nCurFrame, m_vRotate.y, m_vScale );
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
				}
				break;
			case SFXPARTTYPE_PARTICLE:
				{
					//  파티클이 프로세스 안되었으면 아직 생성되지 않았다.
					// m_apPart는 생성된 파티클들의 배열들.
					
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
					
					if( i < m_apParticles.GetSize() )
					{
						CSfxMng::m_pd3dDevice->SetVertexShader( NULL );
						CSfxMng::m_pd3dDevice->SetVertexDeclaration( NULL );
						CSfxMng::m_pd3dDevice->SetFVF( D3DFVF_D3DSFXVERTEX );
						CSfxMng::m_pd3dDevice->SetStreamSource( 0, CSfxMng::m_pSfxVB, 0, sizeof( D3DSFXVERTEX ) );
						RenderParticles( m_vPos, m_nCurFrame, m_vRotate.y, (CSfxPartParticle*)(m_pSfxBase->Part( i ) ),(CPtrArray*)(m_apParticles[ i ] ), m_vScale );
					}

					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
					
				}
				break;
			case SFXPARTTYPE_MESH:
				{
					m_pSfxBase->Part(i)->Render( m_vPos, m_nCurFrame, m_vRotate.y, m_vScale );
				}
				break;
			case SFXPARTTYPE_CUSTOMMESH:
				{
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
					CSfxMng::m_pd3dDevice->SetFVF( D3DFVF_D3DSFXVERTEX );
					m_pSfxBase->Part(i)->Render( m_vPos, m_nCurFrame, m_vRotate.y, m_vScale );
					CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
				}
				break;
			}
		}
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		CSfxMng::m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	}
	return TRUE;
}

void CSfxModel::RenderParticles( D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, CSfxPartParticle* pPartParticle, CPtrArray* pParticles, D3DXVECTOR3 vScale )
{
	// 파티클 part는 다른 part들과 다르게 모델 오브젝트에서 직접 렌더한다.
	// 똑같은 sfx가 여러개 나올수도 있는데 각 파티클들의 데이터를 공유할 수 없기 때문이다.

	// 기본 world 매트릭스의 계산 등은 빌보드part의 그것과 크게 다르지 않다.
	//CSfxMng::m_pd3dDevice->SetTexture(0,g_SfxTex.Tex(pPartParticle->m_strTex));
	SfxKeyFrame* pPrevKey=pPartParticle->GetPrevKey(nFrame);
	SfxKeyFrame* pNextKey=pPartParticle->GetNextKey(nFrame);
	if(pPrevKey==NULL || pNextKey==NULL) return;
	SfxKeyFrame Key;
	int dFrame=pNextKey->nFrame-pPrevKey->nFrame;
	Key=*(pPrevKey);
	if(dFrame!=0) 
	{
		Key.vPos		+= (pNextKey->vPos-pPrevKey->vPos) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vPosRotate	+= (pNextKey->vPosRotate-pPrevKey->vPosRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		Key.vRotate		+= (pNextKey->vRotate-pPrevKey->vRotate) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		
		if( !pPartParticle->m_bRepeatScal )
		{
			Key.vScale	+= (pNextKey->vScale-pPrevKey->vScale) * (FLOAT)( (nFrame-pPrevKey->nFrame) ) / (FLOAT)( dFrame );
		}
		Key.nAlpha+=(pNextKey->nAlpha-pPrevKey->nAlpha)*(nFrame-pPrevKey->nFrame)/dFrame;
	}

	D3DXMATRIX matTemp,matTemp1,matTemp2;
	D3DXMATRIX matTrans,matAngle,matScale,matInv;
	D3DXVECTOR3 vTemp;
	switch(pPartParticle->m_nBillType) 
	{
	case SFXPARTBILLTYPE_BILL:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(Key.vRotate.z));
			matTemp2=g_matView;	matTemp2._41=matTemp2._42=matTemp2._43=.0f;
			matTemp=matAngle*g_matInvView;
			break;
		}
	case SFXPARTBILLTYPE_BOTTOM:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(Key.vRotate.z-fAngle));
			D3DXMatrixRotationX(&matTemp2,DEGREETORADIAN(90));
			matTemp=matAngle*matTemp2;
			break;
		}
	case SFXPARTBILLTYPE_POLE:
		{
			D3DXMatrixRotationZ(&matAngle,DEGREETORADIAN(Key.vRotate.z));
			matTemp2=g_matView;	matTemp2._41=matTemp2._42=matTemp2._43=.0f;
			matTemp=matAngle*g_matInvView;
			break;
		}
	case SFXPARTBILLTYPE_NORMAL:
		{
			D3DXVECTOR3 vTemp2=DEGREETORADIAN(Key.vRotate+D3DXVECTOR3(.0f,fAngle,.0f));
			D3DXMatrixRotationYawPitchRoll(&matAngle,vTemp2.y,vTemp2.x,vTemp2.z);
			matTemp=matAngle;
			break;
		}
	}
	switch(pPartParticle->m_nAlphaType) 
	{
	case SFXPARTALPHATYPE_BLEND:
		{
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	case SFXPARTALPHATYPE_GLOW:
		{
			CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		}
	}

	D3DXVECTOR3 vTemp2 = DEGREETORADIAN( Key.vPosRotate + D3DXVECTOR3(.0f,fAngle,.0f) );
	D3DXMatrixRotationYawPitchRoll( &matTemp2, vTemp2.y, vTemp2.x, vTemp2.z );
	D3DXVec3TransformCoord( &vTemp, &(Key.vPos), &matTemp2 );

	if( pParticles == NULL )
		return;

	// 위에서 산출된 기본 world 매트릭스를 로컬 원점으로 현재 생성되어 있는 모든 파티클들을 렌더한다.
	D3DXVECTOR3 vTemp3;
	Particle* pParticle = NULL;
	CString TexName;

	for( int i = 0; i < pParticles->GetSize(); i++ ) 
	{
		pParticle = (Particle*)(pParticles->GetAt(i));
#ifdef __SFX_OPT
		if(pPartParticle->m_nTexFrame>1) 
		{ // 텍스쳐 애니메이션일 경우
			int nTexFrame= (pPartParticle->m_nTexFrame*(pParticle->nFrame)/pPartParticle->m_nTexLoop)%pPartParticle->m_nTexFrame; // 이 프레임에서 출력할 텍스쳐 번호
			CString tempTexName = GetTextureName(pPartParticle->m_strTex,nTexFrame);
			if(TexName != tempTexName)
			{
				CSfxMng::m_pd3dDevice->SetTexture(0,g_SfxTex.Tex(tempTexName));
				TexName = tempTexName;
			}
		}
		else 
		{
			if(TexName != pPartParticle->m_strTex)
			{
				CSfxMng::m_pd3dDevice->SetTexture( 0, g_SfxTex.Tex( pPartParticle->m_strTex ) );
				TexName = pPartParticle->m_strTex;
			}
		}
#else
		if(pPartParticle->m_nTexFrame>1) 
		{ // 텍스쳐 애니메이션일 경우
			int nTexFrame= (pPartParticle->m_nTexFrame*(pParticle->nFrame)/pPartParticle->m_nTexLoop)%pPartParticle->m_nTexFrame; // 이 프레임에서 출력할 텍스쳐 번호
			CSfxMng::m_pd3dDevice->SetTexture(0,g_SfxTex.Tex(GetTextureName(pPartParticle->m_strTex,nTexFrame)));
		}
		else 
			CSfxMng::m_pd3dDevice->SetTexture( 0, g_SfxTex.Tex( pPartParticle->m_strTex ) );
#endif	

		if( pPartParticle->m_bRepeatScal )
		{

			if( D3DXVec3Length( &pParticle->vScale ) >= D3DXVec3Length( &pParticle->vScaleEnd) )
				pParticle->bSwScal = TRUE;
			else
			if( D3DXVec3Length( &pParticle->vScale ) <= D3DXVec3Length( &pParticle->vScaleStart) )
				pParticle->bSwScal = FALSE;

			if( pParticle->bSwScal )
				pParticle->vScale -= pParticle->vScaleSpeed;
			else
				pParticle->vScale += pParticle->vScaleSpeed;
		}

		vTemp3 = DEGREETORADIAN( Key.vRotate + Key.vPosRotate + D3DXVECTOR3( .0f, fAngle, .0f ) );
		D3DXMatrixRotationYawPitchRoll( &matTemp2, vTemp3.y, vTemp3.x, vTemp3.z );
		D3DXVec3TransformCoord( &vTemp3, &(pParticle->vPos), &matTemp2 );

		D3DXMatrixTranslation( &matTrans, 
			vTemp.x + ( vTemp3.x * m_vScale.x) + vPos.x, 
			vTemp.y + ( vTemp3.y * m_vScale.y) + vPos.y, 
			vTemp.z + ( vTemp3.z * m_vScale.z) + vPos.z );

		D3DXMatrixScaling( &matScale, pParticle->vScale.x * m_vScale.x, pParticle->vScale.y * m_vScale.y, 1.0f * m_vScale.z );
		//D3DXMatrixScaling( &matScale, pParticle->vScale.x, pParticle->vScale.y, 1.0f );
		//D3DXVec3TransformCoord(&vPos,&vPos,&matRot);

//#ifdef __ATEST
		D3DXMATRIX matRot;
		D3DXVECTOR3 v3Rot = D3DXVECTOR3(0.0f,0.0f,0.0f);
		
		if( dFrame != 0 )
		{
			FLOAT Lerp = (FLOAT)(nFrame-pPrevKey->nFrame)/dFrame;
			D3DXVec3Lerp( &v3Rot, &D3DXVECTOR3(0.0f,0.0f,0.0f), &pParticle->vRotation, Lerp );
		}

		D3DXMatrixRotationYawPitchRoll(&matRot, DEGREETORADIAN(v3Rot.y), DEGREETORADIAN(v3Rot.x), DEGREETORADIAN(v3Rot.z) );
		
		matTemp1 = matRot * matScale * matTemp * matTrans;
//#else
//		matTemp1 = matScale * matTemp * matTrans;
//#endif
		
		
		
		//D3DXMATRIX mScale;
		//D3DXMatrixScaling( &mScale, vScale.x, vScale.y, vScale.z );
		//D3DXMatrixMultiply( &matScale, &matScale, &mScale );
		
		CSfxMng::m_pd3dDevice->SetTransform( D3DTS_WORLD, &matTemp1 );

		// 설정된 파라미터와 현재 frame에 따라 알파값 계산
		int nTempAlpha=0;
		if(pParticle->nFrame<pPartParticle->m_nParticleFrameAppear) 
		{ // 나타나는 중이다
			nTempAlpha = pParticle->nFrame*Key.nAlpha/pPartParticle->m_nParticleFrameAppear;
		}
		else 
		if(pParticle->nFrame>pPartParticle->m_nParticleFrameKeep) 
		{ // 사라지는 중이다
			nTempAlpha = Key.nAlpha - (Key.nAlpha * (pParticle->nFrame-pPartParticle->m_nParticleFrameKeep) / (pPartParticle->m_nParticleFrameDisappear-pPartParticle->m_nParticleFrameKeep));
		}
		else 
		{ // 둘 다 아니면 알파값 유지중이다
			nTempAlpha=Key.nAlpha;
		}
		CSfxMng::m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, nTempAlpha<<24 | 0x404040 );

		CSfxMng::m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2);
	}
}
#endif 
BOOL CSfxModel::Process(void)
{
	BOOL ret=TRUE;
	CSfxPartParticle* pPartParticle;
	Particle* pParticle;

	m_nCurFrame++;
	
	// 이 sfx에 파티클 part가 포함되어 있다면 파티클의 생성, 파괴 등을 처리한다.
	for( int i = 0; i < m_apParticles.GetSize(); i++ ) 
	{
		CPtrArray* pParticles = (CPtrArray*)m_apParticles[ i ];
		if( pParticles ) 
		{
			pPartParticle = (CSfxPartParticle*)(m_pSfxBase->Part( i ) );

			if( pPartParticle->m_bUseing == FALSE )
				continue;

			SfxKeyFrame Key;
			pPartParticle->GetKey( m_nCurFrame, &Key );

			// 파티클 이동, 생성 및 제거
			int j;
			for( j = 0; j < pParticles->GetSize(); j++ ) 
			{
				pParticle = (Particle*)(pParticles->GetAt( j ) );
				pParticle->vPos += pParticle->vSpeed;
				pParticle->vSpeed += pPartParticle->m_vParticleAccel;

				if( !pPartParticle->m_bRepeatScal )
					pParticle->vScale += pPartParticle->m_vScaleSpeed;

				pParticle->nFrame++;

				if( pParticle->nFrame >= pPartParticle->m_nParticleFrameDisappear ) 
				{
					safe_delete( pParticle );
					pParticles->RemoveAt( j );
					j--;
				}
			}
			int nStartFrame = 0;
			int nEndFrame = 0;
			if( pPartParticle->Key( 0 ) ) 
			{
				nStartFrame = pPartParticle->Key( 0 )->nFrame;
				nEndFrame = pPartParticle->Key( pPartParticle->m_apKeyFrames.GetSize() - 1 )->nFrame;
			}
			if( m_nCurFrame >= nStartFrame && m_nCurFrame <= nEndFrame - pPartParticle->m_nParticleFrameDisappear ) 
			{
#ifdef __SFX_OPT
				float fRand1 = RANDF;
				float fRand2 = RANDF;
				float fRand3 = RANDF;

				if( pPartParticle->m_nParticleCreate == 0 ) 
				{
					if( m_nCurFrame == nStartFrame ) 
					{
						for( int i = 0; i < pPartParticle->m_nParticleCreateNum; i++ ) 
						{
							pParticle = new Particle;
							pParticle->nFrame = 0;
							float fAngle = RANDF*360.0f;
							pParticle->vPos = 
								D3DXVECTOR3(
									sin(fAngle)*pPartParticle->m_fParticleStartPosVar,
									fRand1*pPartParticle->m_fParticleStartPosVarY,
									cos(fAngle)*pPartParticle->m_fParticleStartPosVar
							);
							float fFactor = pPartParticle->m_fParticleXZLow + fRand2*( pPartParticle->m_fParticleXZHigh - pPartParticle->m_fParticleXZLow );
							pParticle->vSpeed=
								D3DXVECTOR3(
									sin(fAngle)*fFactor,
									pPartParticle->m_fParticleYLow + fRand2 * (pPartParticle->m_fParticleYHigh - pPartParticle->m_fParticleYLow ),
									cos(fAngle)*fFactor
							);
							pParticle->vScaleStart = pParticle->vScale = pPartParticle->m_vScale;
//#ifdef __ATEST
							pParticle->vRotation = D3DXVECTOR3( pPartParticle->m_vRotationLow.x + fRand1 * 
								                                (pPartParticle->m_vRotationHigh.x-pPartParticle->m_vRotationLow.x ),
																pPartParticle->m_vRotationLow.y + fRand3 * 
																(pPartParticle->m_vRotationHigh.y-pPartParticle->m_vRotationLow.y ),
																pPartParticle->m_vRotationLow.z + fRand2 * 
																(pPartParticle->m_vRotationHigh.z-pPartParticle->m_vRotationLow.z ) );
						
							pParticle->bSwScal   = FALSE;
							pParticle->vScaleEnd = pPartParticle->m_vScaleEnd;
							pParticle->vScaleSpeed = D3DXVECTOR3( pPartParticle->m_fScalSpeedXLow + fRand3 * 
								(pPartParticle->m_fScalSpeedXHigh-pPartParticle->m_fScalSpeedXLow ),
								pPartParticle->m_fScalSpeedYLow + fRand2 * 
								(pPartParticle->m_fScalSpeedYHigh-pPartParticle->m_fScalSpeedYLow),
								pPartParticle->m_fScalSpeedZLow + fRand1 * 
								(pPartParticle->m_fScalSpeedZHigh-pPartParticle->m_fScalSpeedZLow) );
//#endif
							pParticles->Add( pParticle );
						}
					}
				}
				else 
				{
					if( ( (m_nCurFrame) - nStartFrame ) % pPartParticle->m_nParticleCreate == 0 ) 
					{
						for( int i = 0; i < pPartParticle->m_nParticleCreateNum; i++ ) 
						{
							pParticle = new Particle;
							pParticle->nFrame=0;
							float fAngle=RANDF*360.0f;
							pParticle->vPos=
								D3DXVECTOR3(
									sin(fAngle)*pPartParticle->m_fParticleStartPosVar,
									fRand1*pPartParticle->m_fParticleStartPosVarY,
									cos(fAngle)*pPartParticle->m_fParticleStartPosVar
							);
							float fFactor = pPartParticle->m_fParticleXZLow+fRand2*(pPartParticle->m_fParticleXZHigh-pPartParticle->m_fParticleXZLow);
							pParticle->vSpeed=
								D3DXVECTOR3(
									sin(fAngle)*fFactor,
									pPartParticle->m_fParticleYLow+fRand2*(pPartParticle->m_fParticleYHigh-pPartParticle->m_fParticleYLow),
									cos(fAngle)*fFactor
							);
							pParticle->vScaleStart = pParticle->vScale = pPartParticle->m_vScale;
//#ifdef __ATEST
							pParticle->vRotation = D3DXVECTOR3( pPartParticle->m_vRotationLow.x + fRand1 * 
								(pPartParticle->m_vRotationHigh.x-pPartParticle->m_vRotationLow.x ),
								pPartParticle->m_vRotationLow.y + fRand3 * 
								(pPartParticle->m_vRotationHigh.y-pPartParticle->m_vRotationLow.y ),
								pPartParticle->m_vRotationLow.z + fRand2 * 
								(pPartParticle->m_vRotationHigh.z-pPartParticle->m_vRotationLow.z ) );

							
							pParticle->bSwScal   = FALSE;
							pParticle->vScaleEnd = pPartParticle->m_vScaleEnd;
							pParticle->vScaleSpeed = D3DXVECTOR3( pPartParticle->m_fScalSpeedXLow + fRand3 * 
								(pPartParticle->m_fScalSpeedXHigh-pPartParticle->m_fScalSpeedXLow ),
								pPartParticle->m_fScalSpeedYLow + fRand2 * 
								(pPartParticle->m_fScalSpeedYHigh-pPartParticle->m_fScalSpeedYLow),
								pPartParticle->m_fScalSpeedZLow + fRand1 * 
								(pPartParticle->m_fScalSpeedZHigh-pPartParticle->m_fScalSpeedZLow) );
							
							
//#endif
							pParticles->Add(pParticle);
						}
					}
					else
						ret = FALSE;
				}
#else 
				if( pPartParticle->m_nParticleCreate == 0 ) 
				{
					if( m_nCurFrame == nStartFrame ) 
					{
						for( int i = 0; i < pPartParticle->m_nParticleCreateNum; i++ ) 
						{
							pParticle = new Particle;
							pParticle->nFrame = 0;
							float fAngle = RANDF*360.0f;
							pParticle->vPos = 
								D3DXVECTOR3(
									sin(fAngle)*pPartParticle->m_fParticleStartPosVar,
									RANDF*pPartParticle->m_fParticleStartPosVarY,
									cos(fAngle)*pPartParticle->m_fParticleStartPosVar
							);
							float fFactor = pPartParticle->m_fParticleXZLow + RANDF*( pPartParticle->m_fParticleXZHigh - pPartParticle->m_fParticleXZLow );
							pParticle->vSpeed=
								D3DXVECTOR3(
									sin(fAngle)*fFactor,
									pPartParticle->m_fParticleYLow + RANDF * (pPartParticle->m_fParticleYHigh - pPartParticle->m_fParticleYLow ),
									cos(fAngle)*fFactor
							);
							pParticle->vScaleStart = pParticle->vScale = pPartParticle->m_vScale;
//#ifdef __ATEST
							pParticle->vRotation = D3DXVECTOR3( pPartParticle->m_vRotationLow.x + RANDF * 
								                                (pPartParticle->m_vRotationHigh.x-pPartParticle->m_vRotationLow.x ),
																pPartParticle->m_vRotationLow.y + RANDF * 
																(pPartParticle->m_vRotationHigh.y-pPartParticle->m_vRotationLow.y ),
																pPartParticle->m_vRotationLow.z + RANDF * 
																(pPartParticle->m_vRotationHigh.z-pPartParticle->m_vRotationLow.z ) );
						
							pParticle->bSwScal   = FALSE;
							pParticle->vScaleEnd = pPartParticle->m_vScaleEnd;
							pParticle->vScaleSpeed = D3DXVECTOR3( pPartParticle->m_fScalSpeedXLow + RANDF * 
								(pPartParticle->m_fScalSpeedXHigh-pPartParticle->m_fScalSpeedXLow ),
								pPartParticle->m_fScalSpeedYLow + RANDF * 
								(pPartParticle->m_fScalSpeedYHigh-pPartParticle->m_fScalSpeedYLow),
								pPartParticle->m_fScalSpeedZLow + RANDF * 
								(pPartParticle->m_fScalSpeedZHigh-pPartParticle->m_fScalSpeedZLow) );
//#endif
							pParticles->Add( pParticle );
						}
					}
				}
				else 
				{
					if( ( (m_nCurFrame) - nStartFrame ) % pPartParticle->m_nParticleCreate == 0 ) 
					{
						for( int i = 0; i < pPartParticle->m_nParticleCreateNum; i++ ) 
						{
							pParticle = new Particle;
							pParticle->nFrame=0;
							float fAngle=RANDF*360.0f;
							pParticle->vPos=
								D3DXVECTOR3(
									sin(fAngle)*pPartParticle->m_fParticleStartPosVar,
									RANDF*pPartParticle->m_fParticleStartPosVarY,
									cos(fAngle)*pPartParticle->m_fParticleStartPosVar
							);
							float fFactor = pPartParticle->m_fParticleXZLow+RANDF*(pPartParticle->m_fParticleXZHigh-pPartParticle->m_fParticleXZLow);
							pParticle->vSpeed=
								D3DXVECTOR3(
									sin(fAngle)*fFactor,
									pPartParticle->m_fParticleYLow+RANDF*(pPartParticle->m_fParticleYHigh-pPartParticle->m_fParticleYLow),
									cos(fAngle)*fFactor
							);
							pParticle->vScaleStart = pParticle->vScale = pPartParticle->m_vScale;
//#ifdef __ATEST
							pParticle->vRotation = D3DXVECTOR3( pPartParticle->m_vRotationLow.x + RANDF * 
								(pPartParticle->m_vRotationHigh.x-pPartParticle->m_vRotationLow.x ),
								pPartParticle->m_vRotationLow.y + RANDF * 
								(pPartParticle->m_vRotationHigh.y-pPartParticle->m_vRotationLow.y ),
								pPartParticle->m_vRotationLow.z + RANDF * 
								(pPartParticle->m_vRotationHigh.z-pPartParticle->m_vRotationLow.z ) );

							
							pParticle->bSwScal   = FALSE;
							pParticle->vScaleEnd = pPartParticle->m_vScaleEnd;
							pParticle->vScaleSpeed = D3DXVECTOR3( pPartParticle->m_fScalSpeedXLow + RANDF * 
								(pPartParticle->m_fScalSpeedXHigh-pPartParticle->m_fScalSpeedXLow ),
								pPartParticle->m_fScalSpeedYLow + RANDF * 
								(pPartParticle->m_fScalSpeedYHigh-pPartParticle->m_fScalSpeedYLow),
								pPartParticle->m_fScalSpeedZLow + RANDF * 
								(pPartParticle->m_fScalSpeedZHigh-pPartParticle->m_fScalSpeedZLow) );
							
							
//#endif
							pParticles->Add(pParticle);
						}
					}
					else
						ret = FALSE;
				}
#endif
			}
			if( pParticles->GetSize() > 0 ) 
				ret = FALSE;
			
			if( m_nCurFrame < nStartFrame ) 
				ret = FALSE;

			if( pPartParticle->m_bRepeat )
			{
				if( nEndFrame >= 0 )
				{
					FLOAT f = (FLOAT)m_nCurFrame / (FLOAT)nEndFrame;
					
					if( f >= 0.65f )
					{
						m_nCurFrame = (int)( nEndFrame * 0.6f );
					}
				}
				else
				{
					Error( " CSfxModel::Process()에서 nEndFrame < 0" );
				}
			}
			
		}
		else 
		{
			if( m_pSfxBase->Part( i )->GetNextKey( m_nCurFrame ) ) 
				ret = FALSE;
		}
	}
	return ret;
}
BOOL CSfxModel::SetFrame( int nFrame )
{
	BOOL ret = TRUE;
//	CSfxPartParticle* pPartParticle;
//	Particle* pParticle;
	// 이 sfx에 파티클 part가 포함되어 있다면 파티클의 생성, 파괴 등을 처리한다.
	for( int i = 0; i < m_apParticles.GetSize(); i++ ) 
	{
		CPtrArray* pParticles = (CPtrArray*)m_apParticles[ i ];
		/*
		if( pParticles ) 
		{
			pPartParticle = (CSfxPartParticle*)(m_pSfxBase->Part( i ) );

			SfxKeyFrame Key;
			pPartParticle->GetKey( m_nCurFrame, &Key );

			// 파티클 이동, 생성 및 제거
			int j;
			for( j = 0; j < pParticles->GetSize(); j++ ) 
			{
				pParticle = (Particle*)(pParticles->GetAt( j ) );
				pParticle->vPos += pParticle->vSpeed;
				pParticle->vSpeed += pPartParticle->m_vParticleAccel;
				pParticle->vScale += pPartParticle->m_vScaleSpeed;
				pParticle->nFrame++;
				if(pParticle->nFrame>=pPartParticle->m_nParticleFrameDisappear) 
				{
					delete pParticle;
					pParticles->RemoveAt( j );
					j--;
				}
			}

			int nStartFrame = 0;
			int nEndFrame = 0;
			if(pPartParticle->Key(0)) 
			{
				nStartFrame = pPartParticle->Key( 0 )->nFrame;
				nEndFrame = pPartParticle->Key( pPartParticle->m_apKeyFrames.GetSize() - 1 )->nFrame;
			}
			if( m_nCurFrame >= nStartFrame && m_nCurFrame <= nEndFrame - pPartParticle->m_nParticleFrameDisappear ) 
			{
				if( pPartParticle->m_nParticleCreate == 0 ) 
				{
					if( m_nCurFrame == nStartFrame ) 
					{
						for( int i = 0; i < pPartParticle->m_nParticleCreateNum; i++ ) 
						{
							pParticle = new Particle;
							pParticle->nFrame = 0;
							float fAngle = RANDF*360.0f;
							pParticle->vPos = 
								D3DXVECTOR3(
									sin(fAngle)*pPartParticle->m_fParticleStartPosVar,
									RANDF*pPartParticle->m_fParticleStartPosVarY,
									cos(fAngle)*pPartParticle->m_fParticleStartPosVar
							);
							float fFactor = pPartParticle->m_fParticleXZLow + RANDF*( pPartParticle->m_fParticleXZHigh - pPartParticle->m_fParticleXZLow );
							pParticle->vSpeed=
								D3DXVECTOR3(
									sin(fAngle)*fFactor,
									pPartParticle->m_fParticleYLow + RANDF * (pPartParticle->m_fParticleYHigh - pPartParticle->m_fParticleYLow ),
									cos(fAngle)*fFactor
							);
							pParticle->vScale = pPartParticle->m_vScale;
							pParticles->Add( pParticle );
						}
					}
				}
				else 
				{
					if( ( m_nCurFrame - nStartFrame ) % pPartParticle->m_nParticleCreate == 0 ) 
					{
						for( int i = 0; i < pPartParticle->m_nParticleCreateNum; i++ ) 
						{
							pParticle = new Particle;
							pParticle->nFrame=0;
							float fAngle=RANDF*360.0f;
							pParticle->vPos=
								D3DXVECTOR3(
									sin(fAngle)*pPartParticle->m_fParticleStartPosVar,
									RANDF*pPartParticle->m_fParticleStartPosVarY,
									cos(fAngle)*pPartParticle->m_fParticleStartPosVar
							);
							float fFactor = pPartParticle->m_fParticleXZLow+RANDF*(pPartParticle->m_fParticleXZHigh-pPartParticle->m_fParticleXZLow);
							pParticle->vSpeed=
								D3DXVECTOR3(
									sin(fAngle)*fFactor,
									pPartParticle->m_fParticleYLow+RANDF*(pPartParticle->m_fParticleYHigh-pPartParticle->m_fParticleYLow),
									cos(fAngle)*fFactor
							);
							pParticle->vScale=pPartParticle->m_vScale;
							pParticles->Add(pParticle);
						}
					}
				}
			}
			if( pParticles->GetSize() > 0 ) ret = FALSE;
		}
		else 
		*/
		{
			if( m_pSfxBase->Part( i )->GetNextKey( nFrame ) ) 
			{
				ret = FALSE;
				m_nCurFrame = nFrame;
			}
			
		}
	}

	return ret;
}


CSfxObjMng::CSfxObjMng()
{
}
CSfxObjMng::~CSfxObjMng()
{
	RemoveAll();
}

void CSfxObjMng::AddObj(CSfxBase *pSfxBase,D3DXVECTOR3 vPos,D3DXVECTOR3 vRotate)
{
	CSfxModel* pSfxObj=new CSfxModel;
	pSfxObj->SetSfx(pSfxBase);
	pSfxObj->m_vPos=vPos; pSfxObj->m_vRotate=vRotate;
	m_apSfxObj.Add(pSfxObj);
}

void CSfxObjMng::Process(void)                                                                                                                                                                                                                                                                                                                                                                           
{
	CSfxModel* pSfxObj;
	for(int i=0;i<m_apSfxObj.GetSize();i++) 
	{
		pSfxObj=((CSfxModel*)(m_apSfxObj.GetAt(i)));
		if(pSfxObj->Process()) 
		{
			safe_delete( pSfxObj );
			m_apSfxObj.RemoveAt(i);
			i--;
		}
	}
}
#ifndef __WORLDSERVER
void CSfxObjMng::Render(void)
{
	for(int i=0;i<m_apSfxObj.GetSize();i++) {
		((CSfxModel*)(m_apSfxObj.GetAt(i)))->Render( CSfxMng::m_pd3dDevice );
	}
}
#endif
void CSfxObjMng::RemoveAll(void)
{
	for(int i=0;i<m_apSfxObj.GetSize();i++) {
		safe_delete( (CSfxModel*)m_apSfxObj.GetAt(i) );
	}
	m_apSfxObj.RemoveAll();
}

CSfxTexture::CSfxTexture()
{
}
CSfxTexture::~CSfxTexture()
{
	DeleteAll();
}

LPDIRECT3DTEXTURE9 CSfxTexture::AddTex(CString str)
{
	LPDIRECT3DTEXTURE9 pTex;
	HRESULT hr = LoadTextureFromRes( CSfxMng::m_pd3dDevice, _T( MakePath( DIR_SFXTEX, LPCTSTR(str) ) ), 
		D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_A1R5G5B5, 
		D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &pTex );
		
	if( hr != D3D_OK ) 
	{
		TRACE( "CSfxTexture::AddTex에서 텍스춰 없음 %s", str );
		return NULL;
	}
	m_apTexture[str]=pTex;
	return pTex;
}
void CSfxTexture::DeleteTex(CString str)
{
	LPDIRECT3DTEXTURE9 pTex=Tex(str);
	SAFE_RELEASE(pTex);
}
LPDIRECT3DTEXTURE9 CSfxTexture::Tex(CString str)
{
	if(str.IsEmpty()) return NULL;
	LPDIRECT3DTEXTURE9 pTex=(LPDIRECT3DTEXTURE9)(m_apTexture[str]);
	if(pTex) return pTex;
	return AddTex(str);
}

void CSfxTexture::DeleteAll(void)
{
	POSITION pos;

	CString key;
	LPDIRECT3DTEXTURE9 pTex;

	for( pos = m_apTexture.GetStartPosition(); pos != NULL; ) {
		m_apTexture.GetNextAssoc( pos, key, (void*&)pTex );
		SAFE_RELEASE(pTex);
	}
	m_apTexture.RemoveAll();
}
CSfxMeshMng::CSfxMeshMng()
{
}
CSfxMeshMng::~CSfxMeshMng()
{
	DeleteAll();
}

CModelObject* CSfxMeshMng::AddMesh(CString str)
{
	CModelObject* pMesh=new CModelObject;
	pMesh->InitDeviceObjects(m_pd3dDevice);
#ifndef __SERVER
	int nTex = g_Option.m_nTextureQuality;
	g_Option.m_nTextureQuality = 0;
#endif // not server
	pMesh->LoadModel(str);
	pMesh->RestoreDeviceObjects();
#ifndef __SERVER
	g_Option.m_nTextureQuality = nTex;
#endif // not server
	
	m_apMesh[str]=pMesh;
	return pMesh;
}
void CSfxMeshMng::DeleteMesh(CString str)
{
	CModelObject* pMesh=Mesh(str);
	if(pMesh) { safe_delete( pMesh ); }
}
CModelObject* CSfxMeshMng::Mesh(CString str)
{
	if(str.IsEmpty()) return NULL;
	CModelObject* pMesh=(CModelObject*)(m_apMesh[str]);
	if(pMesh) return pMesh;
	return AddMesh(str);
}

void CSfxMeshMng::DeleteAll(void)
{
	POSITION pos;
	CString key;
	CModelObject* pMesh;
	for( pos = m_apMesh.GetStartPosition(); pos != NULL; ) {
		m_apMesh.GetNextAssoc( pos, key, (void*&)pMesh);
		if(pMesh) safe_delete( pMesh );
	}
	m_apMesh.RemoveAll();
}

HRESULT CSfxMeshMng::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pd3dDevice=pd3dDevice;
	return S_OK;
}
HRESULT CSfxMeshMng::RestoreDeviceObjects()
{
	POSITION pos;
	CString key;
	CModelObject* pMesh;
	for( pos = m_apMesh.GetStartPosition(); pos != NULL; ) {
		m_apMesh.GetNextAssoc( pos, key, (void*&)pMesh);
		if(pMesh) pMesh->RestoreDeviceObjects();
	}
	return S_OK;
}
HRESULT CSfxMeshMng::InvalidateDeviceObjects()
{
	POSITION pos;
	CString key;
	CModelObject* pMesh;
	for( pos = m_apMesh.GetStartPosition(); pos != NULL; ) 
	{
		m_apMesh.GetNextAssoc( pos, key, (void*&)pMesh );
		if( pMesh ) pMesh->InvalidateDeviceObjects();
	}
	return S_OK;
}
HRESULT CSfxMeshMng::DeleteDeviceObjects()
{
	POSITION pos;
	CString key;
	CModelObject* pMesh;
	for( pos = m_apMesh.GetStartPosition(); pos != NULL; ) 
	{
		m_apMesh.GetNextAssoc( pos, key, (void*&)pMesh );
		if( pMesh ) pMesh->DeleteDeviceObjects();
	}
	return S_OK;
}


#ifdef __BS_EFFECT_LUA

//gmpbigsun: 외부(루아)에서 sfx를 돌리기 위함 ( 20100201 )

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SfxModelSet
SfxModelSet::SfxModelSet( const OBJID idMaster, const char* szSfxName, const char* szBoneName, BOOL bLoop, int nState )
{
	_idMaster = idMaster;

	_pModel = new CSfxModel;
	_pModel->SetSfx( szSfxName );

	// 최대 프레임을 찾는다 
	for(int i = 0; i < _pModel->m_pSfxBase->m_apParts.GetSize(); i++)
	{	
		for(int j = 0; j < _pModel->m_pSfxBase->Part( i )->m_apKeyFrames.GetSize(); j++)
		{
			// 각 파트의 프레임 수와 비교해서 최대 프레임을 찾음
			if(_nMaxFrame < _pModel->m_pSfxBase->Part( i )->Key(j)->nFrame)
				_nMaxFrame = _pModel->m_pSfxBase->Part( i )->Key(j)->nFrame;
		}
		
	}

	strcpy( _szFileName, szSfxName );
	strcpy( _szBone, szBoneName );
	_bLoop = bLoop;

	_nState = nState;
}

SfxModelSet::~SfxModelSet( )
{
	SAFE_DELETE( _pModel );
}

BOOL SfxModelSet::Update( )
{
	CMover* pMaster = prj.GetMover( _idMaster );
	if( !pMaster )
		return FALSE;

	CModelObject* pMasterModel = (CModelObject*)pMaster->GetModel( );
	if( !pMasterModel )
		return FALSE;

	D3DXVECTOR3 kPos( 0.0f, 0.0f, 0.0f );

	BOOL bOK = pMasterModel->GetPosBone( &kPos, _szBone );
	if( !bOK )
		return FALSE;

	D3DXMATRIX* pMat = &pMaster->GetMatrixWorld( );
	D3DXVec3TransformCoord( &kPos, &kPos, pMat );
	_pModel->m_vPos = kPos;
	
	//looping
	if( _nMaxFrame < _pModel->m_nCurFrame )
	{
		if( _bLoop )
			_pModel->m_nCurFrame = 0;
		else 
			return FALSE;
	}

	_pModel->Process( );

	return TRUE;
}

BOOL SfxModelSet::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( _pModel )
		_pModel->Render( pd3dDevice );

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CSfxModelMng

CSfxModelMng* CSfxModelMng::_pThis = NULL;

CSfxModelMng* CSfxModelMng::GetThis()
{
	if( !_pThis )
		_pThis = new CSfxModelMng;

	return _pThis;
}

void CSfxModelMng::Free()
{
	SAFE_DELETE( _pThis );
}

CSfxModelMng::CSfxModelMng( )
{
}

CSfxModelMng::~CSfxModelMng( )
{
	for( SfxModelSetIter iter = _cDatas.begin(); iter != _cDatas.end(); ++iter )
	{
		SMSVector& cSMS = iter->second;
		SeqDeleter< SMSVector >() ( cSMS );
	}

	_cDatas.clear();
}

BOOL CSfxModelMng::IsFull( OBJID objID, const char* szSfx, const char* szBone )
{
	// 꽉 찼는가?
	static const int MAX_PLAY = 5;

	SfxModelSetIter iter = _cDatas.find( objID );
	if( iter != _cDatas.end() )
	{
		SMSVector& cSMS = iter->second;

		// 최대 동시 가능한 이펙트 검사
		if( cSMS.size() > MAX_PLAY )
			return TRUE;

		// 이름이 같은 sfx는 일단 중복 안댐
		for( SMSVector::iterator iter2 = cSMS.begin(); iter2 != cSMS.end(); ++iter2 )
		{
			SfxModelSet* pData = *iter2;
			if( !pData->_bLoop )		//clamp 중복허용 
				continue;

			// looping인데 bone 이 같고 , sfx 가 같으면 이건 무시할 데이터로 판단.
			// 즉 같은곳에 같은 이펙트를 루핑시키자고 했다면 무시한다. ( 이미 같은 이펙트가 출력되고 있으므로 )
			if( strcmp( pData->_szBone, szBone ) == 0 )
			{
				if( strcmp( pData->_szFileName, szSfx ) == 0 )
					return TRUE;
			}
		}
	}

	return FALSE;

}

BOOL CSfxModelMng::AddData( SfxModelSet* pData, BOOL bChecked )
{
	// IsFull로 검사한우 AddData가 불려야 함, 최소한의 안젼대책 
	if( !bChecked )
		return FALSE;

	//! IsFull함수로 이미 검사가 되있다고 판단하여 재 검사는 하지 않음.

	SfxModelSetIter iter = _cDatas.find( pData->_idMaster );
	if( iter != _cDatas.end() )
	{
		//Bone name이 같은경우 looping 속성에 대해서 기존 이펙트 삭제 후 추가 
		SMSVector& rSMS = iter->second;

		for( SMSVector::iterator iter2 = rSMS.begin(); iter2 != rSMS.end();  )
		{
			SfxModelSet* pSet = *iter2;
			
			if( !pSet->_bLoop )		//clamp이면 중복허용 
			{
				++iter2;
				continue;
			}

			if( strcmp( pSet->_szBone, pData->_szBone ) == 0 )
			{
				//위치가 같다. 기존 sfx삭제 
				SAFE_DELETE( pSet );
				iter2 = rSMS.erase( iter2 );
			}
			else ++iter2;
		}

		rSMS.push_back( pData );

		return TRUE;
	}
	else
	{
		// new data
		SMSVector cSMS;
		cSMS.push_back( pData );

		pair< SfxModelSetIter, bool > rst =  _cDatas.insert( SfxModelSetContainer::value_type( pData->_idMaster, cSMS ) );

		assert( rst.second );
		return (BOOL)rst.second;
	}
	 
	 return TRUE;
}

BOOL CSfxModelMng::SubData( OBJID objID )
{
	SfxModelSetIter iter = _cDatas.find( objID );
	if( iter != _cDatas.end() )
	{
		SeqDeleter< SMSVector >() ( iter->second );

		_cDatas.erase( iter++ );
		return TRUE;
	}

	return FALSE;
}

BOOL CSfxModelMng::SubData( OBJID objID, const int nState )
{
	// 해당 상태에 해당하는 모든 sfx를 삭제한다.

	BOOL bOK = FALSE;
	SfxModelSetIter finder = _cDatas.find( objID );
	if( finder != _cDatas.end() )
	{
		SMSVector& kSubData = finder->second;

		for( SMSVector::iterator iter = kSubData.begin(); iter != kSubData.end(); /*none*/ )
		{
			SfxModelSet* pSfx = *iter;

			if( pSfx->_idMaster == objID )
			{
				if( nState == pSfx->_nState )
				{
					SAFE_DELETE( pSfx );
					iter = kSubData.erase( iter );

					bOK = TRUE;		//ok 찾아서 지웠음.
				}
				else ++iter;
			}
		}
	}

	return bOK;

}


BOOL CSfxModelMng::SubData( OBJID objID, const char* szBone )
{
	//해당 본에 링크되어 있는 모든 sfx를 삭제한다.
	BOOL bOK = FALSE;
	SfxModelSetIter finder = _cDatas.find( objID );
	if( finder != _cDatas.end() )
	{
		SMSVector& kSubData = finder->second;

		for( SMSVector::iterator iter = kSubData.begin(); iter != kSubData.end(); /*none*/ )
		{
			SfxModelSet* pSfx = *iter;

			if( pSfx->_idMaster == objID )
			{
				if( 0 == strcmp( pSfx->_szBone, szBone ) )
				{
					SAFE_DELETE( pSfx );
					iter = kSubData.erase( iter );

					bOK = TRUE;		//ok 찾아서 지웠음.
				}
				else ++iter;
			}
		}
	}

	return bOK;
}

void CSfxModelMng::Update( )
{
	if( !_cWaitingObj.empty() )
	{
		for( map<DWORD,int>::iterator iter = _cWaitingObj.begin(); iter != _cWaitingObj.end(); ++iter )
		{
			CMover* pMover = prj.GetMover( iter->first );
			if( pMover )
			{
				int nState = iter->second;
				pMover->m_pActMover->RemoveStateFlag( nState );		//빼고
				pMover->m_pActMover->AddStateFlag( nState );		//넣으면 lua가 발동
			}
		}

		_cWaitingObj.clear();
	}

	for( SfxModelSetIter iter = _cDatas.begin(); iter != _cDatas.end();  )
	{
		SMSVector& cSMS = iter->second;

		//sub loop
		for( SMSVector::iterator iter2 = cSMS.begin(); iter2 != cSMS.end();  )
		{
			SfxModelSet* pData = *iter2;
			if( !pData->Update() )
			{
				SAFE_DELETE( pData );
				iter2 = cSMS.erase( iter2 );
			}
			else 
				++iter2;
		}

		// main loop
		if( cSMS.empty() )
			_cDatas.erase( iter++ );
		else ++iter;
	}
}

void CSfxModelMng::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	for( SfxModelSetIter iter = _cDatas.begin(); iter != _cDatas.end(); ++iter )
	{
		SMSVector& cSMS = iter->second;
	//	for_each( cSMS.begin(), cSMS.end(), bind2nd( mem_fun( &SfxModelSet::Render ), pd3dDevice ) );

		for( SMSVector::iterator iter2 = cSMS.begin(); iter2 != cSMS.end(); ++iter2 )
		{
			SfxModelSet* pData = *iter2;
			pData->Render( pd3dDevice );
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
// lua for sfx

lua_State* g_SfxLua = NULL;

int call_sfx( lua_State* L )
{
	// 루아에서 call ( lua glue )
	int n = lua_gettop( L ); 

	int who = lua_tointeger( L, 1 );				// master id
	int nKind = lua_tointeger( L, 2 );				// kind of the effect
	const char* szSfx = lua_tostring( L, 3 );		// sfx
	const char* szBone = lua_tostring( L, 4 );		// bone
	BOOL bLoop = lua_tointeger( L, 5 );				// is loop ?
	int nState = lua_tointeger( L, 6 );				// state ( create, battle ... )

	CMover* pMover = prj.GetMover( who );
	if( !pMover )
	{
		assert( 0 );
		return 0;
	}

	//! 생성하기 전에 필히 검사해야함.
	BOOL bFull = CSfxModelMng::GetThis()->IsFull( who, szSfx, szBone );
	if( bFull )
		return 0;

	SfxModelSet* pSfxSet = new SfxModelSet( who, szSfx, szBone, bLoop, nState );
	BOOL bOK = CSfxModelMng::GetThis()->AddData( pSfxSet, TRUE );

	if( !bOK )
		SAFE_DELETE( pSfxSet );

	return 0;
}

int call_mte( lua_State* L )
{
	// 루아에서 call ( lua glue )
	// multitexture effect
	int n = lua_gettop( L ); 

	int who = lua_tointeger( L, 1 );				// master id
	int nKind = lua_tointeger( L, 2 );				// kind of the effect
	const char* sz2ndDDS = lua_tostring( L, 3 );	// 2nd dds

	CMover* pMover = prj.GetMover( who );
	if( !pMover )
	{
		assert( 0 );
		return 0;
	}

	if( !pMover->IsNPC( ) )
		return 0;

	CModelObject* pModel = (CModelObject*)pMover->GetModel();
	if( !pModel )
		return 0;

	CObject3D* pObj = pModel->GetObject3D( );
	if( !pObj )
		return 0;

	pObj->m_dwEffect_EX |= XE_MTE;

	char szDDS[ 64 ];
	strcpy( szDDS, sz2ndDDS );
	strcat( szDDS, ".dds" );
	BOOL bOK = pMover->SetDataMTE( pMover->GetNameO3D(), szDDS );

	if( !bOK )
		Error( "please check the mte texture %s", pMover->GetNameO3D() );

	return 0;
}

int stop_sfx_bybone( lua_State* L )
{
	//2010_05_10 gmpbigsun : 일단 만들어놨따.. 혹시나 어떤상태가 끝나면 어떤본에 링크된 sfx만 지우고싶을때를 대비해서..
	int n = lua_gettop( L );

	int who = lua_tointeger( L, 1 );
	const char* szBone = lua_tostring( L, 2 );

	CMover* pMover = prj.GetMover( who );
	if( !pMover )
	{
		assert( 0 );
		return 0;
	}

	BOOL bOK = CSfxModelMng::GetThis()->SubData( who, szBone );

	return bOK;
}

void open_lua_sfx()
{
	if( !g_SfxLua )
 	{
		g_SfxLua = lua_open();
//		luaL_openlibs(g_SfxLua);

		// push function
		lua_register( g_SfxLua, "call_sfx", call_sfx );
		lua_register( g_SfxLua, "call_mte", call_mte );
	}
}

void close_lua_sfx()
{
	if( g_SfxLua )
		lua_close( g_SfxLua );
}


void run_lua_sfx( int nState, OBJID caller, const char* szMoverName )
{
	static const CString strPATH( "SFX\\Scripts\\" );
	static const CString strFORMAT(".lua");

	// setup 
	if( !g_SfxLua )
		open_lua_sfx( );

	// caller는 무버만 지원한다. 현재 2010_0204
	CMover* pWho = prj.GetMover( caller );
	if( !pWho )
	{
		// 흠 생성이 되는도중 불릴수가 있다. 이런경우는 prj에서 못찼을껏이므로 등록상태로 두고 다음 프레임에 시도한다.
		CSfxModelMng::GetThis()->_cWaitingObj.insert( map< DWORD, int >::value_type( caller, nState ) );
		return;
	}

	char szFunc[ 64 ];
	GetFileTitle( szMoverName, szFunc );
	CString strFileName = CString( szFunc ) + strFORMAT;
	CString strFullPath = strPATH + strFileName;
	
	// res를 취급하는 테섭 및 정섭은 buffer를 얻어와서 루아를 돌리자 
#ifdef __BS_EFFECT_LUA
	if( 0 != luaL_dofile( g_SfxLua, strFullPath.GetBuffer(0) ) )
	{
	//	Error( "run_lua_sfx : %s", (char*)lua_tostring( g_SfxLua, -1 ) );
		return;
	}
#else
	// res로 merge된 파일은 CResFile을 이용해 버퍼를 얻고 루아를 호출 
	CResFile file;
	BOOL bRet = file.Open( strFullPath.GetBuffer(0), "rt" );
	if( bRet == FALSE )	
	{
		Error( "run_lua_sfx : %s 읽기 실패", strFullPath.GetBuffer(0) );
		return;
	}

	static char pBuff[ 2048 ];
	int nLength = file.GetLength();
	if( nLength > 2047 )
	{
		Error( "error!! overflowed size of lua file %s", strFullPath.GetBuffer(0) );
		return;
	}

	if( nLength < 2 )		// 존재이유가 없는 길이 이다 .
		return;

	pBuff[ 0 ] = 0;
	file.Read( pBuff, nLength );
	pBuff[ nLength ] = 0;
	
	// TODO : buffer를 얻어와서 call lua
	int a = luaL_loadbuffer( g_SfxLua, pBuff, nLength, NULL );
	int b = lua_pcall(g_SfxLua, 0, LUA_MULTRET, 0);

	if( (a | b) != 0 )
	{
		Error( "error run_lua_sfx : %s", (char*)lua_tostring( g_SfxLua, -1 ) );
		return;
	}
#endif //__BS_EFFECT_LUA

	//만약 몬스터에 대해서 일반적인 lua를 적용하고, 특별한 경우만 몬스터개개인에 대해서 lua를 지정한다면 
	// 우선순위는 특별히 지정된경우다 ,

	if( OBJSTAF_COMBAT == nState )
		strcat( szFunc, "_battle" );
	else
	if( OBJSTA_NONE == nState ) 
	{
		//생성상태로 간주한다.. 즉 생성되고 아무것도 아닌상태란 걸로 해석하기 바람.
		strcat( szFunc, "_create" );
	}
	else
	{
		Error( "SFXLUA is not supported the state : %d ", nState );
		return;
	}

	lua_getglobal( g_SfxLua, szFunc );
	lua_pushnumber( g_SfxLua, caller );

//	lua_call( g_SfxLua, 1, 0 ) ;

//_DEBUG
	if( lua_pcall( g_SfxLua, 1, 0, 0 ) != 0 ) 
	{
		char* szError = (char*)lua_tostring( g_SfxLua, -1 );
		Error( "run_lua_sfx : %s", szError );
	}
}

BOOL stop_sfx( OBJID caller )
{
	return CSfxModelMng::GetThis()->SubData( caller );
}

#endif //__BS_EFFECT_LUA