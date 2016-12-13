#ifndef WRITE_H
#define WRITE_H
#include "helpers/includes.h"
#include "helpers/structs.h"

class Write {
public:
  Write();
  std::vector<QString> get_processed_files();
  void processed_to_file(QString s);
  void data_to_file(std::vector<aoi> *v);
  int get_processed_aoi_count();
private:
  QString check_documents_directory();
};

#endif // WRITE_H
