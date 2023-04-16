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
//스레드 A종류
//스레드 B종류
/*
* A종류 스레드 Run
* A종류 스레드 Stop
* B종류 스레드 Run
* B종류 스레드 Stop
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

	//+X축면
	vLookDir = vEyePt + Vec3(1.f, 0.f, 0.f);
	vUpDir = Vec3(0.f, 1.f, 0.f);
	m_matCubeMapView[0] = XMMatrixLookAtLH(vEyePt, vLookDir, vUpDir);

	//-X축면
	vLookDir = vEyePt + Vec3(-1.f, 0.f, 0.f);
	vUpDir = Vec3(0.f, 1.f, 0.f);
	m_matCubeMapView[1] = XMMatrixLookAtLH(vEyePt, vLookDir, vUpDir);

	//+Y축면
	/*
	z축 안쪽으로 뚫고 들어가는 화면이 vLookDir
	vLookDir -> y축이면 
	vUpDir -> -z축이다.
	*/
	vLookDir = vEyePt + Vec3(0.f, 1.f, 0.f);
	vUpDir = Vec3(0.f, 0.f, -1.f);
	m_matCubeMapView[2] = XMMatrixLookAtLH(vEyePt, vLookDir, vUpDir);

	vLookDir = vEyePt + Vec3(0.f, -1.f, 0.f);
	vUpDir = Vec3(0.f, 0.f, 1.f);
	m_matCubeMapView[3] = XMMatrixLookAtLH(vEyePt, vLookDir, vUpDir);

	//+Z축면
	/*
	* https://sonagi87174.tistory.com/16
	* 기준 축에 따른 Up벡터 계산
	*/
	vLookDir = vEyePt + Vec3(0.f, 0.f, 1.f);
	vUpDir = Vec3(0.f, 1.f, 0.f);
	m_matCubeMapView[4] = XMMatrixLookAtLH(vEyePt, vLookDir, vUpDir);

	//+Z축면
	vLookDir = vEyePt + Vec3(0.f, 0.f, 1.f);
	vUpDir = Vec3(0.f, 1.f, 0.f);
	m_matCubeMapView[5] = XMMatrixLookAtLH(vEyePt, vLookDir, vUpDir);

	m_matCubeMapProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 1.f, 1.f, 100.f);
}

void CCamera::finaltick()
{
	CalcViewMat();

	CalcProjMat();

	// 마우스방향 직선 계산
	CalRay();

	CRenderMgr::GetInst()->RegisterCamera(this);
}

void CCamera::finaltick_module()
{
	// 뷰행렬 계산
	CalcViewMat();

	// 투영행렬 계산
	CalcProjMat();

	// Frustum 구성
	m_Frustum.finaltick();

	// 마우스방향 직선 계산
	CalRay();
}

void CCamera::CalcViewMat()
{
	//역행렬을 구하는 과정
	//View 행렬은 카메라의 역행렬

	Vec3	vPos = Transform()->GetRelativePos();
	Matrix	matViewTrnas = XMMatrixTranslation(-vPos.x, -vPos.y, -vPos.z);
	Vec3	vRight = Transform()->GetRelativeDir(DIR::RIGHT);
	Vec3	vUp = Transform()->GetRelativeDir(DIR::UP);
	Vec3	vLook = Transform()->GetRelativeDir(DIR::FRONT);

	Matrix matViewRot = XMMatrixIdentity();
	/*
	* 직교 벡터, 방향 벡터의 경우 전치 행렬을 만들면, 원본 행렬과 전치행렬 각 성분의 곱이 내적에 따라, 단위 행렬이 된다.
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
	* 직교 벡터, 방향 벡터의 경우 전치 행렬을 만들면, 원본 행렬과 전치행렬 각 성분의 곱이 내적에 따라, 단위 행렬이 된다.
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
	* 여기서 행렬 세팅 -> render transform 업데이트
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
	* 여기서 행렬 세팅 -> render transform 업데이트
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
* 상수 버퍼 불러와서, 오브젝트 -> 텍스쳐 업데이트
* 맵 -> 벡터 -> 오브젝트1 -> 업데이트
* 맵 -> 벡터 -> 오브젝트2 -> 업데이트
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
		// 그룹 오브젝트가 없거나, 쉐이더가 없는 경우
		if (pair.second.empty())
			continue;

		// instancing 개수 조건 미만이거나
		// Animation2D 오브젝트거나(스프라이트 애니메이션 오브젝트)
		// Shader 가 Instancing 을 지원하지 않는경우
		if (pair.second.size() <= 1
			|| pair.second[0].pObj->Animator2D()
			|| pair.second[0].pObj->GetRenderComponent()->GetCurMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetVSInst() == nullptr)
		{
			// 해당 물체들은 단일 랜더링으로 전환
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

		// Instancing 버퍼 클리어
		CInstancingBuffer::GetInst()->Clear();

		int iRowIdx = 0;
		bool bHasAnim3D = false;
		//동일 Mesh, Mtrl 
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			//오브젝트 World, WV, WVP 받아온다.
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			//애니메이션이 있다면 애니메이션 정보도 Compute 셰이더로 업데이트 시킨다.
			if (pair.second[i].pObj->Animator3D())
			{
				pair.second[i].pObj->Animator3D()->UpdateData();
				//결과로 전달할 인스턴싱 데이터 
				tInstData.iRowIdx = iRowIdx++;
				//뼈 행렬 정보도 결과 구조화 버퍼에 취합 후, GPU 전달
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
				bHasAnim3D = true;
			}
			else
				tInstData.iRowIdx = -1;

			//WVP 데이터 임시 버퍼 저장
			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		// 인스턴싱에 필요한 데이터를 세팅(SysMem -> GPU Mem)
		CInstancingBuffer::GetInst()->SetData();

		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(true); // Animation Mesh 알리기
			pMtrl->SetBoneCount(pMesh->GetBoneCount());
		}

		pMtrl->UpdateData_Inst();
		
		//Index에서 사용 Idx Set
		pMesh->render_instancing(pair.second[0].iMtrlIdx);

		// 정리
		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(false); // Animation Mesh 알리기
			pMtrl->SetBoneCount(0);
		}
	}

	// 개별 랜더링
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
		// 그룹 오브젝트가 없거나, 쉐이더가 없는 경우
		if (pair.second.empty())
			continue;

		// instancing 개수 조건 미만이거나
		// Animation2D 오브젝트거나(스프라이트 애니메이션 오브젝트)
		// Shader 가 Instancing 을 지원하지 않는경우
		if (pair.second.size() <= 1
			|| pair.second[0].pObj->Animator2D()
			|| pair.second[0].pObj->GetRenderComponent()->GetCurMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetVSInst() == nullptr)
		{
			// 해당 물체들은 단일 랜더링으로 전환
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

		// Instancing 버퍼 클리어
		CInstancingBuffer::GetInst()->Clear();

		int iRowIdx = 0;
		bool bHasAnim3D = false;
		//동일 Mesh, Mtrl 
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			//오브젝트 World, WV, WVP 받아온다.
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			//애니메이션이 있다면 애니메이션 정보도 Compute 셰이더로 업데이트 시킨다.
			if (pair.second[i].pObj->Animator3D())
			{
				pair.second[i].pObj->Animator3D()->UpdateData();
				//결과로 전달할 인스턴싱 데이터 
				tInstData.iRowIdx = iRowIdx++;
				//뼈 행렬 정보도 결과 구조화 버퍼에 취합 후, GPU 전달
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
				bHasAnim3D = true;
			}
			else
				tInstData.iRowIdx = -1;

			//WVP 데이터 임시 버퍼 저장
			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		// 인스턴싱에 필요한 데이터를 세팅(SysMem -> GPU Mem)
		CInstancingBuffer::GetInst()->SetData();

		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(true); // Animation Mesh 알리기
			pMtrl->SetBoneCount(pMesh->GetBoneCount());
		}

		pMtrl->UpdateData_Inst();
		//Matrerial 개수 만큼 render
		pMesh->render_instancing(pair.second[0].iMtrlIdx);

		// 정리
		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(false); // Animation Mesh 알리기
			pMtrl->SetBoneCount(0);
		}
	}

	// 개별 랜더링
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
	// 광원 카메라의 View, Proj 세팅
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

		//평면 뒤에 있으면 false
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
	// 메테리얼 개수만큼 반복
	for (UINT iMtrl = 0; iMtrl < iMtrlCount; ++iMtrl)
	{
		//RenderComponent 현재 인덱스의 머터리얼을 가져온다.
		if (nullptr != RenderCompoent->GetCurMaterial(iMtrl) || nullptr != RenderCompoent->GetCurMaterial(iMtrl)->GetShader())
		{
			//현재 인덱스의 셰이더를 가져온다.
			Ptr<CGraphicsShader> pShader = RenderCompoent->GetCurMaterial(iMtrl)->GetShader();

			SHADER_DOMAIN eDomain = pShader->GetDomain();

			switch (eDomain)
			{
			case SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQUE:
			case SHADER_DOMAIN::DOMAIN_DEFERRED_MASK:
			case SHADER_DOMAIN::DOMAIN_OPAQUE:
			case SHADER_DOMAIN::DOMAIN_MASK:
			{
				// Shader 의 POV 에 따라서 인스턴싱 그룹을 분류한다.
				// Domain에 따라 받을 Map 포인터 선언
				std::unordered_map<ULONG64, vector<tInstObj>>* pMap = NULL;
				Ptr<CMaterial> pMtrl = RenderCompoent->GetCurMaterial(iMtrl);

				//인스턴싱 그룹 Defferd or Forward
				if (pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQUE
					|| pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED_MASK)
				{
					//디퍼드 그룹
					pMap = &m_mapInstGroup_D;
				}
				else if (pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_OPAQUE
					|| pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_MASK)
				{
					//포워드 그룹
					pMap = &m_mapInstGroup_F;
				}
				else
				{
					//그룹이 없다면 assert
					assert(nullptr);
					continue;
				}

				uInstID uID = {};
				//Res(CEntity) 마다 부여되는 Mesh ID, Material ID, Material Idx -> Union Get
				//vecGameObject에 담긴 객체 중 같은 Mesh, Material, Idx 사용하면 같은 ID-> unorderd_map Push
				uID.llID = RenderCompoent->GetInstID(iMtrl);

				// ID 가 0 다 ==> Mesh 나 Material 이 셋팅되지 않았다.
				if (0 == uID.llID)
					continue;

				//이미 map 안에 있는지 찾는다.
				std::unordered_map<ULONG64, vector<tInstObj>>::iterator iter = pMap->find(uID.llID);
				if (iter == pMap->end())
				{
					//없으면 Insert
					pMap->insert(make_pair(uID.llID, vector<tInstObj>{tInstObj{ pObj, iMtrl }}));
				}
				else
				{
					//있으면 같은 객체가 있는 것이므로 Vector PushBack
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
	// 마우스 방향을 향하는 Ray 구하기
	// SwapChain 타겟의 ViewPort 정보
	CMRT* pMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);
	D3D11_VIEWPORT tVP = pMRT->GetViewPort();

	//  현재 마우스 좌표
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

	// 직선은 카메라의 좌표를 반드시 지난다.
	m_ray.vStart = Transform()->GetWorldPos();

	// view space 에서의 방향
	m_ray.vDir.x = ((((vMousePos.x - tVP.TopLeftX) * 2.f / tVP.Width) - 1.f) - m_matProj._31) / m_matProj._11;
	m_ray.vDir.y = (-(((vMousePos.y - tVP.TopLeftY) * 2.f / tVP.Height) - 1.f) - m_matProj._32) / m_matProj._22;
	m_ray.vDir.z = 1.f;

	// world space 에서의 방향
	m_ray.vDir = XMVector3TransformNormal(m_ray.vDir, m_matViewInv);
	m_ray.vDir.Normalize();
}