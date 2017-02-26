#pragma once

#include "FractoriumPch.h"
#include "DoubleSpinBox.h"

/// <summary>
/// SpinBox class.
/// </summary>

/// <summary>
/// A derivation to prevent the spin box from selecting its own text
/// when editing. Also to prevent multiple spin boxes from all having
/// selected text at once.
/// </summary>
class SpinBox : public QSpinBox
{
	Q_OBJECT

public:
	explicit SpinBox(QWidget* p = nullptr, int height = 16, int step = 1);
	virtual ~SpinBox() { }
	void SetValueStealth(int d);
	void SetValueStealth(size_t d);
	void DoubleClick(bool b);
	void DoubleClickLowVal(int val);
	void DoubleClickZero(int val);
	void DoubleClickNonZero(int val);
	void SmallStep(int step);
	QLineEdit* lineEdit();

public slots:
	void onSpinBoxValueChanged(int i);
	void OnTimeout();

protected:
	bool eventFilter(QObject* o, QEvent* e);
	virtual void focusInEvent(QFocusEvent* e);
	virtual void focusOutEvent(QFocusEvent* e);
	virtual void enterEvent(QEvent* e);
	virtual void leaveEvent(QEvent* e);

private:
	void StartTimer();
	void StopTimer();

	bool m_DoubleClick;
	int m_DoubleClickLowVal;
	int m_DoubleClickNonZero;
	int m_DoubleClickZero;
	int m_Step;
	int m_SmallStep;
	QPoint m_MouseDownPoint;
	QPoint m_MouseMovePoint;
	shared_ptr<FractoriumSettings> m_Settings;
	static QTimer s_Timer;
};
