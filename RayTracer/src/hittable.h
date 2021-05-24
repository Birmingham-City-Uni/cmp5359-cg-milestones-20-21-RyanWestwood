#pragma once
#include "ray.h"
#include "aabb.h"

class Material;

struct Hit_Record {
	Point3f p;
	Vec3f normal;
	double t;
	bool front_face;

	inline void Set_Face_Normal(const Ray& r, const Vec3f& outward_normal) {
		front_face = (r.Direction().dotProduct(outward_normal)) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}

	std::shared_ptr<Material> mat_ptr;
};

class Hittable {
public:
	virtual bool Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const = 0;
	virtual bool Bounding_Box(AABB& output_box) const = 0;

	virtual std::shared_ptr<Hittable> Left() const { return nullptr; };
	virtual std::shared_ptr<Hittable> Right() const { return nullptr; };
	virtual AABB Box() const { return AABB(); };

	virtual void Left(std::shared_ptr<Hittable> l) {};
	virtual void Right(std::shared_ptr<Hittable> r) {};
	virtual void Box(AABB b) {};

public:
	int id = 0;
};