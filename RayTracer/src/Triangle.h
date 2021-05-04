#pragma once

#include "hittable.h"
#include "geometry.h"

class Triangle : public Hittable {
public:
	Triangle() {}
	Triangle(Point3f vert0, Point3f vert1, Point3f vert2, std::shared_ptr<Material> mat)
		: v0(vert0), v1(vert1), v2(vert2), mat_ptr(mat)
	{
		normal = (v1 - v0).crossProduct(v2 - v0);
	};

	Triangle(Point3f vert0, Point3f vert1, Point3f vert2, Vec3f vn, std::shared_ptr<Material> mat)
		: v0(vert0), v1(vert1), v2(vert2), normal(vn), mat_ptr(mat) {};

	virtual bool Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const override;


public:
	Point3f v0, v1, v2;
	Vec3f normal;
	std::shared_ptr<Material> mat_ptr;

};