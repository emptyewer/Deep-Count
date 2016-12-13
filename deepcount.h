#ifndef DEEPCOUNT_H
#define DEEPCOUNT_H

#include "helpers/includes.h"
#include "helpers/structs.h"
#include <QMainWindow>

using namespace std;

namespace Ui {
class DeepCount;
}

class DeepCount : public QMainWindow {
  Q_OBJECT

public:
  explicit DeepCount(QWidget *parent = 0);
  ~DeepCount();

private slots:
  void on_next_clicked();
  void on_previous_clicked();
  void on_load_directory_clicked();

private:
  bool enable_all;
  QCPBars *xbars;
  QCPBars *ybars;
  Ui::DeepCount *ui;
  unsigned long current_frame;
  int current_file;
  int current_aoi_count;
  vector<aoi> aois;
  QList<QString> file_list;
  void plot();
  void setup_plots();
  void refresh_plots();
  void clear_graphs();
  void setCustomStyle();
  void read_aois();
  void display_aoi();
  void update_step_count();
  void record_selection();
  void listFiles(QDir root_dir, QList<QString> *dir_list);

protected:
  void keyPressEvent(QKeyEvent *event);
};

#endif // DEEPCOUNT_H
