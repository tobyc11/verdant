#pragma once
#include "HDRImage.h"
#include "Material.h"
#include "MathDefs.h"
#include "Shape.h"
#include <memory>
#include <vector>

namespace verdant {
class Primitive {
public:
  Primitive(std::shared_ptr<Shape> shape, std::shared_ptr<Material> mat);
  bool intersect(const Ray &ray, Intersection &isect) const;

private:
  std::shared_ptr<Shape> shape;
  std::shared_ptr<Material> material;
};

class PointLight {
public:
  PointLight(float3 position, float3 irradiance)
      : position(position), irradiance(irradiance) {}

  float3 get_position() const { return position; }
  float3 get_irradiance() const { return irradiance; }

private:
  float3 position;
  float3 irradiance;
};

class Scene {
public:
  Scene();

  bool intersect(const Ray &ray, Intersection &isect) const;

  void add_point_light(float3 position, float3 irradiance) {
    point_lights.emplace_back(position, irradiance);
  }

  const std::vector<PointLight> &get_point_lights() const {
    return point_lights;
  }

  void set_sky_light(bool on, float3 value) {
    sky_light = on;
    sky_light_value = value;
  }
  void set_sky_light(bool on, std::shared_ptr<HDRImage> hdr_image) {
    sky_light = on;
    sky_light_hdr_image = std::move(hdr_image);
  }
  bool has_sky_light() const { return sky_light; }
  float3 get_sky_light(const float3 &world_dir) const;

private:
  std::vector<Primitive> primitives;
  std::vector<PointLight> point_lights;
  bool sky_light;
  float3 sky_light_value;
  std::shared_ptr<HDRImage> sky_light_hdr_image;
};
} // namespace verdant
