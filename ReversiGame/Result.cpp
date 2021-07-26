#pragma once
#include "Define.h"

class Result: public MyApp::Scene {
private:
	Rect m_startButton = Rect(Arg::center = Scene::Center().movedBy(0, 0), 300, 60);
	Transition m_startTransition = Transition(0.4s, 0.2s);
	Rect m_exitButton = Rect(Arg::center = Scene::Center().movedBy(0, 100), 300, 60);
	Transition m_exitTransition = Transition(0.4s, 0.2s);

public:
	Result(const InitData& init) :IScene(init) {}
	void update() override {
		m_startTransition.update(m_startButton.mouseOver());
		m_exitTransition.update(m_exitButton.mouseOver());

		if (m_startButton.mouseOver() || m_exitButton.mouseOver()) {
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		if (m_startButton.leftClicked()) {
			changeScene(State::Game);
		}

		if (m_exitButton.leftClicked()) {
			changeScene(State::Title);
		}
	}

	void draw() const override {
		const String resultText = U"Black:{} vs {}:White !"_fmt(getData().black,getData().white);
		const String winnerText = getData().result==0?U"Draw !":(getData().result==1?U"You Win !":U"You Lose ...");
		const Vec2 center(Scene::Center().x, 120);
		const Vec2 center_below(Scene::Center().x, 180);

		FontAsset(U"Result")(resultText).drawAt(center.movedBy(2, 3), ColorF(0.0, 0.5));
		FontAsset(U"Result")(resultText).drawAt(center, getData().result == 0 ? Palette::Green : (getData().result == 1 ? Palette::White : Palette::Gray));
		FontAsset(U"Result")(winnerText).drawAt(center_below.movedBy(2, 3), ColorF(0.0, 0.5));
		FontAsset(U"Result")(winnerText).drawAt(center_below,getData().result==0?Palette::Green:(getData().result == 1?Palette::White:Palette::Gray));


		m_startButton.draw(ColorF(1.0, m_startTransition.value())).drawFrame(2);
		m_exitButton.draw(ColorF(1.0, m_exitTransition.value())).drawFrame(2);

		FontAsset(U"Menu")(U"‚à‚¤ˆê‰ñ").drawAt(m_startButton.center(), ColorF(0.25));
		FontAsset(U"Menu")(U"ƒ^ƒCƒgƒ‹‚É‚à‚Ç‚é").drawAt(m_exitButton.center(), ColorF(0.25));
	}
};