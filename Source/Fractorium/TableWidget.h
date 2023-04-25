#pragma once

#include "FractoriumPch.h"

/// <summary>
/// TableWidget class.
/// </summary>

/// <summary>
/// The purpose of this subclass is to allow for dragging the contents of a table cell.
/// It's used in the palette preview table.
/// </summary>
class TableWidget : public QTableWidget
{
	Q_OBJECT
public:
	/// <summary>
	/// Constructor that passes the parent to the base and installs
	/// the event filter.
	/// </summary>
	/// <param name="p">The parent widget</param>
	explicit TableWidget(QWidget* p = nullptr)
		: QTableWidget(p)
	{
		viewport()->installEventFilter(this);
	}


signals:
	void MouseDragged(const QPointF& local, const QPointF& global);
	void MouseReleased();

protected:

	/// <summary>
	/// Event filter to handle dragging and releasing the mouse.
	/// Sadly, QTableWidget makes these hard to get to, so we must handle them here.
	/// </summary>
	/// <param name="obj">The object sending the event</param>
	/// <param name="e">The event</param>
	/// <returns>The result of calling the base fucntion.</returns>
	bool eventFilter(QObject* obj, QEvent* e) override
	{
		if (e->type() == QEvent::MouseMove)
		{
			if (const auto me = dynamic_cast<const QMouseEvent*>(e))
				emit MouseDragged(me->position(), me->globalPosition());
		}
		else if (e->type() == QEvent::MouseButtonRelease)
			emit MouseReleased();

		return QTableWidget::eventFilter(obj, e);
	}
};
