#pragma once
#include "ray.h"

class Material;

struct Hit_Record {
	Point3f p;
	Vec3f normal;
	double t;
	bool front_face;

	inline void set_face_normal(const Ray& r, const Vec3f& outward_normal) {
		front_face = (r.Direction().dotProduct(outward_normal)) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}

	std::shared_ptr<Material> mat_ptr;
};

class Hittable {
public:
	virtual bool Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const = 0;
};