#ifndef FFT_H
#define FFT_H
#include "armadillo"
#include "helpers/includes.h"
#include "helpers/structs.h"
#include <cfloat>
#include <climits>

class Filtering {
public:
  Filtering();
  //  void fft(aoi *a);
  //  void kalman_filter(aoi *a);
  void normalize(aoi *a);
  void subtract_consecutive_intensities(aoi *a);
  void calculate_histogram(aoi *a);
  void reduce_dimensionality(aoi *a);
  void detect_changepoints(aoi *a);

private:
  std::vector<double> normalize_vector(std::vector<double> arr);
  std::vector<double> subtract(std::vector<double> arr);
  std::vector<double> reduce(std::vector<double> arr);
  std::vector<double> histogram(std::vector<double> arr);
  std::vector<double> cumulative_sums(std::vector<double> arr);
  std::vector<double> randomize(std::vector<double> arr);
  double bootstrap(std::vector<double> arr);
  std::vector<double> convert_arma_std(arma::vec v);
  unsigned long long find_index_of_maximum(std::vector<double> arr);
  std::vector<double> slice(const std::vector<double> v, unsigned long,
                            unsigned long);
  void get_changepoints(std::vector<double>, std::vector<unsigned long long> *,
                        double, unsigned long);
  double find_max(std::vector<double> arr);
  double find_min(std::vector<double> arr);
  double find_mean(std::vector<double> arr);
  void get_filtered_csum_intensities(aoi *, std::vector<unsigned long long> change_points);
  void print_array(std::vector<double> arr);
  void print_int_array(std::vector<unsigned long long> arr);
};

#endif // FFT_H
