#pragma once
#include "Define.h"

class Title :public MyApp::Scene {
private:

	Rect button_weak = Rect(Arg::center = Scene::Center().movedBy(-250, 0), 200, 60);
	Transition button_weak_transition = Transition(0.4s, 0.2s);

	Rect button_normal = Rect(Arg::center = Scene::Center().movedBy(0, 0), 200, 60);
	Transition button_normal_transition = Transition(0.4s, 0.2s);

	Rect button_strong = Rect(Arg::center = Scene::Center().movedBy(250, 0), 200, 60);
	Transition button_strong_transition = Transition(0.4s, 0.2s);

	Rect button_exit = Rect(Arg::center = Scene::Center().movedBy(0, 100), 300, 60);
	Transition button_exit_transition = Transition(0.4s, 0.2s);

	Rect button_player_turn = Rect(Arg::center = Scene::Center().movedBy(0, 200), 250, 50);
	Transition button_player_turn_transition = Transition(0.4s, 0.2s);

public:
	Title(const InitData& init) :IScene(init) {}

	void update() override {
		button_weak_transition.update(button_weak.mouseOver());
		button_normal_transition.update(button_normal.mouseOver());
		button_strong_transition.update(button_strong.mouseOver());
		button_exit_transition.update(button_exit.mouseOver());
		button_player_turn_transition.update(button_player_turn.mouseOver());

		if (button_weak.mouseOver() || button_normal.mouseOver() || button_strong.mouseOver() || button_player_turn.mouseOver() || button_exit.mouseOver()) {
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		if (button_player_turn.leftClicked()) {
			getData().player_is_first = !getData().player_is_first;
		}
		if (button_weak.leftClicked()) {
			getData().cpuType = 0;
			changeScene(State::Game);
		}
		if (button_normal.leftClicked()) {
			getData().cpuType = 1;
			changeScene(State::Game);
		}
		if (button_strong.leftClicked()) {
			getData().cpuType = 2;
			changeScene(State::Game);
		}

		if (button_exit.leftClicked()) {
			System::Exit();
		}
	}

	void draw() const override {
		const String titleText = U"リバーシ";
		const Vec2 center(Scene::Center().x, 120);
		FontAsset(U"Title")(titleText).drawAt(center.movedBy(4, 6), ColorF(0.0, 0.5));
		FontAsset(U"Title")(titleText).drawAt(center);

		button_weak.draw(ColorF(1.0, button_weak_transition.value())).drawFrame(2);
		button_normal.draw(ColorF(1.0, button_normal_transition.value())).drawFrame(2);
		button_strong.draw(ColorF(1.0, button_strong_transition.value())).drawFrame(2);
		button_exit.draw(ColorF(1.0, button_exit_transition.value())).drawFrame(2);
		button_player_turn.draw(ColorF(1.0, button_player_turn_transition.value())).drawFrame(2);


		FontAsset(U"Menu")(U"よわい").drawAt(button_weak.center(), ColorF(0.25));
		FontAsset(U"Menu")(U"ふつう").drawAt(button_normal.center(), ColorF(0.25));
		FontAsset(U"Menu")(U"つよい").drawAt(button_strong.center(), ColorF(0.25));
		FontAsset(U"Menu")(U"おわる").drawAt(button_exit.center(), ColorF(0.25));

		FontAsset(U"Menu")(U"{}でプレイ!"_fmt(getData().player_is_first ? U"先手" : U"後手")).drawAt(button_player_turn.center(), ColorF(0.0));

	}
};
