#pragma once
#include "common.h"
#include "geometry.h"
#include "hittable.h"

Vec3f Reflect(const Vec3f& v, const Vec3f& n) {
	return v - 2 * v.dotProduct(n) * n;
}

Vec3f Refract(const Vec3f& uv, const Vec3f& n, double etai_over_etat) {
	auto cos_theta = fmin(-uv.dotProduct(n), 1.0);
	Vec3f r_out_perp = etai_over_etat * (uv + cos_theta * n);
	Vec3f r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.norm())) * n;
	return r_out_perp + r_out_parallel;
}

struct Hit_Record;
class Material {
public:
	virtual bool Scatter(const Ray& r_in, const Hit_Record& rec, Colour& attenuation, Ray& scattered) const = 0;
};

class Lambertian : public Material {
public:
	Lambertian(const Colour& a) : albedo(a) {}

	virtual bool Scatter(const Ray& r_in, const Hit_Record& rec, Colour& attenuation, Ray& scattered) const override {
		auto scatter_direction = rec.normal + Vec3f().Random_In_Unit_Sphere();
		if (scatter_direction.Near_Zero()) {
			scatter_direction = rec.normal;
		}
		scattered = Ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	}

private:
	Colour albedo;
};

class Metal : public Material {
public:
	Metal(const Colour& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

	virtual bool Scatter(const Ray& r_in, const Hit_Record& rec, Colour& attenuation, Ray& scattered) const override {
		Vec3f reflected = Reflect(r_in.Direction().normalize(), rec.normal);
		scattered = Ray(rec.p, reflected + fuzz * Vec3f().Random_In_Unit_Sphere());
		attenuation = albedo;
		return (scattered.Direction().dotProduct(rec.normal) > 0);
	}
private:
	Colour albedo;
	double fuzz;
};

class Dielectric : public Material {
public:
	Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

	virtual bool Scatter(const Ray& r_in, const Hit_Record& rec, Colour& attenuation, Ray& scattered) const override {
		attenuation = Colour(1.0, 1.0, 1.0);
		double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

		Vec3f unit_direction = r_in.Direction().normalize();
		double cos_theta = fmin(-unit_direction.dotProduct(rec.normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		Vec3f direction;
		if (cannot_refract || Reflectance(cos_theta, refraction_ratio) > Random_Double())
			direction = Reflect(unit_direction, rec.normal);
		else
			direction = Refract(unit_direction, rec.normal, refraction_ratio);

		scattered = Ray(rec.p, direction);
		return true;
	}

public:
	double ir;

private:
	static double Reflectance(double cosine, double ref_idx) {
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};