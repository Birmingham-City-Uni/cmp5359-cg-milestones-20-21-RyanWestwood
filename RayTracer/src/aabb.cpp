#include "aabb.h"

AABB Surrounding_Box(AABB box0, AABB box1) {
    Point3f small(
        fmin(box0.Min().x - 1e-3, box1.Min().x - 1e3),
        fmin(box0.Min().y - 1e-3, box1.Min().y - 1e3),
        fmin(box0.Min().z - 1e-3, box1.Min().z - 1e3));

    Point3f big(
        fmax(box0.Max().x + 1e3, box1.Max().x + 1e3),
        fmax(box0.Max().y + 1e3, box1.Max().y + 1e3),
        fmax(box0.Max().z + 1e3, box1.Max().z + 1e3));

    return AABB(small, big);
}

bool AABB::Hit(const Ray& r, double t_min, double t_max) const {
    for (int a = 0; a < 3; a++) {
        auto t0 = fmin((minimum[a] - r.Origin()[a]) / r.Direction()[a],
            (maximum[a] - r.Origin()[a]) / r.Direction()[a]);
        auto t1 = fmax((minimum[a] - r.Origin()[a]) / r.Direction()[a],
            (maximum[a] - r.Origin()[a]) / r.Direction()[a]);
        t_min = fmax(t0, t_min);
        t_max = fmin(t1, t_max);
        if (t_max <= t_min)
            return false;
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