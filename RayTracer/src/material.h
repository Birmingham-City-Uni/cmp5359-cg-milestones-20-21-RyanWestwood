#pragma once
#include "common.h"
#include "geometry.h"
#include "hittable.h"

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


Vec3f Reflect(const Vec3f& v, const Vec3f& n) {
	return v - 2 * v.dotProduct(n) * n;
}

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