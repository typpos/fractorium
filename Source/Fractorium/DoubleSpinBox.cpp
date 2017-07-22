#include "FractoriumPch.h"
#include "DoubleSpinBox.h"

QTimer DoubleSpinBox::s_Timer;

/// <summary>
/// Constructor that passes parent to the base and sets up height and step.
/// Specific focus policy is used to allow the user to hover over the control
/// and change its value using the mouse wheel without explicitly having to click
/// inside of it.
/// </summary>
/// <param name="p">The parent widget. Default: nullptr.</param>
/// <param name="height">The height of the spin box. Default: 16.</param>
/// <param name="step">The step used to increment/decrement the spin box when using the mouse wheel. Default: 0.05.</param>
DoubleSpinBox::DoubleSpinBox(QWidget* p, int h, double step)
	: QDoubleSpinBox(p)
{
	m_DoubleClick = false;
	m_DoubleClickLowVal = 0;
	m_DoubleClickNonZero = 0;
	m_DoubleClickZero = 1;
	m_Step = step;
	m_SmallStep = step / 10.0;
	m_Settings = FractoriumSettings::DefInstance();
	setSingleStep(step);
	setFrame(false);
	setButtonSymbols(QAbstractSpinBox::NoButtons);
	setFocusPolicy(Qt::StrongFocus);
	setMinimumHeight(h);//setGeometry() has no effect, so must set both of these instead.
	setMaximumHeight(h);
	setContextMenuPolicy(Qt::PreventContextMenu);
	lineEdit()->installEventFilter(this);
	lineEdit()->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	connect(this, SIGNAL(valueChanged(double)), this, SLOT(OnSpinBoxValueChanged(double)), Qt::QueuedConnection);
}

/// <summary>
/// Set the value of the control without triggering signals.
/// </summary>
/// <param name="d">The value to set it to</param>
void DoubleSpinBox::SetValueStealth(double d)
{
	blockSignals(true);
	setValue(d);
	blockSignals(false);
}

/// <summary>
/// Set whether to respond to double click events.
/// </summary>
/// <param name="b">True if this should respond to double click events, else false.</param>
void DoubleSpinBox::DoubleClick(bool b)
{
	m_DoubleClick = b;
}

/// <summary>
/// Set the value to be used instead of zero to represent the lower value
/// used when responding to a double click.
/// </summary>
/// <param name="val">The value to be used for the lower value instead of zero</param>
void DoubleSpinBox::DoubleClickLowVal(double val)
{
	m_DoubleClickLowVal = val;
}

/// <summary>
/// Set the value to be used when the user double clicks the spinner while
/// it contains zero.
/// </summary>
/// <param name="val">The value to be used</param>
void DoubleSpinBox::DoubleClickZero(double val)
{
	m_DoubleClickZero = val;
}

/// <summary>
/// Set the value to be used when the user double clicks the spinner while
/// it contains a non-zero value.
/// </summary>
/// <param name="val">The value to be used</param>
void DoubleSpinBox::DoubleClickNonZero(double val)
{
	m_DoubleClickNonZero = val;
}

/// <summary>
/// Get the default step used when the user scrolls.
/// </summary>
double DoubleSpinBox::Step()
{
	return m_Step;
}

/// <summary>
/// Set the default step to be used when the user scrolls.
/// </summary>
/// <param name="step">The step to use for scrolling</param>
void DoubleSpinBox::Step(double step)
{
	m_Step = step;
}

/// <summary>
/// Get the small step to be used when the user holds down shift while scrolling.
/// </summary>
double DoubleSpinBox::SmallStep()
{
	return m_SmallStep;
}

/// <summary>
/// Set the small step to be used when the user holds down shift while scrolling.
/// The default is step / 10, so use this if something else is needed.
/// </summary>
/// <param name="step">The small step to use for scrolling while the shift key is down</param>
void DoubleSpinBox::SmallStep(double step)
{
	m_SmallStep = step;
}

/// <summary>
/// Expose the underlying QLineEdit control to the caller.
/// </summary>
/// <returns>A pointer to the QLineEdit</returns>
QLineEdit* DoubleSpinBox::lineEdit()
{
	return QDoubleSpinBox::lineEdit();
}

/// <summary>
/// Another workaround for the persistent text selection bug in Qt.
/// </summary>
void DoubleSpinBox::OnSpinBoxValueChanged(double)
{
	lineEdit()->deselect();//Gets rid of nasty "feature" that always has text selected.
}

/// <summary>
/// Called while the timer is activated due to the right mouse button being held down.
/// </summary>
void DoubleSpinBox::OnTimeout()
{
	int xdistance = m_MouseMovePoint.x() - m_MouseDownPoint.x();
	int ydistance = m_MouseMovePoint.y() - m_MouseDownPoint.y();
	int distance = abs(xdistance) > abs(ydistance) ? xdistance : ydistance;
	double scale, val;
	double d = value();
	bool shift = QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier);
	bool ctrl = QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier);
	double amount = (m_SmallStep + m_Step) * 0.5;

	if (shift)
		scale = 0.0001;
	else if (ctrl)
		scale = 0.01;
	else
		scale = 0.001;

	val = d + (distance * amount * scale);
	setValue(val);
}

/// <summary>
/// Event filter for taking special action on double click events.
/// </summary>
/// <param name="o">The object</param>
/// <param name="e">The eevent</param>
/// <returns>false</returns>
bool DoubleSpinBox::eventFilter(QObject* o, QEvent* e)
{
	auto me = dynamic_cast<QMouseEvent*>(e);

	if (isEnabled() && me)
	{
		if (!m_Settings->ToggleType() &&//Ensure double click toggles, not right click.
				me->type() == QMouseEvent::MouseButtonPress &&
				me->button() == Qt::RightButton)
		{
			m_MouseDownPoint = m_MouseMovePoint = me->pos();
			StartTimer();
		}
		else if (!m_Settings->ToggleType() &&
				 me->type() == QMouseEvent::MouseButtonRelease &&
				 me->button() == Qt::RightButton)
		{
			StopTimer();
			m_MouseDownPoint = m_MouseMovePoint = me->pos();
		}
		else if (!m_Settings->ToggleType() &&
				 me->type() == QMouseEvent::MouseMove &&
				 QGuiApplication::mouseButtons() & Qt::RightButton)
		{
			m_MouseMovePoint = me->pos();
		}
		else if (m_DoubleClick &&
				 ((!m_Settings->ToggleType() && e->type() == QMouseEvent::MouseButtonDblClick && me->button() == Qt::LeftButton) ||
				  (m_Settings->ToggleType() && me->type() == QMouseEvent::MouseButtonRelease && me->button() == Qt::RightButton)))
		{
			if (IsClose(m_DoubleClickLowVal, value()))
				setValue(m_DoubleClickZero);
			else
				setValue(m_DoubleClickNonZero);
		}
	}
	else
	{
		if (e->type() == QEvent::Wheel)
		{
			//Take special action for shift to reduce the scroll amount. Control already
			//increases it automatically.
			if (QWheelEvent* we = dynamic_cast<QWheelEvent*>(e))
			{
				Qt::KeyboardModifiers mod = we->modifiers();

				if (mod.testFlag(Qt::ShiftModifier))
					setSingleStep(m_SmallStep);
				else
					setSingleStep(m_Step);
			}
		}
	}

	return QDoubleSpinBox::eventFilter(o, e);
}

/// <summary>
/// Called when focus enters the spinner.
/// </summary>
/// <param name="e">The event</param>
void DoubleSpinBox::focusInEvent(QFocusEvent* e)
{
	StopTimer();
	QDoubleSpinBox::focusInEvent(e);
}

/// <summary>
/// Called when focus leaves the spinner.
/// Qt has a nasty "feature" that leaves the text in a spinner selected
/// and the cursor visible, regardless of whether it has the focus.
/// Manually clear both here.
/// </summary>
/// <param name="e">The event</param>
void DoubleSpinBox::focusOutEvent(QFocusEvent* e)
{
	StopTimer();
	QDoubleSpinBox::focusOutEvent(e);
}

/// <summary>
/// Called when focus enters the spinner.
/// Must set the focus to make sure key down messages don't erroneously go to the GLWidget.
/// </summary>
/// <param name="e">The event</param>
void DoubleSpinBox::enterEvent(QEvent* e)
{
	StopTimer();
	QDoubleSpinBox::enterEvent(e);
}

/// <summary>
/// Called when focus leaves the spinner.
/// Must clear the focus to make sure key down messages don't erroneously go to the GLWidget.
/// </summary>
/// <param name="e">The event</param>
void DoubleSpinBox::leaveEvent(QEvent* e)
{
	StopTimer();
	QDoubleSpinBox::leaveEvent(e);
}

/// <summary>
/// Start the timer in response to the right mouse button being pressed.
/// </summary>
void DoubleSpinBox::StartTimer()
{
	s_Timer.stop();
	connect(&s_Timer, SIGNAL(timeout()), this, SLOT(OnTimeout()));
	s_Timer.start(300);
}

/// <summary>
/// Stop the timer in response to the left mouse button being pressed.
/// </summary>
void DoubleSpinBox::StopTimer()
{
	s_Timer.stop();
	disconnect(&s_Timer, SIGNAL(timeout()), this, SLOT(OnTimeout()));
}

/// <summary>
/// Constructor that passes agruments to the base and assigns the m_Param and m_Variation members.
/// It also sets up the context menu for special numerical values.
/// </summary>
/// <param name="p">The parent widget</param>
/// <param name="widgetItem">The widget item this spinner is contained in</param>
/// <param name="id">The variation this spinner is for</param>
/// <param name="param">The name of the parameter this is for</param>
/// <param name="h">The height of the spin box. Default: 16.</param>
/// <param name="step">The step used to increment/decrement the spin box when using the mouse wheel. Default: 0.05.</param>
VariationTreeDoubleSpinBox::VariationTreeDoubleSpinBox(QWidget* p, VariationTreeWidgetItem* widgetItem, eVariationId id, const string& param, int h, double step)
	: DoubleSpinBox(p, h, step)
{
	m_WidgetItem = widgetItem;
	m_Param = param;
	m_Id = id;
	setDecimals(3);
	//PI
	auto piAction = new QAction("PI", this);
	connect(piAction, SIGNAL(triggered(bool)), this, SLOT(PiActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(piAction);
	//PI * 2
	auto twoPiAction = new QAction("2 PI", this);
	connect(twoPiAction, SIGNAL(triggered(bool)), this, SLOT(TwoPiActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(twoPiAction);
	//PI / 2
	auto piOver2Action = new QAction("PI / 2", this);
	connect(piOver2Action, SIGNAL(triggered(bool)), this, SLOT(PiOver2ActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(piOver2Action);
	//PI / 3
	auto piOver3Action = new QAction("PI / 3", this);
	connect(piOver3Action, SIGNAL(triggered(bool)), this, SLOT(PiOver3ActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(piOver3Action);
	//PI / 4
	auto piOver4Action = new QAction("PI / 4", this);
	connect(piOver4Action, SIGNAL(triggered(bool)), this, SLOT(PiOver4ActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(piOver4Action);
	//PI / 6
	auto piOver6Action = new QAction("PI / 6", this);
	connect(piOver6Action, SIGNAL(triggered(bool)), this, SLOT(PiOver6ActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(piOver6Action);
	//1 / PI
	auto oneOverPiAction = new QAction("1 / PI", this);
	connect(oneOverPiAction, SIGNAL(triggered(bool)), this, SLOT(OneOverPiActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(oneOverPiAction);
	//2 / PI
	auto twoOverPiAction = new QAction("2 / PI", this);
	connect(twoOverPiAction, SIGNAL(triggered(bool)), this, SLOT(TwoOverPiActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(twoOverPiAction);
	//3 / PI
	auto threeOverPiAction = new QAction("3 / PI", this);
	connect(threeOverPiAction, SIGNAL(triggered(bool)), this, SLOT(ThreeOverPiActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(threeOverPiAction);
	//4 / PI
	auto fourOverPiAction = new QAction("4 / PI", this);
	connect(fourOverPiAction, SIGNAL(triggered(bool)), this, SLOT(FourOverPiActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(fourOverPiAction);
	//Sqrt(2)
	auto sqrtTwoAction = new QAction("Sqrt(2)", this);
	connect(sqrtTwoAction, SIGNAL(triggered(bool)), this, SLOT(SqrtTwoActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(sqrtTwoAction);
	//Sqrt(2)
	auto sqrtThreeAction = new QAction("Sqrt(3)", this);
	connect(sqrtThreeAction, SIGNAL(triggered(bool)), this, SLOT(SqrtThreeActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(sqrtThreeAction);
	//Need this for it to show up properly.
	this->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void VariationTreeDoubleSpinBox::PiActionTriggered(bool checked) { setValue(M_PI); }
void VariationTreeDoubleSpinBox::TwoPiActionTriggered(bool checked) { setValue(M_PI * 2); }
void VariationTreeDoubleSpinBox::PiOver2ActionTriggered(bool checked) { setValue(M_PI_2); }
void VariationTreeDoubleSpinBox::PiOver3ActionTriggered(bool checked) { setValue(M_PI / 3); }
void VariationTreeDoubleSpinBox::PiOver4ActionTriggered(bool checked) { setValue(M_PI / 4); }
void VariationTreeDoubleSpinBox::PiOver6ActionTriggered(bool checked) { setValue(M_PI / 6); }
void VariationTreeDoubleSpinBox::OneOverPiActionTriggered(bool checked)   { setValue(1 / M_PI); }
void VariationTreeDoubleSpinBox::TwoOverPiActionTriggered(bool checked)   { setValue(2 / M_PI); }
void VariationTreeDoubleSpinBox::ThreeOverPiActionTriggered(bool checked) { setValue(3 / M_PI); }
void VariationTreeDoubleSpinBox::FourOverPiActionTriggered(bool checked)  { setValue(4 / M_PI); }
void VariationTreeDoubleSpinBox::SqrtTwoActionTriggered(bool checked) { setValue(M_SQRT2); }
void VariationTreeDoubleSpinBox::SqrtThreeActionTriggered(bool checked) { setValue(std::sqrt(3.0)); }

/// <summary>
/// Constructor that sets up the context menu for special numerical values specific to affine spinners.
/// </summary>
/// <param name="p">The parent widget</param>
/// <param name="h">The height of the spin box. Default: 20.</param>
/// <param name="step">The step used to increment/decrement the spin box when using the mouse wheel. Default: 0.01.</param>
AffineDoubleSpinBox::AffineDoubleSpinBox(QWidget* p, int h, double step)
	: DoubleSpinBox(p, h, step)
{
	//-1
	auto neg1Action = new QAction("-1", this);
	connect(neg1Action, SIGNAL(triggered(bool)), this, SLOT(NegOneActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(neg1Action);
	//0
	auto zeroAction = new QAction("0", this);
	connect(zeroAction, SIGNAL(triggered(bool)), this, SLOT(ZeroActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(zeroAction);
	//1
	auto oneAction = new QAction("1", this);
	connect(oneAction, SIGNAL(triggered(bool)), this, SLOT(OneActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(oneAction);
	//45
	auto fortyFiveAction = new QAction("45", this);
	connect(fortyFiveAction, SIGNAL(triggered(bool)), this, SLOT(FortyFiveActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(fortyFiveAction);
	//-45
	auto negFortyFiveAction = new QAction("-45", this);
	connect(negFortyFiveAction, SIGNAL(triggered(bool)), this, SLOT(NegFortyFiveActionTriggered(bool)), Qt::QueuedConnection);
	this->addAction(negFortyFiveAction);
	//Need this for it to show up properly.
	this->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void AffineDoubleSpinBox::NegOneActionTriggered(bool checked) { setValue(-1); }
void AffineDoubleSpinBox::ZeroActionTriggered(bool checked) { setValue(0); }
void AffineDoubleSpinBox::OneActionTriggered(bool checked) { setValue(1); }
void AffineDoubleSpinBox::FortyFiveActionTriggered(bool checked) { setValue(0.707107); }
void AffineDoubleSpinBox::NegFortyFiveActionTriggered(bool checked) { setValue(-0.707107); }
