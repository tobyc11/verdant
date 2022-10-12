#pragma once
#include "BBox3.h"
#include "MathDefs.h"

namespace verdant {
class Shape {
public:
  // Intersection test between a ray and arbitrary shape
  // Will only return true and update isect if this object is closer than
  // isect.t
  virtual bool intersect(const Ray &ray, Intersection &isect) const = 0;
  virtual BBox3 get_bounds() const = 0;
};

class Sphere : public Shape {
public:
  Sphere(float radius, float3 center = float3::ZERO)
      : radius(radius), center(center) {}

  bool intersect(const Ray &ray, Intersection &isect) const override;
  BBox3 get_bounds() const override;

private:
  float radius;
  float3 center;
};

class Triangle : public Shape {
public:
  Triangle(const float3 &p0, const float3 &p1, const float3 &p2);

  bool intersect(const Ray &ray, Intersection &isect) const override;
  BBox3 get_bounds() const override;

  bool moller_trumbore(const Ray &ray, float *t, float *u, float *v) const;

private:
  float3 pos[3];
  float3 normal[3];
};

class LineSegment : public Shape {
public:
  LineSegment(float3 origin, float3 dir, float radius)
      : origin(origin), dir(dir), radius(radius) {}

  bool intersect(const Ray &ray, Intersection &isect) const override {
    float t, u;
    if (line_line_closest_point(ray.origin, ray.dir, origin, dir, t, u)) {
      // Need to know how far
      float3 P = ray.origin + t * ray.dir;
      float3 Q = origin + u * dir;
      float d = P.distance(Q);
      if (d < radius) {
        isect.t = t;
        return true;
      }
    }
    return false;
  }

  BBox3 get_bounds() const override;

  static bool line_line_closest_point(const float3 &O0, const float3 &D0,
                                      const float3 &O1, const float3 &D1,
                                      float &t, float &u) {
    float d00 = dot(D0, D0);
    float d11 = dot(D1, D1);
    float d01 = dot(D0, D1);
    float det = d00 * d11 - d01 * d01;

    // D0 and D1 are parallel, no least norm solution
    if (fabs(det) < std::numeric_limits<float>::epsilon())
      return false;

    float ATb_0 = dot(D0, O1 - O0);
    float ATb_1 = dot(-D1, O1 - O0);
    t = (1 / det) * (d11 * ATb_0 + d01 * ATb_1);
    u = (1 / det) * (d01 * ATb_0 + d00 * ATb_1);
    return true;
  }

private:
  float3 origin;
  float3 dir;
  float radius;
};
} // namespace verdant
