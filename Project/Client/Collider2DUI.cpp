#include "pch.h"
#include "Collider2DUI.h"


#include <Engine\CCollider2D.h>

Collider2DUI::Collider2DUI()
	: ComponentUI("Collider2D", COMPONENT_TYPE::COLLIDER2D)
{
}

Collider2DUI::~Collider2DUI()
{
}


void Collider2DUI::update()
{
	if (nullptr != GetTarget() && nullptr != GetTarget()->Collider2D())
	{
		m_vScale = GetTarget()->Collider2D()->GetScale();
		m_vOffsetPos = GetTarget()->Collider2D()->GetOffsetPos();
		m_vRot = GetTarget()->Collider2D()->GetRotation();
		m_bIgnorObjectScale = GetTarget()->Collider2D()->GetIgnoreObjectScale();
	}

	ComponentUI::update();
}

void Collider2DUI::render_update()
{
	ComponentUI::render_update();

	ImGui::Text("OffsetPos"); ImGui::SameLine(); ImGui::InputFloat2("##OffsetPos", m_vOffsetPos);
	ImGui::Text("Scale   "); ImGui::SameLine(); ImGui::InputFloat3("##Scale", m_vScale);


	m_vRot.ToDegree();
	ImGui::Text("Rotation"); ImGui::SameLine(); ImGui::InputFloat3("##Rotation", m_vRot);

	ImGui::Text("Ignore Object Scale"); ImGui::SameLine(); ImGui::Checkbox("##IgnorParentScale", &m_bIgnorObjectScale);

	if (nullptr != GetTarget() && nullptr != GetTarget()->Collider2D())
	{
		if (GetTarget()->Collider2D())
		{
			GetTarget()->Collider2D()->SetOffsetPos(m_vOffsetPos);
			GetTarget()->Collider2D()->SetScale(m_vScale);

			m_vRot.ToRadian();
			GetTarget()->Collider2D()->SetIgnoreObjectScale(m_bIgnorObjectScale);
		}
	}
}