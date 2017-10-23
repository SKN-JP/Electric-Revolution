#include"Game.h"

void	Game::updateFactory(Factory* _f)
{
	//�M������������
	for (auto& m : _f->machines)
	{
		for (auto& n : m.nodes)
		{
			if (!n.fixed) n.state = NodeState::None;
		}
	}

	//�M���`�B����
	for (;;)
	{
		bool flag = false;
		for (auto& m : _f->machines)
		{
			if (m.updateConnects()) flag = true;
		}
		for (auto& w : _f->wires)
		{
			if (w.update()) flag = true;
		}
		_f->wires.remove_if([](Wire& w) { return w.broken; });
		if (!flag) break;
	}

	//�@�B�ŗL����
	for (auto& m : _f->machines) m.updateSystem();
}