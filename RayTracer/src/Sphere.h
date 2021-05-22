#pragma once

#include "hittable.h"

class Sphere : public Hittable {
public:
	Sphere() {}
	Sphere(Point3f cen, double r, std::shared_ptr<Material> m) : centre(cen), radius(r), mat_ptr(m) {}

	virtual bool Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const override;
	virtual bool Bounding_Box(AABB& output_box) const override;

private:
	Point3f centre;
	double radius;
	std::shared_ptr<Material> mat_ptr;
};