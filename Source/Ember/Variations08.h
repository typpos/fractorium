#pragma once

#include "Variation.h"

namespace EmberNs
{
/// <summary>
/// Gnarly.
/// </summary>
template <typename T>
class GnarlyVariation : public ParametricVariation<T>
{
public:
	GnarlyVariation(T weight = 1.0) : ParametricVariation<T>("gnarly", eVariationId::VAR_GNARLY, weight)
	{
		Init();
	}

	PARVARCOPY(GnarlyVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T Vx, Vy;
		T Cx, Cy;
		T Lx, Ly;
		T r, theta, s, c;
		Vx = helper.In.x;
		Vy = helper.In.y;

		if (m_GnarlyCellSize != T(0))
		{
			Cx = (Floor<T>(Vx / m_GnarlyCellSize) + T(0.5)) * m_GnarlyCellSize;
			Cy = (Floor<T>(Vy / m_GnarlyCellSize) + T(0.5)) * m_GnarlyCellSize;
			Lx = Vx - Cx;
			Ly = Vy - Cy;

			if ((Lx * Lx + Ly * Ly) <= m_R2)
			{
				r = (Lx * Lx + Ly * Ly) / m_R2;
				theta = m_GnarlyTwist * std::log(r);
				sincos(theta, &s, &c);
				Vx = Cx + c * Lx + s * Ly;
				Vy = Cy - s * Lx + c * Ly;
			}
		}

		helper.Out.x = m_Weight * Vx;
		helper.Out.y = m_Weight * Vy;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0;
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string cellsize = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string twist = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string r2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t Vx, Vy, Cx, Cy, Lx, Ly, Lxy;\n"
		   << "\t\treal_t r, theta, s, c;\n"
		   << "\n"
		   << "\t\tVx = vIn.x;\n"
		   << "\t\tVy = vIn.y;\n"
		   << "\n"
		   << "\t\tif (" << cellsize << " != (real_t)(0))\n"
		   << "\t\t{\n"
		   << "\t\t\tCx = (floor(Vx / " << cellsize << ") + (real_t)(0.5)) * " << cellsize << ";\n"
		   << "\t\t\tCy = (floor(Vy / " << cellsize << ") + (real_t)(0.5)) * " << cellsize << ";\n"
		   << "\n"
		   << "\t\t\tLx = Vx - Cx;\n"
		   << "\t\t\tLy = Vy - Cy;\n"
		   << "\t\t\tLxy = fma(Lx, Lx, Ly * Ly);\n"
		   << "\n"
		   << "\t\t\tif (Lxy <= " << r2 << ")\n"
		   << "\t\t\t{\n"
		   << "\t\t\t\tr = Lxy / " << r2 << ";\n"
		   << "\t\t\t\ttheta = " << twist << " * log(r);\n"
		   << "\t\t\t\ts = sin(theta);\n"
		   << "\t\t\t\tc = cos(theta);\n"
		   << "\t\t\t\tVx = Cx + c * Lx + s * Ly;\n"
		   << "\t\t\t\tVy = Cy - s * Lx + c * Ly;\n"
		   << "\t\t\t}\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * Vx;\n"
		   << "\t\tvOut.y = " << weight << " * Vy;\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		T radius = T(0.5) * m_GnarlyCellSize;
		m_R2 = Zeps(SQR(radius));
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_GnarlyCellSize, prefix + "gnarly_cellsize", 1));
		m_Params.push_back(ParamWithName<T>(&m_GnarlyTwist, prefix + "gnarly_twist", 1));
		m_Params.push_back(ParamWithName<T>(true, &m_R2, prefix + "gnarly_r2"));//Precalc.
	}

private:
	T m_GnarlyCellSize;
	T m_GnarlyTwist;
	T m_R2;//Precalc.
};

/// <summary>
/// inkdrop by Jess.
/// </summary>
template <typename T>
class InkdropVariation : public ParametricVariation<T>
{
public:
	InkdropVariation(T weight = 1.0) : ParametricVariation<T>("inkdrop", eVariationId::VAR_INKDROP, weight)
	{
		Init();
	}

	PARVARCOPY(InkdropVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T distx = helper.In.x - m_X;
		T disty = helper.In.y - m_Y;
		T dist2 = SQR(distx) + SQR(disty);
		T adjust = std::sqrt(dist2 + m_Rad2) - std::sqrt(dist2);
		T bearing = std::atan2(disty, distx);
		T x = helper.In.x + (std::cos(bearing) * adjust);
		T y = helper.In.y + (std::sin(bearing) * adjust);
		helper.Out.x = m_Weight * x;
		helper.Out.y = m_Weight * y;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0;
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string r = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string x = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string y = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string rad2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t distx = vIn.x - " << x << ";\n"
		   << "\t\treal_t disty = vIn.y - " << y << ";\n"
		   << "\t\treal_t dist2 = SQR(distx) + SQR(disty);\n"
		   << "\t\treal_t adjust = sqrt(dist2 + " << rad2 << ") - sqrt(dist2);\n"
		   << "\n"
		   << "\t\treal_t bearing = atan2(disty, distx);\n"
		   << "\t\treal_t x = fma(cos(bearing), adjust, vIn.x);\n"
		   << "\t\treal_t y = fma(sin(bearing), adjust, vIn.y);\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * x;\n"
		   << "\t\tvOut.y = " << weight << " * y;\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_Rad2 = SQR(m_R);
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_R, prefix + "inkdrop_r", T(0.5), eParamType::REAL, 0));
		m_Params.push_back(ParamWithName<T>(&m_X, prefix + "inkdrop_x"));
		m_Params.push_back(ParamWithName<T>(&m_Y, prefix + "inkdrop_y"));
		m_Params.push_back(ParamWithName<T>(true, &m_Rad2, prefix + "inkdrop_rad2"));//Precalc.
	}

private:
	T m_R;
	T m_X;
	T m_Y;
	T m_Rad2;//Precalc.
};

/// <summary>
/// hex_modulus.
/// By tatasz.
/// </summary>
template <typename T>
class HexModulusVariation : public ParametricVariation<T>
{
public:
	HexModulusVariation(T weight = 1.0) : ParametricVariation<T>("hex_modulus", eVariationId::VAR_HEX_MODULUS, weight)
	{
		Init();
	}

	PARVARCOPY(HexModulusVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		//get hex
		T X = helper.In.x * m_HsizePrecalc;
		T Y = helper.In.y * m_HsizePrecalc;
		T yover3 = Y / 3;
		T x = M_SQRT3_3 * X - yover3;
		T z = T(2.0) * yover3;
		T y = -x - z;
		//round
		T rx = std::round(x);
		T ry = std::round(y);
		T rz = std::round(z);
		T x_diff = std::abs(rx - x);
		T y_diff = std::abs(ry - y);
		T z_diff = std::abs(rz - z);

		if ((x_diff > y_diff) & (x_diff > z_diff))
			rx = -ry - rz;
		else if (y_diff > z_diff)
			ry = -rx - rz;
		else
			rz = -rx - ry;

		T FX_h = M_SQRT3 * rx + M_SQRT3_2 * rz;
		T FY_h = T(1.5) * rz;
		T FX = X - FX_h;
		T FY = Y - FY_h;
		helper.Out.x = FX * m_WeightPrecalc;
		helper.Out.y = FY * m_WeightPrecalc;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0;
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string size          = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string hsizeprecalc  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string weightprecalc = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\t//get hex\n"
		   << "\t\treal_t X = vIn.x * " << hsizeprecalc << ";\n"
		   << "\t\treal_t Y = vIn.y * " << hsizeprecalc << ";\n"
		   << "\t\treal_t yover3 = Y / (real_t)(3.0);\n"
		   << "\t\treal_t x = fma(M_SQRT3_3, X, -yover3);\n"
		   << "\t\treal_t z = (real_t)(2.0) * yover3;\n"
		   << "\t\treal_t y = -x - z;\n"
		   << "\t\t//round\n"
		   << "\t\treal_t rx = round(x);\n"
		   << "\t\treal_t ry = round(y);\n"
		   << "\t\treal_t rz = round(z);\n"
		   << "\n"
		   << "\t\treal_t x_diff = fabs(rx - x);\n"
		   << "\t\treal_t y_diff = fabs(ry - y);\n"
		   << "\t\treal_t z_diff = fabs(rz - z);\n"
		   << "\n"
		   << "\t\tif ((x_diff > y_diff) & (x_diff > z_diff))\n"
		   << "\t\trx = -ry - rz;\n"
		   << "\t\telse if (y_diff > z_diff)\n"
		   << "\t\try = -rx - rz;\n"
		   << "\t\telse\n"
		   << "\t\trz = -rx - ry;\n"
		   << "\n"
		   << "\t\treal_t FX_h = fma(M_SQRT3, rx, M_SQRT3_2 * rz);\n"
		   << "\t\treal_t FY_h = (real_t)(1.5) * rz;\n"
		   << "\n"
		   << "\t\treal_t FX = X - FX_h;\n"
		   << "\t\treal_t FY = Y - FY_h;\n"
		   << "\n"
		   << "\t\tvOut.x = FX * " << weightprecalc << ";\n"
		   << "\t\tvOut.y = FY * " << weightprecalc << ";\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_HsizePrecalc = M_SQRT3_2 / Zeps(m_Size);
		m_WeightPrecalc = m_Weight / M_SQRT3_2;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Size,                prefix + "hex_modulus_size", 1));
		m_Params.push_back(ParamWithName<T>(true, &m_HsizePrecalc,  prefix + "hex_modulus_hsize_precalc"));//Precalc.
		m_Params.push_back(ParamWithName<T>(true, &m_WeightPrecalc, prefix + "hex_modulus_weight_precalc"));
	}

private:
	T m_Size;
	T m_HsizePrecalc;//Precalc.
	T m_WeightPrecalc;
};

/// <summary>
/// Circle_Rand.
/// A new version of CircleRand from Chaotica by tatasz.
/// </summary>
template <typename T>
class Circle_RandVariation : public ParametricVariation<T>
{
public:
	Circle_RandVariation(T weight = 1.0) : ParametricVariation<T>("Circle_Rand", eVariationId::VAR_CIRCLE_RAND, weight)
	{
		Init();
	}

	PARVARCOPY(Circle_RandVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T a = rand.Frand01<T>() * M_2PI;
		T r = std::sqrt(rand.Frand01<T>());
		T x = 2 * (Floor(rand.Frand01<T>() * m_SideX * 2 + T(0.5)) - m_SideX);
		T y = 2 * (Floor(rand.Frand01<T>() * m_SideY * 2 + T(0.5)) - m_SideY);
		T ret;

		if (VarFuncs<T>::HashShadertoy(y, x, m_Seed) < m_Dens)
			ret = m_Size * r * std::exp(std::log(VarFuncs<T>::HashShadertoy(x, y, m_Seed)) * m_Power);
		else
			ret = 0;

		if (ret > 0)
		{
			helper.Out.x = m_Weight * (std::cos(a) * ret + x);
			helper.Out.y = m_Weight * (std::sin(a) * ret + y);
		}

		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0;
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string x = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string y = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string size = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string power = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string density = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string seed = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string sidex = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string sidey = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t seed = " << seed << ";\n"
		   << "\t\treal_t sidex = " << sidex << ";\n"
		   << "\t\treal_t sidey = " << sidey << ";\n"
		   << "\t\treal_t a = MwcNext01(mwc) * M_2PI;\n"
		   << "\t\treal_t r = sqrt(MwcNext01(mwc));\n"
		   << "\t\treal_t x = 2 * (floor(MwcNext01(mwc) * sidex * 2 + (real_t)(0.5)) - sidex);\n"
		   << "\t\treal_t y = 2 * (floor(MwcNext01(mwc) * sidey * 2 + (real_t)(0.5)) - sidey);\n"
		   << "\n"
		   << "\t\treal_t ret;\n"
		   << "\n"
		   << "\t\tif (HashShadertoy(y, x, seed) < " << density << ")\n"
		   << "\t\t	ret = " << size << " * r * exp(log(HashShadertoy(x, y, seed)) * " << power << ");\n"
		   << "\t\telse"
		   << "\t\t	ret = (real_t)(0.0);"
		   << "\n"
		   << "\t\tif (ret > (real_t)(0.0))\n"
		   << "\t\t{"
		   << "\t\t	vOut.x = " << weight << " * fma((real_t)(cos(a)), ret, x);\n"
		   << "\t\t	vOut.y = " << weight << " * fma((real_t)(sin(a)), ret, y);\n"
		   << "\t\t}"
		   << "\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Fract", "HashShadertoy" };
	}

	virtual void Precalc() override
	{
		m_SideX = m_X * T(0.5);
		m_SideY = m_Y * T(0.5);
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_X, prefix + "Circle_Rand_X", 10));
		m_Params.push_back(ParamWithName<T>(&m_Y, prefix + "Circle_Rand_Y", 10));
		m_Params.push_back(ParamWithName<T>(&m_Size, prefix + "Circle_Rand_size", 1, eParamType::REAL, 0));
		m_Params.push_back(ParamWithName<T>(&m_Power, prefix + "Circle_Rand_power", 1));
		m_Params.push_back(ParamWithName<T>(&m_Dens, prefix + "Circle_Rand_density", 1, eParamType::REAL, 0, 1));
		m_Params.push_back(ParamWithName<T>(&m_Seed, prefix + "Circle_Rand_seed", 1, eParamType::REAL));
		m_Params.push_back(ParamWithName<T>(true, &m_SideX, prefix + "Circle_Rand_side_x"));//Precalc.
		m_Params.push_back(ParamWithName<T>(true, &m_SideY, prefix + "Circle_Rand_side_y"));
	}

private:
	T m_X;
	T m_Y;
	T m_Size;
	T m_Power;
	T m_Dens;
	T m_Seed;
	T m_SideX;//Precalc.
	T m_SideY;
};

MAKEPREPOSTPARVAR(Gnarly, gnarly, GNARLY)
MAKEPREPOSTPARVAR(Inkdrop, inkdrop, INKDROP)
MAKEPREPOSTPARVAR(HexModulus, hex_modulus, HEX_MODULUS)
MAKEPREPOSTPARVAR(Circle_Rand, circle_rand, CIRCLE_RAND)
}