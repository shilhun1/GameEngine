#include "systemclass.h"


SystemClass::SystemClass()
{
    m_Input = 0;
    m_Application = 0;
}


SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}


bool SystemClass::Initialize()
{
    int screenWidth;
    int screenHeight;
    bool result;

    // 화면 크기 변수 초기화
    screenWidth = 0;
    screenHeight = 0;

    // Windows 창 생성
    InitializeWindows(screenWidth, screenHeight);

    // 입력 클래스 생성
    m_Input = new InputClass;

    // 입력 클래스 초기화
    m_Input->Initialize();

    // 그래픽/애플리케이션 클래스 생성
    m_Application = new ApplicationClass;

    // ApplicationClass 초기화
    result = m_Application->Initialize(
        screenWidth,
        screenHeight,
        m_hwnd
    );

    if (!result)
    {
        return false;
    }

    return true;
}


void SystemClass::Shutdown()
{
    // ApplicationClass 제거
    if (m_Application)
    {
        m_Application->Shutdown();

        delete m_Application;
        m_Application = 0;
    }

    // InputClass 제거
    if (m_Input)
    {
        delete m_Input;
        m_Input = 0;
    }

    // Windows 창 제거
    ShutdownWindows();

    return;
}


void SystemClass::Run()
{
    MSG msg;
    bool done;
    bool result;

    // 메시지 구조체 초기화
    ZeroMemory(&msg, sizeof(MSG));

    done = false;

    // 프로그램 메인 루프
    while (!done)
    {
        // Windows 메시지가 존재하는지 확인
        if (PeekMessage(
            &msg,
            NULL,
            0,
            0,
            PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Windows에서 종료 메시지가 왔다면 반복 종료
        if (msg.message == WM_QUIT)
        {
            done = true;
        }
        else
        {
            // 매 프레임 실행
            result = Frame();

            if (!result)
            {
                done = true;
            }
        }
    }

    return;
}


bool SystemClass::Frame()
{
    bool result;

    // ESC가 눌렸는지 확인
    if (m_Input->IsKeyDown(VK_ESCAPE))
    {
        return false;
    }

    // ApplicationClass의 프레임 실행
    result = m_Application->Frame();

    if (!result)
    {
        return false;
    }

    return true;
}


LRESULT CALLBACK SystemClass::MessageHandler(
    HWND hwnd,
    UINT umsg,
    WPARAM wparam,
    LPARAM lparam)
{
    switch (umsg)
    {
        // 키가 눌렸을 때
    case WM_KEYDOWN:
    {
        m_Input->KeyDown(
            (unsigned int)wparam
        );

        return 0;
    }

    // 키를 뗐을 때
    case WM_KEYUP:
    {
        m_Input->KeyUp(
            (unsigned int)wparam
        );

        return 0;
    }

    // 다른 Windows 메시지
    default:
    {
        return DefWindowProc(
            hwnd,
            umsg,
            wparam,
            lparam
        );
    }
    }
}


void SystemClass::InitializeWindows(
    int& screenWidth,
    int& screenHeight)
{
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;

    int posX;
    int posY;

    // 현재 SystemClass 객체를
    // 전역 포인터에 연결
    ApplicationHandle = this;

    // 현재 프로그램 인스턴스 가져오기
    m_hinstance = GetModuleHandle(NULL);

    // 프로그램/Window 이름
    m_applicationName = L"Engine";

    // Window Class 설정
    wc.style =
        CS_HREDRAW |
        CS_VREDRAW |
        CS_OWNDC;

    wc.lpfnWndProc = WndProc;

    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;

    wc.hInstance = m_hinstance;

    wc.hIcon =
        LoadIcon(NULL, IDI_WINLOGO);

    wc.hIconSm = wc.hIcon;

    wc.hCursor =
        LoadCursor(NULL, IDC_ARROW);

    wc.hbrBackground =
        (HBRUSH)GetStockObject(
            BLACK_BRUSH
        );

    wc.lpszMenuName = NULL;

    wc.lpszClassName =
        m_applicationName;

    wc.cbSize =
        sizeof(WNDCLASSEX);

    // Window Class 등록
    RegisterClassEx(&wc);

    // 현재 모니터 해상도 가져오기
    screenWidth =
        GetSystemMetrics(SM_CXSCREEN);

    screenHeight =
        GetSystemMetrics(SM_CYSCREEN);

    // 전체 화면
    if (FULL_SCREEN)
    {
        memset(
            &dmScreenSettings,
            0,
            sizeof(dmScreenSettings)
        );

        dmScreenSettings.dmSize =
            sizeof(dmScreenSettings);

        dmScreenSettings.dmPelsWidth =
            (unsigned long)screenWidth;

        dmScreenSettings.dmPelsHeight =
            (unsigned long)screenHeight;

        dmScreenSettings.dmBitsPerPel =
            32;

        dmScreenSettings.dmFields =
            DM_BITSPERPEL |
            DM_PELSWIDTH |
            DM_PELSHEIGHT;

        ChangeDisplaySettings(
            &dmScreenSettings,
            CDS_FULLSCREEN
        );

        posX = 0;
        posY = 0;
    }
    else
    {
        // 창 모드 크기
        screenWidth = 800;
        screenHeight = 600;

        // 화면 중앙에 배치
        posX =
            (GetSystemMetrics(SM_CXSCREEN)
                - screenWidth) / 2;

        posY =
            (GetSystemMetrics(SM_CYSCREEN)
                - screenHeight) / 2;
    }

    // 실제 Window 생성
    m_hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,

        m_applicationName,

        m_applicationName,

        WS_CLIPSIBLINGS |
        WS_CLIPCHILDREN |
        WS_POPUP,

        posX,
        posY,

        screenWidth,
        screenHeight,

        NULL,
        NULL,

        m_hinstance,

        NULL
    );

    // Window 표시
    ShowWindow(
        m_hwnd,
        SW_SHOW
    );

    // Window를 앞으로 가져오기
    SetForegroundWindow(m_hwnd);

    // 키보드 입력 포커스 지정
    SetFocus(m_hwnd);

    // 마우스 커서 숨김
    ShowCursor(false);

    return;
}


void SystemClass::ShutdownWindows()
{
    // 마우스 커서 다시 표시
    ShowCursor(true);

    // 전체 화면이었다면
    // Windows 화면 설정 복구
    if (FULL_SCREEN)
    {
        ChangeDisplaySettings(
            NULL,
            0
        );
    }

    // Window 제거
    DestroyWindow(m_hwnd);

    m_hwnd = NULL;

    // 등록한 Window Class 제거
    UnregisterClass(
        m_applicationName,
        m_hinstance
    );

    m_hinstance = NULL;

    // 전역 포인터 해제
    ApplicationHandle = NULL;

    return;
}


LRESULT CALLBACK WndProc(
    HWND hwnd,
    UINT umessage,
    WPARAM wparam,
    LPARAM lparam)
{
    switch (umessage)
    {
        // Window가 파괴될 때
    case WM_DESTROY:
    {
        PostQuitMessage(0);

        return 0;
    }

    // Window 종료 요청
    case WM_CLOSE:
    {
        PostQuitMessage(0);

        return 0;
    }

    // 나머지 메시지는 SystemClass로 전달
    default:
    {
        return ApplicationHandle->MessageHandler(
            hwnd,
            umessage,
            wparam,
            lparam
        );
    }
    }
}