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
	bool	fixed;
	Vec2	inMachinePos;		//���΍��W��
	Vec2	deltaVector;
	Machine*	machine;
	NodeState	state;


	Node(const Vec2& _pos, const Vec2& _dv, Machine* _machine, NodeState _state, bool _fixed);

	Vec2	pos() const;
};