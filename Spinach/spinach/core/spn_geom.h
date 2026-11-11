#ifndef SPN_GEOM_H
#define SPN_GEOM_H

#include <cmath>

namespace spn
{
    template<typename T = double>
    struct Vec2d {
        T x;
        T y;
    };

    using Vec2f = Vec2d<float>;
    using Vec2i = Vec2d<int>;

    template<typename T>
    inline static T Lerp(T a, T b, T t) {
        return a + (b - a) * t;
    }

    template<typename T>
    inline static Vec2d<T> Lerp(const Vec2d<T>& a, const Vec2d<T>& b, T t)
    {
        return {
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t
        };
    }

    template<typename T>
    inline static T DistanceSquared(const Vec2d<T>& a, const Vec2d<T>& b)
    {
        T dx = a.x - b.x;
        T dy = a.y - b.y;
        return dx * dx + dy * dy;
    }

    template<typename T>
    inline static double Distance(const Vec2d<T>& a,const Vec2d<T>& b)
    {
        return std::sqrt(
            DistanceSquared(a, b));
    }

    template<typename T>
    inline static double Magnitude(const Vec2d<T>& a)
    {
        return std::sqrt(a.x*a.x+a.y*a.y);
    }

    template<typename T>
    inline static double MagnitudeSquared(const Vec2d<T>& a)
    {
        return a.x * a.x + a.y * a.y;
    }

    template<typename T>
    inline static Vec2d<T> Add(const Vec2d<T>& a, const Vec2d<T>& b)
    {
        return {
            a.x + b.x,
            a.y + b.y
        };
    }

    template<typename T>
    inline static Vec2d<T> Sub(const Vec2d<T>& a, const Vec2d<T>& b)
    {
        return {
            a.x - b.x,
            a.y - b.y
        };
    }

    template<typename T>
    inline static Vec2d<T> Perp(const Vec2d<T>& a)
    {
        return {
            -a.y,
            a.x
        };
    }

    template<typename T>
    inline static Vec2d<T> Scale(const Vec2d<T>& a, T sf)
    {
        return {
            a.x * sf,
            a.y * sf
        };
    }

    template<typename T>
    inline static T Dot(const Vec2d<T>& a, const Vec2d<T>& b)
    {
        return a.x * b.x + a.y * b.y;
    }
}

#endif