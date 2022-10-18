#include "HDRImage.h"
#include "Pipeline.h"
#include "TaskWorker.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

using namespace verdant;

int main(int argc, char **argv) {
  TaskWorker::init_default_workers();
  int samples = 32;
  std::string output_name = "image.ppm";
  std::shared_ptr<HDRImage> image;

  // --single x y
  int x, y, single_shot = 0;

  for (int i = 1; i < argc; i++) {
    auto arg = std::string(argv[i]);
    if (arg == "--single") {
      i += 1;
      if (i < argc) {
        x = atoi(argv[i]);
      } else {
        std::cerr << "--single needs arguments x and y" << std::endl;
        return -1;
      }
      i += 1;
      if (i < argc) {
        y = atoi(argv[i]);
      } else {
        std::cerr << "--single needs arguments x and y" << std::endl;
        return -1;
      }
    } else if (arg == "--samples" || arg == "-s") {
      i += 1;
      if (i < argc) {
        samples = atoi(argv[i]);
        if (samples == 0) {
          std::cerr << "--samples must be followed by a positive integer"
                    << std::endl;
          return -1;
        }
      } else {
        std::cerr << "--samples missing argument" << std::endl;
        return -1;
      }
    } else if (arg == "--output" || arg == "-o") {
      i += 1;
      if (i < argc) {
        output_name = std::string(argv[i]);
      } else {
        std::cerr << "--output missing argument" << std::endl;
        return -1;
      }
    } else if (arg == "--hdr-sky") {
      i += 1;
      if (i < argc) {
        image = std::make_shared<HDRImage>(argv[i]);
      } else {
        std::cerr << "--hdr-sky missing argument" << std::endl;
        return -1;
      }
    }
  }

  if (single_shot) {
    printf("Single shot pixel %d %d\n", x, y);
  } else {
    printf("Sample count is %d\n", samples);
  }
  PathTracePipeline pipeline(320 * 4, 240 * 4, samples);
  if (image) {
    pipeline.get_scene()->set_sky_light(true, image);
  }
  pipeline.get_scene()->build_bvh();

  if (single_shot) {
    pipeline.single_pixel(x, y);
  } else {
    pipeline.run(output_name);
  }
  TaskWorker::shutdown_default_workers();
  return 0;
}
