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
#include "FractoriumPch.h"
#include "ColorPanel.h"

/// <summary>
/// Constructor which passes parent widget to the base and initializes the minimum size.
/// </summary>
/// <param name="p">The parent widget</param>
ColorPanel::ColorPanel(QWidget* p)
	: QPushButton(p)
{
	setMinimumSize(10, 10);
}

/// <summary>
/// Derived paint event which paints the entire button surface with m_Color.
/// </summary>
void ColorPanel::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	p.setPen(m_Pen);
	p.setBrush(QBrush(m_Color));
	p.drawRect(QRect(2, 2, width() - 6, height() - 6));
}

/// <summary>
/// Set the pen object used to paint the button.
/// </summary>
/// <param name="pen">The pen object used to paint the button</param>
void ColorPanel::Pen(const QPen& pen)
{
	m_Pen = pen;
	update();
}

/// <summary>
/// Get the pen object used to paint the button.
/// </summary>
/// <returns>QPen</returns>
QPen ColorPanel::Pen() const
{
	return m_Pen;
}

/// <summary>
/// Set the color used to paint the button.
/// </summary>
/// <param name="color">The color used to paint the button</param>
void ColorPanel::Color(const QColor& color)
{
	m_Color = color;
	update();
}

/// <summary>
/// Get the color used to paint the button.
/// </summary>
/// <returns>QColor</returns>
QColor ColorPanel::Color() const
{
	return m_Color;
}
