#pragma once
#include "common.h"

class AABB {
public:
    AABB() {}
    AABB(const Point3f& mini, const Point3f& maxi) { minimum = mini; maximum = maxi; }

    Point3f Min() const { return minimum; }
    Point3f Max() const { return maximum; }

    bool Hit(const Ray& r, double t_min, double t_max) const {
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

    bool operator==(const AABB& rhs) const {
        return (minimum == rhs.minimum) && (maximum == rhs.maximum);
    }

    bool operator!=(const AABB& rhs) const {
        return (minimum != rhs.minimum) || (maximum != rhs.maximum);
    }

    friend std::ostream& operator << (std::ostream& s, AABB& v)
    {
        return s << v.minimum << "\t" << v.maximum;
    }

    Point3f minimum;
    Point3f maximum;
};

AABB Surrounding_Box(AABB box0, AABB box1);