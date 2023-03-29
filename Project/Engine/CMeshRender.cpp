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

	//���⼭ �� Ʈ������?
	if (nullptr == GetMesh())
		return;
	/*
	* �ؽ�ó ���� ������Ʈ ó��
	*
	* CRenderMgr::UpdateLight2D() ����
	* �̱� �� �Ǵ� static vector ó��
	*/
	//���� ����
	Transform()->UpdateData();

	if (Animator2D())
	{
		//���� ����
		Animator2D()->UpdateData();
	}

	// ����� ���� ������Ʈ
	UINT iSubsetCount = GetMesh()->GetSubsetCount();

	for (int i = 0; i < iSubsetCount; ++i)
	{
		if (nullptr != GetCurMaterial(i))
		{
			GetCurMaterial(i)->UpdateData();

			// ����� �޽� ������Ʈ �� ������
			GetMesh()->render(i);
		}
	}

	CMaterial::Clear();

	if (Animator2D())
		Animator2D()->Clear();
}

void CMeshRender::render_Instancing()
{
	if (!IsActive())
		return;

	//���⼭ �� Ʈ������?
	if (nullptr == GetMesh())
		return;
	/*
	* �ؽ�ó ���� ������Ʈ ó��
	*
	* CRenderMgr::UpdateLight2D() ����
	* �̱� �� �Ǵ� static vector ó��
	*/
	//���� ������ ���� ��
	Transform()->Update();

	//���� ����
	//GetCurMaterial()->UpdateData();

	if (Animator2D())
	{
		//���� ����
		//������Ʈ�� ��Ű��
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

