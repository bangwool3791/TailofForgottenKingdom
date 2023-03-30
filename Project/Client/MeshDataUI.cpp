#include "pch.h"
#include "MeshDataUI.h"

#include "ParamUI.h"
#include "ListUI.h"

#include <Engine\struct.h>
#include <Engine\CShader.h>
#include <Engine\CMaterial.h>
#include <Engine\CResMgr.h>
#include <Engine/CMeshRender.h>
#include <Engine\CEventMgr.h>

#include <Engine\CGameObject.h>
#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>

#include "CImGuiMgr.h"

MeshDataUI::MeshDataUI()
	: ResUI("MeshData##UI", RES_TYPE::MESHDATA)
{
}

MeshDataUI::~MeshDataUI()
{
}

void MeshDataUI::update()
{
	ResUI::update();
}

void MeshDataUI::render_update()
{
	ResUI::render_update();

	CMeshData* pMeshData = (CMeshData*)GetTarget().Get();

	const vector<Ptr<CMaterial>>& vecMaterial = pMeshData->GetMtrls();

	ParamUI::ParamCount = 0;

	for (int j = 0; j < vecMaterial.size(); ++j)
	{
		Ptr<CMaterial> pMtrl = vecMaterial[j];

		string strMtrlKey = string(pMtrl->GetKey().begin(), pMtrl->GetKey().end());

		if (m_strRes != strMtrlKey)
			m_strRes = strMtrlKey;

		string strRelativePath = string(pMtrl->GetRelativePath().begin(), pMtrl->GetRelativePath().end());

		ImGui::Text("Key           ");
		ImGui::SameLine();
		ImGui::PushItemWidth(100.f);

		string text = "##MtrlKey" + std::to_string(j);

		if (ImGui::InputText(text.c_str(), (char*)strMtrlKey.data(), strMtrlKey.length(), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			wstring wstrValidKey(strMtrlKey.begin(), strMtrlKey.end());

			if (!CResMgr::GetInst()->DeleteRes((RES_TYPE)pMtrl->GetResType(), pMtrl->GetKey()))
			{
				MessageBox(nullptr, L"리소스 삭제 실패", L"에러", MB_OK);
			}

			wstring wstrOriginFilePath = CPathMgr::GetInst()->GetContentPath();
			wstrOriginFilePath += pMtrl->GetRelativePath();

			string strPath(wstrOriginFilePath.begin(), wstrOriginFilePath.end());

			if (0 == remove(strPath.c_str()))
				MessageBox(nullptr, L"원본 리소스 삭제됨", L"리소스 변경 확인", MB_OK);
			else
				MessageBox(nullptr, L"원본 리소스 삭제 실패", L"리소스 변경 실패", MB_OK);

			wchar_t sz_data[255];
			wstrValidKey = L"material\\" + wstrValidKey;
			lstrcpy(sz_data, wstrValidKey.c_str());
			wstrValidKey = lstrcat(sz_data, L".mtrl");
			ConvertGameObjectPath(wstrOriginFilePath, wstrValidKey);

			pMtrl->SwapFile(wstrValidKey);
			pMtrl->SetName(wstrValidKey);

			{
				tEvent evn = {};
				evn.eType = EVENT_TYPE::EDIT_RES;
				evn.wParam = (DWORD_PTR)pMtrl->GetResType();
				evn.lParam = (DWORD_PTR)(pMtrl.Get());
				CEventMgr::GetInst()->AddEvent(evn);
			}
		}

		ImGui::Text("Path          ");
		ImGui::SameLine();
		string MtrlPath = "##MtrlPath" + std::to_string(j);
		ImGui::InputText(MtrlPath.c_str(), (char*)strRelativePath.data(), strRelativePath.length(), ImGuiInputTextFlags_ReadOnly);

		string strShaderKey;

		if (nullptr != pMtrl->GetShader())
		{
			strShaderKey = string(pMtrl->GetShader()->GetKey().begin(), pMtrl->GetShader()->GetKey().end());
		}

		ImGui::Text("GraphicsShader");
		ImGui::SameLine();
		ImGui::PushItemWidth(100.f);
		string ShaderName = "##ShaderName" + std::to_string(j);
		ImGui::InputText(ShaderName.c_str(), (char*)strShaderKey.data(), strShaderKey.length(), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		//쉐이더 변경 및 선택
		string ShaderBtn = "##ShaderBtn" + std::to_string(j);
		if (ImGui::Button(ShaderBtn.c_str(), Vec2(15.f, 15.f)))
		{
			ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));

			assert(nullptr != pListUI);

			// 메쉬 목록을 받아와서, ListUI 에 전달
			const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResource(RES_TYPE::GRAPHICS_SHADER);
			static vector<wstring> vecRes;
			vecRes.clear();

			map<wstring, Ptr<CRes>>::const_iterator iter = mapRes.begin();
			for (; iter != mapRes.end(); ++iter)
			{
				vecRes.push_back(iter->first);
			}
			pListUI->SetItemList(vecRes);
			pListUI->AddDynamicDBClicked(this, (FUNC_1)&MeshDataUI::SetShader);

			pListUI->Open();
		}
		ImGui::NewLine();
		ImGui::Text("Shader Parameter");

		if (nullptr == pMtrl->GetShader())
			return;

		const vector<tScalarParam> vecScalar = pMtrl->GetShader()->GetScalarParam();

		for (size_t i{}; i < vecScalar.size(); ++i)
		{
			switch (vecScalar[i].eParam)
			{
			case INT_0:
			case INT_1:
			case INT_2:
			case INT_3:
			{
				int iData = 0;
				pMtrl->GetScalarParam(vecScalar[i].eParam, &iData);
				if (ParamUI::Param_Int(vecScalar[i].strName, &iData))
					pMtrl->SetScalarParam(vecScalar[i].eParam, &iData);
			}
			break;
			case FLOAT_0:
			case FLOAT_1:
			case FLOAT_2:
			case FLOAT_3:
			{
				float fData = 0;
				pMtrl->GetScalarParam(vecScalar[i].eParam, &fData);
				if (ParamUI::Param_Float(vecScalar[i].strName, &fData))
					pMtrl->SetScalarParam(vecScalar[i].eParam, &fData);
			}
			break;
			case VEC2_0:
			case VEC2_1:
			case VEC2_2:
			case VEC2_3:
			{
				Vec2 data;
				pMtrl->GetScalarParam(vecScalar[i].eParam, &data);
				if (ParamUI::Param_Vec2(vecScalar[i].strName, &data))
					pMtrl->SetScalarParam(vecScalar[i].eParam, &data);
			}
			break;
			case VEC4_0:
			case VEC4_1:
			case VEC4_2:
			case VEC4_3:
			{
				Vec4 data;
				pMtrl->GetScalarParam(vecScalar[i].eParam, &data);
				if (ParamUI::Param_Vec4(vecScalar[i].strName, &data))
					pMtrl->SetScalarParam(vecScalar[i].eParam, &data);
			}
			break;
			case MAT_0:
			case MAT_1:
			case MAT_2:
			case MAT_3:
			{

			}
			break;
			}
		}


		const vector<tTextureParam> vecTex = pMtrl->GetShader()->GetTextureParam();
		for (size_t i = 0; i < vecTex.size(); ++i)
		{
			Ptr<CTexture> pTex = pMtrl->GetTexParam(vecTex[i].eParam);
			if (ParamUI::Param_Tex(vecTex[i].strName, pTex, this, (FUNC_1)&MeshDataUI::SetTexture))
			{
				//몇번째 Texture를 변경 할 것인지
				m_iCurMtrl = j;
				m_eSelectTexParam = vecTex[i].eParam;
			}
			else
			{
				//텍스쳐가 세팅 되어있으면 Texture를 Set
				pMtrl->SetTexParam(vecTex[i].eParam, pTex);
			}
		}
	}
}

//List에서 선택 된 Texture를 Load
//클릭 된 머터리얼의 주소에, 선택 된 Texture 인덱스의 Texture를 변경한다.
void MeshDataUI::SetTexture(DWORD_PTR _strTexKey)
{
	string strKey = (char*)_strTexKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(wstrKey);

	assert(nullptr != pTex);

	CMeshData* pMeshData = ((CMeshData*)GetTarget().Get());

	const vector<Ptr<CMaterial>>& vecMaterial = pMeshData->GetMtrls();

	for (size_t i = 0; i < vecMaterial.size(); ++i)
	{
		if (m_iCurMtrl == i)
		{
			vecMaterial[i]->SetTexParam(m_eSelectTexParam, pTex);
			break;
		}
	}
}

void MeshDataUI::SetShader(DWORD_PTR _strShaderKey)
{
	string strKey = (char*)_strShaderKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CGraphicsShader> pShader = CResMgr::GetInst()->FindRes<CGraphicsShader>(wstrKey);

	if (nullptr == pShader)


		((CMaterial*)GetTarget().Get())->SetShader(pShader);
}

void MeshDataUI::ConvertGameObjectPath(const wstring& originPath, const wstring& _NewPath)
{
	for (size_t i{}; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObjects = CLevelMgr::GetInst()->GetCurLevel()->GetLayer(i)->GetObjects();

		for (size_t i{}; i < vecObjects.size(); ++i)
		{
			if (vecObjects[i]->MeshRender())
			{
				wstring wstrPath = vecObjects[i]->MeshRender()->GetSharedMaterial(0)->GetRelativePath();
				if (originPath == wstrPath)
				{
					vecObjects[i]->MeshRender()->GetSharedMaterial(0)->SetRelativePath(_NewPath);
					break;
				}
			}
		}
	}
}

