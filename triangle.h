#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "hittable.h"

class triangle : public hittable {
  public:
    triangle(const point3& a, const point3& b, const point3& c, shared_ptr<material> m)
      : v0(a), v1(b), v2(c), mat(m) {
        n = unit_vector(cross(v1 - v0, v2 - v0)); // flat normal
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        const vec3 e1 = v1 - v0;
        const vec3 e2 = v2 - v0;
        const vec3 p  = cross(r.direction(), e2);
        const double det = dot(e1, p);
        if (std::fabs(det) < 1e-8) return false;

        const double invDet = 1.0 / det;
        const vec3 tvec = r.origin() - v0;
        const double u = dot(tvec, p) * invDet;
        if (u < 0.0 || u > 1.0) return false;

        const vec3 q = cross(tvec, e1);
        const double v = dot(r.direction(), q) * invDet;
        if (v < 0.0 || u + v > 1.0) return false;

        const double t = dot(e2, q) * invDet;
        if (!ray_t.surrounds(t)) return false;

        rec.t = t;
        rec.p = r.at(t);
        rec.mat = mat;
        rec.set_face_normal(r, n);
        return true;
    }

  private:
    point3 v0, v1, v2;
    vec3 n;
    shared_ptr<material> mat;
};

#endif