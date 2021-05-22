#include "Sphere.h"

bool Sphere::Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const {
	Vec3f oc = r.Origin() - centre;
	auto a = r.Direction().norm();

	auto half_b = oc.dotProduct(r.Direction());
	auto c = oc.norm() - radius * radius;
	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	auto sqrtd = sqrt(discriminant);

	auto root = (-half_b - sqrtd) / a;
	if (root < t_min || t_max < root) {
		root = (-half_b + sqrtd) / a;
		if (root < t_min || t_max < root) {
			return false;
		}
	}

	rec.t = root;
	rec.p = r.At(rec.t);
	Vec3f outward_normal = (rec.p - centre) / radius;
	rec.Set_Face_Normal(r, outward_normal);
	rec.mat_ptr = mat_ptr;

	return true;
}

bool Sphere::Bounding_Box(AABB& output_box) const
{
	output_box = AABB(centre - Vec3f(radius, radius, radius), centre + Vec3f(radius, radius, radius));
	return true;
}