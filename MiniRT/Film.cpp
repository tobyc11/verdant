#include "Film.h"
#include "MathDefs.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>

namespace minirt {
Film::Film(unsigned int width, unsigned int height)
    : width(width), height(height) {
  s = std::make_unique<float3[]>(width * height);
  n = std::make_unique<unsigned int[]>(width * height);
  clear();
}

void Film::clear() {
  unsigned int x, y;
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      unsigned int i = y * width + x;
      s[i] = float3::ZERO;
      n[i] = 0;
    }
  }
}

float3 Film::get_radiance(unsigned int x, unsigned int y) const {
  unsigned int i = y * width + x;
  return s[i];
}

void Film::average_radiance(unsigned int x, unsigned int y, float3 Li) {
  unsigned int i = y * width + x;
  // WARNING: thread unsafe
  float3 prev_avg = s[i];
  unsigned int ni = n[i];
  s[i] = (prev_avg * ni + Li) / (ni + 1);
  n[i] = ni + 1;
}

void Film::accumulate_radiance(unsigned int x, unsigned int y, float3 Li) {
  unsigned int i = y * width + x;
  // WARNING: thread unsafe
  s[i] = s[i] + Li;
}

float2 Film::xy_to_uv(unsigned int x, unsigned int y) const {
  return {(float)x / width, (float)y / height};
}

void Film::write_to_ppm(const std::string &file_name) const {
  std::ofstream ofs(file_name);
  if (!ofs.is_open()) {
    fprintf(stderr, "Could not open file %s to write\n", file_name.c_str());
    return;
  }

  ofs << "P3" << std::endl;
  ofs << width << " " << height << std::endl;
  ofs << "255" << std::endl;

  unsigned int x, y;
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      if (x != 0) {
        ofs << " ";
      }
      unsigned int i = y * width + x;
      uint3 colors = clamp_to_255_gamma(s[i], float3::ONE * 3.f, 0.66f);
      ofs << colors[0] << " ";
      ofs << colors[1] << " ";
      ofs << colors[2];
    }
    ofs << std::endl;
  }
}
} // namespace minirt
