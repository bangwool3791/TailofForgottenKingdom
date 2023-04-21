#include "pch.h"
#include "CImGuiMgr.h"

#include <Engine\CDevice.h>
#include "CEditor.h"

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"

#include "CGameObjectEx.h"
#include <Engine\CTransform.h>
#include <Engine\CCamera.h>

#include "UI.h"
#include "PopupUI.h"
#include "ParamUI.h"

CImGuiMgr::CImGuiMgr()
    : m_NotifyHandle{}
{
}

CImGuiMgr::~CImGuiMgr()
{
    for (auto iter{ m_mapUI.begin() }; iter != m_mapUI.end(); ++iter)
    {
        Safe_Delete(iter->second);
    }
    m_mapUI.clear();

    clear();
}


void CImGuiMgr::init(HWND _hWnd)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDefaultParent = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(_hWnd);
    ImGui_ImplDX11_Init(DEVICE, CONTEXT);

    // UI 생성
    //GUI 주석
    CreateUI();

     //파일 변경감지 핸들 등록
    m_NotifyHandle = FindFirstChangeNotification(
        CPathMgr::GetInst()->GetContentPath(), true,
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);
   // GUI 주석 끝
}

// Camera projection
bool isPerspective = true;
float fov = 27.f;
float viewWidth = 10.f; // for orthographic
float camYAngle = 165.f / 180.f * 3.14159f;
float camXAngle = 32.f / 180.f * 3.14159f;

bool firstFrame = true;

void CImGuiMgr::Guizmo()
{
    ImGuizmo::BeginFrame();
    ImGuizmo::Enable(true);

    static Matrix mat{};
    static XMFLOAT4X4* t = nullptr;
    static XMFLOAT4X4 localMat{};
    static XMFLOAT4X4 viewMat{};
    static XMFLOAT4X4 projMat{};

    if (nullptr != m_pSelectedObj)
    {
        CGameObjectEx* pCam = CEditor::GetInst()->FindByName(L"EditorCamera");
        Vec3 pos = m_pSelectedObj->Transform()->GetRelativePos();
        Vec3 rot = m_pSelectedObj->Transform()->GetRelativeRotation();
        Vec3 sca = m_pSelectedObj->Transform()->GetRelativeScale();
        pCam->Camera()->CalcViewMat();
        pCam->Camera()->CalcProjMat();

        XMStoreFloat4x4(&localMat, m_pSelectedObj->Transform()->GetWorldMat());
        XMStoreFloat4x4(&viewMat, pCam->Camera()->GetViewMat());
        XMStoreFloat4x4(&projMat, pCam->Camera()->GetProjMat());
        static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

        if (ImGui::IsKeyPressed(ImGuiKey_T))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

        if (ImGui::IsKeyPressed(ImGuiKey_E))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;

        if (ImGui::IsKeyPressed(ImGuiKey_R))
            mCurrentGizmoOperation = ImGuizmo::SCALE;

        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

        ImGui::SameLine();

        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;

        ImGui::SameLine();

        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;

        if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
            mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;

        float translate[3] = { };
        float rotation[3] = {};
        float scale[3] = {};

        ImGuizmo::DecomposeMatrixToComponents(*localMat.m, translate, rotation, scale);
        ImGui::InputFloat3("Tr", translate, "%3.f", 3);
        ImGui::InputFloat3("Rt", rotation, "%3.f", 3);
        ImGui::InputFloat3("SC", scale, "%3.f", 3);
        ImGuizmo::RecomposeMatrixFromComponents(translate, rotation, scale, *localMat.m);

        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        ImGuizmo::Manipulate(*viewMat.m, *projMat.m, mCurrentGizmoOperation, mCurrentGizmoMode, *localMat.m, NULL, NULL);
        ImGuizmo::DecomposeMatrixToComponents(*localMat.m, translate, rotation, scale);

        if (ImGuizmo::IsUsing())
        {
            mat = XMLoadFloat4x4(&localMat);
            XMVECTOR vScale = { scale[0],scale[1], scale[2] };
            XMVECTOR outRotQuat = { rotation[0], rotation[1], rotation[2] };
            XMVECTOR outTrans = { translate[0],translate[1], translate[2] };

            XMStoreFloat3(&pos, outTrans);
            XMStoreFloat3(&rot, outRotQuat);
            XMStoreFloat3(&sca, vScale);

            switch (mCurrentGizmoOperation)
            {
            case ImGuizmo::TRANSLATE:
                m_pSelectedObj->Transform()->SetRelativePos(pos);
                break;
            case ImGuizmo::ROTATE:
                m_pSelectedObj->Transform()->SetRelativeRotation(rot / 180.f * XM_PI);
                break;
            case ImGuizmo::SCALE:
                m_pSelectedObj->Transform()->SetRelativeScale(sca);
                break;

            case ImGuizmo::UNIVERSAL:
                m_pSelectedObj->Transform()->SetRelativeScale(sca);
                m_pSelectedObj->Transform()->SetRelativeRotation(rot);
                m_pSelectedObj->Transform()->SetRelativePos(pos);
                break;
            default:
                break;
            }
        }
    }
}
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>

void CImGuiMgr::progress()
{
    //알림 확인
    ObserveContent();

    EDIT_MODE mode = CEditor::GetInst()->GetEditMode();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    if(EDIT_MODE::MAPTOOL == CEditor::GetInst()->GetEditMode())
        Guizmo();

    ParamUI::ParamCount = 0;

    {
        bool bTrue = true;
        ImGui::ShowDemoWindow(&bTrue);
        
        unordered_map<string, UI*>::iterator iter = m_mapUI.begin();
        for (; iter != m_mapUI.end(); ++iter)
        {
            switch (mode)
            {
            case EDIT_MODE::ANIMATOR:
                if ("Outliner" == iter->first)
                    iter->second->update();
                else if ("Inspector" == iter->first)
                    iter->second->update();
                else if ("MenuUI" == iter->first)
                    iter->second->update();
                else if ("AnimationUI" == iter->first)
                    iter->second->update();
                else if ("ContentUI" == iter->first)
                    iter->second->update();
                break;
            case EDIT_MODE::MAPTOOL:
                if ("##MenuUI" == iter->first)
                    iter->second->update();
                else if ("TileMapUI" == iter->first)
                    iter->second->update();
                else if ("ProgressUI" == iter->first)
                    iter->second->update();
                else if ("Outliner" == iter->first)
                    iter->second->update();
                else if ("Inspector" == iter->first)
                    iter->second->update();
                else if ("ContentUI" == iter->first)
                    iter->second->update();
                else if ("ListUI" == iter->first)
                    iter->second->update();
                else if ("ModelComUI" == iter->first)
                    iter->second->update();
                break;
            case EDIT_MODE::OBJECT:
                if ("Outliner" == iter->first)
                    iter->second->update();
                else if ("Inspector" == iter->first)
                    iter->second->update();
                else if ("ContentUI" == iter->first)
                    iter->second->update();
                else if ("MenuUI" == iter->first)
                    iter->second->update();
                else if ("ModelComUI" == iter->first)
                    iter->second->update();
                else if ("ListUI" == iter->first)
                    iter->second->update();
                else if ("ContentUI" == iter->first)
                    iter->second->update();
                break;
            case EDIT_MODE::END:
                break;
            default:
                break;
            }
        }       

        iter = m_mapUI.begin();
        for (; iter != m_mapUI.end(); ++iter)
        {
            switch (mode)
            {
            case EDIT_MODE::ANIMATOR:
                if ("Outliner" == iter->first)
                    iter->second->render();
                else if ("Inspector" == iter->first)
                    iter->second->render();
                else if ("##MenuUI" == iter->first)
                    iter->second->render();
                else if ("ListUI" == iter->first)
                    iter->second->render();
                else if ("AnimationUI" == iter->first)
                    iter->second->render();
                else if ("ContentUI" == iter->first)
                    iter->second->render();
                break;
            case EDIT_MODE::MAPTOOL:
                if ("##MenuUI" == iter->first)
                    iter->second->render();
                else if ("TileMapUI" == iter->first)
                    iter->second->render();
                else if ("ProgressUI" == iter->first)
                    iter->second->render();
                else if ("Outliner" == iter->first)
                    iter->second->render();
                else if ("Inspector" == iter->first)
                    iter->second->render();
                else if ("ContentUI" == iter->first)
                    iter->second->render();
                else if ("ListUI" == iter->first)
                    iter->second->render();
                else if ("ModelComUI" == iter->first)
                    iter->second->render();
                break;
            case EDIT_MODE::OBJECT:
                if ("Outliner" == iter->first)
                    iter->second->render();
                else if ("Inspector" == iter->first)
                    iter->second->render();
                //else if ("ContentUI" == iter->first)
                //    iter->second->render();
                else if ("##MenuUI" == iter->first)
                    iter->second->render();
                else if ("ModelComUI" == iter->first)
                    iter->second->render();
                else if ("ListUI" == iter->first)
                    iter->second->render();
                else if ("ContentUI" == iter->first)
                    iter->second->render();
                break;
            case EDIT_MODE::END:
                break;
            default:
                break;
            }
        }
    }

    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

}

void CImGuiMgr::clear()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}


#include "InspectorUI.h"
#include "ListUI.h"
#include "ProgressUI.h"
#include "OutlinerUI.h"
#include "ContentUI.h"
#include "DummyUI.h"
#include "MenuUI.h"
#include "TileMapUI.h"
#include "AnimationUI.h"
#include "CEditor.h"
#include "ModelComUI.h"

void CImGuiMgr::CreateUI()
{
    UI* pUI = nullptr;

    pUI = new InspectorUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new TileMapUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new OutlinerUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new ListUI;    
    pUI->SetModal(true);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new AnimationUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new MenuUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new ModelComUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    ProgressUI* progressui = new ProgressUI;
    progressui->SetModal(true);
    progressui->Close();
    m_mapUI.insert(make_pair(progressui->GetName(), progressui));

    /*
    * Content UI -> Progress UI 참조
    */
    pUI = new ContentUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));
    ((ContentUI*)pUI)->ReloadContent();
    ((ContentUI*)pUI)->ResetContent();
}

UI* CImGuiMgr::FindUI(const string& _name)
{
    unordered_map<string, UI*>::iterator iter =  m_mapUI.find(_name);
    if (iter == m_mapUI.end())
    {
        return nullptr;
    }

    return iter->second;
    
}

void CImGuiMgr::ObserveContent()
{
    DWORD dwWateState = WaitForSingleObject(m_NotifyHandle, 0);

    if (dwWateState == WAIT_OBJECT_0)
    {
        ContentUI* pContentUI = (ContentUI*)FindUI("ContentUI");
        pContentUI->ReloadContent();

        FindNextChangeNotification(m_NotifyHandle);
    }
}