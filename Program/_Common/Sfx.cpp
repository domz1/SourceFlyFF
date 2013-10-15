#include "stdafx.h"
#include "Sfx.h"
#include "light.h"
#include "defineobj.h"
#include "defineSkill.h"
#include "defineSound.h"
#include "..\_Common\ParticleMng.h"

#include "dpclient.h"
extern	CDPClient	g_DPlay;


#include "party.h"
extern	CParty g_Party;


#ifndef __VM_0820
MemPooler<CSfxGenNormalDmg01>*	CSfxGenNormalDmg01::m_pPool	= new MemPooler<CSfxGenNormalDmg01>( 128 );
MemPooler<CSfxGenMoveMark>*	CSfxGenMoveMark::m_pPool	= new MemPooler<CSfxGenMoveMark>( 4 );
MemPooler<CSfxGenWaterCircle>*	CSfxGenWaterCircle::m_pPool	= new MemPooler<CSfxGenWaterCircle>( 64 );
MemPooler<CSfxGenRainCircle>*	CSfxGenRainCircle::m_pPool	= new MemPooler<CSfxGenRainCircle>( 64 );
#endif	// __VM_0820


CSfxGenMoveMark* g_pMoveMark=NULL;





//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static int g_nEffect = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 일반 효과들 

// 일반 공격으로 데미지 입을 때 효과 
CSfxGenNormalDmg01::CSfxGenNormalDmg01()
{
}
CSfxGenNormalDmg01::~CSfxGenNormalDmg01()
{
}
void CSfxGenNormalDmg01::Process()
{
	m_nFrame++; // 한 프레임 진행시키고
	if(m_pSfxObj->Process()) // SFX의 애니메이션을 처리해서 SFX의 재생이 끝났으면
	{
		Delete(); // 자신을 지운다
	}
	// 이것이 Process의 기본. 단순 재생 SFX는 대부분 이 형태를 취한다.
}
void CSfxGenNormalDmg01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos = GetPos();		// 2006/6/20 xuzhu
//	m_pSfxObj->m_vPos = GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}
// 일반 공격으로 데미지 입을 때 효과 
CSfxGenSuperDmg01::CSfxGenSuperDmg01()
{
}
CSfxGenSuperDmg01::~CSfxGenSuperDmg01()
{
}
void CSfxGenSuperDmg01::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
	// 역시 단순 재생 Process이다.
}
void CSfxGenSuperDmg01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos = GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 스킬로 데미지 입을 때 효과 
CSfxGenSkillDmg01::CSfxGenSkillDmg01()
{
}
CSfxGenSkillDmg01::~CSfxGenSkillDmg01()
{
}
void CSfxGenSkillDmg01::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenSkillDmg01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos = GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 몬스터가 데미지 입을 때 효과 
CSfxGenMonsterDmg01::CSfxGenMonsterDmg01()
{
}
CSfxGenMonsterDmg01::~CSfxGenMonsterDmg01()
{
}
void CSfxGenMonsterDmg01::Process()
{
	m_nFrame++; // 한프레임 진행시키고
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc ); // 붙어다닐 놈 ID로 Mover의 포인터를 갖고온다.
	if(pObjSrc) SetPos(pObjSrc->GetPos()); // 그놈이 있으면 그놈의 위치로 이동시키고
	if(m_pSfxObj->Process()) // SFX의 애니메이션을 처리해서 SFX의 재생이 끝났으면
	{
		Delete(); // 자신을 지운다.
	}
	// 이것이 특정 Mover에 붙어다니는 SFX의 Process 기본형이다.
}
void CSfxGenMonsterDmg01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos = GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 부활 효과 
CSfxGenRestoration01::CSfxGenRestoration01()
{
}
CSfxGenRestoration01::~CSfxGenRestoration01()
{
}
void CSfxGenRestoration01::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
	// 역시 m_idSrc를 따라다니는 Process이다.
}
void CSfxGenRestoration01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 치료 효과 
CSfxGenCure::CSfxGenCure()
{
}
CSfxGenCure::~CSfxGenCure()
{
}
void CSfxGenCure::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenCure::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 효력 증대 효과 
CSfxGenIncrease01::CSfxGenIncrease01()
{
}
CSfxGenIncrease01::~CSfxGenIncrease01()
{

}
void CSfxGenIncrease01::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenIncrease01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 레벨 업 효과 
CSfxGenLevelUp::CSfxGenLevelUp()
{
}
CSfxGenLevelUp::~CSfxGenLevelUp()
{
}
void CSfxGenLevelUp::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) {
		SetPos(pObjSrc->GetPos()+D3DXVECTOR3(.0f,.1f,.0f));
		SetAngle(-pObjSrc->GetAngle()); // 따라다니는 Process에 각도까지 일치시키는 부분이 추가되었다.
	}
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenLevelUp::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate=D3DXVECTOR3(.0f,GetAngle(),.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 극단스킬 이펙트 : 스트레칭 01
CSfxTroStretching01::CSfxTroStretching01()
{
}
CSfxTroStretching01::~CSfxTroStretching01()
{
}
void CSfxTroStretching01::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) {
		SetPos(pObjSrc->GetPos()+D3DXVECTOR3(.0f,.1f,.0f));
		SetAngle(-pObjSrc->GetAngle()); // 따라다니는 Process에 각도까지 일치시키는 부분이 추가되었다.
	}
	if(m_pSfxObj->Process())
	{
		Delete();
		
		if( pObjSrc )
		{
			D3DXVECTOR3 v = pObjSrc->GetPos();
			
			ItemProp* pItemProp = prj.GetPartySkill( ST_STRETCHING );
			
			CSfx *pSfx = CreateSfx( g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj2, v, pObjSrc->GetId(), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), NULL_ID, -1 );	// 머리위에 sfx생성.
		}
		
	}
}

void CSfxTroStretching01::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate=D3DXVECTOR3(.0f,GetAngle(),.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 극단스킬 이펙트 : 스트레칭 02
CSfxTroStretching02::CSfxTroStretching02()
{
}
CSfxTroStretching02::~CSfxTroStretching02()
{
}
void CSfxTroStretching02::Process()
{
	m_nFrame++;
	if( m_nSec == 0 )
	{
		if( m_pSfxObj->Process() )
			Delete();
	}
	else
	{
		if( m_pSfxObj->Process() )
			m_pSfxObj->m_nCurFrame = 0;
		if( m_nSec != -1 )
		{
			if( m_nFrame > ( m_nSec * 60 ) )
				Delete();
		}
	}

	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );

	if(pObjSrc) 
	{
		if( !pObjSrc->m_pActMover->IsSit() || g_Party.m_nModeTime[PARTY_STRETCHING_MODE] == FALSE )
		{
			Delete();
		}
		else
		{
			SetPos(pObjSrc->GetPos()+D3DXVECTOR3(.0f,.1f,.0f));
			SetAngle(-pObjSrc->GetAngle()); // 따라다니는 Process에 각도까지 일치시키는 부분이 추가되었다.
		}
	}
	else
		Delete();
}

void CSfxTroStretching02::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate=D3DXVECTOR3(.0f,GetAngle(),.0f);
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 극단스킬 이펙트 : 블릿츠~
CSfxTroBlitz::CSfxTroBlitz()
{
}
CSfxTroBlitz::~CSfxTroBlitz()
{
}
void CSfxTroBlitz::Process()
{
	m_nFrame++;
	if( m_nSec == 0 )
	{
		if( m_pSfxObj->Process() )
			Delete();
	}
	else
	{
		if( m_pSfxObj->Process() )
			m_pSfxObj->m_nCurFrame = 0;
		if( m_nSec != -1 )
		{
			if( m_nFrame > ( m_nSec * 60 ) )
				Delete();
		}
	}
}

void CSfxTroBlitz::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );

	if( IsInvalidObj(pObjDest) )	
		return;

	m_pSfxObj->m_vPos = pObjDest->GetPos();
	m_pSfxObj->m_vPos.y += ((pObjDest->m_pModel->GetMaxHeight()*pObjDest->GetScale().y) - (pObjDest->GetScale().y*0.7f));
	
	m_pSfxObj->m_vRotate=D3DXVECTOR3(.0f,GetAngle(),.0f);
	m_pSfxObj->m_vScale = pObjDest->GetScale();
	
	m_pSfxObj->Render( pd3dDevice );
}

// 로그인 효과 
CSfxGenLogin::CSfxGenLogin()
{
}
CSfxGenLogin::~CSfxGenLogin()
{
}
void CSfxGenLogin::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenLogin::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 워프 효과 
CSfxGenWarp::CSfxGenWarp()
{
}
CSfxGenWarp::~CSfxGenWarp()
{
}
void CSfxGenWarp::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenWarp::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 플레이어 죽을 때 효과 
CSfxGenPcDie::CSfxGenPcDie()
{
}
CSfxGenPcDie::~CSfxGenPcDie()
{
}
void CSfxGenPcDie::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenPcDie::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 몬스터 스폰 할 때 효과 
CSfxGenMonsterSpawn::CSfxGenMonsterSpawn()
{
}
CSfxGenMonsterSpawn::~CSfxGenMonsterSpawn()
{
}
void CSfxGenMonsterSpawn::Process()
{
	m_nFrame++;
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(pObjSrc) SetPos(pObjSrc->GetPos());
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenMonsterSpawn::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 일반 공격으로 데미지 입을 때 효과 
CSfxGenMoveMark::CSfxGenMoveMark()
{
}
CSfxGenMoveMark::~CSfxGenMoveMark()
{
}
void CSfxGenMoveMark::Process()
{
	m_nFrame++;
	g_pMoveMark=this;
	if(m_pSfxObj->Process())
	{
		g_pMoveMark=NULL;
		Delete();
	}
}
void CSfxGenMoveMark::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vPos.y += 0.05f;
	m_pSfxObj->m_vScale = GetScale();

	m_pSfxObj->Render2( pd3dDevice );
}

// 일반 공격으로 데미지 입을 때 효과 
CSfxGenWaterCircle::CSfxGenWaterCircle()
{
}
CSfxGenWaterCircle::~CSfxGenWaterCircle()
{
}
void CSfxGenWaterCircle::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenWaterCircle::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

CSfxGenRainCircle::CSfxGenRainCircle()
{
}
CSfxGenRainCircle::~CSfxGenRainCircle()
{
}
void CSfxGenRainCircle::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}

	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vScale    = GetScale();
}
void CSfxGenRainCircle::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->Render2( pd3dDevice );
}


// 일반 공격으로 데미지 입을 때 효과 
CSfxGenWaterCrown::CSfxGenWaterCrown()
{
}
CSfxGenWaterCrown::~CSfxGenWaterCrown()
{
}
void CSfxGenWaterCrown::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxGenWaterCrown::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 아이템 효과들 

// 공중전에서 발사하는 완드의 발사체.
CSfxItemWandAtkAir::CSfxItemWandAtkAir()
{
	m_SfxObj2.SetSfx( "sfx_HitWand01" ); // 맞는 순간의 SFX를 따로 로딩한다.
	m_bHit = FALSE; // 아직 안맞았다.
}
CSfxItemWandAtkAir::~CSfxItemWandAtkAir()
{
}

int CSfxItemWandAtkAir::SetSfx( LPDIRECT3DDEVICE9 pd3dDevice, int nIndex, 
								 D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	CSfx::SetSfx( pd3dDevice, nIndex, vPosSrc, idSrc, vPosDest, idDest, nSec );		// 기본초기화 먼저하고.

	CMover* pObjSrc = prj.GetMover( m_idSrc );		// 발사측.
	if( IsInvalidObj(pObjSrc) )		return 0;		// 실패.
	CMover* pObjDest = prj.GetMover( m_idDest );	// 타겟측의 포인터.
	if( IsInvalidObj(pObjDest) )	return 0;		// 실패.
	
	// !!!!!!!!!!!! SFX를 SetPos할때 복잡한 처리 하지 않도록 바꾸자.
	SetPos( pObjSrc->GetPos() + D3DXVECTOR3( 0, 1.0f, 0 ) );		// 발사측의 좌표에서 약간 위에서 발사됨.
	m_pSfxObj->m_vPos = GetPos();
	m_vPosDest = pObjDest->GetPos();		// 타겟측 포지션.

	return 1;
}

//
//
//
void CSfxItemWandAtkAir::ShootSfx( float fAngXZ, float fAngY, float fSpeed )
{
	AngleToVector( &m_vDelta, fAngXZ, fAngY, fSpeed );		// 초기 발사힘.	- 완드가 향하고 있는 방향으로 쎄게 방출.
}

	
void CSfxItemWandAtkAir::Process()
{
	FLOAT	fRadius = 0.6f;		// 타겟의 반경.
	CMover* pObjDest = prj.GetMover( m_idDest );		// 타겟측의 포인터.
	if( IsValidObj(pObjDest) )		// 만약 발사체가 날아가는 중에 타겟이 없어졌다면 마지막 있었던 좌표로 날아가서 파괴된다.
	{
		m_vPosDest = pObjDest->GetPos();
		m_vPosDest.y += 1.0f;
		if( pObjDest->IsNPC() )		// 몹의경우 타겟의 크기를 좀더 크게 하자.
			fRadius = 3.0f;
	}
	
	if( m_bHit == FALSE )	// 아직 안맞은 상태이면
	{ 
		m_pSfxObj->Process(); // SFX 애니메이션을 처리하고
		if( m_pSfxObj->m_nCurFrame == 20 ) // 현재 20프레임까지 진행했으면
		{
			m_pSfxObj->m_nCurFrame = 0; // 목표점에 맞을때까지 루프시키기 위해 다시 0프레임으로 돌려놓는다.
		}

		// 현재 위치에서 목표방향쪽으로의 벡터 구함.
		D3DXVECTOR3 vDist = m_vPosDest - GetPos();
		D3DXVECTOR3 vDistXZ = vDist;
		vDistXZ.y = 0;		// XZ평면의 순수 길이를 구하기 위해.

		FLOAT fAngXZ = D3DXToDegree( atan2( vDist.x, -vDist.z ) );		// 목표방향쪽으로의 XZ각도 구함.
		FLOAT fDistXZ = D3DXVec3Length( &vDistXZ );				// 목표방향쪽으로의 XZ평면에서의 길이.
		FLOAT fAngH  = D3DXToDegree( atan2( fDistXZ, vDist.y ) );		// 목표방향쪽으로의 높이 각도.

		fAngH -= 90.0f;
		fAngH = -fAngH;
		D3DXVECTOR3	vAcc;
		AngleToVector( &vAcc, fAngXZ, fAngH, 0.007f );		// 실시간으로 목표쪽으로 향하는 힘.
		
		FLOAT fSpeedSq = D3DXVec3LengthSq( &m_vDelta );		// 디버깅용.  현재 발사체의 속도를 산출.
		if( fSpeedSq < (0.45f * 0.45f) )
		{
			m_vDelta += vAcc;		// 기존 관성에 새로운 힘을 더함.
		}
		m_vDelta *= 0.985f;		// 마찰을 줘서 일정이상 속도가 빨라지지 않도록 하자.
		//m_vDelta *= 0.965f;		// 마찰을 줘서 일정이상 속도가 빨라지지 않도록 하자.
		D3DXVECTOR3 vPos = GetPos();
		vPos += m_vDelta;
		SetPos( vPos );		// 현재 위치 갱신.

		m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = fAngXZ;	// sfx model의 각도도 변화 시켜줌.
		
		if( D3DXVec3LengthSq( &vDist ) < (fRadius * fRadius) )  // 목표점의 거리와 xx cm이하인 경우에는...
		{			
			m_bHit = TRUE; // 맞은 걸로 판단한다.
			m_SfxObj2.m_vPos = GetPos();		// 현재 위치에서 폭발.
			DamageToTarget(); // 여기서 맞는순간처리
		}

		if( IsValidObj(pObjDest) )
		{
			if( pObjDest == CMover::GetActiveMover() )	// 날 타겟으로 잡고 날아오는 놈은 빨간색
				g_ParticleMng.CreateParticle( 0, GetPos(), D3DXVECTOR3(0,0,0), pObjDest->GetPos().y );
			else
				g_ParticleMng.CreateParticle( 1, GetPos(), D3DXVECTOR3(0,0,0), pObjDest->GetPos().y );
			
		}
		
	}
	else 
	{
		if( m_SfxObj2.Process() )		// 마지막으로 맞는 순간 출력되는 SFX의 재생이 끝나면
		{ 
			Delete(); // 자신을 지운다.
		}
	}
	m_pSfxObj->m_vPos = GetPos();
	
	m_nFrame++;
	if( m_nFrame >= 60 * 8 )	
	{
		m_bHit = TRUE;
		m_SfxObj2.m_vPos = GetPos();		// 현재 위치에서 폭발.
		if( g_pPlayer->IsActiveMover() )
			g_DPlay.SendSfxClear( m_idSfxHit );		// 이것이 삭제되었다는걸 서버에 알림.
	}
}

void CSfxItemWandAtkAir::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if( m_bHit == FALSE ) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}



CSfxMagicMiAtk1::CSfxMagicMiAtk1()
{
	m_SfxObj2.SetSfx( "sfx_HitWand01" ); // 맞는 순간의 SFX를 따로 로딩한다.
	m_bHit = FALSE; // 아직 안맞았다.
	m_fCenter = 0;
	m_fRadiusXZ = 0.2f;		// 디폴트 20센치.
}
CSfxMagicMiAtk1::~CSfxMagicMiAtk1()
{
}


void CSfxMagicMiAtk1::Process()
{
	// 이것은 목표점까지 날라가서 맞는 SFX의 일반적인 Process이다.
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( pObjDest ) 
	{
		if( pObjDest->IsDie() )
		{
			Delete();
			return;
		}

		// 발사 목표지점은 상대의 중앙.
		float fHeight = fabs(pObjDest->m_pModel->m_vMax.y) / 2.0f;	// 아이바트같은넘은 공중에 떠있기땜에 순수 높이인 Max값만 취했다.
		fHeight *= pObjDest->GetScale().x;
		m_fCenter = fHeight;	
		
		m_vPosDest = pObjDest->GetPos();		// 타겟의 위치 갱신 - 타겟이 계속 움직이므로...
		m_vPosDest.y += m_fCenter;
		if( m_nFrame == 0 )
		{
			// 맨 첫프레임에서 계산.
			FLOAT fLenX = fabs(pObjDest->m_pModel->m_vMax.x - pObjDest->m_pModel->m_vMin.x);
			FLOAT fLenZ = fabs(pObjDest->m_pModel->m_vMax.z - pObjDest->m_pModel->m_vMin.z);
			m_fRadiusXZ = ((fLenX + fLenZ) / 2.0f);		// X크기 Z크기 평균을 냄
			m_fRadiusXZ *= pObjDest->GetScale().x;
			m_fRadiusXZ /= 3.0f;	// 정확히 반지름을 쓰짐라고 반지름(/2)보다 조금 크게 쓰자 3을 나눔.
		}
	} else
	{
		Delete();
		return;
	}

		
	if( m_bHit == FALSE )		// 아직 안맞은 상태이면
	{ 
		m_pSfxObj->Process(); // SFX 애니메이션을 처리하고
		if( m_pSfxObj->m_nCurFrame == 20 ) // 현재 20프레임까지 진행했으면
		{
			m_pSfxObj->m_nCurFrame=0; // 목표점에 맞을때까지 루프시키기 위해 다시 0프레임으로 돌려놓는다.
		}

		D3DXVECTOR3 vPos = GetPos();
		D3DXVECTOR3 vDelta = m_vPosDest - vPos;		// 현재 위치에서 타겟방향으로의 벡터.
		D3DXVec3Normalize( &vDelta, &vDelta );		// 유닛벡터화 - 1미터화.
		vPos += (vDelta * 0.4f);					// 20센치로 줄여서 현재위치에 더함.
		
		SetPos( vPos );				// 현재 위치 갱신.
		m_pSfxObj->m_vPos = vPos;	// SFXModel의 위치도 갱신.

		D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = m_vPosDest - vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );

		D3DXVECTOR3 vYPW;

		QuaternionRotationToYPW( qRot, vYPW );
		m_pSfxObj->m_vRotate.x = m_SfxObj2.m_vRotate.x = D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z = m_SfxObj2.m_vRotate.z = D3DXToDegree(vYPW.z);

		vDelta = GetPos() - m_vPosDest;
		if( D3DXVec3LengthSq( &vDelta ) < m_fRadiusXZ * m_fRadiusXZ )		// 목표점의 거리와 20cm이하인 경우에는...
		{ 
			m_SfxObj2.m_vPos = GetPos();//m_vPosDest;		// 타겟의 위치에 폭발 이펙트 좌표.
			m_SfxObj2.m_vPos.y+=1.0f;
			m_bHit = TRUE; // 맞은 걸로 판단한다.
		}
	}
	else 
	{
		if( m_SfxObj2.m_nCurFrame == 0 )		// m_SfxObj2는 맞는 순간 출력되는 SFX이다.
		{ 
			DamageToTarget(); // 여기서 맞는순간처리
		}
		if( m_SfxObj2.Process() )		// 마지막으로 맞는 순간 출력되는 SFX의 재생이 끝나면
		{ 
			Delete(); // 자신을 지운다.
		}
	}
	m_nFrame++;
	if( m_nFrame >= SEC1 * 10 )		// 너무오래 쫒아가지 않게 10초지나면 소멸
		Delete();
}
void CSfxMagicMiAtk1::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if( m_bHit == FALSE ) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}



CSfxItemWandAtk1::CSfxItemWandAtk1()
{
	m_SfxObj2.SetSfx( "sfx_HitWand01" ); // 맞는 순간의 SFX를 따로 로딩한다.
	m_bHit = FALSE; // 아직 안맞았다.
	m_fCenter = 0;
	m_fRadiusXZ = 0.2f;		// 디폴트 20센치.
}
CSfxItemWandAtk1::~CSfxItemWandAtk1()
{
}


void CSfxItemWandAtk1::Process()
{
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( pObjDest ) 
	{
		if( pObjDest->IsDie() )
		{
			Delete();
			return;
		}

		// 발사 목표지점은 상대의 중앙.
		float fHeight = fabs(pObjDest->m_pModel->m_vMax.y) / 2.0f;	// 아이바트같은넘은 공중에 떠있기땜에 순수 높이인 Max값만 취했다.
		fHeight *= pObjDest->GetScale().x;
		m_fCenter = fHeight;	
		
		m_vPosDest = pObjDest->GetPos();		// 타겟의 위치 갱신 - 타겟이 계속 움직이므로...
		m_vPosDest.y += m_fCenter;
		if( m_nFrame == 0 )
		{
			// 맨 첫프레임에서 계산.
			FLOAT fLenX = fabs(pObjDest->m_pModel->m_vMax.x - pObjDest->m_pModel->m_vMin.x);
			FLOAT fLenZ = fabs(pObjDest->m_pModel->m_vMax.z - pObjDest->m_pModel->m_vMin.z);
			m_fRadiusXZ = ((fLenX + fLenZ) / 2.0f);		// X크기 Z크기 평균을 냄
			m_fRadiusXZ *= pObjDest->GetScale().x;
			m_fRadiusXZ /= 3.0f;	// 정확히 반지름을 쓰짐라고 반지름(/2)보다 조금 크게 쓰자 3을 나눔.
		}
	} else
	{
		Delete();
		return;
	}

		
	if( m_bHit == FALSE )		// 아직 안맞은 상태이면
	{ 
		m_pSfxObj->Process(); // SFX 애니메이션을 처리하고
		if( m_pSfxObj->m_nCurFrame == 20 ) // 현재 20프레임까지 진행했으면
		{
			m_pSfxObj->m_nCurFrame=0; // 목표점에 맞을때까지 루프시키기 위해 다시 0프레임으로 돌려놓는다.
		}

		D3DXVECTOR3 vPos = GetPos();
		D3DXVECTOR3 vDelta = m_vPosDest - vPos;		// 현재 위치에서 타겟방향으로의 벡터.
		D3DXVec3Normalize( &vDelta, &vDelta );		// 유닛벡터화 - 1미터화.
		vPos += (vDelta * 0.2f);					// 20센치로 줄여서 현재위치에 더함.
		
		SetPos( vPos );				// 현재 위치 갱신.
		m_pSfxObj->m_vPos = vPos;	// SFXModel의 위치도 갱신.

		D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = m_vPosDest - vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );

		D3DXVECTOR3 vYPW;

		QuaternionRotationToYPW( qRot, vYPW );
		m_pSfxObj->m_vRotate.x = m_SfxObj2.m_vRotate.x = D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z = m_SfxObj2.m_vRotate.z = D3DXToDegree(vYPW.z);

		vDelta = GetPos() - m_vPosDest;
		if( D3DXVec3LengthSq( &vDelta ) < m_fRadiusXZ * m_fRadiusXZ )		// 목표점의 거리와 20cm이하인 경우에는...
		{ 
			m_SfxObj2.m_vPos = GetPos();//m_vPosDest;		// 타겟의 위치에 폭발 이펙트 좌표.
			m_SfxObj2.m_vPos.y+=1.0f;
			m_bHit = TRUE; // 맞은 걸로 판단한다.
		}
	}
	else 
	{
		if( m_SfxObj2.m_nCurFrame == 0 )		// m_SfxObj2는 맞는 순간 출력되는 SFX이다.
		{ 
			DamageToTarget(); // 여기서 맞는순간처리
		}
		if( m_SfxObj2.Process() )		// 마지막으로 맞는 순간 출력되는 SFX의 재생이 끝나면
		{ 
			Delete(); // 자신을 지운다.
		}
	}
	m_nFrame++;
	if( m_nFrame >= SEC1 * 10 )		// 너무오래 쫒아가지 않게 10초지나면 소멸
		Delete();
}
void CSfxItemWandAtk1::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if( m_bHit == FALSE ) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}


CSfxItemRangeAtk1::CSfxItemRangeAtk1() : CSfxShoot()
{
	m_pTail			= NULL;
}
CSfxItemRangeAtk1::~CSfxItemRangeAtk1()
{
	if( m_pTail )
		g_TailEffectMng.Delete( m_pTail );
	m_pTail = NULL;
}

void CSfxItemRangeAtk1::Process()
{
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );	// 유도탄 형이기때문에 타겟은 항상 읽어봐야 한다.
	if( IsValidObj(pObjDest) )
	{
		m_vPosDest = pObjDest->GetPos() + D3DXVECTOR3( 0, 1.0f, 0 );
	} else
	{
		// 날아가다 타겟이 없어졌으면 마지막좌표로 날아가서 터지게.
		Delete();	// this는 삭제.
		int a = 0;
	}
	
	if( m_bHit == FALSE ) 
	{
		if( m_pSfxObj->Process() )		// 애니메이션은 루프.
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vPos = GetPos();
		D3DXVECTOR3 vDelta = m_vPosDest - vPos;
		D3DXVec3Normalize( &vDelta, &vDelta );		// 타겟쪽 단위벡터 구해서
		vPos += vDelta * 0.7f;			// 그 벡터로 속도를 결정해서 좌표 이동.
		m_pSfxObj->m_vPos = vPos;
		SetPos( vPos );
		
		{
			// 마법객체 방향벡터로 모델 방향 돌리기~~~
			D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
			D3DXVECTOR3 vDestNor  = m_vPosDest - m_pSfxObj->m_vPos;
			D3DXVECTOR3 vAxis;
			D3DXQUATERNION   qRot;
			
			FLOAT       fTheta;
			D3DXVec3Normalize( &vDestNor, &vDestNor );
			D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
			fTheta = D3DXVec3Dot( &vDir, &vDestNor );
			D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
			
			D3DXVECTOR3 vYPW;
			
			QuaternionRotationToYPW( qRot, vYPW );
			m_pSfxObj->m_vRotate.x = m_SfxObj2.m_vRotate.x = D3DXToDegree(vYPW.x);
			m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = D3DXToDegree(vYPW.y);
			m_pSfxObj->m_vRotate.z = m_SfxObj2.m_vRotate.z = D3DXToDegree(vYPW.z);

		}
		
		if( m_pTail )
		{
			if( m_pTail->GetType() != 2 )	// 생성되었던 꼬리고 일반보드용이 아니면 
				m_pTail->ChangeTexture( D3DDEVICE, "etc_Tail2.bmp", 2 );	// 일반보드용으로 텍스쳐 교체.
		}
		if( m_pTail == NULL )	// 아직 할당 안됐으면 할당하고.
		{
			m_pTail = (CTailEffectBelt*)g_TailEffectMng.AddEffect( g_Neuz.m_pd3dDevice, "etc_Tail2.bmp", 2, 0.35f );
		}
		
		D3DXVECTOR3	vPos1, vPos2;
		FLOAT		fAngXZ = GetAngle();
		FLOAT		fAngH  = GetAngleX();
		
		fAngXZ -= 90.0f;
		if( fAngXZ < 0 )
			fAngXZ += 360.0f;
		AngleToVector( &vPos1, fAngXZ, -fAngH, 0.05f );
		vPos1 += GetPos();
		
		fAngXZ = GetAngle();
		fAngH  = GetAngleX();
		
		fAngXZ += 90.0f;
		if( fAngXZ > 360.0f )
			fAngXZ -= 360.0f;
		AngleToVector( &vPos2, fAngXZ, -fAngH, 0.05f );
		vPos2 += GetPos();

		if( m_pTail )
			m_pTail->CreateTail( vPos1, vPos2 );
		
		vDelta = m_pSfxObj->m_vPos - m_vPosDest;		// 대상과의 거리
		if( D3DXVec3LengthSq( &vDelta ) < 0.4f * 0.4f )		// 폭발 거리까지 다가가면 충돌.
		{
			if( m_dwSndHit != NULL_ID )
				PLAYSND( m_dwSndHit, &GetPos() );		// 사운드 플레이.
			DamageToTarget();
			// 폭발 오브젝트 생성.
			if( m_dwSfxHit != NULL_ID )
			{
				CSfx *pSfx = CreateSfx( D3DDEVICE, m_dwSfxHit, GetPos(), m_idSrc, m_vPosDest, m_idDest, 0 );
				if( pSfx )
				{
					pSfx->SetAngle( m_pSfxObj->m_vRotate.y );
				}
			}
			Delete();	// this는 삭제.
		}
	}
	m_nFrame ++;

	if( m_nFrame >= SEC1 * 10 )		// 너무오래 쫒아가지 않게 10초지나면 소멸
		Delete();	
}
void CSfxItemRangeAtk1::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render2( pd3dDevice );
}

// 요요생성
D3DXVECTOR3	CSfxItemYoyoAtk::SplineSlerp( D3DXVECTOR3 *v1, D3DXVECTOR3 *v2, D3DXVECTOR3 *v3, D3DXVECTOR3 *v4, float fSlerp )
{
	D3DXVECTOR3		vResult;
	
	float	t = fSlerp;
	float	t2 = t * t;
	float	t3 = t2 * t;
	
	D3DXVECTOR3		m0, m1;
	const float alpha = 0.0f;
	
	m0 = ((1 - alpha) / 2.0f) * 
		((*v2 - *v1) + *v3 - *v2);
	m1 = ((1 - alpha) / 2.0f) *
		((*v3 - *v2) + *v4 - *v3);
	
	vResult = (((2 * t3) - (3 * t2) + 1) * *v2) +
		((t3 - (2 * t2) + t) * m0) + 
		((t3 - t2) * m1) +
		(((-2 * t3) + (3 * t2)) *
		*v3 );
	
	return vResult;
}

#define MAX_DELAY    10
CSfxItemYoyoAtk::CSfxItemYoyoAtk() : CSfxShoot()
{
	m_pTail			= NULL;
	m_nStep			= 0;
	m_nCount        = 0;
	m_nType			= 0;
	m_nDelayCount = 0;
}

CSfxItemYoyoAtk::~CSfxItemYoyoAtk()
{
	if( m_pTail )
		g_TailEffectMng.Delete( m_pTail );
	m_pTail = NULL;
}


void CSfxItemYoyoAtk::MakePath(int nType)
{
	// 길이 5미터
	m_fMaxLength = 50.0f;
	
	CMover* pObj = (CMover*)prj.GetCtrl( m_idSrc );	

	if( !IsValidObj(pObj) )
	{
		Delete();	// this는 삭제.
		return;
	}

	FLOAT fLength;
	
	fLength = D3DXVec3LengthSq( &D3DXVECTOR3(m_vPosDest - GetPos()) );

	if( fLength > 7.0f * 7.0f )  // 7미터 이상 못 날라감
		fLength = 49.0f;

	if( fLength < 3.0f * 3.0f )  // 3미터보다 작으면 2미터
		fLength = 12.0f;

	fLength /= 5.0f;

	m_nType = nType;

	///////////////////////////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3		aNewPos[7];

	m_v3SrcPos = GetPos();
	D3DXVECTOR3 vLocal;
	
	// 5미터 앞 위치구함
	AngleToVectorXZ( &vLocal, 0, fLength );
	aNewPos[0] = vLocal;
	aNewPos[0].y -= 0.2f;
	
	AngleToVectorXZ( &vLocal, 0, fLength+0.5f );
	aNewPos[1] = vLocal;
	aNewPos[1].y -= 0.2f;

	AngleToVectorXZ( &vLocal, 0, fLength+1.0f );
	aNewPos[2] = vLocal;
	aNewPos[2].y -= 0.1f;

	// 끝점
	AngleToVectorXZ( &vLocal, 0, fLength+1.2f );
	aNewPos[3] = vLocal;

	// 뒤돌아가기
	AngleToVectorXZ( &vLocal, 0, fLength+1.0f );
	aNewPos[4] = vLocal;
	aNewPos[4].y += 0.1f;
	
	AngleToVectorXZ( &vLocal, 0, fLength+0.5f );
	aNewPos[5] = vLocal;
	aNewPos[5].y += 0.2f;
	
	AngleToVectorXZ( &vLocal, 0, fLength );
	aNewPos[6] = vLocal;
	aNewPos[6].y += 0.2f;

	D3DXVECTOR3 vSlp;
	int		a, b, c, d;
	int		nMaxVertex = 7;

	m_nMaxSpline = 0;
	for( int i = 0; i < nMaxVertex-1; i ++ )
	{
		// i ~ i+1사이를 보간한다.  
		a = i - 1;		if( a < 0 )	a = 0;
		b = i;
		c = i+1;
		d = i+2;		if( d >= nMaxVertex )	d = nMaxVertex - 1;
		for( int j = 0; j < MAX_SF_SLERP+1; j ++ )
		{
			if( m_nMaxSpline >= 30 )
			{
				LPCTSTR szErr = Error( "CSfxItemYoyoAtk Spline : 범위 초과 %d", m_nMaxSpline );
				ADDERRORMSG( szErr );
				break;
			}
			vSlp = SplineSlerp( &aNewPos[a], &aNewPos[b], &aNewPos[c], &aNewPos[d], (float)j / MAX_SF_SLERP );
			m_aSpline[ m_nMaxSpline ] = vSlp;
			m_nMaxSpline ++;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 vDestNor  = m_vPosDest - GetPos();

	D3DXVECTOR3 vAxis;
	D3DXQUATERNION   qRot;
	
	FLOAT       fTheta;
	D3DXVec3Normalize( &vDestNor, &vDestNor );
	D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
	fTheta = D3DXVec3Dot( &vDir, &vDestNor );
	D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
	
	D3DXMATRIX mRot;
	D3DXMatrixRotationQuaternion( &mRot, &qRot );
	///////////////////////////////////////////////////////////////////////////////////////////////
				
	// 스플라인 보간된 점들 월드 변환
	D3DXVECTOR3 vPosPos    = m_v3SrcPos;
	D3DXMATRIX mScal;
	D3DXMatrixScaling( &mScal, 1.0f, 1.0f, 1.0f );

	for( i=0; i<m_nMaxSpline; i++ )
	{
		D3DXMATRIX mWorld = mRot * mScal * pObj->GetMatrixTrans();
		mWorld._42 = vPosPos.y;
		D3DXVec3TransformCoord( &m_aSpline[i], &m_aSpline[i], &mWorld );
	}
}

void CSfxItemYoyoAtk::Process()
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );	

	if( !IsValidObj(pObjSrc) )
	{
		Delete();	// this는 삭제.
		return;
	}
	
	if( m_bHit == FALSE ) 
	{
		if( pObjSrc )
		{
			((CModelObject*)pObjSrc->m_pModel)->SetEffect( m_nType, XE_HIDE ); 
		}

		if( m_nDelayCount )
		{
			m_nDelayCount--;
			return;
		}

		// 날아가는 단계
		if( m_nStep == 0 )
		{
			D3DXVECTOR3 vPos   = GetPos();
			D3DXVECTOR3 vDelta = m_aSpline[0] - vPos;
			D3DXVec3Normalize( &vDelta, &vDelta );	
			vPos += vDelta * 0.3f;		
			m_pSfxObj->m_vPos = vPos;
			SetPos( vPos );
			
			FLOAT fLen = D3DXVec3LengthSq( &D3DXVECTOR3(m_aSpline[0] - vPos) );
			if( fLen <= 0.05f )
			{
				m_nStep = 1;
				m_nDelayCount = 6;
			}
		}
		else
		// 회전하는 단계
		if( m_nStep == 1 )
		{
			D3DXVECTOR3 vPosPos    = m_aSpline[m_nCount];
			
			m_pSfxObj->m_vPos = vPosPos;
			SetPos( vPosPos );

			m_nCount++;

			if( m_nCount >= m_nMaxSpline )
			{
				m_nStep = 2;
			}
		}
		else
		// 되돌아 오는 단계
		if( m_nStep == 2 )
		{
			D3DXVECTOR3 v3SrcPos;
			CModelObject *pModel = (CModelObject *)pObjSrc->m_pModel;
			pModel->GetHandPos( &v3SrcPos, m_nType, pObjSrc->GetMatrixWorld() );
			
			D3DXVECTOR3 vPos   = GetPos();
			D3DXVECTOR3 vDelta = v3SrcPos - vPos;
			D3DXVec3Normalize( &vDelta, &vDelta );		// 타겟쪽 단위벡터 구해서
			vPos += vDelta * 0.4f;			// 그 벡터로 속도를 결정해서 좌표 이동.
			m_pSfxObj->m_vPos = vPos;
			SetPos( vPos );

			FLOAT fLen = D3DXVec3LengthSq( &D3DXVECTOR3(v3SrcPos - vPos) );
			if( fLen <= 0.1f )
			{
				if( m_dwSndHit != NULL_ID )
					PLAYSND( m_dwSndHit, &GetPos() );		// 사운드 플레이.

				// 폭발 오브젝트 생성.
				if( m_dwSfxHit != NULL_ID )
				{
					CSfx *pSfx = CreateSfx( D3DDEVICE, m_dwSfxHit, GetPos(), m_idSrc, m_vPosDest, m_idDest, 0 );
					if( pSfx )
					{
						pSfx->SetAngle( m_pSfxObj->m_vRotate.y );
					}
				}
				Delete();	// this는 삭제.
				return;
			}
		}

		if( m_pSfxObj->Process() )		// 애니메이션은 루프.
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		if( m_pTail == NULL )	// 아직 할당 안됐으면 할당하고.
		{
			if( m_pSfxObj->m_pSfxBase->Part(0) )
				m_pTail = (CTailEffectModel*)g_TailEffectMng.AddEffect( g_Neuz.m_pd3dDevice, m_pSfxObj->m_pSfxBase->Part(0)->m_strTex, 100, 30.0f );
		}

		if( m_pTail )
		{
			D3DXMATRIX mWorld;
			D3DXMatrixTranslation( &mWorld, GetPos().x, GetPos().y, GetPos().z );
			m_pTail->CreateTail( &mWorld );
		}
	}
	m_nFrame ++;

	if( m_nFrame >= SEC1 * 5 )		// 너무오래 쫒아가지 않게 5초지나면 소멸
		Delete();	
}

void CSfxItemYoyoAtk::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render2( pd3dDevice );
}



// 엉터리 화살 생성한다. 하나는 진짜~
CSfxItemRangeAtk_JunkBow::CSfxItemRangeAtk_JunkBow() : CSfxShoot()
{
	m_pTail[0]			 = NULL;
	m_pTail[1]			 = NULL;
	m_pTail[2]			 = NULL;
}
CSfxItemRangeAtk_JunkBow::~CSfxItemRangeAtk_JunkBow()
{
	for( int i=0; i<3; i++ )
	{
		if( m_pTail[i] )
			g_TailEffectMng.Delete( m_pTail[i] );

		m_pTail[i] = NULL;
	}
}

void CSfxItemRangeAtk_JunkBow::Process()
{
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );	// 유도탄 형이기때문에 타겟은 항상 읽어봐야 한다.
	if( IsValidObj(pObjDest) )
	{
		m_vPosDest = pObjDest->GetPos() + D3DXVECTOR3( 0, 1.0f, 0 );
	} else
	{
		// 날아가다 타겟이 없어졌으면 마지막좌표로 날아가서 터지게.
		Delete();	// this는 삭제.
		int a = 0;
	}
	
	if( m_bHit == FALSE ) 
	{
		if( m_pSfxObj->Process() )		// 애니메이션은 루프.
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vPos = GetPos();
		D3DXVECTOR3 vDelta = m_vPosDest - vPos;
		D3DXVec3Normalize( &vDelta, &vDelta );		// 타겟쪽 단위벡터 구해서
		vPos += vDelta * 0.7f;			// 그 벡터로 속도를 결정해서 좌표 이동.
		m_pSfxObj->m_vPos = vPos;
		SetPos( vPos );
		
		//if( m_bDir )
		{
			// 마법객체 방향벡터로 모델 방향 돌리기~~~
			D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
			D3DXVECTOR3 vDestNor  = m_vPosDest - m_pSfxObj->m_vPos;
			D3DXVECTOR3 vAxis;
			D3DXQUATERNION   qRot;
			
			FLOAT       fTheta;
			D3DXVec3Normalize( &vDestNor, &vDestNor );
			D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
			fTheta = D3DXVec3Dot( &vDir, &vDestNor );
			D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
			
			D3DXVECTOR3 vYPW;
			
			QuaternionRotationToYPW( qRot, vYPW );
			m_pSfxObj->m_vRotate.x = m_SfxObj2.m_vRotate.x = D3DXToDegree(vYPW.x);
			m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = D3DXToDegree(vYPW.y);
			m_pSfxObj->m_vRotate.z = m_SfxObj2.m_vRotate.z = D3DXToDegree(vYPW.z);

		}
		
		for( int i=0; i<3; i++ )
		{
			if( m_pTail[i] )
			{
				if( m_pTail[i]->GetType() != 2 )	// 생성되었던 꼬리고 일반보드용이 아니면 
					m_pTail[i]->ChangeTexture( D3DDEVICE, "etc_Tail2.bmp", 2 );	// 일반보드용으로 텍스쳐 교체.
			}
			if( m_pTail[i] == NULL )	// 아직 할당 안됐으면 할당하고.
			{
				m_pTail[i] = (CTailEffectBelt*)g_TailEffectMng.AddEffect( g_Neuz.m_pd3dDevice, "etc_Tail2.bmp", 2, 0.35f );
			}
		}
		
		D3DXVECTOR3	vPos1, vPos2;
		FLOAT		fAngXZ = GetAngle();
		FLOAT		fAngH  = GetAngleX();
		
		fAngXZ -= 90.0f;
		if( fAngXZ < 0 )
			fAngXZ += 360.0f;
		AngleToVector( &vPos1, fAngXZ, -fAngH, 0.05f );
		vPos1 += GetPos();
		
		fAngXZ = GetAngle();
		fAngH  = GetAngleX();
		
		fAngXZ += 90.0f;
		if( fAngXZ > 360.0f )
			fAngXZ -= 360.0f;
		AngleToVector( &vPos2, fAngXZ, -fAngH, 0.05f );
		vPos2 += GetPos();

		vPos1.y -= 0.5f;
		vPos2.y -= 0.5f;
		
		for( i=0; i<3; i++ )
		{
			if( m_pTail[i] )
			{
				m_pTail[i]->CreateTail( vPos1, vPos2 );
			}

			vPos1.y += 0.5f;
			vPos2.y += 0.5f;
		}
		
		vDelta = m_pSfxObj->m_vPos - m_vPosDest;		// 대상과의 거리
		if( D3DXVec3LengthSq( &vDelta ) < 0.4f * 0.4f )		// 폭발 거리까지 다가가면 충돌.
		{
			if( m_dwSndHit != NULL_ID )
				PLAYSND( m_dwSndHit, &GetPos() );		// 사운드 플레이.
			DamageToTarget();
			// 폭발 오브젝트 생성.
			if( m_dwSfxHit != NULL_ID )
			{
				CSfx *pSfx = CreateSfx( D3DDEVICE, m_dwSfxHit, GetPos(), m_idSrc, m_vPosDest, m_idDest, 0 );
				if( pSfx )
				{
					pSfx->SetAngle( m_pSfxObj->m_vRotate.y );
				}
			}
			Delete();	// this는 삭제.
		}
	}
	m_nFrame ++;

	if( m_nFrame >= SEC1 * 10 )		// 너무오래 쫒아가지 않게 10초지나면 소멸
		Delete();	
}
void CSfxItemRangeAtk_JunkBow::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();

	// 좌표 백업
	D3DXVECTOR3 vPosBackup = m_pSfxObj->m_vPos;
	// 진짜 화살
	m_pSfxObj->Render2( pd3dDevice );
	
	// 가짜 1
	m_pSfxObj->m_vPos.y += 0.5f;
	m_pSfxObj->Render2( pd3dDevice );

	// 가짜 2
	m_pSfxObj->m_vPos = vPosBackup;
	m_pSfxObj->m_vPos.y -= 0.5f;
	m_pSfxObj->Render2( pd3dDevice );


	// 좌표복구
	m_pSfxObj->m_vPos = vPosBackup;
	
}


// 아래로 떨어지는 화살 이펙트
CSfxItemRangeAtk1_Allow::CSfxItemRangeAtk1_Allow() : CSfxShoot()
{
	m_nFrame = 0;
	m_fSpeed = xRandomF( 0.7f ) + 0.2f;
}

CSfxItemRangeAtk1_Allow::~CSfxItemRangeAtk1_Allow()
{
}

void CSfxItemRangeAtk1_Allow::Process()
{
	if( m_pSfxObj->Process() )		// 애니메이션은 루프.
	{
		m_pSfxObj->m_nCurFrame = 0;
	}

	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDelta = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
	D3DXVec3Normalize( &vDelta, &vDelta );		// 타겟쪽 단위벡터 구해서

	vPos += vDelta * m_fSpeed;			// 그 벡터로 속도를 결정해서 좌표 이동.

	CWorld* pWorld = g_WorldMng.Get();
	FLOAT fHeight = pWorld->GetLandHeight_Fast( vPos.x, vPos.z );

	// 땅 높이보다 낮으면 안됨
	if( vPos.y < fHeight )
	{
		m_nFrame++;
		vPos.y = fHeight;
		
		CSfx *pSfx = CreateSfx( D3DDEVICE, XI_SKILL_RAG_BOW_ARROWRAIN01, vPos );//, m_idSrc, vPos, m_idDest, 0 );		
		Delete();
	}

	m_pSfxObj->m_vPos = vPos;
		
	SetPos( vPos );
}
void CSfxItemRangeAtk1_Allow::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render2( pd3dDevice );
}

#define MAX_ALLOW		50

// CSfxItemRangeAtk1_Allow생성 관리 - 실제 사용되어지는 부분
CSfxItemRangeAtk1_AllowRain::CSfxItemRangeAtk1_AllowRain() : CSfxShoot()
{
	m_nFrame = 0;
	m_nCount = 0;
}

CSfxItemRangeAtk1_AllowRain::~CSfxItemRangeAtk1_AllowRain()
{
}

void CSfxItemRangeAtk1_AllowRain::Process()
{
	D3DXVECTOR3 vPos = GetPos();
	
	CWorld* pWorld = g_WorldMng.Get();
	FLOAT fHeight = pWorld->GetLandHeight_Fast( vPos.x, vPos.z );
	
	// 10미터위에서 떨어뜨림
	vPos.y = fHeight + xRandomF(2.0f) + 9.0f;

	m_nFrame++;

	if( m_nCount < MAX_ALLOW )
	{
		if( m_nFrame >= SEC1 * 0.07f )
		{
			m_nFrame = 0;
			vPos.x += (xRandomF( 6.0f ) + -3.0f);
			vPos.z += (xRandomF( 6.0f ) + -3.0f);

			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_SKILL_RAG_BOW_ARROWRAIN, vPos, m_idSrc, m_vPosDest, m_idDest, 0 );

			vPos.x += (xRandomF( 6.0f ) + -3.0f);
			vPos.z += (xRandomF( 6.0f ) + -3.0f);
			
			pSfx = CreateSfx( D3DDEVICE, XI_SKILL_RAG_BOW_ARROWRAIN, vPos, m_idSrc, m_vPosDest, m_idDest, 0 );
			m_nCount++;
		}
	}
	else
		Delete();
}
void CSfxItemRangeAtk1_AllowRain::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
}

CSfxItemRangeAtk1_Stone::CSfxItemRangeAtk1_Stone() : CSfxShoot()
{
	m_nFrame = 0;
	m_fSpeed = xRandomF( 0.7f ) + 0.7f;
}

CSfxItemRangeAtk1_Stone::~CSfxItemRangeAtk1_Stone()
{
}

void CSfxItemRangeAtk1_Stone::Process()
{
	if( m_pSfxObj->Process() )		// 애니메이션은 루프.
	{
		m_pSfxObj->m_nCurFrame = 0;
	}
	
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDelta = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
	D3DXVec3Normalize( &vDelta, &vDelta );		// 타겟쪽 단위벡터 구해서
	
	vPos += vDelta * m_fSpeed;			// 그 벡터로 속도를 결정해서 좌표 이동.
	
	CWorld* pWorld = g_WorldMng.Get();
	FLOAT fHeight = pWorld->GetLandHeight_Fast( vPos.x, vPos.z );
	
	// 땅 높이보다 낮으면 안됨
	if( vPos.y < fHeight+2.5f )
	{
		m_nFrame++;
		vPos.y = fHeight;
		
		CSfx *pSfx = CreateSfx( D3DDEVICE, XI_SKILL_CIRCLE_DUST, vPos );		
		if( pSfx )
		{
			pSfx->SetScale( D3DXVECTOR3( 3.0f, 3.0f, 3.0f ) );					
		}			
		Delete();
	}
	
	m_pSfxObj->m_vPos = vPos;
	
	SetPos( vPos );
}
void CSfxItemRangeAtk1_Stone::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render2( pd3dDevice );
}

#define MAX_STONE		50

// CSfxItemRangeAtk1_Allow생성 관리 - 실제 사용되어지는 부분
CSfxItemRangeAtk1_StoneRain::CSfxItemRangeAtk1_StoneRain() : CSfxShoot()
{
	m_nFrame = 0;
	m_nCount = 0;
}

CSfxItemRangeAtk1_StoneRain::~CSfxItemRangeAtk1_StoneRain()
{
}

void CSfxItemRangeAtk1_StoneRain::Process()
{
	D3DXVECTOR3 vPos = GetPos();
	
	CWorld* pWorld = g_WorldMng.Get();
	FLOAT fHeight = pWorld->GetLandHeight_Fast( vPos.x, vPos.z );
	
	// 10미터위에서 떨어뜨림
	vPos.y = fHeight + xRandomF(2.0f) + 40.0f;
	
	m_nFrame++;
	
	if( m_nCount < MAX_STONE )
	{
		if( m_nFrame >= SEC1 * 0.2f )
		{
			m_nFrame = 0;
			vPos.x += (xRandomF( 40.0f ) + -20.0f);
			vPos.z += (xRandomF( 40.0f ) + -20.0f);
			
			CSfx *pSfx = CreateSfx( D3DDEVICE, XI_SKILL_DROP_DUST, vPos, m_idSrc, m_vPosDest, m_idDest, 0 );
			if( pSfx )
			{
				pSfx->SetScale( D3DXVECTOR3( 8.0f, 8.0f, 8.0f ) );					
			}
			m_nCount++;
		}
	}
	else
		Delete();
}
void CSfxItemRangeAtk1_StoneRain::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
}

//
// 일직선으로 뻗어나가는 형태의 장거리 공격
//
CSfxAtkStraight::CSfxAtkStraight()
{
	m_dwExplosion = NULL_ID;
}
CSfxAtkStraight::~CSfxAtkStraight()
{
}

// 발사체의 이동증가량을 선계산함. 
void CSfxAtkStraight::ShootSfx( float fAngXZ, float fAngH, float fSpeed, DWORD dwExplosion )
{
	m_pSfxObj->m_vPos = GetPos();		// sfx모델의 위치를 세팅.
	m_dwExplosion = dwExplosion;

	AngleToVector( &m_vDelta, fAngXZ, fAngH, fSpeed );		// 이동량 vDelta를 구함.

	// 마법객체 방향벡터로 모델 방향 돌리기~~~
	D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 vDestNor  = m_vDelta;//m_vPosDest - GetPos();
	D3DXVECTOR3 vAxis;
	D3DXQUATERNION   qRot;
	
	FLOAT       fTheta;
	D3DXVec3Normalize( &vDestNor, &vDestNor );
	D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
	fTheta = D3DXVec3Dot( &vDir, &vDestNor );
	D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
	
	D3DXVECTOR3 vYPW;
	
	QuaternionRotationToYPW( qRot, vYPW );
	m_pSfxObj->m_vRotate.x = D3DXToDegree(vYPW.x);
	m_pSfxObj->m_vRotate.y = D3DXToDegree(vYPW.y);
	m_pSfxObj->m_vRotate.z = D3DXToDegree(vYPW.z);
	
}
				  
void CSfxAtkStraight::Process()
{
	if( m_pSfxObj->Process() )			// SFX 애니메이션을 처리하고
		m_pSfxObj->m_nCurFrame = 0;		// 목표점에 맞을때까지 루프시키기 위해 다시 0프레임으로 돌려놓는다.
	
	m_pSfxObj->m_vPos += m_vDelta;		// 좌표 이동.
	SetPos( m_pSfxObj->m_vPos );		// this도 좌표 같이 이동.
	
#if __VER >= 10 // __Y_DRAGON_FIRE	
	#ifdef __CLIENT
		//D3DXVECTOR3 vDir = v3 - v1;
		//D3DXVec3Normalize( &vDir, &vDir );
		D3DXVECTOR3 vTemp = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 vScal = D3DXVECTOR3( 0.3f, 0.3f, 0.3f );
		D3DXVECTOR3 vModelScal = GetScale();

		vScal.x *= vModelScal.x;
		vScal.y *= vModelScal.y;
		vScal.z *= vModelScal.z;

		for( int i = 0; i < 2; i ++ )
		{
			//vTemp = vDir * (xRandomF( 0.2f ) + 0.1f );
			//vTemp *= 0.8f;
			//vTemp.y -= (xRandomF( 0.05f ) + 0.01f );
			
			//extern CPartsFireDragon	g_FireDragon;
			//g_FireDragon.Create( D3DDEVICE, m_pSfxObj->m_vPos, XI_NAT_FIRE01_ADV, vScal, vTemp );
		}
	#endif
#endif //__Y_DRAGON_FIRE

	if( IsRangeObj( m_vPosDest, 0 ) )							// 목표좌표에 다다르면
	{
		CreateSfx( D3DDEVICE, m_dwExplosion, m_vPosDest );		// 폭발 이펙트
		Delete();												// 난 삭제.
		return;
	}

	m_nFrame++;
	if( m_nFrame > 60 * 3 )
	{
		Delete();
	}
}
void CSfxAtkStraight::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model이 없으면 출력하지 않음 
	if( lpModelElem == NULL )
		return;
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render2( pd3dDevice, NULL );
	
}


CSfxItemWandAtk2::CSfxItemWandAtk2()
{
	m_SfxObj2.SetSfx("sfx_HitWand01"); // 맞는 순간의 SFX를 따로 로딩한다.
	m_bHit=FALSE;
}
CSfxItemWandAtk2::~CSfxItemWandAtk2()
{
}
void CSfxItemWandAtk2::Process()
{
	if(m_nFrame==0) {
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if(pObjDest) m_SfxObj2.m_vPos=pObjDest->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);

	if(m_bHit==FALSE) {
		m_pSfxObj->Process();
		if(m_pSfxObj->m_nCurFrame==20)
		{
			m_pSfxObj->m_nCurFrame=0;
		}
		D3DXVECTOR3 vDelta=m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos-m_SfxObj2.m_vPos;
		FLOAT fAngle;
		if(vDelta.z>0) {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f)+180;
		}
		else {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f);
		}
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=fAngle;
		if(D3DXVec3Length(&vDelta)<.2f) {
			m_bHit=TRUE;
		}
	}
	else {
		if(m_SfxObj2.m_nCurFrame==0) {
			// 여기다가 맞는순간처리
			DamageToTarget();
		}
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxItemWandAtk2::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
CSfxItemWandAtk3::CSfxItemWandAtk3()
{
	m_SfxObj2.SetSfx("sfx_HitWand01"); // 맞는 순간의 SFX를 따로 로딩한다.
	m_bHit=FALSE;
}
CSfxItemWandAtk3::~CSfxItemWandAtk3()
{
}
void CSfxItemWandAtk3::Process()
{
	if(m_nFrame==0) {
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if(pObjDest) m_SfxObj2.m_vPos=pObjDest->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);

	if(m_bHit==FALSE) {
		m_pSfxObj->Process();
		if(m_pSfxObj->m_nCurFrame==20)
		{
			m_pSfxObj->m_nCurFrame=0;
		}
		D3DXVECTOR3 vDelta=m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos-m_SfxObj2.m_vPos;
		FLOAT fAngle;
		if(vDelta.z>0) {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f)+180;
		}
		else {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f);
		}
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=fAngle;
		if(D3DXVec3Length(&vDelta)<.2f) {
			m_bHit=TRUE;
		}
	}
	else {
		if(m_SfxObj2.m_nCurFrame==0) {
			// 여기다가 맞는순간처리
			DamageToTarget();
		}
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxItemWandAtk3::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
CSfxItemWandAtk4::CSfxItemWandAtk4()
{
	m_SfxObj2.SetSfx("sfx_HitWand01"); // 맞는 순간의 SFX를 따로 로딩한다.
	m_bHit=FALSE;
}
CSfxItemWandAtk4::~CSfxItemWandAtk4()
{
}
void CSfxItemWandAtk4::Process()
{
	if(m_nFrame==0) {
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if(pObjDest) m_SfxObj2.m_vPos=pObjDest->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);

	if(m_bHit==FALSE) {
		m_pSfxObj->Process();
		if(m_pSfxObj->m_nCurFrame==20)
		{
			m_pSfxObj->m_nCurFrame=0;
		}
		D3DXVECTOR3 vDelta=m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos-m_SfxObj2.m_vPos;
		FLOAT fAngle;
		if(vDelta.z>0) {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f)+180;
		}
		else {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f);
		}
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=fAngle;
		if(D3DXVec3Length(&vDelta)<.2f) {
			m_bHit=TRUE;
		}
	}
	else {
		if(m_SfxObj2.m_nCurFrame==0) {
			// 여기다가 맞는순간처리
			DamageToTarget();
		}
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxItemWandAtk4::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 스킬 효과들

// 방랑자 기본 스킬 1
CSfxSkillVagOverCutter::CSfxSkillVagOverCutter()
{
}
CSfxSkillVagOverCutter::~CSfxSkillVagOverCutter()
{
}
void CSfxSkillVagOverCutter::Process()
{
	m_nFrame++;
	{		
		CMover* pObjDst = (CMover*)prj.GetCtrl( m_idDest );
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if( pObjDst && pObjSrc ) 
		{
			SetPos( pObjDst->GetPos() + D3DXVECTOR3( .0f, 1.0f, .0f) );
			SetAngle( 180.0f - pObjSrc->GetAngle() ); // 시전자의 방향과 일치시킨다.
		}
		if( m_pSfxObj->Process() )
			Delete();
	}
}
void CSfxSkillVagOverCutter::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}
// 방랑자 기본 스킬 2
CSfxSkillVagCleanHit::CSfxSkillVagCleanHit()
{
}
CSfxSkillVagCleanHit::~CSfxSkillVagCleanHit()
{
}
void CSfxSkillVagCleanHit::Process()
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if( IsValidObj(pObjSrc) ) 
	{
		SetPos( m_vPosDest );		// 목표지점에 항상 발생되어 있음.
		SetAngle( 180.0f - pObjSrc->GetAngle() ); // 시전자의 방향과 일치시킨다.
	}
	if( m_pSfxObj->Process() )		// 애니메이션이 끝나면 끝.
		Delete();

	m_nFrame++;
	
}
void CSfxSkillVagCleanHit::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();//+180;
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}


// 고정좌표 기준 sfx
CSfxFixed::CSfxFixed()
{
}
CSfxFixed::~CSfxFixed()
{
}
void CSfxFixed::Process()
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if( IsValidObj(pObjSrc) ) 
	{
		SetPos( m_vPosDest );		// 목표지점에 항상 발생되어 있음.
	}
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
	
	m_nFrame++;
}

void CSfxFixed::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 방랑자 기본 스킬 3
CSfxSkillVagBrandish::CSfxSkillVagBrandish()
{
}
CSfxSkillVagBrandish::~CSfxSkillVagBrandish()
{
}
void CSfxSkillVagBrandish::Process()
{
	m_nFrame++;
	{ 
		CMover* pObjDst = (CMover*)prj.GetCtrl( m_idDest );
		if( pObjDst ) 
		{
			SetPos( pObjDst->GetPos() + D3DXVECTOR3( .0f, 1.0f, .0f)  );
			SetAngle( 180.0f - pObjDst->GetAngle() );
		}
		if(m_pSfxObj->Process())
			Delete();
	}
}
void CSfxSkillVagBrandish::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 머셔너리 기본 스킬 1
CSfxSkillMerKeenWheel::CSfxSkillMerKeenWheel()
{
}
CSfxSkillMerKeenWheel::~CSfxSkillMerKeenWheel()
{
}
void CSfxSkillMerKeenWheel::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxSkillMerKeenWheel::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}
// 머셔너리 기본 스킬 2
CSfxSkillMerSplmash::CSfxSkillMerSplmash()
{
	m_SfxObj2.SetSfx(XI_SKILL_MER_ONE_SPLMASH02);
}
CSfxSkillMerSplmash::~CSfxSkillMerSplmash()
{
}
void CSfxSkillMerSplmash::Process()
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if(m_nFrame==0 && pObjSrc) 
	{
		SetPos(pObjSrc->GetPos());
		SetAngle(180.0f-pObjSrc->GetAngle());
	}
	m_pSfxObj->Process();
	if( m_nFrame > 85 ) 
	{
		if( m_SfxObj2.Process() )
		{
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxSkillMerSplmash::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_SfxObj2.m_vPos=GetPos();
	m_SfxObj2.m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_SfxObj2.m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
	if(m_nFrame>85) 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
// 머셔너리 기본 스킬 3
CSfxSkillMerBlindSide::CSfxSkillMerBlindSide()
{
}
CSfxSkillMerBlindSide::~CSfxSkillMerBlindSide()
{
}
void CSfxSkillMerBlindSide::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxSkillMerBlindSide::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 어시 너클스킬
CSfxSkillAssBurstcrack::CSfxSkillAssBurstcrack()
{
}
CSfxSkillAssBurstcrack::~CSfxSkillAssBurstcrack()
{
}
void CSfxSkillAssBurstcrack::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}

	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if( IsValidObj( pObjSrc ) )
	{
		m_pSfxObj->m_vPos = GetPos();
		m_pSfxObj->m_vPos.y += 0.1f;
		m_pSfxObj->m_vRotate.y = 180 - pObjSrc->GetAngle();
		m_pSfxObj->m_vScale    = D3DXVECTOR3( 0.6f, 0.6f, 0.6f );
	}
	else
		Delete();
}
void CSfxSkillAssBurstcrack::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->Render( pd3dDevice );
}

CSfxSkillAssTampinghole::CSfxSkillAssTampinghole()
{
}
CSfxSkillAssTampinghole::~CSfxSkillAssTampinghole()
{
}
void CSfxSkillAssTampinghole::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}

	CMover* pObjSrc  = (CMover*)prj.GetCtrl( m_idSrc );
	
	if( IsValidObj( pObjSrc ) )
	{
		m_pSfxObj->m_vPos = GetPos();
		m_pSfxObj->m_vPos.y += 1.0f;
		
		m_pSfxObj->m_vRotate.y = 180 - pObjSrc->GetAngle();
		
		D3DXMATRIX mRot;
		D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXMatrixRotationY( &mRot, D3DXToRadian( m_pSfxObj->m_vRotate.y ) );
		D3DXVec3TransformCoord( &vDir, &vDir, &mRot );
		
		D3DXVec3Normalize( &vDir, &vDir );
		
		m_pSfxObj->m_vPos += vDir*1.0f;
	}
	else
		Delete();
}
void CSfxSkillAssTampinghole::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->Render( pd3dDevice );
}

CSfxNpcDirSteam::CSfxNpcDirSteam()
{
	m_fInit = TRUE;
}
CSfxNpcDirSteam::~CSfxNpcDirSteam()
{
}
void CSfxNpcDirSteam::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}

	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );

	D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );

	if( IsInvalidObj(pObjDest) )
	{
		Delete();
		return;
	}
	if( IsValidObj( pObjSrc ) )
	{
		MoverProp* pMoverProp = prj.GetMoverProp( pObjSrc->GetIndex() );

		if( pMoverProp == NULL )
			return;

		switch( pMoverProp->dwClass )
		{
			case RANK_LOW:
			case RANK_NORMAL:
			case RANK_CAPTAIN:
			case RANK_BOSS:
				{
					m_pSfxObj->m_vRotate.y = 180 - pObjSrc->GetAngle();
					
					CModelObject *pModel = (CModelObject *)pObjSrc->m_pModel;
					D3DXVECTOR3 v;
					pModel->GetEventPos( &v, 0 );
					D3DXVec3TransformCoord( &m_pSfxObj->m_vPos, &v, &pObjSrc->GetMatrixWorld() );

					if( pMoverProp->dwClass == RANK_BOSS )
						m_pSfxObj->m_vScale = D3DXVECTOR3( 2.0f, 2.0f, 2.0f );
					else
						m_pSfxObj->m_vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
				}
				break;
		}
	}
	else
		Delete();
	
	if( m_fInit )
	{
 		vDir                  = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = pObjDest->GetPos() - m_pSfxObj->m_vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
		
		D3DXVECTOR3 vYPW;
		QuaternionRotationToYPW( qRot, vYPW );
		
		m_pSfxObj->m_vRotate.x = D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y = D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z = D3DXToDegree(vYPW.z);
		
		if( m_pSfxObj->m_vRotate.x >= 45.0f )
			m_pSfxObj->m_vRotate.x = 45.0f;
		
		if( m_pSfxObj->m_vRotate.z >= 45.0f )
			m_pSfxObj->m_vRotate.z = 45.0f;
		
		m_fInit = FALSE;
	}
}
void CSfxNpcDirSteam::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->Render2( pd3dDevice );
}


// 매지션 불 주문 외기
CSfxSkillMagFireCasting::CSfxSkillMagFireCasting()
{
}
CSfxSkillMagFireCasting::~CSfxSkillMagFireCasting()
{
}
void CSfxSkillMagFireCasting::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxSkillMagFireCasting::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if( IsValidObj( pObjSrc ) )
	{
		m_pSfxObj->m_vPos = GetPos();
		m_pSfxObj->m_vPos.y += 1.0f;
		m_pSfxObj->m_vRotate.y = 180 - pObjSrc->GetAngle();
		m_pSfxObj->Render( pd3dDevice );
	}
}

#ifdef __3RD_LEGEND16

CSfxSkillCastingFrontDynamic::CSfxSkillCastingFrontDynamic()
{
}
CSfxSkillCastingFrontDynamic::~CSfxSkillCastingFrontDynamic()
{
}
void CSfxSkillCastingFrontDynamic::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxSkillCastingFrontDynamic::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if( IsValidObj( pObjSrc ) )
	{
		m_pSfxObj->m_vPos = GetPos();
		m_pSfxObj->m_vRotate.y = 360 - pObjSrc->GetAngle();
		m_pSfxObj->Render( pd3dDevice );
	}
}

CSfxSkillCastingLSideDynamic::CSfxSkillCastingLSideDynamic()
{
}
CSfxSkillCastingLSideDynamic::~CSfxSkillCastingLSideDynamic()
{
}
void CSfxSkillCastingLSideDynamic::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxSkillCastingLSideDynamic::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
	if( IsValidObj( pObjSrc ) )
	{
		m_pSfxObj->m_vPos = GetPos();
		m_pSfxObj->m_vRotate.y = 90 - pObjSrc->GetAngle();
		m_pSfxObj->Render( pd3dDevice );
	}
}

CSfxSkillAboveTargetDynamic::CSfxSkillAboveTargetDynamic()
{
}
CSfxSkillAboveTargetDynamic::~CSfxSkillAboveTargetDynamic()
{
}
void CSfxSkillAboveTargetDynamic::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxSkillAboveTargetDynamic::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover* pObjDest = prj.GetMover( m_idDest );
	if( IsValidObj( pObjDest ) )
	{
		m_pSfxObj->m_vPos = pObjDest->GetPos();
		m_pSfxObj->m_vPos.y += pObjDest->m_pModel->m_vMax.y * pObjDest->m_pModel->m_pModelElem->m_fScale * 1.2f;//3.0f;
		//m_pSfxObj->m_vPos.y += ((pObjDest->m_pModel->GetMaxHeight()*pObjDest->GetScale().y) - (pObjDest->GetScale().y*0.7f));
		m_pSfxObj->Render( pd3dDevice );
	}
}
#endif // __3RD_LEGEND16

// 매지션 바람 주문 외기
CSfxSkillMagWindCasting::CSfxSkillMagWindCasting()
{
}
CSfxSkillMagWindCasting::~CSfxSkillMagWindCasting()
{
}
void CSfxSkillMagWindCasting::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxSkillMagWindCasting::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vPos.y += 0.2f;
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}

// 매지션 기본 스킬 1
CSfxSkillMagStrongWind::CSfxSkillMagStrongWind()
{
	m_SfxObj2.SetSfx(XI_SKILL_MAG_WIND_STRONGWIND01_01);		// 맞았을때 효과.
	m_bHit = FALSE;
	m_fAngle = 0;
}
CSfxSkillMagStrongWind::~CSfxSkillMagStrongWind()
{
}
				  
void CSfxSkillMagStrongWind::Process()
{
	if(m_nFrame==0) 
	{
		CMover* pObjSrc = prj.GetMover( m_idSrc );
		if(IsValidObj(pObjSrc)) 
			m_pSfxObj->m_vPos = pObjSrc->GetPos();
	}
	CMover* pObjDest = prj.GetMover( m_idDest );
	if(IsValidObj(pObjDest)) 
		m_SfxObj2.m_vPos = pObjDest->GetPos();

	if( m_bHit == FALSE ) 
	{
		m_pSfxObj->Process();
		if( m_pSfxObj->m_nCurFrame == 60 )
		{
			m_pSfxObj->m_nCurFrame = 30;
		}
		D3DXVECTOR3 vDelta=m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		D3DXVec3Normalize( &vDelta, &vDelta );
		m_pSfxObj->m_vPos += vDelta * .2f;
		SetPos( m_pSfxObj->m_vPos );
		vDelta = m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		FLOAT fAngle;
		fAngle = atan2( vDelta.x, -vDelta.z );
		fAngle = D3DXToDegree( fAngle );
		m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = fAngle;
		if( D3DXVec3Length(&vDelta) < 0.2f ) 
		{
			PLAYSND( SND_PC_SKILLM_STRONGWIND2, &GetPos() );     
			m_bHit = TRUE;
		} else
			m_fAngle = fAngle;	// else에다 넣은이유는 너무 가까이 붙었을때의 각도는 오차가 있을수 있기때문에..
	}
	else {
		if(m_SfxObj2.m_nCurFrame==0)
		{
			float fDmgPower = 0;
			ItemProp *pSkillProp = prj.GetSkillProp( m_dwSkill );
			if( pSkillProp && pSkillProp->dwDmgShift != NULL_ID )
				fDmgPower = (int)pSkillProp->dwDmgShift * 0.2f;		// 적당한값으로 변환해서 사용.
			int fAngle = (int)( m_fAngle );
			if( IsValidObj(pObjDest) )	// 보스몹은 밀리지 않음.
			{
				if( pObjDest->GetProp()->dwClass == RANK_SUPER || pObjDest->GetProp()->dwClass == RANK_MIDBOSS )
				{
					fAngle = 0;		fDmgPower = 0;
				}
			}
			
			DamageToTarget( 0, (float)( fAngle ), fDmgPower );
			// 타겟에 맞은순간 맞은 각도로 외부힘 0.x를 가함.
			if( IsValidObj(pObjDest) )
			{
				// 데미지플라이 중엔 
					if( (pObjDest->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) == 0 )
						AngleToVectorXZ( &pObjDest->m_pActMover->m_vDeltaE, m_fAngle, fDmgPower );	
			}
		}
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxSkillMagStrongWind::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
// 매지션 기본 스킬 2
CSfxSkillMagSwordWind::CSfxSkillMagSwordWind()
{
	m_SfxObj2.SetSfx(XI_SKILL_MAG_WIND_SWORDWIND01_01);
	m_bHit=FALSE;
}
CSfxSkillMagSwordWind::~CSfxSkillMagSwordWind()
{
}
void CSfxSkillMagSwordWind::Process()
{
	if(m_nFrame==0) 
	{
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,0.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if(pObjDest) m_SfxObj2.m_vPos=pObjDest->GetPos()+D3DXVECTOR3(.0f,0.0f,.0f);

	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Process();
		if(m_pSfxObj->m_nCurFrame>=30)
		{
			m_pSfxObj->m_nCurFrame=0;
			m_pSfxObj->Process();
		}
		D3DXVECTOR3 vDelta=m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos-m_SfxObj2.m_vPos;

		FLOAT fAngle;
		if(vDelta.z>0) {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f)+180;
		}
		else {
			fAngle=(float)(atan(vDelta.x/vDelta.z)*180/3.1415926f);
		}
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=fAngle;
		if(D3DXVec3Length(&vDelta)<.4f) 
		{
			PLAYSND( SND_PC_SKILLM_SWORDWIND2, &GetPos() );     
			m_bHit=TRUE;
		}
	}
	else 
	{
		if(m_SfxObj2.m_nCurFrame==0)
			DamageToTarget();
		
		if(m_SfxObj2.Process()) 
			Delete();
	}
	m_nFrame++;
}
void CSfxSkillMagSwordWind::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
// 매지션 기본 스킬 3
CSfxSkillMagFireBoomerang::CSfxSkillMagFireBoomerang()
{
	m_SfxObj2.SetSfx( XI_SKILL_MAG_FIRE_BOOMERANG01_01 );
	m_bHit=FALSE;
}
CSfxSkillMagFireBoomerang::~CSfxSkillMagFireBoomerang()
{
}
void CSfxSkillMagFireBoomerang::Process()
{
	if(m_nFrame==0) 
	{
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( pObjDest )
	{
		m_SfxObj2.m_vPos = pObjDest->GetPos() + D3DXVECTOR3(.0f,0.3f,.0f);
		m_SfxObj2.m_vScale = D3DXVECTOR3( 2.0f, 2.0f, 2.0f );
	}
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Process();
		if( m_pSfxObj->m_nCurFrame == 20 )
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vDelta = m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos - m_SfxObj2.m_vPos;


		D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
		
		D3DXVECTOR3 vYPW;
		
		QuaternionRotationToYPW( qRot, vYPW );
		m_pSfxObj->m_vRotate.x=m_SfxObj2.m_vRotate.x=D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z=m_SfxObj2.m_vRotate.z=D3DXToDegree(vYPW.z);

		if( D3DXVec3Length( &vDelta ) < .2f ) 
		{
			PLAYSND( SND_PC_SKILLM_FIREBOOMERANG2, &GetPos() );
			m_bHit=TRUE;
		}
	}
	else 
	{
		if(m_SfxObj2.m_nCurFrame==0)
			DamageToTarget();
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxSkillMagFireBoomerang::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}
// 매지션 기본 스킬 4
CSfxSkillMagFireBomb::CSfxSkillMagFireBomb()
{
}
CSfxSkillMagFireBomb::~CSfxSkillMagFireBomb()
{
}
void CSfxSkillMagFireBomb::Process()
{
	m_nFrame++;
	if(m_pSfxObj->Process())
	{
		Delete();
	}
}
void CSfxSkillMagFireBomb::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vPos=GetPos();
	m_pSfxObj->m_vRotate.y=GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render( pd3dDevice );
}
// 매지션 기본 스킬 5
CSfxSkillMagHotAir::CSfxSkillMagHotAir()
{
	m_SfxObj2.SetSfx(XI_SKILL_MAG_FIRE_HOTAIR01_01);		// 타겟측에서 펑 터지며 회전하는 효과
	m_nDmgCnt = 0;
}
CSfxSkillMagHotAir::~CSfxSkillMagHotAir()
{
}
void CSfxSkillMagHotAir::Process()
{
	CMover* pObjSrc = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pObjSrc ) )
	{
		Delete();
		return;
	}
	{
		m_pSfxObj->m_vPos = pObjSrc->GetPos();		// 발사측에서 타겟측으로 나타나는 효과
		m_pSfxObj->m_vRotate = D3DXVECTOR3(.0f,-pObjSrc->GetAngle(),.0f);
	}
	BOOL res1 = m_pSfxObj->Process();
	CMover* pObjDest = prj.GetMover( m_idDest );
	if( IsInvalidObj( pObjDest ) )
	{
		Delete();
		return;
	}
#ifdef __CLIENT
	#if __VER >= 11 // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
	else
	{
		DWORD dwTmpID =  pObjDest->GetRemoveSfxObj(XI_SKILL_MAG_FIRE_HOTAIR01 );
		if( dwTmpID && m_nFrame > 0)
		{
			Delete();
			return;
		}
	}
	#endif // __MA_VER11_06				// 확율스킬 효과수정 world,neuz
#endif	// __CLIENT

	m_SfxObj2.m_vPos = pObjDest->GetPos();

	ItemProp *pSkillProp = prj.GetSkillProp( m_dwSkill );
	if( pSkillProp == NULL )		// 실패하면 걍리턴.
	{
		Error( "CSfxSkillMagHotAir::Process : 스킬(%d)의 프로퍼티가 없다.", m_dwSkill );
		Delete();
		return;
	}
	AddSkillProp *pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, m_nMagicPower );	// MagicPower가 레벨 
	if( pAddSkillProp == NULL )
	{
		Error( "CSfxSkillMagHotAir::Process : 스킬(%d)의 애드 프로퍼티가 없다.", m_dwSkill );
		Delete();
		return;
	}
	
	BOOL bHitFrame = FALSE;
	int	nPainFrame = (int)((pAddSkillProp->dwPainTime / 1000.0f) * SEC1);
	if( (m_nFrame % nPainFrame) == 0 )	// nPainFrame 마다 한번씩 데미지 줌.
	{
		int	nMaxDmgCnt = (pAddSkillProp->dwSkillTime / pAddSkillProp->dwPainTime) + 1;		// 몇번 데미지를 먹냐.
		// 康	2006/11/9	// 첫번째만 전송, 이 후 지속 피해 처리
//		DamageToTarget( m_nDmgCnt++, 0, 0, nMaxDmgCnt );
		if( m_nDmgCnt++ == 0 )
			DamageToTarget( m_nDmgCnt, 0, 0, nMaxDmgCnt );
		if( m_nDmgCnt >= nMaxDmgCnt )
			m_idSfxHit = 0;
		PLAYSND( SND_PC_SKILLM_HOTAIR2, &GetPos() );
	}
	BOOL res2 = m_SfxObj2.Process();
	if( res2 )
		m_SfxObj2.m_nCurFrame = 0;
	int nFrameMax = (int)((pAddSkillProp->dwSkillTime / 1000.0f) * SEC1);	// 최대 지속시간(프레임단위).
	if( m_nFrame >= nFrameMax )
	{
		Delete();
	}
	m_nFrame++;
}
void CSfxSkillMagHotAir::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->Render( pd3dDevice );
	m_pSfxObj->m_vScale = GetScale();
	if( !g_Option.m_bSFXRenderOff )
		m_SfxObj2.Render( pd3dDevice );
}



////////////////////////////////////////////////////////////////
void CSfxDuelParty::Process()
{
	CSfx::Process();
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
	{
		Delete();
		return;
	}

	switch( m_nType )
	{
	case ENEMY_PVP_PARTY:
		{
			// process중 src무버의 극단듀얼이 끝났으면 삭제.
			if( pMover->m_nDuel != 2 || ( pMover->m_nDuel == 2 && ( pMover->m_nDuelState == 300 || pMover->m_nDuelState == 0 ) ) )		
			{
				DeleteSfx( pMover );
			}
			break;
		}
	case ENEMY_WAR:
		{
			if( pMover->m_idWar == 0 )		// process중 src무버의 전쟁이 끝났으면 해제.
			{
				DeleteSfx( pMover );
			}
			break;
		}
	case ENEMY_PK:		
		{
			if( !( g_Neuz.m_NeuzEnemy.IsPKing( pMover->GetId() ) ) )
			{
				DeleteSfx( pMover );
			}
			break;
		}
	case ENEMY_PVP:
		{
			if( pMover->m_nDuel != 1 )
			{
				DeleteSfx( pMover );
			}
			break;
		}
	}
}

void CSfxDuelParty::DeleteSfx( CMover* pEnemy )
{
	pEnemy->m_dwFlag &= (~MVRF_WARMARK);
	Delete();
}

void	CSfxDuelParty::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
		return;
			
	if( pMover->HasBuff( BUFF_SKILL, SI_ACR_SUP_DARKILLUSION ) || pMover->IsMode( TRANSPARENT_MODE ) )
		return;

	CSfx::Render(pd3dDevice);
}



////////////////////////////////////////////////////////////////
CSfxSetItem::~CSfxSetItem() 
{
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsValidObj( pMover ) )
	{
		pMover->m_dwFlag &= (~MVRF_SETITEMSFX);
	}
}

void CSfxSetItem::Process()
{
	CSfx::Process();
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
	{
		Delete();
		return;
	}

	int nAbilityOption	= pMover->GetSetItemClient();
	if( nAbilityOption < 3 )
	{
		pMover->m_dwFlag &= (~MVRF_SETITEMSFX);
		Delete();
		return;
	}
}

void	CSfxSetItem::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
		return;
	
	if( ((pMover->IsMode( TRANSPARENT_MODE ) ) == 0) )
		CSfx::Render(pd3dDevice);
}

////////////////////////////////////////////////////////////////
void CSfxCollect::Process()
{
	m_nFrame++;

	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;
	((CModelObject *)pMover->m_pModel)->GetForcePos( &vPos, 0, PARTS_RWEAPON, pMover->GetMatrixWorld() );
	
	SetPos( vPos );

	if( m_pSfxObj->Process() )
		m_pSfxObj->m_nCurFrame = 0;


	if( m_pSfxObj->m_nCurFrame >= 145 ) // 구간 반복.
		m_pSfxObj->m_nCurFrame = 30; // 목표점에 맞을때까지 루프시키기 위해 다시 0프레임으로 돌려놓는다.
	
	// 플레이어의 상태가 채집이 끝났으면 
	if( pMover->m_pActMover->GetActionState() != OBJSTA_COLLECT )
	{
		pMover->m_dwFlag &= (~MVRF_COLLECT);		// SFX해제.
		Delete();		// sfx 삭제.
	}
}

void CSfxCollect::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model이 없으면 출력하지 않음 
	if( lpModelElem == NULL )
		return;
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}


////////////////////////////////////////////////////////////////
void CSfxMushmootCharge::Process()
{
	m_nFrame++;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
	{
		Delete();
		return;
	}
	if( pMover->m_dwFlag & MVRF_HITCONT )		// 보스가 레이저를 쏘기 시작하면 삭제함.
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;
	
	((CModelObject *)pMover->m_pModel)->GetEventPos( &vPos, m_nEventPos );			// 마빡 좌표.
	D3DXVec3TransformCoord( &vPos, &vPos, pMover->GetMatrixWorldPtr() );	// 월드공간으로 변환.
	
	SetPos( vPos );
	SetAngle( -pMover->GetAngle() );
	
	if( m_pSfxObj->Process() )
		m_pSfxObj->m_nCurFrame = 0;
}

void CSfxMushmootCharge::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model이 없으면 출력하지 않음 
	if( lpModelElem == NULL )
		return;
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}

////////////////////////////////////////////////////////////////
void CSfxClockWorksCharge::Process()
{
	m_nFrame++;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
	{
		Delete();
		return;
	}
	if( pMover->m_dwFlag & MVRF_HITCONT )		// 보스가 레이저를 쏘기 시작하면 삭제함.
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;

	((CModelObject *)pMover->m_pModel)->GetEventPos( &vPos, 2 );			// 마빡 좌표.
	D3DXVec3TransformCoord( &vPos, &vPos, pMover->GetMatrixWorldPtr() );	// 월드공간으로 변환.
	
	SetPos( vPos );
	SetAngle( -pMover->GetAngle() );
	
	if( m_pSfxObj->Process() )
		m_pSfxObj->m_nCurFrame = 0;
}

void CSfxClockWorksCharge::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model이 없으면 출력하지 않음 
	if( lpModelElem == NULL )
		return;
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}

////////////////////////////////////////////////////////////////
// 클락워크 보스 왼팔에서 발사할때 나오는 이펙트.
void CSfxClockWorksCannon::Process()
{
	m_nFrame++;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
	
	pModel->GetEventPos( &vPos, 1 );			// 왼손 좌표.
	D3DXVec3TransformCoord( &vPos, &vPos, pMover->GetMatrixWorldPtr() );	// 월드공간으로 변환.
	
	D3DXMATRIX mLArm = *(pModel->GetMatrixBone( 9 ));		// 9가 L ForeArm
	D3DXMatrixMultiply( &mLArm, pMover->GetMatrixWorldPtr(), &mLArm );
	D3DXQUATERNION qLArm;
	D3DXVECTOR3 vYPW;
	
	D3DXQuaternionRotationMatrix( &qLArm, &mLArm );			// 왼팔 매트릭스를 쿼터니언으로 바꿈.
	QuaternionRotationToYPW( qLArm, vYPW );
	vYPW.y += D3DXToRadian( -90.0f );
	m_pSfxObj->m_vRotate.x = D3DXToDegree(vYPW.x);
	m_pSfxObj->m_vRotate.y = D3DXToDegree(vYPW.y);
	m_pSfxObj->m_vRotate.z = D3DXToDegree(vYPW.z);

	SetPos( vPos );
	
	if( m_pSfxObj->Process() )
		Delete();
}

void CSfxClockWorksCannon::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model이 없으면 출력하지 않음 
	if( lpModelElem == NULL )
		return;
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render2( pd3dDevice, NULL );
}


///////////////////////////////////////////////////////////////////




// 매지션 Ice Missile
CSfxSkillMagIceMissile::CSfxSkillMagIceMissile()
{
	m_SfxObj2.SetSfx( XI_SKILL_MAG_WATER_ICEMISSILE01_01 );
	m_bHit=FALSE;
}
CSfxSkillMagIceMissile::~CSfxSkillMagIceMissile()
{
}
void CSfxSkillMagIceMissile::Process()
{
	if(m_nFrame==0) 
	{
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( pObjDest )
	{
		m_SfxObj2.m_vPos = pObjDest->GetPos() + D3DXVECTOR3(.0f,0.3f,.0f);
		m_SfxObj2.m_vScale = D3DXVECTOR3( 2.0f, 2.0f, 2.0f );
	}
	if(m_bHit==FALSE) 
	{
		if( m_pSfxObj->Process() )
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vDelta = m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos - m_SfxObj2.m_vPos;
		
	    // 마법객체 방향벡터로 모델 방향 돌리기~~~
		D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
		
		D3DXVECTOR3 vYPW;
		
		QuaternionRotationToYPW( qRot, vYPW );
		m_pSfxObj->m_vRotate.x=m_SfxObj2.m_vRotate.x=D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z=m_SfxObj2.m_vRotate.z=D3DXToDegree(vYPW.z);

		if( D3DXVec3Length( &vDelta ) < .2f ) 
		{
			PLAYSND( SND_PC_SKILLM_FIREBOOMERANG2, &GetPos() );
			m_bHit=TRUE;
		}
	}
	else 
	{
		if(m_SfxObj2.m_nCurFrame==0)
			DamageToTarget();
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxSkillMagIceMissile::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}


// 매지션 Lightning Ball
CSfxSkillMagLightningBall::CSfxSkillMagLightningBall()
{
	m_SfxObj2.SetSfx( XI_SKILL_MAG_ELECTRICITY_LIGHTINGBALL01_01  );
	m_bHit=FALSE;
}
CSfxSkillMagLightningBall::~CSfxSkillMagLightningBall()
{
}
void CSfxSkillMagLightningBall::Process()
{
	if(m_nFrame==0) 
	{
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( pObjDest )
	{
		m_SfxObj2.m_vPos = pObjDest->GetPos() + D3DXVECTOR3(.0f,0.3f,.0f);
		m_SfxObj2.m_vScale = D3DXVECTOR3( 2.0f, 2.0f, 2.0f );
	}
	if(m_bHit==FALSE) 
	{
		if( m_pSfxObj->Process() )
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vDelta = m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos - m_SfxObj2.m_vPos;
		
		// 마법객체 방향벡터로 모델 방향 돌리기~~~
		D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
		
		D3DXVECTOR3 vYPW;
		
		QuaternionRotationToYPW( qRot, vYPW );
		m_pSfxObj->m_vRotate.x=m_SfxObj2.m_vRotate.x=D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z=m_SfxObj2.m_vRotate.z=D3DXToDegree(vYPW.z);
		
		if( D3DXVec3Length( &vDelta ) < .2f ) 
		{
			PLAYSND( SND_PC_SKILLM_FIREBOOMERANG2, &GetPos() );
			m_bHit=TRUE;
		}
	}
	else 
	{
		if(m_SfxObj2.m_nCurFrame==0)
			DamageToTarget();
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxSkillMagLightningBall::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}

// 매지션 Spike Stone
CSfxSkillMagSpikeStone::CSfxSkillMagSpikeStone()
{
	m_SfxObj2.SetSfx( XI_SKILL_MAG_EARTH_SPIKESTONE01_01 );
	m_bHit=FALSE;
}
CSfxSkillMagSpikeStone::~CSfxSkillMagSpikeStone()
{
}
void CSfxSkillMagSpikeStone::Process()
{
	if(m_nFrame==0) 
	{
		CMover* pObjSrc = (CMover*)prj.GetCtrl( m_idSrc );
		if(pObjSrc) m_pSfxObj->m_vPos=pObjSrc->GetPos()+D3DXVECTOR3(.0f,1.0f,.0f);
	}
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( pObjDest )
	{
		m_SfxObj2.m_vPos = pObjDest->GetPos() + D3DXVECTOR3(.0f,0.3f,.0f);
		m_SfxObj2.m_vScale = D3DXVECTOR3( 2.0f, 2.0f, 2.0f );
	}
	if(m_bHit==FALSE) 
	{
		if( m_pSfxObj->Process() )
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vDelta = m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		D3DXVec3Normalize(&vDelta,&vDelta);
		m_pSfxObj->m_vPos+=vDelta*.2f;
		SetPos(m_pSfxObj->m_vPos);
		vDelta=m_pSfxObj->m_vPos - m_SfxObj2.m_vPos;
		
		// 마법객체 방향벡터로 모델 방향 돌리기~~~
		D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
		D3DXVECTOR3 vDestNor  = m_SfxObj2.m_vPos-m_pSfxObj->m_vPos;
		D3DXVECTOR3 vAxis;
		D3DXQUATERNION   qRot;
		
		FLOAT       fTheta;
		D3DXVec3Normalize( &vDestNor, &vDestNor );
		D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
		fTheta = D3DXVec3Dot( &vDir, &vDestNor );
		D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
		
		D3DXVECTOR3 vYPW;
		
		QuaternionRotationToYPW( qRot, vYPW );
		m_pSfxObj->m_vRotate.x=m_SfxObj2.m_vRotate.x=D3DXToDegree(vYPW.x);
		m_pSfxObj->m_vRotate.y=m_SfxObj2.m_vRotate.y=D3DXToDegree(vYPW.y);
		m_pSfxObj->m_vRotate.z=m_SfxObj2.m_vRotate.z=D3DXToDegree(vYPW.z);
		
		if( D3DXVec3Length( &vDelta ) < .2f ) 
		{
			PLAYSND( SND_PC_SKILLM_FIREBOOMERANG2, &GetPos() );
			m_bHit=TRUE;
		}
	}
	else 
	{
		if(m_SfxObj2.m_nCurFrame==0)
			DamageToTarget();
		if(m_SfxObj2.Process()) {
			Delete();
		}
	}
	m_nFrame++;
}
void CSfxSkillMagSpikeStone::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	if(m_bHit==FALSE) 
	{
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}

////////////////////////////////////////////////////////////////
void CSfxRotate::Process()
{
	m_nFrame++;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
	pModel->GetEventPos( &vPos, 0 );	// 등드리 좌표
	
	D3DXVec3TransformCoord( &vPos, &vPos, pMover->GetMatrixWorldPtr() );

	SetPos( vPos );
	SetAngle( -pMover->GetAngle() );
	
	if( m_pSfxObj->Process() )
		m_pSfxObj->m_nCurFrame = 0;
	
	if( m_nSec != -1 )		// 무한반복(-1)이 아니면
	{
		if( m_nFrame > ( m_nSec * 60 ) )
			Delete();
	}
}

void CSfxRotate::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model이 없으면 출력하지 않음 
	if( lpModelElem == NULL )
		return;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )
		return;

	D3DXVECTOR3 vPos;
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
	pModel->GetEventPos( &vPos, 0 );	// 등드리 좌표
	
	D3DXVec3TransformCoord( &vPos, &vPos, pMover->GetMatrixWorldPtr() );
	
	SetPos( vPos );
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}


// 날아가서 맞고 터지는 일반적인 이펙트.
CSfxShoot::CSfxShoot()
{
	m_bHit = FALSE;
	m_dwSndHit = NULL_ID;
	m_bDir = FALSE;
	m_fHitScale = 1.0f;
	m_dwSfxHit = NULL_ID;
}
CSfxShoot::~CSfxShoot()
{
	m_bHit = FALSE;
	m_dwSndHit = NULL_ID;
	m_bDir = FALSE;
	m_fHitScale = 1.0f;
	m_dwSfxHit = NULL_ID;
}

// 맞았을때 이펙트 등록
void CSfxShoot::SetHitSfx( LPCTSTR szFileName, OBJID idSrc, OBJID idDst, FLOAT fScale ) 
{ 
}
void CSfxShoot::SetHitSfx( DWORD dwIndex, OBJID idSrc, OBJID idDst, FLOAT fScale ) 
{ 
}


// 방향성 있는 sfx 인가.
void CSfxShoot::SetDir( BOOL bDir )
{
	m_bDir = bDir;
}



void CSfxShoot::Process()
{
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );	// 유도탄 형이기때문에 타겟은 항상 읽어봐야 한다.
	if( IsValidObj(pObjDest) )
	{
		m_vPosDest = pObjDest->GetPos() + D3DXVECTOR3( 0, 1.0f, 0 );
	}

	if( m_bHit == FALSE ) 
	{
		if( m_pSfxObj->Process() )		// 애니메이션은 루프.
		{
			m_pSfxObj->m_nCurFrame = 0;
		}
		D3DXVECTOR3 vDelta = m_vPosDest - m_pSfxObj->m_vPos;
		D3DXVec3Normalize( &vDelta, &vDelta );		// 타겟쪽 단위벡터 구해서
		D3DXVECTOR3 vPos = GetPos();
		vPos += vDelta * 0.2f;			// 그 벡터로 속도를 결정해서 좌표 이동.
		m_pSfxObj->m_vPos = vPos;
		SetPos( vPos );
		
		if( m_bDir )
		{
			// 마법객체 방향벡터로 모델 방향 돌리기~~~
			D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
			D3DXVECTOR3 vDestNor  = m_vPosDest - m_pSfxObj->m_vPos;
			D3DXVECTOR3 vAxis;
			D3DXQUATERNION   qRot;
			
			FLOAT       fTheta;
			D3DXVec3Normalize( &vDestNor, &vDestNor );
			D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
			fTheta = D3DXVec3Dot( &vDir, &vDestNor );
			D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
			
			D3DXVECTOR3 vYPW;
			
			QuaternionRotationToYPW( qRot, vYPW );
			m_pSfxObj->m_vRotate.x = /*m_SfxObj2.m_vRotate.x = */D3DXToDegree(vYPW.x);
			m_pSfxObj->m_vRotate.y = /*m_SfxObj2.m_vRotate.y = */D3DXToDegree(vYPW.y);
			m_pSfxObj->m_vRotate.z = /*m_SfxObj2.m_vRotate.z = */D3DXToDegree(vYPW.z);
		}
		
		vDelta = m_pSfxObj->m_vPos - m_vPosDest;		// 대상과의 거리
		if( D3DXVec3LengthSq( &vDelta ) < 0.2f * 0.2f )		// 폭발 거리까지 다가가면 충돌.
		{
			if( m_dwSndHit != NULL_ID )
				PLAYSND( m_dwSndHit, &GetPos() );		// 사운드 플레이.
			DamageToTarget();
			// 폭발 오브젝트 생성.
			if( m_dwSfxHit != NULL_ID )
			{
				CSfx *pSfx = CreateSfx( D3DDEVICE, m_dwSfxHit, GetPos(), m_idSrc, m_vPosDest, m_idDest, 0 );
				if( pSfx )
				{
					pSfx->SetAngle( m_pSfxObj->m_vRotate.y );
				}
			}
			Delete();	// this는 삭제.
		}
	}
	m_nFrame ++;
}
void CSfxShoot::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->Render2( pd3dDevice );
}


// 땅바닥에 붙어서 날아가는 이펙트.
CSfxShootWave::CSfxShootWave()
{
	m_bHit = FALSE;
	m_dwSndHit = NULL_ID;
	m_bDir = FALSE;
	m_fHitScale = 1.0f;
	memset( m_vTail, 0, sizeof(m_vTail) );
	memset( m_nTailFrame, 0, sizeof(m_nTailFrame) );
}
CSfxShootWave::~CSfxShootWave()
{
	m_bHit = FALSE;
	m_dwSndHit = NULL_ID;
	m_bDir = FALSE;
	m_fHitScale = 1.0f;
	memset( m_vTail, 0, sizeof(m_vTail) );
	memset( m_nTailFrame, 0, sizeof(m_nTailFrame) );
}

// 맞았을때 이펙트 등록
void CSfxShootWave::SetHitSfx( LPCTSTR szFileName, OBJID idSrc, OBJID idDst, FLOAT fScale ) 
{ 
	m_SfxObj2.SetSfx( szFileName );		// 맞고 터질때 이펙트 로딩.

	m_fHitScale = fScale;
}
void CSfxShootWave::SetHitSfx( DWORD dwIndex, OBJID idSrc, OBJID idDst, FLOAT fScale ) 
{ 
	m_SfxObj2.SetSfx( dwIndex );		// 맞고 터질때 이펙트 로딩.
	
	m_fHitScale = fScale;
}

// 방향성 있는 sfx 인가.
void CSfxShootWave::SetDir( BOOL bDir )
{
	m_bDir = bDir;
}



void CSfxShootWave::Process()
{
	CMover* pObjDest = (CMover*)prj.GetCtrl( m_idDest );
	if( IsValidObj(pObjDest) )
	{
		m_SfxObj2.m_vPos = pObjDest->GetPos();
		m_SfxObj2.m_vScale = D3DXVECTOR3( m_fHitScale, m_fHitScale, m_fHitScale );			// 크기는 fHitScale배
	} else
	{
		// 날아가다 타겟이 없어졌으면 마지막좌표로 날아가서 터지게.
	}

	if( (m_nFrame & 3) == 0 )
	{
		for( int i = MAX_SHOOTWAVE_TAIL-1; i > 0; i -- )
		{
			m_vTail[i] = m_vTail[i-1];	// 뒤로 한칸씩 민다.
			m_nTailFrame[i] = m_nTailFrame[i-1];
			m_nTailFrame[i]++;
			if( m_nTailFrame[i] >= 56 )
			{
				m_vTail[i].x = m_vTail[i].y = m_vTail[i].z = 0;
				m_nTailFrame[i] = 0;
			}
		}
		m_vTail[0] = GetPos();
		m_nTailFrame[0] = 0;
	}

	if( m_pSfxObj->Process() )		// 애니메이션은 루프.
	{
		m_pSfxObj->m_nCurFrame = 0;
	}
	
	if( m_bHit == FALSE ) 
	{
		D3DXVECTOR3 vDelta = m_SfxObj2.m_vPos - m_pSfxObj->m_vPos;
		D3DXVec3Normalize( &vDelta, &vDelta );		// 타겟쪽 단위벡터 구해서
		D3DXVECTOR3 vPos = GetPos(), vTemp;

		CWorld *pWorld = GetWorld();
		if( pWorld == NULL )	
		{
			Delete();
			return;
		}
		vPos += vDelta * 0.25f;			// 그 벡터로 속도를 결정해서 좌표 이동.
		vTemp = vPos;
		vTemp.y += 1.5f;
		FLOAT y = pWorld->GetUnderHeight( vTemp );	// 바닥을 따라서 가게,...
		vPos.y = y;
		vPos.y	= m_SfxObj2.m_vPos.y;		// 康
		m_pSfxObj->m_vPos = vPos;
		SetPos( vPos );
		
		if( m_bDir )
		{
			// 마법객체 방향벡터로 모델 방향 돌리기~~~
			D3DXVECTOR3 vDir      = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
			D3DXVECTOR3 vDestNor  = m_pSfxObj->m_vPos - m_SfxObj2.m_vPos;
			D3DXVECTOR3 vAxis;
			D3DXQUATERNION   qRot;
			
			FLOAT       fTheta;
			D3DXVec3Normalize( &vDestNor, &vDestNor );
			D3DXVec3Cross( &vAxis, &vDir, &vDestNor );
			fTheta = D3DXVec3Dot( &vDir, &vDestNor );
			D3DXQuaternionRotationAxis( &qRot, &vAxis, acosf( fTheta ) );
			
			D3DXVECTOR3 vYPW;
			
			QuaternionRotationToYPW( qRot, vYPW );
			m_pSfxObj->m_vRotate.x = m_SfxObj2.m_vRotate.x = D3DXToDegree(vYPW.x);
			m_pSfxObj->m_vRotate.y = m_SfxObj2.m_vRotate.y = D3DXToDegree(vYPW.y);
			m_pSfxObj->m_vRotate.z = m_SfxObj2.m_vRotate.z = D3DXToDegree(vYPW.z);
		}
		
		vDelta = m_pSfxObj->m_vPos - m_SfxObj2.m_vPos;		// 대상과의 거리
		if( D3DXVec3LengthSq( &vDelta ) < 0.4f * 0.4f )		// 폭발 거리까지 다가가면 충돌.
		{
			if( m_dwSndHit != NULL_ID )
				PLAYSND( m_dwSndHit, &GetPos() );		// 사운드 플레이.
			m_bHit = TRUE;
			DamageToTarget();
		}
	}
	else 
	{
		if( m_SfxObj2.Process() )		// 애니메이션 다 끝났으면 종료.
			Delete();
	}
	m_nFrame ++;
}
void CSfxShootWave::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pSfxObj->m_vScale = GetScale();
	int nFrame = m_pSfxObj->m_nCurFrame;

	for( int i = 0; i < MAX_SHOOTWAVE_TAIL; i ++ )
	{
		if( m_vTail[i].x )
		{
			m_pSfxObj->m_vPos = m_vTail[i];
			m_pSfxObj->Render2( pd3dDevice );
		}
	}
	
	if( m_bHit == FALSE ) 
	{
		m_pSfxObj->m_vPos = GetPos();
		m_pSfxObj->Render2( pd3dDevice );
	}
	else 
	{
		if( !g_Option.m_bSFXRenderOff )
			m_SfxObj2.Render( pd3dDevice );
	}
}

////////////////////////////////////////////////////////////////
// 무버의 특정 파츠에 링크되는 Sfx
void CSfxPartsLink::Process()
{
	m_nFrame++;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;

	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
	if( m_nPartsLink == 0 )		// 오른손
	{
		pModel->GetHandPos( &vPos, PARTS_RWEAPON, pMover->GetMatrixWorld() );		// 오른손 주먹 월드좌표 구함.
	} else
	{
		pModel->GetHandPos( &vPos, PARTS_LWEAPON, pMover->GetMatrixWorld() );
	}
	
	SetPos( vPos );
	
	if( m_pSfxObj->Process() )
	{
		m_bEndFrame = TRUE;
	}
}

void CSfxPartsLink::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model이 없으면 출력하지 않음 
	if( lpModelElem == NULL )
		return;
	
	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}



void CSfxPartsLinkBlade::Process() // 블레이드 파츠이펙트는 밀리 스킬끝나면 소멸
{
	CSfxPartsLink::Process();

	CMover *pMover = prj.GetMover( m_idSrc );
	
	// 밀리스킬 동작 끝나면 해제.
	if( pMover && (pMover->m_pActMover->GetState() & OBJSTA_ATK_ALL) != OBJSTA_ATK_MELEESKILL )
	{
		Delete();		// sfx 삭제.
	}
}

void CSfxPartsLinkJst::Process() // 제스터 파츠이펙트는 끝프레임이면 소멸
{
	CSfxPartsLink::Process();
	
	if( m_bEndFrame )
	{
		Delete();		// sfx 삭제.
	}
}

////////////////////////////////////////////////////////////////
// 아크로뱃 무버의 특정 파츠에 링크되는 Sfx
void CSfxAllowPartsLink::Process()
{
	m_nFrame++;
	
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos;
	
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
	if( m_nPartsLink == 0 )		// 오른손
	{
		pModel->GetHandPos( &vPos, PARTS_RWEAPON, pMover->GetMatrixWorld() );		// 오른손 주먹 월드좌표 구함.
	} else
	{
		pModel->GetHandPos( &vPos, PARTS_LWEAPON, pMover->GetMatrixWorld() );
	}
	
	SetPos( vPos );
	
	if( m_pSfxObj->Process() )
		Delete();
}

void CSfxAllowPartsLink::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model이 없으면 출력하지 않음 
	if( lpModelElem == NULL )
		return;

	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}

void CSfxReady::Process()
{
	CSfx::Process();

	CMover *pSrc = prj.GetMover( m_idSrc );		// 시전자에게 발동.
	if( IsValidObj( pSrc ) )
	{
		if( pSrc->IsStateMode( STATE_BASEMOTION_MODE ) == FALSE )	// 시전동작 끝났으면 
			Delete();		// 삭제.
	} else
		Delete();		// 시전자가 사라져도 삭제.
}


CSfxPartsLinkShoulder::CSfxPartsLinkShoulder():m_nOldAbilityOption(0), m_v3Offset(D3DXVECTOR3( 0.0f, 0.0f, 0.0f ))
{
#ifdef __SFX_OPT
	SetSkill(FALSE);
#endif
}
CSfxPartsLinkShoulder::~CSfxPartsLinkShoulder() 
{
	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsValidObj( pMover ) )
	{
		pMover->m_dwFlag &= (~MVRF_SETITEMSFX);
	}
}

void CSfxPartsLinkShoulder::Process()
{
	CSfx::Process();

	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
	{
		Delete();
		return;
	}
	
	int nAbilityOption	= pMover->GetSetItemClient();
	if( nAbilityOption < 3 || nAbilityOption != m_nOldAbilityOption )
	{
		pMover->m_dwFlag &= (~MVRF_SETITEMSFX);
		Delete();
		return;
	}
}


void	CSfxPartsLinkShoulder::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CMover *pMover = prj.GetMover( m_idSrc );

	if( pMover->IsActiveMover() && CWorld::m_nZoomLevel != 0 )
		return;

#ifdef __QUIZ
	if( pMover->GetWorld()->GetID() == WI_WORLD_QUIZ && pMover->IsPlayer() == TRUE && pMover->IsDisguise() == TRUE )
		return;
#endif // __QUIZ
	
	D3DXVECTOR3 vVel = D3DXVECTOR3( 0.0f, 0.04f, 0.0f );
	D3DXVECTOR3 vPos;
	D3DXMATRIX  mPartMatrix;
	
	CModelObject *pModel = (CModelObject *)pMover->m_pModel;

	if( m_nPartsLink == 0 )		// 오른 어깨
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(15)), &(pMover->GetMatrixWorld()) );
	} 
	else
	if( m_nPartsLink == 1 )		// 왼 어깨
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(11)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 2 )    // 오른 팔꿈치
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(16)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 3 )	   // 왼 팔꿈치
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(12)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 4 )   // 오른쪽어깨 + 팔꿈치 보간
	{
		D3DXMATRIX  mMatrix1;
		D3DXMATRIX  mMatrix2;

		D3DXMatrixMultiply( &mMatrix1, (pModel->GetMatrixBone(15)), &(pMover->GetMatrixWorld()) );
		D3DXMatrixMultiply( &mMatrix2, (pModel->GetMatrixBone(16)), &(pMover->GetMatrixWorld()) );

		D3DXVECTOR3 vPos1 = D3DXVECTOR3( mMatrix1._41, mMatrix1._42, mMatrix1._43 );
		D3DXVECTOR3 vPos2 = D3DXVECTOR3( mMatrix2._41, mMatrix2._42, mMatrix2._43 );

		D3DXVECTOR3 vPos3;
		D3DXVec3Lerp( &vPos3, &vPos1, &vPos2, 0.6f );

		mPartMatrix = mMatrix1;
		mPartMatrix._41 = vPos3.x; 
		mPartMatrix._42 = vPos3.y; 
		mPartMatrix._43 = vPos3.z; 
	}
	else
	if( m_nPartsLink == 5 )   // 왼쪽어깨 + 팔꿈치 보간
	{
		D3DXMATRIX  mMatrix1;
		D3DXMATRIX  mMatrix2;
		
		D3DXMatrixMultiply( &mMatrix1, (pModel->GetMatrixBone(11)), &(pMover->GetMatrixWorld()) );
		D3DXMatrixMultiply( &mMatrix2, (pModel->GetMatrixBone(12)), &(pMover->GetMatrixWorld()) );
		
		D3DXVECTOR3 vPos1 = D3DXVECTOR3( mMatrix1._41, mMatrix1._42, mMatrix1._43 );
		D3DXVECTOR3 vPos2 = D3DXVECTOR3( mMatrix2._41, mMatrix2._42, mMatrix2._43 );
		
		D3DXVECTOR3 vPos3;
		D3DXVec3Lerp( &vPos3, &vPos1, &vPos2, 0.6f );
		
		mPartMatrix = mMatrix1;
		mPartMatrix._41 = vPos3.x; 
		mPartMatrix._42 = vPos3.y; 
		mPartMatrix._43 = vPos3.z; 
	}
	else
	if( m_nPartsLink == 6 )    // 몸 중간
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(2)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 7 )	   // 몸 아래
	{
		D3DXMatrixMultiply( &mPartMatrix, (pModel->GetMatrixBone(3)), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 8 )    // 오른손
	{
		D3DXMatrixMultiply( &mPartMatrix, pModel->GetMatrixBone(pModel->GetRHandIdx()), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 9 )	   // 왼손
	{
		D3DXMatrixMultiply( &mPartMatrix, pModel->GetMatrixBone(pModel->GetLHandIdx()), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 26 )	   // 왼손
	{
		D3DXMatrixMultiply( &mPartMatrix, pModel->GetMatrixBone(m_nPartsLink), &(pMover->GetMatrixWorld()) );
	}
	else
	if( m_nPartsLink == 29 )	   // 왼손
	{
		D3DXMatrixMultiply( &mPartMatrix, pModel->GetMatrixBone(m_nPartsLink), &(pMover->GetMatrixWorld()) );
	}
		
			
	vPos.x = mPartMatrix._41;
	vPos.y = mPartMatrix._42-0.05f;
	vPos.z = mPartMatrix._43;

	vPos -= m_v3Offset;
	
	SetPos( vPos );

	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
		return;
	if( ((pMover->IsMode( TRANSPARENT_MODE ) ) == 0) )
		CSfx::Render(pd3dDevice);
}

#if __VER >= 12 // __ITEMCREATEMON_S0602
CSfxCursor::CSfxCursor()
{
}
CSfxCursor::~CSfxCursor()
{
}
void CSfxCursor::Process()
{
	SetPos( g_Neuz.m_vCursorPos );		// 마우스 좌표
	m_pSfxObj->Process();

	if( m_nSec != 0 )	// 0은 1회 플레이후 종료.
	{
		// 반복 애니메이션
		if( m_pSfxObj->Process() )		
			m_pSfxObj->m_nCurFrame = 0;
	}

	if( g_Neuz.m_pCreateMonItem == NULL )
		Delete();
}
#endif // __ITEMCREATEMON_S0602


CSfxLinkMover::CSfxLinkMover( )
{
}

CSfxLinkMover::~CSfxLinkMover( )
{
}

void CSfxLinkMover::Process( )
{
	m_nFrame++;
	
	CMover* pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )	// process중 src 무버가 없어졌으면 자동 삭제
	{
		Delete();
		return;
	}
	
	D3DXVECTOR3 vPos = pMover->GetPos( );
	
	SetPos( vPos );
	
	if( m_pSfxObj->Process() )
	{
		m_pSfxObj->m_nCurFrame = 0;
	}
}

void CSfxLinkMover::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !IsVisible() || ( IsCull() && GetType() != 1 ))
		return;
	if( IsUpdateMatrix() )
		UpdateMatrix();
	
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwType, m_dwIndex );
	// Model이 없으면 출력하지 않음 
	if( lpModelElem == NULL )
		return;

	CMover *pMover = prj.GetMover( m_idSrc );
	if( IsInvalidObj( pMover ) )
		return;

	D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );
//	CModelObject *pModel = (CModelObject *)pMover->m_pModel;
//	pModel->GetEventPos( &vPos, 0 );	// 등드리 좌표
	
	D3DXVec3TransformCoord( &vPos, &vPos, pMover->GetMatrixWorldPtr() );
	
	SetPos( vPos );

	m_pSfxObj->m_vPos = GetPos();
	m_pSfxObj->m_vRotate.y = GetAngle();
	m_pSfxObj->m_vScale = GetScale();
	m_pSfxObj->m_matScale = m_matScale;
	m_pSfxObj->Render( pd3dDevice, NULL );
}