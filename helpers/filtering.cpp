#include "filtering.h"
#include <algorithm>
#include <cfloat>
#include <math.h>
//#include "filter/kalman.hpp"
//#include <Eigen/Dense>

Filtering::Filtering() {}

std::vector<double> Filtering::cumulative_sums(std::vector<double> arr) {
  // Calculates the cumulative sums according to
  // http://www.variation.com/cpa/tech/changepoint.html
  double series_average = find_mean(arr);
  std::vector<double> csums = {0.0};
  for (unsigned long i = 0; i < arr.size(); i++) {
    csums.push_back((arr.at(i) - series_average) + csums.at(i));
  }
  return csums;
}

double Filtering::find_mean(std::vector<double> arr) {
  double m = 0;
  for (unsigned long i = 0; i < arr.size(); i++) {
    m = m + arr.at(i);
  }
  return m / arr.size();
}

std::vector<double> Filtering::randomize(std::vector<double> arr) {
  // Shuffles the elements of the vector
  std::random_shuffle(std::begin(arr), std::end(arr));
  return arr;
}

std::vector<double> Filtering::convert_arma_std(arma::vec v) {
  std::vector<double> result;
  for (unsigned long i = 0; i < v.size(); i++) {
    result.push_back(v.at(i));
  }
  return result;
}

double Filtering::bootstrap(std::vector<double> arr) {
  /*Description:    used, along with the confidence interval,
                    to detect if a change occurred int he series.
                    Creates 1000 bootsrapped series, shuffles the original data
    list
                    then calculates the cumulative sum for each shuffled data
    series
    Arguments:      data - a list of floats
    Returns:        returns the confidence interval*/
  double iterations = 1000;
  std::vector<double> cumsum_original = cumulative_sums(arr);
  double sdiff_original = find_max(cumsum_original) - find_min(cumsum_original);

  std::vector<double> x;
  for (unsigned long i = 0; i < iterations; i++) {
    std::vector<double> temp = cumulative_sums(randomize(arr));
    x.push_back(find_max(temp) - find_min(temp));
  }

  unsigned long n = 0;
  for (unsigned long j = 0; j < x.size(); j++) {
    if (x.at(j) < sdiff_original) {
      n = n + 1;
    }
  }
  double s = (n / static_cast<double>(iterations)) * 100.0;
  return s;
}

double Filtering::find_max(std::vector<double> arr) {
  double m = INT64_MIN;
  for (unsigned long i = 0; i < arr.size(); i++) {
    if (arr.at(i) > m) {
      m = arr.at(i);
    }
  }
  return m;
}

double Filtering::find_min(std::vector<double> arr) {
  double m = 999999999999999999999999.0;
  for (unsigned long i = 0; i < arr.size(); i++) {
    if (arr.at(i) < m) {
      m = arr.at(i);
    }
  }
  return m;
}

unsigned long long
Filtering::find_index_of_maximum(std::vector<double> cumsums) {
  double max_number = -9999999999999999999999999.0;
  unsigned long long max_index = 0;
  std::vector<double> absolute_cumsums;
  for (unsigned long i = 0; i < cumsums.size(); i++) {
    absolute_cumsums.push_back(std::abs(cumsums.at(i)));
  }

  for (unsigned long long j = 0; j < absolute_cumsums.size(); j++) {
    if (absolute_cumsums.at(j) > max_number) {
      max_number = absolute_cumsums.at(j);
      max_index = j;
    }
  }
  return max_index;
}

std::vector<double> Filtering::slice(const std::vector<double> v,
                                     unsigned long start, unsigned long end) {
  std::vector<double> nv = {};
  for (unsigned long i = start; i < end - 1; i++) {
    nv.push_back(v.at(i));
  }
  return nv;
}

void Filtering::get_changepoints(std::vector<double> arr,
                                 std::vector<unsigned long long> *cp,
                                 double confidence_level = 95,
                                 unsigned long offset = 0) {
  /* Description:    Call the function by passing a data series
                    Once a change has been detected, break the data into two
    segments,
                    one each side of the change-point, and the analysis repeated
    for each segment.
    Returns:        Indexes of change points detected in the data series */
  double confidence = bootstrap(arr);
  if (confidence > confidence_level) {
    std::vector<double> cumsum = cumulative_sums(arr);
    unsigned long long max_index = find_index_of_maximum(cumsum);
    bool small = false;
    for (unsigned long i = 0; i < cp->size(); i++) {
      if (((long long)max_index + (long long)offset - (long long)cp->at(i)) ==
              1 ||
          ((long long)max_index + (long long)offset - (long long)cp->at(i)) ==
              -1) {
        small = true;
      }
    }
    if (max_index + offset > 0 && small == false) {
      cp->push_back(max_index + offset);
    }
    if (max_index > 0 && max_index < arr.size()) {
      std::vector<double> lower_slice = slice(arr, 0, max_index);
      get_changepoints(lower_slice, cp, confidence_level, offset);

      std::vector<double> upper_slice = slice(arr, max_index, arr.size());
      get_changepoints(upper_slice, cp, confidence_level,
                       offset + max_index - 1);
    }
  }
}

void Filtering::detect_changepoints(aoi *a) {
  std::vector<unsigned long long> change_points = {};
  get_changepoints(a->intensity, &change_points);
  std::sort(change_points.begin(), change_points.end(), std::less<double>());
  if (change_points.size() != 0) {
      get_filtered_csum_intensities(a, change_points);
    }
  else {
      double mean = find_mean(a->intensity);
      for (unsigned long long i = 0; i < a->intensity.size(); i++) {
          a->csum_intensity.push_back(mean);
        }
    }
  //  std::vector<double> b = {};
  //  get_changepoints(b, &change_points);
}

void Filtering::get_filtered_csum_intensities(
    aoi *a, std::vector<unsigned long long> change_points) {
  std::vector<double> means = {};
  unsigned long int start = 0;
  for (unsigned long long i = 0; i < change_points.size(); i++) {
    std::vector<double> temp = slice(a->intensity, start, change_points.at(i));
    double m = find_mean(temp);
    means.push_back(m);
    start = change_points.at(i);
  }

  means.push_back(
      find_mean(slice(a->intensity, change_points.at(change_points.size() - 1),
                      a->intensity.size())));
  unsigned long count = 0;
  for (unsigned long long j = 0; j < change_points.at(change_points.size() - 1);
       j++) {
    a->x.push_back(j + 1);
    if (j < change_points.at(count)) {
      a->csum_intensity.push_back(means.at(count));
    } else {
      count += 1;
      a->csum_intensity.push_back(means.at(count));
    }
  }
  count += 1;
  for (unsigned long k = change_points.at(change_points.size() - 1);
       k < a->intensity.size(); k++) {
    a->x.push_back(k + 1);
    a->csum_intensity.push_back(means.at(count));
  }
}

void Filtering::print_int_array(std::vector<unsigned long long> arr) {
  for (unsigned long long h = 0; h < arr.size(); h++) {
    std::cout << std::to_string(arr.at(h)) << ",";
  }
  std::cout << "\n";
}

void Filtering::print_array(std::vector<double> arr) {
  for (unsigned long long h = 0; h < arr.size(); h++) {
    std::cout << std::to_string(arr.at(h)) << ",";
  }
  std::cout << "\n";
}

void Filtering::subtract_consecutive_intensities(aoi *a) {
  //  qDebug() << "Kalman Subtract Started";
  //  a->kf_intensity_subtracted = subtract(a->kf_intensity_normalized);
  //  qDebug() << "Kalman Subtracted";
  //  a->fft_intensity_subtracted = subtract(a->fft_intensity_normalized);
  //  qDebug() << "FFT Subtracted";
  a->csum_intensity_subtracted = subtract(a->csum_intensity_normalized);
}

std::vector<double> Filtering::subtract(std::vector<double> arr) {
  std::vector<double> ret_array;
  for (unsigned j = 0; j < arr.size(); j++) {
    if (j > 0) {
      double diff = std::abs(arr.at(j - 1) - arr.at(j));
      ret_array.push_back(diff);
    }
  }
  return ret_array;
}

void Filtering::reduce_dimensionality(aoi *a) {
  //  a->kf_x_intensity_histogram = subtract(a->kf_intensity_subtracted);
  //  a->fft_x_intensity_histogram = subtract(a->fft_intensity_subtracted);
  a->csum_x_intensity_histogram = subtract(a->csum_intensity_subtracted);
}

std::vector<double> Filtering::reduce(std::vector<double> arr) {
  arma::vec v = arma::vec(arr);
  unsigned long width = static_cast<unsigned long>(floor(v.size() / 100));
  double fudge = v.size() % 100;
  unsigned long start = 0;
  int c = 0;
  std::vector<double> reduced_vec;
  while (true) {
    if (c < fudge) {
      double s1 = arma::sum(v.subvec(start, start + width));
      reduced_vec.push_back(s1);

      start = start + width + 1;
    } else {
      if ((start + width) > v.size()) {
        break;
      }
      double s2 = arma::sum(v.subvec(start, start + width - 1));
      reduced_vec.push_back(s2);
      start += width;
    }
    c += 1;
  }
  return reduced_vec;
}

void Filtering::normalize(aoi *a) {
  //  a->kf_intensity_normalized = normalize_vector(a->kf_intensity);
  //  a->fft_intensity_normalized = normalize_vector(a->fft_intensity);
  a->csum_intensity_normalized = normalize_vector(a->csum_intensity);
}

std::vector<double> Filtering::normalize_vector(std::vector<double> arr) {
  std::vector<double> ret_arr;
  double minimum =
      (arr.at(arr.size() - 1) + arr.at(arr.size() - 2) + arr.at(arr.size() - 3) +
       arr.at(arr.size() - 4) + arr.at(arr.size() - 5)) /
      5;
  double maximum = arr.at(0);
  for (unsigned long i = 0; i < arr.size(); i++) {
    if (arr.at(i) > maximum) {
      maximum = arr.at(i);
    }
    //    if (arr.at(i) < minimum) {
    //      minimum = arr.at(i);
    //    }
  }

  bool found = false;
  for (unsigned j = 0; j < arr.size(); j++) {
    double value = (arr.at(j) - minimum) / (maximum - minimum);
    if (found == false) {
      if (value > 0) {
        ret_arr.push_back(value);
        found = true;
      }
    } else {
      ret_arr.push_back(value);
    }
  }
  return ret_arr;
}

void Filtering::calculate_histogram(aoi *a) {
  //  a->kf_y_intensity_histogram = histogram(a->kf_intensity_normalized);
  //  a->fft_y_intensity_histogram = histogram(a->fft_intensity_normalized);
  a->csum_y_intensity_histogram = histogram(a->csum_intensity_normalized);
}

std::vector<double> Filtering::histogram(std::vector<double> arr) {
  arma::vec v1 = arma::vec(arr);
  arma::uvec result1 = arma::hist(v1, 100);
  std::vector<double> vec;
  for (unsigned long i = 0; i < result1.size(); ++i) {
    if (result1.at(i) > 1) {
      vec.push_back(1.0);
    } else {
      vec.push_back(0.0);
    }
  }
  return vec;
}

// void Filtering::fft(aoi *a) {
//  arma::vec X(a->intensity);
//  arma::cx_vec Y = arma::fft(X); // FFT
//  for (unsigned long i = 0; i < static_cast<unsigned long
//  long>(Y.size());
//       i++) {
//    // Set the 90% of the higher frequency to zero
//    if (i > static_cast<unsigned long>(Y.size()) / 3) {
//      Y.at(i).real(0.0);
//      Y.at(i).imag(0.0);
//    }
//    a->x.push_back(i + 1);
//  }

//  arma::cx_vec iY = arma::ifft(Y); // Inverse FFT
//  for (unsigned long i = 0; i < static_cast<int>(iY.size()) * 0.95; i++)
//  {
//    a->fft_intensity.push_back(static_cast<double>(iY.at(i).real()));
//    //    a->average_intensity.push_back((static_cast<double>(iY.at(i).real())
//    +
//    //    a->intensity.at(i)) / 2);
//  }
//}

// void Filtering::kalman_filter(aoi *a) {

//  int n = 3; // Number of states
//  int m = 1; // Number of measurements

//  double dt = 1.0; // Time step

//  Eigen::MatrixXd A(n, n); // System dynamics matrix
//  Eigen::MatrixXd C(m, n); // Output matrix
//  Eigen::MatrixXd Q(n, n); // Process noise covariance
//  Eigen::MatrixXd R(m, m); // Measurement noise covariance
//  Eigen::MatrixXd P(n, n); // Estimate error covariance

//  // Discrete LTI projectile motion, measuring position only
//  A << 1, dt, 0, 0, 1, dt, 0, 0, 1;
//  C << 1, 0, 0;

//  // Reasonable covariance matrices
//  Q << .05, .05, .0, .05, .05, .0, .0, .0, .0;
//  R << 5;
//  P << .1, .1, .1, .1, 10000, 10, .1, 10, 100;

//  //  std::cout << "A: \n" << A << std::endl;
//  //  std::cout << "C: \n" << C << std::endl;
//  //  std::cout << "Q: \n" << Q << std::endl;
//  //  std::cout << "R: \n" << R << std::endl;
//  //  std::cout << "P: \n" << P << std::endl;

//  // Construct the filter
//  KalmanFilter::KalmanFilter kf(dt, A, C, Q, R, P);

//  // List of noisy position measurements (y)
//  std::vector<double> measurements = a->intensity;

//  // Best guess of initial states
//  Eigen::VectorXd x0(n);
//  x0 << measurements.at(0), 0, -9.81;
//  kf.init(0, x0);

//  // Feed measurements into filter, output estimated states
//  double t = 0;
//  Eigen::VectorXd y(m);
//  a->x.push_back(0.0);
//  a->kf_intensity.push_back(kf.state().transpose().at(0));
//  for (unsigned long i = 0; i < measurements.size(); i++) {
//    t += dt;
//    y << measurements.at(i);
//    kf.update(y);
//    a->x.push_back(t);
//    a->kf_intensity.push_back(kf.state().transpose().at(0));
//  }
//}
