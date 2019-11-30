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
	std::vector<QPixmap> pixmapList;
	int pixmapListIndexCurrent = 0;
	std::unique_ptr<QGraphicsScene> graphicsScene = std::make_unique<QGraphicsScene>();
	std::unique_ptr<QGraphicsPixmapItem> pixmapItem = std::make_unique<QGraphicsPixmapItem>();
	std::unique_ptr<QShortcut> shortcutSlideLeft = std::make_unique<QShortcut>(QKeySequence(tr("A", "Slide Left")), this);
	std::unique_ptr<QShortcut> shortcutSlideRight = std::make_unique<QShortcut>(QKeySequence(tr("D", "Slide Right")), this);
	std::unique_ptr<QShortcut> shortcutZoomIn = std::make_unique<QShortcut>(QKeySequence(tr("W", "Zoom In")), this);
	std::unique_ptr<QShortcut> shortcutZoomOut = std::make_unique<QShortcut>(QKeySequence(tr("S", "Zoom Out")), this);
	std::unique_ptr<QShortcut> shortcutZoomReset = std::make_unique<QShortcut>(QKeySequence(tr("R", "Zoom Reset")), this);
	const double factorZoomIn = 1.25;
	const double factorZoomOut = 1 / factorZoomIn;
	QNetworkAccessManager netManager;
	void slideLeft();
	void slideRight();
	void zoomIn();
	void zoomOut();
	void zoomReset();
	QString extensionOf(const QString str);
	void imgApply(const QPixmap &pixmap);

private slots:
	void imgApplyFromNetwork(QNetworkReply *netReply);
	void imgOpenFromFile();
	void imgPasteFromClipboard();
	void imgSaveCurrent();
};