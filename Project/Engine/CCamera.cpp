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

#include "CAnimator3D.h"
#include "CInstancingBuffer.h"
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
	m_fWidth = CDevice::GetInst()->GetRenderResolution().x;
	m_fAspectRatio = CDevice::GetInst()->GetRenderResolution().x / CDevice::GetInst()->GetRenderResolution().y;

	m_LightCS = dynamic_cast<CSLight*>(CResMgr::GetInst()->FindRes<CComputeShader>(L"CSLight").Get());

	m_pObjectRenderBuffer = new CStructuredBuffer;
	m_pObjectRenderBuffer->Create(sizeof(tObjectRender), 2, SB_TYPE::SRV_ONLY, nullptr, true);

	m_tViewPort.TopLeftX = 0;
	m_tViewPort.TopLeftY = 0;
	m_tViewPort.Width = m_fWidth;
	m_tViewPort.Height = CDevice::GetInst()->GetRenderResolution().y;
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

	CalcProjMat();

	// ���콺���� ���� ���
	CalRay();

	CRenderMgr::GetInst()->RegisterCamera(this);
}

void CCamera::finaltick_module()
{
	// ����� ���
	CalcViewMat();

	// ������� ���
	CalcProjMat();

	// Frustum ����
	m_Frustum.finaltick();

	// ���콺���� ���� ���
	CalRay();
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

	if (m_eProjType == PERSPECTIVE)
	{
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_fAspectRatio, m_fNear, m_fFar);
	}
	else if (m_eProjType == ORTHOGRAHPIC)
	{
		float fHeight = m_fWidth / m_fAspectRatio;
		m_matProj = XMMatrixOrthographicLH(m_fWidth * m_fScale, fHeight * m_fScale, 1.f, m_fFar);
	}
	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);
}

void CCamera::render()
{
	g_transform.matView = m_matView;
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProj = m_matProj;
	g_transform.matProjInv = m_matProjInv;
	g_transform.matReflect = m_matReflect;

	SortObject();
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->OMSet();
	render_deferred();

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DECAL)->OMSet();
	render_decal();

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMSet();
	const vector<CLight3D*>& vecLight3D = CRenderMgr::GetInst()->GetLight3D();

	for (size_t i = 0; i < vecLight3D.size(); ++i)
		vecLight3D[i]->render();

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMClear();
	m_LightCS->Execute();
	m_LightCS->Clear();

	static Ptr<CMaterial> pMergeMtrl;
	static Ptr<CMesh> pRectMesh;

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
	pMergeMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeMtrl");
	pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	pMergeMtrl->UpdateData();
	pRectMesh->render(0);
	CMaterial::Clear();

	// Forward Rendering
	render_forward();

	render_transparent();
	render_postprocess();
}

void CCamera::EditorRender()
{
	g_transform.matView = m_matView;
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProj = m_matProj;
	g_transform.matProjInv = m_matProjInv;
	g_transform.matReflect = m_matReflect;
	/*
	* ���⼭ ��� ���� -> render transform ������Ʈ
	*/

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->OMSet();
	render_deferred();

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DECAL)->OMSet();
	render_decal();

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMSet();
	const vector<CLight3D*>& vecLight3D = CRenderMgr::GetInst()->GetLight3D();

	for (size_t i = 0; i < vecLight3D.size(); ++i)
		vecLight3D[i]->render();

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMClear();
	m_LightCS->Execute();
	m_LightCS->Clear();

	static Ptr<CMaterial> pMergeMtrl;
	static Ptr<CMesh> pRectMesh;

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
	pMergeMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeMtrl");
	pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	pMergeMtrl->UpdateData();
	pRectMesh->render(0);
	CMaterial::Clear();

	render_forward();
	render_transparent();
	render_postprocess();
}

void CCamera::render(MRT_TYPE _eType)
{
	CRenderMgr::GetInst()->ClearMRT(MRT_TYPE::DEFERRED);

	g_transform.matView = m_matView;
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProj = m_matProj;
	g_transform.matProjInv = m_matProjInv;
	g_transform.matReflect = m_matReflect;
	/*
	* ���⼭ ��� ���� -> render transform ������Ʈ
	*/
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->OMSet();
	render_deferred();
	
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DECAL)->OMSet();
	render_decal();
	
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMSet();
	
	const vector<CLight3D*>& vecLight3D = CRenderMgr::GetInst()->GetLight3D();
	
	for (size_t i = 0; i < vecLight3D.size(); ++i)
		vecLight3D[i]->render();
	
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMClear();
	m_LightCS->Execute();
	m_LightCS->Clear();
	
	
	static Ptr<CMaterial> pMergeMtrl;
	static Ptr<CMesh> pRectMesh;
	
	CRenderMgr::GetInst()->GetMRT(_eType)->OMSet();
	pMergeMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeMtrl");
	pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	pMergeMtrl->UpdateData();
	pRectMesh->render(0);
	CMaterial::Clear();
	
	render_transparent();
	render_postprocess();
	
	CRenderMgr::GetInst()->ClearMRT(MRT_TYPE::DEFERRED);
	CRenderMgr::GetInst()->ClearMRT(MRT_TYPE::DECAL);
	CRenderMgr::GetInst()->ClearMRT(MRT_TYPE::LIGHT);
}
/*
* ��� ���� �ҷ��ͼ�, ������Ʈ -> �ؽ��� ������Ʈ
* �� -> ���� -> ������Ʈ1 -> ������Ʈ
* �� -> ���� -> ������Ʈ2 -> ������Ʈ
*/

void CCamera::render_deferred()
{
	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	// Deferred object render
	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_D)
	{
		// �׷� ������Ʈ�� ���ų�, ���̴��� ���� ���
		if (pair.second.empty())
			continue;

		// instancing ���� ���� �̸��̰ų�
		// Animation2D ������Ʈ�ų�(��������Ʈ �ִϸ��̼� ������Ʈ)
		// Shader �� Instancing �� �������� �ʴ°��
		if (pair.second.size() <= 1
			|| pair.second[0].pObj->Animator2D()
			|| pair.second[0].pObj->GetRenderComponent()->GetCurMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetVSInst() == nullptr)
		{
			// �ش� ��ü���� ���� ���������� ��ȯ
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				std::unordered_map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));
				}
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		Ptr<CMesh> pMesh = pObj->GetRenderComponent()->GetMesh();
		Ptr<CMaterial> pMtrl = pObj->GetRenderComponent()->GetCurMaterial(pair.second[0].iMtrlIdx);

		// Instancing ���� Ŭ����
		CInstancingBuffer::GetInst()->Clear();

		int iRowIdx = 0;
		bool bHasAnim3D = false;
		//���� Mesh, Mtrl 
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			//������Ʈ World, WV, WVP �޾ƿ´�.
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			//�ִϸ��̼��� �ִٸ� �ִϸ��̼� ������ Compute ���̴��� ������Ʈ ��Ų��.
			if (pair.second[i].pObj->Animator3D())
			{
				pair.second[i].pObj->Animator3D()->UpdateData();
				//����� ������ �ν��Ͻ� ������ 
				tInstData.iRowIdx = iRowIdx++;
				//�� ��� ������ ��� ����ȭ ���ۿ� ���� ��, GPU ����
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
				bHasAnim3D = true;
			}
			else
				tInstData.iRowIdx = -1;

			//WVP ������ �ӽ� ���� ����
			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		// �ν��Ͻ̿� �ʿ��� �����͸� ����(SysMem -> GPU Mem)
		CInstancingBuffer::GetInst()->SetData();

		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(true); // Animation Mesh �˸���
			pMtrl->SetBoneCount(pMesh->GetBoneCount());
		}

		pMtrl->UpdateData_Inst();
		
		//Index���� ��� Idx Set
		pMesh->render_instancing(pair.second[0].iMtrlIdx);

		// ����
		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(false); // Animation Mesh �˸���
			pMtrl->SetBoneCount(0);
		}
	}

	// ���� ������
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		pair.second[0].pObj->Transform()->UpdateData();

		for (auto& instObj : pair.second)
		{
			instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx);
		}

		if (pair.second[0].pObj->Animator3D())
		{
			pair.second[0].pObj->Animator3D()->ClearData();
		}
	}
}

void CCamera::render_forward()
{
	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	// Deferred object render
	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_F)
	{
		// �׷� ������Ʈ�� ���ų�, ���̴��� ���� ���
		if (pair.second.empty())
			continue;

		// instancing ���� ���� �̸��̰ų�
		// Animation2D ������Ʈ�ų�(��������Ʈ �ִϸ��̼� ������Ʈ)
		// Shader �� Instancing �� �������� �ʴ°��
		if (pair.second.size() <= 1
			|| pair.second[0].pObj->Animator2D()
			|| pair.second[0].pObj->GetRenderComponent()->GetCurMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetVSInst() == nullptr)
		{
			// �ش� ��ü���� ���� ���������� ��ȯ
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				std::unordered_map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));
				}
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		Ptr<CMesh> pMesh = pObj->GetRenderComponent()->GetMesh();
		Ptr<CMaterial> pMtrl = pObj->GetRenderComponent()->GetCurMaterial(pair.second[0].iMtrlIdx);

		// Instancing ���� Ŭ����
		CInstancingBuffer::GetInst()->Clear();

		int iRowIdx = 0;
		bool bHasAnim3D = false;
		//���� Mesh, Mtrl 
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			//������Ʈ World, WV, WVP �޾ƿ´�.
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			//�ִϸ��̼��� �ִٸ� �ִϸ��̼� ������ Compute ���̴��� ������Ʈ ��Ų��.
			if (pair.second[i].pObj->Animator3D())
			{
				pair.second[i].pObj->Animator3D()->UpdateData();
				//����� ������ �ν��Ͻ� ������ 
				tInstData.iRowIdx = iRowIdx++;
				//�� ��� ������ ��� ����ȭ ���ۿ� ���� ��, GPU ����
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
				bHasAnim3D = true;
			}
			else
				tInstData.iRowIdx = -1;

			//WVP ������ �ӽ� ���� ����
			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		// �ν��Ͻ̿� �ʿ��� �����͸� ����(SysMem -> GPU Mem)
		CInstancingBuffer::GetInst()->SetData();

		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(true); // Animation Mesh �˸���
			pMtrl->SetBoneCount(pMesh->GetBoneCount());
		}

		pMtrl->UpdateData_Inst();
		//Matrerial ���� ��ŭ render
		pMesh->render_instancing(pair.second[0].iMtrlIdx);

		// ����
		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(false); // Animation Mesh �˸���
			pMtrl->SetBoneCount(0);
		}
	}

	// ���� ������
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		pair.second[0].pObj->Transform()->UpdateData();

		for (auto& instObj : pair.second)
		{
			instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx);
		}

		if (pair.second[0].pObj->Animator3D())
		{
			pair.second[0].pObj->Animator3D()->ClearData();
		}
	}
}
void CCamera::render_decal()
{
	for (auto iter{ m_vecDecal.begin() }; iter != m_vecDecal.end(); ++iter)
	{
		(*iter)->render();
	}
}

void CCamera::render_transparent()
{
	for (auto iter{ m_vecTransparent.begin() }; iter != m_vecTransparent.end(); ++iter)
	{
		(*iter)->render();
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

void CCamera::render_ui()
{
	for (auto iter{ m_vecUi.begin() }; iter != m_vecUi.end(); ++iter)
		(*iter)->render();
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

void CCamera::SortObject()
{
	for (auto& pair : m_mapInstGroup_D)
		pair.second.clear();
	for (auto& pair : m_mapInstGroup_F)
		pair.second.clear();

	m_vecDecal.clear();
	m_vecTransparent.clear();
	m_vecUi.clear();
	m_vecPostProcess.clear();

	auto pLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i{ 0 }; i < (UINT)MAX_LAYER; ++i)
	{
		if (m_iLayerMask & (1 << i))
		{
			CLayer* pLayer = pLevel->GetLayer(i);

			const vector<CGameObject*>& vecGameObject = pLayer->GetObjects();

			Process_Sort(vecGameObject);
		}
	}
}

void CCamera::SortObject(const vector<CGameObject*>& vecGameObject)
{
	for (auto& pair : m_mapInstGroup_D)
		pair.second.clear();
	for (auto& pair : m_mapInstGroup_F)
		pair.second.clear();

	m_vecDecal.clear();
	m_vecTransparent.clear();
	m_vecUi.clear();
	m_vecPostProcess.clear();

	Process_Sort(vecGameObject);
}

void CCamera::render_depthmap()
{
	// ���� ī�޶��� View, Proj ����
	g_transform.matView = m_matView;
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProj = m_matProj;

	for (size_t i = 0; i < m_vecDynamicShadow.size(); ++i)
	{
		if(m_vecDynamicShadow[i]->GetRenderComponent())
			m_vecDynamicShadow[i]->GetRenderComponent()->render_depthmap();
	

		const vector<CGameObject*>& vecChilds = m_vecDynamicShadow[i]->GetChilds();

		for (size_t j = 0; j < vecChilds.size(); ++j)
		{
			if(vecChilds[j]->GetRenderComponent())
				vecChilds[j]->GetRenderComponent()->render_depthmap();
		}
	}
}

void CCamera::SortShadowObject()
{
	m_vecDynamicShadow.clear();

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurLevel->GetLayer(i);
		const vector<CGameObject*>& vecObj = pLayer->GetObjects();

		for (size_t j = 0; j < vecObj.size(); ++j)
		{
			CRenderComponent* pRenderCom = vecObj[j]->GetRenderComponent();

			if (pRenderCom && pRenderCom->IsDynamicShadow())
			{
				m_vecDynamicShadow.push_back(vecObj[j]);
			}
		}
	}
}

void CCamera::Process_Sort(const vector<CGameObject*>& vecGameObject)
{
	for (size_t j{ 0 }; j < (size_t)vecGameObject.size(); ++j)
	{
		CRenderComponent* RenderCompoent = vecGameObject[j]->GetRenderComponent();
		CTransform* pTransform = vecGameObject[j]->Transform();

		if (nullptr == RenderCompoent || nullptr == RenderCompoent->GetMesh())
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

		Mtrl_Sort(RenderCompoent, vecGameObject[j]);

		const vector<CGameObject*>& Childs = vecGameObject[j]->GetChilds();

		for (size_t k{ 0 }; k < (size_t)Childs.size(); ++k)
		{
			CRenderComponent* ChildRenderCompoent = Childs[k]->GetRenderComponent();
			CTransform* pChildTransform = Childs[k]->Transform();

			if (nullptr == ChildRenderCompoent || nullptr == ChildRenderCompoent->GetMesh())
			{
				continue;
			}

			Mtrl_Sort(ChildRenderCompoent, Childs[k]);
		}
	}
}

void CCamera::Mtrl_Sort(CRenderComponent* RenderCompoent, CGameObject* pObj)
{
	UINT iMtrlCount = RenderCompoent->GetMtrlCount();
	// ���׸��� ������ŭ �ݺ�
	for (UINT iMtrl = 0; iMtrl < iMtrlCount; ++iMtrl)
	{
		//RenderComponent ���� �ε����� ���͸����� �����´�.
		if (nullptr != RenderCompoent->GetCurMaterial(iMtrl) || nullptr != RenderCompoent->GetCurMaterial(iMtrl)->GetShader())
		{
			//���� �ε����� ���̴��� �����´�.
			Ptr<CGraphicsShader> pShader = RenderCompoent->GetCurMaterial(iMtrl)->GetShader();

			SHADER_DOMAIN eDomain = pShader->GetDomain();

			switch (eDomain)
			{
			case SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQUE:
			case SHADER_DOMAIN::DOMAIN_DEFERRED_MASK:
			case SHADER_DOMAIN::DOMAIN_OPAQUE:
			case SHADER_DOMAIN::DOMAIN_MASK:
			{
				// Shader �� POV �� ���� �ν��Ͻ� �׷��� �з��Ѵ�.
				// Domain�� ���� ���� Map ������ ����
				std::unordered_map<ULONG64, vector<tInstObj>>* pMap = NULL;
				Ptr<CMaterial> pMtrl = RenderCompoent->GetCurMaterial(iMtrl);

				//�ν��Ͻ� �׷� Defferd or Forward
				if (pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQUE
					|| pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED_MASK)
				{
					//���۵� �׷�
					pMap = &m_mapInstGroup_D;
				}
				else if (pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_OPAQUE
					|| pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_MASK)
				{
					//������ �׷�
					pMap = &m_mapInstGroup_F;
				}
				else
				{
					//�׷��� ���ٸ� assert
					assert(nullptr);
					continue;
				}

				uInstID uID = {};
				//Res(CEntity) ���� �ο��Ǵ� Mesh ID, Material ID, Material Idx -> Union Get
				//vecGameObject�� ��� ��ü �� ���� Mesh, Material, Idx ����ϸ� ���� ID-> unorderd_map Push
				uID.llID = RenderCompoent->GetInstID(iMtrl);

				// ID �� 0 �� ==> Mesh �� Material �� ���õ��� �ʾҴ�.
				if (0 == uID.llID)
					continue;

				//�̹� map �ȿ� �ִ��� ã�´�.
				std::unordered_map<ULONG64, vector<tInstObj>>::iterator iter = pMap->find(uID.llID);
				if (iter == pMap->end())
				{
					//������ Insert
					pMap->insert(make_pair(uID.llID, vector<tInstObj>{tInstObj{ pObj, iMtrl }}));
				}
				else
				{
					//������ ���� ��ü�� �ִ� ���̹Ƿ� Vector PushBack
					iter->second.push_back(tInstObj{ pObj, iMtrl });
				}
			}
			break;
			//case SHADER_DOMAIN::DOMAIN_DECAL:
			case SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL:
				m_vecDecal.push_back(pObj);
				break;
			case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
				m_vecTransparent.push_back(pObj);
				break;
			case SHADER_DOMAIN::DOMAIN_POST_PROCESS:
				m_vecPostProcess.push_back(pObj);
				break;
			}
		}
	}
}
void CCamera::SortShadowObject(const vector<CGameObject*>& obj)
{
	m_vecDynamicShadow.clear();

	for (size_t j = 0; j < obj.size(); ++j)
	{
		CRenderComponent* pRenderCom = obj[j]->GetRenderComponent();

		if (pRenderCom && pRenderCom->IsDynamicShadow())
		{
			m_vecDynamicShadow.push_back(obj[j]);
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

void CCamera::CalRay()
{
	// ���콺 ������ ���ϴ� Ray ���ϱ�
	// SwapChain Ÿ���� ViewPort ����
	CMRT* pMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);
	D3D11_VIEWPORT tVP = pMRT->GetViewPort();

	//  ���� ���콺 ��ǥ
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

	// ������ ī�޶��� ��ǥ�� �ݵ�� ������.
	m_ray.vStart = Transform()->GetWorldPos();

	// view space ������ ����
	m_ray.vDir.x = ((((vMousePos.x - tVP.TopLeftX) * 2.f / tVP.Width) - 1.f) - m_matProj._31) / m_matProj._11;
	m_ray.vDir.y = (-(((vMousePos.y - tVP.TopLeftY) * 2.f / tVP.Height) - 1.f) - m_matProj._32) / m_matProj._22;
	m_ray.vDir.z = 1.f;

	// world space ������ ����
	m_ray.vDir = XMVector3TransformNormal(m_ray.vDir, m_matViewInv);
	m_ray.vDir.Normalize();
}