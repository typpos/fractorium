#pragma once

#include "Utils.h"
#include "Ember.h"
#include "DensityFilter.h"

/// <summary>
/// RendererBase, RenderCallback and EmberStats classes.
/// </summary>

namespace EmberNs
{
/// <summary>
/// Function pointers present a major restriction when dealing
/// with member functions, and that is they can only point to
/// static ones. So instead of a straight function pointer, use
/// a callback class with a single virtual callback
/// member function.
/// Template argument expected to be float or double.
/// </summary>
class EMBER_API RenderCallback
{
public:
	RenderCallback() = default;
	RenderCallback(RenderCallback& callback) = delete;

	/// <summary>
	/// Virtual destructor to ensure anything declared in derived classes gets cleaned up.
	/// </summary>
	virtual ~RenderCallback() = default;

	/// <summary>
	/// Empty progress function to be implemented in derived classes to take action on progress updates.
	/// </summary>
	/// <param name="ember">The ember currently being rendered</param>
	/// <param name="foo">An extra dummy parameter</param>
	/// <param name="fraction">The progress fraction from 0-100</param>
	/// <param name="stage">The stage of iteration. 1 is iterating, 2 is density filtering, 2 is final accumulation.</param>
	/// <param name="etaMs">The estimated milliseconds to completion of the current stage</param>
	/// <returns>Override should return 0 if an abort is requested, else 1 to continue rendering</returns>
	virtual int ProgressFunc(Ember<float>& ember, void* foo, double fraction, int stage, double etaMs) { return 0; }
	virtual int ProgressFunc(Ember<double>& ember, void* foo, double fraction, int stage, double etaMs) { return 0; }
};

/// <summary>
/// Render statistics for the number of iterations ran,
/// number of bad values calculated during iteration, and
/// the total time for the entire render from the start of
/// iteration to the end of final accumulation.
/// </summary>
class EMBER_API EmberStats
{
public:
	/// <summary>
	/// Constructor which sets all values to 0.
	/// </summary>
	EmberStats()
	{
		Clear();
	}

	void Clear()
	{
		m_Iters = 0;
		m_Badvals = 0;
		m_IterMs = 0;
		m_RenderMs = 0;
	}

	EmberStats& operator += (const EmberStats& stats)
	{
		m_Iters += stats.m_Iters;
		m_Badvals += stats.m_Badvals;
		m_IterMs += stats.m_IterMs;
		m_RenderMs += stats.m_RenderMs;
		return *this;
	}

	size_t m_Iters, m_Badvals;
	double m_IterMs, m_RenderMs;
};

/// <summary>
/// The types of available renderers.
/// Add more in the future as different rendering methods are experimented with.
/// Possible values might be: CPU+OpenGL, Particle, Inverse.
/// </summary>
enum class eRendererType : et { CPU_RENDERER, OPENCL_RENDERER };

/// <summary>
/// A base class with virtual functions to allow both templating and polymorphism to work together.
/// Derived classes will implement all of these functions.
/// Note that functions which return a decimal number use the most precise type, double.
/// </summary>
class EMBER_API RendererBase : public EmberReport
{
public:
	RendererBase();
	RendererBase(const RendererBase& renderer) = delete;
	RendererBase& operator = (const RendererBase& renderer) = delete;
	virtual ~RendererBase() = default;

	//Non-virtual processing functions.
	void ChangeVal(std::function<void(void)> func, eProcessAction action);
	size_t HistMemoryRequired(size_t strips);
	pair<size_t, size_t> MemoryRequired(size_t strips, bool includeFinal, bool threadedWrite);
	vector<QTIsaac<ISAAC_SIZE, ISAAC_INT>> RandVec();
	bool PrepFinalAccumVector(vector<v4F>& pixels);

	//Virtual processing functions.
	virtual bool Ok() const;
	virtual size_t MemoryAvailable();
	virtual void SetEmber(const Ember<float>& ember, eProcessAction action = eProcessAction::FULL_RENDER, bool prep = false) { }
	virtual void SetEmber(const Ember<double>& ember, eProcessAction action = eProcessAction::FULL_RENDER, bool prep = false) { }
	virtual bool RandVec(vector<QTIsaac<ISAAC_SIZE, ISAAC_INT>>& randVec);

	//Abstract processing functions.
	virtual bool CreateDEFilter(bool& newAlloc) = 0;
	virtual bool CreateSpatialFilter(bool& newAlloc) = 0;
	virtual bool CreateTemporalFilter(bool& newAlloc) = 0;
	virtual void ComputeBounds() = 0;
	virtual void ComputeQuality() = 0;
	virtual void ComputeCamera() = 0;
	virtual eRenderStatus Run(vector<v4F>& finalImage, double time = 0, size_t subBatchCountOverride = 0, bool forceOutput = false, size_t finalOffset = 0) = 0;
	virtual EmberImageComments ImageComments(const EmberStats& stats, size_t printEditDepth = 0, bool hexPalette = true) = 0;
	virtual DensityFilterBase* GetDensityFilter() = 0;

	//Non-virtual renderer properties, getters only.
	size_t		   SuperRasW()					 const;
	size_t		   SuperRasH()					 const;
	size_t		   SuperSize()					 const;
	size_t		   FinalRowSize()				 const;
	size_t		   FinalDimensions()			 const;
	size_t		   FinalBufferSize()			 const;
	size_t		   PixelSize()					 const;
	size_t		   GutterWidth()				 const;
	size_t		   DensityFilterOffset()		 const;
	size_t		   TotalIterCount(size_t strips) const;
	size_t		   ItersPerTemporalSample()		 const;
	eProcessState  ProcessState()				 const;
	eProcessAction ProcessAction()				 const;
	EmberStats     Stats() 						 const;

	//Non-virtual render getters and setters.
	bool LockAccum() const;
	void LockAccum(bool lockAccum);
	bool EarlyClip() const;
	void EarlyClip(bool earlyClip);
	bool YAxisUp() const;
	void YAxisUp(bool yup);
	bool InsertPalette() const;
	void InsertPalette(bool insertPalette);
	bool ReclaimOnResize() const;
	void ReclaimOnResize(bool reclaimOnResize);
	void Callback(RenderCallback* callback);
	void ThreadCount(size_t threads, const char* seedString = nullptr);
	size_t BytesPerChannel() const;
	size_t NumChannels() const;
	eThreadPriority Priority() const;
	void Priority(eThreadPriority priority);
	eInteractiveFilter InteractiveFilter() const;
	void InteractiveFilter(eInteractiveFilter filter);

	//Virtual render properties, getters and setters.
	virtual size_t ThreadCount()   const;
	virtual eRendererType RendererType() const;

	//Abstract render properties, getters only.
	virtual size_t TemporalSamples()			   const = 0;
	virtual size_t HistBucketSize()				   const = 0;
	virtual size_t FinalRasW()		               const = 0;
	virtual size_t FinalRasH()					   const = 0;
	virtual size_t SubBatchSize()				   const = 0;
	virtual size_t FuseCount()					   const = 0;
	virtual double ScaledQuality()                 const = 0;
	virtual double LowerLeftX(bool  gutter = true) const = 0;
	virtual double LowerLeftY(bool  gutter = true) const = 0;
	virtual double UpperRightX(bool gutter = true) const = 0;
	virtual double UpperRightY(bool gutter = true) const = 0;

	//Non-virtual threading control.
	void Reset();
	void EnterRender();
	void LeaveRender();
	void EnterFinalAccum();
	void LeaveFinalAccum();
	void EnterResize();
	void LeaveResize();
	void Abort();
	bool Aborted();
	void Pause(bool pause);
	bool Paused();
	bool InRender();
	bool InFinalAccum();

	void* m_ProgressParameter = nullptr;

protected:
	bool m_EarlyClip = false;
	bool m_YAxisUp = false;
	bool m_LockAccum = false;
	bool m_InRender = false;
	bool m_InFinalAccum = false;
	bool m_InsertPalette = false;
	bool m_ReclaimOnResize = false;
	bool m_CurvesSet = false;
	volatile bool m_Abort = false;
	volatile bool m_Pause = false;
	size_t m_SuperRasW;
	size_t m_SuperRasH;
	size_t m_SuperSize = 0;
	size_t m_GutterWidth;
	size_t m_DensityFilterOffset;
	size_t m_NumChannels = 4;
	size_t m_BytesPerChannel = 4;
	size_t m_ThreadsToUse;
	size_t m_VibGamCount;
	size_t m_LastTemporalSample = 0;
	size_t m_LastIter = 0;
	double m_LastIterPercent = 0;
	eThreadPriority m_Priority = eThreadPriority::NORMAL;
	eProcessAction m_ProcessAction = eProcessAction::FULL_RENDER;
	eProcessState m_ProcessState = eProcessState::NONE;
	eInteractiveFilter m_InteractiveFilter = eInteractiveFilter::FILTER_LOG;
	EmberStats m_Stats;
	RenderCallback* m_Callback = nullptr;
	vector<size_t> m_SubBatch;
	vector<size_t> m_BadVals;
	vector<QTIsaac<ISAAC_SIZE, ISAAC_INT>> m_Rand;
	std::recursive_mutex m_RenderingCs, m_AccumCs, m_FinalAccumCs, m_ResizeCs;
	Timing m_RenderTimer, m_IterTimer, m_ProgressTimer;
};
}
