#include "FractoriumPch.h"
#include "Fractorium.h"

/// <summary>
/// Initialize the library tree UI.
/// </summary>
void Fractorium::InitLibraryUI()
{
	ui.LibraryTree->SetMainWindow(this);
	//Making the TreeItemChanged() events use a direct connection is absolutely critical.
	connect(ui.LibraryTree,  SIGNAL(itemChanged(QTreeWidgetItem*, int)),	   this, SLOT(OnEmberTreeItemChanged(QTreeWidgetItem*, int)),	    Qt::DirectConnection);
	connect(ui.LibraryTree,  SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(OnEmberTreeItemDoubleClicked(QTreeWidgetItem*, int)), Qt::QueuedConnection);
	connect(ui.LibraryTree,  SIGNAL(itemActivated(QTreeWidgetItem*, int)),	   this, SLOT(OnEmberTreeItemDoubleClicked(QTreeWidgetItem*, int)), Qt::QueuedConnection);
	connect(ui.SequenceTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)),	   this, SLOT(OnSequenceTreeItemChanged(QTreeWidgetItem*, int)),	Qt::DirectConnection);
	connect(ui.SequenceStartPreviewsButton, SIGNAL(clicked(bool)), this, SLOT(OnSequenceStartPreviewsButtonClicked(bool)), Qt::QueuedConnection);
	connect(ui.SequenceStopPreviewsButton,  SIGNAL(clicked(bool)), this, SLOT(OnSequenceStopPreviewsButtonClicked(bool)),  Qt::QueuedConnection);
	connect(ui.SequenceAllButton,           SIGNAL(clicked(bool)), this, SLOT(OnSequenceAllButtonClicked(bool)),           Qt::QueuedConnection);
	connect(ui.SequenceGenerateButton,      SIGNAL(clicked(bool)), this, SLOT(OnSequenceGenerateButtonClicked(bool)),      Qt::QueuedConnection);
	connect(ui.SequenceRenderButton,        SIGNAL(clicked(bool)), this, SLOT(OnSequenceRenderButtonClicked(bool)),        Qt::QueuedConnection);
	connect(ui.SequenceSaveButton,          SIGNAL(clicked(bool)), this, SLOT(OnSequenceSaveButtonClicked(bool)),          Qt::QueuedConnection);
	connect(ui.SequenceOpenButton,          SIGNAL(clicked(bool)), this, SLOT(OnSequenceOpenButtonClicked(bool)),          Qt::QueuedConnection);
	connect(ui.SequenceRandomizeStaggerCheckBox,           SIGNAL(stateChanged(int)),    this, SLOT(OnSequenceRandomizeStaggerCheckBoxStateChanged(int)),           Qt::QueuedConnection);
	connect(ui.SequenceRandomizeFramesPerRotCheckBox,      SIGNAL(stateChanged(int)),    this, SLOT(OnSequenceRandomizeFramesPerRotCheckBoxStateChanged(int)),      Qt::QueuedConnection);
	connect(ui.SequenceRandomizeRotationsCheckBox,         SIGNAL(stateChanged(int)),    this, SLOT(OnSequenceRandomizeRotationsCheckBoxStateChanged(int)),         Qt::QueuedConnection);
	connect(ui.SequenceRandomizeBlendFramesCheckBox,       SIGNAL(stateChanged(int)),    this, SLOT(OnSequenceRandomizeBlendFramesCheckBoxStateChanged(int)),       Qt::QueuedConnection);
	connect(ui.SequenceRandomizeRotationsPerBlendCheckBox, SIGNAL(stateChanged(int)),    this, SLOT(OnSequenceRandomizeRotationsPerBlendCheckBoxStateChanged(int)), Qt::QueuedConnection);
	connect(ui.SequenceStaggerSpinBox,                     SIGNAL(valueChanged(double)), this, SLOT(OnSequenceStaggerSpinBoxChanged(double)),                       Qt::QueuedConnection);
	connect(ui.SequenceRandomStaggerMaxSpinBox,            SIGNAL(valueChanged(double)), this, SLOT(OnSequenceRandomStaggerMaxSpinBoxChanged(double)),              Qt::QueuedConnection);
	connect(ui.SequenceStartFlameSpinBox,                  SIGNAL(valueChanged(int)),    this, SLOT(OnSequenceStartFlameSpinBoxChanged(int)),                       Qt::QueuedConnection);
	connect(ui.SequenceStopFlameSpinBox,                   SIGNAL(valueChanged(int)),    this, SLOT(OnSequenceStopFlameSpinBoxChanged(int)),                        Qt::QueuedConnection);
	connect(ui.SequenceFramesPerRotSpinBox,                SIGNAL(valueChanged(int)),    this, SLOT(OnSequenceFramesPerRotSpinBoxChanged(int)),                     Qt::QueuedConnection);
	connect(ui.SequenceRandomFramesPerRotMaxSpinBox,       SIGNAL(valueChanged(int)),    this, SLOT(OnSequenceRandomFramesPerRotMaxSpinBoxChanged(int)),            Qt::QueuedConnection);
	connect(ui.SequenceRotationsSpinBox,                   SIGNAL(valueChanged(double)), this, SLOT(OnSequenceRotationsSpinBoxChanged(double)),                     Qt::QueuedConnection);
	connect(ui.SequenceRandomRotationsMaxSpinBox,          SIGNAL(valueChanged(double)), this, SLOT(OnSequenceRandomRotationsMaxSpinBoxChanged(double)),            Qt::QueuedConnection);
	connect(ui.SequenceBlendFramesSpinBox,                 SIGNAL(valueChanged(int)),    this, SLOT(OnSequenceBlendFramesSpinBoxChanged(int)),                      Qt::QueuedConnection);
	connect(ui.SequenceRandomBlendMaxFramesSpinBox,        SIGNAL(valueChanged(int)),    this, SLOT(OnSequenceRandomBlendMaxFramesSpinBoxChanged(int)),             Qt::QueuedConnection);
	connect(ui.SequenceRotationsPerBlendSpinBox,           SIGNAL(valueChanged(int)),    this, SLOT(OnSequenceRandomRotationsPerBlendSpinBoxChanged(int)),          Qt::QueuedConnection);
	connect(ui.SequenceRotationsPerBlendMaxSpinBox,        SIGNAL(valueChanged(int)),    this, SLOT(OnSequenceRandomRotationsPerBlendMaxSpinBoxChanged(int)),       Qt::QueuedConnection);
	//Stagger
	ui.SequenceStaggerSpinBox->setValue(m_Settings->Stagger());//Lower.
	ui.SequenceStaggerSpinBox->setMaximum(std::numeric_limits<int>::max());//Lower max = upper.
	ui.SequenceRandomStaggerMaxSpinBox->setValue(m_Settings->StaggerMax());//Upper.
	ui.SequenceRandomStaggerMaxSpinBox->setMinimum(m_Settings->Stagger());//Upper min = lower max.
	//Frames per rotation.
	ui.SequenceFramesPerRotSpinBox->setValue(m_Settings->FramesPerRot());//Lower.
	ui.SequenceFramesPerRotSpinBox->setMaximum(std::numeric_limits<int>::max());//Lower max = upper.
	ui.SequenceRandomFramesPerRotMaxSpinBox->setValue(m_Settings->FramesPerRotMax());//Upper.
	ui.SequenceRandomFramesPerRotMaxSpinBox->setMinimum(m_Settings->FramesPerRot());//Upper min = lower max.
	//Rotations.
	ui.SequenceRotationsSpinBox->setValue(m_Settings->Rotations());//Lower.
	ui.SequenceRotationsSpinBox->setMaximum(std::numeric_limits<int>::max());//Lower max = upper.
	ui.SequenceRandomRotationsMaxSpinBox->setValue(m_Settings->RotationsMax());//Upper.
	ui.SequenceRandomRotationsMaxSpinBox->setMinimum(m_Settings->Rotations());//Upper min = lower max.
	//Blend frames.
	ui.SequenceBlendFramesSpinBox->setValue(m_Settings->BlendFrames());//Lower.
	ui.SequenceBlendFramesSpinBox->setMaximum(std::numeric_limits<int>::max());//Lower max = upper.
	ui.SequenceRandomBlendMaxFramesSpinBox->setValue(m_Settings->BlendFramesMax());//Upper.
	ui.SequenceRandomBlendMaxFramesSpinBox->setMinimum(m_Settings->BlendFrames());//Upper min = lower max.
	//Rotations per blend.
	ui.SequenceRotationsPerBlendSpinBox->setValue(m_Settings->RotationsPerBlend());//Lower.
	ui.SequenceRotationsPerBlendSpinBox->setMaximum(std::numeric_limits<int>::max());//Lower max = upper.
	ui.SequenceRotationsPerBlendMaxSpinBox->setValue(m_Settings->RotationsPerBlendMax());//Upper.
	ui.SequenceRotationsPerBlendMaxSpinBox->setMinimum(m_Settings->RotationsPerBlend());//Upper min = lower max.
}

/// <summary>
/// Select the item in the library tree specified by the passed in index.
/// </summary>
/// <param name="index">The 0-based index of the item in the library tree to select</param>
void Fractorium::SelectLibraryItem(size_t index)
{
	if (const auto top = ui.LibraryTree->topLevelItem(0))
	{
		for (int i = 0; i < top->childCount(); i++)
		{
			if (auto emberItem = dynamic_cast<EmberTreeWidgetItemBase*>(top->child(i)))
			{
				emberItem->setSelected(i == index);
				emberItem->setCheckState(0, i == index ? Qt::Checked : Qt::Unchecked);
			}
		}
	}
}

/// <summary>
/// Get the index of the currently selected ember in the library tree.
/// </summary>
/// <param name="isChecked">Whether to search for items that are checked or items that are only selected</param>
/// <returns>A pair containing the index of the item clicked and a pointer to the item</param>
vector<pair<size_t, QTreeWidgetItem*>> Fractorium::GetCurrentEmberIndex(bool isChecked)
{
	int index = 0;
	QTreeWidgetItem* item = nullptr;
	const auto tree = ui.LibraryTree;
	vector<pair<size_t, QTreeWidgetItem*>> v;

	if (const auto top = tree->topLevelItem(0))
	{
		for (int i = 0; i < top->childCount(); i++)//Iterate through all of the children, which will represent the open embers.
		{
			item = top->child(index);

			if (item && item->isSelected())
			{
				if (isChecked)
				{
					if (item->checkState(0) == Qt::Checked)
						v.push_back(make_pair(index, item));
				}
				else
					v.push_back(make_pair(index, item));
			}

			index++;
		}
	}

	return v;
}

/// <summary>
/// Slot function to be called via QMetaObject::invokeMethod() to update preview images in the preview thread.
/// </summary>
/// <param name="item">The item double clicked on</param>
/// <param name="v">The vector holding the RGBA bitmap</param>
/// <param name="w">The width of the bitmap</param>
/// <param name="h">The height of the bitmap</param>
void Fractorium::SetLibraryTreeItemData(EmberTreeWidgetItemBase* item, vv4F& v, uint w, uint h)
{
	m_PreviewVec.resize(size_t(w) * size_t(h) * 4);
	Rgba32ToRgba8(v.data(), m_PreviewVec.data(), w, h, m_Settings->Transparency());
	item->SetImage(m_PreviewVec, w, h);
}

/// <summary>
/// Set all libary tree entries to the name of the corresponding ember they represent.
/// Set all libary tree entries to point to the underlying ember they represent.
/// </summary>
/// <param name="update">A bitfield representing the type of synchronizing to do. Update one or more of index, name or pointer.</param>
template <typename T>
void FractoriumEmberController<T>::SyncLibrary(eLibraryUpdate update)
{
	auto it = m_EmberFile.m_Embers.begin();
	const auto tree = m_Fractorium->ui.LibraryTree;

	if (const auto top = tree->topLevelItem(0))
	{
		for (int i = 0; i < top->childCount() && it != m_EmberFile.m_Embers.end(); ++i, ++it)//Iterate through all of the children, which will represent the open embers.
		{
			if (auto item = dynamic_cast<EmberTreeWidgetItem<T>*>(top->child(i)))//Cast the child widget to the EmberTreeWidgetItem type.
			{
				if (update & eLibraryUpdate::INDEX)
					it->m_Index = i;

				if (update & eLibraryUpdate::NAME)
					item->setText(0, QString::fromStdString(it->m_Name));

				if (update & eLibraryUpdate::POINTER)
					item->SetEmberPointer(&(*it));

				if (item->checkState(0) == Qt::Checked)
					m_EmberFilePointer = item->GetEmber();
			}
		}
	}
}

/// <summary>
/// Fill the library tree with the names of the embers in the
/// currently opened file.
/// Start preview render thread.
/// </summary>
/// <param name="selectIndex">After the tree is filled, select this index. Pass -1 to omit selecting an index.</param>
template <typename T>
void FractoriumEmberController<T>::FillLibraryTree(int selectIndex)
{
	uint size = 64;
	uint i = 0;
	const auto tree = m_Fractorium->ui.LibraryTree;
	vector<byte> v(size * size * 4);
	StopAllPreviewRenderers();
	tree->clear();
	auto fileItem = new QTreeWidgetItem(tree);
	QFileInfo info(m_EmberFile.m_Filename);
	fileItem->setText(0, info.fileName());
	fileItem->setToolTip(0, m_EmberFile.m_Filename);
	fileItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled);

	for (auto& it : m_EmberFile.m_Embers)
	{
		auto emberItem = new EmberTreeWidgetItem<T>(&it, fileItem);

		if (it.m_Name.empty())
			emberItem->setText(0, ToString(i++));
		else
			emberItem->setText(0, it.m_Name.c_str());

		emberItem->setToolTip(0, emberItem->text(0));
		emberItem->SetImage(v, size, size);
	}

	if (selectIndex != -1)
		m_Fractorium->SelectLibraryItem(selectIndex);

	m_Fractorium->SyncFileCountToSequenceCount();
	RenderLibraryPreviews(0, uint(m_EmberFile.Size()));
	tree->expandAll();
}

/// <summary>
/// Update the library tree with the newly added embers (most likely from pasting) and
/// only render previews for the new ones, without clearing the entire tree.
/// </summary>
template <typename T>
void FractoriumEmberController<T>::UpdateLibraryTree()
{
	const uint size = 64;
	vector<byte> v(size * size * 4);
	const auto tree = m_Fractorium->ui.LibraryTree;

	if (auto top = tree->topLevelItem(0))
	{
		const int origChildCount = top->childCount();
		int i = origChildCount;

		for (auto it = Advance(m_EmberFile.m_Embers.begin(), i); it != m_EmberFile.m_Embers.end(); ++it)
		{
			auto emberItem = new EmberTreeWidgetItem<T>(&(*it), top);

			if (it->m_Name.empty())
				emberItem->setText(0, ToString(i++));
			else
				emberItem->setText(0, it->m_Name.c_str());

			emberItem->setToolTip(0, emberItem->text(0));
			emberItem->SetImage(v, size, size);
		}

		//When adding elements, ensure all indices are sequential.
		SyncLibrary(eLibraryUpdate::INDEX);
		m_Fractorium->SyncFileCountToSequenceCount();
		RenderLibraryPreviews(origChildCount, uint(m_EmberFile.Size()));
	}
}

/// <summary>
/// Copy the text of the item which was changed to the name of the current ember.
/// Ensure all names are unique in the opened file.
/// This seems to be called spuriously, so we do a check inside to make sure
/// the text was actually changed.
/// We also have to wrap the dynamic_cast call in a try/catch block  because this can
/// be called on a widget that has already been deleted.
/// </summary>
/// <param name="item">The libary tree item changed</param>
/// <param name="col">The column clicked, ignored.</param>
template <typename T>
void FractoriumEmberController<T>::EmberTreeItemChanged(QTreeWidgetItem* item, int col)
{
	try
	{
		const auto tree = m_Fractorium->ui.LibraryTree;

		if (auto emberItem = dynamic_cast<EmberTreeWidgetItem<T>*>(item))
		{
			auto oldName = emberItem->GetEmber()->m_Name;//First preserve the previous name.
			auto newName = emberItem->text(0).toStdString();

			//Checking/unchecking other items shouldn't perform the processing below.
			//If nothing changed, nothing to do.
			if (!emberItem->isSelected() && newName == oldName)
				return;

			if (newName.empty())//Prevent empty string.
			{
				emberItem->UpdateEditText();
				return;
			}

			emberItem->UpdateEmberName();//Copy edit text to the ember's name variable.
			m_EmberFile.MakeNamesUnique();//Ensure all names remain unique.
			SyncLibrary(eLibraryUpdate::NAME);//Copy all ember names to the tree items since some might have changed to be made unique.
			newName = emberItem->GetEmber()->m_Name;//Get the new, final, unique name.

			if (m_EmberFilePointer && m_EmberFilePointer == emberItem->GetEmber() && oldName != newName)//If the ember edited was the current one, and the name was indeed changed, update the name of the current one.
			{
				m_Ember.m_Name = newName;
				m_LastSaveCurrent = "";//Reset will force the dialog to show on the next save current since the user probably wants a different name.
			}
		}
		else if (auto parentItem = dynamic_cast<QTreeWidgetItem*>(item))
		{
			auto text = parentItem->text(0);

			if (text != "")
			{
				m_EmberFile.m_Filename = text;
				m_LastSaveAll = "";//Reset will force the dialog to show on the next save all since the user probably wants a different name.
			}
		}
	}
	catch (const std::exception& e)
	{
		qDebug() << "FractoriumEmberController<T>::EmberTreeItemChanged() : Exception thrown: " << e.what();
	}
}

void Fractorium::OnEmberTreeItemChanged(QTreeWidgetItem* item, int col)
{
	if (item && ui.LibraryTree->topLevelItemCount())//This can sometimes be spurriously called even when the tree is empty.
		m_Controller->EmberTreeItemChanged(item, col);
}

/// <summary>
/// Set the current ember to the selected item.
/// Clears the undo state.
/// Resets the rendering process.
/// Called when the user double clicks on a library tree item.
/// This will get called twice for some reason, so the check state is checked to prevent duplicate processing.
/// </summary>
/// <param name="item">The item double clicked on</param>
/// <param name="col">The column clicked</param>
template <typename T>
void FractoriumEmberController<T>::EmberTreeItemDoubleClicked(QTreeWidgetItem* item, int col)
{
	if (item->checkState(col) == Qt::Unchecked)
		SetEmber(m_Fractorium->ui.LibraryTree->currentIndex().row(), false);
}

void Fractorium::OnEmberTreeItemDoubleClicked(QTreeWidgetItem* item, int col)
{
	if (ui.LibraryTree->topLevelItemCount())//This can sometimes be spurriously called even when the tree is empty.
		m_Controller->EmberTreeItemDoubleClicked(item, col);
}

/// <summary>
/// Move a possibly disjoint selection of library items from their indices,
/// to another index.
/// </summary>
/// <param name="items">The selected list of items to move</param>
/// <param name="destRow">The destination index to move the item to</param>
template <typename T>
void FractoriumEmberController<T>::MoveLibraryItems(const QModelIndexList& items, int destRow)
{
	int i = 0;
	const auto startRow = items[0].row();
	const auto tree = m_Fractorium->ui.LibraryTree;
	const auto top = tree->topLevelItem(0);
	list<string> names;

	for (auto& item : items)
		if (auto temp = m_EmberFile.Get(item.row()))
			names.push_back(temp->m_Name);

	auto b = m_EmberFile.m_Embers.begin();
	const auto result = Gather(b, m_EmberFile.m_Embers.end(), Advance(b, destRow), [&](const Ember<T>& ember)
	{
		auto position = std::find(names.begin(), names.end(), ember.m_Name);

		if (position != names.end())
		{
			names.erase(position);
			return true;
		}

		return false;
	});
	tree->update();
	SyncLibrary(eLibraryUpdate(eLibraryUpdate::INDEX | eLibraryUpdate::POINTER));
	//SyncLibrary(eLibraryUpdate(eLibraryUpdate::INDEX | eLibraryUpdate::POINTER | eLibraryUpdate::NAME));
}

/// <summary>
/// Delete the currently selected items in the tree.
/// Note this is not necessarilly the current ember, it's just the items
/// in the tree that are selected.
/// </summary>
/// <param name="v">A vector of pairs, each containing the index of the item selected and a pointer to the item</param>
template <typename T>
void FractoriumEmberController<T>::Delete(const vector<pair<size_t, QTreeWidgetItem*>>& v)
{
	size_t offset = 0;
	uint last = 0;

	for (auto& p : v)
	{
		if (p.second && m_EmberFile.Delete(p.first - offset))
		{
			last = uint(p.first - offset);
			delete p.second;
			SyncLibrary(eLibraryUpdate::INDEX);
			m_Fractorium->SyncFileCountToSequenceCount();
		}

		offset++;
	}

	//Select the next item in the tree closest to the last one that was deleted.
	if (const auto top = m_Fractorium->ui.LibraryTree->topLevelItem(0))
	{
		last = std::min<uint>(top->childCount() - 1, last);

		if (auto item = dynamic_cast<EmberTreeWidgetItem<T>*>(top->child(last)))
			if (item->GetEmber()->m_Name != m_Ember.m_Name)
				EmberTreeItemDoubleClicked(item, 0);
	}
}

/// <summary>
/// Called when the user presses and releases the delete key while the library tree has the focus,
/// and an item is selected.
/// </summary>
/// <param name="v">A vector of pairs, each containing the index of the item selected and a pointer to the item</param>
void Fractorium::OnDelete(const vector<pair<size_t, QTreeWidgetItem*>>& v)
{
	m_Controller->Delete(v);
}

/// <summary>
/// Stop the preview renderer if it's already running.
/// Clear all of the existing preview images, then start the preview rendering thread.
/// Optionally only render previews for a subset of all open embers.
/// </summary>
/// <param name="start">The 0-based index to start rendering previews for</param>
/// <param name="end">The 0-based index which is one beyond the last ember to render a preview for</param>
template <typename T>
void FractoriumEmberController<T>::RenderPreviews(QTreeWidget* tree, TreePreviewRenderer<T>* renderer, EmberFile<T>& file, uint start, uint end)
{
	renderer->Stop();

	if (start == UINT_MAX && end == UINT_MAX)
	{
		if (const auto top = tree->topLevelItem(0))
		{
			const auto childCount = top->childCount();
			vector<byte> emptyPreview(PREVIEW_SIZE * PREVIEW_SIZE * 4);

			for (int i = 0; i < childCount; i++)
				if (auto treeItem = dynamic_cast<EmberTreeWidgetItemBase*>(top->child(i)))
					treeItem->SetImage(emptyPreview, PREVIEW_SIZE, PREVIEW_SIZE);
		}

		renderer->Render(0, uint(file.Size()));
	}
	else
		renderer->Render(start, end);
}

/// <summary>
/// Wrapper around calling RenderPreviews with the appropriate values passed in for the previews in the main library tree.
/// </summary>
/// <param name="start">The 0-based index to start rendering previews for</param>
/// <param name="end">The 0-based index which is one beyond the last ember to render a preview for</param>
template <typename T>
void FractoriumEmberController<T>::RenderLibraryPreviews(uint start, uint end)
{
	RenderPreviews(m_Fractorium->ui.LibraryTree, m_LibraryPreviewRenderer.get(), m_EmberFile, start, end);
}

template <typename T>
void FractoriumEmberController<T>::StopLibraryPreviewRender() { m_LibraryPreviewRenderer->Stop(); }

/// <summary>
/// Thing wrapper around StopLibraryPreviewRender() and StopSequencePreviewRender() to stop both preview renderers.
/// </summary>
template <typename T>
void FractoriumEmberController<T>::StopAllPreviewRenderers()
{
	StopLibraryPreviewRender();
	StopSequencePreviewRender();
}

/// <summary>
/// Fill the sequence tree with the names of the embers in the
/// currently generated sequence.
/// Start the sequence preview render thread.
/// </summary>
template <typename T>
void FractoriumEmberController<T>::FillSequenceTree()
{
	const uint size = 64;
	uint i = 0;
	const auto tree = m_Fractorium->ui.SequenceTree;
	vector<byte> v(size * size * 4);
	m_SequencePreviewRenderer->Stop();
	tree->clear();
	auto fileItem = new QTreeWidgetItem(tree);
	QFileInfo info(m_SequenceFile.m_Filename);
	fileItem->setText(0, info.fileName());
	fileItem->setToolTip(0, m_SequenceFile.m_Filename);
	fileItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);

	for (auto& it : m_SequenceFile.m_Embers)
	{
		auto emberItem = new EmberTreeWidgetItemBase(fileItem);

		if (it.m_Name.empty())
			emberItem->setText(0, ToString(i));
		else
			emberItem->setText(0, it.m_Name.c_str());

		i++;
		emberItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		emberItem->setToolTip(0, emberItem->text(0));
		emberItem->SetImage(v, size, size);
	}

	tree->expandAll();
	RenderSequencePreviews(0, uint(m_SequenceFile.Size()));
}

/// <summary>
/// Copy the text of the root item to the name of the sequence file.
/// Called whenever the text of the root item is changed.
/// </summary>
/// <param name="item">The root sequence tree item which changed</param>
/// <param name="col">The column clicked, ignored.</param>
template <typename T>
void FractoriumEmberController<T>::SequenceTreeItemChanged(QTreeWidgetItem* item, int col)
{
	if (item == m_Fractorium->ui.SequenceTree->topLevelItem(0))
	{
		auto text = item->text(0);

		if (text != "")
			m_SequenceFile.m_Filename = text;
	}
}

void Fractorium::OnSequenceTreeItemChanged(QTreeWidgetItem* item, int col)
{
	if (item && ui.SequenceTree->topLevelItemCount())
		m_Controller->SequenceTreeItemChanged(item, col);
}

/// <summary>
/// Wrapper around calling RenderPreviews with the appropriate values passed in for the previews in the sequence tree.
/// Called when Render Previews is clicked.
/// </summary>
/// <param name="start">Ignored, render all.</param>
/// <param name="end">Ignored, render all.</param>
template <typename T>
void FractoriumEmberController<T>::RenderSequencePreviews(uint start, uint end) { RenderPreviews(m_Fractorium->ui.SequenceTree, m_SequencePreviewRenderer.get(), m_SequenceFile, start, end); }
void Fractorium::OnSequenceStartPreviewsButtonClicked(bool checked) { m_Controller->RenderSequencePreviews(); }

/// <summary>
/// Stop rendering the sequence previews.
/// Called when Stop Previews is clicked.
/// </summary>
template <typename T>
void FractoriumEmberController<T>::StopSequencePreviewRender() { m_SequencePreviewRenderer->Stop(); }
void Fractorium::OnSequenceStopPreviewsButtonClicked(bool checked) { m_Controller->StopSequencePreviewRender(); }

/// <summary>
/// Set the start and stop spin boxes to 0 and the length of the ember file minus 1, respectively.
/// Called whenever the count of the current file changes or when All is clicked.
/// </summary>
void Fractorium::SyncFileCountToSequenceCount()
{
	if (const auto top = ui.LibraryTree->topLevelItem(0))
	{
		const int count = top->childCount() - 1;
		ui.LibraryTree->headerItem()->setText(0, "Current Flame File (" + QString::number(top->childCount()) + ")");
		ui.SequenceStartFlameSpinBox->setMinimum(0);
		ui.SequenceStartFlameSpinBox->setMaximum(count);
		ui.SequenceStartFlameSpinBox->setValue(0);
		ui.SequenceStopFlameSpinBox->setMinimum(0);
		ui.SequenceStopFlameSpinBox->setMaximum(count);
		ui.SequenceStopFlameSpinBox->setValue(count);
	}
}

void Fractorium::OnSequenceAllButtonClicked(bool checked) { SyncFileCountToSequenceCount(); }

/// <summary>
/// Generate an animation sequence and place it in the sequence tree. This code is
/// mostly similar to that of EmberGenome.
/// It differs in a few ways:
///		The number of frames used in a rotation and in blending can differ. In EmberGenome, they are the same.
///		The number of rotations, frames used in rotations and frames used in blending can all be randomized.
/// Called when the Generate button is clicked.
/// </summary>
template <typename T>
void FractoriumEmberController<T>::SequenceGenerateButtonClicked()
{
	StopAllPreviewRenderers();
	SaveCurrentToOpenedFile(false);
	Ember<T> result;
	auto& ui = m_Fractorium->ui;
	auto s = m_Fractorium->m_Settings;
	//Bools for determining whether to use hard coded vs. random values.
	const bool randStagger = ui.SequenceRandomizeStaggerCheckBox->isChecked();
	const bool randFramesRot = ui.SequenceRandomizeFramesPerRotCheckBox->isChecked();
	const bool randRot = ui.SequenceRandomizeRotationsCheckBox->isChecked();
	const bool randBlend = ui.SequenceRandomizeBlendFramesCheckBox->isChecked();
	const bool randBlendRot = ui.SequenceRandomizeRotationsPerBlendCheckBox->isChecked();
	//The direction to rotate the loops.
	const bool loopsCw = ui.SequenceRotationsCWCheckBox->isChecked();
	const bool loopsBlendCw = ui.SequenceRotationsPerBlendCWCheckBox->isChecked();
	//Whether to stagger, default is 1 which means no stagger.
	const double stagger = ui.SequenceStaggerSpinBox->value();
	const double staggerMax = ui.SequenceRandomStaggerMaxSpinBox->value();
	//Rotations on keyframes.
	const double rots = ui.SequenceRotationsSpinBox->value();
	const double rotsMax = ui.SequenceRandomRotationsMaxSpinBox->value();
	//Number of frames it takes to rotate a keyframe.
	const int framesPerRot = ui.SequenceFramesPerRotSpinBox->value();
	const int framesPerRotMax = ui.SequenceRandomFramesPerRotMaxSpinBox->value();
	//Number of frames it takes to interpolate.
	const int framesBlend = ui.SequenceBlendFramesSpinBox->value();
	const int framesBlendMax = ui.SequenceRandomBlendMaxFramesSpinBox->value();
	//Number of rotations performed during interpolation.
	const int rotsPerBlend = ui.SequenceRotationsPerBlendSpinBox->value();
	const int rotsPerBlendMax = ui.SequenceRotationsPerBlendMaxSpinBox->value();
	const size_t start = ui.SequenceStartFlameSpinBox->value();
	const size_t stop = ui.SequenceStopFlameSpinBox->value();
	const size_t startCount = ui.SequenceStartCountSpinBox->value();
	const size_t keyFrames = (stop - start) + 1;
	size_t frameCount = 0;
	double frames = 0;
	vector<pair<size_t, size_t>> devices;//Dummy.
	EmberReport emberReport;
	ostringstream os;
	string palettePath = FindFirstDefaultPalette().toStdString();

	if (palettePath.empty())//This should never happen because the program requires a palette file to run this far.
	{
		QMessageBox::warning(nullptr, "Sequence", "No flam3-palettes.xml file found, sequence will not be generated.");
		return;
	}

	if (!randRot && !randBlend)
	{
		if ((!rots || !framesPerRot) && !framesBlend)
		{
			QMessageBox::critical(m_Fractorium, "Animation sequence parameters error",
								  "Rotations and Frames per rot, or blend frames must be positive and non-zero");
			return;
		}

		if (framesPerRot > 1 && !rots)//Because framesPerRot control has a min value of 1, check greater than 1. Also don't need to check the inverse like in EmberGenome.
		{
			QMessageBox::critical(m_Fractorium, "Animation sequence parameters error",
								  "Frames per rot cannot be greater than one while Rotations is zero. Setting it to 1.");
			ui.SequenceFramesPerRotSpinBox->setValue(1);
			return;
		}
	}

	SheepTools<T, float> tools(palettePath, EmberCommon::CreateRenderer<T>(eRendererType::CPU_RENDERER, devices, false, 0, emberReport));
	tools.SetSpinParams(true,
						stagger,//Will be set again below if random is used.
						0,
						0,
						s->Nick().toStdString(),
						s->Url().toStdString(),
						s->Id().toStdString(),
						"",
						0,
						0);

	if (randFramesRot)
		frames = ui.SequenceRandomFramesPerRotMaxSpinBox->value();
	else
		frames = ui.SequenceFramesPerRotSpinBox->value();

	if (randRot)
		frames *= ui.SequenceRandomRotationsMaxSpinBox->value();
	else
		frames *= ui.SequenceRotationsSpinBox->value();

	if (randBlend)
		frames += ui.SequenceRandomBlendMaxFramesSpinBox->value();
	else
		frames += ui.SequenceBlendFramesSpinBox->value();

	frames *= keyFrames;
	frames += startCount;
	os << setfill('0') << setprecision(0) << fixed;
	m_SequenceFile.Clear();
	m_SequenceFile.m_Filename = EmberFile<T>::DefaultFilename("Sequence_");
	double blend;
	size_t frame;
	Ember<T> embers[2];//Spin needs contiguous array below, and this will also get modified, so a copy is needed to avoid modifying the embers in the original file.
	const auto padding = streamsize(std::log10(frames)) + 1;
	auto it = Advance(m_EmberFile.m_Embers.begin(), start);

	for (size_t i = start; i <= stop && it != m_EmberFile.m_Embers.end(); i++, ++it)
	{
		const auto rotations = randRot ? m_Rand.Frand<double>(rots, rotsMax) : rots;
		embers[0] = *it;

		if (rotations > 0)
		{
			const auto rotFrames = randFramesRot ? m_Rand.Frand<double>(framesPerRot, framesPerRotMax) : framesPerRot;
			const auto roundFrames = size_t(std::round(rotFrames * rotations));

			for (frame = 0; frame < roundFrames; frame++)
			{
				blend = frame / rotFrames;
				tools.Spin(embers[0], nullptr, result, startCount + frameCount++, blend, loopsCw);//Result is cleared and reassigned each time inside of Spin().
				FormatName(result, os, padding);
				m_SequenceFile.m_Embers.push_back(result);
			}

			//The loop above will have rotated just shy of a complete rotation.
			//Rotate the next step and save in result, but do not print.
			//result will be the starting point for the interp phase below.
			frame = roundFrames;
			blend = frame / rotFrames;
			tools.Spin(embers[0], nullptr, result, startCount + frameCount, blend, loopsCw);//Do not increment frameCount here.
			FormatName(result, os, padding);
		}

		if (i < stop)
		{
			if (rotations > 0)//Store the last result as the flame to interpolate from. This applies for whole or fractional values of opt.Loops().
				embers[0] = result;

			auto it2 = it;//Need a quick temporary to avoid modifying it which is used in the loop.
			embers[1] = *(++it2);//Get the next ember to be used with blending below.
			const auto blendFrames = randBlend ? m_Rand.Frand<double>(framesBlend, framesBlendMax) : framesBlend;
			const auto d = randBlendRot ? m_Rand.Frand<double>(rotsPerBlend, rotsPerBlendMax) : double(rotsPerBlend);
			const auto rpb = size_t(std::round(d));

			if (randStagger)
				tools.Stagger(m_Rand.Frand<double>(stagger, staggerMax));

			for (frame = 0; frame < blendFrames; frame++)
			{
				//if (frame == 43)
				//	cout << frame << endl;
				const auto seqFlag = frame == 0 || (frame == blendFrames - 1);
				blend = frame / double(blendFrames);
				result.Clear();
				tools.SpinInter(&embers[0], nullptr, result, startCount + frameCount++, seqFlag, blend, rpb, loopsBlendCw);
				FormatName(result, os, padding);
				m_SequenceFile.m_Embers.push_back(result);
			}
		}
	}

	it = Advance(m_EmberFile.m_Embers.begin(), stop);
	tools.Spin(*it, nullptr, result, startCount + frameCount, 0, loopsBlendCw);
	FormatName(result, os, padding);
	m_SequenceFile.m_Embers.push_back(result);
	FillSequenceTree();//The sequence has been generated, now create preview thumbnails.
}

void Fractorium::OnSequenceGenerateButtonClicked(bool checked) { m_Controller->SequenceGenerateButtonClicked(); }

/// <summary>
/// Show the final render dialog and load the sequence into it.
/// This will automatically check the Render All and Render as Animation sequence checkboxes.
/// Called when the Render Sequence button is clicked.
/// </summary>
/// <param name="checked">Ignored.</param>
void Fractorium::OnSequenceRenderButtonClicked(bool checked)
{
	if (ui.SequenceTree->topLevelItemCount() > 0)
	{
		//First completely stop what the current rendering process is doing.
		m_Controller->DeleteRenderer();//Delete the renderer, but not the controller.
		m_Controller->StopAllPreviewRenderers();
		m_Controller->SaveCurrentToOpenedFile(false);//Save whatever was edited back to the current open file.
		m_RenderStatusLabel->setText("Renderer stopped.");
		SetupFinalRenderDialog();

		if (m_FinalRenderDialog)
			m_FinalRenderDialog->Show(true);//Show with a bool specifying that it came from the sequence generator.
	}
}

/// <summary>
/// Save the sequence to a file.
/// </summary>
template <typename T>
void FractoriumEmberController<T>::SequenceSaveButtonClicked()
{
	auto s = m_Fractorium->m_Settings;
	QString filename = m_Fractorium->SetupSaveXmlDialog(m_SequenceFile.m_Filename);

	if (filename != "")
	{
		EmberToXml<T> writer;
		QFileInfo fileInfo(filename);

		for (auto& ember : m_SequenceFile.m_Embers)
			ApplyXmlSavingTemplate(ember);

		if (writer.Save(filename.toStdString().c_str(), m_SequenceFile.m_Embers, 0, true, true, false, false, false))
			s->SaveFolder(fileInfo.canonicalPath());
		else
			m_Fractorium->ShowCritical("Save Failed", "Could not save sequence file, try saving to a different folder.");
	}
}

void Fractorium::OnSequenceSaveButtonClicked(bool checked) { m_Controller->SequenceSaveButtonClicked(); }

/// <summary>
/// Open one or more sequence file, concatenate them all, place them in the sequence
/// tree and begin rendering the sequence previews.
/// </summary>
template <typename T>
void FractoriumEmberController<T>::SequenceOpenButtonClicked()
{
	m_SequencePreviewRenderer->Stop();
	auto filenames = m_Fractorium->SetupOpenXmlDialog();

	if (!filenames.empty())
	{
		size_t i;
		EmberFile<T> emberFile;
		XmlToEmber<T> parser;
		vector<Ember<T>> embers;
		vector<string> errors;
		emberFile.m_Filename = filenames[0];

		for (auto& filename : filenames)
		{
			embers.clear();

			if (parser.Parse(filename.toStdString().c_str(), embers, true) && !embers.empty())
			{
				for (i = 0; i < embers.size(); i++)
					if (embers[i].m_Name == "" || embers[i].m_Name == "No name")//Ensure it has a name.
						embers[i].m_Name = ToString<qulonglong>(i).toStdString();

				emberFile.m_Embers.insert(emberFile.m_Embers.end(), embers.begin(), embers.end());
				errors = parser.ErrorReport();
			}
			else
			{
				errors = parser.ErrorReport();
				m_Fractorium->ShowCritical("Open Failed", "Could not open sequence file, see info tab for details.");
			}

			if (!errors.empty())
				m_Fractorium->ErrorReportToQTextEdit(errors, m_Fractorium->ui.InfoFileOpeningTextEdit, false);//Concat errors from all files.
		}

		if (emberFile.Size() > 0)//Ensure at least something was read.
		{
			emberFile.MakeNamesUnique();
			m_SequenceFile = std::move(emberFile);//Move the temp to avoid creating dupes because we no longer need it.
			FillSequenceTree();
		}
	}
}

void Fractorium::OnSequenceOpenButtonClicked(bool checked) { m_Controller->SequenceOpenButtonClicked(); }

/// <summary>
/// Constrain all min/max spinboxes when the max spinboxes are enabled/disabled via the random checkbox.
/// </summary>
void Fractorium::OnSequenceRandomizeStaggerCheckBoxStateChanged(int state)
{
	ui.SequenceRandomStaggerMaxSpinBox->setMinimum(ui.SequenceStaggerSpinBox->value());
	ui.SequenceStaggerSpinBox->setMaximum(state ? ui.SequenceRandomStaggerMaxSpinBox->value() : std::numeric_limits<int>::max());
	ui.SequenceRandomStaggerMaxSpinBox->setEnabled(state);
}

void Fractorium::OnSequenceRandomizeFramesPerRotCheckBoxStateChanged(int state)
{
	ui.SequenceRandomFramesPerRotMaxSpinBox->setMinimum(ui.SequenceFramesPerRotSpinBox->value());
	ui.SequenceFramesPerRotSpinBox->setMaximum(state ? ui.SequenceRandomFramesPerRotMaxSpinBox->value() : std::numeric_limits<int>::max());
	ui.SequenceRandomFramesPerRotMaxSpinBox->setEnabled(state);
}

void Fractorium::OnSequenceRandomizeRotationsCheckBoxStateChanged(int state)
{
	ui.SequenceRandomRotationsMaxSpinBox->setMinimum(ui.SequenceRotationsSpinBox->value());
	ui.SequenceRotationsSpinBox->setMaximum(state ? ui.SequenceRandomRotationsMaxSpinBox->value() : std::numeric_limits<int>::max());
	ui.SequenceRandomRotationsMaxSpinBox->setEnabled(state);
}

void Fractorium::OnSequenceRandomizeBlendFramesCheckBoxStateChanged(int state)
{
	ui.SequenceRandomBlendMaxFramesSpinBox->setMinimum(ui.SequenceBlendFramesSpinBox->value());
	ui.SequenceBlendFramesSpinBox->setMaximum(state ? ui.SequenceRandomBlendMaxFramesSpinBox->value() : std::numeric_limits<int>::max());
	ui.SequenceRandomBlendMaxFramesSpinBox->setEnabled(state);
}

void Fractorium::OnSequenceRandomizeRotationsPerBlendCheckBoxStateChanged(int state)
{
	ui.SequenceRotationsPerBlendMaxSpinBox->setMinimum(ui.SequenceRotationsPerBlendSpinBox->value());
	ui.SequenceRotationsPerBlendSpinBox->setMaximum(state ? ui.SequenceRotationsPerBlendMaxSpinBox->value() : std::numeric_limits<int>::max());
	ui.SequenceRotationsPerBlendMaxSpinBox->setEnabled(state);
}

/// <summary>
/// Constrain all min/max spinboxes.
/// </summary>
void Fractorium::OnSequenceStaggerSpinBoxChanged(double d)                 { if (ui.SequenceRandomizeStaggerCheckBox->isChecked()) ui.SequenceRandomStaggerMaxSpinBox->setMinimum(d);               }
void Fractorium::OnSequenceRandomStaggerMaxSpinBoxChanged(double d)        { if (ui.SequenceRandomStaggerMaxSpinBox->hasFocus()) ui.SequenceStaggerSpinBox->setMaximum(d);                          }
void Fractorium::OnSequenceStartFlameSpinBoxChanged(int d)                 { ui.SequenceStopFlameSpinBox->setMinimum(d);                                                                            }
void Fractorium::OnSequenceStopFlameSpinBoxChanged(int d)                  { if (ui.SequenceStopFlameSpinBox->hasFocus()) ui.SequenceStartFlameSpinBox->setMaximum(d);                              }
void Fractorium::OnSequenceFramesPerRotSpinBoxChanged(int d)               { if (ui.SequenceRandomizeFramesPerRotCheckBox->isChecked()) ui.SequenceRandomFramesPerRotMaxSpinBox->setMinimum(d);     }
void Fractorium::OnSequenceRandomFramesPerRotMaxSpinBoxChanged(int d)      { if (ui.SequenceRandomFramesPerRotMaxSpinBox->hasFocus()) ui.SequenceFramesPerRotSpinBox->setMaximum(d);                }
void Fractorium::OnSequenceRotationsSpinBoxChanged(double d)               { if (ui.SequenceRandomizeRotationsCheckBox->isChecked()) ui.SequenceRandomRotationsMaxSpinBox->setMinimum(d);           }
void Fractorium::OnSequenceRandomRotationsMaxSpinBoxChanged(double d)      { if (ui.SequenceRandomRotationsMaxSpinBox->hasFocus()) ui.SequenceRotationsSpinBox->setMaximum(d);                      }
void Fractorium::OnSequenceBlendFramesSpinBoxChanged(int d)                { if (ui.SequenceRandomizeBlendFramesCheckBox->isChecked()) ui.SequenceRandomBlendMaxFramesSpinBox->setMinimum(d);       }
void Fractorium::OnSequenceRandomBlendMaxFramesSpinBoxChanged(int d)       { if (ui.SequenceRandomBlendMaxFramesSpinBox->hasFocus()) ui.SequenceBlendFramesSpinBox->setMaximum(d);                  }
void Fractorium::OnSequenceRandomRotationsPerBlendSpinBoxChanged(int d)    { if (ui.SequenceRandomizeRotationsPerBlendCheckBox->isChecked()) ui.SequenceRotationsPerBlendMaxSpinBox->setMinimum(d); }
void Fractorium::OnSequenceRandomRotationsPerBlendMaxSpinBoxChanged(int d) { if (ui.SequenceRotationsPerBlendMaxSpinBox->hasFocus()) ui.SequenceRotationsPerBlendSpinBox->setMaximum(d);            }

/// <summary>
/// Save all sequence settings to match the values in the controls.
/// </summary>
void Fractorium::SyncSequenceSettings()
{
	m_Settings->Stagger(ui.SequenceStaggerSpinBox->value());
	m_Settings->StaggerMax(ui.SequenceRandomStaggerMaxSpinBox->value());
	m_Settings->FramesPerRot(ui.SequenceFramesPerRotSpinBox->value());
	m_Settings->FramesPerRotMax(ui.SequenceRandomFramesPerRotMaxSpinBox->value());
	m_Settings->Rotations(ui.SequenceRotationsSpinBox->value());
	m_Settings->RotationsMax(ui.SequenceRandomRotationsMaxSpinBox->value());
	m_Settings->BlendFrames(ui.SequenceBlendFramesSpinBox->value());
	m_Settings->BlendFramesMax(ui.SequenceRandomBlendMaxFramesSpinBox->value());
	m_Settings->RotationsPerBlend(ui.SequenceRotationsPerBlendSpinBox->value());
	m_Settings->RotationsPerBlendMax(ui.SequenceRotationsPerBlendMaxSpinBox->value());
}

template class FractoriumEmberController<float>;

#ifdef DO_DOUBLE
	template class FractoriumEmberController<double>;
#endif
