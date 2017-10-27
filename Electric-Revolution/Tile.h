#pragma once

struct Machine;
struct Gearbox;
struct Item;

//�}�b�v�`�b�v
struct Tile
{
	int	type;
	bool	isTradeTile;
	bool	canPutMachine;
	Point	pos;
	Machine*	machine;
	Gearbox*	gearbox;
	Array<Item*>	items;

	Tile();
};
