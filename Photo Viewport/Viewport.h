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
#include <memory>
#include <vector>
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QShortcut>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QBuffer>
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>
#include <QClipboard>
#include <QScrollBar>

class Viewport : public QGraphicsView
{
	Q_OBJECT

public:
	Viewport(QWidget *parent = NULL);
	QGraphicsScene* scene();
	QGraphicsPixmapItem* item();

protected:
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dropEvent(QDropEvent *event) override;
	void contextMenuEvent(QContextMenuEvent *event) override;

private:
	QString fileDirLastOpened;
	QString fileDirLastSaved;
	std::unique_ptr<QMenu> contextMenu = std::make_unique<QMenu>();
	std::unique_ptr<QAction> actionFileOpen = std::make_unique<QAction>();
	std::unique_ptr<QAction> actionPasteFromClipboard = std::make_unique<QAction>();
	std::unique_ptr<QAction> actionImageSave = std::make_unique<QAction>();
	std::unique_ptr<QAction> actionToggleAdjustToLastZoomLevel = std::make_unique<QAction>();
	std::vector<QPixmap> pixmapList;
	int pixmapListIndexCurrent = 0;
	std::unique_ptr<QGraphicsScene> graphicsScene = std::make_unique<QGraphicsScene>();
	std::unique_ptr<QGraphicsPixmapItem> pixmapItem = std::make_unique<QGraphicsPixmapItem>();
	std::unique_ptr<QShortcut> shortcutSlideLeft = std::make_unique<QShortcut>(QKeySequence(tr("A", "Slide Left")), this);
	std::unique_ptr<QShortcut> shortcutSlideRight = std::make_unique<QShortcut>(QKeySequence(tr("D", "Slide Right")), this);
	std::unique_ptr<QShortcut> shortcutSlideLeft_Alt = std::make_unique<QShortcut>(QKeySequence(tr("Left", "Slide Left (Alt)")), this);
	std::unique_ptr<QShortcut> shortcutSlideRight_Alt = std::make_unique<QShortcut>(QKeySequence(tr("Right", "Slide Right (Alt)")), this);
	std::unique_ptr<QShortcut> shortcutZoomIn = std::make_unique<QShortcut>(QKeySequence(tr("W", "Zoom In")), this);
	std::unique_ptr<QShortcut> shortcutZoomOut = std::make_unique<QShortcut>(QKeySequence(tr("S", "Zoom Out")), this);
	std::unique_ptr<QShortcut> shortcutZoomReset = std::make_unique<QShortcut>(QKeySequence(tr("R", "Zoom Reset")), this);
	std::unique_ptr<QShortcut> shortcutZoomIn_Alt = std::make_unique<QShortcut>(QKeySequence(tr("Up", "Zoom In")), this);
	std::unique_ptr<QShortcut> shortcutZoomOut_Alt = std::make_unique<QShortcut>(QKeySequence(tr("Down", "Zoom Out")), this);
	std::unique_ptr<QShortcut> shortcutZoomReset_Alt = std::make_unique<QShortcut>(QKeySequence(Qt::Key_0), this);
	const double factorZoomIn = 1.25;
	const double factorZoomOut = 1 / factorZoomIn;
	int lastZoomLevel = 0;
	QNetworkAccessManager netManager;
	void slideLeft();
	void slideRight();
	void adjustToLastZoomLevel(const int &zoomLevel);
	void zoomAdjustScrollPos(const double factorZoom);
	void zoomIn();
	void zoomOut();
	void zoomReset();
	QString extensionOf(const QString str);
	void imgApply(const QPixmap &pixmap);

signals:
	void userIncreasedZoomLevel();
	void userDecreasedZoomLevel();

private slots:
	void imgApplyFromNetwork(QNetworkReply *netReply);
	void imgOpenFromFile();
	void imgPasteFromClipboard();
	void imgSaveCurrent();
};