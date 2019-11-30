#include "PhotoViewport.h"

PhotoViewport::PhotoViewport(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// Set up layout and add viewport to it, then display viewport.
	// Layout ensures that the viewport will expand to take up the whole app space,
	// regardless of image size.
	gridLayout.get()->setMargin(0);
	ui.centralWidget->setLayout(gridLayout.get());
	gridLayout.get()->addWidget(viewport.get());
	viewport.get()->show();
}
