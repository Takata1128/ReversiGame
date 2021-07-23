#pragma once
#include "Define.h"

enum CellState {
	None = 0,
	Black = 1,
	White = 2
};

class GameState {
private:
	const static int BOARD_HEIGHT = 480, BOARD_WIDTH = 480;
	const static int OFFSET = 20;
	int m_height, m_width;
	int m_cell_size;
	int m_black_count, m_white_count;

	class Cell {
	private:
		CellState m_state;
		int m_x, m_y, m_size;
		bool m_clickable;
		Rect m_rect;
		Circle m_stone;
		Circle m_clickable_marker;

	public:
		Cell() :m_state(CellState::None), m_x(0), m_y(0), m_clickable(false) {}
		Cell(CellState state, int y, int x, int cellSize) :m_state(state), m_x(x), m_y(y), m_size(cellSize) {
			m_rect = Rect(OFFSET + m_size * y, OFFSET + m_size * x, m_size);
			m_stone = Circle(OFFSET + m_size * y + m_size / 2, OFFSET + m_size * x + m_size / 2, m_size / 3);
			m_clickable_marker = Circle(OFFSET + m_size * y + m_size / 2, OFFSET + m_size * x + m_size / 2, m_size / 6);
		}
		bool turn(CellState next) {
			if (m_clickable && m_rect.leftClicked()) {
				m_state = next;
				return true;
			}
			return false;
		}
		void update() {
			if (m_clickable && m_rect.leftClicked()) {
				m_state = CellState::Black;
			}
		}
		void draw() const {
			m_rect.drawFrame(2, Palette::Black);
			if (m_state == CellState::Black) {
				m_stone.draw(Palette::Black);
			}
			else if (m_state == CellState::White) {
				m_stone.draw(Palette::White);
			}
			if (m_clickable) {
				m_clickable_marker.draw(Palette::Orange);
			}
		}

		CellState back() {
			if (m_state == CellState::Black)return CellState::White;
			else if (m_state == CellState::White)return CellState::Black;
			else return CellState::None;
		}

		void set_state(CellState state) {
			m_state = state;
		}

		void set_clickable(bool b) {
			m_clickable = b;
		}

		bool get_clickable() {
			return m_clickable;
		}

		CellState get_state() {
			return m_state;
		}

		void set_rect(Rect rect) {
			m_rect = rect;
		}
	};
	std::vector<std::vector<Cell>> m_board;


public:
	GameState(std::initializer_list<std::initializer_list<int>> init) {
		std::vector<std::vector<int>> tmp_board(init.begin(), init.end());
		m_height = tmp_board.size();
		m_width = tmp_board[0].size();
		m_cell_size = BOARD_HEIGHT / m_height;
		m_black_count = 0, m_white_count = 0;
		m_board.assign(m_height, std::vector<Cell>(m_width, Cell()));
		for (int i = 0; i < m_height; i++) {
			for (int j = 0; j < m_width; j++) {
				m_board[i][j] = Cell(CellState(tmp_board[i][j]), i, j, m_cell_size);
				if (tmp_board[i][j] == CellState::Black) {
					m_black_count++;
				}
				else if (tmp_board[i][j] == CellState::White) {
					m_white_count++;
				}
			}
		}
		reset_clickable();
	}
	GameState(int height, int width) :m_height(height), m_width(width), m_cell_size(BOARD_HEIGHT / height), m_black_count(0), m_white_count(0) {
		this->m_board.resize(height);
		for (int i = 0; i < height; i++) {
			m_board[i].resize(width, Cell());
		}

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				m_board[i][j] = Cell(CellState::None, i, j, m_cell_size);
			}
		}
	}

	int get_black_count() const {
		return m_black_count;
	}

	void set_black_count(int black) {
		m_black_count = black;
	}

	int get_white_count() const {
		return m_white_count;
	}

	void set_white_count(int white) {
		m_white_count = white;
	}

	int get_stone_count() const {
		return m_black_count + m_white_count;
	}

	std::pair<int, int> recount() {
		int black = 0, white = 0;
		for (int i = 0; i < m_height; i++) {
			for (int j = 0; j < m_width; j++) {
				if (m_board[i][j].get_state() == CellState::Black) {
					black++;
				}
				else if (m_board[i][j].get_state() == CellState::White) {
					white++;
				}
			}
		}
		return std::make_pair(black, white);
	}

	bool is_first_turn() const {
		return get_stone_count() % 2 == 0;
	}

	CellState currentStone() const {
		return is_first_turn() ? CellState::Black : CellState::White;
	}

	bool is_terminal() const {
		return get_stone_count() == m_height * m_width;
	}

	void update() {
		bool isUpdated = false;
		int py = 0, px = 0;
		for (int i = 0; i < m_height; i++) {
			for (int j = 0; j < m_width; j++) {
				bool b = m_board[i][j].turn(currentStone());
				if (b) {
					py = i, px = j;
				}
				isUpdated |= b;
			}
		}
		if (isUpdated) {
			m_board = calc(py, px, currentStone()).first;
			auto [b, w] = recount();
			set_black_count(b);
			set_white_count(w);
			reset_clickable();
		}

	}

	std::pair<std::vector<std::vector<Cell>>, int> calc(int y, int x, CellState color) {
		std::vector<std::vector<Cell>> next_board = m_board;
		std::vector<std::pair<int, int>> updates;
		next_board[y][x].set_state(color);
		// çXêV
		CellState baseState = next_board[y][x].get_state();
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				if (dx == 0 && dy == 0)continue;
				std::vector<std::pair<int, int>> tmp;
				int nx = x + dx, ny = y + dy;
				while (0 <= nx && nx < m_width && 0 <= ny && ny < m_height && m_board[ny][nx].get_state() != CellState::None) {
					if (baseState != m_board[ny][nx].get_state())
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
		for (auto [py, px] : updates) {
			next_board[py][px].set_state(baseState);
		}
		return std::make_pair(next_board, updates.size());
	}

	void reset_clickable() {
		for (int i = 0; i < m_height; i++) {
			for (int j = 0; j < m_width; j++) {
				m_board[i][j].set_clickable(false);
			}
		}
		for (int i = 0; i < m_height; i++) {
			for (int j = 0; j < m_width; j++) {
				if (m_board[i][j].get_state() != CellState::None)continue;
				auto p = calc(i, j, currentStone());
				if (p.second != 0) {
					m_board[i][j].set_clickable(true);
				}
			}
		}
	}

	void draw() const {
		for (int i = 0; i < m_height; i++) {
			for (int j = 0; j < m_width; j++) {
				m_board[i][j].draw();
			}
		}
	}
};

class Game :public MyApp::Scene {
private:
	bool isFirstTurn = 1;

public:
	Game(const InitData& init) :IScene(init) {}
	GameState board = GameState(
		{
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,1,2,0,0,0},
		{0,0,0,2,1,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0}
		}
	);
	void update() override {
		board.update();
		isFirstTurn = board.is_first_turn();
	}

	void draw() const override {
		board.draw();
		if (isFirstTurn) {
			FontAsset(U"Info")(U"êÊéË").drawAt(Vec2(620, 550), Palette::Black);
		}
		else {
			FontAsset(U"Info")(U"å„éË").drawAt(Vec2(620, 550));
		}
		FontAsset(U"Info")(U"îíêŒÇÃêî:{}"_fmt(board.get_white_count())).drawAt(Vec2(620, Scene::Center().movedBy(0, -100).y), Palette::White);
		FontAsset(U"Info")(U"çïêŒÇÃêî:{}"_fmt(board.get_black_count())).drawAt(Vec2(620, Scene::Center().movedBy(0, -50).y), Palette::Black);

	}
};
