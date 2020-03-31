#include "NxCollider.h"

#include "PhysXMgr.h"
USING(Engine)

CNxCollider::CNxCollider()
	: m_eNxOption(OPTION_END)
{
}

CNxCollider::~CNxCollider()
{
}

_int CNxCollider::Update_Component(const _float & fTimeDelta)
{
	// ���� ���̳����̸� �� ������Ʈ�� �����ϴ� ��ü�� matrix�� �޾Ƽ�
	// �ݶ��̴��� worldpos�� �����Ұ�.
	
//	m_pParentMatrix->
	return _int();
}

HRESULT CNxCollider::Ready_Component_NxBox(const NX_OPTION& eActorOption, const _vec3 & vWorldPos, const _vec3 & vDimensions, const _vec3 & vAngle, const _uint iMaterialIdx, const _vec3 & vLocalPos)
{
	NxActorDesc actorDesc;
	NxBoxShapeDesc boxDesc;

	actorDesc.setToDefault();
	boxDesc.setToDefault();

	boxDesc.dimensions = NxVec3(vDimensions.x, vDimensions.y, vDimensions.z);
	boxDesc.localPose.t = NxVec3(vLocalPos.x, vLocalPos.y, vLocalPos.z);

	boxDesc.materialIndex = iMaterialIdx;

	actorDesc.shapes.pushBack(&boxDesc);

	Set_BodyDesc(&actorDesc, eActorOption);	// body �ɼǿ� ���� �ٸ��� �ֱ� ���� �Լ�.
	actorDesc.density = 0.01f;

	actorDesc.globalPose.t = NxVec3(vWorldPos.x, vWorldPos.y, vWorldPos.z);

	// ȸ���� : Ȥ���� �ٸ��� �����صα�.(�׽�Ʈ �ʿ�)
	actorDesc.globalPose.M.rotX(D3DXToRadian(vAngle.x));
	actorDesc.globalPose.M.rotY(D3DXToRadian(vAngle.y));
	actorDesc.globalPose.M.rotZ(D3DXToRadian(vAngle.z));

	// m_pNxScene�� actor�� �߰����ش� --> m_pNxScene�� ������ �����صα�.
	m_pActor = CPhysXMgr::GetInstance()->Get_NxScene()->createActor(actorDesc);
	FAILED_CHECK_RETURN(m_pActor, E_FAIL);
	
	return S_OK;
}

HRESULT CNxCollider::Ready_Component_NxSphere(const NX_OPTION& eActorOption, const _vec3 & vWorldPos, const _float & fRadius, const _vec3 & vAngle, const _uint iMaterialIdx, const _vec3 & vLocalPos)
{
	NxActorDesc actorDesc;
	NxSphereShapeDesc sphereDesc;

	actorDesc.setToDefault();
	sphereDesc.setToDefault();

	// �� �Ӽ� ���ϱ�
	sphereDesc.radius = fRadius;
	sphereDesc.localPose.t = NxVec3(vLocalPos.x, vLocalPos.y, vLocalPos.z);
	sphereDesc.materialIndex = iMaterialIdx;

	actorDesc.shapes.pushBack(&sphereDesc);		// actor�� �ִ´�.

	Set_BodyDesc(&actorDesc, eActorOption);
	actorDesc.density = 0.01f;

	actorDesc.globalPose.t = NxVec3(vWorldPos.x, vWorldPos.y, vWorldPos.z);

	// ȸ���� : Ȥ���� �ٸ��� �����صα�.(�׽�Ʈ �ʿ�)
	actorDesc.globalPose.M.rotX(D3DXToRadian(vAngle.x));
	actorDesc.globalPose.M.rotY(D3DXToRadian(vAngle.y));
	actorDesc.globalPose.M.rotZ(D3DXToRadian(vAngle.z));

	// m_pNxScene�� actor�� �߰����ش� --> m_pNxScene�� ������ �����صα�.
	m_pActor = CPhysXMgr::GetInstance()->Get_NxScene()->createActor(actorDesc);
	FAILED_CHECK_RETURN(m_pActor, E_FAIL);
	return S_OK;
}

HRESULT CNxCollider::Ready_Component_NxCapsule(const NX_OPTION & eActorOption, const _vec3 & vWorldPos, const _float & fHeight, const _float & fRadius, const _vec3 & vAngle, const _uint iMaterialIdx, const _vec3 & vLocalPos)
{
	NxActorDesc actorDesc;
	NxCapsuleShapeDesc capsDesc;

	actorDesc.setToDefault();
	capsDesc.setToDefault();

	capsDesc.height = fHeight;
	capsDesc.radius = fRadius;
	capsDesc.localPose.t = NxVec3(vLocalPos.x, vLocalPos.y, vLocalPos.z);
	capsDesc.materialIndex = iMaterialIdx;

	actorDesc.shapes.pushBack(&capsDesc);

	Set_BodyDesc(&actorDesc, eActorOption);
	actorDesc.density = 0.01f;

	actorDesc.globalPose.t = NxVec3(vWorldPos.x, vWorldPos.y, vWorldPos.z);

	// ȸ���� : Ȥ���� �ٸ��� �����صα�.(�׽�Ʈ �ʿ�)
	actorDesc.globalPose.M.rotX(D3DXToRadian(vAngle.x));
	actorDesc.globalPose.M.rotY(D3DXToRadian(vAngle.y));
	actorDesc.globalPose.M.rotZ(D3DXToRadian(vAngle.z));

	// m_pNxScene�� actor�� �߰����ش� --> m_pNxScene�� ������ �����صα�.
	m_pActor = CPhysXMgr::GetInstance()->Get_NxScene()->createActor(actorDesc);
	FAILED_CHECK_RETURN(m_pActor, E_FAIL);

	return S_OK;
}

HRESULT CNxCollider::Set_BodyDesc(NxActorDesc* pActorDesc, const NX_OPTION & eActorOption)
{
	if (nullptr == pActorDesc)
		return E_FAIL;

	m_eNxOption = eActorOption;		// �ɼ� ��������� �־��ֱ�.

	NxBodyDesc bodyDesc;
	bodyDesc.setToDefault();
	bodyDesc.angularDamping = 0.5f;

	switch (m_eNxOption)
	{
	case Engine::CNxCollider::DYNAMIC:
		pActorDesc->body = &bodyDesc;
		break;
	case Engine::CNxCollider::KINEMATIC:
		bodyDesc.flags |= NX_BF_KINEMATIC;
		pActorDesc->body = &bodyDesc;
		break;
	case Engine::CNxCollider::STATIC:
		pActorDesc->body = nullptr;
		break;
	default:
		break;
	}
	return S_OK;
}

#ifdef _DEBUG
void CNxCollider::Debug_Render_Component()
{

}
#endif
CNxCollider * CNxCollider::Create_NxBox(const NX_OPTION& eActorOption,
	const _vec3 & vWorldPos,
	const _vec3 & vDimensions,
	const _vec3 & vAngle, 
	const _uint iMaterialIdx,
	const _vec3 & vLocalPos)
{
	CNxCollider* pInstance = new CNxCollider;

	if (FAILED(pInstance->Ready_Component_NxBox(eActorOption, vWorldPos, vDimensions, vAngle, iMaterialIdx, vLocalPos)))
	{
		ERR_BOX("NxCollider_Box Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

CNxCollider * CNxCollider::Create_NxSphere(const NX_OPTION& eActorOption,
	const _vec3 & vWorldPos, 
	const _float & fRadius,
	const _vec3 & vAngle,
	const _uint iMaterialIdx,
	const _vec3 & vLocalPos)
{
	CNxCollider* pInstance = new CNxCollider;

	if (FAILED(pInstance->Ready_Component_NxSphere(eActorOption, vWorldPos, fRadius, vAngle, iMaterialIdx, vLocalPos)))
	{
		ERR_BOX("NxCollider_Sphere Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

CNxCollider * CNxCollider::Create_NxCapsule(const NX_OPTION & eActorOption,
	const _vec3 & vWorldPos, 
	const _float & fHeight, 
	const _float & fRadius, 
	const _vec3 & vAngle,
	const _uint iMaterialIdx,
	const _vec3 & vLocalPos)
{
	CNxCollider* pInstance = new CNxCollider;

	if (FAILED(pInstance->Ready_Component_NxCapsule(eActorOption, vWorldPos, fHeight, fRadius, vAngle, iMaterialIdx, vLocalPos)))
	{
		ERR_BOX("NxCollider_Capsule Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

void CNxCollider::Free()
{
}
