#pragma once

#include "Variation.h"

namespace EmberNs
{
/// <summary>
/// splits3D.
/// </summary>
template <typename T>
class Splits3DVariation : public ParametricVariation<T>
{
public:
	Splits3DVariation(T weight = 1.0) : ParametricVariation<T>("splits3D", eVariationId::VAR_SPLITS3D, weight)
	{
		Init();
	}

	PARVARCOPY(Splits3DVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
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
		   << "\t\tif (vIn.x >= 0)\n"
		   << "\t\t	vOut.x = " << weight << " * (vIn.x + " << x << ");\n"
		   << "\t\telse\n"
		   << "\t\t	vOut.x = " << weight << " * (vIn.x - " << x << ");\n"
		   << "\n"
		   << "\t\tif (vIn.y >= 0)\n"
		   << "\t\t	vOut.y = " << weight << " * (vIn.y + " << y << ");\n"
		   << "\t\telse\n"
		   << "\t\t	vOut.y = " << weight << " * (vIn.y - " << y << ");\n"
		   << "\n"
		   << "\t\tif (vIn.z >= 0)\n"
		   << "\t\t	vOut.z = " << weight << " * (vIn.z + " << z << ");\n"
		   << "\t\telse\n"
		   << "\t\t	vOut.z = " << weight << " * (vIn.z - " << z << ");\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual void Random(QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		m_X = rand.Frand11<T>();
		m_Y = rand.Frand11<T>();
		m_Z = rand.Frand11<T>();
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_X, prefix + "splits3D_x"));
		m_Params.push_back(ParamWithName<T>(&m_Y, prefix + "splits3D_y"));
		m_Params.push_back(ParamWithName<T>(&m_Z, prefix + "splits3D_z"));
	}

private:
	T m_X;
	T m_Y;
	T m_Z;
};

/// <summary>
/// waves2b.
/// Note that _j1() is not implemented in OpenCL, so that conditional is skipped
/// when running on the GPU. The results might look different.
/// </summary>
template <typename T>
class Waves2BVariation : public ParametricVariation<T>
{
public:
	Waves2BVariation(T weight = 1.0) : ParametricVariation<T>("waves2b", eVariationId::VAR_WAVES2B, weight)
	{
		Init();
	}

	PARVARCOPY(Waves2BVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T CsX = 1;
		T CsY = 1;
		T jcbSn = 0, jcbCn, jcbDn;
		CsX = VarFuncs<T>::SafeDivInv(m_Unity, (m_Unity + Sqr(helper.In.x)));
		CsX = CsX * m_Six + m_Scaleinfx;
		CsY = VarFuncs<T>::SafeDivInv(m_Unity, (m_Unity + Sqr(helper.In.y)));
		CsY = CsY * m_Siy + m_Scaleinfy;

		if (m_Pwx >= 0 && m_Pwx < 1e-4)
		{
			VarFuncs<T>::JacobiElliptic(helper.In.y * m_Freqx, m_Jacok, jcbSn, jcbCn, jcbDn);
			helper.Out.x = m_Weight * (helper.In.x + CsX * jcbSn);
		}
		else if (m_Pwx < 0 && m_Pwx > -1e-4)
#ifdef _WIN32
			helper.Out.x = m_Weight * (helper.In.x + CsX * T(_j1(helper.In.y * m_Freqx)));//This is not implemented in OpenCL.

#else
			helper.Out.x = m_Weight * (helper.In.x + CsX * T(j1(helper.In.y * m_Freqx)));//This is not implemented in OpenCL.
#endif
		else
			helper.Out.x = m_Weight * (helper.In.x + CsX * std::sin(VarFuncs<T>::SignNz(helper.In.y) * std::pow(Zeps(std::abs(helper.In.y)), m_Pwx) * m_Freqx));

		if (m_Pwy >= 0 && m_Pwy < 1e-4)
		{
			VarFuncs<T>::JacobiElliptic(helper.In.x * m_Freqy, m_Jacok, jcbSn, jcbCn, jcbDn);
			helper.Out.y = m_Weight * (helper.In.y + CsY * jcbSn);
		}
		else if (m_Pwy < 0 && m_Pwy > -1e-4)
#ifdef _WIN32
			helper.Out.y = m_Weight * (helper.In.y + CsY * T(_j1(helper.In.x * m_Freqy)));

#else
			helper.Out.y = m_Weight * (helper.In.y + CsY * T(j1(helper.In.x * m_Freqy)));
#endif
		else
			helper.Out.y = m_Weight * (helper.In.y + CsY * std::sin(VarFuncs<T>::SignNz(helper.In.x) * std::pow(Zeps(std::abs(helper.In.x)), m_Pwy) * m_Freqy));

		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string freqx = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string freqy = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string pwx = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string pwy = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scalex = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scaleinfx = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scaley = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scaleinfy = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string unity = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string jacok = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string six = "parVars[" + ToUpper(m_Params[i++].Name()) + index;//Precalc.
		string siy = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t CsX = 1;\n"
		   << "\t\treal_t CsY = 1;\n"
		   << "\t\treal_t jcbSn = 0, jcbCn, jcbDn;\n"
		   << "\t\tCsX = SafeDivInv(" << unity << ", (" << unity << " + Sqr(vIn.x)));\n"
		   << "\t\tCsX = CsX * " << six << " + " << scaleinfx << ";\n"
		   << "\t\tCsY = SafeDivInv(" << unity << ", (" << unity << " + Sqr(vIn.y)));\n"
		   << "\t\tCsY = CsY * " << siy << " + " << scaleinfy << ";\n"
		   << "\n"
		   << "\t\tif (" << pwx << " >= 0 && " << pwx << " < 1e-4)\n"
		   << "\t\t{\n"
		   << "\t\t	JacobiElliptic(vIn.y * " << freqx << ", " << jacok << ", &jcbSn, &jcbCn, &jcbDn);\n"
		   << "\t\t	vOut.x = " << weight << " * (vIn.x + CsX * jcbSn);\n"
		   << "\t\t}\n"
		   //<< "\t\telse if (" << pwx << " < 0 && " << pwx << " > -1e-4)\n"
		   //<< "\t\t	vOut.x = " << weight << " * (vIn.x + CsX * _j1(vIn.y * " << freqx << "));\n"//This is not implemented in OpenCL.
		   << "\t\telse\n"
		   << "\t\t	vOut.x = " << weight << " * (vIn.x + CsX * sin(SignNz(vIn.y) * pow(Zeps(fabs(vIn.y)), " << pwx << ") * " << freqx << "));\n"
		   << "\n"
		   << "\t\tif (" << pwy << " >= 0 && " << pwy << " < 1e-4)\n"
		   << "\t\t{\n"
		   << "\t\t	JacobiElliptic(vIn.x * " << freqy << ", " << jacok << ", &jcbSn, &jcbCn, &jcbDn);\n"
		   << "\t\t	vOut.y = " << weight << " * (vIn.y + CsY * jcbSn);\n"
		   << "\t\t}\n"
		   //<< "\t\telse if (" << pwy << " < 0 && " << pwy << " > -1e-4)\n"
		   //<< "\t\t	vOut.y = " << weight << " * (vIn.y + CsY * _j1(vIn.x * " << freqy << "));\n"//This is not implemented in OpenCL.
		   << "\t\telse\n"
		   << "\t\t	vOut.y = " << weight << " * (vIn.y + CsY * sin(SignNz(vIn.x) * pow(Zeps(fabs(vIn.x)), " << pwy << ") * " << freqy << "));\n"
		   << "\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps", "Sqr", "SignNz", "SafeDivInv", "JacobiElliptic" };
	}

	virtual void Precalc() override
	{
		m_Six = m_Scalex - m_Scaleinfx;
		m_Siy = m_Scaley - m_Scaleinfy;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Freqx, prefix + "waves2b_freqx", 2));
		m_Params.push_back(ParamWithName<T>(&m_Freqy, prefix + "waves2b_freqy", 2));
		m_Params.push_back(ParamWithName<T>(&m_Pwx, prefix + "waves2b_pwx", 1, eParamType::REAL, -10, 10));
		m_Params.push_back(ParamWithName<T>(&m_Pwy, prefix + "waves2b_pwy", 1, eParamType::REAL, -10, 10));
		m_Params.push_back(ParamWithName<T>(&m_Scalex, prefix + "waves2b_scalex", 1));
		m_Params.push_back(ParamWithName<T>(&m_Scaleinfx, prefix + "waves2b_scaleinfx", 1));
		m_Params.push_back(ParamWithName<T>(&m_Scaley, prefix + "waves2b_scaley", 1));
		m_Params.push_back(ParamWithName<T>(&m_Scaleinfy, prefix + "waves2b_scaleinfy", 1));
		m_Params.push_back(ParamWithName<T>(&m_Unity, prefix + "waves2b_unity", 1));
		m_Params.push_back(ParamWithName<T>(&m_Jacok, prefix + "waves2b_jacok", T(0.25), eParamType::REAL, -1, 1));
		m_Params.push_back(ParamWithName<T>(true, &m_Six, prefix + "waves2b_six"));//Precalc.
		m_Params.push_back(ParamWithName<T>(true, &m_Siy, prefix + "waves2b_siy"));
	}

private:
	T m_Freqx;
	T m_Freqy;
	T m_Pwx;
	T m_Pwy;
	T m_Scalex;
	T m_Scaleinfx;
	T m_Scaley;
	T m_Scaleinfy;
	T m_Unity;
	T m_Jacok;
	T m_Six;//Precalc.
	T m_Siy;
};

/// <summary>
/// jac_cn.
/// </summary>
template <typename T>
class JacCnVariation : public ParametricVariation<T>
{
public:
	JacCnVariation(T weight = 1.0) : ParametricVariation<T>("jac_cn", eVariationId::VAR_JAC_CN, weight)
	{
		Init();
	}

	PARVARCOPY(JacCnVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T snx, cnx, dnx;
		T sny, cny, dny;
		T numX, numY, denom;
		VarFuncs<T>::JacobiElliptic(helper.In.x, m_K, snx, cnx, dnx);
		VarFuncs<T>::JacobiElliptic(helper.In.y, 1 - m_K, sny, cny, dny);
		numX = cnx * cny;
		numY = -dnx * snx * dny * sny;
		denom = SQR(snx) * SQR(sny) * m_K + SQR(cny);
		denom = m_Weight / Zeps(denom);
		helper.Out.x = denom * numX;
		helper.Out.y = denom * numY;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string k = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t snx, cnx, dnx;\n"
		   << "\t\treal_t sny, cny, dny;\n"
		   << "\t\treal_t numX, numY, denom;\n"
		   << "\t\tJacobiElliptic(vIn.x, " << k << ", &snx, &cnx, &dnx);\n"
		   << "\t\tJacobiElliptic(vIn.y, 1 - " << k << ", &sny, &cny, &dny);\n"
		   << "\t\tnumX = cnx * cny;\n"
		   << "\t\tnumY = -dnx * snx * dny * sny;\n"
		   << "\t\tdenom = SQR(snx) * SQR(sny) * " << k << " + SQR(cny);\n"
		   << "\t\tdenom = " << weight << " / Zeps(denom);\n"
		   << "\t\tvOut.x = denom * numX;\n"
		   << "\t\tvOut.y = denom * numY;\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps", "JacobiElliptic" };
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_K, prefix + "jac_cn_k", T(0.5), eParamType::REAL, -1, 1));
	}

private:
	T m_K;
};

/// <summary>
/// jac_dn.
/// </summary>
template <typename T>
class JacDnVariation : public ParametricVariation<T>
{
public:
	JacDnVariation(T weight = 1.0) : ParametricVariation<T>("jac_dn", eVariationId::VAR_JAC_DN, weight)
	{
		Init();
	}

	PARVARCOPY(JacDnVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T snx, cnx, dnx;
		T sny, cny, dny;
		T numX, numY, denom;
		VarFuncs<T>::JacobiElliptic(helper.In.x, m_K, snx, cnx, dnx);
		VarFuncs<T>::JacobiElliptic(helper.In.y, 1 - m_K, sny, cny, dny);
		numX = dnx * cny * dny;
		numY = -cnx * snx * sny * m_K;
		denom = SQR(snx) * SQR(sny) * m_K + SQR(cny);
		denom = m_Weight / Zeps(denom);
		helper.Out.x = denom * numX;
		helper.Out.y = denom * numY;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string k = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t snx, cnx, dnx;\n"
		   << "\t\treal_t sny, cny, dny;\n"
		   << "\t\treal_t numX, numY, denom;\n"
		   << "\t\tJacobiElliptic(vIn.x, " << k << ", &snx, &cnx, &dnx);\n"
		   << "\t\tJacobiElliptic(vIn.y, 1 - " << k << ", &sny, &cny, &dny);\n"
		   << "\t\tnumX = dnx * cny * dny;\n"
		   << "\t\tnumY = -cnx * snx * sny * " << k << ";\n"
		   << "\t\tdenom = SQR(snx) * SQR(sny) * " << k << " + SQR(cny);\n"
		   << "\t\tdenom = " << weight << " / Zeps(denom);\n"
		   << "\t\tvOut.x = denom * numX;\n"
		   << "\t\tvOut.y = denom * numY;\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps", "JacobiElliptic" };
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_K, prefix + "jac_dn_k", T(0.5), eParamType::REAL, -1, 1));
	}

private:
	T m_K;
};

/// <summary>
/// jac_sn.
/// </summary>
template <typename T>
class JacSnVariation : public ParametricVariation<T>
{
public:
	JacSnVariation(T weight = 1.0) : ParametricVariation<T>("jac_sn", eVariationId::VAR_JAC_SN, weight)
	{
		Init();
	}

	PARVARCOPY(JacSnVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T snx, cnx, dnx;
		T sny, cny, dny;
		T numX, numY, denom;
		VarFuncs<T>::JacobiElliptic(helper.In.x, m_K, snx, cnx, dnx);
		VarFuncs<T>::JacobiElliptic(helper.In.y, 1 - m_K, sny, cny, dny);
		numX = snx * dny;
		numY = cnx * dnx * cny * sny;
		denom = SQR(snx) * SQR(sny) * m_K + SQR(cny);
		denom = m_Weight / Zeps(denom);
		helper.Out.x = denom * numX;
		helper.Out.y = denom * numY;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string k = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t snx, cnx, dnx;\n"
		   << "\t\treal_t sny, cny, dny;\n"
		   << "\t\treal_t numX, numY, denom;\n"
		   << "\t\tJacobiElliptic(vIn.x, " << k << ", &snx, &cnx, &dnx);\n"
		   << "\t\tJacobiElliptic(vIn.y, 1 - " << k << ", &sny, &cny, &dny);\n"
		   << "\t\tnumX = snx * dny;\n"
		   << "\t\tnumY = cnx * dnx * cny * sny;\n"
		   << "\t\tdenom = SQR(snx) * SQR(sny) * " << k << " + SQR(cny);\n"
		   << "\t\tdenom = " << weight << " / Zeps(denom);\n"
		   << "\t\tvOut.x = denom * numX;\n"
		   << "\t\tvOut.y = denom * numY;\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps", "JacobiElliptic" };
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_K, prefix + "jac_sn_k", T(0.5), eParamType::REAL, -1, 1));
	}

private:
	T m_K;
};

/// <summary>
/// pressure_wave.
/// </summary>
template <typename T>
class PressureWaveVariation : public ParametricVariation<T>
{
public:
	PressureWaveVariation(T weight = 1.0) : ParametricVariation<T>("pressure_wave", eVariationId::VAR_PRESSURE_WAVE, weight)
	{
		Init();
	}

	PARVARCOPY(PressureWaveVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		helper.Out.x = m_Weight * (helper.In.x + (1 / Zeps(m_X * M_2PI)) * std::sin(m_X * M_2PI * helper.In.x));
		helper.Out.y = m_Weight * (helper.In.y + (1 / Zeps(m_Y * M_2PI)) * std::sin(m_Y * M_2PI * helper.In.y));
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
		ss << "\t{\n"
		   << "\t\tvOut.x = " << weight << " * (vIn.x + (1 / Zeps(" << x << " * M_2PI)) * sin(" << x << " * M_2PI * vIn.x));\n"
		   << "\t\tvOut.y = " << weight << " * (vIn.y + (1 / Zeps(" << y << " * M_2PI)) * sin(" << y << " * M_2PI * vIn.y));\n"
		   << "\t\tvOut.z = " << DefaultZCl()
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
		m_Params.push_back(ParamWithName<T>(&m_X, prefix + "pressure_wave_x_freq", 1));
		m_Params.push_back(ParamWithName<T>(&m_Y, prefix + "pressure_wave_y_freq", 1));
	}

private:
	T m_X;
	T m_Y;
};

/// <summary>
/// gamma.
/// </summary>
template <typename T>
class GammaVariation : public Variation<T>
{
public:
	GammaVariation(T weight = 1.0) : Variation<T>("gamma", eVariationId::VAR_GAMMA, weight, true, true, false, false, true)
	{
	}

	VARCOPY(GammaVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		helper.Out.x = m_Weight * std::lgamma(helper.m_PrecalcSqrtSumSquares);
		helper.Out.y = m_Weight * helper.m_PrecalcAtanyx;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		ss << "\t{\n"
		   << "\t\tvOut.x = " << weight << " * lgamma(precalcSqrtSumSquares);\n"
		   << "\t\tvOut.y = " << weight << " * precalcAtanyx;\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}
};

/// <summary>
/// prose3D.
/// </summary>
template <typename T>
class PRose3DVariation : public ParametricVariation<T>
{
public:
	PRose3DVariation(T weight = 1.0) : ParametricVariation<T>("pRose3D", eVariationId::VAR_PROSE3D, weight, true, true, false, false, true)
	{
		Init();
	}

	PARVARCOPY(PRose3DVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		int posNeg = 1;
		T th = 0;
		T sth, cth, pang, wig, wag, wag2, wag3, wag12 = 0, waggle = 0;
		T rad = helper.m_PrecalcSqrtSumSquares;
		T curve1 = rad / m_L;
		T curve2 = Sqr(curve1);
		T curve3 = (rad - m_L * T(0.5)) / m_L;
		T curve4 = Sqr(curve2);
		th = helper.m_PrecalcAtanyx;
		sincos(th, &sth, &cth);

		if (rand.Frand01<T>() < T(0.5))
			posNeg = -1;

		pang = th / Zeps(m_Cycle);
		wig = pang * m_Freq * T(0.5) + m_Offset * m_Cycle;

		if (m_OptDir < 0)
		{
			wag = std::sin(curve1 * T(M_PI) * m_AbsOptSc) + m_Wagsc * T(0.4) * rad + m_Crvsc * T(0.5) * (std::sin(curve2 * T(M_PI)));
			wag3 = std::sin(curve4 * T(M_PI) * m_AbsOptSc) + m_Wagsc * SQR(rad) * T(0.4) + m_Crvsc * T(0.5) * (std::cos(curve3 * T(M_PI)));
		}
		else
		{
			wag = std::sin(curve1 * T(M_PI) * m_AbsOptSc) + m_Wagsc * T(0.4) * rad + m_Crvsc * T(0.5) * (std::cos(curve3 * T(M_PI)));
			wag3 = std::sin(curve4 * T(M_PI) * m_AbsOptSc) + m_Wagsc * SQR(rad) * T(0.4) + m_Crvsc * T(0.5) * (std::sin(curve2 * T(M_PI)));
		}

		wag2 = std::sin(curve2 * T(M_PI) * m_AbsOptSc) + m_Wagsc * T(0.4) * rad + m_Crvsc * T(0.5) * (std::cos(curve3 * T(M_PI)));

		if (m_Smooth12 <= 1)
			wag12 = wag;
		else if (m_Smooth12 <= 2 && m_Smooth12 > 1)
			wag12 = wag2 * (1 - m_AntiOpt1) + wag * m_AntiOpt1;
		else if (m_Smooth12 > 2)
			wag12 = wag2;

		if (m_Smooth3 == 0)
			waggle = wag12;
		else if (m_Smooth3 > 0)
			waggle = wag12 * (1 - m_Smooth3) + wag3 * m_Smooth3;

		if (rand.Frand01<T>() < m_Ghost)
		{
			if (posNeg < 0)
			{
				helper.Out.x = m_Weight * T(0.5) * m_RefSc * (m_L * std::cos(m_NumPetals * th + m_C)) * cth;
				helper.Out.y = m_Weight * T(0.5) * m_RefSc * (m_L * std::cos(m_NumPetals * th + m_C)) * sth;
				helper.Out.z = m_Weight * T(-0.5) * ((m_Z2 * waggle + Sqr(rad * T(0.5)) * std::sin(wig) * m_WigScale) + m_Dist);
			}
			else
			{
				helper.Out.x = m_Weight * T(0.5) * (m_L * std::cos(m_NumPetals * th + m_C)) * cth;
				helper.Out.y = m_Weight * T(0.5) * (m_L * std::cos(m_NumPetals * th + m_C)) * sth;
				helper.Out.z = m_Weight * T(0.5) * ((m_Z1 * waggle + Sqr(rad * T(0.5)) * std::sin(wig) * m_WigScale) + m_Dist);
			}
		}
		else
		{
			if (posNeg < 0)
			{
				helper.Out.x = m_Weight * T(0.5) * (m_L * std::cos(m_NumPetals * th + m_C)) * cth;
				helper.Out.y = m_Weight * T(0.5) * (m_L * std::cos(m_NumPetals * th + m_C)) * sth;
				helper.Out.z = m_Weight * T(0.5) * ((m_Z1 * waggle + Sqr(rad * T(0.5)) * std::sin(wig) * m_WigScale) + m_Dist);
			}
			else
			{
				helper.Out.x = m_Weight * T(0.5) * (m_L * std::cos(m_NumPetals * th + m_C)) * cth;
				helper.Out.y = m_Weight * T(0.5) * (m_L * std::cos(m_NumPetals * th + m_C)) * sth;
				helper.Out.z = m_Weight * T(0.5) * ((m_Z1 * waggle + Sqr(rad * T(0.5)) * std::sin(wig) * m_WigScale) + m_Dist);
			}
		}
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string l = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string k = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string c = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string z1 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string z2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string refSc = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string opt = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string optSc = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string opt3 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string transp = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string dist = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string wagsc = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string crvsc = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string f = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string wigsc = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string offset = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cycle = "parVars[" + ToUpper(m_Params[i++].Name()) + index;//Precalc.
		string optDir = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string petalsSign = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string numPetals = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string absOptSc = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string smooth12 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string smooth3 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string antiOpt1 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ghost = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string freq = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string wigScale = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\tint posNeg = 1;\n"
		   << "\t\treal_t th = 0;\n"
		   << "\t\treal_t sth, cth, pang, wig, wag, wag2, wag3, wag12, waggle;\n"
		   << "\t\treal_t rad = precalcSqrtSumSquares;\n"
		   << "\t\treal_t curve1 = rad / " << l << ";\n"
		   << "\t\treal_t curveTwo = Sqr(curve1);\n"
		   << "\t\treal_t curve3 = (rad - " << l << " * 0.5) / " << l << ";\n"
		   << "\t\treal_t curve4 = Sqr(curveTwo);\n"
		   << "\t\tth = precalcAtanyx;\n"
		   << "\t\tsth = sincos(th, &cth);\n"
		   << "\n"
		   << "\t\tif (MwcNext01(mwc) < 0.5)\n"
		   << "\t\t	posNeg = -1;\n"
		   << "\n"
		   << "\t\tpang = th / Zeps(" << cycle << ");\n"
		   << "\t\twig = pang * " << freq << " * 0.5 + " << offset << " * " << cycle << ";\n"
		   << "\n"
		   << "\t\tif (" << optDir << " < 0)\n"
		   << "\t\t{\n"
		   << "\t\t	wag = sin(curve1 * MPI * " << absOptSc << ") + " << wagsc << " * 0.4 * rad + " << crvsc << " * 0.5 * (sin(curveTwo * MPI));\n"
		   << "\t\t	wag3 = sin(curve4 * MPI * " << absOptSc << ") + " << wagsc << " * SQR(rad) * 0.4 + " << crvsc << " * 0.5 * (cos(curve3 * MPI));\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	wag = sin(curve1 * MPI * " << absOptSc << ") + " << wagsc << " * 0.4 * rad + " << crvsc << " * 0.5 * (cos(curve3 * MPI));\n"
		   << "\t\t	wag3 = sin(curve4 * MPI * " << absOptSc << ") + " << wagsc << " * SQR(rad) * 0.4 + " << crvsc << " * 0.5 * (sin(curveTwo * MPI));\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\twag2 = sin(curveTwo * MPI * " << absOptSc << ") + " << wagsc << " * 0.4 * rad + " << crvsc << " * 0.5 * (cos(curve3 * MPI));\n"
		   << "\n"
		   << "\t\tif (" << smooth12 << " <= 1)\n"
		   << "\t\t	wag12 = wag;\n"
		   << "\t\telse if (" << smooth12 << " <= 2 && " << smooth12 << " > 1)\n"
		   << "\t\t	wag12 = wag2 * (1 - " << antiOpt1 << ") + wag * " << antiOpt1 << ";\n"
		   << "\t\telse if (" << smooth12 << " > 2)\n"
		   << "\t\t	wag12 = wag2;\n"
		   << "\n"
		   << "\t\tif (" << smooth3 << " == 0)\n"
		   << "\t\t	waggle = wag12;\n"
		   << "\t\telse if (" << smooth3 << " > 0)\n"
		   << "\t\t	waggle = wag12 * (1 - " << smooth3 << ") + wag3 * " << smooth3 << ";\n"
		   << "\n"
		   << "\t\tif (MwcNext01(mwc) < " << ghost << ")\n"
		   << "\t\t{\n"
		   << "\t\t	if (posNeg < 0)\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = " << weight << " * 0.5 * " << refSc << " * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * cth;\n"
		   << "\t\t		vOut.y = " << weight << " * 0.5 * " << refSc << " * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * sth;\n"
		   << "\t\t		vOut.z = " << weight << " * -0.5 * ((" << z2 << " * waggle + Sqr(rad * 0.5) * sin(wig) * " << wigScale << ") + " << dist << ");\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = " << weight << " * 0.5 * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * cth;\n"
		   << "\t\t		vOut.y = " << weight << " * 0.5 * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * sth;\n"
		   << "\t\t		vOut.z = " << weight << " * 0.5 * ((" << z1 << " * waggle + Sqr(rad * 0.5) * sin(wig) * " << wigScale << ") + " << dist << ");\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	if (posNeg < 0)\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = " << weight << " * 0.5 * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * cth;\n"
		   << "\t\t		vOut.y = " << weight << " * 0.5 * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * sth;\n"
		   << "\t\t		vOut.z = " << weight << " * 0.5 * ((" << z1 << " * waggle + Sqr(rad * 0.5) * sin(wig) * " << wigScale << ") + " << dist << ");\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = " << weight << " * 0.5 * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * cth;\n"
		   << "\t\t		vOut.y = " << weight << " * 0.5 * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * sth;\n"
		   << "\t\t		vOut.z = " << weight << " * 0.5 * ((" << z1 << " * waggle + Sqr(rad * 0.5) * sin(wig) * " << wigScale << ") + " << dist << ");\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Sqr", "Zeps" };
	}

	virtual void Precalc() override
	{
		m_Cycle = M_2PI / Zeps(m_K);
		m_NumPetals = m_K;
		m_Ghost = Sqr(m_Transp);
		m_Freq = m_F * M_2PI;
		m_WigScale = m_Wigsc * T(0.5);

		if (std::abs(m_NumPetals) < T(0.0001))
			m_NumPetals = T(0.0001) * m_PetalsSign;

		m_Smooth12 = std::abs(m_Opt);
		m_Smooth3 = std::abs(m_Opt3);
		m_AbsOptSc = std::abs(m_OptSc);

		if (m_Smooth12 > 2)
			m_Smooth12 = 2;

		m_AntiOpt1 = 2 - m_Smooth12;

		if (m_Smooth3 > 1)
			m_Smooth3 = 1;

		m_OptDir = std::copysign(T(1.0), m_Opt);
		m_PetalsSign = std::copysign(T(1.0), m_K);

		if (m_Opt3 < 0)
			m_OptDir = -1;

		if (m_OptDir > 0)
			m_Ghost = 0;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_L, prefix + "pRose3D_l", 1, eParamType::REAL_NONZERO));
		m_Params.push_back(ParamWithName<T>(&m_K, prefix + "pRose3D_k", 3));
		m_Params.push_back(ParamWithName<T>(&m_C, prefix + "pRose3D_c"));
		m_Params.push_back(ParamWithName<T>(&m_Z1, prefix + "pRose3D_z1", 1));
		m_Params.push_back(ParamWithName<T>(&m_Z2, prefix + "pRose3D_z2", 1));
		m_Params.push_back(ParamWithName<T>(&m_RefSc, prefix + "pRose3D_refSc", 1));
		m_Params.push_back(ParamWithName<T>(&m_Opt, prefix + "pRose3D_opt", 1));
		m_Params.push_back(ParamWithName<T>(&m_OptSc, prefix + "pRose3D_optSc", 1));
		m_Params.push_back(ParamWithName<T>(&m_Opt3, prefix + "pRose3D_opt3"));
		m_Params.push_back(ParamWithName<T>(&m_Transp, prefix + "pRose3D_transp", T(0.5)));
		m_Params.push_back(ParamWithName<T>(&m_Dist, prefix + "pRose3D_dist", 1));
		m_Params.push_back(ParamWithName<T>(&m_Wagsc, prefix + "pRose3D_wagsc", 0));
		m_Params.push_back(ParamWithName<T>(&m_Crvsc, prefix + "pRose3D_crvsc", 0));
		m_Params.push_back(ParamWithName<T>(&m_F, prefix + "pRose3D_f", 3));
		m_Params.push_back(ParamWithName<T>(&m_Wigsc, prefix + "pRose3D_wigsc"));
		m_Params.push_back(ParamWithName<T>(&m_Offset, prefix + "pRose3D_offset"));
		m_Params.push_back(ParamWithName<T>(true, &m_Cycle, prefix + "pRose3D_cycle"));
		m_Params.push_back(ParamWithName<T>(true, &m_OptDir, prefix + "pRose3D_opt_dir"));
		m_Params.push_back(ParamWithName<T>(true, &m_PetalsSign, prefix + "pRose3D_petals_sign"));
		m_Params.push_back(ParamWithName<T>(true, &m_NumPetals, prefix + "pRose3D_num_petals"));
		m_Params.push_back(ParamWithName<T>(true, &m_AbsOptSc, prefix + "pRose3D_abs_optSc"));
		m_Params.push_back(ParamWithName<T>(true, &m_Smooth12, prefix + "pRose3D_smooth12"));
		m_Params.push_back(ParamWithName<T>(true, &m_Smooth3, prefix + "pRose3D_smooth3"));
		m_Params.push_back(ParamWithName<T>(true, &m_AntiOpt1, prefix + "pRose3D_anti_opt1"));
		m_Params.push_back(ParamWithName<T>(true, &m_Ghost, prefix + "pRose3D_ghost"));
		m_Params.push_back(ParamWithName<T>(true, &m_Freq, prefix + "pRose3D_freq"));
		m_Params.push_back(ParamWithName<T>(true, &m_WigScale, prefix + "pRose3D_wig_scale"));
	}

private:
	T m_L;
	T m_K;
	T m_C;
	T m_Z1;
	T m_Z2;
	T m_RefSc;
	T m_Opt;
	T m_OptSc;
	T m_Opt3;
	T m_Transp;
	T m_Dist;
	T m_Wagsc;
	T m_Crvsc;
	T m_F;
	T m_Wigsc;
	T m_Offset;
	T m_Cycle;//Precalc.
	T m_OptDir;
	T m_PetalsSign;
	T m_NumPetals;
	T m_AbsOptSc;
	T m_Smooth12;
	T m_Smooth3;
	T m_AntiOpt1;
	T m_Ghost;
	T m_Freq;
	T m_WigScale;
};

/// <summary>
/// log_db.
/// By DarkBeam, taken from JWildfire.
/// http://jwildfire.org/forum/viewtopic.php?f=23&t=1450&p=2692#p2692
/// </summary>
template <typename T>
class LogDBVariation : public ParametricVariation<T>
{
public:
	LogDBVariation(T weight = 1.0) : ParametricVariation<T>("log_db", eVariationId::VAR_LOG_DB, weight, true, false, false, false, true)
	{
		Init();
	}

	PARVARCOPY(LogDBVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		int i, adp;
		T atanPeriod = 0;

		for (i = 0; i < 7; i++)
		{
			adp = rand.Rand(10) - 5;

			if (std::abs(adp) >= 3)
				adp = 0;

			atanPeriod += adp;
		}

		atanPeriod *= m_FixPe;
		helper.Out.x = m_Denom * std::log(helper.m_PrecalcSumSquares);
		helper.Out.y = m_Weight * (helper.m_PrecalcAtanyx + atanPeriod);
		helper.Out.z = m_Weight * helper.In.z;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string base = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string fixPeriod = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string denom = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string fixPe = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\tint i, adp;\n"
		   << "\t\treal_t atanPeriod = 0;\n"
		   << "\n"
		   << "\t\tfor (i = 0; i < 7; i++)\n"
		   << "\t\t{\n"
		   << "\t\t	adp = MwcNextRange(mwc, 10) - 5;\n"
		   << "\n"
		   << "\t\t	if (abs(adp) >= 3)\n"
		   << "\t\t		adp = 0;\n"
		   << "\n"
		   << "\t\t	atanPeriod += adp;\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tatanPeriod *= " << fixPe << ";\n"
		   << "\t\tvOut.x = " << denom << " * log(precalcSumSquares);\n"
		   << "\t\tvOut.y = " << weight << " * (precalcAtanyx + atanPeriod);\n"
		   << "\t\tvOut.z = " << weight << " * vIn.z;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_Denom = T(0.5);

		if (m_Base > EPS)
			m_Denom = m_Denom / std::log(T(M_E) * m_Base);

		m_Denom *= m_Weight;
		m_FixPe = T(M_PI);

		if (m_FixPeriod > EPS)
			m_FixPe *= m_FixPeriod;
	}
protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Base, prefix + "log_db_base", 1));
		m_Params.push_back(ParamWithName<T>(&m_FixPeriod, prefix + "log_db_fix_period", 1));
		m_Params.push_back(ParamWithName<T>(true, &m_Denom, prefix + "log_db_denom"));
		m_Params.push_back(ParamWithName<T>(true, &m_FixPe, prefix + "log_db_fix_pe"));
	}
private:
	T m_Base;
	T m_FixPeriod;
	T m_Denom;
	T m_FixPe;
};

/// <summary>
/// circlesplit.
/// By tatasz.
/// http://fav.me/dapecsh
/// </summary>
template <typename T>
class CircleSplitVariation : public ParametricVariation<T>
{
public:
	CircleSplitVariation(T weight = 1.0) : ParametricVariation<T>("circlesplit", eVariationId::VAR_CIRCLESPLIT, weight, true, true, false, false, true)
	{
		Init();
	}

	PARVARCOPY(CircleSplitVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T x1, y1;

		if (helper.m_PrecalcSqrtSumSquares < (m_Radius - m_Split))
		{
			x1 = helper.In.x;
			y1 = helper.In.y;
		}
		else
		{
			T a = helper.m_PrecalcAtanyx;
			T len = helper.m_PrecalcSqrtSumSquares + m_Split;
			x1 = std::cos(a) * len;
			y1 = std::sin(a) * len;
		}

		helper.Out.x = m_Weight * x1;
		helper.Out.y = m_Weight * y1;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string cs_radius = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cs_split = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t x1, y1;\n"
		   << "\n"
		   << "\t\tif (precalcSqrtSumSquares < (" << cs_radius << " - " << cs_split << "))\n"
		   << "\t\t{\n"
		   << "\t\t\tx1 = vIn.x;\n"
		   << "\t\t\ty1 = vIn.y;\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t\treal_t a = precalcAtanyx;\n"
		   << "\t\t\treal_t len = precalcSqrtSumSquares + " << cs_split << ";\n"
		   << "\t\t\tx1 = cos(a) * len;\n"
		   << "\t\t\ty1 = sin(a) * len;\n"
		   << "\t\t}"
		   << "\t\tvOut.x = " << weight << " * x1;\n"
		   << "\t\tvOut.y = " << weight << " * y1;\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Radius, prefix + "circlesplit_radius", 1));
		m_Params.push_back(ParamWithName<T>(&m_Split, prefix + "circlesplit_split", T(0.5)));
	}

private:
	T m_Radius;
	T m_Split;
};

/// <summary>
/// cylinder2.
/// By tatasz.
/// http://fav.me/dapecsh
/// </summary>
template <typename T>
class Cylinder2Variation : public Variation<T>
{
public:
	Cylinder2Variation(T weight = 1.0) : Variation<T>("cylinder2", eVariationId::VAR_CYLINDER2, weight) { }

	VARCOPY(Cylinder2Variation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		helper.Out.x = m_Weight * (helper.In.x / Zeps(std::sqrt(SQR(helper.In.x) + 1)));
		helper.Out.y = m_Weight * helper.In.y;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss << "\t{\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * (vIn.x / Zeps(sqrt(SQR(vIn.x) + (real_t)1.0)));\n"
		   << "\t\tvOut.y = " << weight << " * vIn.y;\n"
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
/// tile_log.
/// By zy0rg.
/// http://zy0rg.deviantart.com
/// </summary>
template <typename T>
class TileLogVariation : public ParametricVariation<T>
{
public:
	TileLogVariation(T weight = 1.0) : ParametricVariation<T>("tile_log", eVariationId::VAR_TILE_LOG, weight)
	{
		Init();
	}

	PARVARCOPY(TileLogVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T temp = Round(std::log(rand.Frand01<T>()) * (rand.Rand() & 1 ? m_Spread : -m_Spread));
		helper.Out.x = m_Weight * (helper.In.x + temp);
		helper.Out.y = m_Weight * helper.In.y;
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string spread = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t temp = (real_t) (Round(log(MwcNext01(mwc)) * (MwcNext(mwc) & 1 ? " << spread << " : -" << spread << ")));\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * (vIn.x + temp);\n"
		   << "\t\tvOut.y = " << weight << " * vIn.y;\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Round" };
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Spread, prefix + "tile_log_spread", 1));
	}

private:
	T m_Spread;
};

/// <summary>
/// Truchet_fill.
/// By tatasz.
/// http://fav.me/dapecsh
/// </summary>
template <typename T>
class TruchetFillVariation : public ParametricVariation<T>
{
public:
	TruchetFillVariation(T weight = 1.0) : ParametricVariation<T>("Truchet_fill", eVariationId::VAR_TRUCHET_FILL, weight)
	{
		Init();
	}

	PARVARCOPY(TruchetFillVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T modbase = T(65535);
		T multiplier = T(32747); //1103515245;
		T offset = T(12345);
		T x = helper.In.x * m_Scale;
		T y = helper.In.y * m_Scale;
		T intx = Round(x);
		T inty = Round(y);
		T r = x - intx;

		if (r < 0)
			x = 1 + r;
		else
			x = r;

		r = y - inty;

		if (r < 0)
			y = 1 + r;
		else
			y = r;

		T tiletype = 0;

		if (m_Seed != 0)
		{
			if (m_Seed == 1)
			{
				tiletype = m_Seed;
			}
			else
			{
				T xrand = helper.In.x;
				T yrand = helper.In.y;
				xrand = Round(std::abs(xrand)) * m_Seed2;
				yrand = Round(std::abs(yrand)) * m_Seed2;
				T niter = xrand + yrand + (xrand * yrand);
				T randint = (m_Seed + niter) * m_Seed2 * T(0.5);
				randint = std::fmod((randint * multiplier + offset), modbase);
				tiletype = std::fmod(randint, T(2.0));
			}
		}

		T r0, r1;

		if (tiletype < T(1))
		{
			//Slow drawmode
			r0 = std::pow((std::pow(std::fabs(x), m_FinalExponent) + std::pow(std::fabs(y), m_FinalExponent)), m_OneOverEx);
			r1 = std::pow((std::pow(std::fabs(x - T(1.0)), m_FinalExponent) + std::pow(std::fabs(y - 1), m_FinalExponent)), m_OneOverEx);
		}
		else
		{
			r0 = std::pow((std::pow(std::fabs(x - T(1.0)), m_FinalExponent) + std::pow(std::fabs(y), m_FinalExponent)), m_OneOverEx);
			r1 = std::pow((std::pow(std::fabs(x), m_FinalExponent) + std::pow(std::fabs(y - T(1.0)), m_FinalExponent)), m_OneOverEx);
		}

		T x1, y1;
		T r00 = fabs(r0 - T(0.5)) / m_Rmax;

		if (r00 < 1)
		{
			x1 = 2 * (x + std::floor(helper.In.x));
			y1 = 2 * (y + std::floor(helper.In.y));
		}
		else
		{
			x1 = 0;
			y1 = 0;
		}

		T r11 = std::fabs(r1 - T(0.5)) / m_Rmax;

		if (r11 < 1)
		{
			helper.Out.x = x1 + (2 * (x + std::floor(helper.In.x))) - helper.In.x;
			helper.Out.y = y1 + (2 * (y + std::floor(helper.In.y))) - helper.In.y;
		}
		else
		{
			helper.Out.x = x1 - helper.In.x;
			helper.Out.y = y1 - helper.In.y;
		}
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0;
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string exponent = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string arcWidth = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string seed = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string finalexponent = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string oneOverEx = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string width = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string seed2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string rmax = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scale = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t modbase = 65535;\n"
		   << "\t\treal_t multiplier = 32747;\n"
		   << "\t\treal_t offset = 12345;\n"
		   << "\n"
		   << "\t\treal_t x = vIn.x * " << scale << ";\n"
		   << "\t\treal_t y = vIn.y * " << scale << ";\n"
		   << "\t\treal_t intx = Round(x);\n"
		   << "\t\treal_t inty = Round(y);\n"
		   << "\n"
		   << "\t\treal_t r = x - intx;\n"
		   << "\n"
		   << "\t\tif (r < 0)\n"
		   << "\t\t\tx = r + 1;\n"
		   << "\t\telse\n"
		   << "\t\t\tx = r;\n"
		   << "\n"
		   << "\t\tr = y - inty;\n"
		   << "\n"
		   << "\t\tif (r < 0)\n"
		   << "\t\t\ty = r + 1;\n"
		   << "\t\telse\n"
		   << "\t\t\ty = r;\n"
		   << "\n"
		   << "\t\treal_t tiletype = 0;\n"
		   << "\n"
		   << "\t\tif (" << seed << " != 0)\n"
		   << "\t\t{\n"
		   << "\t\t\tif (" << seed << " == 1)\n"
		   << "\t\t\t{\n"
		   << "\t\t\t\ttiletype = " << seed << ";\n"
		   << "\t\t\t}\n"
		   << "\t\t\telse\n"
		   << "\t\t\t{\n"
		   << "\t\t\t\treal_t xrand = vIn.x;\n"
		   << "\t\t\t\treal_t yrand = vIn.y;\n"
		   << "\n"
		   << "\t\t\t\txrand = Round(fabs(xrand)) * " << seed2 << ";\n"
		   << "\t\t\t\tyrand = Round(fabs(yrand)) * " << seed2 << ";\n"
		   << "\n"
		   << "\t\t\t\treal_t niter = xrand + yrand + (xrand * yrand);\n"
		   << "\t\t\t\treal_t randint = (" << seed << " + niter) * " << seed2 << " * ((real_t) 0.5);\n"
		   << "\n"
		   << "\t\t\t\trandint = fmod((randint * multiplier + offset), modbase);\n"
		   << "\t\t\t\ttiletype = fmod(randint, 2);\n"
		   << "\t\t\t}\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\treal_t r0, r1;\n"
		   << "\n"
		   << "\t\tif (tiletype < 1)\n"
		   << "\t\t{\n"
		   << "\t\t\tr0 = pow((pow(fabs(x), " << finalexponent << ") + pow(fabs(y), " << finalexponent << ")), " << oneOverEx << ");\n"
		   << "\t\t\tr1 = pow((pow(fabs(x-1), " << finalexponent << ") + pow(fabs(y-1), " << finalexponent << ")), " << oneOverEx << ");\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t\tr0 = pow((pow(fabs(x-1), " << finalexponent << ") + pow(fabs(y), " << finalexponent << ")), " << oneOverEx << ");\n"
		   << "\t\t\tr1 = pow((pow(fabs(x), " << finalexponent << ") + pow(fabs(y-1), " << finalexponent << ")), " << oneOverEx << ");\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\treal_t x1, y1;\n"
		   << "\t\treal_t r00 = fabs(r0 - (real_t) 0.5) / " << rmax << ";\n"
		   << "\n"
		   << "\t\tif (r00 < 1.0)\n"
		   << "\t\t{\n"
		   << "\t\t\tx1 = 2 * (x + floor(vIn.x));\n"
		   << "\t\t\ty1 = 2 * (y + floor(vIn.y));\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t\tx1 = 0;\n"
		   << "\t\t\ty1 = 0;\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\treal_t r11 = fabs(r1 - (real_t) 0.5) / " << rmax << ";\n"
		   << "\n"
		   << "\t\tif (r11 < 1)\n"
		   << "\t\t{\n"
		   << "\t\t\tvOut.x = x1 + (2 * (x + floor(vIn.x))) - vIn.x;\n"
		   << "\t\t\tvOut.y = y1 + (2 * (y + floor(vIn.y))) - vIn.y;\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t\tvOut.x = x1 - vIn.x;\n"
		   << "\t\t\tvOut.y = y1 - vIn.y;\n"
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
		m_FinalExponent = m_Exponent > T(2) ? T(2) : (m_Exponent < T(0.001) ? T(0.001) : m_Exponent);
		m_OneOverEx = T(1) / m_FinalExponent;
		m_Width = m_ArcWidth > T(1) ? T(1) : (m_ArcWidth < T(0.001) ? T(0.001) : m_ArcWidth);
		m_Seed2 = std::sqrt(m_Seed * T(1.5)) / Zeps(m_Seed * T(0.5)) * T(0.25);
		m_Rmax = T(0.5) * (std::pow(T(2), m_OneOverEx) - T(1)) * m_Width;
		m_Scale = T(1) / m_Weight;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Exponent, prefix + "Truchet_fill_exponent", 2, eParamType::REAL_CYCLIC, T(0.001), 2));
		m_Params.push_back(ParamWithName<T>(&m_ArcWidth, prefix + "Truchet_fill_arc_width", T(0.5), eParamType::REAL_CYCLIC, T(0.001), 1));
		m_Params.push_back(ParamWithName<T>(&m_Seed, prefix + "Truchet_fill_seed"));
		m_Params.push_back(ParamWithName<T>(true, &m_FinalExponent, prefix + "Truchet_fill_final_exponent"));//Precalc
		m_Params.push_back(ParamWithName<T>(true, &m_OneOverEx, prefix + "Truchet_fill_oneoverex"));
		m_Params.push_back(ParamWithName<T>(true, &m_Width, prefix + "Truchet_fill_width"));
		m_Params.push_back(ParamWithName<T>(true, &m_Seed2, prefix + "Truchet_fill_seed2"));
		m_Params.push_back(ParamWithName<T>(true, &m_Rmax, prefix + "Truchet_fill_rmax"));
		m_Params.push_back(ParamWithName<T>(true, &m_Scale, prefix + "Truchet_fill_scale"));
	}

private:
	T m_Exponent;
	T m_ArcWidth;
	T m_Seed;
	T m_FinalExponent;//Precalc.
	T m_OneOverEx;
	T m_Width;
	T m_Seed2;
	T m_Rmax;
	T m_Scale;
};

/// <summary>
/// waves2_radial.
/// By tatasz.
/// http://fav.me/dapecsh
/// </summary>
template <typename T>
class Waves2RadialVariation : public ParametricVariation<T>
{
public:
	Waves2RadialVariation(T weight = 1.0) : ParametricVariation<T>("waves2_radial", eVariationId::VAR_WAVES2_RADIAL, weight, true, true)
	{
		Init();
	}

	PARVARCOPY(Waves2RadialVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T x0 = helper.In.x;
		T y0 = helper.In.y;
		T dist = helper.m_PrecalcSqrtSumSquares;
		T factor = (dist < m_Distance) ? (dist - m_Null) / Zeps(m_Distance - m_Null) : T(1);
		factor = (dist < m_Null) ? T(0) : factor;
		helper.Out.x = m_Weight * (x0 + factor * std::sin(y0 * m_Freqx) * m_Scalex);
		helper.Out.y = m_Weight * (y0 + factor * std::sin(x0 * m_Freqy) * m_Scaley);
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string freqX = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scaleX = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string freqY = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scaleY = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string nullVar = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string distance = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t x0 = vIn.x;\n"
		   << "\t\treal_t y0 = vIn.y;\n"
		   << "\n"
		   << "\t\treal_t dist = precalcSqrtSumSquares;\n"
		   << "\t\treal_t factor = (dist < " << distance << ") ? (dist - " << nullVar << ") / Zeps(" << distance << "-" << nullVar << ") : (real_t)(1.0);\n"
		   << "\t\tfactor = (dist < " << nullVar << ") ? (real_t) 0.0 : factor;\n"
		   << "\n"
		   << "\t\tvOut.x = " << weight << " * (x0 + factor * sin(y0 * " << freqX << ") * " << scaleX << ");\n"
		   << "\t\tvOut.y = " << weight << " * (y0 + factor * sin(x0 * " << freqY << ") * " << scaleY << ");\n"
		   << "\t\tvOut.z = " << DefaultZCl()
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
		m_Params.push_back(ParamWithName<T>(&m_Freqx, prefix + "waves2_radial_freqx", 7));
		m_Params.push_back(ParamWithName<T>(&m_Scalex, prefix + "waves2_radial_scalex", T(0.1)));
		m_Params.push_back(ParamWithName<T>(&m_Freqy, prefix + "waves2_radial_freqy", 13));
		m_Params.push_back(ParamWithName<T>(&m_Scaley, prefix + "waves2_radial_scaley", T(0.1)));
		m_Params.push_back(ParamWithName<T>(&m_Null, prefix + "waves2_radial_null", 2));
		m_Params.push_back(ParamWithName<T>(&m_Distance, prefix + "waves2_radial_distance", 10));
	}

private:
	T m_Freqx;
	T m_Scalex;
	T m_Freqy;
	T m_Scaley;
	T m_Null;
	T m_Distance;
};

/// <summary>
/// panorama1.
/// </summary>
template <typename T>
class Panorama1Variation : public Variation<T>
{
public:
	Panorama1Variation(T weight = 1.0) : Variation<T>("panorama1", eVariationId::VAR_PANORAMA1, weight, true)
	{
	}

	VARCOPY(Panorama1Variation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T aux = 1 / std::sqrt(helper.m_PrecalcSumSquares + 1);
		T x1 = helper.m_TransX * aux;
		T y1 = helper.m_TransY * aux;
		aux = std::sqrt(x1 * x1 + y1 * y1);
		helper.Out.x = m_Weight * std::atan2(x1, y1) * T(M_1_PI);
		helper.Out.y = m_Weight * (aux - T(0.5));
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		ss << "\t{\n"
		   << "\t\treal_t aux = 1.0 / sqrt(precalcSumSquares + 1);\n"
		   << "\t\treal_t x1 = transX * aux;\n"
		   << "\t\treal_t y1 = transY * aux;\n"
		   << "\t\taux = sqrt(x1 * x1 + y1 * y1);\n"
		   << "\t\tvOut.x = " << weight << " * atan2(x1, y1) * M1PI;\n"
		   << "\t\tvOut.y = " << weight << " * (aux - 0.5);\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}
};

/// <summary>
/// panorama2.
/// </summary>
template <typename T>
class Panorama2Variation : public Variation<T>
{
public:
	Panorama2Variation(T weight = 1.0) : Variation<T>("panorama2", eVariationId::VAR_PANORAMA2, weight, true, true)
	{
	}

	VARCOPY(Panorama2Variation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T aux = 1 / (helper.m_PrecalcSqrtSumSquares + 1);
		T x1 = helper.m_TransX * aux;
		T y1 = helper.m_TransY * aux;
		aux = std::sqrt(x1 * x1 + y1 * y1);
		helper.Out.x = m_Weight * std::atan2(x1, y1) * T(M_1_PI);
		helper.Out.y = m_Weight * (aux - T(0.5));
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t varIndex = IndexInXform();
		string weight = WeightDefineString();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		ss << "\t{\n"
		   << "\t\treal_t aux = 1.0 / (precalcSqrtSumSquares + 1);\n"
		   << "\t\treal_t x1 = transX * aux;\n"
		   << "\t\treal_t y1 = transY * aux;\n"
		   << "\t\taux = sqrt(x1 * x1 + y1 * y1);\n"
		   << "\t\tvOut.x = " << weight << " * atan2(x1, y1) * M1PI;\n"
		   << "\t\tvOut.y = " << weight << " * (aux - 0.5);\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}
};

/// <summary>
/// arcsinh.
/// </summary>
/*
	template <typename T>
	class ArcsinhVariation : public ParametricVariation<T>
	{
	public:
	ArcsinhVariation(T weight = 1.0) : ParametricVariation<T>("arcsinh", eVariationId::VAR_ARCSINH, weight, false, false, false, false, false)
	{
	}

	PARVARCOPY(ArcsinhVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
	std::complex<T> z(helper.m_TransX, helper.m_TransY);
	std::complex<T> result = m_Vpi * std::log(z + std::sqrt(z * z + 1.0));
	helper.Out.x = result.real();
	helper.Out.y = result.imag();
	helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override//Hold off on this for now, opencl doesn't have support for complex.
	{
	ostringstream ss, ss2;

	return ss.str();
	}

	virtual void Precalc() override
	{
	m_Vpi = m_Weight * M_2_PI;
	}

	protected:
	void Init()
	{
	string prefix = Prefix();
	m_Params.clear();
	m_Params.push_back(ParamWithName<T>(true, &m_Vpi, prefix + "arcsinh_vpi"));//Precalc.
	}

	private:
	T m_Vpi;//Precalc.
	};*/

/// <summary>
/// helicoid.
/// </summary>
template <typename T>
class HelicoidVariation : public ParametricVariation<T>
{
public:
	HelicoidVariation(T weight = 1.0) : ParametricVariation<T>("helicoid", eVariationId::VAR_HELICOID, weight, true, true, false, false, true)
	{
		Init();
	}

	PARVARCOPY(HelicoidVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T temp = helper.In.z * m_Freq2Pi + helper.m_PrecalcAtanyx;
		T weightXdist = m_Weight * helper.m_PrecalcSqrtSumSquares;
		helper.Out.x = weightXdist * std::cos(temp);
		helper.Out.y = weightXdist * std::sin(temp);
		helper.Out.z = m_Weight * helper.In.z;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string freq = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string freq2pi = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t temp = vIn.z * " << freq2pi << " + precalcAtanyx;\n"
		   << "\t\treal_t weightXdist = " << weight << " * precalcSqrtSumSquares;\n"
		   << "\t\tvOut.x = weightXdist * cos(temp);\n"
		   << "\t\tvOut.y = weightXdist * sin(temp);\n"
		   << "\t\tvOut.z = " << weight << " * vIn.z;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_Freq2Pi = m_Freq * M_2PI;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Freq, prefix + "helicoid_frequency", 1));
		m_Params.push_back(ParamWithName<T>(true, &m_Freq2Pi, prefix + "helicoid_frequency_2pi"));//Precalc.
	}

private:
	T m_Freq;
	T m_Freq2Pi;//Precalc.
};

/// <summary>
/// helix.
/// </summary>
template <typename T>
class HelixVariation : public ParametricVariation<T>
{
public:
	HelixVariation(T weight = 1.0) : ParametricVariation<T>("helix", eVariationId::VAR_HELIX, weight)
	{
		Init();
	}

	PARVARCOPY(HelixVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T temp = helper.In.z * m_Freq2Pi;
		helper.Out.x = m_Weight * (helper.In.x + std::cos(temp) * m_Width);
		helper.Out.y = m_Weight * (helper.In.y + std::sin(temp) * m_Width);
		helper.Out.z = m_Weight * helper.In.z;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string freq = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string width = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string freq2pi = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t temp = vIn.z  * " << freq2pi << ";\n"
		   << "\t\tvOut.x = " << weight << " * (vIn.x + cos(temp) * " << width << ");\n"
		   << "\t\tvOut.y = " << weight << " * (vIn.y + sin(temp) * " << width << ");\n"
		   << "\t\tvOut.z = " << weight << " * vIn.z ;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_Freq2Pi = m_Freq * M_2PI;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Freq, prefix + "helix_frequency", 1));
		m_Params.push_back(ParamWithName<T>(&m_Width, prefix + "helix_width", T(0.5)));
		m_Params.push_back(ParamWithName<T>(true, &m_Freq2Pi, prefix + "helix_frequency_2pi"));//Precalc.
	}

private:
	T m_Freq;
	T m_Width;
	T m_Freq2Pi;//Precalc.
};

/// <summary>
/// sphereblur.
/// </summary>
template <typename T>
class SphereblurVariation : public ParametricVariation<T>
{
public:
	SphereblurVariation(T weight = 1.0) : ParametricVariation<T>("sphereblur", eVariationId::VAR_SPHEREBLUR, weight)
	{
		Init();
	}

	PARVARCOPY(SphereblurVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T ang = rand.Frand01<T>() * M_2PI;
		T angz = std::acos(rand.Frand01<T>() * 2 - 1);
		T r = m_Weight * std::exp(std::log(std::acos((m_Power == T(1.0) ? rand.Frand01<T>() : std::exp(std::log(rand.Frand01<T>()) * m_Power)) * 2 - 1) / T(M_PI)) / T(1.5));
		T s = std::sin(ang);
		T c = std::cos(ang);
		T sz = std::sin(angz);
		T cz = std::cos(angz);
		helper.Out.x = r * c * sz;
		helper.Out.y = r * s * sz;
		helper.Out.z = r * cz;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string power = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t ang = MwcNext01(mwc) * M_2PI;\n"
		   << "\t\treal_t angz = acos(MwcNext01(mwc) * 2 - 1);\n"
		   << "\t\treal_t r = " << weight << " * exp(log(acos((" << power << " == 1.0 ? MwcNext01(mwc) : exp(log(MwcNext01(mwc)) * " << power << ")) * 2 - 1) / M_PI) / 1.5);\n"
		   << "\t\treal_t s = sin(ang);\n"
		   << "\t\treal_t c = cos(ang);\n"
		   << "\t\treal_t sz = sin(angz);\n"
		   << "\t\treal_t cz = cos(angz);\n"
		   << "\t\tvOut.x = r * c * sz;\n"
		   << "\t\tvOut.y = r * s * sz;\n"
		   << "\t\tvOut.z = r * cz;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		if (m_Power < 0)
			m_Power = 0;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Power, prefix + "sphereblur_power", 1, eParamType::REAL, 0));
	}

private:
	T m_Power;
};

/// <summary>
/// cpow3.
/// </summary>
template <typename T>
class Cpow3Variation : public ParametricVariation<T>
{
public:
	Cpow3Variation(T weight = 1.0) : ParametricVariation<T>("cpow3", eVariationId::VAR_CPOW3, weight, true, false, false, false, true)
	{
		Init();
	}

	PARVARCOPY(Cpow3Variation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T a = helper.m_PrecalcAtanyx;

		if (a < 0) a += M_2PI;

		if (std::cos(a / 2) < rand.Frand11<T>())
			a -= M_2PI;

		a += (rand.RandBit() ? M_2PI : -M_2PI) * std::round(std::log(rand.Frand01<T>()) * m_Coeff);
		T lnr2 = std::log(helper.m_PrecalcSumSquares);
		T r = m_Weight * std::exp(m_HalfC * lnr2 - m_PrecalcD * a);
		T temp = m_PrecalcC * a + m_HalfD * lnr2 + m_Ang * rand.Rand();
		helper.Out.x = r * std::cos(temp);
		helper.Out.y = r * std::sin(temp);
		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string r = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string d = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string divisor = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string spread = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string precalcc = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string halfc = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string precalcd = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string halfd = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ang = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string coeff = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t a = precalcAtanyx;\n"
		   << "\n"
		   << "\t\tif (a < 0) a += M_2PI;\n"
		   << "\n"
		   << "\t\tif (cos(a / 2) < MwcNextNeg1Pos1(mwc))\n"
		   << "\t\ta -= M_2PI;\n"
		   << "\n"
		   << "\t\ta += ((MwcNext(mwc) & 1) ? M_2PI : -M_2PI) * round(log(MwcNext01(mwc)) * " << coeff << ");\n"
		   << "\t\treal_t lnr2 = log(precalcSumSquares);\n"
		   << "\t\treal_t r = " << weight << " * exp(" << halfc << " * lnr2 - " << precalcd << " * a);\n"
		   << "\t\treal_t temp = " << precalcc << " * a + " << halfd << " * lnr2 + " << ang << " * MwcNext(mwc);\n"
		   << "\t\tvOut.x = r * cos(temp);\n"
		   << "\t\tvOut.y = r * sin(temp);\n"
		   << "\t\tvOut.z = " << DefaultZCl()
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_Ang = M_2PI / Zeps(m_Divisor);
		T a = std::atan2((m_D < 0 ? -std::log(-m_D) : std::log(m_D)) * m_R, M_2PI);
		m_PrecalcC = std::cos(a) * m_R * std::cos(a) / m_Divisor;
		m_PrecalcD = std::cos(a) * m_R * std::sin(a) / m_Divisor;
		m_HalfC = m_PrecalcC / 2;
		m_HalfD = m_PrecalcD / 2;
		m_Coeff = m_PrecalcD == 0 ? 0 : T(-0.095) * m_Spread / m_PrecalcD;
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_R, prefix + "cpow3_r", 1));
		m_Params.push_back(ParamWithName<T>(&m_D, prefix + "cpow3_d", 1, eParamType::REAL_NONZERO));
		m_Params.push_back(ParamWithName<T>(&m_Divisor, prefix + "cpow3_divisor", 1, eParamType::INTEGER_NONZERO));
		m_Params.push_back(ParamWithName<T>(&m_Spread, prefix + "cpow3_spread", 1));
		m_Params.push_back(ParamWithName<T>(true, &m_PrecalcC, prefix + "cpow3_precalc_c"));//Precalc.
		m_Params.push_back(ParamWithName<T>(true, &m_HalfC, prefix + "cpow3_half_c"));
		m_Params.push_back(ParamWithName<T>(true, &m_PrecalcD, prefix + "cpow3_precalc_d"));
		m_Params.push_back(ParamWithName<T>(true, &m_HalfD, prefix + "cpow3_half_d"));
		m_Params.push_back(ParamWithName<T>(true, &m_Ang, prefix + "cpow3_ang"));
		m_Params.push_back(ParamWithName<T>(true, &m_Coeff, prefix + "cpow3_coeff"));
	}

private:
	T m_R;
	T m_D;
	T m_Divisor;
	T m_Spread;
	T m_PrecalcC;//Precalc.
	T m_HalfC;
	T m_PrecalcD;
	T m_HalfD;
	T m_Ang;
	T m_Coeff;
};

/// <summary>
/// concentric.
/// </summary>
template <typename T>
class ConcentricVariation : public ParametricVariation<T>
{
public:
	ConcentricVariation(T weight = 1.0) : ParametricVariation<T>("concentric", eVariationId::VAR_CONCENTRIC, weight)
	{
		Init();
	}

	PARVARCOPY(ConcentricVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T lvl = (Floor<T>(rand.Frand01<T>() * m_Density) / Zeps(m_Density)); //random level. should care if density=0 but meh, works fine
		T randa = rand.Frand01<T>() * M_2PI; //random angle
		T randr = lvl * m_Radius; //calc radius of rings

		if (m_Rblur != 0)
			randr += (std::sqrt(rand.Frand01<T>()) * 2 - 1) * m_Rblur; //blur ring. sqrt is expensive but gives nice effect

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			outPoint.m_X = 0;//This variation intentionally assigns instead of summing.
			outPoint.m_Y = 0;
			outPoint.m_Z = 0;
		}

		helper.Out.x = randr * std::cos(randa) * m_Weight; //polar to cartesian coords, origin is origin
		helper.Out.y = randr * std::sin(randa) * m_Weight;
		T zb = 0;

		if (m_Zblur != 0)
			zb = (rand.Frand01<T>() * 2 - 1) * m_Zblur;

		helper.Out.z = (-lvl + zb) * m_Weight;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string radius = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string density = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string rblur = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string zblur = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t lvl = floor(MwcNext01(mwc) * " << density << ") / Zeps(" << density << ");\n"
		   << "\t\treal_t randa = MwcNext01(mwc) * M_2PI;\n"
		   << "\t\treal_t randr = lvl * " << radius << ";\n"
		   << "\n"
		   << "\t\tif (" << rblur << " != 0)\n"
		   << "\t\t\trandr += (sqrt(MwcNext01(mwc)) * 2.0 - 1.0) * " << rblur << ";\n";

		if (m_VarType == eVariationType::VARTYPE_REG)
		{
			ss << "\t\toutPoint->m_X = 0;\n"
			   << "\t\toutPoint->m_Y = 0;\n"
			   << "\t\toutPoint->m_Z = 0;\n";
		}

		ss << "\t\tvOut.x = randr * cos(randa) * " << weight << ";\n"
		   << "\t\tvOut.y = randr * sin(randa) * " << weight << ";\n"
		   << "\t\treal_t zb = 0;\n"
		   << "\n"
		   << "\t\tif (" << zblur << " != 0)\n"
		   << "\t\t\tzb = (MwcNext01(mwc) * 2.0 - 1.0) * " << zblur << ";\n"
		   << "\n"
		   << "\t\tvOut.z = (-lvl + zb) * " << weight << ";\n"
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
		m_Params.push_back(ParamWithName<T>(&m_Radius, prefix + "concentric_radius", 1));
		m_Params.push_back(ParamWithName<T>(&m_Density, prefix + "concentric_density", 10, eParamType::REAL_NONZERO));
		m_Params.push_back(ParamWithName<T>(&m_Rblur, prefix + "concentric_R_blur"));
		m_Params.push_back(ParamWithName<T>(&m_Zblur, prefix + "concentric_Z_blur"));
	}

private:
	T m_Radius;
	T m_Density;
	T m_Rblur;
	T m_Zblur;
};

/// <summary>
/// hypercrop.
/// </summary>
template <typename T>
class HypercropVariation : public ParametricVariation<T>
{
public:
	HypercropVariation(T weight = 1.0) : ParametricVariation<T>("hypercrop", eVariationId::VAR_HYPERCROP, weight, false, false, false, false, true)
	{
		Init();
	}

	PARVARCOPY(HypercropVariation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T fx = helper.In.x;
		T fy = helper.In.y;
		T fz = helper.In.z;
		T a0 = T(M_PI) / m_N;
		T len = 1 / Zeps(std::cos(a0));
		T d = m_Rad * std::sin(a0) * len;
		T angle = Floor<T>(helper.m_PrecalcAtanyx * m_Coeff) / m_Coeff + T(M_PI) / m_N;
		T x0 = std::cos(angle) * len;
		T y0 = std::sin(angle) * len;

		if (std::sqrt(Sqr(helper.In.x - x0) + Sqr(helper.In.y - y0)) < d)
		{
			if (m_Zero > 1.5)
			{
				fx = x0;
				fy = y0;
				fz = 0;
			}
			else
			{
				if (m_Zero > 0.5)
				{
					fx = 0;
					fy = 0;
					fz = 0;
				}
				else
				{
					T rangle = std::atan2(helper.In.y - y0, helper.In.x - x0);
					fx = x0 + std::cos(rangle) * d;
					fy = y0 + std::sin(rangle) * d;
					fz = 0;
				}
			}
		}

		helper.Out.x = fx * m_Weight;
		helper.Out.y = fy * m_Weight;
		helper.Out.z = fz * m_Weight;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string n     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string rad   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string zero  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string coeff = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t fx = vIn.x;\n"
		   << "\t\treal_t fy = vIn.y;\n"
		   << "\t\treal_t fz = vIn.z;\n"
		   << "\t\treal_t a0 = M_PI / " << n << ";\n"
		   << "\t\treal_t len = 1 / Zeps(cos(a0));\n"
		   << "\t\treal_t d = " << rad << " * sin(a0) * len;\n"
		   << "\t\treal_t angle = floor(precalcAtanyx * " << coeff << ") / " << coeff << " + M_PI / " << n << ";\n"
		   << "\t\treal_t x0 = cos(angle) * len;\n"
		   << "\t\treal_t y0 = sin(angle) * len;\n"
		   << "\n"
		   << "\t\tif (sqrt(Sqr(vIn.x - x0) + Sqr(vIn.y - y0)) < d)\n"
		   << "\t\t{\n"
		   << "\t\t	if (" << zero << " > 1.5)\n"
		   << "\t\t	{\n"
		   << "\t\t		fx = x0;\n"
		   << "\t\t		fy = y0;\n"
		   << "\t\t		fz = 0;\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		if (" << zero << " > 0.5)\n"
		   << "\t\t		{\n"
		   << "\t\t			fx = 0;\n"
		   << "\t\t			fy = 0;\n"
		   << "\t\t			fz = 0;\n"
		   << "\t\t		}\n"
		   << "\t\t		else\n"
		   << "\t\t		{\n"
		   << "\t\t			real_t rangle = atan2(vIn.y - y0, vIn.x - x0);\n"
		   << "\t\t			fx = x0 + cos(rangle) * d;\n"
		   << "\t\t			fy = y0 + sin(rangle) * d;\n"
		   << "\t\t			fz = 0;\n"
		   << "\t\t		}\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\tvOut.x = fx * " << weight << ";\n"
		   << "\t\tvOut.y = fy * " << weight << ";\n"
		   << "\t\tvOut.z = fz * " << weight << ";\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		m_N = Zeps(m_N);
		m_Coeff = Zeps<T>(m_N * T(0.5) / T(M_PI));
	}

	virtual vector<string> OpenCLGlobalFuncNames() const override
	{
		return vector<string> { "Zeps", "Sqr" };
	}

protected:
	void Init()
	{
		string prefix = Prefix();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_N,     prefix + "hypercrop_n", 4));
		m_Params.push_back(ParamWithName<T>(&m_Rad,   prefix + "hypercrop_rad", 1));
		m_Params.push_back(ParamWithName<T>(&m_Zero,  prefix + "hypercrop_zero"));
		m_Params.push_back(ParamWithName<T>(true, &m_Coeff, prefix + "hypercrop_coeff"));//Precalc.
	}

private:
	T m_N;
	T m_Rad;
	T m_Zero;
	T m_Coeff;//Precalc.
};

/// <summary>
/// hypershift2.
/// </summary>
template <typename T>
class Hypershift2Variation : public ParametricVariation<T>
{
public:
	Hypershift2Variation(T weight = 1.0) : ParametricVariation<T>("hypershift2", eVariationId::VAR_HYPERSHIFT2, weight)
	{
		Init();
	}

	PARVARCOPY(Hypershift2Variation)

	virtual void Func(IteratorHelper<T>& helper, Point<T>& outPoint, QTIsaac<ISAAC_SIZE, ISAAC_INT>& rand) override
	{
		T fx = helper.In.x * m_Scale2;
		T fy = helper.In.y * m_Scale2;
		T rad = 1 / Zeps(fx * fx + fy * fy);
		T x = rad * fx + m_Shift;
		T y = rad * fy;
		rad = m_Weight * m_Scale / Zeps(x * x + y * y);
		T angle = ((rand.Rand() % int(m_P)) * 2 + 1) * T(M_PI) / m_P;
		T X = rad * x + m_Shift;
		T Y = rad * y;
		T cosa = std::cos(angle);
		T sina = std::sin(angle);

		if (m_VarType == eVariationType::VARTYPE_REG)
			outPoint.m_X = outPoint.m_Y = outPoint.m_Z = 0;//This variation assigns, instead of summing, so order will matter.

		helper.Out.x = cosa * X - sina * Y;
		helper.Out.y = sina * X + cosa * Y;
		helper.Out.z = helper.In.z * rad;
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string weight = WeightDefineString();
		string p      = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string q      = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string shift  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scale  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scale2 = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\treal_t fx = vIn.x * " << scale2 << ";\n"
		   << "\t\treal_t fy = vIn.y * " << scale2 << ";\n"
		   << "\t\treal_t rad = 1 / Zeps(fx * fx + fy * fy);\n"
		   << "\t\treal_t x = rad * fx + " << shift << ";\n"
		   << "\t\treal_t y = rad * fy;\n"
		   << "\t\trad = " << weight << " * " << shift << " / Zeps(x * x + y * y);\n"
		   << "\t\treal_t angle = ((MwcNext(mwc) % (int)" << p << ") * 2 + 1) * M_PI / " << p << ";\n"
		   << "\t\treal_t X = rad * x + " << shift << ";\n"
		   << "\t\treal_t Y = rad * y;\n"
		   << "\t\treal_t cosa = cos(angle);\n"
		   << "\t\treal_t sina = sin(angle);\n";

		if (m_VarType == eVariationType::VARTYPE_REG)
			ss << "\t\toutPoint->m_X = outPoint->m_Y = outPoint->m_Z = 0;\n";

		ss << "\t\tvOut.x = cosa * X - sina * Y;\n"
		   << "\t\tvOut.y = sina * X + cosa * Y;\n"
		   << "\t\tvOut.z = vIn.z * rad;\n"
		   << "\t}\n";
		return ss.str();
	}

	virtual void Precalc() override
	{
		T pq = T(M_PI) / m_Q;
		T pp = T(M_PI) / m_P;
		T spq = std::sin(pq);
		T spp = std::sin(pp);
		m_Shift = std::sin(T(M_PI) * T(0.5) - pq - pp);
		m_Shift = m_Shift / std::sqrt(1 - Sqr(spq) - Sqr(spp));
		m_Scale2 = 1 / std::sqrt(Sqr(sin(T(M_PI) / 2 + pp)) / Sqr(spq) - 1);
		m_Scale2 = m_Scale2 * (std::sin(T(M_PI) / 2 + pp) / spq - 1);
		m_Scale = 1 - m_Shift * m_Shift;
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
		m_Params.push_back(ParamWithName<T>(&m_P, prefix + "hypershift2_p", 3, eParamType::INTEGER_NONZERO));
		m_Params.push_back(ParamWithName<T>(&m_Q, prefix + "hypershift2_q", 7, eParamType::INTEGER_NONZERO));
		m_Params.push_back(ParamWithName<T>(true, &m_Shift,  prefix + "hypershift2_shift"));//Precalc.
		m_Params.push_back(ParamWithName<T>(true, &m_Scale,  prefix + "hypershift2_scale"));
		m_Params.push_back(ParamWithName<T>(true, &m_Scale2, prefix + "hypershift2_scale2"));
	}

private:
	T m_P;
	T m_Q;
	T m_Shift;//Precalc.
	T m_Scale;
	T m_Scale2;
};

MAKEPREPOSTPARVAR(Splits3D, splits3D, SPLITS3D)
MAKEPREPOSTPARVAR(Waves2B, waves2b, WAVES2B)
MAKEPREPOSTPARVAR(JacCn, jac_cn, JAC_CN)
MAKEPREPOSTPARVAR(JacDn, jac_dn, JAC_DN)
MAKEPREPOSTPARVAR(JacSn, jac_sn, JAC_SN)
MAKEPREPOSTPARVAR(PressureWave, pressure_wave, PRESSURE_WAVE)
MAKEPREPOSTVAR(Gamma, gamma, GAMMA)
MAKEPREPOSTPARVAR(PRose3D, pRose3D, PROSE3D)
MAKEPREPOSTPARVAR(LogDB, log_db, LOG_DB)
MAKEPREPOSTPARVAR(CircleSplit, circlesplit, CIRCLESPLIT)
MAKEPREPOSTVAR(Cylinder2, cylinder2, CYLINDER2)
MAKEPREPOSTPARVAR(TileLog, tile_log, TILE_LOG)
MAKEPREPOSTPARVAR(TruchetFill, Truchet_fill, TRUCHET_FILL)
MAKEPREPOSTPARVAR(Waves2Radial, waves2_radial, WAVES2_RADIAL)
MAKEPREPOSTVAR(Panorama1, panorama1, PANORAMA1)
MAKEPREPOSTVAR(Panorama2, panorama2, PANORAMA2)
//MAKEPREPOSTPARVAR(Arcsinh, arcsinh, ARCSINH)
MAKEPREPOSTPARVAR(Helicoid, helicoid, HELICOID)
MAKEPREPOSTPARVAR(Helix, helix, HELIX)
MAKEPREPOSTPARVAR(Sphereblur, sphereblur, SPHEREBLUR)
MAKEPREPOSTPARVAR(Cpow3, cpow3, CPOW3)
MAKEPREPOSTPARVAR(Concentric, concentric, CONCENTRIC)
MAKEPREPOSTPARVAR(Hypercrop, hypercrop, HYPERCROP)
MAKEPREPOSTPARVAR(Hypershift2, hypershift2, HYPERSHIFT2)
}
