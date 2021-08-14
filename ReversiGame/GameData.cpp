#pragma once
#include <Siv3D.hpp>

struct GameData {
	int cpuType = 0;
	bool player_is_first = true;
	int score = 0;
	int black = 0, white = 0;
	int result = 0; // Draw:0,Win:1,Lose:2
};