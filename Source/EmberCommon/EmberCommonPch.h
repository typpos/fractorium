#ifdef _WIN32
	#pragma once
#endif

/// <summary>
/// Precompiled header file. Place all system includes here with appropriate #defines for different operating systems and compilers.
/// </summary>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN//Exclude rarely-used stuff from Windows headers.
#define _USE_MATH_DEFINES

#ifdef _WIN32
	#pragma warning(disable : 4251; disable : 4661; disable : 4100)
	#include <SDKDDKVer.h>
	#include <windows.h>
	#include <winsock.h>//For htons().
	#include <BaseTsd.h>
	#include <crtdbg.h>
	#include <tchar.h>
#else
	#include <arpa/inet.h>
	#include <unistd.h>
	#define _TCHAR char
	#define _tmain main
	#define _T
#endif

#include <iostream>
#include <iomanip>
#include <ostream>
#include <random>
#include <sstream>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jconfig.h"
#include "jpeglib.h"

#define PNG_SKIP_SETJMP_CHECK 1

#include "png.h"

//Ember.
#include "Ember.h"
#include "Variation.h"
#include "EmberToXml.h"
#include "XmlToEmber.h"
#include "PaletteList.h"
#include "Iterator.h"
#include "Renderer.h"
#include "RendererCL.h"
#include "SheepTools.h"

//Options.
#include "SimpleGlob.h"
#include "SimpleOpt.h"

//Exr
#ifdef _WIN32
	#define OPENEXR_DLL 1
#endif

#ifdef __APPLE__
	#include <OpenEXR/ImfRgbaFile.h>
	#include <OpenEXR/ImfStringAttribute.h>
	#include <OpenEXR/half.h>
    // #undef FALSE -- michel
    // #undef TRUE -- michel
    #define ENUM_DYLD_BOOL // michel
    #include <mach-o/dyld.h>
#else
	#include <ImfRgbaFile.h>
	#include <ImfStringAttribute.h>
	#include <half.h>
#endif

using namespace Imf;
using namespace Imath;

using namespace EmberNs;
using namespace EmberCLns;
