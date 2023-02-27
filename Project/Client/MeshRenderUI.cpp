#include "pch.h"
#include "MeshRenderUI.h"

#include <Engine\CResMgr.h>
#include <Engine\CGameObject.h>
#include <Engine\CMeshRender.h>

#include "CImGuiMgr.h"
#include "ListUI.h"

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
		m_Mtrl = GetTarget()->MeshRender()->GetCurMaterial();
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

	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(wstrKey);
	
	assert(nullptr != pMtrl);

	GetTarget()->MeshRender()->SetSharedMaterial(pMtrl);

	if (!lstrcmp(L"ObjectMtrl", pMtrl->GetKey().c_str()))
		GetTarget()->MeshRender()->SetInstancingType(INSTANCING_TYPE::USED);
	else
		GetTarget()->MeshRender()->SetInstancingType(INSTANCING_TYPE::NONE);
	
}

