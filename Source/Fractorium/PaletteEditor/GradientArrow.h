/****************************************************************************/
// This file is part of the gradLib library originally made by Stian Broen
//
// For more free libraries, please visit <http://broentech.no>
//
// gradLib is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this library.  If not, see <http://www.gnu.org/licenses/>
/****************************************************************************/

#pragma once

#include "FractoriumPch.h"

/// <summary>
/// Class for drawing the small arrows below the gradient in the palette editor.
/// The drawing is accomplished via a QPolygon object.
/// </summary>
class GradientArrow
{
public:
	/// <summary>
	/// Default constructor which sets up the size of the arrow.
	/// </summary>
	explicit GradientArrow()
	{
		QPolygon area;
		area << QPoint(5, 5) << QPoint(10, 0) << QPoint(15, 5) << QPoint(15, 15) << QPoint(5, 15) << QPoint(5, 5);
		Area(area);
	}

	/// <summary>
	/// Constructor which takes the color and focus state of the arrow.
	/// </summary>
	/// <param name="col">The color of the arrow</param>
	/// <param name="focus">Whether the arrow is focused</param>
	explicit GradientArrow(QColor col, bool focus)
		: GradientArrow()
	{
		m_Color = col;
		m_Focus = focus;
	}

	/// <summary>
	/// Copy constructor to copy another GradientArrow.
	/// </summary>
	/// <param name="other">The GradientArrow object to copy</param>
	GradientArrow(const GradientArrow& other)
		: m_Focus(other.Focus()),
		  m_Area(other.Area()),
		  m_Color(other.Color())
	{
	}

	/// <summary>
	/// Getters and setters.
	/// </summary>
	inline bool Focus() const { return m_Focus; }
	inline void Focus(bool val) { m_Focus = val; }
	inline const QPolygon Area() const { return m_Area; }
	inline void Area(const QPolygon& val) {m_Area = val; }
	inline const QColor Color() const { return m_Color; }
	inline void Color(const QColor& val) { m_Color = val; }

private:
	bool m_Focus;
	QPolygon m_Area;
	QColor m_Color;
};
