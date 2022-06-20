#include "Scene.h"
#include "Material.h"
#include "MathDefs.h"
#include "Shape.h"
#include <cmath>
#include <cstdio>
#include <limits>
#include <memory>
#include <vector>

namespace minirt {
Primitive::Primitive(std::shared_ptr<Shape> shape,
                     std::shared_ptr<Material> mat)
    : shape(shape), material(mat) {}

bool Primitive::intersect(const Ray &ray, Intersection &isect) const {
  bool hit = shape->intersect(ray, isect);
  if (hit) {
    isect.material = material;
  }
  return hit;
}

Scene::Scene() {
  sky_light = float3::ZERO;

  std::shared_ptr<Material> default_material =
      Material::create_lambert(float3::ONE * 0.9f);
  std::shared_ptr<Material> green_material =
      Material::create_lambert(float3(0.2f, 1.0f, 0.2f));

  primitives.emplace_back(std::make_shared<Sphere>(1.0f), default_material);
  primitives.emplace_back(
      std::make_shared<Sphere>(1.0f, float3(2.0f, 0.0f, 0.0f)),
      default_material);
  float xl = 4.f;
  float yl = 1.f;
  float zl = 4.f;
  primitives.emplace_back(std::make_shared<Triangle>(float3(-xl, -yl, zl),
                                                     float3(xl, -yl, -zl),
                                                     float3(-xl, -yl, -zl)),
                          green_material);
  primitives.emplace_back(std::make_shared<Triangle>(float3(-xl, -yl, zl),
                                                     float3(xl, -yl, zl),
                                                     float3(xl, -yl, -zl)),
                          green_material);
  primitives.emplace_back(std::make_shared<Triangle>(float3(-xl, -yl, -zl),
                                                     float3(xl, yl, -zl),
                                                     float3(-xl, yl, -zl)),
                          default_material);
  primitives.emplace_back(std::make_shared<Triangle>(float3(-xl, -yl, -zl),
                                                     float3(xl, -yl, -zl),
                                                     float3(xl, yl, -zl)),
                          default_material);
}

bool Scene::intersect(const Ray &ray, Intersection &isect) const {
  // printf("(%.3f, %.3f, %.3f) (%.3f, %.3f, %.3f)\n", world_ray.origin.x(),
  //        world_ray.origin.y(), world_ray.origin.z(), world_ray.dir.x(),
  //        world_ray.dir.y(), world_ray.dir.z());

  bool any_hit = false;
  isect.t = std::numeric_limits<float>::infinity();
  for (auto prim : primitives) {
    bool hit;
    Intersection prim_isect;
    hit = prim.intersect(ray, prim_isect);
    if (hit && prim_isect.t < isect.t) {
      isect = prim_isect;
      any_hit = true;
    }
  }
  return any_hit;
}
} // namespace minirt
