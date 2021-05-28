#include "aabb.h"

constexpr double offset = 1e-3;

AABB Surrounding_Box(AABB box0, AABB box1) {
    Point3f small(
        fmin(box0.Min().x - offset, box1.Min().x - offset),
        fmin(box0.Min().y - offset, box1.Min().y - offset),
        fmin(box0.Min().z - offset, box1.Min().z - offset));

    Point3f big(
        fmax(box0.Max().x + offset, box1.Max().x + offset),
        fmax(box0.Max().y + offset, box1.Max().y + offset),
        fmax(box0.Max().z + offset, box1.Max().z + offset));

    return {small, big};
}

bool AABB::Hit(const Ray& r, double t_min, double t_max) const {
    for (int a = 0; a < 3; a++) {
        auto t0 = std::fmin((minimum[a] - r.Origin()[a]) / r.Direction()[a],
            (maximum[a] - r.Origin()[a]) / r.Direction()[a]);
        auto t1 = std::fmax((minimum[a] - r.Origin()[a]) / r.Direction()[a],
            (maximum[a] - r.Origin()[a]) / r.Direction()[a]);
        t_min = std::fmax(t0, t_min);
        t_max = std::fmin(t1, t_max);
        if (t_max <= t_min) {
            return false;
        }
    }
    return true;
}

bool AABB::operator==(const AABB& rhs) const {
    return (minimum == rhs.minimum) && (maximum == rhs.maximum);
}

bool AABB::operator!=(const AABB& rhs) const {
    return (minimum != rhs.minimum) || (maximum != rhs.maximum);
}

std::ostream& operator << (std::ostream& s, AABB& v)
{
    return s << v.minimum << "\t" << v.maximum;;
}