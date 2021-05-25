#pragma once

#include "hittable.h"
#include "geometry.h"

class Triangle : public Hittable {
public:
	Triangle() { id = 2; }
	Triangle(Point3f vert0, Point3f vert1, Point3f vert2, Point3f vert0n, Point3f vert1n, Point3f vert2n, std::shared_ptr<Material> mat, int m_idx)
		: v0(vert0), v1(vert1), v2(vert2), v0n(vert0n), v1n(vert1n), v2n(vert2n), mat_ptr(mat), mat_index(m_idx)
	{
		id = 2;
		//	Add average normal calc here??
	};

	Triangle(Point3f vert0, Point3f vert1, Point3f vert2, Vec3f vn, std::shared_ptr<Material> mat)
		: v0(vert0), v1(vert1), v2(vert2), /*normal(vn),*/ mat_ptr(mat) { id = 2; };

	virtual bool Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const override;
	virtual bool Bounding_Box(AABB& output_box) const override;

public:
	Point3f v0, v1, v2;
	Point3f v0n, v1n, v2n;
	std::shared_ptr<Material> mat_ptr;
	int mat_index;
};