#include "mainwindow.h"
#include <QApplication>

int main ( int argc, char *argv[] ) { // exact ca la server
  QApplication a ( argc, argv );
  MainWindow w;
  w.show ( ); // clasa mainwindow e cea care se ocupa si de interfata grafica

  return a.exec ( );
}
