#include "digital.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	digital w;
	w.show();
	return a.exec();
}
