#include "pch.h"
#include "CMeshRender.h"
#include "CRenderMgr.h"
#include "CTransform.h"
#include "CAnimator2D.h"
#include "CAnimator3D.h"
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
	if (nullptr == GetMesh())
		return;
	/*
	* 텍스처 버퍼 업데이트 처리
	*
	* CRenderMgr::UpdateLight2D() 참고
	* 싱글 톤 또는 static vector 처리
	*/
	//삭제 예정
	Transform()->UpdateData();

	if (Animator2D())
	{
		//삭제 예정
		Animator2D()->UpdateData();
	}

	// Animator3D 업데이트
	if (Animator3D())
	{
		Animator3D()->UpdateData();

		for (size_t i = 0; i < GetMtrlCount(); ++i)
		{
			if (nullptr == GetCurMaterial(i))
				continue;
			GetCurMaterial(i)->SetAnim3D(true); // Animation Mesh 알리기
			GetCurMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
		}
	}

	// 사용할 재질 업데이트
	UINT iSubsetCount = GetMesh()->GetSubsetCount();

	for (int i = 0; i < iSubsetCount; ++i)
	{
		if (nullptr != GetCurMaterial(i))
		{
			GetCurMaterial(i)->UpdateData();

			// 사용할 메쉬 업데이트 및 렌더링
			GetMesh()->render(i);
		}
	}

	CMaterial::Clear();

	if (Animator2D())
		Animator2D()->Clear();

	if (Animator3D())
		Animator3D()->ClearData();
}

void CMeshRender::render_Instancing()
{
	if (!IsActive())
		return;

	//여기서 왜 트랜스폼?
	if (nullptr == GetMesh())
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
	tMtrlConst tMtrl = GetCurMaterial(0)->GetMaterial();
	tAnim2DInfo tAnimInfo{};

	if (Animator2D())
	{
		tAnimInfo = Animator2D()->GetAniInfo();
	}

	tObjectRender tObjectInfo = { transform, tMtrl, tAnimInfo };

	g_vecInfoObject.push_back(tObjectInfo);
}

void CMeshRender::SaveToFile(FILE* _File)
{
	COMPONENT_TYPE eType = GetType();

	fwrite(&eType, sizeof(COMPONENT_TYPE), 1, _File);

	__super::SaveToFile(_File);
}

void CMeshRender::LoadFromFile(FILE* _File)
{
	__super::LoadFromFile(_File);
}

