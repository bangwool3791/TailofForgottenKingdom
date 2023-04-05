#include "pch.h"
#include "Collider3DUI.h"


#include <Engine\CCollider3D.h>

Collider3DUI::Collider3DUI()
	: ComponentUI("Collider3D", COMPONENT_TYPE::COLLIDER3D)
{
}

Collider3DUI::~Collider3DUI()
{
}


void Collider3DUI::update()
{
	if (nullptr != GetTarget() && nullptr != GetTarget()->Collider3D())
	{
		m_vScale = GetTarget()->Collider3D()->GetScale();
		m_vOffsetPos = GetTarget()->Collider3D()->GetOffsetPos();
		m_vRot = GetTarget()->Collider3D()->GetRotation();
		m_bIgnorObjectScale = GetTarget()->Collider3D()->GetIgnoreObjectScale();
	}

	ComponentUI::update();
}

void Collider3DUI::render_update()
{
	ComponentUI::render_update();

	ImGui::Text("OffsetPos"); ImGui::SameLine(); ImGui::InputFloat2("##OffsetPos", m_vOffsetPos);
	ImGui::Text("Scale   "); ImGui::SameLine(); ImGui::InputFloat3("##Scale", m_vScale);


	m_vRot.ToDegree();
	ImGui::Text("Rotation"); ImGui::SameLine(); ImGui::InputFloat3("##Rotation", m_vRot);

	ImGui::Text("Ignore Object Scale"); ImGui::SameLine(); ImGui::Checkbox("##IgnorParentScale", &m_bIgnorObjectScale);

	if (nullptr != GetTarget() && nullptr != GetTarget()->Collider3D())
	{
		if (GetTarget()->Collider3D())
		{
			GetTarget()->Collider3D()->SetOffsetPos(m_vOffsetPos);
			GetTarget()->Collider3D()->SetScale(m_vScale);

			m_vRot.ToRadian();
			GetTarget()->Collider3D()->SetIgnoreObjectScale(m_bIgnorObjectScale);
		}
	}
}