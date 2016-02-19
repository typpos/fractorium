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
		string x = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string y = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string z = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\tif (vIn.x >= 0)\n"
		   << "\t\t	vOut.x = xform->m_VariationWeights[" << varIndex << "] * (vIn.x + " << x << ");\n"
		   << "\t\telse\n"
		   << "\t\t	vOut.x = xform->m_VariationWeights[" << varIndex << "] * (vIn.x - " << x << ");\n"
		   << "\n"
		   << "\t\tif (vIn.y >= 0)\n"
		   << "\t\t	vOut.y = xform->m_VariationWeights[" << varIndex << "] * (vIn.y + " << y << ");\n"
		   << "\t\telse\n"
		   << "\t\t	vOut.y = xform->m_VariationWeights[" << varIndex << "] * (vIn.y - " << y << ");\n"
		   << "\n"
		   << "\t\tif (vIn.z >= 0)\n"
		   << "\t\t	vOut.z = xform->m_VariationWeights[" << varIndex << "] * (vIn.z + " << z << ");\n"
		   << "\t\telse\n"
		   << "\t\t	vOut.z = xform->m_VariationWeights[" << varIndex << "] * (vIn.z - " << z << ");\n"
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
		CsX = SafeDivInv(m_Unity, (m_Unity + Sqr(helper.In.x)));
		CsX = CsX * m_Six + m_Scaleinfx;
		CsY = SafeDivInv(m_Unity, (m_Unity + Sqr(helper.In.y)));
		CsY = CsY * m_Siy + m_Scaleinfy;

		if (m_Pwx >= 0 && m_Pwx < 1e-4)
		{
			m_VarFuncs->JacobiElliptic(helper.In.y * m_Freqx, m_Jacok, jcbSn, jcbCn, jcbDn);
			helper.Out.x = m_Weight * (helper.In.x + CsX * jcbSn);
		}
		else if (m_Pwx < 0 && m_Pwx > -1e-4)
#ifdef _WIN32
			helper.Out.x = m_Weight * (helper.In.x + CsX * T(_j1(helper.In.y * m_Freqx)));//This is not implemented in OpenCL.

#else
			helper.Out.x = m_Weight * (helper.In.x + CsX * T(j1(helper.In.y * m_Freqx)));//This is not implemented in OpenCL.
#endif
		else
			helper.Out.x = m_Weight * (helper.In.x + CsX * std::sin(SignNz(helper.In.y) * std::pow(Zeps(std::abs(helper.In.y)), m_Pwx) * m_Freqx));

		if (m_Pwy >= 0 && m_Pwy < 1e-4)
		{
			m_VarFuncs->JacobiElliptic(helper.In.x * m_Freqy, m_Jacok, jcbSn, jcbCn, jcbDn);
			helper.Out.y = m_Weight * (helper.In.y + CsY * jcbSn);
		}
		else if (m_Pwy < 0 && m_Pwy > -1e-4)
#ifdef _WIN32
			helper.Out.y = m_Weight * (helper.In.y + CsY * T(_j1(helper.In.x * m_Freqy)));

#else
			helper.Out.y = m_Weight * (helper.In.y + CsY * T(j1(helper.In.x * m_Freqy)));
#endif
		else
			helper.Out.y = m_Weight * (helper.In.y + CsY * std::sin(SignNz(helper.In.x) * std::pow(Zeps(std::abs(helper.In.x)), m_Pwy) * m_Freqy));

		helper.Out.z = DefaultZ(helper);
	}

	virtual string OpenCLString() const override
	{
		ostringstream ss, ss2;
		intmax_t i = 0, varIndex = IndexInXform();
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		string freqx     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string freqy     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string pwx       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string pwy       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scalex    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scaleinfx = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scaley    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string scaleinfy = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string unity     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string jacok     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string six       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;//Precalc.
		string siy       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
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
		   << "\t\t	vOut.x = xform->m_VariationWeights[" << varIndex << "] * (vIn.x + CsX * jcbSn);\n"
		   << "\t\t}\n"
		   //<< "\t\telse if (" << pwx << " < 0 && " << pwx << " > -1e-4)\n"
		   //<< "\t\t	vOut.x = xform->m_VariationWeights[" << varIndex << "] * (vIn.x + CsX * _j1(vIn.y * " << freqx << "));\n"//This is not implemented in OpenCL.
		   << "\t\telse\n"
		   << "\t\t	vOut.x = xform->m_VariationWeights[" << varIndex << "] * (vIn.x + CsX * sin(SignNz(vIn.y) * pow(Zeps(fabs(vIn.y)), " << pwx << ") * " << freqx << "));\n"
		   << "\n"
		   << "\t\tif (" << pwy << " >= 0 && " << pwy << " < 1e-4)\n"
		   << "\t\t{\n"
		   << "\t\t	JacobiElliptic(vIn.x * " << freqy << ", " << jacok << ", &jcbSn, &jcbCn, &jcbDn);\n"
		   << "\t\t	vOut.y = xform->m_VariationWeights[" << varIndex << "] * (vIn.y + CsY * jcbSn);\n"
		   << "\t\t}\n"
		   //<< "\t\telse if (" << pwy << " < 0 && " << pwy << " > -1e-4)\n"
		   //<< "\t\t	vOut.y = xform->m_VariationWeights[" << varIndex << "] * (vIn.y + CsY * _j1(vIn.x * " << freqy << "));\n"//This is not implemented in OpenCL.
		   << "\t\telse\n"
		   << "\t\t	vOut.y = xform->m_VariationWeights[" << varIndex << "] * (vIn.y + CsY * sin(SignNz(vIn.x) * pow(Zeps(fabs(vIn.x)), " << pwy << ") * " << freqy << "));\n"
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
		m_VarFuncs = VarFuncs<T>::Instance();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_Freqx,     prefix + "waves2b_freqx", 2));
		m_Params.push_back(ParamWithName<T>(&m_Freqy,     prefix + "waves2b_freqy", 2));
		m_Params.push_back(ParamWithName<T>(&m_Pwx,       prefix + "waves2b_pwx", 1, eParamType::REAL, -10, 10));
		m_Params.push_back(ParamWithName<T>(&m_Pwy,       prefix + "waves2b_pwy", 1, eParamType::REAL, -10, 10));
		m_Params.push_back(ParamWithName<T>(&m_Scalex,    prefix + "waves2b_scalex", 1));
		m_Params.push_back(ParamWithName<T>(&m_Scaleinfx, prefix + "waves2b_scaleinfx", 1));
		m_Params.push_back(ParamWithName<T>(&m_Scaley,    prefix + "waves2b_scaley", 1));
		m_Params.push_back(ParamWithName<T>(&m_Scaleinfy, prefix + "waves2b_scaleinfy", 1));
		m_Params.push_back(ParamWithName<T>(&m_Unity,     prefix + "waves2b_unity", 1));
		m_Params.push_back(ParamWithName<T>(&m_Jacok,     prefix + "waves2b_jacok", T(0.25), eParamType::REAL, -1, 1));
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
	shared_ptr<VarFuncs<T>> m_VarFuncs;
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
		m_VarFuncs->JacobiElliptic(helper.In.x, m_K, snx, cnx, dnx);
		m_VarFuncs->JacobiElliptic(helper.In.y, 1 - m_K, sny, cny, dny);
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
		   << "\t\tdenom = xform->m_VariationWeights[" << varIndex << "] / Zeps(denom);\n"
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
		m_VarFuncs = VarFuncs<T>::Instance();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_K, prefix + "jac_cn_k", T(0.5), eParamType::REAL, -1, 1));
	}

private:
	T m_K;
	shared_ptr<VarFuncs<T>> m_VarFuncs;
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
		m_VarFuncs->JacobiElliptic(helper.In.x, m_K, snx, cnx, dnx);
		m_VarFuncs->JacobiElliptic(helper.In.y, 1 - m_K, sny, cny, dny);
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
		   << "\t\tdenom = xform->m_VariationWeights[" << varIndex << "] / Zeps(denom);\n"
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
		m_VarFuncs = VarFuncs<T>::Instance();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_K, prefix + "jac_dn_k", T(0.5), eParamType::REAL, -1, 1));
	}

private:
	T m_K;
	shared_ptr<VarFuncs<T>> m_VarFuncs;
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
		m_VarFuncs->JacobiElliptic(helper.In.x, m_K, snx, cnx, dnx);
		m_VarFuncs->JacobiElliptic(helper.In.y, 1 - m_K, sny, cny, dny);
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
		   << "\t\tdenom = xform->m_VariationWeights[" << varIndex << "] / Zeps(denom);\n"
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
		m_VarFuncs = VarFuncs<T>::Instance();
		m_Params.clear();
		m_Params.push_back(ParamWithName<T>(&m_K, prefix + "jac_sn_k", T(0.5), eParamType::REAL, -1, 1));
	}

private:
	T m_K;
	shared_ptr<VarFuncs<T>> m_VarFuncs;
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
		string x = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string y = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		ss << "\t{\n"
		   << "\t\tvOut.x = xform->m_VariationWeights[" << varIndex << "] * (vIn.x + (1 / Zeps(" << x << " * M_2PI)) * sin(" << x << " * M_2PI * vIn.x));\n"
		   << "\t\tvOut.y = xform->m_VariationWeights[" << varIndex << "] * (vIn.y + (1 / Zeps(" << y << " * M_2PI)) * sin(" << y << " * M_2PI * vIn.y));\n"
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
		ss2 << "_" << XformIndexInEmber() << "]";
		string index = ss2.str();
		ss << "\t{\n"
		   << "\t\tvOut.x = xform->m_VariationWeights[" << varIndex << "] * lgamma(precalcSqrtSumSquares);\n"
		   << "\t\tvOut.y = xform->m_VariationWeights[" << varIndex << "] * precalcAtanyx;\n"
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
			wag  = std::sin(curve1 * T(M_PI) * m_AbsOptSc) + m_Wagsc * T(0.4) * rad + m_Crvsc * T(0.5) * (std::sin(curve2 * T(M_PI)));
			wag3 = std::sin(curve4 * T(M_PI) * m_AbsOptSc) + m_Wagsc * SQR(rad) * T(0.4) + m_Crvsc * T(0.5) * (std::cos(curve3 * T(M_PI)));
		}
		else
		{
			wag  = std::sin(curve1 * T(M_PI) * m_AbsOptSc) + m_Wagsc * T(0.4) * rad + m_Crvsc * T(0.5) * (std::cos(curve3 * T(M_PI)));
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
		string l          = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string k          = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string c          = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string z1         = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string z2         = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string refSc      = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string opt        = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string optSc      = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string opt3       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string transp     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string dist       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string wagsc      = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string crvsc      = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string f          = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string wigsc      = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string offset     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string cycle      = "parVars[" + ToUpper(m_Params[i++].Name()) + index;//Precalc.
		string optDir     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string petalsSign = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string numPetals  = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string absOptSc   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string smooth12   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string smooth3    = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string antiOpt1   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string ghost      = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string freq       = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string wigScale   = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
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
		   << "\t\t	wag = sin(curve1 * M_PI * " << absOptSc << ") + " << wagsc << " * 0.4 * rad + " << crvsc << " * 0.5 * (sin(curveTwo * M_PI));\n"
		   << "\t\t	wag3 = sin(curve4 * M_PI * " << absOptSc << ") + " << wagsc << " * SQR(rad) * 0.4 + " << crvsc << " * 0.5 * (cos(curve3 * M_PI));\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	wag = sin(curve1 * M_PI * " << absOptSc << ") + " << wagsc << " * 0.4 * rad + " << crvsc << " * 0.5 * (cos(curve3 * M_PI));\n"
		   << "\t\t	wag3 = sin(curve4 * M_PI * " << absOptSc << ") + " << wagsc << " * SQR(rad) * 0.4 + " << crvsc << " * 0.5 * (sin(curveTwo * M_PI));\n"
		   << "\t\t}\n"
		   << "\n"
		   << "\t\twag2 = sin(curveTwo * M_PI * " << absOptSc << ") + " << wagsc << " * 0.4 * rad + " << crvsc << " * 0.5 * (cos(curve3 * M_PI));\n"
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
		   << "\t\t		vOut.x = xform->m_VariationWeights[" << varIndex << "] * 0.5 * " << refSc << " * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * cth;\n"
		   << "\t\t		vOut.y = xform->m_VariationWeights[" << varIndex << "] * 0.5 * " << refSc << " * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * sth;\n"
		   << "\t\t		vOut.z = xform->m_VariationWeights[" << varIndex << "] * -0.5 * ((" << z2 << " * waggle + Sqr(rad * 0.5) * sin(wig) * " << wigScale << ") + " << dist << ");\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = xform->m_VariationWeights[" << varIndex << "] * 0.5 * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * cth;\n"
		   << "\t\t		vOut.y = xform->m_VariationWeights[" << varIndex << "] * 0.5 * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * sth;\n"
		   << "\t\t		vOut.z = xform->m_VariationWeights[" << varIndex << "] * 0.5 * ((" << z1 << " * waggle + Sqr(rad * 0.5) * sin(wig) * " << wigScale << ") + " << dist << ");\n"
		   << "\t\t	}\n"
		   << "\t\t}\n"
		   << "\t\telse\n"
		   << "\t\t{\n"
		   << "\t\t	if (posNeg < 0)\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = xform->m_VariationWeights[" << varIndex << "] * 0.5 * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * cth;\n"
		   << "\t\t		vOut.y = xform->m_VariationWeights[" << varIndex << "] * 0.5 * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * sth;\n"
		   << "\t\t		vOut.z = xform->m_VariationWeights[" << varIndex << "] * 0.5 * ((" << z1 << " * waggle + Sqr(rad * 0.5) * sin(wig) * " << wigScale << ") + " << dist << ");\n"
		   << "\t\t	}\n"
		   << "\t\t	else\n"
		   << "\t\t	{\n"
		   << "\t\t		vOut.x = xform->m_VariationWeights[" << varIndex << "] * 0.5 * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * cth;\n"
		   << "\t\t		vOut.y = xform->m_VariationWeights[" << varIndex << "] * 0.5 * (" << l << " * cos(" << numPetals << " * th + " << c << ")) * sth;\n"
		   << "\t\t		vOut.z = xform->m_VariationWeights[" << varIndex << "] * 0.5 * ((" << z1 << " * waggle + Sqr(rad * 0.5) * sin(wig) * " << wigScale << ") + " << dist << ");\n"
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
		m_Params.push_back(ParamWithName<T>(&m_K,      prefix + "pRose3D_k", 3));
		m_Params.push_back(ParamWithName<T>(&m_C,      prefix + "pRose3D_c"));
		m_Params.push_back(ParamWithName<T>(&m_Z1,     prefix + "pRose3D_z1", 1));
		m_Params.push_back(ParamWithName<T>(&m_Z2,     prefix + "pRose3D_z2", 1));
		m_Params.push_back(ParamWithName<T>(&m_RefSc,  prefix + "pRose3D_refSc", 1));
		m_Params.push_back(ParamWithName<T>(&m_Opt,    prefix + "pRose3D_opt", 1));
		m_Params.push_back(ParamWithName<T>(&m_OptSc,  prefix + "pRose3D_optSc", 1));
		m_Params.push_back(ParamWithName<T>(&m_Opt3,   prefix + "pRose3D_opt3"));
		m_Params.push_back(ParamWithName<T>(&m_Transp, prefix + "pRose3D_transp", T(0.5)));
		m_Params.push_back(ParamWithName<T>(&m_Dist,   prefix + "pRose3D_dist", 1));
		m_Params.push_back(ParamWithName<T>(&m_Wagsc,  prefix + "pRose3D_wagsc", 0));
		m_Params.push_back(ParamWithName<T>(&m_Crvsc,  prefix + "pRose3D_crvsc", 0));
		m_Params.push_back(ParamWithName<T>(&m_F,      prefix + "pRose3D_f", 3));
		m_Params.push_back(ParamWithName<T>(&m_Wigsc,  prefix + "pRose3D_wigsc"));
		m_Params.push_back(ParamWithName<T>(&m_Offset, prefix + "pRose3D_offset"));
		m_Params.push_back(ParamWithName<T>(true, &m_Cycle,      prefix + "pRose3D_cycle"));
		m_Params.push_back(ParamWithName<T>(true, &m_OptDir,     prefix + "pRose3D_opt_dir"));
		m_Params.push_back(ParamWithName<T>(true, &m_PetalsSign, prefix + "pRose3D_petals_sign"));
		m_Params.push_back(ParamWithName<T>(true, &m_NumPetals,  prefix + "pRose3D_num_petals"));
		m_Params.push_back(ParamWithName<T>(true, &m_AbsOptSc,   prefix + "pRose3D_abs_optSc"));
		m_Params.push_back(ParamWithName<T>(true, &m_Smooth12,   prefix + "pRose3D_smooth12"));
		m_Params.push_back(ParamWithName<T>(true, &m_Smooth3,    prefix + "pRose3D_smooth3"));
		m_Params.push_back(ParamWithName<T>(true, &m_AntiOpt1,   prefix + "pRose3D_anti_opt1"));
		m_Params.push_back(ParamWithName<T>(true, &m_Ghost,      prefix + "pRose3D_ghost"));
		m_Params.push_back(ParamWithName<T>(true, &m_Freq,       prefix + "pRose3D_freq"));
		m_Params.push_back(ParamWithName<T>(true, &m_WigScale,   prefix + "pRose3D_wig_scale"));
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
		string base      = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string fixPeriod = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string denom     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
		string fixPe     = "parVars[" + ToUpper(m_Params[i++].Name()) + index;
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
		   << "\t\tvOut.y = xform->m_VariationWeights[" << varIndex << "] * (precalcAtanyx + atanPeriod);\n"
		   << "\t\tvOut.z = xform->m_VariationWeights[" << varIndex << "] * vIn.z;\n"
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
		m_Params.push_back(ParamWithName<T>(&m_Base,        prefix + "log_db_base", 1));
		m_Params.push_back(ParamWithName<T>(&m_FixPeriod,   prefix + "log_db_fix_period", 1));
		m_Params.push_back(ParamWithName<T>(true, &m_Denom, prefix + "log_db_denom"));
		m_Params.push_back(ParamWithName<T>(true, &m_FixPe, prefix + "log_db_fix_pe"));
	}
private:
	T m_Base;
	T m_FixPeriod;
	T m_Denom;
	T m_FixPe;
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
}
