#include "pch.h"
#include "MenuUI.h"
#include "InspectorUI.h"
#include "ContentUI.h"

#include "CSaveLoadMgr.h"
#include "CEditor.h"
#include "CImGuiMgr.h"

#include <Engine\CGameObject.h>
#include <Engine\CEventMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CScript.h>
#include <Script\CScriptMgr.h>
MenuUI::MenuUI()
	:UI("##MenuUI")
    , m_eEditMode{EDIT_MODE::ANIMATOR }
{

}
MenuUI::~MenuUI()
{

}

void MenuUI::render()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Level Save"))
            {
                CSaveLoadMgr::GetInst()->SaveLevel(CLevelMgr::GetInst()->GetCurLevel(), L"level\\Test.lv");
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Animator"))
            {
                CEditor::GetInst()->SetEditMode(EDIT_MODE::ANIMATOR);
                m_eEditMode = EDIT_MODE::ANIMATOR;
            }

            if (ImGui::MenuItem("Maptool"))
            {
                CEditor::GetInst()->SetEditMode(EDIT_MODE::MAPTOOL);
                m_eEditMode = EDIT_MODE::MAPTOOL;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Component"))
        {
            if (ImGui::BeginMenu("AddScript"))
            {
                
                vector<string> vecScriptName{};
                CScriptMgr::GetScriptInfo(vecScriptName);

                for (size_t i{}; i < vecScriptName.size(); ++i)
                {
                    if (ImGui::MenuItem(vecScriptName[i].c_str()))
                    {
                        InspectorUI* pInspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
                        CGameObject* pTargetObject = pInspector->GetTargetObject();

                        if (pTargetObject)
                        {
                            CComponent* pComponent =  CScriptMgr::GetScript(vecScriptName[i]);
                            pTargetObject->AddComponent(pComponent);
                            pComponent->begin();
                            //갱신 용도
                            pInspector->SetTargetObject(pTargetObject);
                        }
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Level"))
        {
            LEVEL_STATE State = CLevelMgr::GetInst()->GetLevelState();

            bool PlayEnable = true;
            bool PauseEnable = true;
            bool StopEnable = true;

            if (LEVEL_STATE::PLAY == State)
                PlayEnable = false;
            else
                PlayEnable = true;

            if (LEVEL_STATE::PLAY != State)
                PauseEnable = false;
            else
                PauseEnable = true;

            if (LEVEL_STATE::STOP == State)
                StopEnable = false;
            else
                StopEnable = true;

            if (ImGui::MenuItem("Play", nullptr, nullptr, PlayEnable))
            {
                tEvent evn = {};
                evn.eType = EVENT_TYPE::CHANGE_LEVEL_STATE;
                evn.wParam = (DWORD_PTR)LEVEL_STATE::PLAY;
                CEditor::GetInst()->SetEditMode(EDIT_MODE::OBJECT);
                CEventMgr::GetInst()->AddEvent(evn);
            }

            if (ImGui::MenuItem("Pause", nullptr, nullptr, PauseEnable))
            {
                tEvent evn = {};
                evn.eType = EVENT_TYPE::CHANGE_LEVEL_STATE;
                evn.wParam = (DWORD_PTR)LEVEL_STATE::PAUSE;
                CEditor::GetInst()->SetEditMode(m_eEditMode);
                CEventMgr::GetInst()->AddEvent(evn);
            }

            if (ImGui::MenuItem("Stop", nullptr, nullptr, StopEnable))
            {
                tEvent evn = {};
                evn.eType = EVENT_TYPE::CHANGE_LEVEL_STATE;
                evn.wParam = (DWORD_PTR)LEVEL_STATE::STOP;
                CEventMgr::GetInst()->AddEvent(evn);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Resource"))
        {
            if (ImGui::MenuItem("Create Material"))
            {
                Ptr<CMaterial> pMtrl = new CMaterial;
                wstring strKey = CResMgr::GetInst()->GetNewResName<CMaterial>();

                CResMgr::GetInst()->AddRes<CMaterial>(strKey, pMtrl.Get());

                ContentUI* pContent = (ContentUI*)CImGuiMgr::GetInst()->FindUI("Content");
                pContent->ResetContent();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void MenuUI::render_update()
{

}