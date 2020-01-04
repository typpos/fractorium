#include "FractoriumPch.h"
#include "FractoriumEmberController.h"
#include "FinalRenderEmberController.h"
#include "FinalRenderDialog.h"
#include "Fractorium.h"

/// <summary>
/// Constructor which accepts a pointer to the final render dialog.
/// It passes a pointer to the main window to the base and initializes members.
/// </summary>
/// <param name="finalRender">Pointer to the final render dialog</param>
FinalRenderEmberControllerBase::FinalRenderEmberControllerBase(FractoriumFinalRenderDialog* finalRenderDialog)
	: FractoriumEmberControllerBase(finalRenderDialog->m_Fractorium),
	  m_FinalRenderDialog(finalRenderDialog)
{
	m_FinishedImageCount.store(0);
	m_Settings = FractoriumSettings::DefInstance();
}

/// <summary>
/// Cancel the render by calling Abort().
/// This will block until the cancelling is actually finished.
/// It should never take longer than a few milliseconds because the
/// renderer checks the m_Abort flag in many places during the process.
/// </summary>
template <typename T>
void FinalRenderEmberController<T>::CancelRender()
{
	if (m_Result.isRunning())
	{
		std::thread th([&]
		{
			m_Run = false;

			if (m_Renderer.get())
			{
				m_Renderer->Abort();

				while (m_Renderer->InRender())
					std::this_thread::sleep_for(std::chrono::milliseconds(10));

				m_Renderer->EnterRender();
				m_Renderer->EnterFinalAccum();
				m_Renderer->LeaveFinalAccum();
				m_Renderer->LeaveRender();
			}
			else
			{
				for (auto& renderer : m_Renderers)
				{
					renderer->Abort();

					while (renderer->InRender())
						QApplication::processEvents();

					renderer->EnterRender();
					renderer->EnterFinalAccum();
					renderer->LeaveFinalAccum();
					renderer->LeaveRender();
				}
			}
		});
		Join(th);

		while (m_Result.isRunning())
			QApplication::processEvents();

		m_FinalRenderDialog->ui.FinalRenderTextOutput->append("Render canceled.");
	}
}

/// <summary>
/// Create a new renderer based on the options selected on the GUI.
/// If a renderer matching the options has already been created, no action is taken.
/// </summary>
/// <returns>True if a valid renderer is created or if no action is taken, else false.</returns>
bool FinalRenderEmberControllerBase::CreateRendererFromGUI()
{
	bool useOpenCL = m_Info->Ok() && m_FinalRenderDialog->OpenCL();
	auto v = Devices(m_FinalRenderDialog->Devices());
	return CreateRenderer((useOpenCL && !v.empty()) ? eRendererType::OPENCL_RENDERER : eRendererType::CPU_RENDERER,
						  v, false, false); //Not shared.
}

/// <summary>
/// Thin wrapper around invoking a call to append text to the output.
/// </summary>
/// <param name="s">The string to append</param>
void FinalRenderEmberControllerBase::Output(const QString& s)
{
	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderTextOutput, "append", Qt::QueuedConnection, Q_ARG(const QString&, s));
}

/// <summary>
/// Constructor which accepts a pointer to the final render dialog and passes it to the base.
/// The main final rendering lambda function is constructed here.
/// </summary>
/// <param name="finalRender">Pointer to the final render dialog</param>
template<typename T>
FinalRenderEmberController<T>::FinalRenderEmberController(FractoriumFinalRenderDialog* finalRender)
	: FinalRenderEmberControllerBase(finalRender)
{
	m_FinalPreviewRenderer = make_unique<FinalRenderPreviewRenderer<T>>(this);
	//The main rendering function which will be called in a Qt thread.
	//A backup Xml is made before the rendering process starts just in case it crashes before finishing.
	//If it finishes successfully, delete the backup file.
	m_FinalRenderFunc = [&]()
	{
		m_Run = true;
		m_TotalTimer.Tic();//Begin timing for progress of all operations.
		m_GuiState = m_FinalRenderDialog->State();//Cache render settings from the GUI before running.
		size_t i = 0;
		bool doAll = m_GuiState.m_DoAll && m_EmberFile.Size() > 1;
		bool isBump = !doAll && m_IsQualityBump && m_GuiState.m_Strips == 1;//Should never get called with m_IsQualityBump otherwise, but check one last time to be safe.
		size_t currentStripForProgress = 0;//Sort of a hack to get the strip value to the progress function.
		QString path = doAll ? ComposePath(QString::fromStdString(m_EmberFile.m_Embers.begin()->m_Name)) : ComposePath(Name());
		QString backup = path + "_backup.flame";
		m_FinishedImageCount.store(0);
		Pause(false);
		ResetProgress();

		if (!isBump)
		{
			//Save backup Xml.
			if (doAll)
				m_XmlWriter.Save(backup.toStdString().c_str(), m_EmberFile.m_Embers, 0, true, false, true, false, false);
			else
				m_XmlWriter.Save(backup.toStdString().c_str(), *m_Ember, 0, true, false, true, false, false);

			SyncGuiToRenderer();
			FirstOrDefaultRenderer()->m_ProgressParameter = reinterpret_cast<void*>(&currentStripForProgress);//When animating, only the first (primary) device has a progress parameter.
			m_GuiState.m_Strips = VerifyStrips(m_Ember->m_FinalRasH, m_GuiState.m_Strips,
			[&](const string & s) { Output(QString::fromStdString(s)); }, //Greater than height.
			[&](const string & s) { Output(QString::fromStdString(s)); }, //Mod height != 0.
			[&](const string & s) { Output(QString::fromStdString(s) + "\n"); }); //Final strips value to be set.
		}

		//The rendering process is different between doing a single image, and doing multiple.
		if (doAll)
		{
			m_ImageCount = m_EmberFile.Size();

			//Different action required for rendering as animation or not.
			if (m_GuiState.m_DoSequence && !m_Renderers.empty())
			{
				Ember<T>* prev = nullptr;
				vector<Ember<T>> embers;
				vector<std::thread> threadVec;
				std::atomic<size_t> atomfTime;
				auto firstEmber = m_EmberFile.m_Embers.begin();

				//Need to loop through and set all w, h, q, ts, ss and t vals.
				for (auto& it : m_EmberFile.m_Embers)
				{
					if (!m_Run)
						break;

					SyncGuiToEmber(it, firstEmber->m_FinalRasW, firstEmber->m_FinalRasH);

					if (&it == &(*firstEmber))//First.
					{
						it.m_Time = 0;
					}
					else//All others
					{
						if (it.m_Time <= prev->m_Time)
							it.m_Time  = prev->m_Time + 1;
					}

					it.m_TemporalSamples = m_GuiState.m_TemporalSamples;
					prev = &it;
				}

				//Not supporting strips with animation.
				//Shouldn't be a problem because animations will be at max 4k x 2k which will take about 1GB
				//even when using double precision, which most cards at the time of this writing already exceed.
				m_GuiState.m_Strips = 1;
				atomfTime.store(0);
				CopyCont(embers, m_EmberFile.m_Embers);
				std::function<void(size_t)> iterFunc = [&](size_t index)
				{
					size_t ftime;
					size_t finalImageIndex = 0;
					std::thread writeThread;
					vector<v4F> finalImages[2];
					EmberStats stats;
					EmberImageComments comments;
					Timing renderTimer;
					auto renderer = m_Renderers[index].get();
					renderer->SetExternalEmbersPointer(&embers);//All will share a pointer to the original vector to conserve memory with large files. Ok because the vec doesn't get modified.

					//Render each image, cancelling if m_Run ever gets set to false.
					//Render each image, cancelling if m_Run ever gets set to false.
					//The conditions of this loop use atomics to synchronize when running on multiple GPUs.
					//The order is reversed from the usual loop: rather than compare and increment the counter,
					//it's incremented, then compared. This is done to ensure the GPU on this thread "claims" this
					//frame before working on it.
					//The mechanism for incrementing is:
					//	Do an atomic add, which returns the previous value.
					//	Add 1 to the return value to mimic what the new atomic value should be.
					//	Assign the result to the ftime counter.
					//	Do a <= comparison to m_EmberFile.Size() and check m_Run.
					//		If true, enter the loop and immediately decrement the counter by 1 to make up for the fact
					//		that it was first incremented before comparing.
					while (((ftime = (atomfTime.fetch_add(1) + 1)) <= m_EmberFile.Size()) && m_Run)//Needed to set 1 to claim this iter from other threads, so decrement it below to be zero-indexed here.
					{
						--ftime;
						Output("Image " + ToString(ftime + 1ULL) + ":\n" + ComposePath(QString::fromStdString(m_EmberFile.Get(ftime)->m_Name)));
						renderer->Reset();//Have to manually set this since the ember is not set each time through.
						renderTimer.Tic();//Toc() is called in RenderComplete().

						//Can't use strips render here. Run() must be called directly for animation.
						if (renderer->Run(finalImages[finalImageIndex], T(ftime)) != eRenderStatus::RENDER_OK)
						{
							Output("Rendering failed.\n");
							m_Fractorium->ErrorReportToQTextEdit(renderer->ErrorReport(), m_FinalRenderDialog->ui.FinalRenderTextOutput, false);//Internally calls invoke.
							atomfTime.store(m_EmberFile.Size() + 1);//Abort all threads if any of them encounter an error.
							break;
						}
						else
						{
							Join(writeThread);
							stats = renderer->Stats();
							comments = renderer->ImageComments(stats, 0, true);
							writeThread = std::thread([&](size_t tempTime, size_t threadFinalImageIndex)
							{
								SaveCurrentRender(*m_EmberFile.Get(tempTime),
												  comments,//These all don't change during the renders, so it's ok to access them in the thread.
												  finalImages[threadFinalImageIndex],
												  renderer->FinalRasW(),
												  renderer->FinalRasH(),
												  m_FinalRenderDialog->Png16Bit(),
												  m_FinalRenderDialog->Transparency());
							}, ftime, finalImageIndex);
							m_FinishedImageCount.fetch_add(1);
							RenderComplete(*m_EmberFile.Get(ftime), stats, renderTimer);

							if (!index)//Only first device has a progress callback, so it also makes sense to only manually set the progress on the first device as well.
								HandleFinishedProgress();
						}

						finalImageIndex ^= 1;//Toggle the index.
					}

					Join(writeThread);//One final check to make sure all writing is done before exiting this thread.
				};
				threadVec.reserve(m_Renderers.size());

				for (size_t r = 0; r < m_Renderers.size(); r++)
				{
					threadVec.push_back(std::thread([&](size_t index)
					{
						iterFunc(index);
					}, r));
				}

				Join(threadVec);
				HandleFinishedProgress();//One final check that all images were finished.
			}
			else if (m_Renderer.get())//Make sure a renderer was created and render all images, but not as an animation sequence (without temporal samples motion blur).
			{
				//Render each image, cancelling if m_Run ever gets set to false.
				for (auto& it : m_EmberFile.m_Embers)
				{
					if (!m_Run)
						break;

					Output("Image " + ToString<qulonglong>(m_FinishedImageCount.load() + 1) + ":\n" + ComposePath(QString::fromStdString(it.m_Name)));
					it.m_TemporalSamples = 1;//No temporal sampling.
					m_Renderer->SetEmber(it);
					m_Renderer->PrepFinalAccumVector(m_FinalImage);//Must manually call this first because it could be erroneously made smaller due to strips if called inside Renderer::Run().
					m_Stats.Clear();
					m_RenderTimer.Tic();//Toc() is called in RenderComplete().
					StripsRender<T>(m_Renderer.get(), it, m_FinalImage, 0, m_GuiState.m_Strips, m_GuiState.m_YAxisUp,
					[&](size_t strip) { currentStripForProgress = strip; },//Pre strip.
					[&](size_t strip) { m_Stats += m_Renderer->Stats(); },//Post strip.
					[&](size_t strip)//Error.
					{
						Output("Rendering failed.\n");
						m_Fractorium->ErrorReportToQTextEdit(m_Renderer->ErrorReport(), m_FinalRenderDialog->ui.FinalRenderTextOutput, false);//Internally calls invoke.
					},
					[&](Ember<T>& finalEmber)
					{
						m_FinishedImageCount.fetch_add(1);
						SaveCurrentRender(finalEmber);
						RenderComplete(finalEmber);
						HandleFinishedProgress();
					});//Final strip.
				}
			}
			else
			{
				Output("No renderer present, aborting.");
			}
		}
		else if (m_Renderer.get())//Render a single image.
		{
			m_ImageCount = 1;
			m_Ember->m_TemporalSamples = 1;
            m_Fractorium->m_Controller->ParamsToEmber(*m_Ember, true); // update color and filter params
			m_Renderer->SetEmber(*m_Ember, isBump ? eProcessAction::KEEP_ITERATING : eProcessAction::FULL_RENDER);
			m_Renderer->PrepFinalAccumVector(m_FinalImage);//Must manually call this first because it could be erroneously made smaller due to strips if called inside Renderer::Run().
			m_Stats.Clear();
			Output(ComposePath(QString::fromStdString(m_Ember->m_Name)));
			m_RenderTimer.Tic();//Toc() is called in RenderComplete().
			StripsRender<T>(m_Renderer.get(), *m_Ember, m_FinalImage, 0, m_GuiState.m_Strips, m_GuiState.m_YAxisUp,
			[&](size_t strip) { currentStripForProgress = strip; },//Pre strip.
			[&](size_t strip) { m_Stats += m_Renderer->Stats(); },//Post strip.
			[&](size_t strip)//Error.
			{
				Output("Rendering failed.\n");
				m_Fractorium->ErrorReportToQTextEdit(m_Renderer->ErrorReport(), m_FinalRenderDialog->ui.FinalRenderTextOutput, false);//Internally calls invoke.
			},
			[&](Ember<T>& finalEmber)
			{
				m_FinishedImageCount.fetch_add(1);
				SaveCurrentRender(finalEmber);
				RenderComplete(finalEmber);
				HandleFinishedProgress();
			});//Final strip.
		}
		else
		{
			Output("No renderer present, aborting.");
		}

		QString totalTimeString = "All renders completed in: " + QString::fromStdString(m_TotalTimer.Format(m_TotalTimer.Toc())) + ".";
		Output(totalTimeString);
		QFile::remove(backup);
		QMetaObject::invokeMethod(m_FinalRenderDialog, "Pause", Qt::QueuedConnection, Q_ARG(bool, false));
		m_Run = false;
	};
}

/// <summary>
/// Virtual functions overridden from FractoriumEmberControllerBase.
/// </summary>

/// <summary>
/// Setters for embers and ember files which convert between float and double types.
/// These are used to preserve the current ember/file when switching between renderers.
/// Note that some precision will be lost when going from double to float.
/// </summary>
template <typename T> void FinalRenderEmberController<T>::SetEmberFile(const EmberFile<float>& emberFile, bool move)
{
	move ? m_EmberFile = std::move(emberFile) : m_EmberFile = emberFile;
	m_Ember = m_EmberFile.Get(0);
}
template <typename T> void FinalRenderEmberController<T>::CopyEmberFile(EmberFile<float>& emberFile, bool sequence, std::function<void(Ember<float>& ember)> perEmberOperation)
{
	emberFile.m_Filename = m_EmberFile.m_Filename;
	CopyCont(emberFile.m_Embers, m_EmberFile.m_Embers, perEmberOperation);
}

#ifdef DO_DOUBLE
template <typename T> void FinalRenderEmberController<T>::SetEmberFile(const EmberFile<double>& emberFile, bool move)
{
	move ? m_EmberFile = std::move(emberFile) : m_EmberFile = emberFile;
	m_Ember = m_EmberFile.Get(0);
}
template <typename T> void FinalRenderEmberController<T>::CopyEmberFile(EmberFile<double>& emberFile, bool sequence, std::function<void(Ember<double>& ember)> perEmberOperation)
{
	emberFile.m_Filename = m_EmberFile.m_Filename;
	CopyCont(emberFile.m_Embers, m_EmberFile.m_Embers, perEmberOperation);
}
#endif

/// <summary>
/// Set the ember at the specified index from the currently opened file as the current Ember.
/// Clears the undo state.
/// Resets the rendering process.
/// </summary>
/// <param name="index">The index in the file from which to retrieve the ember</param>
/// <param name="verbatim">Unused</param>
template <typename T>
void FinalRenderEmberController<T>::SetEmber(size_t index, bool verbatim)
{
	if (index < m_EmberFile.Size())
	{
		m_Ember = m_EmberFile.Get(index);
		SyncCurrentToGui();
	}
	else if (m_EmberFile.Size() > 1)
	{
		m_Ember = m_EmberFile.Get(0);//Should never happen.
	}
}

/// <summary>
/// Start the final rendering process.
/// Create the needed renderer from the GUI if it has not been created yet.
/// </summary>
/// <returns></returns>
template<typename T>
bool FinalRenderEmberController<T>::Render()
{
	QString filename = m_FinalRenderDialog->Path();

	if (filename == "")
	{
		m_Fractorium->ShowCritical("File Error", "Please enter a valid path and filename for the output.");
		return false;
	}

	m_IsQualityBump = false;

	if (CreateRendererFromGUI())
	{
		m_FinalRenderDialog->ui.FinalRenderTextOutput->setText("Preparing all parameters.\n");
		//Note that a Qt thread must be used, rather than a tbb task.
		//This is because tbb does a very poor job of allocating thread resources
		//and dedicates an entire core just to this thread which does nothing waiting for the
		//parallel iteration loops inside of the CPU renderer to finish. The result is that
		//the renderer ends up using ThreadCount - 1 to iterate, instead of ThreadCount.
		//By using a Qt thread here, and tbb inside the renderer, all cores can be maxed out.
		m_Result = QtConcurrent::run(m_FinalRenderFunc);
		m_Settings->sync();
		return true;
	}
	else
		return false;
}


/// <summary>
/// Increase the quality of the last render and start rendering again.
/// Note this is only when rendering a single image with no strips.
/// </summary>
/// <param name="d">The amount to increase the quality by, expressed as a decimal percentage. Eg: 0.5 means to increase by 50%.</param>
/// <returns>True if nothing went wrong, else false.</returns>
template <typename T>
bool FinalRenderEmberController<T>::BumpQualityRender(double d)
{
	m_Ember->m_Quality += std::ceil(m_Ember->m_Quality * d);
	m_Renderer->SetEmber(*m_Ember, eProcessAction::KEEP_ITERATING, true);
	QString filename = m_FinalRenderDialog->Path();

	if (filename == "")
	{
		m_Fractorium->ShowCritical("File Error", "Please enter a valid path and filename for the output.");
		return false;
	}

	m_IsQualityBump = true;
	auto iterCount = m_Renderer->TotalIterCount(1);
	m_FinalRenderDialog->ui.FinalRenderParamsTable->item(m_FinalRenderDialog->m_ItersCellIndex, 1)->setText(ToString<qulonglong>(iterCount));
	m_FinalRenderDialog->ui.FinalRenderTextOutput->setText("Preparing all parameters.\n");
	m_Result = QtConcurrent::run(m_FinalRenderFunc);
	m_Settings->sync();
	return true;
}

/// <summary>
/// Stop rendering and initialize a new renderer, using the specified type and the options on the final render dialog.
/// </summary>
/// <param name="renderType">The type of render to create</param>
/// <param name="devices">The platform,device index pairs of the devices to use</param>
/// <param name="updatePreviews">Unused</param>
/// <param name="shared">True if shared with OpenGL, else false. Always false in this case.</param>
/// <returns>True if nothing went wrong, else false.</returns>
template <typename T>
bool FinalRenderEmberController<T>::CreateRenderer(eRendererType renderType, const vector<pair<size_t, size_t>>& devices, bool updatePreviews, bool shared)
{
	bool ok = true;
	bool renderTypeMismatch = (m_Renderer.get() && (m_Renderer->RendererType() != renderType)) ||
							  (!m_Renderers.empty() && (m_Renderers[0]->RendererType() != renderType));
	CancelRender();

	if ((!m_FinalRenderDialog->DoSequence() && (!m_Renderer.get() || !m_Renderer->Ok())) ||
			(m_FinalRenderDialog->DoSequence() && m_Renderers.empty()) ||
			renderTypeMismatch ||
			!Equal(m_Devices, devices))
	{
		EmberReport emberReport;
		vector<string> errorReport;
		m_Devices = devices;//Store values for re-creation later on.
		m_OutputTexID = 0;//Don't care about tex ID when doing final render.

		if (m_FinalRenderDialog->DoSequence())
		{
			m_Renderer.reset();
			m_Renderers = ::CreateRenderers<T>(renderType, m_Devices, shared, m_OutputTexID, emberReport);

			for (auto& renderer : m_Renderers)
				if (auto rendererCL = dynamic_cast<RendererCLBase*>(renderer.get()))
					rendererCL->OptAffine(true);//Optimize empty affines for final renderers, this is normally false for the interactive renderer.
		}
		else
		{
			m_Renderers.clear();
			m_Renderer = unique_ptr<EmberNs::RendererBase>(::CreateRenderer<T>(renderType, m_Devices, shared, m_OutputTexID, emberReport));

			if (auto rendererCL = dynamic_cast<RendererCLBase*>(m_Renderer.get()))
				rendererCL->OptAffine(true);//Optimize empty affines for final renderers, this is normally false for the interactive renderer.
		}

		errorReport = emberReport.ErrorReport();

		if (!errorReport.empty())
		{
			ok = false;
			m_Fractorium->ShowCritical("Renderer Creation Error", "Could not create requested renderer, fallback CPU renderer created. See info tab for details.");
			m_Fractorium->ErrorReportToQTextEdit(errorReport, m_Fractorium->ui.InfoRenderingTextEdit);
		}
	}

	return SyncGuiToRenderer() && ok;
}

/// <summary>
/// Progress function.
/// Take special action to sync options upon finishing.
/// Note this is only called on the primary renderer.
/// </summary>
/// <param name="ember">The ember currently being rendered</param>
/// <param name="foo">An extra dummy parameter, unused.</param>
/// <param name="fraction">The progress fraction from 0-100</param>
/// <param name="stage">The stage of iteration. 1 is iterating, 2 is density filtering, 2 is final accumulation.</param>
/// <param name="etaMs">The estimated milliseconds to completion of the current stage</param>
/// <returns>0 if the user has clicked cancel, else 1 to continue rendering.</returns>
template <typename T>
int FinalRenderEmberController<T>::ProgressFunc(Ember<T>& ember, void* foo, double fraction, int stage, double etaMs)
{
	static int count = 0;
	size_t strip = *(reinterpret_cast<size_t*>(FirstOrDefaultRenderer()->m_ProgressParameter));
	double fracPerStrip = std::ceil(100.0 / m_GuiState.m_Strips);
	double stripsfrac = std::ceil(fracPerStrip * strip) + std::ceil(fraction / m_GuiState.m_Strips);
	int intFract = int(stripsfrac);

	if (stage == 0)
		QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderIterationProgress, "setValue", Qt::QueuedConnection, Q_ARG(int, intFract));
	else if (stage == 1)
		QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderFilteringProgress, "setValue", Qt::QueuedConnection, Q_ARG(int, intFract));
	else if (stage == 2)
		QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderAccumProgress, "setValue", Qt::QueuedConnection, Q_ARG(int, intFract));

	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderImageCountLabel, "setText", Qt::QueuedConnection, Q_ARG(const QString&, ToString<qulonglong>(m_FinishedImageCount.load() + 1) + " / " + ToString<qulonglong>(m_ImageCount) + " Eta: " + QString::fromStdString(m_RenderTimer.Format(etaMs))));
	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderTextOutput, "update", Qt::QueuedConnection);
	return m_Run ? 1 : 0;
}

/// <summary>
/// Virtual functions overridden from FinalRenderEmberControllerBase.
/// </summary>

/// <summary>
/// Copy current ember values to widgets.
/// </summary>
template <typename T>
void FinalRenderEmberController<T>::SyncCurrentToGui()
{
	SyncCurrentToSizeSpinners(true, true);
	m_FinalRenderDialog->ui.FinalRenderCurrentSpin->setSuffix("  " + Name());
	m_FinalRenderDialog->Scale(m_Ember->ScaleType());
	m_FinalRenderDialog->m_QualitySpin->SetValueStealth(m_Ember->m_Quality);
	m_FinalRenderDialog->m_SupersampleSpin->SetValueStealth(m_Ember->m_Supersample);
	m_FinalRenderDialog->Path(ComposePath(Name()));
}

/// <summary>
/// Copy GUI values to either the current ember, or all embers in the file
/// depending on whether Render All is checked.
/// </summary>
/// <param name="widthOverride">Width override to use instead of scaling the original width</param>
/// <param name="heightOverride">Height override to use instead of scaling the original height</param>
/// <param name="dowidth">Whether to apply width adjustment to the ember</param>
/// <param name="doheight">Whether to apply height adjustment to the ember</param>
template <typename T>
void FinalRenderEmberController<T>::SyncGuiToEmbers(size_t widthOverride, size_t heightOverride, bool dowidth, bool doheight)
{
	if (m_FinalRenderDialog->ApplyToAll())
	{
		for (auto& ember : m_EmberFile.m_Embers)
			SyncGuiToEmber(ember, widthOverride, heightOverride, dowidth, doheight);
	}
	else
	{
		SyncGuiToEmber(*m_Ember, widthOverride, heightOverride, dowidth, doheight);
	}
}

/// <summary>
/// Copy GUI values to the renderers.
/// </summary>
template <typename T>
bool FinalRenderEmberController<T>::SyncGuiToRenderer()
{
	bool ok = true;

	if (m_Renderer.get())
	{
		m_Renderer->Callback(this);
		m_Renderer->EarlyClip(m_FinalRenderDialog->EarlyClip());
		m_Renderer->YAxisUp(m_FinalRenderDialog->YAxisUp());
		m_Renderer->ThreadCount(m_FinalRenderDialog->ThreadCount());
		m_Renderer->Priority((eThreadPriority)m_FinalRenderDialog->ThreadPriority());

		if (auto rendererCL = dynamic_cast<RendererCL<T, float>*>(m_Renderer.get()))
			rendererCL->SubBatchPercentPerThread(m_FinalRenderDialog->OpenCLSubBatchPct());
	}
	else if (!m_Renderers.empty())
	{
		for (size_t i = 0; i < m_Renderers.size(); i++)
		{
			m_Renderers[i]->Callback(!i ? this : nullptr);
			m_Renderers[i]->EarlyClip(m_FinalRenderDialog->EarlyClip());
			m_Renderers[i]->YAxisUp(m_FinalRenderDialog->YAxisUp());
			m_Renderers[i]->ThreadCount(m_FinalRenderDialog->ThreadCount());
			m_Renderers[i]->Priority((eThreadPriority)m_FinalRenderDialog->ThreadPriority());

			if (auto rendererCL = dynamic_cast<RendererCL<T, float>*>(m_Renderers[i].get()))
				rendererCL->SubBatchPercentPerThread(m_FinalRenderDialog->OpenCLSubBatchPct());
		}
	}
	else
	{
		ok = false;
		m_Fractorium->ShowCritical("Renderer Creation Error", "No renderer present, aborting. See info tab for details.");
	}

	return ok;
}

/// <summary>
/// Set values for scale spinners based on the ratio of the original dimensions to the current dimensions
/// of the current ember. Also update the size suffix text.
/// </summary>
/// <param name="scale">Whether to update the scale values</param>
/// <param name="size">Whether to update the size suffix text</param>
/// <param name="dowidth">Whether to apply width value to the width scale spinner</param>
/// <param name="doheight">Whether to apply height value to the height scale spinner</param>
template <typename T>
void FinalRenderEmberController<T>::SyncCurrentToSizeSpinners(bool scale, bool size, bool doWidth, bool doHeight)
{
	if (scale)
	{
		if (doWidth)
			m_FinalRenderDialog->m_WidthScaleSpin->SetValueStealth(double(m_Ember->m_FinalRasW) / m_Ember->m_OrigFinalRasW);//Work backward to determine the scale.

		if (doHeight)
			m_FinalRenderDialog->m_HeightScaleSpin->SetValueStealth(double(m_Ember->m_FinalRasH) / m_Ember->m_OrigFinalRasH);
	}

	if (size)
	{
		if (doWidth)
			m_FinalRenderDialog->m_WidthSpinnerWidget->m_SpinBox->SetValueStealth(m_Ember->m_FinalRasW);

		if (doHeight)
			m_FinalRenderDialog->m_HeightSpinnerWidget->m_SpinBox->SetValueStealth(m_Ember->m_FinalRasH);
	}
}

/// <summary>
/// Reset the progress bars.
/// </summary>
/// <param name="total">True to reset render image and total progress bars, else false to only do iter, filter and accum bars.</param>
template <typename T>
void FinalRenderEmberController<T>::ResetProgress(bool total)
{
	if (total)
	{
		QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderImageCountLabel, "setText",  Qt::QueuedConnection, Q_ARG(const QString&, "0 / " + ToString<qulonglong>(m_ImageCount)));
		QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderTotalProgress,   "setValue", Qt::QueuedConnection, Q_ARG(int, 0));
	}

	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderIterationProgress, "setValue", Qt::QueuedConnection, Q_ARG(int, 0));
	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderFilteringProgress, "setValue", Qt::QueuedConnection, Q_ARG(int, 0));
	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderAccumProgress,     "setValue", Qt::QueuedConnection, Q_ARG(int, 0));
}

/// <summary>
/// Set various parameters in the renderers and current ember with the values
/// specified in the widgets and compute the amount of memory required to render.
/// This includes the memory needed for the final output image.
/// </summary>
/// <returns>If successful, a tuple specifying the memory required in bytes for the histogram int he first element, the total memory in the second, and the iter count in the last, else zero.</returns>
template <typename T>
tuple<size_t, size_t, size_t> FinalRenderEmberController<T>::SyncAndComputeMemory()
{
	size_t iterCount = 0;
	pair<size_t, size_t> p(0, 0);
	size_t strips;
	uint channels = m_FinalRenderDialog->Ext() == "png" ? 4 : 3;//4 channels for Png, else 3.
	SyncGuiToEmbers();

	if (m_Renderer.get())
	{
		strips = VerifyStrips(m_Ember->m_FinalRasH, m_FinalRenderDialog->Strips(),
		[&](const string & s) {}, [&](const string & s) {}, [&](const string & s) {});
		m_Renderer->SetEmber(*m_Ember, eProcessAction::FULL_RENDER, true);
		m_FinalPreviewRenderer->Render(UINT_MAX, UINT_MAX);
		p = m_Renderer->MemoryRequired(strips, true, m_FinalRenderDialog->DoSequence());
		iterCount = m_Renderer->TotalIterCount(strips);
	}
	else if (!m_Renderers.empty())
	{
		for (auto& renderer : m_Renderers)
		{
			renderer->SetEmber(*m_Ember, eProcessAction::FULL_RENDER, true);
		}

		m_FinalPreviewRenderer->Render(UINT_MAX, UINT_MAX);
		strips = 1;
		p = m_Renderers[0]->MemoryRequired(1, true, m_FinalRenderDialog->DoSequence());
		iterCount = m_Renderers[0]->TotalIterCount(strips);
	}

	m_FinalRenderDialog->m_StripsSpin->setSuffix(" (" + ToString<qulonglong>(strips) + ")");
	return tuple<size_t, size_t, size_t>(p.first, p.second, iterCount);
}

/// <summary>
/// Compose a final output path given a base name.
/// This includes the base path, the prefix, the name, the suffix and the
/// extension.
/// </summary>
/// <param name="name">The base filename to compose a full path for</param>
/// <returns>The fully composed path</returns>
template <typename T>
QString FinalRenderEmberController<T>::ComposePath(const QString& name)
{
	QString path = MakeEnd(m_Settings->SaveFolder(), '/');//Base path.
	QString full = path + m_FinalRenderDialog->Prefix() + name + m_FinalRenderDialog->Suffix() + "." + m_FinalRenderDialog->Ext();
	return EmberFile<T>::UniqueFilename(full);
}

/// <summary>
/// Non-virtual functions declared in FinalRenderEmberController<T>.
/// </summary>

/// <summary>
/// Return either m_Renderer in the case of running a CPU renderer, else
/// m_Renderers[0] in the case of running OpenCL.
/// </summary>
/// <returns>The primary renderer</returns>
template <typename T>
EmberNs::Renderer<T, float>* FinalRenderEmberController<T>::FirstOrDefaultRenderer()
{
	if (m_Renderer.get())
		return dynamic_cast<EmberNs::Renderer<T, float>*>(m_Renderer.get());
	else if (!m_Renderers.empty())
		return dynamic_cast<EmberNs::Renderer<T, float>*>(m_Renderers[0].get());
	else
	{
		throw "No final renderer, exiting.";
		return nullptr;
	}
}

/// <summary>
/// Save the output of the last rendered image using the existing image output buffer in the renderer.
/// </summary>
/// <returns>The full path and filename the image was saved to.</returns>
template<typename T>
QString FinalRenderEmberController<T>::SaveCurrentAgain()
{
    if (!m_Ember)
        return "";

    m_Fractorium->m_Controller->ParamsToEmber(*m_Ember, true); // update color and filter params
    m_Run = true;
    m_Ember->m_TemporalSamples = 1;
    m_Renderer->SetEmber(*m_Ember, eProcessAction::FILTER_AND_ACCUM);
    m_Renderer->Run(m_FinalImage, 0,  m_GuiState.m_Strips, m_GuiState.m_YAxisUp);
    m_Run = false;
    return SaveCurrentRender(*m_Ember);
}

/// <summary>
/// Save the output of the render.
/// </summary>
/// <param name="ember">The ember whose rendered output will be saved</param>
/// <returns>The full path and filename the image was saved to.</returns>
template<typename T>
QString FinalRenderEmberController<T>::SaveCurrentRender(Ember<T>& ember)
{
	auto comments = m_Renderer->ImageComments(m_Stats, 0, true);
	return SaveCurrentRender(ember, comments, m_FinalImage, m_Renderer->FinalRasW(), m_Renderer->FinalRasH(), m_FinalRenderDialog->Png16Bit(), m_FinalRenderDialog->Transparency());
}

/// <summary>
/// Save the output of the render.
/// </summary>
/// <param name="ember">The ember whose rendered output will be saved</param>
/// <param name="comments">The comments to save in the png, jpg or exr</param>
/// <param name="pixels">The buffer containing the pixels</param>
/// <param name="width">The width in pixels of the image</param>
/// <param name="height">The height in pixels of the image</param>
/// <param name="png16Bit">Whether to use 16 bits per channel per pixel when saving as Png/32-bits per channel when saving as Exr.</param>
/// <param name="transparency">Whether to use alpha when saving as Png or Exr.</param>
/// <returns>The full path and filename the image was saved to.</returns>
template<typename T>
QString FinalRenderEmberController<T>::SaveCurrentRender(Ember<T>& ember, const EmberImageComments& comments, vector<v4F>& pixels, size_t width, size_t height, bool png16Bit, bool transparency)
{
	QString filename = ComposePath(QString::fromStdString(ember.m_Name));
	FractoriumEmberControllerBase::SaveCurrentRender(filename, comments, pixels, width, height, png16Bit, transparency);
	return filename;
}

/// <summary>
/// Action to take when rendering an image completes.
/// Thin wrapper around the function of the same name that takes more arguments.
/// Just passes m_Renderer and m_FinalImage.
/// </summary>
/// <param name="ember">The ember currently being rendered</param>
template<typename T>
void FinalRenderEmberController<T>::RenderComplete(Ember<T>& ember)
{
	if (auto renderer = dynamic_cast<EmberNs::Renderer<T, float>*>(m_Renderer.get()))
		RenderComplete(ember, m_Stats, m_RenderTimer);
}

/// <summary>
/// Pause or resume the renderer(s).
/// </summary>
/// <param name="pause">True to pause, false to unpause.</param>
template<typename T>
void FinalRenderEmberController<T>::Pause(bool pause)
{
	if (m_Renderer.get())
	{
		m_Renderer->Pause(pause);
	}
	else
	{
		for (auto& r : m_Renderers)
			r->Pause(pause);
	}
}

/// <summary>
/// Retrieve the paused state of the renderer(s).
/// </summary>
/// <returns>True if the renderer(s) is paused, else false.</returns>
template<typename T>
bool FinalRenderEmberController<T>::Paused()
{
	if (m_Renderer.get())
	{
		return m_Renderer->Paused();
	}
	else
	{
		bool b = !m_Renderers.empty();

		for (auto& r : m_Renderers)
			b &= r->Paused();

		return b;
	}
}

/// <summary>
/// Handle setting the appropriate progress bar values when an image render has finished.
/// This handles single image, animations, and strips.
/// </summary>
template<typename T>
void FinalRenderEmberController<T>::HandleFinishedProgress()
{
	auto finishedCountCached = m_FinishedImageCount.load();//Make sure to use the same value throughout this function even if the atomic is changing.
	bool doAll = m_GuiState.m_DoAll && m_EmberFile.Size() > 1;

	if (m_FinishedImageCount.load() != m_ImageCount)
		ResetProgress(false);
	else
		SetProgressComplete(100);//Just to be safe.

	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderTotalProgress, "setValue", Qt::QueuedConnection, Q_ARG(int, int((float(finishedCountCached) / float(m_ImageCount)) * 100)));
	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderImageCountLabel, "setText", Qt::QueuedConnection, Q_ARG(const QString&, ToString<qulonglong>(finishedCountCached) + " / " + ToString<qulonglong>(m_ImageCount)));
	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderSaveAgainAsButton, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, !doAll && m_Renderer.get() && m_GuiState.m_Strips == 1));
	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderBumpQualityStartButton, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, !doAll && m_Renderer.get() && m_GuiState.m_Strips == 1));
}

/// <summary>
/// Action to take when rendering an image completes.
/// </summary>
/// <param name="ember">The ember currently being rendered</param>
/// <param name="stats">The renderer stats</param>
/// <param name="renderTimer">The timer which was started at the beginning of the render</param>
template<typename T>
void FinalRenderEmberController<T>::RenderComplete(Ember<T>& ember, const EmberStats& stats, Timing& renderTimer)
{
	rlg l(m_ProgressCs);
	string renderTimeString = renderTimer.Format(renderTimer.Toc());
	QString status, filename = ComposePath(QString::fromStdString(ember.m_Name));
	QString itersString = ToString<qulonglong>(stats.m_Iters);
	QString itersPerSecString = ToString<qulonglong>(size_t(stats.m_Iters / (stats.m_IterMs / 1000.0)));

	if (m_GuiState.m_SaveXml)
	{
		QFileInfo xmlFileInfo(filename);//Create another one in case it was modified for batch rendering.
		QString newPath = xmlFileInfo.absolutePath() + '/' + xmlFileInfo.completeBaseName() + ".flame";
		xmlDocPtr tempEdit = ember.m_Edits;
		ember.m_Edits = m_XmlWriter.CreateNewEditdoc(&ember, nullptr, "edit", m_Settings->Nick().toStdString(), m_Settings->Url().toStdString(), m_Settings->Id().toStdString(), "", 0, 0);
		m_XmlWriter.Save(newPath.toStdString().c_str(), ember, 0, true, false, true);//Note that the ember passed is used, rather than m_Ember because it's what was actually rendered.

		if (tempEdit)
			xmlFreeDoc(tempEdit);
	}

	status = "Render time: " + QString::fromStdString(renderTimeString);
	Output(status);
	status = "Total iters: " + itersString + "\nIters/second: " + itersPerSecString + "\n";
	Output(status);
	QMetaObject::invokeMethod(m_FinalRenderDialog, "MoveCursorToEnd", Qt::QueuedConnection);

	if (m_FinishedImageCount.load() == m_ImageCount)//Finished, save whatever options were specified on the GUI to the settings.
	{
		m_Settings->FinalEarlyClip(m_GuiState.m_EarlyClip);
		m_Settings->FinalYAxisUp(m_GuiState.m_YAxisUp);
		m_Settings->FinalTransparency(m_GuiState.m_Transparency);
		m_Settings->FinalOpenCL(m_GuiState.m_OpenCL);
		m_Settings->FinalDouble(m_GuiState.m_Double);
		m_Settings->FinalDevices(m_GuiState.m_Devices);
		m_Settings->FinalSaveXml(m_GuiState.m_SaveXml);
		m_Settings->FinalDoAll(m_GuiState.m_DoAll);
		m_Settings->FinalDoSequence(m_GuiState.m_DoSequence);
		m_Settings->FinalPng16Bit(m_GuiState.m_Png16Bit);
		m_Settings->FinalKeepAspect(m_GuiState.m_KeepAspect);
		m_Settings->FinalScale(uint(m_GuiState.m_Scale));
		m_Settings->FinalExt(m_GuiState.m_Ext);
		m_Settings->FinalThreadCount(m_GuiState.m_ThreadCount);
		m_Settings->FinalThreadPriority(m_GuiState.m_ThreadPriority);
		m_Settings->FinalOpenCLSubBatchPct(m_GuiState.m_SubBatchPct);
		m_Settings->FinalQuality(m_GuiState.m_Quality);
		m_Settings->FinalTemporalSamples(m_GuiState.m_TemporalSamples);
		m_Settings->FinalSupersample(m_GuiState.m_Supersample);
		m_Settings->FinalStrips(m_GuiState.m_Strips);
	}

	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderTextOutput, "update", Qt::QueuedConnection);
}

/// <summary>
/// Copy widget values to the ember passed in.
/// </summary>
/// <param name="ember">The ember whose values will be modified</param>
/// <param name="widthOverride">Width override to use instead of scaling the original width</param>
/// <param name="heightOverride">Height override to use instead of scaling the original height</param>
/// <param name="dowidth">Whether to use the computed/overridden width value, or use the existing value in the ember</param>
/// <param name="doheight">Whether to use the computed/overridden height value, or use the existing value in the ember</param>
template <typename T>
void FinalRenderEmberController<T>::SyncGuiToEmber(Ember<T>& ember, size_t widthOverride, size_t heightOverride, bool dowidth, bool doheight)
{
	size_t w;
	size_t h;

	if (widthOverride && heightOverride)
	{
		w = widthOverride;
		h = heightOverride;
	}
	else
	{
		double wScale = m_FinalRenderDialog->m_WidthScaleSpin->value();
		double hScale = m_FinalRenderDialog->m_HeightScaleSpin->value();
		w = ember.m_OrigFinalRasW * wScale;
		h = ember.m_OrigFinalRasH * hScale;
	}

	w = dowidth ? std::max<size_t>(w, 10) : ember.m_FinalRasW;
	h = doheight ? std::max<size_t>(h, 10) : ember.m_FinalRasH;
	ember.SetSizeAndAdjustScale(w, h, false, m_FinalRenderDialog->Scale());
	ember.m_Quality = m_FinalRenderDialog->m_QualitySpin->value();
	ember.m_Supersample = m_FinalRenderDialog->m_SupersampleSpin->value();
}

/// <summary>
/// Set the iteration, density filter, and final accumulation progress bars to the same value.
/// Usually 0 or 100.
/// </summary>
/// <param name="val">The value to set them to</param>
template <typename T>
void FinalRenderEmberController<T>::SetProgressComplete(int val)
{
	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderIterationProgress, "setValue", Qt::QueuedConnection, Q_ARG(int, val));//Just to be safe.
	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderFilteringProgress, "setValue", Qt::QueuedConnection, Q_ARG(int, val));
	QMetaObject::invokeMethod(m_FinalRenderDialog->ui.FinalRenderAccumProgress,		"setValue", Qt::QueuedConnection, Q_ARG(int, val));
}

/// <summary>
/// Check if the amount of required memory is greater than that available on
/// all required OpenCL devices. Also check if enough space is available for the max allocation.
/// No check is done for CPU renders.
/// Report errors if not enough memory is available for any of the selected devices.
/// </summary>
/// <returns>A string with an error report if required memory exceeds available memory on any device, else empty string.</returns>
template <typename T>
QString FinalRenderEmberController<T>::CheckMemory(const tuple<size_t, size_t, size_t>& p)
{
	bool error = false;
	QString s;
	size_t histSize = get<0>(p);
	size_t totalSize = get<1>(p);
	auto selectedDevices = m_FinalRenderDialog->Devices();
	static vector<RendererCL<T, float>*> clRenderers;
	clRenderers.clear();

	//Find all OpenCL renderers currently being used and place them in a vector of pointers.
	if (m_FinalRenderDialog->DoSequence())
	{
		for (auto& r : m_Renderers)
			if (auto clr = dynamic_cast<RendererCL<T, float>*>(r.get()))
				clRenderers.push_back(clr);
	}
	else
	{
		if (auto clr = dynamic_cast<RendererCL<T, float>*>(m_Renderer.get()))
			clRenderers.push_back(clr);
	}

	//Iterate through each renderer and examine each device it's using.
	for (auto r : clRenderers)
	{
		auto& devices = r->Devices();

		for (auto& d : devices)
		{
			auto& wrapper = d->m_Wrapper;
			auto index = wrapper.TotalDeviceIndex();

			if (selectedDevices.contains(int(index)))
			{
				bool err = false;
				QString temp;
				size_t maxAlloc = wrapper.MaxAllocSize();
				size_t totalAvail = wrapper.GlobalMemSize();

				if (histSize > maxAlloc)
				{
					err = true;
					temp = "Histogram/Accumulator memory size of " + ToString<qulonglong>(histSize) +
						   " is greater than the max OpenCL allocation size of " + ToString<qulonglong>(maxAlloc);
				}

				if (totalSize > totalAvail)
				{
					if (err)
						temp += "\n\n";

					temp += "Total required memory size of " + ToString<qulonglong>(totalSize) +
							" is greater than the max OpenCL available memory of " + ToString<qulonglong>(totalAvail);
				}

				if (!temp.isEmpty())
				{
					error = true;
					s += QString::fromStdString(wrapper.DeviceName()) + ":\n" + temp + "\n\n";
				}
			}
		}
	}

	if (!s.isEmpty())
		s += "Rendering will most likely fail.\n\nMake strips > 1 to fix this. Strips must divide into the height evenly, and will also scale the number of iterations performed.";

	return s;
}

/// <summary>
/// Thin derivation to handle preview rendering that is specific to the final render dialog.
/// This differs from the preview renderers on the main window because they render multiple embers
/// to a tree, whereas this renders a single preview.
/// </summary>
/// <param name="start">Ignored</param>
/// <param name="end">Ignored</param>
template <typename T>
void FinalRenderPreviewRenderer<T>::PreviewRenderFunc(uint start, uint end)
{
	T scalePercentage;
	size_t maxDim = 100;
	auto d = m_Controller->m_FinalRenderDialog;
	QLabel* widget = d->ui.FinalRenderPreviewLabel;
	//Determine how to scale the scaled ember to fit in the label with a max of 100x100.
	auto e = m_Controller->m_Ember;
	auto settings = FractoriumSettings::Instance();

	if (e->m_FinalRasW >= e->m_FinalRasH)
		scalePercentage = T(maxDim) / e->m_FinalRasW;
	else
		scalePercentage = T(maxDim) / e->m_FinalRasH;

	m_PreviewEmber = *e;
	m_PreviewEmber.m_Quality = 100;
	m_PreviewEmber.m_TemporalSamples = 1;
	m_PreviewEmber.m_FinalRasW = std::max<size_t>(1, std::min<size_t>(maxDim, size_t(scalePercentage * e->m_FinalRasW)));//Ensure neither is zero.
	m_PreviewEmber.m_FinalRasH = std::max<size_t>(1, std::min<size_t>(maxDim, size_t(scalePercentage * e->m_FinalRasH)));
	m_PreviewEmber.m_PixelsPerUnit = scalePercentage * e->m_PixelsPerUnit;
	m_PreviewRenderer.EarlyClip(d->EarlyClip());
	m_PreviewRenderer.YAxisUp(d->YAxisUp());
	m_PreviewRenderer.Callback(nullptr);
	m_PreviewRenderer.SetEmber(m_PreviewEmber);
	m_PreviewRenderer.PrepFinalAccumVector(m_PreviewFinalImage);//Must manually call this first because it could be erroneously made smaller due to strips if called inside Renderer::Run().
	auto strips = VerifyStrips(m_PreviewEmber.m_FinalRasH, d->Strips(),
	[&](const string & s) {}, [&](const string & s) {}, [&](const string & s) {});
	StripsRender<T>(&m_PreviewRenderer, m_PreviewEmber, m_PreviewFinalImage, 0, strips, d->YAxisUp(),
	[&](size_t strip) {},//Pre strip.
	[&](size_t strip) {},//Post strip.
	[&](size_t strip) {},//Error.
	[&](Ember<T>& finalEmber)//Final strip.
	{
		m_PreviewVec.resize(finalEmber.m_FinalRasW * finalEmber.m_FinalRasH * 4);
		Rgba32ToRgba8(m_PreviewFinalImage.data(), m_PreviewVec.data(), finalEmber.m_FinalRasW, finalEmber.m_FinalRasH, d->Transparency());
		QImage image(int(finalEmber.m_FinalRasW), int(finalEmber.m_FinalRasH), QImage::Format_RGBA8888);//The label wants RGBA.
		memcpy(image.scanLine(0), m_PreviewVec.data(), SizeOf(m_PreviewVec));//Memcpy the data in.
		QPixmap pixmap(QPixmap::fromImage(image));
		QMetaObject::invokeMethod(widget, "setPixmap", Qt::QueuedConnection, Q_ARG(QPixmap, pixmap));
	});
}

template class FinalRenderEmberController<float>;

#ifdef DO_DOUBLE
	template class FinalRenderEmberController<double>;
#endif
