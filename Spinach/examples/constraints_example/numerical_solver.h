#ifndef NUMERICAL_SOLVER
#define NUMERICAL_SOLVER

// Developed with AI assistance for 
// mathematical discussions, 
// review of implementation, and debugging.

//TODO: Optimize code
//TODO: Add documentation

#include <constraints_common.h>
#include <two_var_linear_eqns_solver.h>

//types
using ConstraintFn = double (*)(double, double, Line2d&, Line2d&);
using JacobianMatrix = double[2][2];
using JxFn = void (*)(JacobianMatrix, Line2d&, Line2d&, double, double);

//parallelism constraints
inline static double Cnst1_Parallel_Numerical(double x, double y, Line2d& first, Line2d& second) {
    double x0 = first.p0.x;
    double y0 = first.p0.y;
    double x1 = first.p1.x;
    double y1 = first.p1.y;
    double x2 = second.p0.x;
    double y2 = second.p0.y;
    return (x1 - x0) * (y - y2) - (y1 - y0) * (x - x2);
}

inline static double Cnst2_Parallel_Numerical(double x, double y, Line2d& second, Line2d& mouseLine) {
    double x2 = second.p0.x;
    double y2 = second.p0.y;
    double x3 = mouseLine.p1.x;
    double y3 = mouseLine.p1.y;
    double L = (x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2);
    return (x - x2) * (x - x2) + (y - y2) * (y - y2) - L;
}

//perpendicularity constraints
inline static double Cnst1_Perpendicular_Numerical(double x, double y, Line2d& first, Line2d& second) {
    double x0 = first.p0.x;
    double y0 = first.p0.y;
    double x1 = first.p1.x;
    double y1 = first.p1.y;
    double x2 = second.p0.x;
    double y2 = second.p0.y;
    return (x1 - x0) * (x - x2) + (y1 - y0) * (y - y2);
}

inline static double Cnst2_Perpendicular_Numerical(double x, double y, Line2d& second, Line2d& mouseLine) {
    double x2 = second.p0.x;
    double y2 = second.p0.y;
    double x3 = mouseLine.p1.x;
    double y3 = mouseLine.p1.y;
    double L = (x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2);
    return (x - x2) * (x - x2) + (y - y2) * (y - y2) - L;
}

//Jacobian matrix preparation for parallelism
inline static void JXkParallel(JacobianMatrix j,
    Line2d& first, Line2d& second,
    double Xkx, double Xky) {
    double x0 = first.p0.x;
    double y0 = first.p0.y;
    double x1 = first.p1.x;
    double y1 = first.p1.y;
    double x2 = second.p0.x;
    double y2 = second.p0.y;
    j[0][0] = -(y1 - y0);
    j[0][1] = x1 - x0;
    j[1][0] = 2.0 * (Xkx - x2);
    j[1][1] = 2.0 * (Xky - y2);
}

//Jacobian matrix preparation perpendicularity
inline static void JXkPerpendicular(JacobianMatrix j,
    Line2d& first, Line2d& second,
    double Xkx, double Xky) {
    double x0 = first.p0.x;
    double y0 = first.p0.y;
    double x1 = first.p1.x;
    double y1 = first.p1.y;
    double x2 = second.p0.x;
    double y2 = second.p0.y;
    j[0][0] = x1 - x0;
    j[0][1] = y1 - y0;
    j[1][0] = 2.0 * (Xkx - x2);
    j[1][1] = 2.0 * (Xky - y2);
}

//Newton-Raphson root finding algorithm implementation
inline static bool NewtonRaphson(
    Line2d& first, Line2d& second, 
    Line2d& mouseLine, 
    ConstraintFn cnst1, 
    ConstraintFn cnst2, 
    JxFn Jx) {
    double Xkx = mouseLine.p1.x;
    double Xky = mouseLine.p1.y;
    JacobianMatrix j;
    constexpr int maxIterations = 50;
    for (int i = 0; i < maxIterations; i++) {
        double c1 = -cnst1(Xkx, Xky, first, second);
        double c2 = -cnst2(Xkx, Xky, second, mouseLine);
        if (IsCloseToZero(c1) && IsCloseToZero(c2)) {
            second.p1.x = Xkx;
            second.p1.y = Xky;
            return true;
        }
        Jx(j, first, second, Xkx, Xky);
        double a1 = j[0][0];
        double b1 = j[0][1];
        double a2 = j[1][0];
        double b2 = j[1][1];
        spn::Vec2d<double> delta;
        if (!SolveTwoVarLinEqn(a1, b1, a2, b2, c1, c2, delta)) {
            return false;
        }
        Xkx += delta.x;
        Xky += delta.y;
    }
    return false;
}

//api
inline static bool MakeParallelNumerically(Line2d& first, Line2d& second, Line2d& mouseLine) {
    return NewtonRaphson(
        first, second, mouseLine,
        Cnst1_Parallel_Numerical, 
        Cnst2_Parallel_Numerical,
        JXkParallel);
 }

//api
inline static bool MakePerpendicularNumerically(Line2d& first, Line2d& second, Line2d& mouseLine) {
    return NewtonRaphson(
        first, second, mouseLine,
        Cnst1_Perpendicular_Numerical, 
        Cnst2_Perpendicular_Numerical, 
        JXkPerpendicular);
}

#endif