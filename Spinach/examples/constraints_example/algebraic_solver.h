#ifndef ALGEBRAIC_SOLVER_H
#define ALGEBRAIC_SOLVER_H
#include <iostream>
#include <constraints_common.h>

// Developed with AI assistance for 
// mathematical discussions, 
// review of implementation, and debugging.

//TODO: Optimize code
//TODO: Add documentation

//Helper
inline bool AlgebraicParallelRootSelectionCriteria(
    double x, double y,
    double mx, double my,
    double p2x, double p2y) {
    //DOT(P2Q, P2M) > 0
    return (x - p2x) * (mx - p2x) + (y - p2y) * (my - p2y) > 0;
};

inline bool AlgebraicPerpendicularRootSelectionCriteria(
    double q0x, double q0y,
    double q1x, double q1y,
    double p2x, double p2y,
    double p3x, double p3y
    )
{
   //select the solution which when combined 
   //with p2 and makes a smaller dot product w.r.t mouseLine
    double mx = p3x - p2x;
    double my = p3y - p2y;
    double p2q0dx = q0x - p2x;
    double p2q0dy = q0y - p2y;
    double p2q1dx = q1x - p2x;
    double p2q1dy = q1y - p2y;
    double dot1 = mx * p2q0dx + my * p2q0dy;
    double dot2 = mx * p2q1dx + my * p2q1dy;
    return dot1 > dot2;
}

//TODO: add documentation
void MakeParallelAlgebraically(Line2d& first, Line2d& second, Line2d& mouseLine) {
    spn::Vec2d<double> firstVec = spn::Sub(first.p1, first.p0);
    //Constraint1: firstline and newline should be parrallel
    double A = firstVec.y;
    if (IsCloseToZero(A)) {
        //edge case: first line is horizontal
        double y2 = second.p0.y;
        double D = 1.0;
        double E = 1.0;
        double G = -second.p0.y;
        double LenSq = (mouseLine.p1.x - mouseLine.p0.x) * (mouseLine.p1.x - mouseLine.p0.x) +
            (mouseLine.p1.y - mouseLine.p0.y) * (mouseLine.p1.y - mouseLine.p0.y);
        double H = second.p0.x * second.p0.x + second.p0.y * second.p0.y - LenSq;
        double F = -second.p0.x;
        double I = D;
        double K = E * y2 * y2 + 2.0 * G * y2 + H;
        double J = 2.0 * F;
        double dc = J * J - 4.0 * K * I;
        if (dc < 0) {
            std::cout << "error1 calculating solution\n";
            return;
        }
        double xx0 = (-J + sqrt(dc)) / (2.0 * I);
        double yy0 = y2;
        double xx1 = (-J - sqrt(dc)) / (2.0 * I);
        double yy1 = y2;
        if (AlgebraicParallelRootSelectionCriteria(xx0, yy0,
            mouseLine.p1.x, mouseLine.p1.y,
            second.p0.x, second.p0.y)) {

            second.p1.x = xx0;
            second.p1.y = yy0;
        }
        else if (
            AlgebraicParallelRootSelectionCriteria(xx1, yy1,
                mouseLine.p1.x, mouseLine.p1.y,
                second.p0.x, second.p0.y)
            ) {
            second.p1.x = xx1;
            second.p1.y = yy1;
        }
        return;
    }
    double B = -firstVec.x;
    double C = -(second.p0.x * firstVec.y - second.p0.y * firstVec.x);

    //Constraint2: Length of the mouseLine should be equal to length of the newLine
    double D = 1.0;
    double E = 1.0;
    double F = -second.p0.x;
    double G = -second.p0.y;
    double mx = (mouseLine.p1.x - mouseLine.p0.x);
    double my = (mouseLine.p1.y - mouseLine.p0.y);
    double H = second.p0.x * second.p0.x + second.p0.y * second.p0.y - (mx * mx + my * my);
    double squaredA = A * A;
    double squaredB = B * B;
    double squaredC = C * C;
    double reciprocalA = 1.0 / A;
    double reciprocalAsquared = 1.0 / squaredA;
    double I = (D * squaredB * reciprocalAsquared) + E;
    double J = 2.0 * (B * C * D * reciprocalAsquared - B * F * reciprocalA + G);
    double K = (squaredC * D * reciprocalAsquared) - 2.0 * C * F * reciprocalA + H;

    //substituting first constraint in second constraint and solving quadratic equation
    double d1 = J * J - 4.0 * K * I;
    if (IsNegative(d1)) {
        std::cout << "error1 calculating solution\n";
        return;
    }
    double yy0 = (-J + sqrt(d1)) / (2.0 * I);
    double xx0 = (-B * yy0 - C) * reciprocalA;
    double yy1 = (-J - sqrt(d1)) / (2.0 * I);
    double xx1 = (-B * yy1 - C) * reciprocalA;

    //accept the solution that satisfies the third condtion
    if (AlgebraicParallelRootSelectionCriteria(xx0, yy0,
        mouseLine.p1.x, mouseLine.p1.y,
        second.p0.x, second.p0.y)) {

        second.p1.x = xx0;
        second.p1.y = yy0;
    }
    else if (AlgebraicParallelRootSelectionCriteria(xx1, yy1,
        mouseLine.p1.x, mouseLine.p1.y,
        second.p0.x, second.p0.y)) {
        second.p1.x = xx1;
        second.p1.y = yy1;
    }
    return;
}


void MakePerpendicularAlgebraically(Line2d& first, Line2d& second, Line2d& mouseLine) {
    spn::Vec2d<double> firstVec = spn::Sub(first.p1, first.p0);
    //Constraint1: firstline and newline should be parrallel
    double A = firstVec.x;
    if (IsCloseToZero(A)) {
        //edge case: first line is vertical 
        double y2 = second.p0.y;
        double D = 1.0;
        double E = 1.0;
        double F = -second.p0.x;
        double G = -second.p0.y;
        double mx = (mouseLine.p1.x - mouseLine.p0.x);
        double my = (mouseLine.p1.y - mouseLine.p0.y);
        double H = second.p0.x * second.p0.x + second.p0.y * second.p0.y - (mx * mx + my * my);
        double I = D;
        double K = E * y2 * y2 + 2.0 * G * y2 + H;
        double J = 2.0 * F;
        double dc = J * J - 4.0 * K * I;
        if (dc < 0) {
            std::cout << "error1 calculating solution\n";
            return;
        }
        double xx0 = (-J + sqrt(dc)) / (2.0 * I);
        double yy0 = y2;
        double xx1 = (-J - sqrt(dc)) / (2.0 * I);
        double yy1 = y2;
        if (AlgebraicPerpendicularRootSelectionCriteria(
            xx0,
            yy0,
            xx1,
            yy1,
            second.p0.x,
            second.p0.y,
            mouseLine.p1.x,
            mouseLine.p1.y))
        {

            second.p1.x = xx0;
            second.p1.y = yy0;
        }
        else {
            second.p1.x = xx1;
            second.p1.y = yy1;
        }
        return;
    }
    double B = firstVec.y;
    double C = -(firstVec.x*second.p0.x+firstVec.y*second.p0.y);

    //Constraint2: Length of the mouseLine should be equal to length of the newLine
    double D = 1.0;
    double E = 1.0;
    double F = -second.p0.x;
    double G = -second.p0.y;
    double LenSq = (mouseLine.p1.x - mouseLine.p0.x) * (mouseLine.p1.x - mouseLine.p0.x) +
        (mouseLine.p1.y - mouseLine.p0.y) * (mouseLine.p1.y - mouseLine.p0.y);
    double H = second.p0.x * second.p0.x + second.p0.y * second.p0.y - LenSq;
    double squaredA = A * A;
    double squaredB = B * B;
    double squaredC = C * C;
    double reciprocalA = 1.0 / A;
    double reciprocalAsquared = 1.0 / squaredA;
    double I = (D * squaredB * reciprocalAsquared) + E;
    double J = 2.0 * (B * C * D * reciprocalAsquared - B * F * reciprocalA + G);
    double K = (squaredC * D * reciprocalAsquared) - 2.0 * C * F * reciprocalA + H;

    //substituting first constraint in second constraint and solving quadratic equation
    double d1 = J * J - 4.0 * K * I;
    if (IsNegative(d1)) {
        std::cout << "error1 calculating solution\n";
        return;
    }
    double yy0 = (-J + sqrt(d1)) / (2.0 * I);
    double xx0 = (-B * yy0 - C) * reciprocalA;
    double yy1 = (-J - sqrt(d1)) / (2.0 * I);
    double xx1 = (-B * yy1 - C) * reciprocalA;

    //accept the solution that satisfies the third condtion
    if (AlgebraicPerpendicularRootSelectionCriteria(
        xx0,
        yy0,
        xx1,
        yy1,
        second.p0.x, 
        second.p0.y,
        mouseLine.p1.x, 
        mouseLine.p1.y))
    {
        second.p1.x = xx0;
        second.p1.y = yy0;
    }
    else 
    {
        second.p1.x = xx1;
        second.p1.y = yy1;
    }
    return;
}



#endif