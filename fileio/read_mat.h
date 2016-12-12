#ifndef READ_MAT_H
#define READ_MAT_H
#include "helpers/includes.h"
#include "helpers/structs.h"

class ReadMat {
public:
  ReadMat();
  std::vector<aoi> read_aois(const char *inputfile);
  unsigned long get_number_of_aois();
private:
  unsigned long number_of_aois;
};

#endif // READ_MAT_H
