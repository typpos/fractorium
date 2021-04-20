#pragma once

#include "EmberDefines.h"

/// <summary>
/// Timing and CriticalSection classes.
/// </summary>

namespace EmberNs
{
/// <summary>
/// Since the algorithm is so computationally intensive, timing and benchmarking are an integral portion
/// of both the development process and the execution results. This class provides an easy way to time
/// things by simply calling its Tic() and Toc() member functions. It also assists with formatting the
/// elapsed time as a string.
/// </summary>
class EMBER_API Timing
{
public:
	/// <summary>
	/// Constructor that takes an optional precision argument which specifies how many digits after the decimal place should be printed for seconds.
	/// As a convenience, the Tic() function is called automatically.
	/// </summary>
	/// <param name="precision">The precision of the seconds field of the elapsed time. Default: 2.</param>
	Timing(int precision = 2)
	{
		m_Precision = precision;
		Init();
		Tic();
	}

	/// <summary>
	/// Set the begin time.
	/// </summary>
	/// <returns>The begin time cast to a double</returns>
	double Tic()
	{
		m_BeginTime = NowMsD();
		return BeginTime();
	}

	/// <summary>
	/// Set the end time and optionally output a string showing the elapsed time.
	/// </summary>
	/// <param name="str">The string to output. Default: nullptr.</param>
	/// <param name="fullString">If true, output the string verbatim, else output the text " processing time: " in between str and the formatted time.</param>
	/// <returns>The elapsed time in milliseconds as a double</returns>
	double Toc(const char* str = nullptr, bool fullString = false)
	{
		m_EndTime = NowMsD();
		const auto ms = ElapsedTime();

		if (str)
		{
			cout << string(str) << (fullString ? "" : " processing time: ") << Format(ms) << "\n";
		}

		return ms;
	}

	/// <summary>
	/// Return the begin time as a double.
	/// </summary>
	/// <returns></returns>
	double BeginTime() const { return static_cast<double>(m_BeginTime.time_since_epoch().count()); }

	/// <summary>
	/// Return the end time as a double.
	/// </summary>
	/// <returns></returns>
	double EndTime() const { return static_cast<double>(m_EndTime.time_since_epoch().count()); }

	/// <summary>
	/// Return the elapsed time in milliseconds.
	/// </summary>
	/// <returns>The elapsed time in milliseconds as a double</returns>
	double ElapsedTime() const
	{
		return (m_EndTime - m_BeginTime).count();
	}

	/// <summary>
	/// Formats a specified milliseconds value as a string.
	/// This uses some intelligence to determine what to return depending on how much time has elapsed.
	/// Days, hours and minutes are only included if 1 or more of them has elapsed. Seconds are always
	/// included as a decimal value with the precision the user specified in the constructor.
	/// </summary>
	/// <param name="ms">The time in milliseconds to format</param>
	/// <returns>The formatted string</returns>
	string Format(double ms) const
	{
		stringstream ss;
		double x = ms / 1000;
		const auto secs = fmod(x, 60);
		x /= 60;
		const auto mins = fmod(x, 60);
		x /= 60;
		const auto hours = fmod(x, 24);
		x /= 24;
		const auto days = x;

		if (days >= 1)
			ss << static_cast<int>(days) << "d ";

		if (hours >= 1)
			ss << static_cast<int>(hours) << "h ";

		if (mins >= 1)
			ss << static_cast<int>(mins) << "m ";

		ss << std::fixed << std::setprecision(m_Precision) << secs << "s";
		return ss.str();
	}

	/// <summary>
	/// Return the number of cores in the system.
	/// </summary>
	/// <returns>The number of cores in the system</returns>
	static uint ProcessorCount()
	{
		Init();
		return m_ProcessorCount;
	}

private:
	/// <summary>
	/// Query and store the performance info of the system.
	/// Since it will never change it only needs to be queried once.
	/// This is achieved by keeping static state and performance variables.
	/// </summary>
	static void Init()
	{
		if (!m_TimingInit)
		{
			m_ProcessorCount = thread::hardware_concurrency();
			m_TimingInit = true;
		}
	}

	int m_Precision;//How many digits after the decimal place to print for seconds.
	DoubleMsTimePoint m_BeginTime;//The start of the timing, set with Tic().
	DoubleMsTimePoint m_EndTime;//The end of the timing, set with Toc().
	static bool m_TimingInit;//Whether the performance info has bee queried.
	static uint m_ProcessorCount;//The number of cores on the system, set in Init().
};
}
