#pragma once

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <thread>
#include <iostream>
#include "crypt.h"
#include "comm.h" 

void start();
 
//
// blairdumper alpha
// -> processed entire game frame
// -> dumping image..
// -> scanning known patterns..
// -> scanned! (17 matches)
// -> getting objects..
// -> dumping offset variables by name
// -> dumped! (24 matches)
// -> writing to buffer..
// -> detaching from module
// -> successfully dumped at 05.12.23 at 9:19PM GMT!
//
#define uworld 0x1123AF78
#define fname 0x11400C80
#define free_fn 0xC566C28
#define get_bone_matrix 0x149B864
#define project_world_to_screen 0x35BD6A8
#define gobjects 0x11213D40
#define get_name_by_index 0x1004854
#define line_of_sight_to 0x65403D0
#define find_object 0x3EDBF1C
#define persistent_level 0x30
#define owning_game_instance 0x1d0
#define player_camera_manager 0x348
#define local_players 0x38
#define acknowledged_pawn 0x338
#define player_state 0x2b0
#define player_controller 0x30
#define player_array 0x2a8
#define root_component 0x198
#define relative_location 0x128
#define aactor 0x98
#define current_weapon 0xa20
#define weapon_data 0x4a8
#define mesh 0x318
#define levels 0x170
#define tier 0x73
#define team_index 0x10e0
#define game_state 0x158
#define revive_from_dbno_time 0x4ae0
#define pawn_private 0x308
#define server_world_time_seconds_delta 0x2c8
#define bis_dbno 0x93a
#define is_bot 0x29a
#define ammo_count 0xdcc
#define bone_array 0x610
#define bone_cache 0x648
#define component_to_world 0x240 
#define kills 0x1144
#define chests 0x114c
#define llamas 0x1164
#define alive 0x1174