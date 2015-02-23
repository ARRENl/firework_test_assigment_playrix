#include "stdafx.h"
#include "TestWidget.h"

TestWidget::TestWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _curTex(0)
	, _timer(0)
	, _angle(0)
	, _scale(0.f)
{
	Init();
}

void TestWidget::Init()
{
	srand((unsigned)time(NULL));
	_start_launch_position = IPoint(0,120);
	_lifetime = 1 * 100; //~ (1 seconds * 100 fps) minimum
	_blowdistance = 10; // �������� �� ������ ������ (�������� ����� ���� ������ ������ ��������� � �������� �������) 
						//����� ������ �������� ���� � ������ ������ �� ���������

	_tex0 = Core::resourceManager.Get<Render::Texture>("Background");
	_tex1 = Core::resourceManager.Get<Render::Texture>("Launch_green");
	_tex2 = Core::resourceManager.Get<Render::Texture>("Launch_red");

	_curTex = 0;
	_angle = 0;

	ifstream in;
	in.open(INPUT_PATH);
	if(in.fail())
		return;

	char temp;
	in.get(temp);
	while(!in.eof())
	{
		if(temp == 'L')
		{
			while(temp != '=')
				 in.get(temp);
			 in >> _level;
		}
		else if(temp == 'C')
		{
			while(temp != '=')
				 in.get(temp);
			 in >> _parts_count;
		}
		else
			in.get(temp);
	}
	in.close();
		
}
void TestWidget::DrawBackGround()
{
	IRect texRect = _tex0->getBitmapRect();
	FRect rect(texRect), uv(0, 1, 0, 1);
	_tex0->TranslateUV(rect, uv);

	//�������������� (������� ���� "�����" � "������" �����������)
	//Render::device.MatrixScale(_scale);
	//Render::device.MatrixTranslate(math::Vector3(-texRect.width / 2.f, -texRect.height / 2.f, 0.f));

	_tex0->Bind();
		
	Render::DrawRect(rect, uv);
}

void TestWidget::Draw() 
{
	TestWidget::DrawBackGround();

	//
	// ���� ����� ��������� ��������������� ��� ���������.
	//
	Render::device.SetTexturing(false);
	
	//
	// ����� BeginColor() ������������ � ���� ������� ���� ������ � ������������� �����.
	//
	Render::BeginColor(Color(0, 0, 0, 255)); // green = Color(0, 128, 0, 255)
	
	//
	// ����� DrawRect() ������� � ����������� ���������� ���������� ������, ��������� �� ����
	// ���������� - �������������, ��������� ��� ���� ������� ���� ��� ������ � ����������� (binded) ��������,
	// ���� ��������� ���������������.
	//
	// ����� ������� DrawRect() ������ ���� ����� Texture::Bind(), ���� SetTexturing(false),
	// ����� ���������� ��������� ����� ���������������.
	//
	Render::DrawRect(924, 0, 100, 80);
	
	//
	// ����� EndColor() ������� �� ����� ������� ���� ������, �������������� �������.
	//
	Render::EndColor();
	
	//
	// ����� �������� ���������������.
	//
	Render::device.SetTexturing(true);

	//������� �����
	Render::BindFont("arial");
	Render::PrintString(924 + 100 / 2, 60, "Level = " + utils::lexical_cast(_level), 1.f, CenterAlign);
	Render::PrintString(924 + 100 / 2, 30, "Parts = " + utils::lexical_cast(_parts_count), 1.f, CenterAlign);

	//
	// �������� ������� ��������� ������� ����.
	//
	IPoint mouse_pos = Core::mainInput.GetMousePos();

	//
	// ������������ � ���� ������� �������������� ���������, ����� � ����������
	// ����� ���� ������������ ��� �������������� ������� PopMatrix.
	//
	Render::device.PushMatrix();
	
	//
	// �������� ������� �������������� ���������, ��������� ����� ��������� � ������� ����
	// � ����������� ���������� ������������ ����� ������ ������ ��� z �� ���� _angle.
	//
	Render::device.MatrixTranslate(math::Vector3((float)mouse_pos.x, (float)mouse_pos.y, 0));
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), _angle);

	if (!_curTex)
	{
		//
		// ����� Texture::Draw() ������� ���������� ������ � ��������� ��������
		// � ������ ��������� (0, 0). ����� ��������� � �������������� ��������� ������
		// ������� ��������������� � ����������� �������� ��������������.
		//
		// ��� ������ ������ Texture::Draw() �������� Texture::Bind() �������������.
		//
		_tex1->Draw();
	}
	else
	{
		IRect texRect = _tex2->getBitmapRect();
		//
		// ��� ��������� �������� ����� ������� �������� UV ���������� ��� ����� ��������,
		// ������� ����� �������� �� ������� �������. UV ���������� ������ ���� �������������,
		// �.�., ��� ����������� �� ������� �������� � ��������, ������ ����� ��������
		// ����� 1 (UV ���������� �������� � ��������� 0..1, ���� ����� �� ������ ������ ��
		// ������ �������, ��� ���� � ������ ������������� ��������� �������� REPEAT, ��������
		// ����� ���������� �� ���� ������� ��������������� ���������� ���).
		//

		FRect rect(texRect), uv(0, 1, 0, 1);
		_tex2->TranslateUV(rect, uv);

		Render::device.MatrixScale(_scale);
		Render::device.MatrixTranslate(math::Vector3(-texRect.width / 2.f, -texRect.height / 2.f, 0.f));

		//
		// ����������� ��������.
		//
		_tex2->Bind();
		
		//
		// ����� DrawRect() ������� � ����������� ���������� ���������� ������, ��������� �� ����
		// ���������� - �������������, ��������� ��� ���� ������� ���� ��� ������ � ����������� (binded) ��������,
		// ���� ��������� ���������������.
		//
		// ����� ������� DrawRect() ������ ���� ����� Texture::Bind(), ���� SetTexturing(false),
		// ����� ���������� ��������� ����� ���������������.
		//
		Render::DrawRect(rect, uv);
	}

	//
	// ��������������� ������� �������������� ���������, ������ �� ����� ��������� �����.
	//
	Render::device.PopMatrix();

	//
	// ������ ��� �������, ������� �������� � ��������� (Update() ��� ���������� �������� �� �����).
	//
	_effCont.Draw();
}

void TestWidget::Update(float dt)
{
	//
	// ������� ��������� � ���������
	//
	_effCont.Update(dt);

	//
	// dt - �������� ������� � ��������, ��������� �� ����������� �����.
	// ��� ����� ��������� ������ ��������, � ����������� �� ������������������ �������
	// � ��������� �����.
	//
	// ��� ����, ����� ���� �������� �������� ������ �� �������, � �� �������� ��
	// ������������������ �������, �� ������ ������������ � �� ����� ��������.
	//
	// ����������� ��� ������ � ��������� ���������.
	//
	_timer += dt * 2;
	
	//
	// ����������� ������ � ��������� (0, 2�).
	// ��� ����� ������ ��� �������������� ��������� ������������ ��������,
	// ���� ����� ������������ ��������� ����� (float ������������) ��� ��������
	// �� ����������� ����� ���� ������� ������� (������� �������-�-��).
	//
	// �������� �������� ������ ������ (0, 2�), ������ ��� �� ���������� ��� ��������
	// ��� ������� ������, � ������� �������� ����� ������ ������������ ��������� ���������.
	//
	while (_timer > 2 * math::PI)
	{
		_timer -= 2 * math::PI;
	}
	
	//
	// ������������ ��������� ��������������� � ����������� �� �������.
	//
	_scale = 0.8f + 0.25f * sinf(_timer);

	
	//Drawing Rockets!(and update)
	for(int i = 0; i < _rockets.size(); i++)
	{

		if(!_rockets[i]->lifetime  || !_rockets[i]->dead)
		{			
			if(_rockets[i]->lifetime > 0) _rockets[i]->dir.y -= 0.006; // ����� ��������� ������� ������� ��� �������� �����

			_rockets[i]->start += (_rockets[i]->dir * _rockets[i]->speed);
			_rockets[i]->lifetime -= 1;
			FPoint temp = _rockets[i]->end - _rockets[i]->start;
	
			// ���������� ������ � ���� �� �������
			//
			_rockets[i]->eff->posX = _rockets[i]->start.x;
			_rockets[i]->eff->posY = _rockets[i]->start.y;

			//����� ������� ����� ������ �� ����� �������� �� ������� ������, �� ��� ����� � ��� ��������
			/*if(_rockets[i]->start.x > 1020 || _rockets[i]->start.x < 20 || _rockets[i]->start.y > 748 || _rockets[i]->start.y < 20)
			{
				_rockets[i]->lifetime = 0;
				continue;
			}*/ 

			int distance = sqrt(temp.x * temp.x + temp.y * temp.y);
			if( distance <_blowdistance || !_rockets[i]->lifetime)  
			{
				_rockets[i]->dead = true;

					// ��������� ������.
					//
					_rockets[i]->eff->Finish();
					//_rockets[i]->eff = NULL;  //������ �� ��������� � ��������� ������ ����

				//�������� ������
				ParticleEffect *eff = _effCont.AddEffect("FindCoin2");// FindItem2
				eff->posX = _rockets[i]->start.x;
				eff->posY = _rockets[i]->start.y;
				eff->SetScale(2.0);
				eff->Reset();
				//� ����������
				if(_rockets[i]->level)
				{
					_rockets[i]->level--;
					int counter = _parts_count;
					
					while(counter)
					{
						IPoint randpos;
					
						randpos.x = rand() % 768;
						randpos.y = rand() % 1024;
						int randtime = _lifetime + (100 * (rand() % 2) );
						randtime += rand() % 100;

						rocket * ptr = LaunchRocket(randpos,IPoint( (int)_rockets[i]->start.x, (int)_rockets[i]->start.y), randtime, _rockets[i]->level);
						ptr->speed /= 2.5;
						counter--;
					}
				}

			}
		}
	}
}

bool TestWidget::MouseDown(const IPoint &mouse_pos)
{
	if (Core::mainInput.GetMouseLeftButton())
	{
		//
		// ��� ������� �� ������ ������ ����, ������ ������ ������ �� ������.
		//
		int lt = -1; //� "�������������" ������ ����� ����� �� �����������
		rocket * ptr = LaunchRocket(mouse_pos,_start_launch_position,lt, _level);

		
		
		//
		// � �������� ���� ������� ��������.
		//
		//_angle += 25;
		//while (_angle > 360)
		//{
		//	_angle -= 360;
		//}
	}
	else
	{

		//
		// �������� �������� � 0 �� 1 � ��������.
		//
		_curTex = 1 - _curTex;
	}
	return false;
}

void TestWidget::MouseMove(const IPoint &mouse_pos)
{
	
}

void TestWidget::MouseUp(const IPoint &mouse_pos)
{
	
}

void TestWidget::AcceptMessage(const Message& message)
{
	//
	// ������� ����� ���������� ��������� � �����������.
	//
	// ��������, ��� ����� ������� � ���������� �������� �� �������� ���� ����������
	// ��������� KeyPress � ����� ������� � ���������.
	//
	if (message.getPublisher() == "KeyPress") 
	{
		int code = utils::lexical_cast<int>(message.getData());
		if (code < 0)
		{
			//
			// ������������� �������� ���������� ��� ����������� ����� ������,
			// ��������, -VK_RETURN, -VK_ESCAPE � �.�.
			//
			return;
		}
		
		//
		// ������������� �������� ���������� ��� �������� ��������.
		//
		if (code == 'a')
		{
		}
	}
}

rocket* TestWidget::LaunchRocket(const IPoint & endpos, IPoint startpos, int lt, int rlevel)
{	
	float rocket_speed = 5 + rand() % 6;
	float frocket_speed = (rand() % 100) / 100;
	rocket_speed += frocket_speed;

	rocket * ptr = new rocket(startpos, rocket_speed, lt);
	ptr->end = endpos;
	ptr->dir = endpos - startpos;
	//normalize dir vector
	int vector_lenght = sqrt((ptr->dir.x * ptr->dir.x)  + (ptr->dir.y * ptr->dir.y));
	ptr->dir.x /= vector_lenght;
	ptr->dir.y /= vector_lenght;

	ptr->level = rlevel;
	ptr->id = _rockets.size();
	
	ptr->eff = _effCont.AddEffect("Iskra");
	ptr->eff->posX = ptr->start.x;
	ptr->eff->posY = ptr->start.y;
	ptr->eff->Reset();
	
	_rockets.push_back(ptr);
	return ptr;
}