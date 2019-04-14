// This is a combination of this:
// https://stackoverflow.com/questions/25379422/b-spline-curves/25379851#25379851
// and this, but modified to operate on a spline with any number of points intead of just >= 4:
//
// Spline.h
// CubicSplineLib/
//
// Header file for the "CubicSpline" class. This object facilitates natural
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
#pragma once
#include "Utils.h"

namespace EmberNs
{
/// <summary>
/// Class taking passed in x,y points, sorting them, and providing a function
/// to compute and return an interpolated spline curve for any value between the
/// first and last x.
/// Template argument expected to be float.
/// </summary>
template<class T = float>
class EMBER_API Spline
{
public:
	Spline(const std::vector<v2T>& _vals, bool sorted = false);
	std::vector<T> Interpolate(const std::vector<T>& newX);
	T Interpolate(T newX);

private:
	void BuildSplines();
	std::vector<v2T> vals;
	std::vector<T> a, b, c, d;
	std::vector<T> c_prime, d_prime;
	std::vector<T> k;
	int n;
};
}
