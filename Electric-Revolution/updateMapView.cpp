#include"Game.h"

void	Game::updateMapView()
{
	//�J�����̈ړ������X�V
	planet.camera.update();

	{
		//���_�ړ��̓K�p
		auto t = planet.camera.createTransformer();

		//�}�b�v�e�N�X�`��
		planet.mapTexture.draw();

		//Urban�̕`��
		for (auto& u : planet.urbans)
		{
			if (Circle(u.pos, 6).mouseOver())
			{
				Circle(u.pos, 4).draw(Palette::Red).drawFrame(1, Palette::Black);

				//�A�C�R���������ꂽ��V�[�����ڍs
				if (MouseL.down())
				{
					sceneState = SceneState::FactoryView;

					mainFactory = &u.factory;
					mainFactory->isMain = true;
				}
			}
			else
			{
				Circle(u.pos, 4).draw(Palette::White).drawFrame(1, Palette::Black);
			}

			//�s�s���̕\��
			font(16)(u.name).drawAt(u.pos.movedBy(0, -12), Palette::Black);
		}
	}
}