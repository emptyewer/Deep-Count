#include "deepcount.h"
#include "fileio/read_mat.h"
#include "fileio/write.h"
#include "helpers/filtering.h"
#include "ui_deepcount.h"
#include <QFileDialog>

DeepCount::DeepCount(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::DeepCount) {
  ui->setupUi(this);
  setCustomStyle();
  current_frame = 0;
  current_file = 0;
  enable_all = false;
}

void DeepCount::on_load_directory_clicked() {
  QString dir = QFileDialog::getExistingDirectory(
      this, tr("Choose Directory"), "~",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (dir != NULL) {
    listFiles(QDir(dir), &file_list);
    ui->root_dir->setText(dir);
    read_aois();
    process_data();
    display_aoi();
    enable_all = true;
  }

  //  foreach (QString str, file_list) { qDebug() << str; }
}

void DeepCount::listFiles(QDir root_dir, QList<QString> *dir_list) {
  QDir dir(root_dir);
  QFileInfoList list =
      dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  foreach (QFileInfo finfo, list) {
    if (finfo.suffix() == "dat") {
      dir_list->append(finfo.absoluteFilePath());
    }
    if (finfo.isDir()) {
      listFiles(QDir(finfo.absoluteFilePath()), dir_list);
    }
  }
}

void DeepCount::read_aois() {
  bool processed = false;
  foreach (QString s, Write().get_processed_files()) {
    if (QString::compare(s, file_list.at(current_file), Qt::CaseSensitive) ==
        0) {
      processed = true;
      ui->statusBar->showMessage("Skipped over already processed files...",
                                 5000);
    }
  }

  if (processed) {
    current_file += 1;
    read_aois();
  } else {
    current_aoi_count = Write().get_processed_aoi_count();
    if (current_aoi_count == 0) {
      current_aoi_count = 0;
    }
    ReadMat read_mat_files = ReadMat::ReadMat();
    QString str;
    QTextStream(&str) << current_file + 1 << " of " << file_list.size();
    ui->number_of_files->setText(str);
    ui->current_dir->setText(
        QFileInfo(file_list.at(current_file)).dir().dirName());
    ui->file_name->setText(QFileInfo(file_list.at(current_file)).fileName());
    QByteArray file_name = file_list.at(current_file).toLocal8Bit();
    char *file_name_buffer = file_name.data();
    aois = read_mat_files.read_aois(file_name_buffer);
  }
}

void DeepCount::process_data() {
  if (aois.at(current_frame).csum_intensity.size() == 0) {
    Filtering filter = Filtering();
    filter.detect_changepoints(&aois.at(current_frame));
    filter.normalize(&aois.at(current_frame));
    //      qDebug() << "Normalize";
    filter.calculate_histogram(&aois.at(current_frame));
    //      qDebug() << "Y Histogram";
    filter.subtract_consecutive_intensities(&aois.at(current_frame));
    //      qDebug() << "X_ Subtract";
    filter.reduce_dimensionality(&aois.at(current_frame));
  }
}

void DeepCount::display_aoi() {
  QString str;
  QTextStream(&str) << current_frame + 1 << " of " << aois.size();
  ui->numbe_of_aois->setText(str);
  if (aois.at(current_frame).classified != true) {
    ui->classified->setText(QString::number(current_aoi_count));
  }
  plot();
  refresh_plots();
  update_step_count();
}

void DeepCount::plot() {
  std::vector<double> range;
  unsigned int i = 0;
  for (i = 0; i < 100; i++) {
    range.push_back(static_cast<double>(i));
  }
  QVector<double> xvalues =
      QVector<double>::fromStdVector(aois.at(current_frame).x);
  QVector<double> ave_xvalues = QVector<double>::fromStdVector(range);
  QVector<double> intensity =
      QVector<double>::fromStdVector(aois.at(current_frame).intensity);
  QVector<double> csum_intensity =
      QVector<double>::fromStdVector(aois.at(current_frame).csum_intensity);
  QVector<double> xhist = QVector<double>::fromStdVector(
      aois.at(current_frame).csum_x_intensity_histogram);
  QVector<double> yhist = QVector<double>::fromStdVector(
      aois.at(current_frame).csum_y_intensity_histogram);

  ui->trace_plot->addGraph();
  ui->trace_plot->graph(0)->setData(xvalues, intensity);
  ui->trace_plot->graph(0)->setPen(QPen(QColor(238, 31, 95, 255)));
  ui->trace_plot->addGraph();
  ui->trace_plot->graph(1)->setData(xvalues, csum_intensity);
  ui->trace_plot->graph(1)->setPen(QPen(QColor(0, 189, 242, 255), 2));

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
  ui->x_plot->yAxis->setRange(0.0, 1.05);
  ui->y_plot->yAxis->setRange(0, 1.05);
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
//    std::cout << current_frame << "\n";
//    for (int i = 0; i < aois.at(current_frame).intensity.size(); i++) {
//      std::cout << aois.at(current_frame).intensity[i] << ",";
//    }
//    std::cout << "\n-------\n";
    process_data();
    if (aois.at(current_frame).classified != true) {
      current_aoi_count += 1;
    }
  } else {
    Write().data_to_file(&aois);
    Write().processed_to_file(file_list.at(current_file));
    ui->statusBar->showMessage("Saving data and current progress.", 5000);
    aois.clear();
    current_file += 1;
    read_aois();
    current_frame = 0;
  }
  clear_graphs();
  display_aoi();
}

void DeepCount::on_previous_clicked() {
  if (current_frame == 0) {
    current_frame = 0;
  } else {
    if (aois.at(current_frame).classified != true) {
      current_aoi_count -= 1;
    }
    current_frame -= 1;
    process_data();
  }
  clear_graphs();
  display_aoi();
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
  if (enable_all) {
    QVector<double> xvalues =
        QVector<double>::fromStdVector(aois.at(current_frame).x);
    QVector<double> intensity =
        QVector<double>::fromStdVector(aois.at(current_frame).intensity);
    QVector<double> csum_intensity =
        QVector<double>::fromStdVector(aois.at(current_frame).csum_intensity);
    QVector<double> csum_intensity_normalized = QVector<double>::fromStdVector(
        aois.at(current_frame).csum_intensity_normalized);
    switch (event->key()) {
    case 16777235: // UP
      ui->trace_plot->clearGraphs();
      ui->trace_plot->addGraph();
      ui->trace_plot->graph(0)->setData(xvalues, intensity);
      ui->trace_plot->graph(0)->setPen(QPen(QColor(238, 31, 95, 255)));
      ui->trace_plot->addGraph();
      ui->trace_plot->graph(1)->setData(xvalues, csum_intensity);
      ui->trace_plot->graph(1)->setPen(QPen(QColor(0, 189, 242, 255), 2));
      refresh_plots();
      break;
    case 16777237: // Down
      ui->trace_plot->clearGraphs();
      ui->trace_plot->addGraph();
      ui->trace_plot->graph(0)->setData(xvalues, csum_intensity_normalized);
      ui->trace_plot->graph(0)->setPen(QPen(QColor(238, 31, 95, 255), 2));
      refresh_plots();
      break;
    case 16777236:
      on_next_clicked();
      break;
    case 16777234:
      on_previous_clicked();
      break;
    case 48:
      ui->zero_step->setChecked(true);
      ui->step_count_group->setChecked(true);
      break;
    case 49:
      ui->one_step->setChecked(true);
      ui->step_count_group->setChecked(true);
      break;
    case 50:
      ui->two_step->setChecked(true);
      ui->step_count_group->setChecked(true);
      break;
    case 51:
      ui->three_step->setChecked(true);
      ui->step_count_group->setChecked(true);
      break;
    case 52:
      ui->four_step->setChecked(true);
      ui->step_count_group->setChecked(true);
      break;
    case 53:
      ui->five_step->setChecked(true);
      ui->step_count_group->setChecked(true);
      break;
    case 83:
      ui->skip_step->setChecked(true);
      ui->step_count_group->setChecked(true);
      break;
    default:
      break;
    }
  }
}

void DeepCount::update_step_count() {
  if (aois.at(current_frame).classified == true) {
    switch (aois.at(current_frame).step_number) {
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
    case 6:
      ui->skip_step->setChecked(true);
      break;
    default:
      break;
    }
    ui->step_count_group->setTitle("Steps (Classified)");
    ui->step_count_group->setChecked(true);
  } else {
    ui->step_count_group->setTitle("Steps");
    ui->step_count_group->setChecked(false);
  }
}

void DeepCount::record_selection() {
  QList<QRadioButton *> list =
      ui->step_count_group->findChildren<QRadioButton *>();
  for (int i = 0; i < list.size(); i++) {
    if (list.at(i)->isChecked()) {
      if (list.at(i)->objectName() == "zero_step") {
        aois.at(current_frame).step_number = 0;
      } else if (list.at(i)->objectName() == "one_step") {
        aois.at(current_frame).step_number = 1;
      } else if (list.at(i)->objectName() == "two_step") {
        aois.at(current_frame).step_number = 2;
      } else if (list.at(i)->objectName() == "three_step") {
        aois.at(current_frame).step_number = 3;
      } else if (list.at(i)->objectName() == "four_step") {
        aois.at(current_frame).step_number = 4;
      } else if (list.at(i)->objectName() == "five_step") {
        aois.at(current_frame).step_number = 5;
      } else if (list.at(i)->objectName() == "skip_step") {
        aois.at(current_frame).step_number = 6;
      }
    }
    if (aois.at(current_frame).classified == false) {
      aois.at(current_frame).classified = true;
    }
  }
}

DeepCount::~DeepCount() { delete ui; }
