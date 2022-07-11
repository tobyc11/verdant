#pragma once
#include "MathDefs.h"
#include "Sampler.h"
#include "Scene.h"

namespace verdant {
class PathTraceIntegrator {
public:
  float3 Lo_from_ray(UniformSampler &sampler, const Scene &scene,
                     const Ray &ray, int depth = 1) const;

  float3 integrate_direct(UniformSampler &sampler, const Scene &scene,
                          const Intersection &isect, const float3 &world_view,
                          const float3 &world_pos) const;

  float3 integrate_indirect(UniformSampler &sampler, const Scene &scene,
                            const Intersection &isect, const float3 &world_view,
                            const float3 &world_pos) const;

private:
  int light_samples = 8;
  float continue_prob = 0.7f;
};

class DirectOnlyIntegrator {
public:
  float3 Lo_from_ray(UniformSampler &sampler, const Scene &scene,
                     const Ray &ray) const;

  float3 direct_lighting(const Scene &scene, const Ray &ray) const;
};
} // namespace verdant
