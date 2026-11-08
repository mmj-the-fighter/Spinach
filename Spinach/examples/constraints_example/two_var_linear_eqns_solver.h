#ifndef TWO_VAR_LINEAR_EQNS_SOLVER
#define TWO_VAR_LINEAR_EQNS_SOLVER

#include <cmath>
#include <iostream>
#include <algorithm>
#include <spn_geom.h>
#include <constraints_common.h>

struct Eqn
{
	double a;
	double b;
	double c;
};

inline static double Determinant(const double mat[2][2]) {
	return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
}

inline static bool SolveUsingCrammersRule(spn::Vec2d<double>& X, const Eqn& f, const Eqn& s)
{
	/*
	x = |DX| / |D|
	y = |DY| / |D|
	*/
	double mat[2][2];
	mat[0][0] = f.a; mat[0][1] = f.b;
	mat[1][0] = s.a; mat[1][1] = s.b;
	double m = Determinant(mat);
	if (m == 0) {
		return false;
	}

	mat[0][0] = f.c; mat[0][1] = f.b;
	mat[1][0] = s.c; mat[1][1] = s.b;
	double mx = Determinant(mat);

	mat[0][0] = f.a; mat[0][1] = f.c;
	mat[1][0] = s.a; mat[1][1] = s.c;
	double my = Determinant(mat);

	X.x = mx / m;
	X.y = my / m;

	return true;
}

bool SolveTwoVarLinEqn(double a1, double b1, double a2, double b2, double c1, double c2, spn::Vec2d<double>& x) {
	Eqn f = { a1, b1, c1 };
	Eqn s = { a2, b2, c2 };
	return SolveUsingCrammersRule(x, f, s);
}

#endif
