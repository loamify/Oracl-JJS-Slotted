#pragma once
#include "Globals.h"
#include "Offsets.h"
#include "Communication.h"
#include <map>
inline Vector3 Positioning;
inline float camfov = 90;

Vector3 get_entity_bone(uintptr_t mesh, int bone_id)
{
	int is_cached = r0->rpm<int>((uintptr_t)mesh + BONE_ARRAY_CACHE);
	auto bone_transform = r0->rpm<FTransform>(r0->rpm<uintptr_t>((uintptr_t)mesh + 0x10 * is_cached + BONE_ARRAY) + 0x60 * bone_id);

	FTransform ComponentToWorld = r0->rpm<FTransform>((uintptr_t)mesh + 0x230);

	D3DMATRIX Matrix = {};
	Matrix = matrix_multiplication(bone_transform.to_matrix_with_scale(), ComponentToWorld.to_matrix_with_scale());

	return Vector3((int)Matrix._41, (int)Matrix._42, (int)Matrix._43);
}

std::string username(__int64 playerstate)
{

	__int64 Name = r0->rpm<__int64>(playerstate + 0xAE8);

	if (!Name) return std::string("[BOT/AI]");
	int Length = r0->rpm<int>(Name + 16i64);
	__int64 v6 = Length;
	if (!v6) return std::string("[BOT/AI]");
	uintptr_t Data = r0->rpm<__int64>(Name + 8);
	if (!Data) return std::string("[BOT/AI]");
	wchar_t NameBuffer[64];

	if (r0->is_valid(Data))
	{
		r0->read_array((uintptr_t)Data, (uint8_t*)(NameBuffer), (Length * 2));
		//NameBuffer = r0->readphysical<wchar_t*>(Data, Length * 2);


		char v21; // al
		int v22; // r8d
		int i; // ecx
		int v25; // eax
		_WORD* v23;

		v21 = v6 - 1;
		if (!(_DWORD)v6)
			v21 = 0;
		v22 = 0;
		v23 = (_WORD*)NameBuffer;
		for (i = (v21) & 3; ; *v23++ += i & 7)
		{
			v25 = v6 - 1;
			if (!(_DWORD)v6)
				v25 = 0;
			if (v22 >= v25)
				break;
			i += 3;
			++v22;
		}

		std::wstring Temp{ NameBuffer };
		return std::string(Temp.begin(), Temp.end());
	}
	else
	{
		return "READ FAIL!";
	}
	//return "player";

}


std::map<int, std::string> cached_fnames{};
static std::string GetNameFromIndex(int key)
{
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;
	//uint64_t NamePoolChunk = r0->rpm<uint64_t>(r0->base + 0x11947880 + (8 * ChunkOffset) + 16) + (unsigned int)(2 * NameOffset);


	auto NamePoolChunk = r0->rpm<uint64_t>(
		r0->base + 0x11947880 +
		(8 * (uint32_t)((int)(key) >> 16))
		+ 16) + (unsigned int)(2 * (uint16_t)key
			);
	uint16_t nameEntry = r0->rpm<uint16_t>(NamePoolChunk);
	int nameLength = nameEntry >> 6;
	char buff[2048] = {};


	if (nameLength && nameLength < 50)
	{
		r0->read_array(NamePoolChunk + 2, buff, nameLength);

		//for (int x = 0; x < nameLength; ++x)
		//{
		//	buff[x] = r0->rpm<char>(NamePoolChunk + 2 + x);
		//}

		char* v2 = buff; // rdi
		int v4 = 14; // edx
		__int64 result = nameLength; // rax
		__int64 v6; // r8
		unsigned int v7; // edx	

		if ((int)result)
		{
			v6 = (unsigned int)result;
			do {
				v7 = v4 + 45297;
				*v2 = v7 + ~*v2;
				result = v7 << 8;
				++v2;
				v4 = result | (v7 >> 8);
				--v6;
			} while (v6);
		}
		if (nameLength)
		{
			buff[nameLength] = '\0';
			return std::string(buff);
		}

	}
	else
	{
		return "NONE";
	}


}

static std::string GetNameFromFName(int key)
{

	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;


	if (!key)
	{
		return "NONE";
	}


	auto cached_name = cached_fnames.find(key);
	if (cached_name != cached_fnames.end())
		return cached_name->second;

	uint64_t NamePoolChunk = r0->rpm<uint64_t>(r0->base + 0x11947880 + (8 * ChunkOffset) + 16) + (unsigned int)(2 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED


	if (r0->is_valid(NamePoolChunk))
	{
		if (r0->rpm<uint16_t>(NamePoolChunk) < 64)
		{
			std::string fname = GetNameFromIndex(
				r0->rpm<DWORD>(NamePoolChunk + 2)
			);

			cached_fnames.emplace(key, fname);

			return fname;
		}
		else
		{
			std::string fname = GetNameFromIndex(key);

			cached_fnames.emplace(key, fname);

			return fname;
		}
	}

	//auto NamePoolChunk = r0->rpm<uint64_t>(r0->base + 0x11947880 + (8 * (uint32_t)((int)(key) >> 16)) + 16) + (unsigned int)(2 * (uint16_t)key);


}

Camera get_view_point()
{
	Camera view_point{};
	uintptr_t location_pointer = r0->rpm<uintptr_t>(Globals::uworld + 0x110);
	if (!location_pointer)
		return view_point;

	uintptr_t rotation_pointer = r0->rpm<uintptr_t>(Globals::uworld + 0x120);
	if (!rotation_pointer)
		return view_point;

	FNRot fnrot{};
	fnrot.a = r0->rpm<double>(rotation_pointer);
	fnrot.b = r0->rpm<double>(rotation_pointer + 0x20);
	fnrot.c = r0->rpm<double>(rotation_pointer + 0x1D0);
	view_point.location = r0->rpm<Vector3>(location_pointer);
	Positioning = view_point.location;
	view_point.rotation.x = asin(fnrot.c) * (180.0 / M_PI);
	view_point.rotation.y = ((atan2(fnrot.a * -1, fnrot.b) * (180.0 / M_PI)) * -1) * -1;
	view_point.fov = r0->rpm<float>(Globals::player_controller + 0x394) * camfov;
	return view_point;
}

Vector2 project_world_to_screen(Vector3 world_location)
{
	D3DMATRIX temp_matrix = to_matrix(Globals::local_camera.rotation);
	Vector3 vaxisx = Vector3(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
	Vector3 vaxisy = Vector3(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
	Vector3 vaxisz = Vector3(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);
	Vector3 vdelta = world_location - Globals::local_camera.location;
	Vector3 vtransformed = Vector3(vdelta.dot(vaxisy), vdelta.dot(vaxisz), vdelta.dot(vaxisx));
	if (vtransformed.z < 1) vtransformed.z = 1;
	return Vector2(screen_center_x + vtransformed.x * ((screen_center_x / tanf(Globals::local_camera.fov * M_PI / 360))) / vtransformed.z, screen_center_y - vtransformed.y * ((screen_center_x / tanf(Globals::local_camera.fov * M_PI / 360))) / vtransformed.z);
}

bool is_visible(uintptr_t mesh)
{
	if (!mesh)
		return false;
	float last_sumbit_time = r0->rpm<float>(mesh + LAST_SUMBIT_TIME);
	float last_render_time_on_screen = r0->rpm<float>(mesh + LAST_SUMBIT_TIME_ON_SCREEN);
	return last_render_time_on_screen + 0.06f >= last_sumbit_time;
}





inline char* wchar_to_char(const wchar_t* pwchar)
{
	int currentCharIndex = 0;
	char currentChar = pwchar[currentCharIndex];

	while (currentChar != '\0')
	{
		currentCharIndex++;
		currentChar = pwchar[currentCharIndex];
	}

	const int charCount = currentCharIndex + 1;

	char* filePathC = (char*)malloc(sizeof(char) * charCount);

	for (int i = 0; i < charCount; i++)
	{
		char character = pwchar[i];

		*filePathC = character;

		filePathC += sizeof(char);

	}
	filePathC += '\0';

	filePathC -= (sizeof(char) * charCount);

	return filePathC;
}


auto in_screen(Vector2 screen_location) -> bool {

	if (screen_location.x > 0 && screen_location.x < screen_width && screen_location.y > 0 && screen_location.y < screen_height) return true;
	else return false;

}

int UsedBone[4] = { 109, 67, 66, 2 };

