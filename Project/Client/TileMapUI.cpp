#include "pch.h"
#include "TileMapUI.h"
#include "CGameObjectEx.h"

#include "CImGuiMgr.h"

#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>

#include <Engine\CGameObject.h>

#include <Engine\CTexture.h>
#include <Engine\CResMgr.h>

#include <Engine\CScript.h>

#include "CEditor.h"
#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "ProgressUI.h"
#include "Func.h"

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

	Text(HSV_SKY_GREY, Vec2(65.f, 30.f), "Heigtmap");

	if (ImGui::Button("Save", ImVec2(65.f, 40.f)))
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

	if (ImGui::Button("Load##1", ImVec2(65.f, 40.f)))
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

	if (ImGui::Button("Load##2", ImVec2(65.f, 40.f)))
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

	if (ImGui::Button("Init", ImVec2(65.f, 40.f)))
	{
		ProgressUI* pProgressUI = dynamic_cast<ProgressUI*>(CImGuiMgr::GetInst()->FindUI("ProgressUI"));
		assert(nullptr != pProgressUI);
		pProgressUI->Open();
		m_pLandScape->Initialize();
	}

	ImGui::PushItemWidth(150.f);
	if (ImGui::InputFloat("Land Scale", &g_LandScale, 2, 4, "%.3f"))
	{
		if (0 > g_LandScale)
			g_LandScale = 0;

		m_pLandObj->Transform()->SetRelativeScale({ g_LandScale ,g_LandScale ,g_LandScale });
		float fScale = g_BrushScale * g_LandScale * g_FaceCount;
		m_pBrushObj->Transform()->SetRelativeScale(Vec3(fScale, fScale, fScale));
	}

	ImGui::PushItemWidth(150.f);
	if (ImGui::InputInt("FaceCount ", &g_FaceCount, 1, 2))
	{
		if (0 > g_FaceCount)
			g_FaceCount = 0;

		m_pLandScape->SetFaceCount(g_FaceCount, g_FaceCount);
		float fScale = g_BrushScale * g_LandScale * g_FaceCount;
		m_pBrushObj->Transform()->SetRelativeScale(Vec3(fScale, fScale, fScale));
	}

	ImGui::PushItemWidth(150.f);
	if (ImGui::InputFloat("Brush Scale", &g_BrushScale, 0.001, 0.01, "%.3f"))
	{
		if (1 < g_BrushScale)
			g_BrushScale = 1.f;

		m_pLandScape->SetBrushScale(Vec2{ g_BrushScale,g_BrushScale });
		float fScale = g_BrushScale * g_LandScale * g_FaceCount;
		m_pBrushObj->Transform()->SetRelativeScale(Vec3(fScale, fScale, fScale));
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
