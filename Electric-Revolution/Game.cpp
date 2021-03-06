#include"Game.h"

Game::Game()
	: sceneState(SceneState::Title)
	, planet(this)
{
	//ブループリントの設定
	{
		auto dc = FileSystem::DirectoryContents(U"assets/machines");

		//ブループリント数の計算
		int numBlueprint = 0;
		for (auto c : dc)
		{
			if (c.includes(U"main.lua")) numBlueprint++;
		}

		//ブループリントのメモリ領域予約
		blueprints.reserve(numBlueprint);

		//ブループリントの読み込み
		for (auto c : dc)
		{
			if (c.includes(U"main.lua"))
			{
				sol::state lua;
				lua.script_file(Unicode::Narrow(c));
				auto name = Unicode::FromUTF8(lua["machine"]["name"].get<std::string>());
				auto group = Unicode::FromUTF8(lua["machine"]["group"].get<std::string>());
				blueprints.emplace_back(int(blueprints.size()), name, c.removed(U"main.lua"));
				if (!groups.any([&group](const Group& g) { return group == g.name; })) groups.emplace_back(group);

				//デバッグ用
				Logger << group << U", " << name;
			}
		}

		//グループへの登録
		for (auto& g : groups)
		{
			for (auto& b : blueprints)
			{
				sol::state lua;
				lua.script_file(Unicode::Narrow(b.mainPath + U"main.lua"));
				auto group = Unicode::FromUTF8(lua["machine"]["group"].get<std::string>());
				if (g.name == group) g.blueprints.emplace_back(&b);
			}
		}
	}

	//システム設定
	Graphics::SetBackground(Palette::Skyblue);
	Window::Resize(1280, 720);
	Window::SetTitle(U"Electric Revolution");
	//System::SetExitEvent(WindowEvent::CloseButton);
	//while (!MouseL.down() && System::Update());
	music.setMusic(U"assets/music/星くずの行進.mp3");
	music.setVolume(0.25);

	//メインループ
	while (System::Update())
	{
		if (KeyF11.down())
		{
			if (Window::GetState().fullScreen) Graphics::SetFullScreen(false, Size(1280, 720));
			else Graphics::SetFullScreen(true, Graphics::EnumOutputs().front().displayModes.back().size);
		}

		switch (sceneState)
		{
		case SceneState::Title:
			updateTitle();
			break;
		case SceneState::MapView:
			updateMapView();
			break;
		case SceneState::FactoryView:
			updateFactoryView();
			break;
		}
	}
}

Font&	Game::font(int _size)
{
	while (_size > int(fontAssets.size()))
	{
		fontAssets.emplace_back(int(fontAssets.size() + 1));
	}

	return fontAssets[_size - 1];
}

Texture	Game::texture(const FilePath& _path)
{
	for (auto& ta : textureAssets)
	{
		if (ta.first == _path) return ta.second;
	}

	return textureAssets.emplace_back(_path, _path).second;
}

Texture	Factory::texture(const FilePath& _path)
{
	return urban->planet->game->texture(_path);
}