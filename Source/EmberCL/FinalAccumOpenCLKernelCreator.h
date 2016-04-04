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
/// Alpha channel, no alpha channel
/// Alpha with/without transparency
/// </summary>
class EMBERCL_API FinalAccumOpenCLKernelCreator
{
public:
	FinalAccumOpenCLKernelCreator(bool doublePrecision);

	const string& GammaCorrectionWithAlphaCalcKernel() const;
	const string& GammaCorrectionWithAlphaCalcEntryPoint() const;

	const string& GammaCorrectionWithoutAlphaCalcKernel() const;
	const string& GammaCorrectionWithoutAlphaCalcEntryPoint() const;

	const string& FinalAccumEarlyClipKernel() const;
	const string& FinalAccumEarlyClipEntryPoint() const;
	const string& FinalAccumEarlyClipWithAlphaCalcWithAlphaAccumKernel() const;
	const string& FinalAccumEarlyClipWithAlphaCalcWithAlphaAccumEntryPoint() const;
	const string& FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumKernel() const;
	const string& FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumEntryPoint() const;

	const string& FinalAccumLateClipKernel() const;
	const string& FinalAccumLateClipEntryPoint() const;
	const string& FinalAccumLateClipWithAlphaCalcWithAlphaAccumKernel() const;
	const string& FinalAccumLateClipWithAlphaCalcWithAlphaAccumEntryPoint() const;
	const string& FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumKernel() const;
	const string& FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumEntryPoint() const;
	const string& GammaCorrectionEntryPoint(size_t channels, bool transparency) const;
	const string& GammaCorrectionKernel(size_t channels, bool transparency) const;
	const string& FinalAccumEntryPoint(bool earlyClip, size_t channels, bool transparency, double& alphaBase, double& alphaScale) const;
	const string& FinalAccumKernel(bool earlyClip, size_t channels, bool transparency) const;

private:
	string CreateFinalAccumKernelString(bool earlyClip, size_t channels, bool transparency);
	string CreateGammaCorrectionKernelString(bool alphaCalc);

	string CreateFinalAccumKernelString(bool earlyClip, bool alphaCalc, bool alphaAccum);
	string CreateGammaCorrectionFunctionString(bool globalBucket, bool alphaCalc, bool alphaAccum, bool finalOut);
	string CreateCalcNewRgbFunctionString(bool globalBucket);

	string m_GammaCorrectionWithAlphaCalcKernel;
	string m_GammaCorrectionWithAlphaCalcEntryPoint = "GammaCorrectionWithAlphaCalcKernel";

	string m_GammaCorrectionWithoutAlphaCalcKernel;
	string m_GammaCorrectionWithoutAlphaCalcEntryPoint = "GammaCorrectionWithoutAlphaCalcKernel";

	string m_FinalAccumEarlyClipKernel;//False, false.
	string m_FinalAccumEarlyClipEntryPoint = "FinalAccumEarlyClipKernel";
	string m_FinalAccumEarlyClipWithAlphaCalcWithAlphaAccumKernel;//True, true.
	string m_FinalAccumEarlyClipWithAlphaCalcWithAlphaAccumEntryPoint = "FinalAccumEarlyClipWithAlphaCalcWithAlphaAccumKernel";
	string m_FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumKernel;//False, true.
	string m_FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumEntryPoint = "FinalAccumEarlyClipWithoutAlphaCalcWithAlphaAccumKernel";

	string m_FinalAccumLateClipKernel;//False, false.
	string m_FinalAccumLateClipEntryPoint = "FinalAccumLateClipKernel";
	string m_FinalAccumLateClipWithAlphaCalcWithAlphaAccumKernel;//True, true.
	string m_FinalAccumLateClipWithAlphaCalcWithAlphaAccumEntryPoint = "FinalAccumLateClipWithAlphaCalcWithAlphaAccumKernel";
	string m_FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumKernel;//False, true.
	string m_FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumEntryPoint = "FinalAccumLateClipWithoutAlphaCalcWithAlphaAccumKernel";

	string m_Empty;
	bool m_DoublePrecision;
};
}
