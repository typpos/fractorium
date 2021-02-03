#pragma once

#include "Utils.h"
#include "Isaac.h"
#include "Curves.h"

#define CURVE_POINTS 5

/// <summary>
/// Curves class.
/// </summary>

namespace EmberNs
{
/// <summary>
/// The b-spline curves used to adjust the colors during final accumulation.
/// This functionality was gotten inferred from Chaotica.
/// Note this is now incompatible with Apophysis, which uses Bezier curves instead.
/// </summary>
template <typename T>
class EMBER_API Curves
{
public:
	/// <summary>
	/// Constructor which sets the curve and weight values to their defaults.
	/// </summary>
	Curves(bool init = false)
	{
		if (init)
			Init();
		else
			Clear();
	}

	/// <summary>
	/// Default copy constructor.
	/// </summary>
	/// <param name="curves">The Curves object to copy</param>
	Curves(const Curves<T>& curves)
	{
		Curves<T>::operator=<T>(curves);
	}

	/// <summary>
	/// Copy constructor to copy a Curves object of type U.
	/// Special case that must be here in the header because it has
	/// a second template parameter.
	/// </summary>
	/// <param name="curves">The Curves object to copy</param>
	template <typename U>
	Curves(const Curves<U>& curves)
	{
		Curves<T>::operator=<U>(curves);
	}

	/// <summary>
	/// Default assignment operator.
	/// </summary>
	/// <param name="curves">The Curves object to copy</param>
	Curves<T>& operator = (const Curves<T>& curves)
	{
		if (this != &curves)
			Curves<T>::operator=<T>(curves);

		return *this;
	}

	/// <summary>
	/// Assignment operator to assign a Curves object of type U.
	/// </summary>
	/// <param name="curves">The Curves object to copy</param>
	/// <returns>Reference to updated self</returns>
	template <typename U>
	Curves<T>& operator = (const Curves<U>& curves)
	{
		int i = 0;

		for (auto& pp : curves.m_Points)
		{
			int j = 0;
			m_Points[i].clear();

			for (auto& p : pp)
			{
				m_Points[i].push_back(p);
				j++;
			}

			i++;
		}

		i = 0;
		return *this;
	}

	/// <summary>
	/// Unary addition operator to add a Curves<T> object to this one.
	/// </summary>
	/// <param name="curves">The Curves object to add</param>
	/// <returns>Reference to updated self</returns>
	template <typename U>
	Curves<T>& operator += (const Curves<U>& curves)
	{
		int i = 0;

		for (auto& pp : m_Points)
		{
			int j = 0;

			for (auto& p : pp)
			{
				if (j < curves.m_Points[i].size())
					p += curves.m_Points[i][j];
				else
					break;

				j++;
			}

			i++;
		}

		return *this;
	}

	/// <summary>
	/// Unary multiplication operator to multiply this object by another Curves<T> object.
	/// </summary>
	/// <param name="curves">The Curves object to multiply this one by</param>
	/// <returns>Reference to updated self</returns>
	template <typename U>
	Curves<T>& operator *= (const Curves<U>& curves)
	{
		int i = 0;

		for (auto& pp : m_Points)
		{
			int j = 0;

			for (auto& p : pp)
			{
				if (j < curves.m_Points[i].size())
					p *= curves.m_Points[i][j];
				else
					break;

				j++;
			}

			i++;
		}

		return *this;
	}

	/// <summary>
	/// Unary multiplication operator to multiply this object by a scalar of type T.
	/// </summary>
	/// <param name="t">The scalar to multiply this object by</param>
	/// <returns>Reference to updated self</returns>
	template <typename U>
	Curves<T>& operator *= (const U& t)
	{
		for (auto& pp : m_Points)
			for (auto& p : pp)
				p *= T(t);

		return *this;
	}

	/// <summary>
	/// Set the curve and weight values to their default state.
	/// </summary>
	void Init()
	{
		for (size_t i = 0; i < 4; i++)
			Init(i);
	}

	/// <summary>
	/// Set a specific curve and its weight value to their default state.
	/// </summary>
	void Init(size_t i)
	{
		if (i < 4)
		{
			m_Points[i].resize(5);
			m_Points[i][0] = v2T(0);
			m_Points[i][1] = v2T(T(0.25));
			m_Points[i][2] = v2T(T(0.50));
			m_Points[i][3] = v2T(T(0.75));
			m_Points[i][4] = v2T(1);
		}
	}

	/// <summary>
	/// Set the curve and weight values to an empty state.
	/// </summary>
	void Clear()
	{
		for (auto& p : m_Points)
			p.clear();
	}

	/// <summary>
	/// Whether any points are not the default.
	/// </summary>
	/// <returns>True if any point has been set to a value other than the default, else false.</returns>
	bool CurvesSet()
	{
		bool set = false;

		for (size_t i = 0; i < 4; i++)
		{
			if (m_Points[i].size() != CURVE_POINTS)
			{
				set = true;
				break;
			}

			if ((m_Points[i][0] != v2T(0)) ||
					(m_Points[i][1] != v2T(T(0.25))) ||
					(m_Points[i][2] != v2T(T(0.50))) ||
					(m_Points[i][3] != v2T(T(0.75))) ||
					(m_Points[i][4] != v2T(1))
			   )
			{
				set = true;
				break;
			}
		}

		return set;
	}


public:
	std::array<std::vector<v2T>, 4> m_Points;
};

//Must declare this outside of the class to provide for both orders of parameters.

/// <summary>
/// Multiplication operator to multiply a Curves<T> object by a scalar of type U.
/// </summary>
/// <param name="curves">The curves object to multiply</param>
/// <param name="t">The scalar to multiply curves by by</param>
/// <returns>Copy of new Curves<T></returns>
template <typename T, typename U>
Curves<T> operator * (const Curves<T>& curves, const U& t)
{
	T tt = T(t);
	Curves<T> c(curves);

	for (auto& pp : c.m_Points)
		for (auto& p : pp)
			p *= tt;

	return c;
}

/// <summary>
/// Multiplication operator for reverse order.
/// </summary>
/// <param name="t">The scalar to multiply curves by by</param>
/// <param name="curves">The curves object to multiply</param>
/// <returns>Copy of new Curves<T></returns>
template <typename T, typename U>
Curves<T> operator * (const U& t, const Curves<T>& curves)
{
	return curves * t;
}
}
