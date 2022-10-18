#pragma once
#include "BBox3.h"
#include "MathDefs.h"
#include <vector>

namespace verdant {
class Primitive;

// This could be a template not dependent on Primitive, but there is no use for
// that yet
class BVHNode {
public:
  BVHNode(BBox3 bounds, std::vector<Primitive *> items)
      : bounds(bounds), left(nullptr), right(nullptr), items(std::move(items)) {
  }

  BVHNode(BBox3 bounds, BVHNode *left, BVHNode *right)
      : bounds(bounds), left(left), right(right) {}

  const BBox3 &get_bounds() const { return bounds; }
  bool intersect(const Ray &ray, Intersection &isect) const;

private:
  BBox3 bounds;
  BVHNode *left;
  BVHNode *right;
  std::vector<Primitive *> items;
};

class BVH {
public:
  BVH() = default;
  explicit BVH(std::vector<Primitive *> items) { root = build_from(items, 4); }

  bool intersect(const Ray &ray, Intersection &isect) const;
  BBox3 get_bounds() const;

private:
  static BVHNode *build_from(std::vector<Primitive *> items, int max_leaf_size);

  BVHNode *root{};
};
} // namespace verdant
