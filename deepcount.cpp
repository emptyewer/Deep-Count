#include "deepcount.h"
#include "fileio/read_mat.h"
#include "filter/kalman.hpp"
#include "helpers/filtering.h"
#include "ui_deepcount.h"
#include <Eigen/Dense>
#include <iostream>
#include <vector>

DeepCount::DeepCount(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::DeepCount) {
  ui->setupUi(this);
  setCustomStyle();
  current_frame = 0;
  get_aois();
  setup_plots();
  plot();
  refresh_plots();
  update_step_count();
}

void DeepCount::get_aois() {
  ReadMat read_mat_files = ReadMat::ReadMat();
  aois = read_mat_files.read_aois("/Users/Venky/Work/Softwares/Photobleaching/"
                                  "Experimental Data/Jason Data/Organized "
                                  "Liposomes/20151112 IWIW "
                                  "Photobleaching/3.10.0ugpermg_IWIW.dat");
  Filtering filter = Filtering();
  for (unsigned long i = 0; i < read_mat_files.get_number_of_aois(); i++) {
    filter.kalman_filter(&aois[i]);
    //    filter.fft(&aois[i]);
    filter.normalize(&aois[i]);
    filter.calculate_histogram(&aois[i]);
    filter.subtract_consecutive_intensities(&aois[i]);
    filter.reduce_dimensionality(&aois[i]);
  }
}

void DeepCount::plot() {
  QVector<double> xvalues =
      QVector<double>::fromStdVector(aois[current_frame].x);
  QVector<double> ave_xvalues =
      QVector<double>::fromStdVector(aois[current_frame].average_x);
  QVector<double> intensity =
      QVector<double>::fromStdVector(aois[current_frame].intensity);
  QVector<double> fft_yvalues =
      QVector<double>::fromStdVector(aois[current_frame].average_kf_intensity);
  QVector<double> xhist = QVector<double>::fromStdVector(
      aois[current_frame].average_x_intensity_reduced);
  QVector<double> yhist = QVector<double>::fromStdVector(
      aois[current_frame].average_y_intensity_histogram);
  ui->trace_plot->addGraph();
  ui->trace_plot->graph(0)->setData(xvalues, intensity);
  ui->trace_plot->graph(0)->setPen(QPen(QColor(120, 120, 120, 255)));

  ui->trace_plot->addGraph();
  ui->trace_plot->graph(1)->setData(xvalues, fft_yvalues);
  ui->trace_plot->graph(1)->setPen(QPen(QColor(0, 189, 242, 255), 3));

  ui->trace_plot->xAxis->setLabel("Time (*)");
  ui->trace_plot->yAxis->setLabel("Intensity");

  xbars = new QCPBars(ui->x_plot->xAxis, ui->x_plot->yAxis);
  xbars->setAntialiased(true);
  xbars->setPen(QPen(QColor(0, 168, 140).lighter(130)));
  xbars->setBrush(QColor(0, 168, 140));
  xbars->setData(ave_xvalues, xhist);
  xbars->rescaleAxes(true);

  ybars = new QCPBars(ui->y_plot->xAxis, ui->y_plot->yAxis);
  ybars->setAntialiased(true);
  ybars->setPen(QPen(QColor(111, 9, 176).lighter(170)));
  ybars->setBrush(QColor(111, 9, 176));
  ybars->setData(ave_xvalues, yhist);
  ybars->rescaleAxes(true);
}

void DeepCount::refresh_plots() {
  ui->trace_plot->rescaleAxes(true);
  ui->x_plot->yAxis->setRange(-1.0, 1.0);
  ui->y_plot->yAxis->setRange(0, 1.0);
  ui->trace_plot->replot();
  ui->x_plot->replot();
  ui->y_plot->replot();
}

void DeepCount::clear_graphs() {
  ui->trace_plot->clearGraphs();
  ui->x_plot->clearGraphs();
  ui->y_plot->clearGraphs();
  xbars->clearData();
  ybars->clearData();
}

void DeepCount::setup_plots() {
  ui->trace_plot->xAxis2->setVisible(true);
  ui->trace_plot->xAxis2->setTickLabels(false);
  ui->trace_plot->yAxis2->setVisible(true);
  ui->trace_plot->yAxis2->setTickLabels(false);

  ui->x_plot->xAxis2->setVisible(false);
  ui->x_plot->xAxis2->setTickLabels(false);
  ui->x_plot->yAxis2->setVisible(false);
  ui->x_plot->yAxis->setTickLabels(false);
  ui->x_plot->xAxis->setVisible(false);
  ui->x_plot->xAxis->setTickLabels(false);
  ui->x_plot->yAxis->setVisible(false);
  ui->x_plot->yAxis->setTickLabels(false);
  ui->y_plot->xAxis->setVisible(false);
  ui->y_plot->xAxis->setTickLabels(false);
  ui->y_plot->yAxis->setVisible(false);
  ui->y_plot->yAxis->setTickLabels(false);
  ui->y_plot->xAxis2->setVisible(false);
  ui->y_plot->xAxis2->setTickLabels(false);
  ui->y_plot->yAxis2->setVisible(false);
  ui->y_plot->yAxis2->setTickLabels(false);
}

void DeepCount::on_next_clicked() {
  record_selection();
  if (current_frame < aois.size() - 1) {
    current_frame += 1;
  }
  clear_graphs();
  plot();
  refresh_plots();
  update_step_count();
}

void DeepCount::on_previous_clicked() {
  if (current_frame == 0) {
    current_frame = 0;
  } else {
    current_frame -= 1;
  }
  clear_graphs();
  plot();
  refresh_plots();
  update_step_count();
}

void DeepCount::setCustomStyle() {
  QFile f(":qdarkstyle/style.qss");
  if (!f.exists()) {
    printf("Unable to set stylesheet, file not found\n");
  } else {
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    qApp->setStyleSheet(ts.readAll());
  }
}

void DeepCount::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case 16777236:
      on_next_clicked();
    break;
  case 16777234:
      on_previous_clicked();
    break;
  case 48:
    ui->zero_step->setChecked(true);
    break;
  case 49:
    ui->one_step->setChecked(true);
    break;
  case 50:
    ui->two_step->setChecked(true);
    break;
  case 51:
    ui->three_step->setChecked(true);
    break;
  case 52:
    ui->four_step->setChecked(true);
    break;
  case 53:
    ui->five_step->setChecked(true);
    break;
  case 83:
    ui->skip_step->setChecked(true);
    break;
  default:
    break;
  }
}

void DeepCount::update_step_count() {
  if (aois[current_frame].classified == true) {
    switch (aois[current_frame].step_number) {
    case 0:
      ui->zero_step->setChecked(true);
      break;
    case 1:
      ui->one_step->setChecked(true);
      break;
    case 2:
      ui->two_step->setChecked(true);
      break;
    case 3:
      ui->three_step->setChecked(true);
      break;
    case 4:
      ui->four_step->setChecked(true);
      break;
    case 5:
      ui->five_step->setChecked(true);
      break;
    case 9:
      ui->skip_step->setChecked(true);
      break;
    default:
      break;
    }
    ui->step_count_group->setTitle("Steps (Classified)");
  }
  else {
      ui->step_count_group->setTitle("Steps");
    }
}

void DeepCount::record_selection() {
  QList<QRadioButton *> list =
      ui->step_count_group->findChildren<QRadioButton *>();
  for (int i = 0; i < list.size(); i++) {
    if (list[i]->isChecked()) {
      if (list[i]->objectName() == "zero_step") {
        aois[current_frame].step_number = 0;
      } else if (list[i]->objectName() == "one_step") {
        aois[current_frame].step_number = 1;
      } else if (list[i]->objectName() == "two_step") {
        aois[current_frame].step_number = 2;
      } else if (list[i]->objectName() == "three_step") {
        aois[current_frame].step_number = 3;
      } else if (list[i]->objectName() == "four_step") {
        aois[current_frame].step_number = 4;
      } else if (list[i]->objectName() == "five_step") {
        aois[current_frame].step_number = 5;
      } else if (list[i]->objectName() == "skip_step") {
        aois[current_frame].step_number = 9;
      }
    }
    if (aois[current_frame].classified == false) {
      aois[current_frame].classified = true;
    }
  }
}

DeepCount::~DeepCount() { delete ui; }
