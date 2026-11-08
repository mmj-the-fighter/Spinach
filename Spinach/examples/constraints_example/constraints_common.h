#ifndef COMMON_H
#define COMMON_H
#include <spn_geom.h>

struct Line2d {
    spn::Vec2d<double> p0;
    spn::Vec2d<double> p1;
};

enum Constraint {
    NIL,
    PARALLEL,
    PERPENDICULAR
};

enum Algorithm {
    SNAP,
    ALGEBRAIC,
    NUMERICAL
};

//AI generated function
template<typename T>
inline static bool IsCloseToZero(T value, T epsilon = T(1e-6))
{
    return std::abs(value) <= epsilon;
}

//AI generated function
template<typename T>
inline static bool IsClose(T a, T b, T epsilon = T(1e-6))
{
    return std::abs(a - b) <= epsilon;
}

//AI generated function
template<typename T>
inline static bool IsNegative(T value, T epsilon = T(1e-6))
{
    return value < -epsilon;
}
#endif
