#include "stdafx.h"
#include "TestApplication.h"

#define MYSETTINGS_REG_KEY "Software\\Playrix Entertainment\\Test"
#define MYWINDOW_CLASS_NAME "MYTEST_CLASS"
#define MYAPPLICATION_NAME "Test"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Core::fileSystem.SetWriteDirectory("./write_directory");
	Log::log.AddSink(new Log::HtmlFileLogSink("log.htm", true));
		
	IO::FileStream fs("settings.ini");
	IO::TextReader reader(&fs);

	std::string str, name, value;
	bool fullscreen = false;

	str = reader.ReadLine();
	if (utils::ReadNvp(str, name, value) && name == "path")
	{
		Core::fileSystem.MountDirectory(value);
	}

	str = reader.ReadLine();	
	if (utils::ReadNvp(str, name, value) && name == "fullscreen")
	{
		fullscreen = utils::lexical_cast<bool>(value);
	}

	TestApplication app(hInstance, nCmdShow, fullscreen);

	app.SETTINGS_REG_KEY = MYSETTINGS_REG_KEY;
	app.APPLICATION_NAME = MYAPPLICATION_NAME;
	app.WINDOW_CLASS_NAME = MYWINDOW_CLASS_NAME;

	app.Init(true);
	app.Start();
	app.ShutDown();
	
	return 0;
}
