#include "FractoriumPch.h"
#include "Fractorium.h"
#include <QtWidgets/QApplication>

#include <xmmintrin.h>
#include <immintrin.h>
#include <pmmintrin.h>

#ifdef __APPLE__
/// <summary>
/// Export default user data to ./config/fractorium
/// </summary>
void ExportUserData()
{
	auto exec = new QProcess();
	exec->setWorkingDirectory(QCoreApplication::applicationDirPath());
	exec->start("/bin/sh", QStringList() << "fractorium-sh");
}
#endif

/// <summary>
/// Main program entry point for Fractorium.exe.
/// </summary>
/// <param name="argc">The number of command line arguments passed</param>
/// <param name="argv">The command line arguments passed</param>
/// <returns>0 if successful, else 1.</returns>
int main(int argc, char* argv[])
{
	int rv = -1;
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);
#ifdef TEST_CL
	QMessageBox::critical(QApplication::desktop(), "Error", "Fractorium cannot be run in test mode, undefine TEST_CL first.");
	return 1;
#endif
#ifdef ISAAC_FLAM3_DEBUG
	QMessageBox::critical(QApplication::desktop(), "Error", "Fractorium cannot be run in test mode, undefine ISAAC_FLAM3_DEBUG first.");
	return 1;
#endif
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
	auto vf = VarFuncs<float>::Instance();//Create instances that will stay alive until the program exits.
	auto vlf = VariationList<float>::Instance();
	auto palf = PaletteList<float>::Instance();
	auto settings = FractoriumSettings::Instance();
#ifdef DO_DOUBLE
	auto vd = VarFuncs<float>::Instance();
	auto vld = VariationList<double>::Instance();//No further creations should occur after this.
#endif

	try
	{
		//Required for large allocs, else GPU memory usage will be severely limited to small sizes.
		//This must be done in the application and not in the EmberCL DLL.
#ifdef _WIN32
		_putenv_s("GPU_MAX_ALLOC_PERCENT", "100");
#else
		putenv(const_cast<char*>("GPU_MAX_ALLOC_PERCENT=100"));
#endif
		Fractorium w;
		w.show();
#ifdef __APPLE__
		// exporting user data
		ExportUserData();
#endif
		a.installEventFilter(&w);
		rv = a.exec();
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(nullptr, "Fatal Error", QString::fromStdString(e.what()));
	}
	catch (const char* e)
	{
		QMessageBox::critical(nullptr, "Fatal Error", e);
	}

	return rv;
}

