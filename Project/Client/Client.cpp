// Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "Client.h"

#include "CEditor.h"
#include "CreateTestLevel.h"
#include <Engine/CDevice.h>
#include "CSaveLoadMgr.h"
#include <Engine\PhysXMgr.h>
#include "CImGuiMgr.h"

#include <Engine\CGameObject.h>
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

HINSTANCE g_hInst = nullptr;
HWND    g_hWnd = nullptr;
HHOOK   g_Hook = nullptr;
// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                D3DMemoryLeakCheck();
LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(115085);

    srand(0);

    g_hInst = hInstance;
    // 전역 문자열을 초기화합니다.
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;

    if (FAILED(CEngine::GetInst()->init(g_hWnd, 1600, 900)))
    {
        MessageBox(nullptr, L"엔진 초기화 실패", L"에러", MB_OK);
        return 0;
    }

    ::SetMenu(g_hWnd, NULL);

    CSaveLoadMgr::GetInst()->init();
    // ImGui 초기화  
    CImGuiMgr::GetInst()->init(g_hWnd);
    CreateTestLelvel();
    //GUI 주석
    CEditor::GetInst()->init();
    //Navi Mesh 로딩 후 init
    PhysXMgr::GetInst()->init();
    // 주석 끝
     // 기본 메시지 루프입니다:

     //g_Hook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, hInstance, NULL);
    vector<CGameObject*> vec{};

    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (WM_QUIT == msg.message)
                break;

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            CEngine::GetInst()->progress();
            //GUI 주석
            CEditor::GetInst()->progress();

            auto map = CEditor::GetInst()->GetEdiotrObj(EDIT_MODE::MAPTOOL);

            for (auto iter = map.begin(); iter != map.end(); ++iter)
            {
                vec.push_back((CGameObject*)iter->second);

                //const vector<CGameObject*>& childs = ((CGameObject*)iter->second)->GetChilds();
                
                //for (UINT i = 0; i < childs.size(); ++i)
                //    vec.push_back(childs[i]);
            }

            CEngine::GetInst()->render(vec);
            CEditor::GetInst()->debug_render();
            vec.clear();

            CImGuiMgr::GetInst()->progress();
            // 주석 끝
            // Present(SwapChain)
            CDevice::GetInst()->Present();
        }
    }
    UnhookWindowsHookEx(g_Hook);
    D3DMemoryLeakCheck();
    return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLIENT);
    wcex.lpszClassName = L"Client";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    g_hWnd = CreateWindowW(L"Client", L"Client", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!g_hWnd)
    {
        return FALSE;
    }

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;

    if (pMouseStruct != NULL)
    {
        if (wParam == WM_MOUSEWHEEL)
        {
            if (HIWORD(pMouseStruct->mouseData) == 120) g_ScrollUp = 1;
            else g_ScrollDown = 1;
        }
        if (wParam == WM_MBUTTONDOWN)
            g_ScrollClick = 1;

        //printf("Mouse position X = %d  Mouse Position Y = %d\n", pMouseStruct->pt.x, pMouseStruct->pt.y);
    }
    return CallNextHookEx(g_Hook, nCode, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            //const int dpi = HIWORD(wParam);
            //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;


    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}



void D3DMemoryLeakCheck()
{
    HMODULE dxgidebugdll = GetModuleHandleW(L"DXGIDebug.dll");
    decltype(&DXGIGetDebugInterface) GetDebugInterface = reinterpret_cast<decltype(&DXGIGetDebugInterface)>(GetProcAddress(dxgidebugdll, "DXGIGetDebugInterface"));
    IDXGIDebug* debug;
    GetDebugInterface(IID_PPV_ARGS(&debug));
    OutputDebugStringW(L"▽▽▽▽▽▽▽▽▽ Direct3D Object ref count 메모리 누수 체크 ▽▽▽▽▽▽▽▽▽\r\n");
      debug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
    OutputDebugStringW(L"△△△△△△△△△ 반환되지 않은 IUnkown 객체가 있을경우 위에 나타납니다. △△△△△△△△△\r\n");

    debug->Release();
    // decltype GetDebugInterface = 
}
