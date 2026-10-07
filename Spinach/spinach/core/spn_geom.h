#ifndef SPN_GEOM_H
#define SPN_GEOM_H

namespace spn
{
	struct Vec2d {
		float x;
		float y;
	};
	
	inline static float Lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}
	
	inline static Vec2d Lerp(const Vec2d& a, const Vec2d& b, float t) {
		return Vec2d{ 
			a.x + (b.x - a.x) * t, 
			a.y + (b.y - a.y) * t
		};
	}
	
	inline static float DistanceSquared(const Vec2d& a, const Vec2d& b) {
		return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
	}
	
	inline static Vec2d Add(const Vec2d& a, const Vec2d& b) {
		return Vec2d{ 
			a.x + b.x, 
			a.y + b.y
		};
	}
	
	inline static Vec2d Sub(const Vec2d& a, const Vec2d& b) {
		return Vec2d{ 
			a.x - b.x, 
			a.y - b.y
		};
	}
	
	inline static Vec2d Scale(const Vec2d& a, float sf) {
		return Vec2d{ 
			a.x * sf, 
			a.y * sf
		};
	}
	
	inline static float Dot(const Vec2d& a, const Vec2d& b) {
		return a.x * b.x + a.y * b.y;
	}
	
}

#endif
