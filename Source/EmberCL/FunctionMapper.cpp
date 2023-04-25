#include "EmberCLPch.h"
#include "FunctionMapper.h"

namespace EmberCLns
{
std::unordered_map<string, string> FunctionMapper::s_GlobalMap;

FunctionMapper::FunctionMapper()
{
	if (s_GlobalMap.empty())
	{
		s_GlobalMap["LRint"] =
			"inline real_t LRint(real_t x)\n"
			"{\n"
			"    intPrec temp = (x >= (real_t)0.0 ? (intPrec)(x + (real_t)0.5) : (intPrec)(x - (real_t)0.5));\n"
			"    return (real_t)temp;\n"
			"}\n";
		s_GlobalMap["Round"] =
			"inline real_t Round(real_t r)\n"
			"{\n"
			"	return (r > (real_t)0.0) ? floor(r + (real_t)0.5) : ceil(r - (real_t)0.5);\n"
			"}\n";
		s_GlobalMap["Fract"] =
			"inline real_t Fract(real_t x)\n"
			"{\n"
			"	return x - floor(x);\n"
			"}\n";
		s_GlobalMap["HashShadertoy"] =
			"inline real_t HashShadertoy(real_t x, real_t y, real_t seed)\n"
			"{\n"
			"	return Fract(sin(fma(x, (real_t)12.9898, fma(y, (real_t)78.233, seed))) * (real_t)43758.5453);\n"
			"}\n";
		s_GlobalMap["Sign"] =
			"inline real_t Sign(real_t v)\n"
			"{\n"
			"	return (v < (real_t)0.0) ? (real_t)-1.0 : (v > (real_t)0.0) ? 1 : (real_t)0.0;\n"
			"}\n";
		s_GlobalMap["SignNz"] =
			"inline real_t SignNz(real_t v)\n"
			"{\n"
			"	return (v < (real_t)0.0) ? (real_t)-1.0 : (real_t)1.0;\n"
			"}\n";
		s_GlobalMap["Sqr"] =
			"inline real_t Sqr(real_t v)\n"
			"{\n"
			"	return v * v;\n"
			"}\n";
		s_GlobalMap["SafeSqrt"] =
			"inline real_t SafeSqrt(real_t x)\n"
			"{\n"
			"	if (x <= (real_t)0.0)\n"
			"		return (real_t)0.0;\n"
			"\n"
			"	return sqrt(x);\n"
			"}\n";
		s_GlobalMap["SafeDivInv"] =
			"inline real_t SafeDivInv(real_t q, real_t r)\n"
			"{\n"
			"	if (r < EPS)\n"
			"		return (real_t)1.0 / r;\n"
			"\n"
			"	return q / r;\n"
			"}\n";
		s_GlobalMap["Cube"] =
			"inline real_t Cube(real_t v)\n"
			"{\n"
			"	return v * v * v;\n"
			"}\n";
		s_GlobalMap["Hypot"] =
			"inline real_t Hypot(real_t x, real_t y)\n"
			"{\n"
			"	return sqrt(fma(x, x, SQR(y)));\n"
			"}\n";
		s_GlobalMap["Spread"] =
			"inline real_t Spread(real_t x, real_t y)\n"
			"{\n"
			"	return Hypot(x, y) * ((x) > (real_t)0.0 ? (real_t)1.0 : (real_t)-1.0);\n"
			"}\n";
		s_GlobalMap["Powq4"] =
			"inline real_t Powq4(real_t x, real_t y)\n"
			"{\n"
			"	return pow(fabs(x), y) * SignNz(x);\n"
			"}\n";
		s_GlobalMap["Powq4c"] =
			"inline real_t Powq4c(real_t x, real_t y)\n"
			"{\n"
			"	return y == (real_t)1.0 ? x : Powq4(x, y);\n"
			"}\n";
		s_GlobalMap["Zeps"] =
			"inline real_t Zeps(real_t x)\n"
			"{\n"
			"	return x != (real_t)0.0 ? x : EPS;\n"
			"}\n";
		s_GlobalMap["Lerp"] =
			"inline real_t Lerp(real_t a, real_t b, real_t p)\n"
			"{\n"
			"	return fma(p, (b - a), a);\n"
			"}\n";
		s_GlobalMap["Fabsmod"] =
			"inline real_t Fabsmod(real_t v)\n"
			"{\n"
			"	real_t dummy;\n"
			"\n"
			"	return modf(v, &dummy);\n"
			"}\n";
		s_GlobalMap["Fosc"] =
			"inline real_t Fosc(real_t p, real_t amp, real_t ph)\n"
			"{\n"
			"	return (real_t)0.5 - cos(fma(p, amp, ph)) * (real_t)0.5;\n"
			"}\n";
		s_GlobalMap["Foscn"] =
			"inline real_t Foscn(real_t p, real_t ph)\n"
			"{\n"
			"	return (real_t)0.5 - cos(p + ph) * (real_t)0.5;\n"
			"}\n";
		s_GlobalMap["LogScale"] =
			"inline real_t LogScale(real_t x)\n"
			"{\n"
			"	return x == (real_t)0.0 ? (real_t)0.0 : log((fabs(x) + 1) * M_E) * SignNz(x) / M_E;\n"
			"}\n";
		s_GlobalMap["LogMap"] =
			"inline real_t LogMap(real_t x)\n"
			"{\n"
			"	return x == (real_t)0.0 ? (real_t)0.0 : (M_E + log(x * M_E)) * (real_t)0.25 * SignNz(x);\n"
			"}\n";
		s_GlobalMap["ClampGte"] =
			"inline real_t ClampGte(real_t val, real_t gte)\n"
			"{\n"
			"	return (val < gte) ? gte : val;\n"
			"}\n";
		s_GlobalMap["Swap"] =
			"inline void Swap(real_t* val1, real_t* val2)\n"
			"{\n"
			"	real_t tmp = *val1;\n"
			"	*val1 = *val2;\n"
			"	*val2 = tmp;\n"
			"}\n";
		s_GlobalMap["Modulate"] =
			"inline real_t Modulate(real_t amp, real_t freq, real_t x)\n"
			"{\n"
			"	return amp * cos(x * freq * M_2PI);\n"
			"}\n";
		s_GlobalMap["RealDivComplex"] =
			"inline real2 RealDivComplex(real_t x, real2 a)\n"
			"{\n"
			"	real_t s = x / Zeps(fma(a.x, a.x, a.y * a.y));\n"
			"	return (real2)(a.x * s, -a.y * s);\n"
			"}\n";
		s_GlobalMap["ComplexDivComplex"] =
			"inline real2 ComplexDivComplex(real2 a, real2 b)\n"
			"{\n"
			"	real_t s = (real_t)1.0 / Zeps(fma(b.x, b.x, b.y * b.y));\n"
			"	return (real2)(fma(a.x, b.x, a.y * b.y), fma(a.y, b.x, -(a.x * b.y))) * s;\n"
			"}\n";
		s_GlobalMap["ComplexMultReal"] =
			"inline real2 ComplexMultReal(real2 a, real_t x)\n"
			"{\n"
			"	return (real2)(a.x * x, a.y * x);\n"
			"}\n";
		s_GlobalMap["ComplexMultComplex"] =
			"inline real2 ComplexMultComplex(real2 a, real2 b)\n"
			"{\n"
			"	return (real2)(fma(a.x, b.x, -(a.y * b.y)), fma(a.x, b.y, a.y * b.x));\n"
			"}\n";
		s_GlobalMap["ComplexPlusReal"] =
			"inline real2 ComplexPlusReal(real2 a, real_t x)\n"
			"{\n"
			"	return (real2)(a.x + x, a.y);\n"
			"}\n";
		s_GlobalMap["ComplexPlusComplex"] =
			"inline real2 ComplexPlusComplex(real2 a, real2 b)\n"
			"{\n"
			"	return (real2)(a.x + b.x, a.y + b.y);\n"
			"}\n";
		s_GlobalMap["ComplexMinusReal"] =
			"inline real2 ComplexMinusReal(real2 a, real_t x)\n"
			"{\n"
			"	return (real2)(a.x - x, a.y);\n"
			"}\n";
		s_GlobalMap["ComplexMinusComplex"] =
			"inline real2 ComplexMinusComplex(real2 a, real2 b)\n"
			"{\n"
			"	return (real2)(a.x - b.x, a.y - b.y);\n"
			"}\n";
		s_GlobalMap["ComplexSqrt"] =
			"inline real2 ComplexSqrt(real2 a)\n"
			"{\n"
			"	real_t mag = Hypot(a.x, a.y);\n"
			"	return ComplexMultReal((real2)(sqrt(mag + a.x), Sign(a.y) * sqrt(mag - a.x)), (real_t)0.5 * sqrt((real_t)2.0));\n"
			"}\n";
		s_GlobalMap["ComplexLog"] =
			"inline real2 ComplexLog(real2 a)\n"
			"{\n"
			"	return (real2)((real_t)0.5 * log(fma(a.x, a.x, a.y * a.y)), atan2(a.y, a.x));\n"
			"}\n";
		s_GlobalMap["ComplexExp"] =
			"inline real2 ComplexExp(real2 a)\n"
			"{\n"
			"	return (real2)(cos(a.y), sin(a.y)) * exp(a.x);\n"
			"}\n";
		s_GlobalMap["Hash"] =
			"inline real_t Hash(int a)\n"
			"{\n"
			"	a = (a ^ 61) ^ (a >> 16);\n"
			"	a = a + (a << 3);\n"
			"	a = a ^ (a >> 4);\n"
			"	a = a * 0x27d4eb2d;\n"
			"	a = a ^ (a >> 15);\n"
			"	return (real_t)a / INT_MAX;\n"
			"}\n";
		s_GlobalMap["Vratio"] =
			"inline real_t Vratio(real2* p, real2* q, real2* u)\n"
			"{\n"
			"	real2 pmq = *p - *q;\n"
			"\n"
			"	if (pmq.x == (real_t)0.0 && pmq.y == (real_t)0.0)\n"
			"		return 1.0;\n"
			"\n"
			"	return 2 * (((*u).x - (*q).x) * pmq.x + ((*u).y - (*q).y) * pmq.y) / Zeps(SQR(pmq.x) + SQR(pmq.y));\n"
			"}\n";
		s_GlobalMap["Closest"] =
			"inline int Closest(real2* p, int n, real2* u)\n"
			"{\n"
			"	real_t d2;\n"
			"	real_t d2min = TMAX;\n"
			"	int i, j = 0;\n"
			"\n"
			"	for (i = 0; i < n; i++)\n"
			"	{\n"
			"		real_t pxmx = p[i].x - (*u).x;\n"
			"		d2 = fma(pxmx, pxmx, Sqr(p[i].y - (*u).y));\n"
			"\n"
			"		if (d2 < d2min)\n"
			"		{\n"
			"			d2min = d2;\n"
			"			j = i;\n"
			"		}\n"
			"	}\n"
			"\n"
			"	return j;\n"
			"}\n";
		s_GlobalMap["Voronoi"] =
			"inline real_t Voronoi(real2* p, int n, int q, real2* u)\n"
			"{\n"
			"	real_t ratio;\n"
			"	real_t ratiomax = TLOW;\n"
			"	int i;\n"
			"\n"
			"	for (i = 0; i < n; i++)\n"
			"	{\n"
			"		if (i != q)\n"
			"		{\n"
			"			ratio = Vratio(&p[i], &p[q], u);\n"
			"\n"
			"			if (ratio > ratiomax)\n"
			"				ratiomax = ratio;\n"
			"		}\n"
			"	}\n"
			"\n"
			"	return ratiomax;\n"
			"}\n";
		s_GlobalMap["SimplexNoise3D"] =
			"inline real_t SimplexNoise3D(real4* v, __global real_t* p, __global real_t* grad)\n"
			"{\n"
			"	real4 c[4];\n"
			"	real_t n = 0;\n"
			"	int gi[4];\n"
			"	real_t skewIn = ((*v).x + (*v).y + (*v).z) * (real_t)0.333333;\n"
			"	int i = (int)floor((*v).x + skewIn);\n"
			"	int j = (int)floor((*v).y + skewIn);\n"
			"	int k = (int)floor((*v).z + skewIn);\n"
			"	real_t t = (i + j + k) * (real_t)0.1666666;\n"
			"	real_t x0 = i - t;\n"
			"	real_t y0 = j - t;\n"
			"	real_t z0 = k - t;\n"
			"	c[0].x = (*v).x - x0;\n"
			"	c[0].y = (*v).y - y0;\n"
			"	c[0].z = (*v).z - z0;\n"
			"	int i1, j1, k1;\n"
			"	int i2, j2, k2;\n"
			"	real4 u;\n"
			"\n"
			"	if (c[0].x >= c[0].y)\n"
			"	{\n"
			"		if (c[0].y >= c[0].z)\n"
			"		{\n"
			"			i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;\n"
			"		}\n"
			"		else\n"
			"		{\n"
			"			if (c[0].x >= c[0].z)\n"
			"			{\n"
			"				i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1;\n"
			"			}\n"
			"			else\n"
			"			{\n"
			"				i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1;\n"
			"			}\n"
			"		}\n"
			"	}\n"
			"	else\n"
			"	{\n"
			"		if (c[0].y < c[0].z)\n"
			"		{\n"
			"			i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1;\n"
			"		}\n"
			"		else\n"
			"		{\n"
			"			if (c[0].x < c[0].z)\n"
			"			{\n"
			"				i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1;\n"
			"			}\n"
			"			else\n"
			"			{\n"
			"				i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0;\n"
			"			}\n"
			"		}\n"
			"	}\n"
			"\n"
			"	c[1].x = c[0].x - i1 + (real_t)0.1666666;\n"
			"	c[1].y = c[0].y - j1 + (real_t)0.1666666;\n"
			"	c[1].z = c[0].z - k1 + (real_t)0.1666666;\n"
			"	c[2].x = c[0].x - i2 + 2 * (real_t)0.1666666;\n"
			"	c[2].y = c[0].y - j2 + 2 * (real_t)0.1666666;\n"
			"	c[2].z = c[0].z - k2 + 2 * (real_t)0.1666666;\n"
			"	c[3].x = c[0].x - 1 + 3 * (real_t)0.1666666;\n"
			"	c[3].y = c[0].y - 1 + 3 * (real_t)0.1666666;\n"
			"	c[3].z = c[0].z - 1 + 3 * (real_t)0.1666666;\n"
			"	int ii = i & 0x3ff;\n"
			"	int jj = j & 0x3ff;\n"
			"	int kk = k & 0x3ff;\n"
			"	gi[0] = (int)p[ii + (int)p[jj + (int)p[kk]]];\n"
			"	gi[1] = (int)p[ii + i1 + (int)p[jj + j1 + (int)p[kk + k1]]];\n"
			"	gi[2] = (int)p[ii + i2 + (int)p[jj + j2 + (int)p[kk + k2]]];\n"
			"	gi[3] = (int)p[ii + 1 + (int)p[jj + 1 + (int)p[kk + 1]]];\n"
			"\n"
			"	for (uint corner = 0; corner < 4; corner++)\n"
			"	{\n"
			"		t = 0.6 - Sqr(c[corner].x) - Sqr(c[corner].y) - Sqr(c[corner].z);\n"
			"\n"
			"		if (t > 0)\n"
			"		{\n"
			"			int index = gi[corner] * 3;\n"
			"			u.x = grad[index];\n"
			"			u.y = grad[index + 1];\n"
			"			u.z = grad[index + 2];\n"
			"			t *= t;\n"
			"			n += t * t * (u.x * c[corner].x + u.y * c[corner].y + u.z * c[corner].z);\n"
			"		}\n"
			"	}\n"
			"\n"
			"	return 32.0 * n;\n"
			"}\n";
		s_GlobalMap["PerlinNoise3D"] =
			"inline real_t PerlinNoise3D(real4* v, __global real_t* p, __global real_t* grad, real_t aScale, real_t fScale, int octaves)\n"
			"{\n"
			"	int i;\n"
			"	real_t n = 0.0, a = (real_t)1.0;\n"
			"	real4 u = *v;\n"
			"\n"
			"	for (i = 0; i < octaves; i++)\n"
			"	{\n"
			"		n += SimplexNoise3D(&u, p, grad) / Zeps(a);\n"
			"		a *= aScale;\n"
			"		u.x *= fScale;\n"
			"		u.y *= fScale;\n"
			"		u.x *= fScale;\n"
			"	}\n"
			"\n"
			"	return n;\n"
			"}\n";
		s_GlobalMap["EvalRational"] =
			"inline real_t EvalRational(__global real_t* num, __global real_t* denom, real_t z_, int count)//This function was taken from boost.org.\n"
			"{\n"
			"	real_t z = z_;\n"
			"	real_t s1, s2;\n"
			"\n"
			"	if (z <= 1)\n"
			"	{\n"
			"		s1 = num[count - 1];\n"
			"		s2 = denom[count - 1];\n"
			"\n"
			"		for (int i = count - 2; i >= 0; --i)\n"
			"		{\n"
			"			s1 *= z;\n"
			"			s2 *= z;\n"
			"			s1 += num[i];\n"
			"			s2 += denom[i];\n"
			"		}\n"
			"	}\n"
			"	else\n"
			"	{\n"
			"		z = (real_t)1.0 / z;\n"
			"		s1 = num[0];\n"
			"		s2 = denom[0];\n"
			"\n"
			"		for (unsigned i = 1; i < count; ++i)\n"
			"		{\n"
			"			s1 *= z;\n"
			"			s2 *= z;\n"
			"			s1 += num[i];\n"
			"			s2 += denom[i];\n"
			"		}\n"
			"	}\n"
			"\n"
			"	return s1 / s2;\n"
			"}\n";
		s_GlobalMap["J1"] =
			"inline real_t J1(real_t x, __global real_t* P1, __global real_t* Q1, __global real_t* P2, __global real_t* Q2, __global real_t* PC, __global real_t* QC, __global real_t* PS, __global real_t* QS)//This function was taken from boost.org.\n"
			"{\n"
			"	real_t x1    = (real_t)3.8317059702075123156e+00,\n"
			"		   x2    = (real_t)7.0155866698156187535e+00,\n"
			"		   x11   = (real_t)9.810e+02,\n"
			"		   x12   = (real_t)-3.2527979248768438556e-04,\n"
			"		   x21   = (real_t)1.7960e+03,\n"
			"		   x22   = (real_t)-3.8330184381246462950e-05;\n"
			"	real_t value, factor, r, rc, rs, w;\n"
			"	w = fabs(x);\n"
			"\n"
			"	if (x == (real_t)0.0)\n"
			"	{\n"
			"		return (real_t)0.0;\n"
			"	}\n"
			"\n"
			"	if (w <= (real_t)4.0)\n"
			"	{\n"
			"		real_t y = x * x;\n"
			"		r = EvalRational(P1, Q1, y, 7);\n"
			"		factor = w * (w + x1) * ((w - x11 / (real_t)256.0) - x12);\n"
			"		value = factor * r;\n"
			"	}\n"
			"	else if (w <= (real_t)8.0)\n"
			"	{\n"
			"		real_t y = x * x;\n"
			"		r = EvalRational(P2, Q2, y, 8);\n"
			"		factor = w * (w + x2) * ((w - x21 / (real_t)256.0) - x22);\n"
			"		value = factor * r;\n"
			"	}\n"
			"	else\n"
			"	{\n"
			"		real_t y = (real_t)8.0 / w;\n"
			"		real_t y2 = y * y;\n"
			"		rc = EvalRational(PC, QC, y2, 7);\n"
			"		rs = EvalRational(PS, QS, y2, 7);\n"
			"		factor = 1 / (sqrt(w) * (real_t)1.772453850905516027);//sqrt pi\n"
			"		real_t sx = sin(x);\n"
			"		real_t cx = cos(x);\n"
			"		value = factor * (rc * (sx - cx) + y * rs * (sx + cx));\n"
			"	}\n"
			"\n"
			"	if (x < (real_t)0.0)\n"
			"	{\n"
			"		value *= (real_t)-1.0;\n"
			"	}\n"
			"\n"
			"	return value;\n"
			"}\n";
		s_GlobalMap["JacobiElliptic"] =
			"inline void JacobiElliptic(real_t uu, real_t emmc, real_t* sn, real_t* cn, real_t* dn)\n"
			"{\n"
			"	real_t CA = (real_t)0.0003;\n"
			"	real_t a, b, c, d = (real_t)1.0, em[13], en[13];\n"
			"	int bo;\n"
			"	int l;\n"
			"	int ii;\n"
			"	int i;\n"
			"	real_t emc = emmc;\n"
			"	real_t u = uu;\n"
			"\n"
			"	if (emc != 0)\n"
			"	{\n"
			"		bo = 0;\n"
			"\n"
			"		if (emc < 0)\n"
			"			bo = 1;\n"
			"\n"
			"		if (bo != 0)\n"
			"		{\n"
			"			d = (real_t)1.0 - emc;\n"
			"			emc = -emc / d;\n"
			"			d = sqrt(d);\n"
			"			u = d * u;\n"
			"		}\n"
			"\n"
			"		a = (real_t)1.0;\n"
			"		*dn = (real_t)1.0;\n"
			"\n"
			"		for (i = 0; i < 8; i++)\n"
			"		{\n"
			"			l = i;\n"
			"			em[i] = a;\n"
			"			emc = sqrt(emc);\n"
			"			en[i] = emc;\n"
			"			c = (real_t)0.5 * (a + emc);\n"
			"\n"
			"			if (fabs(a - emc) <= CA * a)\n"
			"				break;\n"
			"\n"
			"			emc = a * emc;\n"
			"			a = c;\n"
			"		}\n"
			"\n"
			"		u = c * u;\n"
			"		*sn = sincos(u, cn);\n"
			"\n"
			"		if (*sn != (real_t)0.0)\n"
			"		{\n"
			"			a = *cn / *sn;\n"
			"			c = a * c;\n"
			"\n"
			"			for (ii = l; ii >= 0; --ii)\n"
			"			{\n"
			"				b = em[ii];\n"
			"				a = c * a;\n"
			"				c = *dn * c;\n"
			"				*dn = (en[ii] + a) / (b + a);\n"
			"				a = c / b;\n"
			"			}\n"
			"\n"
			"			a = 1 / sqrt(fma(c, c, (real_t)(1.0)));\n"
			"\n"
			"			if (*sn < (real_t)0.0)\n"
			"				*sn = -a;\n"
			"			else\n"
			"				*sn = a;\n"
			"\n"
			"			*cn = c * *sn;\n"
			"		}\n"
			"\n"
			"		if (bo != 0)\n"
			"		{\n"
			"			a = *dn;\n"
			"			*dn = *cn;\n"
			"			*cn = a;\n"
			"			*sn = *sn / d;\n"
			"		}\n"
			"	}\n"
			"	else\n"
			"	{\n"
			"		*cn = 1 / cosh(u);\n"
			"		*dn = *cn;\n"
			"		*sn = tanh(u);\n"
			"	}\n"
			"}\n";
	}
}

/// <summary>
/// Get a pointer to the text of the global function whose name is the passed in string.
/// </summary>
/// <param name="func">The function name to retrieve</param>
/// <returns>A pointer to the function body string if found, else nullptr.</returns>
const string* FunctionMapper::GetGlobalFunc(const string& func)
{
	const auto& text = s_GlobalMap.find(func);

	if (text != s_GlobalMap.end())
		return &text->second;
	else
		return nullptr;
}

/// <summary>
/// Get a copy of the function map.
/// This is useful only for debugging/testing.
/// </summary>
/// <returns>A copy of the function map</returns>
const std::unordered_map<string, string> FunctionMapper::GetGlobalMapCopy()
{
	return s_GlobalMap;
}
}
