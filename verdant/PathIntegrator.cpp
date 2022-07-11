#include "PathIntegrator.h"

namespace verdant {
PathIntegrator::PathIntegrator(const Scene &scene, UniformSampler &sampler,
                               float pr_continue)
    : scene(scene), sampler(sampler), pr_continue(pr_continue) {}

float3 PathIntegrator::Lo_from_ray(const Ray &ray) {
  // Remember to divide the return result by pr_continue
  auto [_pdf, x_continue] = sampler.sample();
  if (x_continue > pr_continue) {
    return float3::ZERO;
  }

  Intersection isect;
  bool hit = scene.intersect(ray, isect);
  if (hit) {
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

    // BRDF centric sampling
    float3 L;
    float pdf;
    float3 f = isect.material->sample_f(sampler, V, L, pdf);
    float3 world_L = L2W * L;
    Ray next_ray(world_pos + world_L * RAY_EPS, world_L);
    float3 Li = Lo_from_ray(next_ray);
    float cosThetaL = std::abs(L.z());
    return Li * f * cosThetaL / pdf / pr_continue;
  } else {
    if (scene.has_sky_light()) {
      return scene.get_sky_light(ray.dir) / pr_continue;
    } else {
      return float3::ZERO;
    }
  }
}
} // namespace verdant
