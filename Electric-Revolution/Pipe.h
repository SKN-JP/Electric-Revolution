#pragma once

struct Factory;
struct Pipe;

struct Plumber
{
	int	numRegistered;
	bool	enabled;
	double	mole;	//������
	double	volume;	//�e��
	Factory*	factory;

	Plumber(Factory* _factory);

	void	addPipe(Pipe* _pipe);
	void	removePipe(Pipe* _pipe);
	double	pressure() const;	//����
};

struct Pipe
{
	bool enabled;
	Point	pos;
	Plumber*	plumber;
	Factory*	factory;

	Pipe(Factory* _factory);
	void	setPos(const Point& _pos);
	Array<Pipe*>	nearPipes();
};