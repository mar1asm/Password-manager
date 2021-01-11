#include "server.h"
#include <QApplication>

int main ( int argc, char *argv[] ) { // clasa asta nu face nimic, doar creaza o
                      // instanta a clasei server
  QApplication a ( argc, argv );
  Server s;
  return a.exec ( );
}
