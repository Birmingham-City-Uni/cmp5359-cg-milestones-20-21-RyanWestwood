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