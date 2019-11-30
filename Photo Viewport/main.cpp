#include "PhotoViewport.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":/PhotoViewport/Icon/photo-viewport-icon.ico"));
	PhotoViewport w;
	w.show();
	return a.exec();
}
