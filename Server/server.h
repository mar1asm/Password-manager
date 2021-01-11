#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMainWindow>
#include <QMap>
#include <QTextStream>
#include <QVariant>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;

class Server : public QMainWindow {
  Q_OBJECT

public:
  explicit Server ( QWidget *parent = 0 );
  ~Server ( );

  void startServer ( );

private:
  int setup ( );	     // setarea serverului
  struct sockaddr_in server; // structura folosita de server
  struct sockaddr_in from;
  int sd;
  int PORT = 3232;

  void serve ( );      // astept sa se conecteze clientii
  void treat ( int );  // callback pentru thread
  int process ( int ); // functie care proceseaza mesajele primite de la client

  // functii ce proceseaza comenzile
  int login ( int );
  int signup ( int );
  int showPassList ( int );
  int deletePassword ( int );
  int editPassword ( int );
  int addPassword ( int );

  QString hash ( QString );

  bool sendToClient ( int, QString );
  QString readFromClient ( int );
};

#endif // MAINWINDOW_H
