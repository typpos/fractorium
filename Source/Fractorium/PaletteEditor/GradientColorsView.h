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
#include "GradientArrow.h"

/// <summary>
/// Class for drawing the resulting palette created by interpolating the key colors
/// as well as the arrows underneath the palette.
/// The arrows are held in a sorted map whose key is the normalized index of the arrow,
/// between 0 and 1 inclusive. They value is the arrow itself.
/// The resulting palette is always stored in the m_Palette member.
/// </summary>
class GradientColorsView : public QWidget
{
	Q_OBJECT

public:
	explicit GradientColorsView(QWidget* p = nullptr);
	bool Blend();
	void Blend(bool blend);
	void SetFocus(float position);
	void SetFocus(size_t position);
	void SetFocusColor(const QColor& col);
	void AddArrow(const QColor& color);
	void AddArrow(float position, const QColor& color);
	void DeleteFocusedArrow();
	void InvertColors();
	void RandomColors();
	void DistributeColors();
	void ResetToDefault();
	void ClearArrows();
	void NewFocusColor(const QColor& col, int index);
	void SetArrows(map<float, GradientArrow>& newArrows);
	int ArrowCount();
	int GetFocusedIndex();
	map<float, GradientArrow>& GetArrows();
	Palette<float>& GetPalette(int size);
	void SetPalette(const Palette<float>& palette);
	map<size_t, float> GetColorIndices() const;
	void SetColorIndices(const map<size_t, float>& indices);

Q_SIGNALS:
	void ArrowMove(qreal lastPos, const GradientArrow& arrow);
	void ArrowDoubleClicked(const GradientArrow& arrow);
	void ColorIndexMove(size_t index, float value);

protected:
	void paintEvent(QPaintEvent* e) override;
	void mousePressEvent(QMouseEvent* e) override;
	void mouseDoubleClickEvent(QMouseEvent* e) override;
	void mouseMoveEvent(QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	void resizeEvent(QResizeEvent*) override;

private:
	int RectWidth();
	int RectHeight();
	bool m_ArrowMoving = false;
	bool m_ColorIndexArrowMoving = false;
	bool m_Blend = true;
	QPoint m_ViewRectSize;
	QPoint m_ViewRectOffset = QPoint(5, 15);
	QPoint m_ViewRectTranslate = QPoint(5, 5);
	QRect m_ViewRect;
	QPoint m_DragStart;
	map<float, GradientArrow> m_Arrows;
	map<size_t, pair<float, TopArrow>> m_ColorIndicesArrows;
	Palette<float> m_Palette;
	QPixmap m_FinalFixedPixmap;
};
