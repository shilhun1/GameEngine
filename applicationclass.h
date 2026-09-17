#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

#include <Windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>


const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;


class ApplicationClass
{
private:

    // 삼각형 정점 하나의 데이터 구조
    struct VertexType
    {
        float x;
        float y;
        float z;

        float r;
        float g;
        float b;
        float a;
    };


public:
    ApplicationClass();
    ApplicationClass(const ApplicationClass&);
    ~ApplicationClass();


    bool Initialize(
        int screenWidth,
        int screenHeight,
        HWND hwnd
    );

    void Shutdown();

    bool Frame();


private:

    // DirectX 기본 장치 생성
    bool InitializeDirect3D(
        int screenWidth,
        int screenHeight,
        HWND hwnd
    );

    // 삼각형 관련 데이터 생성
    bool InitializeTriangle();

    // DirectX 자원 해제
    void ShutdownDirect3D();

    // 삼각형 자원 해제
    void ShutdownTriangle();

    // 한 프레임 렌더링
    bool Render();


private:

    // DirectX 기본 객체
    IDXGISwapChain* m_swapChain;

    ID3D11Device* m_device;

    ID3D11DeviceContext* m_deviceContext;

    ID3D11RenderTargetView* m_renderTargetView;


    // 삼각형
    ID3D11Buffer* m_vertexBuffer;


    // Shader
    ID3D11VertexShader* m_vertexShader;

    ID3D11PixelShader* m_pixelShader;

    ID3D11InputLayout* m_inputLayout;
};


#endif