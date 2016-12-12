#ifndef FFT_H
#define FFT_H
#include "helpers/includes.h"
#include "helpers/structs.h"

class Filtering
{
public:
  Filtering();
  void fft(aoi *a);
  void normalize(aoi *a);
  void subtract_consecutive_intensities(aoi *a);
  void calculate_histogram(aoi *a);
  void reduce_dimensionality(aoi *a);
  void kalman_filter(aoi *a);
private:
  std::vector<double> normalize_vector(std::vector<double> arr);
};

#endif // FFT_H
