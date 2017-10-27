#pragma once

struct Tile;
struct Machine;

//���͌n��
struct Gearbox
{
	Tile*	tile;
	double	resistance;
	double	pressure;
	double	gain;
	Point	inMachinePos;		//���΍��W��
	Machine*	machine;
	Array<Gearbox*>	connectedGearbox;	//�ڑ����̃M�A�{�b�N�X

	Gearbox(Machine* _machine, const Point& _pos, double _resistance);

	Point	pos() const;
	Vec2	centerPos() const;
};