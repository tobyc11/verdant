#include "BVH.h"
#include "Scene.h"

namespace verdant {
bool BVHNode::intersect(const Ray &ray, Intersection &isect) const {
  if (!bounds.intersect(ray))
    return false;

  bool is_leaf = left == nullptr;
  bool any_hit = false;
  if (is_leaf) {
    for (const auto &item : items) {
      any_hit = item->intersect(ray, isect) || any_hit;
    }
  } else {
    any_hit = left->intersect(ray, isect) || any_hit;
    any_hit = right->intersect(ray, isect) || any_hit;
  }
  return any_hit;
}

bool BVH::intersect(const Ray &ray, Intersection &isect) const {
  return root->intersect(ray, isect);
}

BBox3 BVH::get_bounds() const { return root->get_bounds(); }

BVHNode *BVH::build_from(std::vector<Primitive *> prims, int max_leaf_size) {
  BBox3 centroid_box, bbox;

  for (Primitive *p : prims) {
    BBox3 bb = p->get_bounds();
    bbox.expand(bb);
    float3 c = bb.centroid();
    centroid_box.expand(c);
  }

  if (prims.size() <= max_leaf_size || prims.size() == 1) {
    // This is a leaf node
    BVHNode *node = new BVHNode(bbox, prims);
    return node;
  }

  // TODO: implement surface area huristic
  std::vector<Primitive *> left_prims, right_prims;
  int split_axis = centroid_box.max_extent_axis();
  double split_value = centroid_box.get_min()[split_axis] +
                       centroid_box.get_extent()[split_axis] * 0.5;

  // The two children are allowed to overlap so we don't double-include
  // primitives that intersect both
  for (Primitive *p : prims) {
    BBox3 bb = p->get_bounds();
    float3 c = bb.centroid();
    if (c[split_axis] < split_value)
      left_prims.push_back(p);
    else
      right_prims.push_back(p);
  }

  BVHNode *node = new BVHNode(bbox, build_from(left_prims, max_leaf_size),
                              build_from(right_prims, max_leaf_size));
  return node;
}
} // namespace verdant
