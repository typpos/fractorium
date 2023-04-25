#pragma once

#include "Palette.h"
#include "Point.h"

/// <summary>
/// PaletteList class.
/// </summary>

namespace EmberNs
{
/// <summary>
/// Holds a list of palettes read from an Xml file. Since the default list from flam3-palettes.xml is fairly large at 700 palettes,
/// the list member is kept as a static. This class derives from EmberReport in order to report any errors that occurred while reading the Xml.
/// Note that although the Xml color values are expected to be 0-255, they are converted and stored as normalized colors, with values from 0-1.
/// This can hold read only palettes, as well as user created and modifiable ones.
/// The key in the map is the fully qualified path and filename to each palette file.
/// Despite the keys being full paths, the same filename cannot be inserted twice, even if they reside
/// in different folders. Functions are provided to retrieve palette files via filename only, or full path.
/// Template argument should always be float (which makes the templating of this class pointless).
/// </summary>
template <typename T>
class EMBER_API PaletteList : public EmberReport, public Singleton<PaletteList<T>>
{
public:
	const char* m_DefaultFilename = "flam3-palettes.xml";

	bool AddPaletteFile(const string& filename, const vector<Palette<T>>& palettes);
	bool AddEmptyPaletteFile(const string& filename);
	bool AddPaletteToFile(const string& filename, const Palette<T>& palette);
	bool Replace(const string& filename, const Palette<T>& palette);
	bool Replace(const string& filename, const Palette<T>& palette, int index);
	bool Delete(const string& filename, int index);
	bool Add(const string& filename, bool force = false);
	Palette<T>* GetRandomPalette();
	Palette<T>* GetPaletteByFilename(const string& filename, size_t i);
	Palette<T>* GetPaletteByFullPath(const string& filename, size_t i);
	Palette<T>* GetPaletteByName(const string& filename, const string& name);
	vector<Palette<T>>* GetPaletteListByFilename(const string& filename);
	vector<Palette<T>>* GetPaletteListByFullPath(const string& filename);
	vector<Palette<T>>* GetPaletteListByFullPathOrFilename(const string& filename);
	string GetFullPathFromFilename(const string& filename);
	bool GetHueAdjustedPalette(const string& filename, size_t i, T hue, Palette<T>& palette);
	void Clear();
	size_t Size();
	size_t Size(size_t index);
	size_t Size(const string& s);
	const string& Name(size_t index);
	bool IsModifiable(const string& filename);
	const map<string, vector<Palette<T>>>& Palettes() const;

	SINGLETON_DERIVED_DECL(PaletteList<T>);
private:
	PaletteList();
	bool Save(const string& filename);
	void ParsePalettes(xmlNode* node, const shared_ptr<string>& filename, vector<Palette<T>>& palettes);
	bool ParsePalettes(const string& buf, const shared_ptr<string>& filename, vector<Palette<T>>& palettes);
	map<string, vector<Palette<T>>> s_Palettes;//The map of filenames to vectors that store the palettes.
};
}
