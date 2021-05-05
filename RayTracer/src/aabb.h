////#pragma once
////#include "common.h"
////
////class AABB {
////public: 
////	AABB() {}
////	AABB(const Point3f& min, const Point3f& max) : minimum(min), maximum(max) {};
////
////	Point3f Min() { return minimum; };
////	Point3f Max() { return maximum; };
////
////	bool Hit(const Ray& r, double t_min, double t_max) const {
////		for (int a = 0; a < 3; a++)
////		{
////			auto t0 = fmin((minimum[a] - r.Origin()[a]) / r.Direction()[a],
////				(maximum[a] - r.Origin()[a]) / r.Direction()[a]);
////
////			auto t1 = fmax((minimum[a] - r.Origin()[a]) / r.Direction()[a],
////				(maximum[a] - r.Origin()[a]) / r.Direction()[a]);
////
////			t_min = fmax(t0, t_min);
////			t_max = fmin(t1, t_max);
////			if (t_max <= t_min) return false;
////		}
////		return true;
////	};
////
////	AABB Surrounding_Box(AABB box0, AABB box1) {
////		Point3f small(fmin(box0.Min().x, box1.Min().x),
////			fmin(box0.Min().y, box1.Min().y),
////			fmin(box0.Min().z, box1.Min().z));
////
////		Point3f big(fmax(box0.Max().x, box1.Max().x),
////			fmax(box0.Max().y, box1.Max().y),
////			fmax(box0.Max().z, box1.Max().z));
////
////		return AABB{ small, big };
////	};
////
////public:
////	Point3f minimum, maximum;
////};
//
//#pragma once
//#include "common.h"
//
//class AABB {
//public:
//	AABB() {}
//	AABB(const Point3f& min, const Point3f& max) : minimum(min), maximum(max) {}
//
//	Point3f Min() const { return minimum; }
//	Point3f Max() const { return maximum; }
//
//	bool Hit(const Ray& r, double t_min, double t_max) const {
//		for (int a = 0; a < 3; a++) {
//			auto t0 = fmin((minimum[a] - r.Origin()[a]) / r.Direction()[a], (maximum[a] - r.Origin()[a]) / r.Direction()[a]);
//			auto t1 = fmax((minimum[a] - r.Origin()[a]) / r.Direction()[a], (maximum[a] - r.Origin()[a]) / r.Direction()[a]);
//			t_min = fmax(t0, t_min);
//			t_max = fmin(t1, t_max);
//			if (t_max <= t_min)
//				return false;
//		}
//		return true;
//	}
//
//private:
//	Point3f minimum, maximum;
//};
//
//AABB Surrounding_Box(AABB box0, AABB box1);

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

    Point3f minimum;
    Point3f maximum;
};

AABB Surrounding_Box(AABB box0, AABB box1);