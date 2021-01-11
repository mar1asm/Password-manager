#ifndef CLIENT_H
#define CLIENT_H
#include <QObject>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
using namespace std;

class Client : public QObject {
  Q_OBJECT
public:
  Client ( );
  bool sendToServer ( QString );
  QString readFromServer ( );
  int setup ( );
  ::thread *serverCommunication;

private:
  int port;
  string address;
  int sd;
  struct sockaddr_in server;
};

#endif // CLIENT_H
