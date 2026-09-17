#include "applicationclass.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")


// ----------------------------------------------------
// Shader Compile Error 표시용 함수
// ----------------------------------------------------
static void ShowShaderError(
    ID3DBlob* errorMessage,
    const char* title)
{
    if (errorMessage)
    {
        MessageBoxA(
            NULL,
            (char*)errorMessage->GetBufferPointer(),
            title,
            MB_OK | MB_ICONERROR
        );
    }
}


// ----------------------------------------------------
// 생성자
// ----------------------------------------------------
ApplicationClass::ApplicationClass()
{
    m_swapChain = nullptr;

    m_device = nullptr;

    m_deviceContext = nullptr;

    m_renderTargetView = nullptr;

    m_vertexBuffer = nullptr;

    m_vertexShader = nullptr;

    m_pixelShader = nullptr;

    m_inputLayout = nullptr;
}


// ----------------------------------------------------
// 복사 생성자
// ----------------------------------------------------
ApplicationClass::ApplicationClass(
    const ApplicationClass& other)
{
}


// ----------------------------------------------------
// 소멸자
// ----------------------------------------------------
ApplicationClass::~ApplicationClass()
{
}


// ----------------------------------------------------
// Application 초기화
// ----------------------------------------------------
bool ApplicationClass::Initialize(
    int screenWidth,
    int screenHeight,
    HWND hwnd)
{
    bool result;


    // 1. DirectX 기본 시스템 생성
    result = InitializeDirect3D(
        screenWidth,
        screenHeight,
        hwnd
    );

    if (!result)
    {
        return false;
    }


    // 2. 삼각형 생성
    result = InitializeTriangle();

    if (!result)
    {
        return false;
    }


    return true;
}


// ----------------------------------------------------
// 종료
// ----------------------------------------------------
void ApplicationClass::Shutdown()
{
    // 생성의 역순으로 제거
    ShutdownTriangle();

    ShutdownDirect3D();
}


// ----------------------------------------------------
// 매 프레임 실행
// ----------------------------------------------------
bool ApplicationClass::Frame()
{
    return Render();
}


// ----------------------------------------------------
// DirectX 11 초기화
// ----------------------------------------------------
bool ApplicationClass::InitializeDirect3D(
    int screenWidth,
    int screenHeight,
    HWND hwnd)
{
    HRESULT result;


    // ------------------------------------------------
    // 1. Swap Chain 설정
    // ------------------------------------------------

    DXGI_SWAP_CHAIN_DESC swapChainDesc;

    ZeroMemory(
        &swapChainDesc,
        sizeof(swapChainDesc)
    );


    // Back Buffer 1개
    swapChainDesc.BufferCount = 1;


    // 화면 크기
    swapChainDesc.BufferDesc.Width =
        screenWidth;

    swapChainDesc.BufferDesc.Height =
        screenHeight;


    // 화면 색상 포맷
    swapChainDesc.BufferDesc.Format =
        DXGI_FORMAT_R8G8B8A8_UNORM;


    // Refresh Rate
    swapChainDesc.BufferDesc.RefreshRate.Numerator =
        60;

    swapChainDesc.BufferDesc.RefreshRate.Denominator =
        1;


    // Back Buffer를 RenderTarget으로 사용
    swapChainDesc.BufferUsage =
        DXGI_USAGE_RENDER_TARGET_OUTPUT;


    // 출력할 Window
    swapChainDesc.OutputWindow =
        hwnd;


    // MSAA 사용 안 함
    swapChainDesc.SampleDesc.Count =
        1;

    swapChainDesc.SampleDesc.Quality =
        0;


    // 전체화면 또는 창 모드
    swapChainDesc.Windowed =
        FULL_SCREEN ? FALSE : TRUE;


    // 기본 Swap 방식
    swapChainDesc.SwapEffect =
        DXGI_SWAP_EFFECT_DISCARD;


    swapChainDesc.Flags = 0;



    // ------------------------------------------------
    // 2. DirectX Device / Context / SwapChain 생성
    // ------------------------------------------------

    D3D_FEATURE_LEVEL featureLevel;

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0
    };


    result =
        D3D11CreateDeviceAndSwapChain(
            nullptr,

            D3D_DRIVER_TYPE_HARDWARE,

            nullptr,

            0,

            featureLevels,

            1,

            D3D11_SDK_VERSION,

            &swapChainDesc,

            &m_swapChain,

            &m_device,

            &featureLevel,

            &m_deviceContext
        );


    if (FAILED(result))
    {
        MessageBoxA(
            hwnd,
            "DirectX 11 Device 생성에 실패했습니다.",
            "DirectX Error",
            MB_OK | MB_ICONERROR
        );

        return false;
    }



    // ------------------------------------------------
    // 3. SwapChain의 Back Buffer 가져오기
    // ------------------------------------------------

    ID3D11Texture2D* backBuffer = nullptr;


    result =
        m_swapChain->GetBuffer(
            0,

            __uuidof(ID3D11Texture2D),

            (void**)&backBuffer
        );


    if (FAILED(result))
    {
        return false;
    }



    // ------------------------------------------------
    // 4. Render Target View 생성
    // ------------------------------------------------

    result =
        m_device->CreateRenderTargetView(
            backBuffer,

            nullptr,

            &m_renderTargetView
        );


    // 이제 BackBuffer 포인터 자체는 필요 없음
    backBuffer->Release();

    backBuffer = nullptr;


    if (FAILED(result))
    {
        return false;
    }



    // ------------------------------------------------
    // 5. Render Target을 파이프라인에 연결
    // ------------------------------------------------

    m_deviceContext->OMSetRenderTargets(
        1,

        &m_renderTargetView,

        nullptr
    );



    // ------------------------------------------------
    // 6. Viewport 생성
    // ------------------------------------------------

    D3D11_VIEWPORT viewport;


    viewport.TopLeftX = 0.0f;

    viewport.TopLeftY = 0.0f;

    viewport.Width =
        static_cast<float>(screenWidth);

    viewport.Height =
        static_cast<float>(screenHeight);

    viewport.MinDepth = 0.0f;

    viewport.MaxDepth = 1.0f;



    // Viewport 적용
    m_deviceContext->RSSetViewports(
        1,

        &viewport
    );


    return true;
}


// ----------------------------------------------------
// 삼각형 생성
// ----------------------------------------------------
bool ApplicationClass::InitializeTriangle()
{
    HRESULT result;


    // ------------------------------------------------
    // 1. 삼각형 꼭짓점 3개 생성
    // ------------------------------------------------

    VertexType vertices[3];


    // 위쪽 정점 - 빨강
    vertices[0] =
    {
         0.0f,
         0.5f,
         0.0f,

         1.0f,
         0.0f,
         0.0f,
         1.0f
    };


    // 오른쪽 아래 - 초록
    vertices[1] =
    {
         0.5f,
        -0.5f,
         0.0f,

         0.0f,
         1.0f,
         0.0f,
         1.0f
    };


    // 왼쪽 아래 - 파랑
    vertices[2] =
    {
        -0.5f,
        -0.5f,
         0.0f,

         0.0f,
         0.0f,
         1.0f,
         1.0f
    };



    // ------------------------------------------------
    // 2. Vertex Buffer 설명
    // ------------------------------------------------

    D3D11_BUFFER_DESC vertexBufferDesc;

    ZeroMemory(
        &vertexBufferDesc,
        sizeof(vertexBufferDesc)
    );


    vertexBufferDesc.Usage =
        D3D11_USAGE_DEFAULT;


    vertexBufferDesc.ByteWidth =
        sizeof(VertexType) * 3;


    vertexBufferDesc.BindFlags =
        D3D11_BIND_VERTEX_BUFFER;


    vertexBufferDesc.CPUAccessFlags =
        0;



    // ------------------------------------------------
    // 3. Vertex Buffer에 들어갈 실제 데이터
    // ------------------------------------------------

    D3D11_SUBRESOURCE_DATA vertexData;

    ZeroMemory(
        &vertexData,
        sizeof(vertexData)
    );


    vertexData.pSysMem =
        vertices;



    // ------------------------------------------------
    // 4. 실제 GPU Vertex Buffer 생성
    // ------------------------------------------------

    result =
        m_device->CreateBuffer(
            &vertexBufferDesc,

            &vertexData,

            &m_vertexBuffer
        );


    if (FAILED(result))
    {
        return false;
    }



    // ------------------------------------------------
    // 5. Vertex Shader Compile
    // ------------------------------------------------

    ID3DBlob* vertexShaderBuffer =
        nullptr;

    ID3DBlob* pixelShaderBuffer =
        nullptr;

    ID3DBlob* errorMessage =
        nullptr;


    UINT shaderFlags =
        D3DCOMPILE_ENABLE_STRICTNESS;


#ifdef _DEBUG

    shaderFlags |=
        D3DCOMPILE_DEBUG |
        D3DCOMPILE_SKIP_OPTIMIZATION;

#endif


    result =
        D3DCompileFromFile(
            L"triangle.hlsl",

            nullptr,

            D3D_COMPILE_STANDARD_FILE_INCLUDE,

            "VSMain",

            "vs_5_0",

            shaderFlags,

            0,

            &vertexShaderBuffer,

            &errorMessage
        );


    if (FAILED(result))
    {
        ShowShaderError(
            errorMessage,
            "Vertex Shader Compile Error"
        );


        if (errorMessage)
        {
            errorMessage->Release();

            errorMessage = nullptr;
        }


        return false;
    }



    // ------------------------------------------------
    // 6. Vertex Shader 생성
    // ------------------------------------------------

    result =
        m_device->CreateVertexShader(
            vertexShaderBuffer->GetBufferPointer(),

            vertexShaderBuffer->GetBufferSize(),

            nullptr,

            &m_vertexShader
        );


    if (FAILED(result))
    {
        vertexShaderBuffer->Release();

        return false;
    }



    // ------------------------------------------------
    // 7. Pixel Shader Compile
    // ------------------------------------------------

    result =
        D3DCompileFromFile(
            L"triangle.hlsl",

            nullptr,

            D3D_COMPILE_STANDARD_FILE_INCLUDE,

            "PSMain",

            "ps_5_0",

            shaderFlags,

            0,

            &pixelShaderBuffer,

            &errorMessage
        );


    if (FAILED(result))
    {
        ShowShaderError(
            errorMessage,
            "Pixel Shader Compile Error"
        );


        if (errorMessage)
        {
            errorMessage->Release();

            errorMessage = nullptr;
        }


        vertexShaderBuffer->Release();


        return false;
    }



    // ------------------------------------------------
    // 8. Pixel Shader 생성
    // ------------------------------------------------

    result =
        m_device->CreatePixelShader(
            pixelShaderBuffer->GetBufferPointer(),

            pixelShaderBuffer->GetBufferSize(),

            nullptr,

            &m_pixelShader
        );


    if (FAILED(result))
    {
        vertexShaderBuffer->Release();

        pixelShaderBuffer->Release();


        return false;
    }



    // ------------------------------------------------
    // 9. Input Layout
    // ------------------------------------------------

    D3D11_INPUT_ELEMENT_DESC inputLayout[2];


    // POSITION
    inputLayout[0].SemanticName =
        "POSITION";

    inputLayout[0].SemanticIndex =
        0;

    inputLayout[0].Format =
        DXGI_FORMAT_R32G32B32_FLOAT;

    inputLayout[0].InputSlot =
        0;

    inputLayout[0].AlignedByteOffset =
        0;

    inputLayout[0].InputSlotClass =
        D3D11_INPUT_PER_VERTEX_DATA;

    inputLayout[0].InstanceDataStepRate =
        0;



    // COLOR
    inputLayout[1].SemanticName =
        "COLOR";

    inputLayout[1].SemanticIndex =
        0;

    inputLayout[1].Format =
        DXGI_FORMAT_R32G32B32A32_FLOAT;

    inputLayout[1].InputSlot =
        0;

    inputLayout[1].AlignedByteOffset =
        12;

    inputLayout[1].InputSlotClass =
        D3D11_INPUT_PER_VERTEX_DATA;

    inputLayout[1].InstanceDataStepRate =
        0;



    // ------------------------------------------------
    // 10. Input Layout 생성
    // ------------------------------------------------

    result =
        m_device->CreateInputLayout(
            inputLayout,

            2,

            vertexShaderBuffer->GetBufferPointer(),

            vertexShaderBuffer->GetBufferSize(),

            &m_inputLayout
        );


    // Compile Buffer는 이제 필요 없음
    vertexShaderBuffer->Release();

    vertexShaderBuffer = nullptr;


    pixelShaderBuffer->Release();

    pixelShaderBuffer = nullptr;


    if (FAILED(result))
    {
        return false;
    }


    return true;
}


// ----------------------------------------------------
// 삼각형 관련 자원 제거
// ----------------------------------------------------
void ApplicationClass::ShutdownTriangle()
{
    if (m_inputLayout)
    {
        m_inputLayout->Release();

        m_inputLayout = nullptr;
    }


    if (m_pixelShader)
    {
        m_pixelShader->Release();

        m_pixelShader = nullptr;
    }


    if (m_vertexShader)
    {
        m_vertexShader->Release();

        m_vertexShader = nullptr;
    }


    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();

        m_vertexBuffer = nullptr;
    }
}


// ----------------------------------------------------
// DirectX 자원 제거
// ----------------------------------------------------
void ApplicationClass::ShutdownDirect3D()
{
    if (m_swapChain)
    {
        m_swapChain->SetFullscreenState(
            FALSE,
            nullptr
        );
    }


    if (m_renderTargetView)
    {
        m_renderTargetView->Release();

        m_renderTargetView = nullptr;
    }


    if (m_swapChain)
    {
        m_swapChain->Release();

        m_swapChain = nullptr;
    }


    if (m_deviceContext)
    {
        m_deviceContext->Release();

        m_deviceContext = nullptr;
    }


    if (m_device)
    {
        m_device->Release();

        m_device = nullptr;
    }
}


// ----------------------------------------------------
// 렌더링
// ----------------------------------------------------
bool ApplicationClass::Render()
{
    // ------------------------------------------------
    // 1. 배경 Clear
    // ------------------------------------------------

    float clearColor[4] =
    {
        0.05f,
        0.05f,
        0.10f,
        1.0f
    };


    m_deviceContext->ClearRenderTargetView(
        m_renderTargetView,

        clearColor
    );



    // ------------------------------------------------
    // 2. Vertex Buffer를 Input Assembler에 연결
    // ------------------------------------------------

    UINT stride =
        sizeof(VertexType);

    UINT offset =
        0;


    m_deviceContext->IASetVertexBuffers(
        0,

        1,

        &m_vertexBuffer,

        &stride,

        &offset
    );


    // Input Layout 설정
    m_deviceContext->IASetInputLayout(
        m_inputLayout
    );


    // 3개 정점을 삼각형 하나로 해석
    m_deviceContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );



    // ------------------------------------------------
    // 3. Shader 설정
    // ------------------------------------------------

    m_deviceContext->VSSetShader(
        m_vertexShader,

        nullptr,

        0
    );


    m_deviceContext->PSSetShader(
        m_pixelShader,

        nullptr,

        0
    );



    // ------------------------------------------------
    // 4. 삼각형 그리기
    // ------------------------------------------------

    m_deviceContext->Draw(
        3,
        0
    );



    // ------------------------------------------------
    // 5. Back Buffer 화면에 출력
    // ------------------------------------------------

    HRESULT result =
        m_swapChain->Present(
            VSYNC_ENABLED ? 1 : 0,

            0
        );


    if (FAILED(result))
    {
        return false;
    }


    return true;
}