#ifndef SNAP_SOLVER
#define SNAP_SOLVER

// Developed with AI assistance for 
// mathematical discussions, 
// review of implementation, and debugging.

//TODO: Optimize code
//TODO: Add documentation

#include <constraints_common.h>
#include <spn_geom.h>

//Direct Snap Solver for Parllelism
void MakeParallelSnap(Line2d& first, Line2d& second) {
    double distanceSecond = spn::Distance(second.p1, second.p0);
    double distanceFirst = spn::Distance(first.p1, first.p0);
    spn::Vec2d<double> firstVec = spn::Sub(first.p1, first.p0);
    spn::Vec2d<double> secondVec = spn::Sub(second.p1, second.p0);
    double sign = spn::Dot(firstVec, secondVec) < 0.0 ? -1.0 : 1.0;
    double ndx = firstVec.x / distanceFirst;
    double ndy = firstVec.y / distanceFirst;
    second.p1.x = second.p0.x + sign * ndx * distanceSecond;
    second.p1.y = second.p0.y + sign * ndy * distanceSecond;
    double distanceSecond2 = spn::Distance(second.p1, second.p0);
    double delta = distanceSecond2 - distanceSecond;
    //std::cout << delta << " parallel error \n";
}

//Direct Snap Solver for Perpendicularity
void MakePerpendicularSnap(Line2d& first, Line2d& second) {
    double distanceSecond = spn::Distance(second.p1, second.p0);
    double distanceFirst = spn::Distance(first.p1, first.p0);
    spn::Vec2d<double> firstVec = spn::Sub(first.p1, first.p0);
    spn::Vec2d<double> firstVecPerp = spn::Perp(firstVec);
    spn::Vec2d<double> secondVec = spn::Sub(second.p1, second.p0);
    double sign = spn::Dot(firstVecPerp, secondVec) < 0.0 ? -1.0 : 1.0;
    double ndx = firstVecPerp.x / distanceFirst;
    double ndy = firstVecPerp.y / distanceFirst;
    second.p1.x = second.p0.x + sign * ndx * distanceSecond;
    second.p1.y = second.p0.y + sign * ndy * distanceSecond;
    double distanceSecond2 = spn::Distance(second.p1, second.p0);
    double delta = distanceSecond2 - distanceSecond;
    secondVec = spn::Sub(second.p1, second.p0);
    double dot = spn::Dot(firstVec, secondVec);
    //std::cout << " length error" << delta << "dot product error" << 0 - dot << '\n';
}

#endif