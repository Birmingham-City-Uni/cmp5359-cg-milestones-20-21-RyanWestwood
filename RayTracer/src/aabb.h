#pragma once
#include "common.h"

class AABB {
public:
    AABB() : minimum({ 0.F,0.F,0.F }), maximum({ 0.F, 0.F, 0.F }) {}
    AABB(const Point3f& mini, const Point3f& maxi) : minimum(mini), maximum(maxi) {}

    Point3f Min() const { return minimum; }
    Point3f Max() const { return maximum; }

    bool Hit(const Ray& r, double t_min, double t_max) const;

    bool operator==(const AABB& rhs) const;

    bool operator!=(const AABB& rhs) const;

    friend std::ostream& operator << (std::ostream& s, AABB& v);

    Point3f minimum;
    Point3f maximum;
};

AABB Surrounding_Box(AABB box0, AABB box1);