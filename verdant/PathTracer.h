#pragma once
#include "Scene.h"

namespace verdant {
// Implements the classical Kajiya path-tracing
class PathTracer {
public:
  PathTracer(const Scene &scene, UniformSampler &sampler);

  float3 radiance(const Ray &in_ray);

private:
  const Scene &scene;
  UniformSampler &sampler;
};
} // namespace verdant
