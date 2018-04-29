#include "EmberPch.h"
#include "VariationList.h"
#include "Variations01.h"
#include "Variations02.h"
#include "Variations03.h"
#include "Variations04.h"
#include "Variations05.h"
#include "Variations06.h"
#include "Variations07.h"
#include "VariationsDC.h"

namespace EmberNs
{
#define ADDPREPOSTREGVAR(varName) \
	m_Variations.push_back(new varName##Variation<T>()); \
	m_Variations.push_back(new Pre##varName##Variation<T>()); \
	m_Variations.push_back(new Post##varName##Variation<T>());

/// <summary>
/// Constructor which initializes all of the variation objects and stores them in the list.
/// </summary>
template <typename T>
VariationList<T>::VariationList()
{
	m_Variations.reserve(size_t(eVariationId::LAST_VAR));
	ADDPREPOSTREGVAR(Linear)
	ADDPREPOSTREGVAR(Sinusoidal)
	ADDPREPOSTREGVAR(Spherical)
	ADDPREPOSTREGVAR(Swirl)
	ADDPREPOSTREGVAR(Horseshoe)
	ADDPREPOSTREGVAR(Polar)
	ADDPREPOSTREGVAR(Handkerchief)
	ADDPREPOSTREGVAR(Heart)
	ADDPREPOSTREGVAR(Disc)
	ADDPREPOSTREGVAR(Spiral)
	ADDPREPOSTREGVAR(Hyperbolic)
	ADDPREPOSTREGVAR(Diamond)
	ADDPREPOSTREGVAR(Ex)
	ADDPREPOSTREGVAR(Julia)
	ADDPREPOSTREGVAR(Bent)
	ADDPREPOSTREGVAR(Waves)
	ADDPREPOSTREGVAR(Fisheye)
	ADDPREPOSTREGVAR(Popcorn)
	ADDPREPOSTREGVAR(Exponential)
	ADDPREPOSTREGVAR(Power)
	ADDPREPOSTREGVAR(Cosine)
	ADDPREPOSTREGVAR(Rings)
	ADDPREPOSTREGVAR(Fan)
	ADDPREPOSTREGVAR(Blob)
	ADDPREPOSTREGVAR(Pdj)
	ADDPREPOSTREGVAR(Fan2)
	ADDPREPOSTREGVAR(Rings2)
	ADDPREPOSTREGVAR(Eyefish)
	ADDPREPOSTREGVAR(Bubble)
	ADDPREPOSTREGVAR(Cylinder)
	ADDPREPOSTREGVAR(Perspective)
	ADDPREPOSTREGVAR(Noise)
	ADDPREPOSTREGVAR(JuliaNGeneric)
	ADDPREPOSTREGVAR(JuliaScope)
	ADDPREPOSTREGVAR(Blur)
	ADDPREPOSTREGVAR(GaussianBlur)
	ADDPREPOSTREGVAR(RadialBlur)
	ADDPREPOSTREGVAR(Pie)
	ADDPREPOSTREGVAR(Ngon)
	ADDPREPOSTREGVAR(Curl)
	ADDPREPOSTREGVAR(Rectangles)
	ADDPREPOSTREGVAR(Arch)
	ADDPREPOSTREGVAR(Tangent)
	ADDPREPOSTREGVAR(Square)
	ADDPREPOSTREGVAR(Rays)
	ADDPREPOSTREGVAR(Rays1)
	ADDPREPOSTREGVAR(Rays2)
	ADDPREPOSTREGVAR(Rays3)
	ADDPREPOSTREGVAR(Blade)
	ADDPREPOSTREGVAR(Secant2)
	ADDPREPOSTREGVAR(TwinTrian)
	ADDPREPOSTREGVAR(Cross)
	ADDPREPOSTREGVAR(Disc2)
	ADDPREPOSTREGVAR(SuperShape)
	ADDPREPOSTREGVAR(Flower)
	ADDPREPOSTREGVAR(Conic)
	ADDPREPOSTREGVAR(Parabola)
	ADDPREPOSTREGVAR(Bent2)
	ADDPREPOSTREGVAR(Bipolar)
	ADDPREPOSTREGVAR(Boarders)
	ADDPREPOSTREGVAR(Butterfly)
	ADDPREPOSTREGVAR(Cell)
	ADDPREPOSTREGVAR(Cpow)
	ADDPREPOSTREGVAR(Curve)
	ADDPREPOSTREGVAR(Edisc)
	ADDPREPOSTREGVAR(Elliptic)
	ADDPREPOSTREGVAR(Escher)
	ADDPREPOSTREGVAR(Foci)
	ADDPREPOSTREGVAR(LazySusan)
	ADDPREPOSTREGVAR(Loonie)
	ADDPREPOSTREGVAR(Modulus)
	ADDPREPOSTREGVAR(Oscilloscope)
	ADDPREPOSTREGVAR(Polar2)
	ADDPREPOSTREGVAR(Popcorn2)
	ADDPREPOSTREGVAR(Scry)
	ADDPREPOSTREGVAR(Separation)
	ADDPREPOSTREGVAR(Split)
	ADDPREPOSTREGVAR(Splits)
	ADDPREPOSTREGVAR(Stripes)
	ADDPREPOSTREGVAR(Wedge)
	ADDPREPOSTREGVAR(WedgeJulia)
	ADDPREPOSTREGVAR(WedgeSph)
	ADDPREPOSTREGVAR(Whorl)
	ADDPREPOSTREGVAR(Waves2)
	ADDPREPOSTREGVAR(Exp)
	ADDPREPOSTREGVAR(Log)
	ADDPREPOSTREGVAR(Sin)
	ADDPREPOSTREGVAR(Cos)
	ADDPREPOSTREGVAR(Tan)
	ADDPREPOSTREGVAR(Sec)
	ADDPREPOSTREGVAR(Csc)
	ADDPREPOSTREGVAR(Cot)
	ADDPREPOSTREGVAR(Sinh)
	ADDPREPOSTREGVAR(Cosh)
	ADDPREPOSTREGVAR(Tanh)
	ADDPREPOSTREGVAR(Sech)
	ADDPREPOSTREGVAR(Csch)
	ADDPREPOSTREGVAR(Coth)
	ADDPREPOSTREGVAR(Auger)
	ADDPREPOSTREGVAR(Flux)
	ADDPREPOSTREGVAR(Hemisphere)
	ADDPREPOSTREGVAR(Epispiral)
	ADDPREPOSTREGVAR(Bwraps)
	ADDPREPOSTREGVAR(BlurCircle)
	ADDPREPOSTREGVAR(BlurZoom)
	ADDPREPOSTREGVAR(BlurPixelize)
	ADDPREPOSTREGVAR(Crop)
	ADDPREPOSTREGVAR(BCircle)
	ADDPREPOSTREGVAR(BlurLinear)
	ADDPREPOSTREGVAR(BlurSquare)
	ADDPREPOSTREGVAR(Boarders2)
	ADDPREPOSTREGVAR(Cardioid)
	ADDPREPOSTREGVAR(Checks)
	ADDPREPOSTREGVAR(Circlize)
	ADDPREPOSTREGVAR(Circlize2)
	ADDPREPOSTREGVAR(CosWrap)
	ADDPREPOSTREGVAR(DeltaA)
	ADDPREPOSTREGVAR(Expo)
	ADDPREPOSTREGVAR(Extrude)
	ADDPREPOSTREGVAR(FDisc)
	ADDPREPOSTREGVAR(Fibonacci)
	ADDPREPOSTREGVAR(Fibonacci2)
	ADDPREPOSTREGVAR(Glynnia)
	ADDPREPOSTREGVAR(GridOut)
	ADDPREPOSTREGVAR(Hole)
	ADDPREPOSTREGVAR(Hypertile)
	ADDPREPOSTREGVAR(Hypertile1)
	ADDPREPOSTREGVAR(Hypertile2)
	ADDPREPOSTREGVAR(Hypertile3D)
	ADDPREPOSTREGVAR(Hypertile3D1)
	ADDPREPOSTREGVAR(Hypertile3D2)
	ADDPREPOSTREGVAR(IDisc)
	ADDPREPOSTREGVAR(Julian2)
	ADDPREPOSTREGVAR(JuliaQ)
	ADDPREPOSTREGVAR(Murl)
	ADDPREPOSTREGVAR(Murl2)
	ADDPREPOSTREGVAR(NPolar)
	ADDPREPOSTREGVAR(Ortho)
	ADDPREPOSTREGVAR(Poincare)
	ADDPREPOSTREGVAR(Poincare3D)
	ADDPREPOSTREGVAR(Polynomial)
	ADDPREPOSTREGVAR(PSphere)
	ADDPREPOSTREGVAR(Rational3)
	ADDPREPOSTREGVAR(Ripple)
	ADDPREPOSTREGVAR(Sigmoid)
	ADDPREPOSTREGVAR(SinusGrid)
	ADDPREPOSTREGVAR(Stwin)
	ADDPREPOSTREGVAR(TwoFace)
	ADDPREPOSTREGVAR(Unpolar)
	ADDPREPOSTREGVAR(WavesN)
	ADDPREPOSTREGVAR(XHeart)
	ADDPREPOSTREGVAR(Barycentroid)
	ADDPREPOSTREGVAR(BiSplit)
	ADDPREPOSTREGVAR(Crescents)
	ADDPREPOSTREGVAR(Mask)
	ADDPREPOSTREGVAR(Cpow2)
	ADDPREPOSTREGVAR(Curl3D)
	ADDPREPOSTREGVAR(Disc3D)
	ADDPREPOSTREGVAR(Funnel)
	ADDPREPOSTREGVAR(Linear3D)
	ADDPREPOSTREGVAR(PowBlock)
	ADDPREPOSTREGVAR(Squirrel)
	ADDPREPOSTREGVAR(Ennepers)
	ADDPREPOSTREGVAR(SphericalN)
	ADDPREPOSTREGVAR(Kaleidoscope)
	ADDPREPOSTREGVAR(GlynnSim1)
	ADDPREPOSTREGVAR(GlynnSim2)
	ADDPREPOSTREGVAR(GlynnSim3)
	ADDPREPOSTREGVAR(Starblur)
	ADDPREPOSTREGVAR(Sineblur)
	ADDPREPOSTREGVAR(Circleblur)
	ADDPREPOSTREGVAR(CropN)
	ADDPREPOSTREGVAR(ShredRad)
	ADDPREPOSTREGVAR(Blob2)
	ADDPREPOSTREGVAR(Julia3D)
	ADDPREPOSTREGVAR(Julia3Dz)
	ADDPREPOSTREGVAR(LinearT)
	ADDPREPOSTREGVAR(LinearT3D)
	ADDPREPOSTREGVAR(Ovoid)
	ADDPREPOSTREGVAR(Ovoid3D)
	ADDPREPOSTREGVAR(Spirograph)
	ADDPREPOSTREGVAR(Petal)
	ADDPREPOSTREGVAR(RoundSpher)
	ADDPREPOSTREGVAR(RoundSpher3D)
	ADDPREPOSTREGVAR(SpiralWing)
	ADDPREPOSTREGVAR(Squarize)
	ADDPREPOSTREGVAR(Sschecks)
	ADDPREPOSTREGVAR(PhoenixJulia)
	ADDPREPOSTREGVAR(Mobius)
	ADDPREPOSTREGVAR(MobiusN)
	ADDPREPOSTREGVAR(MobiusStrip)
	ADDPREPOSTREGVAR(Lissajous)
	ADDPREPOSTREGVAR(Svf)
	ADDPREPOSTREGVAR(Target)
	ADDPREPOSTREGVAR(Taurus)
	ADDPREPOSTREGVAR(Collideoscope)
	ADDPREPOSTREGVAR(BMod)
	ADDPREPOSTREGVAR(BSwirl)
	ADDPREPOSTREGVAR(BTransform)
	ADDPREPOSTREGVAR(BCollide)
	ADDPREPOSTREGVAR(Eclipse)
	ADDPREPOSTREGVAR(FlipCircle)
	ADDPREPOSTREGVAR(FlipY)
	ADDPREPOSTREGVAR(ECollide)
	ADDPREPOSTREGVAR(EJulia)
	ADDPREPOSTREGVAR(EMod)
	ADDPREPOSTREGVAR(EMotion)
	ADDPREPOSTREGVAR(EPush)
	ADDPREPOSTREGVAR(ERotate)
	ADDPREPOSTREGVAR(EScale)
	ADDPREPOSTREGVAR(ESwirl)
	ADDPREPOSTREGVAR(LazyTravis)
	ADDPREPOSTREGVAR(Squish)
	ADDPREPOSTREGVAR(Circus)
	ADDPREPOSTREGVAR(Tancos)
	ADDPREPOSTREGVAR(Rippled)
	ADDPREPOSTREGVAR(RotateX)
	ADDPREPOSTREGVAR(RotateY)
	ADDPREPOSTREGVAR(RotateZ)
	ADDPREPOSTREGVAR(Flatten)
	ADDPREPOSTREGVAR(Zblur)
	ADDPREPOSTREGVAR(Blur3D)
	ADDPREPOSTREGVAR(ZScale)
	ADDPREPOSTREGVAR(ZTranslate)
	ADDPREPOSTREGVAR(ZCone)
	ADDPREPOSTREGVAR(MirrorX)
	ADDPREPOSTREGVAR(MirrorY)
	ADDPREPOSTREGVAR(MirrorZ)
	ADDPREPOSTREGVAR(Depth)
	ADDPREPOSTREGVAR(Spherical3D)
	ADDPREPOSTREGVAR(RBlur)
	ADDPREPOSTREGVAR(JuliaNab)
	ADDPREPOSTREGVAR(Sintrange)
	ADDPREPOSTREGVAR(Voron)
	ADDPREPOSTREGVAR(Waffle)
	ADDPREPOSTREGVAR(Square3D)
	ADDPREPOSTREGVAR(SuperShape3D)
	ADDPREPOSTREGVAR(Sphyp3D)
	ADDPREPOSTREGVAR(Circlecrop)
	ADDPREPOSTREGVAR(Julian3Dx)
	ADDPREPOSTREGVAR(Fourth)
	ADDPREPOSTREGVAR(Mobiq)
	ADDPREPOSTREGVAR(Spherivoid)
	ADDPREPOSTREGVAR(Farblur)
	ADDPREPOSTREGVAR(CurlSP)
	ADDPREPOSTREGVAR(Heat)
	ADDPREPOSTREGVAR(Interference2)
	ADDPREPOSTREGVAR(Sinq)
	ADDPREPOSTREGVAR(Sinhq)
	ADDPREPOSTREGVAR(Secq)
	ADDPREPOSTREGVAR(Sechq)
	ADDPREPOSTREGVAR(Tanq)
	ADDPREPOSTREGVAR(Tanhq)
	ADDPREPOSTREGVAR(Cosq)
	ADDPREPOSTREGVAR(Coshq)
	ADDPREPOSTREGVAR(Cotq)
	ADDPREPOSTREGVAR(Cothq)
	ADDPREPOSTREGVAR(Cscq)
	ADDPREPOSTREGVAR(Cschq)
	ADDPREPOSTREGVAR(Estiq)
	ADDPREPOSTREGVAR(Loq)
	ADDPREPOSTREGVAR(Curvature)
	ADDPREPOSTREGVAR(Qode)
	ADDPREPOSTREGVAR(BlurHeart)
	ADDPREPOSTREGVAR(Truchet)
	ADDPREPOSTREGVAR(Gdoffs)
	ADDPREPOSTREGVAR(Octagon)
	ADDPREPOSTREGVAR(Trade)
	ADDPREPOSTREGVAR(Juliac)
	ADDPREPOSTREGVAR(Blade3D)
	ADDPREPOSTREGVAR(Blob3D)
	ADDPREPOSTREGVAR(Blocky)
	ADDPREPOSTREGVAR(Bubble2)
	ADDPREPOSTREGVAR(CircleLinear)
	ADDPREPOSTREGVAR(CircleRand)
	ADDPREPOSTREGVAR(CircleTrans1)
	ADDPREPOSTREGVAR(Cubic3D)
	ADDPREPOSTREGVAR(CubicLattice3D)
	ADDPREPOSTREGVAR(Foci3D)
	ADDPREPOSTREGVAR(Ho)
	ADDPREPOSTREGVAR(Julia3Dq)
	ADDPREPOSTREGVAR(Line)
	ADDPREPOSTREGVAR(Loonie2)
	ADDPREPOSTREGVAR(Loonie3)
	ADDPREPOSTREGVAR(Loonie3D)
	ADDPREPOSTREGVAR(Mcarpet)
	ADDPREPOSTREGVAR(Waves23D)
	ADDPREPOSTREGVAR(Pie3D)
	ADDPREPOSTREGVAR(Popcorn23D)
	ADDPREPOSTREGVAR(Sinusoidal3D)
	ADDPREPOSTREGVAR(Scry3D)
	ADDPREPOSTREGVAR(Shredlin)
	ADDPREPOSTREGVAR(SplitBrdr)
	ADDPREPOSTREGVAR(Wdisc)
	ADDPREPOSTREGVAR(Falloff)
	ADDPREPOSTREGVAR(Falloff2)
	ADDPREPOSTREGVAR(Falloff3)
	ADDPREPOSTREGVAR(Xtrb)
	ADDPREPOSTREGVAR(Hexaplay3D)
	ADDPREPOSTREGVAR(Hexnix3D)
	ADDPREPOSTREGVAR(Hexcrop)
	ADDPREPOSTREGVAR(Hexes)
	ADDPREPOSTREGVAR(Nblur)
	ADDPREPOSTREGVAR(Octapol)
	ADDPREPOSTREGVAR(Crob)
	ADDPREPOSTREGVAR(BubbleT3D)
	ADDPREPOSTREGVAR(Synth)
	ADDPREPOSTREGVAR(Crackle)
	m_Variations.push_back(new PostSmartcropVariation<T>());//Post only
	ADDPREPOSTREGVAR(Xerf)
	ADDPREPOSTREGVAR(Erf)
	ADDPREPOSTREGVAR(W)
	ADDPREPOSTREGVAR(X)
	ADDPREPOSTREGVAR(Y)
	ADDPREPOSTREGVAR(Z)
	ADDPREPOSTREGVAR(Splits3D)
	ADDPREPOSTREGVAR(Waves2B)
	ADDPREPOSTREGVAR(JacCn)
	ADDPREPOSTREGVAR(JacDn)
	ADDPREPOSTREGVAR(JacSn)
	ADDPREPOSTREGVAR(PressureWave)
	ADDPREPOSTREGVAR(Gamma)
	ADDPREPOSTREGVAR(PRose3D)
	ADDPREPOSTREGVAR(LogDB)
	ADDPREPOSTREGVAR(CircleSplit)
	ADDPREPOSTREGVAR(Cylinder2)
	ADDPREPOSTREGVAR(TileLog)
	ADDPREPOSTREGVAR(TruchetFill)
	ADDPREPOSTREGVAR(Waves2Radial)
	ADDPREPOSTREGVAR(Panorama1)
	ADDPREPOSTREGVAR(Panorama2)
	ADDPREPOSTREGVAR(Helicoid)
	ADDPREPOSTREGVAR(Helix)
	ADDPREPOSTREGVAR(Sphereblur)
	ADDPREPOSTREGVAR(Cpow3)
	ADDPREPOSTREGVAR(Concentric)
	ADDPREPOSTREGVAR(Hypercrop)
	ADDPREPOSTREGVAR(Hypershift2)
	//ADDPREPOSTREGVAR(LinearXZ)
	//ADDPREPOSTREGVAR(LinearYZ)
	//DC are special.
	ADDPREPOSTREGVAR(DCBubble)
	ADDPREPOSTREGVAR(DCCarpet)
	ADDPREPOSTREGVAR(DCCube)
	ADDPREPOSTREGVAR(DCCylinder)
	ADDPREPOSTREGVAR(DCGridOut)
	ADDPREPOSTREGVAR(DCLinear)
	ADDPREPOSTREGVAR(DCPerlin)
	ADDPREPOSTREGVAR(DCTriangle)
	ADDPREPOSTREGVAR(DCZTransl)
	ADDPREPOSTREGVAR(RandCubes)
	ADDPREPOSTREGVAR(PixelFlow)

	for (auto var : m_Variations) const_cast<Variation<T>*>(var)->Precalc();//Modify once here, then const after this.

	std::sort(m_Variations.begin(), m_Variations.end(), [&](const Variation<T>* var1, const Variation<T>* var2) { return var1->VariationId() < var2->VariationId(); });
	m_RegVariations.reserve(m_Variations.size() / 3);
	m_PreVariations.reserve(m_Variations.size() / 3);
	m_PostVariations.reserve(m_Variations.size() / 3);
	m_ParametricVariations.reserve(size_t(m_Variations.size() * .90));//This is a rough guess at how many are parametric.
	m_NonParametricVariations.reserve(size_t(m_Variations.size() * 0.20));//This is a rough guess at how many are not parametric. These don't add to 1 just to allow extra padding.

	//Place pointers to variations in vectors specific to their type.
	//Many of the elements in m_ParametricVariations will be present in the reg, pre and post vectors.
	//Note that these are not new copies, rather just pointers to the original instances in m_Variations.
	for (auto var : m_Variations)
	{
		if (var->VarType() == eVariationType::VARTYPE_REG)
			m_RegVariations.push_back(var);
		else if (var->VarType() == eVariationType::VARTYPE_PRE)
			m_PreVariations.push_back(var);
		else if (var->VarType() == eVariationType::VARTYPE_POST)
			m_PostVariations.push_back(var);

		if (auto parVar = dynamic_cast<const ParametricVariation<T>*>(var))
		{
			bool b = false;
			auto& params = parVar->ParamsVec();

			//Some non-parametric variations are actually stored as ParametricVariation<T> just to hold some precalcs.
			//So to populate parametric, check to see at least one parameter was not a precalc, if so, treat this as parametric.
			for (auto& param : params)
			{
				if (!param.IsPrecalc())
				{
					m_ParametricVariations.push_back(parVar);
					b = true;
					break;
				}
			}

			if (!b)
				m_NonParametricVariations.push_back(var);//Only get here if all parameters were non-precalc, so treat this as non-parametric.
		}
		else
			m_NonParametricVariations.push_back(var);
	}
}

/// <summary>
/// Delete each element of the list.
/// </summary>
template <typename T>
VariationList<T>::~VariationList()
{
	ClearVec(m_Variations);//No need to delete parametric because they point to the entries in original vector.
}

/// <summary>
/// Get a pointer to the variation at the specified index.
/// </summary>
/// <param name="index">The index in the list to retrieve</param>
/// <returns>A pointer to the variation at the index if in range, else nullptr.</returns>
template <typename T>
const Variation<T>* VariationList<T>::GetVariation(size_t index) const { return index < m_Variations.size() ? m_Variations[index] : nullptr; }

/// <summary>
/// Get a pointer to the variation of a specified type at the specified index.
/// </summary>
/// <param name="index">The index in the list to retrieve</param>
/// <param name="varType">The type of variation to retrieve</param>
/// <returns>A pointer to the variation of the specified type at the index if in range, else nullptr.</returns>
template <typename T>
const Variation<T>* VariationList<T>::GetVariation(size_t index, eVariationType varType) const
{
	switch (varType)
	{
		case eVariationType::VARTYPE_REG:
			return index < m_RegVariations.size() ? m_RegVariations[index] : nullptr;
			break;

		case eVariationType::VARTYPE_PRE:
			return index < m_PreVariations.size() ? m_PreVariations[index] : nullptr;
			break;

		case eVariationType::VARTYPE_POST:
			return index < m_PostVariations.size() ? m_PostVariations[index] : nullptr;
			break;

		default:
			return nullptr;
			break;
	}
}

/// <summary>
/// Get a pointer to a copy of the variation at the specified index.
/// Optionally specify a weight to assign the new copy.
/// </summary>
/// <param name="index">The index in the list to make a copy of</param>
/// <param name="weight">The weight to assign the new copy. Default: 1</param>
/// <returns>A pointer to the variation at the index if in range, else nullptr.</returns>
template <typename T>
Variation<T>* VariationList<T>::GetVariationCopy(size_t index, T weight) const { return MakeCopyWithWeight(GetVariation(index), weight); }
template <typename T>
Variation<T>* VariationList<T>::GetVariationCopy(size_t index, eVariationType varType, T weight) const { return MakeCopyWithWeight(GetVariation(index, varType), weight); }

/// <summary>
/// Get a pointer to the variation with the specified ID.
/// </summary>
/// <param name="id">The ID to search for</param>
/// <returns>A pointer to the variation if found, else nullptr.</returns>
template <typename T>
const Variation<T>* VariationList<T>::GetVariation(eVariationId id) const
{
	for (auto var : m_Variations)
		if (var && id == var->VariationId())
			return var;

	return nullptr;
}

/// <summary>
/// Get a pointer to a copy of the variation with the specified ID.
/// Optionally specify a weight to assign the new copy.
/// </summary>
/// <param name="id">The id of the variation in the list to make a copy of</param>
/// <param name="weight">The weight to assign the new copy. Default: 1</param>
/// <returns>A pointer to the variation with a matching ID, else nullptr.</returns>
template <typename T>
Variation<T>* VariationList<T>::GetVariationCopy(eVariationId id, T weight) const { return MakeCopyWithWeight(GetVariation(id), weight); }

/// <summary>
/// Get a pointer to the variation with the specified name.
/// </summary>
/// <param name="name">The name to search for</param>
/// <returns>A pointer to the variation if found, else nullptr.</returns>
template <typename T>
const Variation<T>* VariationList<T>::GetVariation(const string& name) const { return SearchVarName(m_Variations, name); }

/// <summary>
/// Get a pointer to a copy of the variation with the specified name.
/// Optionally specify a weight to assign the new copy.
/// </summary>
/// <param name="name">The name of the variation in the list to make a copy of</param>
/// <param name="weight">The weight to assign the new copy. Default: 1</param>
/// <returns>A pointer to the variation with a matching name, else nullptr.</returns>
template <typename T>
Variation<T>* VariationList<T>::GetVariationCopy(const string& name, T weight) const { return MakeCopyWithWeight(GetVariation(name), weight); }

/// <summary>
/// Get a parametric variation at the specified index.
/// Note this is the index in the parametric variations list, not in the master list.
/// </summary>
/// <param name="index">The index in the parametric variations list to retrieve</param>
/// <returns>The parametric variation at the index specified if in range, else nullptr.</returns>
template <typename T>
const ParametricVariation<T>* VariationList<T>::GetParametricVariation(size_t index) const { return index < m_ParametricVariations.size() ? m_ParametricVariations[index] : nullptr; }

/// <summary>
/// Get a parametric variation with the specified name.
/// </summary>
/// <param name="name">The name of the variation in the parametric variations list to retrieve</param>
/// <returns>The parametric variation with a matching name, else nullptr.</returns>
template <typename T>
const ParametricVariation<T>* VariationList<T>::GetParametricVariation(const string& name) const
{
	return SearchVarName(m_ParametricVariations, name);
}

/// <summary>
/// Get a pointer to a copy of the parametric variation at the specified index.
/// Optionally specify a weight to assign the new copy.
/// </summary>
/// <param name="index">The index in the list to make a copy of</param>
/// <param name="weight">The weight to assign the new copy. Default: 1</param>
/// <returns>A pointer to the parametric variation at the index if in range, else nullptr.</returns>
template <typename T>
ParametricVariation<T>* VariationList<T>::GetParametricVariationCopy(eVariationId id, T weight) const
{
	return dynamic_cast<ParametricVariation<T>*>(MakeCopyWithWeight(GetVariation(id), weight));
}

/// <summary>
/// Get a pointer to the pre variation with the specified name.
/// </summary>
/// <param name="name">The name to search for</param>
/// <returns>A pointer to the pre variation if found, else nullptr.</returns>
template <typename T>
const Variation<T>* VariationList<T>::GetPreVariation(const string& name) const { return SearchVarName(m_PreVariations, name); }

/// <summary>
/// Get a pointer to the post variation with the specified name.
/// </summary>
/// <param name="name">The name to search for</param>
/// <returns>A pointer to the post variation if found, else nullptr.</returns>
template <typename T>
const Variation<T>* VariationList<T>::GetPostVariation(const string& name) const { return SearchVarName(m_PostVariations, name); }

/// <summary>
/// Get the index of the variation with the specified name.
/// </summary>
/// <param name="name">The name of the variation whose index is returned</param>
/// <returns>The index of the variation with the matching name, else -1</returns>
template <typename T>
int VariationList<T>::GetVariationIndex(const string& name) const
{
	for (size_t i = 0; i < m_Variations.size() && m_Variations[i]; i++)
		if (!_stricmp(name.c_str(), m_Variations[i]->Name().c_str()))
			return int(i);

	return -1;
}

/// <summary>
/// Accessors.
/// </summary>
template <typename T> size_t VariationList<T>::Size() const { return m_Variations.size(); }
template <typename T> size_t VariationList<T>::RegSize() const { return m_RegVariations.size(); }
template <typename T> size_t VariationList<T>::PreSize() const { return m_PreVariations.size(); }
template <typename T> size_t VariationList<T>::PostSize() const { return m_PostVariations.size(); }
template <typename T> size_t VariationList<T>::ParametricSize() const { return m_ParametricVariations.size(); }
template <typename T> size_t VariationList<T>::NonParametricSize() const { return m_NonParametricVariations.size(); }
template <typename T> const vector<const Variation<T>*>& VariationList<T>::AllVars() const { return m_Variations; }
template <typename T> const vector<const Variation<T>*>& VariationList<T>::RegVars() const { return m_RegVariations; }
template <typename T> const vector<const Variation<T>*>& VariationList<T>::PreVars() const { return m_PreVariations; }
template <typename T> const vector<const Variation<T>*>& VariationList<T>::PostVars() const { return m_PostVariations; }
template <typename T> const vector<const Variation<T>*>& VariationList<T>::NonParametricVariations() const { return m_NonParametricVariations; }
template <typename T> const vector<const ParametricVariation<T>*>& VariationList<T>::ParametricVariations() const { return m_ParametricVariations; }

/// <summary>
/// Make a dyncamically allocated copy of a variation and assign it a specified weight.
/// Return a pointer to the new copy.
/// </summary>
/// <param name="var">The variation to copy</param>
/// <param name="weight">The weight to assign it</param>
/// <returns>A pointer to the new variation copy if success, else nullptr.</returns>
template <typename T>
Variation<T>* VariationList<T>::MakeCopyWithWeight(const Variation<T>* var, T weight) const
{
	if (var)
	{
		auto var2 = var->Copy();
		var2->m_Weight = weight;
		return var2;
	}

	return nullptr;
}

/// <summary>
/// Search the passed in container of variations for a name which matches the passed in name.
/// Note that since the elements of vars is a nested template, they can be Variation<T>*
/// or anything which derives from that.
/// </summary>
/// <param name="vars">The vector of variations to search</param>
/// <param name="name">The name to search for</param>
/// <returns>True if found, else false.</returns>
template <typename T>
template <template <typename> class U>
const U<T>* VariationList<T>::SearchVarName(const vector<const U<T>*>& vars, const string& name) const
{
	for (auto var : vars)
		if (var && !_stricmp(name.c_str(), var->Name().c_str()))
			return var;

	return nullptr;
}

//This class was implemented in a cpp file to avoid exposing so many variation classes.
//So the explicit instantiation must be declared here rather than in Ember.cpp where
//all of the other classes are done.
template EMBER_API class VariationList<float>;

#ifdef DO_DOUBLE
	template EMBER_API class VariationList<double>;
#endif
}
