#ifndef STRUCTS_H
#define STRUCTS_H
#include "includes.h"

struct aoi {
  unsigned long step_number;
  bool classified = false;
  std::vector<double> x;
  std::vector<double> intensity;
  std::vector<double> fft_intensity;
  std::vector<double> fft_intensity_normalized;
  std::vector<double> fft_intensity_subtracted;
  std::vector<double> fft_y_intensity_histogram;
  std::vector<double> fft_x_intensity_histogram;
  std::vector<double> kf_intensity;
  std::vector<double> kf_intensity_normalized;
  std::vector<double> kf_intensity_subtracted;
  std::vector<double> kf_y_intensity_histogram;
  std::vector<double> kf_x_intensity_histogram;
};
#endif // STRUCTS_H
