#include "Triangle.h"

bool Triangle::Hit(const Ray& r, double t_min, double t_max, Hit_Record& rec) const
{
    float thit, t, u, v;

    Vec3f v0v1 = v1 - v0;
    Vec3f v0v2 = v2 - v0;
    Vec3f pvec = r.Direction().crossProduct(v0v2);

    float det = pvec.dotProduct(v0v1);
    float kEpsilon = 0.00001;

    if (det < kEpsilon) return false;

    float invDet = 1 / det;
    
    Vec3f tvec = r.Origin() - v0;
    u = tvec.dotProduct(pvec) * invDet;
    if (u < 0 || u > 1) return false;


    Vec3f qvec = tvec.crossProduct(v0v1);
    v = r.Direction().dotProduct(qvec) * invDet;
    if (v < 0 || u + v > 1) return false;

    t = v0v2.dotProduct(qvec) * invDet;
    if (t < 0) return false;

    rec.p = r.At(t);
    rec.t = t;

    //TODO FIX tri norms
    rec.normal = normal;
    rec.mat_ptr = mat_ptr;

    return true;
}
