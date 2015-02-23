#pragma once

#include <iostream>
#include <random>

//#include <cstdlib>
//#include <ctime>
using namespace std;

#define INPUT_PATH "input.txt"

///
/// Виджет - основной визуальный элемент на экране.
/// Он отрисовывает себя, а также может содержать другие виджеты.
///

struct rocket
{
	int id;
	FPoint start;
	FPoint end;
	bool dead;
	FPoint dir;
	float speed;
	ParticleEffect * eff;
	int lifetime;
	int level;
	rocket() { id = -1; start = IPoint(0.0,0.0); end = IPoint(0.0,0.0); dead = false; dir = IPoint(0.0,0.0); speed = 0.0; eff = nullptr; lifetime = 0; level = 0; };
	rocket(IPoint s, float sp, int lt) { start = s; end = IPoint(0.0,0.0); dead = false; dir = IPoint(0.0,0.0); speed = sp; eff = nullptr; lifetime = lt; level = 0; };
};

class TestWidget : public GUI::Widget
{
public:
	TestWidget(const std::string& name, rapidxml::xml_node<>* elem);

	void Draw();
	void DrawBackGround();
	void Update(float dt);
	
	void AcceptMessage(const Message& message);
	
	bool MouseDown(const IPoint& mouse_pos);
	void MouseMove(const IPoint& mouse_pos);
	void MouseUp(const IPoint& mouse_pos);

	rocket* LaunchRocket(const IPoint & startpos, IPoint endpos, int lt, int rlevel);

private:
	void Init();

private:
	float _timer;
	
	float _scale;
	float _angle;

	//Rockets	
	IPoint _start_launch_position;
	std::vector<rocket*> _rockets;
	unsigned int _level;
	unsigned int _parts_count;
	int _lifetime;
	int _blowdistance;
	
	Render::Texture *_tex0;
	Render::Texture *_tex1;
	Render::Texture *_tex2;
	int _curTex;

	EffectsContainer _effCont;
};
