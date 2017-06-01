/*

	Copyright (C) 2009, Etienne Moutot <e.moutot@gmail.com>

	This file is part of colorPickerWidget.

	colorPickerWidget is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	colorPickerWidget is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "FractoriumPch.h"
#include "ColorTriangle.h"
#include "ColorPanel.h"

/// <summary>
/// Aggregator class to package a color triangle, color panel, and color dialog
/// all together on a layout.
/// </summary>
class ColorPickerWidget : public QWidget
{
	Q_OBJECT

public:
	ColorPickerWidget(QWidget* p = nullptr);

	QColor Color() const;
	void SetColorPanelColor(const QColor& col);

Q_SIGNALS:
	void ColorChanged(const QColor& col);

protected:
	void resizeEvent(QResizeEvent* event) override;

private Q_SLOTS:
	void OnColorViewerClicked();
	void OnTriangleColorChanged(const QColor& col);

private:
	ColorTriangle* m_ColorTriangle;
	ColorPanel* m_ColorPanel;
	QColorDialog* m_ColorDialog;
};
