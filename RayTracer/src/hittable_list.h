#pragma once
#include "hittable.h"
#include<memory>
#include<vector>

class Hittable_List : public Hittable {
public:
	Hittable_List() {}
	Hittable_List(std::shared_ptr<Hittable> object) { Add(object); }

	void Clear() { objects.clear(); }
	void Add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

	virtual bool Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const override;
	virtual bool Bounding_Box(AABB& output_box) const override;

public:
	std::vector<std::shared_ptr<Hittable>> objects;
};