#include"Game.h"

void	Game::updateTitle()
{
	font(128)(U"Electric Revolution").drawAt(Window::Center());

	if (MouseL.down())
	{
		music.setMusic(U"assets/music/�A�[�P�[�h�X.mp3");
		sceneState = SceneState::MapView;
	}
}