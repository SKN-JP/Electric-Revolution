#include"Game.h"

Game::Game()
	: sceneState(SceneState::Title)
	, planet(this)
{
	//�u���[�v�����g�̐ݒ�
	{
		auto dc = FileSystem::DirectoryContents(L"assets/machines");

		//�u���[�v�����g���̌v�Z
		int numBlueprint = 0;
		for (auto c : dc)
		{
			if (c.includes(L"main.lua")) numBlueprint++;
		}

		//�u���[�v�����g�̃������̈�\��
		blueprints.reserve(numBlueprint);

		//�u���[�v�����g�̓ǂݍ���
		for (auto c : dc)
		{
			if (c.includes(L"main.lua"))
			{
				sol::state lua;
				lua.script_file(CharacterSet::Narrow(c));
				auto name = CharacterSet::FromUTF8(lua["machine"]["name"].get<std::string>());
				auto group = CharacterSet::FromUTF8(lua["machine"]["group"].get<std::string>());
				blueprints.emplace_back(int(blueprints.size()), name, c.removed(L"main.lua"));
				if (!groups.any([&group](const Group& g) { return group == g.name; })) groups.emplace_back(group);

				//�f�o�b�O�p
				Output << group << L", " << name;
			}
		}

		//�O���[�v�ւ̓o�^
		for (auto& g : groups)
		{
			for (auto& b : blueprints)
			{
				sol::state lua;
				lua.script_file(CharacterSet::Narrow(b.mainPath + L"main.lua"));
				auto group = CharacterSet::FromUTF8(lua["machine"]["group"].get<std::string>());
				if (g.name == group) g.blueprints.emplace_back(&b);
			}
		}
	}

	//�V�X�e���ݒ�
	Graphics::SetBackground(Palette::Skyblue);
	Window::Resize(1280, 720);
	Window::SetTitle(L"Electric Revolution");
	//System::SetExitEvent(WindowEvent::CloseButton);
	//while (!MouseL.down() && System::Update());
	music.setMusic(L"assets/music/�������̍s�i.mp3");
	music.setVolume(0.25);

	//���C�����[�v
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