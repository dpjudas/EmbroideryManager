#pragma once

#include <QMainWindow>
#include <QItemSelection>

class VP3Viewer;
class QSplitter;
class QListView;
class QStandardItemModel;

class MainWindow : public QMainWindow
{
	Q_OBJECT;
public:
	MainWindow();

	std::string libraryFolder = "C:\\EmbroideryLibrary\\";
	std::string usbFolder = "D:\\";

private slots:
	void pcListViewSelectionChanged(QItemSelection);
	void usbListViewSelectionChanged(QItemSelection);
	void actionCopyToUSBTriggered();
	void actionCopyToPCTriggered();
	void actionDeleteTriggered();
	void actionImportTriggered();
	void actionOptionsTriggered();

private:
	void setupUi();
	void copySelection(QListView* srcListView, QStandardItemModel* destModel, std::string destFolder);
	std::string showOpenFileDialog();

	QSplitter* horzSplitter = nullptr;
	QSplitter* vertSplitter = nullptr;
	QListView* pcListView = nullptr;
	QListView* usbListView = nullptr;
	VP3Viewer* viewer = nullptr;
	QStandardItemModel* pcItemModel = nullptr;
	QStandardItemModel* usbItemModel = nullptr;

	QAction* actionCopyToUSB = nullptr;
	QAction* actionCopyToPC = nullptr;
	QAction* actionDelete = nullptr;
	QAction* actionImport = nullptr;
	QAction* actionOptions = nullptr;
};
