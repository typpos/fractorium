#pragma once

#include "Variation.h"

/// <summary>
/// VariationList class.
/// </summary>

namespace EmberNs
{
/// <summary>
/// Since the list of variations is numerous, it's convenient to be able to make copies
/// of specific ones. This class holds a list of pointers to variation objects for every
/// variation available. Similar to the PaletteList class, a caller can look up a variation
/// by name or ID and retrieve a copy of it.
/// This class follows the singleton pattern.
/// All variations are deleted upon destruction.
/// Template argument expected to be float or double.
/// </summary>
template <typename T>
class EMBER_API VariationList: public Singleton<VariationList<T>>
{
public:
	const Variation<T>* GetVariation(size_t index) const;
	const Variation<T>* GetVariation(size_t index, eVariationType varType) const;
	Variation<T>* GetVariationCopy(size_t index, T weight = 1) const;
	Variation<T>* GetVariationCopy(size_t index, eVariationType varType, T weight = 1) const;
	const Variation<T>* GetVariation(eVariationId id) const;
	Variation<T>* GetVariationCopy(eVariationId id, T weight = 1) const;
	const Variation<T>* GetVariation(const string& name) const;
	Variation<T>* GetVariationCopy(const string& name, T weight = 1) const;
	const ParametricVariation<T>* GetParametricVariation(size_t index) const;
	const ParametricVariation<T>* GetParametricVariation(const string& name) const;
	ParametricVariation<T>* GetParametricVariationCopy(eVariationId id, T weight = 1) const;
	int GetVariationIndex(const string& name) const;
	size_t Size() const;
	size_t RegSize() const;
	size_t PreSize() const;
	size_t PostSize() const;
	size_t ParametricSize() const;
	size_t NonParametricSize() const;

	const vector<const Variation<T>*>& AllVars()  const;
	const vector<const Variation<T>*>& RegVars()  const;
	const vector<const Variation<T>*>& PreVars()  const;
	const vector<const Variation<T>*>& PostVars() const;
	const vector<const Variation<T>*>& NonParametricVariations() const;
	const vector<const ParametricVariation<T>*>& ParametricVariations() const;

	SINGLETON_DERIVED_DECL(VariationList<T>);

private:
	VariationList();
	Variation<T>* MakeCopyWithWeight(const Variation<T>* var, T weight) const;

	vector<const Variation<T>*> m_Variations;//A list of pointers to dynamically allocated variation objects.
	vector<const Variation<T>*> m_RegVariations;
	vector<const Variation<T>*> m_PreVariations;
	vector<const Variation<T>*> m_PostVariations;
	vector<const Variation<T>*> m_NonParametricVariations;
	vector<const ParametricVariation<T>*> m_ParametricVariations;//A list of pointers to elements in m_Variations which are derived from ParametricVariation.
};
}
