#include "PathIntegrator.h"
#include "Sampler.h"

namespace verdant {
PathIntegrator::PathIntegrator(const Scene &scene, UniformSampler &sampler,
                               float pr_continue, int min_bounces)
    : scene(scene), sampler(sampler), pr_continue(pr_continue),
      min_bounces(min_bounces) {}

float3 PathIntegrator::Lo_from_ray(const Ray &ray, int bounces, int flags) {
  // Remember to divide the return result by pr_continue
  float my_pr_continue = pr_continue;
  if (bounces < min_bounces) {
    my_pr_continue = 1.0f;
  } else {
    auto [_pdf, x_continue] = sampler.sample();
    if (x_continue > my_pr_continue) {
      return float3::ZERO;
    }
  }

  float3 Lo = float3::ZERO;
  Intersection isect;
  bool hit = scene.intersect(ray, isect);
  if (hit && !(flags & PIF_NO_INDIRECT)) {
    // Setup surface interaction
    float3 world_pos = ray.origin + isect.t * ray.dir;
    float3 world_view = -ray.dir;
    float3x3 L2W = isect.make_tangent_basis();
    float3x3 W2L = transpose(L2W);
    float3 V = W2L * world_view;

    // Evaluate the rendering equation integral:
    //   /
    //   | Li * BRDF(L, V) * cosTheta * dw
    //   /

    // bool separate_direct = !isect.material->is_delta();
    bool separate_direct = false;
    if (separate_direct) {
      // Sample direct lighting
      CosineWeightedHemisphereDistribution dist;
      auto [pdf, L] = dist.sample(sampler);
      float3 world_L = L2W * L;
      Ray next_ray(world_pos + world_L * RAY_EPS, world_L);
      float3 Li = Lo_from_ray(next_ray, bounces + 1, PIF_NO_INDIRECT);
      float cosThetaL = std::abs(L.z());
      float3 f = isect.material->f(L, V);
      Lo += Li * f * cosThetaL / pdf;
    }

    {
      // BRDF centric sampling
      float3 L;
      float pdf;
      float3 f = isect.material->sample_f(sampler, V, L, pdf);
      float3 world_L = L2W * L;
      Ray next_ray(world_pos + world_L * RAY_EPS, world_L);
      float3 Li = Lo_from_ray(next_ray, bounces + 1,
                              separate_direct ? PIF_NO_DIRECT : 0);
      float cosThetaL = std::abs(L.z());
      Lo += Li * f * cosThetaL / pdf;
#ifndef NDEBUG
      printf("bounce %d Li = (%.3f, %.3f, %.3f)\n", bounces, Li.x(), Li.y(),
             Li.z());
      Li = f * cosThetaL / pdf;
      printf("bounce %d f * cosThetaL / pdf = (%.3f, %.3f, %.3f)\n", bounces,
             Li.x(), Li.y(), Li.z());
#endif
    }
    float3 debug = Lo;
#ifndef NDEBUG
    printf("bounce %d (%.3f, %.3f, %.3f)\n", bounces, debug.x(), debug.y(),
           debug.z());
#endif
    return Lo / my_pr_continue;
  } else if (!(flags & PIF_NO_DIRECT)) {
    if (scene.has_sky_light()) {
      float3 debug = scene.get_sky_light(ray.dir) / my_pr_continue;
#ifndef NDEBUG
      printf("bounce %d sky light (%.3f, %.3f, %.3f)\n", bounces, debug.x(),
             debug.y(), debug.z());
#endif
      return scene.get_sky_light(ray.dir) / my_pr_continue;
    } else {
      return float3::ZERO;
    }
  }
  return float3::ZERO;
}
} // namespace verdant
