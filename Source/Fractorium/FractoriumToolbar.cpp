#include "FractoriumPch.h"
#include "Fractorium.h"
#include "QssDialog.h"

/// <summary>
/// Initialize the toolbar UI.
/// </summary>
void Fractorium::InitToolbarUI()
{
	auto clGroup = new QActionGroup(this);
	clGroup->addAction(ui.ActionCpu);
	clGroup->addAction(ui.ActionCL);
	auto spGroup = new QActionGroup(this);
	spGroup->addAction(ui.ActionSP);
	spGroup->addAction(ui.ActionDP);
	SyncOptionsToToolbar();
	ui.ActionDrawImage->setChecked(true);
	connect(ui.ActionCpu,	            SIGNAL(triggered(bool)), this, SLOT(OnActionCpu(bool)),	              Qt::QueuedConnection);
	connect(ui.ActionCL,	            SIGNAL(triggered(bool)), this, SLOT(OnActionCL(bool)),	              Qt::QueuedConnection);
	connect(ui.ActionSP,	            SIGNAL(triggered(bool)), this, SLOT(OnActionSP(bool)),	              Qt::QueuedConnection);
	connect(ui.ActionDP,	            SIGNAL(triggered(bool)), this, SLOT(OnActionDP(bool)),	              Qt::QueuedConnection);
	connect(ui.ActionStyle,             SIGNAL(triggered(bool)), this, SLOT(OnActionStyle(bool)),             Qt::QueuedConnection);
	connect(ui.ActionStartStopRenderer, SIGNAL(triggered(bool)), this, SLOT(OnActionStartStopRenderer(bool)), Qt::QueuedConnection);
	connect(ui.ActionDrawXforms,        SIGNAL(triggered(bool)), this, SLOT(OnActionDrawXforms(bool)),        Qt::QueuedConnection);
	connect(ui.ActionDrawImage,         SIGNAL(triggered(bool)), this, SLOT(OnActionDrawImage(bool)),	      Qt::QueuedConnection);
	connect(ui.ActionDrawGrid,          SIGNAL(triggered(bool)), this, SLOT(OnActionDrawGrid(bool)),          Qt::QueuedConnection);
}

/// <summary>
/// GUI wrapper functions, getters only.
/// </summary>

bool Fractorium::DrawXforms() { return ui.ActionDrawXforms->isChecked(); }
bool Fractorium::DrawImage()  { return ui.ActionDrawImage->isChecked();  }
bool Fractorium::DrawGrid()   { return ui.ActionDrawGrid->isChecked();   }

/// <summary>
/// Called when the CPU render option on the toolbar is clicked.
/// </summary>
/// <param name="checked">Check state, action only taken if true.</param>
void Fractorium::OnActionCpu(bool checked)
{
	if (checked && m_Settings->OpenCL())
	{
		m_Settings->OpenCL(false);
		ShutdownAndRecreateFromOptions(false);
	}
}

/// <summary>
/// Called when the OpenCL render option on the toolbar is clicked.
/// </summary>
/// <param name="checked">Check state, action only taken if true.</param>
void Fractorium::OnActionCL(bool checked)
{
	if (checked && !m_Settings->OpenCL())
	{
		m_Settings->OpenCL(true);
		ShutdownAndRecreateFromOptions(false);
	}
}

/// <summary>
/// Called when the single precision render option on the toolbar is clicked.
/// </summary>
/// <param name="checked">Check state, action only taken if true.</param>
void Fractorium::OnActionSP(bool checked)
{
	if (checked && m_Settings->Double())
	{
		m_Settings->Double(false);
		ShutdownAndRecreateFromOptions(true);//Pass true, but it's not needed because creating a new controller will force a library tree re-render.
	}
}

/// <summary>
/// Called when the double precision render option on the toolbar is clicked.
/// </summary>
/// <param name="checked">Check state, action only taken if true.</param>
void Fractorium::OnActionDP(bool checked)
{
	if (checked && !m_Settings->Double())
	{
		m_Settings->Double(true);
		ShutdownAndRecreateFromOptions(true);//Pass true, but it's not needed because creating a new controller will force a library tree re-render.
	}
}

/// <summary>
/// Called when the show style button is clicked.
/// </summary>
/// <param name="checked">Ignored</param>
void Fractorium::OnActionStyle(bool checked)
{
#ifdef __APPLE__
	m_QssDialog->exec();
#else
	m_QssDialog->show();
#endif
}

/// <summary>
/// Called when the start/stop renderer button is clicked.
/// </summary>
/// <param name="checked">Check state, stop renderer if true, else start.</param>
void Fractorium::OnActionStartStopRenderer(bool checked)
{
	EnableRenderControls(!checked);

	if (checked)
	{
		m_Controller->StopRenderTimer(true);
		ui.ActionStartStopRenderer->setToolTip("Start Renderer");
		ui.ActionStartStopRenderer->setIcon(QIcon(":/Fractorium/Icons/control.png"));
	}
	else
	{
		m_Controller->StartRenderTimer();
		ui.ActionStartStopRenderer->setToolTip("Stop Renderer");
		ui.ActionStartStopRenderer->setIcon(QIcon(":/Fractorium/Icons/control-stop-square.png"));
	}
}

/// <summary>
/// Toggle whether to show the affines.
/// Called when the editor image button is clicked.
/// </summary>
/// <param name="checked">Check state, show editor if true, else hide.</param>
void Fractorium::OnActionDrawXforms(bool checked)
{
	if (!ui.ActionDrawImage->isChecked() && !ui.ActionDrawXforms->isChecked())
		ui.ActionDrawImage->setChecked(true);

	ui.GLDisplay->update();
}

/// <summary>
/// Toggle whether to show the image.
/// Called when the image button is clicked.
/// </summary>
/// <param name="checked">Check state, show image if true, else hide.</param>
void Fractorium::OnActionDrawImage(bool checked)
{
	if (!ui.ActionDrawImage->isChecked() && !ui.ActionDrawXforms->isChecked())
		ui.ActionDrawXforms->setChecked(true);

	ui.GLDisplay->update();
}

/// <summary>
/// Toggle whether to show the grid.
/// Called when the grid image button is clicked.
/// </summary>
/// <param name="checked">Check state, show grid if true, else hide.</param>
void Fractorium::OnActionDrawGrid(bool checked)
{
	ui.GLDisplay->update();
}

/// <summary>
/// Sync options data to the check state of the toolbar buttons.
/// This does not trigger a clicked() event.
/// </summary>
void Fractorium::SyncOptionsToToolbar()
{
	static bool openCL = !m_Info->Devices().empty();

	if (!openCL)
	{
		ui.ActionCL->setEnabled(false);
	}

	if (openCL && m_Settings->OpenCL())
	{
		ui.ActionCpu->setChecked(false);
		ui.ActionCL->setChecked(true);
	}
	else
	{
		ui.ActionCpu->setChecked(true);
		ui.ActionCL->setChecked(false);
	}

	if (m_Settings->Double())
	{
		ui.ActionSP->setChecked(false);
		ui.ActionDP->setChecked(true);
	}
	else
	{
		ui.ActionSP->setChecked(true);
		ui.ActionDP->setChecked(false);
	}

	ui.ActionDrawGrid->setChecked(m_Settings->ShowGrid());
	ui.ActionDrawXforms->setChecked(m_Settings->ShowXforms());
}