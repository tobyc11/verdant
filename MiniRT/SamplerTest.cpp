#include "Sampler.h"

/*
// For testing the distributions
template <typename T, typename U> float integrate_one(T &dist, U &base, int N) {
  float c = 1.0f / N;
  float result = 0.0f;
  for (int i = 0; i < N; i++) {
    auto [pdf, sample] = dist.sample(base);
    result += c * 1 / pdf;
  }
  return result;
}

// For testing the distributions
int main() {
  const int N = 10000;
  const int ITERS = 100;
  float result = 0.0f;
  minirt::UniformSampler base;
  for (int i = 0; i < ITERS; i++) {
    minirt::CosineWeightedHemisphereDistribution dist;
    float batch_result = integrate_one(dist, base, N);
    result += batch_result / ITERS;
  }
  std::cout << result << std::endl;
  return 0;
}
*/
