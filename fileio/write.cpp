#include "write.h"

Write::Write() {}

std::vector<QString> Write::get_processed_files() {
  QString path = check_documents_directory();
  QString file_path = QDir(path).filePath(".processed_files");
  std::vector<QString> filenames;
  std::ifstream input(file_path.toStdString());
  for (std::string line; getline(input, line);) {
    filenames.push_back(QString::fromStdString(line));
  }
  input.close();
  return filenames;
}

void Write::processed_to_file(QString s) {
  QString path = check_documents_directory();
  QString file_path = QDir(path).filePath(".processed_files");
  QByteArray file_name = file_path.toLocal8Bit();
  char *file_name_buffer = file_name.data();
  FILE *f = fopen(file_name_buffer, "aw");
  fprintf(f, "%s\n", s.toStdString().c_str());
  fclose(f);
}

QString Write::check_documents_directory() {
  QString documents_dir =
      QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(),
                             QStandardPaths::LocateDirectory);
  QString path = QDir(documents_dir).filePath("DeepCountFiles");
  if (!QDir(path).exists()) {
    QDir().mkpath(path);
  }
  return path;
}

 int Write::get_processed_aoi_count() {
   QString path = check_documents_directory();
   QString pf = QDir(path).filePath(".number_aoi_processed");
   std::ifstream input(pf.toStdString());
   std::string line;
   getline(input, line);
   int count = atoi(line.c_str());
   input.close();
   return count;
 }

void Write::data_to_file(std::vector<aoi> *v) {
  QString path = check_documents_directory();
  QString kf_file_path = QDir(path).filePath("kalman_filtered_data.csv");
  QString fftf_file_path = QDir(path).filePath("fft_filtered_data.csv");
  QByteArray kf_file_name = kf_file_path.toLocal8Bit();
  char *kf_file_name_buffer = kf_file_name.data();
  FILE *kff = fopen(kf_file_name_buffer, "aw");

  QByteArray fftf_file_name = fftf_file_path.toLocal8Bit();
  char *fftf_file_name_buffer = fftf_file_name.data();
  FILE *fftf = fopen(fftf_file_name_buffer, "aw");
  if (kff == NULL || fftf == NULL) {
    printf("cant save data");
    return;
  }

  for (unsigned long i = 0; i < v->size(); i++) {
    std::string kf_data_str = "";
    std::string fft_data_str = "";
    std::string value_str = "";
    aoi a = v->at(i);
    for (unsigned long j = 0; j < a.kf_x_intensity_histogram.size(); j++) {
      kf_data_str += std::to_string(a.kf_x_intensity_histogram[j]);
      fft_data_str += std::to_string(a.fft_x_intensity_histogram[j]);
      kf_data_str += ",";
      fft_data_str += ",";
    }
    for (unsigned long k = 0; k < a.kf_y_intensity_histogram.size(); k++) {
      kf_data_str += std::to_string(a.kf_y_intensity_histogram[k]);
      fft_data_str += std::to_string(a.fft_y_intensity_histogram[k]);
      if (k != (a.kf_y_intensity_histogram.size() - 1)) {
        kf_data_str += ',';
        fft_data_str += ',';
      }
    }
    std::vector<int> values(7, 0);
    values[a.step_number] = 1;
    for (unsigned long l = 0; l < values.size(); l++) {
      value_str += std::to_string(values[l]);
      if (l != (values.size() - 1)) {
        value_str += ',';
      }
    }
    fprintf(kff, "%s\n", kf_data_str.c_str());
    fprintf(fftf, "%s\n", fft_data_str.c_str());
    fprintf(kff, "%s\n", value_str.c_str());
    fprintf(fftf, "%s\n", value_str.c_str());
  }
  fclose(kff);
  fclose(fftf);

  int old_count = get_processed_aoi_count();
  int new_count = old_count + static_cast<int> (v->size());
  QString pf = QDir(path).filePath(".number_aoi_processed");
  QByteArray bitarry = pf.toLocal8Bit();
  char *buffer = bitarry.data();
  FILE *nap = fopen(buffer, "w");
  fprintf(nap, "%d", new_count);
  fclose(nap);
}
