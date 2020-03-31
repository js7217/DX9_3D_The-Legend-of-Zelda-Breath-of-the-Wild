#include "stdafx.h"
#include "MainQuest.h"
#include "Npc.h"
#include "MainQuest.h"
#include "KeyMgr.h"
#include "FontMgr.h"
#include "Bokurin.h"
#include "Inventory_UI.h"

CMainQuest::CMainQuest(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CMainQuest::CMainQuest(CMainQuest & rhs)
	: CGameObject(rhs)
{
}

void CMainQuest::Set_Npc(CNpc * pNpc)
{
	if(nullptr != pNpc)
		m_pNpc = pNpc;

	//ScriptAnimation();
}

_bool CMainQuest::Get_QuestClear()
{
	if (TRUE == m_bIsHinox && TRUE == m_bIsMaracus)
		return TRUE;

	return FALSE;
}

HRESULT CMainQuest::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CMainQuest::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (m_pNpc != nullptr)
		m_pNpc->Set_StartTalk();


	if (m_pNpc != nullptr)
		m_pNpc->Set_TalkEnd();



	//UI SIZE
	m_fSizeX = 605.f / 2;
	m_fSizeY = 130.f / 2;

	m_fX = 100.f;
	m_fY = 100.f;

	m_fIEnterSpeed = 0.22f;
	m_fAlphaSpeed = 0.05f;
	return S_OK;
}

_int CMainQuest::Update_GameObject(const _double & TimeDelta)
{
	if (nullptr == m_pTransform)
		return -1;

	_int iSceneNum =GET_INSTANCE(CManagement)->Get_CurrentSceneNum();

	if (iSceneNum != SCENE_FIELD)
	{
		m_pNpc = nullptr;
		return NO_EVENT;
	}

	if (!m_bIsDraw)
	{
		m_nScriptNum_Apply = 0;
		m_nScriptNum = 0;
		m_fAlphaSpeed = 0.05f;
		m_fQuestNameAlpha = -1.f;
		m_fAlphaSpeed = 0.05f;
		m_fMinScale = 1.f;
		m_fEndAlpha = 1.f;

		m_bIsDialog = false;
		
	}

	if (m_bIsDraw)
	{
		//if (iSceneNum == 8)
		//{
		//	if (m_pNpc != nullptr)
		//		m_pNpc->Set_StartTalk();
		//}
	}

	//

	m_fScale_EnterArrow += m_fIEnterSpeed;
	if (m_fScale_EnterArrow >= 4.5f)
	{
		m_fIEnterSpeed *= -1.f;
	}
	if (m_fScale_EnterArrow <= 0.55f)
	{
		m_fIEnterSpeed *= -1.f;
	}
	//

	//m_nScriptNum_Apply = 0;
	//m_nScriptNum = 0;

	//
	if (m_nScriptNum_Apply == 1 && m_nScriptNum == 0)
	{
		m_fQuestNameAlpha += m_fAlphaSpeed;
		m_fAlphaTime += (_float)TimeDelta;

		if (m_fAlphaTime > 2.0f)
		{
			if (m_fQuestNameAlpha >= 1.f)
				m_fAlphaSpeed *= -1.f;
			m_fAlphaTime = 0.f;
		}
		if (m_fQuestNameAlpha >= 1.f)
		{
			m_bIsDialog = true;
			
		}
		if (m_fQuestNameAlpha <= -1.7f)
		{
			m_bIsDraw = false;
			m_bIsQuestStart = true;
		}
	}

	if(m_bIsDraw)
		KeyInput();
	//m_bIsMaracus = false;
	//m_bIsHinox = true;

	if (m_bIsHinox && m_bIsMaracus)
	{
		m_bIsQuestStart = false;

	}
	if (m_bIsHinox && m_bIsMaracus && m_bIsDraw)
		m_bMissionClear = true;


	///////
	if (m_bIsReward)
	{
		m_fRewardTime += (_float)TimeDelta;

		if (m_fRewardTime >= 3.5f)
		{
			m_fRewardTime = 0.f;
			m_bIsReward = false;
			m_fPlusUp = 0.f;
			
			m_fAlphaSpeed = 0.05f;
			m_fQuestNameAlpha = -1.f;
			m_fMinScale = 1.f;
			m_fEndAlpha = 1.f;
			m_bMissionClear = false;
			m_bIsDraw = false;

			//m_bIsDraw = false;
		}
	}
	if (m_bIsPlusUp)
	{

		if (50.f <= m_fPlusUp)
		{
			m_fPlusUp = 50.f;
			m_bIsPlusUp = false;
		}
		else
			m_fPlusUp += 1.f;
	}


	return _int();
}

_int CMainQuest::LateUpdate_GameObject(const _double & TimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;
	m_bIsHinox =  GET_INSTANCE(CManagement)->Get_DeadGiant();
//	m_bIsMaracus = GET_INSTANCE(CManagement)->Get_Ma
	if (m_bMissionClear && m_bIsDraw)
	{
		m_fQuestNameAlpha += m_fAlphaSpeed;
		m_fAlphaTime += (_float)TimeDelta;

		if (m_fMinScale <= 2.f)
			m_fMinScale += (_float)TimeDelta * 10.f;
		if (m_fAlphaTime > 1.5f)
		{
			if (m_fQuestNameAlpha >= 1.f)
				m_fAlphaSpeed *= -1.f;
			m_fAlphaTime = 0.f;
		}
		if (m_fQuestNameAlpha <= -1.7f)
		{
			m_bIsReward = true;
			m_bIsPlusUp = true;

			//m_fAlphaSpeed = 0.05f;
			//m_fQuestNameAlpha = -1.f;
			//m_fMinScale = 1.f;
			//m_fEndAlpha = 1.f;
			//m_bMissionClear = false;
			//m_bIsDraw = false;

		}
		//if (m_fQuestNameAlpha <= -1.f)
		//	m_bIsEndAlpha = true;

		if (m_fAlphaSpeed <= 0.f &&  m_fQuestNameAlpha <= -0.1f)
			m_bIsEndAlpha = true;

	}
	if (m_bIsEndAlpha)
	{
		if (m_fEndAlpha >= 0.f)
			m_fEndAlpha -= (_float)TimeDelta;

		if (m_fEndAlpha <= 0.f)
		{
			
			m_bIsEndAlpha = false;
		}
	}


	//Render Position 정 중 앙^
	m_pTransform->Set_Scale(&_vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransform->Set_State(CTransform::STATE_POSITION, &_vec3(0.f, -200.f, 0.0f));

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_UI, this)))
		return -1;

	return _int();
}

HRESULT CMainQuest::Render_GameObject()
{
	//if (m_bIsDraw == false)
	//	return S_OK;

	if (nullptr == m_pBuffer ||
		nullptr == m_pTexture ||
		nullptr == m_pShader)
		return E_FAIL;

	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;
	_matrix			matView, matProj;
	_matrix		matWorld;
	if (m_bIsDraw)
	{
		if (!m_bIsDialog && !m_bMissionClear)
		{
			pEffect->SetMatrix("g_matWorld", m_pTransform->Get_WorldMatrixPointer());
			pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));
			D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);

			pEffect->SetMatrix("g_matProj", &matProj);
			pEffect->SetFloat("g_fAlpha_Main", 1.f);
			if (FAILED(m_pTexture->SetUp_OnShader(pEffect, "g_BaseTexture", 3)))
				return E_FAIL;


			//if (FAILED(SetUp_ConstantTable(pEffect)))
			//	return E_FAIL;

			pEffect->Begin(nullptr, 0);
			pEffect->BeginPass(3);

			m_pBuffer->Render_VIBuffer();

			pEffect->EndPass();
			pEffect->End();

			///////////////////////////
			matWorld = *m_pTransform->Get_WorldMatrixPointer();

			matWorld._11 = 27 / 2.f;
			matWorld._22 = 19 / 2.f;
			matWorld._42 -= (65.f + m_fScale_EnterArrow);
			//
			pEffect->SetMatrix("g_matWorld", &matWorld);

			pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));

			D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);

			pEffect->SetMatrix("g_matProj", &matProj);

			if (FAILED(m_pTex_EnterArrow->SetUp_OnShader(pEffect, "g_BaseTexture")))
				return E_FAIL;

			pEffect->Begin(nullptr, 0);
			pEffect->BeginPass(3);

			m_pBuffer->Render_VIBuffer();

			pEffect->EndPass();
			pEffect->End();
			///////////////////////////////////////



		}
		/////////////// fonts

		//_tchar szStr[MAX_PATH] = L"";
		//swprintf_s(szStr, L"%d", m_Arrow);
		//GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Default", szStr, &_vec2(650.f, 330.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));


		if (m_nScriptNum_Apply == 0 && m_nScriptNum == 2 && !m_bMissionClear)
		{
			matWorld = *m_pTransform->Get_WorldMatrixPointer();
			matWorld._11 = 243 / 2.f;
			matWorld._22 = 48 / 2.f;
			matWorld._41 += 485.f;
			matWorld._42 += (30.f);

			//
			pEffect->SetMatrix("g_matWorld", &matWorld);

			pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));

			D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);
			pEffect->SetFloat("g_fAlpha_Main", 1.f);
			pEffect->SetMatrix("g_matProj", &matProj);

			if (FAILED(m_pTexBtn_One->SetUp_OnShader(pEffect, "g_BaseTexture", 0)))
				return E_FAIL;

			pEffect->Begin(nullptr, 0);
			pEffect->BeginPass(3);

			m_pBuffer->Render_VIBuffer();

			pEffect->EndPass();
			pEffect->End();

			matWorld = *m_pTransform->Get_WorldMatrixPointer();
			matWorld._11 = 243 / 2.f;
			matWorld._22 = 48 / 2.f;
			matWorld._41 += 485.f;
			matWorld._42 -= (40.f);

			pEffect->SetMatrix("g_matWorld", &matWorld);

			pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));

			D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);
			pEffect->SetFloat("g_fAlpha_Main", 1.f);
			pEffect->SetMatrix("g_matProj", &matProj);

			if (FAILED(m_pTexBtn_One->SetUp_OnShader(pEffect, "g_BaseTexture", 1)))
				return E_FAIL;

			pEffect->Begin(nullptr, 0);
			pEffect->BeginPass(3);

			m_pBuffer->Render_VIBuffer();

			pEffect->EndPass();
			pEffect->End();


			//
			matWorld = *m_pTransform->Get_WorldMatrixPointer();
			matWorld._11 = 19 / 2.f;
			matWorld._22 = 27 / 2.f;
			matWorld._41 += 365.f;
			matWorld._42 += (30.f + m_fArrowMove);

			pEffect->SetMatrix("g_matWorld", &matWorld);
			pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));
			D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);

			pEffect->SetMatrix("g_matProj", &matProj);

			if (FAILED(m_pTex_Arrow->SetUp_OnShader(pEffect, "g_BaseTexture")))
				return E_FAIL;

			pEffect->Begin(nullptr, 0);
			pEffect->BeginPass(3);

			m_pBuffer->Render_VIBuffer();

			pEffect->EndPass();
			pEffect->End();
			//


		}

		if (m_nScriptNum_Apply == 1 && m_nScriptNum == 0)
		{
			//m_pTexName
			matWorld = *m_pTransform->Get_WorldMatrixPointer();
			matWorld._11 = 1280.f / 2.f;
			matWorld._22 = 720.f / 2.f;
			matWorld._41 = 0.f;
			matWorld._42 = 0.f;
			pEffect->SetFloat("g_AlphaQuestName", m_fQuestNameAlpha);
			pEffect->SetMatrix("g_matWorld", &matWorld);
			pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));
			pEffect->SetFloat("g_fAlpha_Main", 1.f);
			D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);
			pEffect->SetMatrix("g_matProj", &matProj);

			if (FAILED(m_pTexName->SetUp_OnShader(pEffect, "g_BaseTexture", 0)))
				return E_FAIL;
			pEffect->Begin(nullptr, 0);
			pEffect->BeginPass(9);
			m_pBuffer->Render_VIBuffer();
			pEffect->EndPass();
			pEffect->End();
			//
		}
	}
	//미션 시작시
	if (m_bIsQuestStart && !m_bMissionClear)
	{
		//퀘스트 창 띄우기
		//m_pTexMissionWin
		matWorld = *m_pTransform->Get_WorldMatrixPointer();
		matWorld._11 = 203.f / 2.0f;
		matWorld._22 = 120.f / 2.0f;
		matWorld._41 = -540.f;
		matWorld._42 = 50.f;
		pEffect->SetFloat("g_AlphaQuestName", m_fQuestNameAlpha);
		pEffect->SetMatrix("g_matWorld", &matWorld);
		pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));
		pEffect->SetFloat("g_fAlpha_Main", 1.f);
		D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);
		pEffect->SetMatrix("g_matProj", &matProj);

		if (FAILED(m_pTexMissionWin->SetUp_OnShader(pEffect, "g_BaseTexture", 0)))
			return E_FAIL;
		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(3);
		m_pBuffer->Render_VIBuffer();
		pEffect->EndPass();
		pEffect->End();
		//
		if (m_bIsHinox)
		{
			//Complete
			//m_pTexComplete
			matWorld = *m_pTransform->Get_WorldMatrixPointer();
			matWorld._11 = 36.f / 2.f;
			matWorld._22 = 15.f / 2.f;
			matWorld._41 = -508.f;
			matWorld._42 = 52.f;
			pEffect->SetFloat("g_AlphaQuestName", m_fQuestNameAlpha);
			pEffect->SetMatrix("g_matWorld", &matWorld);
			pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));
			pEffect->SetFloat("g_fAlpha_Main", 1.f);
			D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);
			pEffect->SetMatrix("g_matProj", &matProj);

			if (FAILED(m_pTexComplete->SetUp_OnShader(pEffect, "g_BaseTexture", 0)))
				return E_FAIL;
			pEffect->Begin(nullptr, 0);
			pEffect->BeginPass(3);
			m_pBuffer->Render_VIBuffer();
			pEffect->EndPass();
			pEffect->End();
			//
		}
		if (m_bIsMaracus)
		{
			//m_pTexComplete
			matWorld = *m_pTransform->Get_WorldMatrixPointer();
			matWorld._11 = 36.f / 2.f;
			matWorld._22 = 15.f / 2.f;
			matWorld._41 = -495.f;
			matWorld._42 = 30.f;
			pEffect->SetFloat("g_AlphaQuestName", m_fQuestNameAlpha);
			pEffect->SetMatrix("g_matWorld", &matWorld);
			pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));
			pEffect->SetFloat("g_fAlpha_Main", 1.f);
			D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);
			pEffect->SetMatrix("g_matProj", &matProj);

			if (FAILED(m_pTexComplete->SetUp_OnShader(pEffect, "g_BaseTexture", 0)))
				return E_FAIL;
			pEffect->Begin(nullptr, 0);
			pEffect->BeginPass(3);
			m_pBuffer->Render_VIBuffer();
			pEffect->EndPass();
			pEffect->End();
			//
		}

	}

	//m_bMissionClear 미션 완료 시 
	//m_bMissionClear = true;
	if (m_bMissionClear && m_bIsDraw)
	{
		pEffect->SetMatrix("g_matWorld", m_pTransform->Get_WorldMatrixPointer());
		pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));
		D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);

		pEffect->SetMatrix("g_matProj", &matProj);
		pEffect->SetFloat("g_fAlpha_Main", 1.f);
		if (FAILED(m_pTexture->SetUp_OnShader(pEffect, "g_BaseTexture", 3)))
			return E_FAIL;


		//if (FAILED(SetUp_ConstantTable(pEffect)))
		//	return E_FAIL;

		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(3);

		m_pBuffer->Render_VIBuffer();

		pEffect->EndPass();
		pEffect->End();

		///////////////////////////
		matWorld = *m_pTransform->Get_WorldMatrixPointer();

		matWorld._11 = 27 / 2.f;
		matWorld._22 = 19 / 2.f;
		matWorld._42 -= (65.f + m_fScale_EnterArrow);
		//
		pEffect->SetMatrix("g_matWorld", &matWorld);

		pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));

		D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);

		pEffect->SetMatrix("g_matProj", &matProj);

		if (FAILED(m_pTex_EnterArrow->SetUp_OnShader(pEffect, "g_BaseTexture")))
			return E_FAIL;

		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(3);

		m_pBuffer->Render_VIBuffer();

		pEffect->EndPass();
		pEffect->End();
		///////////////////////////////////////

		//Name Render
		matWorld = *m_pTransform->Get_WorldMatrixPointer();
		matWorld._11 = 1280.f / 2.f;
		matWorld._22 = 720.f / 2.f;
		matWorld._41 = 0.f;
		matWorld._42 = 0.f;
		pEffect->SetFloat("g_AlphaQuestName", m_fQuestNameAlpha);
		pEffect->SetMatrix("g_matWorld", &matWorld);
		pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));
		pEffect->SetFloat("g_fAlpha_Main", 1.f);
		D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);
		pEffect->SetMatrix("g_matProj", &matProj);

		if (FAILED(m_pTexName->SetUp_OnShader(pEffect, "g_BaseTexture", 0)))
			return E_FAIL;
		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(9);
		m_pBuffer->Render_VIBuffer();
		pEffect->EndPass();
		pEffect->End();

		//m_pTexComplete
		matWorld = *m_pTransform->Get_WorldMatrixPointer();
		matWorld._11 = (202.f / m_fMinScale);
		matWorld._22 = (42.f / m_fMinScale);
		matWorld._41 = 350.f;
		matWorld._42 = 160.f;
		pEffect->SetFloat("g_AlphaQuestName", m_fQuestNameAlpha);
		pEffect->SetMatrix("g_matWorld", &matWorld);
		pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));
		pEffect->SetFloat("g_fAlpha_Main", m_fEndAlpha);
		D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);
		pEffect->SetMatrix("g_matProj", &matProj);

		if (FAILED(m_pTexCompleteName->SetUp_OnShader(pEffect, "g_BaseTexture", 0)))
			return E_FAIL;
		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(3);
		m_pBuffer->Render_VIBuffer();
		pEffect->EndPass();
		pEffect->End();
		/////////////////////////////////////////////////////////////

		MissionClearRender();
	}

	if (m_bIsReward)
	{
		matWorld = *m_pTransform->Get_WorldMatrixPointer();
		matWorld._11 = 557.f / 2.f;
		matWorld._22 = 207.f / 2.f;
		matWorld._41 = 0.f;
		matWorld._42 = 50.f + m_fPlusUp;
		pEffect->SetFloat("g_AlphaQuestName", m_fQuestNameAlpha);
		pEffect->SetMatrix("g_matWorld", &matWorld);
		pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));
		pEffect->SetFloat("g_fAlpha_Main", 1.f);
		D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);
		pEffect->SetMatrix("g_matProj", &matProj);

		if (FAILED(m_pTexReward->SetUp_OnShader(pEffect, "g_BaseTexture", 0)))
			return E_FAIL;
		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(3);
		m_pBuffer->Render_VIBuffer();
		pEffect->EndPass();
		pEffect->End();


		matWorld = *m_pTransform->Get_WorldMatrixPointer();
		matWorld._11 = 185 / 2.f;
		matWorld._22 = 36 / 2.f;
		matWorld._41 = 550.f;
		matWorld._42 = 255.f;
		pEffect->SetFloat("g_AlphaQuestName", m_fQuestNameAlpha);
		pEffect->SetMatrix("g_matWorld", &matWorld);
		pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));
		pEffect->SetFloat("g_fAlpha_Main", 1.f);
		D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);
		pEffect->SetMatrix("g_matProj", &matProj);

		if (FAILED(m_pTexMoneyBar->SetUp_OnShader(pEffect, "g_BaseTexture", 0)))
			return E_FAIL;
		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(3);
		m_pBuffer->Render_VIBuffer();
		pEffect->EndPass();
		pEffect->End();

		m_fMoneyPlusTime += .08f;

		if (m_fMoneyPlusTime > 0.1f)
		{
			if (m_iPlayerMoney != m_iMoneyPre)
				++m_iMoneyPre;
			m_fMoneyPlusTime = 0.f;
		}

		_int m_iMoney1000 = _int(ceilf((_float)(m_iMoneyPre / 1000)));
		int m_iNum100 = (m_iMoneyPre % 1000) / 100;
		int m_iNum10 = (m_iMoneyPre % 100) / 10;
		int iNum = m_iMoneyPre % 10;

		//m_iMoney100 = m_iNum100 / 100;
		//m_iMoney10 = m_iNum10 / 10;
		//m_iMoney = iNum;

		D3DXMatrixIdentity(&matWorld);
		matWorld._11 = 17.f / 2.f;
		matWorld._22 = 15.f / 2.f;
		matWorld._41 = 552.f;
		matWorld._42 = 250.f;

		pEffect->SetFloat("g_fAlpha_Main", 1.f);
		pEffect->SetMatrix("g_matWorld", &matWorld);

		pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));

		D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);

		pEffect->SetMatrix("g_matProj", &matProj);

		if (FAILED(m_pTexNum->SetUp_OnShader(pEffect, "g_BaseTexture", iNum)))
			return E_FAIL;

		//if (FAILED(SetUp_ConstantTable(pEffect)))
		//	return E_FAIL;

		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(3);

		m_pBuffer->Render_VIBuffer();

		pEffect->EndPass();
		pEffect->End();
		/////////////////////////////////////////////////////////////
		D3DXMatrixIdentity(&matWorld);
		matWorld._11 = 17.f / 2.f;
		matWorld._22 = 15.f / 2.f;
		matWorld._41 = 541.f;
		matWorld._42 = 250.f;
		//g_AlphaQuestName

		pEffect->SetFloat("g_fAlpha_Main", 1.f);
		pEffect->SetMatrix("g_matWorld", &matWorld);

		pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));

		D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);

		pEffect->SetMatrix("g_matProj", &matProj);

		if (FAILED(m_pTexNum->SetUp_OnShader(pEffect, "g_BaseTexture", m_iNum10)))
			return E_FAIL;

		//if (FAILED(SetUp_ConstantTable(pEffect)))
		//	return E_FAIL;

		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(3);

		m_pBuffer->Render_VIBuffer();

		pEffect->EndPass();
		pEffect->End();
		/////////////////////////////////////////////////////////////
		D3DXMatrixIdentity(&matWorld);
		matWorld._11 = 17.f / 2.f;
		matWorld._22 = 15.f / 2.f;
		matWorld._41 = 530.f;
		matWorld._42 = 250.f;
		//g_AlphaQuestName

		pEffect->SetFloat("g_fAlpha_Main", 1.f);
		pEffect->SetMatrix("g_matWorld", &matWorld);

		pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));

		D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);

		pEffect->SetMatrix("g_matProj", &matProj);

		if (FAILED(m_pTexNum->SetUp_OnShader(pEffect, "g_BaseTexture", m_iNum100)))
			return E_FAIL;

		//if (FAILED(SetUp_ConstantTable(pEffect)))
		//	return E_FAIL;

		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(3);

		m_pBuffer->Render_VIBuffer();

		pEffect->EndPass();
		pEffect->End();
		/////////////////////////////////////////////////////////////
		D3DXMatrixIdentity(&matWorld);
		matWorld._11 = 17.f / 2.f;
		matWorld._22 = 15.f / 2.f;
		matWorld._41 = 520.f;
		matWorld._42 = 250.f;
		//g_AlphaQuestName

		pEffect->SetFloat("g_fAlpha_Main", 1.f);
		pEffect->SetMatrix("g_matWorld", &matWorld);

		pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matView));

		D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.0f, 1.f);

		pEffect->SetMatrix("g_matProj", &matProj);

		if (FAILED(m_pTexNum->SetUp_OnShader(pEffect, "g_BaseTexture", m_iMoney1000)))
			return E_FAIL;

		//if (FAILED(SetUp_ConstantTable(pEffect)))
		//	return E_FAIL;

		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(3);

		m_pBuffer->Render_VIBuffer();

		pEffect->EndPass();
		pEffect->End();

	}




	//퀘스트 완료시 대화 필요함
	if (!m_bIsDialog && !m_bMissionClear && m_bIsDraw)
		ScriptRender();




	return NOERROR;
}

HRESULT CMainQuest::Add_Component()
{
	// For.Com_UITransform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Shader_UI", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// For.Com_Buffer
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Buffer_RcTex", L"Com_Buffer", (CComponent**)&m_pBuffer)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_QuestDialog", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;
	//Component_Texture_QuestDialog_One m_pTexBtn_One
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_MainQuestBtn", L"Com_BtnOne", (CComponent**)&m_pTexBtn_One)))
		return E_FAIL;
	//m_pTex_EnterArrow
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_Dialog_Enter_Arrow", L"Com_Texture_EnterArrow", (CComponent**)&m_pTex_EnterArrow)))
		return E_FAIL;
	//Component_Texture_UI_Shop_Arrow m_pTex_Arrow
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_UI_Shop_Arrow", L"Com_Texture_Arrow", (CComponent**)&m_pTex_Arrow)))
		return E_FAIL;
	//Component_Texture_MainQuestName
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_MainQuestName", L"Com_Texture_QuestName", (CComponent**)&m_pTexName)))
		return E_FAIL;
	//m_pTexComplete
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_MainQuest_Window", L"Com_Texture_Font_Complete", (CComponent**)&m_pTexMissionWin)))
		return E_FAIL;
	//m_pTexMissionWin
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_MainQuest_Complete", L"Com_Texture_MissionWin", (CComponent**)&m_pTexComplete)))
		return E_FAIL;

	//m_pTexComplete
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_Quest_MissionCompleteFont", L"Com_Texture_Complete", (CComponent**)&m_pTexCompleteName)))
		return E_FAIL;

	//m_pTexNum;
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_BallonNum", L"Com_Texture_TexNum", (CComponent**)&m_pTexNum)))
		return E_FAIL;
	//m_pTexReward Component_Texture_Quest_k`okoRewardItem
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_Quest_kokoRewardItem", L"Com_Texture_Reward", (CComponent**)&m_pTexReward)))
		return E_FAIL;
	//Component_Texture_UI_MoneyBar   m_pTexMoneyBar
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_UI_MoneyBar", L"Com_Texture_MoneyBar", (CComponent**)&m_pTexMoneyBar)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainQuest::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect)
		return E_FAIL;

	pEffect->AddRef();




	Safe_Release(pEffect);

	return S_OK;
}

void CMainQuest::ScriptRender()
{
	_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();
	
	_tchar szStr[MAX_PATH] = L"";
	//m_nScriptNum_Apply = 1;
	//m_nScriptNum = 1;
	if (m_nScriptNum_Apply == 0)
	{
		switch (m_nScriptNum)
		{
		case 0:
		{
			swprintf_s(szStr, L"난 일뀨뀨!");
			GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(560.f, 535.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
			swprintf_s(szStr, L"뀨뀨 난 기운이 없어...");
			GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(510.f, 570.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

			if (iSceneNum == 8)
			{
				if (m_pNpc != nullptr)
					m_pNpc->Set_StartTalk();
			}
		}
		break;
		case 1:
		{
			swprintf_s(szStr, L"마을 괴물 히녹스가");
			GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(520.f, 535.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
			swprintf_s(szStr, L"마라카스를 훔쳐갔어...");
			GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(510.f, 570.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		}
		break;
		case 2:
		{
			swprintf_s(szStr, L"링크!!!히녹스를 잡고 ");
			GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(480.f, 535.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
			swprintf_s(szStr, L"마라카스를 갖다줄 수 있어?");
			GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(430.f, 570.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		}
		break;

		default:
			break;
		}
	}
	else if (m_nScriptNum_Apply == 1)
	{
		switch (m_nScriptNum)
		{
		case 0:
		{
			swprintf_s(szStr, L"역시 링크라면");
			GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(560.f, 535.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
			swprintf_s(szStr, L"도와줄 줄 알었어!!");
			GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(535.f, 570.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

			if (iSceneNum == 8)
			{
				if (m_pNpc != nullptr)
					m_pNpc->Set_TalkEnd();
			}
			
		}
		break;
		default:
			break;
		}
	}
	else if (m_nScriptNum_Apply == 2)
	{
		switch (m_nScriptNum)
		{
		case 0:
		{
			swprintf_s(szStr, L"일뀨뀨...");
			GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(600.f, 535.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
			swprintf_s(szStr, L"난 쓸쓸해....");
			GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(580.f, 570.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

			if (iSceneNum == 8)
			{
				if (m_pNpc != nullptr)
					m_pNpc->Set_TalkEnd();
			}
		}
		break;
		default:
			break;
		}
	}
}

void CMainQuest::MissionClearRender()
{
	_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();

	_tchar szStr[MAX_PATH] = L"";
	//m_nScriptNum = 1;
	switch (m_nScriptNum)
	{
	case 0:
	{
		swprintf_s(szStr, L"역시 링크라면 가져다 줄 수 ");
		GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(450.f, 535.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		swprintf_s(szStr, L"있다고 생각했어~ 정말 고마워!!");
		GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(430.f, 570.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
	break;
	case 1:
	{
		swprintf_s(szStr, L"별 건 아니지만 받아줘");
		GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(500.f, 535.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		swprintf_s(szStr, L"일뀨뀨~~~~");
		GET_INSTANCE(CFontMgr)->Render_Font(L"Font_Nanum", szStr, &_vec2(560.f, 570.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

		if (iSceneNum == 8)
		{
			if (m_pNpc != nullptr)
				m_pNpc->Set_TalkEnd();
		}
	}
		break;
	default:
		break;
	}
}

void CMainQuest::KeyInput()
{
	if (CKeyMgr::Get_Instance()->Key_Down(dwKEY_RETURN))
	{

		if (m_nScriptNum_Apply == 0 && m_nScriptNum == 2)
		{
			if (m_fArrowMove >= -75.f && m_fArrowMove < 0.f)
				m_nScriptNum_Apply = 2;
			else
				m_nScriptNum_Apply = 1;


			m_nScriptNum = 0;
		}
		else
			++m_nScriptNum;

		if (m_nScriptNum_Apply == 1 && m_nScriptNum == 1)
		{
			//여기서 제목 띄우고 
			//왼쪽에 퀘스트 띄운다
			if(nullptr != m_pNpc)
				dynamic_cast<CBokurin*>(m_pNpc)->Set_Disable_Collision(TRUE);

			m_bIsQuestStart = true;
			m_bIsDraw = false;
		}
		if (m_bMissionClear)
		{
			if (m_nScriptNum == 1)
			{

				//m_bIsReward = true;
				//m_bIsPlusUp = true;

				//플레이어 돈을 채우는 부분 
				CInventory_UI* pInven_UI = (CInventory_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(4, L"Layer_Inven", 0);
				if (pInven_UI == nullptr)
					return;

				_int iPrice = 50;

				m_iMoneyPre = pInven_UI->Get_iMoney();
				pInven_UI->Set_iMoney(-iPrice);
				m_iPlayerMoney = pInven_UI->Get_iMoney();
				//

			}
			//else if (m_nScriptNum == 2)
			//{
			//	m_fRewardTime = 0.f;
			//	m_bIsReward = false;
			//	m_fPlusUp = 0.f;

			//	m_fAlphaSpeed = 0.05f;
			//	m_fQuestNameAlpha = -1.f;
			//	m_fMinScale = 1.f;
			//	m_fEndAlpha = 1.f;
			//	m_bMissionClear = false;
			//	m_bIsDraw = false;
			//}
		}
		if (m_bIsReward)
		{
			if (m_nScriptNum == 2)
			{
				m_fRewardTime = 0.f;
				m_bIsReward = false;
				m_fPlusUp = 0.f;

				m_fAlphaSpeed = 0.05f;
				m_fQuestNameAlpha = -1.f;
				m_fMinScale = 1.f;
				m_fEndAlpha = 1.f;
				m_bMissionClear = false;
				m_bIsDraw = false;

				// 미션완료.
				if (nullptr != m_pNpc)
				{
					m_pNpc->Set_TalkEnd();
					dynamic_cast<CBokurin*>(m_pNpc)->Set_Disable_Collision(TRUE);
				}
				GET_INSTANCE(CManagement)->Set_ClearMainQuest();

			}
		}
	}

	if (CKeyMgr::Get_Instance()->Key_Down(dwKEY_DOWN) && m_fArrowMove >= 0.f)
	{
		GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"BotW - Sheikah Sensor Ping.flac", CSoundMgr::FIELD_DEFAULT);
		m_fArrowMove -= 75.f;
	}
	if (CKeyMgr::Get_Instance()->Key_Down(dwKEY_UP) && m_fArrowMove <= -75.f)
	{
		GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"BotW - Sheikah Sensor Ping.flac", CSoundMgr::FIELD_DEFAULT);
		m_fArrowMove += 75.f;

	}
}

CMainQuest * CMainQuest::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CMainQuest* pInstance = new CMainQuest(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CMainQuest::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CMainQuest(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMainQuest::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pTexBtn_One);
	Safe_Release(m_pTex_EnterArrow);
	Safe_Release(m_pTex_Arrow);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexName);
	Safe_Release(m_pTexMissionWin);
	Safe_Release(m_pTexComplete);
	Safe_Release(m_pTexCompleteName);
	Safe_Release(m_pTexMoneyBar);
	Safe_Release(m_pTexNum);
	Safe_Release(m_pTexReward);
	CGameObject::Free();
}
