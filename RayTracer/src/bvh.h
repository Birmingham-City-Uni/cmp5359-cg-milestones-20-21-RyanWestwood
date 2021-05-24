#pragma once
#include "common.h"
#include "hittable.h"
#include "hittable_list.h"
#include <algorithm>
#include <memory>

class BVH_Node : public Hittable {
public:
    BVH_Node() { id = 1; }
    BVH_Node(AABB box) : box(box) { id = 1; }
    BVH_Node(const Hittable_List& list) : BVH_Node(list.objects, 0, list.objects.size()) { id = 1; }
    BVH_Node(const std::vector<std::shared_ptr<Hittable>>& src_objects, size_t start, size_t end);

    virtual bool Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const override;
    virtual bool Bounding_Box(AABB& output_box) const override;

    virtual std::shared_ptr<Hittable> Left() const override { return left; }
    virtual std::shared_ptr<Hittable> Right() const override { return right; }
    virtual AABB Box() const override{ return box; }

    virtual void Left(std::shared_ptr<Hittable> l) { left = l; };
    virtual void Right(std::shared_ptr<Hittable> r) { right = r; };
    virtual void Box(AABB b) { box = b; };

public: 
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB box;
};