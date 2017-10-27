#include"Planet.h"

Planet::Planet(Game* _game)
	: game(_game)
	, mapTexture(L"assets/map/mapTexture.png")
	, camera(Rect(1280, 720), 4.0, 1.0)
{
	//�}�b�v�̊�b���ǂݍ���
	{
		JSONReader json(L"assets/map/planet.json");
		name = json[L"name"].get<String>();
	}

	//�s�s�f�[�^�̓ǂݍ���
	{
		JSONReader json(L"assets/map/urbans.json");
		urbans.reserve(json.arrayCount());	//�|�C���^�����炳�Ȃ��ׂ�Urban�̃������m��
		for (auto j : json.arrayView()) urbans.emplace_back(this, j);
	}
}