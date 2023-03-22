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

	if(ImGui::Button("Save", ImVec2(65.f, 40.f)))
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

	if (ImGui::Button("LoadHeigtMap", ImVec2(65.f, 40.f)))
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
			LoadTextureFromEdit(m_fileDialogLoad.resultPath);
		}
	}

	if (ImGui::Button("Initialize", ImVec2(65.f, 40.f)))
	{
		ProgressUI* pProgressUI = dynamic_cast<ProgressUI*>(CImGuiMgr::GetInst()->FindUI("ProgressUI"));
		
		assert(nullptr != pProgressUI);
		pProgressUI->Open();
		m_pLandScape->Initialize();
	}

	if (ImGui::Button("Test", ImVec2(65.f, 40.f)))
	{
		string path = CPathMgr::GetInst()->GetSingleContentPath();
		path.append("\\texture\\test.bmp");
		m_pLandScape->SaveBmpFile(wstring(path.begin(), path.end()));
	}

	if (ImGui::Button("Test1", ImVec2(65.f, 40.f)))
	{
		string path = CPathMgr::GetInst()->GetSingleContentPath();
		path.append("\\texture\\test.bmp");
		m_pLandScape->LoadBmpFile(wstring(path.begin(), path.end()));
	}
	
	if (ImGui::InputInt("FaceCount X ", &m_iXFaceCount))
	{
		if (1 > m_iXFaceCount)
			m_iXFaceCount = 1;

		m_pLandScape->SetFaceCount(m_iXFaceCount, m_iZFaceCount);
	}

	if (ImGui::InputInt("FaceCount Z ", &m_iZFaceCount))
	{
		if (1 > m_iZFaceCount)
			m_iZFaceCount = 1;

		m_pLandScape->SetFaceCount(m_iXFaceCount, m_iZFaceCount);
	}
}

#include <Engine\CTexture.h>

void TileMapUI::LoadTextureFromEdit(const wstring& _path)
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
		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(wstrFileName.c_str());
		
		if(nullptr != pTex)
			m_pLandScape->SetHeightMap(pTex);
	}
}

void TileMapUI::InitializeHeightMap()
{
	m_pLandScape->CreateTexture();
}
