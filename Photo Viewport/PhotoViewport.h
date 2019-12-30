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

#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PhotoViewport.h"
#include "Viewport.h"
#include <QGridLayout>

class PhotoViewport : public QMainWindow
{
	Q_OBJECT

public:
	PhotoViewport(QWidget *parent = Q_NULLPTR);

private:
	Ui::PhotoViewportClass ui;

	std::unique_ptr<QGridLayout> gridLayout = std::make_unique<QGridLayout>();
	std::unique_ptr<Viewport> viewport = std::make_unique<Viewport>(this);
};
