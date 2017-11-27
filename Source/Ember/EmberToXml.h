#pragma once

#include "Utils.h"
#include "PaletteList.h"
#include "VariationList.h"
#include "Ember.h"

/// <summary>
/// EmberToXml class.
/// </summary>

namespace EmberNs
{
/// <summary>
/// Class for converting ember objects to Xml documents.
/// Support for saving one or more to a single file.
/// Template argument expected to be float or double.
/// </summary>
template <typename T>
class EMBER_API EmberToXml : public EmberReport
{
public:
	/// <summary>
	/// Empty constructor.
	/// </summary>
	EmberToXml() = default;
	~EmberToXml() = default;
	EmberToXml(const EmberToXml<T>& e) = delete;

	bool Save(const string& filename, Ember<T>& ember, size_t printEditDepth, bool doEdits, bool hexPalette, bool append = false, bool start = false, bool finish = false);
	template <typename Alloc, template <typename, typename> class C>
	bool Save(const string& filename, C<Ember<T>, Alloc>& embers, size_t printEditDepth, bool doEdits, bool hexPalette, bool append, bool start, bool finish);
	string ToString(Ember<T>& ember, const string& extraAttributes, size_t printEditDepth, bool doEdits, bool hexPalette = true);
	xmlDocPtr CreateNewEditdoc(Ember<T>* parent0, Ember<T>* parent1, const string& action, const string& nick, const string& url, const string& id, const string& comment, intmax_t sheepGen = 0, intmax_t sheepId = 0);

private:
	string ToString(Xform<T>& xform, size_t xformCount, bool isFinal, bool doMotion);
	string ToString(xmlNodePtr editNode, size_t tabs, bool formatting, size_t printEditDepth);
	string ToString(const EmberMotion<T>& motion);
	void AddFilenameWithoutAmpersand(xmlNodePtr node, string& filename);
};
}
