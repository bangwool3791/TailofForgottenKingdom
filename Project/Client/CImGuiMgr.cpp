#include "pch.h"
#include "CImGuiMgr.h"

#include <Engine\CDevice.h>

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
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

void CImGuiMgr::progress()
{
    //알림 확인
    ObserveContent();


    EDIT_MODE mode = CEditor::GetInst()->GetEditMode();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

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
                if ("Inspector" == iter->first)
                    iter->second->update();
                if ("ComInspector" == iter->first)
                    iter->second->update();
                if ("MenuUI" == iter->first)
                    iter->second->update();
                if ("ModelComUI" == iter->first)
                    iter->second->update();
                break;
            case EDIT_MODE::MAPTOOL:
                if ("TileMapUI" == iter->first)
                    iter->second->update();
                if ("##MenuUI" == iter->first)
                    iter->second->update();
                if ("ProgressUI" == iter->first)
                    iter->second->update();
                break;
            case EDIT_MODE::OBJECT:
                if ("Outliner" == iter->first)
                    iter->second->update();
                if ("Inspector" == iter->first)
                    iter->second->update();
                if ("ContentUI" == iter->first)
                    iter->second->update();
                if ("MenuUI" == iter->first)
                    iter->second->update();
                if ("ModelComUI" == iter->first)
                    iter->second->update();
                if ("ListUI" == iter->first)
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
                else if ("ComInspector" == iter->first)
                    iter->second->render();
                else if ("##MenuUI" == iter->first)
                    iter->second->render();
                else if ("ModelComUI" == iter->first)
                    iter->second->render();
                else if ("ListUI" == iter->first)
                    iter->second->render();
                break;
            case EDIT_MODE::MAPTOOL:
                if ("TileMapUI" == iter->first)
                    iter->second->render();
                else if ("##MenuUI" == iter->first)
                    iter->second->render();
                else if ("ProgressUI" == iter->first)
                    iter->second->render();
                break;
            case EDIT_MODE::OBJECT:
                if ("Outliner" == iter->first)
                    iter->second->render();
                else if ("Inspector" == iter->first)
                    iter->second->render();
                else if ("ContentUI" == iter->first)
                    iter->second->render();
                else if ("##MenuUI" == iter->first)
                    iter->second->render();
                else if ("ModelComUI" == iter->first)
                    iter->second->render();
                else if ("ListUI" == iter->first)
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
#include "ModelUI.h"
#include "ModelComUI.h"
#include "MenuUI.h"

#include "CEditor.h"

void CImGuiMgr::CreateUI()
{
    UI* pUI = nullptr;

    pUI = new InspectorUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new OutlinerUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new ContentUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));
    ((ContentUI*)pUI)->ReloadContent();
    ((ContentUI*)pUI)->ResetContent();

    pUI = new ListUI;    
    pUI->SetModal(true);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    //pUI = new ModelComUI;
    //m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new MenuUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    ProgressUI* progressui = new ProgressUI;
    progressui->SetModal(true);
    progressui->Close();
    m_mapUI.insert(make_pair(progressui->GetName(), progressui));
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