/*
This file is part of Photo Viewport.
	Photo Viewport is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	Photo Viewport is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with Photo Viewport.  If not, see <https://www.gnu.org/licenses/>.
*/

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
