#include "Scene.h"
#include "MathDefs.h"
#include "Shape.h"
#include "Surface.h"
#include <cmath>
#include <cstdio>
#include <limits>
#include <memory>
#include <vector>

namespace verdant {
Primitive::Primitive(std::shared_ptr<Shape> shape, std::shared_ptr<Surface> mat)
    : shape(shape), material(mat) {}

bool Primitive::intersect(const Ray &ray, Intersection &isect) const {
  bool hit = shape->intersect(ray, isect);
  if (hit) {
    isect.material = material;
  }
  return hit;
}

Scene::Scene() {
  set_sky_light(true, float3::ONE);
  std::shared_ptr<Surface> furnace_material =
      Surface::create_lambert(float3::ONE * 0.18f);
  std::shared_ptr<Surface> default_material =
      Surface::create_lambert(float3::ONE * 0.9f);
  std::shared_ptr<Surface> green_material =
      Surface::create_lambert(float3(0.2f, 1.0f, 0.2f));

  std::shared_ptr<Surface> glass_material =
      Surface::create_glass(float3(1.0f, 1.0f, 1.0f), 1.5f);
  std::shared_ptr<Surface> transparent_material =
      Surface::create_refract(float3(1.0f, 1.0f, 1.0f), 1.5f);

  if (false) {
    // std::shared_ptr<Surface> default_material =
    //     Surface::create_lambert(float3::ONE * 0.9f);
    primitives.emplace_back(std::make_shared<Sphere>(3.0f), default_material);
  } else {
    // add_point_light({2.5f, 0.0f, 2.5f}, {1.0f, 3.0f, 5.0f});
    for (int i = 0; i < 6; i++) {
      primitives.emplace_back(
          std::make_shared<Sphere>(1.0f, float3(-4 + 2 * i, 0.f, 0.f)),
          i != 2 ? Surface::create_glass(
                       float3(abs(sinf(i)), abs(cosf(i)), abs(1 - sinf(i))),
                       1.1f + 0.1f * i)
                 : Surface::create_glass(float3::ONE, 1.5f));
    }
    // primitives.emplace_back(std::make_shared<Sphere>(1.0f),
    //                         transparent_material);
    // primitives.emplace_back(
    //     std::make_shared<Sphere>(1.0f, float3(2.0f, 0.0f, 0.0f)),
    //     transparent_material);
    float xl = 4.f;
    float yl = 1.f;
    float zl = 4.f;
    // Floor
    primitives.emplace_back(std::make_shared<Triangle>(float3(-xl, -yl, zl),
                                                       float3(xl, -yl, -zl),
                                                       float3(-xl, -yl, -zl)),
                            green_material);
    primitives.emplace_back(std::make_shared<Triangle>(float3(-xl, -yl, zl),
                                                       float3(xl, -yl, zl),
                                                       float3(xl, -yl, -zl)),
                            green_material);
    // Back wall
    primitives.emplace_back(std::make_shared<Triangle>(float3(-xl, -yl, -zl),
                                                       float3(xl, yl, -zl),
                                                       float3(-xl, yl, -zl)),
                            default_material);
    primitives.emplace_back(std::make_shared<Triangle>(float3(-xl, -yl, -zl),
                                                       float3(xl, -yl, -zl),
                                                       float3(xl, yl, -zl)),
                            default_material);
  }
}

bool Scene::intersect(const Ray &ray, Intersection &isect) const {
  // printf("(%.3f, %.3f, %.3f) (%.3f, %.3f, %.3f)\n", world_ray.origin.x(),
  //        world_ray.origin.y(), world_ray.origin.z(), world_ray.dir.x(),
  //        world_ray.dir.y(), world_ray.dir.z());

  bool any_hit = false;
  isect.t = std::numeric_limits<float>::infinity();
  for (const auto &prim : primitives) {
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

float3 Scene::get_sky_light(const float3 &world_dir) const {
  // phi in [0, 2*pi]
  // When phi==pi, x is 1 and z is 0
  float phi = atan2f(world_dir.z(), world_dir.x());
  phi += M_PI;
  // theta in [0, pi]
  float theta = acosf(world_dir.y());

  if (sky_light_hdr_image) {
    return sky_light_hdr_image->get_color_spherical(theta, phi);
  }

  return sky_light_value;
}
} // namespace verdant
