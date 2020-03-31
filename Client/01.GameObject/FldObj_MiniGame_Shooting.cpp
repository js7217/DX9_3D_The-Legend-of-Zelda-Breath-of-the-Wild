#include "stdafx.h"
#include "FldObj_MiniGame_Shooting.h"
#include "ObjectManager.h"
#include "Obj_YabusameTarget_A.h"
#include "Quest_Ballon_UI.h"

CFldObj_MiniGame_Shooting::CFldObj_MiniGame_Shooting(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
	, m_pManagement(GET_INSTANCE(CManagement))
{

}

CFldObj_MiniGame_Shooting::CFldObj_MiniGame_Shooting(const CFldObj_MiniGame_Shooting & rhs)
	: CGameObject(rhs)
	, m_pManagement(rhs.m_pManagement)
{

}

HRESULT CFldObj_MiniGame_Shooting::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CFldObj_MiniGame_Shooting::Ready_GameObject(void * pArg)
{	
	m_matWorld = *(_matrix*)pArg;

	m_dwRandom = 5.0;
	return S_OK;
}

_int CFldObj_MiniGame_Shooting::Update_GameObject(const _double & TimeDelta)
{
	_int iSceneNum = m_pManagement->Get_CurrentSceneNum();
	list<CGameObject*> pList;
	
	if(m_iCnt > 0)
		pList = *GET_INSTANCE(CObjectManager)->Get_ObjectList(iSceneNum, L"Layer_YabusameTarget_A");

	if (m_iCnt >= 20 && pList.empty())
	{
		GET_INSTANCE(CSoundMgr)->Stop_SoundChannel(CSoundMgr::BGM);
		GET_INSTANCE(CSoundMgr)->Play_BGM(L"Hateno.mp3");
		
		CQuest_Ballon_UI* pQuest_UI = (CQuest_Ballon_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Ballon_UI", 0);
		if (pQuest_UI == nullptr)
			return E_FAIL;
		//pQuest_UI->Set_GameStart(true);
		pQuest_UI->Set_GameStart(false);
		pQuest_UI->Set_IsDraw(false, 0);
		return END_EVENT;
	}

	if (m_dwRandom <= m_dwTimer)
	{
		m_dwTimer = 0.0;		// 시간 초기화
		m_iCnt++;				// 풍선 카운트 추가.
		//풍선 랜덤한 위치 생성.
		_vec3 vPos = { (float)NxMath::rand(-10, 10), (float)NxMath::rand(5, 15), (float)NxMath::rand(-25, -10) };

		_matrix matWorld = m_matWorld;

		matWorld._41 += vPos.x;
		matWorld._42 += vPos.y;
		matWorld._43 += vPos.z;

		
		if(m_iCnt <= 20)
		if (FAILED(m_pManagement->Add_GameObject(iSceneNum, L"GameObject_Obj_YabusameTarget_A", iSceneNum, L"Layer_YabusameTarget_A", (void*)&matWorld)))
			return E_FAIL;

		m_dwRandom = (_double)NxMath::rand(1.5f, 3.f);
	}
	else
		m_dwTimer += TimeDelta;

	CGameObject::Update_GameObject(TimeDelta);
	return NO_EVENT;
}

_int CFldObj_MiniGame_Shooting::LateUpdate_GameObject(const _double & TimeDelta)
{

	return NO_EVENT;
}

HRESULT CFldObj_MiniGame_Shooting::Render_GameObject()
{
	return NO_EVENT;
}

CGameObject * CFldObj_MiniGame_Shooting::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CFldObj_MiniGame_Shooting(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CFldObj_MiniGame_Shooting::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CFldObj_MiniGame_Shooting(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CFldObj_MiniGame_Shooting::Free()
{
	CGameObject::Free();
}

