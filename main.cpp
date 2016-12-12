#include "deepcount.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  DeepCount w;
  w.show();

  return a.exec();
}
