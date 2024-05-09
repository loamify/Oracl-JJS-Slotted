#pragma once
#include "ESP.h"
#include "FA.h"
#pragma comment (lib, "d3dx9.lib")


bool menu_active = true;
bool windowblurred = true;
bool dpi_changed = false;
bool minimized = false;

float dpi_scale = 1.f;
float dpi_settings = 1.f;
int tabs = 0;



static const char* bonename[] =
{
	"Head",
	"Neck",
	"Chest",
	"Pelvis"
};



static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void TabButton(const char* fmt, ImVec2 Pos, ImVec2 Size, int desired, int& range)
{
	ImGui::SetCursorPos(Pos);
	const auto& pForegroundDrawList = ImGui::GetForegroundDrawList();
	const auto& CurrentWindowPos = ImGui::GetWindowPos();
	ImVec2 P1, P2;
	ImDrawList* pDrawList;
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.25f, 0.4f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1 * dpi_scale);
	if (desired == range)
	{

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 1.0, 1.0, 0.2));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0, 1.0, 1.0, 0.5));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0, 1.0, 1.0, 0.2));


		P1 = ImVec2(Pos.x + 0 + dpi_scale, Pos.y - ImGui::GetScrollY() + 6 * dpi_scale);
		P1.x += CurrentWindowPos.x;
		P1.y += CurrentWindowPos.y;
		P2 = ImVec2(Pos.x + 3 * dpi_scale, Pos.y + Size.y * dpi_scale - ImGui::GetScrollY() - 6 * dpi_scale);
		P2.x += CurrentWindowPos.x;
		P2.y += CurrentWindowPos.y;
		pDrawList = pForegroundDrawList;
		//pDrawList->AddRectFilled(P1, P2, ImColor((float)251 / 255.0, (float)107 / 255.0, (float)144 / 255.0, 1.00f), 3.5 * dpi_scale);
		pDrawList->AddRectFilled(P1, P2, ImColor((float)255 / 255.0, (float)255 / 255.0, (float)255 / 255.0, 1.00f), 3.5 * dpi_scale);


	}
	else
	{

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 1.0, 1.0, 0.0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0, 1.0, 1.0, 0.5));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0, 1.0, 1.0, 0.2));

	}
	//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
	ImGui::Button(fmt, Size);
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
	if (ImGui::IsItemClicked())
	{
		range = desired;
	}

}


void RenderMenu()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::NewFrame();
	ActorLoop();
	Globals::local_camera = get_view_point();
	if (draw_watermark)
	{
		char fpsString[2048];
		snprintf(fpsString, sizeof(fpsString), ("JJS SLOTTED\nFPS: %.1f\nPLAYERS: %i\nF2: OPEN | CLOSE\nF3: EXIT"), io.Framerate, Globals::player_count);
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(10, 0), ImVec2(ImGui::CalcTextSize(fpsString).x + 20, ImGui::CalcTextSize(fpsString).y), ImColor(0, 0, 0, 100), 2 * dpi_scale);
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(10, 0), ImColor(231, 69, 112, 255), fpsString);
	}
	if (draw_fov)
	{
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - fov, ImGui::GetIO().DisplaySize.y / 2 - fov), ImVec2(ImGui::GetIO().DisplaySize.x / 2 + fov, ImGui::GetIO().DisplaySize.y / 2 + fov), ImColor(255, 255, 255), 4, 0, 1.3);
	}
	if (draw_crosshair)
	{
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2((ImGui::GetIO().DisplaySize.x / 2) - 8, (ImGui::GetIO().DisplaySize.y / 2) - 1), ImVec2((ImGui::GetIO().DisplaySize.x / 2) + 8, (ImGui::GetIO().DisplaySize.y / 2) - 1), ImColor(231, 69, 112, 255), 1);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2((ImGui::GetIO().DisplaySize.x / 2) - 1, (ImGui::GetIO().DisplaySize.y / 2) - 8), ImVec2((ImGui::GetIO().DisplaySize.x / 2) - 1, (ImGui::GetIO().DisplaySize.y / 2) + 8), ImColor(231, 69, 112, 255), 1);
	}

	if (!minimized)
	{
		ShowCursor(true);
		if (bg == nullptr) D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, bgvector.data(), bgvector.size(), 1920, 1080, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &bg);
		if (logo == nullptr) D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, logovector.data(), logovector.size(), 1920, 1080, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &logo);
		if (mlmtexture == nullptr) D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, mlmvector.data(), mlmvector.size(), 1920, 1080, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &mlmtexture);
		ImGui::SetNextWindowSize(ImVec2(720 * dpi_scale, 450 * dpi_scale));
		ImGui::Begin("JJS", &menu_active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		const auto& p = ImGui::GetWindowPos();
		const auto& pWindowDrawList = ImGui::GetWindowDrawList();
		const auto& pBackgroundDrawList = ImGui::GetBackgroundDrawList();
		const auto& pForegroundDrawList = ImGui::GetForegroundDrawList();
		ImGui::SetCursorPos(ImVec2(5 * dpi_scale, 10 * dpi_scale));
		ImGui::Image(logo, ImVec2(105 * dpi_scale, 70 * dpi_scale));
		pWindowDrawList->AddLine(ImVec2(p.x + 130 * dpi_scale, 0), ImVec2(p.x + 130 * dpi_scale, p.y + 450 * dpi_scale), ImColor(44, 44, 44, 255), 1.1 * dpi_scale);
		pWindowDrawList->AddLine(ImVec2(p.x + 530 * dpi_scale, 0), ImVec2(p.x + 530 * dpi_scale, p.y + 450 * dpi_scale), ImColor(44, 44, 44, 255), 1.1 * dpi_scale);

		ImGui::PushFont(Segoemedium);
		ImGui::SetCursorPos(ImVec2((650 * dpi_scale) / 2 - ImGui::CalcTextSize("JJS").x / 2, 10));
		ImGui::TextColored(ImColor(255, 255, 255, 255), "JJS");
		ImGui::PopFont();
		TabButton(ICON_FA_HOUSE"  Combat", ImVec2(10 * dpi_scale, 90 * dpi_scale), ImVec2(110, 25), 0, tabs);
		TabButton(ICON_FA_EYE"  Visuals", ImVec2(10 * dpi_scale, 120 * dpi_scale), ImVec2(110, 25), 1, tabs);
		TabButton(ICON_FA_EARTH_AMERICAS"  Objects", ImVec2(10 * dpi_scale, 150 * dpi_scale), ImVec2(110, 25), 2, tabs);
		TabButton(ICON_FA_GEAR"  Settings", ImVec2(10 * dpi_scale, 180 * dpi_scale), ImVec2(110, 25), 3, tabs);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.14f, 0.14f, 0.14f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.16f, 0.16f, 0.16f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgDisabled, ImVec4(0.43f, 0.43f, 0.43f, 1.0f));


		ImGui::SetCursorPos(ImVec2(540 * dpi_scale, 20 * dpi_scale));
		ImGui::BeginChild("##ChatDocker", ImVec2(170, 390), true, 0);
		ImGui::PushFont(Segoemedium);
		ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 5 * dpi_scale));
		ImGui::Text(ICON_FA_PEOPLE_GROUP);
		ImGui::SameLine();
		//const std::vector<std::string>& received_messages = c.getReceivedMessages();
		//std::string totalmessages = std::to_string(received_messages.size()) + " Total messages";
		//ImGui::Text(totalmessages.c_str());
		ImGui::PopFont();
		ImGui::EndChild();
		ImGui::SetCursorPos(ImVec2(540 * dpi_scale, 45 * dpi_scale));
		ImGui::BeginChild("##ChatTab2", ImVec2(170, 365), true, 0);
		ImGui::PushFont(Segoesmallnotbold);
		//for (const auto& message : received_messages)
		//{
		//	ImGui::Text(message.c_str());
		//}
		ImGui::PopFont();
		ImGui::EndChild();
		ImGui::SetCursorPos(ImVec2(540 * dpi_scale, 420 * dpi_scale));
		ImGui::SetNextItemWidth(140 * dpi_scale);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6 * dpi_settings);
		ImGui::PushFont(Segoemediumnotbold);
		//static char inputBuffer[chat_message::max_body_length + 1];
		//ImGui::InputText("##MessageInput", inputBuffer, sizeof(inputBuffer));
		ImGui::PopFont();
		ImGui::PopStyleVar();
		ImGui::SetCursorPos(ImVec2(690 * dpi_scale, 420 * dpi_scale));
		if (ImGui::Button(ICON_FA_ARROW_RIGHT, ImVec2(20, 20)))
		{
			//chat_message msg;
			//msg.body_length(std::strlen(inputBuffer));
			//std::memcpy(msg.body(), inputBuffer, msg.body_length());
			//msg.encode_header();
			//c.write(msg);
		}
		ImGui::PopStyleColor();
		pForegroundDrawList->AddRectFilledMultiColor(ImVec2(p.x + 540 * dpi_scale, p.y + 45 * dpi_scale), ImVec2(p.x + 700 * dpi_scale, p.y + 145 * dpi_scale), ImColor(44, 44, 44, 255), ImColor(44, 44, 44, 255), ImColor(44, 44, 44, 0), ImColor(44, 44, 44, 0));

		if (tabs == 0)
		{
			ImGui::SetCursorPos(ImVec2(140 * dpi_scale, 60 * dpi_scale));
			ImGui::BeginChild("##AimTab", ImVec2(185, 130), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::PushFont(Segoemedium);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 8 * dpi_scale));
			ImGui::TextColored(ImColor(251, 255, 255, 255), ICON_FA_WRENCH);
			//ImGui::TextColored(ImColor(251, 107, 144, 255), ICON_FA_WRENCH);
			ImGui::SameLine();
			ImGui::Text("General");
			ImGui::PopFont();

			ImGui::PushFont(Segoesmallnotbold);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 40 * dpi_scale));
			ImGui::Text("Hotkey");
			ImGui::SetCursorPos(ImVec2(100 * dpi_scale, 38 * dpi_scale));
			HotkeyButton(aimkey, ChangeKey, keystatus);

			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("TriggerBot");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Enable Triggerbot"), &triggerbot);



			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Humanization");
			ImGui::SameLine();
			ImGui::SetCursorPosX(90 * dpi_scale);
			ImGui::SetNextItemWidth(120 * dpi_scale);
			ImGui::SliderFloat(("##Randomization Scale:"), &randfrequency, 0, 4);
			ImGui::PopFont();
			ImGui::EndChild();


			ImGui::SetCursorPos(ImVec2(140 * dpi_scale, 200 * dpi_scale));
			ImGui::BeginChild("##AimTab2", ImVec2(185, 130), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::PushFont(Segoemedium);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 8 * dpi_scale));
			ImGui::TextColored(ImColor(251, 255, 255, 255), ICON_FA_SKULL_CROSSBONES);
			ImGui::SameLine();
			ImGui::Text("Aimbone");
			ImGui::PopFont();
			ImGui::PushFont(Segoesmallnotbold);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 40 * dpi_scale));
			ImGui::Text("Current Bone:");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale - ImGui::CalcTextSize(bonename[target_bone]).x / 2);
			ImGui::Text(bonename[target_bone]);

			ImGui::SetCursorPosX(10 * dpi_scale);
			if (ImGui::Button("Head", ImVec2(40, 15)))
			{
				target_bone = 0;
			}
			ImGui::SameLine();
			if (ImGui::Button("Neck", ImVec2(40, 15)))
			{
				target_bone = 1;
			}
			ImGui::SameLine();
			if (ImGui::Button("Chest", ImVec2(40, 15)))
			{
				target_bone = 2;
			}
			ImGui::SetCursorPosX(10 * dpi_scale);
			if (ImGui::Button("Pelvis", ImVec2(40, 15)))
			{
				target_bone = 3;
			}
			ImGui::PopFont();

			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(335 * dpi_scale, 60 * dpi_scale));
			ImGui::BeginChild("##AimTab3", ImVec2(185, 300), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::PushFont(Segoemedium);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 8 * dpi_scale));
			ImGui::TextColored(ImColor(251, 255, 255, 255), ICON_FA_LIST);
			ImGui::SameLine();
			ImGui::Text("Customization");
			ImGui::PopFont();

			ImGui::PushFont(Segoesmallnotbold);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 40 * dpi_scale));
			ImGui::Text("Target Line");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Target Line"), &targetline);

			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Show FOV");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Show FOV"), &draw_fov);

			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 90 * dpi_scale));
			ImGui::Text("Aim Easing");
			ImGui::SameLine();
			ImGui::SetCursorPosX(90 * dpi_scale);
			ImGui::SetNextItemWidth(120 * dpi_scale);
			ImGui::SliderFloat(("##Smoothing"), &long_range_smoothing, 8, 30);

			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 115 * dpi_scale));
			ImGui::Text("Close Range");
			ImGui::SameLine();
			ImGui::SetCursorPosX(90 * dpi_scale);
			ImGui::SetNextItemWidth(120 * dpi_scale);
			ImGui::SliderFloat(("##CRSmoothing"), &close_range_smoothing, 2, 30);


			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 140 * dpi_scale));
			ImGui::Text("Aim Radius");
			ImGui::SameLine();
			ImGui::SetCursorPosX(90 * dpi_scale);
			ImGui::SetNextItemWidth(120 * dpi_scale);
			ImGui::SliderInt(("##FOVRADIUS"), &fov, 25, 400);


			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 165 * dpi_scale));
			ImGui::Text("W2S FOV");
			ImGui::SameLine();
			ImGui::SetCursorPosX(90 * dpi_scale);
			ImGui::SetNextItemWidth(120 * dpi_scale);
			ImGui::SliderFloat(("##camfov"), &camfov, 70, 100);

			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 190 * dpi_scale));
			ImGui::Text("Projectile Prediction");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##AimPrediction"), &aim_prediction);


			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Visible Check");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##VisCheck"), &visible_check);

			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Team Check");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Lobby ESP / Ignore Team"), &lobby_esp);

			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Skip Downed");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Skip Downed"), &skip_knocked);

			ImGui::PopFont();
			ImGui::EndChild();
		}
		else if (tabs == 1)
		{
			ImGui::SetCursorPos(ImVec2(140 * dpi_scale, 60 * dpi_scale));
			ImGui::BeginChild("##ESPTab", ImVec2(185, 100), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::PushFont(Segoemedium);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 8 * dpi_scale));
			ImGui::TextColored(ImColor(251, 255, 255, 255), ICON_FA_WRENCH);
			ImGui::SameLine();
			ImGui::Text("General");
			ImGui::PopFont();


			ImGui::PushFont(Segoesmallnotbold);


			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 40 * dpi_scale));
			//ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Refresh Delay");
			ImGui::SameLine();
			ImGui::SetCursorPosX(90 * dpi_scale);
			ImGui::SetNextItemWidth(120 * dpi_scale);
			ImGui::SliderFloat(("##CacheDelay:"), &cachedelay, 3, 8);

			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 65 * dpi_scale));
			ImGui::Text("ESP Distance");
			ImGui::SameLine();
			ImGui::SetCursorPosX(90 * dpi_scale);
			ImGui::SetNextItemWidth(120 * dpi_scale);
			ImGui::SliderFloat(("##ESPDISTANCE:"), &limitingfactor, 200, 700);

			ImGui::PopFont();
			ImGui::EndChild();


			ImGui::SetCursorPos(ImVec2(335 * dpi_scale, 60 * dpi_scale));
			ImGui::BeginChild("##ESPTab2", ImVec2(185, 320), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::PushFont(Segoemedium);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 8 * dpi_scale));
			ImGui::TextColored(ImColor(251, 255, 255, 255), ICON_FA_LIST);
			ImGui::SameLine();
			ImGui::Text("Customization");
			ImGui::PopFont();

			ImGui::PushFont(Segoesmallnotbold);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 40 * dpi_scale));
			ImGui::Text("Draw Distance");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Draw Distance"), &draw_distance);

			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Draw Username");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Draw Username"), &draw_username);


			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Draw Snaplines");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Draw Snaplines"), &line);

			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 115 * dpi_scale));
			ImGui::Text("Line Thick");
			ImGui::SameLine();
			ImGui::SetCursorPosX(90 * dpi_scale);
			ImGui::SetNextItemWidth(120 * dpi_scale);
			ImGui::SliderFloat(("##Line Thickness"), &smoothing, 1, 4);

			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 140 * dpi_scale));
			ImGui::Text("Draw Filled Box");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Draw Filled Box"), &filled_box);

			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Draw ESP Box");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Draw ESP Box"), &box_esp);


			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 190 * dpi_scale));
			ImGui::Text("Box Thickness");
			ImGui::SameLine();
			ImGui::SetCursorPosX(90 * dpi_scale);
			ImGui::SetNextItemWidth(120 * dpi_scale);
			ImGui::SliderFloat(("##Box Thickness"), &box_thickness, 1, 4);

			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 215 * dpi_scale));
			ImGui::Text("Draw Platform");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Draw Platform"), &draw_platform);

			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Draw Corner Box");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Draw Corner Box"), &box_corner);

			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Draw Weapon");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Draw Weapon"), &draw_held_weapon);

			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Draw Radar");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Draw Radar"), &radar_esp);

			ImGui::PopFont();
			ImGui::EndChild();


			ImGui::SetCursorPos(ImVec2(140 * dpi_scale, 170 * dpi_scale));
			ImGui::BeginChild("##ESPTab3", ImVec2(185, 150), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::PushFont(Segoemedium);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 8 * dpi_scale));
			ImGui::TextColored(ImColor(251, 255, 255, 255), ICON_FA_PAINTBRUSH);
			ImGui::SameLine();
			ImGui::Text("Effects");
			ImGui::PopFont();
			ImGui::PushFont(Segoesmallnotbold);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 40 * dpi_scale));
			ImGui::Text("RGB ESP");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##RGB ESP"), &rgb_esp);

			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Gay Pride ESP");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##MLM ESP"), &mlm_esp);

			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Furry Pride ESP");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Fur ESP"), &furry_esp);

			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Hitmarkers");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Hitmarkers"), &hitmarkers);
			ImGui::PopFont();

			ImGui::EndChild();
		}

		else if (tabs == 3)
		{
			ImGui::SetCursorPos(ImVec2(140 * dpi_scale, 60 * dpi_scale));
			ImGui::BeginChild("##SettingsTab", ImVec2(185, 100), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::PushFont(Segoemedium);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 8 * dpi_scale));
			ImGui::TextColored(ImColor(251, 255, 255, 255), ICON_FA_TOOLBOX);
			ImGui::SameLine();
			ImGui::Text("Config");
			ImGui::PopFont();


			ImGui::PushFont(Segoesmallnotbold);


			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 40 * dpi_scale));
			if (ImGui::Button("Load Config", ImVec2(160, 20)))
			{
				loadcfg((("C:\\thtr")));
			}
			ImGui::SetCursorPosX(10 * dpi_scale);
			if (ImGui::Button("Save Config", ImVec2(160, 20)))
			{
				savecfg((("C:\\thtr")));
			}

			ImGui::PopFont();
			ImGui::EndChild();



			ImGui::SetCursorPos(ImVec2(335 * dpi_scale, 60 * dpi_scale));
			ImGui::BeginChild("##SettingsTab2", ImVec2(185, 90), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::PushFont(Segoemedium);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 8 * dpi_scale));
			ImGui::TextColored(ImColor(251, 255, 255, 255), ICON_FA_CROSSHAIRS);
			ImGui::SameLine();
			ImGui::Text("Cheat Related");
			ImGui::PopFont();

			ImGui::PushFont(Segoesmallnotbold);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 40 * dpi_scale));
			ImGui::Text("Watermark");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Watermark"), &draw_watermark);

			ImGui::SetCursorPosX(10 * dpi_scale);
			ImGui::Text("Draw Crosshair");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##Draw Crosshair"), &draw_crosshair);

			ImGui::PopFont();
			ImGui::EndChild();



			ImGui::SetCursorPos(ImVec2(140 * dpi_scale, 170 * dpi_scale));
			ImGui::BeginChild("##SettingsTab3", ImVec2(185, 120), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::PushFont(Segoemedium);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 8 * dpi_scale));
			ImGui::TextColored(ImColor(251, 255, 255, 255), ICON_FA_CODE);
			ImGui::SameLine();
			ImGui::Text("UI Settings");
			ImGui::PopFont();
			ImGui::PushFont(Segoesmallnotbold);
			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 40 * dpi_scale));
			ImGui::Text("UI Audio");
			ImGui::SameLine();
			ImGui::SetCursorPosX(155 * dpi_scale);
			ImGui::Checkbox(("##UI Audio"), &rgb_esp);


			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 65 * dpi_scale));
			ImGui::Text("UI Scale");
			ImGui::SameLine();
			ImGui::SetCursorPosX(90 * dpi_scale);
			ImGui::SetNextItemWidth(120 * dpi_scale);
			ImGui::SliderFloat(("##DPI Scake"), &dpi_settings, 1, 3);

			ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 90 * dpi_scale));
			if (ImGui::Button("Apply Scale", ImVec2(160, 20)))
			{
				if (dpi_scale != dpi_settings)
				{
					dpi_scale = dpi_settings; dpi_changed = true;
				}
			};

			ImGui::PopFont();

			ImGui::EndChild();
		}

		ImGui::PopStyleColor(4);
		ImGui::End();
	}


	ImGui::EndFrame();
}


bool isMyWindowActive(HWND hwnd) {
	HWND foregroundWindow = GetForegroundWindow();
	return foregroundWindow == hwnd;
}


void windowpos()
{
	while (true)
	{
		if (isMyWindowActive(windowid))
		{
			SetWindowPos(windowid, hwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			//ShowWindow(hShellWnd, SW_HIDE);
		}
		Sleep(400);
	}
}

bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	return true;
}

void CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
