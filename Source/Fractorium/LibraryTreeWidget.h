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