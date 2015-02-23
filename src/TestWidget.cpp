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
	_blowdistance = 10; // проверка на радиус взырва (проблемы могут если радиус взрыва маленький а скорость большая) 
						//тогда снаряд пролетит мимо и тригер взрыва не сработает

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

	//преобразование (функции ниже "тянут" и "сужают" изображение)
	//Render::device.MatrixScale(_scale);
	//Render::device.MatrixTranslate(math::Vector3(-texRect.width / 2.f, -texRect.height / 2.f, 0.f));

	_tex0->Bind();
		
	Render::DrawRect(rect, uv);
}

void TestWidget::Draw() 
{
	TestWidget::DrawBackGround();

	//
	// Этот вызов отключает текстурирование при отрисовке.
	//
	Render::device.SetTexturing(false);
	
	//
	// Метод BeginColor() проталкивает в стек текущий цвет вершин и устанавливает новый.
	//
	Render::BeginColor(Color(0, 0, 0, 255)); // green = Color(0, 128, 0, 255)
	
	//
	// Метод DrawRect() выводит в графическое устройство квадратный спрайт, состоящий их двух
	// примитивов - треугольников, используя при этом текущий цвет для вершин и привязанную (binded) текстуру,
	// если разрешено текстурирование.
	//
	// Перед вызовом DrawRect() должен быть вызов Texture::Bind(), либо SetTexturing(false),
	// иначе визуальный результат будет непредсказуемым.
	//
	Render::DrawRect(924, 0, 100, 80);
	
	//
	// Метод EndColor() снимает со стека текущий цвет вершин, восстанавливая прежний.
	//
	Render::EndColor();
	
	//
	// Опять включаем текстурирование.
	//
	Render::device.SetTexturing(true);

	//выводим текст
	Render::BindFont("arial");
	Render::PrintString(924 + 100 / 2, 60, "Level = " + utils::lexical_cast(_level), 1.f, CenterAlign);
	Render::PrintString(924 + 100 / 2, 30, "Parts = " + utils::lexical_cast(_parts_count), 1.f, CenterAlign);

	//
	// Получаем текущее положение курсора мыши.
	//
	IPoint mouse_pos = Core::mainInput.GetMousePos();

	//
	// Проталкиваем в стек текущее преобразование координат, чтобы в дальнейшем
	// можно было восстановить это преобразование вызовом PopMatrix.
	//
	Render::device.PushMatrix();
	
	//
	// Изменяем текущее преобразование координат, перемещая центр координат в позицию мыши
	// и поворачивая координаты относительно этого центра вокруг оси z на угол _angle.
	//
	Render::device.MatrixTranslate(math::Vector3((float)mouse_pos.x, (float)mouse_pos.y, 0));
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), _angle);

	if (!_curTex)
	{
		//
		// Метод Texture::Draw() выводит квадратный спрайт с размерами текстуры
		// в центре координат (0, 0). Центр координат и преобразование координат вершин
		// спрайта устанавливаются с применением текущего преобразования.
		//
		// При вызове метода Texture::Draw() вызывать Texture::Bind() необязательно.
		//
		_tex1->Draw();
	}
	else
	{
		IRect texRect = _tex2->getBitmapRect();
		//
		// При отрисовке текстуры можно вручную задавать UV координаты той части текстуры,
		// которая будет натянута на вершины спрайта. UV координаты должны быть нормализованы,
		// т.е., вне зависимости от размера текстуры в текселях, размер любой текстуры
		// равен 1 (UV координаты задаются в диапазоне 0..1, хотя ничто не мешает задать их
		// больше единицы, при этом в случае установленной адресации текстуры REPEAT, текстура
		// будет размножена по этой стороне соответствующее количество раз).
		//

		FRect rect(texRect), uv(0, 1, 0, 1);
		_tex2->TranslateUV(rect, uv);

		Render::device.MatrixScale(_scale);
		Render::device.MatrixTranslate(math::Vector3(-texRect.width / 2.f, -texRect.height / 2.f, 0.f));

		//
		// Привязываем текстуру.
		//
		_tex2->Bind();
		
		//
		// Метод DrawRect() выводит в графическое устройство квадратный спрайт, состоящий их двух
		// примитивов - треугольников, используя при этом текущий цвет для вершин и привязанную (binded) текстуру,
		// если разрешено текстурирование.
		//
		// Перед вызовом DrawRect() должен быть вызов Texture::Bind(), либо SetTexturing(false),
		// иначе визуальный результат будет непредсказуемым.
		//
		Render::DrawRect(rect, uv);
	}

	//
	// Воостанавливаем прежнее преобразование координат, снимая со стека изменённый фрейм.
	//
	Render::device.PopMatrix();

	//
	// Рисуем все эффекты, которые добавили в контейнер (Update() для контейнера вызывать не нужно).
	//
	_effCont.Draw();
}

void TestWidget::Update(float dt)
{
	//
	// Обновим контейнер с эффектами
	//
	_effCont.Update(dt);

	//
	// dt - значение времени в секундах, прошедшее от предыдущего кадра.
	// Оно может принимать разные значения, в зависимости от производительности системы
	// и сложности сцены.
	//
	// Для того, чтобы наша анимация зависела только от времени, и не зависела от
	// производительности системы, мы должны рассчитывать её от этого значения.
	//
	// Увеличиваем наш таймер с удвоенной скоростью.
	//
	_timer += dt * 2;
	
	//
	// Зацикливаем таймер в диапазоне (0, 2п).
	// Это нужно делать для предотвращения получения некорректных значений,
	// если вдруг переполнится разрядная сетка (float переполнился) или задержка
	// от предыдущего кадра была слишкой большой (система тормози-и-ит).
	//
	// Диапазон значений выбран равным (0, 2п), потому что мы используем это значение
	// для расчёта синуса, и большие значения будут просто периодически повторять результат.
	//
	while (_timer > 2 * math::PI)
	{
		_timer -= 2 * math::PI;
	}
	
	//
	// Анимирование параметра масштабирования в зависимости от таймера.
	//
	_scale = 0.8f + 0.25f * sinf(_timer);

	
	//Drawing Rockets!(and update)
	for(int i = 0; i < _rockets.size(); i++)
	{

		if(!_rockets[i]->lifetime  || !_rockets[i]->dead)
		{			
			if(_rockets[i]->lifetime > 0) _rockets[i]->dir.y -= 0.006; // здесь добавляем немного падения для НЕпервых ракет

			_rockets[i]->start += (_rockets[i]->dir * _rockets[i]->speed);
			_rockets[i]->lifetime -= 1;
			FPoint temp = _rockets[i]->end - _rockets[i]->start;
	
			// перемещаем эффект в след за ракетой
			//
			_rockets[i]->eff->posX = _rockets[i]->start.x;
			_rockets[i]->eff->posY = _rockets[i]->start.y;

			//можно сделать чтобы рокеты не могли вылететь за пределы экрана, но мне лично и так нравится
			/*if(_rockets[i]->start.x > 1020 || _rockets[i]->start.x < 20 || _rockets[i]->start.y > 748 || _rockets[i]->start.y < 20)
			{
				_rockets[i]->lifetime = 0;
				continue;
			}*/ 

			int distance = sqrt(temp.x * temp.x + temp.y * temp.y);
			if( distance <_blowdistance || !_rockets[i]->lifetime)  
			{
				_rockets[i]->dead = true;

					// завершаем эффект.
					//
					_rockets[i]->eff->Finish();
					//_rockets[i]->eff = NULL;  //ракеты не удаляются и указатель должен быть

				//анимация взрыва
				ParticleEffect *eff = _effCont.AddEffect("FindCoin2");// FindItem2
				eff->posX = _rockets[i]->start.x;
				eff->posY = _rockets[i]->start.y;
				eff->SetScale(2.0);
				eff->Reset();
				//и разделение
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
		// При нажатии на правую кнопку мыши, создаём эффект шлейфа за мышкой.
		//
		int lt = -1; //у "перворождённой" ракеты время жизни не ограниченно
		rocket * ptr = LaunchRocket(mouse_pos,_start_launch_position,lt, _level);

		
		
		//
		// И изменяем угол наклона текстуры.
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
		// Изменяем значение с 0 на 1 и наоборот.
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
	// Виджету могут посылаться сообщения с параметрами.
	//
	// Например, при вводе символа с клавиатуры виджетам на активном слое посылается
	// сообщение KeyPress с кодом символа в параметре.
	//
	if (message.getPublisher() == "KeyPress") 
	{
		int code = utils::lexical_cast<int>(message.getData());
		if (code < 0)
		{
			//
			// Отрицательные значения посылаются для виртуальных кодов клавиш,
			// например, -VK_RETURN, -VK_ESCAPE и т.д.
			//
			return;
		}
		
		//
		// Положительные значения посылаются для символов алфавита.
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