#include "Shape.h"

namespace verdant {
bool Sphere::intersect(const Ray &ray, Intersection &isect) const {
  float t0, t1;
  float3 L = center - ray.origin;
  float tca = dot(L, ray.dir);
  float d2 = dot(L, L) - tca * tca;
  float radius2 = radius * radius;
  if (d2 > radius2) {
    return false;
  }
  float thc = sqrt(radius2 - d2);
  t0 = tca - thc;
  t1 = tca + thc;

  if (t0 > t1) {
    float tmp = t0;
    t0 = t1;
    t1 = tmp;
  }
  if (t0 < 0.f) {
    t0 = t1;
    if (t0 < 0.f) {
      return false;
    }
  }
  isect.t = t0;
  float3 position = ray.origin + t0 * ray.dir;
  isect.normal = normalize(position - center);
  return true;
}

BBox3 Sphere::get_bounds() const { return {center - radius, center + radius}; }

Triangle::Triangle(const float3 &p0, const float3 &p1, const float3 &p2) {
  pos[0] = p0;
  pos[1] = p1;
  pos[2] = p2;

  // The position only constructor calculates face normal
  float3 u = p1 - p0;
  float3 v = p2 - p0;
  float3 n = u.cross(v);
  n.normalize();
  normal[0] = normal[1] = normal[2] = n;
}

bool Triangle::intersect(const Ray &ray, Intersection &isect) const {
  float t, u, v;
  bool hit = moller_trumbore(ray, &t, &u, &v);
  if (t < 0) {
    return false;
  }
  isect.t = t;
  isect.normal = u * normal[0] + v * normal[1] + (1 - u - v) * normal[2];
  return hit;
}

BBox3 Triangle::get_bounds() const {
  BBox3 b;
  b.expand(pos[0]).expand(pos[1]).expand(pos[2]);
  return b;
}

bool Triangle::moller_trumbore(const Ray &ray, float *t, float *u,
                               float *v) const {
  float3 v0v1 = pos[1] - pos[0];
  float3 v0v2 = pos[2] - pos[0];
  float3 pvec = cross(ray.dir, v0v2);
  float det = dot(v0v1, pvec);

  // No backface culling
  if (fabs(det) < std::numeric_limits<float>::epsilon())
    return false;

  float inv_det = 1 / det;

  float3 tvec = ray.origin - pos[0];
  *u = dot(tvec, pvec) * inv_det;
  if (*u < 0 || *u > 1)
    return false;

  float3 qvec = cross(tvec, v0v1);
  *v = dot(ray.dir, qvec) * inv_det;
  if (*v < 0 || *u + *v > 1)
    return false;

  *t = dot(v0v2, qvec) * inv_det;

  return true;
}

BBox3 LineSegment::get_bounds() const { throw "unimplemented"; }
} // namespace verdant
