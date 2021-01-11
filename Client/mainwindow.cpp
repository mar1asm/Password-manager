#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow ( QWidget *parent )
    : QMainWindow ( parent ), ui ( new Ui::MainWindow ) {
  ui->setupUi ( this );		   // pentru interfata grafica
  this->setFixedSize ( size ( ) ); // ca sa nu dea userul resize
  this->setWindowTitle ( "Password manager" );
  client = new Client ( );	      // pentru comunicarea cu serverul
  this->mainWidget = new QWidget ( ); // asta tine doar de interfata grafica
  mainWidget->setGeometry (
      100, 100, 800, 700 ); // seteaza pozitia si dimensiunea mainWidgetului
  mainWidget->setParent ( this );
  showLoginPage ( );
}

MainWindow::~MainWindow ( ) {
  client->sendToServer ( "disconnected" );
  delete ui;
}

void MainWindow::showLoginPage ( ) {
  if ( signupPageActive ) {
    signupPageActive = false;
    delete signupForm;
  }
  loginPageActive = true;
  loginForm = new LoginForm ( );
  loginForm->setParent ( mainWidget );
  loginForm->move ( 200, 100 );
  loginForm->show ( );
  connect ( loginForm, &LoginForm::signup, this, &MainWindow::showSignupPage );
  connect ( loginForm, &LoginForm::login, this, &MainWindow::on_login );

  // functia connect are ca parametri: un pointer la obiectul care emite un
  // semnal, semnalul emis, un pointer catre obiectul care primeste semnalul, si
  // un slot. un slot e doar o functie
  // Atunci cand obiectul respectiv emite semnalul este apelata functia
  // De exemplu aici atunci cand obiectul loginForm emite semnalul login, este
  // apelata functia on_login din clasa MainWindow(clasa asta)
}

void MainWindow::on_login (
    QString username,
    QString password ) { // functia care e apelata atunci cand se apasa butonul
             // login din loginForm

  QString hashedPass = hash ( password );

  if ( username.size ( ) == 0 || password.size ( ) == 0 ) {
    QMessageBox::warning ( this, "Date invalide",
               "Trebuie sa completezi toate campurile" );
    return;
  }
  if ( ! client->sendToServer ( "login" ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( username ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }

  if ( ! client->sendToServer ( hashedPass ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  QString raspuns = client->readFromServer ( );
  if ( raspuns != "error" ) {
    // logat cu succes
    if ( raspuns == "ok" ) {
      this->username = username;
      QMessageBox::information ( this, "Succes", "Te-ai logat cu succes" );
      showMainPage ( );
      return;
    } else {
      QMessageBox::warning ( this, "Date invalide",
                 "Username sau parola gresite!" );
      return;
    }
  } else
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
}

void MainWindow::showSignupPage ( ) { // exact acelasi lucru ca showLoginPage
  if ( loginPageActive ) {
    loginPageActive = false;
    delete loginForm;
  }
  signupPageActive = true;
  signupForm = new SignupForm ( );
  signupForm->setParent ( mainWidget );
  signupForm->move ( 200, 100 );
  signupForm->show ( );
  connect ( signupForm, &SignupForm::signup, this, &MainWindow::on_signup );
  connect ( signupForm, &SignupForm::login, this, &MainWindow::showLoginPage );
}

void MainWindow::on_signup ( QString username, QString email, QString password,
                 QString cPassword ) {
  std::cout << "register " << username.toStdString ( ) << " "
        << password.toStdString ( ) << " " << cPassword.toStdString ( )
        << "\n";
  if ( username.size ( ) == 0 || password.size ( ) == 0 ||
       cPassword.size ( ) == 0 || email.size ( ) == 0 ) {
    QMessageBox::warning ( this, "Date invalide",
               "Trebuie sa completezi toate campurile" );
    return;
  }
  if ( password != cPassword ) {
    QMessageBox::critical ( this, "Passwords", "Parolele nu sunt la fel" );
    return;
  }

  QString hashedPass = hash ( password );

  if ( ! client->sendToServer ( "register" ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( username ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( email ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( hashedPass ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  QString raspuns = client->readFromServer ( );
  if ( raspuns != "error" ) {
    // logat cu succes
    if ( raspuns == "ok" ) {
      QMessageBox::information ( this, "Succes",
                 "Te-ai inregistrat cu succes" );
      delete signupForm;
      signupPageActive = false;
      showLoginPage ( );
      return;
    }
    if ( raspuns == "usernameUsed" ) {
      QMessageBox::warning ( this, "Username invalid",
                 "Username deja folosit!" );
      return;
    }
    if ( raspuns == "emailUsed" ) {
      QMessageBox::warning ( this, "Email",
                 "Exista deja un utilizator cu acelasi mail!" );
      return;
    }
  } else
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
}

class MyScrollArea
    : public QScrollArea { // am facut aceasta manevra pentru ca altfel daca
               // dadea resize la scrollarea nu mai mergea scrollul
  virtual QSize sizeHint ( ) const { return QSize ( 800, 500 ); }
};

void MainWindow::showMainPage ( ) { // asta afiseaza pagina principala, dupa ce
  // te-ai logat. e putin complicata functia asta din cauza interfetei grafice
  delete loginForm;

  if ( ! client->sendToServer ( "showPassList" ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( this->username ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  QString answer = client->readFromServer ( );
  if ( answer == "error" ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
    return;
  }

  QPushButton *addNew = new QPushButton ( "Add" ); // adauga un button de add
  addNew->setParent ( this->ui->centralWidget );
  addNew->move ( 800, 70 );
  addNew->show ( );
  this->addNew = addNew;
  connect ( addNew, &QPushButton::clicked, this,
        &MainWindow::on_addNewButton_clicked );
  int numberOfPasswords = answer.toInt ( );

  // de aici

  QWidget *central = new QWidget;
  QScrollArea *scroll = new MyScrollArea;
  scroll->move ( 0, 100 );
  QVBoxLayout *layout = new QVBoxLayout ( central );
  scroll->setWidget ( central );
  scroll->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
  scroll->setWidgetResizable ( true );
  this->layout = layout;

  // pana aici sunt doar pentru interfata grafica

  for ( int i = 0; i < numberOfPasswords;
    i++ ) { // primeste de la server parolele si informatiile despre parole
    QString title = client->readFromServer ( );
    if ( title == "error" ) {
      QMessageBox::critical ( this, "Eroare",
                  "Eroare la comunicarea cu serverul" );
      return;
    }
    QString user = client->readFromServer ( );
    if ( user == "error" ) {
      QMessageBox::critical ( this, "Eroare",
                  "Eroare la comunicarea cu serverul" );
      return;
    }

    QString pass = client->readFromServer ( );
    if ( pass == "error" ) {
      QMessageBox::critical ( this, "Eroare",
                  "Eroare la comunicarea cu serverul" );
      return;
    }
    QString passUrl = client->readFromServer ( );
    if ( passUrl == "error" ) {
      QMessageBox::critical ( this, "Eroare",
                  "Eroare la comunicarea cu serverul" );
      return;
    }
    QString notes = client->readFromServer ( );
    if ( notes == "error" ) {
      QMessageBox::critical ( this, "Eroare",
                  "Eroare la comunicarea cu serverul" );
      return;
    }
    this->scroll = scroll; // am nevoie ca sa l sterg mai tarziu

    passwordsInfo.push_back ( QStringList (
    { title, user, pass, passUrl,
      notes } ) ); // in vectorul asta retin parolele si informatiile despre
    // ele ca sa nu le cer de la server de 100 ori

    // restul e pentru interfata

    QWidget *p1 = new QWidget;
    QHBoxLayout *hl = new QHBoxLayout ( p1 );
    QLabel *site = new QLabel ( title );
    QString hiddenPassword = pass;
    hiddenPassword.fill ( '*' );
    QLabel *password = new QLabel ( hiddenPassword );
    password->setProperty ( "hidden", true );
    passwords.push_back ( password );
    titles.push_back ( site );

    QPushButton *editButton = new QPushButton ( "Edit" );
    QPushButton *deleteButton = new QPushButton ( "Delete" );
    QPushButton *showButton = new QPushButton ( "Toggle visible" );
    QPushButton *detailsButton = new QPushButton ( "Details" );

    editButton->setProperty ( "id", i );
    deleteButton->setProperty ( "id", i );
    showButton->setProperty ( "id", i );
    detailsButton->setProperty ( "id", i );

    hl->addWidget ( site );
    hl->addWidget ( password );
    hl->addWidget ( showButton );
    hl->addWidget ( detailsButton );
    hl->addWidget ( editButton );
    hl->addWidget ( deleteButton );

    entries.push_back ( hl );
    listItems.push_back ( p1 );
    layout->addWidget ( p1 );

    connect ( showButton, SIGNAL ( clicked ( ) ), this,
          SLOT ( on_showPassword ( ) ) );
    connect ( detailsButton, SIGNAL ( clicked ( ) ), this,
          SLOT ( on_showDetailsPassword ( ) ) );
    connect ( editButton, SIGNAL ( clicked ( ) ), this,
          SLOT ( on_editPassword ( ) ) );
    connect ( deleteButton, SIGNAL ( clicked ( ) ), this,
          SLOT ( on_deletePassword ( ) ) );
  }
  scroll->setParent ( mainWidget );
  scroll->show ( );

  QPushButton *logout = new QPushButton ( "Logout" );
  connect ( logout, &QPushButton::clicked, this, &MainWindow::on_logout );
  logout->setParent ( this->ui->centralWidget );
  logout->move ( 50, 50 );
  logout->show ( );
  this->logout = logout;
}

void MainWindow::on_addNewButton_clicked ( ) { // functie apelata atunci cand
                           // apesi buttonul add
  addPasswordDialog = new AddPasswordDialog (
      this ); // se creaza un dialog in care sa scrii informatiile despre parola
  connect ( addPasswordDialog, &AddPasswordDialog::cancel, this,
        &MainWindow::on_cancelButtonClicked );
  connect (
      addPasswordDialog, &AddPasswordDialog::addPassword, this,
      &MainWindow::on_addButtonClicked ); // daca dupa ce scrii info despre
                      // parola apesi buttonul add, se
                      // apeleaza functia de mai jos
  addPasswordDialog->show ( );
}

void MainWindow::on_addButtonClicked ( QString title, QString username,
                       QString password, QString url,
                       QString notes ) {
  if ( ! client->sendToServer ( "addPassword" ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( this->username ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( title ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( username ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( password ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( url ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( notes ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  delete addPasswordDialog;
  int passwordId = passwordsInfo.size ( );
  passwordsInfo.push_back (
      QStringList ( { title, username, password, url, notes } ) );
  // interfata
  QWidget *p1 = new QWidget;
  QHBoxLayout *hl = new QHBoxLayout ( p1 );
  QLabel *site = new QLabel ( title );
  QString hiddenPassword = password;
  hiddenPassword.fill ( '*' );
  QLabel *lpassword = new QLabel ( hiddenPassword );
  lpassword->setProperty ( "hidden", true );
  passwords.push_back ( lpassword );
  titles.push_back ( site );
  QPushButton *editButton = new QPushButton ( "Edit" );
  QPushButton *deleteButton = new QPushButton ( "Delete" );
  QPushButton *showButton = new QPushButton ( "Toggle visible" );
  QPushButton *detailsButton = new QPushButton ( "Details" );
  editButton->setProperty ( "id", passwordId );
  deleteButton->setProperty ( "id", passwordId );
  showButton->setProperty ( "id", passwordId );
  detailsButton->setProperty ( "id", passwordId );

  hl->addWidget ( site );
  hl->addWidget ( lpassword );
  hl->addWidget ( showButton );
  hl->addWidget ( detailsButton );
  hl->addWidget ( editButton );
  hl->addWidget ( deleteButton );

  entries.push_back ( hl );
  listItems.push_back ( p1 );
  layout->addWidget ( p1 );

  connect ( showButton, SIGNAL ( clicked ( ) ), this,
        SLOT ( on_showPassword ( ) ) );
  connect ( detailsButton, SIGNAL ( clicked ( ) ), this,
        SLOT ( on_showDetailsPassword ( ) ) );
  connect ( editButton, SIGNAL ( clicked ( ) ), this,
        SLOT ( on_editPassword ( ) ) );
  connect ( deleteButton, SIGNAL ( clicked ( ) ), this,
        SLOT ( on_deletePassword ( ) ) );
}

void MainWindow::on_cancelButtonClicked ( ) { delete addPasswordDialog; }

void MainWindow::on_deletePassword ( ) {
  QString idString = sender ( )->property ( "id" ).toString ( );
  int id = idString.toInt ( );

  QLayoutItem *child;
  while ( entries.at ( id )->count ( ) ) {
    child = entries.at ( id )->takeAt ( 0 );
    delete child->widget ( );
    delete child->spacerItem ( );
    delete child->layout ( );
  }
  entries.removeAt ( id );

  if ( ! client->sendToServer ( "delete" ) )
    return;
  if ( ! client->sendToServer ( this->username ) )
    return;
  if ( ! client->sendToServer ( idString ) )
    return;
  for ( int i = id; i < entries.size ( );
    i++ ) // atunci cand o parola e stearsa, trebuie schimbare id-urile
          // parolelor de dupa ea
    for ( int j = 2; j < 6; j++ ) {
      int index =
      entries[ i ]->itemAt ( j )->widget ( )->property ( "id" ).toInt ( );
      entries[ i ]->itemAt ( j )->widget ( )->setProperty ( "id", index - 1 );
    }
  listItems.removeAt ( id );
  passwordsInfo.removeAt ( id );
  passwords.removeAt ( id );
  titles.removeAt ( id );
}
void MainWindow::on_editPassword ( ) {
  int id = sender ( )->property ( "id" ).toInt ( );
  editPasswordDialog =
      new EditPasswordDialog ( this, id, &passwordsInfo[ id ] );
  connect ( editPasswordDialog, &EditPasswordDialog::cancelEdit, this,
        &MainWindow::on_cancelEdit );
  connect ( editPasswordDialog, &EditPasswordDialog::saveEdit, this,
        &MainWindow::on_saveEdit );
  editPasswordDialog->show ( );
}

void MainWindow::on_cancelEdit ( ) { delete editPasswordDialog; }

void MainWindow::on_saveEdit ( int id, QString title, QString username,
                   QString password, QString url, QString notes ) {

  if ( ! client->sendToServer ( "edit" ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( this->username ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( QString ( to_string ( id ).data ( ) ) ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( title ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( username ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( password ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( url ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }
  if ( ! client->sendToServer ( notes ) ) {
    QMessageBox::critical ( this, "Eroare",
                "Eroare la comunicarea cu serverul" );
  }

  passwordsInfo[ id ][ 0 ] = title;
  passwordsInfo[ id ][ 1 ] = username;
  passwordsInfo[ id ][ 2 ] = password;
  passwordsInfo[ id ][ 3 ] = url;
  passwordsInfo[ id ][ 4 ] = notes;
  bool hidden = passwords[ id ]->property ( "hidden" ).toBool ( );
  if ( hidden ) {
    QString hiddenPassword = password;
    hiddenPassword.fill ( '*' );
    passwords[ id ]->setText ( hiddenPassword );
  } else {
    passwords[ id ]->setText ( password );
  }
  titles[ id ]->setText ( title );
  delete editPasswordDialog;
}

void MainWindow::on_showPassword ( ) { // functia asta modifica modul in care e
  // afisata parola, daca sunt afisate *****
  // sau parola
  int id = sender ( )->property ( "id" ).toInt ( );
  bool hidden = passwords[ id ]->property ( "hidden" ).toBool ( );
  if ( hidden ) {
    passwords[ id ]->setText ( passwordsInfo[ id ][ 2 ] );
    passwords[ id ]->setProperty ( "hidden", false );
  } else {
    QString hiddenPassword = passwords[ id ]->text ( );
    hiddenPassword.fill ( '*' );
    passwords[ id ]->setText ( hiddenPassword );
    passwords[ id ]->setProperty ( "hidden", true );
  }
}

void MainWindow::on_showDetailsPassword ( ) {
  int id = sender ( )->property ( "id" ).toInt ( );
  passwordDetailsDialog =
      new PasswordDetailsDialog ( this, &passwordsInfo[ id ] );
  passwordDetailsDialog->show ( );
}

void MainWindow::on_logout ( ) { // cand dai logout sa se afiseze pagina de
  // login. nu merge momentan dar o sa mearga acus
  while ( mainWidget->children ( ).size ( ) != 0 ) {
    QObject *child = mainWidget->children ( )[ 0 ];
    delete child;
  }
  delete addNew;
  delete logout;

  passwords.resize ( 0 );
  titles.resize ( 0 );
  passwordsInfo.resize ( 0 );
  entries.resize ( 0 );
  listItems.resize ( 0 );

  showLoginPage ( );
}

QString MainWindow::hash ( QString text ) {
  QByteArray hash = QCryptographicHash::hash ( text.toLocal8Bit ( ),
                           QCryptographicHash::Sha1 );
  QString hashedPass = QLatin1String ( hash.toHex ( ) );
  return hashedPass;
}
