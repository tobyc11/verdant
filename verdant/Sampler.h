#pragma once
#include "MathDefs.h"
#include <random>
#include <tuple>

namespace verdant {
class UniformSampler {
public:
  UniformSampler();

  std::tuple<float, float> sample() {
    std::uniform_real_distribution<float> dist(0, 1);
    return {pdf(), dist(random_engine)};
  }

  float pdf() const { return 1.0f; }

  static UniformSampler &per_thread();

private:
  std::mt19937 random_engine;
};

class UniformHemisphereDistribution {
public:
  // Returns the pdf and the unit vector representing the direction
  std::tuple<float, float3> sample(UniformSampler &base_sampler) const {
    auto [u0_pdf, u0] = base_sampler.sample();
    auto [u1_pdf, u1] = base_sampler.sample();

    float phi = 2 * M_PI * u0;
    float theta = acos(u1);

    float x = sin(theta) * cos(phi);
    float y = sin(theta) * sin(phi);
    float z = cos(theta);

    float pdf = 1.0f / (2 * M_PI);
    return {pdf, {x, y, z}};
  }

  float pdf(const float3 &value) const { return 1.0f / (2 * M_PI); }
};

class CosineWeightedHemisphereDistribution {
public:
  // Returns the pdf and the unit vector representing the direction
  std::tuple<float, float3> sample(UniformSampler &base_sampler) const {
    auto [u0_pdf, u0] = base_sampler.sample();
    auto [u1_pdf, u1] = base_sampler.sample();

    float phi = 2 * M_PI * u0;
    float theta = acos(sqrt(u1));

    float x = sin(theta) * cos(phi);
    float y = sin(theta) * sin(phi);
    float z = cos(theta);

    float pdf = z / M_PI;
    return {pdf, {x, y, z}};
  }

  float pdf(const float3 &value) const {
    // Assert value is a normal vector
    return value.z() / M_PI;
  }
};

// Takes the value 1 with probability p and value 0 with probability 1-p
inline int bernoulli_toss(UniformSampler &sampler, float p) {
  auto [pdf, x] = sampler.sample();
  if (x < p) {
    return 1;
  }
  return 0;
}
} // namespace verdant
