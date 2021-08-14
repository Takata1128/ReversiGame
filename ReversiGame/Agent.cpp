#pragma once
#include "Define.h"
#include "SimpleState.cpp"

class Agent {
protected:
	std::random_device rnd;
	std::mt19937 mt;
public:
	Agent() :mt(rnd()) {}
	virtual std::pair<int, int> select_action(SimpleState state) = 0;
};

class RandomAgent :public Agent {
public:
	RandomAgent() {}
	std::pair<int, int> select_action(SimpleState state) {
		auto legal_actions = state.legal_actions();
		int n = legal_actions.size();
		if (n >= 2) {
			legal_actions.pop_back();
			n--;
		}
		else if (n == 1) {
			return legal_actions[0];
		}
		std::uniform_int_distribution<> rand(0, n - 1);
		int idx = rand(mt);
		assert(0 <= idx && idx < legal_actions.size());
		return legal_actions[idx];
	}
};

class MonteCalroAgent :public Agent {
	RandomAgent random_agent;
public:
	MonteCalroAgent() {}
	std::pair<int, int> select_action(SimpleState state) {
		auto legal_actions = state.legal_actions();

		// パス以外ができるならパスを除外
		if (legal_actions.size() > 1)legal_actions.pop_back();

		auto values = std::vector<int>(legal_actions.size());
		for (int i = 0; i < legal_actions.size(); i++) {
			for (int _ = 0; _ < MONTECALRO_SEARCH_COUNT; _++) {
				values[i] += -playout(state.next(legal_actions[i]));
			}
		}
		int val_max = -10000;
		std::vector<std::pair<int, int>> cood;
		for (int i = 0; i < legal_actions.size(); i++) {
			if (values[i] > val_max) {
				val_max = values[i];
				cood.clear();
				cood.push_back(legal_actions[i]);
			}
			else if (values[i] == val_max) {
				cood.push_back(legal_actions[i]);
			}
		}
		int m = cood.size();
		std::uniform_int_distribution<> rand(0, m - 1);
		return cood[rand(mt)];
	}

	int playout(SimpleState state) {
		if (state.is_lose()) {
			return -1;
		}
		if (state.is_draw()) {
			return 0;
		}
		return -playout(state.next(random_agent.select_action(state)));
	}
};

class MonteCalroTreeAgent :public Agent {
	RandomAgent random_agent;
public:
	int playout(SimpleState state) {
		if (state.is_lose()) {
			return -1;
		}
		if (state.is_draw()) {
			return 0;
		}
		return -playout(state.next(random_agent.select_action(state)));
	}
	MonteCalroTreeAgent() {}
	class Node {
		SimpleState state;
		int w, n;
		std::vector<Node> child_nodes;
	public:
		Node(SimpleState state) :state(state), w(0), n(0) {}
		int evaluate(MonteCalroTreeAgent& agent) {
			if (this->state.is_done()) {
				int value = this->state.is_lose() ? -1 : 0;
				this->w += value;
				this->n++;
				return value;
			}

			if (this->child_nodes.empty()) {
				int value = agent.playout(this->state);
				this->w += value;
				this->n++;
				if (this->n == MCTS_EXPAND_LIMIT) {
					expand();
				}
				return value;
			}
			else {
				Node& next_node = next_child_node();
				int value = -next_node.evaluate(agent);
				this->w += value;
				this->n++;
				return value;
			}
		}

		void expand() {
			auto legal_actions = state.legal_actions();
			for (auto action : legal_actions) {
				this->child_nodes.push_back(Node(this->state.next(action)));
			}
		}

		Node& next_child_node() {
			for (auto& child_node : this->child_nodes) {
				if (child_node.n == 0)return child_node;
			}
			int t = 0;
			for (auto child_node : this->child_nodes) {
				t += child_node.n;
			}
			std::vector<double> ucb1_values;
			for (auto child_node : this->child_nodes) {
				ucb1_values.push_back(-child_node.w / (double)child_node.n + sqrt((2 * log(t) / (double)child_node.n)));
			}
			int idx = -1;
			double val_max = -10000;
			for (int i = 0; i < ucb1_values.size(); i++) {
				if (ucb1_values[i] > val_max) {
					idx = i;
					val_max = ucb1_values[i];
				}
			}
			return this->child_nodes[idx];
		}

		int get_n() {
			return this->n;
		}

		std::vector<Node> get_child_nodes() {
			return this->child_nodes;
		}
	};


	std::pair<int, int> select_action(SimpleState state) {
		Node root_node = Node(state);
		root_node.expand();
		for (int i = 0; i < MONTECALRO_TREE_SEARCH_COUNT; i++) {
			root_node.evaluate(*this);
		}
		auto legal_actions = state.legal_actions();
		// パス以外ができるならパスを除外
		if (legal_actions.size() > 1)legal_actions.pop_back();

		std::vector<int> n_list;
		for (auto child_node : root_node.get_child_nodes()) {
			n_list.push_back(child_node.get_n());
		}
		int n_max = -10000;
		int idx = -1;
		for (int i = 0; i < legal_actions.size(); i++) {
			if (n_list[i] > n_max) {
				idx = i;
				n_max = n_list[i];
			}
		}
		return legal_actions[idx];
	}
};