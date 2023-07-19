#ifdef _WIN32
	#pragma once
#endif

/// <summary>
/// Precompiled header file. Place all system includes here with appropriate #defines for different operating systems and compilers.
/// </summary>

#define NOMINMAX
#define _USE_MATH_DEFINES
#define __TBB_NO_IMPLICIT_LINKAGE 1//Prevent tbb from automatically looking for tbb_debug.lib. We only care about the release tbb.lib/dll.

#ifdef _WIN32
	#pragma warning(disable : 4251; disable : 4661; disable : 4100)
	#define basename(x) _strdup(x)
	#define WIN32_LEAN_AND_MEAN
	#define EMBER_OS "WIN"

	#include <SDKDDKVer.h>
	#include <windows.h>
#elif __APPLE__
	#define EMBER_OS "OSX"
#else
	#include <libgen.h>
	#include <unistd.h>
	#define EMBER_OS "LNX"
#endif

//Standard headers.
#include <algorithm>
#include <array>
#include <chrono>
#include <complex>
#include <cstdint>
#include <fstream>
#include <functional>
#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include <limits>
#include <list>
#ifdef __APPLE__
	#include <malloc/malloc.h>
    #include <sys/sysctl.h>
#else
	#include <malloc.h>
#endif
#include <map>
#include <math.h>
#include <memory>
#include <mutex>
#include <numeric>
#include <ostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <thread>
#include <time.h>
#include <type_traits>
#include <vector>
#include <unordered_map>

//Third party headers.
#ifdef _WIN32
	#include "libxml/parser.h"
#else
	#include "libxml2/libxml/parser.h"
#endif

#define GLM_FORCE_RADIANS 1
#define GLM_ENABLE_EXPERIMENTAL 1

#ifndef __APPLE__
	#define GLM_FORCE_INLINE 1
#endif

//glm is what's used for matrix math.
#include <glm/glm.hpp>
#if GLM_VERSION <= 990
	#include <glm/detail/type_int.hpp>
#endif
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace std::chrono;
using namespace glm;
using namespace glm::detail;
using glm::uint;
using glm::uint16;
