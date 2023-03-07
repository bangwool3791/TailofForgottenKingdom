#include "pch.h"
#include "ParamUI.h"

#include <Engine\CResMgr.h>

#include "imgui.h"
#include "CImGuiMgr.h"
#include "ListUI.h"
#include "TreeUI.h"

UINT ParamUI::ParamCount = 0;

void ParamUI::Param_Int(const string& _ParamName, int* _pInOut)
{
	char szName[50] = "";
	sprintf_s(szName, 50, "##int%d", ParamCount++);

	ImGui::Text(_ParamName.c_str());
	ImGui::SameLine();
	ImGui::InputInt(szName, _pInOut);

	if (ImGui::InputInt(szName, _pInOut, 1, 1, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
	{
		UI::ResetFocus();
	}
}

void ParamUI::Param_Float(const string& _ParamName, float* _pInOut)
{
	char szName[50] = "";
	sprintf_s(szName, 50, "##Float%d", ParamCount++);

	ImGui::Text(_ParamName.c_str());
	ImGui::SameLine();
	//if (ImGui::DragFloat(szName, _pInOut, 0.01f, 0.f, 5.f, "%6.3f"))
	if (ImGui::InputFloat(szName, _pInOut, 0.01f, 0.f, "%6.3f"))
	{
		if (ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			UI::ResetFocus();
		}
	}
}

void ParamUI::Param_Vec2(const string& _ParamName, Vec2* _pInOut)
{
	char szName[50] = "";
	sprintf_s(szName, 50, "##Vec2%d", ParamCount++);

	ImGui::Text(_ParamName.c_str());
	ImGui::SameLine();

	if (ImGui::DragFloat2(szName, *_pInOut, 0.f, 0.f, 1.f, "%6.3f"))
	{
		if (ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			UI::ResetFocus();
		}
	}

	//if (ImGui::InputFloat(szName, *_pInOut, 0.f, 0.f, "%6.3f", ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
	//{
	//	UI::ResetFocus();
	//}
}

void ParamUI::Param_Vec4(const string& _ParamName, Vec4* _pInOut)
{
	char szName[50] = "";
	sprintf_s(szName, 50, "##Vec4%d", ParamCount++);

	ImGui::Text(_ParamName.c_str());
	ImGui::SameLine();

	ImGui::PushItemWidth(200.f);

	if (ImGui::DragFloat4(szName, *_pInOut, 0.f, 0.f, 1.f, "%6.3f"))
	{
		if (ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			UI::ResetFocus();
		}
	}
	//ImGui::PushItemWidth(200.f);
	//if (ImGui::InputFloat4(szName, *_pInOut, "%.3f", ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
	//{
	//	UI::ResetFocus();
	//}
}


void ParamUI::Param_Mat(const string& _ParamName, Matrix* _pInOut)
{
}

bool ParamUI::Param_Tex(const string& _ParamName, Ptr<CTexture>& _Tex, UI* _Inst, FUNC_1 _Func)
{
	ImGui::Text(_ParamName.c_str());

	if (nullptr != _Tex)
	{
		ImGui::Image(_Tex->GetSRV().Get(), ImVec2(100.f, 100.f));
	}
	else
	{
		ImGui::Image(nullptr, ImVec2(100.f, 100.f));
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##ContentTree"))
		{
			TreeNode* pNode = (TreeNode*)payload->Data;
			CRes* pRes = (CRes*)pNode->GetData();

			if (RES_TYPE::TEXTURE == pRes->GetResType())
			{
				_Tex = (CTexture*)pRes;
			}
		}
		ImGui::EndDragDropTarget();
	}

	char szName[50] = "";
	sprintf_s(szName, 50, "##TexBtn%d", ParamCount++);

	ImGui::SameLine();
	if (ImGui::Button(szName, Vec2(15.f, 15.f)))
	{
		ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));

		assert(nullptr != pListUI);

		//메쉬 목록을 받아와서, ListUI에 전달
		const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResource(RES_TYPE::TEXTURE);
		static vector<wstring> vecRes;
		vecRes.clear();

		for (auto iter{ mapRes.begin() }; iter != mapRes.end(); ++iter)
		{
			vecRes.push_back(iter->first);
		}
		pListUI->SetItemList(vecRes);
		pListUI->AddDynamicDBClicked(_Inst, _Func);
		pListUI->Open();

		return true;
	}
	return false;
}