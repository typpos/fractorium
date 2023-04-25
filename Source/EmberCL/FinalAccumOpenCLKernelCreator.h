#pragma once

#include "EmberCLPch.h"
#include "EmberCLStructs.h"
#include "EmberCLFunctions.h"

/// <summary>
/// FinalAccumOpenCLKernelCreator class.
/// </summary>

namespace EmberCLns
{
/// <summary>
/// Class for creating the final accumulation code in OpenCL.
/// There are many conditionals in the CPU code to create the
/// final output image. This class creates many different kernels
/// with all conditionals and unnecessary calculations stripped out.
/// The conditionals are:
/// Early clip/late clip
/// </summary>
class EMBERCL_API FinalAccumOpenCLKernelCreator
{
public:
	FinalAccumOpenCLKernelCreator(bool doublePrecision);

	const string& FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumKernel() const;
	const string& FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumEntryPoint() const;
	const string& FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumKernel() const;
	const string& FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumEntryPoint() const;
	const string& GammaCorrectionEntryPoint() const;
	const string& GammaCorrectionKernel() const;
	const string& FinalAccumEntryPoint(bool earlyClip) const;
	const string& FinalAccumKernel(bool earlyClip) const;

private:
	string CreateFinalAccumKernelString(bool earlyClip);
	string CreateGammaCorrectionKernelString();

	string CreateGammaCorrectionFunctionString(bool globalBucket, bool finalOut);
	string CreateCalcNewRgbFunctionString(bool globalBucket);

	string m_GammaCorrectionWithoutAlphaCalcKernel;
	string m_GammaCorrectionWithoutAlphaCalcEntryPoint = "GammaCorrectionWithoutAlphaCalcKernel";

	string m_FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumKernel;
	string m_FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumEntryPoint = "FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumKernel";

	string m_FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumKernel;
	string m_FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumEntryPoint = "FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumKernel";

	string m_Empty;
	bool m_DoublePrecision;
};
}
