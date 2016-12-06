#include "FractoriumPch.h"
#include "FractoriumSettings.h"

/// <summary>
/// Constructor that passes the parent to the base and sets up reasonable defaults
/// if the settings file was not present or corrupted.
/// </summary>
FractoriumSettings::FractoriumSettings()
#ifdef _WIN32
	: QSettings(QSettings::IniFormat, QSettings::UserScope, "Fractorium", "Fractorium", nullptr)
#else
	: QSettings(QSettings::IniFormat, QSettings::UserScope, "fractorium", "fractorium", nullptr)
#endif
{
	EnsureDefaults();
}

/// <summary>
/// Make sure options have reasonable values in them first.
/// </summary>
void FractoriumSettings::EnsureDefaults()
{
	auto info = OpenCLInfo::Instance();

	if (FinalQuality() == 0)
		FinalQuality(1000);

	if (FinalTemporalSamples() == 0)
		FinalTemporalSamples(100);

	if (FinalSupersample() == 0)
		FinalSupersample(2);

	if (FinalStrips() == 0)
		FinalStrips(1);

	if (XmlTemporalSamples() == 0)
		XmlTemporalSamples(100);

	if (XmlQuality() == 0)
		XmlQuality(1000);

	if (XmlSupersample() == 0)
		XmlSupersample(2);

	if (Devices().empty() && !info->Devices().empty())
		Devices(QList<QVariant> { 0 });

	if (ThreadCount() == 0 || ThreadCount() > Timing::ProcessorCount())
		ThreadCount(std::max(1u, Timing::ProcessorCount() - 1));//Default to one less to keep the UI responsive for first time users.

	if (FinalThreadCount() == 0 || FinalThreadCount() > Timing::ProcessorCount())
		FinalThreadCount(Timing::ProcessorCount());

	FinalThreadPriority(Clamp<int>(FinalThreadPriority(), (int)eThreadPriority::LOWEST, (int)eThreadPriority::HIGHEST));
	CpuSubBatch(std::max(1u, CpuSubBatch()));
	OpenCLSubBatch(std::max(1u, OpenCLSubBatch()));
	RandomCount(std::max(1u, RandomCount()));

	if (FinalScale() > int(eScaleType::SCALE_HEIGHT))
		FinalScale(0);

	if (OpenXmlExt() == "")
		OpenXmlExt("Flame (*.flame)");

	if (SaveXmlExt() == "")
		SaveXmlExt("Flame (*.flame)");

	if (OpenImageExt() == "")
		OpenImageExt("Png (*.png)");

	if (SaveImageExt() == "")
		SaveImageExt("Png (*.png)");

	if (FinalExt() != "jpg" && FinalExt() != "png")
		FinalExt("png");

	QString s = SaveFolder();
	QDir dir(s);

	if (s.isEmpty() || !dir.exists())
	{
		QStringList paths = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);

		if (!paths.empty())
			SaveFolder(paths[0]);
	}
}

/// <summary>
/// Interactive renderer settings.
/// </summary>

bool FractoriumSettings::EarlyClip()							 { return value(EARLYCLIP).toBool();       }
void FractoriumSettings::EarlyClip(bool b)						 { setValue(EARLYCLIP, b);                 }

bool FractoriumSettings::YAxisUp()								 { return value(YAXISUP).toBool();		   }
void FractoriumSettings::YAxisUp(bool b)						 { setValue(YAXISUP, b);				   }

bool FractoriumSettings::Transparency()							 { return value(TRANSPARENCY).toBool();    }
void FractoriumSettings::Transparency(bool b)					 { setValue(TRANSPARENCY, b);              }

bool FractoriumSettings::OpenCL()								 { return value(OPENCL).toBool();          }
void FractoriumSettings::OpenCL(bool b)							 { setValue(OPENCL, b);                    }

bool FractoriumSettings::Double()								 { return value(DOUBLEPRECISION).toBool(); }
void FractoriumSettings::Double(bool b)							 { setValue(DOUBLEPRECISION, b);		   }

bool FractoriumSettings::ShowAllXforms()						 { return value(SHOWALLXFORMS).toBool();   }
void FractoriumSettings::ShowAllXforms(bool b)					 { setValue(SHOWALLXFORMS, b);			   }

bool FractoriumSettings::ToggleType()                            { return value(TOGGLETYPE).toBool();      }
void FractoriumSettings::ToggleType(bool b)                      { setValue(TOGGLETYPE, b);                }

bool FractoriumSettings::ContinuousUpdate()						 { return value(CONTUPDATE).toBool();	   }
void FractoriumSettings::ContinuousUpdate(bool b)				 { setValue(CONTUPDATE, b);				   }

QList<QVariant> FractoriumSettings::Devices()					 { return value(DEVICES).toList();		   }
void FractoriumSettings::Devices(const QList<QVariant>& devices) { setValue(DEVICES, devices);			   }

uint FractoriumSettings::ThreadCount()							 { return value(THREADCOUNT).toUInt();     }
void FractoriumSettings::ThreadCount(uint i)					 { setValue(THREADCOUNT, i);               }

bool FractoriumSettings::CpuDEFilter()							 { return value(CPUDEFILTER).toBool();     }
void FractoriumSettings::CpuDEFilter(bool b)					 { setValue(CPUDEFILTER, b);               }

bool FractoriumSettings::OpenCLDEFilter()						 { return value(OPENCLDEFILTER).toBool();  }
void FractoriumSettings::OpenCLDEFilter(bool b)					 { setValue(OPENCLDEFILTER, b);            }

uint FractoriumSettings::CpuSubBatch()							 { return value(CPUSUBBATCH).toUInt();	   }
void FractoriumSettings::CpuSubBatch(uint i)					 { setValue(CPUSUBBATCH, i);			   }

uint FractoriumSettings::OpenCLSubBatch()						 { return value(OPENCLSUBBATCH).toUInt();  }
void FractoriumSettings::OpenCLSubBatch(uint i)					 { setValue(OPENCLSUBBATCH, i);			   }

uint FractoriumSettings::RandomCount()							 { return value(RANDOMCOUNT).toUInt();	   }
void FractoriumSettings::RandomCount(uint i)					 { setValue(RANDOMCOUNT, i);			   }

/// <summary>
/// Sequence generation settings.
/// </summary>

double FractoriumSettings::Stagger()             { return value(STAGGER).toDouble();       }
void FractoriumSettings::Stagger(double d)       { setValue(STAGGER, d);                   }

double FractoriumSettings::StaggerMax()          { return value(STAGGERMAX).toDouble();    }
void FractoriumSettings::StaggerMax(double d)    { setValue(STAGGERMAX, d);                }

uint FractoriumSettings::FramesPerRot()          { return value(FRAMESPERROT).toUInt();    }
void FractoriumSettings::FramesPerRot(uint i)    { setValue(FRAMESPERROT, i);              }

uint FractoriumSettings::FramesPerRotMax()       { return value(FRAMESPERROTMAX).toUInt(); }
void FractoriumSettings::FramesPerRotMax(uint i) { setValue(FRAMESPERROTMAX, i);           }

uint FractoriumSettings::Rotations()             { return value(ROTATIONS).toDouble();     }
void FractoriumSettings::Rotations(double d)     { setValue(ROTATIONS, d);                 }

uint FractoriumSettings::RotationsMax()          { return value(ROTATIONSMAX).toDouble();  }
void FractoriumSettings::RotationsMax(double d)  { setValue(ROTATIONSMAX, d);              }

uint FractoriumSettings::BlendFrames()           { return value(BLENDFRAMES).toUInt();     }
void FractoriumSettings::BlendFrames(uint i)     { setValue(BLENDFRAMES, i);               }

uint FractoriumSettings::BlendFramesMax()        { return value(BLENDFRAMESMAX).toUInt();  }
void FractoriumSettings::BlendFramesMax(uint i)  { setValue(BLENDFRAMESMAX, i);            }

/// <summary>
/// Variations filter settings.
/// </summary>

int  FractoriumSettings::VarFilterSum()           { return value(VARFILTERSUM).toInt();      }
void FractoriumSettings::VarFilterSum(int i)      { setValue(VARFILTERSUM, i);               }

int  FractoriumSettings::VarFilterAssign()        { return value(VARFILTERASSIGN).toInt();   }
void FractoriumSettings::VarFilterAssign(int i)   { setValue(VARFILTERASSIGN, i);            }

int  FractoriumSettings::VarFilterPpsum()         { return value(VARFILTERPPSUM).toInt();    }
void FractoriumSettings::VarFilterPpsum(int i)    { setValue(VARFILTERPPSUM, i);             }

int  FractoriumSettings::VarFilterPpassign()      { return value(VARFILTERPPASSIGN).toInt(); }
void FractoriumSettings::VarFilterPpassign(int i) { setValue(VARFILTERPPASSIGN, i);          }

int  FractoriumSettings::VarFilterSdc()           { return value(VARFILTERSDC).toInt();      }
void FractoriumSettings::VarFilterSdc(int i)      { setValue(VARFILTERSDC, i);               }

int  FractoriumSettings::VarFilterState()         { return value(VARFILTERSTATE).toInt();    }
void FractoriumSettings::VarFilterState(int i)    { setValue(VARFILTERSTATE, i);             }

int  FractoriumSettings::VarFilterParam()         { return value(VARFILTERPARAM).toInt();    }
void FractoriumSettings::VarFilterParam(int i)    { setValue(VARFILTERPARAM, i);             }

int  FractoriumSettings::VarFilterNonparam()      { return value(VARFILTERNONPARAM).toInt(); }
void FractoriumSettings::VarFilterNonparam(int i) { setValue(VARFILTERNONPARAM, i);          }

/// <summary>
/// Final render settings.
/// </summary>

bool FractoriumSettings::FinalEarlyClip()							  { return value(FINALEARLYCLIP).toBool();       }
void FractoriumSettings::FinalEarlyClip(bool b)						  { setValue(FINALEARLYCLIP, b);                 }

bool FractoriumSettings::FinalYAxisUp()								  { return value(FINALYAXISUP).toBool();		 }
void FractoriumSettings::FinalYAxisUp(bool b)						  { setValue(FINALYAXISUP, b);					 }

bool FractoriumSettings::FinalTransparency()						  { return value(FINALTRANSPARENCY).toBool();    }
void FractoriumSettings::FinalTransparency(bool b)					  { setValue(FINALTRANSPARENCY, b);              }

bool FractoriumSettings::FinalOpenCL()								  { return value(FINALOPENCL).toBool();          }
void FractoriumSettings::FinalOpenCL(bool b)						  { setValue(FINALOPENCL, b);                    }

bool FractoriumSettings::FinalDouble()								  { return value(FINALDOUBLEPRECISION).toBool(); }
void FractoriumSettings::FinalDouble(bool b)						  { setValue(FINALDOUBLEPRECISION, b);			 }

bool FractoriumSettings::FinalSaveXml()								  { return value(FINALSAVEXML).toBool();		 }
void FractoriumSettings::FinalSaveXml(bool b)						  { setValue(FINALSAVEXML, b);					 }

bool FractoriumSettings::FinalDoAll()								  { return value(FINALDOALL).toBool();		     }
void FractoriumSettings::FinalDoAll(bool b)							  { setValue(FINALDOALL, b);					 }

bool FractoriumSettings::FinalDoSequence()							  { return value(FINALDOSEQUENCE).toBool();	     }
void FractoriumSettings::FinalDoSequence(bool b)					  { setValue(FINALDOSEQUENCE, b);				 }

bool FractoriumSettings::FinalKeepAspect()							  { return value(FINALKEEPASPECT).toBool();		 }
void FractoriumSettings::FinalKeepAspect(bool b)					  { setValue(FINALKEEPASPECT, b);				 }

uint FractoriumSettings::FinalScale()								  { return value(FINALSCALE).toUInt();			 }
void FractoriumSettings::FinalScale(uint i)							  { setValue(FINALSCALE, i);					 }

QString FractoriumSettings::FinalExt()								  { return value(FINALEXT).toString();			 }
void FractoriumSettings::FinalExt(const QString& s)					  { setValue(FINALEXT, s);						 }

QList<QVariant> FractoriumSettings::FinalDevices()					  { return value(FINALDEVICES).toList();		 }
void FractoriumSettings::FinalDevices(const QList<QVariant>& devices) { setValue(FINALDEVICES, devices);			 }

uint FractoriumSettings::FinalThreadCount()							  { return value(FINALTHREADCOUNT).toUInt();     }
void FractoriumSettings::FinalThreadCount(uint i)					  { setValue(FINALTHREADCOUNT, i);               }

int FractoriumSettings::FinalThreadPriority()						  { return value(FINALTHREADPRIORITY).toInt();   }
void FractoriumSettings::FinalThreadPriority(int i)					  { setValue(FINALTHREADPRIORITY, i);			 }

uint FractoriumSettings::FinalQuality()								  { return value(FINALQUALITY).toUInt();         }
void FractoriumSettings::FinalQuality(uint i)						  { setValue(FINALQUALITY, i);                   }

uint FractoriumSettings::FinalTemporalSamples()						  { return value(FINALTEMPORALSAMPLES).toUInt(); }
void FractoriumSettings::FinalTemporalSamples(uint i)				  { setValue(FINALTEMPORALSAMPLES, i);           }

uint FractoriumSettings::FinalSupersample()							  { return value(FINALSUPERSAMPLE).toUInt();     }
void FractoriumSettings::FinalSupersample(uint i)					  { setValue(FINALSUPERSAMPLE, i);               }

size_t FractoriumSettings::FinalStrips()							  { return value(FINALSTRIPS).toULongLong();	 }
void FractoriumSettings::FinalStrips(size_t i)						  { setValue(FINALSTRIPS, uint(i));				 }

/// <summary>
/// Xml file saving settings.
/// </summary>

uint FractoriumSettings::XmlTemporalSamples()       { return value(XMLTEMPORALSAMPLES).toUInt(); }
void FractoriumSettings::XmlTemporalSamples(uint i) { setValue(XMLTEMPORALSAMPLES, i);           }

uint FractoriumSettings::XmlQuality()               { return value(XMLQUALITY).toUInt();         }
void FractoriumSettings::XmlQuality(uint i)         { setValue(XMLQUALITY, i);                   }

uint FractoriumSettings::XmlSupersample()           { return value(XMLSUPERSAMPLE).toUInt();     }
void FractoriumSettings::XmlSupersample(uint i)     { setValue(XMLSUPERSAMPLE, i);               }

QString FractoriumSettings::Id()                    { return value(IDENTITYID).toString();       }
void FractoriumSettings::Id(const QString& s)       { setValue(IDENTITYID, s);                   }

QString FractoriumSettings::Url()                   { return value(IDENTITYURL).toString();      }
void FractoriumSettings::Url(const QString& s)      { setValue(IDENTITYURL, s);                  }

QString FractoriumSettings::Nick()                  { return value(IDENTITYNICK).toString();     }
void FractoriumSettings::Nick(const QString& s)     { setValue(IDENTITYNICK, s);                 }

/// <summary>
/// General operations settings.
/// </summary>

QString FractoriumSettings::OpenFolder()							  { return value(OPENFOLDER).toString();   }
void FractoriumSettings::OpenFolder(const QString& s)				  { setValue(OPENFOLDER, s);			   }

QString FractoriumSettings::SaveFolder()							  { return value(SAVEFOLDER).toString();   }
void FractoriumSettings::SaveFolder(const QString& s)				  { setValue(SAVEFOLDER, s);			   }

QString FractoriumSettings::OpenXmlExt()							  { return value(OPENXMLEXT).toString();   }
void FractoriumSettings::OpenXmlExt(const QString& s)				  { setValue(OPENXMLEXT, s);			   }

QString FractoriumSettings::SaveXmlExt()							  { return value(SAVEXMLEXT).toString();   }
void FractoriumSettings::SaveXmlExt(const QString& s)				  { setValue(SAVEXMLEXT, s);			   }

QString FractoriumSettings::OpenImageExt()							  { return value(OPENIMAGEEXT).toString(); }
void FractoriumSettings::OpenImageExt(const QString& s)				  { setValue(OPENIMAGEEXT, s);			   }

QString FractoriumSettings::SaveImageExt()							  { return value(SAVEIMAGEEXT).toString(); }
void FractoriumSettings::SaveImageExt(const QString& s)				  { setValue(SAVEIMAGEEXT, s);		       }

bool FractoriumSettings::SaveAutoUnique()							  { return value(AUTOUNIQUE).toBool();	   }
void FractoriumSettings::SaveAutoUnique(bool b)						  { setValue(AUTOUNIQUE, b);			   }

QMap<QString, QVariant> FractoriumSettings::Variations()			  { return value(UIVARIATIONS).toMap();	   }
void FractoriumSettings::Variations(const QMap<QString, QVariant>& m) { setValue(UIVARIATIONS, m);			   }

QString FractoriumSettings::Theme()									  { return value(STYLETHEME).toString();   }
void FractoriumSettings::Theme(const QString& s)					  { setValue(STYLETHEME, s);			   }

