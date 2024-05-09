#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "SDK.h"
#include <unordered_map>
#include "Settings.h"
#include <shared_mutex>
#include <concurrent_vector.h>

class Actors {
public:
	uintptr_t
		Actor,
		Mesh,
		Root_Component,
		CurrentWeapon,
		PlayerState;
	int
		TeamID,
		ID;
	std::string
		Username,
		Platform;
	Vector3 BaseBonePosition,
		BottomBonePosition;
};

typedef struct _LootEntity {
	std::string GNames;
	uintptr_t ACurrentItem, ACurrentItemRoot;
}LootEntity;

std::map<int, Actors> entities;
std::vector<Actors> entitycopies(100);
std::shared_mutex entitiesMutex;  // Add this line to declare a mutex

std::vector<Actors> tmpList;

std::vector<Actors> PlayerAddresses(100);
std::mutex newentitiesMutex;  // Add this line to declare a mutex


std::unordered_map<int, LootEntity> lootentities;
std::unordered_map<int, LootEntity> lootentitycopies;
std::shared_mutex lootentitiesMutex;  // Add this line to declare a mutex


std::vector<Actors> entitylist;

void ResizeBar()
{
	while (true)
	{

		Globals::uworld = r0->rpm<uintptr_t>(r0->base + UWORLD);
		Globals::game_instance = r0->rpm<uintptr_t>(Globals::uworld + GAME_INSTANCE);
		Globals::game_state = r0->rpm<uintptr_t>(Globals::uworld + GAME_STATE);
		Globals::local_players = r0->rpm<uintptr_t>(r0->rpm<uintptr_t>(Globals::game_instance + LOCAL_PLAYERS));
		Globals::player_controller = r0->rpm<uintptr_t>(Globals::local_players + PLAYER_CONTROLLER);
		Globals::local_pawn = r0->rpm<uintptr_t>(Globals::player_controller + LOCAL_PAWN);
		Globals::player_state = r0->rpm<uintptr_t>(Globals::local_pawn + PLAYER_STATE);
		Globals::root_component = r0->rpm<uintptr_t>(Globals::local_pawn + ROOT_COMPONENT);
		Globals::my_team_id = r0->rpm<int>(Globals::player_state + TEAM_INDEX);
		Globals::player_array = r0->rpm<uintptr_t>(Globals::game_state + PLAYER_ARRAY);
		Globals::player_count = r0->rpm<int>(Globals::game_state + (PLAYER_ARRAY + sizeof(uintptr_t)));
		while (Globals::player_count == 0)
		{
			Globals::uworld = r0->rpm<uintptr_t>(r0->base + UWORLD);
			Globals::game_instance = r0->rpm<uintptr_t>(Globals::uworld + GAME_INSTANCE);
			Globals::game_state = r0->rpm<uintptr_t>(Globals::uworld + GAME_STATE);
			Globals::local_players = r0->rpm<uintptr_t>(r0->rpm<uintptr_t>(Globals::game_instance + LOCAL_PLAYERS));
			Globals::player_controller = r0->rpm<uintptr_t>(Globals::local_players + PLAYER_CONTROLLER);
			Globals::local_pawn = r0->rpm<uintptr_t>(Globals::player_controller + LOCAL_PAWN);
			Globals::player_state = r0->rpm<uintptr_t>(Globals::local_pawn + PLAYER_STATE);
			Globals::root_component = r0->rpm<uintptr_t>(Globals::local_pawn + ROOT_COMPONENT);
			Globals::my_team_id = r0->rpm<int>(Globals::player_state + TEAM_INDEX);
			Globals::player_array = r0->rpm<uintptr_t>(Globals::game_state + PLAYER_ARRAY);
			Globals::player_count = r0->rpm<int>(Globals::game_state + (PLAYER_ARRAY + sizeof(uintptr_t)));
			Sleep(2);
		}

		if (Globals::local_pawn != Globals::local_pawn_old) {
			if (!tmpList.empty())
				tmpList.clear();
			Globals::local_pawn = Globals::local_pawn_old;
		}

		std::cout << "---------------------------------------" << std::endl;
		std::cout << "Player Count: " << Globals::player_count << std::endl;
		std::cout << "Vector Size: " << entitylist.size() << std::endl;
		std::cout << "---------------------------------------" << std::endl;

		if (Globals::player_count > 0)
		{
			Globals::relative_location = Positioning;
			//std::map<int, Actors> tempentities;
			const int numberOfEntities = Globals::player_count;
			std::vector<uintptr_t> entitiesData(numberOfEntities);

			for (int i = 0; i < Globals::player_count; ++i) {
				auto player_state = r0->rpm<uintptr_t>(Globals::player_array + (i * sizeof(uintptr_t)));
				std::cout << "hello!";
				if (!player_state) continue;

				auto current_actor = r0->rpm<uintptr_t>(player_state + PAWN_PRIVATE);
				if (!current_actor) continue;
				auto skeletalmesh = r0->rpm<uintptr_t>(current_actor + MESH);
				if (!skeletalmesh) continue;
				auto base_bone = get_entity_bone(skeletalmesh, 0);
				if (base_bone.x == 0 && base_bone.y == 0 && base_bone.z == 0) continue;
				Actors cached_actors{};
				cached_actors.Actor = current_actor;
				cached_actors.Mesh = r0->rpm<uintptr_t>(current_actor + MESH);
				cached_actors.Root_Component = r0->rpm<uintptr_t>(current_actor + ROOT_COMPONENT);
				cached_actors.PlayerState = r0->rpm<uintptr_t>(current_actor + PLAYER_STATE);
				cached_actors.TeamID = r0->rpm<int>(cached_actors.PlayerState + TEAM_INDEX);


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
				tmpList.emplace_back(cached_actors);
				//tempentities[i] = cached_actors; //breakpoint //lol
				//tempentities.insert(i, cached_actors);
				//tempentities.emplace(i, cached_actors);
				//tempentities.insert(std::make_pair(i, cached_actors));
				
			}
		}

		entitiesMutex.lock();
		entitylist = std::move(tmpList);
		entitiesMutex.unlock();
		Sleep(cachedelay * 1000);
	}
}

bool finished = true;
void ActorQueue()
{
	std::vector<Actors> m_entities(100);
	for (;; ) {
		m_entities.clear();
		Globals::uworld = r0->rpm<uintptr_t>(r0->base + UWORLD);
		std::cout << Globals::uworld << std::endl;
		if (!Globals::uworld)
		{
			continue;
		}

		Globals::game_instance = r0->rpm<uintptr_t>(Globals::uworld + GAME_INSTANCE);
		if (!Globals::game_instance)
		{
			continue;
		}

		Globals::game_state = r0->rpm<uintptr_t>(Globals::uworld + GAME_STATE);
		if (!Globals::game_state)
		{
			continue;
		}

		Globals::player_array = r0->rpm<uintptr_t>(Globals::game_state + PLAYER_ARRAY);
		std::cout << Globals::player_array << std::endl;
		if (!Globals::player_array)
		{
			continue;
		}

		Globals::player_count = r0->rpm<int>(Globals::game_state + (PLAYER_ARRAY + 0x8));
		std::cout << Globals::player_count << std::endl;
		if (!Globals::player_count)
		{
			continue;
		}
		/////////
		
		//std::cout << Globals::player_count << std::endl;
		//system("cls");
		for (int i = 0; i < Globals::player_count; ++i) {
			if (!Globals::player_count) {
				continue;
			}

			Globals::local_players = r0->rpm<uintptr_t>(r0->rpm<uintptr_t>(Globals::game_instance + LOCAL_PLAYERS));
			if (!Globals::local_players)
			{
				continue;
			}

			Globals::player_controller = r0->rpm<uintptr_t>(Globals::local_players + PLAYER_CONTROLLER);
			if (!Globals::player_controller)
			{
				continue;
			}

			Globals::local_pawn = r0->rpm<uintptr_t>(Globals::player_controller + LOCAL_PAWN);
			if (!Globals::local_pawn)
			{
				continue;
			}

			Globals::player_state = r0->rpm<uintptr_t>(Globals::local_pawn + PLAYER_STATE);
			if (!Globals::player_state)
			{
				continue;
			}

			Globals::root_component = r0->rpm<uintptr_t>(Globals::local_pawn + ROOT_COMPONENT);
			if (!Globals::root_component)
			{
				continue;
			}

			Globals::my_team_id = r0->rpm<int>(Globals::player_state + TEAM_INDEX);
			

			auto player_state = r0->rpm<uintptr_t>(Globals::player_array + (i * 0x8));

			auto current_actor = r0->rpm<uintptr_t>(player_state + PAWN_PRIVATE);

			if (r0->is_valid((uint64_t)current_actor))
			{
				if (current_actor == Globals::local_pawn)
				{
					continue;
				}

				auto Mesh = r0->rpm<uintptr_t>(current_actor + MESH);
				if (!Mesh)
				{
					continue;
				}

				Actors cached_actors;
				cached_actors.Actor = current_actor;
				cached_actors.Mesh = Mesh;
				cached_actors.PlayerState = r0->rpm<uintptr_t>(current_actor + PLAYER_STATE);
				//cached_actors.BaseBonePosition = get_entity_bone(Mesh, 109);
				//cached_actors.BottomBonePosition = get_entity_bone(Mesh, 0);
				//std::cout << "BonsePos: " << cached_actors.BaseBonePosition.x << ", " << cached_actors.BaseBonePosition.y << ", " << cached_actors.BaseBonePosition.z << std::endl;
				//system("cls");
				m_entities.emplace_back(cached_actors);
			}

		}


		entitiesMutex.lock();
		PlayerAddresses.clear();
		//for (auto ent : m_entities) {
		//	PlayerAddresses.emplace_back(ent);
		//}
		//memcpy(PlayerAddresses, m_entities)
		//std::copy(PlayerAddresses.begin(), PlayerAddresses.end(), m_entities);
		//PlayerAddresses.assign(m_entities.begin(), m_entities.end());
		//PlayerAddresses.swap(m_entities);
		PlayerAddresses = m_entities;
		entitiesMutex.unlock();

		//std::cout << "Globals::player_count" << Globals::player_count << std::endl;
		//std::cout << "Globals::local_players" << Globals::local_players << std::endl;
		//std::cout << "Globals::player_controller" << Globals::player_controller << std::endl;
		//std::cout << "Globals::player_state" << Globals::player_state << std::endl;
		//std::cout << "PlayerAddresses" << PlayerAddresses.size() << std::endl;

		if (PlayerAddresses.size() && finished)
		{
			//std::copy(entitycopies.begin(), entitycopies.end(), entitycopies);
			//entitycopies.assign(PlayerAddresses.begin(), PlayerAddresses.end());
			//entitycopies.swap(PlayerAddresses);
			entitycopies = PlayerAddresses;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	}
}

std::unordered_map<int, LootEntity> LootResizeBar() {

	//Globals::uworld = r0->rpm<uintptr_t>(r0->base + UWORLD);
	//Globals::game_instance = r0->rpm<uintptr_t>(Globals::uworld + GAME_INSTANCE);
	//Globals::game_state = r0->rpm<uintptr_t>(Globals::uworld + GAME_STATE);
	//Globals::local_players = r0->rpm<uintptr_t>(r0->rpm<uintptr_t>(Globals::game_instance + LOCAL_PLAYERS));
	//Globals::player_controller = r0->rpm<uintptr_t>(Globals::local_players + PLAYER_CONTROLLER);
	//Globals::local_pawn = r0->rpm<uintptr_t>(Globals::player_controller + LOCAL_PAWN);
	//Globals::player_state = r0->rpm<uintptr_t>(Globals::local_pawn + PLAYER_STATE);
	//Globals::root_component = r0->rpm<uintptr_t>(Globals::local_pawn + ROOT_COMPONENT);
	//Globals::my_team_id = r0->rpm<int>(Globals::player_state + TEAM_INDEX);
	//Globals::player_array = r0->rpm<uintptr_t>(Globals::game_state + PLAYER_ARRAY);
	//Globals::player_count = r0->rpm<int>(Globals::game_state + (PLAYER_ARRAY + sizeof(uintptr_t)));


	//std::cout << Globals::player_count << std::endl;
	//Globals::relative_location = Positioning;
	uintptr_t ItemLevels = r0->rpm<uintptr_t>(Globals::uworld + 0x170);
	std::unordered_map<int, LootEntity> entities;
	//std::cout << "Passed Check 1" << std::endl;
	if (r0->is_valid(ItemLevels))
	{
		//std::cout << "Passed Check 2" << std::endl;
		for (int i = 0; i < r0->rpm<DWORD>(Globals::uworld + (0x170 + sizeof(PVOID))); ++i) {


			uintptr_t ItemLevel = r0->rpm<uintptr_t>(ItemLevels + (i * sizeof(uintptr_t)));


			if (r0->is_valid(ItemLevels))
			{
				//std::cout << "Passed Check 3" << std::endl;
				for (int i = 0; i < r0->rpm<DWORD>(ItemLevel + (0xA0 + sizeof(PVOID))); ++i) {

					uintptr_t ItemsPawns = r0->rpm<uintptr_t>(ItemLevel + 0xA0);
					if (r0->is_valid(ItemsPawns))
					{
						//std::cout << "Passed Check 4" << std::endl;
						uintptr_t CurrentItemPawn = r0->rpm<uintptr_t>(ItemsPawns + (i * sizeof(uintptr_t))); // CurrentActor

						if (r0->is_valid(CurrentItemPawn))
						{
							uintptr_t ItemRootComponent = r0->rpm<uintptr_t>(CurrentItemPawn + ROOT_COMPONENT);
							Vector3 ItemPosition = r0->rpm<Vector3>(ItemRootComponent + RELATIVE_LOCATION);
							float ItemDist = Globals::relative_location.distance(ItemPosition) / 100.f;


							if (ItemDist < 40)
							{
								int ItemIndex = r0->rpm<int>(CurrentItemPawn + 0x18);
								//auto CurrentItemPawnName = GetNameFromFName(ItemIndex);
								////std::cout << CurrentItemPawnName << std::endl;
								//if ((strstr(CurrentItemPawnName.c_str(), (("Tiered_Chest")))) || strstr(CurrentItemPawnName.c_str(), (("AlwaysSpawn"))) || strstr(CurrentItemPawnName.c_str(), (("FortPickupAthena"))) || strstr(CurrentItemPawnName.c_str(), (("Tiered_Ammo"))) || strstr(CurrentItemPawnName.c_str(), (("Fort_Pickup_Creative_C"))) )
								//{
									LootEntity LevelObjects{ };
									LevelObjects.ACurrentItem = CurrentItemPawn;
									//LevelObjects.GNames = CurrentItemPawnName;
									LevelObjects.ACurrentItemRoot = ItemRootComponent;
									entities[i] = LevelObjects;
								//}
							}
						}
					}
				}
			}
			Sleep(1);
		}
	}

	return entities;
}



bool IsTargetUnderReticle(Actors actor)
{

	if (!actor.PlayerState)
		return false;
	auto m_LocationUnderReticle = r0->rpm<Vector3>((uintptr_t)Globals::player_controller + 0x2560);
	//std::cout << m_LocationUnderReticle.x << std::endl;

	auto m_TargetBone = get_entity_bone(actor.Mesh, 109);

	//std::cout << m_TargetBone.x << std::endl;

	if (m_LocationUnderReticle.x >= m_TargetBone.x - 20 && m_LocationUnderReticle.x <= m_TargetBone.x + 20 &&
		m_LocationUnderReticle.y >= m_TargetBone.y - 20 && m_LocationUnderReticle.y <= m_TargetBone.y + 20 &&
		m_LocationUnderReticle.z >= m_TargetBone.z - 20 && m_LocationUnderReticle.z <= m_TargetBone.z + 20)
	{
		return true;
	}
	else
	{
		return false;
	}
}




class AthenaActor
{
	float Distance;
	int Team_ID;
	Vector3 Position;
	std::string
	Username,
	Platform;
	uintptr_t
	Actor,
	Mesh,
	Root_Component,
	CurrentWeapon,
	PlayerState;
};


