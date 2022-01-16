#include "MLAssistGidTool.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":/MLAssistGidTool/Resources/logo.ico"));
	MLAssistGidTool w;
	w.show();
	return a.exec();
}
