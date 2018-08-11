#include "FractoriumPch.h"
#include "Fractorium.h"

/// <summary>
/// Initialize the xforms UI.
/// </summary>
void Fractorium::InitXformsUI()
{
	int spinHeight = 20, row = 0;
	connect(ui.AddXformButton,		 SIGNAL(clicked(bool)),			   this, SLOT(OnAddXformButtonClicked(bool)),	    Qt::QueuedConnection);
	connect(ui.AddLinkedXformButton, SIGNAL(clicked(bool)),			   this, SLOT(OnAddLinkedXformButtonClicked(bool)),	Qt::QueuedConnection);
	connect(ui.DuplicateXformButton, SIGNAL(clicked(bool)),			   this, SLOT(OnDuplicateXformButtonClicked(bool)),	Qt::QueuedConnection);
	connect(ui.ClearXformButton,	 SIGNAL(clicked(bool)),			   this, SLOT(OnClearXformButtonClicked(bool)),	    Qt::QueuedConnection);
	connect(ui.DeleteXformButton,	 SIGNAL(clicked(bool)),			   this, SLOT(OnDeleteXformButtonClicked(bool)),    Qt::QueuedConnection);
	connect(ui.AddFinalXformButton,  SIGNAL(clicked(bool)),			   this, SLOT(OnAddFinalXformButtonClicked(bool)),  Qt::QueuedConnection);
	connect(ui.CurrentXformCombo,	 SIGNAL(currentIndexChanged(int)), this, SLOT(OnCurrentXformComboChanged(int)),	    Qt::QueuedConnection);
	connect(ui.AnimateXformCheckBox, SIGNAL(stateChanged(int)),        this, SLOT(OnXformAnimateCheckBoxStateChanged(int)), Qt::QueuedConnection);
	SetFixedTableHeader(ui.XformWeightNameTable->horizontalHeader(), QHeaderView::ResizeToContents);
	//Use SetupSpinner() just to create the spinner, but use col of -1 to prevent it from being added to the table.
	SetupSpinner<DoubleSpinBox, double>(ui.XformWeightNameTable, this, row, -1, m_XformWeightSpin, spinHeight, 0, 1000, 0.05, SIGNAL(valueChanged(double)), SLOT(OnXformWeightChanged(double)), false, 0, 1, 0);
	m_XformWeightSpin->setDecimals(3);
	m_XformWeightSpin->SmallStep(0.001);
	m_XformWeightSpin->setMinimumWidth(40);
	m_XformWeightSpinnerButtonWidget = new SpinnerLabelButtonWidget(m_XformWeightSpin, "=", 20, 19, ui.XformWeightNameTable);
	m_XformWeightSpinnerButtonWidget->m_SpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_XformWeightSpinnerButtonWidget->m_Label->setStyleSheet("border: 0px;");
	m_XformWeightSpinnerButtonWidget->m_Label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_XformWeightSpinnerButtonWidget->m_Label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	m_XformWeightSpinnerButtonWidget->m_Button->setToolTip("Equalize weights");
	m_XformWeightSpinnerButtonWidget->m_Button->setStyleSheet("text-align: center center");
	m_XformWeightSpinnerButtonWidget->setMaximumWidth(130);
	connect(m_XformWeightSpinnerButtonWidget->m_Button, SIGNAL(clicked(bool)), this, SLOT(OnEqualWeightButtonClicked(bool)), Qt::QueuedConnection);
	ui.XformWeightNameTable->setCellWidget(0, 0, m_XformWeightSpinnerButtonWidget);
	ui.XformWeightNameTable->setItem(0, 1, new QTableWidgetItem());
	connect(ui.XformWeightNameTable, SIGNAL(cellChanged(int, int)), this, SLOT(OnXformNameChanged(int, int)), Qt::QueuedConnection);
	ui.CurrentXformCombo->view()->setMinimumWidth(100);
	ui.CurrentXformCombo->view()->setMaximumWidth(500);
	//ui.CurrentXformCombo->view()->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	ui.CurrentXformCombo->view()->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustToContentsOnFirstShow);
#ifndef _WIN32
	//For some reason linux makes these 24x24, even though the designer explicitly says 16x16.
	ui.AddXformButton->setIconSize(QSize(16, 16));
	ui.DuplicateXformButton->setIconSize(QSize(16, 16));
	ui.ClearXformButton->setIconSize(QSize(16, 16));
	ui.DeleteXformButton->setIconSize(QSize(16, 16));
	ui.AddFinalXformButton->setIconSize(QSize(16, 16));
	ui.CurrentXformCombo->setIconSize(QSize(16, 16));
#endif
}

/// <summary>
/// Get the current xform.
/// </summary>
/// <returns>The current xform as specified by the current xform combo box index. nullptr if out of range (should never happen).</returns>
template <typename T>
Xform<T>* FractoriumEmberController<T>::CurrentXform()
{
	bool hasFinal = m_Fractorium->HaveFinal();
	return m_Ember.GetTotalXform(m_Fractorium->ui.CurrentXformCombo->currentIndex(), hasFinal);//Need to force final for the special case they created a final, then cleared it, but did not delete it.
}

/// <summary>
/// Set the current xform to the index passed in.
/// </summary>
/// <param name="i">The index to set the current xform to</param>
void Fractorium::CurrentXform(uint i)
{
	if (i < uint(ui.CurrentXformCombo->count()))
		ui.CurrentXformCombo->setCurrentIndex(i);
}
/// <summary>
/// Set the current xform and populate all GUI widgets.
/// Called when the current xform combo box index changes.
/// </summary>
/// <param name="index">The selected combo box index</param>
template <typename T>
void FractoriumEmberController<T>::CurrentXformComboChanged(int index)
{
	bool forceFinal = m_Fractorium->HaveFinal();

	if (auto xform = m_Ember.GetTotalXform(index, forceFinal))
	{
		FillWithXform(xform);
		m_GLController->SetSelectedXform(xform);
		int solo = m_Ember.m_Solo;
		m_Fractorium->ui.SoloXformCheckBox->blockSignals(true);
		m_Fractorium->ui.SoloXformCheckBox->setChecked(solo == index);
		m_Fractorium->ui.SoloXformCheckBox->blockSignals(false);
		bool enable = !IsFinal(CurrentXform());
		m_Fractorium->ui.DuplicateXformButton->setEnabled(enable);
		m_Fractorium->m_XformWeightSpin->setEnabled(enable);
		m_Fractorium->ui.SoloXformCheckBox->setEnabled(enable);
		m_Fractorium->ui.AddLinkedXformButton->setEnabled(enable);
		m_Fractorium->ui.AddFinalXformButton->setEnabled(!m_Ember.UseFinalXform());
	}
}

void Fractorium::OnCurrentXformComboChanged(int index) { m_Controller->CurrentXformComboChanged(index); }

/// <summary>
/// Add an empty xform in the current ember and set it as the current xform.
/// Called when the add xform button is clicked.
/// Resets the rendering process.
/// </summary>
/// <param name="checked">Ignored</param>
template <typename T>
void FractoriumEmberController<T>::AddXform()
{
	bool forceFinal = m_Fractorium->HaveFinal();
	Update([&]()
	{
		Xform<T> newXform;
		newXform.m_Weight = 0.25;
		newXform.m_ColorX = m_Rand.Frand01<T>();
		newXform.AddVariation(m_VariationList->GetVariationCopy(eVariationId::VAR_LINEAR));
		m_Ember.AddXform(newXform);
		int index = int(m_Ember.TotalXformCount(forceFinal) - (forceFinal ? 2 : 1));//Set index to the last item before final.
		FillXforms(index);
	});
}

void Fractorium::OnAddXformButtonClicked(bool checked) { m_Controller->AddXform(); }

/// <summary>
/// Add a new linked xform in the current ember and set it as the current xform.
/// Linked means:
///		Add an xform whose xaos values are:
///			From: All xaos values from the current xform are zero when going to any xform but the new one added, which is 1.
///			To: The xaos value coming from the current xform is 1 and the xaos values from all other xforms are 0, when going to the newly added xform.
///     Take different action when a single xform is selected vs. multiple.
///         Single: Copy the current xform's xaos values to the new one.
///         Multiple: Set the new xform's xaos values to 1, and except the last entry which is 0.
/// Called when the add xform button is clicked.
/// Resets the rendering process.
/// </summary>
/// <param name="checked">Ignored</param>
template <typename T>
void FractoriumEmberController<T>::AddLinkedXform()
{
	bool hasAdded = false;
	bool forceFinal = m_Fractorium->HaveFinal();
	auto selCount = m_Fractorium->SelectedXformCount(false);

	if (!selCount)//If none explicitly selected, use current.
		selCount = 1;

	Ember<T> ember = m_Ember;
	m_Ember.Reserve(m_Ember.XformCount() + 1);//Doing this ahead of time ensures pointers remain valid.
	auto iterCount = 0;
	UpdateXform([&](Xform<T>* xform, size_t xfindex, size_t selIndex)
	{
		//Covers very strange case where final is selected, but initially not considered because final is excluded,
		//but after adding the new xform, it thinks its selected index is non-final.
		if (iterCount < selCount)
		{
			size_t i, count = m_Ember.XformCount();

			if (!hasAdded)
			{
				Xform<T> newXform;
				newXform.m_Weight = 0.5;
				newXform.m_Opacity = xform->m_Opacity;
				newXform.m_ColorSpeed = 0;
				newXform.m_ColorX = 0;
				//newXform.m_ColorY = xform->m_ColorY;
				newXform.AddVariation(m_VariationList->GetVariationCopy(eVariationId::VAR_LINEAR));

				//Set all of the new xform's xaos values to the selected xform's xaos values,
				//then set the selected xform's xaos values to 0.
				for (i = 0; i < count; i++)
				{
					if (selCount == 1)
						newXform.SetXaos(i, xform->Xaos(i));
					else
						newXform.SetXaos(i, 1);
				}

				//Add the new xform and update the total count.
				m_Ember.AddXform(newXform);
				count = m_Ember.XformCount();

				//Set the xaos for all xforms pointing to the new one to zero.
				//Will set the last element of all linking and non-linking xforms, including the one we just added.
				//Linking xforms will have their last xaos element set to 1 below.
				for (i = 0; i < count; i++)
					if (auto xf = m_Ember.GetXform(i))
						xf->SetXaos(count - 1, 0);

				hasAdded = true;
			}

			//Linking xform, so set all xaos elements to 0, except the last.
			for (i = 0; i < count - 1; i++)
				xform->SetXaos(i, 0);

			xform->SetXaos(count - 1, 1);//Set the xaos value for the linking xform pointing to the new one to one.
			xform->m_Opacity = 0;//Clear the opacity of the all linking xform.
			iterCount++;
		}
	}, eXformUpdate::UPDATE_SELECTED_EXCEPT_FINAL);
	//Now update the GUI.
	int index = int(m_Ember.TotalXformCount(forceFinal) - (forceFinal ? 2 : 1));//Set index to the last item before final.
	FillXforms(index);
	FillXaos();
}

void Fractorium::OnAddLinkedXformButtonClicked(bool checked) { m_Controller->AddLinkedXform(); }

/// <summary>
/// Duplicate the specified xforms in the current ember, and set the last one as the current xform.
/// Called when the duplicate xform button is clicked.
/// Resets the rendering process.
/// </summary>
/// <param name="checked">Ignored</param>
template <typename T>
void FractoriumEmberController<T>::DuplicateXform()
{
	bool forceFinal = m_Fractorium->HaveFinal();
	vector<Xform<T>> vec;
	vec.reserve(m_Ember.XformCount());
	UpdateXform([&] (Xform<T>* xform, size_t xfindex, size_t selIndex)
	{
		vec.push_back(*xform);
	}, eXformUpdate::UPDATE_SELECTED_EXCEPT_FINAL, false);
	Update([&]()
	{
		AddXformsWithXaos(m_Ember, vec, true);
		int index = int(m_Ember.TotalXformCount(forceFinal) - (forceFinal ? 2 : 1));//Set index to the last item before final.
		FillXforms(index);//Handles xaos.
	});
}

void Fractorium::OnDuplicateXformButtonClicked(bool checked) { m_Controller->DuplicateXform(); }

/// <summary>
/// Clear all variations from the selected xforms. Affine, palette and xaos are left untouched.
/// Called when the clear xform button is clicked.
/// Resets the rendering process.
/// </summary>
/// <param name="checked">Ignored</param>
template <typename T>
void FractoriumEmberController<T>::ClearXform()
{
	UpdateXform([&] (Xform<T>* xform, size_t xfindex, size_t selIndex)
	{
		xform->ClearAndDeleteVariations();//Note xaos is left alone.
	}, eXformUpdate::UPDATE_SELECTED);
	FillVariationTreeWithCurrentXform();
}

void Fractorium::OnClearXformButtonClicked(bool checked) { m_Controller->ClearXform(); }

/// <summary>
/// Delete the selected xforms.
/// Cache a copy of the final xform if it's been selected for removal.
/// Will not delete the last remaining non-final xform.
/// Called when the delete xform button is clicked.
/// Resets the rendering process.
/// </summary>
/// <param name="checked">Ignored</param>
template <typename T>
void FractoriumEmberController<T>::DeleteXforms()
{
	bool removed = false;
	bool anyChecked = false;
	bool haveFinal = m_Fractorium->HaveFinal();
	auto combo = m_Fractorium->ui.CurrentXformCombo;
	Xform<T>* finalXform = nullptr;
	vector<Xform<T>> xformsToKeep;
	xformsToKeep.reserve(m_Ember.TotalXformCount());
	//Iterating over the checkboxes must be done instead of using UpdateXform() to iterate over xforms
	//because xforms are being deleted inside the loop.
	//Also manually calling UpdateRender() rather than using the usual Update() call because
	//it should only be called if an xform has actually been deleted.
	//Rather than go through and delete, it's easier to just make a list of what we want to keep.
	m_Fractorium->ForEachXformCheckbox([&](int i, QCheckBox * w, bool isFinal)
	{
		if (!w->isChecked())//Keep if not checked.
		{
			if (isFinal)
				finalXform = m_Ember.NonConstFinalXform();
			else if (auto xform = m_Ember.GetXform(i))
				xformsToKeep.push_back(*xform);
		}
		else
			anyChecked = true;//At least one was selected for removal.
	});
	//They might not have selected any checkboxes, in which case just delete the current.
	auto current = combo->currentIndex();
	auto totalCount = m_Ember.TotalXformCount();
	bool keepFinal = finalXform && haveFinal;

	//Nothing was selected, so just delete current.
	if (!anyChecked)
	{
		//Disallow deleting the only remaining non-final xform.
		if (!(haveFinal && totalCount <= 2 && current == 0) &&//One non-final, one final, disallow deleting non-final.
				!(!haveFinal && totalCount == 1))//One non-final, no final, disallow deleting.
		{
			if (haveFinal && m_Ember.IsFinalXform(CurrentXform()))//Is final the current?
				m_Ember.m_CachedFinal = *m_Ember.FinalXform();//Keep a copy in case the user wants to re-add the final.

			m_Ember.DeleteTotalXform(current, haveFinal);//Will cover the case of current either being final or non-final.
			removed = true;
		}
	}
	else
	{
		if (!xformsToKeep.empty())//Remove if they requested to do so, but ensure it's not removing all.
		{
			removed = true;
			m_Ember.ReplaceXforms(xformsToKeep);//Replace with only those they chose to keep (the inverse of what was checked).
		}
		else//They selected all to delete, which is not allowed, so just keep the first xform.
		{
			removed = true;

			while (m_Ember.XformCount() > 1)
				m_Ember.DeleteXform(m_Ember.XformCount() - 1);
		}

		if (!keepFinal)//They selected final to delete.
		{
			removed = true;
			m_Ember.m_CachedFinal = *m_Ember.FinalXform();//Keep a copy in case the user wants to re-add the final.
			m_Ember.NonConstFinalXform()->Clear();
		}
	}

	if (removed)
	{
		int index = int(m_Ember.TotalXformCount() - (m_Ember.UseFinalXform() ? 2 : 1));//Set index to the last item before final. Note final is requeried one last time.
		FillXforms(index);
		UpdateRender();
		m_Fractorium->ui.GLDisplay->repaint();//Force update because for some reason it doesn't always happen.
	}
}

void Fractorium::OnDeleteXformButtonClicked(bool checked) { m_Controller->DeleteXforms(); }
/// <summary>
/// Add a final xform to the ember and set it as the current xform.
/// Will only take action if a final xform is not already present.
/// Will re-add a copy of the last used final xform for the current ember if one had already been added then removed.
/// Called when the add final xform button is clicked.
/// Resets the rendering process.
/// </summary>
/// <param name="checked">Ignored</param>
template <typename T>
void FractoriumEmberController<T>::AddFinalXform()
{
	//Check to see if a final xform is already present.
	if (!m_Fractorium->HaveFinal())
	{
		Update([&]()
		{
			auto& final = m_Ember.m_CachedFinal;
			final.m_Animate = 0;

			if (final.Empty())
				final.AddVariation(m_VariationList->GetVariationCopy(eVariationId::VAR_LINEAR));//Just a placeholder so other parts of the code don't see it as being empty.

			m_Ember.SetFinalXform(final);
			int index = int(m_Ember.TotalXformCount() - 1);//Set index to the last item.
			FillXforms(index);
		});
	}
}
void Fractorium::OnAddFinalXformButtonClicked(bool checked) { m_Controller->AddFinalXform(); }
/// <summary>
/// Set the weight of the selected xforms.
/// Called when weight spinner changes.
/// Resets the rendering process.
/// </summary>
/// <param name="d">The weight</param>
template <typename T>
void FractoriumEmberController<T>::XformWeightChanged(double d)
{
	UpdateXform([&] (Xform<T>* xform, size_t xfindex, size_t selIndex)
	{
		xform->m_Weight = d;
	}, eXformUpdate::UPDATE_SELECTED_EXCEPT_FINAL);
	SetNormalizedWeightText(CurrentXform());
}
void Fractorium::OnXformWeightChanged(double d) { m_Controller->XformWeightChanged(d); }
/// <summary>
/// Equalize the weights of all xforms in the ember.
/// </summary>
template <typename T>
void FractoriumEmberController<T>::EqualizeWeights()
{
	UpdateXform([&] (Xform<T>* xform, size_t xfindex, size_t selIndex)
	{
		m_Ember.EqualizeWeights();
		m_Fractorium->m_XformWeightSpin->setValue(xform->m_Weight);//Will trigger an update, so pass false to updateRender below.
	}, eXformUpdate::UPDATE_CURRENT, false);
}
void Fractorium::OnEqualWeightButtonClicked(bool checked) { m_Controller->EqualizeWeights(); }
/// <summary>
/// Set the name of the current xform.
/// Update the corresponding xform checkbox text with the name.
/// Called when the user types in the name cell of the table.
/// </summary>
/// <param name="row">The row of the cell</param>
/// <param name="col">The col of the cell</param>
template <typename T>
void FractoriumEmberController<T>::XformNameChanged(int row, int col)
{
	bool forceFinal = m_Fractorium->HaveFinal();
	UpdateXform([&] (Xform<T>* xform, size_t xfindex, size_t selIndex)
	{
		xform->m_Name = m_Fractorium->ui.XformWeightNameTable->item(row, col)->text().toStdString();
		XformCheckboxAt(int(xfindex), [&](QCheckBox * checkbox) { checkbox->setText(MakeXformCaption(xfindex)); });
	}, eXformUpdate::UPDATE_CURRENT, false);
	FillSummary();//Manually update because this does not trigger a render, which is where this would normally be called.
}

void Fractorium::OnXformNameChanged(int row, int col)
{
	m_Controller->XformNameChanged(row, col);
	m_Controller->UpdateXformName(ui.CurrentXformCombo->currentIndex());
}

/// <summary>
/// Set the animate field of the selected xforms, this allows excluding current if it's not checked, but applies only to it if none are checked.
/// This has no effect on interactive rendering, it only sets a value
/// that will later be saved to Xml when the user saves.
/// This value is observed when creating sequences for animation.
/// Applies to all embers if "Apply All" is checked.
/// Called when the user toggles the animate xform checkbox.
/// </summary>
/// <param name="state">1 for checked, else false</param>
template <typename T>
void FractoriumEmberController<T>::XformAnimateChanged(int state)
{
	T animate = state > 0 ? 1 : 0;
	UpdateXform([&](Xform<T>* xform, size_t xfindex, size_t selIndex)
	{
		bool final = IsFinal(xform);
		UpdateAll([&](Ember<T>& ember, bool isMain)
		{
			if (final)//If the current xform was final, only apply to other embers which also have a final xform.
			{
				if (ember.UseFinalXform())
				{
					auto xform = ember.NonConstFinalXform();
					xform->m_Animate = animate;
				}

				if (!m_Fractorium->ApplyAll())
					if (m_EmberFilePointer && m_EmberFilePointer->UseFinalXform())
						m_EmberFilePointer->NonConstFinalXform()->m_Animate = animate;
			}
			else//Current was not final, so apply to other embers which have a non-final xform at this index.
			{
				if (auto xform = ember.GetXform(xfindex))
					xform->m_Animate = animate;

				if (!m_Fractorium->ApplyAll() && m_EmberFilePointer)
					if (auto xform = m_EmberFilePointer->GetXform(xfindex))
						xform->m_Animate = animate;
			}
		}, false, eProcessAction::NOTHING, m_Fractorium->ApplyAll());
	}, eXformUpdate::UPDATE_SELECTED, false);
}
void Fractorium::OnXformAnimateCheckBoxStateChanged(int state) { m_Controller->XformAnimateChanged(state); }

/// <summary>
/// Fill all GUI widgets with values from the passed in xform.
/// </summary>
/// <param name="xform">The xform whose values will be used to populate the widgets</param>
template <typename T>
void FractoriumEmberController<T>::FillWithXform(Xform<T>* xform)
{
	m_Fractorium->m_XformWeightSpin->SetValueStealth(xform->m_Weight);
	SetNormalizedWeightText(xform);
	m_Fractorium->ui.AnimateXformCheckBox->blockSignals(true);
	m_Fractorium->ui.AnimateXformCheckBox->setChecked(xform->m_Animate > 0 ? true : false);
	m_Fractorium->ui.AnimateXformCheckBox->blockSignals(false);

	if (auto item = m_Fractorium->ui.XformWeightNameTable->item(0, 1))
	{
		m_Fractorium->ui.XformWeightNameTable->blockSignals(true);
		item->setText(QString::fromStdString(xform->m_Name));
		m_Fractorium->ui.XformWeightNameTable->blockSignals(false);
	}

	FillVariationTreeWithXform(xform);
	FillColorWithXform(xform);
	FillAffineWithXform(xform, true);
	FillAffineWithXform(xform, false);
}
/// <summary>
/// Set the normalized weight of the current xform as the suffix text of the weight spinner.
/// </summary>
/// <param name="xform">The current xform whose normalized weight will be shown</param>
template <typename T>
void FractoriumEmberController<T>::SetNormalizedWeightText(Xform<T>* xform)
{
	if (xform)
	{
		int index = m_Ember.GetXformIndex(xform);
		m_Ember.CalcNormalizedWeights(m_NormalizedWeights);

		if (index != -1 && index < m_NormalizedWeights.size())
			m_Fractorium->m_XformWeightSpinnerButtonWidget->m_Label->setText(QString(" (") + QLocale::system().toString(double(m_NormalizedWeights[index]), 'g', 3) + ")");
	}
}
/// <summary>
/// Determine whether the specified xform is the final xform in the ember.
/// </summary>
/// <param name="xform">The xform to examine</param>
/// <returns>True if final, else false.</returns>
template <typename T>
bool FractoriumEmberController<T>::IsFinal(Xform<T>* xform)
{
	return (m_Fractorium->HaveFinal() && (xform == m_Ember.FinalXform()));
}
/// <summary>
/// Fill the xforms combo box with the xforms in the current ember.
/// Select the index passed in and fill all widgets with its values.
/// Also dynamically generate a checkbox for each xform which will allow the user
/// to select which xforms to apply operations to.
/// </summary>
/// <param name="index">The index to select after populating, default 0.</param>
template <typename T>
void FractoriumEmberController<T>::FillXforms(int index)
{
	int i = 0, count = int(XformCount());
	auto combo = m_Fractorium->ui.CurrentXformCombo;
	combo->blockSignals(true);
	combo->clear();
	//First clear all dynamically created checkboxes.
	m_Fractorium->ClearXformsSelections();
	m_Fractorium->m_XformsSelectionLayout->blockSignals(true);

	//Fill combo box and create new checkboxes.
	for (i = 0; i < count; i++)
	{
		combo->addItem(ToString(i + 1));
		combo->setItemIcon(i, m_Fractorium->m_XformComboIcons[i % XFORM_COLOR_COUNT]);
		UpdateXformName(i);
	}

	i = 0;

	while (i < count)
	{
		if (i < count - 1)
		{
			auto cb1 = new QCheckBox(MakeXformCaption(i), m_Fractorium);
			auto cb2 = new QCheckBox(MakeXformCaption(i + 1), m_Fractorium);
			QObject::connect(cb1, &QCheckBox::stateChanged, [&](int state) { m_Fractorium->ui.GLDisplay->update(); });//Ensure circles are drawn immediately after toggle.
			QObject::connect(cb2, &QCheckBox::stateChanged, [&](int state) { m_Fractorium->ui.GLDisplay->update(); });
			m_Fractorium->m_XformSelections.push_back(cb1);
			m_Fractorium->m_XformSelections.push_back(cb2);
			m_Fractorium->m_XformsSelectionLayout->addRow(cb1, cb2);
			i += 2;
		}
		else if (i < count)
		{
			auto cb = new QCheckBox(MakeXformCaption(i), m_Fractorium);
			QObject::connect(cb, &QCheckBox::stateChanged, [&](int state) { m_Fractorium->ui.GLDisplay->update(); });
			m_Fractorium->m_XformSelections.push_back(cb);
			m_Fractorium->m_XformsSelectionLayout->addRow(cb, new QWidget(m_Fractorium));
			i++;
		}
	}

	//Special case for final xform.
	if (UseFinalXform())
	{
		auto cb = new QCheckBox(MakeXformCaption(i), m_Fractorium);
		m_Fractorium->m_XformSelections.push_back(cb);
		m_Fractorium->m_XformsSelectionLayout->addRow(cb, new QWidget(m_Fractorium));
		combo->addItem("Final");
		combo->setItemIcon(i, m_Fractorium->m_FinalXformComboIcon);
		UpdateXformName(i);
	}

	m_Fractorium->m_XformsSelectionLayout->blockSignals(false);
	combo->blockSignals(false);

	if (index < combo->count())
		combo->setCurrentIndex(index);

	m_Fractorium->ui.SoloXformCheckBox->blockSignals(true);

	if (m_Ember.m_Solo == combo->currentIndex())
		m_Fractorium->ui.SoloXformCheckBox->setChecked(true);
	else
		m_Fractorium->ui.SoloXformCheckBox->setChecked(false);

	SoloXformCheckBoxStateChanged(m_Ember.m_Solo > -1 ? Qt::Checked : Qt::Unchecked, m_Ember.m_Solo);
	m_Fractorium->ui.SoloXformCheckBox->blockSignals(false);
	m_Fractorium->FillXaosTable();
	m_Fractorium->OnCurrentXformComboChanged(index);//Make sure the event gets called, because it won't if the zero index is already selected.
}

/// <summary>
/// Update the text in xforms combo box to show the name of Xform.
/// </summary>
/// <param name="index">The index of the Xform to update.</param>
///
template<typename T>
void FractoriumEmberController<T>::UpdateXformName(int index)
{
	bool forceFinal = m_Fractorium->HaveFinal();
	bool isFinal = m_Ember.FinalXform() == m_Ember.GetTotalXform(index, forceFinal);
	QString name = isFinal ? "Final" : QString::number(index + 1);

	if (auto xform = m_Ember.GetTotalXform(index, forceFinal))
	{
		if (!xform->m_Name.empty())
			name += "    " + QString::fromStdString(xform->m_Name);

		m_Fractorium->ui.CurrentXformCombo->setItemText(index, name);
		auto view = m_Fractorium->ui.CurrentXformCombo->view();
		auto fontMetrics1 = view->fontMetrics();
		auto textWidth = m_Fractorium->ui.CurrentXformCombo->width();
		auto ww = fontMetrics1.width("WW");

		for (int i = 0; i < m_Fractorium->ui.CurrentXformCombo->count(); ++i)
			textWidth = std::max(fontMetrics1.width(m_Fractorium->ui.CurrentXformCombo->itemText(i)) + ww, textWidth);

		view->setMinimumWidth(textWidth);
		view->setMaximumWidth(textWidth);
	}
}

template class FractoriumEmberController<float>;
#ifdef DO_DOUBLE
	template class FractoriumEmberController<double>;
#endif