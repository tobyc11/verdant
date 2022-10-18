#include "PathTracer.h"
#include <cassert>

namespace verdant {
// Implements the classical Kajiya path-tracing
PathTracer::PathTracer(const Scene &scene, UniformSampler &sampler)
    : scene(scene), sampler(sampler) {}

float3 PathTracer::radiance(const Ray &in_ray) {
  Ray ray = in_ray;
  bool specular_bounce = false;
  float3 beta(1, 1, 1);
  float3 L_out(0, 0, 0);
  const CosineWeightedHemisphereDistribution dist;

  for (unsigned int bounces = 0; bounces <= 5; bounces++) {
    // printf("Bounce %d\n", bounces);
    // printf("Ray: o=(%f, %f, %f) d=(%f, %f, %f)\n", ray.origin.x(),
    //        ray.origin.y(), ray.origin.z(), ray.dir.x(), ray.dir.y(),
    //        ray.dir.z());
    Intersection isect;
    bool hit = scene.intersect(ray, isect);
    if (hit) {
      float3 p = ray.origin + isect.t * ray.dir;
      // printf("Hit: (%f, %f, %f)\n", p.x(), p.y(), p.z());
    } else {
      // printf("Miss\n");
    }
    // Specular bounces skip direct lighting. The next bounce should add its
    // emission
    if (bounces == 0 || specular_bounce) {
      if (hit) {
        // TODO: emissive objects
      } else {
        L_out += scene.get_sky_light(ray.dir) * beta;
      }
    }
    if (!hit) {
      break;
    }

    // standard local frame setup after ray hit
    float3 world_pos = ray.origin + isect.t * ray.dir;
    float3 world_view = -ray.dir;
    float3x3 L2W = isect.make_tangent_basis();
    float3x3 W2L = transpose(L2W);
    float3 V = W2L * world_view;

    // Sample direct lighting
    if (!isect.material->is_delta()) {
      const int n_direct = 4;
      for (int i = 0; i < n_direct; i++) {
        auto [pdf, L] = dist.sample(sampler);
        Ray next_ray(world_pos + L2W * L * RAY_EPS, L2W * L);
        Intersection next_isect;
        bool hit = scene.intersect(next_ray, next_isect);
        if (hit) {
          // TODO: emissive objects
        } else {
          L_out += isect.material->f(L, V) * scene.get_sky_light(next_ray.dir) *
                   L.z() / pdf / n_direct * beta;
        }
      }
    }

    // Determine next bounce
    float3 fr, L;
    float pdf;
    if (isect.material->is_delta()) {
      specular_bounce = true;
      fr = isect.material->sample_f(sampler, V, L, pdf);
    } else {
      specular_bounce = false;
      std::tie(pdf, L) = dist.sample(sampler);
      fr = isect.material->f(L, V);
    }
    ray = Ray(world_pos + L2W * L * RAY_EPS, L2W * L);
    beta *= fr * fabs(L.z()) / pdf;
  }
  return L_out;
}
} // namespace verdant
