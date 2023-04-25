#pragma once

#include "ui_AboutDialog.h"

/// <summary>
/// FractoriumAboutDialog class.
/// </summary>

/// <summary>
/// The about dialog displays several group boxes showing the
/// code and icons used in this project and their respective authors
/// and licenses. It performs no other function.
/// </summary>
class FractoriumAboutDialog : public QDialog
{
	Q_OBJECT
public:
	FractoriumAboutDialog(QWidget* p = nullptr, Qt::WindowFlags f = Qt::WindowType::Widget);

private:
	Ui::AboutDialog ui;
};
