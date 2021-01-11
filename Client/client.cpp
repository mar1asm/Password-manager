#include "client.h"

Client::Client ( ) {
  address = "127.0.0.1";
  port = 3232;
  if ( setup ( ) )
    cout << "eroare la conectarea la server\n";
  else
    cout << "conectat la server";
}

bool Client::sendToServer ( QString msg ) {
  int lungime = msg.size ( );
  char mesaj[ lungime + 1 ];
  strcpy ( mesaj, msg.toStdString ( ).data ( ) );
  if ( write ( sd, &lungime, sizeof ( int ) ) < 0 ) {
    perror ( "[client] eroare la write spre server" );
    return false;
  }
  if ( write ( sd, mesaj, lungime ) < 0 ) {
    perror ( "[client] eroare la write spre server" );
    return false;
  }
  return true;
}

QString Client::readFromServer ( ) {
  int lungime;
  if ( read ( sd, &lungime, sizeof ( int ) ) < 0 ) {
    perror ( "[client] eroare la read de la server" );
    return "error";
  }
  char msg[ lungime + 1 ];
  if ( read ( sd, msg, lungime ) < 0 ) {
    perror ( "[client] eroare la read de la server" );
    return "error";
  }
  msg[ lungime ] = '\0';
  return QString ( msg );
}

int Client::setup ( ) {
  if ( ( sd = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
    perror ( "Eroare la socket().\n" );
    return errno;
  }
  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  char adresa[ address.size ( ) ];
  strcpy ( adresa, address.data ( ) );
  server.sin_addr.s_addr = inet_addr ( adresa );
  /* portul de conectare */
  server.sin_port = htons ( port );

  /* ne conectam la server */
  if ( ::connect ( sd, ( struct sockaddr * ) &server,
           sizeof ( struct sockaddr ) ) == -1 ) {
    perror ( "[client]Eroare la connect().\n" );
    return errno;
  }

  return 0;
}
