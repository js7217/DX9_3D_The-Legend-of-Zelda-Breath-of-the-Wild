#pragma once

#include "NxTransform.h"
#include "Quest_Koko_UI.h"
namespace ENGINE
{
	class CManagement;
	class CGameObject;
}
class CFldObj_MiniGame_LostKokko :	public CGameObject
{
public:
	explicit CFldObj_MiniGame_LostKokko(LPDIRECT3DDEVICE9 pDevice);
	explicit CFldObj_MiniGame_LostKokko(const CFldObj_MiniGame_LostKokko& rhs);
	virtual ~CFldObj_MiniGame_LostKokko() = default;

public:
	void	Set_QuestUI(CGameObject* pQuestUI) { m_pQuestUI = pQuestUI; }
	void	Set_EnterKokko() { dynamic_cast<CQuest_Koko_UI*>(m_pQuestUI)->Set_Cnt(); }
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CManagement*	m_pManagement = nullptr;
	CNxTransform*	m_pTransform = nullptr;

private:
	CGameObject*	m_pQuestUI = nullptr;
private:
	HRESULT		Add_Component();
public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();



};

