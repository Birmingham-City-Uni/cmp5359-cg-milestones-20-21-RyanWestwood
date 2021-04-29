#pragma once
#include "geometry.h"

class Ray {
public:

	Ray() {}
	Ray(const Point3f& origin, const Vec3f& direction) : origin(origin), direction(direction) {}

	Point3f Origin() const { return origin; }
	Vec3f Direction() const { return direction; }
	Point3f At(double t) const { return origin + t * direction; }

private:
	Point3f origin;
	Vec3f direction;
};