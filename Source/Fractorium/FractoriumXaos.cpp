#include "FractoriumPch.h"
#include "Fractorium.h"

#define XAOS_PREC 6

/// <summary>
/// Initialize the xforms xaos UI.
/// </summary>
void Fractorium::InitXaosUI()
{
	int spinHeight = 20;
	ui.XaosTableView->verticalHeader()->setSectionsClickable(true);
	ui.XaosTableView->horizontalHeader()->setSectionsClickable(true);
	m_XaosSpinBox = new DoubleSpinBox(nullptr, spinHeight, 0.1);
	m_XaosSpinBox->DoubleClick(true);
	m_XaosSpinBox->DoubleClickZero(1);
	m_XaosSpinBox->DoubleClickNonZero(0);
	m_XaosSpinBox->setDecimals(XAOS_PREC);
	m_XaosSpinBox->setObjectName("XaosSpinBox");
	m_XaosTableModel = nullptr;
	m_XaosTableItemDelegate = new DoubleSpinBoxTableItemDelegate(m_XaosSpinBox, this);
	connect(m_XaosSpinBox, SIGNAL(valueChanged(double)), this, SLOT(OnXaosChanged(double)), Qt::QueuedConnection);
	connect(ui.ClearXaosButton, SIGNAL(clicked(bool)), this, SLOT(OnClearXaosButtonClicked(bool)), Qt::QueuedConnection);
	connect(ui.RandomXaosButton, SIGNAL(clicked(bool)), this, SLOT(OnRandomXaosButtonClicked(bool)), Qt::QueuedConnection);
	connect(ui.XaosTableView->verticalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(OnXaosRowDoubleClicked(int)), Qt::QueuedConnection);
	connect(ui.XaosTableView->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(OnXaosColDoubleClicked(int)), Qt::QueuedConnection);
}

/// <summary>
/// Fill the xaos table with the values from the ember.
/// </summary>
template <typename T>
void FractoriumEmberController<T>::FillXaos()
{
	for (int i = 0, count = int(XformCount()); i < count; i++)//Column.
	{
		if (auto xform = m_Ember.GetXform(i))
		{
			for (int j = 0; j < count; j++)//Row.
			{
				QModelIndex index = m_Fractorium->m_XaosTableModel->index(j, i, QModelIndex());//j and i are intentionally swapped here.
				m_Fractorium->m_XaosTableModel->setData(index, xform->Xaos(j));
			}
		}
	}

	m_Fractorium->ui.XaosTableView->resizeRowsToContents();
	m_Fractorium->ui.XaosTableView->resizeColumnsToContents();
}

/// <summary>
/// Set the xaos value.
/// Called when any xaos spinner is changed.
/// It actually gets called multiple times as the user clicks around the
/// xaos table due to how QTableView passes events to and from its model.
/// To filter out spurrious events, the value is checked against the existing
/// xaos value.
/// Resets the rendering process.
/// </summary>
/// <param name="x">The index of the xform whose xaos value was changed (column)</param>
/// <param name="y">The index of the to value that was changed for the xform (row)</param>
/// <param name="val">The changed value of the xaos element</param>
template <typename T>
void FractoriumEmberController<T>::XaosChanged(int x, int y, double val)
{
	auto newVal = TruncPrecision(val, XAOS_PREC);//Sometimes 0 comes in as a very small number, so round.

	if (auto xform = m_Ember.GetXform(x))
		if (!IsClose<T>(newVal, xform->Xaos(y), T(1e-7)))
			Update([&] { xform->SetXaos(y, newVal); });
}

void Fractorium::OnXaosChanged(double d)
{
	if (auto senderSpinBox = qobject_cast<DoubleSpinBox*>(sender()))
	{
		auto p = senderSpinBox->property("tableindex").toPoint();
		m_Controller->XaosChanged(p.y(), p.x(), d);//Intentionally switched, column is the from xform, row is the to xform.
	}
}

void Fractorium::OnXaosTableModelDataChanged(const QModelIndex& indexA, const QModelIndex& indexB)
{
	m_Controller->XaosChanged(indexA.column(), indexA.row(), indexA.data().toDouble());//Intentionally switched, column is the from xform, row is the to xform.
}

/// <summary>
/// Clear xaos table, recreate all spinners based on the xaos used in the current ember.
/// </summary>
void Fractorium::FillXaosTable()
{
	int count = int(m_Controller->XformCount());
	QStringList hl, vl;
	auto oldModel = m_XaosTableModel;
	hl.reserve(count);
	vl.reserve(count);
	m_XaosTableModel = new QStandardItemModel(count, count, this);
	connect(m_XaosTableModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), SLOT(OnXaosTableModelDataChanged(QModelIndex, QModelIndex)));
	ui.XaosTableView->blockSignals(true);

	for (int i = 0; i < count; i++)
	{
		auto s = QString::number(i + 1);
		hl.push_back("F" + s);
		vl.push_back("T" + s);
	}

	m_XaosTableModel->setHorizontalHeaderLabels(hl);
	m_XaosTableModel->setVerticalHeaderLabels(vl);
	ui.XaosTableView->setModel(m_XaosTableModel);
	ui.XaosTableView->setItemDelegate(m_XaosTableItemDelegate);
	SetTabOrder(this, ui.ClearXaosButton, ui.RandomXaosButton);
	m_Controller->FillXaos();
	ui.XaosTableView->blockSignals(false);

	if (oldModel)
		delete oldModel;

	//Needed to get the dark stylesheet to correctly color the top left corner button.
	auto widgetList = ui.XaosTableView->findChildren<QAbstractButton*>();

	for (auto& it : widgetList)
		it->setEnabled(true);
}

/// <summary>
/// Clear all xaos from the current ember.
/// </summary>
template <typename T>
void FractoriumEmberController<T>::ClearXaos()
{
	Update([&] { m_Ember.ClearXaos(); });
	FillXaos();
}

void Fractorium::OnClearXaosButtonClicked(bool checked) { m_Controller->ClearXaos(); }

/// <summary>
/// Set all xaos values to random numbers.
/// There is a 50% chance they're set to 0 or 1, and
/// 50% that they're 0-3.
/// Resets the rendering process.
/// </summary>
template <typename T>
void FractoriumEmberController<T>::RandomXaos()
{
	Update([&]
	{
		for (size_t i = 0; i < m_Ember.XformCount(); i++)
		{
			if (auto xform = m_Ember.GetXform(i))
			{
				for (size_t j = 0; j < m_Ember.XformCount(); j++)
				{
					if (m_Rand.RandBit())
						xform->SetXaos(j, T(m_Rand.RandBit()));
					else
						xform->SetXaos(j, m_Rand.Frand<T>(0, 3));
				}
			}
		}
	});
	FillXaos();
}

void Fractorium::OnRandomXaosButtonClicked(bool checked) { m_Controller->RandomXaos(); }

/// <summary>
/// Toggle all xaos values in one row.
/// Resets the rendering process.
/// </summary>
/// <param name="logicalIndex">The index of the row that was double clicked</param>
void Fractorium::OnXaosRowDoubleClicked(int logicalIndex)
{
	ToggleTableRow(ui.XaosTableView, logicalIndex);
	ui.XaosTableView->resizeRowsToContents();
	ui.XaosTableView->resizeColumnsToContents();
}

/// <summary>
/// Toggle all xaos values in one column.
/// Resets the rendering process.
/// </summary>
/// <param name="logicalIndex">The index of the column that was double clicked</param>
void Fractorium::OnXaosColDoubleClicked(int logicalIndex)
{
	ToggleTableCol(ui.XaosTableView, logicalIndex);
	ui.XaosTableView->resizeRowsToContents();
	ui.XaosTableView->resizeColumnsToContents();
}

template class FractoriumEmberController<float>;

#ifdef DO_DOUBLE
	template class FractoriumEmberController<double>;
#endif
