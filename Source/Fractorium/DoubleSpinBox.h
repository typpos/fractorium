#pragma once

#include "FractoriumPch.h"
#include "FractoriumSettings.h"

/// <summary>
/// DoubleSpinBox and VariationTreeDoubleSpinBox classes.
/// </summary>

enum class eSpinToggle : et { NONE = 0, SPIN_DOUBLE_CLICK = 1, SPIN_RIGHT_CLICK = 2 };

/// <summary>
/// A derivation to prevent the spin box from selecting its own text
/// when editing. Also to prevent multiple spin boxes from all having
/// selected text at once.
/// </summary>
class DoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT

public:
	explicit DoubleSpinBox(QWidget* parent = nullptr, int height = 16, double step = 0.05);
	virtual ~DoubleSpinBox() { }
	void SetValueStealth(double d);
	void DoubleClick(bool b);
	void DoubleClickLowVal(double val);
	void DoubleClickZero(double val);
	void DoubleClickNonZero(double val);
	double Step();
	void Step(double step);
	double SmallStep();
	void SmallStep(double step);
	QLineEdit* lineEdit();

public slots:
	void OnSpinBoxValueChanged(double d);
	void OnTimeout();

protected:
	virtual bool eventFilter(QObject* o, QEvent* e) override;
	virtual void keyPressEvent(QKeyEvent* event) override;
	virtual void focusInEvent(QFocusEvent* e) override;
	virtual void focusOutEvent(QFocusEvent* e) override;
	virtual void enterEvent(QEvent* e) override;
	virtual void leaveEvent(QEvent* e) override;

	bool m_DoubleClick;
	shared_ptr<FractoriumSettings> m_Settings;

private:
	void StartTimer();
	void StopTimer();

	double m_DoubleClickLowVal;
	double m_DoubleClickNonZero;
	double m_DoubleClickZero;
	double m_Step;
	double m_SmallStep;
	QPoint m_MouseDownPoint;
	QPoint m_MouseMovePoint;
	static QTimer s_Timer;
};

/// <summary>
/// Thin derivation to implement the eventFilter() override which subsequently derived
/// classes will use to suppress showing the context menu when right clicking is used for toggling,
/// unless shift is pressed.
/// </summary>
class SpecialDoubleSpinBox : public DoubleSpinBox
{
	Q_OBJECT

public:
	explicit SpecialDoubleSpinBox(QWidget* p = nullptr, int h = 16, double step = 0.05);
	virtual ~SpecialDoubleSpinBox() { }

protected:
	virtual bool eventFilter(QObject* o, QEvent* e) override;
};

/// <summary>
/// VariationTreeWidgetItem and VariationTreeDoubleSpinBox need each other, but each can't include the other.
/// So VariationTreeWidgetItem includes this file, and use a forward declaration here.
/// </summary>
class VariationTreeWidgetItem;

/// <summary>
/// Derivation for the double spin boxes that are in the
/// variations tree.
/// </summary>
class VariationTreeDoubleSpinBox : public SpecialDoubleSpinBox
{
	Q_OBJECT

public:
	explicit VariationTreeDoubleSpinBox(QWidget* p, VariationTreeWidgetItem* widgetItem, eVariationId id, const string& param, int h = 16, double step = 0.05);
	virtual ~VariationTreeDoubleSpinBox() { }
	bool IsParam() { return !m_Param.empty(); }
	string ParamName() { return m_Param; }
	eVariationId GetVariationId() { return m_Id; }
	VariationTreeWidgetItem* WidgetItem() { return m_WidgetItem; }
	virtual QString textFromValue(double value) const override;
	virtual double valueFromText(const QString& text) const override;

public slots:
	void PiActionTriggered(bool checked = false);
	void TwoPiActionTriggered(bool checked = false);
	void PiOver2ActionTriggered(bool checked = false);
	void PiOver3ActionTriggered(bool checked = false);
	void PiOver4ActionTriggered(bool checked = false);
	void PiOver6ActionTriggered(bool checked = false);
	void OneOverPiActionTriggered(bool checked = false);
	void TwoOverPiActionTriggered(bool checked = false);
	void ThreeOverPiActionTriggered(bool checked = false);
	void FourOverPiActionTriggered(bool checked = false);
	void SqrtTwoActionTriggered(bool checked = false);
	void SqrtThreeActionTriggered(bool checked = false);

private:
	string m_Param;
	eVariationId m_Id;
	VariationTreeWidgetItem* m_WidgetItem;
};

/// <summary>
/// Derivation for the double spin boxes that are in the
/// affine controls.
/// </summary>
class AffineDoubleSpinBox : public SpecialDoubleSpinBox
{
	Q_OBJECT

public:
	explicit AffineDoubleSpinBox(QWidget* p, int h = 20, double step = 0.01);
	virtual ~AffineDoubleSpinBox() { }

public slots:
	void NegOneActionTriggered(bool checked = false);
	void ZeroActionTriggered(bool checked = false);
	void OneActionTriggered(bool checked = false);
	void FortyFiveActionTriggered(bool checked = false);
	void NegFortyFiveActionTriggered(bool checked = false);
};
