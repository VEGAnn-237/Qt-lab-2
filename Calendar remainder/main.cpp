#include <QApplication>

#include "main_window.h"

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);

  MainWindow Window;
  Window.show();

  return QApplication::exec();
}
