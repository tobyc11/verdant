#include "Integrator.h"
#include "MathDefs.h"
#include "Sampler.h"
#include "Scene.h"
#include <limits>

namespace verdant {
float3 PathTraceIntegrator::Lo_from_ray(UniformSampler &sampler,
                                        const Scene &scene, const Ray &ray,
                                        int depth) const {
  Intersection isect;
  if (scene.intersect(ray, isect)) {
    float3 world_hit_pos = ray.origin + ray.dir * isect.t;

    float3 Lo = float3::ZERO;

    Lo += integrate_direct(sampler, scene, isect, -ray.dir, world_hit_pos);

    Lo += integrate_indirect(sampler, scene, isect, -ray.dir, world_hit_pos);

    return Lo;
  } else {
    if (scene.has_sky_light()) {
      return scene.get_sky_light(ray.dir);
    } else {
      return float3::ZERO;
    }
  }
}

float3 PathTraceIntegrator::integrate_direct(UniformSampler &sampler,
                                             const Scene &scene,
                                             const Intersection &isect,
                                             const float3 &world_view,
                                             const float3 &world_pos) const {
  float3 Lo = float3::ZERO;

  // L2W is orthonormal
  float3x3 L2W = isect.make_tangent_basis();
  float3x3 W2L = transpose(L2W);
  float3 V = W2L * world_view;

  // Loop through scene's point lights
  if (!isect.material->is_delta()) {
    for (const auto &pl : scene.get_point_lights()) {
      float3 L = pl.get_position() - world_pos;
      float light_distance = L.normalize();
      Ray new_ray(world_pos + L * RAY_EPS, L);
      Intersection new_isect;
      bool hit = scene.intersect(new_ray, new_isect);
      if (hit && new_isect.t < light_distance) {
        continue;
      }

      // Light is not occluded
      Lo += isect.material->f(W2L * L, V) * pl.get_irradiance() *
            dot(L, isect.normal);
    }
  }

  // For sky light, sample a direction from the normal hemisphere
  if (scene.has_sky_light()) {
    if (isect.material->is_delta()) {
      float3 L;
      float pdf;
      float3 f = isect.material->sample_f(sampler, V, L, pdf);

      // Only taking one sample
      float3 world_L = L2W * L;
      Ray new_ray(world_pos + world_L * RAY_EPS, world_L);
      Intersection new_isect;
      bool hit = scene.intersect(new_ray, new_isect);
      if (!hit) {
        Lo += f * scene.get_sky_light(world_L) * std::abs(L.z()) / pdf;
      }
    } else {
      float3 Lo_sky = float3::ZERO;
      for (int i = 0; i < light_samples; i++) {
        CosineWeightedHemisphereDistribution dist;
        auto [pdf, local_dir] = dist.sample(sampler);

        float3 world_dir = L2W * local_dir;
        Ray new_ray(world_pos + world_dir * RAY_EPS, world_dir);
        Intersection new_isect;
        bool hit = scene.intersect(new_ray, new_isect);
        if (!hit) {
          Lo_sky += isect.material->f(local_dir, V) *
                    scene.get_sky_light(world_dir) * local_dir.z() / pdf;
        }
      }

      Lo += Lo_sky / light_samples;
    }
  }

  return Lo;
}

float3 PathTraceIntegrator::integrate_indirect(UniformSampler &sampler,
                                               const Scene &scene,
                                               const Intersection &isect,
                                               const float3 &world_view,
                                               const float3 &world_pos) const {
  // Russian roulette termination
  auto [_unused, u] = sampler.sample();
  if (u > continue_prob) {
    return float3::ZERO;
  }

  // L2W is orthonormal
  float3x3 L2W = isect.make_tangent_basis();
  float3x3 W2L = transpose(L2W);
  float3 V = W2L * world_view;

  float3 L;
  float pdf;
  float3 f = isect.material->sample_f(sampler, V, L, pdf);
  float3 world_L = L2W * L;
  Ray next_ray(world_pos + world_L * RAY_EPS, world_L);
  Intersection next_isect;
  if (scene.intersect(next_ray, next_isect)) {
    float3 Li = float3::ZERO;
    Li = integrate_direct(sampler, scene, next_isect, -world_L,
                          next_ray.origin + next_isect.t * next_ray.dir);

    Li += integrate_indirect(sampler, scene, next_isect, -world_L,
                             next_ray.origin + next_isect.t * next_ray.dir);

    float3 Lo = f * Li * std::abs(L.z()) / pdf / continue_prob;
    return Lo;
  } else {
    return float3::ZERO;
  }

  /*
  CosineWeightedHemisphereDistribution hemi_dist;
  auto [pdf, local_dir] = hemi_dist.sample(sampler);
  float3 world_dir = L2W * local_dir;
  Ray next_ray(world_pos + world_dir * RAY_EPS, world_dir);
  Intersection next_isect;
  if (scene.intersect(next_ray, next_isect)) {
    float3 Li = float3::ZERO;
    Li = integrate_direct(sampler, scene, next_isect, -world_dir,
                          next_ray.origin + next_isect.t * next_ray.dir);

    Li += integrate_indirect(sampler, scene, next_isect, -world_dir,
                             next_ray.origin + next_isect.t * next_ray.dir);

    float3 Lo = isect.material->f(local_dir, V) * Li * local_dir.z() / pdf /
                continue_prob;
    return Lo;
  } else {
    return float3::ZERO;
  }
  */
}

float3 DirectOnlyIntegrator::Lo_from_ray(UniformSampler &sampler,
                                         const Scene &scene,
                                         const Ray &ray) const {
  // Camera ray
  Intersection isect;
  bool hit;
  hit = scene.intersect(ray, isect);
  if (!hit) {
    return direct_lighting(scene, ray);
  } else {
    // Direct lighting for the surface
    float3x3 L2W = isect.make_tangent_basis();
    float3x3 W2L = transpose(L2W);
    float3 V = W2L * -ray.dir;
    float3 world_pos = ray.origin + ray.dir * isect.t;

    float3 L;
    float pdf;
    float3 f = isect.material->sample_f(sampler, V, L, pdf);
    float3 world_L = L2W * L;
    float3 Li =
        direct_lighting(scene, Ray(world_pos + world_L * RAY_EPS, world_L));
    return f * Li * L.z() / pdf;
  }
}

float3 DirectOnlyIntegrator::direct_lighting(const Scene &scene,
                                             const Ray &ray) const {
  Intersection isect;
  bool hit;
  hit = scene.intersect(ray, isect);
  if (!hit) {
    if (scene.has_sky_light()) {
      return scene.get_sky_light(ray.dir);
    }
  }
  return float3::ZERO;
}
} // namespace verdant
