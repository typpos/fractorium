#include "EmberCLPch.h"
#include "FinalAccumOpenCLKernelCreator.h"

namespace EmberCLns
{
/// <summary>
/// Constructor that creates all kernel strings.
/// The caller will access these strings through the accessor functions.
/// </summary>
FinalAccumOpenCLKernelCreator::FinalAccumOpenCLKernelCreator(bool doublePrecision)
{
	m_DoublePrecision = doublePrecision;
	m_GammaCorrectionWithoutAlphaCalcKernel                   = CreateGammaCorrectionKernelString();
	m_FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumKernel = CreateFinalAccumKernelString(true);
	m_FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumKernel  = CreateFinalAccumKernelString(false);
}

/// <summary>
/// Kernel source and entry point properties, getters only.
/// </summary>

const string& FinalAccumOpenCLKernelCreator::FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumKernel()     const { return m_FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumKernel;     }
const string& FinalAccumOpenCLKernelCreator::FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumEntryPoint() const { return m_FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumEntryPoint; }

const string& FinalAccumOpenCLKernelCreator::FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumKernel()     const { return m_FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumKernel;     }
const string& FinalAccumOpenCLKernelCreator::FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumEntryPoint() const { return m_FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumEntryPoint; }

const string& FinalAccumOpenCLKernelCreator::GammaCorrectionEntryPoint() const { return m_GammaCorrectionWithoutAlphaCalcEntryPoint; }
const string& FinalAccumOpenCLKernelCreator::GammaCorrectionKernel() const { return m_GammaCorrectionWithoutAlphaCalcKernel; }

/// <summary>
/// Get the final accumulation entry point.
/// </summary>
/// <param name="earlyClip">True if early clip is desired, else false.</param>
/// <returns>The name of the final accumulation entry point kernel function</returns>
const string& FinalAccumOpenCLKernelCreator::FinalAccumEntryPoint(bool earlyClip) const
{
	if (earlyClip)
		return FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumEntryPoint();
	else
		return FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumEntryPoint();
}

/// <summary>
/// Get the final accumulation kernel string.
/// </summary>
/// <param name="earlyClip">True if early clip is desired, else false.</param>
/// <returns>The final accumulation kernel string</returns>
const string& FinalAccumOpenCLKernelCreator::FinalAccumKernel(bool earlyClip) const
{
	if (earlyClip)
		return FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumKernel();
	else
		return FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumKernel();
}

/// <summary>
/// Create the final accumulation kernel string
/// </summary>
/// <param name="earlyClip">True if early clip is desired, else false.</param>
/// <returns>The final accumulation kernel string</returns>
string FinalAccumOpenCLKernelCreator::CreateFinalAccumKernelString(bool earlyClip)
{
	ostringstream os;
	os <<
	   ConstantDefinesString(m_DoublePrecision) <<
	   UnionCLStructString <<
	   RgbToHsvFunctionString <<
	   HsvToRgbFunctionString <<
	   CalcAlphaFunctionString <<
	   CurveAdjustFunctionString <<
	   SpatialFilterCLStructString;

	if (earlyClip)
	{
		os << "__kernel void " << m_FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumEntryPoint << "(\n";
	}
	else
	{
		os <<
		   CreateCalcNewRgbFunctionString(false) <<
		   CreateGammaCorrectionFunctionString(false, true) <<
		   "__kernel void " << m_FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumEntryPoint << "(\n";
	}

	os <<
	   "	const __global real4reals_bucket* accumulator,\n"
	   "	__write_only image2d_t pixels,\n"
	   "	__constant SpatialFilterCL* spatialFilter,\n"
	   "	__constant real_bucket_t* filterCoefs,\n"
	   "	__global real4reals_bucket* csa,\n"
	   "	const uint doCurves\n"
	   "\t)\n"
	   "{\n"
	   "\n"
	   "	if ((GLOBAL_ID_Y >= spatialFilter->m_FinalRasH) || (GLOBAL_ID_X >= spatialFilter->m_FinalRasW))\n"
	   "		return;\n"
	   "\n"
	   "	uint accumX = spatialFilter->m_DensityFilterOffset + (GLOBAL_ID_X * spatialFilter->m_Supersample);\n"
	   "	uint accumY = spatialFilter->m_DensityFilterOffset + (GLOBAL_ID_Y * spatialFilter->m_Supersample);\n"
	   "    uint clampedFilterH = min((uint)spatialFilter->m_FilterWidth, spatialFilter->m_SuperRasH - accumY);"
	   "    uint clampedFilterW = min((uint)spatialFilter->m_FilterWidth, spatialFilter->m_SuperRasW - accumX);"
	   "	int2 finalCoord;\n"
	   "	finalCoord.x = GLOBAL_ID_X;\n"
	   "	finalCoord.y = (int)((spatialFilter->m_YAxisUp == 1) ? ((spatialFilter->m_FinalRasH - GLOBAL_ID_Y) - 1) : GLOBAL_ID_Y);\n"
	   "	float4floats finalColor;\n"
	   "	int ii, jj;\n"
	   "	uint filterKRowIndex;\n"
	   "	const __global real4reals_bucket* accumBucket;\n"
	   "	real4reals_bucket newBucket;\n"
	   "	newBucket.m_Real4 = 0;\n"
	   "\n"
	   "	for (jj = 0; jj < clampedFilterH; jj++)\n"
	   "	{\n"
	   "		filterKRowIndex = jj * spatialFilter->m_FilterWidth;\n"//Use the full, non-clamped width to get the filter value.
	   "\n"
	   "		for (ii = 0; ii < clampedFilterW; ii++)\n"
	   "		{\n"
	   "			real_bucket_t k = filterCoefs[filterKRowIndex + ii];\n"
	   "\n"
	   "			accumBucket = accumulator + ((accumY + jj) * spatialFilter->m_SuperRasW) + (accumX + ii);\n"
	   "			newBucket.m_Real4 += (k * accumBucket->m_Real4);\n"
	   "		}\n"
	   "	}\n"
	   "\n";

	if (earlyClip)//If early clip, simply assign values directly to the temp float4 since they've been gamma corrected already, then write it straight to the output image below.
	{
		os <<
		   "	finalColor.m_Float4.x = (float)newBucket.m_Real4.x;\n"//CPU side clamps, skip here because write_imagef() does the clamping for us.
		   "	finalColor.m_Float4.y = (float)newBucket.m_Real4.y;\n"
		   "	finalColor.m_Float4.z = (float)newBucket.m_Real4.z;\n"
		   "	finalColor.m_Float4.w = (float)newBucket.m_Real4.w;\n";
	}
	else
	{
		//Late clip, so must gamma correct from the temp newBucket to temp finalColor float4.
		if (m_DoublePrecision)
		{
			os <<
			   "	real4reals_bucket realFinal;\n"
			   "\n"
			   "	GammaCorrectionFloats(&newBucket, &(spatialFilter->m_Background[0]), spatialFilter->m_Gamma, spatialFilter->m_LinRange, spatialFilter->m_Vibrancy, spatialFilter->m_HighlightPower, &(realFinal.m_Reals[0]));\n"
			   "	finalColor.m_Float4.x = (float)realFinal.m_Real4.x;\n"
			   "	finalColor.m_Float4.y = (float)realFinal.m_Real4.y;\n"
			   "	finalColor.m_Float4.z = (float)realFinal.m_Real4.z;\n"
			   "	finalColor.m_Float4.w = (float)realFinal.m_Real4.w;\n"
			   ;
		}
		else
		{
			os <<
			   "	GammaCorrectionFloats(&newBucket, &(spatialFilter->m_Background[0]), spatialFilter->m_Gamma, spatialFilter->m_LinRange, spatialFilter->m_Vibrancy, spatialFilter->m_HighlightPower, &(finalColor.m_Floats[0]));\n";
		}
	}

	os <<
	   "\n"
	   "	if (doCurves)\n"
	   "	{\n"
	   "		CurveAdjust(csa, &(finalColor.m_Floats[0]), 1);\n"
	   "		CurveAdjust(csa, &(finalColor.m_Floats[1]), 2);\n"
	   "		CurveAdjust(csa, &(finalColor.m_Floats[2]), 3);\n"
	   "	}\n"
	   "\n"
	   "	write_imagef(pixels, finalCoord, finalColor.m_Float4);\n"//Use write_imagef instead of write_imageui because only the former works when sharing with an OpenGL texture.
	   "	barrier(CLK_GLOBAL_MEM_FENCE);\n"//Required, or else page tearing will occur during interactive rendering.
	   "}\n"
	   ;
	return os.str();
}

/// <summary>
/// Creates the gamma correction function string.
/// This is not a full kernel, just a function that is used in the kernels.
/// </summary>
/// <param name="globalBucket">True if writing to a global buffer (early clip), else false (late clip).</param>
/// <param name="finalOut">True if writing to global buffer (late clip), else false (early clip).</param>
/// <returns>The gamma correction function string</returns>
string FinalAccumOpenCLKernelCreator::CreateGammaCorrectionFunctionString(bool globalBucket, bool finalOut)
{
	ostringstream os;
	string dataType;
	string unionMember;
	dataType = "real_bucket_t";
	//Use real_t for all cases, early clip and final accum.
	os << "void GammaCorrectionFloats(" << (globalBucket ? "__global " : "") << "real4reals_bucket* bucket, __constant real_bucket_t* background, real_bucket_t g, real_bucket_t linRange, real_bucket_t vibrancy, real_bucket_t highlightPower, " << (finalOut ? "" : "__global") << " real_bucket_t* correctedChannels)\n";
	os << "{\n"
	   << "	real_bucket_t alpha, ls, tmp, a;\n"
	   << "	real4reals_bucket newRgb;\n"
	   << "\n"
	   << "	if (bucket->m_Reals[3] <= 0)\n"
	   << "	{\n"
	   << "		alpha = 0;\n"
	   << "		ls = 0;\n"
	   << "	}\n"
	   << "	else\n"
	   << "	{\n"
	   << "		tmp = bucket->m_Reals[3];\n"
	   << "		alpha = CalcAlpha(tmp, g, linRange);\n"
	   << "		ls = vibrancy * alpha / tmp;\n"
	   << "		alpha = clamp(alpha, (real_bucket_t)0.0, (real_bucket_t)1.0);\n"
	   << "	}\n"
	   << "\n"
	   << "	CalcNewRgb(bucket, ls, highlightPower, &newRgb);\n"
	   << "\n"
	   << "	for (uint rgbi = 0; rgbi < 3; rgbi++)\n"
	   << "	{\n"
	   << "		a = newRgb.m_Reals[rgbi] + ((1.0 - vibrancy) * pow(fabs(bucket->m_Reals[rgbi]), g));\n"
	   << "		a += ((1.0 - alpha) * background[rgbi]);\n"
	   << "		correctedChannels[rgbi] = (" << dataType << ")clamp(a, (real_bucket_t)0.0, (real_bucket_t)1.0);\n"
	   << "	}\n"
	   << "\n"
	   << "	correctedChannels[3] = (" << dataType << ")alpha;\n"
	   << "}\n"
	   << "\n";
	return os.str();
}

/// <summary>
/// OpenCL equivalent of Palette::CalcNewRgb().
/// </summary>
/// <param name="globalBucket">True if writing the corrected value to a global buffer (early clip), else false (late clip).</param>
/// <returns>The CalcNewRgb function string</returns>
string FinalAccumOpenCLKernelCreator::CreateCalcNewRgbFunctionString(bool globalBucket)
{
	ostringstream os;
	os <<
	   "static void CalcNewRgb(" << (globalBucket ? "__global " : "") << "real4reals_bucket* oldRgb, real_bucket_t ls, real_bucket_t highPow, real4reals_bucket* newRgb)\n"
	   "{\n"
	   "	int rgbi;\n"
	   "	real_bucket_t lsratio;\n"
	   "	real4reals_bucket newHsv;\n"
	   "	real_bucket_t maxa, maxc, newls;\n"
	   "	real_bucket_t adjhlp;\n"
	   "\n"
	   "	if (ls == 0 || (oldRgb->m_Real4.x == 0 && oldRgb->m_Real4.y == 0 && oldRgb->m_Real4.z == 0))\n"//Can't do a vector compare to zero.
	   "	{\n"
	   "		newRgb->m_Real4 = 0;\n"
	   "		return;\n"
	   "	}\n"
	   "\n"
	   //Identify the most saturated channel.
	   "	maxc = max(max(oldRgb->m_Reals[0], oldRgb->m_Reals[1]), oldRgb->m_Reals[2]);\n"
	   "	maxa = ls * maxc;\n"
	   "	newls = 1 / maxc;\n"
	   "\n"
	   //If a channel is saturated and highlight power is non-negative
	   //modify the color to prevent hue shift.
	   "	if (maxa > 1 && highPow >= 0)\n"
	   "	{\n"
	   "		lsratio = pow(newls / ls, highPow);\n"
	   "\n"
	   //Calculate the max-value color (ranged 0 - 1).
	   "		for (rgbi = 0; rgbi < 3; rgbi++)\n"
	   "			newRgb->m_Reals[rgbi] = newls * oldRgb->m_Reals[rgbi];\n"
	   "\n"
	   //Reduce saturation by the lsratio.
	   "		RgbToHsv(&(newRgb->m_Real4), &(newHsv.m_Real4));\n"
	   "		newHsv.m_Real4.y *= lsratio;\n"
	   "		HsvToRgb(&(newHsv.m_Real4), &(newRgb->m_Real4));\n"
	   "	}\n"
	   "	else\n"
	   "	{\n"
	   "		adjhlp = -highPow;\n"
	   "\n"
	   "		if (adjhlp > 1)\n"
	   "			adjhlp = 1;\n"
	   "\n"
	   "		if (maxa <= 1)\n"
	   "			adjhlp = 1;\n"
	   "\n"
	   //Calculate the max-value color (ranged 0 - 1) interpolated with the old behavior.
	   "		for (rgbi = 0; rgbi < 3; rgbi++)\n"//Unrolling, caching and vectorizing makes no difference.
	   "			newRgb->m_Reals[rgbi] = ((1.0 - adjhlp) * newls + adjhlp * ls) * oldRgb->m_Reals[rgbi];\n"
	   "	}\n"
	   "}\n"
	   "\n";
	return os.str();
}

/// <summary>
/// Create the gamma correction kernel string used for early clipping.
/// </summary>
/// <returns>The gamma correction kernel string used for early clipping</returns>
string FinalAccumOpenCLKernelCreator::CreateGammaCorrectionKernelString()
{
	ostringstream os;
	string dataType;
	os <<
	   ConstantDefinesString(m_DoublePrecision) <<
	   UnionCLStructString <<
	   RgbToHsvFunctionString <<
	   HsvToRgbFunctionString <<
	   CalcAlphaFunctionString <<
	   CreateCalcNewRgbFunctionString(true) <<
	   SpatialFilterCLStructString <<
	   CreateGammaCorrectionFunctionString(true, false);//Will only be used with float in this case, early clip. Will always alpha accum.
	os << "__kernel void " << m_GammaCorrectionWithoutAlphaCalcEntryPoint << "(\n" <<
	   "	__global real4reals_bucket* accumulator,\n"
	   "	__constant SpatialFilterCL* spatialFilter\n"
	   ")\n"
	   "{\n"
	   "	int testGutter = 0;\n"
	   "\n"
	   "	if (GLOBAL_ID_Y >= (spatialFilter->m_SuperRasH - testGutter) || GLOBAL_ID_X >= (spatialFilter->m_SuperRasW - testGutter))\n"
	   "		return;\n"
	   "\n"
	   "	uint superIndex = (GLOBAL_ID_Y * spatialFilter->m_SuperRasW) + GLOBAL_ID_X;\n"
	   "	__global real4reals_bucket* bucket = accumulator + superIndex;\n"
	   "	GammaCorrectionFloats(bucket, &(spatialFilter->m_Background[0]), spatialFilter->m_Gamma, spatialFilter->m_LinRange, spatialFilter->m_Vibrancy, spatialFilter->m_HighlightPower, &(bucket->m_Reals[0]));\n"
	   "}\n"
	   ;
	return os.str();
}
}
