#include "pch.h"
#include "TileMapUI.h"
#include "CGameObjectEx.h"

#include "CImGuiMgr.h"

#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>

#include <Engine\CGameObject.h>

#include <Engine\CTexture.h>
#include <Engine\CResMgr.h>
#include <Engine\CRenderMgr.h>

#include <Engine\CScript.h>

#include "CEditor.h"
#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "ProgressUI.h"
#include "ListUI.h"
#include "Func.h"

#define ButtonScale ImVec2(60.f, 20.f) 
#define LargeButtonScale ImVec2(200.f, 20.f) 
#define ListBoxScale ImVec2(300.f, 120.f) 

TileMapUI::TileMapUI()
	: UI("TileMapUI")
	, m_tFaceid{}
	, m_bDialogLoad{}
	, m_bDialogSave{}
	, m_fileDialogLoad{}
	, m_fileDialogSave{}
{
	
}

TileMapUI::~TileMapUI()
{
}

void TileMapUI::begin()
{
	m_pLandObj = CEditor::GetInst()->FindByName(L"EditLandScape");
	m_pLandScape = m_pLandObj->LandScape();

	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"HeightMap");
	m_HeightImage = pTex->GetSRV().Get();

	m_pLandScape->GetHeightTexture(m_HeightImage);

	m_pLandScape->GetBrushSRV(m_BrushImage);

	m_pBrushObj = CEditor::GetInst()->FindByName(L"BrushObject");

	m_iWeightIdx = m_pLandScape->GetWeightIndex();
}

void TileMapUI::update()
{
	UI::update();
}

void TileMapUI::render_update()
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.8f));
	ImGui::Button("Tile");
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	/*
	LandScale
	*/



	if (ImGui::Button("Save##1", ButtonScale))
	{
		m_fileDialogLandscapeSave.type = ImGuiFileDialogType_SaveFile;
		m_fileDialogLandscapeSave.title = "Save File";
		m_fileDialogLandscapeSave.fileName = "land.dat";

		string path = CPathMgr::GetInst()->GetSingleContentPath();
		std::filesystem::path _path(path + "\\landscape");
		m_fileDialogLandscapeSave.directoryPath = _path;
		m_bDialogLandscapeSave = true;
	}

	if (m_bDialogLandscapeSave)
	{
		if (ImGui::FileDialog(&m_bDialogLandscapeSave, &m_fileDialogLandscapeSave))
		{
			FILE* pFile = nullptr;
			_wfopen_s(&pFile, m_fileDialogLandscapeSave.resultPath.c_str(), L"wb");

			m_pLandScape->SaveToFile(pFile);
			fclose(pFile);
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Load##1", ButtonScale))
	{
		m_fileDialogLandscapeLoad.type = ImGuiFileDialogType_OpenFile;
		m_fileDialogLandscapeLoad.title = "Open File";
		m_fileDialogLandscapeLoad.fileName = "land.dat";

		string path = CPathMgr::GetInst()->GetSingleContentPath();
		std::filesystem::path _path(path + "\\landscape");
		m_fileDialogLandscapeLoad.directoryPath = _path;
		m_bDialogLandscapeLoad = true;
	}

	if (m_bDialogLandscapeLoad)
	{
		if (ImGui::FileDialog(&m_bDialogLandscapeLoad, &m_fileDialogLandscapeLoad))
		{
			FILE* pFile = nullptr;
			_wfopen_s(&pFile, m_fileDialogLandscapeLoad.resultPath.c_str(), L"rb");
			m_pLandScape->LoadFromFile(pFile);
			fclose(pFile);
		}
	}

	if (ImGui::Button("Init", ButtonScale))
	{
		ProgressUI* pProgressUI = dynamic_cast<ProgressUI*>(CImGuiMgr::GetInst()->FindUI("ProgressUI"));
		assert(nullptr != pProgressUI);
		pProgressUI->Open();
		m_pLandScape->Initialize();
	}

	ImGui::PushItemWidth(150.f);
	ImGui::SameLine();

	if (ImGui::InputFloat("Land Scale", &g_LandScale, 2, 4, "%.3f"))
	{
		if (1 > g_LandScale)
			g_LandScale = 1;

		m_pLandObj->Transform()->SetRelativeScale({ g_LandScale ,g_LandScale ,g_LandScale });
		float fScale = g_BrushScale * g_LandScale * g_FaceCount;
		m_pBrushObj->Transform()->SetRelativeScale(Vec3(fScale, fScale, fScale));
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(150.f);
	if (ImGui::InputInt("FaceCount ", &g_FaceCount, 1, 2))
	{
		if (0 > g_FaceCount)
			g_FaceCount = 0;

		m_pLandScape->SetFaceCount(g_FaceCount, g_FaceCount);
		float fScale = g_BrushScale * g_LandScale * g_FaceCount;
		m_pBrushObj->Transform()->SetRelativeScale(Vec3(fScale, fScale, fScale));
	}

	static float fHeightmapOffset;
	ImGui::SameLine();
	ImGui::PushItemWidth(150.f);
	if (ImGui::InputFloat("Not used Heightmap offset", &fHeightmapOffset, 0.1f, 0.2f, "%.1f"))
	{
		m_pLandScape->SetHeightmapOffset(fHeightmapOffset);
	}

	/*
	Texture array list box
	*/
	Text(HSV_SKY_GREY, Vec2(150.f, 30.f), "Texture Array");

	if (ImGui::InputText("##1", m_szTexArrName, IM_ARRAYSIZE(m_szTexArrName)))
	{

	}

	ImGui::SameLine();

	if (ImGui::Button("Create##1", ButtonScale))
	{
		if (3 == m_vecColorTexture.size() && 3 == m_vecDirTexture.size())
		{
			string str;
			str.append(m_szTexArrName);

			if (!str.empty())
			{
				m_TextureArray = new CTexture(true);
				wstring wstr(str.begin(), str.end());
				m_TextureArray->SetKey(wstr);

				vector<Ptr<CTexture>> vec;

				for (size_t i = 0; i < 3; ++i)
					vec.push_back(m_vecColorTexture[i].first);

				for (size_t i = 0; i < 3; ++i)
					vec.push_back(m_vecDirTexture[i].first);
				vector<D3D11_TEXTURE2D_DESC> desc{};

				for (size_t i = 0; i < 6; ++i)
					desc.push_back(vec[i]->GetDesc());

				bool bSuccess = true;

				for (size_t i = 0; i < 6; ++i)
				{
					for (size_t j = 0; j < 6; ++j)
					{
						if (i == j)
							continue;
						
						if (desc[i].Width != desc[j].Width ||
							desc[i].Width != desc[j].Width)
						{
							wchar_t strBuff[50] = {};
							wsprintf(strBuff, L"에러인덱스 : %d", j);
							MessageBox(nullptr, strBuff, L"텍스쳐 해상도 불일치", MB_OK);
							bSuccess = false;
							break;
						}
					}
					if (!bSuccess)
						break;
				}
				if(bSuccess)
					m_TextureArray->CreateArrayTexture(vec, 1);
			}
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Save##2", ButtonScale))
	{
		m_fileDialogTextureArrSave.type = ImGuiFileDialogType_SaveFile;
		m_fileDialogTextureArrSave.title = "Save File";
		m_fileDialogTextureArrSave.fileName = "blank.png";

		string path = CPathMgr::GetInst()->GetSingleContentPath();
		std::filesystem::path _path(path + "\\texture");
		m_fileDialogTextureArrSave.directoryPath = _path;
		m_bDialogTextureArrSave = true;
	}

	if (m_bDialogTextureArrSave)
	{
		if (nullptr != m_TextureArray && ImGui::FileDialog(&m_bDialogTextureArrSave, &m_fileDialogTextureArrSave))
		{
			m_TextureArray->SaveTextureArray(m_fileDialogTextureArrSave.resultPath.c_str());
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Load##3", ButtonScale))
	{
		m_fileDialogTextureArrLoad.type = ImGuiFileDialogType_OpenFile;
		m_fileDialogTextureArrLoad.title = "Open File";
		m_fileDialogTextureArrLoad.fileName = "blank.png";

		string path = CPathMgr::GetInst()->GetSingleContentPath();
		std::filesystem::path _path(path + "\\texture");
		m_fileDialogTextureArrLoad.directoryPath = _path;
		m_bDialogTextureArrLoad = true;
	}

	if (m_bDialogTextureArrLoad)
	{
		if (ImGui::FileDialog(&m_bDialogTextureArrLoad, &m_fileDialogTextureArrLoad))
		{
			//m_TextureArray->Load(m_fileDialogTextureArrLoad.resultPath);
			wstring wstrContent = CPathMgr::GetInst()->GetContentPath();
			string strContent = string(wstrContent.begin(), wstrContent.end());
			string strFullPath = m_fileDialogTextureArrLoad.resultPath.string();

			std::size_t ind = strFullPath.find(strContent); // Find the starting position of substring in the string

			if (ind != std::string::npos)
			{
				strFullPath.erase(ind, strContent.length()); // erase function takes two parameter, the starting index in the string from where you want to erase characters and total no of characters you want to erase.

				wstring wstrFileName = wstring(strFullPath.begin() + 1, strFullPath.end());
				wchar_t sz_data[255];
				lstrcpy(sz_data, wstrFileName.c_str());
				//230325 수정 필요 문자열 
				Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(sz_data);

				if (nullptr != pTex)
					m_pLandScape->SetTexArr(pTex);
				else
				{
					wchar_t strBuff[50] = {};
					MessageBox(nullptr, L"에러", L"텍스쳐 배열 찾을 수 없음", MB_OK);
				}
			}
		}
	}

	/*
	* 툴팁 처리 필요
	*/
	if (ImGui::Button("Pop Texture", LargeButtonScale))
	{
		ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));

		assert(nullptr != pListUI);

		// 메쉬 목록을 받아와서, ListUI 에 전달
		const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResource(RES_TYPE::TEXTURE);
		static vector<wstring> vecRes;
		vecRes.clear();

		map<wstring, Ptr<CRes>>::const_iterator iter = mapRes.begin();

		for (; iter != mapRes.end(); ++iter)
			vecRes.push_back(iter->first);

		pListUI->SetItemList(vecRes);

		pListUI->AddDynamicDBClicked(this, (FUNC_1)&TileMapUI::SetColorTexture);
		pListUI->AddDynamicDBRightClicked(this, (FUNC_1)&TileMapUI::SetDirTexture);

		pListUI->Open();
	}


	if (ImGui::BeginListBox("##ColorListBox", ListBoxScale))
	{
		for (size_t i = 0; i < m_vecColorTexture.size(); ++i)
		{
			wstring key = m_vecColorTexture[i].first->GetKey();
			string uistr = string(key.begin(), key.end());
			if (ImGui::Selectable(uistr.c_str(), m_iColorTexIdx == i))
			{
				m_iColorTexIdx = i;
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}

	ImGui::SameLine();

	if (ImGui::BeginListBox("##DirListBox", ListBoxScale))
	{
		for (size_t i = 0; i < m_vecDirTexture.size(); ++i)
		{
			wstring key = m_vecDirTexture[i].first->GetKey();
			string uistr = string(key.begin(), key.end());
			if (ImGui::Selectable(uistr.c_str(), m_iDirTexIdx == i))
			{
				m_iDirTexIdx = i;
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}

	for (size_t i = 0; i < m_vecColorTexture.size(); ++i)
	{
		ImGui::Image(m_vecColorTexture[i].second, ImVec2(100.f, 100.f), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f));
		if (i != (m_vecColorTexture.size() - 1))
			ImGui::SameLine();
	}

	for (size_t i = 0; i < m_vecDirTexture.size(); ++i)
	{
		ImGui::Image(m_vecDirTexture[i].second, ImVec2(100.f, 100.f), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f));

		if (i != (m_vecDirTexture.size() - 1))
			ImGui::SameLine();
	}

	if (ImGui::Button("EraseColorTex##1", ButtonScale))
	{
		if (-1 != m_iColorTexIdx)
		{
			wstring key = m_vecColorTexture[m_iColorTexIdx].first->GetKey();

			for (auto iter = m_vecColorTexture.begin(); iter != m_vecColorTexture.end();)
			{
				if (!lstrcmp(key.c_str(), iter->first->GetKey().c_str()))
				{
					m_vecColorTexture.erase(iter);
					break;
				}
				else
					++iter;
			}
			if (m_vecColorTexture.empty())
				m_iColorTexIdx = -1;
			else
				m_iColorTexIdx = 0;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("EraseDirTex##1", ButtonScale))
	{
		if (-1 != m_iDirTexIdx)
		{
			wstring key = m_vecDirTexture[m_iDirTexIdx].first->GetKey();

			for (auto iter = m_vecDirTexture.begin(); iter != m_vecDirTexture.end();)
			{
				if (!lstrcmp(key.c_str(), iter->first->GetKey().c_str()))
				{
					m_vecDirTexture.erase(iter);
					break;
				}
				else
					++iter;
			}

			if (m_vecDirTexture.empty())
				m_iDirTexIdx = -1;
			else
				m_iDirTexIdx = 0;
		}
	}

	Text(HSV_SKY_GREY, Vec2(65.f, 30.f), "Heigtmap");

	static int e = 0;
	ImGui::RadioButton("None", &e, 0); ImGui::SameLine();
	ImGui::RadioButton("Height map", &e, 1); ImGui::SameLine();
	ImGui::RadioButton("Weight map", &e, 2); ImGui::SameLine();
	ImGui::RadioButton("Weight map clear", &e, 3);

	switch (e)
	{
	case 0:
		m_pLandScape->SetMode(LANDSCAPE_MOD::NONE);
		break;
	case 1:
		m_pLandScape->SetMode(LANDSCAPE_MOD::HEIGHT_MAP);
		break;
	case 2:
		m_pLandScape->SetMode(LANDSCAPE_MOD::SPLAT);
		break;
	case 3:
		m_pLandScape->SetMode(LANDSCAPE_MOD::SPLAT_CLEAR);
		break;
	default:
		break;
	}

	ImGui::PushItemWidth(150.f);
	if (ImGui::InputFloat("Brush Scale", &g_BrushScale, 0.01, 0.01, "%.3f"))
	{
		if (1 < g_BrushScale)
			g_BrushScale = 1.f;

		m_pLandScape->SetBrushScale(Vec2{ g_BrushScale,g_BrushScale });
		float fScale = g_BrushScale * g_LandScale * g_FaceCount;
		m_pBrushObj->Transform()->SetRelativeScale(Vec3(fScale, fScale, fScale));
	}

	if (ImGui::RadioButton("1 weight", &m_iWeightIdx, 0))
		m_pLandScape->SetWeightIndex(m_iWeightIdx);
	ImGui::SameLine();
	if (ImGui::RadioButton("2 weight", &m_iWeightIdx, 1))
		m_pLandScape->SetWeightIndex(m_iWeightIdx);
	ImGui::SameLine();
	if (ImGui::RadioButton("3 weight", &m_iWeightIdx, 2))
		m_pLandScape->SetWeightIndex(m_iWeightIdx);

	static bool bDir = false;
	if (ImGui::Checkbox("Dir - ", &bDir))
	{
		if (bDir)
			m_pLandScape->SetDir(-1);
		else
			m_pLandScape->SetDir(1);
	}

	ImGui::SameLine();

	if (ImGui::Button("Save##3", ButtonScale))
	{
		m_fileDialogSave.type = ImGuiFileDialogType_SaveFile;
		m_fileDialogSave.title = "Save File";
		m_fileDialogSave.fileName = "blank.png";

		string path = CPathMgr::GetInst()->GetSingleContentPath();
		std::filesystem::path _path(path + "\\texture");
		m_fileDialogSave.directoryPath = _path;
		m_bDialogSave = true;
	}

	if (m_bDialogSave)
	{
		if (ImGui::FileDialog(&m_bDialogSave, &m_fileDialogSave))
		{
			m_pLandScape->SaveTexture(m_fileDialogSave.resultPath);
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Load##2", ButtonScale))
	{
		m_fileDialogLoad.type = ImGuiFileDialogType_OpenFile;
		m_fileDialogLoad.title = "Open File";
		m_fileDialogLoad.fileName = "blank.png";

		string path = CPathMgr::GetInst()->GetSingleContentPath();
		std::filesystem::path _path(path + "\\texture");
		m_fileDialogLoad.directoryPath = _path;
		m_bDialogLoad = true;
	}

	if (m_bDialogLoad)
	{
		// Any place in draw loop
		if (ImGui::FileDialog(&m_bDialogLoad, &m_fileDialogLoad))
		{
			LoadHeightMap(m_fileDialogLoad.resultPath, m_HeightImage);
		}
	}

	ImGui::Image(m_HeightImage, ImVec2(212.f, 212.f), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f));

	Text(HSV_SKY_GREY, Vec2(65.f, 30.f), "Brushmap");

	if (ImGui::Button("Load##3", ButtonScale))
	{
		m_fileDialogBrushLoad.type = ImGuiFileDialogType_OpenFile;
		m_fileDialogBrushLoad.title = "Open File";
		m_fileDialogBrushLoad.fileName = "blank.png";

		string path = CPathMgr::GetInst()->GetSingleContentPath();
		std::filesystem::path _path(path + "\\texture");
		m_fileDialogBrushLoad.directoryPath = _path;
		m_bDialogBrushLoad = true;
	}

	if (m_bDialogBrushLoad)
	{
		// Any place in draw loop
		if (ImGui::FileDialog(&m_bDialogBrushLoad, &m_fileDialogBrushLoad))
		{
			LoadBrushMap(m_fileDialogBrushLoad.resultPath, m_BrushImage);
		}
	}

	ImGui::Image(m_BrushImage, ImVec2(212.f, 212.f), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f));

	Text(HSV_SKY_GREY, Vec2(150.f, 30.f), "Env Cube map");

	ImGui::SameLine();

	if (ImGui::Button("Capture##4", ButtonScale))
	{
		m_fileDialogCubemapSave.type = ImGuiFileDialogType_SaveFile;
		m_fileDialogCubemapSave.title = "Save File";
		m_fileDialogCubemapSave.fileName = "blank.png";

		string path = CPathMgr::GetInst()->GetSingleContentPath();
		std::filesystem::path _path(path + "texture");
		m_fileDialogCubemapSave.directoryPath = _path;
		m_bDialogCubemapSave = true;
	}

	if (m_bDialogCubemapSave)
	{
		if (ImGui::FileDialog(&m_bDialogCubemapSave, &m_fileDialogCubemapSave))
		{
			auto map = CEditor::GetInst()->GetEdiotrObj(EDIT_MODE::MAPTOOL);

			static vector<CGameObject*> vec{};

			for (auto iter{ map.begin() }; iter != map.end(); ++iter)
				vec.push_back(iter->second);

			CRenderMgr::GetInst()->render_env(vec, m_fileDialogCubemapSave.resultPath, m_fileDialogCubemapSave.resultPath);
		}
	}

}

#include <Engine\CTexture.h>
void TileMapUI::FindTexture(const wstring& _path, Ptr<CTexture>& pTex)
{
	//Texture 파일 경로 찾기
	wstring wstrFilePath = CPathMgr::GetInst()->GetContentPath();
	string strFilePath = string(wstrFilePath.begin(), wstrFilePath.end());
	strFilePath.append("\\");
	string strFullPath({ _path.begin(), _path.end() });

	std::size_t ind = strFullPath.find(strFilePath); // Find the starting position of substring in the string

	if (ind != std::string::npos) {
		strFullPath.erase(ind, strFilePath.length()); // erase function takes two parameter, the starting index in the string from where you want to erase characters and total no of characters you want to erase.

		wstring wstrFileName = wstring(strFullPath.begin(), strFullPath.end());

		pTex = CResMgr::GetInst()->FindRes<CTexture>(wstrFileName.c_str());
	}
}

void TileMapUI::LoadHeightMap(const wstring& _path, ImTextureID& _image)
{
	Ptr<CTexture> pTex = nullptr;

	FindTexture(_path, pTex);

	if (nullptr != pTex)
	{
		_image = pTex->GetSRV().Get();
		m_pLandScape->SetHeightMap(pTex);
	}
}

void TileMapUI::LoadBrushMap(const wstring& _path, ImTextureID& _image)
{
	Ptr<CTexture> pTex = nullptr;

	FindTexture(_path, pTex);

	if (nullptr != pTex)
	{
		_image = pTex->GetSRV().Get();
		m_pLandScape->SetBrushMap(pTex);
		m_pBrushObj->Decal()->SetDecalTexture(m_pLandScape->LandScape()->GetBrushTexture());
	}
}

void TileMapUI::InitializeHeightMap()
{
	m_pLandScape->CreateTexture();
}

void TileMapUI::SetColorTexture(DWORD_PTR _strMeshKey)
{
	string strKey = (char*)_strMeshKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(wstrKey);

	assert(nullptr != pTexture);

	if (3 > m_vecColorTexture.size())
		m_vecColorTexture.push_back({ pTexture, pTexture->GetSRV().Get()});
}

void TileMapUI::SetDirTexture(DWORD_PTR _strMeshKey)
{
	string strKey = (char*)_strMeshKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(wstrKey);

	assert(nullptr != pTexture);

	if (3 > m_vecDirTexture.size())
		m_vecDirTexture.push_back({ pTexture, pTexture->GetSRV().Get() });
}
