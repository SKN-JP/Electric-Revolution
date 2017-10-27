#pragma once

struct Machine;

//�M�����
enum struct NodeState
{
	None,
	Hi,
	Low,
};

//�M���ڑ��_
struct Node
{
	bool	fixed;			//true:�o�� false:����
	Vec2	inMachinePos;	//���΍��W��
	Vec2	deltaVector;	//�x�W�F�Ȑ��p
	Machine*	machine;
	NodeState	state;

	Node(const Vec2& _pos, const Vec2& _dv, Machine* _machine, NodeState _state, bool _fixed);

	Vec2	pos() const;
};