#include "read_mat.h"
#include "matio.h"

ReadMat::ReadMat() {}

std::vector<aoi> ReadMat::read_aois(const char *inputfile) {
  int err = 0;
  mat_t *mat_file;
  matvar_t *aoifits;
  std::vector<aoi> aois;
  //  qDebug() << inputfile;
  mat_file = Mat_Open(inputfile, MAT_ACC_RDONLY);
  if (mat_file) {
    aoifits = Mat_VarRead(mat_file, static_cast<const char *>("aoifits"));
    if (aoifits == NULL) {
      err = 1;
    } else {
      matvar_t **fields = static_cast<matvar_t **>(aoifits->data);
      unsigned long nfields =
          aoifits->nbytes / static_cast<unsigned long>(aoifits->data_size);
      for (unsigned long i = 0; i < nfields; i++) {
        if (std::string(fields[i]->name).compare("centers") == 0) {
          number_of_aois = fields[i]->dims[0];
          for (unsigned long t = 0; t < number_of_aois; t++) {
            struct aoi temp = aoi();
            aois.push_back(temp);
          }
          //          qDebug() << "Number of AOIs : " << number_of_aois;
        }
        if (std::string(fields[i]->name).compare("data") == 0) {
          //          qDebug() << "Data Type : " << fields[i]->data_type;
          //          qDebug() << "Class Type : " << fields[i]->class_type;
          //          qDebug() << "Rank : " << fields[i]->rank;
          unsigned long count = 0;
          for (unsigned long k = 0;
               k < static_cast<unsigned long>(fields[i]->dims[0]); k++) {
            aois[count].intensity.push_back(static_cast<double *>(
                fields[i]
                    ->data)[fields[i]->dims[0] * (fields[i]->dims[1] - 1) + k]);
            count += 1;
            if (count == number_of_aois) {
              count = 0;
            }
          }
        }
      }
      Mat_VarFree(aoifits);
    }
    Mat_Close(mat_file);
  }
  return aois;
}

unsigned long ReadMat::get_number_of_aois() { return number_of_aois; }
