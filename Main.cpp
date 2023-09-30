
#include "Precomp.h"
#include "MainWindow.h"
#include "UTF16.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	try
	{
		QApplication app(argc, argv);
		MainWindow mainwindow;
		mainwindow.showMaximized();
		return app.exec();
	}
	catch (const std::exception& e)
	{
		MessageBox(0, to_utf16(e.what()).c_str(), L"Unhandled Exception", MB_OK);
	}
}
