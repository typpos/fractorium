// This is a combination of this:
// https://stackoverflow.com/questions/25379422/b-spline-curves/25379851#25379851
// and this, but modified to operate on a spline with any number of points intead of just >= 4:
//
// Spline.cc
// CubicSplineLib/
//
// Source file for the "CubicSpline" class. This object facilitates natural
// cubic spline interpolation. Once instantiated the
// constructor builds the spline polynomials on the intervals of the (x, y)
// data provided and retains them for later invocation. Parallelized using
// OpenMP.
//
// Copyright (C) Geoffrey Lentner 2015. All rights reserved.
// See LICENCE file. (GPL v2.0)
//
// contact: Geoffrey Lentner, B.S.
//          Graduate Student / Researcher
//          102 Natural Science Building
//          Department of Physics & Astronomy
//          University of Louisville
//          Louisville, KY 40292 USA
//
// email:   geoffrey.lentner@louisville.edu
//
// updated: 2015-1-19 13:10:30 EST
//
#include "EmberPch.h"
#include "Spline.h"

namespace EmberNs
{
/// <summary>
/// Constructor that takes a vector of x,y points, optionally sorts them
/// and builds the spline values.
/// </summary>
/// <param name="_vals">The vector of x,y points</param>
/// <param name="sorted">True to skip sorting, false to sort.</param>
template<class T>
Spline<T>::Spline(const std::vector<v2T>& _vals, bool sorted)
{
	n = int(_vals.size() - 1);
	vals = _vals;

	// if not suppressed, ensure 'x' elements are in ascending order
	if (!sorted)
		std::sort(vals.begin(), vals.end(), [&](const v2T & lhs, const v2T & rhs) { return lhs.x < rhs.x; });
	BuildSplines();
}

/// <summary>
/// Compute spline values for the passed in points.
/// This only needs to be done once.
/// </summary>
template<class T>
void Spline<T>::BuildSplines()
{
	a.resize(n + 1);
	b.resize(n + 1);
	c.resize(n + 1);
	d.resize(n + 1);
	std::vector<T> w(n);
	std::vector<T> h(n);
	std::vector<T> ftt(n + 1);

	for (int i = 0; i < n; i++)
	{
		w[i] = (vals[i + 1].x - vals[i].x);
		h[i] = (vals[i + 1].y - vals[i].y) / w[i];
	}

	ftt[0] = 0;

	for (int i = 0; i < n - 1; i++)
		ftt[i + 1] = 3 * (h[i + 1] - h[i]) / (w[i + 1] + w[i]);

	ftt[n] = 0;

	for (int i = 0; i < n; i++)
	{
		a[i] = (ftt[i + 1] - ftt[i]) / (6 * w[i]);
		b[i] = ftt[i] / 2;
		c[i] = h[i] - w[i] * (ftt[i + 1] + 2 * ftt[i]) / 6;
		d[i] = vals[i].y;
	}
}

/// <summary>
/// Wrapper to generate y points on the spline for a vector of passed in points.
/// </summary>
/// <param name="newX">The vector of x points to generate spline points for</param>
/// <returns>The vector of computed spline y points.</returns>
template<class T>
std::vector<T> Spline<T>::Interpolate(const std::vector<T>& newX)
{
	std::vector<T> output; output.resize(newX.size());

	for (int i = 0; i < newX.size(); i++)
		output[i] = Interpolate(newX[i]);

	return output;
}

/// <summary>
/// Compute a y point on the spline for a the passed in value of x.
/// </summary>
/// <param name="newX">The x points to compute the spline point for</param>
/// <returns>The computed spline y points.</returns>
template<class T>
T Spline<T>::Interpolate(T newX)
{
	ClampRef(newX, vals[0].x, vals[n].x);
	int j = 0;

	while (j < n && newX > vals[j + 1].x)
		j++;

	auto xmxj = newX - vals[j].x;
	auto output = a[j] * (xmxj * xmxj * xmxj) +
				  b[j] * (xmxj * xmxj) +
				  c[j] * xmxj +
				  d[j];
	return output;
}

template EMBER_API class Spline<float>;
}
