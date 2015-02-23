#pragma once

#include "TestWidget.h"

///
/// Главный класс приложения, им определяется жизненный цикл игры.
///
class TestApplication : public Core::Application
{
public:
	TestApplication(HINSTANCE hInstance, int nCmdShow, bool fullscreen)
		: Application(hInstance, nCmdShow, fullscreen)
	{
		//
		// Здесь задаётся размер игрового экрана в логических единицах.
		//
		// Это значит, что и в оконном, и в полноэкранном режиме размер экрана
		// всегда будет равен 1024x768 "пикселей".
		//
		GAME_CONTENT_WIDTH = 1024;
		GAME_CONTENT_HEIGHT = 768;
	}
	
	///
	/// Здесь регистрируются пользовательские типы в Lua и виджеты.
	///
	void RegisterTypes() {
		Application::RegisterTypes();

		//
		// Чтобы можно было декларативно использовать виджет в xml по имени,
		// его необходимо зарегистрировать таким образом.
		//
		REGISTER_WIDGET_XML(TestWidget, "TestWidget");
	}
	
	///
	/// Метод LoadResources вызывается перед началом игрового цикла.
	///
	void LoadResources()
	{
		//
		// Обычно в этом методе выполняется скрипт, в котором определяется,
		// какие ресурсы нужно загрузить и какой игровой слой положить на экран.
		//
		Core::LuaExecuteStartupScript("start.lua");
	}

	///
	/// Метод DrawFps вызывается после всех других отрисовок, если установлено
	/// свойство application.showFps = true.
	///
	void DrawFps()
	{
		if (!Render::isFontLoaded("arial"))
		{
			return;
		}
		
		//
		// Перед вызовом метода Render::PrintString() нужно привязать нужный шрифт
		//
		Render::BindFont("arial");
		
		int dy = Render::getFontHeight();
		
		int x = Render::device.Width() - dy;
		int y = Render::device.Height() - 2 * dy;
		
		Render::PrintString(x, y, std::string("FPS: ") + utils::lexical_cast(currentFps), 1.0f, RightAlign);
		Render::PrintString(x, y - 1 * dy, std::string("Vm: ") + utils::lexical_cast(Render::device.GetVideoMemUsage()) + std::string("MB"), 1.0f, RightAlign);
		Render::PrintString(x, y - 2 * dy, std::string("Am: ") + utils::lexical_cast(Core::resourceManager.GetMemoryInUse<MM::AudioResource>() / 1024) + std::string("KB"), 1.0f, RightAlign);
	}
};