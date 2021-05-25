#pragma once
#include "common.h"
#include "geometry.h"
#include "hittable.h"

inline Vec3f Reflect(const Vec3f& v, const Vec3f& n) {
	return v - 2 * v.dotProduct(n) * n;
}

inline Vec3f Refract(const Vec3f& uv, const Vec3f& n, double etai_over_etat) {
	auto cos_theta = fmin(-uv.dotProduct(n), 1.0);
	Vec3f r_out_perp = etai_over_etat * (uv + cos_theta * n);
	Vec3f r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.norm())) * n;
	return r_out_perp + r_out_parallel;
}

struct Hit_Record;

class Material {
public:
	virtual bool Scatter(const Ray& r_in, const Hit_Record& rec, Colour& attenuation, Ray& scattered) const = 0;
	virtual Colour Emitted() const { return Colour(0, 0, 0); }

public:
	int id = 0;
	int index = 0;
};

class Lambertian : public Material {
public:
	Lambertian(const Colour& a, int n) : albedo(a) { id = 1; index = n; }

	virtual bool Scatter(const Ray& r_in, const Hit_Record& rec, Colour& attenuation, Ray& scattered) const override {
		auto scatter_direction = rec.normal + Vec3f().Random_In_Unit_Sphere();
		if (scatter_direction.Near_Zero()) {
			scatter_direction = rec.normal;
		}
		scattered = Ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	}

public:
	Colour albedo;
};

class Metal : public Material {
public:
	Metal(const Colour& a, double f, int n) : albedo(a), fuzz(f < 1 ? f : 1) { id = 2; index = n;}

	virtual bool Scatter(const Ray& r_in, const Hit_Record& rec, Colour& attenuation, Ray& scattered) const override {
		Vec3f reflected = Reflect(r_in.Direction().normalize(), rec.normal);
		scattered = Ray(rec.p, reflected + fuzz * Vec3f().Random_In_Unit_Sphere());
		attenuation = albedo;
		return (scattered.Direction().dotProduct(rec.normal) > 0);
	}
public:
	Colour albedo;
	double fuzz;
};

class Dielectric : public Material {
public:
	Dielectric(double index_of_refraction, int n) : ir(index_of_refraction) { id = 3; index = n; }

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

class Diffuse_Light : public Material {
public:
	Diffuse_Light() { id = 4; }
	Diffuse_Light(Colour c, int n) : emit(std::make_shared<Colour>(c)), colour(c) { id = 4; index = n; }

	virtual bool Scatter(const Ray& r_in, const Hit_Record& rec, Colour& attenuation, Ray& scattered) const override { return false; }
	virtual Colour Emitted() const override { return *emit; }

public:
	std::shared_ptr<Colour> emit;
	Colour colour;
};