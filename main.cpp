#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "Interface.h"
#include <tchar.h>
#include <imgui_internal.h>
#include <dwmapi.h>
#include "Xorstr.h"
#include "Json.h"
#include <fstream>
#include <ostream>
#include "Vuln.h"

namespace physmeme
{
    NTSTATUS __cdecl map_driver(std::vector<std::uint8_t>& raw_driver);
    NTSTATUS __cdecl map_driver(std::uint8_t* image, std::size_t size);
}

unsigned char driverdata[80024];
std::string ResponseSuccess = xorstr("PROTECTED_BY_EPIC_EOS");
std::string ResponseInvalid = xorstr("K8ODX2BG");
std::string ResponseInvalidUsername = xorstr("K8ODX2BA");
std::string ResponseExpired = xorstr("CV8ISVD2");
std::string ResponseNotRegistered = xorstr("LCMGDMRI");
std::string ResponseHwid = xorstr("W7Y371IL");
std::string ResponseAlreadyRegistered = xorstr("20KEWNC7");
std::string ResponseUserCheck = xorstr("USERCHECK");
std::string ResponseCountCheck = xorstr("COUNTCHECK");
std::string ValidateKey = xorstr("VALIDATE");
std::string key;
std::string DiscordBot = xorstr("A4D");
std::string SpooferVersion = xorstr("06");
std::string VMDetection = xorstr("VM8");

using namespace nlohmann;
using namespace boost::asio;
using namespace boost::asio::ip;


inline bool exists_test1(const std::string& name) {
    if (FILE* file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}


void writeBytesToFile(const unsigned char* bytes, size_t size, const std::string& filePath) {
    // Open the file in binary mode for writing
    std::ofstream outFile(filePath, std::ios::binary);

    if (!outFile.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    // Write the bytes to the file
    outFile.write(reinterpret_cast<const char*>(bytes), size);

    // Close the file
    outFile.close();

    if (outFile.fail()) {
        std::cerr << "Error writing to file: " << filePath << std::endl;
    }
    else {
        std::cout << "Bytes written to file: " << filePath << std::endl;
    }
}


int main(int, char**)
{
    io_service io_service;
    tcp::socket socket(io_service);
    socket.connect(tcp::endpoint(ip::address::from_string(xorstr("128.254.192.175")), 1337));
    bgvector = DownloadImage(("https://cdn.discordapp.com/attachments/1184162253537284188/1187935536556544011/bgImage.jpg"));
    //logovector = DownloadImage(("https://cdn.discordapp.com/attachments/1000572130091610195/1155574224312881242/360_F_142103920_HX5XxEAHvaHG4uP7YfgHMM05A25Jjm2q-removebg-preview.png"));
    logovector = DownloadImage(("https://cdn.discordapp.com/attachments/1186300910188646513/1203489282019561574/White-1.png"));
    mlmvector = DownloadImage(("https://cdn.discordapp.com/attachments/1184162253537284188/1192219346055991418/1017982.png"));

    std::wstring currentUserSID = GetCurrentUserSID();
    system(xorstr("reg add \"HKLM\\SOFTWARE\\Microsoft\\MemoryDiagnostic\" /v LastScanTime /t REG_QWORD /d 0 /f"));
    system(xorstr("sc start nvvhci"));
    system("cls");

    DEVMODE devMode;
    devMode.dmSize = sizeof(DEVMODE);

    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
    screen_width = devMode.dmPelsWidth;
    screen_height = devMode.dmPelsHeight;
    screen_center_x = screen_width / 2;
    screen_center_y = screen_height / 2;
    if (screen_width == 2560)
    {
        camfov = 83.0;
        dpi_scale = 2.0;
        dpi_settings = 2.0;
    }
    else
    {
        dpi_scale = 1.0;
        dpi_settings = 1.0;
        camfov = 90;
    }


    system("color d");
    std::cout << R"(

                                                              
                                                             
)";
    ResponseSuccess = DiscordBot + SpooferVersion + VMDetection;
    std::cout << xorstr("[*] Enter License: ");
    std::cin >> key;
    r0->InitComm();
    json licenses;

    licenses[xorstr("COMMANDBUFFER")] = { xorstr("VALIDATE"), key, currentUserSID };
    socket.write_some(buffer(to_string(licenses)));
    char data[80024];
    size_t length = socket.read_some(buffer(data));

    if (data == ResponseExpired)
    {
        std::cout << xorstr("[*] EXPIRED!");
    }
    else if (data == ResponseHwid)
    {
        std::cout << xorstr("[*] INVALID HWID!");
    }
    else if (data == ResponseInvalid)
    {
        std::cout << xorstr("[*] INVALID KEY!");
    }
    else if (data == ResponseSuccess)
    {

        if (!r0->init())
        {
            std::cout << xorstr("[*] DID NOT FIND EXISTING CONNECTION") << std::endl;

            if (exists_test1("C:\\Windows\\System32\\drivers\\nvvhci.sys"))
            {
                system("sc start nvvhci");
            }
            else
            {
                writeBytesToFile(rawData, sizeof(rawData), "C:\\Windows\\System32\\drivers\\nvvhci.sys");
                system("sc create nvvhci binPath= C:\\Windows\\System32\\drivers\\nvvhci.sys type= kernel");
                system("sc start nvvhci");
            }
            system("cls");
            system(xorstr("reg add \"HKLM\\SOFTWARE\\Microsoft\\MemoryDiagnostic\" /v LastScanTime /t REG_QWORD /d 0 /f"));
            json licenses2;
            licenses2[xorstr("COMMANDBUFFER")] = { xorstr("STARTSTREAM"), key, currentUserSID };
            socket.write_some(buffer(to_string(licenses2)));
            size_t length = socket.read_some(buffer(driverdata));
           // physmeme::map_driver(reinterpret_cast<std::uint8_t*>(driverdata), sizeof(driverdata));
            std::cout << xorstr("[*] LOADED BACKEND");
            r0->InitComm();
        }
        else
        {
            std::cout << "[*] FOUND EXISTING CONNECTION";
            r0->InitComm();
        }
        Sleep(1000);

        windowid = FindWindowA(0, ("Fortnite  "));
        r0->initsystem();
        ImGui_ImplWin32_EnableDpiAwareness();
        WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"TETHER", NULL };
        ::RegisterClassExW(&wc);
        MARGINS Margin = { -1 };

        hwnd = ::CreateWindowExW(WS_EX_LAYERED, wc.lpszClassName, _T("TETHER OVERLAY"), WS_VISIBLE | WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, wc.hInstance, NULL);
        //ITaskbarList* pTaskList = NULL;
        //HRESULT initRet = CoInitialize(NULL);
        //HRESULT createRet = CoCreateInstance(CLSID_TaskbarList,
        //	NULL,
        //	CLSCTX_INPROC_SERVER,
        //	IID_ITaskbarList,
        //	(LPVOID*)&pTaskList);

        //if (createRet == S_OK)
        //{
        //	pTaskList->DeleteTab(hwnd);
        //	pTaskList->Release();
        //}
        //CoUninitialize();
        //FreeConsole();
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

        DwmExtendFrameIntoClientArea(hwnd, &Margin);
        if (!CreateDeviceD3D(hwnd))
        {
            CleanupDeviceD3D();
            ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
            return 1;
        }

        ::ShowWindow(hwnd, SW_SHOW);
        ::UpdateWindow(hwnd);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        auto config = ImFontConfig();
        config.FontDataOwnedByAtlas = false;
        // config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
        static ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };
        io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 17 * dpi_scale);
        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
        ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
        io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 11 * dpi_scale, &icons_config, icons_ranges);
        Segoelarge = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 23.0f * dpi_scale);
        Segoemedium = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 17.0f * dpi_scale);
        Segoemedium = io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 12 * dpi_scale, &icons_config, icons_ranges);
        Segoemediumnotbold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 17.0f * dpi_scale);
        Segoesmallnotbold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 13.0f * dpi_scale);
        Segoelargenotbold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 42.0f * dpi_scale);
        Segoesmallnotbold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 12.0f * dpi_scale);

        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4 HighlightColor = ImVec4(255, 0, 0, 255);
        ImVec4 accent_color = ImVec4(255, 255, 255, 255);
        //auto const accent_color{ ui_settings.GetColorValue(UIColorType::Accent) };
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
        style.Colors[ImGuiCol_ChildBg] = ImColor(44, 44, 44, 255);
        style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(37.0 / 255.0, 42.0 / 255.0, 62.0 / 255.0, 1.00f);
        style.Colors[ImGuiCol_Border] = ImColor(44, 44, 44, 255);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(accent_color.x / 255.0, accent_color.y / 255.0, accent_color.z / 255.0, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImColor(251, 255, 255, 200);
        style.Colors[ImGuiCol_FrameBgActive] = ImColor(251, 255, 255, 255);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.05f, 0.05f, 0.79f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.0f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(251, 255, 255, 255);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(251, 255, 255, 255);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(251, 255, 255, 255);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(HighlightColor.x / 255.0, HighlightColor.y / 255.0, HighlightColor.z / 255.0, 0.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(HighlightColor.x / 255.0, HighlightColor.y / 255.0, HighlightColor.z / 255.0, 0.90f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(HighlightColor.x / 255.0, HighlightColor.y / 255.0, HighlightColor.z / 255.0, 0.70f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.12f, 0.12f, 0.12f, 0.04f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.12f, 0.12f, 0.12f, 0.04f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.12f, 0.12f, 0.04f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);

        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX9_Init(g_pd3dDevice);

        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        bool done = false;

        std::thread windowthread(windowpos);
        windowthread.detach();

        //std::thread gamecache(ActorQueue);
        //gamecache.detach();

        //_beginthread(update_actors, 0, nullptr);
        //_beginthread(find_actors, 0, nullptr);

        //std::thread itemcache(lootcachethread);
        //itemcache.detach();



        while (!done)
        {

            MSG msg;
            while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                    done = true;

            }
            if (done)
                break;

            ImGui_ImplDX9_NewFrame();
            ImGui_ImplWin32_NewFrame();
            if (GetAsyncKeyState(VK_F2) & 1) minimized = !minimized;
            if (GetAsyncKeyState(VK_F3) & 1)
            {
                //ShowWindow(hShellWnd, SW_SHOW);
                system(("reg add \"HKLM\\SOFTWARE\\Microsoft\\MemoryDiagnostic\" /v LastScanTime /t REG_QWORD /d 0 /f"));
                exit(0);
            }
            if (minimized)
            {
                if (windowblurred)
                {
                    SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_APPWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT);
                    windowblurred = false;
                }
            }
            else
            {
                if (!windowblurred)
                {
                    SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT);
                    windowblurred = true;
                }
            }

            if (dpi_changed)
            {
                io.Fonts->Clear();
                io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 17 * dpi_scale);
                static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
                ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
                io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 11 * dpi_scale, &icons_config, icons_ranges);
                Segoelarge = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 23.0f * dpi_scale);
                Segoemedium = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 17.0f * dpi_scale);
                Segoemedium = io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 12 * dpi_scale, &icons_config, icons_ranges);
                Segoemediumnotbold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 17.0f * dpi_scale);
                Segoesmallnotbold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 13.0f * dpi_scale);
                Segoelargenotbold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 42.0f * dpi_scale);
                Segoesmallnotbold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 12.0f * dpi_scale);
                io.Fonts->Build();
                ImGui_ImplDX9_CreateDeviceObjects();
                dpi_changed = false;
            }

            style.WindowPadding = ImVec2(0, 0);
            style.WindowBorderSize = 0;
            style.WindowRounding = 6 * dpi_scale;
            style.ChildRounding = 4 * dpi_scale;
            style.FrameRounding = 2 * dpi_scale;
            style.ItemInnerSpacing = ImVec2(12, 12);
            style.ScrollbarSize = 4.6f * dpi_scale;
            style.ItemSpacing = ImVec2(27 * dpi_scale, 0);
            style.ButtonTextAlign = ImVec2(0.5, 0.2);
            style.FrameBorderSize = 1;
            style.ScrollbarRounding = 0;

            RenderMenu();
            g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
            g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
            D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
            g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
            if (g_pd3dDevice->BeginScene() >= 0)
            {
                ImGui::Render();
                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
                g_pd3dDevice->EndScene();
            }
            HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

            if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
                ResetDevice();
        }

        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupDeviceD3D();
        ::DestroyWindow(hwnd);
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    }
    return 0;
}

