
#include "Precomp.h"
#include "MainWindow.h"
#include "VP3Viewer.h"
#include "VP3Painter.h"
#include "VP3Embroidery.h"
#include "File.h"
#include "UTF16.h"
#include <QSplitter>
#include <QListView>
#include <QStandardItemModel>
#include <QPainter>
#include <QLabel>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QToolBar>
#include <QMenuBar>
#include <Shobjidl.h>

MainWindow::MainWindow()
{
	setupUi();
	setWindowTitle("Embroidery Manager");
	setWindowIcon(QIcon(":/Resources/Icons/sewingmachine.png"));
	resize(1600, 700);

	for (const std::string& file : Directory::files(libraryFolder + "*.vp3"))
	{
		std::string filename = libraryFolder + file;

		try
		{
			auto embroidery = VP3Embroidery::load(filename);
			QPixmap icon(128, 256);
			QPainter painter(&icon);
			VP3Painter::paint(embroidery.get(), &painter, 128, 256);

			QStandardItem* item = new QStandardItem(QIcon(icon), QString::fromStdString(file));
			item->setData(QString::fromStdString(filename), Qt::UserRole + 1);
			pcItemModel->invisibleRootItem()->appendRow(item);
		}
		catch (...)
		{
		}
	}

	for (const std::string& file : Directory::files(usbFolder + "*.vp3"))
	{
		std::string filename = usbFolder + file;

		try
		{
			auto embroidery = VP3Embroidery::load(filename);
			QPixmap icon(128, 256);
			QPainter painter(&icon);
			VP3Painter::paint(embroidery.get(), &painter, 128, 256);

			QStandardItem* item = new QStandardItem(QIcon(icon), QString::fromStdString(file));
			item->setData(QString::fromStdString(filename), Qt::UserRole + 1);
			usbItemModel->invisibleRootItem()->appendRow(item);
		}
		catch (...)
		{
		}
	}
}

void MainWindow::setupUi()
{
	pcItemModel = new QStandardItemModel(this);
	usbItemModel = new QStandardItemModel(this);

	auto pcWidget = new QWidget();

	auto pcLabel = new QLabel(pcWidget);
	pcLabel->setText("Designs on PC:");

	pcListView = new QListView(pcWidget);
	pcListView->setModel(pcItemModel);
	pcListView->setViewMode(QListView::IconMode);
	pcListView->setIconSize(QSize(128, 256));
	pcListView->setResizeMode(QListView::Adjust);

	auto pcLayout = new QVBoxLayout(pcWidget);
	pcLayout->setContentsMargins(7, 7, 0, 0);
	pcLayout->addWidget(pcLabel);
	pcLayout->addWidget(pcListView);

	auto usbWidget = new QWidget();

	auto usbLabel = new QLabel(usbWidget);
	usbLabel->setText("Designs on USB-Stick:");

	usbListView = new QListView(usbWidget);
	usbListView->setModel(usbItemModel);
	usbListView->setViewMode(QListView::IconMode);
	usbListView->setIconSize(QSize(128, 256));
	usbListView->setResizeMode(QListView::Adjust);

	auto usbLayout = new QVBoxLayout(usbWidget);
	usbLayout->setContentsMargins(5, 5, 0, 0);
	usbLayout->addWidget(usbLabel);
	usbLayout->addWidget(usbListView);

	viewer = new VP3Viewer(this);

	vertSplitter = new QSplitter(Qt::Vertical);
	vertSplitter->addWidget(pcWidget);
	vertSplitter->addWidget(usbWidget);
	vertSplitter->setStretchFactor(0, 3);
	vertSplitter->setStretchFactor(1, 2);

	horzSplitter = new QSplitter(Qt::Horizontal);
	horzSplitter->addWidget(vertSplitter);
	horzSplitter->addWidget(viewer);
	horzSplitter->setStretchFactor(0, 3);
	horzSplitter->setStretchFactor(1, 1);

	auto toolbar = addToolBar("Toolbar");
	toolbar->setMovable(false);
	toolbar->setIconSize(QSize(32, 32));
	toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	actionCopyToUSB = toolbar->addAction(QIcon(":/Resources/Icons/usbkey.png"), "Copy to USB-Stick");
	toolbar->addSeparator();
	actionCopyToPC = toolbar->addAction(QIcon(":/Resources/Icons/laptop.png"), "Copy to PC");
	toolbar->addSeparator();
	actionDelete = toolbar->addAction(QIcon(":/Resources/Icons/delete.png"), "Delete");
	toolbar->addSeparator();
	actionImport = toolbar->addAction(QIcon(":/Resources/Icons/import.png"), "Import");
	toolbar->addSeparator();
	//actionOptions = toolbar->addAction(QIcon(":/Resources/Icons/options.png"), "Options");

	statusBar()->showMessage("Ready");
	statusBar()->setFixedHeight(28);
	setCentralWidget(horzSplitter);

	connect(pcListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(pcListViewSelectionChanged(QItemSelection)));
	connect(usbListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(usbListViewSelectionChanged(QItemSelection)));

	connect(actionCopyToUSB, SIGNAL(triggered()), this, SLOT(actionCopyToUSBTriggered()));
	connect(actionCopyToPC, SIGNAL(triggered()), this, SLOT(actionCopyToPCTriggered()));
	connect(actionDelete, SIGNAL(triggered()), this, SLOT(actionDeleteTriggered()));
	connect(actionImport, SIGNAL(triggered()), this, SLOT(actionImportTriggered()));
	//connect(actionOptions, SIGNAL(triggered()), this, SLOT(actionOptionsTriggered()));
}

void MainWindow::actionCopyToUSBTriggered()
{
	copySelection(pcListView, usbItemModel, usbFolder);
}

void MainWindow::actionCopyToPCTriggered()
{
	copySelection(usbListView, pcItemModel, libraryFolder);
}

void MainWindow::copySelection(QListView* srcListView, QStandardItemModel* destModel, std::string destFolder)
{
	QModelIndexList selection = srcListView->selectionModel()->selectedRows();
	for (QModelIndex& index : selection)
	{
		std::string filename = index.data(Qt::UserRole + 1).toString().toStdString();
		size_t lastSlash = filename.find_last_of("/\\");
		std::string name = (lastSlash == std::string::npos) ? filename : filename.substr(lastSlash + 1);
		std::string newfilename = destFolder + name;

		BOOL result = CopyFile(to_utf16(filename).c_str(), to_utf16(newfilename).c_str(), FALSE);
		if (result == TRUE)
		{
			auto embroidery = VP3Embroidery::load(filename);
			QPixmap icon(128, 256);
			QPainter painter(&icon);
			VP3Painter::paint(embroidery.get(), &painter, 128, 256);

			QStandardItem* item = new QStandardItem(QIcon(icon), QString::fromStdString(name));
			item->setData(QString::fromStdString(newfilename), Qt::UserRole + 1);
			destModel->invisibleRootItem()->appendRow(item);
		}
	}
}

void MainWindow::actionDeleteTriggered()
{
	QModelIndexList selection = usbListView->selectionModel()->selectedRows();
	std::sort(selection.begin(), selection.end(), [](const auto& a, const auto& b) { return a.row() > b.row(); });
	for (QModelIndex& index : selection)
	{
		std::string filename = index.data(Qt::UserRole + 1).toString().toStdString();
		DeleteFile(to_utf16(filename).c_str());
		usbItemModel->removeRow(index.row());
	}
}

void MainWindow::actionImportTriggered()
{
	std::string filename = showOpenFileDialog();
	if (filename.empty())
		return;

	size_t lastSlash = filename.find_last_of("/\\");
	std::string name = (lastSlash == std::string::npos) ? filename : filename.substr(lastSlash + 1);
	std::string newfilename = libraryFolder + name;

	BOOL result = CopyFile(to_utf16(filename).c_str(), to_utf16(newfilename).c_str(), FALSE);
	if (result == TRUE)
	{
		auto embroidery = VP3Embroidery::load(filename);
		QPixmap icon(128, 256);
		QPainter painter(&icon);
		VP3Painter::paint(embroidery.get(), &painter, 128, 256);

		QStandardItem* item = new QStandardItem(QIcon(icon), QString::fromStdString(name));
		item->setData(QString::fromStdString(newfilename), Qt::UserRole + 1);
		pcItemModel->invisibleRootItem()->appendRow(item);
	}
}

std::string MainWindow::showOpenFileDialog()
{
	std::string resultFilename;

	IFileDialog* filedialog = nullptr;
	HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&filedialog));
	if (FAILED(result))
		return resultFilename;

	COMDLG_FILTERSPEC saveTypes[] =
	{
		{ L"VP3 Embroidery Files", L"*.vp3" },
		{ L"All Files", L"*.*" }
	};

	DWORD dwFlags;
	result = filedialog->GetOptions(&dwFlags);
	if (SUCCEEDED(result))
	{
		result = filedialog->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
		if (SUCCEEDED(result))
		{
			result = filedialog->SetFileTypes(ARRAYSIZE(saveTypes), saveTypes);
			if (SUCCEEDED(result))
			{
				result = filedialog->SetFileTypeIndex(0);
				if (SUCCEEDED(result))
				{
					result = filedialog->SetDefaultExtension(L"vp3");
					if (SUCCEEDED(result))
					{
						result = filedialog->Show(nullptr);
						if (SUCCEEDED(result))
						{
							IShellItem* psiResult;
							result = filedialog->GetResult(&psiResult);
							if (SUCCEEDED(result))
							{
								PWSTR pszFilePath = nullptr;
								result = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
								if (SUCCEEDED(result))
								{
									resultFilename = from_utf16(pszFilePath);
								}
								psiResult->Release();
							}
						}
					}
				}
			}
		}
	}

	filedialog->Release();
	return resultFilename;
}

void MainWindow::actionOptionsTriggered()
{
}

void MainWindow::pcListViewSelectionChanged(QItemSelection)
{
	QModelIndexList selection = pcListView->selectionModel()->selectedRows();
	if (selection.count() > 0)
	{
		QModelIndex index = selection.at(0);
		std::string filename = index.data(Qt::UserRole + 1).toString().toStdString();
		try
		{
			viewer->setEmbroidery(VP3Embroidery::load(filename));
		}
		catch (...)
		{
		}
	}
}

void MainWindow::usbListViewSelectionChanged(QItemSelection)
{
	QModelIndexList selection = usbListView->selectionModel()->selectedRows();
	if (selection.count() > 0)
	{
		QModelIndex index = selection.at(0);
		std::string filename = index.data(Qt::UserRole + 1).toString().toStdString();
		try
		{
			viewer->setEmbroidery(VP3Embroidery::load(filename));
		}
		catch (...)
		{
		}
	}
}
