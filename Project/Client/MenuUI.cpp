#include "pch.h"
#include "MenuUI.h"
#include "InspectorUI.h"
#include "ContentUI.h"

#include "CSaveLoadMgr.h"
#include "CEditor.h"
#include "CImGuiMgr.h"
#include "OutlinerUI.h"
#include "AnimationUI.h"

#include "CGameObjectEx.h"
#include <Engine\CEventMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CScript.h>
#include <Script\CScriptMgr.h>
MenuUI::MenuUI()
	:UI("##MenuUI")
    , m_eEditMode{EDIT_MODE::MAPTOOL }
{

}
MenuUI::~MenuUI()
{

}

void MenuUI::render()
{
    //지형 중복 피킹 예외처리
    //첫 프레임 클릭 시 
    //다음 프레임에서 해제

    if (true == CImGuiMgr::GetInst()->GetMenuClick())
        CImGuiMgr::GetInst()->SetMenuClick(false);

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
                AnimationUI* pUI = (AnimationUI*)CImGuiMgr::GetInst()->FindUI("AnimationUI");
                InspectorUI* Inspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");

                CGameObjectEx* pObj = Inspector->GetTargetObject();
                
                if (nullptr == pObj)
                {
                    MessageBox(nullptr, L"타겟 오브젝트 nullptr", L"Error", MB_OK);
                }
                else if (nullptr == pObj->Animator3D())
                {
                    MessageBox(nullptr, L"타겟 오브젝트 Animation nullptr", L"Error", MB_OK);
                }
                else if (nullptr != pObj)
                {
                    CEditor::GetInst()->SetEditMode(EDIT_MODE::ANIMATOR);
                    pUI->SetTargetObj(pObj);
                }
            }

            if (ImGui::MenuItem("Maptool"))
            {
                //메뉴 클릭시 지형 피킹 예외처리
                CImGuiMgr::GetInst()->SetMenuClick(true);

                CEditor::GetInst()->SetEditMode(EDIT_MODE::MAPTOOL);
                OutlinerUI* pUI = (OutlinerUI*)CImGuiMgr::GetInst()->FindUI("Outliner");
                pUI->ResetLevel();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Component"))
        {
            if (ImGui::BeginMenu("AddScript"))
            {
                //메뉴 클릭시 지형 피킹 예외처리
                CImGuiMgr::GetInst()->SetMenuClick(true);

                vector<string> vecScriptName{};
                CScriptMgr::GetScriptInfo(vecScriptName);

                for (size_t i{}; i < vecScriptName.size(); ++i)
                {
                    if (ImGui::MenuItem(vecScriptName[i].c_str()))
                    {
                        InspectorUI* pInspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
                        CGameObjectEx* pTargetObject = pInspector->GetTargetObject();

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

                OutlinerUI* pUI = (OutlinerUI*)CImGuiMgr::GetInst()->FindUI("Outliner");
                pUI->ResetLevel();
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