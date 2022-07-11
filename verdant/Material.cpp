#include "Material.h"
#include "BRDF.h"
#include "Sampler.h"
#include <memory>

namespace verdant {
std::shared_ptr<Material> Material::create_lambert(float3 c) {
  std::shared_ptr<Material> m(new Material(MaterialKind::Lambert));
  m->c = c;
  return m;
}

std::shared_ptr<Material> Material::create_reflect(float3 c, float eta) {
  std::shared_ptr<Material> m(new Material(MaterialKind::Reflect));
  m->c = c;
  m->etaI = 1.0f;
  m->etaT = eta;
  return m;
}

std::shared_ptr<Material> Material::create_refract(float3 c, float eta) {
  std::shared_ptr<Material> m(new Material(MaterialKind::Refract));
  m->c = c;
  m->etaI = 1.0f;
  m->etaT = eta;
  return m;
}

std::shared_ptr<Material> Material::create_glass(float3 c, float eta) {
  std::shared_ptr<Material> m(new Material(MaterialKind::Glass));
  m->c = c;
  m->etaI = 1.0f;
  m->etaT = eta;
  return m;
}

std::shared_ptr<Material> Material::create_specular(float3 c, float eta) {
  std::shared_ptr<Material> m(new Material(MaterialKind::Specular));
  m->c = c;
  m->etaI = 1.0f;
  m->etaT = eta;
  return m;
}

float3 Material::f(const float3 &L, const float3 &V) const {
  if (is_delta()) {
    return float3::ZERO;
  }
  return F_lambert(c);
}

inline bool refract(const float3 &V, const float3 &N, float eta, float3 &T) {
  // Based on the pbrt implementation
  float cosThetaI = std::abs(dot(V, N)); // abs??
  float sin2ThetaI = std::max(0.f, 1.f - cosThetaI * cosThetaI);
  float sin2ThetaT = eta * eta * sin2ThetaI;
  if (sin2ThetaT >= 1) {
    return false;
  }
  float cosThetaT = std::sqrt(1 - sin2ThetaT);

  T = eta * -V + (eta * cosThetaI - cosThetaT) * N;
  T = normalize(T);
  return true;
}

float3 Material::sample_f(UniformSampler &sampler, const float3 &V, float3 &L,
                          float &pdf) const {
  pdf = 1.0f;
  switch (kind) {
  case MaterialKind::Lambert:
    return sample_f_lambert(sampler, V, L, pdf);
  case MaterialKind::Reflect:
    return sample_f_reflection(sampler, V, L, pdf);
  case MaterialKind::Refract:
    return sample_f_refraction(sampler, V, L, pdf);
  case MaterialKind::Glass:
    return sum_f(sampler, V, L, pdf, &Material::sample_f_reflection,
                 &Material::sample_f_refraction);
  case MaterialKind::Specular:
    return sum_f(sampler, V, L, pdf, &Material::sample_f_reflection,
                 &Material::sample_f_lambert);
  }
}

float3 Material::sample_f_lambert(UniformSampler &sampler, const float3 &V,
                                  float3 &L, float &pdf) const {
  CosineWeightedHemisphereDistribution dist;
  auto [pdf2, L2] = dist.sample(sampler);
  L = L2;
  pdf *= pdf2;
  return F_lambert(c);
}

float3 Material::sample_f_reflection(UniformSampler &sampler, const float3 &V,
                                     float3 &L, float &pdf) const {
  // Specular reflection
  L = float3(-V.x(), -V.y(), V.z());
  float cosThetaT = std::abs(L.z());
  return c * Fr_dielectric(cosThetaT, etaI, etaT) / cosThetaT;
}

float3 Material::sample_f_refraction(UniformSampler &sampler, const float3 &V,
                                     float3 &L, float &pdf) const {
  // Specular transmission
  bool entering = V.z() > 0.0f;
  float etaA = entering ? etaI : etaT;
  float etaB = entering ? etaT : etaI;

  bool ok = refract(V, float3(0, 0, 1), etaA / etaB, L);
  if (!ok) {
    // Total internal reflection occured, no refraction possible
    return float3::ZERO;
  }

  float cosThetaT = std::abs(L.z());
  return c * (1 - Fr_dielectric(cosThetaT, etaA, etaB)) / cosThetaT;
}
} // namespace verdant
