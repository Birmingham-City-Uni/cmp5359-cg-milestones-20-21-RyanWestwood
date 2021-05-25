#pragma once

#include "hittable.h"

class Sphere : public Hittable {
public:
	Sphere() { id = 3; }
	Sphere(Point3f cen, double r, std::shared_ptr<Material> m, int m_idx) : centre(cen), radius(r), mat_ptr(m), mat_index(m_idx){ id = 3; }

	virtual bool Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const override;
	virtual bool Bounding_Box(AABB& output_box) const override;

public:
	Point3f centre;
	double radius;
	std::shared_ptr<Material> mat_ptr;
	int mat_index;
};