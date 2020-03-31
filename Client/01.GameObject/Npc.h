#pragma once

#include "GameObject.h"
#include "NxTransform.h"

namespace ENGINE
{
	class CRenderer;
	class CShader;
	class CTexture;
	class CMesh_Static;
}
class CNxPlayer;
class CNpc :	public CGameObject
{

public:
	enum NPC_TYPE
	{
		NPC_BOLSON,
		NPC_KOKO,
		NPC_BOWLING,
		NPC_BOKURIN,
		NPC_END
	};
public:
	explicit CNpc(LPDIRECT3DDEVICE9 pDevice);
	explicit CNpc(const CNpc& rhs);
	virtual ~CNpc() = default;

public:
	virtual void Set_StartTalk() { m_bIsStartTalk = true; m_bTalkEnd = false; }
	virtual void Set_TalkEnd() { m_bIsStartTalk = false; m_bTalkEnd = true; }
	virtual void Set_Look() { m_bIsLook = TRUE; }

public:
	NPC_TYPE Get_NpcType() { return m_eNpcType; }
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

protected:
	_bool			m_bIsLook = FALSE;
	_bool			m_bIsStartTalk = false;
	_bool			m_bTalkEnd = false;
	NPC_TYPE		m_eNpcType = NPC_END;
	_float			m_fRotationAngle = 0.f;
	CNxPlayer*		m_pPlayer = nullptr;
public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};

