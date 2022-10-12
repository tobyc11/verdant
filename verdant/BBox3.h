#pragma once
#include "MathDefs.h"
#include <cmath>

namespace verdant {
class BBox3 {
public:
  BBox3()
      : min_(INFINITY, INFINITY, INFINITY),
        max_(-INFINITY, -INFINITY, -INFINITY) {}

  BBox3(float3 min_, float3 max_) : min_(min_), max_(max_) {}

  BBox3 &expand(const float3 &rhs) {
    min_ = min(min_, rhs);
    max_ = max(max_, rhs);
    return *this;
  }

  BBox3 &expand(const BBox3 &rhs) {
    min_ = min(min_, rhs.min_);
    min_ = min(min_, rhs.max_);
    max_ = max(max_, rhs.min_);
    max_ = max(max_, rhs.max_);
    return *this;
  }

  float3 centroid() const { return max_ / 2 + min_ / 2; }

  int max_extent_axis() const {
    int m = 0;
    float v = max_[0] - min_[0];
    for (int i = 1; i < 3; i++) {
      float vv = max_[i] - min_[i];
      if (vv > v) {
        v = vv;
        m = i;
      }
    }
    return m;
  }

  float3 get_min() const { return min_; }
  float3 get_max() const { return max_; }
  float3 get_extent() const { return max_ - min_; }

  bool contains(const float3 &p) const {
    for (int i = 0; i < 3; i++) {
      if (!(min_[i] <= p[i] && p[i] <= max_[i])) {
        return false;
      }
    }
    return true;
  }

  bool contains(const float3 &p, int skip_axis) const {
    for (int i = 0; i < 3; i++) {
      if (i == skip_axis) {
        continue;
      }
      if (!(min_[i] <= p[i] && p[i] <= max_[i])) {
        return false;
      }
    }
    return true;
  }

  bool intersect(const Ray &ray) const {
    // Simple but probably not the most efficient algorithm:
    // one of the 6 faces must be hit
    auto tm = (min_ - ray.origin) / ray.dir;
    auto tn = (max_ - ray.origin) / ray.dir;
    for (int a = 0; a < 3; a++) {
      auto point = ray.origin + tm[a] * ray.dir;
      if (contains(point, a)) {
        return true;
      }
    }
    for (int a = 0; a < 3; a++) {
      auto point = ray.origin + tn[a] * ray.dir;
      if (contains(point), a) {
        return true;
      }
    }
    return false;
  }

private:
  float3 min_, max_;
};
} // namespace verdant
