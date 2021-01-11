#include "server.h"

// unele functii au " :: " in fata atunci cand sunt apelate pentru ca altfel nu
// merge pentru ca sunt confundate cu functiile cu acelasi nume din QT

Server::Server ( QWidget *parent ) : QMainWindow ( parent ) {
  startServer ( );
} // in constructor doar se apeleaza functia startServer

Server::~Server ( ) {}

void Server::startServer ( ) {
  if ( setup ( ) ) // setup returneaza 0 daca totul e ok
    return;
  cout << "Serverul a pornit!\n";
  serve ( );
}

//de pe site-ul cursului
int Server::setup ( ) {
  /* crearea unui socket */
  if ( ( sd = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
    perror ( "[server]Eroare la socket().\n" );
    return errno;
  }
  /* utilizarea optiunii SO_REUSEADDR */
  int on = 1;
  setsockopt ( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof ( on ) );

  /* pregatirea structurilor de date */
  bzero ( &server, sizeof ( server ) );
  bzero ( &from, sizeof ( from ) );

  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
  server.sin_family = AF_INET;
  /* acceptam orice adresa */
  server.sin_addr.s_addr = htonl ( INADDR_ANY );
  /* utilizam un port utilizator */
  server.sin_port = htons ( PORT );

  /* atasam socketul */
  if ( bind ( sd, ( struct sockaddr * ) &server, sizeof ( struct sockaddr ) ) ==
       -1 ) {
    perror ( "[server]Eroare la bind().\n" );
    return errno;
  }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if ( listen ( sd, 2 ) == -1 ) {
    perror ( "[server]Eroare la listen().\n" );
    return errno;
  }
  return 0;
}

void Server::serve ( ) { // in functia asta se asteapta conectarea unui client
  while ( true ) {
    int client = 0;

    unsigned int length = sizeof ( from );

    cout << "[server]Asteptam la portul" << PORT << "\n";
    if ( ( client = ::accept ( sd, ( struct sockaddr * ) &from, &length ) ) <
     0 ) {
      perror ( "[server]Eroare la accept().\n" );
      continue;
    }

    new ::thread ( &Server::treat, this,
           client ); // pentru fiecare client se creaza un nou thread
                 // care executa functia treat
  }
}

void Server::treat ( int client ) {
  cout << "[server] S-a conectat un clientul cu socket descriptorul " << client
       << ". Id-ul threadului asociat este " << this_thread::get_id ( ) << "\n";
  while ( true ) {
    // while-ul asta se executa cat timp functia process(client) nu returneaza
    // -1

    if ( process ( client ) == -1 ) {
      ::close ( client );
      return;
    }
  }
}

int Server::process (
    int client ) { // in functia asta se citeste un mesaj de la client si se
           // apeleaza functiile corespunzatoare
  QString message ( readFromClient ( client ) );

  cout << "[server] Am primit mesajul: " << message.toStdString ( )
       << " de la clientul cu sd " << client << "\n";

  if ( message == "login" ) {
    if ( login ( client ) == 0 ) {
      cout << "[server] Clientul cu sd " << client << " s-a logat cu succes"
       << "\n";
      sendToClient ( client, "ok" );
    } else {
      cout << "[server] Clientul cu sd " << client
       << " a introdus date de logare invalide"
       << "\n";
      sendToClient ( client, "notok" );
    }

    return 0;
  }

  // signup(client) returneaza -1 = eroare la read, -2 = username folosit deja,
  // -3 email deja folosit, -4 eroare la file, 0 = ok
  if ( message == "register" ) {
    int ret = signup ( client );
    if ( ret == 0 ) {
      cout << "[server] Clientul cu sd " << client
       << " s-a inregistrat cu succes"
       << "\n";
      sendToClient ( client, "ok" );

      return 0;
    }
    if ( ret == -2 ) {
      cout << "[server] Clientul cu sd " << client
       << " a incercat sa se inregistreze cu un username deja folosit"
       << "\n";
      sendToClient ( client, "usernameUsed" );
      return 0;
    }
    if ( ret == -3 ) {
      cout << "[server] Clientul cu sd " << client
       << " a incercat sa se inregistreze cu un email deja folosit"
       << "\n";
      sendToClient ( client, "emailUsed" );
      return 0;
    }
    cout << "[server] Eroare la citirea din fisier sau la comunicareac cu "
        "clientul "
     << client << " \n";
  }
  if ( message == "showPassList" ) {
    showPassList ( client );
    return 0;
  }

  if ( message == "delete" ) {
    cout << "[server] Clientul cu sd " << client << " a sters o parola."
     << "\n";
    deletePassword ( client );
    return 0;
  }

  if ( message == "addPassword" ) {
    cout << "[server] Clientul cu sd " << client << " a adaugat o parola."
     << "\n";
    addPassword ( client );
    return 0;
  }
  if ( message == "edit" ) {
    cout << "[server] Clientul cu sd " << client << " a editat o parola."
     << "\n";
    editPassword ( client );
    return 0;
  }
  if ( message == "disconnected" ) {
    cout << "[server] S-a deconectat clientul cu socket descriptorul " << client
     << "\n";
    return -1;
  }
  return -1;
}

int Server::deletePassword ( int client ) {
  QString username ( readFromClient ( client ) );

  if ( username == "error" )
    return -1;
  QString idString ( readFromClient ( client ) );
  if ( idString == "error" )
    return -1;
  int passId = idString.toInt ( ); // asta e id-ul parolei care trebuie stearsa
  QFile userFile (
      "./Infos/" +
      username ); // in folderul Infos e cate un fisier pentru fiecare client
          // inregistrat unde sunt stocate parolele
  if ( ! userFile.open ( QIODevice::ReadWrite ) )
    return -4;
  QJsonDocument passwordDocument = QJsonDocument::fromJson (
      userFile
      .readAll ( ) ); // citeste continutul fisierului intr-un QJSonDocument
  QJsonArray passwordsArray = passwordDocument.array ( );
  passwordsArray.removeAt ( passId ); // sterge parola cu id-ul respectiv
  passwordDocument.setArray ( passwordsArray );
  userFile.resize ( 0 ); // sterge continutul vechi al fisierului
  userFile.write (
      passwordDocument.toJson ( ) ); // scrie continutul nou al fisierului
  userFile.close ( );
  return 0;
}

int Server::editPassword ( int client ) { // functioneaza la fel ca la delete
  QString username ( readFromClient ( client ) );
  if ( username == "error" )
    return -1;
  QString idString ( readFromClient ( client ) );
  if ( idString == "error" )
    return -1;
  int passId = idString.toInt ( );

  QFile userFile ( "./Infos/" + username );
  if ( ! userFile.open ( QIODevice::ReadWrite ) )
    return -4;
  QJsonDocument passwordsDocument =
      QJsonDocument::fromJson ( userFile.readAll ( ) );
  QJsonArray passwordsArray = passwordsDocument.array ( );
  QJsonObject newPassword;
  QString info (
      readFromClient ( client ) ); // clientul trimite pe rand titlul,
                   // usernameul, parola, url-ul si notele
  if ( info == "error" )
    return -1;
  newPassword.insert ( "title", QJsonValue::fromVariant ( info ) );

  info = readFromClient ( client );
  if ( info == "error" )
    return -1;
  newPassword.insert ( "username", QJsonValue::fromVariant ( info ) );

  info = readFromClient ( client );
  if ( info == "error" )
    return -1;
  newPassword.insert ( "password", QJsonValue::fromVariant ( info ) );

  info = readFromClient ( client );
  if ( info == "error" )
    return -1;
  newPassword.insert ( "url", QJsonValue::fromVariant ( info ) );

  info = readFromClient ( client );
  if ( info == "error" )
    return -1;
  newPassword.insert ( "notes", QJsonValue::fromVariant ( info ) );

  passwordsArray.removeAt (
      passId ); // se sterge vechea parola si informatiile despre ea
  passwordsArray.insert ( passId, newPassword ); // se adauga parola noua
  passwordsDocument.setArray ( passwordsArray );
  userFile.resize ( 0 );
  userFile.write ( passwordsDocument.toJson ( ) );
  userFile.close ( );
  return 0;
}

int Server::addPassword ( int client ) { // ca cele de mai sus
  QString username ( readFromClient ( client ) );
  if ( username == "error" )
    return -1;
  QFile userFile ( "./Infos/" + username );
  if ( ! userFile.open ( QIODevice::ReadWrite ) )
    return -4;

  QJsonDocument passwordsDocument =
      QJsonDocument::fromJson ( userFile.readAll ( ) );
  QJsonArray passwordsArray = passwordsDocument.array ( );
  QJsonObject newPassword;

  QString info ( readFromClient ( client ) );
  if ( info == "error" )
    return -1;
  newPassword.insert ( "title", QJsonValue::fromVariant ( info ) );

  info = readFromClient ( client );
  if ( info == "error" )
    return -1;
  newPassword.insert ( "username", QJsonValue::fromVariant ( info ) );

  info = readFromClient ( client );
  if ( info == "error" )
    return -1;
  newPassword.insert ( "password", QJsonValue::fromVariant ( info ) );

  info = readFromClient ( client );
  if ( info == "error" )
    return -1;
  newPassword.insert ( "url", QJsonValue::fromVariant ( info ) );

  info = readFromClient ( client );
  if ( info == "error" )
    return -1;
  newPassword.insert ( "notes", QJsonValue::fromVariant ( info ) );

  passwordsArray.push_back ( newPassword );
  passwordsDocument.setArray ( passwordsArray );
  userFile.resize ( 0 );
  userFile.write ( passwordsDocument.toJson ( ) );
  userFile.close ( );
  return 0;
}

int Server::login (
    int client ) { // returneaza  -1 = eroare la read, -2 = date de logare
  // invalide, -3 eroare la file, 0 = ok

  QString username ( readFromClient ( client ) );
  if ( username == "error" )
    return -1;
  QString password ( readFromClient ( client ) );
  if ( password == "error" )
    return -1;

  QString hashedPass = hash ( password );

  QString path ( "./Infos" );
  QDir dir;
  if ( ! dir.exists ( path ) )
    dir.mkpath ( path );
  QFile userFile ( path + '/' + "users.json" );
  if ( ! userFile.open ( QIODevice::ReadWrite ) )
    return -3;

  QJsonDocument usersJson = QJsonDocument::fromJson ( userFile.readAll ( ) );

  QList< QVariant > users = usersJson.toVariant ( ).toList ( );
  for ( QVariant userData : users ) { // verifica daca exista un user
                      // inregistrat cu datele de logare primite
    QMap< QString, QVariant > user = userData.toMap ( );
    if ( user[ "username" ].toString ( ) == username &&
     user[ "password" ].toString ( ) == hashedPass )
      return 0;
  }
  return -2;
}

int Server::signup ( int client ) { // returneaza -1 = eroare la read, -2 =
                    // username folosit deja,
  // -3 email deja folosit, -4 eroare la file, 0 = ok
  QString username ( readFromClient ( client ) );
  if ( username == "error" )
    return -1;
  QString email ( readFromClient ( client ) );
  if ( email == "error" )
    return -1;
  QString password ( readFromClient ( client ) );
  if ( password == "error" )
    return -1;

  QString path ( "./Infos" );
  QDir dir;
  if ( ! dir.exists ( path ) )
    dir.mkpath ( path );
  QFile file ( path + '/' + "users.json" );
  if ( ! file.open ( QIODevice::ReadWrite ) )
    return -4;

  QJsonDocument usersJson = QJsonDocument::fromJson ( file.readAll ( ) );
  QList< QVariant > users = usersJson.toVariant ( ).toList ( );
  for ( QVariant userData :
    users ) { // verifica daca exista deja un user inregistrat cu acelasi
          // username sau email
    QMap< QString, QVariant > user = userData.toMap ( );
    if ( user[ "username" ].toString ( ) == username )
      return -2;
    if ( user[ "email" ].toString ( ) == email )
      return -3;
  }
  QJsonArray usersArray = usersJson.array ( ); // daca nu exista deja un user,
                           // il adauga in lista de useri
  QString hashedPass = hash ( password );
  QJsonObject newUser;
  newUser.insert ( "username", QJsonValue::fromVariant ( username ) );
  newUser.insert ( "password", QJsonValue::fromVariant ( hashedPass ) );
  newUser.insert ( "email", QJsonValue::fromVariant ( email ) );

  usersArray.push_back ( newUser );
  usersJson.setArray ( usersArray );
  file.resize ( 0 );
  file.write ( usersJson.toJson ( ) );
  file.close ( );
  return 0;
}

int Server::showPassList (
    int client ) { // trimite lista de parole si informatiile corespunzatoare
           // catre client
  QString username = readFromClient ( client );
  if ( username == "error" ) {
    return -1;
  }
  QString path ( "./Infos/" );
  QFile userFile ( path + username );
  if ( ! userFile.open ( QIODevice::ReadWrite ) )
    return -4;
  QJsonDocument passwordDocument =
      QJsonDocument::fromJson ( userFile.readAll ( ) );
  QJsonArray passwordsArray = passwordDocument.array ( );
  int numberOfPasswords = passwordsArray.size ( );
  if ( ! sendToClient ( client,
            QString ( to_string ( numberOfPasswords ).data ( ) ) ) )
    return -4;
  for ( int i = 0; i < numberOfPasswords; i++ ) {
    QJsonObject passwordInfo = passwordsArray[ i ].toObject ( );
    sendToClient ( client, passwordInfo[ "title" ].toString ( ) );
    sendToClient ( client, passwordInfo[ "username" ].toString ( ) );
    sendToClient ( client, passwordInfo[ "password" ].toString ( ) );
    sendToClient ( client, passwordInfo[ "url" ].toString ( ) );
    sendToClient ( client, passwordInfo[ "notes" ].toString ( ) );
  }
  return 0;
}

bool Server::sendToClient ( int client, QString msg ) { // trimite catre client
  int lungime = msg.size ( );
  char mesaj[ lungime + 1 ];
  strcpy ( mesaj, msg.toStdString ( ).data ( ) );
  if ( write ( client, &lungime, sizeof ( int ) ) < 0 ) {
    perror ( "[Server] eroare la write spre client" );
    return false;
  }
  if ( write ( client, mesaj, lungime ) < 0 ) {
    perror ( "[Server] eroare la write spre client" );
    return false;
  }
  return true;
}

QString Server::readFromClient ( int client ) { // citeste de la client
  int lungime;
  if ( read ( client, &lungime, sizeof ( int ) ) < 0 ) {
    perror ( "[Server] eroare la read de la client" );
    return "error";
  }
  char msg[ lungime + 1 ];
  if ( read ( client, msg, lungime ) < 0 ) {
    perror ( "[Server] eroare la read de la client" );
    return "error";
  }
  msg[ lungime ] = '\0';
  return QString ( msg );
}

QString Server::hash ( QString text ) {
  QByteArray hash = QCryptographicHash::hash ( text.toLocal8Bit ( ),
                           QCryptographicHash::Sha1 );
  QString hashedPass = QLatin1String ( hash.toHex ( ) );
  return hashedPass;
}
