#include "FractoriumPch.h"
#include "LibraryTreeWidget.h"
#include "Fractorium.h"

/// <summary>
/// Set a pointer to the main window.
/// </summary>
/// <param name="f">Pointer to the main Fractorium object</param>
void LibraryTreeWidget::SetMainWindow(Fractorium* f)
{
	m_Fractorium = f;
}

/// <summary>
/// Process the drop event to allow for moving items around inside of the tree.
/// </summary>
/// <param name="de">Pointer to the QDropEvent object</param>
void LibraryTreeWidget::dropEvent(QDropEvent* de)
{
	QModelIndex droppedIndex = indexAt(de->pos());
	auto items = selectionModel()->selectedIndexes();

	if (!droppedIndex.isValid())//Don't process drop because it's outside of the droppable area.
	{
		de->ignore();
		return;
	}
	else if (!items.empty())//Actually do the drop and move the item to a new location.
	{
		int row = droppedIndex.row();
		DropIndicatorPosition dp = dropIndicatorPosition();

		if (dp == QAbstractItemView::BelowItem)
			row++;

		m_Fractorium->m_Controller->MoveLibraryItems(items[0].row(), row);
	}

	QTreeWidget::dropEvent(de);
}