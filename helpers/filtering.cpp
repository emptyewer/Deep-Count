#include "filtering.h"
#include "armadillo"
#include "filter/kalman.hpp"
#include <Eigen/Dense>

Filtering::Filtering() {}

void Filtering::fft(aoi *a) {
  arma::vec X(a->intensity);
  arma::cx_vec Y = arma::fft(X); // FFT
  for (unsigned long long i = 0; i < static_cast<unsigned long long>(Y.size());
       i++) {
    // Set the 90% of the higher frequency to zero
    if (i > static_cast<unsigned long long>(Y.size()) / 3) {
      Y[i].real(0.0);
      Y[i].imag(0.0);
    }
    a->x.push_back(i + 1);
  }

  arma::cx_vec iY = arma::ifft(Y); // Inverse FFT
  for (unsigned long long i = 0; i < static_cast<int>(iY.size()) * 0.95; i++) {
    a->fft_intensity.push_back(static_cast<double>(iY[i].real()));
    //    a->average_intensity.push_back((static_cast<double>(iY[i].real()) +
    //    a->intensity[i]) / 2);
  }
}

void Filtering::kalman_filter(aoi *a) {

  int n = 3; // Number of states
  int m = 1; // Number of measurements

  double dt = 1.0; // Time step

  Eigen::MatrixXd A(n, n); // System dynamics matrix
  Eigen::MatrixXd C(m, n); // Output matrix
  Eigen::MatrixXd Q(n, n); // Process noise covariance
  Eigen::MatrixXd R(m, m); // Measurement noise covariance
  Eigen::MatrixXd P(n, n); // Estimate error covariance

  // Discrete LTI projectile motion, measuring position only
  A << 1, dt, 0, 0, 1, dt, 0, 0, 1;
  C << 1, 0, 0;

  // Reasonable covariance matrices
  Q << .05, .05, .0, .05, .05, .0, .0, .0, .0;
  R << 5;
  P << .1, .1, .1, .1, 10000, 10, .1, 10, 100;

  //  std::cout << "A: \n" << A << std::endl;
  //  std::cout << "C: \n" << C << std::endl;
  //  std::cout << "Q: \n" << Q << std::endl;
  //  std::cout << "R: \n" << R << std::endl;
  //  std::cout << "P: \n" << P << std::endl;

  // Construct the filter
  KalmanFilter::KalmanFilter kf(dt, A, C, Q, R, P);

  // List of noisy position measurements (y)
  std::vector<double> measurements = a->intensity;

  // Best guess of initial states
  Eigen::VectorXd x0(n);
  x0 << measurements[0], 0, -9.81;
  kf.init(0, x0);

  // Feed measurements into filter, output estimated states
  double t = 0;
  Eigen::VectorXd y(m);
  a->x.push_back(0.0);
  a->kf_intensity.push_back(kf.state().transpose()[0]);
  for (unsigned long i = 0; i < measurements.size(); i++) {
    t += dt;
    y << measurements[i];
    kf.update(y);
    a->x.push_back(t);
    a->kf_intensity.push_back(kf.state().transpose()[0]);
  }
}

void Filtering::subtract_consecutive_intensities(aoi *a) {
  //  qDebug() << "Kalman Subtract Started";
  a->kf_intensity_subtracted = subtract(a->kf_intensity_normalized);
  //  qDebug() << "Kalman Subtracted";
  a->fft_intensity_subtracted = subtract(a->fft_intensity_normalized);
  //  qDebug() << "FFT Subtracted";
}

std::vector<double> Filtering::subtract(std::vector<double> arr) {
  std::vector<double> ret_array;
  for (unsigned j = 0; j < arr.size(); j++) {
    if (j > 0) {
      double diff = arr[j - 1] - arr[j];
      ret_array.push_back(diff);
    }
  }
  return ret_array;
}

void Filtering::reduce_dimensionality(aoi *a) {
  a->kf_x_intensity_histogram = subtract(a->kf_intensity_subtracted);
  a->fft_x_intensity_histogram = subtract(a->fft_intensity_subtracted);
}

std::vector<double> Filtering::reduce(std::vector<double> arr) {
  arma::vec v = arma::vec(arr);
  unsigned long long width =
      static_cast<unsigned long long>(floor(v.size() / 100));
  double fudge = v.size() % 100;
  unsigned long long start = 0;
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
  a->kf_intensity_normalized = normalize_vector(a->kf_intensity);
  a->fft_intensity_normalized = normalize_vector(a->fft_intensity);
}

std::vector<double> Filtering::normalize_vector(std::vector<double> arr) {
  std::vector<double> ret_arr;
  double minimum =
      (arr[arr.size() - 1] + arr[arr.size() - 2] + arr[arr.size() - 3] +
       arr[arr.size() - 4] + arr[arr.size() - 5]) /
      5;
  double maximum = arr[0];
  for (unsigned long i = 0; i < arr.size(); i++) {
    if (arr[i] > maximum) {
      maximum = arr[i];
    }
    //    if (arr[i] < minimum) {
    //      minimum = arr[i];
    //    }
  }

  bool found = false;
  for (unsigned j = 0; j < arr.size(); j++) {
    double value = (arr[j] - minimum) / (maximum - minimum);
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
  a->kf_y_intensity_histogram = histogram(a->kf_intensity_normalized);
  a->fft_y_intensity_histogram = histogram(a->fft_intensity_normalized);
}

std::vector<double> Filtering::histogram(std::vector<double> arr) {
  arma::vec v1 = arma::vec(arr);
  arma::uvec result1 = arma::hist(v1, 100);
  std::vector<double> vec;
  for (unsigned long i = 0; i < result1.size(); ++i) {
    if (result1[i] > 1) {
      vec.push_back(1.0);
    } else {
      vec.push_back(0.0);
    }
  }
  return vec;
}
