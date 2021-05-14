#pragma once
#include "common.h"

class Camera {
public:

	Camera(Point3f lookfrom, Point3f lookat, Vec3f vup,double vfov, double aspect_ratio, double aperture, double focus_dist) {
		theta = Degrees_To_Radians(vfov);
		h = tan(theta / 2);
		viewport_height = 2.0 * h;
		viewport_width = aspect_ratio * viewport_height;
		up = vup;
		focus_distance = focus_dist;
		look_at = lookat;

		w = (lookfrom - lookat).normalize();
		u = (vup.crossProduct(w)).normalize();
		v = w.crossProduct(u);
		origin = lookfrom;
		horizontal = focus_dist * viewport_width * u;
		vertical = focus_dist * viewport_height * v;
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;
		lens_radius = aperture / 2;
	}

	Ray Get_Ray(double s, double t) const {
		Vec3f rd = lens_radius * Vec3f().Random_In_Unit_Disk();
		Vec3f offset = u * rd.x + v * rd.y;
		return Ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
	}

	void LookFrom(Point3f lookfrom) {
		w = (lookfrom - look_at).normalize();
		u = (up.crossProduct(w)).normalize();
		v = w.crossProduct(u);
		origin = lookfrom;
		horizontal = focus_distance * viewport_width * u;
		vertical = focus_distance * viewport_height * v;
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_distance * w;
	}

private:

	//TESTING

	double theta;
	double h;
	double viewport_height;
	double viewport_width;
	double focus_distance;
	Vec3f up;
	Vec3f look_at;

	//	

	Vec3f u, v, w;
	double lens_radius;

	Point3f origin;
	Vec3f horizontal;
	Vec3f vertical;
	Vec3f lower_left_corner;
};