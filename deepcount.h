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

private:
  QCPBars *xbars;
  QCPBars *ybars;
  Ui::DeepCount *ui;
  void plot();
  void setup_plots();
  void refresh_plots();
  void clear_graphs();
  void setCustomStyle();
  void get_aois();
  void update_step_count();
  void record_selection();
  unsigned long current_frame;
  vector<aoi> aois;

protected:
  void keyPressEvent(QKeyEvent *event);
};

#endif // DEEPCOUNT_H
