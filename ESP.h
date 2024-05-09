#pragma once
#include "Cache.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <curl/curl.h>

IDirect3DTexture9* logo = nullptr;
IDirect3DTexture9* mlmtexture = nullptr;
IDirect3DTexture9* bg = nullptr;

std::vector<unsigned char> bgvector;
std::vector<unsigned char> logovector;
std::vector<unsigned char> mlmvector;

ImFont* Segoelarge;
ImFont* Segoelargenotbold;
ImFont* Segoemedium;
ImFont* Segoesmallnotbold;
ImFont* Segoemediumnotbold;
ImFont* SegoeEmj;
ImFont* FontAwesome;

void draw_cornered_box(int x, int y, int w, int h, const ImColor color, int thickness)
{
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, thickness);
}

void draw_filled_rect(int x, int y, int w, int h, const ImColor color)
{
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}
//
//void draw_mlm_rect(int x, int y, int w, int h)
//{
//	ImGui::GetBackgroundDrawList()->AddImage(mlmtexture, ImVec2(x, y), ImVec2(x + w, y + h));
//}

void draw_line(Vector2 origin, Vector2 target, const ImColor color)
{
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(origin.x, origin.y), ImVec2(target.x, target.y), color, linethickness);
}

void draw_distancevoid(Vector2 location, float distance, const ImColor color)
{
	char dist[64];
	sprintf_s(dist, "%.fm", distance);
	ImVec2 text_size = ImGui::CalcTextSize(dist);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - text_size.x / 2, location.y - text_size.y / 2), color, dist);
}

void skeletonesp(uintptr_t mesh, float distance)
{
	const int color = ImColor(255, 255, 255);
	if (mesh)
	{
		Vector2 bonePositions[] = {
	project_world_to_screen(get_entity_bone(mesh, 109)),  // HeadBone
	project_world_to_screen(get_entity_bone(mesh, 2)),   // Hip
	project_world_to_screen(get_entity_bone(mesh, 66)),  // Neck
	project_world_to_screen(get_entity_bone(mesh, 9)),   // UpperArmLeft
	project_world_to_screen(get_entity_bone(mesh, 38)),  // UpperArmRight
	project_world_to_screen(get_entity_bone(mesh, 10)),  // LeftHand
	project_world_to_screen(get_entity_bone(mesh, 39)),  // RightHand
	project_world_to_screen(get_entity_bone(mesh, 11)),  // LeftHand1
	project_world_to_screen(get_entity_bone(mesh, 40)),  // RightHand1
	project_world_to_screen(get_entity_bone(mesh, 78)),  // RightThigh
	project_world_to_screen(get_entity_bone(mesh, 71)),  // LeftThigh
	project_world_to_screen(get_entity_bone(mesh, 79)),  // RightCalf
	project_world_to_screen(get_entity_bone(mesh, 72)),  // LeftCalf
	project_world_to_screen(get_entity_bone(mesh, 75)),  // LeftFoot 
	project_world_to_screen(get_entity_bone(mesh, 82))   // RightFoot
		};


		ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(bonePositions[0].x, bonePositions[0].y), 200 / distance, color, 128, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[0].x, bonePositions[0].y + 200 / distance), ImVec2(bonePositions[2].x, bonePositions[2].y), color, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[1].x, bonePositions[1].y), ImVec2(bonePositions[2].x, bonePositions[2].y), color, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[3].x, bonePositions[3].y), ImVec2(bonePositions[2].x, bonePositions[2].y), color, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[4].x, bonePositions[4].y), ImVec2(bonePositions[2].x, bonePositions[2].y), color, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[5].x, bonePositions[5].y), ImVec2(bonePositions[3].x, bonePositions[3].y), color, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[6].x, bonePositions[6].y), ImVec2(bonePositions[4].x, bonePositions[4].y), color, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[5].x, bonePositions[5].y), ImVec2(bonePositions[7].x, bonePositions[7].y), color, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[6].x, bonePositions[6].y), ImVec2(bonePositions[8].x, bonePositions[8].y), color, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[10].x, bonePositions[10].y), ImVec2(bonePositions[1].x, bonePositions[1].y), color, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[9].x, bonePositions[9].y), ImVec2(bonePositions[1].x, bonePositions[1].y), color, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[12].x, bonePositions[12].y), ImVec2(bonePositions[10].x, bonePositions[10].y), color, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[11].x, bonePositions[11].y), ImVec2(bonePositions[9].x, bonePositions[9].y), color, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[13].x, bonePositions[13].y), ImVec2(bonePositions[12].x, bonePositions[12].y), color, 1.7f);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositions[14].x, bonePositions[14].y), ImVec2(bonePositions[11].x, bonePositions[11].y), color, 1.7f);
	}
}


float zoom;
ImVec2 siz = ImVec2(300, 300);
ImVec2 pos = ImVec2(10, 40);
ImColor VisibleColor;
ImColor InVisibleColor;
inline Actors closest_actor;


int iterationCount = 0;
float randoffset;
void aimbotvoid(uintptr_t target_mesh, Actors actor)
{

	if (!target_mesh) return;

	if (visible_check)
	{
		if (!is_visible(target_mesh)) return;
	}
	Vector3 head3d = get_entity_bone(target_mesh, UsedBone[target_bone]);
	Vector2 head2d = project_world_to_screen(head3d);
	float distance = Globals::relative_location.distance(head3d) / 100;
	if (distance < 30)
	{
		smoothing = close_range_smoothing;
	}
	else
	{
		smoothing = long_range_smoothing;
	}
	Vector2 target{};
	if ((int)head2d.x != 0)
	{
		if ((int)head2d.x > screen_center_x)
		{
			target.x = -(screen_center_x - head2d.x);
			target.x /= smoothing;
			if ((int)target.x + screen_center_x > screen_center_x * 2) target.x = 0;
		}
		if ((int)head2d.x < (int)screen_center_x)
		{
			target.x = head2d.x - screen_center_x;
			target.x /= smoothing;
			if ((int)target.x + screen_center_x < 0) target.x = 0;
		}
	}
	if ((int)head2d.y != 0)
	{
		if ((int)head2d.y > screen_center_y)
		{
			target.y = -(screen_center_y - head2d.y);
			target.y /= smoothing;
			if ((int)target.y + screen_center_y > screen_center_y * 2) target.y = 0;
		}
		if ((int)head2d.y < screen_center_y)
		{
			target.y = head2d.y - screen_center_y;
			target.y /= smoothing;
			if ((int)target.y + screen_center_y < 0) target.y = 0;
		}
	}

	mouse_event(MOUSEEVENTF_MOVE, randoffset + (int)target.x, randoffset + (int)target.y, 0, 0);
	//MouseController::Move_Mouse(randoffset + (int)target.x, randoffset + (int)target.y);

	HWND hwnd = GetForegroundWindow();
	if (triggerbot && is_visible && IsTargetUnderReticle(actor)) {
		PostMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(target.x, target.y));
		PostMessage(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(target.x, target.y));
	}

	if (targetline) {
		if (filled_box)
		{
			ImColor Color2 = ImColor(0, 0, 0, 255);
			int thickk = linethickness + 2;
		}
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(screen_width / 2, screen_height / 2), ImVec2(head2d.x, head2d.y), ImColor(255, 0, 255), linethickness);
	}
}


auto ActorLoop() -> void {
	//std::lock_guard<std::shared_mutex> lock(entitiesMutex);
	//std::lock_guard<std::shared_mutex> lock2(lootentitiesMutex);
	//std::lock_guard<std::mutex> lock(newentitiesMutex);
	//std::vector <Actors>entitycopies = PlayerAddresses;

	//try
	//{
	//	entitycopies = entities;
	//}
	//catch(...)
	//{
	//	entitycopies;
	//}


	Globals::uworld = r0->rpm<uintptr_t>(r0->base + UWORLD);

	std::cout << r0->base << std::endl;
	std::cout << Globals::uworld << std::endl;

	Globals::game_instance = r0->rpm<uintptr_t>(Globals::uworld + GAME_INSTANCE);

	Globals::game_state = r0->rpm<uintptr_t>(Globals::uworld + GAME_STATE);

	Globals::player_array = r0->rpm<uintptr_t>(Globals::game_state + PLAYER_ARRAY);
	std::cout << Globals::player_array << std::endl;


	Globals::player_count = r0->rpm<int>(Globals::game_state + (PLAYER_ARRAY + 0x8));
	//std::cout << Globals::player_count << std::endl;



	Globals::local_players = r0->rpm<uintptr_t>(r0->rpm<uintptr_t>(Globals::game_instance + LOCAL_PLAYERS));


	Globals::player_controller = r0->rpm<uintptr_t>(Globals::local_players + PLAYER_CONTROLLER);


	Globals::local_pawn = r0->rpm<uintptr_t>(Globals::player_controller + LOCAL_PAWN);


	Globals::player_state = r0->rpm<uintptr_t>(Globals::local_pawn + PLAYER_STATE);


	Globals::root_component = r0->rpm<uintptr_t>(Globals::local_pawn + ROOT_COMPONENT);


	Globals::my_team_id = r0->rpm<int>(Globals::player_state + TEAM_INDEX);

	//lootentitycopies = lootentities;
	Globals::closest_distance = FLT_MAX;
	Globals::closest_mesh = NULL;
	Globals::relative_location = Positioning;

	VisibleColor = ImColor(231, 69, 112, 255);
	InVisibleColor = ImColor(228, 240, 0, 255);

	Vector2 origin;
	Vector3 selfbottom3d;
	Vector2 selfbottom2d;

	//for (const auto& pair : lootentitycopies) {
	//	int entityIndex = pair.first;
	//	const LootEntity& entity = pair.second;
	//	//uintptr_t ItemRootComponent = r0->rpm<uintptr_t>(entity.ACurrentItem + ROOT_COMPONENT);
	//	Vector3 ItemPosition = r0->rpm<Vector3>(entity.ACurrentItemRoot + RELATIVE_LOCATION);


	//	if (r0->is_valid(entity.ACurrentItem))
	//	{
	//		float ItemDist = Globals::relative_location.distance(ItemPosition) / 100.f;
	//		Vector2 ChestPosition;
	//		ChestPosition = project_world_to_screen(ItemPosition);
	//		//std::cout << ChestPosition.x << std::endl;
	//		// 

	//		if (strstr(entity.GNames.c_str(), ("Tiered_Chest")) || strstr(entity.GNames.c_str(), ("AlwaysSpawn")))
	//		{
	//			std::string Text = ("Chest [") + std::to_string((int)ItemDist) + ("m]");
	//			ImGui::GetBackgroundDrawList()->AddText(ImVec2(ChestPosition.x - ImGui::CalcTextSize(Text.c_str()).x / 2, ChestPosition.y - ImGui::CalcTextSize(Text.c_str()).y), ImColor(255, 255, 255, 255), Text.c_str());
	//		}
	//		else if (strstr(entity.GNames.c_str(), ("FortPickupAthena")) || strstr(entity.GNames.c_str(), ("Fort_Pickup_Creative_C")))
	//		{
	//			std::string Text = ("Pickup [") + std::to_string((int)ItemDist) + ("m]");
	//			ImGui::GetBackgroundDrawList()->AddText(ImVec2(ChestPosition.x - ImGui::CalcTextSize(Text.c_str()).x / 2, ChestPosition.y - ImGui::CalcTextSize(Text.c_str()).y), ImColor(255, 255, 255, 255), Text.c_str());
	//		}
	//		else
	//		{
	//			std::string Text = ("Pickup [") + std::to_string((int)ItemDist) + ("m]");
	//			ImGui::GetBackgroundDrawList()->AddText(ImVec2(ChestPosition.x - ImGui::CalcTextSize(Text.c_str()).x / 2, ChestPosition.y - ImGui::CalcTextSize(Text.c_str()).y), ImColor(255, 255, 255, 255), Text.c_str());
	//		}
	//	}
	//}

	ImGui::PushFont(Segoesmallnotbold);

	finished = false;
	for (int i = 0; i < Globals::player_count; ++i) {
		if (!Globals::uworld) continue;

		if (!Globals::player_count)
			continue;


		auto player_state = r0->rpm<uintptr_t>(Globals::player_array + (i * 0x8));
		if (!player_state)
			continue;

		auto current_actor = r0->rpm<uintptr_t>(player_state + PAWN_PRIVATE);

		if (current_actor == Globals::local_pawn)
		{
			continue;
		}

		//auto is_despawning = (r0->rpm<char>(current_actor + 0x758) >> 3);
		//if (is_despawning) continue;

		auto teamID = r0->rpm<int>(player_state + TEAM_INDEX);

		auto Mesh = r0->rpm<uintptr_t>(current_actor + MESH);

		if (!Mesh) continue;
		uintptr_t mesh = Mesh;

		Vector3 head3d = get_entity_bone(mesh, 109);
		std::string test = std::to_string(head3d.x);
		if (!head3d.x)
			continue;

		//ImGui::GetBackgroundDrawList()->AddText(ImVec2(100, 400), ImColor(255, 255, 255, 255), test.c_str());

		//system("cls");
		//std::cout << head3d.x << std::endl;

		Actors cached_actors{};
		cached_actors.Actor = current_actor;
		cached_actors.Mesh = Mesh;
		cached_actors.PlayerState = player_state;
		cached_actors.TeamID = teamID;
		if (draw_username)
		{
			cached_actors.Username = username((uintptr_t)cached_actors.PlayerState);
		}

		if (draw_platform)
		{
			DWORD_PTR test_platform = r0->rpm<DWORD_PTR>((uintptr_t)cached_actors.PlayerState + 0x438);
			wchar_t platform[128];

			if (r0->is_valid((uintptr_t)test_platform))
			{
				r0->read_array((uintptr_t)test_platform, (uint8_t*)platform, sizeof(platform));
				std::wstring platform_wstr(platform);
				std::string platform_str(platform_wstr.begin(), platform_wstr.end());
				cached_actors.Platform = platform_str;
			}

		}
		if (!Globals::local_camera.fov)
			continue;
		Vector2 head2d = project_world_to_screen(Vector3(head3d.x, head3d.y, head3d.z + 19));

		Vector3 bottom3d = get_entity_bone(mesh, 0);
		if (!bottom3d.x)
			continue;

		Vector2 bottom2d = project_world_to_screen(bottom3d);
		float box_height = abs(head2d.y - bottom2d.y);
		float box_width = box_height * 0.50f;
		float distance = Globals::relative_location.distance(bottom3d) / 100;


		if (lobby_esp)
		{
			if (teamID == Globals::my_team_id) continue;
		}


		bool viewCheck = true;

		if (radar_esp)
		{
			zoom = Globals::local_camera.rotation.y;
			Vector3 EntitiesPos = RotatePoint(head3d, Positioning, pos.x, pos.y, siz.x, siz.y, -zoom, 2, &viewCheck, false);
			ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(EntitiesPos.x, EntitiesPos.y), 4, VisibleColor, 32);
		}

		if (distance < limitingfactor)
		{
			std::cout << distance;
			if (distance < 60)
			{
				skeletonesp(mesh, distance);
			}

			if (box_corner)
			{
				if (is_visible(mesh))
				{
					draw_cornered_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, VisibleColor, box_thickness);
				}
				else
				{
					draw_cornered_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, InVisibleColor, box_thickness);
				}
			}

			if (filled_box) draw_filled_rect(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(0, 0, 0, 50));

			if (mlm_esp)
			{
				//draw_mlm_rect(head2d.x - (box_width / 2), head2d.y, box_width, box_height);
			}


			if (box_esp) {

				//DrawBox(head2d.x - box_width / 2, head2d.y, box_width, CornerHeight, ImColor(250, 250, 250, 250), visuals->box_thickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(head2d.x - (box_width / 2) - 5, head2d.y), ImVec2(bottom2d.x + (box_width / 2) + 5, head2d.y), VisibleColor, box_thickness); //top line

				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(head2d.x - (box_width / 2) - 5, head2d.y), ImVec2(head2d.x - (box_width / 2) - 5, bottom2d.y), VisibleColor, box_thickness); // left line

				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(head2d.x - (box_width / 2) - 5, bottom2d.y), ImVec2(bottom2d.x + (box_width / 2) + 5, bottom2d.y), VisibleColor, box_thickness); // bottom line

				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bottom2d.x + (box_width / 2) + 5, head2d.y), ImVec2(bottom2d.x + (box_width / 2) + 5, bottom2d.y), VisibleColor, box_thickness); // right line


			}
			if (line)
			{
				if (is_visible(mesh))
				{
					draw_line(Vector2(project_world_to_screen(Vector3(Positioning.x, Positioning.y, Positioning.z - 0.2))), bottom2d, VisibleColor);
				}
				else
				{
					draw_line(Vector2(project_world_to_screen(Vector3(Positioning.x, Positioning.y, Positioning.z - 0.2))), bottom2d, InVisibleColor);
				}

			}
			if (draw_distance)
			{
				draw_distancevoid(bottom2d, distance, VisibleColor);
			}
			//if (bonespots)
			//{
			//	boneweakspots(mesh, ImColor(255, 0, 0, 255), 8 / distance);
			//}

			if (draw_username)
			{
				if (player_state)
				{
					ImVec2 textPosition(head2d.x, head2d.y - 13);
					ImGui::GetBackgroundDrawList()->AddText(ImVec2(bottom2d.x - ImGui::CalcTextSize(cached_actors.Username.c_str()).x / 2, head2d.y - ImGui::CalcTextSize(cached_actors.Username.c_str()).y - 10), VisibleColor, cached_actors.Username.c_str());
				}
			}
			if (draw_held_weapon) {
				uintptr_t currentweapon = r0->rpm<uintptr_t>(current_actor + 0xa20);

				if (r0->is_valid(current_actor))
				{
					int ammocount = r0->rpm <int>(currentweapon + 0xdcc);
					//std::cout << "value: " << ammocount << std::endl;
					auto ItemData = r0->rpm<DWORD_PTR>(currentweapon + 0x4a8);
					//std::cout << "value: " << (int)tier << std::endl;




					if (r0->is_valid(ItemData))
					{
						BYTE tier = r0->rpm<BYTE>(ItemData + 0x73);
						uint64_t ftext_ptr = r0->rpm <uint64_t>(ItemData + 0x90);
						if (r0->is_valid(ftext_ptr))
						{
							uint64_t ftext_data = r0->rpm <uint64_t>(ftext_ptr + 0x28);
							int ftext_length = r0->rpm <int>(ftext_ptr + 0x30);
							std::string weapon_name;
							//std::cout << "value: " << ftext_length << std::endl;
							if (ftext_length > 0 && ftext_length < 50)
							{
								wchar_t ftext_buf[64];
								if (r0->is_valid(ftext_data))
								{
									r0->read_array(ftext_data, ftext_buf, ftext_length * sizeof(wchar_t));
									std::wstring wstr_buf(ftext_buf);
									weapon_name = std::string(wstr_buf.begin(), wstr_buf.end());
									//std::cout << "value: " << weapon_name << std::endl;
								}
							}
							else
							{
								weapon_name = "Unarmed";
							}
							ImColor Color;

							if (tier == 2)
							{
								Color = IM_COL32(0, 255, 0, 255);
							}
							else if ((tier == 3))
							{
								Color = IM_COL32(0, 0, 255, 255);
							}
							else if ((tier == 4))
							{
								Color = IM_COL32(128, 0, 128, 255); // p
							}
							else if ((tier == 5))
							{
								Color = IM_COL32(255, 255, 0, 255); // p
							}
							else if ((tier == 6))
							{
								Color = IM_COL32(255, 255, 0, 255); // p
							}
							else if ((tier == 0) || (tier == 1))
							{
								Color = IM_COL32(255, 255, 255, 255); // p
							}


							ImVec2 textPosition(bottom2d.x, bottom2d.y + 10);

							ImGui::GetBackgroundDrawList()->AddText(ImVec2(bottom2d.x - ImGui::CalcTextSize(weapon_name.c_str()).x / 2, head2d.y - ImGui::CalcTextSize(weapon_name.c_str()).y - 35), Color, weapon_name.c_str());
						}
					}


					//auto DisplayName = r0->rpm<uint64_t>(ItemData + 0x98);
					//auto WeaponLength = r0->rpm<uint32_t>(DisplayName + 0x40);
					//wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];
					//uintptr_t WeaponBuffer = r0->rpm<uintptr_t>(DisplayName + 0x38);
					//r0->read_array((WeaponBuffer + 0x38), (uint8_t*)WeaponName, WeaponLength * sizeof(wchar_t));

					//std::wcout << "weapon: " << WeaponName << std::endl;
					//BYTE tier;
					//ImColor Color;

					//if (tier == 2)
					//{
					//	Color = IM_COL32(0, 255, 0, 255);
					//}
					//else if ((tier == 3))
					//{
					//	Color = IM_COL32(0, 0, 255, 255);
					//}
					//else if ((tier == 4))
					//{
					//	Color = IM_COL32(128, 0, 128, 255); // p
					//}
					//else if ((tier == 5))
					//{
					//	Color = IM_COL32(255, 255, 0, 255); // p
					//}
					//else if ((tier == 6))
					//{
					//	Color = IM_COL32(255, 255, 0, 255); // p
					//}
					//else if ((tier == 0) || (tier == 1))
					//{
					//	Color = IM_COL32(255, 255, 255, 255); // p
					//}
					//if (!WeaponName || !tier)
					//{
					//	delete[] WeaponName;
					//	WeaponName = new wchar_t[1];
					//	wcscpy(WeaponName, L"");
					//}

					//	std::string wep_str = wchar_to_char(WeaponName);
					//	if (wep_str.length() < 1)
					//	{
					//		wep_str = "Pickaxe";
					//		tier = 0;
					//	}

					//	ImVec2 textPosition(bottom2d.x, bottom2d.y + 10);

					//	ImGui::GetBackgroundDrawList()->AddText(ImVec2(bottom2d.x - ImGui::CalcTextSize(wep_str.c_str()).x / 2, head2d.y - ImGui::CalcTextSize(wep_str.c_str()).y - 35), Color, wep_str.c_str());
				}
			}

			if (draw_platform) {


				ImVec2 textPosition(head2d.x, head2d.y - 33);

				ImGui::GetBackgroundDrawList()->AddText(ImVec2(bottom2d.x - ImGui::CalcTextSize(cached_actors.Platform.c_str()).x / 2, head2d.y - ImGui::CalcTextSize(cached_actors.Platform.c_str()).y - 60), VisibleColor, cached_actors.Platform.c_str());
			}

			double dx = head2d.x - screen_center_x;
			double dy = head2d.y - screen_center_y;
			float dist = sqrtf(dx * dx + dy * dy);
			if (dist <= fov && dist < Globals::closest_distance)
			{
				Globals::closest_distance = dist;
				Globals::closest_mesh = mesh;
				closest_actor = cached_actors;
			}
		}
		
		if (lobby_esp)
		{
			if (teamID == Globals::my_team_id) continue;
		}

	}

	finished = true;
	ImGui::PopFont();
	if (aimbot)
	{
		if (GetAsyncKeyState(aimkey))
		{

			aimbotvoid(Globals::closest_mesh, closest_actor);
		}
	}
	Sleep(2);

}


//void cachethread()
//{
//	while (true)
//	{
//
//		Sleep(cachedelay * 300);
//		entitiesMutex.lock();
//		entities = ResizeBar();
//		//std::lock_guard<std::shared_mutex> lock(entitiesMutex);
//		entitiesMutex.unlock();
//		Sleep(cachedelay * 700);
//
//	}
//}


void lootcachethread()
{
	while (true)
	{
		//std::lock_guard<std::shared_mutex> lock(lootentitiesMutex);
		lootentitiesMutex.lock();
		lootentities = LootResizeBar();
		lootentitiesMutex.unlock();
		Sleep(6000);
	}
}



size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::vector<char>* output) {
	size_t totalSize = size * nmemb;
	output->insert(output->end(), static_cast<char*>(contents), static_cast<char*>(contents) + totalSize);
	return totalSize;
}

std::vector<unsigned char> DownloadImage(const char* url) {
	std::vector<unsigned char> imageData;


	// Initialize cURL
	curl_global_init(CURL_GLOBAL_DEFAULT);

	CURL* curl = curl_easy_init();
	if (curl) {
		// Set cURL options
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);

		// Perform the cURL request
		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
		}

		// Cleanup cURL
		curl_easy_cleanup(curl);
	}

	// Cleanup cURL global state
	curl_global_cleanup();

	return imageData;
}