#include "FractoriumPch.h"
#include "Fractorium.h"
#include "QssDialog.h"

// X11 headers on Linux define this, causing build errors.
#ifdef KeyRelease
	#undef KeyRelease
#endif

/// <summary>
/// Constructor that initializes the entire program.
/// The setup process is very lengthy because it requires many custom modifications
/// to the GUI widgets that are not possible to do through the designer. So if something
/// is present here, it's safe to assume it can't be done in the designer.
/// </summary>
/// <param name="p">The parent widget of this item</param>
Fractorium::Fractorium(QWidget* p)
	: QMainWindow(p)
{
	int iconSize_ = 9;
	size_t i = 0;
	string s;
	Timing t;
	ui.setupUi(this);
	m_Info = OpenCLInfo::Instance();
	qRegisterMetaType<size_t>("size_t");
	qRegisterMetaType<QVector<int>>("QVector<int>");//For previews.
	qRegisterMetaType<vector<byte>>("vector<byte>");
	qRegisterMetaType<vv4F>("vv4F");
	qRegisterMetaType<EmberTreeWidgetItemBase*>("EmberTreeWidgetItemBase*");
	tabifyDockWidget(ui.LibraryDockWidget, ui.FlameDockWidget);
	tabifyDockWidget(ui.FlameDockWidget, ui.XformsDockWidget);
	tabifyDockWidget(ui.XformsDockWidget, ui.XaosDockWidget);
	tabifyDockWidget(ui.XaosDockWidget, ui.PaletteDockWidget);
	tabifyDockWidget(ui.PaletteDockWidget, ui.InfoDockWidget);
	setTabPosition(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea, QTabWidget::TabPosition::North);
	setTabShape(QTabWidget::TabShape::Triangular);
	m_Docks.reserve(8);
	m_Docks.push_back(ui.LibraryDockWidget);
	m_Docks.push_back(ui.FlameDockWidget);
	m_Docks.push_back(ui.XformsDockWidget);
	m_Docks.push_back(ui.XaosDockWidget);
	m_Docks.push_back(ui.PaletteDockWidget);
	m_Docks.push_back(ui.InfoDockWidget);

	for (auto dock : m_Docks)//Prevents a dock from ever getting accidentally hidden.
	{
		dock->setWindowFlags(dock->windowFlags() & Qt::WindowStaysOnTopHint);
		dock->setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea
//#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
							  | Qt::DockWidgetArea::RightDockWidgetArea
//#endif
							 );
	}

	m_Urls << QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first())
		   << QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).first())
		   << QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first())
		   << QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first())
		   << QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first())
		   ;
	m_FontSize = 9;
	m_VarSortMode = 1;//Sort by weight by default.
	m_PaletteSortMode = 0;//Sort by palette ascending by default.
	m_ColorDialog = new QColorDialog(this);
	m_Settings = FractoriumSettings::Instance();
	m_QssDialog = new QssDialog(this);
	m_FinalRenderDialog = new FractoriumFinalRenderDialog(this);
	m_OptionsDialog = new FractoriumOptionsDialog(this);
	m_VarDialog = new FractoriumVariationsDialog(this);
	m_AboutDialog = new FractoriumAboutDialog(this);
	//Put the about dialog in the screen center.
	const QRect screen = QApplication::desktop()->screenGeometry();
	m_AboutDialog->move(screen.center() - m_AboutDialog->rect().center());
	connect(m_ColorDialog, SIGNAL(colorSelected(const QColor&)), this, SLOT(OnColorSelected(const QColor&)), Qt::QueuedConnection);
	m_XformComboColors[i++] = QColor(0XFF, 0X00, 0X00);
	m_XformComboColors[i++] = QColor(0XCC, 0XCC, 0X00);
	m_XformComboColors[i++] = QColor(0X00, 0XCC, 0X00);
	m_XformComboColors[i++] = QColor(0X00, 0XCC, 0XCC);
	m_XformComboColors[i++] = QColor(0X40, 0X40, 0XFF);
	m_XformComboColors[i++] = QColor(0XCC, 0X00, 0XCC);
	m_XformComboColors[i++] = QColor(0XCC, 0X80, 0X00);
	m_XformComboColors[i++] = QColor(0X80, 0X00, 0X4F);
	m_XformComboColors[i++] = QColor(0X80, 0X80, 0X22);
	m_XformComboColors[i++] = QColor(0X60, 0X80, 0X60);
	m_XformComboColors[i++] = QColor(0X50, 0X80, 0X80);
	m_XformComboColors[i++] = QColor(0X4F, 0X4F, 0X80);
	m_XformComboColors[i++] = QColor(0X80, 0X50, 0X80);
	m_XformComboColors[i++] = QColor(0X80, 0X60, 0X22);
	m_FinalXformComboColor  = QColor(0x7F, 0x7F, 0x7F);

	for (i = 0; i < XFORM_COLOR_COUNT; i++)
	{
		QPixmap pixmap(iconSize_, iconSize_);
		pixmap.fill(m_XformComboColors[i]);
		m_XformComboIcons[i] = QIcon(pixmap);
	}

	QPixmap pixmap(iconSize_, iconSize_);
	pixmap.fill(m_FinalXformComboColor);
	m_FinalXformComboIcon = QIcon(pixmap);
	InitToolbarUI();
	InitParamsUI();
	InitXformsUI();
	InitXformsColorUI();
	InitXformsAffineUI();
	InitXformsVariationsUI();
	InitXformsSelectUI();
	InitXaosUI();
	InitPaletteUI();
	InitLibraryUI();
	InitInfoUI();
	InitMenusUI();
	//This will init the controller and fill in the variations and palette tables with template specific instances
	//of their respective objects.
#ifdef DO_DOUBLE

	if (m_Settings->Double())
		m_Controller = unique_ptr<FractoriumEmberControllerBase>(new FractoriumEmberController<double>(this));
	else
#endif
		m_Controller = unique_ptr<FractoriumEmberControllerBase>(new FractoriumEmberController<float>(this));

	m_Controller->SetupVariationsTree();
	m_Controller->FilteredVariations();

	if (m_Info->Ok() && m_Settings->OpenCL() && m_QualitySpin->value() < (m_Settings->OpenClQuality() * m_Settings->Devices().size()))
		m_QualitySpin->setValue(m_Settings->OpenClQuality() * m_Settings->Devices().size());

	int statusBarHeight = 20;// *devicePixelRatio();
	ui.StatusBar->setMinimumHeight(statusBarHeight);
	ui.StatusBar->setMaximumHeight(statusBarHeight);
	m_RenderStatusLabel = new QLabel(this);
	m_RenderStatusLabel->setMinimumWidth(200);
	m_RenderStatusLabel->setAlignment(Qt::AlignRight);
	ui.StatusBar->addPermanentWidget(m_RenderStatusLabel);
	m_CoordinateStatusLabel = new QLabel(this);
	m_CoordinateStatusLabel->setMinimumWidth(300);
	m_CoordinateStatusLabel->setMaximumWidth(300);
	m_CoordinateStatusLabel->setAlignment(Qt::AlignLeft);
	ui.StatusBar->addWidget(m_CoordinateStatusLabel);
	int progressBarHeight = 15;
	int progressBarWidth = 300;
	m_ProgressBar = new QProgressBar(this);
	m_ProgressBar->setRange(0, 100);
	m_ProgressBar->setValue(0);
	m_ProgressBar->setMinimumHeight(progressBarHeight);
	m_ProgressBar->setMaximumHeight(progressBarHeight);
	m_ProgressBar->setMinimumWidth(progressBarWidth);
	m_ProgressBar->setMaximumWidth(progressBarWidth);
	ui.StatusBar->addPermanentWidget(m_ProgressBar);
	//Setup pointer in the GL window to point back to here.
	ui.GLDisplay->SetMainWindow(this);
	bool restored = restoreState(m_Settings->value("windowState").toByteArray());
	showMaximized();//This won't fully set things up and show them until after this constructor exits.
	connect(ui.LibraryDockWidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(dockLocationChanged(Qt::DockWidgetArea)));
	connect(ui.LibraryDockWidget, SIGNAL(topLevelChanged(bool)),                   this, SLOT(OnDockTopLevelChanged(bool)));

	//Always ensure the library tab is selected if not restoring, which will show preview renders.
	if (!restored)
	{
		ui.LibraryDockWidget->raise();
		ui.LibraryDockWidget->show();
		ui.XformsTabWidget->setCurrentIndex(2);//Make variations tab the currently selected one under the Xforms tab.
	}

	m_PreviousPaletteRow = -1;//Force click handler the first time through.
	SetCoordinateStatus(0, 0, 0, 0);
	SetTabOrders();
	m_SettingsPath = QFileInfo(m_Settings->fileName()).absoluteDir().absolutePath();
	ifstream ifs((m_SettingsPath + "/default.qss").toStdString().c_str(), ifstream::in);

	if (ifs.is_open())
	{
		string total, qs;
		total.reserve(20 * 1024);

		while (std::getline(ifs, qs))
			total += qs + "\n";

		m_Style = QString::fromStdString(total);
	}
	else
		m_Style = BaseStyle();

	setStyleSheet(m_Style);

	if (!m_Settings->Theme().isEmpty())
	{
		if (auto theme = QStyleFactory::create(m_Settings->Theme()))
		{
			m_Theme = theme;
			setStyle(m_Theme);
		}
	}
	else
	{
		if (!QStyleFactory::keys().empty())
		{
			m_Theme = QStyleFactory::create(qApp->style()->objectName());
			setStyle(m_Theme);
		}
	}

#ifdef __APPLE__

	for (auto dock : m_Docks)//Fixes focus problem on OSX.
	{
		if (!dock->isHidden())
		{
			dock->setFloating(!dock->isFloating());
			dock->setFloating(!dock->isFloating());
			break;
		}
	}

#endif
	//At this point, everything has been setup except the renderer. Shortly after
	//this constructor exits, GLWidget::InitGL() will create the initial flock and start the rendering timer
	//which executes whenever the program is idle. Upon starting the timer, the renderer
	//will be initialized.
	//auto cdc = wglGetCurrentDC();
	//auto cc = wglGetCurrentContext();
	//qDebug() << "Fractorium::Fractorium():";
	//qDebug() << "Current DC: " << cdc;
	//qDebug() << "Current Context: " << cc;
	QTimer::singleShot(1000, [&]() { ui.GLDisplay->InitGL(); });
}

/// <summary>
/// Destructor which saves out the settings file.
/// All other memory is cleared automatically through the use of STL.
/// </summary>
Fractorium::~Fractorium()
{
	SyncSequenceSettings();
	m_VarDialog->SyncSettings();
	m_Settings->ShowXforms(ui.ActionDrawXforms->isChecked());
	m_Settings->ShowGrid(ui.ActionDrawGrid->isChecked());
	m_Settings->setValue("windowState", saveState());
	m_Settings->sync();

	if (m_Settings->LoadLast())
		m_Controller->SaveCurrentFileOnShutdown();
}

/// <summary>
/// Return the URLs used to determine the icons that show up in the location bar in all file/folder dialogs.
/// </summary>
QList<QUrl> Fractorium::Urls()
{
	return m_Urls;
}

/// <summary>
/// Set the coordinate text in the status bar.
/// </summary>
/// <param name="rasX">The raster x coordinate</param>
/// <param name="rasY">The raster y coordinate</param>
/// <param name="worldX">The cartesian world x coordinate</param>
/// <param name="worldY">The cartesian world y coordinate</param>
void Fractorium::SetCoordinateStatus(int rasX, int rasY, float worldX, float worldY)
{
	static QString coords;
	coords.sprintf("Window: %4d, %4d World: %2.2f, %2.2f", rasX, rasY, worldX, worldY);
	m_CoordinateStatusLabel->setText(coords);
}

/// <summary>
/// Center the scroll area.
/// Called in response to a resizing, or setting of new ember.
/// </summary>
void Fractorium::CenterScrollbars()
{
	QScrollBar* w = ui.GLParentScrollArea->horizontalScrollBar();
	QScrollBar* h = ui.GLParentScrollArea->verticalScrollBar();
	w->setValue(w->maximum() / 2);
	h->setValue(h->maximum() / 2);
}

/// <summary>
/// Apply the settings for saving an ember to an Xml file to an ember (presumably about to be saved).
/// </summary>
/// <param name="ember">The ember to apply the settings to</param>
template <typename T>
void FractoriumEmberController<T>::ApplyXmlSavingTemplate(Ember<T>& ember)
{
	ember.m_Quality         = m_Fractorium->m_Settings->XmlQuality();
	ember.m_Supersample     = m_Fractorium->m_Settings->XmlSupersample();
	ember.m_TemporalSamples = m_Fractorium->m_Settings->XmlTemporalSamples();
}

/// <summary>
/// Return whether the current ember contains a final xform and the GUI is aware of it.
/// Note this can be true even if the final is empty, as long as they've added one and have
/// not explicitly deleted it.
/// </summary>
/// <returns>True if the current ember contains a final xform, else false.</returns>
bool Fractorium::HaveFinal()
{
	auto combo = ui.CurrentXformCombo;
	return (combo->count() > 0 && combo->itemText(combo->count() - 1) == "Final");
}

/// <summary>
/// Slots.
/// </summary>

/// <summary>
/// Empty placeholder for now.
/// Qt has a severe bug where the dock gets hidden behind the window.
/// Perhaps this will be used in the future if Qt ever fixes that bug.
/// Called when the top level dock is changed.
/// </summary>
/// <param name="topLevel">True if top level, else false.</param>
void Fractorium::OnDockTopLevelChanged(bool topLevel)
{
	//setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::TabPosition::North);
	//if (topLevel)
	//{
	//	if (ui.DockWidget->y() <= 0)
	//		ui.DockWidget->setGeometry(ui.DockWidget->x(), ui.DockWidget->y() + 100, ui.DockWidget->width(), ui.DockWidget->height());
	//
	//	ui.DockWidget->setFloating(true);
	//}
	//else
	//	ui.DockWidget->setFloating(false);
}

/// <summary>
/// Empty placeholder for now.
/// Qt has a severe bug where the dock gets hidden behind the window.
/// Perhaps this will be used in the future if Qt ever fixes that bug.
/// Called when the dock location is changed.
/// </summary>
/// <param name="area">The dock widget area</param>
void Fractorium::dockLocationChanged(Qt::DockWidgetArea area)
{
	//setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::TabPosition::North);
	//ui.DockWidget->resize(500, ui.DockWidget->height());
	//ui.DockWidget->update();
	//ui.dockWidget->setFloating(true);
	//ui.dockWidget->setFloating(false);
}

/// <summary>
/// Virtual event overrides.
/// </summary>

/// <summary>
/// Event filter for taking special action on:
/// Dock widget resize events, which in turn trigger GLParentScrollArea events.
/// Library tree key events, specifically delete.
/// Library tree drag n drop events.
/// </summary>
/// <param name="o">The object</param>
/// <param name="e">The eevent</param>
/// <returns>false</returns>
bool Fractorium::eventFilter(QObject* o, QEvent* e)
{
	static int fcount = 0;//Qt seems to deliver three events for every key press. So a count must be kept to only respond to the third event.
	static int xfupcount = 0;
	static int xfdncount = 0;

	if (o == ui.GLParentScrollArea && e->type() == QEvent::Resize)
	{
		m_WidthSpin->DoubleClickNonZero(ui.GLParentScrollArea->width() * ui.GLDisplay->devicePixelRatioF());
		m_HeightSpin->DoubleClickNonZero(ui.GLParentScrollArea->height() * ui.GLDisplay->devicePixelRatioF());
	}
	else if (auto ke = dynamic_cast<QKeyEvent*>(e))
	{
		auto combo = ui.CurrentXformCombo;
		bool shift = QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier);

		if (ke->key() >= Qt::Key_F1 && ke->key() <= Qt::Key_F32)
		{
			fcount++;

			if (fcount >= 3)
			{
				int val = ke->key() - (int)Qt::Key_F1;

				if (val < combo->count())
					combo->setCurrentIndex(val);

				fcount = 0;
				//qDebug() << "global function key press: " << ke->key() << " " << o->metaObject()->className() << " " << o->objectName();
			}

			return true;
		}
		else if (o == ui.LibraryTree)
		{
			//Require shift for deleting to prevent it from triggering when the user enters delete in the edit box.
			if (ke->key() == Qt::Key_Delete && e->type() == QEvent::KeyRelease && shift)
			{
				auto v = GetCurrentEmberIndex();

				if (ui.LibraryTree->topLevelItem(0)->childCount() > 1)
					OnDelete(v);

				return true;
			}
		}
		else if (o == this)
		{
			auto focusedctrlSpin = dynamic_cast<QSpinBox*>(this->focusWidget());
			auto focusedctrlDbSpin = dynamic_cast<QDoubleSpinBox*>(this->focusWidget());

			if (!focusedctrlSpin && !focusedctrlDbSpin)//Must exclude these because otherwise, typing a minus key in any of the spinners will switch the xform.
			{
				unsigned int index = combo->currentIndex();

				if (ke->key() == Qt::Key_Plus || ke->key() == Qt::Key_Equal)
				{
					xfupcount++;

					if (xfupcount >= 3)
					{
						xfupcount = 0;
						combo->setCurrentIndex((index + 1) % combo->count());
						//qDebug() << "global arrow plus key press: " << ke->key() << " " << o->metaObject()->className() << " " << o->objectName();
					}

					return true;
				}
				else if (ke->key() == Qt::Key_Minus || ke->key() == Qt::Key_Underscore)
				{
					xfdncount++;

					if (xfdncount >= 3)
					{
						xfdncount = 0;

						if (index == 0)
							index = combo->count();

						combo->setCurrentIndex((index - 1) % combo->count());
						//qDebug() << "global arrow minus key press: " << ke->key() << " " << o->metaObject()->className() << " " << o->objectName();
					}

					return true;
				}
			}
		}
	}

	return QMainWindow::eventFilter(o, e);
}

/// <summary>
/// Respond to a resize event which will set the double click default value
/// in the width and height spinners.
/// Note, this does not change the size of the ember being rendered or
/// the OpenGL texture it's being drawn on.
/// <param name="e">The event</param>
void Fractorium::resizeEvent(QResizeEvent* e)
{
	m_WidthSpin->DoubleClickNonZero(ui.GLParentScrollArea->width() * ui.GLDisplay->devicePixelRatioF());
	m_HeightSpin->DoubleClickNonZero(ui.GLParentScrollArea->height() * ui.GLDisplay->devicePixelRatioF());
	QMainWindow::resizeEvent(e);
}

/// <summary>
/// Respond to a show event to ensure Qt updates the native menubar.
/// On first create, Qt can fail to create the native menu bar properly,
/// but telling it that this window has become the focus window forces
/// it to refresh this.
/// <param name="e">The event</param>
void Fractorium::showEvent(QShowEvent* e)
{
	//Tell Qt to refresh the native menubar from this widget.
	emit qGuiApp->focusWindowChanged(windowHandle());
	QMainWindow::showEvent(e);
}

/// <summary>
/// Stop rendering and block before exiting.
/// Called on program exit.
/// </summary>
/// <param name="e">The event</param>
void Fractorium::closeEvent(QCloseEvent* e)
{
	if (m_Controller.get())
	{
		m_Controller->StopRenderTimer(true);//Will wait until fully exited and stopped.
		m_Controller->StopAllPreviewRenderers();
	}

	if (e)
		e->accept();
}

/// <summary>
/// Examine the files dragged when it first enters the window area.
/// Ok if at least one file is .flam3, .flam3 or .xml, else not ok.
/// Called when the user first drags files in.
/// </summary>
/// <param name="e">The event</param>
void Fractorium::dragEnterEvent(QDragEnterEvent* e)
{
	if (e->mimeData()->hasUrls())
	{
		auto urls = e->mimeData()->urls();

		for (auto& url : urls)
		{
			QString localFile = url.toLocalFile();
			QFileInfo fileInfo(localFile);
			QString suf = fileInfo.suffix();

			if (suf == "flam3" || suf == "flame" || suf == "xml" || suf == "chaos")
			{
				e->accept();
				break;
			}
		}
	}
}

/// <summary>
/// Always accept drag when moving, so that the drop event will correctly be called.
/// </summary>
/// <param name="e">The event</param>
void Fractorium::dragMoveEvent(QDragMoveEvent* e)
{
	e->accept();
}

/// <summary>
/// Examine and open the dropped files.
/// Called when the user drops a file in.
/// </summary>
/// <param name="e">The event</param>
void Fractorium::dropEvent(QDropEvent* e)
{
	QStringList filenames;
	Qt::KeyboardModifiers mod = e->keyboardModifiers();
	bool append = mod.testFlag(Qt::ControlModifier) ? false : true;

	if (e->mimeData()->hasUrls())
	{
		auto urls = e->mimeData()->urls();

		for (auto& url : urls)
		{
			QString localFile = url.toLocalFile();
			QFileInfo fileInfo(localFile);
			QString suf = fileInfo.suffix();

			if (suf == "flam3" || suf == "flame" || suf == "xml" || suf == "chaos")
				filenames << localFile;
		}
	}

	if (!filenames.empty())
		m_Controller->OpenAndPrepFiles(filenames, append);
}

/// <summary>
/// Setup a combo box to be placed in a table cell.
/// </summary>
/// <param name="table">The table the combo box belongs to</param>
/// <param name="receiver">The receiver object</param>
/// <param name="row">The row in the table where this combo box resides</param>
/// <param name="col">The col in the table where this combo box resides</param>
/// <param name="comboBox">Double pointer to combo box which will hold the spinner upon exit</param>
/// <param name="vals">The string values to populate the combo box with</param>
/// <param name="signal">The signal the combo box emits</param>
/// <param name="slot">The slot to receive the signal</param>
/// <param name="connectionType">Type of the connection. Default: Qt::QueuedConnection.</param>
void Fractorium::SetupCombo(QTableWidget* table, const QObject* receiver, int& row, int col, StealthComboBox*& comboBox, const vector<string>& vals, const char* signal, const char* slot, Qt::ConnectionType connectionType)
{
	comboBox = new StealthComboBox(table);

	for (auto& s : vals) comboBox->addItem(s.c_str());

	table->setCellWidget(row, col, comboBox);
	connect(comboBox, signal, receiver, slot, connectionType);
	row++;
}

/// <summary>
/// Set the header of a table to be fixed.
/// </summary>
/// <param name="header">The header to set</param>
/// <param name="mode">The resizing mode to use. Default: QHeaderView::Fixed.</param>
void Fractorium::SetFixedTableHeader(QHeaderView* header, QHeaderView::ResizeMode mode)
{
	header->setVisible(true);//For some reason, the designer keeps clobbering this value, so force it here.
	header->setSectionsClickable(false);
	header->setSectionResizeMode(mode);
}

/// <summary>
/// Setup and show the open XML dialog.
/// This will perform lazy instantiation.
/// </summary>
/// <returns>The list of filenames selected</returns>
QStringList Fractorium::SetupOpenXmlDialog()
{
#ifndef __APPLE__

	//Lazy instantiate since it takes a long time.
	if (!m_OpenFileDialog)
	{
		m_OpenFileDialog = new QFileDialog(this);
		m_OpenFileDialog->setViewMode(QFileDialog::List);
		m_OpenFileDialog->setOption(QFileDialog::DontUseNativeDialog, true);
		connect(m_OpenFileDialog, &QFileDialog::filterSelected, [&](const QString & filter) { m_Settings->OpenXmlExt(filter); });
		m_OpenFileDialog->setFileMode(QFileDialog::ExistingFiles);
		m_OpenFileDialog->setAcceptMode(QFileDialog::AcceptOpen);
		m_OpenFileDialog->setNameFilter("flam3 (*.flam3);;flame (*.flame);;xml (*.xml);;chaos (*.chaos)");
		m_OpenFileDialog->setWindowTitle("Open Flame");
		m_OpenFileDialog->setSidebarUrls(m_Urls);
	}

	QStringList filenames;
	m_OpenFileDialog->setDirectory(m_Settings->OpenFolder());
	m_OpenFileDialog->selectNameFilter(m_Settings->OpenXmlExt());

	if (m_OpenFileDialog->exec() == QDialog::Accepted)
	{
		filenames = m_OpenFileDialog->selectedFiles();

		if (!filenames.empty())
			m_Settings->OpenFolder(QFileInfo(filenames[0]).canonicalPath());
	}

#else
	auto defaultFilter(m_Settings->OpenXmlExt());
	auto filenames = QFileDialog::getOpenFileNames(this, tr("Open Flame"), m_Settings->OpenFolder(), tr("flam3(*.flam3);; flame(*.flame);; xml(*.xml);; chaos (*.chaos)"), &defaultFilter);
	m_Settings->OpenXmlExt(defaultFilter);

	if (!filenames.empty())
		m_Settings->OpenFolder(QFileInfo(filenames[0]).canonicalPath());

#endif
	return filenames;
}

/// <summary>
/// Setup and show the save XML dialog.
/// This will perform lazy instantiation.
/// </summary>
/// <param name="defaultFilename">The default filename to populate the text box with</param>
/// <returns>The filename selected</returns>
QString Fractorium::SetupSaveXmlDialog(const QString& defaultFilename)
{
#ifndef __APPLE__

	//Lazy instantiate since it takes a long time.
	//QS
	if (!m_SaveFileDialog)
	{
		m_SaveFileDialog = new QFileDialog(this);
		m_SaveFileDialog->setViewMode(QFileDialog::List);
		m_SaveFileDialog->setFileMode(QFileDialog::FileMode::AnyFile);
		m_SaveFileDialog->setOption(QFileDialog::DontUseNativeDialog, true);
		//This must be done once here because clears various internal states which allow the file text to be properly set.
		//This is most likely a bug in QFileDialog.
		m_SaveFileDialog->setAcceptMode(QFileDialog::AcceptSave);
		connect(m_SaveFileDialog, &QFileDialog::filterSelected, [&](const QString & filter)
		{
			m_Settings->SaveXmlExt(filter);
			m_SaveFileDialog->setDefaultSuffix(filter);
		});
		m_SaveFileDialog->setNameFilter("flam3 (*.flam3);;flame (*.flame);;xml (*.xml)");
		m_SaveFileDialog->setWindowTitle("Save flame as xml");
		m_SaveFileDialog->setSidebarUrls(m_Urls);
	}

	QString filename;
	m_SaveFileDialog->selectFile(defaultFilename);
	m_SaveFileDialog->setDirectory(m_Settings->SaveFolder());
	m_SaveFileDialog->selectNameFilter(m_Settings->SaveXmlExt());
	m_SaveFileDialog->setDefaultSuffix(m_Settings->SaveXmlExt());

	if (m_SaveFileDialog->exec() == QDialog::Accepted)
	{
		filename = m_SaveFileDialog->selectedFiles().value(0);
		auto filenames = filename.split(" (*");//This is a total hack, but Qt has the unfortunate behavior of including the description with the extension. It's probably a bug.
		filename = filenames[0];
		m_Settings->SaveXmlExt(m_SaveFileDialog->selectedNameFilter());
	}

#else
	auto defaultFilter(m_Settings->SaveXmlExt());
	auto filename = QFileDialog::getSaveFileName(this, tr("Save flame as xml"), m_Settings->SaveFolder() + "/" + defaultFilename, tr("flam3 (*.flam3);;flame (*.flame);;xml (*.xml)"), &defaultFilter);
	m_Settings->SaveXmlExt(defaultFilter);
#endif
	return filename;
}

/// <summary>
/// Setup and show the save image dialog.
/// This will perform lazy instantiation.
/// </summary>
/// <param name="defaultFilename">The default filename to populate the text box with</param>
/// <returns>The filename selected</returns>
QString Fractorium::SetupSaveImageDialog(const QString& defaultFilename)
{
#ifndef __APPLE__

	//Lazy instantiate since it takes a long time.
	if (!m_SaveImageDialog)
	{
		m_SaveImageDialog = new QFileDialog(this);
		m_SaveImageDialog->setViewMode(QFileDialog::List);
		m_SaveImageDialog->setFileMode(QFileDialog::FileMode::AnyFile);
		m_SaveImageDialog->setOption(QFileDialog::DontUseNativeDialog, true);
		//This must be done once here because clears various internal states which allow the file text to be properly set.
		//This is most likely a bug in QFileDialog.
		m_SaveImageDialog->setAcceptMode(QFileDialog::AcceptSave);
		connect(m_SaveImageDialog, &QFileDialog::filterSelected, [&](const QString & filter)
		{
			m_Settings->SaveImageExt(filter);
			m_SaveImageDialog->setDefaultSuffix(filter);
		});
#ifdef _WIN32
		m_SaveImageDialog->setNameFilter(".bmp;;.jpg;;.png;;.exr");
#else
		m_SaveImageDialog->setNameFilter(".jpg;;.png;;.exr");
#endif
		m_SaveImageDialog->setWindowTitle("Save image");
		m_SaveImageDialog->setSidebarUrls(m_Urls);
	}

	QString filename;
	m_SaveImageDialog->selectFile(defaultFilename);
	m_SaveImageDialog->setDirectory(m_Settings->SaveFolder());
	m_SaveImageDialog->selectNameFilter(m_Settings->SaveImageExt());
	m_SaveImageDialog->setDefaultSuffix(m_Settings->SaveImageExt());

	if (m_SaveImageDialog->exec() == QDialog::Accepted)
		filename = m_SaveImageDialog->selectedFiles().value(0);

#else
	auto defaultFilter(m_Settings->SaveImageExt());
	auto filename = QFileDialog::getSaveFileName(this, tr("Save image"), m_Settings->SaveFolder() + "/" + defaultFilename, tr("Jpg (*.jpg);;Png (*.png);;Exr (*.exr)"), &defaultFilter);
	m_Settings->SaveImageExt(defaultFilter);
#endif
	return filename;
}

/// <summary>
/// Setup and show the save folder dialog.
/// This will perform lazy instantiation.
/// </summary>
/// <returns>The folder selected, with '/' appended to the end</returns>
QString Fractorium::SetupSaveFolderDialog()
{
#ifndef __APPLE__

	//Lazy instantiate since it takes a long time.
	if (!m_FolderDialog)
	{
		m_FolderDialog = new QFileDialog(this);
		m_FolderDialog->setViewMode(QFileDialog::List);
		m_FolderDialog->setOption(QFileDialog::DontUseNativeDialog, true);
		//This must come first because it clears various internal states which allow the file text to be properly set.
		//This is most likely a bug in QFileDialog.
		m_FolderDialog->setAcceptMode(QFileDialog::AcceptSave);
		m_FolderDialog->setFileMode(QFileDialog::Directory);
		m_FolderDialog->setOption(QFileDialog::ShowDirsOnly, true);
		m_FolderDialog->setWindowTitle("Save to folder");
		m_FolderDialog->setSidebarUrls(m_Urls);
	}

	QString filename;
	m_FolderDialog->selectFile("");
	m_FolderDialog->setDirectory(m_Settings->SaveFolder());

	if (m_FolderDialog->exec() == QDialog::Accepted)
	{
		filename = MakeEnd(m_FolderDialog->selectedFiles().value(0), '/');
	}

#else
	auto filename = QFileDialog::getExistingDirectory(this, tr("Save to folder"),
					m_Settings->SaveFolder(),
					QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (filename.size() > 0)
		filename = MakeEnd(filename, '/');

#endif
	return filename;
}

/// <summary>
/// Thin wrapper around QMessageBox::critical() to allow it to be invoked from another thread.
/// </summary>
/// <param name="title">The title of the message box</param>
/// <param name="text">The text displayed on the message box</param>
/// <param name="invokeRequired">True if running on another thread, else false. Default: false.</param>
void Fractorium::ShowCritical(const QString& title, const QString& text, bool invokeRequired)
{
	if (!invokeRequired)
		QMessageBox::critical(this, title, text);
	else
		QMetaObject::invokeMethod(this, "ShowCritical", Qt::QueuedConnection, Q_ARG(const QString&, title), Q_ARG(const QString&, text), Q_ARG(bool, false));
}

/// <summary>
/// Explicitly set the tab orders for the entire program.
/// Qt has a facility to do this, but it fails when using custom widgets in
/// tables, so it must be done manually here.
/// This list must be kept in sync with any UI changes.
/// </summary>
void Fractorium::SetTabOrders()
{
	QWidget* w = SetTabOrder(this, ui.ColorTable, m_BrightnessSpin);//Flame color.
	w = SetTabOrder(this, w, m_GammaSpin);
	w = SetTabOrder(this, w, m_GammaThresholdSpin);
	w = SetTabOrder(this, w, m_VibrancySpin);
	w = SetTabOrder(this, w, m_HighlightSpin);
	w = SetTabOrder(this, w, m_BackgroundColorButton);
	w = SetTabOrder(this, w, m_PaletteModeCombo);
	w = SetTabOrder(this, w, m_WidthSpin);//Flame geometry.
	w = SetTabOrder(this, w, m_HeightSpin);
	w = SetTabOrder(this, w, m_CenterXSpin);
	w = SetTabOrder(this, w, m_CenterYSpin);
	w = SetTabOrder(this, w, m_ScaleSpin);
	w = SetTabOrder(this, w, m_ZoomSpin);
	w = SetTabOrder(this, w, m_RotateSpin);
	w = SetTabOrder(this, w, m_ZPosSpin);
	w = SetTabOrder(this, w, m_PerspectiveSpin);
	w = SetTabOrder(this, w, m_PitchSpin);
	w = SetTabOrder(this, w, m_YawSpin);
	w = SetTabOrder(this, w, m_DepthBlurSpin);
	w = SetTabOrder(this, w, m_SpatialFilterWidthSpin);//Flame filter.
	w = SetTabOrder(this, w, m_SpatialFilterTypeCombo);
	w = SetTabOrder(this, w, m_DEFilterMinRadiusSpin);
	w = SetTabOrder(this, w, m_DEFilterMaxRadiusSpin);
	w = SetTabOrder(this, w, m_DECurveSpin);
	w = SetTabOrder(this, w, m_SbsSpin);//Flame iteration.
	w = SetTabOrder(this, w, m_FuseSpin);
	w = SetTabOrder(this, w, m_RandRangeSpin);
	w = SetTabOrder(this, w, m_QualitySpin);
	w = SetTabOrder(this, w, m_SupersampleSpin);
	w = SetTabOrder(this, w, m_InterpTypeCombo);//Flame animation.
	w = SetTabOrder(this, w, m_AffineInterpTypeCombo);
	w = SetTabOrder(this, w, m_TemporalFilterWidthSpin);
	w = SetTabOrder(this, w, m_TemporalFilterTypeCombo);
	w = SetTabOrder(this, ui.LibraryTree, ui.SequenceStartCountSpinBox);//Library.
	w = SetTabOrder(this, w, ui.SequenceStartPreviewsButton);
	w = SetTabOrder(this, w, ui.SequenceStopPreviewsButton);
	w = SetTabOrder(this, w, ui.SequenceStartFlameSpinBox);
	w = SetTabOrder(this, w, ui.SequenceStopFlameSpinBox);
	w = SetTabOrder(this, w, ui.SequenceAllButton);
	w = SetTabOrder(this, w, ui.SequenceRandomizeStaggerCheckBox);
	w = SetTabOrder(this, w, ui.SequenceStaggerSpinBox);
	w = SetTabOrder(this, w, ui.SequenceRandomStaggerMaxSpinBox);
	w = SetTabOrder(this, w, ui.SequenceRandomizeRotationsCheckBox);
	w = SetTabOrder(this, w, ui.SequenceRotationsSpinBox);
	w = SetTabOrder(this, w, ui.SequenceRotationsCWCheckBox);
	w = SetTabOrder(this, w, ui.SequenceRandomRotationsMaxSpinBox);
	w = SetTabOrder(this, w, ui.SequenceRandomizeFramesPerRotCheckBox);
	w = SetTabOrder(this, w, ui.SequenceFramesPerRotSpinBox);
	w = SetTabOrder(this, w, ui.SequenceRandomFramesPerRotMaxSpinBox);
	w = SetTabOrder(this, w, ui.SequenceRandomizeBlendFramesCheckBox);
	w = SetTabOrder(this, w, ui.SequenceBlendFramesSpinBox);
	w = SetTabOrder(this, w, ui.SequenceRandomBlendMaxFramesSpinBox);
	w = SetTabOrder(this, w, ui.SequenceRandomizeRotationsPerBlendCheckBox);
	w = SetTabOrder(this, w, ui.SequenceRotationsPerBlendSpinBox);
	w = SetTabOrder(this, w, ui.SequenceRotationsPerBlendCWCheckBox);
	w = SetTabOrder(this, w, ui.SequenceRotationsPerBlendMaxSpinBox);
	w = SetTabOrder(this, w, ui.SequenceGenerateButton);
	w = SetTabOrder(this, w, ui.SequenceRenderButton);
	w = SetTabOrder(this, w, ui.SequenceSaveButton);
	w = SetTabOrder(this, w, ui.SequenceOpenButton);
	w = SetTabOrder(this, w, ui.SequenceTree);
	w = SetTabOrder(this, ui.CurrentXformCombo, ui.AddXformButton);//Xforms.
	w = SetTabOrder(this, w, ui.AddLinkedXformButton);
	w = SetTabOrder(this, w, ui.DuplicateXformButton);
	w = SetTabOrder(this, w, ui.ClearXformButton);
	w = SetTabOrder(this, w, ui.DeleteXformButton);
	w = SetTabOrder(this, w, ui.AddFinalXformButton);
	w = SetTabOrder(this, w, m_XformWeightSpin);
	w = SetTabOrder(this, w, m_XformWeightSpinnerButtonWidget->m_Button);
	w = SetTabOrder(this, m_XformColorIndexSpin, ui.XformColorScroll);//Xforms color.
	w = SetTabOrder(this, w, ui.RandomColorIndicesButton);
	w = SetTabOrder(this, w, ui.ToggleColorIndicesButton);
	w = SetTabOrder(this, w, m_XformColorSpeedSpin);
	w = SetTabOrder(this, w, m_XformOpacitySpin);
	w = SetTabOrder(this, w, m_XformDirectColorSpin);
	w = SetTabOrder(this, w, ui.SoloXformCheckBox);
	w = SetTabOrder(this, w, m_PreX1Spin);
	w = SetTabOrder(this, w, m_PreX2Spin);
	w = SetTabOrder(this, w, m_PreY1Spin);
	w = SetTabOrder(this, w, m_PreY2Spin);
	w = SetTabOrder(this, w, m_PreO1Spin);
	w = SetTabOrder(this, w, m_PreO2Spin);
	w = SetTabOrder(this, w, ui.PreCopyButton);
	w = SetTabOrder(this, w, ui.PreFlipVerticalButton);
	w = SetTabOrder(this, w, ui.PreResetButton);
	w = SetTabOrder(this, w, ui.PreFlipHorizontalButton);
	w = SetTabOrder(this, w, ui.PrePasteButton);
	w = SetTabOrder(this, w, ui.PreRotate90CcButton);
	w = SetTabOrder(this, w, ui.PreRotateCcButton);
	w = SetTabOrder(this, w, ui.PreRotateCombo);
	w = SetTabOrder(this, w, ui.PreRotateCButton);
	w = SetTabOrder(this, w, ui.PreRotate90CButton);
	w = SetTabOrder(this, w, ui.PreMoveUpButton);
	w = SetTabOrder(this, w, ui.PreMoveDownButton);
	w = SetTabOrder(this, w, ui.PreMoveCombo);
	w = SetTabOrder(this, w, ui.PreMoveLeftButton);
	w = SetTabOrder(this, w, ui.PreMoveRightButton);
	w = SetTabOrder(this, w, ui.PreScaleDownButton);
	w = SetTabOrder(this, w, ui.PreScaleCombo);
	w = SetTabOrder(this, w, ui.PreScaleUpButton);
	w = SetTabOrder(this, w, ui.PreRandomButton);
	w = SetTabOrder(this, w, ui.ShowPreAffineCurrentRadio);
	w = SetTabOrder(this, w, ui.ShowPreAffineSelectedRadio);
	w = SetTabOrder(this, w, ui.ShowPreAffineAllRadio);
	w = SetTabOrder(this, w, ui.SwapAffinesButton);
	w = SetTabOrder(this, w, ui.PostAffineGroupBox);
	w = SetTabOrder(this, w, m_PostX1Spin);
	w = SetTabOrder(this, w, m_PostX2Spin);
	w = SetTabOrder(this, w, m_PostY1Spin);
	w = SetTabOrder(this, w, m_PostY2Spin);
	w = SetTabOrder(this, w, m_PostO1Spin);
	w = SetTabOrder(this, w, m_PostO2Spin);
	w = SetTabOrder(this, w, ui.PostCopyButton);
	w = SetTabOrder(this, w, ui.PostFlipVerticalButton);
	w = SetTabOrder(this, w, ui.PostResetButton);
	w = SetTabOrder(this, w, ui.PostFlipHorizontalButton);
	w = SetTabOrder(this, w, ui.PostPasteButton);
	w = SetTabOrder(this, w, ui.PostRotate90CcButton);
	w = SetTabOrder(this, w, ui.PostRotateCcButton);
	w = SetTabOrder(this, w, ui.PostRotateCombo);
	w = SetTabOrder(this, w, ui.PostRotateCButton);
	w = SetTabOrder(this, w, ui.PostRotate90CButton);
	w = SetTabOrder(this, w, ui.PostMoveUpButton);
	w = SetTabOrder(this, w, ui.PostMoveDownButton);
	w = SetTabOrder(this, w, ui.PostMoveCombo);
	w = SetTabOrder(this, w, ui.PostMoveLeftButton);
	w = SetTabOrder(this, w, ui.PostMoveRightButton);
	w = SetTabOrder(this, w, ui.PostScaleDownButton);
	w = SetTabOrder(this, w, ui.PostScaleCombo);
	w = SetTabOrder(this, w, ui.PostScaleUpButton);
	w = SetTabOrder(this, w, ui.PostRandomButton);
	w = SetTabOrder(this, w, ui.ShowPostAffineCurrentRadio);
	w = SetTabOrder(this, w, ui.ShowPostAffineSelectedRadio);
	w = SetTabOrder(this, w, ui.ShowPostAffineAllRadio);
	w = SetTabOrder(this, w, ui.PolarAffineCheckBox);
	w = SetTabOrder(this, w, ui.LocalPivotRadio);
	w = SetTabOrder(this, w, ui.WorldPivotRadio);
	w = SetTabOrder(this, ui.VariationsFilterLineEdit, ui.VariationsFilterClearButton);//Xforms variation.
	w = SetTabOrder(this, w, ui.VariationsTree);
	w = SetTabOrder(this, w, ui.ClearXaosButton);
	w = SetTabOrder(this, w, ui.RandomXaosButton);
	w = SetTabOrder(this, w, ui.AddLayerButton);
	w = SetTabOrder(this, w, ui.AddLayerSpinBox);
	w = SetTabOrder(this, w, ui.TransposeXaosButton);
	//Xforms xaos is done dynamically every time.
	w = SetTabOrder(this, ui.PaletteFilenameCombo, m_PaletteHueSpin);//Palette.
	w = SetTabOrder(this, w, m_PaletteContrastSpin);
	w = SetTabOrder(this, w, m_PaletteSaturationSpin);
	w = SetTabOrder(this, w, m_PaletteBlurSpin);
	w = SetTabOrder(this, w, m_PaletteBrightnessSpin);
	w = SetTabOrder(this, w, m_PaletteFrequencySpin);
	w = SetTabOrder(this, w, ui.PaletteRandomSelectButton);
	w = SetTabOrder(this, w, ui.PaletteRandomAdjustButton);
	w = SetTabOrder(this, w, ui.PaletteEditorButton);
	w = SetTabOrder(this, w, ui.PaletteFilterLineEdit);
	w = SetTabOrder(this, w, ui.PaletteFilterClearButton);
	w = SetTabOrder(this, w, ui.PaletteListTable);
	w = SetTabOrder(this, w, ui.ResetCurvesButton);//Palette curves.
	w = SetTabOrder(this, w, ui.CurvesView);
	w = SetTabOrder(this, w, ui.CurvesGroupBox);
	w = SetTabOrder(this, w, ui.CurvesAllRadio);
	w = SetTabOrder(this, w, ui.CurvesRedRadio);
	w = SetTabOrder(this, w, ui.CurvesGreenRadio);
	w = SetTabOrder(this, w, ui.CurvesBlueRadio);
	w = SetTabOrder(this, ui.SummaryTable, ui.SummaryTree);//Info summary.
	w = SetTabOrder(this, ui.InfoBoundsGroupBox, ui.InfoBoundsFrame);//Info bounds.
	w = SetTabOrder(this, w, ui.InfoBoundsTable);
	w = SetTabOrder(this, w, ui.InfoFileOpeningGroupBox);
	w = SetTabOrder(this, w, ui.InfoFileOpeningTextEdit);
	w = SetTabOrder(this, w, ui.InfoRenderingGroupBox);
	w = SetTabOrder(this, w, ui.InfoRenderingTextEdit);
}

/// <summary>
/// Toggle all table spinner values in one row.
/// The logic is:
///		If any cell in the row is non zero, set all cells to zero, else 1.
///		If shift is held down, reverse the logic.
/// Resets the rendering process.
/// </summary>
/// <param name="table">The QTableWidget or QTableView whose row will be toggled</param>
/// <param name="logicalIndex">The index of the row that was double clicked</param>
void Fractorium::ToggleTableRow(QTableView* table, int logicalIndex)
{
	bool allZero = true;
	auto model = table->model();
	int cols = model->columnCount();
	bool shift = QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier);
	auto tableWidget = qobject_cast<QTableWidget*>(table);

	if (tableWidget)
	{
		for (int i = 0; i < cols; i++)
		{
			if (auto spinBox = qobject_cast<DoubleSpinBox*>(tableWidget->cellWidget(logicalIndex, i)))
			{
				if (!IsNearZero(spinBox->value()))
				{
					allZero = false;
					break;
				}
			}
		}

		if (shift)
			allZero = !allZero;

		double val = allZero ? 1.0 : 0.0;

		for (int i = 0; i < cols; i++)
			if (auto spinBox = qobject_cast<DoubleSpinBox*>(tableWidget->cellWidget(logicalIndex, i)))
				spinBox->setValue(val);
	}
	else
	{
		for (int i = 0; i < cols; i++)
		{
			if (!IsNearZero(model->data(model->index(logicalIndex, i, QModelIndex())).toDouble()))
			{
				allZero = false;
				break;
			}
		}

		if (shift)
			allZero = !allZero;

		double val = allZero ? 1.0 : 0.0;

		for (int i = 0; i < cols; i++)
			model->setData(model->index(logicalIndex, i), val, Qt::EditRole);
	}
}

/// <summary>
/// Toggle all table spinner values in one column.
/// The logic is:
///		If any cell in the column is non zero, set all cells to zero, else 1.
///		If shift is held down, reverse the logic.
/// Resets the rendering process.
/// </summary>
/// <param name="table">The QTableWidget or QTableView whose column will be toggled</param>
/// <param name="logicalIndex">The index of the column that was double clicked</param>
void Fractorium::ToggleTableCol(QTableView* table, int logicalIndex)
{
	bool allZero = true;
	auto model = table->model();
	int rows = model->rowCount();
	bool shift = QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier);
	auto tableWidget = qobject_cast<QTableWidget*>(table);

	if (tableWidget)
	{
		for (int i = 0; i < rows; i++)
		{
			if (auto spinBox = qobject_cast<DoubleSpinBox*>(tableWidget->cellWidget(i, logicalIndex)))
			{
				if (!IsNearZero(spinBox->value()))
				{
					allZero = false;
					break;
				}
			}
		}

		if (shift)
			allZero = !allZero;

		double val = allZero ? 1.0 : 0.0;

		for (int i = 0; i < rows; i++)
			if (auto spinBox = qobject_cast<DoubleSpinBox*>(tableWidget->cellWidget(i, logicalIndex)))
				spinBox->setValue(val);
	}
	else
	{
		for (int i = 0; i < rows; i++)
		{
			if (!IsNearZero(model->data(model->index(i, logicalIndex, QModelIndex())).toDouble()))
			{
				allZero = false;
				break;
			}
		}

		if (shift)
			allZero = !allZero;

		double val = allZero ? 1.0 : 0.0;

		for (int i = 0; i < rows; i++)
			model->setData(model->index(i, logicalIndex), val, Qt::EditRole);
	}
}

template class FractoriumEmberController<float>;

#ifdef DO_DOUBLE
	template class FractoriumEmberController<double>;
#endif
