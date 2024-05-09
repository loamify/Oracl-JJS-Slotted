#include <Windows.h>
#include <stdio.h>
#include <string>
#include <imgui.h>
#pragma once
inline int aimkey = 2;



inline float playersize = 1;
inline bool enableexploits = false;
inline bool menu_open = false;
inline bool skip_knocked = false;
inline bool aimbot = true;
inline float smoothing = 15;
inline float long_range_smoothing = 15;
inline float close_range_smoothing = 15;
inline float randfrequency = 0;
inline float cachedelay = 3;
inline float limitingfactor = 200;
inline int target_bone = 0;
inline int fov = 250;
inline bool draw_fov = false;
inline bool aim_prediction = false;
inline bool visible_check = true;
inline bool triggerbot = true;
inline bool targetline = false;
inline bool rgb_esp = false;
inline bool mlm_esp = false;
inline bool furry_esp = false;
inline bool hitmarkers = false;


inline bool esp = true;
inline bool box_esp = false;
inline bool filled_box = false;
inline bool bonespots = false;
inline bool box_round = false;
inline bool box_corner = true;
inline bool line = false;
inline bool skeleton = false;
inline bool draw_username = false;
inline bool draw_platform = false;
inline bool draw_distance = false;
inline bool draw_held_weapon = false;
inline bool radar_esp = false;
inline bool lobby_esp = true;
inline int box_type = 0;
inline float filled_box_thickness = 2.5;
inline int max_esp_distance = 250;



inline bool draw_crosshair = false;
inline bool draw_fps = true;
inline bool draw_watermark = true;
inline float skel_thickness = 1;
inline float box_thickness = 1.5;
inline float linethickness = 1.5;

inline BOOL WritePrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName) {
	char lpString[1024];
	sprintf_s(lpString, sizeof(lpString), ("%d"), nInteger);
	return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}

inline BOOL WritePrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, float nInteger, LPCSTR lpFileName) {
	char lpString[1024];
	sprintf_s(lpString, sizeof(lpString), ("%f"), nInteger);
	return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}
inline float GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flDefault, LPCSTR lpFileName)
{
	char szData[32];

	GetPrivateProfileStringA(lpAppName, lpKeyName, std::to_string(flDefault).c_str(), szData, 32, lpFileName);

	return (float)atof(szData);
}
inline void savecfg(LPCSTR path)
{
	WritePrivateProfileInt(("a"), ("Aimbot"), aimbot, path);
	WritePrivateProfileInt(("a"), ("Triggerbot"), triggerbot, path);
	WritePrivateProfileInt(("a"), ("TargetLine"), targetline, path);
	WritePrivateProfileInt(("a"), ("RadarEsp"), radar_esp, path);
	WritePrivateProfileInt(("a"), ("DrawFov"), draw_fov, path);
	WritePrivateProfileFloat(("a"), ("FOV"), fov, path);
	WritePrivateProfileFloat(("a"), ("LRSmoothing"), long_range_smoothing, path);
	WritePrivateProfileFloat(("a"), ("CRSmoothing"), close_range_smoothing, path);
	WritePrivateProfileFloat(("a"), ("Randfrequency"), randfrequency, path);
	WritePrivateProfileFloat(("a"), ("Aimbone"), target_bone, path);


	WritePrivateProfileInt(("b"), ("LobbyEsp"), lobby_esp, path);
	WritePrivateProfileInt(("b"), ("Distance"), draw_distance, path);
	WritePrivateProfileInt(("b"), ("Userame"), draw_username, path);
	WritePrivateProfileInt(("b"), ("Lines"), line, path);
	WritePrivateProfileInt(("b"), ("FilledBox"), filled_box, path);
	WritePrivateProfileInt(("b"), ("EspBox"), box_esp, path);
	WritePrivateProfileInt(("b"), ("Platform"), draw_platform, path);
	WritePrivateProfileInt(("b"), ("CornerBox"), box_corner, path);
	WritePrivateProfileInt(("b"), ("Weapon"), draw_held_weapon, path);
	WritePrivateProfileFloat(("b"), ("Boxthick"), box_thickness, path);
	WritePrivateProfileFloat(("b"), ("Linethix"), linethickness, path);

	WritePrivateProfileFloat(("c"), ("DrawCrosshair"), draw_crosshair, path);


}
inline void loadcfg(LPCSTR path)
{
	aimbot = GetPrivateProfileIntA(("a"), ("Aimbot"), aimbot, path);
	triggerbot = GetPrivateProfileIntA(("a"), ("Triggerbot"), triggerbot, path);
	targetline = GetPrivateProfileIntA(("a"), ("TargetLine"), targetline, path);
	radar_esp = GetPrivateProfileIntA(("a"), ("RadarEsp"), radar_esp, path);
	draw_fov = GetPrivateProfileIntA(("a"), ("DrawFov"), draw_fov, path);
	fov = GetPrivateProfileFloat(("a"), ("FOV"), fov, path);
	long_range_smoothing = GetPrivateProfileFloat(("a"), ("LRSmoothing"), long_range_smoothing, path);
	close_range_smoothing = GetPrivateProfileFloat(("a"), ("CRSmoothing"), close_range_smoothing, path);
	randfrequency = GetPrivateProfileFloat(("a"), ("Randfrequency"), randfrequency, path);
	target_bone = GetPrivateProfileFloat(("a"), ("Aimbone"), target_bone, path);

	lobby_esp = GetPrivateProfileIntA(("b"), ("LobbyEsp"), lobby_esp, path);
	draw_distance = GetPrivateProfileIntA(("b"), ("Distance"), draw_distance, path);
	draw_username = GetPrivateProfileIntA(("b"), ("Userame"), draw_username, path);
	line = GetPrivateProfileIntA(("b"), ("Lines"), line, path);
	filled_box = GetPrivateProfileIntA(("b"), ("FilledBox"), filled_box, path);
	box_esp = GetPrivateProfileIntA(("b"), ("EspBox"), box_esp, path);
	draw_platform = GetPrivateProfileIntA(("b"), ("Platform"), draw_platform, path);
	box_corner = GetPrivateProfileIntA(("b"), ("CornerBox"), box_corner, path);
	draw_held_weapon = GetPrivateProfileIntA(("b"), ("Weapon"), draw_held_weapon, path);
	box_thickness = GetPrivateProfileFloat(("b"), ("Boxthick"), box_thickness, path);
	linethickness = GetPrivateProfileFloat(("b"), ("Linethix"), linethickness, path);

	draw_crosshair = GetPrivateProfileFloat(("c"), ("DrawCrosshair"), draw_crosshair, path);
}



static const char* keyNames[] =
{
	"Keybind",
	"Left Mouse",
	"Right Mouse",
	"Cancel",
	"Middle Mouse",
	"Mouse 5",
	"Mouse 4",
	"",
	"Backspace",
	"Tab",
	"",
	"",
	"Clear",
	"Enter",
	"",
	"",
	"Shift",
	"Control",
	"Alt",
	"Pause",
	"Caps",
	"",
	"",
	"",
	"",
	"",
	"",
	"Escape",
	"",
	"",
	"",
	"",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"",
	"",
	"",
	"Print",
	"Insert",
	"Delete",
	"",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"",
	"",
	"",
	"",
	"",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply",
	"Add",
	"",
	"Subtract",
	"Decimal",
	"Divide",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
};

inline static int keystatus = 0;


inline void ChangeKey(void* blank)
{
	keystatus = 1;
	while (true)
	{
		for (int i = 0; i < 0x87; i++)
		{
			if (GetKeyState(i) & 0x8000)
			{
				aimkey = i;
				keystatus = 0;
				return;
			}
		}
	}
}


inline static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = (const char* const*)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}

inline void HotkeyButton(int aimkey, void* changekey, int status)
{
	const char* preview_value = NULL;
	if (aimkey >= 0 && aimkey < IM_ARRAYSIZE(keyNames))
		Items_ArrayGetter(keyNames, aimkey, &preview_value);
	std::string aimkeys;
	if (preview_value == NULL)
		aimkeys = ("Select Key");
	else
		aimkeys = preview_value;

	if (status == 1)
	{

		aimkeys = ("Press the Key");
	}
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.4f));
	if (ImGui::Button(aimkeys.c_str(), ImVec2(70, 20)))
	{
		if (status == 0)
		{
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekey, nullptr, 0, nullptr);
		}
	}
	ImGui::PopStyleVar();
}
