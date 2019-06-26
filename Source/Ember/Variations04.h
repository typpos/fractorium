#pragma once

#include "Variation.h"

namespace EmberNs
{
/// <summary>
/// eSwirl.
/// </summary>
template <typename T>
class ESwirlVariation : public ParametricVariation<T>
{
public:
	ESwirlVariation(T weight = 1.0) : ParametricVariation<T>("eSwirl", eVariationId::VAR_ESWIRL, weight, true)
	{
		Init();
	}

	PARVARCOPY(ESwirlVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T tmp = helper.m_PrecalcSumSquares + 1;
		T tmp2 = 2 * helper.In.x;
		T xmax = (VarFuncs<T>::SafeSqrt(tmp + tmp2) + VarFuncs<T>::SafeSqrt(tmp - tmp2)) * T(0.5);
		ClampGteRef<T>(xmax, -1);
		T mu = std::acosh(xmax);
		T nu = std::acos(Clamp<T>(helper.In.x / xmax, -1, 1));//-Pi < nu < Pi.

		if (helper.In.y < 0)
			nu *= -1;

		nu = nu + mu * m_Out + m_In / mu;
		helper.Out.x = m_Weight * std::cosh(mu) * std::cos(nu);
		helper.Out.y = m_Weight * std::sinh(mu) * std::sin(nu);
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string in = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string out = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t tmp = precalcSumSquares + 1;\n"
		   << "\t\treal_t tmp2 = 2 * vIn.x;\n"
		   << "\t\treal_t xmax = (SafeSqrt(tmp + tmp2) + SafeSqrt(tmp - tmp2)) * (real_t)(0.5);\n"
		   << "\n"
		   << "\t\tif (xmax < 1)\n"
		   << "\t\t	xmax = 1;\n"
		   << "\n"
		   << "\t\treal_t mu = acosh(xmax);\n"
		   << "\t\treal_t nu = acos(clamp(vIn.x / xmax, -(real_t)(1.0), (real_t)(1.0)));\n"
		   << "\n"
		   << "\t\tif (vIn.y < 0)\n"
		   << "\t\t	nu *= -1;\n"
		   << "\n"
		   << "\t\tnu = nu + mu * " << out << " + " << in << " / mu;\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * cosh(mu) * cos(nu);\n"
		   << "\t\tvOut.y = " << weight << " * sinh(mu) * sin(nu);\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "SafeSqrt" };
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_In, prefix + "eSwirl_in"));
		m_Params.push_back(ParamWithName<T>(&m_Out, prefix + "eSwirl_out"));
	}

private:
	T m_In;
	T m_Out;
};

/// <summary>
/// lazyjess.
/// By FarDareisMai.
/// </summary>
template <typename T>
class LazyJessVariation : public ParametricVariation<T>
{
public:
	LazyJessVariation(T weight = 1.0) : ParametricVariation<T>("lazyjess", eVariationId::VAR_LAZYJESS, weight, true, true)
	{
		Init();
	}

	PARVARCOPY(LazyJessVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T theta, sina, cosa;
		T x = helper.In.x;
		T y = helper.In.y;
		T modulus = helper.m_PrecalcSqrtSumSquares;

		// n==2 requires a special case
		if (m_N == T(2))
		{
			if (std::abs(x) < m_Weight) // If the input point falls inside the designated area...
			{
				//	// ...then rotate it.
				theta = std::atan2(y, x) + m_Spin;
				sina = std::sin(theta);
				cosa = std::cos(theta);
				x = m_Weight * modulus * cosa;
				y = m_Weight * modulus * sina;

				if (std::abs(x) < m_Weight)
				{
					helper.Out.x = x;
					helper.Out.y = y;
				}
				else // If it is now part of a corner that falls outside the designated area...
				{
					// ...then flip and rotate into place.
					theta = std::atan2(y, x) - m_Spin + m_CornerRotation;
					sina = std::sin(theta);
					cosa = std::cos(theta);
					helper.Out.x = m_Weight * modulus * cosa;
					helper.Out.y = -(m_Weight * modulus * sina);
				}
			}
			else
			{
				modulus = 1 + m_Space / Zeps(modulus);
				helper.Out.x = m_Weight * modulus * x;
				helper.Out.y = m_Weight * modulus * y;
			}
		}
		else
		{
			// Calculate the distance r from origin to the edge of the polygon at the angle of the input point.
			theta = std::atan2(y, x) + M_2PI;
			T theta_diff = std::fmod(theta + m_HalfSlice, m_PieSlice);
			T r = m_Weight * T(M_SQRT2) * m_SinVertex / Zeps(std::sin(T(M_PI) - theta_diff - m_Vertex));

			if (modulus < r)
			{
				// Again, rotating points within designated area.
				theta = std::atan2(y, x) + m_Spin + M_2PI;
				sina = std::sin(theta);
				cosa = std::cos(theta);
				x = m_Weight * modulus * cosa;
				y = m_Weight * modulus * sina;
				// Calculating r and modulus for our rotated point.
				theta_diff = std::fmod(theta + m_HalfSlice, m_PieSlice);
				r = m_Weight * T(M_SQRT2) * m_SinVertex / Zeps(std::sin(T(M_PI) - theta_diff - m_Vertex));
				modulus = VarFuncs<T>::Hypot(x, y);

				if (modulus < r)
				{
					helper.Out.x = x;
					helper.Out.y = y;
				}
				else
				{
					// Again, flipping and rotating corners that fall outside the designated area.
					theta = std::atan2(y, x) - m_Spin + m_CornerRotation + M_2PI;
					sina = std::sin(theta);
					cosa = std::cos(theta);
					helper.Out.x = m_Weight * modulus * cosa;
					helper.Out.y = -(m_Weight * modulus * sina);
				}
			}
			else
			{
				modulus = 1 + m_Space / Zeps(modulus);
				helper.Out.x = m_Weight * modulus * x;
				helper.Out.y = m_Weight * modulus * y;
			}
		}

		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string n              = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string spin           = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string space          = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string corner         = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string vertex         = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string sinvertex      = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string pieslice       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string halfslice      = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cornerrotation = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t theta, sina, cosa;\n"
		   << "\t\treal_t x = vIn.x;\n"
		   << "\t\treal_t y = vIn.y;\n"
		   << "\t\treal_t modulus = precalcSqrtSumSquares;\n"
		   << "\n"
		   << "\t\tif (" << n << " == 2.0)\n"
		   << "\t\t{\n"
		   << "\t\t	if (fabs(x) < " << weight << ")\n"
		   << "\t\t	{\n"
		   << "\t\t		theta = atan2(y, x) + " << spin << ";\n"
		   << "\t\t		sina = sin(theta);\n"
		   << "\t\t		cosa = cos(theta);\n"
		   << "\t\t		x = " << weight << " * modulus * cosa;\n"
		   << "\t\t		y = " << weight << " * modulus * sina;\n"
		   << "\n"
		   << "\t\t		if (fabs(x) < " << weight << ")\n"
		   << "\t\t		{\n"
		   << "\t\t			vOut.x = x;\n"
		   << "\t\t			vOut.y = y;\n"
		   << "\t\t		}\n"
		   << "\t\t		else\n"
		   << "\t\t		{\n"
		   << "\t\t			theta = atan2(y, x) - " << spin << " + " << cornerrotation << ";\n"
		   << "\t\t			sina = sin(theta);\n"
		   << "\t\t			cosa = cos(theta);\n"
		   << "\t\t			vOut.x = " << weight << " * modulus * cosa;\n"
		   << "\t\t			vOut.y = -(" << weight << " * modulus * sina);\n"
		   << "\t\t		}\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		modulus = 1 + " << space << " / Zeps(modulus);\n"
		   << "\t\t		vOut.x = " << weight << " * modulus * x;\n"
		   << "\t\t		vOut.y = " << weight << " * modulus * y;\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	theta = atan2(y, x) + M_2PI;\n"
		   << "\t\t	real_t theta_diff = fmod(theta + " << halfslice << ", " << pieslice << ");\n"
		   << "\t\t	real_t r = " << weight << " * M_SQRT2 * " << sinvertex << " / Zeps(sin(MPI - theta_diff - " << vertex << "));\n"
		   << "\n"
		   << "\t\t	if (modulus < r)\n"
		   << "\t\t	{\n"
		   << "\t\t		theta = atan2(y, x) + " << spin << " + M_2PI;\n"
		   << "\t\t		sina = sin(theta);\n"
		   << "\t\t		cosa = cos(theta);\n"
		   << "\t\t		x = " << weight << " * modulus * cosa;\n"
		   << "\t\t		y = " << weight << " * modulus * sina;\n"
		   << "\t\t		theta_diff = fmod(theta + " << halfslice << ", " << pieslice << ");\n"
		   << "\t\t		r = " << weight << " * M_SQRT2 * " << sinvertex << " / Zeps(sin(MPI - theta_diff - " << vertex << "));\n"
		   << "\t\t		modulus = Hypot(x, y);\n"
		   << "\n"
		   << "\t\t		if (modulus < r)\n"
		   << "\t\t		{\n"
		   << "\t\t			vOut.x = x;\n"
		   << "\t\t			vOut.y = y;\n"
		   << "\t\t		}\n"
		   << "\t\t		else\n"
		   << "\t\t		{\n"
		   << "\t\t			theta = atan2(y, x) - " << spin << " + " << cornerrotation << " + M_2PI;\n"
		   << "\t\t			sina = sin(theta);\n"
		   << "\t\t			cosa = cos(theta);\n"
		   << "\t\t			vOut.x = " << weight << " * modulus * cosa;\n"
		   << "\t\t			vOut.y = -(" << weight << " * modulus * sina);\n"
		   << "\t\t		}\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		modulus = 1 + " << space << " / Zeps(modulus);\n"
		   << "\t\t		vOut.x = " << weight << " * modulus * x;\n"
		   << "\t\t		vOut.y = " << weight << " * modulus * y;\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_Vertex = T(M_PI) * (m_N - 2) / Zeps(2 * m_N);
		m_SinVertex = std::sin(m_Vertex);
		m_PieSlice = M_2PI / Zeps(m_N);
		m_HalfSlice = m_PieSlice / 2;
		m_CornerRotation = (m_Corner - 1) * m_PieSlice;
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps", "Hypot" };
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_N,                    prefix + "lazyjess_n", 4, eParamType::INTEGER_NONZERO, 2));
		m_Params.push_back(ParamWithName<T>(&m_Spin,                 prefix + "lazyjess_spin", T(M_PI), eParamType::REAL_CYCLIC, 0, M_2PI));
		m_Params.push_back(ParamWithName<T>(&m_Space,                prefix + "lazyjess_space"));
		m_Params.push_back(ParamWithName<T>(&m_Corner,               prefix + "lazyjess_corner", 1, eParamType::INTEGER_NONZERO));
		m_Params.push_back(ParamWithName<T>(true, &m_Vertex,         prefix + "lazyjess_vertex"));//Precalc.
		m_Params.push_back(ParamWithName<T>(true, &m_SinVertex,      prefix + "lazyjess_sin_vertex"));
		m_Params.push_back(ParamWithName<T>(true, &m_PieSlice,       prefix + "lazyjess_pie_slice"));
		m_Params.push_back(ParamWithName<T>(true, &m_HalfSlice,      prefix + "lazyjess_half_slice"));
		m_Params.push_back(ParamWithName<T>(true, &m_CornerRotation, prefix + "lazyjess_corner_rotation"));
	}

private:
	T m_N;
	T m_Spin;
	T m_Space;
	T m_Corner;
	T m_Vertex;//Precalc.
	T m_SinVertex;
	T m_PieSlice;
	T m_HalfSlice;
	T m_CornerRotation;
};

/// <summary>
/// lazyTravis.
/// </summary>
template <typename T>
class LazyTravisVariation : public ParametricVariation<T>
{
public:
	LazyTravisVariation(T weight = 1.0) : ParametricVariation<T>("lazyTravis", eVariationId::VAR_LAZY_TRAVIS, weight)
	{
		Init();
	}

	PARVARCOPY(LazyTravisVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T x = std::abs(helper.In.x);
		T y = std::abs(helper.In.y);
		T s;
		T p;
		T x2, y2;

		if (x > m_Weight || y > m_Weight)
		{
			if (x > y)
			{
				s = x;

				if (helper.In.x > 0)
					p = s + helper.In.y + s * m_Out4;
				else
					p = 5 * s - helper.In.y + s * m_Out4;
			}
			else
			{
				s = y;

				if (helper.In.y > 0)
					p = 3 * s - helper.In.x + s * m_Out4;
				else
					p = 7 * s + helper.In.x + s * m_Out4;
			}

			p = fmod(p, s * 8);

			if (p <= 2 * s)
			{
				x2 = s + m_Space;
				y2 = -(1 * s - p);
				y2 = y2 + y2 / s * m_Space;
			}
			else if (p <= 4 * s)
			{
				y2 = s + m_Space;
				x2 = (3 * s - p);
				x2 = x2 + x2 / s * m_Space;
			}
			else if (p <= 6 * s)
			{
				x2 = -(s + m_Space);
				y2 = (5 * s - p);
				y2 = y2 + y2 / s * m_Space;
			}
			else
			{
				y2 = -(s + m_Space);
				x2 = -(7 * s - p);
				x2 = x2 + x2 / s * m_Space;
			}

			helper.Out.x = m_Weight * x2;
			helper.Out.y = m_Weight * y2;
		}
		else
		{
			if (x > y)
			{
				s = x;

				if (helper.In.x > 0)
					p = s + helper.In.y + s * m_In4;
				else
					p = 5 * s - helper.In.y + s * m_In4;
			}
			else
			{
				s = y;

				if (helper.In.y > 0)
					p = 3 * s - helper.In.x + s * m_In4;
				else
					p = 7 * s + helper.In.x + s * m_In4;
			}

			p = fmod(p, s * 8);

			if (p <= 2 * s)
			{
				helper.Out.x = m_Weight * s;
				helper.Out.y = -(m_Weight * (s - p));
			}
			else if (p <= 4 * s)
			{
				helper.Out.x = m_Weight * (3 * s - p);
				helper.Out.y = m_Weight * s;
			}
			else if (p <= 6 * s)
			{
				helper.Out.x = -(m_Weight * s);
				helper.Out.y = m_Weight * (5 * s - p);
			}
			else
			{
				helper.Out.x = -(m_Weight * (7 * s - p));
				helper.Out.y = -(m_Weight * s);
			}
		}

		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string spinIn  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string spinOut = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string space   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string in4     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string out4    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t x = fabs(vIn.x);\n"
		   << "\t\treal_t y = fabs(vIn.y);\n"
		   << "\t\treal_t s;\n"
		   << "\t\treal_t p;\n"
		   << "\t\treal_t x2, y2;\n"
		   << "\n"
		   << "\t\tif (x > " << weight << " || y > " << weight << ")\n"
		   << "\t\t{\n"
		   << "\t\t	if (x > y)\n"
		   << "\t\t	{\n"
		   << "\t\t		s = x;\n"
		   << "\n"
		   << "\t\t		if (vIn.x > 0)\n"
		   << "\t\t			p = fma(s, " << out4 << ", s + vIn.y);\n"
		   << "\t\t		else\n"
		   << "\t\t			p = fma((real_t)(5.0), s, fma(s, " << out4 << ", -vIn.y));\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		s = y;\n"
		   << "\n"
		   << "\t\t		if (vIn.y > 0)\n"
		   << "\t\t			p = fma((real_t)(3.0), s, fma(s, " << out4 << ", -vIn.x));\n"
		   << "\t\t		else\n"
		   << "\t\t			p = fma((real_t)(7.0), s, fma(s, " << out4 << ", vIn.x));\n"
		   << "\t\t	}\n"
		   << "\n"
		   << "\t\t	p = fmod(p, s * 8);\n"
		   << "\n"
		   << "\t\t	if (p <= 2 * s)\n"
		   << "\t\t	{\n"
		   << "\t\t		x2 = s + " << space << ";\n"
		   << "\t\t		y2 = -fma((real_t)(1.0), s, -p);\n"
		   << "\t\t		y2 = y2 + y2 / s * " << space << ";\n"
		   << "\t\t	}\n"
		   << "\t\t	else if (p <= 4 * s)\n"
		   << "\t\t	{\n"
		   << "\t\t		y2 = s + " << space << ";\n"
		   << "\t\t		x2 = fma((real_t)(3.0), s, -p);\n"
		   << "\t\t		x2 = x2 + x2 / s * " << space << ";\n"
		   << "\t\t	}\n"
		   << "\t\t	else if (p <= 6 * s)\n"
		   << "\t\t	{\n"
		   << "\t\t		x2 = -(s + " << space << ");\n"
		   << "\t\t		y2 = fma((real_t)(5.0), s, -p);\n"
		   << "\t\t		y2 = y2 + y2 / s * " << space << ";\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		y2 = -(s + " << space << ");\n"
		   << "\t\t		x2 = -fma((real_t)(7.0), s, -p);\n"
		   << "\t\t		x2 = x2 + x2 / s * " << space << ";\n"
		   << "\t\t	}\n"
		   << "\n"
		   << "\t\t	vOut.x = " << weight << " * x2;\n"
		   << "\t\t	vOut.y = " << weight << " * y2;\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	if (x > y)\n"
		   << "\t\t	{\n"
		   << "\t\t		s = x;\n"
		   << "\n"
		   << "\t\t		if (vIn.x > 0)\n"
		   << "\t\t			p = fma(s, " << in4 << ", s + vIn.y);\n"
		   << "\t\t		else\n"
		   << "\t\t			p = fma((real_t)(5.0), s, fma(s, " << in4 << ", -vIn.y));\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		s = y;\n"
		   << "\n"
		   << "\t\t		if (vIn.y > 0)\n"
		   << "\t\t			p = fma((real_t)(3.0), s, fma(s, " << in4 << ", -vIn.x));\n"
		   << "\t\t		else\n"
		   << "\t\t			p = fma((real_t)(7.0), s, fma(s, " << in4 << ", vIn.x));\n"
		   << "\t\t	}\n"
		   << "\n"
		   << "\t\t	p = fmod(p, s * 8);\n"
		   << "\n"
		   << "\t\t	if (p <= 2 * s)\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = " << weight << " * s;\n"
		   << "\t\t		vOut.y = -(" << weight << " * (s - p));\n"
		   << "\t\t	}\n"
		   << "\t\t	else if (p <= 4 * s)\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = " << weight << " * fma((real_t)(3.0), s, -p);\n"
		   << "\t\t		vOut.y = " << weight << " * s;\n"
		   << "\t\t	}\n"
		   << "\t\t	else if (p <= 6 * s)\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = -(" << weight << " * s);\n"
		   << "\t\t		vOut.y = " << weight << " * fma((real_t)(5.0), s, -p);\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = -(" << weight << " * fma((real_t)(7.0), s, -p));\n"
		   << "\t\t		vOut.y = -(" << weight << " * s);\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_In4 = 4 * m_SpinIn;
		m_Out4 = 4 * m_SpinOut;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_SpinIn, prefix + "lazyTravis_spin_in", 1, eParamType::REAL_CYCLIC, 0, 2));
		m_Params.push_back(ParamWithName<T>(&m_SpinOut, prefix + "lazyTravis_spin_out", 0, eParamType::REAL_CYCLIC, 0, 2));
		m_Params.push_back(ParamWithName<T>(&m_Space, prefix + "lazyTravis_space"));
		m_Params.push_back(ParamWithName<T>(true, &m_In4, prefix + "lazyTravis_in4"));//Precalc.
		m_Params.push_back(ParamWithName<T>(true, &m_Out4, prefix + "lazyTravis_out4"));
	}

private:
	T m_SpinIn;
	T m_SpinOut;
	T m_Space;
	T m_In4;//Precalc.
	T m_Out4;
};

/// <summary>
/// squish.
/// </summary>
template <typename T>
class SquishVariation : public ParametricVariation<T>
{
public:
	SquishVariation(T weight = 1.0) : ParametricVariation<T>("squish", eVariationId::VAR_SQUISH, weight)
	{
		Init();
	}

	PARVARCOPY(SquishVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T x = std::abs(helper.In.x);
		T y = std::abs(helper.In.y);
		T s;
		T p;

		if (x > y)
		{
			s = x;

			if (helper.In.x > 0)
				p = helper.In.y;
			else
				p = 4 * s - helper.In.y;
		}
		else
		{
			s = y;

			if (helper.In.y > 0)
				p = 2 * s - helper.In.x;
			else
				p = 6 * s + helper.In.x;
		}

		p = m_InvPower * (p + 8 * s * Floor<T>(m_Power * rand.Frand01<T>()));

		if (p <= s)
		{
			helper.Out.x = m_Weight * s;
			helper.Out.y = m_Weight * p;
		}
		else if (p <= 3 * s)
		{
			helper.Out.x = m_Weight * (2 * s - p);
			helper.Out.y = m_Weight * s;
		}
		else if (p <= 5 * s)
		{
			helper.Out.x = -(m_Weight * s);
			helper.Out.y = m_Weight * (4 * s - p);
		}
		else if (p <= 7 * s)
		{
			helper.Out.x = -(m_Weight * (6 * s - p));
			helper.Out.y = -(m_Weight * s);
		}
		else
		{
			helper.Out.x = m_Weight * s;
			helper.Out.y = (m_Weight * (8 * s - p));
		}

		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string power    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string invPower = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t x = fabs(vIn.x);\n"
		   << "\t\treal_t y = fabs(vIn.y);\n"
		   << "\t\treal_t s;\n"
		   << "\t\treal_t p;\n"
		   << "\n"
		   << "\t\tif (x > y)\n"
		   << "\t\t{\n"
		   << "\t\t	s = x;\n"
		   << "\n"
		   << "\t\t	if (vIn.x > 0)\n"
		   << "\t\t		p = vIn.y;\n"
		   << "\t\t	else\n"
		   << "\t\t		p = fma((real_t)(4.0), s, -vIn.y);\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	s = y;\n"
		   << "\n"
		   << "\t\t	if (vIn.y > 0)\n"
		   << "\t\t		p = fma((real_t)(2.0), s, -vIn.x);\n"
		   << "\t\t	else\n"
		   << "\t\t		p = fma((real_t)(6.0), s, vIn.x);\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tp = " << invPower << " * fma((real_t)(8.0), s * floor(" << power << " * MwcNext01(mwc)), p);\n"
		   << "\n"
		   << "\t\tif (p <= s)\n"
		   << "\t\t{\n"
		   << "\t\t	vOut.x = " << weight << " * s;\n"
		   << "\t\t	vOut.y = " << weight << " * p;\n"
		   << "\t\t}\n"
		   << "\t\telse if (p <= 3 * s)\n"
		   << "\t\t{\n"
		   << "\t\t	vOut.x = " << weight << " * fma((real_t)(2.0), s, -p);\n"
		   << "\t\t	vOut.y = " << weight << " * s;\n"
		   << "\t\t}\n"
		   << "\t\telse if (p <= 5 * s)\n"
		   << "\t\t{\n"
		   << "\t\t	vOut.x = -(" << weight << " * s);\n"
		   << "\t\t	vOut.y = " << weight << " * fma((real_t)(4.0), s, -p);\n"
		   << "\t\t}\n"
		   << "\t\telse if (p <= 7 * s)\n"
		   << "\t\t{\n"
		   << "\t\t	vOut.x = -(" << weight << " * fma((real_t)(6.0), s, -p));\n"
		   << "\t\t	vOut.y = -(" << weight << " * s);\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	vOut.x = " << weight << " * s;\n"
		   << "\t\t	vOut.y = -(" << weight << " * fma((real_t)(8.0), s, -p));\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_InvPower = 1 / m_Power;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Power, prefix + "squish_power", 2, eParamType::INTEGER, 2, T(INT_MAX)));
		m_Params.push_back(ParamWithName<T>(true, &m_InvPower, prefix + "squish_inv_power"));//Precalc.
	}

private:
	T m_Power;
	T m_InvPower;//Precalc.
};

/// <summary>
/// circus.
/// </summary>
template <typename T>
class CircusVariation : public ParametricVariation<T>
{
public:
	CircusVariation(T weight = 1.0) : ParametricVariation<T>("circus", eVariationId::VAR_CIRCUS, weight, true, true, true)
	{
		Init();
	}

	PARVARCOPY(CircusVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T r = helper.m_PrecalcSqrtSumSquares;

		if (r <= 1)
			r *= m_Scale;
		else
			r *= m_InvScale;

		helper.Out.x = m_Weight * r * helper.m_PrecalcCosa;
		helper.Out.y = m_Weight * r * helper.m_PrecalcSina;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string scale = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string invScale = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t r = precalcSqrtSumSquares;\n"
		   << "\n"
		   << "\t\tif (r <= 1)\n"
		   << "\t\t	r *= " << scale << ";\n"
		   << "\t\telse\n"
		   << "\t\t	r *= " << invScale << ";\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * r * precalcCosa;\n"
		   << "\t\tvOut.y = " << weight << " * r * precalcSina;\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_InvScale = 1 / m_Scale;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Scale, prefix + "circus_scale", 1));
		m_Params.push_back(ParamWithName<T>(true, &m_InvScale, prefix + "circus_inv_power"));//Precalc.
	}

private:
	T m_Scale;
	T m_InvScale;//Precalc.
};

/// <summary>
/// tancos.
/// </summary>
template <typename T>
class TancosVariation : public Variation<T>
{
public:
	TancosVariation(T weight = 1.0) : Variation<T>("tancos", eVariationId::VAR_TANCOS, weight, true) { }

	VARCOPY(TancosVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T d = Zeps(helper.m_PrecalcSumSquares);
		helper.Out.x = (m_Weight / d) * (std::tanh(d) * (2 * helper.In.x));
		helper.Out.y = (m_Weight / d) * (std::cos(d)  * (2 * helper.In.y));
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t d = Zeps(precalcSumSquares);\n"
		   << "\n"
		   << "\t\tvOut.x = (" << weight << " / d) * (tanh(d) * ((real_t)(2.0) * vIn.x));\n"
		   << "\t\tvOut.y = (" << weight << " / d) * (cos(d)  * ((real_t)(2.0) * vIn.y));\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps" };
	}
};

/// <summary>
/// rippled.
/// </summary>
template <typename T>
class RippledVariation : public Variation<T>
{
public:
	RippledVariation(T weight = 1.0) : Variation<T>("rippled", eVariationId::VAR_RIPPLED, weight, true) { }

	VARCOPY(RippledVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T d = Zeps(helper.m_PrecalcSumSquares);
		helper.Out.x = (m_Weight / 2) * (std::tanh(d) * (2 * helper.In.x));
		helper.Out.y = (m_Weight / 2) * (std::cos(d)  * (2 * helper.In.y));
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t d = Zeps(precalcSumSquares);\n"
		   << "\n"
		   << "\t\tvOut.x = (" << weight << " / (real_t)(2.0)) * (tanh(d) * ((real_t)(2.0) * vIn.x));\n"
		   << "\t\tvOut.y = (" << weight << " / (real_t)(2.0)) * (cos(d)  * ((real_t)(2.0) * vIn.y));\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps" };
	}
};

/// <summary>
/// RotateX.
/// This uses in/out in a rare and different way.
/// </summary>
template <typename T>
class RotateXVariation : public ParametricVariation<T>
{
public:
	RotateXVariation(T weight = 1.0) : ParametricVariation<T>("rotate_x", eVariationId::VAR_ROTATE_X, weight)
	{
		Init();
	}

	PARVARCOPY(RotateXVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T z = m_RxCos * helper.In.z - m_RxSin * helper.In.y;

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			helper.Out.x = helper.In.x;
			outPoint.m_X = 0;
		}
		else
		{
			helper.Out.x = helper.In.x;
		}

		helper.Out.y = m_RxSin * helper.In.z + m_RxCos * helper.In.y;
		helper.Out.z = z;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		int i = 0;
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string rxSin = "parVars[" + ToUpper(m_Params[i++].Name()) + index;//Precalcs only, no params.
		string rxCos = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t z = fma(" << rxCos << ", vIn.z, -(" << rxSin << " * vIn.y));\n"
		   << "\n";

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			ss <<
			   "\t\tvOut.x = 0;\n"
			   "\t\toutPoint->m_X = vIn.x;\n";
		}
		else
		{
			ss <<
			   "\t\tvOut.x = vIn.x;\n";
		}

		ss << "\t\tvOut.y = fma(" << rxSin << ", vIn.z, " << rxCos << " * vIn.y);\n"
		   << "\t\tvOut.z = z;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_RxSin = std::sin(m_Weight * T(M_PI_2));
		m_RxCos = std::cos(m_Weight * T(M_PI_2));
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(true, &m_RxSin, prefix + "rotate_x_sin"));//Precalcs only, no params.
		m_Params.push_back(ParamWithName<T>(true, &m_RxCos, prefix + "rotate_x_cos"));//Original used a prefix of rx_, which is incompatible with Ember's design.
	}

private:
	T m_RxSin;
	T m_RxCos;
};

/// <summary>
/// RotateY.
/// This uses in/out in a rare and different way.
/// </summary>
template <typename T>
class RotateYVariation : public ParametricVariation<T>
{
public:
	RotateYVariation(T weight = 1.0) : ParametricVariation<T>("rotate_y", eVariationId::VAR_ROTATE_Y, weight)
	{
		Init();
	}

	PARVARCOPY(RotateYVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		helper.Out.x = m_RyCos * helper.In.x - m_RySin * helper.In.z;

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			helper.Out.y = 0;
			outPoint.m_Y = helper.In.y;
		}
		else
		{
			helper.Out.y = helper.In.y;
		}

		helper.Out.z = m_RySin * helper.In.x + m_RyCos * helper.In.z;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		int i = 0;
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string rySin = "parVars[" + ToUpper(m_Params[i++].Name()) + index;//Precalcs only, no params.
		string ryCos = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\tvOut.x = fma(" << ryCos << ", vIn.x, -(" << rySin << " * vIn.z));\n";

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			ss <<
			   "\t\tvOut.y = 0;\n"
			   "\t\toutPoint->m_Y = vIn.y;\n";
		}
		else
		{
			ss <<
			   "\t\tvOut.y = vIn.y;\n";
		}

		ss << "\t\tvOut.z = fma(" << rySin << ", vIn.x, " << ryCos << " * vIn.z);\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_RySin = std::sin(m_Weight * T(M_PI_2));
		m_RyCos = std::cos(m_Weight * T(M_PI_2));
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(true, &m_RySin, prefix + "rotate_y_sin"));//Precalcs only, no params.
		m_Params.push_back(ParamWithName<T>(true, &m_RyCos, prefix + "rotate_y_cos"));//Original used a prefix of ry_, which is incompatible with Ember's design.
	}

private:
	T m_RySin;
	T m_RyCos;
};

/// <summary>
/// RotateZ.
/// This was originally pre and post spin_z, consolidated here to be consistent with the other rotate variations.
/// </summary>
template <typename T>
class RotateZVariation : public ParametricVariation<T>
{
public:
	RotateZVariation(T weight = 1.0) : ParametricVariation<T>("rotate_z", eVariationId::VAR_ROTATE_Z, weight)
	{
		Init();
	}

	PARVARCOPY(RotateZVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		helper.Out.x = m_RzSin * helper.In.y + m_RzCos * helper.In.x;
		helper.Out.y = m_RzCos * helper.In.y - m_RzSin * helper.In.x;

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			helper.Out.z = helper.In.z;
			outPoint.m_Z = 0;
		}
		else
		{
			helper.Out.z = helper.In.z;
		}
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		int i = 0;
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string rzSin = "parVars[" + ToUpper(m_Params[i++].Name()) + index;//Precalcs only, no params.
		string rzCos = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\tvOut.x = fma(" << rzSin << ", vIn.y, " << rzCos << " * vIn.x);\n"
		   << "\t\tvOut.y = fma(" << rzCos << ", vIn.y, -(" << rzSin << " * vIn.x));\n";

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			ss <<
			   "\t\tvOut.z = 0;\n"
			   "\t\toutPoint->m_Z = vIn.z;\n";
		}
		else
		{
			ss <<
			   "\t\tvOut.z = vIn.z;\n";
		}

		ss << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_RzSin = std::sin(m_Weight * T(M_PI_2));
		m_RzCos = std::cos(m_Weight * T(M_PI_2));
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(true, &m_RzSin, prefix + "rotate_z_sin"));//Precalcs only, no params.
		m_Params.push_back(ParamWithName<T>(true, &m_RzCos, prefix + "rotate_z_cos"));
	}

private:
	T m_RzSin;
	T m_RzCos;
};

/// <summary>
/// MirrorX.
/// This uses in/out in a rare and different way.
/// </summary>
template <typename T>
class MirrorXVariation : public Variation<T>
{
public:
	MirrorXVariation(T weight = 1.0) : Variation<T>("mirror_x", eVariationId::VAR_MIRROR_X, weight) { }

	VARCOPY(MirrorXVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		helper.Out.x = std::abs(helper.In.x);

		if (rand.RandBit())
			helper.Out.x = -helper.Out.x;

		helper.Out.y = helper.In.y;
		helper.Out.z = helper.In.z;

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			outPoint.m_X = 0;//All will be added.
			outPoint.m_Y = 0;
			outPoint.m_Z = 0;
		}
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		ss << "\t{\n"
		   "\t\tvOut.x = fabs(vIn.x);\n"
		   "\n"
		   "\t\tif (MwcNext(mwc) & 1)\n"
		   "\t\t	vOut.x = -vOut.x;\n"
		   "\n"
		   "\t\tvOut.y = vIn.y;\n"
		   "\t\tvOut.z = vIn.z;\n";

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			ss <<
			   "\t\toutPoint->m_X = 0;\n"
			   "\t\toutPoint->m_Y = 0;\n"
			   "\t\toutPoint->m_Z = 0;\n";
		}

		ss << "\t}\n";
		return ss.str();
	}
};

/// <summary>
/// MirrorY.
/// This uses in/out in a rare and different way.
/// </summary>
template <typename T>
class MirrorYVariation : public Variation<T>
{
public:
	MirrorYVariation(T weight = 1.0) : Variation<T>("mirror_y", eVariationId::VAR_MIRROR_Y, weight) { }

	VARCOPY(MirrorYVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		helper.Out.y = std::abs(helper.In.y);

		if (rand.RandBit())
			helper.Out.y = -helper.Out.y;

		helper.Out.x = helper.In.x;
		helper.Out.z = helper.In.z;

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			outPoint.m_X = 0;//All will be added.
			outPoint.m_Y = 0;
			outPoint.m_Z = 0;
		}
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		ss << "\t{\n"
		   "\t\tvOut.y = fabs(vIn.y);\n"
		   "\n"
		   "\t\tif (MwcNext(mwc) & 1)\n"
		   "\t\t	vOut.y = -vOut.y;\n"
		   "\n"
		   "\t\tvOut.x = vIn.x;\n"
		   "\t\tvOut.z = vIn.z;\n";

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			ss <<
			   "\t\toutPoint->m_X = 0;\n"
			   "\t\toutPoint->m_Y = 0;\n"
			   "\t\toutPoint->m_Z = 0;\n";
		}

		ss << "\t}\n";
		return ss.str();
	}
};

/// <summary>
/// MirrorZ.
/// This uses in/out in a rare and different way.
/// </summary>
template <typename T>
class MirrorZVariation : public Variation<T>
{
public:
	MirrorZVariation(T weight = 1.0) : Variation<T>("mirror_z", eVariationId::VAR_MIRROR_Z, weight) { }

	VARCOPY(MirrorZVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		helper.Out.z = std::abs(helper.In.z);

		if (rand.RandBit())
			helper.Out.z = -helper.Out.z;

		helper.Out.x = helper.In.x;
		helper.Out.y = helper.In.y;

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			outPoint.m_X = 0;//All will be added.
			outPoint.m_Y = 0;
			outPoint.m_Z = 0;
		}
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		ss << "\t{\n"
		   "\t\tvOut.z = fabs(vIn.z);\n"
		   "\n"
		   "\t\tif (MwcNext(mwc) & 1)\n"
		   "\t\t	vOut.z = -vOut.z;\n"
		   "\n"
		   "\t\tvOut.x = vIn.x;\n"
		   "\t\tvOut.y = vIn.y;\n";

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			ss <<
			   "\t\toutPoint->m_X = 0;\n"
			   "\t\toutPoint->m_Y = 0;\n"
			   "\t\toutPoint->m_Z = 0;\n";
		}

		ss << "\t}\n";
		return ss.str();
	}
};

/// <summary>
/// RBlur.
/// </summary>
template <typename T>
class RBlurVariation : public ParametricVariation<T>
{
public:
	RBlurVariation(T weight = 1.0) : ParametricVariation<T>("rblur", eVariationId::VAR_RBLUR, weight)
	{
		Init();
	}

	PARVARCOPY(RBlurVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T sx = helper.In.x - m_CenterX;
		T sy = helper.In.y - m_CenterY;
		T r = std::sqrt(SQR(sx) + SQR(sy)) - m_Offset;
		r = r < 0 ? 0 : r;
		r *= m_S2;
		helper.Out.x = m_Weight * (helper.In.x + (rand.Frand01<T>() - T(0.5)) * r);
		helper.Out.y = m_Weight * (helper.In.y + (rand.Frand01<T>() - T(0.5)) * r);
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string strength = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string offset   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string centerX  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string centerY  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string s2       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t sx = vIn.x - " << centerX << ";\n"
		   << "\t\treal_t sy = vIn.y - " << centerY << ";\n"
		   << "\t\treal_t r = sqrt(fma(sx, sx, SQR(sy))) - " << offset << ";\n"
		   << "\n"
		   << "\t\tr = r < 0 ? 0 : r;\n"
		   << "\t\tr *= " << s2 << ";\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * fma(MwcNext01(mwc) - (real_t)(0.5), r, vIn.x);\n"
		   << "\t\tvOut.y = " << weight << " * fma(MwcNext01(mwc) - (real_t)(0.5), r, vIn.y);\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_S2 = 2 * m_Strength;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Strength, prefix + "rblur_strength", 1));
		m_Params.push_back(ParamWithName<T>(&m_Offset, prefix + "rblur_offset", 1));
		m_Params.push_back(ParamWithName<T>(&m_CenterX, prefix + "rblur_center_x"));
		m_Params.push_back(ParamWithName<T>(&m_CenterY, prefix + "rblur_center_y"));
		m_Params.push_back(ParamWithName<T>(true, &m_S2, prefix + "rblur_s2"));//Precalc.
	}

private:
	T m_Strength;
	T m_Offset;
	T m_CenterX;
	T m_CenterY;
	T m_S2;//Precalc.
};

/// <summary>
/// JuliaNab.
/// </summary>
template <typename T>
class JuliaNabVariation : public ParametricVariation<T>
{
public:
	JuliaNabVariation(T weight = 1.0) : ParametricVariation<T>("juliaNab", eVariationId::VAR_JULIANAB, weight, true)
	{
		Init();
	}

	PARVARCOPY(JuliaNabVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T jun = Zeps(std::abs(m_N));
		T a = (std::atan2(helper.In.y, std::pow(std::abs(helper.In.x), m_Sep)) + M_2PI * Floor<T>(rand.Frand01<T>() * m_AbsN)) / jun;
		T r = m_Weight * std::pow(helper.m_PrecalcSumSquares, m_Cn * m_A);
		helper.Out.x = r * std::cos(a) + m_B;
		helper.Out.y = r * std::sin(a) + m_B;
		helper.Out.z = helper.In.z;

		if (m_VarType == eVariationType::VARTYPE_REG)
			outPoint.m_Z = 0;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string n    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string a    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string b    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string sep  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string absN = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cn   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t jun = Zeps(fabs(" << n << "));\n"
		   << "\n"
		   << "\t\treal_t a = fma(M_2PI, floor(MwcNext01(mwc) * " << absN << "), atan2(vIn.y, pow(fabs(vIn.x), " << sep << "))) / jun;\n"
		   << "\t\treal_t r = " << weight << " * pow(precalcSumSquares, " << cn << " * " << a << ");\n"
		   << "\n"
		   << "\t\tvOut.x = fma(r, cos(a), " << b << ");\n"
		   << "\t\tvOut.y = fma(r, sin(a), " << b << ");\n"
		   << "\t\tvOut.z = vIn.z;\n";

		if (m_VarType == eVariationType::VARTYPE_REG)
			ss << "\t\toutPoint->m_Z = 0;\n";

		ss
				<< "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps" };
	}

	virtual void Precalc() override
	{
		T jun = Zeps(std::abs(m_N));
		m_AbsN = abs(m_N);
		m_Cn = 1 / jun / 2;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_N, prefix + "juliaNab_n", 1));
		m_Params.push_back(ParamWithName<T>(&m_A, prefix + "juliaNab_a", 1));
		m_Params.push_back(ParamWithName<T>(&m_B, prefix + "juliaNab_b", 1));
		m_Params.push_back(ParamWithName<T>(&m_Sep, prefix + "juliaNab_separ", 1));
		m_Params.push_back(ParamWithName<T>(true, &m_AbsN, prefix + "juliaNab_absn"));//Precalc.
		m_Params.push_back(ParamWithName<T>(true, &m_Cn, prefix + "juliaNab_cn"));
	}

private:
	T m_N;
	T m_A;
	T m_B;
	T m_Sep;
	T m_AbsN;//Precalc.
	T m_Cn;
};

/// <summary>
/// Sintrange.
/// </summary>
template <typename T>
class SintrangeVariation : public ParametricVariation<T>
{
public:
	SintrangeVariation(T weight = 1.0) : ParametricVariation<T>("sintrange", eVariationId::VAR_SINTRANGE, weight)
	{
		Init();
	}

	PARVARCOPY(SintrangeVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T sqX = SQR(helper.In.x);
		T sqY = SQR(helper.In.y);
		T v = (sqX + sqY) * m_W;//Do not use precalcSumSquares here because its components are needed below.
		helper.Out.x = m_Weight * std::sin(helper.In.x) * (sqX + m_W - v);
		helper.Out.y = m_Weight * std::sin(helper.In.y) * (sqY + m_W - v);
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string w = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t sqX = SQR(vIn.x);\n"
		   << "\t\treal_t sqY = SQR(vIn.y);\n"
		   << "\t\treal_t v = (sqX + sqY) * " << w << ";\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * sin(vIn.x) * (sqX + " << w << " - v);\n"
		   << "\t\tvOut.y = " << weight << " * sin(vIn.y) * (sqY + " << w << " - v);\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_W, prefix + "sintrange_w", 1));
	}

private:
	T m_W;
};

/// <summary>
/// Voron.
/// </summary>
template <typename T>
class VoronVariation : public ParametricVariation<T>
{
public:
	VoronVariation(T weight = 1.0) : ParametricVariation<T>("Voron", eVariationId::VAR_VORON, weight)
	{
		Init();
	}

	PARVARCOPY(VoronVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		intmax_t l, k;
		int i, j, m, m1, n, n1;
		T r, rMin, offsetX, offsetY, x0 = 0, y0 = 0, x, y;
		rMin = 20;
		m = int(Floor<T>(helper.In.x / m_Step));
		n = int(Floor<T>(helper.In.y / m_Step));

		for (i = -1; i < 2; i++)
		{
			m1 = m + i;

			for (j = -1; j < 2; j++)
			{
				n1 = n + j;
				k = 1 + Floor<T>(m_Num * DiscreteNoise(int(19 * m1 + 257 * n1 + m_XSeed)));

				for (l = 0; l < k; l++)
				{
					x = T(DiscreteNoise(int(l + 64 * m1 + 15 * n1 + m_XSeed)) + m1) * m_Step;
					y = T(DiscreteNoise(int(l + 21 * m1 + 33 * n1 + m_YSeed)) + n1) * m_Step;
					offsetX = helper.In.x - x;
					offsetY = helper.In.y - y;
					r = std::sqrt(SQR(offsetX) + SQR(offsetY));

					if (r < rMin)
					{
						rMin = r;
						x0 = x;
						y0 = y;
					}
				}
			}
		}

		helper.Out.x = m_Weight * (m_K * (helper.In.x - x0) + x0);
		helper.Out.y = m_Weight * (m_K * (helper.In.y - y0) + y0);
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string m_k = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string step = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string num = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string xSeed = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ySeed = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\tint i, j, l, k, m, m1, n, n1;\n"
		   << "\t\treal_t r, rMin, offsetX, offsetY, x0 = (real_t)(0.0), y0 = (real_t)(0.0), x, y;\n"
		   << "\n"
		   << "\t\trMin = 20;\n"
		   << "\t\tm = (int)floor(vIn.x / " << step << ");\n"
		   << "\t\tn = (int)floor(vIn.y / " << step << ");\n"
		   << "\n"
		   << "\t\tfor (i = -1; i < 2; i++)\n"
		   << "\t\t{\n"
		   << "\t\t	m1 = m + i;\n"
		   << "\n"
		   << "\t\t	for (j = -1; j < 2; j++)\n"
		   << "\t\t	{\n"
		   << "\t\t		n1 = n + j;\n"
		   << "\t\t		k = 1 + (int)floor(" << num << " * VoronDiscreteNoise((int)(19 * m1 + 257 * n1 + " << xSeed << ")));\n"
		   << "\n"
		   << "\t\t		for (l = 0; l < k; l++)\n"
		   << "\t\t		{\n"
		   << "\t\t			x = (real_t)(VoronDiscreteNoise((int)(l + 64 * m1 + 15 * n1 + " << xSeed << ")) + m1) * " << step << ";\n"
		   << "\t\t			y = (real_t)(VoronDiscreteNoise((int)(l + 21 * m1 + 33 * n1 + " << ySeed << ")) + n1) * " << step << ";\n"
		   << "\t\t			offsetX = vIn.x - x;\n"
		   << "\t\t			offsetY = vIn.y - y;\n"
		   << "\t\t			r = sqrt(fma(offsetX, offsetX, SQR(offsetY)));\n"
		   << "\n"
		   << "\t\t			if (r < rMin)\n"
		   << "\t\t			{\n"
		   << "\t\t				rMin = r;\n"
		   << "\t\t				x0 = x;\n"
		   << "\t\t				y0 = y;\n"
		   << "\t\t			}\n"
		   << "\t\t		}\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * fma(" << m_k << ", (vIn.x - x0), x0);\n"
		   << "\t\tvOut.y = " << weight << " * fma(" << m_k << ", (vIn.y - y0), y0);\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual string OpenCLFuncsString() const override
	{
		return
			"real_t VoronDiscreteNoise(int x)\n"
			"{\n"
			"	const real_t im = 2147483647;\n"
			"	const real_t am = (1 / im);\n"
			"\n"
			"	int n = x;\n"
			"	n = (n << 13) ^ n;\n"
			"	return ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) * am;\n"
			"}\n"
			"\n";
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_K, prefix + "Voron_K", T(0.99)));
		m_Params.push_back(ParamWithName<T>(&m_Step, prefix + "Voron_Step", T(0.25), eParamType::REAL_NONZERO));
		m_Params.push_back(ParamWithName<T>(&m_Num, prefix + "Voron_Num", 1, eParamType::INTEGER, 1, 25));
		m_Params.push_back(ParamWithName<T>(&m_XSeed, prefix + "Voron_XSeed", 3, eParamType::INTEGER));
		m_Params.push_back(ParamWithName<T>(&m_YSeed, prefix + "Voron_YSeed", 7, eParamType::INTEGER));
	}

private:
	T DiscreteNoise(int x)
	{
		const T im = T(2147483647);
		const T am = (1 / im);
		int n = x;
		n = (n << 13) ^ n;
		return ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) * am;
	}

	T m_K;//Params.
	T m_Step;
	T m_Num;
	T m_XSeed;
	T m_YSeed;
};

/// <summary>
/// Waffle.
/// </summary>
template <typename T>
class WaffleVariation : public ParametricVariation<T>
{
public:
	WaffleVariation(T weight = 1.0) : ParametricVariation<T>("waffle", eVariationId::VAR_WAFFLE, weight)
	{
		Init();
	}

	PARVARCOPY(WaffleVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T a = 0, r = 0;

		switch (rand.Rand(5))
		{
			case 0:
				a = (rand.Rand(ISAAC_INT(m_Slices)) + rand.Frand01<T>() * m_XThickness) / m_Slices;
				r = (rand.Rand(ISAAC_INT(m_Slices)) + rand.Frand01<T>() * m_YThickness) / m_Slices;
				break;

			case 1:
				a = (rand.Rand(ISAAC_INT(m_Slices)) + rand.Frand01<T>()) / m_Slices;
				r = (rand.Rand(ISAAC_INT(m_Slices)) + m_YThickness) / m_Slices;
				break;

			case 2:
				a = (rand.Rand(ISAAC_INT(m_Slices)) + m_XThickness) / m_Slices;
				r = (rand.Rand(ISAAC_INT(m_Slices)) + rand.Frand01<T>()) / m_Slices;
				break;

			case 3:
				a = rand.Frand01<T>();
				r = (rand.Rand(ISAAC_INT(m_Slices)) + m_YThickness + rand.Frand01<T>() * (1 - m_YThickness)) / m_Slices;
				break;

			case 4:
			default:
				a = (rand.Rand(ISAAC_INT(m_Slices)) + m_XThickness + rand.Frand01<T>() * (1 - m_XThickness)) / m_Slices;
				r = rand.Frand01<T>();
				break;
		}

		helper.Out.x = m_CosR * a + m_SinR * r;
		helper.Out.y = -m_SinR * a + m_CosR * r;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		int i = 0;
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string slices     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string xThickness = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string yThickness = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string rotation   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string sinr       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cosr       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t a = 0, r = 0;\n"
		   << "\n"
		   << "\t\tswitch (MwcNextRange(mwc, 5))\n"
		   << "\t\t{\n"
		   << "\t\t	case 0:\n"
		   << "\t\t		a = (MwcNextRange(mwc, (int)" << slices << ") + MwcNext01(mwc) * " << xThickness << ") / " << slices << ";\n"
		   << "\t\t		r = (MwcNextRange(mwc, (int)" << slices << ") + MwcNext01(mwc) * " << yThickness << ") / " << slices << ";\n"
		   << "\t\t		break;\n"
		   << "\t\t	case 1:\n"
		   << "\t\t		a = (MwcNextRange(mwc, (int)" << slices << ") + MwcNext01(mwc)) / " << slices << ";\n"
		   << "\t\t		r = (MwcNextRange(mwc, (int)" << slices << ") + " << yThickness << ") / " << slices << ";\n"
		   << "\t\t		break;\n"
		   << "\t\t	case 2:\n"
		   << "\t\t		a = (MwcNextRange(mwc, (int)" << slices << ") + " << xThickness << ") / " << slices << ";\n"
		   << "\t\t		r = (MwcNextRange(mwc, (int)" << slices << ") + MwcNext01(mwc)) / " << slices << ";\n"
		   << "\t\t		break;\n"
		   << "\t\t	case 3:\n"
		   << "\t\t		a = MwcNext01(mwc);\n"
		   << "\t\t		r = fma(MwcNext01(mwc), 1 - " << yThickness << ", MwcNextRange(mwc, (int)" << slices << ") + " << yThickness << ") / " << slices << ";\n"
		   << "\t\t		break;\n"
		   << "\t\t	case 4:\n"
		   << "\t\t		a = fma(MwcNext01(mwc), (1 - " << xThickness << "), MwcNextRange(mwc, (int)" << slices << ") + " << xThickness << ") / " << slices << ";\n"
		   << "\t\t		r = MwcNext01(mwc);\n"
		   << "\t\t		break;\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.x =  fma(" << cosr << ", a, " << sinr << " * r);\n"
		   << "\t\tvOut.y = -fma(" << sinr << ", a, " << cosr << " * r);\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_SinR = std::sin(m_Rotation);
		m_CosR = std::cos(m_Rotation);
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Slices, prefix + "waffle_slices", 6, eParamType::INTEGER_NONZERO));
		m_Params.push_back(ParamWithName<T>(&m_XThickness, prefix + "waffle_xthickness", T(0.5)));
		m_Params.push_back(ParamWithName<T>(&m_YThickness, prefix + "waffle_ythickness", T(0.5)));
		m_Params.push_back(ParamWithName<T>(&m_Rotation, prefix + "waffle_rotation"));
		m_Params.push_back(ParamWithName<T>(true, &m_SinR, prefix + "waffle_sinr"));
		m_Params.push_back(ParamWithName<T>(true, &m_CosR, prefix + "waffle_cosr"));
	}

private:
	T m_Slices;
	T m_XThickness;
	T m_YThickness;
	T m_Rotation;
	T m_SinR;//Precalc.
	T m_CosR;
};

/// <summary>
/// Square3D.
/// </summary>
template <typename T>
class Square3DVariation : public Variation<T>
{
public:
	Square3DVariation(T weight = 1.0) : Variation<T>("square3D", eVariationId::VAR_SQUARE3D, weight) { }

	VARCOPY(Square3DVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		helper.Out.x = m_Weight * (rand.Frand01<T>() - T(0.5));
		helper.Out.y = m_Weight * (rand.Frand01<T>() - T(0.5));
		helper.Out.z = m_Weight * (rand.Frand01<T>() - T(0.5));
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\tvOut.x = " << weight << " * (MwcNext01(mwc) - (real_t)(0.5));\n"
		   << "\t\tvOut.y = " << weight << " * (MwcNext01(mwc) - (real_t)(0.5));\n"
		   << "\t\tvOut.z = " << weight << " * (MwcNext01(mwc) - (real_t)(0.5));\n"
		   << "\t}\n";
		return ss.str();
	}
};

/// <summary>
/// SuperShape3D.
/// </summary>
template <typename T>
class SuperShape3DVariation : public ParametricVariation<T>
{
public:
	SuperShape3DVariation(T weight = 1.0) : ParametricVariation<T>("SuperShape3D", eVariationId::VAR_SUPER_SHAPE3D, weight)
	{
		Init();
	}

	PARVARCOPY(SuperShape3DVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T pr1, r1, pr2, r2, rho1, phi1, sinr, sinp, cosr, cosp, msinr, msinp, mcosr, mcosp, temp;
		rho1 = rand.Frand01<T>() * m_Rho2Pi;
		phi1 = rand.Frand01<T>() * m_Phi2Pi;

		if (rand.RandBit())
			phi1 = -phi1;

		sinr = std::sin(rho1);
		cosr = std::cos(rho1);
		sinp = std::sin(phi1);
		cosp = std::cos(phi1);
		temp = m_M4_1 * rho1;
		msinr = std::sin(temp);
		mcosr = std::cos(temp);
		temp = m_M4_2 * phi1;
		msinp = std::sin(temp);
		mcosp = std::cos(temp);
		pr1 = m_An2_1 * std::pow(std::abs(mcosr), m_N2_1) + m_Bn3_1 * std::pow(std::abs(msinr), m_N3_1);
		pr2 = m_An2_2 * std::pow(std::abs(mcosp), m_N2_2) + m_Bn3_2 * std::pow(std::abs(msinp), m_N3_2);
		r1 = std::pow(std::abs(pr1), m_N1_1) + m_Spiral * rho1;
		r2 = std::pow(std::abs(pr2), m_N1_2);

		if (int(m_Toroidmap) == 1)
		{
			helper.Out.x = m_Weight * cosr * (r1 + r2 * cosp);
			helper.Out.y = m_Weight * sinr * (r1 + r2 * cosp);
			helper.Out.z = m_Weight * r2 * sinp;
		}
		else
		{
			helper.Out.x = m_Weight * r1 * cosr * r2 * cosp;
			helper.Out.y = m_Weight * r1 * sinr * r2 * cosp;
			helper.Out.z = m_Weight * r2 * sinp;
		}
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string rho    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string phi    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string m1     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string m2     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string a1     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string a2     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string b1     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string b2     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string n1_1   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string n1_2   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string n2_1   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string n2_2   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string n3_1   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string n3_2   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string spiral = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string toroid = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string n1n_1  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string n1n_2  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string an2_1  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string an2_2  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string bn3_1  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string bn3_2  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string m4_1   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string m4_2   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string rho2pi = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string phi2pi = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t pr1, r1, pr2, r2, rho1, phi1, sinr, sinp, cosr, cosp, msinr, msinp, mcosr, mcosp, temp;\n"
		   << "\n"
		   << "\t\trho1 = MwcNext01(mwc) * " << rho2pi << ";\n"
		   << "\t\tphi1 = MwcNext01(mwc) * " << phi2pi << ";\n"
		   << "\n"
		   << "\t\tif (MwcNext(mwc) & 1)\n"
		   << "\t\t	phi1 = -phi1;\n"
		   << "\n"
		   << "\t\tsinr = sin(rho1);\n"
		   << "\t\tcosr = cos(rho1);\n"
		   << "\n"
		   << "\t\tsinp = sin(phi1);\n"
		   << "\t\tcosp = cos(phi1);\n"
		   << "\n"
		   << "\t\ttemp = " << m4_1 << " * rho1;\n"
		   << "\t\tmsinr = sin(temp);\n"
		   << "\t\tmcosr = cos(temp);\n"
		   << "\n"
		   << "\t\ttemp = " << m4_2 << " * phi1;\n"
		   << "\t\tmsinp = sin(temp);\n"
		   << "\t\tmcosp = cos(temp);\n"
		   << "\n"
		   << "\t\tpr1 = fma(" << an2_1 << ", pow(fabs(mcosr), " << n2_1 << "), " << bn3_1 << " * pow(fabs(msinr), " << n3_1 << "));\n"
		   << "\t\tpr2 = fma(" << an2_2 << ", pow(fabs(mcosp), " << n2_2 << "), " << bn3_2 << " * pow(fabs(msinp), " << n3_2 << "));\n"
		   << "\t\tr1 = fma(" << spiral << ", rho1, pow(fabs(pr1), " << n1_1 << "));\n"
		   << "\t\tr2 = pow(fabs(pr2), " << n1_2 << ");\n"
		   << "\n"
		   << "\t\tif ((int)" << toroid << " == 1)\n"
		   << "\t\t{\n"
		   << "\t\t	vOut.x = " << weight << " * cosr * fma(r2, cosp, r1);\n"
		   << "\t\t	vOut.y = " << weight << " * sinr * fma(r2, cosp, r1);\n"
		   << "\t\t	vOut.z = " << weight << " * r2 * sinp;\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	vOut.x = " << weight << " * r1 * cosr * r2 * cosp;\n"
		   << "\t\t	vOut.y = " << weight << " * r1 * sinr * r2 * cosp;\n"
		   << "\t\t	vOut.z = " << weight << " * r2 * sinp;\n"
		   << "\t\t}\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_N1n_1 = (-1 / m_N1_1);
		m_N1n_2 = (-1 / m_N1_2);
		m_An2_1 = std::pow(std::abs(1 / m_A1), m_N2_1);
		m_An2_2 = std::pow(std::abs(1 / m_A2), m_N2_2);
		m_Bn3_1 = std::pow(std::abs(1 / m_B1), m_N3_1);
		m_Bn3_2 = std::pow(std::abs(1 / m_B2), m_N3_2);
		m_M4_1 = m_M1 / 4;
		m_M4_2 = m_M2 / 4;
		m_Rho2Pi = m_Rho * T(M_2_PI);
		m_Phi2Pi = m_Phi * T(M_2_PI);
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Rho, prefix + "SuperShape3D_rho", T(9.9)));
		m_Params.push_back(ParamWithName<T>(&m_Phi, prefix + "SuperShape3D_phi", T(2.5)));
		m_Params.push_back(ParamWithName<T>(&m_M1, prefix + "SuperShape3D_m1", 6));
		m_Params.push_back(ParamWithName<T>(&m_M2, prefix + "SuperShape3D_m2", 3));
		m_Params.push_back(ParamWithName<T>(&m_A1, prefix + "SuperShape3D_a1", 1));
		m_Params.push_back(ParamWithName<T>(&m_A2, prefix + "SuperShape3D_a2", 1));
		m_Params.push_back(ParamWithName<T>(&m_B1, prefix + "SuperShape3D_b1", 1));
		m_Params.push_back(ParamWithName<T>(&m_B2, prefix + "SuperShape3D_b2", 1));
		m_Params.push_back(ParamWithName<T>(&m_N1_1, prefix + "SuperShape3D_n1_1", 1));
		m_Params.push_back(ParamWithName<T>(&m_N1_2, prefix + "SuperShape3D_n1_2", 1));
		m_Params.push_back(ParamWithName<T>(&m_N2_1, prefix + "SuperShape3D_n2_1", 1));
		m_Params.push_back(ParamWithName<T>(&m_N2_2, prefix + "SuperShape3D_n2_2", 1));
		m_Params.push_back(ParamWithName<T>(&m_N3_1, prefix + "SuperShape3D_n3_1", 1));
		m_Params.push_back(ParamWithName<T>(&m_N3_2, prefix + "SuperShape3D_n3_2", 1));
		m_Params.push_back(ParamWithName<T>(&m_Spiral, prefix + "SuperShape3D_spiral"));
		m_Params.push_back(ParamWithName<T>(&m_Toroidmap, prefix + "SuperShape3D_toroidmap", 0, eParamType::INTEGER, 0, 1));
		m_Params.push_back(ParamWithName<T>(true, &m_N1n_1, prefix + "SuperShape3D_n1n1"));//Precalc.
		m_Params.push_back(ParamWithName<T>(true, &m_N1n_2, prefix + "SuperShape3D_n1n2"));
		m_Params.push_back(ParamWithName<T>(true, &m_An2_1, prefix + "SuperShape3D_an21"));
		m_Params.push_back(ParamWithName<T>(true, &m_An2_2, prefix + "SuperShape3D_an22"));
		m_Params.push_back(ParamWithName<T>(true, &m_Bn3_1, prefix + "SuperShape3D_bn31"));
		m_Params.push_back(ParamWithName<T>(true, &m_Bn3_2, prefix + "SuperShape3D_bn32"));
		m_Params.push_back(ParamWithName<T>(true, &m_M4_1, prefix + "SuperShape3D_m41"));
		m_Params.push_back(ParamWithName<T>(true, &m_M4_2, prefix + "SuperShape3D_m42"));
		m_Params.push_back(ParamWithName<T>(true, &m_Rho2Pi, prefix + "SuperShape3D_rho2pi"));
		m_Params.push_back(ParamWithName<T>(true, &m_Phi2Pi, prefix + "SuperShape3D_phi2pi"));
	}

private:
	T m_Rho;
	T m_Phi;
	T m_M1;
	T m_M2;
	T m_A1;
	T m_A2;
	T m_B1;
	T m_B2;
	T m_N1_1;
	T m_N1_2;
	T m_N2_1;
	T m_N2_2;
	T m_N3_1;
	T m_N3_2;
	T m_Spiral;
	T m_Toroidmap;
	T m_N1n_1;//Precalc.
	T m_N1n_2;
	T m_An2_1;
	T m_An2_2;
	T m_Bn3_1;
	T m_Bn3_2;
	T m_M4_1;
	T m_M4_2;
	T m_Rho2Pi;
	T m_Phi2Pi;
};

/// <summary>
/// sphyp3D.
/// </summary>
template <typename T>
class Sphyp3DVariation : public ParametricVariation<T>
{
public:
	Sphyp3DVariation(T weight = 1.0) : ParametricVariation<T>("sphyp3D", eVariationId::VAR_SPHYP3D, weight, true)
	{
		Init();
	}

	PARVARCOPY(Sphyp3DVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T t, rX, rY, rZ;
		t = Zeps(helper.m_PrecalcSumSquares + SQR(helper.In.z));
		rX = m_Weight / std::pow(t, m_StretchX);
		rY = m_Weight / std::pow(t, m_StretchY);
		helper.Out.x = helper.In.x * rX;
		helper.Out.y = helper.In.y * rY;

		//Optional 3D calculation.
		if (int(m_ZOn) == 1)
		{
			rZ = m_Weight / std::pow(t, m_StretchZ);
			helper.Out.z = helper.In.z * rZ;
		}
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string stretchX = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string stretchY = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string stretchZ = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string zOn = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t t, rX, rY, rZ;\n"
		   << "\n"
		   << "\t\tt  = Zeps(fma(vIn.z, vIn.z, precalcSumSquares));\n"
		   << "\t\trX = " << weight << " / pow(t, " << stretchX << ");\n"
		   << "\t\trY = " << weight << " / pow(t, " << stretchY << ");\n"
		   << "\n"
		   << "\t\tvOut.x = vIn.x * rX;\n"
		   << "\t\tvOut.y = vIn.y * rY;\n"
		   << "\n"
		   << "\t\tif ((int)" << zOn << " == 1)\n"
		   << "\t\t{\n"
		   << "\t\trZ = " << weight << " / pow(t, " << stretchZ << ");\n"
		   << "\n"
		   << "\t\tvOut.z = vIn.z * rZ;\n"
		   << "\t\t}\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps" };
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_StretchX, prefix + "sphyp3D_stretchX", 1));
		m_Params.push_back(ParamWithName<T>(&m_StretchY, prefix + "sphyp3D_stretchY", 1));
		m_Params.push_back(ParamWithName<T>(&m_StretchZ, prefix + "sphyp3D_stretchZ", 1));
		m_Params.push_back(ParamWithName<T>(&m_ZOn, prefix + "sphyp3D_zOn", 1, eParamType::INTEGER, 0, 1));
	}

private:
	T m_StretchX;
	T m_StretchY;
	T m_StretchZ;
	T m_ZOn;
};

/// <summary>
/// circlecrop.
/// </summary>
template <typename T>
class CirclecropVariation : public ParametricVariation<T>
{
public:
	CirclecropVariation(T weight = 1.0) : ParametricVariation<T>("circlecrop", eVariationId::VAR_CIRCLECROP, weight)
	{
		Init();
	}

	PARVARCOPY(CirclecropVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T xi = helper.In.x - m_X;
		T yi = helper.In.y - m_Y;

		if (m_VarType == eVariationType::VARTYPE_REG)//Original altered the input pointed to for reg.
		{
			helper.m_TransX -= m_X;
			helper.m_TransY -= m_Y;
			helper.Out.z = m_Weight * helper.In.z;//Original only assigned z for reg. Will be summed.
		}
		else
		{
			helper.Out.z = helper.In.z;//Original did nothing with z for pre/post, so passthrough direct assign.
		}

		const T rad = std::sqrt(SQR(xi) + SQR(yi));
		const T ang = std::atan2(yi, xi);
		const T rdc = m_Radius + (rand.Frand01<T>() * T(0.5) * m_Ca);
		const T s = std::sin(ang);
		const T c = std::cos(ang);
		const int esc = rad > m_Radius;
		const int cr0 = int(m_Zero);

		if (cr0 && esc)
		{
			helper.Out.x = helper.Out.y = 0;

			if (m_VarType == eVariationType::VARTYPE_REG)
				outPoint.m_X = outPoint.m_Y = 0;
		}
		else if (cr0 && !esc)
		{
			helper.Out.x = m_Weight * xi + m_X;
			helper.Out.y = m_Weight * yi + m_Y;
		}
		else if (!cr0 && esc)
		{
			helper.Out.x = m_Weight * rdc * c + m_X;
			helper.Out.y = m_Weight * rdc * s + m_Y;
		}
		else if (!cr0 && !esc)
		{
			helper.Out.x = m_Weight * xi + m_X;
			helper.Out.y = m_Weight * yi + m_Y;
		}
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string radius = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string x = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string y = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scatterArea = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string zero = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ca = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t xi = vIn.x - " << x << ";\n"
		   << "\t\treal_t yi = vIn.y - " << y << ";\n"
		   << "\n";

		if (m_VarType == eVariationType::VARTYPE_REG)//Original altered the input pointed to for reg.
		{
			ss
					<< "\t\ttransX -= " << x << ";\n"
					<< "\t\ttransY -= " << y << ";\n"
					<< "\t\tvOut.z = " << weight << " * vIn.z;\n";
		}
		else
		{
			ss
					<< "\t\tvOut.z = vIn.z;\n";
		}

		ss
				<< "\t\tconst real_t rad = sqrt(SQR(xi) + SQR(yi));\n"
				<< "\t\tconst real_t ang = atan2(yi, xi);\n"
				<< "\t\tconst real_t rdc = fma(MwcNext01(mwc) * (real_t)(0.5), " << ca << ", " << radius << ");\n"
				<< "\t\tconst real_t s = sin(ang);\n"
				<< "\t\tconst real_t c = cos(ang);\n"
				<< "\n"
				<< "\t\tconst int esc = rad > " << radius << ";\n"
				<< "\t\tconst int cr0 = (int)" << zero << ";\n"
				<< "\n"
				<< "\t\tif (cr0 && esc)\n"
				<< "\t\t{\n"
				<< "\t\t	vOut.x = vOut.y = 0;\n";

		if (m_VarType == eVariationType::VARTYPE_REG)
			ss << "\t\t	outPoint->m_X = outPoint->m_Y = 0;\n";

		ss
				<< "\t\t}\n"
				<< "\t\telse if (cr0 && !esc)\n"
				<< "\t\t{\n"
				<< "\t\t	vOut.x = fma(" << weight << ", xi, " << x << ");\n"
				<< "\t\t	vOut.y = fma(" << weight << ", yi, " << y << ");\n"
				<< "\t\t}\n"
				<< "\t\telse if (!cr0 &&  esc)\n"
				<< "\t\t{\n"
				<< "\t\t	vOut.x = fma(" << weight << ", rdc * c, " << x << ");\n"
				<< "\t\t	vOut.y = fma(" << weight << ", rdc * s, " << y << ");\n"
				<< "\t\t}\n"
				<< "\t\telse if (!cr0 && !esc)\n"
				<< "\t\t{\n"
				<< "\t\t	vOut.x = fma(" << weight << ", xi, " << x << ");\n"
				<< "\t\t	vOut.y = fma(" << weight << ", yi, " << y << ");\n"
				<< "\t\t}\n"
				<< "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_Ca = Clamp<T>(m_ScatterArea, -1, 1);
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Radius, prefix + "circlecrop_radius", 1));
		m_Params.push_back(ParamWithName<T>(&m_X, prefix + "circlecrop_x"));
		m_Params.push_back(ParamWithName<T>(&m_Y, prefix + "circlecrop_y"));
		m_Params.push_back(ParamWithName<T>(&m_ScatterArea, prefix + "circlecrop_scatter_area"));
		m_Params.push_back(ParamWithName<T>(&m_Zero, prefix + "circlecrop_zero", 1, eParamType::INTEGER, 0, 1));
		m_Params.push_back(ParamWithName<T>(true, &m_Ca, prefix + "circlecrop_ca"));
	}

private:
	T m_Radius;
	T m_X;
	T m_Y;
	T m_ScatterArea;
	T m_Zero;
	T m_Ca;//Precalc.
};

/// <summary>
/// circlecrop2.
/// By tatasz.
/// </summary>
template <typename T>
class Circlecrop2Variation : public ParametricVariation<T>
{
public:
	Circlecrop2Variation(T weight = 1.0) : ParametricVariation<T>("circlecrop2", eVariationId::VAR_CIRCLECROP2, weight, true, true, false, false, true)
	{
		Init();
	}

	PARVARCOPY(Circlecrop2Variation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T rad = helper.m_PrecalcSqrtSumSquares;
		T ang = helper.m_PrecalcAtanyx;
		T s = 0;
		T c = 0;

		if (rad > m_Out || rad < m_In)
		{
			if (!m_Zero)
			{
				s = std::sin(ang) * m_OutWeight;
				c = std::cos(ang) * m_OutWeight;
			}
		}
		else
		{
			s = helper.In.x * m_Weight;
			c = helper.In.y * m_Weight;
		}

		helper.Out.x = s;
		helper.Out.y = c;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string inner       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string outer       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string zero        = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string in          = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string out         = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string outweight   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t rad = precalcSqrtSumSquares;\n"
		   << "\t\treal_t ang = precalcAtanyx;\n"
		   << "\t\treal_t s = 0;\n"
		   << "\t\treal_t c = 0;\n"
		   << "\n"
		   << "\t\tif (rad > " << out << " || rad < " << in << ")\n"
		   << "\t\t{\n"
		   << "\t\t	if (" << zero << " == 0)\n"
		   << "\t\t	{\n"
		   << "\t\t		s = sin(ang) * " << outweight << ";\n"
		   << "\t\t		c = cos(ang) * " << outweight << ";\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	s = vIn.x * " << weight << ";\n"
		   << "\t\t	c = vIn.y * " << weight << ";\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.x = s;\n"
		   << "\t\tvOut.y = c;\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_In  = std::min(m_Inner, m_Outer);
		m_Out = std::max(m_Inner, m_Outer);
		m_OutWeight = m_Out * m_Weight;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Inner,           prefix + "circlecrop2_inner", T(0.5)));
		m_Params.push_back(ParamWithName<T>(&m_Outer,           prefix + "circlecrop2_outer", 1));
		m_Params.push_back(ParamWithName<T>(&m_Zero,            prefix + "circlecrop2_zero", 1, eParamType::INTEGER, 0, 1));
		m_Params.push_back(ParamWithName<T>(true, &m_In,        prefix + "circlecrop2_in"));
		m_Params.push_back(ParamWithName<T>(true, &m_Out,       prefix + "circlecrop2_out"));
		m_Params.push_back(ParamWithName<T>(true, &m_OutWeight, prefix + "circlecrop2_out_weight"));
	}

private:
	T m_Inner;
	T m_Outer;
	T m_Zero;
	T m_In;//Precalc.
	T m_Out;
	T m_OutWeight;
};

/// <summary>
/// julian3Dx.
/// </summary>
template <typename T>
class Julian3DxVariation : public ParametricVariation<T>
{
public:
	Julian3DxVariation(T weight = 1.0) : ParametricVariation<T>("julian3Dx", eVariationId::VAR_JULIAN3DX, weight, true, true)
	{
		Init();
	}

	PARVARCOPY(Julian3DxVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		const T z = helper.In.z / m_AbsN;
		const T radiusOut = m_Weight * std::pow(helper.m_PrecalcSumSquares + z * z, m_Cn);
		const T x = m_A * helper.In.x + m_B * helper.In.y + m_E;
		const T y = m_C * helper.In.x + m_D * helper.In.y + m_F;
		const T tempRand = T(int(rand.Frand01<T>() * m_AbsN));
		const T alpha = (std::atan2(y, x) + M_2PI * tempRand) / m_Power;
		const T gamma = radiusOut * helper.m_PrecalcSqrtSumSquares;
		helper.Out.x = gamma * std::cos(alpha);
		helper.Out.y = gamma * std::sin(alpha);
		helper.Out.z = radiusOut * z;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string power = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string dist  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string a     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string b     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string c     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string d     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string e     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string f     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string absn  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cn    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\tconst real_t z = vIn.z / " << absn << ";\n"
		   << "\t\tconst real_t radiusOut = " << weight << " * pow(fma(z, z, precalcSumSquares), " << cn << ");\n"
		   << "\t\tconst real_t x = fma(" << a << ", vIn.x, fma(" << b << ", vIn.y, " << e << "));\n"
		   << "\t\tconst real_t y = fma(" << c << ", vIn.x, fma(" << d << ", vIn.y, " << f << "));\n"
		   << "\t\tconst real_t rand = (int)(MwcNext01(mwc) * " << absn << ");\n"
		   << "\t\tconst real_t alpha = fma(M_2PI, rand, atan2(y, x)) / " << power << ";\n"
		   << "\t\tconst real_t gamma = radiusOut * precalcSqrtSumSquares;\n"
		   << "\n"
		   << "\t\tvOut.x = gamma * cos(alpha);\n"
		   << "\t\tvOut.y = gamma * sin(alpha);\n"
		   << "\t\tvOut.z = radiusOut * z;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_AbsN = std::abs(m_Power);
		m_Cn = (m_Dist / m_Power - 1) / 2;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Dist, prefix + "julian3Dx_dist", 1));
		m_Params.push_back(ParamWithName<T>(&m_Power, prefix + "julian3Dx_power", 2, eParamType::INTEGER_NONZERO));
		m_Params.push_back(ParamWithName<T>(&m_A, prefix + "julian3Dx_a", 1));
		m_Params.push_back(ParamWithName<T>(&m_B, prefix + "julian3Dx_b"));
		m_Params.push_back(ParamWithName<T>(&m_C, prefix + "julian3Dx_c"));
		m_Params.push_back(ParamWithName<T>(&m_D, prefix + "julian3Dx_d", 1));
		m_Params.push_back(ParamWithName<T>(&m_E, prefix + "julian3Dx_e"));
		m_Params.push_back(ParamWithName<T>(&m_F, prefix + "julian3Dx_f"));
		m_Params.push_back(ParamWithName<T>(true, &m_AbsN, prefix + "julian3Dx_absn"));//Precalc.
		m_Params.push_back(ParamWithName<T>(true, &m_Cn, prefix + "julian3Dx_cn"));
	}

private:
	T m_Dist;//Params.
	T m_Power;
	T m_A;
	T m_B;
	T m_C;
	T m_D;
	T m_E;
	T m_F;
	T m_AbsN;//Precalc.
	T m_Cn;
};

/// <summary>
/// fourth.
/// </summary>
template <typename T>
class FourthVariation : public ParametricVariation<T>
{
public:
	FourthVariation(T weight = 1.0) : ParametricVariation<T>("fourth", eVariationId::VAR_FOURTH, weight, true, true, false, false, true)
	{
		Init();
	}

	PARVARCOPY(FourthVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		if (helper.In.x > 0 && helper.In.y > 0)//Quadrant IV: spherical.
		{
			T r = 1 / helper.m_PrecalcSqrtSumSquares;
			helper.Out.x = m_Weight * r * std::cos(helper.m_PrecalcAtanyx);
			helper.Out.y = m_Weight * r * std::sin(helper.m_PrecalcAtanyx);
		}
		else if (helper.In.x > 0 && helper.In.y < 0)//Quadrant I: loonie.
		{
			T r2 = helper.m_PrecalcSumSquares;

			if (r2 < m_SqrWeight)
			{
				T r = m_Weight * std::sqrt(m_SqrWeight / r2 - 1);
				helper.Out.x = r * helper.In.x;
				helper.Out.y = r * helper.In.y;
			}
			else
			{
				helper.Out.x = m_Weight * helper.In.x;
				helper.Out.y = m_Weight * helper.In.y;
			}
		}
		else if (helper.In.x < 0 && helper.In.y > 0)//Quadrant III: susan.
		{
			T x = helper.In.x - m_X;
			T y = helper.In.y + m_Y;
			T r = std::sqrt(SQR(x) + SQR(y));

			if (r < m_Weight)
			{
				T a = std::atan2(y, x) + m_Spin + m_Twist * (m_Weight - r);
				r *= m_Weight;
				helper.Out.x = r * std::cos(a) + m_X;
				helper.Out.y = r * std::sin(a) - m_Y;
			}
			else
			{
				r = m_Weight * (1 + m_Space / Zeps(r));
				helper.Out.x = r * x + m_X;
				helper.Out.y = r * y - m_Y;
			}
		}
		else//Quadrant II: Linear.
		{
			helper.Out.x = m_Weight * helper.In.x;
			helper.Out.y = m_Weight * helper.In.y;
		}

		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string spin      = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string space     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string twist     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string x         = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string y         = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string sqrWeight = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\tif (vIn.x > 0 && vIn.y > 0)\n"
		   << "\t\t{\n"
		   << "\t\t	real_t r = 1 / precalcSqrtSumSquares;\n"
		   << "\n"
		   << "\t\t	vOut.x = " << weight << " * r * cos(precalcAtanyx);\n"
		   << "\t\t	vOut.y = " << weight << " * r * sin(precalcAtanyx);\n"
		   << "\t\t}\n"
		   << "\t\telse if (vIn.x > 0 && vIn.y < 0)\n"
		   << "\t\t{\n"
		   << "\t\t	real_t r2 = precalcSumSquares;\n"
		   << "\n"
		   << "\t\t	if (r2 < " << sqrWeight << ")\n"
		   << "\t\t	{\n"
		   << "\t\t		real_t r = " << weight << " * sqrt(" << sqrWeight << " / r2 - 1);\n"
		   << "\n"
		   << "\t\t		vOut.x = r * vIn.x;\n"
		   << "\t\t		vOut.y = r * vIn.y;\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = " << weight << " * vIn.x;\n"
		   << "\t\t		vOut.y = " << weight << " * vIn.y;\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\t\telse if (vIn.x < 0 && vIn.y > 0)\n"
		   << "\t\t{\n"
		   << "\t\t	real_t x = vIn.x - " << x << ";\n"
		   << "\t\t	real_t y = vIn.y + " << y << ";\n"
		   << "\t\t	real_t r = sqrt(fma(x, x, SQR(y)));\n"
		   << "\n"
		   << "\t\t	if (r < " << weight << ")\n"
		   << "\t\t	{\n"
		   << "\t\t		real_t a = fma(" << twist << ", " << weight << " - r, atan2(y, x) + " << spin << ");\n"
		   << "\n"
		   << "\t\t		r *= " << weight << ";\n"
		   << "\t\t		vOut.x = fma(r, cos(a), " << x << ");\n"
		   << "\t\t		vOut.y = fma(r, sin(a), -" << y << ");\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		r = " << weight << " * (1 + " << space << " / Zeps(r));\n"
		   << "\t\t		vOut.x = fma(r, x, " << x << ");\n"
		   << "\t\t		vOut.y = fma(r, y, -" << y << ");\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	vOut.x = " << weight << " * vIn.x;\n"
		   << "\t\t	vOut.y = " << weight << " * vIn.y;\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps" };
	}

	virtual void Precalc() override
	{
		m_SqrWeight = SQR(m_Weight);
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Spin, prefix + "fourth_spin", T(M_PI), eParamType::REAL_CYCLIC, 0, M_2PI));
		m_Params.push_back(ParamWithName<T>(&m_Space, prefix + "fourth_space"));
		m_Params.push_back(ParamWithName<T>(&m_Twist, prefix + "fourth_twist"));
		m_Params.push_back(ParamWithName<T>(&m_X, prefix + "fourth_x"));
		m_Params.push_back(ParamWithName<T>(&m_Y, prefix + "fourth_y"));
		m_Params.push_back(ParamWithName<T>(true, &m_SqrWeight, prefix + "fourth_sqr_weight"));//Precalc.
	}

private:
	T m_Spin;
	T m_Space;
	T m_Twist;
	T m_X;
	T m_Y;
	T m_SqrWeight;//Precalc.
};

/// <summary>
/// mobiq.
/// </summary>
template <typename T>
class MobiqVariation : public ParametricVariation<T>
{
public:
	MobiqVariation(T weight = 1.0) : ParametricVariation<T>("mobiq", eVariationId::VAR_MOBIQ, weight)
	{
		Init();
	}

	PARVARCOPY(MobiqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		const T t1 = m_At;
		const T t2 = helper.In.x;
		const T t3 = m_Bt;
		const T t4 = m_Ct;
		const T t5 = m_Dt;
		const T x1 = m_Ax;
		const T x2 = helper.In.y;
		const T x3 = m_Bx;
		const T x4 = m_Cx;
		const T x5 = m_Dx;
		const T y1 = m_Ay;
		const T y2 = helper.In.z;
		const T y3 = m_By;
		const T y4 = m_Cy;
		const T y5 = m_Dy;
		const T z1 = m_Az;
		const T z3 = m_Bz;
		const T z4 = m_Cz;
		const T z5 = m_Dz;
		T nt = t1 * t2 - x1 * x2 - y1 * y2 + t3;
		T nx = t1 * x2 + x1 * t2 - z1 * y2 + x3;
		T ny = t1 * y2 + y1 * t2 + z1 * x2 + y3;
		T nz = z1 * t2 + x1 * y2 - y1 * x2 + z3;
		T dt = t4 * t2 - x4 * x2 - y4 * y2 + t5;
		T dx = t4 * x2 + x4 * t2 - z4 * y2 + x5;
		T dy = t4 * y2 + y4 * t2 + z4 * x2 + y5;
		T dz = z4 * t2 + x4 * y2 - y4 * x2 + z5;
		T ni = m_Weight / (SQR(dt) + SQR(dx) + SQR(dy) + SQR(dz));
		helper.Out.x = (nt * dt + nx * dx + ny * dy + nz * dz) * ni;
		helper.Out.y = (nx * dt - nt * dx - ny * dz + nz * dy) * ni;
		helper.Out.z = (ny * dt - nt * dy - nz * dx + nx * dz) * ni;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string at = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ax = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ay = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string az = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string bt = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string bx = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string by = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string bz = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ct = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cx = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cy = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cz = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string dt = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string dx = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string dy = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string dz = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\tconst real_t t1 = " << at << ";\n"
		   << "\t\tconst real_t t2 = vIn.x;\n"
		   << "\t\tconst real_t t3 = " << bt << ";\n"
		   << "\t\tconst real_t t4 = " << ct << ";\n"
		   << "\t\tconst real_t t5 = " << dt << ";\n"
		   << "\t\tconst real_t x1 = " << ax << ";\n"
		   << "\t\tconst real_t x2 = vIn.y;\n"
		   << "\t\tconst real_t x3 = " << bx << ";\n"
		   << "\t\tconst real_t x4 = " << cx << ";\n"
		   << "\t\tconst real_t x5 = " << dx << ";\n"
		   << "\t\tconst real_t y1 = " << ay << ";\n"
		   << "\t\tconst real_t y2 = vIn.z;\n"
		   << "\t\tconst real_t y3 = " << by << ";\n"
		   << "\t\tconst real_t y4 = " << cy << ";\n"
		   << "\t\tconst real_t y5 = " << dy << ";\n"
		   << "\t\tconst real_t z1 = " << az << ";\n"
		   << "\t\tconst real_t z3 = " << bz << ";\n"
		   << "\t\tconst real_t z4 = " << cz << ";\n"
		   << "\t\tconst real_t z5 = " << dz << ";\n"
		   << "\n"
		   << "\t\treal_t nt = fma(t1, t2, -(x1 * x2)) - y1 * y2 + t3;\n"
		   << "\t\treal_t nx = fma(t1, x2, x1 * t2) - z1 * y2 + x3;\n"
		   << "\t\treal_t ny = fma(t1, y2, fma(y1, t2, fma(z1, x2, y3)));\n"
		   << "\t\treal_t nz = fma(z1, t2, x1 * y2) - y1 * x2 + z3;\n"
		   << "\t\treal_t dt = fma(t4, t2, -(x4 * x2)) - y4 * y2 + t5;\n"
		   << "\t\treal_t dx = fma(t4, x2, x4 * t2) - z4 * y2 + x5;\n"
		   << "\t\treal_t dy = fma(t4, y2, fma(y4, t2, fma(z4, x2, y5)));\n"
		   << "\t\treal_t dz = fma(z4, t2, x4 * y2) - y4 * x2 + z5;\n"
		   << "\t\treal_t ni = " << weight << " / fma(dt, dt, fma(dx, dx, fma(dy, dy, SQR(dz))));\n"
		   << "\n"
		   << "\t\tvOut.x = fma(nt, dt, fma(nx, dx, fma(ny, dy, nz * dz))) * ni;\n"
		   << "\t\tvOut.y = (fma(nx, dt, -(nt * dx)) - ny * dz + nz * dy) * ni;\n"
		   << "\t\tvOut.z = (fma(ny, dt, -(nt * dy)) - nz * dx + nx * dz) * ni;\n"
		   << "\t}\n";
		return ss.str();
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_At, prefix + "mobiq_at", 1));
		m_Params.push_back(ParamWithName<T>(&m_Ax, prefix + "mobiq_ax"));
		m_Params.push_back(ParamWithName<T>(&m_Ay, prefix + "mobiq_ay"));
		m_Params.push_back(ParamWithName<T>(&m_Az, prefix + "mobiq_az"));
		m_Params.push_back(ParamWithName<T>(&m_Bt, prefix + "mobiq_bt"));
		m_Params.push_back(ParamWithName<T>(&m_Bx, prefix + "mobiq_bx"));
		m_Params.push_back(ParamWithName<T>(&m_By, prefix + "mobiq_by"));
		m_Params.push_back(ParamWithName<T>(&m_Bz, prefix + "mobiq_bz"));
		m_Params.push_back(ParamWithName<T>(&m_Ct, prefix + "mobiq_ct"));
		m_Params.push_back(ParamWithName<T>(&m_Cx, prefix + "mobiq_cx"));
		m_Params.push_back(ParamWithName<T>(&m_Cy, prefix + "mobiq_cy"));
		m_Params.push_back(ParamWithName<T>(&m_Cz, prefix + "mobiq_cz"));
		m_Params.push_back(ParamWithName<T>(&m_Dt, prefix + "mobiq_dt", 1));
		m_Params.push_back(ParamWithName<T>(&m_Dx, prefix + "mobiq_dx"));
		m_Params.push_back(ParamWithName<T>(&m_Dy, prefix + "mobiq_dy"));
		m_Params.push_back(ParamWithName<T>(&m_Dz, prefix + "mobiq_dz"));
	}

private:
	T m_At;
	T m_Ax;
	T m_Ay;
	T m_Az;
	T m_Bt;
	T m_Bx;
	T m_By;
	T m_Bz;
	T m_Ct;
	T m_Cx;
	T m_Cy;
	T m_Cz;
	T m_Dt;
	T m_Dx;
	T m_Dy;
	T m_Dz;
};

/// <summary>
/// spherivoid.
/// </summary>
template <typename T>
class SpherivoidVariation : public ParametricVariation<T>
{
public:
	SpherivoidVariation(T weight = 1.0) : ParametricVariation<T>("spherivoid", eVariationId::VAR_SPHERIVOID, weight, true, true, false, false, true)
	{
		Init();
	}

	PARVARCOPY(SpherivoidVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		const T zr = VarFuncs<T>::Hypot(helper.In.z, helper.m_PrecalcSqrtSumSquares);
		const T phi = std::acos(Clamp<T>(helper.In.z / zr, -1, 1));
		const T ps = std::sin(phi);
		const T pc = std::cos(phi);
		helper.Out.x = m_Weight * std::cos(helper.m_PrecalcAtanyx) * ps * (zr + m_Radius);
		helper.Out.y = m_Weight * std::sin(helper.m_PrecalcAtanyx) * ps * (zr + m_Radius);
		helper.Out.z = m_Weight * pc * (zr + m_Radius);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string radius = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\tconst real_t zr = Hypot(vIn.z, precalcSqrtSumSquares);\n"
		   << "\t\tconst real_t phi = acos(clamp(vIn.z / zr, -(real_t)(1.0), (real_t)(1.0)));\n"
		   << "\t\tconst real_t ps = sin(phi);\n"
		   << "\t\tconst real_t pc = cos(phi);\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * cos(precalcAtanyx) * ps * (zr + " << radius << ");\n"
		   << "\t\tvOut.y = " << weight << " * sin(precalcAtanyx) * ps * (zr + " << radius << ");\n"
		   << "\t\tvOut.z = " << weight << " * pc * (zr + " << radius << ");\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Hypot" };
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Radius, prefix + "spherivoid_radius"));
	}

private:
	T m_Radius;
};

/// <summary>
/// farblur.
/// </summary>
template <typename T>
class FarblurVariation : public ParametricVariation<T>
{
public:
	FarblurVariation(T weight = 1.0) : ParametricVariation<T>("farblur", eVariationId::VAR_FARBLUR, weight)
	{
		Init();
	}

	PARVARCOPY(FarblurVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T r = m_Weight * (Sqr(helper.In.x - m_XOrigin) +
						  Sqr(helper.In.y - m_YOrigin) +
						  Sqr(helper.In.z - m_ZOrigin)) *
			  (rand.Frand01<T>() + rand.Frand01<T>() + rand.Frand01<T>() + rand.Frand01<T>() - 2);
		T u = rand.Frand01<T>() * M_2PI;
		T su = std::sin(u);
		T cu = std::cos(u);
		T v = rand.Frand01<T>() * M_2PI;
		T sv = std::sin(v);
		T cv = std::cos(v);
		helper.Out.x = m_X * r * sv * cu;
		helper.Out.y = m_Y * r * sv * su;
		helper.Out.z = m_Z * r * cv;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string x       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string y       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string z       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string xOrigin = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string yOrigin = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string zOrigin = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t xmx = vIn.x - " << xOrigin << ";\n"
		   << "\t\treal_t ymy = vIn.y - " << yOrigin << ";\n"
		   << "\t\treal_t zmz = vIn.z - " << zOrigin << ";\n"
		   << "\t\treal_t r = " << weight << " * (fma(xmx, xmx, fma(ymy, ymy, SQR(zmz))))\n"
		   << "\t\t * (MwcNext01(mwc) + MwcNext01(mwc) + MwcNext01(mwc) + MwcNext01(mwc) - 2);\n"
		   << "\t\treal_t u = MwcNext01(mwc) * M_2PI;\n"
		   << "\t\treal_t su = sin(u);\n"
		   << "\t\treal_t cu = cos(u);\n"
		   << "\t\treal_t v = MwcNext01(mwc) * M_2PI;\n"
		   << "\t\treal_t sv = sin(v);\n"
		   << "\t\treal_t cv = cos(v);\n"
		   << "\n"
		   << "\t\tvOut.x = " << x << " * r * sv * cu;\n"
		   << "\t\tvOut.y = " << y << " * r * sv * su;\n"
		   << "\t\tvOut.z = " << z << " * r * cv;\n"
		   << "\t}\n";
		return ss.str();
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_X, prefix + "farblur_x", 1));
		m_Params.push_back(ParamWithName<T>(&m_Y, prefix + "farblur_y", 1));
		m_Params.push_back(ParamWithName<T>(&m_Z, prefix + "farblur_z", 1));
		m_Params.push_back(ParamWithName<T>(&m_XOrigin, prefix + "farblur_x_origin"));
		m_Params.push_back(ParamWithName<T>(&m_YOrigin, prefix + "farblur_y_origin"));
		m_Params.push_back(ParamWithName<T>(&m_ZOrigin, prefix + "farblur_z_origin"));
	}

private:
	T m_X;
	T m_Y;
	T m_Z;
	T m_XOrigin;
	T m_YOrigin;
	T m_ZOrigin;
};

/// <summary>
/// curl_sp.
/// </summary>
template <typename T>
class CurlSPVariation : public ParametricVariation<T>
{
public:
	CurlSPVariation(T weight = 1.0) : ParametricVariation<T>("curl_sp", eVariationId::VAR_CURL_SP, weight)
	{
		Init();
	}

	PARVARCOPY(CurlSPVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		const T x = VarFuncs<T>::Powq4c(helper.In.x, m_Power);
		const T y = VarFuncs<T>::Powq4c(helper.In.y, m_Power);
		const T z = VarFuncs<T>::Powq4c(helper.In.z, m_Power);
		const T d = SQR(x) - SQR(y);
		const T re = VarFuncs<T>::Spread(m_C1 * x + m_C2 * d, m_Sx) + 1;
		const T im = VarFuncs<T>::Spread(m_C1 * y + m_C2x2 * x * y, m_Sy);
		T c = Zeps(VarFuncs<T>::Powq4c(SQR(re) + SQR(im), m_PowerInv));
		const T r = m_Weight / c;
		helper.Out.x = (x * re + y * im) * r;
		helper.Out.y = (y * re - x * im) * r;
		helper.Out.z = (z * m_Weight) / c;
		outPoint.m_ColorX = Clamp<T>(outPoint.m_ColorX + m_DcAdjust * c, 0, 1);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string power = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string c1 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string c2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string sx = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string sy = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string dc = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string c2x2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string dcAdjust = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string powerInv = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\tconst real_t x = Powq4c(vIn.x, " << power << ");\n"
		   << "\t\tconst real_t y = Powq4c(vIn.y, " << power << ");\n"
		   << "\t\tconst real_t z = Powq4c(vIn.z, " << power << ");\n"
		   << "\t\tconst real_t d = fma(x, x, -SQR(y));\n"
		   << "\t\tconst real_t re = Spread(fma(" << c1 << ", x, " << c2 << " * d), " << sx << ") + (real_t)(1.0);\n"
		   << "\t\tconst real_t im = Spread(fma(" << c1 << ", y, " << c2x2 << " * x * y), " << sy << ");\n"
		   << "\t\treal_t c = Zeps(Powq4c(fma(re, re, SQR(im)), " << powerInv << "));\n"
		   << "\n"
		   << "\t\tconst real_t r = " << weight << " / c;\n"
		   << "\n"
		   << "\t\tvOut.x = fma(x, re, y * im) * r;\n"
		   << "\t\tvOut.y = fma(y, re, -(x * im)) * r;\n"
		   << "\t\tvOut.z = (z * " << weight << ") / c;\n"
		   << "\t\toutPoint->m_ColorX = clamp(fma(" << dcAdjust << ", c, outPoint->m_ColorX), (real_t)(0.0), (real_t)(1.0));\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Hypot", "Spread", "SignNz", "Powq4", "Powq4c", "Zeps" };
	}

	virtual void Precalc() override
	{
		m_C2x2 = 2 * m_C2;
		m_DcAdjust = T(0.1) * m_Dc;
		m_Power = Zeps(m_Power);
		m_PowerInv = 1 / m_Power;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Power, prefix + "curl_sp_pow", 1, eParamType::REAL_NONZERO));
		m_Params.push_back(ParamWithName<T>(&m_C1, prefix + "curl_sp_c1"));
		m_Params.push_back(ParamWithName<T>(&m_C2, prefix + "curl_sp_c2"));
		m_Params.push_back(ParamWithName<T>(&m_Sx, prefix + "curl_sp_sx"));
		m_Params.push_back(ParamWithName<T>(&m_Sy, prefix + "curl_sp_sy"));
		m_Params.push_back(ParamWithName<T>(&m_Dc, prefix + "curl_sp_dc"));
		m_Params.push_back(ParamWithName<T>(true, &m_C2x2, prefix + "curl_sp_c2_x2"));
		m_Params.push_back(ParamWithName<T>(true, &m_DcAdjust, prefix + "curl_sp_dc_adjust"));
		m_Params.push_back(ParamWithName<T>(true, &m_PowerInv, prefix + "curl_sp_power_inv"));
	}

private:
	T m_Power;
	T m_C1;
	T m_C2;
	T m_Sx;
	T m_Sy;
	T m_Dc;
	T m_C2x2;//Precalc.
	T m_DcAdjust;
	T m_PowerInv;
};

/// <summary>
/// heat.
/// </summary>
template <typename T>
class HeatVariation : public ParametricVariation<T>
{
public:
	HeatVariation(T weight = 1.0) : ParametricVariation<T>("heat", eVariationId::VAR_HEAT, weight, true, false, false, false, true)
	{
		Init();
	}

	PARVARCOPY(HeatVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T r = std::sqrt(std::abs(helper.m_PrecalcSumSquares + helper.In.z));
		r += m_Ar * std::sin(fma(m_Br, r, m_Cr));

		if (r == 0)
			r = EPS;

		T temp = fma(m_At, std::sin(fma(m_Bt, r, m_Ct)), helper.m_PrecalcAtanyx);
		T st = std::sin(temp);
		T ct = std::cos(temp);
		temp = fma(m_Ap, std::sin(fma(m_Bp, r, m_Cp)), std::acos(Clamp<T>(helper.In.z / r, -1, 1)));
		T sp = std::sin(temp);
		T cp = std::cos(temp);
		helper.Out.x = r * ct * sp;
		helper.Out.y = r * st * sp;
		helper.Out.z = r * cp;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		int i = 0;
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string thetaPeriod = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string thetaPhase = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string thetaAmp = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string phiPeriod = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string phiPhase = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string phiAmp = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string rperiod = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string rphase = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ramp = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string at = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string bt = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ct = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ap = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string bp = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cp = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ar = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string br = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cr = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t r = sqrt(fabs(precalcSumSquares + vIn.z));\n"
		   << "\n"
		   << "\t\tr += " << ar << " * sin(fma(" << br << ", r, " << cr << "));\n"
		   << "\n"
		   << "\t\tif (r == 0)\n"
		   << "\t\t	r = EPS;\n"
		   << "\n"
		   << "\t\treal_t temp = fma(" << at << ", sin(fma(" << bt << ", r, " << ct << ")), precalcAtanyx);\n"
		   << "\t\treal_t st = sin(temp);\n"
		   << "\t\treal_t ct = cos(temp);\n"
		   << "\n"
		   << "\t\ttemp = fma(" << ap << ", sin(fma(" << bp << ", r, " << cp << ")), acos(clamp(vIn.z / r, (real_t)(-1.0), (real_t)(1.0))));\n"
		   << "\n"
		   << "\t\treal_t sp = sin(temp);\n"
		   << "\t\treal_t cp = cos(temp);\n"
		   << "\n"
		   << "\t\tvOut.x = r * ct * sp;\n"
		   << "\t\tvOut.y = r * st * sp;\n"
		   << "\t\tvOut.z = r * cp;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		T tx = m_ThetaPeriod == 0 ? 0 : (1 / m_ThetaPeriod);
		T px = m_PhiPeriod == 0 ? 0 : (1 / m_PhiPeriod);
		T rx = m_Rperiod == 0 ? 0 : (1 / m_Rperiod);
		m_At = m_Weight * m_ThetaAmp;
		m_Bt = M_2PI * tx;
		m_Ct = m_ThetaPhase * tx;
		m_Ap = m_Weight * m_PhiAmp;
		m_Bp = M_2PI * px;
		m_Cp = m_PhiPhase * px;
		m_Ar = m_Weight * m_Ramp;
		m_Br = M_2PI * rx;
		m_Cr = m_Rphase * rx;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_ThetaPeriod, prefix + "heat_theta_period", 1));
		m_Params.push_back(ParamWithName<T>(&m_ThetaPhase, prefix + "heat_theta_phase"));
		m_Params.push_back(ParamWithName<T>(&m_ThetaAmp, prefix + "heat_theta_amp", 1));
		m_Params.push_back(ParamWithName<T>(&m_PhiPeriod, prefix + "heat_phi_period", 1));
		m_Params.push_back(ParamWithName<T>(&m_PhiPhase, prefix + "heat_phi_phase"));
		m_Params.push_back(ParamWithName<T>(&m_PhiAmp, prefix + "heat_phi_amp"));
		m_Params.push_back(ParamWithName<T>(&m_Rperiod, prefix + "heat_r_period", 1));
		m_Params.push_back(ParamWithName<T>(&m_Rphase, prefix + "heat_r_phase"));
		m_Params.push_back(ParamWithName<T>(&m_Ramp, prefix + "heat_r_amp"));
		m_Params.push_back(ParamWithName<T>(true, &m_At, prefix + "heat_at"));
		m_Params.push_back(ParamWithName<T>(true, &m_Bt, prefix + "heat_bt"));
		m_Params.push_back(ParamWithName<T>(true, &m_Ct, prefix + "heat_ct"));
		m_Params.push_back(ParamWithName<T>(true, &m_Ap, prefix + "heat_ap"));
		m_Params.push_back(ParamWithName<T>(true, &m_Bp, prefix + "heat_bp"));
		m_Params.push_back(ParamWithName<T>(true, &m_Cp, prefix + "heat_cp"));
		m_Params.push_back(ParamWithName<T>(true, &m_Ar, prefix + "heat_ar"));
		m_Params.push_back(ParamWithName<T>(true, &m_Br, prefix + "heat_br"));
		m_Params.push_back(ParamWithName<T>(true, &m_Cr, prefix + "heat_cr"));
	}

private:
	T m_ThetaPeriod;
	T m_ThetaPhase;
	T m_ThetaAmp;
	T m_PhiPeriod;
	T m_PhiPhase;
	T m_PhiAmp;
	T m_Rperiod;
	T m_Rphase;
	T m_Ramp;
	T m_At;//Precalc.
	T m_Bt;
	T m_Ct;
	T m_Ap;
	T m_Bp;
	T m_Cp;
	T m_Ar;
	T m_Br;
	T m_Cr;
};

/// <summary>
/// interference2.
/// </summary>
template <typename T>
class Interference2Variation : public ParametricVariation<T>
{
public:
	Interference2Variation(T weight = 1.0) : ParametricVariation<T>("interference2", eVariationId::VAR_INTERFERENCE2, weight)
	{
		Init();
	}

	PARVARCOPY(Interference2Variation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T fp1x;
		T fp1y;
		T fp2x;
		T fp2y;

		switch (int(m_T1))
		{
			case 0:
				fp1x = Sine(m_A1, m_B1, m_C1, m_P1, helper.In.x);
				fp1y = Sine(m_A1, m_B1, m_C1, m_P1, helper.In.y);
				break;

			case 1:
				fp1x = Tri(m_A1, m_B1, m_C1, m_P1, helper.In.x);
				fp1y = Tri(m_A1, m_B1, m_C1, m_P1, helper.In.y);
				break;

			case 2:
				fp1x = Squ(m_A1, m_B1, m_C1, m_P1, helper.In.x);
				fp1y = Squ(m_A1, m_B1, m_C1, m_P1, helper.In.y);
				break;

			default:
				fp1x = Sine(m_A1, m_B1, m_C1, m_P1, helper.In.x);
				fp1y = Sine(m_A1, m_B1, m_C1, m_P1, helper.In.y);
				break;
		}

		switch (int(m_T2))
		{
			case 0:
				fp2x = Sine(m_A2, m_B2, m_C2, m_P2, helper.In.x);
				fp2y = Sine(m_A2, m_B2, m_C2, m_P2, helper.In.y);
				break;

			case 1:
				fp2x = Tri(m_A2, m_B2, m_C2, m_P2, helper.In.x);
				fp2y = Tri(m_A2, m_B2, m_C2, m_P2, helper.In.y);
				break;

			case 2:
				fp2x = Squ(m_A2, m_B2, m_C2, m_P2, helper.In.x);
				fp2y = Squ(m_A2, m_B2, m_C2, m_P2, helper.In.y);
				break;

			default:
				fp2x = Sine(m_A2, m_B2, m_C2, m_P2, helper.In.x);
				fp2y = Sine(m_A2, m_B2, m_C2, m_P2, helper.In.y);
				break;
		}

		helper.Out.x = m_Weight * (fp1x + fp2x);
		helper.Out.y = m_Weight * (fp1y + fp2y);
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string a1 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string b1 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string c1 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string p1 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string t1 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string a2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string b2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string c2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string p2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string t2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t fp1x;\n"
		   << "\t\treal_t fp1y;\n"
		   << "\t\treal_t fp2x;\n"
		   << "\t\treal_t fp2y;\n"
		   << "\n"
		   << "\t\tswitch ((int)" << t1 << ")\n"
		   << "\t\t{\n"
		   << "\t\t	case 0:\n"
		   << "\t\t		fp1x = Interference2Sine(" << a1 << ", " << b1 << ", " << c1 << ", " << p1 << ", vIn.x);\n"
		   << "\t\t		fp1y = Interference2Sine(" << a1 << ", " << b1 << ", " << c1 << ", " << p1 << ", vIn.y);\n"
		   << "\t\t		break;\n"
		   << "\t\t	case 1:\n"
		   << "\t\t		fp1x = Interference2Tri(" << a1 << ", " << b1 << ", " << c1 << ", " << p1 << ", vIn.x);\n"
		   << "\t\t		fp1y = Interference2Tri(" << a1 << ", " << b1 << ", " << c1 << ", " << p1 << ", vIn.y);\n"
		   << "\t\t		break;\n"
		   << "\t\t	case 2:\n"
		   << "\t\t		fp1x = Interference2Squ(" << a1 << ", " << b1 << ", " << c1 << ", " << p1 << ", vIn.x);\n"
		   << "\t\t		fp1y = Interference2Squ(" << a1 << ", " << b1 << ", " << c1 << ", " << p1 << ", vIn.y);\n"
		   << "\t\t		break;\n"
		   << "\t\t	default:\n"
		   << "\t\t		fp1x = Interference2Sine(" << a1 << ", " << b1 << ", " << c1 << ", " << p1 << ", vIn.x);\n"
		   << "\t\t		fp1y = Interference2Sine(" << a1 << ", " << b1 << ", " << c1 << ", " << p1 << ", vIn.y);\n"
		   << "\t\t		break;\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tswitch ((int)" << t2 << ")\n"
		   << "\t\t{\n"
		   << "\t\t	case 0:\n"
		   << "\t\t		fp2x = Interference2Sine(" << a2 << ", " << b2 << ", " << c2 << ", " << p2 << ", vIn.x);\n"
		   << "\t\t		fp2y = Interference2Sine(" << a2 << ", " << b2 << ", " << c2 << ", " << p2 << ", vIn.y);\n"
		   << "\t\t		break;\n"
		   << "\t\t	case 1:\n"
		   << "\t\t		fp2x = Interference2Tri(" << a2 << ", " << b2 << ", " << c2 << ", " << p2 << ", vIn.x);\n"
		   << "\t\t		fp2y = Interference2Tri(" << a2 << ", " << b2 << ", " << c2 << ", " << p2 << ", vIn.y);\n"
		   << "\t\t		break;\n"
		   << "\t\t	case 2:\n"
		   << "\t\t		fp2x = Interference2Squ(" << a2 << ", " << b2 << ", " << c2 << ", " << p2 << ", vIn.x);\n"
		   << "\t\t		fp2y = Interference2Squ(" << a2 << ", " << b2 << ", " << c2 << ", " << p2 << ", vIn.y);\n"
		   << "\t\t		break;\n"
		   << "\t\t	default:\n"
		   << "\t\t		fp2x = Interference2Sine(" << a2 << ", " << b2 << ", " << c2 << ", " << p2 << ", vIn.x);\n"
		   << "\t\t		fp2y = Interference2Sine(" << a2 << ", " << b2 << ", " << c2 << ", " << p2 << ", vIn.y);\n"
		   << "\t\t		break;\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * (fp1x + fp2x);\n"
		   << "\t\tvOut.y = " << weight << " * (fp1y + fp2y);\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual string OpenCLFuncsString() const override
	{
		return
			"real_t Interference2Sine(real_t a, real_t b, real_t c, real_t p, real_t x)\n"
			"{\n"
			"	return a * pow(fabs(sin(fma(b, x, c))), p);\n"
			"}\n"
			"\n"
			"real_t Interference2Tri(real_t a, real_t b, real_t c, real_t p, real_t x)\n"
			"{\n"
			"	return a * 2 * pow(fabs(asin(cos(fma(b, x, c - MPI2)))) * M1PI, p);\n"
			"}\n"
			"\n"
			"real_t Interference2Squ(real_t a, real_t b, real_t c, real_t p, real_t x)\n"
			"{\n"
			"	return a * pow(sin(fma(b, x, c)) < 0 ? EPS : 1, p);\n"
			"}\n"
			"\n";
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_A1, prefix + "interference2_a1", 1));//Original used a prefix of intrfr2_, which is incompatible with Ember's design.
		m_Params.push_back(ParamWithName<T>(&m_B1, prefix + "interference2_b1", 1));
		m_Params.push_back(ParamWithName<T>(&m_C1, prefix + "interference2_c1"));
		m_Params.push_back(ParamWithName<T>(&m_P1, prefix + "interference2_p1", 1));
		m_Params.push_back(ParamWithName<T>(&m_T1, prefix + "interference2_t1", 0, eParamType::INTEGER, 0, 2));
		m_Params.push_back(ParamWithName<T>(&m_A2, prefix + "interference2_a2", 1));
		m_Params.push_back(ParamWithName<T>(&m_B2, prefix + "interference2_b2", 1));
		m_Params.push_back(ParamWithName<T>(&m_C2, prefix + "interference2_c2"));
		m_Params.push_back(ParamWithName<T>(&m_P2, prefix + "interference2_p2", 1));
		m_Params.push_back(ParamWithName<T>(&m_T2, prefix + "interference2_t2", 0, eParamType::INTEGER, 0, 2));
	}

private:
	inline static T Sine(T a, T b, T c, T p, T x)
	{
		return a * std::pow(std::abs(std::sin(b * x + c)), p);//Original did not fabs().
	}

	inline static T Tri(T a, T b, T c, T p, T x)
	{
		return a * 2 * std::pow(std::abs(std::asin(std::cos(b * x + c - T(M_PI_2)))) * T(M_1_PI), p);//Original did not fabs().
	}

	inline static T Squ(T a, T b, T c, T p, T x)
	{
		return a * std::pow(std::sin(b * x + c) < 0 ? EPS : T(1), p);//Original passed -1 to pow if sin() was < 0. Doing so will return NaN, so EPS is passed instead.
	}

	T m_A1;
	T m_B1;
	T m_C1;
	T m_P1;
	T m_T1;
	T m_A2;
	T m_B2;
	T m_C2;
	T m_P2;
	T m_T2;
};

/// <summary>
/// sinq.
/// </summary>
template <typename T>
class SinqVariation : public Variation<T>
{
public:
	SinqVariation(T weight = 1.0) : Variation<T>("sinq", eVariationId::VAR_SINQ, weight) { }

	VARCOPY(SinqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T absV = VarFuncs<T>::Hypot(helper.In.y, helper.In.z);
		T s = std::sin(helper.In.x);
		T c = std::cos(helper.In.x);
		T sh = std::sinh(absV);
		T ch = std::cosh(absV);
		T d = m_Weight * c * sh / Zeps(absV);
		helper.Out.x = m_Weight * s * ch;
		helper.Out.y = d * helper.In.y;
		helper.Out.z = d * helper.In.z;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t absV = Hypot(vIn.y, vIn.z);\n"
		   << "\t\treal_t s = sin(vIn.x);\n"
		   << "\t\treal_t c = cos(vIn.x);\n"
		   << "\t\treal_t sh = sinh(absV);\n"
		   << "\t\treal_t ch = cosh(absV);\n"
		   << "\t\treal_t d = " << weight << " * c * sh / Zeps(absV);\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * s * ch;\n"
		   << "\t\tvOut.y = d * vIn.y;\n"
		   << "\t\tvOut.z = d * vIn.z;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Hypot", "Zeps" };
	}
};

/// <summary>
/// sinhq.
/// </summary>
template <typename T>
class SinhqVariation : public Variation<T>
{
public:
	SinhqVariation(T weight = 1.0) : Variation<T>("sinhq", eVariationId::VAR_SINHQ, weight) { }

	VARCOPY(SinhqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T absV = VarFuncs<T>::Hypot(helper.In.y, helper.In.z);
		T s = std::sin(absV);
		T c = std::cos(absV);
		T sh = std::sinh(helper.In.x);
		T ch = std::cosh(helper.In.x);
		T d = m_Weight * ch * s / Zeps(absV);
		helper.Out.x = m_Weight * sh * c;
		helper.Out.y = d * helper.In.y;
		helper.Out.z = d * helper.In.z;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t absV = Hypot(vIn.y, vIn.z);\n"
		   << "\t\treal_t s = sin(absV);\n"
		   << "\t\treal_t c = cos(absV);\n"
		   << "\t\treal_t sh = sinh(vIn.x);\n"
		   << "\t\treal_t ch = cosh(vIn.x);\n"
		   << "\t\treal_t d = " << weight << " * ch * s / Zeps(absV);\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * sh * c;\n"
		   << "\t\tvOut.y = d * vIn.y;\n"
		   << "\t\tvOut.z = d * vIn.z;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Hypot", "Zeps" };
	}
};

/// <summary>
/// secq.
/// </summary>
template <typename T>
class SecqVariation : public Variation<T>
{
public:
	SecqVariation(T weight = 1.0) : Variation<T>("secq", eVariationId::VAR_SECQ, weight, true) { }

	VARCOPY(SecqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T absV = VarFuncs<T>::Hypot(helper.In.y, helper.In.z);
		T ni = m_Weight / Zeps(helper.m_PrecalcSumSquares + SQR(helper.In.z));
		T s = std::sin(-helper.In.x);
		T c = std::cos(-helper.In.x);
		T sh = std::sinh(absV);
		T ch = std::cosh(absV);
		T d = ni * s * sh / Zeps(absV);
		helper.Out.x = c * ch * ni;
		helper.Out.y = -(d * helper.In.y);
		helper.Out.z = -(d * helper.In.z);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t absV = Hypot(vIn.y, vIn.z);\n"
		   << "\t\treal_t ni = " << weight << " / Zeps(fma(vIn.z, vIn.z, precalcSumSquares));\n"
		   << "\t\treal_t s = sin(-vIn.x);\n"
		   << "\t\treal_t c = cos(-vIn.x);\n"
		   << "\t\treal_t sh = sinh(absV);\n"
		   << "\t\treal_t ch = cosh(absV);\n"
		   << "\t\treal_t d = ni * s * sh / Zeps(absV);\n"
		   << "\n"
		   << "\t\tvOut.x =   c * ch * ni;\n"
		   << "\t\tvOut.y = -(d * vIn.y);\n"
		   << "\t\tvOut.z = -(d * vIn.z);\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Hypot", "Zeps" };
	}
};

/// <summary>
/// sechq.
/// </summary>
template <typename T>
class SechqVariation : public Variation<T>
{
public:
	SechqVariation(T weight = 1.0) : Variation<T>("sechq", eVariationId::VAR_SECHQ, weight, true) { }

	VARCOPY(SechqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T absV = VarFuncs<T>::Hypot(helper.In.y, helper.In.z);
		T ni = m_Weight / Zeps(helper.m_PrecalcSumSquares + SQR(helper.In.z));
		T s = std::sin(absV);
		T c = std::cos(absV);
		T sh = std::sinh(helper.In.x);
		T ch = std::cosh(helper.In.x);
		T d = ni * sh * s / Zeps(absV);
		helper.Out.x = ch * c * ni;
		helper.Out.y = -(d * helper.In.y);
		helper.Out.z = -(d * helper.In.z);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t absV = Hypot(vIn.y, vIn.z);\n"
		   << "\t\treal_t ni = " << weight << " / Zeps(fma(vIn.z, vIn.z, precalcSumSquares));\n"
		   << "\t\treal_t s = sin(absV);\n"
		   << "\t\treal_t c = cos(absV);\n"
		   << "\t\treal_t sh = sinh(vIn.x);\n"
		   << "\t\treal_t ch = cosh(vIn.x);\n"
		   << "\t\treal_t d = ni * sh * s / absV;\n"
		   << "\n"
		   << "\t\tvOut.x =  ch * c * ni;\n"
		   << "\t\tvOut.y = -(d * vIn.y);\n"
		   << "\t\tvOut.z = -(d * vIn.z);\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Hypot", "Zeps" };
	}
};

/// <summary>
/// tanq.
/// </summary>
template <typename T>
class TanqVariation : public Variation<T>
{
public:
	TanqVariation(T weight = 1.0) : Variation<T>("tanq", eVariationId::VAR_TANQ, weight) { }

	VARCOPY(TanqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T sysz = SQR(helper.In.y) + SQR(helper.In.z);
		T absV = std::sqrt(sysz);
		T ni = m_Weight / Zeps(SQR(helper.In.x) + sysz);
		T s = std::sin(helper.In.x);
		T c = std::cos(helper.In.x);
		T sh = std::sinh(absV);
		T ch = std::cosh(absV);
		T d = c * sh / Zeps(absV);
		T b = -s * sh / Zeps(absV);
		T stcv = s * ch;
		T nstcv = -stcv;
		T ctcv = c * ch;
		helper.Out.x = (stcv * ctcv + d * b * sysz) * ni;
		helper.Out.y = (nstcv * b * helper.In.y + d * helper.In.y * ctcv) * ni;
		helper.Out.z = (nstcv * b * helper.In.z + d * helper.In.z * ctcv) * ni;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t sysz = fma(vIn.y, vIn.y, SQR(vIn.z));\n"
		   << "\t\treal_t absV = sqrt(sysz);\n"
		   << "\t\treal_t ni = " << weight << " / Zeps(fma(vIn.x, vIn.x, sysz));\n"
		   << "\t\treal_t s = sin(vIn.x);\n"
		   << "\t\treal_t c = cos(vIn.x);\n"
		   << "\t\treal_t sh = sinh(absV);\n"
		   << "\t\treal_t ch = cosh(absV);\n"
		   << "\t\treal_t d = c * sh / Zeps(absV);\n"
		   << "\t\treal_t b = -s * sh / Zeps(absV);\n"
		   << "\t\treal_t stcv = s * ch;\n"
		   << "\t\treal_t nstcv = -stcv;\n"
		   << "\t\treal_t ctcv = c * ch;\n"
		   << "\n"
		   << "\t\tvOut.x = fma(stcv, ctcv, d * b * sysz) * ni;\n"
		   << "\t\tvOut.y = fma(nstcv, b * vIn.y, d * vIn.y * ctcv) * ni;\n"
		   << "\t\tvOut.z = fma(nstcv, b * vIn.z, d * vIn.z * ctcv) * ni;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps" };
	}
};

/// <summary>
/// tanhq.
/// </summary>
template <typename T>
class TanhqVariation : public Variation<T>
{
public:
	TanhqVariation(T weight = 1.0) : Variation<T>("tanhq", eVariationId::VAR_TANHQ, weight) { }

	VARCOPY(TanhqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T sysz = SQR(helper.In.y) + SQR(helper.In.z);
		T absV = std::sqrt(sysz);
		T ni = m_Weight / Zeps(SQR(helper.In.x) + sysz);
		T s = std::sin(absV);
		T c = std::cos(absV);
		T sh = std::sinh(helper.In.x);
		T ch = std::cosh(helper.In.x);
		T d = ch * s / Zeps(absV);
		T b = sh * s / Zeps(absV);
		T stcv = sh * c;
		T nstcv = -stcv;
		T ctcv = c * ch;
		helper.Out.x = (stcv * ctcv + d * b * sysz) * ni;
		helper.Out.y = (nstcv * b * helper.In.y + d * helper.In.y * ctcv) * ni;
		helper.Out.z = (nstcv * b * helper.In.z + d * helper.In.z * ctcv) * ni;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t sysz = fma(vIn.y, vIn.y, SQR(vIn.z));\n"
		   << "\t\treal_t absV = sqrt(sysz);\n"
		   << "\t\treal_t ni = " << weight << " / Zeps(fma(vIn.x, vIn.x, sysz));\n"
		   << "\t\treal_t s = sin(absV);\n"
		   << "\t\treal_t c = cos(absV);\n"
		   << "\t\treal_t sh = sinh(vIn.x);\n"
		   << "\t\treal_t ch = cosh(vIn.x);\n"
		   << "\t\treal_t d = ch * s / Zeps(absV);\n"
		   << "\t\treal_t b = sh * s / Zeps(absV);\n"
		   << "\t\treal_t stcv = sh * c;\n"
		   << "\t\treal_t nstcv = -stcv;\n"
		   << "\t\treal_t ctcv = c * ch;\n"
		   << "\n"
		   << "\t\tvOut.x = fma(stcv, ctcv, d * b * sysz) * ni;\n"
		   << "\t\tvOut.y = fma(nstcv, b * vIn.y, d * vIn.y * ctcv) * ni;\n"
		   << "\t\tvOut.z = fma(nstcv, b * vIn.z, d * vIn.z * ctcv) * ni;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps" };
	}
};

/// <summary>
/// cosq.
/// </summary>
template <typename T>
class CosqVariation : public Variation<T>
{
public:
	CosqVariation(T weight = 1.0) : Variation<T>("cosq", eVariationId::VAR_COSQ, weight) { }

	VARCOPY(CosqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T absV = VarFuncs<T>::Hypot(helper.In.y, helper.In.z);
		T s = std::sin(helper.In.x);
		T c = std::cos(helper.In.x);
		T sh = std::sinh(absV);
		T ch = std::cosh(absV);
		T d = -m_Weight * s * sh / Zeps(absV);
		helper.Out.x = m_Weight * c * ch;
		helper.Out.y = d * helper.In.y;
		helper.Out.z = d * helper.In.z;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t absV = Hypot(vIn.y, vIn.z);\n"
		   << "\t\treal_t s = sin(vIn.x);\n"
		   << "\t\treal_t c = cos(vIn.x);\n"
		   << "\t\treal_t sh = sinh(absV);\n"
		   << "\t\treal_t ch = cosh(absV);\n"
		   << "\t\treal_t d = -" << weight << " * s * sh / Zeps(absV);\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * c * ch;\n"
		   << "\t\tvOut.y = d * vIn.y;\n"
		   << "\t\tvOut.z = d * vIn.z;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Hypot", "Zeps" };
	}
};

/// <summary>
/// coshq.
/// </summary>
template <typename T>
class CoshqVariation : public Variation<T>
{
public:
	CoshqVariation(T weight = 1.0) : Variation<T>("coshq", eVariationId::VAR_COSHQ, weight) { }

	VARCOPY(CoshqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T absV = VarFuncs<T>::Hypot(helper.In.y, helper.In.z);
		T s = std::sin(absV);
		T c = std::cos(absV);
		T sh = std::sinh(helper.In.x);
		T ch = std::cosh(helper.In.x);
		T d = m_Weight * sh * s / Zeps(absV);
		helper.Out.x = m_Weight * c * ch;
		helper.Out.y = d * helper.In.y;
		helper.Out.z = d * helper.In.z;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t absV = Hypot(vIn.y, vIn.z);\n"
		   << "\t\treal_t s = sin(absV);\n"
		   << "\t\treal_t c = cos(absV);\n"
		   << "\t\treal_t sh = sinh(vIn.x);\n"
		   << "\t\treal_t ch = cosh(vIn.x);\n"
		   << "\t\treal_t d = " << weight << " * sh * s / Zeps(absV);\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * c * ch;\n"
		   << "\t\tvOut.y = d * vIn.y;\n"
		   << "\t\tvOut.z = d * vIn.z;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Hypot", "Zeps" };
	}
};

/// <summary>
/// cotq.
/// </summary>
template <typename T>
class CotqVariation : public Variation<T>
{
public:
	CotqVariation(T weight = 1.0) : Variation<T>("cotq", eVariationId::VAR_COTQ, weight) { }

	VARCOPY(CotqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T sysz = SQR(helper.In.y) + SQR(helper.In.z);
		T absV = std::sqrt(sysz);
		T ni = m_Weight / Zeps(SQR(helper.In.x) + sysz);
		T s = std::sin(helper.In.x);
		T c = std::cos(helper.In.x);
		T sh = std::sinh(absV);
		T ch = std::cosh(absV);
		T d = c * sh / Zeps(absV);
		T b = -s * sh / Zeps(absV);
		T stcv = s * ch;
		T nstcv = -stcv;
		T ctcv = c * ch;
		helper.Out.x = (stcv * ctcv + d * b * sysz) * ni;
		helper.Out.y = -(nstcv * b * helper.In.y + d * helper.In.y * ctcv) * ni;
		helper.Out.z = -(nstcv * b * helper.In.z + d * helper.In.z * ctcv) * ni;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t sysz = fma(vIn.y, vIn.y, SQR(vIn.z));\n"
		   << "\t\treal_t absV = sqrt(sysz);\n"
		   << "\t\treal_t ni = " << weight << " / Zeps(fma(vIn.x, vIn.x, sysz));\n"
		   << "\t\treal_t s = sin(vIn.x);\n"
		   << "\t\treal_t c = cos(vIn.x);\n"
		   << "\t\treal_t sh = sinh(absV);\n"
		   << "\t\treal_t ch = cosh(absV);\n"
		   << "\t\treal_t d = c * sh / Zeps(absV);\n"
		   << "\t\treal_t b = -s * sh / Zeps(absV);\n"
		   << "\t\treal_t stcv = s * ch;\n"
		   << "\t\treal_t nstcv = -stcv;\n"
		   << "\t\treal_t ctcv = c * ch;\n"
		   << "\n"
		   << "\t\tvOut.x =  fma(stcv, ctcv, d * b * sysz) * ni;\n"
		   << "\t\tvOut.y = -fma(nstcv * b, vIn.y, d * vIn.y * ctcv) * ni;\n"
		   << "\t\tvOut.z = -fma(nstcv * b, vIn.z, d * vIn.z * ctcv) * ni;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps" };
	}
};

/// <summary>
/// cothq.
/// </summary>
template <typename T>
class CothqVariation : public Variation<T>
{
public:
	CothqVariation(T weight = 1.0) : Variation<T>("cothq", eVariationId::VAR_COTHQ, weight) { }

	VARCOPY(CothqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T sysz = SQR(helper.In.y) + SQR(helper.In.z);
		T absV = std::sqrt(sysz);
		T ni = m_Weight / Zeps(Sqr(SQR(helper.In.x) + sysz));
		T s = std::sin(absV);
		T c = std::cos(absV);
		T sh = std::sinh(helper.In.x);
		T ch = std::cosh(helper.In.x);
		T d = ch * s / Zeps(absV);
		T b = sh * s / Zeps(absV);
		T stcv = sh * c;
		T nstcv = -stcv;
		T ctcv = ch * c;
		helper.Out.x = (stcv * ctcv + d * b * sysz) * ni;
		helper.Out.y = (nstcv * b * helper.In.y + d * helper.In.y * ctcv) * ni;
		helper.Out.z = (nstcv * b * helper.In.z + d * helper.In.z * ctcv) * ni;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t sysz = fma(vIn.y, vIn.y, SQR(vIn.z));\n"
		   << "\t\treal_t absV = sqrt(sysz);\n"
		   << "\t\treal_t ni = " << weight << " / Zeps(Sqr(fma(vIn.x, vIn.x, sysz)));\n"
		   << "\t\treal_t s = sin(absV);\n"
		   << "\t\treal_t c = cos(absV);\n"
		   << "\t\treal_t sh = sinh(vIn.x);\n"
		   << "\t\treal_t ch = cosh(vIn.x);\n"
		   << "\t\treal_t d = ch * s / Zeps(absV);\n"
		   << "\t\treal_t b = sh * s / Zeps(absV);\n"
		   << "\t\treal_t stcv = sh * c;\n"
		   << "\t\treal_t nstcv = -stcv;\n"
		   << "\t\treal_t ctcv = ch * c;\n"
		   << "\n"
		   << "\t\tvOut.x = fma(stcv, ctcv, d * b * sysz) * ni;\n"
		   << "\t\tvOut.y = fma(nstcv * b, vIn.y, d * vIn.y * ctcv) * ni;\n"
		   << "\t\tvOut.z = fma(nstcv * b, vIn.z, d * vIn.z * ctcv) * ni;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps", "Sqr" };
	}
};

/// <summary>
/// cscq.
/// </summary>
template <typename T>
class CscqVariation : public Variation<T>
{
public:
	CscqVariation(T weight = 1.0) : Variation<T>("cscq", eVariationId::VAR_CSCQ, weight, true) { }

	VARCOPY(CscqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T absV = VarFuncs<T>::Hypot(helper.In.y, helper.In.z);
		T ni = m_Weight / Zeps(helper.m_PrecalcSumSquares + SQR(helper.In.z));
		T s = std::sin(helper.In.x);
		T c = std::cos(helper.In.x);
		T sh = std::sinh(absV);
		T ch = std::cosh(absV);
		T d = ni * c * sh / Zeps(absV);
		helper.Out.x = s * ch * ni;
		helper.Out.y = -(d * helper.In.y);
		helper.Out.z = -(d * helper.In.z);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t absV = Hypot(vIn.y, vIn.z);\n"
		   << "\t\treal_t ni = " << weight << " / Zeps(fma(vIn.z, vIn.z, precalcSumSquares));\n"
		   << "\t\treal_t s = sin(vIn.x);\n"
		   << "\t\treal_t c = cos(vIn.x);\n"
		   << "\t\treal_t sh = sinh(absV);\n"
		   << "\t\treal_t ch = cosh(absV);\n"
		   << "\t\treal_t d = ni * c * sh / Zeps(absV);\n"
		   << "\n"
		   << "\t\tvOut.x =   s * ch * ni;\n"
		   << "\t\tvOut.y = -(d * vIn.y);\n"
		   << "\t\tvOut.z = -(d * vIn.z);\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Hypot", "Zeps" };
	}
};

/// <summary>
/// cschq.
/// </summary>
template <typename T>
class CschqVariation : public Variation<T>
{
public:
	CschqVariation(T weight = 1.0) : Variation<T>("cschq", eVariationId::VAR_CSCHQ, weight, true) { }

	VARCOPY(CschqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T absV = VarFuncs<T>::Hypot(helper.In.y, helper.In.z);
		T ni = m_Weight / Zeps(helper.m_PrecalcSumSquares + SQR(helper.In.z));
		T s = std::sin(absV);
		T c = std::cos(absV);
		T sh = std::sinh(helper.In.x);
		T ch = std::cosh(helper.In.x);
		T d = ni * ch * s / Zeps(absV);
		helper.Out.x = sh * c * ni;
		helper.Out.y = -(d * helper.In.y);
		helper.Out.z = -(d * helper.In.z);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t absV = Hypot(vIn.y, vIn.z);\n"
		   << "\t\treal_t ni = " << weight << " / Zeps(fma(vIn.z, vIn.z, precalcSumSquares));\n"
		   << "\t\treal_t s = sin(absV);\n"
		   << "\t\treal_t c = cos(absV);\n"
		   << "\t\treal_t sh = sinh(vIn.x);\n"
		   << "\t\treal_t ch = cosh(vIn.x);\n"
		   << "\t\treal_t d = ni * ch * s / Zeps(absV);\n"
		   << "\n"
		   << "\t\tvOut.x =  sh * c * ni;\n"
		   << "\t\tvOut.y = -(d * vIn.y);\n"
		   << "\t\tvOut.z = -(d * vIn.z);\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Hypot", "Zeps" };
	}
};

/// <summary>
/// estiq.
/// </summary>
template <typename T>
class EstiqVariation : public Variation<T>
{
public:
	EstiqVariation(T weight = 1.0) : Variation<T>("estiq", eVariationId::VAR_ESTIQ, weight) { }

	VARCOPY(EstiqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T absV = VarFuncs<T>::Hypot(helper.In.y, helper.In.z);
		T e = std::exp(helper.In.x);
		T s = std::sin(absV);
		T c = std::cos(absV);
		T a = e * s / Zeps(absV);
		helper.Out.x = m_Weight * e * c;
		helper.Out.y = m_Weight * a * helper.In.y;
		helper.Out.z = m_Weight * a * helper.In.z;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t absV = Hypot(vIn.y, vIn.z);\n"
		   << "\t\treal_t e = exp(vIn.x);\n"
		   << "\t\treal_t s = sin(absV);\n"
		   << "\t\treal_t c = cos(absV);\n"
		   << "\t\treal_t a = e * s / Zeps(absV);\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * e * c;\n"
		   << "\t\tvOut.y = " << weight << " * a * vIn.y;\n"
		   << "\t\tvOut.z = " << weight << " * a * vIn.z;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Hypot", "Zeps" };
	}
};

/// <summary>
/// loq.
/// </summary>
template <typename T>
class LoqVariation : public ParametricVariation<T>
{
public:
	LoqVariation(T weight = 1.0) : ParametricVariation<T>("loq", eVariationId::VAR_LOQ, weight)
	{
		Init();
	}

	PARVARCOPY(LoqVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T absV = VarFuncs<T>::Hypot(helper.In.y, helper.In.z);
		T c = m_Weight * std::atan2(absV, helper.In.x) / Zeps(absV);
		helper.Out.x = std::log(SQR(helper.In.x) + SQR(absV)) * m_Denom;
		helper.Out.y = c * helper.In.y;
		helper.Out.z = c * helper.In.z;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string base  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string denom = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t absV = Hypot(vIn.y, vIn.z);\n"
		   << "\t\treal_t c = " << weight << " * atan2(absV, vIn.x) / Zeps(absV);\n"
		   << "\n"
		   << "\t\tvOut.x = log(fma(vIn.x, vIn.x, SQR(absV))) * " << denom << ";\n"
		   << "\t\tvOut.y = c * vIn.y;\n"
		   << "\t\tvOut.z = c * vIn.z;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Hypot", "Zeps" };
	}

	virtual void Precalc() override
	{
		m_Denom = T(0.5) / std::log(m_Base);
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Base, prefix + "loq_base", T(M_E), eParamType::REAL, EPS, TMAX));
		m_Params.push_back(ParamWithName<T>(true, &m_Denom, prefix + "loq_denom"));//Precalc.
	}

private:
	T m_Base;
	T m_Denom;//Precalc.
};

/// <summary>
/// curvature.
/// </summary>
template <typename T>
class CurvatureVariation : public Variation<T>
{
public:
	CurvatureVariation(T weight = 1.0) : Variation<T>("curvature", eVariationId::VAR_CURVATURE, weight, true, true, false, false, true) { }

	VARCOPY(CurvatureVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		helper.Out.x = m_Weight / Zeps(helper.m_PrecalcSqrtSumSquares);
		helper.Out.y = helper.m_PrecalcAtanyx;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\tvOut.x = " << weight << " / Zeps(precalcSqrtSumSquares);\n"
		   << "\t\tvOut.y = precalcAtanyx;\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps" };
	}
};

/// <summary>
/// q_ode.
/// </summary>
template <typename T>
class QodeVariation : public ParametricVariation<T>
{
public:
	QodeVariation(T weight = 1.0) : ParametricVariation<T>("q_ode", eVariationId::VAR_Q_ODE, weight)
	{
		Init();
	}

	PARVARCOPY(QodeVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T sqx = SQR(helper.In.x);
		T sqy = SQR(helper.In.y);
		T xy = helper.In.x * helper.In.y;
		helper.Out.x = (m_Q01 + m_Weight * m_Q02 * helper.In.x + m_Q03 * sqx) +
					   (m_Q04 * xy + m_Q05 * helper.In.y + m_Q06 * sqy);
		helper.Out.y = (m_Q07 + m_Q08 * helper.In.x + m_Q09 * sqx) +
					   (m_Q10 * xy + m_Weight * m_Q11 * helper.In.y + m_Q12 * sqy);
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string q01 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string q02 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string q03 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string q04 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string q05 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string q06 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string q07 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string q08 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string q09 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string q10 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string q11 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string q12 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t sqx = SQR(vIn.x);\n"
		   << "\t\treal_t sqy = SQR(vIn.y);\n"
		   << "\t\treal_t xy = vIn.x * vIn.y;\n"
		   << "\n"
		   << "\t\tvOut.x = (" << q01 << " + fma(" << weight << ", " << q02 << " * vIn.x, " << q03 << " * sqx)) + \n"
		   << "\t\t			fma(" << q04 << ", xy, fma(" << q05 << ", vIn.y, " << q06 << " * sqy));\n"
		   << "\t\tvOut.y = (" << q07 << " + fma(" << q08 << ", vIn.x, " << q09 << " * sqx)) + \n"
		   << "\t\t			fma(" << q10 << ", xy, fma(" << weight << ", " << q11 << " * vIn.y, " << q12 << " * sqy));\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Q01, prefix + "q_ode01", 1));
		m_Params.push_back(ParamWithName<T>(&m_Q02, prefix + "q_ode02", -1));
		m_Params.push_back(ParamWithName<T>(&m_Q03, prefix + "q_ode03"));
		m_Params.push_back(ParamWithName<T>(&m_Q04, prefix + "q_ode04"));
		m_Params.push_back(ParamWithName<T>(&m_Q05, prefix + "q_ode05"));
		m_Params.push_back(ParamWithName<T>(&m_Q06, prefix + "q_ode06"));
		m_Params.push_back(ParamWithName<T>(&m_Q07, prefix + "q_ode07", 1));
		m_Params.push_back(ParamWithName<T>(&m_Q08, prefix + "q_ode08"));
		m_Params.push_back(ParamWithName<T>(&m_Q09, prefix + "q_ode09"));
		m_Params.push_back(ParamWithName<T>(&m_Q10, prefix + "q_ode10"));
		m_Params.push_back(ParamWithName<T>(&m_Q11, prefix + "q_ode11"));
		m_Params.push_back(ParamWithName<T>(&m_Q12, prefix + "q_ode12"));
	}

private:
	T m_Q01;
	T m_Q02;
	T m_Q03;
	T m_Q04;
	T m_Q05;
	T m_Q06;
	T m_Q07;
	T m_Q08;
	T m_Q09;
	T m_Q10;
	T m_Q11;
	T m_Q12;
};

/// <summary>
/// blur_heart.
/// </summary>
template <typename T>
class BlurHeartVariation : public ParametricVariation<T>
{
public:
	BlurHeartVariation(T weight = 1.0) : ParametricVariation<T>("blur_heart", eVariationId::VAR_BLUR_HEART, weight)
	{
		Init();
	}

	PARVARCOPY(BlurHeartVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T xx = (rand.Frand01<T>() - T(0.5)) * 2;
		T yy = (rand.Frand01<T>() - T(0.5)) * 2;
		T k = VarFuncs<T>::SignNz(yy);
		T yymax = ((m_A * std::pow(std::abs(xx), m_P) + k * m_B * std::sqrt(std::abs(1 - SQR(xx)))) - m_A);
		//The function must be in a range 0-1 to work properly.
		yymax /= Zeps(std::abs(m_A) + std::abs(m_B));

		//Quick and dirty way to force y to be in range without altering the density.
		if (k > 0)
		{
			if (yy > yymax)
				yy = yymax;
		}
		else
		{
			if (yy < yymax)
				yy = yymax;
		}

		helper.Out.x = xx * m_Weight;
		helper.Out.y = yy * m_Weight;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string p = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string a = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string b = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t xx = (MwcNext01(mwc) - (real_t)(0.5)) * 2;\n"
		   << "\t\treal_t yy = (MwcNext01(mwc) - (real_t)(0.5)) * 2;\n"
		   << "\t\treal_t k = SignNz(yy);\n"
		   << "\t\treal_t yymax = ((" << a << " * pow(fabs(xx), " << p << ") + k * " << b << " * sqrt(fabs(1 - SQR(xx)))) - " << a << ");\n"
		   << "\n"
		   << "\t\tyymax /= Zeps(fabs(" << a << ") + fabs(" << b << "));\n"
		   << "\n"
		   << "\t\tif (k > 0)\n"
		   << "\t\t{\n"
		   << "\t\t	if (yy > yymax)\n"
		   << "\t\t		yy = yymax;\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	if (yy < yymax)\n"
		   << "\t\t		yy = yymax;\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.x = xx * " << weight << ";\n"
		   << "\t\tvOut.y = yy * " << weight << ";\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "SignNz", "Zeps" };
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_P, prefix + "blur_heart_p", T(0.5)));
		m_Params.push_back(ParamWithName<T>(&m_A, prefix + "blur_heart_a", T(-T(0.6))));
		m_Params.push_back(ParamWithName<T>(&m_B, prefix + "blur_heart_b", T(0.7)));
	}

private:
	T m_P;
	T m_A;
	T m_B;
};

/// <summary>
/// Truchet.
/// </summary>
template <typename T>
class TruchetVariation : public ParametricVariation<T>
{
public:
	TruchetVariation(T weight = 1.0) : ParametricVariation<T>("Truchet", eVariationId::VAR_TRUCHET, weight)
	{
		Init();
	}

	PARVARCOPY(TruchetVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		int extended = int(m_Extended);
		T seed = m_AbsSeed;
		T r = -m_Rotation;
		T r0 = 0;
		T r1 = 0;
		T tileType = 0;
		T randInt = 0;
		T modBase = 65535;
		T multiplier = 32747;
		T offset = 12345;
		T niter = 0;
		T x = helper.In.x * m_Scale;
		T y = helper.In.y * m_Scale;
		int intx = int(Round(x));
		int inty = int(Round(y));
		int randiter;
		r = x - intx;

		if (r < 0)
			x = 1 + r;
		else
			x = r;

		r = y - inty;

		if (r < 0)
			y = 1 + r;
		else
			y = r;

		//Calculate the tile type.
		if (seed == 0)
			tileType = 0;
		else if (seed == 1)
			tileType = 1;
		else
		{
			if (extended == 0)
			{
				T xrand = Round(helper.In.x);
				T yrand = Round(helper.In.y);
				xrand = xrand * m_Seed2;
				yrand = yrand * m_Seed2;
				niter = xrand + yrand + xrand * yrand;
				randInt = (niter + seed) * m_Seed2 / 2;
				randInt = fmod((randInt * multiplier + offset), modBase);
			}
			else
			{
				int xrand = int(Round(helper.In.x));
				int yrand = int(Round(helper.In.y));
				seed = T(Floor<T>(seed));
				niter = T(abs(xrand + yrand + xrand * yrand));
				randInt = seed + niter;
				randiter = 0;

				while (randiter < niter && randiter < 20)//Allow it to escape.
				{
					randiter++;
					randInt = fmod((randInt * multiplier + offset), modBase);
				}
			}

			tileType = fmod(randInt, T(2));
		}

		//Drawing the points.
		if (extended == 0)//Fast drawmode
		{
			if (tileType < 1)
			{
				r0 = std::pow((pow(std::abs(x), m_Exponent) + std::pow(std::abs(y), m_Exponent)), m_OneOverEx);
				r1 = std::pow((pow(std::abs(x - 1), m_Exponent) + std::pow(std::abs(y - 1), m_Exponent)), m_OneOverEx);
			}
			else
			{
				r0 = std::pow((pow(std::abs(x - 1), m_Exponent) + std::pow(std::abs(y), m_Exponent)), m_OneOverEx);
				r1 = std::pow((pow(std::abs(x), m_Exponent) + std::pow(std::abs(y - 1), m_Exponent)), m_OneOverEx);
			}
		}
		else//Slow drawmode
		{
			if (tileType == 1)
			{
				r0 = std::pow((std::pow(std::abs(x), m_Exponent) + std::pow(std::abs(y), m_Exponent)), m_OneOverEx);
				r1 = std::pow((std::pow(std::abs(x - 1), m_Exponent) + std::pow(std::abs(y - 1), m_Exponent)), m_OneOverEx);
			}
			else
			{
				r0 = std::pow((std::pow(std::abs(x - 1), m_Exponent) + std::pow(std::abs(y), m_Exponent)), m_OneOverEx);
				r1 = std::pow((std::pow(std::abs(x), m_Exponent) + std::pow(std::abs(y - 1), m_Exponent)), m_OneOverEx);
			}
		}

		r = std::abs(r0 - T(0.5)) * m_OneOverRmax;

		if (r < 1)
		{
			helper.Out.x = m_Size * (x + Floor<T>(helper.In.x));
			helper.Out.y = m_Size * (y + Floor<T>(helper.In.y));
		}
		else
		{
			helper.Out.x = 0;//Needed because of possible sum below.
			helper.Out.y = 0;
		}

		r = std::abs(r1 - T(0.5)) * m_OneOverRmax;

		if (r < 1)
		{
			helper.Out.x += m_Size * (x + Floor<T>(helper.In.x));//The += is intended here.
			helper.Out.y += m_Size * (y + Floor<T>(helper.In.y));
		}

		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		int i = 0;
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string extended    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string exponent    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string arcWidth    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string rotation    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string size        = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string seed        = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string oneOverEx   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string absSeed     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string seed2       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string oneOverRmax = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scale       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\tint extended = (int)" << extended << ";\n"
		   << "\t\treal_t seed = " << absSeed << ";\n"
		   << "\t\treal_t r = -" << rotation << ";\n"
		   << "\t\treal_t r0 = 0;\n"
		   << "\t\treal_t r1 = 0;\n"
		   << "\t\treal_t tileType = 0;\n"
		   << "\t\treal_t randInt = 0;\n"
		   << "\t\treal_t modBase = 65535;\n"
		   << "\t\treal_t multiplier = 32747;\n"
		   << "\t\treal_t offset = 12345;\n"
		   << "\t\treal_t niter = 0;\n"
		   << "\t\treal_t x = vIn.x * " << scale << ";\n"
		   << "\t\treal_t y = vIn.y * " << scale << ";\n"
		   << "\t\tint intx = (int)Round(x);\n"
		   << "\t\tint inty = (int)Round(y);\n"
		   << "\t\tint randiter;\n"
		   << "\n"
		   << "\t\tr = x - intx;\n"
		   << "\n"
		   << "\t\tif (r < 0)\n"
		   << "\t\t	x = 1 + r;\n"
		   << "\t\telse\n"
		   << "\t\t	x = r;\n"
		   << "\n"
		   << "\t\tr = y - inty;\n"
		   << "\n"
		   << "\t\tif (r < 0)\n"
		   << "\t\t	y = 1 + r;\n"
		   << "\t\telse\n"
		   << "\t\t	y = r;\n"
		   << "\n"
		   << "\t\tif (seed == 0)\n"
		   << "\t\t	tileType = 0;\n"
		   << "\t\telse if (seed == 1)\n"
		   << "\t\t	tileType = 1;\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	if (extended == 0)\n"
		   << "\t\t	{\n"
		   << "\t\t		real_t xrand = Round(vIn.x);\n"
		   << "\t\t		real_t yrand = Round(vIn.y);\n"
		   << "\n"
		   << "\t\t		xrand = xrand * " << seed2 << ";\n"
		   << "\t\t		yrand = yrand * " << seed2 << ";\n"
		   << "\t\t		niter = fma(xrand, yrand, xrand + yrand);\n"
		   << "\t\t		randInt = (niter + seed) * " << seed2 << " / 2;\n"
		   << "\t\t		randInt = fmod(fma(randInt, multiplier, offset), modBase);\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		int xrand = (int)Round(vIn.x);\n"
		   << "\t\t		int yrand = (int)Round(vIn.y);\n"
		   << "\n"
		   << "\t\t		seed = floor(seed);\n"
		   << "\t\t		niter = (real_t)abs(xrand + yrand + xrand * yrand);\n"
		   << "\t\t		randInt = seed + niter;\n"
		   << "\t\t		randiter = 0;\n"
		   << "\n"
		   << "\t\t		while (randiter < niter && randiter < 20)\n"
		   << "\t\t		{\n"
		   << "\t\t			randiter++;\n"
		   << "\t\t			randInt = fmod((randInt * multiplier + offset), modBase);\n"
		   << "\t\t		}\n"
		   << "\t\t	}\n"
		   << "\n"
		   << "\t\t	tileType = fmod(randInt, 2);\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tif (extended == 0)\n"
		   << "\t\t{\n"
		   << "\t\t	if (tileType < 1)\n"
		   << "\t\t	{\n"
		   << "\t\t		r0 = pow((pow(fabs(x    ), " << exponent << ") + pow(fabs(y    ), " << exponent << ")), " << oneOverEx << ");\n"
		   << "\t\t		r1 = pow((pow(fabs(x - 1), " << exponent << ") + pow(fabs(y - 1), " << exponent << ")), " << oneOverEx << ");\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		r0 = pow((pow(fabs(x - 1), " << exponent << ") + pow(fabs(y    ), " << exponent << ")), " << oneOverEx << ");\n"
		   << "\t\t		r1 = pow((pow(fabs(x    ), " << exponent << ") + pow(fabs(y - 1), " << exponent << ")), " << oneOverEx << ");\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	if (tileType == 1)\n"
		   << "\t\t	{\n"
		   << "\t\t		r0 = pow((pow(fabs(x    ), " << exponent << ") + pow(fabs(y    ), " << exponent << ")), " << oneOverEx << ");\n"
		   << "\t\t		r1 = pow((pow(fabs(x - 1), " << exponent << ") + pow(fabs(y - 1), " << exponent << ")), " << oneOverEx << ");\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		r0 = pow((pow(fabs(x - 1), " << exponent << ") + pow(fabs(y    ), " << exponent << ")), " << oneOverEx << ");\n"
		   << "\t\t		r1 = pow((pow(fabs(x    ), " << exponent << ") + pow(fabs(y - 1), " << exponent << ")), " << oneOverEx << ");\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tr = fabs(r0 - (real_t)(0.5)) * " << oneOverRmax << ";\n"
		   << "\n"
		   << "\t\tif (r < 1)\n"
		   << "\t\t{\n"
		   << "\t\t	vOut.x = " << size << " * (x + floor(vIn.x));\n"
		   << "\t\t	vOut.y = " << size << " * (y + floor(vIn.y));\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	vOut.x = (real_t)(0.0);\n"
		   << "\t\t	vOut.y = (real_t)(0.0);\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tr = fabs(r1 - (real_t)(0.5)) * " << oneOverRmax << ";\n"
		   << "\n"
		   << "\t\tif (r < 1)\n"
		   << "\t\t{\n"
		   << "\t\t	vOut.x += " << size << " * (x + floor(vIn.x));\n"
		   << "\t\t	vOut.y += " << size << " * (y + floor(vIn.y));\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Round" };
	}

	virtual void Precalc() override
	{
		m_OneOverEx = 1 / m_Exponent;
		m_AbsSeed = std::abs(m_Seed);
		m_Seed2 = std::sqrt(Zeps(m_AbsSeed + (m_AbsSeed / 2))) / Zeps((m_AbsSeed * T(0.5))) * T(0.25);
		m_OneOverRmax = 1 / (T(0.5) * (std::pow(T(2), 1 / m_Exponent) - 1) * m_ArcWidth);
		m_Scale = (std::cos(-m_Rotation) - std::sin(-m_Rotation)) / m_Weight;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Extended, prefix + "Truchet_extended", 0, eParamType::INTEGER, 0, 1));
		m_Params.push_back(ParamWithName<T>(&m_Exponent, prefix + "Truchet_exponent", 2, eParamType::REAL_CYCLIC, T(0.001), 2));
		m_Params.push_back(ParamWithName<T>(&m_ArcWidth, prefix + "Truchet_arc_width", T(0.5), eParamType::REAL_CYCLIC, T(0.001), 1));
		m_Params.push_back(ParamWithName<T>(&m_Rotation, prefix + "Truchet_rotation"));
		m_Params.push_back(ParamWithName<T>(&m_Size, prefix + "Truchet_size", 1, eParamType::REAL_CYCLIC, T(0.001), 10));
		m_Params.push_back(ParamWithName<T>(&m_Seed, prefix + "Truchet_seed", 50));
		m_Params.push_back(ParamWithName<T>(true, &m_OneOverEx, prefix + "Truchet_one_over_ex"));//Precalc.
		m_Params.push_back(ParamWithName<T>(true, &m_AbsSeed, prefix + "Truchet_abs_seed"));
		m_Params.push_back(ParamWithName<T>(true, &m_Seed2, prefix + "Truchet_seed2"));
		m_Params.push_back(ParamWithName<T>(true, &m_OneOverRmax, prefix + "Truchet_one_over_rmax"));
		m_Params.push_back(ParamWithName<T>(true, &m_Scale, prefix + "Truchet_scale"));
	}

private:
	T m_Extended;
	T m_Exponent;
	T m_ArcWidth;
	T m_Rotation;
	T m_Size;
	T m_Seed;
	T m_OneOverEx;//Precalc.
	T m_AbsSeed;
	T m_Seed2;
	T m_OneOverRmax;
	T m_Scale;
};

/// <summary>
/// truchet_knot.
/// </summary>
template <typename T>
class TruchetKnotVariation : public Variation<T>
{
public:
	TruchetKnotVariation(T weight = 1.0) : Variation<T>("truchet_knot", eVariationId::VAR_TRUCHET_KNOT, weight) { }

	VARCOPY(TruchetKnotVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T wd = T(0.5);
		T space = T(0.1);
		T cellx = T(Floor<T>(helper.In.x));
		T celly = T(Floor<T>(helper.In.y));
		T xy0x = (rand.Frand01<T>() - T(0.5)) * wd;
		T xy0y = (rand.Frand01<T>() * 2 - 1) * (1 - space - wd * T(0.5));
		T dir0 = std::abs(cellx + celly);
		T dir1 = dir0 - 2 * Floor<T>(dir0 * T(0.5));
		T xyx, xyy;

		if (dir1 < 0.5)
		{
			xyx = xy0x;
			xyy = xy0y;
		}
		else
		{
			xyx = -xy0y;//y and x intentionally flipped.
			xyy = xy0x;
		}

		helper.Out.x = m_Weight * (cellx + xyx);
		helper.Out.y = m_Weight * (celly + xyy);
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t wd = 0.5;\n"
		   << "\t\treal_t space = 0.1;\n"
		   << "\t\treal_t cellx = floor(vIn.x);\n"
		   << "\t\treal_t celly = floor(vIn.y);\n"
		   << "\t\treal_t xy0x = (MwcNext01(mwc) - 0.5) * wd;\n"
		   << "\t\treal_t xy0y = fma(MwcNext01(mwc), (real_t)(2.0), (real_t)(-1.0)) * (1.0 - space - wd * 0.5);\n"
		   << "\t\treal_t dir0 = fabs(cellx + celly);\n"
		   << "\t\treal_t dir1 = dir0 - 2.0 * floor(dir0 / 2.0);\n"
		   << "\t\treal_t xyx, xyy;\n"
		   << "\n"
		   << "\t\tif (dir1 < 0.5)\n"
		   << "\t\t{\n"
		   << "\t\t	xyx = xy0x;\n"
		   << "\t\t	xyy = xy0y;\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	xyx = -xy0y;//y and x intentionally flipped.\n"
		   << "\t\t	xyy = xy0x;\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * (cellx + xyx);\n"
		   << "\t\tvOut.y = " << weight << " * (celly + xyy);\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}
};

/// <summary>
/// gdoffs.
/// </summary>
template <typename T>
class GdoffsVariation : public ParametricVariation<T>
{
public:
	GdoffsVariation(T weight = 1.0) : ParametricVariation<T>("gdoffs", eVariationId::VAR_GDOFFS, weight)
	{
		Init();
	}

	PARVARCOPY(GdoffsVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T oscX = GdoffsFosc(m_Dx, 1);
		T oscY = GdoffsFosc(m_Dy, 1);
		T inX = helper.In.x + m_Cx;
		T inY = helper.In.y + m_Cy;
		T outX;
		T outY;

		if (m_Square != 0)
		{
			outX = GdoffsFlip(GdoffsFlip(inX, GdoffsFosc(inX, 4), oscX), GdoffsFosc(GdoffsFclp(m_B * inX), 4), oscX);
			outY = GdoffsFlip(GdoffsFlip(inY, GdoffsFosc(inY, 4), oscX), GdoffsFosc(GdoffsFclp(m_B * inY), 4), oscX);
		}
		else
		{
			outX = GdoffsFlip(GdoffsFlip(inX, GdoffsFosc(inX, 4), oscX), GdoffsFosc(GdoffsFclp(m_B * inX), 4), oscX);
			outY = GdoffsFlip(GdoffsFlip(inY, GdoffsFosc(inY, 4), oscY), GdoffsFosc(GdoffsFclp(m_B * inY), 4), oscY);
		}

		helper.Out.x = m_Weight * outX;
		helper.Out.y = m_Weight * outY;
		helper.Out.z = m_Weight * helper.In.z;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string deltaX = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string deltaY = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string areaX = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string areaY = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string centerX = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string centerY = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string gamma = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string square = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string dx = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ax = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cx = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string dy = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ay = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cy = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string b = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t oscX = GdoffsFosc_(" << dx << ", 1);\n"
		   << "\t\treal_t oscY = GdoffsFosc_(" << dy << ", 1);\n"
		   << "\t\treal_t inX = vIn.x + " << cx << ";\n"
		   << "\t\treal_t inY = vIn.y + " << cy << ";\n"
		   << "\t\treal_t outX;\n"
		   << "\t\treal_t outY;\n"
		   << "\n"
		   << "\t\tif (" << square << " != 0)\n"
		   << "\t\t{\n"
		   << "\t\t	outX = GdoffsFlip(GdoffsFlip(inX, GdoffsFosc_(inX, 4), oscX), GdoffsFosc_(GdoffsFclp(" << b << " * inX), 4), oscX);\n"
		   << "\t\t	outY = GdoffsFlip(GdoffsFlip(inY, GdoffsFosc_(inY, 4), oscX), GdoffsFosc_(GdoffsFclp(" << b << " * inY), 4), oscX);\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	outX = GdoffsFlip(GdoffsFlip(inX, GdoffsFosc_(inX, 4), oscX), GdoffsFosc_(GdoffsFclp(" << b << " * inX), 4), oscX);\n"
		   << "\t\t	outY = GdoffsFlip(GdoffsFlip(inY, GdoffsFosc_(inY, 4), oscY), GdoffsFosc_(GdoffsFclp(" << b << " * inY), 4), oscY);\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * outX;\n"
		   << "\t\tvOut.y = " << weight << " * outY;\n"
		   << "\t\tvOut.z = " << weight << " * vIn.z;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual string OpenCLFuncsString() const override
	{
		return
			"inline real_t GdoffsFcip(real_t a) { return (real_t)((a < 0) ? -((int)(fabs(a)) + 1) : 0) + ((a > 1) ? ((int)(a)) : 0); }\n"
			"inline real_t GdoffsFclp(real_t a) { return ((a < 0) ? -(fmod(fabs(a), 1)) : fmod(fabs(a), 1)); }\n"
			"inline real_t GdoffsFscl(real_t a) { return GdoffsFclp((a + 1) / 2); }\n"
			"inline real_t GdoffsFosc_(real_t p, real_t a) { return GdoffsFscl(-1 * cos(p * a * M_2PI)); }\n"//Underscore added to name to prevent false detection with the global function Fosc() in TestGlobalFuncs() in EmberTester.
			"inline real_t GdoffsFlip(real_t a, real_t b, real_t c) { return fma(c, (b - a), a); }\n"
			"\n";
	}

	virtual void Precalc() override
	{
		const T agdod = T(0.1);
		const T agdoa = 2;
		const T agdoc = 1;
		m_Dx = m_DeltaX * agdod;
		m_Dy = m_DeltaY * agdod;
		m_Ax = ((std::abs(m_AreaX) < 0.1) ? T(0.1) : std::abs(m_AreaX)) * agdoa;
		m_Ay = ((std::abs(m_AreaY) < 0.1) ? T(0.1) : std::abs(m_AreaY)) * agdoa;
		m_Cx = m_CenterX * agdoc;
		m_Cy = m_CenterY * agdoc;
		m_B = m_Gamma * agdoa / (std::max(m_Ax, m_Ay));
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_DeltaX, prefix + "gdoffs_delta_x", 0, eParamType::REAL, 0, 16));
		m_Params.push_back(ParamWithName<T>(&m_DeltaY, prefix + "gdoffs_delta_y", 0, eParamType::REAL, 0, 16));
		m_Params.push_back(ParamWithName<T>(&m_AreaX, prefix + "gdoffs_area_x", 2));
		m_Params.push_back(ParamWithName<T>(&m_AreaY, prefix + "gdoffs_area_y", 2));
		m_Params.push_back(ParamWithName<T>(&m_CenterX, prefix + "gdoffs_center_x"));
		m_Params.push_back(ParamWithName<T>(&m_CenterY, prefix + "gdoffs_center_y"));
		m_Params.push_back(ParamWithName<T>(&m_Gamma, prefix + "gdoffs_gamma", 1, eParamType::INTEGER, 1, 6));
		m_Params.push_back(ParamWithName<T>(&m_Square, prefix + "gdoffs_square", 0, eParamType::INTEGER, 0, 1));
		m_Params.push_back(ParamWithName<T>(true, &m_Dx, prefix + "gdoffs_dx"));
		m_Params.push_back(ParamWithName<T>(true, &m_Ax, prefix + "gdoffs_ax"));
		m_Params.push_back(ParamWithName<T>(true, &m_Cx, prefix + "gdoffs_cx"));
		m_Params.push_back(ParamWithName<T>(true, &m_Dy, prefix + "gdoffs_dyd"));
		m_Params.push_back(ParamWithName<T>(true, &m_Ay, prefix + "gdoffs_ay"));
		m_Params.push_back(ParamWithName<T>(true, &m_Cy, prefix + "gdoffs_cy"));
		m_Params.push_back(ParamWithName<T>(true, &m_B, prefix + "gdoffs_b"));
	}

private:
	static inline T GdoffsFcip(T a) { return T((a < 0) ? -(int(std::abs(a)) + 1) : 0) + ((a > 1) ? (int(a)) : 0); }
	static inline T GdoffsFclp(T a) { return ((a < 0) ? -(fmod(std::abs(a), T(1))) : fmod(std::abs(a), T(1))); }
	static inline T GdoffsFscl(T a) { return GdoffsFclp((a + 1) / 2); }
	static inline T GdoffsFosc(T p, T a) { return GdoffsFscl(-1 * std::cos(p * a * M_2PI)); }
	static inline T GdoffsFlip(T a, T b, T c) { return (c * (b - a) + a); }

	T m_DeltaX;//Params.
	T m_DeltaY;
	T m_AreaX;
	T m_AreaY;
	T m_CenterX;
	T m_CenterY;
	T m_Gamma;
	T m_Square;
	T m_Dx;//Precalc.
	T m_Ax;
	T m_Cx;
	T m_Dy;
	T m_Ay;
	T m_Cy;
	T m_B;
};

/// <summary>
/// octagon.
/// </summary>
template <typename T>
class OctagonVariation : public ParametricVariation<T>
{
public:
	OctagonVariation(T weight = 1.0) : ParametricVariation<T>("octagon", eVariationId::VAR_OCTAGON, weight)
	{
		Init();
	}

	PARVARCOPY(OctagonVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T x2 = SQR(helper.In.x);
		T y2 = SQR(helper.In.y);
		T z2 = SQR(helper.In.z);
		T r = m_Weight / Zeps(SQR(x2) + z2 + SQR(y2) + z2);

		if (r < 2)
		{
			helper.Out.x = r * helper.In.x;
			helper.Out.y = r * helper.In.y;
			helper.Out.z = r * helper.In.z;
		}
		else
		{
			helper.Out.x = m_Weight * helper.In.x;
			helper.Out.y = m_Weight * helper.In.y;
			helper.Out.z = m_Weight * helper.In.z;
			T t = m_Weight / Zeps(std::sqrt(SQR(helper.In.x)) + std::sqrt(helper.In.z) + std::sqrt(SQR(helper.In.y)) + std::sqrt(helper.In.z));

			if (r >= 0)
			{
				helper.Out.x = t * helper.In.x;
				helper.Out.y = t * helper.In.y;
				helper.Out.z = t * helper.In.z;
			}
			else
			{
				helper.Out.x = m_Weight * helper.In.x;
				helper.Out.y = m_Weight * helper.In.y;
				helper.Out.z = m_Weight * helper.In.z;
			}

			if (helper.In.x >= 0)
				helper.Out.x = m_Weight * (helper.In.x + m_X);
			else
				helper.Out.x = m_Weight * (helper.In.x - m_X);

			if (helper.In.y >= 0)
				helper.Out.y = m_Weight * (helper.In.y + m_Y);
			else
				helper.Out.y = m_Weight * (helper.In.y - m_Y);

			if (helper.In.z >= 0)
				helper.Out.z = m_Weight * (helper.In.z + m_Z);
			else
				helper.Out.z = m_Weight * (helper.In.z - m_Z);
		}
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string x = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string y = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string z = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t x2 = SQR(vIn.x);\n"
		   << "\t\treal_t y2 = SQR(vIn.y);\n"
		   << "\t\treal_t z2 = SQR(vIn.z);\n"
		   << "\t\treal_t r = " << weight << " / Zeps(fma(x2, x2, z2) + fma(y2, y2, z2));\n"
		   << "\n"
		   << "\t\tif (r < 2)\n"
		   << "\t\t{\n"
		   << "\t\t	vOut.x = r * vIn.x;\n"
		   << "\t\t	vOut.y = r * vIn.y;\n"
		   << "\t\t	vOut.z = r * vIn.z;\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	vOut.x = " << weight << " * vIn.x;\n"
		   << "\t\t	vOut.y = " << weight << " * vIn.y;\n"
		   << "\t\t	vOut.z = " << weight << " * vIn.z;\n"
		   << "\n"
		   << "\t\t	real_t t = " << weight << " / Zeps(sqrt(SQR(vIn.x)) + sqrt(vIn.z) + sqrt(SQR(vIn.y)) + sqrt(vIn.z));\n"
		   << "\n"
		   << "\t\t	if (r >= 0)\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = t * vIn.x;\n"
		   << "\t\t		vOut.y = t * vIn.y;\n"
		   << "\t\t		vOut.z = t * vIn.z;\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = " << weight << " * vIn.x;\n"
		   << "\t\t		vOut.y = " << weight << " * vIn.y;\n"
		   << "\t\t		vOut.z = " << weight << " * vIn.z;\n"
		   << "\t\t	}\n"
		   << "\n"
		   << "\t\t	if (vIn.x >= 0)\n"
		   << "\t\t		vOut.x = " << weight << " * (vIn.x + " << x << ");\n"
		   << "\t\t	else\n"
		   << "\t\t		vOut.x = " << weight << " * (vIn.x - " << x << ");\n"
		   << "\n"
		   << "\t\t	if (vIn.y >= 0)\n"
		   << "\t\t		vOut.y = " << weight << " * (vIn.y + " << y << ");\n"
		   << "\t\t	else\n"
		   << "\t\t		vOut.y = " << weight << " * (vIn.y - " << y << ");\n"
		   << "\n"
		   << "\t\t	if (vIn.z >= 0)\n"
		   << "\t\t		vOut.z = " << weight << " * (vIn.z + " << z << ");\n"
		   << "\t\t	else\n"
		   << "\t\t		vOut.z = " << weight << " * (vIn.z - " << z << ");\n"
		   << "\t\t}\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps" };
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_X, prefix + "octagon_x"));//Original used a prefix of octa_, which is incompatible with Ember's design.
		m_Params.push_back(ParamWithName<T>(&m_Y, prefix + "octagon_y"));
		m_Params.push_back(ParamWithName<T>(&m_Z, prefix + "octagon_z"));
	}

private:
	T m_X;
	T m_Y;
	T m_Z;
};

/// <summary>
/// trade.
/// </summary>
template <typename T>
class TradeVariation : public ParametricVariation<T>
{
public:
	TradeVariation(T weight = 1.0) : ParametricVariation<T>("trade", eVariationId::VAR_TRADE, weight)
	{
		Init();
	}

	PARVARCOPY(TradeVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T r, temp, c1mx;

		if (helper.In.x > 0)
		{
			c1mx = m_C1 - helper.In.x;
			r = std::sqrt(SQR(c1mx) + SQR(helper.In.y));

			if (r <= m_R1)
			{
				r *= m_R2 / m_R1;
				temp = std::atan2(helper.In.y, c1mx);
				helper.Out.x = m_Weight * (r * std::cos(temp) - m_C2);
				helper.Out.y = m_Weight *  r * std::sin(temp);
			}
			else
			{
				helper.Out.x = m_Weight * helper.In.x;
				helper.Out.y = m_Weight * helper.In.y;
			}
		}
		else
		{
			c1mx = -m_C2 - helper.In.x;
			r = std::sqrt(SQR(c1mx) + SQR(helper.In.y));

			if (r <= m_R2)
			{
				r *= m_R1 / m_R2;
				temp = std::atan2(helper.In.y, c1mx);
				helper.Out.x = m_Weight * (r * std::cos(temp) + m_C1);
				helper.Out.y = m_Weight *  r * std::sin(temp);
			}
			else
			{
				helper.Out.x = m_Weight * helper.In.x;
				helper.Out.y = m_Weight * helper.In.y;
			}
		}

		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string r1 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string d1 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string r2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string d2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string c1 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string c2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t r, temp, c1mx;\n"
		   << "\n"
		   << "\t\tif (vIn.x > 0)\n"
		   << "\t\t{\n"
		   << "\t\t	c1mx = " << c1 << " - vIn.x;\n"
		   << "\t\t	r = sqrt(fma(c1mx, c1mx, SQR(vIn.y)));\n"
		   << "\n"
		   << "\t\t	if (r <= " << r1 << ")\n"
		   << "\t\t	{\n"
		   << "\t\t		r *= " << r2 << " / " << r1 << ";\n"
		   << "\t\t		temp = atan2(vIn.y, c1mx);\n"
		   << "\n"
		   << "\t\t		vOut.x = " << weight << " * fma(r, cos(temp), -" << c2 << ");\n"
		   << "\t\t		vOut.y = " << weight << " *  r * sin(temp);\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = " << weight << " * vIn.x;\n"
		   << "\t\t		vOut.y = " << weight << " * vIn.y;\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	c1mx = -" << c2 << " - vIn.x;\n"
		   << "\t\t	r = sqrt(fma(c1mx, c1mx, SQR(vIn.y)));\n"
		   << "\n"
		   << "\t\t	if (r <= " << r2 << ")\n"
		   << "\t\t	{\n"
		   << "\t\t		r *= " << r1 << " / " << r2 << ";\n"
		   << "\t\t		temp = atan2(vIn.y, c1mx);\n"
		   << "\n"
		   << "\t\t		vOut.x = " << weight << " * fma(r, cos(temp), " << c1 << ");\n"
		   << "\t\t		vOut.y = " << weight << " *  r * sin(temp);\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = " << weight << " * vIn.x;\n"
		   << "\t\t		vOut.y = " << weight << " * vIn.y;\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_C1 = m_R1 + m_D1;
		m_C2 = m_R2 + m_D2;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_R1, prefix + "trade_r1", 1, eParamType::REAL, EPS, TMAX));
		m_Params.push_back(ParamWithName<T>(&m_D1, prefix + "trade_d1", 1, eParamType::REAL, 0, TMAX));
		m_Params.push_back(ParamWithName<T>(&m_R2, prefix + "trade_r2", 1, eParamType::REAL, EPS, TMAX));
		m_Params.push_back(ParamWithName<T>(&m_D2, prefix + "trade_d2", 1, eParamType::REAL, 0, TMAX));
		m_Params.push_back(ParamWithName<T>(true, &m_C1, prefix + "trade_c1"));
		m_Params.push_back(ParamWithName<T>(true, &m_C2, prefix + "trade_c2"));
	}

private:
	T m_R1;
	T m_D1;
	T m_R2;
	T m_D2;
	T m_C1;//Precalc.
	T m_C2;
};

/// <summary>
/// Juliac.
/// </summary>
template <typename T>
class JuliacVariation : public ParametricVariation<T>
{
public:
	JuliacVariation(T weight = 1.0) : ParametricVariation<T>("Juliac", eVariationId::VAR_JULIAC, weight, true, false, false, false, true)
	{
		Init();
	}

	PARVARCOPY(JuliacVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T arg = helper.m_PrecalcAtanyx + fmod(T(rand.Rand()), T(1 / m_ReInv)) * M_2PI;
		T lnmod = m_Dist * T(0.5) * std::log(helper.m_PrecalcSumSquares);
		T temp = arg * m_ReInv + lnmod * m_Im100;
		T mod2 = std::exp(lnmod * m_ReInv - arg * m_Im100);
		helper.Out.x = m_Weight * mod2 * std::cos(temp);
		helper.Out.y = m_Weight * mod2 * std::sin(temp);
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string re    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string im    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string dist  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string reInv = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string im100 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t arg = fma(fmod((real_t)MwcNext(mwc), (real_t)((real_t)(1.0) / " << reInv << ")), M_2PI, precalcAtanyx);\n"
		   << "\t\treal_t lnmod = " << dist << " * (real_t)(0.5) * log(precalcSumSquares);\n"
		   << "\t\treal_t temp = fma(arg, " << reInv << ", lnmod * " << im100 << ");\n"
		   << "\t\treal_t mod2 = exp(fma(lnmod, " << reInv << ", -(arg * " << im100 << ")));\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * mod2 * cos(temp);\n"
		   << "\t\tvOut.y = " << weight << " * mod2 * sin(temp);\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_ReInv = 1 / Zeps(m_Re);
		m_Im100 = m_Im * T(0.01);
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Re, prefix + "Juliac_re", 2));
		m_Params.push_back(ParamWithName<T>(&m_Im, prefix + "Juliac_im", 1));
		m_Params.push_back(ParamWithName<T>(&m_Dist, prefix + "Juliac_dist", 1));
		m_Params.push_back(ParamWithName<T>(true, &m_ReInv, prefix + "Juliac_re_inv"));
		m_Params.push_back(ParamWithName<T>(true, &m_Im100, prefix + "Juliac_im100"));
	}

private:
	T m_Re;
	T m_Im;
	T m_Dist;
	T m_ReInv;
	T m_Im100;
};

/// <summary>
/// blade3D.
/// </summary>
template <typename T>
class Blade3DVariation : public Variation<T>
{
public:
	Blade3DVariation(T weight = 1.0) : Variation<T>("blade3D", eVariationId::VAR_BLADE3D, weight, true, true) { }

	VARCOPY(Blade3DVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T r = rand.Frand01<T>() * m_Weight * helper.m_PrecalcSqrtSumSquares;
		T sinr, cosr;
		sincos(r, &sinr, &cosr);
		helper.Out.x = m_Weight * helper.In.x * (cosr + sinr);
		helper.Out.y = m_Weight * helper.In.x * (cosr - sinr);
		helper.Out.z = m_Weight * helper.In.z * (sinr - cosr);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\t\treal_t r = MwcNext01(mwc) * " << weight << " * precalcSqrtSumSquares;\n"
		   << "\t\treal_t sinr = sin(r);\n"
		   << "\t\treal_t cosr = cos(r);\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * vIn.x * (cosr + sinr);\n"
		   << "\t\tvOut.y = " << weight << " * vIn.x * (cosr - sinr);\n"
		   << "\t\tvOut.z = " << weight << " * vIn.z * (sinr - cosr);\n"
		   << "\t}\n";
		return ss.str();
	}
};

/// <summary>
/// Blob3D.
/// </summary>
template <typename T>
class Blob3DVariation : public ParametricVariation<T>
{
public:
	Blob3DVariation(T weight = 1.0) : ParametricVariation<T>("blob3D", eVariationId::VAR_BLOB3D, weight, true, true, true, true)
	{
		Init();
	}

	PARVARCOPY(Blob3DVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T r = helper.m_PrecalcSqrtSumSquares * (m_BlobLow + m_BlobDiff * (T(0.5) + T(0.5) * std::sin(m_BlobWaves * helper.m_PrecalcAtanxy)));
		helper.Out.x = m_Weight * helper.m_PrecalcSina * r;
		helper.Out.y = m_Weight * helper.m_PrecalcCosa * r;
		helper.Out.z = m_Weight * std::sin(m_BlobWaves * helper.m_PrecalcAtanxy) * r;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string blobLow   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string blobHigh  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string blobWaves = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string blobDiff  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t r = precalcSqrtSumSquares * fma(" << blobDiff << ", fma((real_t)(0.5), sin(" << blobWaves << " * precalcAtanxy), (real_t)(0.5)), " << blobLow << ");\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * (precalcSina * r);\n"
		   << "\t\tvOut.y = " << weight << " * (precalcCosa * r);\n"
		   << "\t\tvOut.z = " << weight << " * (sin(" << blobWaves << " * precalcAtanxy) * r);\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_BlobDiff = m_BlobHigh - m_BlobLow;
	}

	virtual void Random(QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		m_BlobLow = T(0.2) + T(0.5) * rand.Frand01<T>();
		m_BlobHigh = T(0.8) + T(0.4) * rand.Frand01<T>();
		m_BlobWaves = T(int(2 + 5 * rand.Frand01<T>()));
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_BlobLow, prefix + "blob3D_low"));
		m_Params.push_back(ParamWithName<T>(&m_BlobHigh, prefix + "blob3D_high", 1));
		m_Params.push_back(ParamWithName<T>(&m_BlobWaves, prefix + "blob3D_waves", 1));
		m_Params.push_back(ParamWithName<T>(true, &m_BlobDiff, prefix + "blob3D_diff"));//Precalc.
	}

private:
	T m_BlobLow;
	T m_BlobHigh;
	T m_BlobWaves;
	T m_BlobDiff;//Precalc.
};

/// <summary>
/// blocky.
/// </summary>
template <typename T>
class BlockyVariation : public ParametricVariation<T>
{
public:
	BlockyVariation(T weight = 1.0) : ParametricVariation<T>("blocky", eVariationId::VAR_BLOCKY, weight, true)
	{
		Init();
	}

	PARVARCOPY(BlockyVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T t = Zeps((std::cos(helper.In.x) + std::cos(helper.In.y)) / m_Mp + 1);
		T r = m_Weight / t;
		T tmp = helper.m_PrecalcSumSquares + 1;
		T x2 = 2 * helper.In.x;
		T y2 = 2 * helper.In.y;
		T xmax = T(0.5) * (std::sqrt(tmp + x2) + std::sqrt(tmp - x2));
		T ymax = T(0.5) * (std::sqrt(tmp + y2) + std::sqrt(tmp - y2));
		T a = helper.In.x / Zeps(xmax);
		T b = VarFuncs<T>::SafeSqrt(1 - SQR(a));
		helper.Out.x = m_Vx * std::atan2(a, b) * r;
		a = helper.In.y / Zeps(ymax);
		b = VarFuncs<T>::SafeSqrt(1 - SQR(a));
		helper.Out.y = m_Vy * std::atan2(a, b) * r;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string x = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string y = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string mp = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string v = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string vx = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string vy = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t t = Zeps((cos(vIn.x) + cos(vIn.y)) / " << mp << " + 1);\n"
		   << "\t\treal_t r = " << weight << " / t;\n"
		   << "\t\treal_t tmp = precalcSumSquares + 1;\n"
		   << "\t\treal_t x2 = 2 * vIn.x;\n"
		   << "\t\treal_t y2 = 2 * vIn.y;\n"
		   << "\t\treal_t xmax = (real_t)(0.5) * (sqrt(tmp + x2) + sqrt(tmp - x2));\n"
		   << "\t\treal_t ymax = (real_t)(0.5) * (sqrt(tmp + y2) + sqrt(tmp - y2));\n"
		   << "\t\treal_t a = vIn.x / Zeps(xmax);\n"
		   << "\t\treal_t b = SafeSqrt(1 - SQR(a));\n"
		   << "\n"
		   << "\t\tvOut.x = " << vx << " * atan2(a, b) * r;\n"
		   << "\n"
		   << "\t\ta = vIn.y / Zeps(ymax);\n"
		   << "\t\tb = SafeSqrt(1 - SQR(a));\n"
		   << "\n"
		   << "\t\tvOut.y = " << vy << " * atan2(a, b) * r;\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "SafeSqrt", "Zeps" };
	}

	virtual void Precalc() override
	{
		m_V = m_Weight / T(M_PI_2);
		m_Vx = m_V * m_X;
		m_Vy = m_V * m_Y;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_X, prefix + "blocky_x", 1));
		m_Params.push_back(ParamWithName<T>(&m_Y, prefix + "blocky_y", 1));
		m_Params.push_back(ParamWithName<T>(&m_Mp, prefix + "blocky_mp", 4, eParamType::REAL_NONZERO));
		m_Params.push_back(ParamWithName<T>(true, &m_V, prefix + "blocky_v"));//Precalc.
		m_Params.push_back(ParamWithName<T>(true, &m_Vx, prefix + "blocky_vx"));
		m_Params.push_back(ParamWithName<T>(true, &m_Vy, prefix + "blocky_vy"));
	}

private:
	T m_X;
	T m_Y;
	T m_Mp;
	T m_V;//Precalc.
	T m_Vx;
	T m_Vy;
};

/// <summary>
/// block.
/// By TyrantWave.
/// </summary>
template <typename T>
class BlockVariation : public ParametricVariation<T>
{
public:
	BlockVariation(T weight = 1.0) : ParametricVariation<T>("block", eVariationId::VAR_BLOCK, weight, true)
	{
		Init();
	}

	PARVARCOPY(BlockVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T tmp = helper.m_PrecalcSumSquares + 1;
		T x2 = 2 * helper.In.x;
		T y2 = 2 * helper.In.y;
		T xmax = T(0.5) * (std::sqrt(tmp + x2) + std::sqrt(tmp - x2));
		T ymax = T(0.5) * (std::sqrt(tmp + y2) + std::sqrt(tmp - y2));
		T a = helper.In.x / Zeps(xmax);
		T b = VarFuncs<T>::SafeSqrt(1 - SQR(a));
		helper.Out.x = m_WightDivPiOver2 * std::atan2(a, b);
		a = helper.In.y / Zeps(ymax);
		b = VarFuncs<T>::SafeSqrt(1 - SQR(a));
		helper.Out.y = m_WightDivPiOver2 * std::atan2(a, b);
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		int i = 0;
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string wdpio2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;//Precalcs only, no params.
		ss << "\t{\n"
		   << "\t\treal_t tmp = precalcSumSquares + 1;\n"
		   << "\t\treal_t x2 = 2 * vIn.x;\n"
		   << "\t\treal_t y2 = 2 * vIn.y;\n"
		   << "\t\treal_t xmax = 0.5 * (sqrt(tmp + x2) + sqrt(tmp - x2));\n"
		   << "\t\treal_t ymax = 0.5 * (sqrt(tmp + y2) + sqrt(tmp - y2));\n"
		   << "\t\treal_t a = vIn.x / Zeps(xmax);\n"
		   << "\t\treal_t b = SafeSqrt(1 - SQR(a));\n"
		   << "\t\tvOut.x = " << wdpio2 << " * atan2(a, b);\n"
		   << "\t\ta = vIn.y / Zeps(ymax);\n"
		   << "\t\tb = SafeSqrt(1 - SQR(a));\n"
		   << "\t\tvOut.y = " << wdpio2 << " * atan2(a, b);\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "SafeSqrt", "Zeps" };
	}

	virtual void Precalc() override
	{
		m_WightDivPiOver2 = m_Weight * T(M_2_PI);
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(true, &m_WightDivPiOver2, prefix + "block_weightdivpiover2"));//Precalcs only, no params.
	}

private:
	T m_WightDivPiOver2;
};

MAKEPREPOSTPARVAR(ESwirl, eSwirl, ESWIRL)
MAKEPREPOSTPARVAR(LazyJess, lazyjess, LAZYJESS)
MAKEPREPOSTPARVAR(LazyTravis, lazyTravis, LAZY_TRAVIS)
MAKEPREPOSTPARVAR(Squish, squish, SQUISH)
MAKEPREPOSTPARVAR(Circus, circus, CIRCUS)
MAKEPREPOSTVAR(Tancos, tancos, TANCOS)
MAKEPREPOSTVAR(Rippled, rippled, RIPPLED)
MAKEPREPOSTPARVAR(RotateX, rotate_x, ROTATE_X)
MAKEPREPOSTPARVAR(RotateY, rotate_y, ROTATE_Y)
MAKEPREPOSTPARVAR(RotateZ, rotate_z, ROTATE_Z)
MAKEPREPOSTVAR(MirrorX, mirror_x, MIRROR_X)
MAKEPREPOSTVAR(MirrorY, mirror_y, MIRROR_Y)
MAKEPREPOSTVAR(MirrorZ, mirror_z, MIRROR_Z)
MAKEPREPOSTPARVAR(RBlur, rblur, RBLUR)
MAKEPREPOSTPARVAR(JuliaNab, juliaNab, JULIANAB)
MAKEPREPOSTPARVAR(Sintrange, sintrange, SINTRANGE)
MAKEPREPOSTPARVAR(Voron, Voron, VORON)
MAKEPREPOSTPARVARASSIGN(Waffle, waffle, WAFFLE, eVariationAssignType::ASSIGNTYPE_SUM)
MAKEPREPOSTVARASSIGN(Square3D, square3D, SQUARE3D, eVariationAssignType::ASSIGNTYPE_SUM)
MAKEPREPOSTPARVARASSIGN(SuperShape3D, SuperShape3D, SUPER_SHAPE3D, eVariationAssignType::ASSIGNTYPE_SUM)
MAKEPREPOSTPARVAR(Sphyp3D, sphyp3D, SPHYP3D)
MAKEPREPOSTPARVAR(Circlecrop, circlecrop, CIRCLECROP)
MAKEPREPOSTPARVAR(Circlecrop2, circlecrop2, CIRCLECROP2)
MAKEPREPOSTPARVAR(Julian3Dx, julian3Dx, JULIAN3DX)
MAKEPREPOSTPARVAR(Fourth, fourth, FOURTH)
MAKEPREPOSTPARVAR(Mobiq, mobiq, MOBIQ)
MAKEPREPOSTPARVAR(Spherivoid, spherivoid, SPHERIVOID)
MAKEPREPOSTPARVAR(Farblur, farblur, FARBLUR)
MAKEPREPOSTPARVAR(CurlSP, curl_sp, CURL_SP)
MAKEPREPOSTPARVAR(Heat, heat, HEAT)
MAKEPREPOSTPARVAR(Interference2, interference2, INTERFERENCE2)
MAKEPREPOSTVAR(Sinq, sinq, SINQ)
MAKEPREPOSTVAR(Sinhq, sinhq, SINHQ)
MAKEPREPOSTVAR(Secq, secq, SECQ)
MAKEPREPOSTVAR(Sechq, sechq, SECHQ)
MAKEPREPOSTVAR(Tanq, tanq, TANQ)
MAKEPREPOSTVAR(Tanhq, tanhq, TANHQ)
MAKEPREPOSTVAR(Cosq, cosq, COSQ)
MAKEPREPOSTVAR(Coshq, coshq, COSHQ)
MAKEPREPOSTVAR(Cotq, cotq, COTQ)
MAKEPREPOSTVAR(Cothq, cothq, COTHQ)
MAKEPREPOSTVAR(Cscq, cscq, CSCQ)
MAKEPREPOSTVAR(Cschq, cschq, CSCHQ)
MAKEPREPOSTVAR(Estiq, estiq, ESTIQ)
MAKEPREPOSTPARVAR(Loq, loq, LOQ)
MAKEPREPOSTVAR(Curvature, curvature, CURVATURE)
MAKEPREPOSTPARVAR(Qode, q_ode, Q_ODE)
MAKEPREPOSTPARVARASSIGN(BlurHeart, blur_heart, BLUR_HEART, eVariationAssignType::ASSIGNTYPE_SUM)
MAKEPREPOSTPARVAR(Truchet, Truchet, TRUCHET)
MAKEPREPOSTVAR(TruchetKnot, truchet_knot, TRUCHET_KNOT)
MAKEPREPOSTPARVAR(Gdoffs, gdoffs, GDOFFS)
MAKEPREPOSTPARVAR(Octagon, octagon, OCTAGON)
MAKEPREPOSTPARVAR(Trade, trade, TRADE)
MAKEPREPOSTPARVAR(Juliac, Juliac, JULIAC)
MAKEPREPOSTVAR(Blade3D, blade3D, BLADE3D)
MAKEPREPOSTPARVAR(Blob3D, blob3D, BLOB3D)
MAKEPREPOSTPARVAR(Blocky, blocky, BLOCKY)
MAKEPREPOSTPARVAR(Block, block, BLOCK)


///// <summary>
///// LinearXZ.
///// </summary>
//template <typename T>
//class LinearXZVariation : public Variation<T>
//{
//public:
//	LinearXZVariation(T weight = 1.0) : Variation<T>("linearxz", eVariationId::VAR_LINEAR_XZ, weight) { }
//
//	VARCOPY(LinearXZVariation)
//
//	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
//	{
//		helper.Out.x = m_Weight * helper.In.x;
//		helper.Out.z = m_Weight * helper.In.z;
//	}
//
//	virtual string OpenCLString() const override
//	{
//		ostringstream ss;
//		intmax_t varIndex = IndexInXform();
// string weight = WeightDefineString();
//
//		ss << "\t{\n"
//		   << "\t\tvOut.x = " << weight << " * vIn.x;\n"
//		   << "\t\tvOut.z = " << weight << " * vIn.z;\n"
//		   << "\t}\n";
//
//		return ss.str();
//	}
//};
//
///// <summary>
///// LinearYZ.
///// </summary>
//template <typename T>
//class LinearYZVariation : public Variation<T>
//{
//public:
//	LinearYZVariation(T weight = 1.0) : Variation<T>("linearyz", eVariationId::VAR_LINEAR_YZ, weight) { }
//
//	VARCOPY(LinearYZVariation)
//
//	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
//	{
//		helper.Out.y = m_Weight * helper.In.y;
//		helper.Out.z = m_Weight * helper.In.z;
//	}
//
//	virtual string OpenCLString() const override
//	{
//		ostringstream ss;
//		intmax_t varIndex = IndexInXform();
// string weight = WeightDefineString();
//
//		ss << "\t{\n"
//		   << "\t\tvOut.y = " << weight << " * vIn.y;\n"
//		   << "\t\tvOut.z = " << weight << " * vIn.z;\n"
//		   << "\t}\n";
//
//		return ss.str();
//	}
//};
}
