#include"Game.h"
#include"Factory.h"

Game::Game()
	: mainFactory(nullptr)
	, camera()
{
	//Window::Resize(1280, 720);
	Graphics::SetFullScreen(true, Graphics::EnumOutputs().front().displayModes.back().size);
	Window::SetTitle(L"Electric Revolution");
	Graphics::SetBackground(Palette::Darkkhaki);

	for (auto c : FileSystem::DirectoryContents(L"assets/machines/"))
	{
		if (c.includes(L"config.ini")) Machine::blueprints.emplace_back(c.removed(L"config.ini"));
	}
	for (auto c : FileSystem::DirectoryContents(L"assets/items/")) Item::textureAssets.emplace_back(c);

	Size factorySize(64, 128);
	mainFactory = &factories.emplace_back(factorySize);
	camera.restrictedRegion.set(Size(-10, -10), factorySize + Size(20, 20));
	camera.drawingRegion.set(RectF(Window::ClientRect()).scaledAt(Vec2::Zero(), 0.05).setCenter(factorySize / 2));
	camera.smoothDrawingRegion.set(camera.drawingRegion);

}

void	Game::update()
{
	ClearPrint();
	
	Print << L"�I�𒆂�R�L�[�ŉ�]�ł��܂��B";
	Print << L"�J�E���^�͍����̃s�������͂ł�";
	Print << L"�@�B���ړ�����Ƃ���Shift�������Ȃ��獶�N���b�N���Ă�������";

	//�J��������
	camera.update();

	for (auto& f : factories) updateFactory(&f);

	drawFactory(mainFactory);

	ui.update();
}
