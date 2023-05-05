#include "pch.h"
#include "MeshRenderUI.h"

#include <Engine\CResMgr.h>
#include <Engine\CGameObject.h>
#include <Engine\CMeshRender.h>

#include "CGameObjectEx.h"

#include "CImGuiMgr.h"
#include "ListUI.h"
#include "InspectorUI.h"

MeshRenderUI::MeshRenderUI()
	: ComponentUI("MeshRender", COMPONENT_TYPE::MESHRENDER)
{
}

MeshRenderUI::~MeshRenderUI()
{
}

void MeshRenderUI::update()
{
	if (nullptr != GetTarget() && nullptr != GetTarget()->MeshRender())
	{
		m_Mesh = GetTarget()->MeshRender()->GetMesh();
		m_Mtrl = GetTarget()->MeshRender()->GetCurMaterial(0);
	}

	ComponentUI::update();
}

void MeshRenderUI::render_update()
{
	ComponentUI::render_update();

	string MeshName, MtrlName;
	if (nullptr != m_Mesh)
	{
		MeshName = string(m_Mesh->GetKey().begin(), m_Mesh->GetKey().end());
	}

	if (nullptr != m_Mtrl)
	{
		MtrlName = string(m_Mtrl->GetKey().begin(), m_Mtrl->GetKey().end());
	}

	ImGui::Text("Mesh     "); 
	ImGui::SameLine(); 
	ImGui::InputText("##MeshName", (char*)MeshName.data(), MeshName.length(), ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	if (ImGui::Button("##MeshBtn", Vec2(15.f, 15.f)))
	{
		ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
		
		assert(nullptr != pListUI);

		// 메쉬 목록을 받아와서, ListUI 에 전달
		const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResource(RES_TYPE::MESH);
		static vector<wstring> vecRes;
		vecRes.clear();

		map<wstring, Ptr<CRes>>::const_iterator iter = mapRes.begin();
		for (; iter != mapRes.end(); ++iter)
		{
			vecRes.push_back(iter->first);
		}
		pListUI->SetItemList(vecRes);
		pListUI->AddDynamicDBClicked(this, (FUNC_1)&MeshRenderUI::SetMesh);

		pListUI->Open();
	}

	ImGui::Text("Material "); 
	ImGui::SameLine(); 
	ImGui::InputText("##MtrlName", (char*)MtrlName.data(), MtrlName.length(), ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	if (ImGui::Button("##MtrlBtn", Vec2(15.f, 15.f)))
	{
		ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
		
		assert(nullptr != pListUI);

		// 메테리얼 목록을 받아와서, ListUI 에 전달
		const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResource(RES_TYPE::MATERIAL);
		static vector<wstring> vecRes;
		vecRes.clear();

		map<wstring, Ptr<CRes>>::const_iterator iter = mapRes.begin();
		for (; iter != mapRes.end(); ++iter)
		{
			vecRes.push_back(iter->first);
		}
		pListUI->SetItemList(vecRes);
		pListUI->AddDynamicDBClicked(this, (FUNC_1)&MeshRenderUI::SetMaterial);

		pListUI->Open();
	}

	if (GetTarget() && GetTarget()->MeshRender())
	{
		static bool bActive = GetTarget()->MeshRender()->IsActive();

		bActive = GetTarget()->MeshRender()->IsActive();

		if (ImGui::Checkbox("Is Active##1", &bActive))
		{
			if(!bActive)
				GetTarget()->MeshRender()->Deactivate();
			else
				GetTarget()->MeshRender()->Activate();
		}

		m_eShadowType = GetTarget()->MeshRender()->GetShadowType();

		const char* items[] = { "dynamic", "static", "none" };
		static int item_current_idx = 0; // Here we store our selection data as an index.
		const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
		
		item_current_idx = (int)GetTarget()->MeshRender()->GetShadowType();

		if (ImGui::BeginCombo("Shadow Render Type", combo_preview_value, 0))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(items[n], is_selected))
					item_current_idx = n;

				if (0 == item_current_idx)
				{
					GetTarget()->MeshRender()->SetShadowType(ShadowType::DYNAMIC);
				}
				else if (1 == item_current_idx)
				{
					GetTarget()->MeshRender()->SetShadowType(ShadowType::STATIC);
				}
				else if (2 == item_current_idx)
				{
					GetTarget()->MeshRender()->SetShadowType(ShadowType::NONE);
				}
				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

	}
}

void MeshRenderUI::SetMesh(DWORD_PTR _strMeshKey)
{
	string strKey = (char*)_strMeshKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(wstrKey);
	
	assert(nullptr != pMesh);

	GetTarget()->MeshRender()->SetMesh(pMesh);
}

void MeshRenderUI::SetMaterial(DWORD_PTR _strMaterialKey)
{
	string strKey = (char*)_strMaterialKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	CGameObject* pTargetObject = GetTarget();
	CRenderComponent* pRenderCom = pTargetObject->GetRenderComponent();

	UINT iCount = pRenderCom->GetMtrlCount();

	for (UINT i = 0; i < iCount; ++i)
	{
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(wstrKey);

		assert(nullptr != pMtrl);

		GetTarget()->MeshRender()->SetSharedMaterial(pMtrl, i);

	}
}

