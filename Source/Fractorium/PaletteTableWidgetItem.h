#pragma once

#include "FractoriumPch.h"

/// <summary>
/// PaletteTableWidgetItem class.
/// </summary>

/// <summary>
/// A thin derivation of QTableWidgetItem which keeps a pointer to a palette object.
/// The lifetime of the palette object must be greater than or equal to
/// the lifetime of this object.
/// </summary>
class PaletteTableWidgetItem : public QTableWidgetItem
{
public:
	PaletteTableWidgetItem(Palette<float>* palette)
		: m_Palette(palette)
	{
	}

	size_t Index() const { return m_Palette->m_Index; }
	Palette<float>* GetPalette() const { return m_Palette; }

private:
	Palette<float>* m_Palette;
};