#include "pch.h"
#include "TransformUI.h"


#include <Engine\CTransform.h>

TransformUI::TransformUI()
	: ComponentUI("Transform", COMPONENT_TYPE::TRANSFORM)	
	, m_bIgnorScale(false)
{
}

TransformUI::~TransformUI()
{
}


void TransformUI::update()
{
	if (nullptr != GetTarget() && nullptr != GetTarget()->Transform())
	{
		CGameObject* pObj = GetTarget();
		m_vPos = GetTarget()->Transform()->GetRelativePos();
		m_vScale = GetTarget()->Transform()->GetRelativeScale();
		m_vRot = GetTarget()->Transform()->GetRelativeRotation();

		m_bIgnorScale = GetTarget()->Transform()->IsIgnoreParentScale();
	}

	ComponentUI::update();
}

void TransformUI::render_update()
{
	ComponentUI::render_update();
	
	ImGui::Text("Position"); ImGui::SameLine(); ImGui::DragFloat3("##Position", m_vPos);
	ImGui::Text("Scale   "); ImGui::SameLine(); ImGui::DragFloat3("##Scale", m_vScale);

	m_vRot.ToDegree();
	ImGui::Text("Rotation"); ImGui::SameLine(); ImGui::DragFloat3("##Rotation", m_vRot);
		
	ImGui::Text("Ignore Parent Scale"); ImGui::SameLine(); ImGui::Checkbox("##IgnorParentScale", &m_bIgnorScale);

	if (nullptr != GetTarget() && nullptr != GetTarget()->Transform())
	{
		if (nullptr == GetTarget()->Transform())
			return;

		GetTarget()->Transform()->SetRelativePos(m_vPos);
		GetTarget()->Transform()->SetRelativeScale(m_vScale);

		m_vRot.ToRadian();
		GetTarget()->Transform()->SetRelativeRotation(m_vRot);

		GetTarget()->Transform()->SetIgnoreParentScale(m_bIgnorScale);
	}
}