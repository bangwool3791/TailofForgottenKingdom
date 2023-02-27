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
	if (nullptr == GetCurMaterial() || nullptr == GetMesh())
		return;
	/*
	* �ؽ�ó ���� ������Ʈ ó��
	*
	* CRenderMgr::UpdateLight2D() ����
	* �̱� �� �Ǵ� static vector ó��
	*/
	//���� ����
	Transform()->UpdateData();

	//���� ����
	GetCurMaterial()->UpdateData();

	if (Animator2D())
	{
		//���� ����
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

	//���⼭ �� Ʈ������?
	if (nullptr == GetCurMaterial() || nullptr == GetMesh() || nullptr == Transform())
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
	tMtrlConst tMtrl = GetCurMaterial()->GetMaterial();
	tAnim2DInfo tAnimInfo{};

	if (Animator2D())
	{
		tAnimInfo = Animator2D()->GetAniInfo();
	}

	tObjectRender tObjectInfo = { transform, tMtrl, tAnimInfo };

	g_vecInfoObject.push_back(tObjectInfo);
}

