#pragma once
#include <Siv3D.hpp>
#include "State.cpp"
#include "GameData.cpp"

using MyApp = SceneManager<State, GameData>;


const int MONTECALRO_SEARCH_COUNT = 500;

const int MCTS_EXPAND_LIMIT = 10;
const int MONTECALRO_TREE_SEARCH_COUNT = 500;