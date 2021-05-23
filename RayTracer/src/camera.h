#pragma once
#include "common.h"

class Camera {
public:

	Camera(Point3f lookfrom, Point3f lookat, Vec3f vup, double vfov, double aspect_ratio, double aperture, double focus_dist);

	Ray Get_Ray(double s, double t) const;
	void LookFrom(Point3f lookfrom);

private:

	double theta;
	double h;
	double viewport_height;
	double viewport_width;
	double focus_distance;
	Vec3f up;
	Vec3f look_at;

	Vec3f u, v, w;
	double lens_radius;

	Point3f origin;
	Vec3f horizontal;
	Vec3f vertical;
	Vec3f lower_left_corner;
};