#include "framework.h"
#include "ue5.h"  
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h> 
#include <vector>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#define M_PI 3.14159265358979323846
#define buffer_exceed_limit 0x650072006e0075  
#define draw ImGui::GetBackgroundDrawList()
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
struct camstruct
{
    f_vector location;
    f_vector rotation;
    float fov;
};
uintptr_t world;
uintptr_t lc;
uintptr_t lpw;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool CreateDeviceD3D(HWND hWnd)
{
    
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
} 
std::vector<f_vector> positions; 
f_vector m_rel;
camstruct m_viewpoint;

D3DXMATRIX matrixize(f_vector rot, f_vector origin = f_vector(0, 0, 0))
{
    float radPitch = (rot.x * float(M_PI) / 180.f);
    float radYaw = (rot.y * float(M_PI) / 180.f);
    float radRoll = (rot.z * float(M_PI) / 180.f);

    float SP = sinf(radPitch);
    float CP = cosf(radPitch);
    float SY = sinf(radYaw);
    float CY = cosf(radYaw);
    float SR = sinf(radRoll);
    float CR = cosf(radRoll);
    D3DMATRIX matrix;
    matrix.m[0][0] = CP * CY;
    matrix.m[0][1] = CP * SY;
    matrix.m[0][2] = SP;
    matrix.m[0][3] = 0.f;

    matrix.m[1][0] = SR * SP * CY - CR * SY;
    matrix.m[1][1] = SR * SP * SY + CR * CY;
    matrix.m[1][2] = -SR * CP;
    matrix.m[1][3] = 0.f;

    matrix.m[2][0] = -(CR * SP * CY + SR * SY);
    matrix.m[2][1] = CY * SR - CR * SP * SY;
    matrix.m[2][2] = CR * CP;
    matrix.m[2][3] = 0.f;

    matrix.m[3][0] = origin.x;
    matrix.m[3][1] = origin.y;
    matrix.m[3][2] = origin.z;
    matrix.m[3][3] = 1.f;

    return matrix;
}


camstruct get_camera()
{
    camstruct camera;

    auto location_pointer = r0->rpm<uintptr_t>(world + 0x110);
    auto rotation_pointer = r0->rpm<uintptr_t>(world + 0x120);

    struct FNRot
    {
        double a; //0x0000
        char pad_0008[24]; //0x0008
        double b; //0x0020
        char pad_0028[424]; //0x0028
        double c; //0x01D0
    }fnRot;

    fnRot.a = r0->rpm<double>(rotation_pointer);
    fnRot.b = r0->rpm<double>(rotation_pointer + 0x20);
    fnRot.c = r0->rpm<double>(rotation_pointer + 0x1d0);

    camera.location = r0->rpm<f_vector>(location_pointer);
    camera.rotation.x = asin(fnRot.c) * (180.0 / M_PI);
    camera.rotation.y = ((atan2(fnRot.a * -1, fnRot.b) * (180.0 / M_PI)) * -1) * -1;
    camera.fov = r0->rpm<float>((uintptr_t)lc + 0x394) * 83.f;

    return camera;
}

f_vector project(f_vector WorldLocation)
{
    D3DMATRIX tempMatrix = matrixize(m_viewpoint.rotation);
    f_vector vAxisX = f_vector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
    f_vector vAxisY = f_vector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
    f_vector vAxisZ = f_vector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);
    f_vector vDelta = WorldLocation - m_viewpoint.location;
    f_vector vTransformed = f_vector(vDelta.dot(vAxisY), vDelta.dot(vAxisZ), vDelta.dot(vAxisX));
    if (vTransformed.z < 1.f)  
        vTransformed.z = 1.f;
    return f_vector((1080 / 2.0f) + vTransformed.x * (((1080 / 2.0f) / tanf(m_viewpoint.fov * (float)M_PI / 360.f))) / vTransformed.z, (1920 / 2.0f) - vTransformed.y * (((1080 / 2.0f) / tanf(m_viewpoint.fov * (float)M_PI / 360.f))) / vTransformed.z, 0);
}
struct ent
{
    f_vector2d project;
    float dist;
    bool mate;
};
std::vector<ent> projected_ents;
uintptr_t him;
bool activate_cam;
void mem()
{ 
    while (true)
    {
        world = r0->rpm<uintptr_t>(r0->base + uworld);
        std::vector<ent> tmp;
        std::cout << skCrypt("> world returned 0x") << std::hex << world << std::endl;
        auto instance = r0->rpm<uintptr_t>(world + owning_game_instance);
        auto lp_ = r0->rpm<uintptr_t>(instance + local_players);
        auto lp = r0->rpm<uintptr_t>(lp_);
        lc = r0->rpm<uintptr_t>(lp + player_controller);
        lpw = r0->rpm<uintptr_t>(lc + acknowledged_pawn);
        if (!lpw)
        { 
            continue;
        }
        auto gamestate = r0->rpm<uintptr_t>(world + game_state);
        auto players = r0->rpm<uintptr_t>(gamestate + player_array);
        auto players_n = r0->rpm<int>(gamestate + (player_array + sizeof(uintptr_t)));
        std::cout << skCrypt("> gamestate returned  0x") << std::hex << gamestate << std::endl;
        m_viewpoint = get_camera();
        activate_cam = true;
        for (int i = 0; i < players_n; i++)
        {
            ent actor;
            him = r0->rpm<uintptr_t>(players + i * 0x8);
            uintptr_t t_him = r0->rpm<uintptr_t>(him + pawn_private);
            uintptr_t t_root = r0->rpm<uintptr_t>(t_him + root_component);
            f_vector t_rel = r0->rpm<f_vector>(t_root + relative_location);
            uintptr_t t_team = r0->rpm< uintptr_t>(him + team_index);
            uintptr_t m_stt = r0->rpm< uintptr_t>(lc +player_state);
            uintptr_t m_team = r0->rpm< uintptr_t>(him + team_index);
            uintptr_t m_root = r0->rpm<uintptr_t>(lpw + root_component);
            m_rel = r0->rpm<f_vector>(m_root + relative_location) ;  
            auto EntityPos = project(t_rel); 
            actor.dist = m_rel.distance(t_rel) / 100;
            actor.project = { (float)EntityPos.x, (float)EntityPos.y };
            if (m_team == t_team)  actor.mate = true;
            else  actor.mate = false;
            tmp.push_back(actor); 
        }
        projected_ents = tmp;
        Sleep(1);
    }
}

void update_view()
{ 
}
f_vector RotatePoint(f_vector EntityPos, f_vector LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, bool* viewCheck, bool angleInRadians)
{
    float r_1, r_2;
    float x_1, y_1;

    r_1 = -(EntityPos.y - LocalPlayerPos.y);
    r_2 = EntityPos.x - LocalPlayerPos.x;

    float Yaw = angle - 90.0f;

    float yawToRadian = Yaw * (float)(M_PI / 180.0F);
    x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
    y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;

    *viewCheck = y_1 < 0;

    int sizX = sizeX / 2;
    int sizY = sizeY / 2;

    x_1 += sizX;
    y_1 += sizY;

    if (x_1 < 5)
        x_1 = 5;

    if (x_1 > sizeX - 5)
        x_1 = sizeX - 5;

    if (y_1 < 5)
        y_1 = 5;

    if (y_1 > sizeY - 5)
        y_1 = sizeY - 5;


    x_1 += posX;
    y_1 += posY;


    return f_vector(x_1, y_1, 0);
}

void dodraw()
{

    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"  ", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    ImVec4 clear_color = ImVec4(0, 0, 0, 1);
    bool done = false;
    bool mates = false;
    bool opps = false;
    bool self = false;
    bool aim = false;
    bool radar = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            draw->AddText({ 15,15 }, ImColor(255, 255, 255), skCrypt("1.5 - shadow build\n"));
            draw->AddText({ 15,30 }, ImColor(255, 255, 255), skCrypt("[user] custom seed generated!\n[khv] obtained process peb\n[khv] unshuffled cr3"));

            ImGuiStyle& style = ImGui::GetStyle();
            ImVec2 oldPadding = style.WindowPadding;
            float oldAlpha = style.Colors[ImGuiCol_WindowBg].w;
            style.WindowPadding = ImVec2(0, 0);
            style.Colors[ImGuiCol_WindowBg].w = (float)255 / 255.0f; 

            ImGui::Begin(skCrypt("interface "));
            ImGui::Checkbox(skCrypt("radar"), &radar);
            ImGui::Checkbox(skCrypt("show opponents"), &opps);
            ImGui::Checkbox(skCrypt("show teammates"), &mates);
            ImGui::Checkbox(skCrypt("show self"), &self);
            ImGui::Separator();
            ImGui::Checkbox(skCrypt("aiming assistance "), &aim);
            ImGui::End();
            if (self)
            { 
                ImGui::GetBackgroundDrawList()->AddCircle({ 1080 / 2, 1920 / 2 }, 5, ImColor(0, 255, 0));
                ImGui::GetBackgroundDrawList()->AddText({ 1080 / 2, 1920 / 2 }, ImColor(255, 255, 255), skCrypt("me"));
            }
            auto pe = projected_ents; 
            if (radar && activate_cam)
            {
                for (auto b : pe)
                {
                    auto a = b.project;
                    if (!b.mate && mates)
                    {
                        ImGui::GetBackgroundDrawList()->AddCircle({ (float)a.x, (float)a.y }, 5, ImColor(0, 255, 0));
                        ImGui::GetBackgroundDrawList()->AddText({ (float)a.x, (float)a.y }, ImColor(255, 255, 255), skCrypt("teammate"));
                        ImGui::GetBackgroundDrawList()->AddText({ (float)a.x, (float)a.y + 15 }, ImColor(255, 255, 255), std::to_string(b.dist).c_str());
                    }
                    else if (b.mate && opps)
                    {
                        ImGui::GetBackgroundDrawList()->AddCircle({ (float)a.x, (float)a.y }, 5, ImColor(255, 0, 0));
                        ImGui::GetBackgroundDrawList()->AddText({ (float)a.x, (float)a.y }, ImColor(255, 255, 255), skCrypt("opponent"));
                        ImGui::GetBackgroundDrawList()->AddText({ (float)a.x, (float)a.y + 15 }, ImColor(255, 255, 255), std::to_string(b.dist).c_str());
                    }
                }
            }
        }
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); 
        g_pSwapChain->Present(0, 0); // Present without vsync   
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}
void start()
{

    COORD topLeft = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(console, topLeft);

    FILE* x;
    AllocConsole();
    freopen_s(&x, "CONOUT$", "w", stdout);
    SetConsoleTitleA(skCrypt("risku"));
    std::cout << skCrypt("> target shellcode linked to realtek callback executed at 0x") << std::hex << (uint64_t)(GetModuleHandleA(0)) << std::endl;
    std::cout << skCrypt("> pid request returned 0x") << std::hex << r0->procID(skCrypt("FortniteClient-Win64-Shipping.exe")) << std::endl;
    if (r0->get_fort_base()) 
    {
        std::cout << skCrypt("> image base returned 0x") << std::hex << r0->base << std::endl;
        std::thread(dodraw).detach();
        std::thread(update_view).detach();
        std::thread(mem).detach();
    }
}