#include "Surface.h"
#include "BRDF.h"
#include "Sampler.h"
#include <memory>

namespace verdant {
std::shared_ptr<Surface> Surface::create_lambert(float3 c) {
  std::shared_ptr<Surface> m(new Surface(SurfaceKind::Lambert));
  m->c = c;
  return m;
}

std::shared_ptr<Surface> Surface::create_reflect(float3 c, float eta) {
  std::shared_ptr<Surface> m(new Surface(SurfaceKind::Reflect));
  m->c = c;
  m->etaI = 1.0f;
  m->etaT = eta;
  return m;
}

std::shared_ptr<Surface> Surface::create_refract(float3 c, float eta) {
  std::shared_ptr<Surface> m(new Surface(SurfaceKind::Refract));
  m->c = c;
  m->etaI = 1.0f;
  m->etaT = eta;
  return m;
}

std::shared_ptr<Surface> Surface::create_glass(float3 c, float eta) {
  std::shared_ptr<Surface> m(new Surface(SurfaceKind::Glass));
  m->c = c;
  m->etaI = 1.0f;
  m->etaT = eta;
  return m;
}

std::shared_ptr<Surface> Surface::create_specular(float3 c, float eta) {
  std::shared_ptr<Surface> m(new Surface(SurfaceKind::Specular));
  m->c = c;
  m->etaI = 1.0f;
  m->etaT = eta;
  return m;
}

float3 Surface::f(const float3 &L, const float3 &V) const {
  if (is_delta()) {
    return float3::ZERO;
  }
  return F_lambert(c);
}

float3 Surface::sample_f(UniformSampler &sampler, const float3 &V, float3 &L,
                         float &pdf) const {
  pdf = 1.0f;
  switch (kind) {
  case SurfaceKind::Lambert:
    return sample_f_lambert(sampler, V, L, pdf);
  case SurfaceKind::Reflect:
    return sample_f_reflection(sampler, V, L, pdf);
  case SurfaceKind::Refract:
    return sample_f_refraction(sampler, V, L, pdf);
  case SurfaceKind::Glass:
    return sample_f_glass(sampler, V, L, pdf);
  case SurfaceKind::Specular:
    return sample_f_specular(sampler, V, L, pdf);
  }
}

float3 Surface::sample_f_lambert(UniformSampler &sampler, const float3 &V,
                                 float3 &L, float &pdf) const {
  CosineWeightedHemisphereDistribution dist;
  auto [pdf2, L2] = dist.sample(sampler);
  L = L2;
  pdf *= pdf2;
  return F_lambert(c);
}

float3 Surface::sample_f_reflection(UniformSampler &sampler, const float3 &V,
                                    float3 &L, float &pdf) const {
  // Specular reflection
  L = reflect(V, float3(0, 0, 1));
  float cosThetaT = std::abs(L.z());
  return c * Fr_dielectric(cosThetaT, etaI, etaT) / cosThetaT;
}

float3 Surface::sample_f_refraction(UniformSampler &sampler, const float3 &V,
                                    float3 &L, float &pdf) const {
  // Specular transmission
  bool entering = V.z() > 0.0f;
  float etaA = entering ? etaI : etaT;
  float etaB = entering ? etaT : etaI;

  L = refract(V, float3(0, 0, 1), etaA / etaB);
  if (L.z() < 1e-6) {
    // Total internal reflection occured, no refraction possible
    return float3::ZERO;
  }

  float cosThetaT = std::abs(L.z());
  return c * (1 - Fr_dielectric(cosThetaT, etaA, etaB)) / cosThetaT;
}

float3 Surface::sample_f_glass(UniformSampler &sampler, const float3 &V,
                               float3 &L, float &pdf) const {
  bool entering = V.z() > 0.f;
  float etaA = entering ? etaI : etaT;
  float etaB = entering ? etaT : etaI;
  float k_reflect = Fr_dielectric(std::abs(V.z()), etaA, etaB);

  auto [_pdf, roll] = sampler.sample();
  if (roll < k_reflect) {
    L = reflect(V, float3(0, 0, 1));
    return c / std::abs(L.z());
  } else {
    L = refract(V, float3(0, 0, 1), etaA / etaB);
    if (L.z() < 1e-6) {
      return float3::ZERO;
    }
    return c / std::abs(L.z());
  }
}

float3 Surface::sample_f_specular(UniformSampler &sampler, const float3 &V,
                                  float3 &L, float &pdf) const {
  bool entering = V.z() > 0.f;
  float etaA = entering ? etaI : etaT;
  float etaB = entering ? etaT : etaI;
  float k_reflect = Fr_dielectric(std::abs(V.z()), etaA, etaB);

  auto [_pdf, roll] = sampler.sample();
  if (roll < k_reflect) {
    L = float3(-V.x(), -V.y(), V.z());
    return c / std::abs(L.z());
  } else {
    return sample_f_lambert(sampler, V, L, pdf);
  }
}
} // namespace verdant
