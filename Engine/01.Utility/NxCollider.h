#ifndef NxCollider_h__
#define NxCollider_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNxCollider : public CComponent
{
public:
	enum NX_OPTION { DYNAMIC, KINEMATIC, STATIC, OPTION_END };
private:
	explicit CNxCollider();
	virtual	~CNxCollider();

public:
	NxActor*		Get_Actor() { return m_pActor; }
private:
	HRESULT			Set_BodyDesc(NxActorDesc* pActorDesc, const NX_OPTION& eActorOption);
public:
	virtual _int	Update_Component(const _float& fTimeDelta);
private:
	HRESULT			Ready_Component_NxBox(const NX_OPTION& eActorOption,
									const _vec3& vWorldPos,
									const _vec3& vDimensions,
									const _vec3& vAngle,
									const _uint iMaterialIdx,
									const _vec3& vLocalPos);

	HRESULT			Ready_Component_NxSphere(const NX_OPTION& eActorOption, 
											const _vec3& vWorldPos,
											const _float& fRadius,
											const _vec3& vAngle,
											const _uint iMaterialIdx = 0,
											const _vec3& vLocalPos = _vec3(0.f, 0.f, 0.f));

	HRESULT			Ready_Component_NxCapsule(const NX_OPTION& eActorOption,
											const _vec3& vWorldPos,				
											const _float& fHeight,				
											const _float& fRadius,				
											const _vec3& vAngle,				
											const _uint iMaterialIdx = 0,
											const _vec3& vLocalPos = _vec3(0.f, 0.f, 0.f));

#ifdef _DEBUG
private:
	void Debug_Render_Component();
private:
	VTXCOL*			m_DebugRenderVertex;
	DWORD			m_FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
#endif
private:
	NxActor*		m_pActor;
	NX_OPTION		m_eNxOption;
	_matrix*		m_pParentMatrix;	// 포인터를 받는다.
private:
	static CNxCollider* Create_NxBox(
		const NX_OPTION& eActorOption,		// Dynamic, Kinematic, Static
		const _vec3& vWorldPos,				// World Pos
		const _vec3& vDimensions,			// Scale
		const _vec3& vAngle,				// x,y,z Angle
		const _uint iMaterialIdx = 0,		// 재질 정보 : 반발계수 , 운동마찰계수, 정지마찰계수
		const _vec3& vLocalPos = _vec3(0.f, 0.f, 0.f));	// Local Pos

	static CNxCollider* Create_NxSphere(
		const NX_OPTION& eActorOption,		// Dynamic, Kinematic, Static
		const _vec3& vWorldPos,				// World Pos
		const _float& fRadius,				// Radius
		const _vec3& vAngle,				// x,y,z Angle
		const _uint iMaterialIdx = 0,		// 재질 정보 : 반발계수 , 운동마찰계수, 정지마찰계수
		const _vec3& vLocalPos = _vec3(0.f, 0.f, 0.f)); // Local Pos

	static CNxCollider* Create_NxCapsule(
		const NX_OPTION& eActorOption,		// Dynamic, Kinematic, Static
		const _vec3& vWorldPos,				// World Pos
		const _float& fHeight,				// Height
		const _float& fRadius,				// Radius
		const _vec3& vAngle,				// x,y,z Angle
		const _uint iMaterialIdx = 0,		// 재질 정보 : 반발계수 , 운동마찰계수, 정지마찰계수
		const _vec3& vLocalPos = _vec3(0.f, 0.f, 0.f));
	// WorldPos, Size, MaterialIdx, LocalPos
private:
	virtual void Free();
};
END
#endif // NxCollider_h__
