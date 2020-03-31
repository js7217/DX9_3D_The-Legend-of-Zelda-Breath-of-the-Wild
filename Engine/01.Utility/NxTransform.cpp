#include "NxTransform.h"
#include "GameObject.h"

CNxTransform::CNxTransform(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
{
	m_isClone = FALSE;
}

CNxTransform::CNxTransform(CNxTransform & rhs)
	: CComponent(rhs)
{
	m_isClone = TRUE;
	m_bBomb = FALSE;
	for (int i = 0; i < INFO_END; ++i)
		m_vInfo[i] = _vec3(0.f, 0.f, 0.f);

	m_vScale = { 1.f, 1.f, 1.f };
	m_vAngle = { 0.f, 0.f, 0.f };



	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matWeaponWorld);
	D3DXMatrixIdentity(&m_matNRotWorld);

	m_NxWorld.isIdentity();
}

HRESULT CNxTransform::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName)
{
	if (nullptr == pEffect)
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix(pConstantName, &m_matWorld)))
		return E_FAIL;
	return S_OK;
}

_float CNxTransform::Calc_TargetDistance(CNxTransform * pTrans)
{
	_vec3 vPos = *pTrans->Get_Info(INFO_POS) - *Get_Info(INFO_POS);

	return D3DXVec3Length(&vPos);
}

_vec3 CNxTransform::Calc_TargetDirection(CNxTransform * pTrans)
{
	_vec3 vNewDir = *pTrans->Get_Info(INFO_POS) - *Get_Info(INFO_POS);
	vNewDir.y = 0.f;

	return *D3DXVec3Normalize(&vNewDir, &vNewDir);
}

_vec3 CNxTransform::Calc_CameraPos(_float fCamDist, _vec3 vTargetPos, _float fHeigth)
{
	_vec3 vDir = m_vInfo[INFO_LOOK] * -1.f;
	D3DXVec3Normalize(&vDir, &vDir);
	vDir.y = 0.f;
	_vec3 vNewPos = vTargetPos + (vDir * (fCamDist - fHeigth));

	return vNewPos;
}

_vec3 CNxTransform::Calc_CameraDir(_float fCamDist, _vec3 vTargetPos)
{
	_vec3 vNewDir = vTargetPos + m_vInfo[INFO_POS];
	D3DXVec3Normalize(&vNewDir, &vNewDir);
	vNewDir.y = 0.f;

	return vNewDir;
}

void CNxTransform::Move_TargetPos(const _vec3 * pTargetPos, const _float & fSpeed, const _float & fTimeDelta)
{
	_vec3		vDir = *pTargetPos - m_vInfo[INFO_POS];
	m_vInfo[INFO_POS] += *D3DXVec3Normalize(&vDir, &vDir) * fSpeed * fTimeDelta;
}

HRESULT CNxTransform::Ready_Component(void)
{

	return S_OK;
}

HRESULT CNxTransform::Ready_Status(void * pArg)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matNRotWorld);

	memcpy(&m_tState, pArg, sizeof(COLL_STATE));

#ifdef _DEBUG
	if (m_tState.eCollider != COL_TRIMESH)
		m_tState.NxFlag |= NX_SF_VISUALIZATION;
#endif // !_DEBUG


	switch (m_tState.eCollider)
	{
	case COL_SPHERE:
		Create_Sphere_Coll();
		break;
	case COL_BOX:
		Create_Box_Coll();
		break;
	case COL_CAPSULE:
		Create_Capsule_Coll();
		break;
	case COL_TRIMESH:
		Create_TriangleMesh();
		break;
	default:
		return E_FAIL;
	}


	return S_OK;
}

_int CNxTransform::Update_Component(const _double & TimeDelta)
{
	//Physx World
	if (m_pActor == nullptr)
		return 0;

	Limit_Velocity();

	m_NxWorld = m_pActor->getGlobalPose();
	memcpy(m_vInfo[INFO_POS], &m_NxWorld.t, sizeof(NxVec3));
	m_NxWorld.getColumnMajor44(m_matWorld.m);

	//////////////////////////////////////

	for (_ulong i = 0; i < INFO_POS; ++i)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));

	// 크기 변환 적용 중
	for (_ulong i = 0; i < INFO_POS; ++i)
	{
		D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]);
		m_vInfo[i] *= *(((_float*)&m_vScale) + i);
	}

	for (_ulong i = 0; i < INFO_END; ++i)
		memcpy(&m_matNRotWorld.m[i][0], &m_vInfo[i], sizeof(_vec3));

	//////////////////////////////////////

	////NxMatWorld Scaling
	//NxVec3 NxvRight, NxvUp, NxvLook;

	//NxvRight = m_NxWorld.M.getColumn(0);
	//NxvRight.normalize();
	//NxvRight *= m_vScale.x;
	//m_NxWorld.M.setColumn(0, NxvRight);

	//NxvUp = m_NxWorld.M.getColumn(1);
	//NxvUp.normalize();
	//NxvUp *= m_vScale.y;
	//m_NxWorld.M.setColumn(1, NxvUp);

	//NxvLook = m_NxWorld.M.getColumn(2);
	//NxvLook.normalize();
	//NxvLook *= m_vScale.z;
	//m_NxWorld.M.setColumn(2, NxvLook);

	//for (int i = 0; i < 3; ++i)
	//	memcpy(m_vInfo[i], &m_NxWorld.M.getColumn(i), sizeof(NxVec3));
	////////////////////////////////////////////

	//NxMatWorld Rotation
	_matrix			matRot[ROT_END];

	D3DXMatrixRotationX(&matRot[ROT_X], m_vAngle.x);
	D3DXMatrixRotationY(&matRot[ROT_Y], m_vAngle.y);
	D3DXMatrixRotationZ(&matRot[ROT_Z], m_vAngle.z);

	for (_ulong i = 0; i < INFO_POS; ++i)
	{
		for (_ulong j = 0; j < ROT_END; ++j)
			D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
	}

	//m_NxWorld.M.setColumn(0, (NxVec3)m_vInfo[INFO_RIGHT]);
	//m_NxWorld.M.setColumn(1, (NxVec3)m_vInfo[INFO_UP]);
	//m_NxWorld.M.setColumn(2, (NxVec3)m_vInfo[INFO_LOOK]);
	////////////////////////////////////////////

	//NxMatWorld Translation	//Translation은 Set_NxPos_Direction 함수로 적용
	//m_pActorMesh->setGlobalPosition((NxVec3)m_vInfo[INFO_POS]);
	////////////////////////////////////////////

	//Copy NxMatWorld to Transform MatWorld
	//for (int i = 0; i < 3; ++i)
	//	memcpy(m_matWorld.m[i], &m_NxWorld.M.getColumn(i), sizeof(NxVec3));

	//memcpy(m_matWorld.m[INFO_POS], &m_NxWorld.t, sizeof(NxVec3));
	////////////////////////////////////////////

	// 위치 변환 적용 중
	for (_ulong i = 0; i < INFO_END; ++i)
		memcpy(&m_matWorld.m[i][0], &m_vInfo[i], sizeof(_vec3));

	m_NxWorld.setColumnMajor44(m_matWorld.m);

	return 0;
}

_int CNxTransform::LateUpdate_Component(const _double & TimeDelta)
{
	//if (D3DXToDegree(m_vAngle.x) > 360.f || D3DXToDegree(m_vAngle.x) < -360.f)
	//	m_vAngle.x = 0.f;
	//if (D3DXToDegree(m_vAngle.y) > 360.f || D3DXToDegree(m_vAngle.y) < -360.f)
	//	m_vAngle.y = 0.f;
	//if (D3DXToDegree(m_vAngle.z) > 360.f || D3DXToDegree(m_vAngle.z) < -360.f)
	//	m_vAngle.z = 0.f;

	//NxVec3 nxPos = m_pActor->getGlobalPosition();
	//memcpy(m_matWorld.m[INFO_POS], &nxPos, sizeof(_vec3));

	return 0;
}

HRESULT CNxTransform::Set_BodyDesc(NxActorDesc * pActorDesc)
{
	if (nullptr == pActorDesc)
		return E_FAIL;

	NxBodyDesc bodyDesc;
	bodyDesc.setToDefault();
	bodyDesc.flags = 0;
	bodyDesc.angularDamping = 0.5f;

	switch (m_tState.eCollOpt)
	{
	case OPT_DYNAMIC:
		pActorDesc->body = &bodyDesc;
		break;
	case OPT_KINEMATIC:
		bodyDesc.flags |= NX_BF_KINEMATIC;
		pActorDesc->body = &bodyDesc;
		break;
	case OPT_STATIC:
		pActorDesc->body = nullptr;
		break;
	}

	return S_OK;
}

HRESULT CNxTransform::Create_Capsule_Coll()
{
	NxActorDesc actorDesc;
	NxCapsuleShapeDesc capsDesc;

	actorDesc.setToDefault();
	capsDesc.setToDefault();

	capsDesc.height = m_tState.fHeight;		//콜라이더 높이(캡슐 허리)
	capsDesc.radius = m_tState.fRadius;		//콜라이더 반경(캡슐 위 아래 둥근 부분)
	capsDesc.localPose.t = (NxVec3)m_tState.vLocalPos;		//로컬포지션(콜라이더 위치 조정)
	capsDesc.materialIndex = m_tState.iMaterial;
	capsDesc.shapeFlags = m_tState.NxFlag;
	capsDesc.skinWidth = (NxReal)0.075;
	//	capsDesc.mass = NxReal(1000);

	//	capsDesc.flags |= NX_SWEPT_SHAPE;

	if (m_tState.cName != nullptr)
		actorDesc.name = m_tState.cName;
	else
		actorDesc.name = "";

	actorDesc.shapes.pushBack(&capsDesc);
	//Set_BodyDesc(&actorDesc);
	if (m_tState.fDensity == 0.f)
		actorDesc.density = 1.f;
	else
		actorDesc.density = m_tState.fDensity;

	//콜라이더 최초 생성 위치 == 객체의 최초 생성 위치
	actorDesc.globalPose.t = (NxVec3)m_tState.vStartPos;

	NxBodyDesc bodyDesc;
	bodyDesc.setToDefault();
	bodyDesc.flags = 0;
	//	bodyDesc.angularDamping = 0.5f;
	//	bodyDesc.linearDamping = 0.5f;

	switch (m_tState.eCollOpt)
	{
	case OPT_DYNAMIC:
		actorDesc.body = &bodyDesc;
		break;
	case OPT_KINEMATIC:
		bodyDesc.flags |= NX_BF_KINEMATIC;
		actorDesc.body = &bodyDesc;
		break;
	case OPT_STATIC:
		actorDesc.body = nullptr;
		break;
	default:
		actorDesc.body = &bodyDesc;
		break;
	}

	if (m_tState.pGameObject != nullptr)
		actorDesc.userData = m_tState.pGameObject;

	NxScene* pScene = GET_INSTANCE(CPhysXMgr)->Get_NxScene();
	//Physx Device에 물리적용 시켜달라고 등록OI
	m_pActor = pScene->createActor(actorDesc);
	NULL_CHECK_RETURN(m_pActor, E_FAIL);

	//_tchar pName[256] = { 0, };
	//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_tState.cName, (_int)strlen(m_tState.cName), pName, 256);
	//GET_INSTANCE(CPhysXMgr)->Insert_MapActor(pName, m_pActor);

	//Physx World
	m_NxWorld = m_pActor->getGlobalPose();

	NxMat33 NxmatRot[ROT_END];

	NxmatRot[ROT_X].rotX(NxMath::degToRad(m_tState.vAngle.x));
	NxmatRot[ROT_Y].rotY(NxMath::degToRad(m_tState.vAngle.y));
	NxmatRot[ROT_Z].rotZ(NxMath::degToRad(m_tState.vAngle.z));

	m_NxWorld.M = NxmatRot[ROT_X] * NxmatRot[ROT_Y] * NxmatRot[ROT_Z];

	m_pActor->setGlobalPose(m_NxWorld);

	for (int i = 0; i < 3; ++i)
		memcpy(m_vInfo[i], &m_NxWorld.M.getColumn(i), sizeof(NxVec3));

	memcpy(m_vInfo[INFO_POS], &m_NxWorld.t, sizeof(NxVec3));


	//Create_SubShape_Trigger_Box(NxVec3(m_tState.vLocalPos.x, m_tState.vLocalPos.y + 1.7f, m_tState.vLocalPos.z));

	return S_OK;
}

HRESULT CNxTransform::Create_Box_Coll()
{
	NxActorDesc actorDesc;
	NxBoxShapeDesc boxDesc;

	actorDesc.setToDefault();
	boxDesc.setToDefault();

	boxDesc.dimensions = (NxVec3)m_tState.vDimensions;
	boxDesc.localPose.t = (NxVec3)m_tState.vLocalPos;
	boxDesc.materialIndex = m_tState.iMaterial;
	boxDesc.skinWidth = (NxReal)0.025f;
	boxDesc.shapeFlags = m_tState.NxFlag;

	if (m_tState.cName != nullptr)
		actorDesc.name = m_tState.cName;
	else
		actorDesc.name = "";

	NxBodyDesc bodyDesc;
	bodyDesc.setToDefault();
	bodyDesc.flags = 0;
	bodyDesc.angularDamping = 0.5f;

	switch (m_tState.eCollOpt)
	{
	case OPT_DYNAMIC:
		bodyDesc.flags = 0;
		actorDesc.body = &bodyDesc;
		break;
	case OPT_KINEMATIC:
		bodyDesc.flags |= NX_BF_KINEMATIC;
		actorDesc.body = &bodyDesc;
		break;
	case OPT_STATIC:
		bodyDesc.flags = 0;
		actorDesc.body = nullptr;
		break;
	default:
		actorDesc.body = &bodyDesc;
		break;
	}

	if (m_tState.pGameObject != nullptr)
		actorDesc.userData = m_tState.pGameObject;

	actorDesc.shapes.pushBack(&boxDesc);
	if (m_tState.fDensity == 0.f)
		actorDesc.density = 10.f;
	else
		actorDesc.density = m_tState.fDensity;

	//콜라이더 최초 생성 위치 == 객체의 최초 생성 위치
	actorDesc.globalPose.t = (NxVec3)m_tState.vStartPos;

	NxScene* pScene = GET_INSTANCE(CPhysXMgr)->Get_NxScene(m_tState.iType);
	//Physx Device에 물리적용 시켜달라고 등록
	m_pActor = pScene->createActor(actorDesc);
	NULL_CHECK_RETURN(m_pActor, E_FAIL);

	//Physx World
	m_NxWorld = m_pActor->getGlobalPose();

	NxMat33 NxmatRot[ROT_END];

	NxmatRot[ROT_X].rotX(NxMath::degToRad(m_tState.vAngle.x));
	NxmatRot[ROT_Y].rotY(NxMath::degToRad(m_tState.vAngle.y));
	NxmatRot[ROT_Z].rotZ(NxMath::degToRad(m_tState.vAngle.z));

	m_NxWorld.M = NxmatRot[ROT_X] * NxmatRot[ROT_Y] * NxmatRot[ROT_Z];

	m_pActor->setGlobalPose(m_NxWorld);

	for (int i = 0; i < 3; ++i)
		memcpy(m_vInfo[i], &m_NxWorld.M.getColumn(i), sizeof(NxVec3));

	memcpy(m_vInfo[INFO_POS], &m_NxWorld.t, sizeof(NxVec3));

	return S_OK;
}

HRESULT CNxTransform::Create_Sphere_Coll()
{
	NxActorDesc actorDesc;
	NxSphereShapeDesc sphereDesc;

	sphereDesc.radius = m_tState.fRadius;
	sphereDesc.localPose.t = (NxVec3)m_tState.vLocalPos;
	sphereDesc.materialIndex = m_tState.iMaterial;
	actorDesc.shapes.pushBack(&sphereDesc);
	sphereDesc.shapeFlags = m_tState.NxFlag;

	if (m_tState.cName != nullptr)
		actorDesc.name = m_tState.cName;
	else
		actorDesc.name = "";

	NxBodyDesc bodyDesc;

	bodyDesc.setToDefault();
	bodyDesc.flags = 0;
	bodyDesc.angularDamping = 0.5f;
	if (0.f == m_tState.fDensity)
		actorDesc.density = 0.5f;
	else
		actorDesc.density = m_tState.fDensity;

	bodyDesc.linearVelocity = (NxVec3)m_tState.vVelocity;

	switch (m_tState.eCollOpt)
	{
	case OPT_DYNAMIC:
		bodyDesc.flags = 0;
		actorDesc.body = &bodyDesc;
		break;
	case OPT_KINEMATIC:
		bodyDesc.flags |= NX_BF_KINEMATIC;
		actorDesc.body = &bodyDesc;
		break;
	case OPT_STATIC:
		actorDesc.body = nullptr;
		break;
	default:
		actorDesc.body = &bodyDesc;
		break;
	}

	if (m_tState.pGameObject != nullptr)
		actorDesc.userData = m_tState.pGameObject;

	//콜라이더 최초 생성 위치 == 객체의 최초 생성 위치
	actorDesc.globalPose.t = (NxVec3)m_tState.vStartPos;

	NxScene* pScene = GET_INSTANCE(CPhysXMgr)->Get_NxScene(m_tState.iType);
	//Physx Device에 물리적용 시켜달라고 등록
	m_pActor = pScene->createActor(actorDesc);
	NULL_CHECK_RETURN(m_pActor, E_FAIL);

	//Physx World
	m_NxWorld = m_pActor->getGlobalPose();

	NxMat33 NxmatRot[ROT_END];

	NxmatRot[ROT_X].rotX(NxMath::degToRad(m_tState.vAngle.x));
	NxmatRot[ROT_Y].rotY(NxMath::degToRad(m_tState.vAngle.y));
	NxmatRot[ROT_Z].rotZ(NxMath::degToRad(m_tState.vAngle.z));

	m_NxWorld.M = NxmatRot[ROT_X] * NxmatRot[ROT_Y] * NxmatRot[ROT_Z];

	m_pActor->setGlobalPose(m_NxWorld);

	m_NxWorld.getColumnMajor44(m_matWorld.m);

	//for (int i = 0; i < 3; ++i)
	//	memcpy(m_vInfo[i], &m_NxWorld.M.getColumn(i), sizeof(NxVec3));

	//memcpy(m_vInfo[INFO_POS], &m_NxWorld.t, sizeof(NxVec3));

	return S_OK;
}

HRESULT CNxTransform::Create_TriangleMesh()
{
	//// 트라이앵글 메쉬 만들기.
	NxActorDesc actorDesc;
	actorDesc.setToDefault();
	NxTriangleMeshShapeDesc shapeDesc = Create_NxMeshDesc();
	shapeDesc.name = m_tState.cName;
	shapeDesc.shapeFlags = m_tState.NxFlag;

	shapeDesc.localPose.t = (NxVec3)m_tState.vLocalPos;
	shapeDesc.skinWidth = (NxReal)0.025;
	actorDesc.shapes.pushBack(&shapeDesc);

	if (m_tState.cName != nullptr)
		actorDesc.name = m_tState.cName;
	else
		actorDesc.name = "";

	//동적 충돌판정용 모델 설정
	NxBodyDesc bodyDesc;
	bodyDesc.setToDefault();

	bodyDesc.angularDamping = 0.5f;

	switch (m_tState.eCollOpt)
	{
	case OPT_DYNAMIC:
		bodyDesc.flags = 0;
		actorDesc.body = &bodyDesc;
		break;
	case OPT_KINEMATIC:
		bodyDesc.flags |= NX_BF_KINEMATIC;
		actorDesc.body = &bodyDesc;
		break;
	case OPT_STATIC:
		actorDesc.body = nullptr;
		break;
	default:
		actorDesc.body = nullptr;
		break;
	}

	if (m_tState.pGameObject != nullptr)
		actorDesc.userData = m_tState.pGameObject;

	if (0.f == m_tState.fDensity)
		actorDesc.density = 1.f;
	else
		actorDesc.density = m_tState.fDensity;

	actorDesc.globalPose.t = (NxVec3)m_tState.vStartPos;

	if (m_tState.pGameObject != nullptr)
		actorDesc.userData = m_tState.pGameObject;

	NxScene* pScene = GET_INSTANCE(CPhysXMgr)->Get_NxScene();
	//Physx Device에 물리적용 시켜달라고 등록
	m_pActor = pScene->createActor(actorDesc);
	NULL_CHECK_RETURN(m_pActor, E_FAIL);

	//Physx World
	m_NxWorld = m_pActor->getGlobalPose();

	NxMat33 NxmatRot[ROT_END];

	NxmatRot[ROT_X].rotX(NxMath::degToRad(m_tState.vAngle.x));
	NxmatRot[ROT_Y].rotY(NxMath::degToRad(m_tState.vAngle.y));
	NxmatRot[ROT_Z].rotZ(NxMath::degToRad(m_tState.vAngle.z));

	m_NxWorld.M = NxmatRot[ROT_X] * NxmatRot[ROT_Y] * NxmatRot[ROT_Z];

	m_pActor->setGlobalPose(m_NxWorld);

	for (int i = 0; i < 3; ++i)
		memcpy(m_vInfo[i], &m_NxWorld.M.getColumn(i), sizeof(NxVec3));

	memcpy(m_vInfo[INFO_POS], &m_NxWorld.t, sizeof(NxVec3));

	return S_OK;
}

NxActor* CNxTransform::CreateConvexMesh()
{

	NxConvexMeshDesc* convexDesc = NULL;
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	LPD3DXMESH pTempMesh = NULL;
	LPDIRECT3DDEVICE9 pGraphicDev = NULL;
	D3DVECTOR* pVec = NULL;
	LPBYTE pBits = NULL;

	m_tState.pMesh->GetDevice(&pGraphicDev);

	m_tState.pMesh->CloneMeshFVF(m_tState.pMesh->GetOptions(), D3DFVF_XYZ, pGraphicDev, &pTempMesh);
	Safe_Release(pGraphicDev);

	if (!convexDesc)
	{
		convexDesc = new NxConvexMeshDesc();
		assert(convexDesc);
	}
	convexDesc->numVertices = pTempMesh->GetNumVertices();
	convexDesc->pointStrideBytes = sizeof(NxVec3);
	convexDesc->flags = NX_CF_COMPUTE_CONVEX;

	pVec = new D3DVECTOR[convexDesc->numVertices];

	// 버텍스 버퍼를 참조해서 버텍스 좌표를 얻고,  NxTriangleMeshDesc에 저장한다.
	pTempMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pBits);
	CopyMemory(pVec, pBits, convexDesc->pointStrideBytes *convexDesc->numVertices);
	convexDesc->points = pVec;
	pTempMesh->UnlockVertexBuffer();

	Safe_Release(pTempMesh);

	NxConvexShapeDesc convexShapeDesc;
	convexShapeDesc.localPose.t = NxVec3(0, 0, 0);
	convexShapeDesc.userData = convexDesc;

	NxInitCooking();
	MemoryWriteBuffer buf;
	bool status = NxCookConvexMesh(*convexDesc, buf);

	NxConvexMesh* pMesh = GET_INSTANCE(CPhysXMgr)->Get_NxPhysicsSDK()->createConvexMesh(MemoryReadBuffer(buf.data));
	assert(pMesh);
	convexShapeDesc.meshData = pMesh;
	NxCloseCooking();

	if (pMesh)
	{
		//assert를 통과했으므로 항상 여기 들어와야함.
		pMesh->saveToDesc(*convexDesc);
		assert(convexShapeDesc.isValid());
		actorDesc.shapes.push_back(&convexShapeDesc);
		actorDesc.body = nullptr;
		actorDesc.name = m_tState.cName;

		/*actorDesc.body = &bodyDesc;
		actorDesc.density = 1.0f;*/

		actorDesc.globalPose.t = NxVec3(0.0f, 0.0f, 0.0f);
		assert(actorDesc.isValid());
		NxActor* actor = GET_INSTANCE(CPhysXMgr)->Get_NxScene()->createActor(actorDesc);
		assert(actor);
		return actor;
	}
	return NULL;
}

NxTriangleMeshShapeDesc CNxTransform::Create_NxMeshDesc()
{
	//트라이앵글 매쉬를 만들기위한 준비
	LPDIRECT3DDEVICE9 pGraphicDev = NULL;
	LPD3DXMESH pTempMesh = NULL;
	NxTriangleMeshDesc meshDesc;
	D3DVECTOR* pVec = NULL;
	LPBYTE pIndex = NULL;
	LPBYTE pBits = NULL;
	NxTriangleMeshShapeDesc meshShapeDesc;
	MemoryWriteBuffer buf;
	NxTriangleMesh* pTriangleMesh = NULL;

	NxU32 flg = 0;

	if (nullptr == m_tState.pMesh)			// 메쉬 로드 실패시.
	{
		Safe_Release(pTempMesh);
		Safe_Delete_Array(pVec);
		Safe_Delete_Array(pIndex);

		return meshShapeDesc;
	}

	m_tState.pMesh->GetDevice(&pGraphicDev);
	//  버텍스 좌표정보만 복사해서 재생성한다.
	m_tState.pMesh->CloneMeshFVF(m_tState.pMesh->GetOptions(), D3DFVF_XYZ, pGraphicDev, &pTempMesh);
	Safe_Release(pGraphicDev);

	//if (pTempMesh == nullptr)
	//	return meshShapeDesc;

	// 인덱스 버퍼로부터 인덱스 사이즈를 얻어온다.
	LPDIRECT3DINDEXBUFFER9 pIndexBuffer = NULL;
	pTempMesh->GetIndexBuffer(&pIndexBuffer);
	D3DINDEXBUFFER_DESC desc;
	pIndexBuffer->GetDesc(&desc);
	Safe_Release(pIndexBuffer);

	switch (desc.Format)
	{
	case D3DFMT_INDEX16:
		flg |= NX_CF_16_BIT_INDICES;
	}

	//인덱스 정보를 세팅
	meshDesc.flags = flg;
	// 버텍스 수
	meshDesc.numVertices = pTempMesh->GetNumVertices();
	// 버텍스 사이즈
	meshDesc.pointStrideBytes = pTempMesh->GetNumBytesPerVertex();
	// 삼각형 폴리곤 수
	meshDesc.numTriangles = pTempMesh->GetNumFaces();


	if (flg & NX_CF_16_BIT_INDICES)
		meshDesc.triangleStrideBytes = 3 * sizeof(WORD);
	else
		meshDesc.triangleStrideBytes = 3 * sizeof(DWORD);

	pVec = new D3DVECTOR[meshDesc.numVertices];

	// 버텍스 버퍼를 참조해서 버텍스 좌표를 얻고,  NxTriangleMeshDesc에 저장한다.
	pTempMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pBits);
	CopyMemory(pVec, pBits, meshDesc.pointStrideBytes * meshDesc.numVertices);
	meshDesc.points = pVec;
	pTempMesh->UnlockVertexBuffer();

	pIndex = new BYTE[meshDesc.triangleStrideBytes * meshDesc.numTriangles];

	// 인덱스 버퍼를 참조해서 버텍스 좌표를 얻고 NxTriangleMeshDesc에 저장한다.
	pTempMesh->LockIndexBuffer(D3DLOCK_READONLY, (void**)&pBits);
	CopyMemory(pIndex, pBits, meshDesc.triangleStrideBytes * meshDesc.numTriangles);
	meshDesc.triangles = pIndex;
	pTempMesh->UnlockIndexBuffer();

	//Cooking 초기화
	NxInitCooking();
	NxCookingParams params;
	params.targetPlatform = PLATFORM_PC;
	params.skinWidth = (NxReal)0.025;
	params.hintCollisionSpeed = false;
	NxSetCookingParams(params);

	//NxTriangleMeshDesc 클래스 오브젝트를 직렬화 한다.
	if (NxCookTriangleMesh(meshDesc, buf) == false)
	{
		Safe_Release(pTempMesh);
		Safe_Delete_Array(pVec);
		Safe_Delete_Array(pIndex);

		return meshShapeDesc;
	}

	pTriangleMesh = GET_INSTANCE(CPhysXMgr)->Get_NxPhysicsSDK()->createTriangleMesh(MemoryReadBuffer(buf.data));
	if (nullptr == pTriangleMesh)
	{
		Safe_Release(pTempMesh);
		Safe_Delete_Array(pVec);
		Safe_Delete_Array(pIndex);

		return meshShapeDesc;
	}

	meshShapeDesc.meshData = pTriangleMesh;
	meshShapeDesc.shapeFlags = NX_SF_FEATURE_INDICES;
	meshShapeDesc.skinWidth = (NxReal)0.025;
	meshShapeDesc.meshFlags = NX_MESH_SMOOTH_SPHERE_COLLISIONS | NX_MESH_DOUBLE_SIDED;

	NxCloseCooking();

	Safe_Release(pTempMesh);
	Safe_Delete_Array(pVec);
	Safe_Delete_Array(pIndex);

	return meshShapeDesc;
}

void CNxTransform::Limit_Velocity()
{
	NxVec3 NxVelocity = m_pActor->getLinearVelocity();

	//물리작용으로 인한 속도제한
	if (NxVelocity.x > m_fLimit)
		NxVelocity.x = m_fLimit;
	else if (NxVelocity.x < -m_fLimit)
		NxVelocity.x = -m_fLimit;
	if (NxVelocity.y > m_fLimit)
		NxVelocity.y = m_fLimit;
	//else if (NxVelocity.y < -m_fLimit)	//추락속도
	//	NxVelocity.y = -m_fLimit;
	if (NxVelocity.z > m_fLimit)
		NxVelocity.z = m_fLimit;
	else if (NxVelocity.z < -m_fLimit)
		NxVelocity.z = -m_fLimit;

	m_pActor->setLinearVelocity(NxVelocity);
	//m_pActor->getGlobalPose().t.distanceSquared(NxVec3(0.f, 1.f, 0.f));
}

HRESULT CNxTransform::Create_BombActor()
{
	NxActorDesc actorDesc;
	NxSphereShapeDesc sphereDesc;

	sphereDesc.radius = 4.f;
	sphereDesc.localPose.t = NxVec3(0.f, 0.f, 0.f);
	sphereDesc.materialIndex = m_tState.iMaterial;
	sphereDesc.shapeFlags |= NX_TRIGGER_ENABLE;

	actorDesc.shapes.pushBack(&sphereDesc);

	actorDesc.name = "KaBomb";

	NxBodyDesc bodyDesc;
	bodyDesc.setToDefault();
	bodyDesc.flags |= NX_BF_FROZEN;
	bodyDesc.angularDamping = 0.5f;
	actorDesc.density = 10.f;
	actorDesc.body = nullptr;

	//콜라이더 최초 생성 위치 == 객체의 최초 생성 위치
	NxVec3 NxPos = m_pActor->getGlobalPosition();
	actorDesc.globalPose.t = NxPos;

	if (m_tState.pGameObject != nullptr)
		actorDesc.userData = m_tState.pGameObject;

	NxScene* pScene = GET_INSTANCE(CPhysXMgr)->Get_NxScene();
	//Physx Device에 물리적용 시켜달라고 등록
	m_pSubActor[0] = pScene->createActor(actorDesc);
	NULL_CHECK_RETURN(m_pSubActor, E_FAIL);

	return S_OK;
}

HRESULT CNxTransform::Create_LadderActor(_float fHeight)
{
	NxActorDesc actorDesc;
	NxBoxShapeDesc boxDesc;

	actorDesc.setToDefault();
	boxDesc.setToDefault();

	NxVec3 nxDimension = m_tState.vDimensions;
	nxDimension.x += 0.8f;
	nxDimension.y += 0.5f;
	nxDimension.z += 0.8f;

	boxDesc.dimensions = nxDimension;
	boxDesc.localPose.t = (NxVec3)m_tState.vLocalPos;
	boxDesc.materialIndex = m_tState.iMaterial;
	boxDesc.skinWidth = 0.0f;
	boxDesc.shapeFlags = m_tState.NxFlag;
	boxDesc.shapeFlags |= NX_TRIGGER_ENABLE;

	actorDesc.name = "IceCube_Ladder";
	actorDesc.body = nullptr;

	if (m_tState.pGameObject != nullptr)
		actorDesc.userData = m_tState.pGameObject;

	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.density = 0.01f;

	//콜라이더 최초 생성 위치 == 객체의 최초 생성 위치
	NxVec3 nxStartPos = (NxVec3)m_tState.vStartPos;
	nxStartPos.y += fHeight;
	actorDesc.globalPose.t = nxStartPos;

	NxScene* pScene = GET_INSTANCE(CPhysXMgr)->Get_NxScene();
	//Physx Device에 물리적용 시켜달라고 등록
	m_pSubActor[0] = pScene->createActor(actorDesc);
	NULL_CHECK_RETURN(m_pSubActor, E_FAIL);

	//Physx World
	m_NxWorld = m_pSubActor[0]->getGlobalPose();

	NxMat33 NxmatRot[ROT_END];

	NxmatRot[ROT_X].rotX(NxMath::degToRad(m_tState.vAngle.x));
	NxmatRot[ROT_Y].rotY(NxMath::degToRad(m_tState.vAngle.y));
	NxmatRot[ROT_Z].rotZ(NxMath::degToRad(m_tState.vAngle.z));

	m_NxWorld.M = NxmatRot[ROT_X] * NxmatRot[ROT_Y] * NxmatRot[ROT_Z];

	m_pSubActor[0]->setGlobalPose(m_NxWorld);

	return S_OK;
}

HRESULT CNxTransform::Create_LadderActor_Up()
{
	NxActorDesc actorDesc;
	NxBoxShapeDesc boxDesc;

	actorDesc.setToDefault();
	boxDesc.setToDefault();

	NxVec3 nxDimension = { 1.f, 1.f, 1.5f };

	boxDesc.dimensions = nxDimension;
	boxDesc.localPose.t = NxVec3(0.f, 0.5f, 0.f);
	boxDesc.materialIndex = m_tState.iMaterial;
	boxDesc.shapeFlags |= NX_TRIGGER_ENABLE;

	actorDesc.name = "Ladder_Up";
	actorDesc.body = nullptr;

	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.density = 0.1f;

	//콜라이더 최초 생성 위치 == 객체의 최초 생성 위치
	NxVec3 nxStartPos = (NxVec3)m_tState.vStartPos;
	nxStartPos.y += m_tState.vDimensions.y;
	nxStartPos.y -= m_tState.vVelocity.x;
	actorDesc.globalPose.t = nxStartPos;

	NxScene* pScene = GET_INSTANCE(CPhysXMgr)->Get_NxScene();
	//Physx Device에 물리적용 시켜달라고 등록
	m_pSubActor[0] = pScene->createActor(actorDesc);
	NULL_CHECK_RETURN(m_pSubActor[0], E_FAIL);

	//Physx World
	NxMat34 NxWorld;
	NxWorld = m_pSubActor[0]->getGlobalPose();

	NxMat33 NxmatRot[ROT_END];

	NxmatRot[ROT_X].rotX(NxMath::degToRad(m_tState.vAngle.x));
	NxmatRot[ROT_Y].rotY(NxMath::degToRad(m_tState.vAngle.y));
	NxmatRot[ROT_Z].rotZ(NxMath::degToRad(m_tState.vAngle.z));

	NxWorld.M = NxmatRot[ROT_X] * NxmatRot[ROT_Y] * NxmatRot[ROT_Z];

	m_pSubActor[0]->setGlobalPose(NxWorld);

	return S_OK;
}

HRESULT CNxTransform::Create_LadderActor_Down()
{
	NxActorDesc actorDesc;
	NxBoxShapeDesc boxDesc;

	actorDesc.setToDefault();
	boxDesc.setToDefault();

	NxVec3 nxDimension = { 1.f, 1.f, 1.f };

	boxDesc.dimensions = nxDimension;
	boxDesc.localPose.t = NxVec3(0.f, 0.f, 0.f);
	boxDesc.materialIndex = m_tState.iMaterial;
	boxDesc.shapeFlags |= NX_TRIGGER_ENABLE;

	actorDesc.name = "Ladder_Down";
	actorDesc.body = nullptr;

	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.density = 0.1f;

	//콜라이더 최초 생성 위치 == 객체의 최초 생성 위치
	NxVec3 nxStartPos = (NxVec3)m_tState.vStartPos;
	nxStartPos.y -= m_tState.vDimensions.y;
	nxStartPos.y += m_tState.vVelocity.y;
	actorDesc.globalPose.t = nxStartPos;

	NxScene* pScene = GET_INSTANCE(CPhysXMgr)->Get_NxScene();
	//Physx Device에 물리적용 시켜달라고 등록
	m_pSubActor[1] = pScene->createActor(actorDesc);
	NULL_CHECK_RETURN(m_pSubActor[1], E_FAIL);

	//Physx World
	NxMat34 NxWorld;
	NxWorld = m_pSubActor[1]->getGlobalPose();

	NxMat33 NxmatRot[ROT_END];

	NxmatRot[ROT_X].rotX(NxMath::degToRad(m_tState.vAngle.x));
	NxmatRot[ROT_Y].rotY(NxMath::degToRad(m_tState.vAngle.y));
	NxmatRot[ROT_Z].rotZ(NxMath::degToRad(m_tState.vAngle.z));

	NxWorld.M = NxmatRot[ROT_X] * NxmatRot[ROT_Y] * NxmatRot[ROT_Z];

	m_pSubActor[1]->setGlobalPose(NxWorld);

	return S_OK;
}

HRESULT CNxTransform::Create_Door_Right()
{
	NxActorDesc actorDesc;
	NxBoxShapeDesc boxDesc;

	actorDesc.setToDefault();
	boxDesc.setToDefault();

	boxDesc.dimensions = (NxVec3)m_tState.vDimensions;
	boxDesc.localPose.t = (NxVec3)m_tState.vVelocity;
	boxDesc.materialIndex = m_tState.iMaterial;
	boxDesc.skinWidth = 0.1f;
	boxDesc.shapeFlags = m_tState.NxFlag;


	actorDesc.name = "IronDoorR";

	NxBodyDesc bodyDesc;
	bodyDesc.setToDefault();
	bodyDesc.flags = 0;
	bodyDesc.angularDamping = 0.5f;

	bodyDesc.flags = 0;
	actorDesc.body = &bodyDesc;

	if (m_tState.pGameObject != nullptr)
		actorDesc.userData = m_tState.pGameObject;

	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.density = 10.f;

	//콜라이더 최초 생성 위치 == 객체의 최초 생성 위치
	actorDesc.globalPose.t = (NxVec3)m_tState.vStartPos;

	NxScene* pScene = GET_INSTANCE(CPhysXMgr)->Get_NxScene();
	//Physx Device에 물리적용 시켜달라고 등록
	m_pSubActor[0] = pScene->createActor(actorDesc);
	NULL_CHECK_RETURN(m_pSubActor[0], E_FAIL);

	//Physx World
	m_NxWorld = m_pActor->getGlobalPose();

	NxMat33 NxmatRot[ROT_END];

	NxmatRot[ROT_X].rotX(NxMath::degToRad(m_tState.vAngle.x));
	NxmatRot[ROT_Y].rotY(NxMath::degToRad(m_tState.vAngle.y));
	NxmatRot[ROT_Z].rotZ(NxMath::degToRad(m_tState.vAngle.z));

	m_NxWorld.M = NxmatRot[ROT_X] * NxmatRot[ROT_Y] * NxmatRot[ROT_Z];

	m_pSubActor[0]->setGlobalPose(m_NxWorld);

	//for (int i = 0; i < 3; ++i)
	//	memcpy(m_vInfo[i], &m_NxWorld.M.getColumn(i), sizeof(NxVec3));

	//memcpy(m_vInfo[INFO_POS], &m_NxWorld.t, sizeof(NxVec3));

	return S_OK;
}

HRESULT CNxTransform::Create_Door_Left()
{
	NxActorDesc actorDesc;
	NxBoxShapeDesc boxDesc;

	actorDesc.setToDefault();
	boxDesc.setToDefault();

	boxDesc.dimensions = (NxVec3)m_tState.vDimensions;
	boxDesc.localPose.t = (NxVec3)m_tState.vVelocity;
	boxDesc.materialIndex = m_tState.iMaterial;
	boxDesc.skinWidth = 0.1f;
	boxDesc.shapeFlags = m_tState.NxFlag;


	actorDesc.name = "IronDoorL";

	NxBodyDesc bodyDesc;
	bodyDesc.setToDefault();
	bodyDesc.flags = 0;
	bodyDesc.angularDamping = 0.5f;

	bodyDesc.flags = 0;
	actorDesc.body = &bodyDesc;

	if (m_tState.pGameObject != nullptr)
		actorDesc.userData = m_tState.pGameObject;

	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.density = 10.f;

	//콜라이더 최초 생성 위치 == 객체의 최초 생성 위치
	actorDesc.globalPose.t = (NxVec3)m_tState.vStartPos;

	NxScene* pScene = GET_INSTANCE(CPhysXMgr)->Get_NxScene();
	//Physx Device에 물리적용 시켜달라고 등록
	m_pSubActor[0] = pScene->createActor(actorDesc);
	NULL_CHECK_RETURN(m_pSubActor[0], E_FAIL);

	//Physx World
	m_NxWorld = m_pActor->getGlobalPose();

	NxMat33 NxmatRot[ROT_END];

	NxmatRot[ROT_X].rotX(NxMath::degToRad(m_tState.vAngle.x));
	NxmatRot[ROT_Y].rotY(NxMath::degToRad(m_tState.vAngle.y));
	NxmatRot[ROT_Z].rotZ(NxMath::degToRad(m_tState.vAngle.z));

	m_NxWorld.M = NxmatRot[ROT_X] * NxmatRot[ROT_Y] * NxmatRot[ROT_Z];

	m_pSubActor[0]->setGlobalPose(m_NxWorld);

	//for (int i = 0; i < 3; ++i)
	//	memcpy(m_vInfo[i], &m_NxWorld.M.getColumn(i), sizeof(NxVec3));

	//memcpy(m_vInfo[INFO_POS], &m_NxWorld.t, sizeof(NxVec3));

	return S_OK;
}

HRESULT CNxTransform::Create_GrabActor()
{
	NxActorDesc actorDesc;
	NxSphereShapeDesc sphereDesc;

	sphereDesc.radius = 0.75f;
	sphereDesc.localPose.t = NxVec3(0.f, 0.5f, 0.f);
	sphereDesc.materialIndex = m_tState.iMaterial;
	sphereDesc.shapeFlags |= NX_TRIGGER_ENABLE | NX_SF_DISABLE_RAYCASTING;

	actorDesc.shapes.pushBack(&sphereDesc);

	actorDesc.name = "Player_Grab";

	NxBodyDesc bodyDesc;
	bodyDesc.setToDefault();
	actorDesc.density = 10.f;
	actorDesc.body = nullptr;

	//콜라이더 최초 생성 위치 == 객체의 최초 생성 위치
	NxVec3 NxPos = m_pActor->getGlobalPosition();
	actorDesc.globalPose.t = NxPos;

	if (m_tState.pGameObject != nullptr)
		actorDesc.userData = m_tState.pGameObject;

	NxScene* pScene = GET_INSTANCE(CPhysXMgr)->Get_NxScene();
	//Physx Device에 물리적용 시켜달라고 등록
	m_pSubActor[0] = pScene->createActor(actorDesc);
	NULL_CHECK_RETURN(m_pSubActor[0], E_FAIL);

	return S_OK;
}

HRESULT CNxTransform::Create_CameraActor()
{
	NxActorDesc actorDesc;
	NxSphereShapeDesc sphereDesc;

	sphereDesc.radius = 1.f;
	sphereDesc.localPose.t = NxVec3(0.f, 0.f, 0.f);
	sphereDesc.materialIndex = m_tState.iMaterial;
	sphereDesc.shapeFlags = NX_TRIGGER_ENABLE | NX_SF_DISABLE_RAYCASTING;

	actorDesc.shapes.pushBack(&sphereDesc);

	actorDesc.name = "Camera_Eye";

	NxBodyDesc bodyDesc;
	bodyDesc.setToDefault();
	actorDesc.density = 1.f;
	actorDesc.body = nullptr;

	//콜라이더 최초 생성 위치 == 객체의 최초 생성 위치
	NxVec3 NxPos = m_pActor->getGlobalPosition();
	actorDesc.globalPose.t = NxPos;

	if (m_tState.pGameObject != nullptr)
		actorDesc.userData = m_tState.pGameObject;

	NxScene* pScene = GET_INSTANCE(CPhysXMgr)->Get_NxScene();
	//Physx Device에 물리적용 시켜달라고 등록
	m_pSubActor[0] = pScene->createActor(actorDesc);
	NULL_CHECK_RETURN(m_pSubActor[0], E_FAIL);

	return S_OK;
}

HRESULT CNxTransform::Create_DgnEntranceTrigger()
{


	return S_OK;
}

HRESULT CNxTransform::Create_SubActor(NxVec3 vGlobalPos, _uint iNum)
{
	NxActorDesc actorDesc;
	NxSphereShapeDesc sphereDesc;
	sphereDesc.setToDefault();

	sphereDesc.radius = 1.5f;
	sphereDesc.localPose.t = (NxVec3)m_tState.vLocalPos;
	sphereDesc.shapeFlags |= NX_TRIGGER_ENABLE;

	actorDesc.shapes.pushBack(&sphereDesc);
	actorDesc.name = "Create_SeekerSkill";

	NxBodyDesc bodyDesc;
	bodyDesc.setToDefault();
	actorDesc.body = nullptr;
	actorDesc.globalPose.t = vGlobalPos;

	if (m_tState.pGameObject != nullptr)
		actorDesc.userData = m_tState.pGameObject;

	NxScene* pScene = GET_INSTANCE(CPhysXMgr)->Get_NxScene();
	//Physx Device에 물리적용 시켜달라고 등록
	m_pSubActor[iNum] = pScene->createActor(actorDesc);
	NULL_CHECK_RETURN(m_pSubActor[iNum], E_FAIL);

	return S_OK;
}

HRESULT CNxTransform::Create_SubShape_Trigger_Box(NxVec3 vLocalPos)
{
	NxBoxShapeDesc BoxDesc;
	//	NxSphereShapeDesc sphereDesc;
	BoxDesc.setToDefault();
	BoxDesc.dimensions = NxVec3(0.5f, 0.5f, 0.5f);
	BoxDesc.skinWidth = 0.01f;
	BoxDesc.localPose.t = vLocalPos;

	BoxDesc.name = "Player_Sub";
	BoxDesc.shapeFlags |= NX_SF_DISABLE_RAYCASTING;
	BoxDesc.group = 1;	// 다른 그룹.


	m_pActor->createShape(BoxDesc);


	return S_OK;
}

HRESULT CNxTransform::Create_SubShape_Trigger_Sphere(NxVec3 vLocalPos)
{
	//NxSphereShapeDesc SphereDesc;
	//SphereDesc.setToDefault();
	//SphereDesc.radius = .25f;
	//SphereDesc.localPose.t = vLocalPos;
	//SphereDesc.name = "Shield_Sub";
	//SphereDesc.group = 1;

	NxSphereShapeDesc SphereDesc;
	SphereDesc.setToDefault();
	SphereDesc.shapeFlags |= NX_TRIGGER_ENABLE /*| NX_SF_DISABLE_COLLISION*/;
	SphereDesc.radius = .6f;
	SphereDesc.localPose.t = vLocalPos;
	SphereDesc.name = "Weapon_Shield";

	m_pActor->createShape(SphereDesc);


	return S_OK;
}

HRESULT CNxTransform::Create_SubShape_Trigger_ClimbSphere(NxVec3 vLocalPos)
{
	NxSphereShapeDesc SphereDesc;
	SphereDesc.setToDefault();
	
	SphereDesc.shapeFlags |= NX_TRIGGER_ENABLE | NX_SF_DISABLE_RAYCASTING;
	SphereDesc.radius = 1.4f;
	SphereDesc.localPose.t = vLocalPos;
	SphereDesc.name = "ClimbCol";

	m_pActor->createShape(SphereDesc);

	return S_OK;
}

void CNxTransform::Grab_Advanced(CNxTransform * pTrans, _float fAngle, _float fHeight, _bool bCheck)
{
	m_pActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
	NxMat34 NxMat = pTrans->m_NxWorld;
	NxQuat nxQy;
	NxQuat nxQx;
	nxQy.fromAngleAxis(D3DXToDegree(pTrans->m_vAngle.y), NxVec3(0, 1, 0));
	nxQx.fromAngleAxis(fAngle, NxVec3(1, 0, 0));
	NxMat.M.fromQuat(nxQy * nxQx);

	NxVec3 NxPos = NxMat.t + (NxMat.M.getColumn(2) * (0.85f + fHeight));
	NxPos.y += 1.f;

	NxMat34 pMatX, pMatY, pMatZ;
	pMatX.M.rotX(D3DXToRadian(m_tState.vAngle.x));

	if (!bCheck)
	{
		pMatY.M.rotY(pTrans->m_vAngle.y);
		pMatZ.M.rotZ(D3DXToRadian(0.f));
	}
	else
	{
		pMatY.M.rotY(D3DXToRadian(0.f));
		pMatZ.M.rotZ(pTrans->m_vAngle.y);
	}

	NxMat34 NxWorld = pMatX * pMatY * pMatZ;

	NxWorld.t = NxPos;
	m_pActor->setGlobalPose(NxWorld);
}

void CNxTransform::Grab_Item(CNxTransform * pTrans, _float fHeight)
{
	NxVec3 vTarget = (NxVec3)*pTrans->Get_Info(INFO_POS) + (NxVec3)*pTrans->Get_Info(INFO_LOOK) * 0.15f;

	NxMat34 pMatX, pMatY, pMatZ;

	pMatX.M.rotX(D3DXToRadian(m_tState.vAngle.x));
	pMatY.M.rotY(D3DXToRadian(0.f));
	pMatZ.M.rotZ(pTrans->m_vAngle.y);

	NxMat34 NxWorld = pMatX * pMatY * pMatZ;

	//pMat.M.rotY(pTrans->m_vAngle.y);
	vTarget.y += fHeight;
	NxWorld.t = vTarget;
	m_pActor->setGlobalPose(NxWorld);
}

void CNxTransform::Grab_Chicken(CNxTransform * pTrans, _float fAngle, _float fHeight)
{
	//NxVec3 vTarget = (NxVec3)*pTrans->Get_Info(INFO_POS) + (NxVec3)*pTrans->Get_Info(INFO_LOOK) * 0.15f;

	//NxMat34 pMatX, pMatY, pMatZ;

	//pMatX.M.rotX(D3DXToRadian(0.f));
	//pMatY.M.rotY(pTrans->m_vAngle.y);
	//pMatZ.M.rotZ(D3DXToRadian(0.f));

	//NxMat34 NxWorld = pMatX * pMatY * pMatZ;

	////pMat.M.rotY(pTrans->m_vAngle.y);
	//vTarget.y += fHeight;
	//NxWorld.t = vTarget;
	//m_pActor->setGlobalPose(NxWorld);

	///////////////////////////////

	m_pActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
	NxMat34 NxMat = pTrans->m_NxWorld;
	NxQuat nxQy;
	NxQuat nxQx;
	nxQy.fromAngleAxis(D3DXToDegree(pTrans->m_vAngle.y), NxVec3(0, 1, 0));
	nxQx.fromAngleAxis(fAngle, NxVec3(1, 0, 0));
	NxMat.M.fromQuat(nxQy * nxQx);

	NxVec3 NxPos = NxMat.t + (NxMat.M.getColumn(2) * (0.85f + fHeight));
	NxPos.y += 1.f;

	NxMat34 pMatX, pMatY, pMatZ;
	pMatX.M.rotX(D3DXToRadian(m_tState.vAngle.x));


	pMatY.M.rotY(pTrans->m_vAngle.y);
	pMatZ.M.rotZ(D3DXToRadian(0.f));

	NxMat34 NxWorld = pMatX * pMatY * pMatZ;

	NxWorld.t = NxPos;
	m_pActor->setGlobalPose(NxWorld);
}

void CNxTransform::NxRotation_Y(CNxTransform * pTrans)
{
	NxVec3 vPos = m_pActor->getGlobalPosition();

	_float fAngleY = pTrans->m_vAngle.y;

	NxMat34 matRotX;
	matRotX.M.rotX(m_tState.vAngle.x);
	NxMat34 matRotY;
	matRotY.M.rotY(fAngleY);

	NxMat34 matWorld;
	matWorld = matRotY;
	matWorld.t = vPos;
	m_pActor->setGlobalPose(matWorld);

	if (m_pSubActor[0] != nullptr)
		m_pSubActor[0]->setGlobalPose(matWorld);
	if (m_pSubActor[1] != nullptr)
		m_pSubActor[1]->setGlobalPose(matWorld);
}

CNxTransform * CNxTransform::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CNxTransform* pInstance = new CNxTransform(pDevice);

	if (FAILED(pInstance->Ready_Component()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CNxTransform::Clone_Component(void * pArg)
{
	CNxTransform* pInstance = new CNxTransform(*this);

	if (FAILED(pInstance->Ready_Status(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CNxTransform::Free()
{
	if (m_isClone)
	{
		_int iShapeCnt = m_pActor->getNbShapes();
		for (_int i = 0; i < iShapeCnt; ++i)
		{
			NxShape* shape = m_pActor->getShapes()[i];
			m_pActor->releaseShape(*shape);
		}

		GET_INSTANCE(CPhysXMgr)->Get_NxScene()->releaseActor(*m_pActor);
		for (_uint i = 0; i < 2; ++i)
			GET_INSTANCE(CPhysXMgr)->Get_NxScene()->releaseActor(*m_pSubActor[i]);
	}

	CComponent::Free();
}
