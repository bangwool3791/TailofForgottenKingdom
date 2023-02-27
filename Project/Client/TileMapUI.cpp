#include "pch.h"
#include "TileMapUI.h"
#include "CGameObjectEx.h"

#include "CImGuiMgr.h"

#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>

#include <Engine\CGameObject.h>
#include <Engine\CTerrain.h>

#include <Engine\CTexture.h>
#include <Engine\CResMgr.h>

#include <Engine\CScript.h>

#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "ProgressUI.h"

TileMapUI::TileMapUI()
	: UI("TileMapUI")
	, m_vTileSize{1.f, 1.f }
{
}

TileMapUI::~TileMapUI()
{
}

void TileMapUI::begin()
{
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

	if(ImGui::Button("Save"))
	{
		m_pEditTerrainObject->GetRenderComponent()->GetMesh()->Save(L"Terrain\\Terrain.dat");
	}

	ImGui::SameLine();

	if (ImGui::Button("Load"))
	{
		m_pEditTerrainObject->GetRenderComponent()->GetMesh()->Load(L"Terrain\\Terrain.dat");
	}

	if (ImGui::Button("추후 업데이트 Apply"))
	{
		ProgressUI* pProgressUI = dynamic_cast<ProgressUI*>(CImGuiMgr::GetInst()->FindUI("ProgressUI"));
		
		assert(nullptr != pProgressUI);
		pProgressUI->Open();
	}
}

void TileMapUI::Initialize(void* pAddr)
{
	m_pEditTerrainObject = (CGameObject*)pAddr;
	m_pEditTerrain = static_cast<CTerrain*>(m_pEditTerrainObject->GetRenderComponent());
}




