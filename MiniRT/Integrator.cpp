#include "Integrator.h"
#include "MathDefs.h"
#include "Sampler.h"
#include "Scene.h"
#include <limits>

namespace verdant {
float3 PathTraceIntegrator::zero_bounce_radiance(const Scene &scene,
                                                 const float3 &world_hit_pos,
                                                 const Intersection &isect) {
  float3 Lo = float3::ZERO;

  // Loop through scene's point lights
  for (const auto &pl : scene.get_point_lights()) {
    float3 L = pl.get_position() - world_hit_pos;
    float light_distance = L.normalize();
    Ray new_ray(world_hit_pos + L * 1e-4, L);
    Intersection new_isect;
    bool hit = scene.intersect(new_ray, new_isect);
    if (hit && new_isect.t < light_distance) {
      continue;
    }

    // Light is not occluded
    Lo += isect.material->f() * pl.get_irradiance() * dot(L, isect.normal);
  }

  // For sky light, sample a direction from the normal hemisphere
  if (scene.has_sky_light()) {
    CosineWeightedHemisphereDistribution dist;
    auto [pdf, local_dir] = dist.sample(uniform_sampler);

    float3 world_dir = isect.make_tangent_basis() * local_dir;
    Ray new_ray(world_hit_pos + world_dir * 1e-4, world_dir);
    Intersection new_isect;
    bool hit = scene.intersect(new_ray, new_isect);
    if (!hit) {
      // Skylight is 1
      float3 sky_light = float3::ONE;
      Lo += isect.material->f() * sky_light * local_dir.z() / pdf;
    }
  }

  return Lo;
}

float3 PathTraceIntegrator::Lo_from_ray(const Scene &scene, const Ray &ray,
                                        int depth) {
  Intersection isect;
  if (scene.intersect(ray, isect)) {
    float3 world_hit_pos = ray.origin + ray.dir * isect.t;

    float3 Lo = float3::ZERO;

    if (depth == only_depth) {
      Lo += zero_bounce_radiance(scene, world_hit_pos, isect);
    }

    // Cutoff
    if (depth >= only_depth) {
      return Lo;
    }

    // Russian roulette termination
    auto [_unused, u] = uniform_sampler.sample();
    float continue_prob = 1.0f;
    if (u > continue_prob) {
      return Lo;
    }

    CosineWeightedHemisphereDistribution hemi_dist;
    auto [pdf, local_dir] = hemi_dist.sample(uniform_sampler);
    float3 world_dir = isect.make_tangent_basis() * local_dir;
    Ray next_ray(world_hit_pos + world_dir * 1e-4, world_dir);
    Lo += isect.material->f() * Lo_from_ray(scene, next_ray, depth + 1) *
          local_dir.z() / pdf / continue_prob;

    return Lo;
  } else {
    return float3::ZERO;
  }
}
} // namespace verdant
