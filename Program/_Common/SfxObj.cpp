#include "stdafx.h"
#include "Sfx.h"

#ifdef __CLIENT
#include "dpclient.h"
extern	CDPClient	g_DPlay;
#endif	// __CLIENT

#include "defineObj.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSfx::CSfx()
{
	m_dwType = OT_SFX;
	m_idSrc = NULL_ID;
	m_idDest = NULL_ID;
	m_vPosDest = D3DXVECTOR3( 0, 0, 0 );
	m_nFrame = 0;
	m_nSec = -1;
#ifdef __CLIENT
	m_idSfxHit	= 0;
	m_nMagicPower = 0;
	m_dwSkill = NULL_ID;
//	m_dwSkillLevel = 0;
#endif	// __CLIENT
	m_pSfxObj = NULL;
}
CSfx::CSfx( int nIndex, const OBJID idSrc, const D3DXVECTOR3& vPosSrc, const OBJID idDest, const D3DXVECTOR3& vPosDest )
{
//	SetSfx( nIndex, idSrc, vPosSrc, idDest, vPosDest );
#ifdef __CLIENT
	m_idSfxHit	= 0;
	m_nMagicPower = 0;
#endif	// __CLIENT
	m_pSfxObj = NULL;
}
CSfx::~CSfx()
{
#ifdef __CLIENT
	if( m_idSfxHit )
	{
		if( m_idSrc == NULL_ID )	// 이런상황이 생겨선 안된다.
		{
			LPCTSTR szErr = Error( "CSfx::~CSfx : %d %d %d", m_dwIndex, m_idSfxHit, m_idSrc );
			ADDERRORMSG( szErr );
		} 
	#ifdef __CLIENT
		else
			g_DPlay.SendSfxClear( m_idSfxHit, m_idSrc );	// m_idSfxHit가 있는 거였으면 서버에 요놈 지우라고 보내자 
	#endif//__CLIENT
	}
#endif // __CLIENT
}

BOOL CSfx::SetIndex( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwIndex, BOOL bInitProp )
{
	BOOL bResult = FALSE;
	m_idSrc = NULL_ID;
	m_idDest = NULL_ID;
	m_vPosDest = D3DXVECTOR3( 0, 0, 0);

	if( dwIndex >= 0 ) 
	{
#ifndef __WORLDSERVER
		bResult = SetTypeIndex( pd3dDevice, OT_SFX, dwIndex, bInitProp );
		m_pSfxObj = (CSfxModel*)m_pModel;
#endif	// __WORLDSERVER
	}
	UpdateLocalMatrix();
	return bResult;
}

int	CSfx::SetSfx( LPDIRECT3DDEVICE9 pd3dDevice, int nIndex, 
				   D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	m_idSrc = idSrc;
	m_idDest = idDest;
	m_vPosDest = vPosDest;
	m_nSec = nSec;

	vPosSrc.y += 0.01f; // 지면에서 약간 띄워주자. 안그러면 밑에 뭍혀서 지글거림.
	SetPos( vPosSrc );
	
	if( nIndex >= 0 ) 
	{
#ifndef __WORLDSERVER
		SetTypeIndex( pd3dDevice, OT_SFX, nIndex,TRUE);
		m_pSfxObj = (CSfxModel*)m_pModel;
#endif	// __WORLDSERVER
		return 1;
	}
	UpdateLocalMatrix();

	if( m_pSfxObj )
		return 1;

	return 0;
}

//
// nDmgCnt : 일반적으론 0 : 지속데미지를 사용할경우에 0이 아닌값이 들어온다.
//
void CSfx::DamageToTarget( int nDmgCnt, float fDmgAngle, float fDmgPower, int nMaxDmgCnt )
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	CCtrl* pObjDest = prj.GetCtrl( m_idDest );
	
	if( IsInvalidObj(pObjSrc) )		return;		// 지금은 걍 리턴하지만 이렇게 실패한경우는 m_idSfxHit을 Clear해주는작업이 필요하다.
	if(	IsInvalidObj(pObjDest) )	return;

	if( pObjDest->GetType() == OT_MOVER )
	{
		CMover* pMover = (CMover*) pObjDest;
#ifdef __CLIENT
		PLAYSND( pMover->GetProp()->dwSndDmg2, &pMover->GetPos() );	// 마법류 맞을때 타격음.	
#endif 

#ifdef __CLIENT
		// 쏜놈이 플레이어이거나 / 쏜놈은 플레이어가 아닌데 맞은놈이 플레이어일경우 전송
		if( pObjSrc->IsActiveMover() || (pObjSrc->IsPlayer() == FALSE && pObjDest->IsActiveObj()) )
		{
			pMover->SetDmgCnt( 10 );	// 발사체 맞아도 이제 흔들린다,
			g_DPlay.SendSfxHit( m_idSfxHit, m_nMagicPower, m_dwSkill, pObjSrc->GetId(), nDmgCnt, fDmgAngle, fDmgPower );
			if( nMaxDmgCnt == 1 )	// 한방짜리 데미지만 id를 클리어 함.
				m_idSfxHit = 0;		// 0으로 해놔야 this가 삭제될때 SendSfxClear를 또 보내지 않는다.
		}
#endif	// __CLIENT
	}
}
void CSfx::Process()
{
#ifdef _DEBUG
	if( m_dwIndex == XI_SKILL_MAG_FIRE_HOTAIR01 )	// 디버깅 하려면 이걸 바꿔 쓰세요.
	{
		int a = 0;
	}
#endif
	m_nFrame++;
	if( m_nSec == 0 )	// 0은 1회 플레이후 종료.
	{
		if( m_pSfxObj->Process() )		// return true는 애니메이션 끝.
			Delete();
	}
	else
	{
		// 반복 애니메이션
		if( m_pSfxObj->Process() )		
			m_pSfxObj->m_nCurFrame = 0;
		if( m_nSec != -1 )		// 무한반복(-1)이 아니면
		{
			if( m_nFrame > ( m_nSec * 60 ) )	// 시간 체크를 한다.
				Delete();
		}
	}

#ifdef __CLIENT
	if(m_pSfxObj->m_pSfxBase != NULL)
	{
		for( int j=0; j<m_pSfxObj->m_pSfxBase->m_apParts.GetSize(); j++ )
		{
			if( m_pSfxObj->m_pSfxBase->Part(j)->m_nType != SFXPARTTYPE_MESH )
				continue;
			
			CModelObject* pMesh = NULL;
			
			pMesh = g_SfxMeshMng.Mesh( m_pSfxObj->m_pSfxBase->Part(j)->m_strTex );
			
			if( pMesh )
				pMesh->FrameMove();
		}
	}
#endif //__CLIENT

	if( m_idDest != NULL_ID )		// Dest가 지정되어 있을때.
	{
		CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
		if( IsValidObj( pObjDest ) )		// 유효한넘인가?
			m_vPosDest = pObjDest->GetPos();	// 당시 좌표를 계속 받아둠.  Invalid상태가 되면 마지막 좌표로 세팅된다.
		
		SetPos( m_vPosDest );	// 타겟에 오브젝트 발동.
#ifdef __CLIENT
	#if __VER >= 11 // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
		if( m_dwIndex == XI_SKILL_PSY_HERO_STONE02 )
		{
			if( IsValidObj( pObjDest ) )		// 유효한넘인가?
			{
				DWORD dwTmpID =  pObjDest->GetRemoveSfxObj(XI_SKILL_PSY_HERO_STONE02 );
				if( dwTmpID && m_nFrame > 0)
				{
					Delete();
					return;
				}
			}
		}
	#endif // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
		// 091022 mirchang - 프로텍션, 펜바리어 스킬 버프 해제 시 sfx 삭제
		if( m_dwIndex == XI_SKILL_MER_SHIELD_PANBARRIER02 )
		{
			if( IsValidObj( pObjDest ) )		// 유효한넘인가?
			{
				DWORD dwTmpID =  pObjDest->GetRemoveSfxObj( XI_SKILL_MER_SHIELD_PANBARRIER02 );
				if( dwTmpID && m_nFrame > 0)
				{
					Delete();
					return;
				}
			}
		}
		if( m_dwIndex == XI_SKILL_MER_SHIELD_PROTECTION02 )
		{
			if( IsValidObj( pObjDest ) )		// 유효한넘인가?
			{
				DWORD dwTmpID =  pObjDest->GetRemoveSfxObj( XI_SKILL_MER_SHIELD_PROTECTION02 );
				if( dwTmpID && m_nFrame > 0)
				{
					Delete();
					return;
				}
			}
		}
#endif	// __CLIENT
	} else
	{
		// Dest가 지정되어 있지 않을때. Src로...
		if( m_idSrc != NULL_ID )
		{
			CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
			if( IsValidObj( pObjSrc ) )			// 소스아이디가 지정되어 있으면
				SetPos( pObjSrc->GetPos() );	// 소스측에 이펙 발동.
			else
				Delete();
		}
	}
}
#ifndef __WORLDSERVER
// y축으로만 회전도는 버전.
void CSfx::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model이 없으면 출력하지 않음 
	if( lpModelElem == NULL )
		return;
#ifdef __3RD_LEGEND16
	CMover* pObjDest = prj.GetMover( m_idDest );
	CMover* pObjSrc = prj.GetMover( m_idSrc );

	m_pSfxObj->m_vPos = GetPos();
	switch( m_dwIndex )
	{
	case XI_SKILL_MENT_DARKNESSSCREAM03:
	case XI_SKILL_ELE_SLEEPING02:
		if( IsValidObj( pObjDest ) )
		{
			m_pSfxObj->m_vPos = pObjDest->GetPos();
			m_pSfxObj->m_vPos.y += ((pObjDest->m_pModel->GetMaxHeight()*pObjDest->GetScale().y) - (pObjDest->GetScale().y*0.7f));
		}
		else
			return;
		break;
	default: m_pSfxObj->m_vPos.y += 0.2f;	 break;
	}
	switch( m_dwIndex )
	{
	case XI_SKILL_CRACK_HWAKEYE01:
	case XI_SKILL_CRACK_HWAKEYE02:
		if( IsValidObj( pObjSrc ))
		{
			m_pSfxObj->m_vRotate.y = -pObjSrc->GetAngle();
		}
		break;
	default: m_pSfxObj->m_vRotate.y = GetAngle();
	}
#else // __3RD_LEGEND16
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vPos.y += 0.2f;		// 0.2씩이나 올릴필요가 있남?
	m_pSfxObj->m_vRotate.y = GetAngle();
#endif // __3RD_LEGEND16
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
	
}
#endif // not worldserver

