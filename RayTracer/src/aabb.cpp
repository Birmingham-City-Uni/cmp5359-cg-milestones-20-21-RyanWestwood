#include "aabb.h"

AABB Surrounding_Box(AABB box0, AABB box1) {
    Point3f small(fmin(box0.Min().x, box1.Min().x),
        fmin(box0.Min().y, box1.Min().y),
        fmin(box0.Min().z, box1.Min().z));

    Point3f big(fmax(box0.Max().x, box1.Max().x),
        fmax(box0.Max().y, box1.Max().y),
        fmax(box0.Max().z, box1.Max().z));

    return AABB(small, big);
}