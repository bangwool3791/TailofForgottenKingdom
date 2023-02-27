#include "pch.h"
#include "CMeshRender.h"
#include "CRenderMgr.h"
#include "CTransform.h"
#include "CAnimator2D.h"

CMeshRender::CMeshRender()
	:CRenderComponent{ COMPONENT_TYPE::MESHRENDER }
{
	SetName(L"CMeshRender");
	m_eInsType = INSTANCING_TYPE::NONE;
}

CMeshRender::CMeshRender(INSTANCING_TYPE _eInsType)
	:CRenderComponent{ COMPONENT_TYPE::MESHRENDER }
{
	m_eInsType = _eInsType;
}


CMeshRender::CMeshRender(const CMeshRender& rhs)
	:CRenderComponent{ rhs }
{
	m_eInsType = rhs.m_eInsType;
}

CMeshRender::~CMeshRender()
{

}

void CMeshRender::tick()
{

}

void CMeshRender::finaltick()
{

}

void CMeshRender::render()
{
	if (!IsActive())
		return;

	//여기서 왜 트랜스폼?
	if (nullptr == GetCurMaterial() || nullptr == GetMesh())
		return;
	/*
	* 텍스처 버퍼 업데이트 처리
	*
	* CRenderMgr::UpdateLight2D() 참고
	* 싱글 톤 또는 static vector 처리
	*/
	//삭제 예정
	Transform()->UpdateData();

	//삭제 예정
	GetCurMaterial()->UpdateData();

	if (Animator2D())
	{
		//삭제 예정
		Animator2D()->UpdateData();
	}

	GetMesh()->render();

	CMaterial::Clear();

	if (Animator2D())
		Animator2D()->Clear();
}

void CMeshRender::render_Instancing()
{
	if (!IsActive())
		return;

	//여기서 왜 트랜스폼?
	if (nullptr == GetCurMaterial() || nullptr == GetMesh() || nullptr == Transform())
		return;
	/*
	* 텍스처 버퍼 업데이트 처리
	*
	* CRenderMgr::UpdateLight2D() 참고
	* 싱글 톤 또는 static vector 처리
	*/
	//월드 정보만 갱신 후
	Transform()->Update();

	//삭제 예정
	//GetCurMaterial()->UpdateData();

	if (Animator2D())
	{
		//삭제 예정
		//업데이트만 시키고
		Animator2D()->Update();
	}

	tTransform transform = g_transform;
	tMtrlConst tMtrl = GetCurMaterial()->GetMaterial();
	tAnim2DInfo tAnimInfo{};

	if (Animator2D())
	{
		tAnimInfo = Animator2D()->GetAniInfo();
	}

	tObjectRender tObjectInfo = { transform, tMtrl, tAnimInfo };

	g_vecInfoObject.push_back(tObjectInfo);
}

