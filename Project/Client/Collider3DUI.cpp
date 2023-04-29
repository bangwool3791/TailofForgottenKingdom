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
		m_vRotOffset = GetTarget()->Collider3D()->GetRotOffset();
		m_bIgnorObjectScale = GetTarget()->Collider3D()->GetIgnoreObjectScale();
	}

	ComponentUI::update();
}

void Collider3DUI::render_update()
{
	ComponentUI::render_update();

	if (nullptr != GetTarget() && nullptr != GetTarget()->Collider3D())
	{

		ImGui::Text("OffsetPos"); ImGui::SameLine(); ImGui::InputFloat3("##OffsetPos", m_vOffsetPos);
		ImGui::Text("Scale   "); ImGui::SameLine(); ImGui::InputFloat3("##Scale", m_vScale);


		m_vRot.ToDegree();
		ImGui::Text("Rotation"); ImGui::SameLine(); 

		if (ImGui::InputFloat3("##Rotation", m_vRot))
		{
			m_vRot.ToRadian();
			GetTarget()->Collider3D()->SetRotation(m_vRot);
		}

		m_vRotOffset.ToDegree();
		ImGui::Text("Rot Offset"); ImGui::SameLine();

		if (ImGui::InputFloat3("##Rot Offset", m_vRotOffset))
		{
			m_vRotOffset.ToRadian();
			GetTarget()->Collider3D()->SetRotOffset(m_vRotOffset);
		}

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

		static bool bUse = false;

		bUse = GetTarget()->Collider3D()->GetIsUpdate();
		if (ImGui::Checkbox("Update##1", &bUse))
		{
			GetTarget()->Collider3D()->SetIsUpdate(bUse);
		}

		m_iBoneIdx = GetTarget()->Collider3D()->GetBoneIdx();

		ImGui::Text("Bone Idx   "); ImGui::SameLine();

		if (ImGui::InputInt("##BoneIdx", &m_iBoneIdx))
		{
			if (m_iBoneIdx < 0)
				m_iBoneIdx = 0;

			if (126 <= m_iBoneIdx)
				m_iBoneIdx = 125;

			GetTarget()->Collider3D()->SetBoneIdx(m_iBoneIdx);
		}

		static bool bSphere = false;

		if (COLLIDER3D_TYPE::COLLIDER3D_SPHERE == GetTarget()->Collider3D()->GetCollider3DType())
			bSphere = true;
		else
			bSphere = false;

		ImGui::Text("Set Collider Type   "); ImGui::SameLine();

		if (ImGui::Checkbox("Is Sphere##1", &bSphere))
		{
			if (bSphere)
				GetTarget()->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::COLLIDER3D_SPHERE);
			else
				GetTarget()->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::COLLIDER3D_CUBE);
		}
	}
}