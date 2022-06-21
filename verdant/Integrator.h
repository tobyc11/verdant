#pragma once
#include "MathDefs.h"
#include "Sampler.h"
#include "Scene.h"

namespace verdant {
class PathTraceIntegrator {
public:
  int get_only_depth() const { return only_depth; }
  void set_only_depth(int value) { only_depth = value; }

  float3 zero_bounce_radiance(const Scene &scene, const float3 &world_hit_pos,
                              const Intersection &isect);
  float3 Lo_from_ray(const Scene &scene, const Ray &ray, int depth = 1);

private:
  UniformSampler uniform_sampler;
  int only_depth = 0;
};
} // namespace verdant
