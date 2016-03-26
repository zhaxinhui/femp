#include <sigc++/sigc++.h> 	// to side step a compiler error caused by a conflict with Qt and libsigc++
#include <QApplication>

#include "MainWindow.h++"
#include "ProgramOptions.h++"

#include <string>
#include <iostream>
#include <fstream>

#include "parsers/MshParser.h++"
#include <libfemp/Model.h++>



Q_DECLARE_METATYPE(size_t);
Q_DECLARE_METATYPE(std::string);


int main(int argc, char *argv[])
{
	qRegisterMetaType<size_t>("size_t");
	qRegisterMetaType<std::string>("std::string");

	QApplication app(argc, argv);

	// register types

	MainWindow mainWin;
	mainWin.show();

	return app.exec();
}

