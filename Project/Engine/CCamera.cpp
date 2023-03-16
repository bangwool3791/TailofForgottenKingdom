#include "pch.h"
#include "CCamera.h"

#include "CMesh.h"
#include "CRenderMgr.h"
#include "CMRT.h"
#include "CStructuredBuffer.h"

#include "CTimeMgr.h"

#include "CLevel.h"
#include "CLevelMgr.h"
#include "CLayer.h"
#include "CDevice.h"
#include "CTransform.h"
#include "CRenderComponent.h"

#include "CKeyMgr.h"

#include "CScript.h"
#include "CSLight.h"

#include "frustum_culling.h"

//������ A����
//������ B����
/*
* A���� ������ Run
* A���� ������ Stop
* B���� ������ Run
* B���� ������ Stop
*/
CCamera::CCamera()
	:CComponent(COMPONENT_TYPE::CAMERA)
	, m_eProjType{ PROJ_TYPE::PERSPECTIVE }
	, m_matView{}
	, m_matProj{}
	, m_fAspectRatio{ 1.f }
	, m_fFar{ 100000.f }
	, m_fScale{ 1.f }
	, m_iLayerMask(0)
	, m_iCamIdx(0)
	, m_fNear{ 0.1f }
    , m_Frustum(this)
	, m_FOV{ XM_2PI / 6.f }
{
	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();
	m_fAspectRatio = vRenderResolution.x / vRenderResolution.y;

	m_LightCS = dynamic_cast<CSLight*>(CResMgr::GetInst()->FindRes<CComputeShader>(L"CSLight").Get());

	m_pObjectRenderBuffer = new CStructuredBuffer;
	m_pObjectRenderBuffer->Create(sizeof(tObjectRender), 2, SB_TYPE::SRV_ONLY, nullptr, true);

	m_tViewPort.TopLeftX = 0;
	m_tViewPort.TopLeftY = 0;
	m_tViewPort.Width = vRenderResolution.x;
	m_tViewPort.Height = vRenderResolution.y;
	m_tViewPort.MinDepth = 0.f;
	m_tViewPort.MaxDepth = 1.f;

	m_tEnvViewPort[0].TopLeftX = 0;
	m_tEnvViewPort[0].TopLeftY = 0;
	m_tEnvViewPort[0].Width = 256;
	m_tEnvViewPort[0].Height = 256;
	m_tEnvViewPort[0].MinDepth = 0.f;
	m_tEnvViewPort[0].MaxDepth = 1.f;
}

CCamera::CCamera(const CCamera& rhs)
	: CComponent(rhs)
	, m_Frustum(this)
	//, m_pObjectRenderBuffer{ rhs .m_pObjectRenderBuffer}
	//, m_iLayerMask{ rhs.m_iLayerMask }
	//, m_iCamIdx{ rhs.m_iCamIdx }
	//, m_fScale{ rhs.m_fScale }
	//, m_fNear{ rhs.m_fNear }
	//, m_fFar{ rhs.m_fFar }
	//, m_fAspectRatio{ rhs.m_fAspectRatio }
	//, m_eProjType{ rhs.m_eProjType }
	//
{

}

CCamera::~CCamera()
{
	Safe_Delete(m_pObjectRenderBuffer);
}

void CCamera::begin()
{
	m_LightCS->SetTargetTex(CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex"));
}

void CCamera::InitializeEnvView(Vec3 vEyePos)
{
	Vec3 vEyePt = vEyePos;
	Vec3 vLookDir;
	Vec3 vUpDir;

	//+X���
	vLookDir = vEyePt + Vec3(1.f, 0.f, 0.f);
	vUpDir = Vec3(0.f, 1.f, 0.f);
	m_matCubeMapView[0] = XMMatrixLookAtLH(vEyePt, vLookDir, vUpDir);

	//-X���
	vLookDir = vEyePt + Vec3(-1.f, 0.f, 0.f);
	vUpDir = Vec3(0.f, 1.f, 0.f);
	m_matCubeMapView[1] = XMMatrixLookAtLH(vEyePt, vLookDir, vUpDir);

	//+Y���
	/*
	z�� �������� �հ� ���� ȭ���� vLookDir
	vLookDir -> y���̸� 
	vUpDir -> -z���̴�.
	*/
	vLookDir = vEyePt + Vec3(0.f, 1.f, 0.f);
	vUpDir = Vec3(0.f, 0.f, -1.f);
	m_matCubeMapView[2] = XMMatrixLookAtLH(vEyePt, vLookDir, vUpDir);

	vLookDir = vEyePt + Vec3(0.f, -1.f, 0.f);
	vUpDir = Vec3(0.f, 0.f, 1.f);
	m_matCubeMapView[3] = XMMatrixLookAtLH(vEyePt, vLookDir, vUpDir);

	//+Z���
	/*
	* https://sonagi87174.tistory.com/16
	* ���� �࿡ ���� Up���� ���
	*/
	vLookDir = vEyePt + Vec3(0.f, 0.f, 1.f);
	vUpDir = Vec3(0.f, 1.f, 0.f);
	m_matCubeMapView[4] = XMMatrixLookAtLH(vEyePt, vLookDir, vUpDir);

	//+Z���
	vLookDir = vEyePt + Vec3(0.f, 0.f, 1.f);
	vUpDir = Vec3(0.f, 1.f, 0.f);
	m_matCubeMapView[5] = XMMatrixLookAtLH(vEyePt, vLookDir, vUpDir);

	m_matCubeMapProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 1.f, 1.f, 100.f);
}

void CCamera::finaltick()
{
	CalcViewMat();

	CalcReflectMat(-1.5f);

	CalcProjMat();

	CRenderMgr::GetInst()->RegisterCamera(this);
}

void CCamera::CalcViewMat()
{
	//������� ���ϴ� ����
	//View ����� ī�޶��� �����

	Vec3	vPos = Transform()->GetRelativePos();
	Matrix	matViewTrnas = XMMatrixTranslation(-vPos.x, -vPos.y, -vPos.z);
	Vec3	vRight = Transform()->GetRelativeDir(DIR::RIGHT);
	Vec3	vUp = Transform()->GetRelativeDir(DIR::UP);
	Vec3	vLook = Transform()->GetRelativeDir(DIR::FRONT);

	Matrix matViewRot = XMMatrixIdentity();
	/*
	* ���� ����, ���� ������ ��� ��ġ ����� �����, ���� ��İ� ��ġ��� �� ������ ���� ������ ����, ���� ����� �ȴ�.
	*/
	matViewRot._11 = vRight.x;
	matViewRot._21 = vRight.y;
	matViewRot._31 = vRight.z;

	matViewRot._12 = vUp.x;
	matViewRot._22 = vUp.y;
	matViewRot._32 = vUp.z;

	matViewRot._13 = vLook.x;
	matViewRot._23 = vLook.y;
	matViewRot._33 = vLook.z;

	m_matView = matViewTrnas * matViewRot;
	XMVECTOR determinant{};
	bool IsInf = false;

	Matrix mat = XMMatrixInverse(&determinant, m_matView);

	for (int i = 0; i < 4; ++i)
	{
		if (determinant.m128_f32[i])
			IsInf = true;
	}
	
	if (IsInf)
		m_matViewInv = mat;
}

void CCamera::CalcReflectMat(float height)
{
	Vec3	vPos = Transform()->GetRelativePos();
	Matrix	matViewTrnas = XMMatrixTranslation(-vPos.x, vPos.y - (height *2.0f), -vPos.z);
	Vec3	vRight = Transform()->GetRelativeDir(DIR::RIGHT);
	Vec3	vUp = Transform()->GetRelativeDir(DIR::UP);
	Vec3	vLook = Transform()->GetRelativeDir(DIR::FRONT);

	Matrix matViewRot = XMMatrixIdentity();
	/*
	* ���� ����, ���� ������ ��� ��ġ ����� �����, ���� ��İ� ��ġ��� �� ������ ���� ������ ����, ���� ����� �ȴ�.
	*/
	matViewRot._11 = vRight.x;
	matViewRot._21 = vRight.y;
	matViewRot._31 = vRight.z;

	matViewRot._12 = vUp.x;
	matViewRot._22 = vUp.y;
	matViewRot._32 = vUp.z;

	matViewRot._13 = vLook.x;
	matViewRot._23 = vLook.y;
	matViewRot._33 = vLook.z;

	m_matReflect = matViewTrnas * matViewRot;
}


void CCamera::CalcProjMat()
{
	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();

	switch (m_eProjType)
	{
	case PERSPECTIVE:
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_fAspectRatio, m_fNear, m_fFar);
		break;
	case ORTHOGRAHPICS:
		m_matProj = XMMatrixOrthographicLH(vRenderResolution.x * m_fScale, vRenderResolution.y * m_fScale, 1.f, m_fFar);
		break;
	}

	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);
}

void CCamera::render()
{
	g_transform.matView			= m_matView;
	g_transform.matViewInv		= m_matViewInv;
	g_transform.matProj			= m_matProj;
	g_transform.matProjInv		= m_matProjInv;
	g_transform.matReflect		= m_matReflect;

	for (size_t i = 0; i < 6; ++i)
		g_transform.matEnvView[i] = m_matCubeMapView[i];

	g_transform.matEvnProj		= m_matCubeMapProj;
	/*
	* ���⼭ ��� ���� -> render transform ������Ʈ
	*/

	CONTEXT->RSSetViewports(1, &m_tViewPort);
	SortObject();
	CRenderMgr::GetInst()->ClearMRT(MRT_TYPE::DEFERRED);
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->OMSet();
	render_deferred();
	
	CRenderMgr::GetInst()->ClearMRT(MRT_TYPE::DECAL);
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DECAL)->OMSet();
	render_decal(); 
	
	CRenderMgr::GetInst()->ClearMRT(MRT_TYPE::LIGHT);
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMSet();
	
	const vector<CLight3D*>& vecLight3D = CRenderMgr::GetInst()->GetLight3D();
	
	for (size_t i = 0; i < vecLight3D.size(); ++i)
		vecLight3D[i]->render();
	
	//CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMClear();
	//m_LightCS->Excute();
 	//m_LightCS->Clear();
	
	static Ptr<CMaterial> pMergeMtrl;
	static Ptr<CMesh> pRectMesh;
	//
	CRenderMgr::GetInst()->ClearMRT(MRT_TYPE::SWAPCHAIN);
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
	pMergeMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeMtrl");
	pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	pMergeMtrl->UpdateData();
	pRectMesh->render();
	CMaterial::Clear();
	
	render_opaque();
	render_mask();
	
	render_transparent();
	render_postprocess();

	static bool bstart = false;

	if (!bstart)
	{
		CONTEXT->RSSetViewports(1, &m_tEnvViewPort[0]);
		CRenderMgr::GetInst()->ClearMRT(MRT_TYPE::ENVBOX);
		CRenderMgr::GetInst()->GetMRT(MRT_TYPE::ENVBOX)->OMSet();
		pMergeMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"EnvCubeMtrl");
		pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
		pMergeMtrl->UpdateData();
		pRectMesh->render();
		CMaterial::Clear();
		bstart = false;
	}
	
	CONTEXT->RSSetViewports(1, &m_tViewPort);
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

	//CRenderMgr::GetInst()->ClearMRT(MRT_TYPE::DEFERRED);
	//CRenderMgr::GetInst()->ClearMRT(MRT_TYPE::DECAL);
	//CRenderMgr::GetInst()->ClearMRT(MRT_TYPE::LIGHT);
}

/*
* ��� ���� �ҷ��ͼ�, ������Ʈ -> �ؽ��� ������Ʈ
* �� -> ���� -> ������Ʈ1 -> ������Ʈ
* �� -> ���� -> ������Ʈ2 -> ������Ʈ
*/

void CCamera::render_deferred()
{
	for (auto iter{ m_vecDeferred.begin() }; iter != m_vecDeferred.end(); ++iter)
	{
		(*iter)->render();
	}
}
/*
* ������ �ƴ� ���;ȿ� ������ ���� �ִ� ���� �� Shader ������ �ڵ尡 �����Ѵ�.
*/
void CCamera::render_opaque()
{
	for (auto iter{ m_vecOpaque.begin() }; iter != m_vecOpaque.end(); ++iter)
	{
		(*iter)->render();
	}

	for (auto elem{ m_mapOpaqueVec.begin() }; elem != m_mapOpaqueVec.end(); ++elem)
	{
		auto iterbegin = elem->second.begin();
		Ptr<CMesh> pMesh = (*iterbegin)->GetRenderComponent()->GetMesh();
		//�ν��Ͻ� ��ü �߿��� Mtrl ������ �ٲ� ��� ���� ó�� �ȵ�
		Ptr<CMaterial> pMtrl = (*iterbegin)->GetRenderComponent()->GetCurMaterial();

		//���͸��� ���ؼ� 
		for (auto elem2{ elem->second.begin() }; elem2 != elem->second.end(); ++elem2)
		{
			(*elem2)->GetRenderComponent()->render_Instancing();
		}
		if (!g_vecInfoObject.empty())
		{
			//���͸��� ������Ʈ
			pMtrl->UpdateData();
			m_pObjectRenderBuffer->SetData(g_vecInfoObject.data(), (UINT)g_vecInfoObject.size());
			m_pObjectRenderBuffer->UpdateData(57, PIPELINE_STAGE::VS | PIPELINE_STAGE::PS);
			if (pMesh.Get())
				pMesh->render_particle((UINT)g_vecInfoObject.size());
		}

		CMaterial::Clear();
		m_pObjectRenderBuffer->Clear();
		g_vecInfoObject.clear();
	}
}

void CCamera::render_mask()
{

	for (auto iter{ m_vecMask.begin() }; iter != m_vecMask.end(); ++iter)
	{
		(*iter)->render();
	}

	for (auto elem{ m_mapMaskVec.begin() }; elem != m_mapMaskVec.end(); ++elem)
	{
		auto iterbegin = elem->second.begin();

		Ptr<CMesh> pMesh = (*iterbegin)->GetRenderComponent()->GetMesh();
		Ptr<CMaterial> pMtrl = (*iterbegin)->GetRenderComponent()->GetCurMaterial();

		for (auto elem2{ elem->second.begin() }; elem2 != elem->second.end(); ++elem2)
		{
			(*elem2)->GetRenderComponent()->render_Instancing();
		}
		pMtrl->UpdateData();
		m_pObjectRenderBuffer->SetData(g_vecInfoObject.data(), (UINT)g_vecInfoObject.size());
		m_pObjectRenderBuffer->UpdateData(57, PIPELINE_STAGE::VS | PIPELINE_STAGE::PS);

		if (pMesh.Get())
			pMesh->render_particle((UINT)g_vecInfoObject.size());

		CMaterial::Clear();
		m_pObjectRenderBuffer->Clear();
		g_vecInfoObject.clear();
	}
}

void CCamera::render_decal()
{
	for (auto iter{ m_vecDecal.begin() }; iter != m_vecDecal.end(); ++iter)
	{
		(*iter)->render();
	}

	for (auto elem{ m_mapDecalVec.begin() }; elem != m_mapDecalVec.end(); ++elem)
	{
		auto iterbegin = elem->second.begin();

		Ptr<CMesh> pMesh = (*iterbegin)->GetRenderComponent()->GetMesh();
		Ptr<CMaterial> pMtrl = (*iterbegin)->GetRenderComponent()->GetCurMaterial();

		for (auto elem2{ elem->second.begin() }; elem2 != elem->second.end(); ++elem2)
		{
			(*elem2)->GetRenderComponent()->render_Instancing();
		}
		m_pObjectRenderBuffer->SetData(g_vecInfoObject.data(), (UINT)g_vecInfoObject.size());
		m_pObjectRenderBuffer->UpdateData(57, PIPELINE_STAGE::VS | PIPELINE_STAGE::PS);
		pMtrl->UpdateData();

		if (pMesh.Get())
			pMesh->render_particle((UINT)g_vecInfoObject.size());

		CMaterial::Clear();
		m_pObjectRenderBuffer->Clear();
		g_vecInfoObject.clear();
	}
}

void CCamera::render_transparent()
{
	for (auto iter{ m_vecTransparent.begin() }; iter != m_vecTransparent.end(); ++iter)
	{
		(*iter)->render();
	}

	for (auto elem{ m_mapTransparentVec.begin() }; elem != m_mapTransparentVec.end(); ++elem)
	{
		auto iterbegin = elem->second.begin();

		Ptr<CMesh> pMesh = (*iterbegin)->GetRenderComponent()->GetMesh();
		Ptr<CMaterial> pMtrl = (*iterbegin)->GetRenderComponent()->GetCurMaterial();

		for (auto elem2{ elem->second.begin() }; elem2 != elem->second.end(); ++elem2)
		{
			(*elem2)->GetRenderComponent()->render_Instancing();
		}
		m_pObjectRenderBuffer->SetData(g_vecInfoObject.data(), (UINT)g_vecInfoObject.size());
		m_pObjectRenderBuffer->UpdateData(57, PIPELINE_STAGE::VS | PIPELINE_STAGE::PS);
		pMtrl->UpdateData();

		if (pMesh.Get())
			pMesh->render_particle((UINT)g_vecInfoObject.size());

		CMaterial::Clear();
		m_pObjectRenderBuffer->Clear();
		g_vecInfoObject.clear();
	}
}

void CCamera::render_postprocess()
{
	for (auto iter{ m_vecPostProcess.begin() }; iter != m_vecPostProcess.end(); ++iter)
	{
		CRenderMgr::GetInst()->CopyRenderTarget();
		(*iter)->render();
	}
}

void CCamera::SetLayerMask(const wstring& _strLayerName)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	CLayer* pLayer = pCurLevel->GetLayer(_strLayerName);
	assert(pLayer);

	SetLayerMask(pLayer->GetLayerIndex());
}

void CCamera::SetLayerMask(int _iLayerIdx)
{
	if (m_iLayerMask & (1 << _iLayerIdx))
	{
		m_iLayerMask &= ~(1 << _iLayerIdx);
	}
	else
	{
		m_iLayerMask |= (1 << _iLayerIdx);
	}
}

/*
* Object Render
*/
void CCamera::SortObject()
{
	m_vecDeferred.clear();
	m_vecOpaque.clear();
	m_vecMask.clear();
	m_vecDecal.clear();
	m_vecTransparent.clear();
	Clear_VecOfMap(m_mapOpaqueVec);
	Clear_VecOfMap(m_mapMaskVec);
	Clear_VecOfMap(m_mapDecalVec);
	Clear_VecOfMap(m_mapTransparentVec);
	m_vecPostProcess.clear();

	auto pLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i{ 0 }; i < (UINT)MAX_LAYER; ++i)
	{
		if (m_iLayerMask & (1 << i))
		{
			CLayer* pLayer = pLevel->GetLayer(i);

			const vector<CGameObject*>& vecGameObject = pLayer->GetObjects();

			for (size_t j{ 0 }; j < (size_t)vecGameObject.size(); ++j)
			{
				CRenderComponent* RenderCompoent = vecGameObject[j]->GetRenderComponent();
				CTransform* pTransform = vecGameObject[j]->Transform();

				if (RenderCompoent == nullptr ||
					RenderCompoent->GetCurMaterial() == nullptr ||
					RenderCompoent->GetCurMaterial()->GetShader() == nullptr ||
					RenderCompoent->GetMesh() == nullptr)
				{
					continue;
				}

				//CFrustum* pFrustum = CLevelMgr::GetInst()->GetCurLevel()->GetLayer(0)->FindParent(L"MainCamera")->Camera()->GetFrustum();
				//
				//Vec3 vScale = vecGameObject[j]->Transform()->GetRelativeScale();
				//
				//float radius = vScale.x > vScale.y ? vScale.x : vScale.y > vScale.z ? vScale.y : vScale.z;
				//radius *= 0.5f;
				//if (!pFrustum->CheckFrustumRadius(vecGameObject[j]->Transform()->GetWorldPos(), radius))
				//{
				//	DebugDrawSphere(Vec4(1.f, 1.f, 1.f, 1.f), Vec3(pTransform->GetRelativePos()), radius);
				//	continue;
				//}

				//DebugDrawSphere(Vec4(0.f, 0.f, 1.f, 1.f), Vec3(pTransform->GetRelativePos()), radius);
				frustum_t frus;
				Vec3 vScale = vecGameObject[j]->Transform()->GetRelativeScale();
				Vec3 vRot = vecGameObject[j]->Transform()->GetRelativeRotation();
				aabb_t arrayAABB{ {-vScale.x, -vScale.y, -vScale.z},{vScale.x, vScale.y, vScale.z} };
				// Calculate Frustum
				//CCamera* pCamera = CLevelMgr::GetInst()->GetCurLevel()->GetLayer(0)->FindParent(L"MainCamera")->Camera();
				//Matrix matVP = pCamera->GetViewMat() * pCamera->GetProjMat();
				//Matrix matWorld = pCamera->GetOwner()->Transform()->GetWorldMat();
				//calculate_frustum(frus, matVP, matWorld);

				//��� �ڿ� ������ false
				//bool didHit = aabb_to_frustum(arrayAABB, frus);
				//
				//if (!didHit)
				//{
				//	DebugDrawCube(Vec4(1.f, 1.f, 1.f, 1.f), Vec3(pTransform->GetRelativePos()), vScale, vRot);
				//	continue;
				//}
				//
				DebugDrawCube(Vec4(0.f, 0.f, 1.f, 1.f), Vec3(pTransform->GetRelativePos()), vScale, vRot);
				Ptr<CGraphicsShader> GraphicsShader = RenderCompoent->GetCurMaterial()->GetShader();

				SHADER_DOMAIN eDomain = GraphicsShader->GetDomain();

				auto Type = vecGameObject[j]->GetRenderComponent()->GetInstancingType();

				switch (eDomain)
				{
				case SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQUE:
				case SHADER_DOMAIN::DOMAIN_DEFERRED_MASK:
					m_vecDeferred.push_back(vecGameObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_OPAQUE:
				{

					if (INSTANCING_TYPE::NONE == Type)
					{
						m_vecOpaque.push_back(vecGameObject[j]);
					}
					else
					{
						m_mapOpaqueVec[vecGameObject[j]->GetName()].push_back(vecGameObject[j]);
					}
				}
				break;
				case SHADER_DOMAIN::DOMAIN_MASK:
				{
					auto Type = vecGameObject[j]->GetRenderComponent()->GetInstancingType();

					if (INSTANCING_TYPE::NONE == Type)
					{
						m_vecMask.push_back(vecGameObject[j]);
					}
					else
					{
						m_mapMaskVec[vecGameObject[j]->GetName()].push_back(vecGameObject[j]);
					}
				}
				break;
				case SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL:
				case SHADER_DOMAIN::DOMAIN_DECAL:
				{
					auto Type = vecGameObject[j]->GetRenderComponent()->GetInstancingType();

					if (INSTANCING_TYPE::NONE == Type)
					{
						m_vecDecal.push_back(vecGameObject[j]);
					}
					else
					{
						m_mapDecalVec[vecGameObject[j]->GetName()].push_back(vecGameObject[j]);
					}
				}
					break;
				case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
				{
					auto Type = vecGameObject[j]->GetRenderComponent()->GetInstancingType();

					if (INSTANCING_TYPE::NONE == Type)
					{
						m_vecTransparent.push_back(vecGameObject[j]);
					}
					else
					{
						m_mapTransparentVec[vecGameObject[j]->GetName()].push_back(vecGameObject[j]);
					}
				}
				break;
				case SHADER_DOMAIN::DOMAIN_POST_PROCESS:
					m_vecPostProcess.push_back(vecGameObject[j]);
					break;
				}
			}
		}
	}
}

void CCamera::SaveToFile(FILE* _File)
{
	COMPONENT_TYPE type = GetType();
	fwrite(&type, sizeof(UINT), 1, _File);

	fwrite(&m_eProjType, sizeof(PROJ_TYPE), 1, _File);
	fwrite(&m_fAspectRatio, sizeof(float), 1, _File);
	fwrite(&m_fFar, sizeof(float), 1, _File);
	fwrite(&m_fScale, sizeof(float), 1, _File);
	fwrite(&m_iLayerMask, sizeof(UINT), 1, _File);
	fwrite(&m_iCamIdx, sizeof(int), 1, _File);
}

void CCamera::LoadFromFile(FILE* _File)
{
	fread(&m_eProjType, sizeof(PROJ_TYPE), 1, _File);
	fread(&m_fAspectRatio, sizeof(float), 1, _File);
	fread(&m_fFar, sizeof(float), 1, _File);
	fread(&m_fScale, sizeof(float), 1, _File);
	fread(&m_iLayerMask, sizeof(UINT), 1, _File);
	fread(&m_iCamIdx, sizeof(int), 1, _File);
}

Ray CCamera::CalRay()
{
	// ���콺 ������ ���ϴ� Ray ���ϱ�
	// SwapChain Ÿ���� ViewPort ����
	D3D11_VIEWPORT tVP;
	UINT num = 1;
	CONTEXT->RSGetViewports(&num, &tVP);

	////  ���� ���콺 ��ǥ
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

	//// ������ ī�޶��� ��ǥ�� �ݵ�� ������.
	m_ray.position = Transform()->GetWorldPos();

	//// view space ������ ����
	m_ray.direction = Transform()->GetRelativeDir(DIR::FRONT);

	return m_ray;
}