#pragma once
#include "Define.h"

class SimpleState {
private:
	bool pass_end;
	std::vector<std::vector<int>> board;
	int height;
	int width;
	int depth;

	std::vector<std::pair<int, int>> get_action_flip_list(int y,int x) const {
		std::vector<std::pair<int, int>> updates;
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				if (dx == 0 && dy == 0)continue;
				std::vector<std::pair<int, int>> tmp;
				int nx = x + dx, ny = y + dy;
				while (0 <= nx && nx < width && 0 <= ny && ny < height && getColor(ny,nx) != -1) {
					if (teban() != getColor(ny,nx))
						tmp.emplace_back(ny, nx);
					else {
						for (auto p : tmp) {
							updates.push_back(p);
						}
						break;
					}
					nx += dx, ny += dy;
				}
			}
		}
		return updates;
	}

	bool is_legal_action(int y,int x) const {
		if (getColor(y, x) != -1)return false;
		auto updates = get_action_flip_list(y, x);
		return updates.size() != 0;
	}

	bool take_action(int y, int x) {
		board[y][x] = teban();
		auto updates = get_action_flip_list(y, x);
		for (auto [py, px] : updates) {
			board[py][px] = teban();
		}
		return updates.size() != 0;
	}

public:
	SimpleState() {
		this->board = std::vector<std::vector<int>>();
	}
	SimpleState(std::vector<std::vector<int>> board,int depth) {
		this->board = board;
		this->depth = depth;
		this->height = board.size();
		this->width = board[0].size();
		this->pass_end = false;
	}

	bool teban() const {
		return this->depth % 2;
	}

	std::pair<int, int> stone_count() const {
		std::pair<int, int> ret = { 0,0 };
		for (auto row : board) {
			for (auto cell : row) {
				if (cell == -1)continue;
				if (cell == teban()) {
					ret.first++;
				}
				else {
					ret.second++;
				}
			}
		}
		return ret;
	}

	int getColor(int y, int x) const {
		return this->board[y][x];
	}

	SimpleState next(std::pair<int, int> action) {
		SimpleState state = SimpleState(this->board, this->depth);
		if (action != std::make_pair(-1, -1)) {
			state.take_action(action.first, action.second);
		}
		state.depth++;
		auto legal_actions = state.legal_actions();
		if (action == std::make_pair(-1, -1) && legal_actions[0] == std::make_pair(-1, -1)) {
			state.pass_end = true;
		}
		return state;
	}

	std::vector<std::pair<int, int>> legal_actions() const {
		std::vector<std::pair<int, int>> ret;
		for (int i = 0; i < board.size(); i++) {
			for (int j = 0; j < board[0].size(); j++) {
				if (is_legal_action(i, j))ret.emplace_back(i, j);
			}
		}
		ret.emplace_back(-1, -1);
		return ret;
	}

	bool is_lose() const {
		auto stones = stone_count();
		return is_done() && stones.first < stones.second;
	}

	bool is_draw() const {
		auto stones = stone_count();
		return is_done() && stones.first == stones.second;
	}

	bool is_done() const {
		auto stones = stone_count();
		return stones.first + stones.second == (int)(board.size()) * (int)(board[0].size()) || this->pass_end;
	}

};