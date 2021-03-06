#pragma once

struct Machine;
struct Factory;
struct Group;
struct Blueprint;

enum struct UIMode
{
	None,
	EditMachineMode,
	EditWireMode,
	EditGearbox,
	EditPipeMode,
	Exit,
};

struct Forklift
{
	int angle;
	bool	enabled;
	Size	baseSize;
	Rect	region;
	Texture	baseTexture;
	Machine*	selectedMachine;	//nullptrなら新規Machine作成
	Factory*	factory;
	Blueprint*	blueprint;

	Forklift(Factory* _factory);

	static Rect	trashboxRegion()
	{
		return Rect(Window::Size().x - 128, 32, 96, 96);
	}
	void	update();
	void	set(Machine* _sm);
	void	set(Blueprint* _blueprint);
	bool	canPutMachine() const;
};

struct UI
{
	bool	mouseOver;
	Group*	selectedGroup;
	UIMode	uiMode;
	Texture	uiTexture;
	Factory*	factory;

	UI(Factory* _factory);
};