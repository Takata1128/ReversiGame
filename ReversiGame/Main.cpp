#include "Define.h"
#include "Title.cpp"
#include "Game.cpp"
#include "Result.cpp"

using MyApp = SceneManager<State, GameData>;

void Main()
{
	FontAsset::Register(U"Title", 120, Typeface::Regular);
	FontAsset::Register(U"Menu", 30, Typeface::Regular);
	FontAsset::Register(U"Score", 36, Typeface::Bold);
	FontAsset::Register(U"Info", 36, Typeface::Bold);
	FontAsset::Register(U"Result", 60, Typeface::Bold);



	// 背景色を設定
	Scene::SetBackground(ColorF(0.2, 0.8, 0.4));

	MyApp manager;
	manager.add<Title>(State::Title).add<Game>(State::Game).add<Result>(State::Result).setFadeColor(ColorF(1.0));

	while (System::Update())
	{
		if (!manager.update()) {
			break;
		}
	}
}

