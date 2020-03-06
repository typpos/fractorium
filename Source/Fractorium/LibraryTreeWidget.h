#pragma once

#include "FractoriumPch.h"

class Fractorium;

/// <summary>
/// A thin derivation of QTreeWidget which allows for processing the drop event.
/// </summary>
class LibraryTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	/// <summary>
	/// Constructor that passes p to the parent.
	/// </summary>
	/// <param name="p">The parent widget</param>
	explicit LibraryTreeWidget(QWidget* p = nullptr)
		: QTreeWidget(p)
	{
	}

	void SetMainWindow(Fractorium* f);

protected:
	virtual void dropEvent(QDropEvent* de) override;

	Fractorium* m_Fractorium = nullptr;
};


class InfoTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	/// <summary>
	/// Constructor that passes p to the parent.
	/// </summary>
	/// <param name="p">The parent widget</param>
	explicit InfoTreeWidget(QWidget* p = nullptr)
		: QTreeWidget(p)
	{
	}

	void SetMainWindow(Fractorium* f);
	const QString& LastNonVarField() const { return m_LastNonVarField; }

protected:
	virtual void dropEvent(QDropEvent* de) override;
	virtual void dragMoveEvent(QDragMoveEvent* dme) override;

	Fractorium* m_Fractorium = nullptr;
	QString m_LastNonVarField = "Direct color";//It is critical to update this if any more fields are ever added before the variations start.
};
