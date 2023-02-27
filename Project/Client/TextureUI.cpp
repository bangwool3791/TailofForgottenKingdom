#include "pch.h"
#include "TextureUI.h"

#include <Engine\CTexture.h>

TextureUI::TextureUI()
	: ResUI("Texture##UI", RES_TYPE::TEXTURE)
{
}

TextureUI::~TextureUI()
{
}

void TextureUI::update()
{


	ResUI::update();
}

void TextureUI::render_update()
{
	ResUI::render_update();

	CTexture* pTargetTex = ((CTexture*)GetTarget().Get());

	ImGui::Image(pTargetTex->GetSRV().Get(), ImVec2(150.f, 150.f));

	string strKey = string(pTargetTex->GetKey().begin(), pTargetTex->GetKey().end());
	string strRelativePath = string(pTargetTex->GetRelativePath().begin(), pTargetTex->GetRelativePath().end());

	ImGui::Text("Key   ");
	ImGui::SameLine();
	ImGui::InputText("##TexKey", (char*)strKey.data(), strKey.length(), ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Path  ");
	ImGui::SameLine();
	ImGui::InputText("##TexPath", (char*)strRelativePath.data(), strRelativePath.length(), ImGuiInputTextFlags_ReadOnly);

	char szNum[50] = {};
	_itoa_s(pTargetTex->GetWidth(), szNum, 10);


	ImGui::Text("Width ");
	ImGui::SameLine();
	ImGui::InputText("##TexWidth", szNum, 50, ImGuiInputTextFlags_ReadOnly);

	_itoa_s(pTargetTex->GetHeight(), szNum, 10);
	ImGui::Text("Height");
	ImGui::SameLine();
	ImGui::InputText("##TexHeight", szNum, 50, ImGuiInputTextFlags_ReadOnly);
}
