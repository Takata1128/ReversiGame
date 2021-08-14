#pragma once
#include "Define.h"
#include "SimpleState.cpp"
#include "Agent.cpp"

enum CellState {
	None = -1,
	Black = 0,
	White = 1
};

class GameState {
private:
	const static int BOARD_HEIGHT = 480, BOARD_WIDTH = 480;
	const static int OFFSET = 20;
	int m_height, m_width;
	int m_cell_size;
	SimpleState state;


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

		bool select() {
			if (m_clickable && m_rect.leftClicked()) {
				return true;
			}
			return false;
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
		state = SimpleState(tmp_board, 0);
		m_height = tmp_board.size();
		m_width = tmp_board[0].size();
		m_cell_size = BOARD_HEIGHT / m_height;
		m_board.assign(m_height, std::vector<Cell>(m_width, Cell()));
		update_board();
	}

	void update_board() {
		for (int i = 0; i < m_height; i++) {
			for (int j = 0; j < m_width; j++) {
				this->m_board[i][j] = Cell(CellState(this->state.getColor(i, j)), i, j, m_cell_size);
			}
		}
		auto legal_actions = state.legal_actions();
		for (int i = 0; i < m_height; i++) {
			for (int j = 0; j < m_width; j++) {
				m_board[i][j].set_clickable(false);
			}
		}
		for (auto [y, x] : legal_actions) {
			if (y >= 0 && x >= 0) {
				m_board[y][x].set_clickable(true);
			}
		}
	}

	int get_black_count() const {
		return is_first_turn() ? this->state.stone_count().first : this->state.stone_count().second;
	}

	int get_white_count() const {
		return is_first_turn() ? this->state.stone_count().second : this->state.stone_count().first;
	}

	int get_stone_count() const {
		return get_black_count() + get_white_count();
	}

	bool is_terminal() const {
		return state.is_done();
	}

	bool is_first_turn() const {
		return state.teban() == 0;
	}

	void update(bool pass) {
		if (pass) {
			take_action(-1, -1);
		}
		else {
			bool isUpdated = false;
			int py = 0, px = 0;
			for (int i = 0; i < m_height; i++) {
				for (int j = 0; j < m_width; j++) {
					bool b = m_board[i][j].select();
					if (b) {
						py = i, px = j;
					}
					isUpdated |= b;
				}
			}
			if (isUpdated) {
				take_action(py, px);
			}
		}
	}

	void update_by_cpu(Agent& agent) {
		auto [y, x] = agent.select_action(this->state);
		take_action(y, x);
	}

	void take_action(int y, int x) {
		this->state = state.next(std::make_pair(y, x));
		update_board();
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
	bool player_is_first;
	bool is_first_turn = true;
	Rect info_area_rect = Rect(Arg::center = Scene::Center().movedBy(250, 0), 300, 600);
	Rect m_passButton = Rect(Arg::center = Scene::Center().movedBy(250, 150), 200, 60);
	Transition m_passTransition = Transition(0.4s, 0.2s);
	std::unique_ptr<Agent> agent;

public:
	Game(const InitData& init) :IScene(init) {
		int cpu_type = getData().cpuType;
		this->player_is_first = getData().player_is_first;
		if (cpu_type == 0) {
			this->agent = std::make_unique<RandomAgent>();
		}
		else if (cpu_type == 1) {
			this->agent = std::make_unique<MonteCalroAgent>();
		}
		else {
			this->agent = std::make_unique<MonteCalroTreeAgent>();
		}
	}
	GameState board = GameState(
		{
		{-1,-1,-1,-1,-1,-1,-1,-1},
		{-1,-1,-1,-1,-1,-1,-1,-1},
		{-1,-1,-1,-1,-1,-1,-1,-1},
		{-1,-1,-1,1,0,-1,-1,-1},
		{-1,-1,-1,0,1,-1,-1,-1},
		{-1,-1,-1,-1,-1,-1,-1,-1},
		{-1,-1,-1,-1,-1,-1,-1,-1},
		{-1,-1,-1,-1,-1,-1,-1,-1}
		}
	);
	void update() override {
		m_passTransition.update(m_passButton.mouseOver());
		bool is_pass = m_passButton.leftClicked();

		if (player_is_first) {
			if (is_first_turn) { // Human
				board.update(is_pass);
			}
			else { // AI
				board.update_by_cpu(*agent);
			}
		}
		else {
			if (is_first_turn) { // AI
				board.update_by_cpu(*agent);
			}
			else { // Human
				board.update(is_pass);
			}
		}

		is_first_turn = board.is_first_turn();
		if (board.is_terminal()) {
			getData().black = board.get_black_count();
			getData().white = board.get_white_count();
			int winner = 1, loser = 2;
			if (!player_is_first)std::swap(winner, loser);
			getData().result = board.get_black_count() == board.get_white_count() ? 0 : (board.get_black_count() > board.get_white_count() ? winner : loser);
			changeScene(State::Result);
		}
	}

	void draw() const override {
		board.draw();
		info_area_rect.draw(Palette::Gray);

		if (is_first_turn) {
			if (!player_is_first)FontAsset(U"Info")(U"CPU思考中...").drawAt(Scene::Center(), Palette::White);
			FontAsset(U"Info")(U"先手：{}"_fmt(this->player_is_first ? U"あなた" : U"CPU")).drawAt(Vec2(620, 550), Palette::Black);
		}
		else {
			if (player_is_first)FontAsset(U"Info")(U"CPU思考中...").drawAt(Scene::Center(), Palette::White);
			FontAsset(U"Info")(U"後手：{}"_fmt(this->player_is_first ? U"CPU" : U"あなた")).drawAt(Vec2(620, 550), Palette::White);
		}


		FontAsset(U"Info")(U"{}:{}"_fmt(this->player_is_first ? U"あなた" : U"CPU", this->player_is_first ? board.get_white_count() : board.get_black_count())).drawAt(Vec2(620, Scene::Center().movedBy(0, -250).y), this->player_is_first ? Palette::White : Palette::Black);
		FontAsset(U"Info")(U"{}:{}"_fmt(this->player_is_first ? U"CPU" : U"あなた", this->player_is_first ? board.get_black_count() : board.get_white_count())).drawAt(Vec2(620, Scene::Center().movedBy(0, -200).y), this->player_is_first ? Palette::Black : Palette::White);

		m_passButton.draw(ColorF(1.0, m_passTransition.value())).drawFrame(2);
		FontAsset(U"Info")(U"パス").drawAt(m_passButton.center(), ColorF(0.25));
	}
};
