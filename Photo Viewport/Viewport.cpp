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

#include "Viewport.h"

Viewport::Viewport(QWidget* parent)
	: QGraphicsView(parent)
{
	// We account for if user is trying to open a file via the context menu / 
	// double-clicking, without this program open.
	// We do some basic checks, like making sure it's a supported image file, to avoid nonsense being loaded.
	// Currently, we are not checking for multiple instances of the program running, so
	// double-clicking images in this way over and over will create multiple instances of the program.
	// It's probably better to leave multiple instances programming alone, to allow the user to have
	// multiple instances with multiple different slideshows if they so desire.
	if (QApplication::arguments().size() > 1)
	{
		const QString filename = QApplication::arguments().at(1);
		if (!filename.isEmpty())
		{
			QString urlExtension = extensionOf(filename);
			if (urlExtension == ".jpg" ||
				urlExtension == ".png" ||
				urlExtension == ".bmp" ||
				urlExtension == ".gif")
			{
				if (QFile::exists(filename))
				{
					imgApply(QPixmap(filename));
					fileDirLastOpened = filename;
				}
			}
		}
	}

	// Viewport and dependents' attributes (scene, item) are set on init inside viewport
	// to minimize need for access from main window class.
	setDragMode(QGraphicsView::ScrollHandDrag);
	setAcceptDrops(true);
	this->setStyleSheet("QGraphicsView{border: 0px; background-color: #000000;}");
	this->setMinimumSize(QSize(this->width(), this->height() - 20));
	this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	this->setScene(graphicsScene.get());
	graphicsScene.get()->setParent(this->parent());
	graphicsScene.get()->addItem(pixmapItem.get());
	actionFileOpen.get()->setObjectName("actionFileOpen");
	actionFileOpen.get()->setText("Open Image");
	actionPasteFromClipboard.get()->setObjectName("actionPasteFromClipboard");
	actionPasteFromClipboard.get()->setText("Paste Image");
	actionImageSave.get()->setObjectName("actionImageSave");
	actionImageSave.get()->setText("Save Current Image");
	actionToggleAdjustToLastZoomLevel.get()->setObjectName("actionToggleAdjustToLastZoomLevel");
	actionToggleAdjustToLastZoomLevel.get()->setText("Maintain Zoom Level on Slide");
	actionToggleAdjustToLastZoomLevel.get()->setCheckable(true);
	actionToggleAdjustToLastZoomLevel.get()->setChecked(true);
	contextMenu.get()->addAction(actionFileOpen.get());
	contextMenu.get()->addAction(actionPasteFromClipboard.get());
	contextMenu.get()->addAction(actionImageSave.get());
	contextMenu.get()->addSeparator();
	contextMenu.get()->addAction(actionToggleAdjustToLastZoomLevel.get());

	// We allow the user to move left or right in the list of loaded images (stored as QPixmap variables and
	// applied, in turn, to a single QGraphicsPixmapItem, swapping out its current QPixmap as we move),
	// thereby creating a slideshow effect.
	// This location in the list is pushed to the end after a new image is loaded, 
	// so that the order of images stays the same and display focus is placed on the new image when it's loaded.
	connect(shortcutSlideLeft.get(), &QShortcut::activated, this, &Viewport::slideLeft);
	connect(shortcutSlideRight.get(), &QShortcut::activated, this, &Viewport::slideRight);
	connect(shortcutSlideLeft_Alt.get(), &QShortcut::activated, this, &Viewport::slideLeft);
	connect(shortcutSlideRight_Alt.get(), &QShortcut::activated, this, &Viewport::slideRight);

	connect(shortcutZoomIn.get(), &QShortcut::activated, this, [=]() {
		zoomIn();
		emit userIncreasedZoomLevel();
	});
	connect(shortcutZoomOut.get(), &QShortcut::activated, this, [=]() {
		zoomOut();
		emit userDecreasedZoomLevel();
	});
	connect(shortcutZoomReset.get(), &QShortcut::activated, this, &Viewport::zoomReset);

	connect(shortcutZoomIn_Alt.get(), &QShortcut::activated, this, [=]() {
		zoomIn();
		emit userIncreasedZoomLevel();
	});
	connect(shortcutZoomOut_Alt.get(), &QShortcut::activated, this, [=]() {
		zoomOut();
		emit userDecreasedZoomLevel();
	});
	connect(shortcutZoomReset_Alt.get(), &QShortcut::activated, this, &Viewport::zoomReset);

	connect(this, &Viewport::userIncreasedZoomLevel, this, [=]() { lastZoomLevel++; });
	connect(this, &Viewport::userDecreasedZoomLevel, this, [=]() { lastZoomLevel--; });

	connect(&netManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(imgApplyFromNetwork(QNetworkReply*)));

	connect(actionFileOpen.get(), &QAction::triggered, this, &Viewport::imgOpenFromFile);
	connect(actionPasteFromClipboard.get(), &QAction::triggered, this, &Viewport::imgPasteFromClipboard);
	connect(actionImageSave.get(), &QAction::triggered, this, &Viewport::imgSaveCurrent);
}

// public

QGraphicsScene* Viewport::scene()
{
	return graphicsScene.get();
}

QGraphicsPixmapItem* Viewport::item()
{
	return pixmapItem.get();
}


// protected

void Viewport::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls())
		event->acceptProposedAction();
}

void Viewport::dragMoveEvent(QDragMoveEvent *event)
{
	event->acceptProposedAction();
}

void Viewport::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasImage())
	{
		// Presumably there are situations where the user could be dragging such that
		// drag picks up image data, not just url location, so we account for that here.
		imgApply(QPixmap::fromImage(qvariant_cast<QImage>(event->mimeData()->imageData())));
	}
	else if (event->mimeData()->hasUrls())
	{
		for (auto& mimeUrl : event->mimeData()->urls())
		{
			QString urlExtension = extensionOf(mimeUrl.toString());
			if (urlExtension == ".jpg" ||
				urlExtension == ".png" ||
				urlExtension == ".bmp" ||
				urlExtension == ".gif")
			{
				if (QFile::exists(mimeUrl.toLocalFile()))
				{
					// Assumption here is that user is dragging from local file "url".
					imgApply(QPixmap(mimeUrl.toLocalFile()));
				}
				else
				{
					// Otherwise, we assume url is from the web and load it with network.
					QNetworkRequest request(mimeUrl.toString());
					netManager.get(request);
				}
			}
			else
			{
				QMessageBox::information(this->parentWidget(), tr("Url Image Not Loaded"), tr("Url must end in supported image file extension (.jpg, .png, .gif, .bmp)\n\nIf you are dragging from the web, make sure you're dragging from an image link with a supported extension. In some cases, dragging from the web will give you an html link instead."));
			}
		}
	}
}

void Viewport::contextMenuEvent(QContextMenuEvent *event)
{
	contextMenu.get()->exec(event->globalPos());
}


// private

void Viewport::adjustToLastZoomLevel(const int &zoomLevel)
{
	if (zoomLevel > 0)
		for (int i = 0; i < zoomLevel; i++)
			zoomIn();
	else if (zoomLevel < 0)
		for (int i = 0; i < abs(zoomLevel); i++)
			zoomOut();
}

void Viewport::slideLeft()
{
	if (pixmapListIndexCurrent - 1 >= 0)
	{
		pixmapItem.get()->setPixmap(pixmapList[pixmapListIndexCurrent - 1]);
		graphicsScene.get()->setSceneRect(pixmapItem.get()->boundingRect());
		pixmapListIndexCurrent--;

		if (actionToggleAdjustToLastZoomLevel.get()->isChecked())
			adjustToLastZoomLevel(lastZoomLevel);
	}
}

void Viewport::slideRight()
{
	if (pixmapListIndexCurrent + 1 <= pixmapList.size() - 1)
	{
		pixmapItem.get()->setPixmap(pixmapList[pixmapListIndexCurrent + 1]);
		graphicsScene.get()->setSceneRect(pixmapItem.get()->boundingRect());
		pixmapListIndexCurrent++;

		if (actionToggleAdjustToLastZoomLevel.get()->isChecked())
			adjustToLastZoomLevel(lastZoomLevel);
	}
}

void Viewport::zoomAdjustScrollPos(const double factorZoom)
{
	auto adjustScroll = [=](QScrollBar *scrollBar) {
		scrollBar->setValue(int(
			factorZoom * scrollBar->value() +
			((factorZoom - 1) * scrollBar->pageStep() / 2)
		));
	};

	adjustScroll(this->horizontalScrollBar());
	adjustScroll(this->verticalScrollBar());
}

void Viewport::zoomIn()
{
	QPixmap temp = pixmapList[pixmapListIndexCurrent].scaled(pixmapItem.get()->pixmap().size() * factorZoomIn, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	pixmapItem.get()->setPixmap(temp);
	graphicsScene.get()->setSceneRect(pixmapItem.get()->boundingRect());
	zoomAdjustScrollPos(factorZoomIn);
}

void Viewport::zoomOut()
{
	QPixmap temp = pixmapList[pixmapListIndexCurrent].scaled(pixmapItem.get()->pixmap().size() * factorZoomOut, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	pixmapItem.get()->setPixmap(temp);
	graphicsScene.get()->setSceneRect(pixmapItem.get()->boundingRect());
	zoomAdjustScrollPos(factorZoomOut);
}

void Viewport::zoomReset()
{
	pixmapItem.get()->setPixmap(pixmapList[pixmapListIndexCurrent]);
	graphicsScene.get()->setSceneRect(pixmapItem.get()->boundingRect());
	lastZoomLevel = 0;
}

QString Viewport::extensionOf(const QString str)
{
	// We look for a period to indicate the file has an extension.
	// Then return the extension if it does.
	// In theory, the extension should never return as something bonkers since we're looking for the last period.
	// (And the last period should always be for an extension if the "url" has a period, or periods.)
	if (str.lastIndexOf(".") == -1)
		return QString();
	else
		return QString(str.right(str.size() - str.lastIndexOf(".")));
}

void Viewport::imgApply(const QPixmap &pixmap)
{
	// We use a generic function for adding an image as pixmap to the scene and adding it to images list.
	// This should be the only place for general operations for adding to the scene.
	// (e.g. the exception is operations that only apply to a specific image loading scenario,
	// such as in reading data from network request.)
	pixmapItem.get()->setPixmap(pixmap);
	graphicsScene.get()->setSceneRect(pixmapItem.get()->boundingRect());
	pixmapList.push_back(pixmap);
	pixmapListIndexCurrent = pixmapList.size() - 1;
}


// private slots

void Viewport::imgApplyFromNetwork(QNetworkReply *netReply)
{
	QByteArray imgData = netReply->readAll();
	netReply->deleteLater();
	QPixmap newImg;
	newImg.loadFromData(imgData);
	imgApply(newImg);
}

void Viewport::imgOpenFromFile()
{
	QStringList filenameList = QFileDialog::getOpenFileNames(this, tr("Open"), fileDirLastOpened, tr("IMG Files (*.png *.gif *.jpg *.bmp)"));
	if (!filenameList.isEmpty())
	{
		for (auto& filename : filenameList)
		{
			imgApply(QPixmap(filename));
			fileDirLastOpened = filename;
		}
	}
}

void Viewport::imgPasteFromClipboard()
{
	// Clipboard support is especially useful in the case of web browsers, as dragging can be more difficult.
	// With clipboard, the user can copy/paste the image data, rather than being concerned with the correct url.
	if (QApplication::clipboard()->mimeData()->hasImage())
	{
		imgApply(QPixmap::fromImage(qvariant_cast<QImage>(QApplication::clipboard()->mimeData()->imageData())));
	}
	else if (QApplication::clipboard()->mimeData()->hasUrls())
	{
		for (auto& mimeUrl : QApplication::clipboard()->mimeData()->urls())
		{
			QString urlExtension = extensionOf(mimeUrl.toString());
			if (urlExtension == ".jpg" ||
				urlExtension == ".png" ||
				urlExtension == ".bmp" ||
				urlExtension == ".gif")
			{
				if (QFile::exists(mimeUrl.toLocalFile()))
				{
					// Assumption here is that user is copy/pasting from local file "url".
					imgApply(QPixmap(mimeUrl.toLocalFile()));
				}
				else
				{
					// Otherwise, we assume url is from the web and load it with network.
					QNetworkRequest request(mimeUrl.toString());
					netManager.get(request);
				}
			}
			else
			{
				QMessageBox::information(this->parentWidget(), tr("Url Image Not Copied"), tr("Url must end in supported image file extension (.jpg, .png, .gif, .bmp)\n\nIf you are dragging from the web, make sure you're dragging from an image link with a supported extension. In some cases, dragging from the web will give you an html link instead."));
			}
		}
	}
	else
		QMessageBox::information(this->parentWidget(), tr("Image Not Copied"), tr("No image found in the clipboard."));
}

void Viewport::imgSaveCurrent()
{
	// We allow the user to save the image they are currently on in the "slideshow."
	// There are no editing capabilities in the program, so this is purely for saving something loaded in, as is.
	QFileDialog dialog(this, tr("Save As"), fileDirLastSaved, tr("Image Files (*.png)"));
	dialog.setWindowModality(Qt::WindowModal);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	if (dialog.exec() == QFileDialog::Accepted)
	{
		QString selectedFile = dialog.selectedFiles().first();
		QFile fileWrite(selectedFile);
		fileWrite.open(QIODevice::WriteOnly);
		pixmapList[pixmapListIndexCurrent].save(&fileWrite, "PNG");
		fileDirLastSaved = selectedFile;
	}
}