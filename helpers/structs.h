#ifndef STRUCTS_H
#define STRUCTS_H
#include "includes.h"

struct aoi {
  unsigned long step_number;
  bool classified = false;
  std::vector<double> x;
  std::vector<double> intensity;
  std::vector<double> fft_intensity;
  std::vector<double> average_x;
  std::vector<double> average_kf_intensity;
  std::vector<double> average_intensity_normalized;
  std::vector<double> average_intensity_subtracted;
  std::vector<double> average_y_intensity_histogram;
  std::vector<double> average_x_intensity_reduced;
};
#endif // STRUCTS_H
