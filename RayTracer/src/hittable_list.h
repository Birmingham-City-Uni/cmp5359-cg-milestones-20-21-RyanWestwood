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

bool Hittable_List::Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const {
	Hit_Record temp_rec;
	bool hit_anything = false;
	auto closest_so_far = t_max;

	for (const auto& object : objects) {
		if (object->Hit(r, t_min, closest_so_far, temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
}

inline bool Hittable_List::Bounding_Box(AABB& output_box) const
{
	if (objects.empty()) return false;

	AABB temp_box;
	bool first_box = true;

	for (const auto& object : objects) {
		if (!object->Bounding_Box(temp_box)) return false;
		output_box = first_box ? temp_box : Surrounding_Box(output_box, temp_box);
		first_box = false;
	}

	return true;
}
