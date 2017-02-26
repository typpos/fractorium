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

/// <summary>
/// A derivation of a QPushButton which makes a large, clickable panel
/// which custom paints the button based on a user specified color.
/// </summary>
class ColorPanel : public QPushButton
{
	Q_OBJECT

public:
	ColorPanel(QWidget* p = nullptr);

	void Pen(const QPen& pen);
	QPen Pen() const;
	void Color(const QColor& color);
	QColor Color() const;

protected:
	virtual void paintEvent(QPaintEvent* event) override;

private:
	QPen m_Pen;
	QColor m_Color;
};
