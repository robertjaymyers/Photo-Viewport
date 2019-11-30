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
